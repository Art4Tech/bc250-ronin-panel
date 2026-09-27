#include "main.h"
#include "esp_lvgl_port.h"
#include "esp_system.h"
#include <stdint.h>
#include "bsp_mic.h"
#include "bsp_audio.h"
static lv_obj_t *status_label;
static lv_obj_t *audio_button;
static unsigned taps, seen;
static void audio_worker(void *arg) {
 (void)arg;
 esp_err_t err = mic_read_to_audio(5);
 if(lvgl_port_lock(0)) {
  lv_label_set_text(status_label,err == ESP_OK ? "Playback finished. Did you hear your voice?" : "Audio test failed - see serial log");
  lv_obj_remove_state(audio_button, LV_STATE_DISABLED);
  lvgl_port_unlock();
 }
 MAIN_INFO("AUDIO_TEST result=%s",esp_err_to_name(err));
 vTaskDelete(NULL);
}
static void record_clicked(lv_event_t *e) {
 (void)e;
 lv_obj_add_state(audio_button,LV_STATE_DISABLED);
 lv_label_set_text(status_label,"Speak now: recording 5 seconds, then playback");
 if(xTaskCreate(audio_worker,"audio_test",4096,NULL,5,NULL) != pdPASS) {
  lv_label_set_text(status_label,"Unable to start audio task");
  lv_obj_remove_state(audio_button,LV_STATE_DISABLED);
 }
}
static void tapped(lv_event_t *e) {
 unsigned n = (unsigned)(uintptr_t)lv_event_get_user_data(e);
 seen |= 1U << n; taps++;
 lv_obj_set_style_bg_color(lv_event_get_target(e), lv_color_hex(0x16794b), 0);
 lv_label_set_text_fmt(status_label,"Taps: %u  Targets: %s",taps,seen==31 ? "5/5 - PASS" : "tap all five");
 MAIN_INFO("TOUCH target=%u taps=%u mask=%u",n+1,taps,seen);
}
static void build_ui(void) {
 lv_obj_t *s=lv_screen_active();
 lv_obj_set_style_bg_color(s,lv_color_hex(0x10243b),0);
 lv_obj_set_style_text_color(s,lv_color_hex(0xffffff),0);
 lv_obj_t *title=lv_label_create(s);
 lv_label_set_text(title,"BC250 PANEL - TOUCH TEST");
 lv_obj_align(title,LV_ALIGN_TOP_MID,0,20);
 status_label=lv_label_create(s);
 lv_label_set_text(status_label,"Taps: 0  Targets: tap all five");
 lv_obj_align(status_label,LV_ALIGN_BOTTOM_MID,0,-20);
 const int xy[5][2]={{20,65},{630,65},{325,205},{20,340},{630,340}};
 for(unsigned i=0;i<5;i++) {
  lv_obj_t *b=lv_button_create(s); lv_obj_set_pos(b,xy[i][0],xy[i][1]);
  lv_obj_set_size(b,150,75);
  lv_obj_add_event_cb(b,tapped,LV_EVENT_CLICKED,(void *)(uintptr_t)i);
  lv_obj_t *l=lv_label_create(b); lv_label_set_text_fmt(l,"TAP %u",i+1); lv_obj_center(l);
 }
 audio_button=lv_button_create(s); lv_obj_set_pos(audio_button,300,305);
 lv_obj_set_size(audio_button,200,65);
 lv_obj_add_event_cb(audio_button,record_clicked,LV_EVENT_CLICKED,NULL);
 lv_obj_t *a=lv_label_create(audio_button);lv_label_set_text(a,"RECORD / PLAY");lv_obj_center(a);
}
void app_main(void) {
 MAIN_INFO("BC250 panel test; no external control outputs enabled");
 ESP_ERROR_CHECK(i2c_init()); vTaskDelay(pdMS_TO_TICKS(200));
 ESP_ERROR_CHECK(stc8_i2c_init()); ESP_ERROR_CHECK(touch_init());
 ESP_ERROR_CHECK(audio_ctrl_init()); ESP_ERROR_CHECK(set_Audio_ctrl(false));
 ESP_ERROR_CHECK(audio_init()); ESP_ERROR_CHECK(mic_init());
 ESP_ERROR_CHECK(display_init());
 if(lvgl_port_lock(0)){build_ui();lvgl_port_unlock();}
 ESP_ERROR_CHECK(set_lcd_blight(40));
 while(true){MAIN_INFO("ALIVE heap=%u",(unsigned)esp_get_free_heap_size());vTaskDelay(pdMS_TO_TICKS(5000));}
}
