from pathlib import Path
p=Path('work/panel-test/main/main.c')
s=p.read_text().replace('bool has_temp;','bool has_temp,wake;')
s=s.replace('static bool was_connected;','''static bool was_connected,sleeping;
static lv_obj_t *night_image,*panel,*petals[18],*mist[3];
static int night_opacity;
static void set_sleep(bool value);
extern const uint8_t day_data[] asm("_binary_day_rgb565_start");
extern const uint8_t night_data[] asm("_binary_night_rgb565_start");
static const lv_image_dsc_t day_art={.header={.magic=LV_IMAGE_HEADER_MAGIC,.cf=LV_COLOR_FORMAT_RGB565,.w=800,.h=480,.stride=1600},.data_size=768000,.data=day_data};
static const lv_image_dsc_t night_art={.header={.magic=LV_IMAGE_HEADER_MAGIC,.cf=LV_COLOR_FORMAT_RGB565,.w=800,.h=480,.stride=1600},.data_size=768000,.data=night_data};''')
s=s.replace('m.has_temp=number', 'm.wake=cJSON_IsTrue(cJSON_GetObjectItemCaseSensitive(j,"wake"));\n  m.has_temp=number')
s=s.replace('last_received=esp_timer_get_time();was_connected=true;', 'if(sleeping&&(m.wake||esp_timer_get_time()-last_received>5000000))set_sleep(false);\n  last_received=esp_timer_get_time();was_connected=true;')
a=s.index('static lv_obj_t *card(');b=s.index('void app_main(',a)
s=s[:a]+'''static void fade(void *obj,int32_t v){
 night_opacity=v;lv_obj_set_style_opa(obj,v,0);
}
static void fade_done(lv_anim_t *a){(void)a;set_lcd_blight(sleeping?8:40);}
static void set_sleep(bool value){
 if(sleeping==value)return;
 sleeping=value;
 if(value)lv_obj_add_flag(panel,LV_OBJ_FLAG_HIDDEN);
 else {lv_obj_remove_flag(panel,LV_OBJ_FLAG_HIDDEN);set_lcd_blight(40);}
 lv_anim_delete(night_image,fade);
 lv_anim_t a;lv_anim_init(&a);lv_anim_set_var(&a,night_image);lv_anim_set_values(&a,night_opacity,value?255:0);
 lv_anim_set_duration(&a,2200);lv_anim_set_exec_cb(&a,fade);lv_anim_set_path_cb(&a,lv_anim_path_ease_in_out);lv_anim_set_completed_cb(&a,fade_done);lv_anim_start(&a);
}
static void sleep_clicked(lv_event_t *e){(void)e;set_sleep(true);}
static void scene_clicked(lv_event_t *e){(void)e;if(sleeping)set_sleep(false);}
static void atmosphere(lv_timer_t *timer){
 (void)timer;float t=esp_timer_get_time()/1000000.0f;
 for(int i=0;i<18;i++){
  float y=fmodf(t*(9+i%4*3)+i*31,530)-25;
  float x=fmodf(i*47+t*7,840)-20+12*sinf(t*.7f+i);
  lv_obj_set_pos(petals[i],(int)x,(int)y);
  lv_obj_set_style_bg_opa(petals[i],sleeping?50:150,0);
 }
 for(int i=0;i<3;i++){
  lv_obj_set_pos(mist[i],280+i*38+(int)(12*sinf(t*.22f+i)),310-i*17-(int)(9*sinf(t*.32f+i)));
  lv_obj_set_style_bg_opa(mist[i],(sleeping?8:14)+(int)(5*sinf(t*.4f+i)),0);
 }
}
static lv_obj_t *text_at(lv_obj_t *parent,int x,int y,int w,const char *text,bool large){
 lv_obj_t *l=lv_label_create(parent);lv_obj_set_pos(l,x,y);lv_obj_set_width(l,w);lv_label_set_text(l,text);
 if(large)lv_obj_set_style_text_font(l,&lv_font_montserrat_24,0);
 return l;
}
static lv_obj_t *button(lv_obj_t *parent,int x,int y,int w,const char *text,lv_event_cb_t cb){
 lv_obj_t *b=lv_button_create(parent);lv_obj_set_pos(b,x,y);lv_obj_set_size(b,w,38);
 lv_obj_set_style_bg_color(b,lv_color_hex(0x374943),0);lv_obj_set_style_bg_opa(b,200,0);lv_obj_set_style_radius(b,8,0);
 lv_obj_set_style_shadow_width(b,0,0);lv_obj_add_event_cb(b,cb,LV_EVENT_CLICKED,NULL);
 lv_obj_t *l=lv_label_create(b);lv_label_set_text(l,text);lv_obj_center(l);return b;
}
static void build_ui(void){
 lv_obj_t *s=lv_screen_active();lv_obj_remove_flag(s,LV_OBJ_FLAG_SCROLLABLE);
 lv_obj_set_style_text_color(s,lv_color_hex(0xf7edda),0);
 lv_obj_t *day=lv_image_create(s);lv_image_set_src(day,&day_art);
 night_image=lv_image_create(s);lv_image_set_src(night_image,&night_art);lv_obj_set_style_opa(night_image,0,0);
 lv_obj_t *touch=lv_obj_create(s);lv_obj_remove_style_all(touch);lv_obj_set_size(touch,800,480);
 lv_obj_add_flag(touch,LV_OBJ_FLAG_CLICKABLE);lv_obj_add_event_cb(touch,scene_clicked,LV_EVENT_CLICKED,NULL);
 for(int i=0;i<3;i++){
  mist[i]=lv_obj_create(s);lv_obj_remove_style_all(mist[i]);lv_obj_set_size(mist[i],180,35);
  lv_obj_set_style_radius(mist[i],LV_RADIUS_CIRCLE,0);lv_obj_set_style_bg_color(mist[i],lv_color_hex(0xf3ede4),0);lv_obj_remove_flag(mist[i],LV_OBJ_FLAG_CLICKABLE);
 }
 for(int i=0;i<18;i++){
  petals[i]=lv_obj_create(s);lv_obj_remove_style_all(petals[i]);lv_obj_set_size(petals[i],4+i%3,3+i%2);
  lv_obj_set_style_radius(petals[i],3,0);lv_obj_set_style_bg_color(petals[i],lv_color_hex(0xffc7d5),0);lv_obj_remove_flag(petals[i],LV_OBJ_FLAG_CLICKABLE);
 }
 panel=lv_obj_create(s);lv_obj_set_pos(panel,476,16);lv_obj_set_size(panel,308,448);
 lv_obj_set_style_bg_color(panel,lv_color_hex(0x162923),0);lv_obj_set_style_bg_opa(panel,205,0);
 lv_obj_set_style_border_width(panel,1,0);lv_obj_set_style_border_color(panel,lv_color_hex(0xafa88d),0);lv_obj_set_style_border_opa(panel,90,0);
 lv_obj_set_style_radius(panel,16,0);lv_obj_set_style_pad_all(panel,14,0);lv_obj_remove_flag(panel,LV_OBJ_FLAG_SCROLLABLE);
 text_at(panel,0,0,270,"MOUNTAIN REFUGE",true);
 host_label=text_at(panel,0,32,278,"RESOURCE PANEL | USB",false);
 cpu_label=text_at(panel,0,64,132,"CPU LOAD\\n--",true);
 temp_label=text_at(panel,145,64,133,"CPU TEMP\\n--",true);
 ram_label=text_at(panel,0,148,278,"RAM\\n--",true);
 disk_label=text_at(panel,0,245,278,"SYSTEM DISK\\n--",true);
 link_label=text_at(panel,0,313,278,"Waiting for USB host",false);
 button(panel,0,346,133,"SLEEP DISPLAY",sleep_clicked);
 audio_button=button(panel,143,346,135,"AUDIO TEST",record_clicked);
 audio_label=text_at(panel,0,393,278,"Sakura drift | USB resources",false);
 lv_timer_create(update,250,NULL);lv_timer_create(atmosphere,50,NULL);
}
''' +s[b:]
p.write_text(s)
p=Path('work/panel-test/main/CMakeLists.txt');s=p.read_text();s+='\ntarget_add_binary_data(${COMPONENT_LIB} "${CMAKE_CURRENT_SOURCE_DIR}/day.rgb565" BINARY)\ntarget_add_binary_data(${COMPONENT_LIB} "${CMAKE_CURRENT_SOURCE_DIR}/night.rgb565" BINARY)\n';p.write_text(s)
