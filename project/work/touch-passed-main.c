#include "main.h"
#include "esp_lvgl_port.h"
#include "esp_system.h"
#include <stdint.h>
static lv_obj_t *status_label;
static unsigned taps, seen;
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
}
void app_main(void) {
 MAIN_INFO("BC250 panel test; no external control outputs enabled");
 ESP_ERROR_CHECK(i2c_init()); vTaskDelay(pdMS_TO_TICKS(200));
 ESP_ERROR_CHECK(stc8_i2c_init()); ESP_ERROR_CHECK(touch_init());
 ESP_ERROR_CHECK(display_init());
 if(lvgl_port_lock(0)){build_ui();lvgl_port_unlock();}
 ESP_ERROR_CHECK(set_lcd_blight(40));
 while(true){MAIN_INFO("ALIVE heap=%u",(unsigned)esp_get_free_heap_size());vTaskDelay(pdMS_TO_TICKS(5000));}
}
