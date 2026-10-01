#include "main.h"
#include "esp_lvgl_port.h"
#include "esp_system.h"
#include "bsp_mic.h"
#include "bsp_audio.h"
#include "driver/uart.h"
#include "freertos/queue.h"
#include "cJSON.h"
#include <math.h>
#include <string.h>
#include <stdatomic.h>
#include "setup_qr_size.h"
extern void panel_bt_start(void);
extern bool panel_bt_poll(char *out);
extern bool panel_bt_take_wake(void);
extern void panel_video_start(void);
extern bool panel_video_command(cJSON *j);

typedef struct {char host[64];double cpu,ram,used,total,disk,temp;bool has_temp,wake;} metrics_t;
static QueueHandle_t samples;
static lv_obj_t *host_label,*cpu_label,*ram_label,*disk_label,*temp_label,*link_label,*audio_label,*audio_button;
static int64_t last_received;
static bool was_connected;
static atomic_bool sleeping;
static atomic_bool movie_mode;
static lv_obj_t *settings_overlay,*settings_status;
static bool last_has_temp;
extern bool panel_movie_toggle_audio(void);
static bool video_active[2];
static lv_obj_t *day_image,*night_image,*panel,*petals[36],*mist[3],*ripples[10],*clouds[3],*branch;
bool panel_video_is_night(void){return atomic_load(&sleeping);}
void panel_video_present(const lv_image_dsc_t *frame,bool night){if(lvgl_port_lock(0)){video_active[night?1:0]=true;lv_image_cache_drop(frame);lv_image_set_src(night?night_image:day_image,frame);lv_obj_invalidate(night?night_image:day_image);lvgl_port_unlock();}}
void panel_movie_present(const lv_image_dsc_t *frame,bool settled){
 static int brightness=-1;
 if(lvgl_port_lock(0)){
  if(!movie_mode){movie_mode=true;video_active[0]=video_active[1]=true;lv_obj_set_style_opa(night_image,0,0);lv_label_set_text(lv_obj_get_child(audio_button,0),"MUTE / SOUND");}
  lv_image_cache_drop(frame);lv_image_set_src(day_image,frame);lv_obj_invalidate(day_image);
  int level=settled&&sleeping?12:60;if(level!=brightness){set_lcd_blight(level);brightness=level;}
  lvgl_port_unlock();
 }
}
static int night_opacity;
extern const uint8_t branch_data[] asm("_binary_branch_argb_start");
static const lv_image_dsc_t branch_art={.header={.magic=LV_IMAGE_HEADER_MAGIC,.cf=LV_COLOR_FORMAT_ARGB8888,.w=360,.h=150,.stride=1440},.data_size=216000,.data=branch_data};
static void set_sleep(bool value);
extern const uint8_t day_data[] asm("_binary_day_rgb565_start");
extern const uint8_t night_data[] asm("_binary_night_rgb565_start");
static const lv_image_dsc_t day_art={.header={.magic=LV_IMAGE_HEADER_MAGIC,.cf=LV_COLOR_FORMAT_RGB565,.w=800,.h=480,.stride=1600},.data_size=768000,.data=day_data};
static const lv_image_dsc_t night_art={.header={.magic=LV_IMAGE_HEADER_MAGIC,.cf=LV_COLOR_FORMAT_RGB565,.w=800,.h=480,.stride=1600},.data_size=768000,.data=night_data};
static void audio_worker(void *arg){
 (void)arg;esp_err_t err=mic_read_to_audio(5);
 if(lvgl_port_lock(0)){lv_label_set_text(audio_label,err==ESP_OK?"Playback finished":"Audio test failed");lv_obj_remove_state(audio_button,LV_STATE_DISABLED);lvgl_port_unlock();}
 vTaskDelete(NULL);
}
static void record_clicked(lv_event_t *e){
 if(movie_mode){bool muted=panel_movie_toggle_audio();lv_label_set_text(audio_label,muted?"Nature sound muted":"Nature sound playing");return;}
 (void)e;lv_obj_add_state(audio_button,LV_STATE_DISABLED);lv_label_set_text(audio_label,"Speak for 5 seconds...");
 if(xTaskCreate(audio_worker,"audio_test",4096,NULL,5,NULL)!=pdPASS){lv_obj_remove_state(audio_button,LV_STATE_DISABLED);lv_label_set_text(audio_label,"Audio task unavailable");}
}
static bool number(cJSON *j,const char *key,double min,double max,double *out){
 cJSON *v=cJSON_GetObjectItemCaseSensitive(j,key);
 if(!cJSON_IsNumber(v)||!isfinite(v->valuedouble)||v->valuedouble<min||v->valuedouble>max)return false;
 *out=v->valuedouble;return true;
}
static void parse_line(char *line){
 cJSON *j=cJSON_Parse(line);if(!j)return;
 if(panel_video_command(j)){cJSON_Delete(j);return;}
 metrics_t m={0};double version;
 bool ok=number(j,"v",1,1,&version)&&number(j,"cpu",0,100,&m.cpu)&&number(j,"ram",0,100,&m.ram)&&number(j,"disk",0,100,&m.disk)&&number(j,"used",0,1000000,&m.used)&&number(j,"total",0.1,1000000,&m.total);
 cJSON *h=cJSON_GetObjectItemCaseSensitive(j,"host");cJSON *o=cJSON_GetObjectItemCaseSensitive(j,"os");
 if(!cJSON_IsString(h)||!cJSON_IsString(o))ok=false;
 if(ok){
  snprintf(m.host,sizeof(m.host),"%.40s | %.16s",h->valuestring,o->valuestring);
  for(char *p=m.host;*p;p++)if((unsigned char)*p<32)*p=' ';
  m.wake=cJSON_IsTrue(cJSON_GetObjectItemCaseSensitive(j,"wake"));
  m.has_temp=number(j,"temp",-40,150,&m.temp);
  xQueueOverwrite(samples,&m);
 }
 cJSON_Delete(j);
}
static void receiver(void *arg){
 (void)arg;char line[2048];size_t n=0;bool discard=false;uint8_t bytes[128];
 for(;;){int count=uart_read_bytes(UART_NUM_0,bytes,sizeof(bytes),pdMS_TO_TICKS(200));
  for(int i=0;i<count;i++){
   char c=bytes[i];if(c=='\n'){if(!discard){line[n]=0;parse_line(line);}n=0;discard=false;}
   else if(!discard&&c!='\r'){if(n<sizeof(line)-1)line[n++]=c;else discard=true;}
  }
 }
}
static void update(lv_timer_t *t){
 (void)t;metrics_t m;
 char bt_message[96];if(panel_bt_poll(bt_message))lv_label_set_text(audio_label,bt_message);
 if(panel_bt_take_wake())set_sleep(false);
 if(xQueueReceive(samples,&m,0)==pdTRUE){
  if(sleeping&&(m.wake||esp_timer_get_time()-last_received>5000000))set_sleep(false);
  last_received=esp_timer_get_time();was_connected=true;
  last_has_temp=m.has_temp;
  lv_label_set_text(host_label,m.host);
  /* LVGL's minimal formatter has floating-point support disabled. Use libc. */
  char formatted[128];
  snprintf(formatted,sizeof(formatted),"CPU LOAD\n%.1f %%",m.cpu);lv_label_set_text(cpu_label,formatted);
  snprintf(formatted,sizeof(formatted),"RAM\n%.1f %%\n%.1f / %.1f GiB",m.ram,m.used,m.total);lv_label_set_text(ram_label,formatted);
  snprintf(formatted,sizeof(formatted),"SYSTEM DISK\n%.1f %% used",m.disk);lv_label_set_text(disk_label,formatted);
  if(m.has_temp){snprintf(formatted,sizeof(formatted),"CPU TEMP\n%.1f C",m.temp);lv_label_set_text(temp_label,formatted);}
  else lv_label_set_text(temp_label,"CPU TEMP\nN/A");
  lv_label_set_text(link_label,"USB connected - live readings");
 }else if(was_connected&&esp_timer_get_time()-last_received>5000000){
  was_connected=false;lv_label_set_text(link_label,"Disconnected - waiting for host");
  lv_label_set_text(cpu_label,"CPU LOAD\n--");lv_label_set_text(ram_label,"RAM\n--");
  lv_label_set_text(disk_label,"SYSTEM DISK\n--");lv_label_set_text(temp_label,"CPU TEMP\n--");
 }
}
static void settings_update(lv_timer_t *timer){(void)timer;if(settings_status&&!lv_obj_has_flag(settings_overlay,LV_OBJ_FLAG_HIDDEN))lv_label_set_text(settings_status,was_connected?(last_has_temp?"USB connected | CPU temperature received":"USB connected | No CPU temperature received"):"USB disconnected | Start the PC companion");}
static void fade(void *obj,int32_t v){
 night_opacity=v;lv_obj_set_style_opa(obj,v,0);
}
static void fade_done(lv_anim_t *a){(void)a;set_lcd_blight(sleeping?12:60);}
static void set_sleep(bool value){
 if(sleeping==value)return;
 sleeping=value;
 if(value)lv_obj_add_flag(panel,LV_OBJ_FLAG_HIDDEN);
 else {lv_obj_remove_flag(panel,LV_OBJ_FLAG_HIDDEN);set_lcd_blight(60);}
 if(movie_mode)return;
 lv_anim_delete(night_image,fade);
 lv_anim_t a;lv_anim_init(&a);lv_anim_set_var(&a,night_image);lv_anim_set_values(&a,night_opacity,value?255:0);
 lv_anim_set_duration(&a,2200);lv_anim_set_exec_cb(&a,fade);lv_anim_set_path_cb(&a,lv_anim_path_ease_in_out);lv_anim_set_completed_cb(&a,fade_done);lv_anim_start(&a);
}
static void sleep_clicked(lv_event_t *e){(void)e;set_sleep(true);}
static void scene_clicked(lv_event_t *e){(void)e;if(sleeping)set_sleep(false);}
static void atmosphere(lv_timer_t *timer){
 (void)timer;float t=esp_timer_get_time()/1000000.0f;
 bool video=video_active[sleeping?1:0];
 for(int i=0;i<36;i++){if(video)lv_obj_add_flag(petals[i],LV_OBJ_FLAG_HIDDEN);else lv_obj_remove_flag(petals[i],LV_OBJ_FLAG_HIDDEN);}
 for(int i=0;i<3;i++){if(video){lv_obj_add_flag(mist[i],LV_OBJ_FLAG_HIDDEN);lv_obj_add_flag(clouds[i],LV_OBJ_FLAG_HIDDEN);}else{lv_obj_remove_flag(mist[i],LV_OBJ_FLAG_HIDDEN);lv_obj_remove_flag(clouds[i],LV_OBJ_FLAG_HIDDEN);}}
 for(int i=0;i<10;i++){if(video)lv_obj_add_flag(ripples[i],LV_OBJ_FLAG_HIDDEN);else lv_obj_remove_flag(ripples[i],LV_OBJ_FLAG_HIDDEN);}
 if(video){lv_obj_add_flag(branch,LV_OBJ_FLAG_HIDDEN);return;}else lv_obj_remove_flag(branch,LV_OBJ_FLAG_HIDDEN);
 lv_image_set_rotation(branch,(int)(8*sinf(t*.33f)));
 lv_image_set_pivot(branch,0,0);
 lv_obj_set_pos(branch,-8,-57+(int)(2*sinf(t*.27f)));
 lv_obj_set_style_image_recolor(branch,lv_color_hex(0x253c62),0);
 lv_obj_set_style_image_recolor_opa(branch,night_opacity*150/255,0);
 for(int i=0;i<10;i++){
  float phase=fmodf(t*.12f+i*.1f,1);
  lv_obj_set_pos(ripples[i],300+(i%4)*36+(int)(4*sinf(t*.4f+i)),337+(i/4)*12);
  lv_obj_set_width(ripples[i],14+(int)(phase*22));
  lv_obj_set_style_border_opa(ripples[i],(int)(sinf(phase*3.14159f)*(sleeping?18:50)),0);
 }
 for(int i=0;i<3;i++){
  lv_obj_set_pos(clouds[i],370+i*65+(int)(18*sinf(t*.04f+i)),105+i*48);
  lv_obj_set_style_bg_opa(clouds[i],sleeping?4:7,0);
 }
 for(int i=0;i<36;i++){
  float y=fmodf(t*(9+i%4*3)+i*19,530)-25;
  float x=fmodf(i*29+t*7,840)-20+12*sinf(t*.7f+i);
  lv_obj_set_pos(petals[i],(int)x,(int)y);
  lv_obj_set_style_bg_opa(petals[i],sleeping?50:150,0);
 }
 for(int i=0;i<3;i++){
  lv_obj_set_pos(mist[i],280+i*38+(int)(12*sinf(t*.22f+i)),310-i*17-(int)(9*sinf(t*.32f+i)));
  lv_obj_set_style_bg_opa(mist[i],sleeping?0:14+(int)(5*sinf(t*.4f+i)),0);
 }
}
static lv_obj_t *text_at(lv_obj_t *parent,int x,int y,int w,const char *text,bool large){
 lv_obj_t *l=lv_label_create(parent);lv_obj_set_pos(l,x,y);lv_obj_set_width(l,w);lv_label_set_text(l,text);lv_obj_set_style_text_color(l,lv_color_hex(0xfff3da),0);
 if(large)lv_obj_set_style_text_font(l,&lv_font_montserrat_24,0);
 return l;
}
static lv_obj_t *button(lv_obj_t *parent,int x,int y,int w,const char *text,lv_event_cb_t cb){
 lv_obj_t *b=lv_button_create(parent);lv_obj_set_pos(b,x,y);lv_obj_set_size(b,w,38);
 lv_obj_set_style_bg_color(b,lv_color_hex(0x374943),0);lv_obj_set_style_bg_opa(b,200,0);lv_obj_set_style_radius(b,8,0);
 lv_obj_set_style_shadow_width(b,0,0);lv_obj_add_event_cb(b,cb,LV_EVENT_CLICKED,NULL);
 lv_obj_t *l=lv_label_create(b);lv_label_set_text(l,text);lv_obj_set_style_text_color(l,lv_color_hex(0xfff8e8),0);lv_obj_center(l);return b;
}
static void settings_close(lv_event_t *e){(void)e;lv_obj_add_flag(settings_overlay,LV_OBJ_FLAG_HIDDEN);}
static void settings_open(lv_event_t *e){(void)e;if(sleeping)set_sleep(false);lv_obj_remove_flag(settings_overlay,LV_OBJ_FLAG_HIDDEN);lv_obj_move_foreground(settings_overlay);settings_update(NULL);}
static void build_settings(lv_obj_t *screen){
 extern const uint8_t qr_data[] asm("_binary_setup_qr_rgb565_start");
 static lv_image_dsc_t qr={.header={.magic=LV_IMAGE_HEADER_MAGIC,.cf=LV_COLOR_FORMAT_RGB565,.w=SETUP_QR_SIZE,.h=SETUP_QR_SIZE,.stride=SETUP_QR_SIZE*2},.data_size=SETUP_QR_SIZE*SETUP_QR_SIZE*2};qr.data=qr_data;
 settings_overlay=lv_obj_create(screen);lv_obj_set_pos(settings_overlay,12,12);lv_obj_set_size(settings_overlay,776,456);lv_obj_set_style_pad_all(settings_overlay,20,0);lv_obj_set_style_bg_color(settings_overlay,lv_color_hex(0x243d35),0);lv_obj_set_style_bg_opa(settings_overlay,255,0);lv_obj_remove_flag(settings_overlay,LV_OBJ_FLAG_SCROLLABLE);
 text_at(settings_overlay,0,0,570,"WINDOWS MONITORING",true);button(settings_overlay,635,0,90,"CLOSE",settings_close);
 settings_status=text_at(settings_overlay,0,42,730,"Checking connection...",false);
 lv_obj_t *code=lv_image_create(settings_overlay);lv_image_set_src(code,&qr);lv_obj_set_pos(code,0,84);
 text_at(settings_overlay,260,84,465,"Scan for the temperature helper",true);
 text_at(settings_overlay,260,121,465,"1. Open the official download page on your Windows PC.\n\n2. Install Libre Hardware Monitor and its required components.\n\n3. Run it as administrator and leave it running.\n\n4. Keep the panel companion running. CPU temperature appears if supported.",false);
 text_at(settings_overlay,0,328,730,"github.com/LibreHardwareMonitor/\nLibreHardwareMonitor/releases/latest",false);
 text_at(settings_overlay,0,394,730,"Sensor-helper download only. The USB companion is separate.",false);
 lv_obj_add_flag(settings_overlay,LV_OBJ_FLAG_HIDDEN);lv_timer_create(settings_update,1000,NULL);
}
static void build_ui(void){
 lv_obj_t *s=lv_screen_active();lv_obj_remove_flag(s,LV_OBJ_FLAG_SCROLLABLE);lv_obj_set_style_bg_color(s,lv_color_hex(0x101c2b),0);
 lv_obj_set_style_text_color(s,lv_color_hex(0xf7edda),0);
 day_image=lv_image_create(s);lv_image_set_src(day_image,&day_art);
 night_image=lv_image_create(s);lv_image_set_src(night_image,&night_art);lv_obj_set_style_opa(night_image,0,0);
 lv_obj_t *touch=lv_obj_create(s);lv_obj_remove_style_all(touch);lv_obj_set_size(touch,800,480);
 lv_obj_add_flag(touch,LV_OBJ_FLAG_CLICKABLE);lv_obj_add_event_cb(touch,scene_clicked,LV_EVENT_CLICKED,NULL);
 branch=lv_image_create(s);lv_image_set_src(branch,&branch_art);lv_obj_remove_flag(branch,LV_OBJ_FLAG_CLICKABLE);
 for(int i=0;i<10;i++){
  ripples[i]=lv_obj_create(s);lv_obj_remove_style_all(ripples[i]);lv_obj_set_size(ripples[i],25,5);
  lv_obj_set_style_radius(ripples[i],LV_RADIUS_CIRCLE,0);lv_obj_set_style_border_width(ripples[i],1,0);
  lv_obj_set_style_border_color(ripples[i],lv_color_hex(0xbdd4d9),0);lv_obj_set_style_bg_opa(ripples[i],0,0);lv_obj_remove_flag(ripples[i],LV_OBJ_FLAG_CLICKABLE);
 }
 for(int i=0;i<3;i++){
  clouds[i]=lv_obj_create(s);lv_obj_remove_style_all(clouds[i]);lv_obj_set_size(clouds[i],170,22);
  lv_obj_set_style_radius(clouds[i],LV_RADIUS_CIRCLE,0);lv_obj_set_style_bg_color(clouds[i],lv_color_hex(0xb3bfce),0);lv_obj_remove_flag(clouds[i],LV_OBJ_FLAG_CLICKABLE);
 }
 for(int i=0;i<3;i++){
  mist[i]=lv_obj_create(s);lv_obj_remove_style_all(mist[i]);lv_obj_set_size(mist[i],180,35);
  lv_obj_set_style_radius(mist[i],LV_RADIUS_CIRCLE,0);lv_obj_set_style_bg_color(mist[i],lv_color_hex(0xf3ede4),0);lv_obj_remove_flag(mist[i],LV_OBJ_FLAG_CLICKABLE);
 }
 for(int i=0;i<36;i++){
  petals[i]=lv_obj_create(s);lv_obj_remove_style_all(petals[i]);lv_obj_set_size(petals[i],4+i%3,3+i%2);
  lv_obj_set_style_radius(petals[i],3,0);lv_obj_set_style_bg_color(petals[i],lv_color_hex(0xffc7d5),0);lv_obj_remove_flag(petals[i],LV_OBJ_FLAG_CLICKABLE);
 }
 panel=lv_obj_create(s);lv_obj_set_pos(panel,476,16);lv_obj_set_size(panel,308,448);
 lv_obj_set_style_bg_color(panel,lv_color_hex(0x3a5048),0);lv_obj_set_style_bg_opa(panel,235,0);
 lv_obj_set_style_border_width(panel,1,0);lv_obj_set_style_border_color(panel,lv_color_hex(0xafa88d),0);lv_obj_set_style_border_opa(panel,90,0);
 lv_obj_set_style_radius(panel,16,0);lv_obj_set_style_pad_all(panel,14,0);lv_obj_remove_flag(panel,LV_OBJ_FLAG_SCROLLABLE);
 lv_obj_t *title=text_at(panel,0,0,270,"MOUNTAIN REFUGE",true);lv_obj_set_style_text_color(title,lv_color_hex(0xffc7d5),0);
 host_label=text_at(panel,0,32,278,"RESOURCE PANEL | USB",false);
 cpu_label=text_at(panel,0,64,132,"CPU LOAD\n--",true);
 temp_label=text_at(panel,145,64,133,"CPU TEMP\n--",true);
 ram_label=text_at(panel,0,148,278,"RAM\n--",true);
 disk_label=text_at(panel,0,245,278,"SYSTEM DISK\n--",true);
 link_label=text_at(panel,0,313,278,"Waiting for USB host",false);
 button(panel,0,346,133,"SLEEP DISPLAY",sleep_clicked);
 audio_button=button(panel,143,346,135,"AUDIO TEST",record_clicked);
 audio_label=text_at(panel,0,393,278,"Sakura drift | USB resources",false);
 button(s,16,16,125,LV_SYMBOL_SETTINGS " SETTINGS",settings_open);build_settings(s);
 lv_timer_create(update,250,NULL);lv_timer_create(atmosphere,50,NULL);
}
void app_main(void){
 ESP_ERROR_CHECK(i2c_init());vTaskDelay(pdMS_TO_TICKS(200));ESP_ERROR_CHECK(stc8_i2c_init());ESP_ERROR_CHECK(touch_init());
 ESP_ERROR_CHECK(audio_ctrl_init());ESP_ERROR_CHECK(set_Audio_ctrl(false));ESP_ERROR_CHECK(audio_init());ESP_ERROR_CHECK(mic_init());ESP_ERROR_CHECK(display_init());
 samples=xQueueCreate(1,sizeof(metrics_t));assert(samples);
 if(lvgl_port_lock(0)){build_ui();lvgl_port_unlock();}
 ESP_ERROR_CHECK(set_lcd_blight(60));
 const uart_config_t cfg={.baud_rate=115200,.data_bits=UART_DATA_8_BITS,.parity=UART_PARITY_DISABLE,.stop_bits=UART_STOP_BITS_1,.flow_ctrl=UART_HW_FLOWCTRL_DISABLE,.source_clk=UART_SCLK_DEFAULT};
 ESP_ERROR_CHECK(uart_param_config(UART_NUM_0,&cfg));ESP_ERROR_CHECK(uart_driver_install(UART_NUM_0,8192,0,0,NULL,0));
 assert(xTaskCreate(receiver,"telemetry_rx",12288,NULL,4,NULL)==pdPASS);
 panel_bt_start();
 panel_video_start();
}
