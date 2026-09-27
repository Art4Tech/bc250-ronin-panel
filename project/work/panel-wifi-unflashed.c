#include "lvgl.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "nvs_flash.h"
#include "nvs.h"
#include <string.h>
#include <stdio.h>

typedef struct { char ssid[33]; char pass[65]; bool reconnect; } request_t;
typedef struct { char text[128]; } notice_t;
static QueueHandle_t requests, notices;
static EventGroupHandle_t flags;
static lv_obj_t *window, *ssid_field, *pass_field, *keyboard, *net_label;
static char latest[128]="Enter your 2.4 GHz network, then Connect.";
static void notice(const char *s) { notice_t n; snprintf(n.text,sizeof(n.text),"%s",s); xQueueOverwrite(notices,&n); }
static void wifi_event(void *arg, esp_event_base_t base,int32_t id,void *data) {
 (void)arg;
 if(base==WIFI_EVENT && id==WIFI_EVENT_STA_DISCONNECTED) { xEventGroupClearBits(flags,1); notice("Disconnected. Retrying; check name/password if this persists."); }
 if(base==IP_EVENT && id==IP_EVENT_STA_GOT_IP) {
  ip_event_got_ip_t *ip=data; char msg[128];
  snprintf(msg,sizeof(msg),"CONNECTED - IP " IPSTR "  |  Reconnect tests the link.",IP2STR(&ip->ip_info.ip));
  notice(msg); xEventGroupSetBits(flags,1);
 }
}
static esp_err_t start_radio(void) {
 esp_err_t e=esp_netif_init(); if(e!=ESP_OK)return e;
 e=esp_event_loop_create_default(); if(e!=ESP_OK)return e;
 if(!esp_netif_create_default_wifi_sta())return ESP_FAIL;
 wifi_init_config_t config=WIFI_INIT_CONFIG_DEFAULT();
 e=esp_wifi_init(&config); if(e!=ESP_OK)return e;
 e=esp_wifi_set_storage(WIFI_STORAGE_RAM); if(e!=ESP_OK)return e;
 e=esp_event_handler_register(WIFI_EVENT,ESP_EVENT_ANY_ID,wifi_event,NULL);if(e!=ESP_OK)return e;
 e=esp_event_handler_register(IP_EVENT,IP_EVENT_STA_GOT_IP,wifi_event,NULL);if(e!=ESP_OK)return e;
 e=esp_wifi_set_mode(WIFI_MODE_STA);if(e!=ESP_OK)return e;
 return esp_wifi_start();
}
static void wifi_worker(void *arg) {
 (void)arg;
 bool initialized=false, active=false, saved=false;
 request_t current={0}, next;
 esp_err_t e=nvs_flash_init();
 if(e!=ESP_OK){notice("Settings storage unavailable. No data erased.");vTaskDelete(NULL);return;}
 nvs_handle_t nv;
 if(nvs_open("bc250_wifi",NVS_READONLY,&nv)==ESP_OK){
  size_t a=sizeof(current.ssid), b=sizeof(current.pass);
  if(nvs_get_str(nv,"ssid",current.ssid,&a)==ESP_OK && nvs_get_str(nv,"pass",current.pass,&b)==ESP_OK) xQueueSend(requests,&current,0);
  nvs_close(nv);
 }
 TickType_t last_try=0;
 for(;;){
  if(xQueueReceive(requests,&next,pdMS_TO_TICKS(1000))==pdTRUE){
   if(next.reconnect && !active){notice("Connect to a network first.");continue;}
   if(!next.reconnect)current=next;
   if(!initialized){
    notice("Starting Wi-Fi chip - please wait...");
    e=start_radio();
    if(e!=ESP_OK){notice("Wi-Fi initialization failed. Restart panel; check serial log.");vTaskDelete(NULL);return;}
    initialized=true;
   }
   esp_wifi_disconnect();xEventGroupClearBits(flags,1);
   vTaskDelay(pdMS_TO_TICKS(200));
   wifi_config_t cfg={0};
   memcpy(cfg.sta.ssid,current.ssid,strlen(current.ssid));
   memcpy(cfg.sta.password,current.pass,strlen(current.pass));
   cfg.sta.threshold.authmode=WIFI_AUTH_WPA2_PSK;
   cfg.sta.pmf_cfg.capable=true;
   cfg.sta.sae_pwe_h2e=WPA3_SAE_PWE_BOTH;
   e=esp_wifi_set_config(WIFI_IF_STA,&cfg);
   if(e!=ESP_OK){notice("Network configuration rejected.");active=false;continue;}
   active=true;saved=false;
   notice("Connecting... Please wait for an IP address.");
   e=esp_wifi_connect(); last_try=xTaskGetTickCount();
   if(e!=ESP_OK)notice("Connection request failed; retrying in five seconds.");
  }
  if(active && !(xEventGroupGetBits(flags)&1) && xTaskGetTickCount()-last_try>pdMS_TO_TICKS(5000)){
   esp_wifi_connect();last_try=xTaskGetTickCount();
  }
  if(active && !saved && (xEventGroupGetBits(flags)&1)){
   if(nvs_open("bc250_wifi",NVS_READWRITE,&nv)==ESP_OK){
    e=nvs_set_str(nv,"ssid",current.ssid);
    if(e==ESP_OK)e=nvs_set_str(nv,"pass",current.pass);
    if(e==ESP_OK)e=nvs_commit(nv);
    nvs_close(nv);
    if(e!=ESP_OK)notice("Connected, but could not save network settings.");
   }else notice("Connected, but could not save network settings.");
   saved=true;
  }
 }
}
static void refresh(lv_timer_t *t){
 (void)t;notice_t n;
 if(xQueueReceive(notices,&n,0)==pdTRUE){snprintf(latest,sizeof(latest),"%s",n.text);if(net_label)lv_label_set_text(net_label,latest);}
}
static void focused(lv_event_t *e){lv_keyboard_set_textarea(keyboard,lv_event_get_target(e));}
static void close_window(lv_event_t *e){(void)e;lv_obj_delete(window);window=NULL;net_label=NULL;keyboard=NULL;}
static void connect_clicked(lv_event_t *e){
 request_t r={0};r.reconnect=(uintptr_t)lv_event_get_user_data(e)==1;
 if(!r.reconnect){
  const char *s=lv_textarea_get_text(ssid_field),*p=lv_textarea_get_text(pass_field);
  if(strlen(s)<1||strlen(s)>32||strlen(p)<8||strlen(p)>63){lv_label_set_text(net_label,"Use SSID 1-32 bytes and WPA2/3 password 8-63 bytes.");return;}
  snprintf(r.ssid,sizeof(r.ssid),"%s",s);snprintf(r.pass,sizeof(r.pass),"%s",p);
 }
 xQueueOverwrite(requests,&r);
 lv_label_set_text(net_label,"Request queued...");
}
static lv_obj_t *button(lv_obj_t *parent,const char *text,int x,int y,int w,lv_event_cb_t cb,uintptr_t arg){
 lv_obj_t *b=lv_button_create(parent);lv_obj_set_pos(b,x,y);lv_obj_set_size(b,w,40);
 lv_obj_add_event_cb(b,cb,LV_EVENT_CLICKED,(void *)arg);
 lv_obj_t *l=lv_label_create(b);lv_label_set_text(l,text);lv_obj_center(l);return b;
}
static void show_wifi(lv_event_t *e){
 (void)e;if(window)return;
 window=lv_obj_create(lv_screen_active());lv_obj_set_size(window,800,480);lv_obj_set_pos(window,0,0);lv_obj_set_style_pad_all(window,10,0);
 lv_obj_set_style_bg_color(window,lv_color_hex(0x10243b),0);lv_obj_remove_flag(window,LV_OBJ_FLAG_SCROLLABLE);
 ssid_field=lv_textarea_create(window);lv_obj_set_pos(ssid_field,0,0);lv_obj_set_size(ssid_field,330,45);lv_textarea_set_one_line(ssid_field,true);lv_textarea_set_max_length(ssid_field,32);lv_textarea_set_placeholder_text(ssid_field,"Network name (2.4 GHz)");
 pass_field=lv_textarea_create(window);lv_obj_set_pos(pass_field,345,0);lv_obj_set_size(pass_field,330,45);lv_textarea_set_one_line(pass_field,true);lv_textarea_set_max_length(pass_field,63);lv_textarea_set_password_mode(pass_field,true);lv_textarea_set_placeholder_text(pass_field,"Password (saved on panel)");
 button(window,"Close",685,0,85,close_window,0);
 button(window,"Connect / Save",0,55,200,connect_clicked,0);button(window,"Reconnect",215,55,180,connect_clicked,1);
 net_label=lv_label_create(window);lv_obj_set_pos(net_label,0,110);lv_obj_set_width(net_label,760);lv_label_set_text(net_label,latest);
 keyboard=lv_keyboard_create(window);lv_obj_set_size(keyboard,775,245);lv_obj_set_pos(keyboard,0,205);lv_keyboard_set_textarea(keyboard,ssid_field);
 lv_obj_add_event_cb(ssid_field,focused,LV_EVENT_FOCUSED,NULL);lv_obj_add_event_cb(pass_field,focused,LV_EVENT_FOCUSED,NULL);
}
void panel_wifi_ui_init(void){
 requests=xQueueCreate(1,sizeof(request_t));notices=xQueueCreate(1,sizeof(notice_t));flags=xEventGroupCreate();
 if(!requests||!notices||!flags)return;
 button(lv_screen_active(),"Wi-Fi",300,380,200,show_wifi,0);
 lv_timer_create(refresh,250,NULL);
 if(xTaskCreate(wifi_worker,"panel_wifi",8192,NULL,4,NULL)!=pdPASS)notice("Unable to start network task.");
}
