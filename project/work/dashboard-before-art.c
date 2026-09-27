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

typedef struct {char host[64];double cpu,ram,used,total,disk,temp;bool has_temp;} metrics_t;
static QueueHandle_t samples;
static lv_obj_t *host_label,*cpu_label,*ram_label,*disk_label,*temp_label,*link_label,*audio_label,*audio_button;
static int64_t last_received;
static bool was_connected;
static void audio_worker(void *arg){
 (void)arg;esp_err_t err=mic_read_to_audio(5);
 if(lvgl_port_lock(0)){lv_label_set_text(audio_label,err==ESP_OK?"Playback finished":"Audio test failed");lv_obj_remove_state(audio_button,LV_STATE_DISABLED);lvgl_port_unlock();}
 vTaskDelete(NULL);
}
static void record_clicked(lv_event_t *e){
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
 metrics_t m={0};double version;
 bool ok=number(j,"v",1,1,&version)&&number(j,"cpu",0,100,&m.cpu)&&number(j,"ram",0,100,&m.ram)&&number(j,"disk",0,100,&m.disk)&&number(j,"used",0,1000000,&m.used)&&number(j,"total",0.1,1000000,&m.total);
 cJSON *h=cJSON_GetObjectItemCaseSensitive(j,"host");cJSON *o=cJSON_GetObjectItemCaseSensitive(j,"os");
 if(!cJSON_IsString(h)||!cJSON_IsString(o))ok=false;
 if(ok){
  snprintf(m.host,sizeof(m.host),"%.40s | %.16s",h->valuestring,o->valuestring);
  for(char *p=m.host;*p;p++)if((unsigned char)*p<32)*p=' ';
  m.has_temp=number(j,"temp",-40,150,&m.temp);
  xQueueOverwrite(samples,&m);
 }
 cJSON_Delete(j);
}
static void receiver(void *arg){
 (void)arg;char line[512];size_t n=0;bool discard=false;uint8_t bytes[128];
 for(;;){int count=uart_read_bytes(UART_NUM_0,bytes,sizeof(bytes),pdMS_TO_TICKS(200));
  for(int i=0;i<count;i++){
   char c=bytes[i];if(c=='\n'){if(!discard){line[n]=0;parse_line(line);}n=0;discard=false;}
   else if(!discard&&c!='\r'){if(n<sizeof(line)-1)line[n++]=c;else discard=true;}
  }
 }
}
static void update(lv_timer_t *t){
 (void)t;metrics_t m;
 if(xQueueReceive(samples,&m,0)==pdTRUE){
  last_received=esp_timer_get_time();was_connected=true;
  lv_label_set_text(host_label,m.host);
  lv_label_set_text_fmt(cpu_label,"CPU LOAD\n%.1f %%",m.cpu);
  lv_label_set_text_fmt(ram_label,"RAM\n%.1f %%\n%.1f / %.1f GiB",m.ram,m.used,m.total);
  lv_label_set_text_fmt(disk_label,"SYSTEM DISK\n%.1f %% used",m.disk);
  if(m.has_temp)lv_label_set_text_fmt(temp_label,"CPU TEMP\n%.1f C",m.temp);
  else lv_label_set_text(temp_label,"CPU TEMP\nUnavailable");
  lv_label_set_text(link_label,"USB connected - live readings");
 }else if(was_connected&&esp_timer_get_time()-last_received>5000000){
  was_connected=false;lv_label_set_text(link_label,"Disconnected - waiting for host");
  lv_label_set_text(cpu_label,"CPU LOAD\n--");lv_label_set_text(ram_label,"RAM\n--");
  lv_label_set_text(disk_label,"SYSTEM DISK\n--");lv_label_set_text(temp_label,"CPU TEMP\n--");
 }
}
static lv_obj_t *card(lv_obj_t *screen,int x,int y,const char *text){
 lv_obj_t *box=lv_obj_create(screen);lv_obj_set_pos(box,x,y);lv_obj_set_size(box,365,135);
 lv_obj_set_style_bg_color(box,lv_color_hex(0x1d3954),0);lv_obj_set_style_border_width(box,0,0);lv_obj_remove_flag(box,LV_OBJ_FLAG_SCROLLABLE);
 lv_obj_t *l=lv_label_create(box);lv_obj_set_width(l,335);lv_label_set_text(l,text);lv_obj_set_style_text_font(l,&lv_font_montserrat_24,0);return l;
}
static void build_ui(void){
 lv_obj_t *s=lv_screen_active();lv_obj_set_style_bg_color(s,lv_color_hex(0x10243b),0);lv_obj_set_style_text_color(s,lv_color_hex(0xffffff),0);
 host_label=lv_label_create(s);lv_obj_set_pos(host_label,25,15);lv_label_set_text(host_label,"RESOURCE PANEL | USB");
 cpu_label=card(s,25,55,"CPU LOAD\n--");ram_label=card(s,410,55,"RAM\n--");
 disk_label=card(s,25,205,"SYSTEM DISK\n--");temp_label=card(s,410,205,"CPU TEMP\n--");
 link_label=lv_label_create(s);lv_obj_set_pos(link_label,25,355);lv_label_set_text(link_label,"Waiting for host companion program...");
 audio_button=lv_button_create(s);lv_obj_set_pos(audio_button,25,395);lv_obj_set_size(audio_button,220,55);lv_obj_add_event_cb(audio_button,record_clicked,LV_EVENT_CLICKED,NULL);
 lv_obj_t *a=lv_label_create(audio_button);lv_label_set_text(a,"MIC / SPEAKER TEST");lv_obj_center(a);
 audio_label=lv_label_create(s);lv_obj_set_pos(audio_label,265,412);lv_label_set_text(audio_label,"USB telemetry - no Wi-Fi required");
 lv_timer_create(update,250,NULL);
}
void app_main(void){
 ESP_ERROR_CHECK(i2c_init());vTaskDelay(pdMS_TO_TICKS(200));ESP_ERROR_CHECK(stc8_i2c_init());ESP_ERROR_CHECK(touch_init());
 ESP_ERROR_CHECK(audio_ctrl_init());ESP_ERROR_CHECK(set_Audio_ctrl(false));ESP_ERROR_CHECK(audio_init());ESP_ERROR_CHECK(mic_init());ESP_ERROR_CHECK(display_init());
 samples=xQueueCreate(1,sizeof(metrics_t));assert(samples);
 if(lvgl_port_lock(0)){build_ui();lvgl_port_unlock();}
 ESP_ERROR_CHECK(set_lcd_blight(40));
 const uart_config_t cfg={.baud_rate=115200,.data_bits=UART_DATA_8_BITS,.parity=UART_PARITY_DISABLE,.stop_bits=UART_STOP_BITS_1,.flow_ctrl=UART_HW_FLOWCTRL_DISABLE,.source_clk=UART_SCLK_DEFAULT};
 ESP_ERROR_CHECK(uart_param_config(UART_NUM_0,&cfg));ESP_ERROR_CHECK(uart_driver_install(UART_NUM_0,2048,0,0,NULL,0));
 assert(xTaskCreate(receiver,"telemetry_rx",6144,NULL,4,NULL)==pdPASS);
}
