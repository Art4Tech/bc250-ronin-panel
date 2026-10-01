#include <string.h>
#include <stdio.h>
#include <stdatomic.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "esp_hosted.h"
#include "esp_hosted_bluedroid.h"
#include "esp_bt_main.h"
#include "esp_gap_ble_api.h"
#include "esp_hidh.h"

static QueueHandle_t notices,targets;
typedef struct {uint8_t address[6];uint8_t type;} target_t;
static target_t selected;
static atomic_bool connecting;
static bool known_peer;
static uint8_t peer_address[6];
static atomic_bool wake_pending;
bool panel_bt_take_wake(void){return atomic_exchange(&wake_pending,false);}
static const char *TAG="PANEL_BT";
static void notice(const char *s){char msg[96];snprintf(msg,sizeof(msg),"%s",s);ESP_LOGI(TAG,"%s",msg);if(notices)xQueueOverwrite(notices,msg);}
bool panel_bt_poll(char *out){return notices&&xQueueReceive(notices,out,0)==pdTRUE;}
static esp_ble_scan_params_t scan={.scan_type=BLE_SCAN_TYPE_ACTIVE,.own_addr_type=BLE_ADDR_TYPE_PUBLIC,.scan_filter_policy=BLE_SCAN_FILTER_ALLOW_ALL,.scan_interval=0x80,.scan_window=0x40,.scan_duplicate=BLE_SCAN_DUPLICATE_DISABLE};
static void gap(esp_gap_ble_cb_event_t event,esp_ble_gap_cb_param_t *p){
 if(event==ESP_GAP_BLE_SCAN_PARAM_SET_COMPLETE_EVT){notice("BT: searching for GameSir");esp_ble_gap_start_scanning(30);}
 if(event==ESP_GAP_BLE_SCAN_RESULT_EVT){
  if(p->scan_rst.search_evt==ESP_GAP_SEARCH_INQ_CMPL_EVT&&!connecting){esp_ble_gap_start_scanning(30);return;}
  if(p->scan_rst.search_evt!=ESP_GAP_SEARCH_INQ_RES_EVT||connecting)return;
  uint8_t length=0;uint8_t *name=esp_ble_resolve_adv_data(p->scan_rst.ble_adv,ESP_BLE_AD_TYPE_NAME_CMPL,&length);
  if(!name)name=esp_ble_resolve_adv_data(p->scan_rst.ble_adv,ESP_BLE_AD_TYPE_NAME_SHORT,&length);
  char text[64]={0};if(name)memcpy(text,name,length<63?length:63);
  if(strstr(text,"GameSir-Nova 2 Lite")||(known_peer&&!memcmp(peer_address,p->scan_rst.bda,6))){
   connecting=true;memcpy(selected.address,p->scan_rst.bda,6);selected.type=p->scan_rst.ble_addr_type;
   notice("BT: GameSir found, connecting");esp_ble_gap_stop_scanning();xQueueOverwrite(targets,&selected);
  }
 }
 if(event==ESP_GAP_BLE_SEC_REQ_EVT){esp_ble_gap_security_rsp(p->ble_security.ble_req.bd_addr,connecting&&!memcmp(selected.address,p->ble_security.ble_req.bd_addr,6));}
 if(event==ESP_GAP_BLE_AUTH_CMPL_EVT){
  notice(p->ble_security.auth_cmpl.success?"BT: GameSir pairing accepted":"BT: GameSir pairing failed");
  if(p->ble_security.auth_cmpl.success){memcpy(peer_address,selected.address,6);known_peer=true;nvs_handle_t n;if(nvs_open("panel_bt",NVS_READWRITE,&n)==ESP_OK){nvs_set_blob(n,"peer",peer_address,6);nvs_commit(n);nvs_close(n);}}
 }
}
static void hid(void *arg,esp_event_base_t base,int32_t event,void *data){
 (void)arg;(void)base;esp_hidh_event_data_t *p=data;
 if(event==ESP_HIDH_OPEN_EVENT){
  if(p->open.status==ESP_OK){notice("BT: GameSir connected - wake");atomic_store(&wake_pending,true);esp_hidh_dev_dump(p->open.dev,stdout);}
  else{notice("BT: HID connection failed");connecting=false;esp_ble_gap_start_scanning(30);}
 }else if(event==ESP_HIDH_CLOSE_EVENT){notice("BT: GameSir disconnected");connecting=false;esp_ble_gap_start_scanning(30);
 }else if(event==ESP_HIDH_INPUT_EVENT){
  // Diagnostics only: no power pin and no wake action until Home is identified.
  static int64_t last_notice,last_log;static uint8_t previous[64],previous_id;static size_t previous_len;int64_t now=esp_timer_get_time();
  if(now-last_notice>500000){notice("BT: controller input received");last_notice=now;}
  size_t length=p->input.length>sizeof(previous)?sizeof(previous):p->input.length;
  if(now-last_log>50000&&(previous_len!=length||previous_id!=p->input.report_id||memcmp(previous,p->input.data,length))){
   ESP_LOGI(TAG,"INPUT id=%u len=%u",p->input.report_id,(unsigned)p->input.length);
   ESP_LOG_BUFFER_HEX(TAG,p->input.data,length);
   memcpy(previous,p->input.data,length);previous_len=length;previous_id=p->input.report_id;last_log=now;
  }
 }
}
static bool check(esp_err_t err,const char *stage){if(err==ESP_OK)return true;char s[96];snprintf(s,sizeof(s),"BT: %s: %s",stage,esp_err_to_name(err));notice(s);return false;}
static void worker(void *arg){
 (void)arg;notice("BT: starting panel radio");
 if(!check(nvs_flash_init(),"storage"))goto done;
 nvs_handle_t n;if(nvs_open("panel_bt",NVS_READONLY,&n)==ESP_OK){size_t len=6;known_peer=nvs_get_blob(n,"peer",peer_address,&len)==ESP_OK&&len==6;nvs_close(n);}
 if(!check(esp_hosted_init(),"transport init"))goto done;
 if(!check(esp_hosted_connect_to_slave(),"radio link"))goto done;
 esp_err_t controller=esp_hosted_bt_controller_init();
 if(controller==ESP_ERR_NOT_SUPPORTED){
  // Factory C6 2.3 firmware advertises BLE/HCI and starts its controller itself.
  notice("BT: using factory HCI controller");
 }else{
  if(!check(controller,"controller init"))goto done;
  if(!check(esp_hosted_bt_controller_enable(),"controller enable"))goto done;
 }
 hosted_hci_bluedroid_open();
 esp_bluedroid_hci_driver_operations_t ops={.send=hosted_hci_bluedroid_send,.check_send_available=hosted_hci_bluedroid_check_send_available,.register_host_callback=hosted_hci_bluedroid_register_host_callback};
 esp_bluedroid_attach_hci_driver(&ops);
 if(!check(esp_bluedroid_init(),"host init"))goto done;
 if(!check(esp_bluedroid_enable(),"host enable"))goto done;
 if(!check(esp_ble_gap_register_callback(gap),"scan callback"))goto done;
 if(!check(esp_ble_gattc_register_callback(esp_hidh_gattc_event_handler),"HID callback"))goto done;
 esp_hidh_config_t config={.callback=hid,.event_stack_size=6144,.callback_arg=NULL};
 if(!check(esp_hidh_init(&config),"HID init"))goto done;
 uint8_t auth=ESP_LE_AUTH_BOND,io=ESP_IO_CAP_NONE,keysize=16;
 esp_ble_gap_set_security_param(ESP_BLE_SM_AUTHEN_REQ_MODE,&auth,1);
 esp_ble_gap_set_security_param(ESP_BLE_SM_IOCAP_MODE,&io,1);
 esp_ble_gap_set_security_param(ESP_BLE_SM_MAX_KEY_SIZE,&keysize,1);
 if(!check(esp_ble_gap_set_scan_params(&scan),"scan"))goto done;
 for(;;){target_t t;if(xQueueReceive(targets,&t,portMAX_DELAY)==pdTRUE){vTaskDelay(pdMS_TO_TICKS(200));if(!esp_hidh_dev_open(t.address,ESP_HID_TRANSPORT_BLE,t.type)&&connecting){connecting=false;notice("BT: retrying GameSir scan");esp_ble_gap_start_scanning(30);}}}
done:vTaskDelete(NULL);
}
void panel_bt_start(void){notices=xQueueCreate(1,96);targets=xQueueCreate(1,sizeof(target_t));if(notices&&targets)xTaskCreate(worker,"panel_bt",8192,NULL,3,NULL);}

