#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_vfs_fat.h"
#include "sdmmc_cmd.h"
#include "driver/sdmmc_host.h"
#include "driver/jpeg_decode.h"
#include "driver/uart.h"
#include "esp_timer.h"
#include "esp_log.h"
#include "mbedtls/base64.h"
#include "mbedtls/sha256.h"
#include "cJSON.h"
#include "lvgl.h"
#include "esp_crc.h"

#define ROOT "/sdcard/BC250"
#define FRAME_LIMIT (256*1024)
extern bool panel_video_is_night(void);
extern void panel_video_present(const lv_image_dsc_t *frame,bool night);
static SemaphoreHandle_t access_lock;
static bool mounted;
static FILE *upload;
static size_t expected,written;
static char destination[96],partial[104],expected_hash[65];
static mbedtls_sha256_context hash;
static const char *TAG="PANEL_VIDEO";
static bool allowed_asset(const char *name){const char *names[]={"day.rvj","night.rvj","day2.rvj","nite2.rvj","sleep2.rvj","wake2.rvj","nature2.pcm","day3.rvj","nite3.rvj","sleep3.rvj","wake3.rvj","day3.pcm","nite3.pcm","sleep3.pcm","wake3.pcm"};for(int i=0;i<15;i++)if(name&&!strcmp(name,names[i]))return true;return false;}
extern bool panel_movie_run(jpeg_decoder_handle_t decoder,uint8_t *input,uint8_t **output,size_t *sizes,lv_image_dsc_t *images);
static void ack(const char *status){printf("@ASSET {\"status\":\"%s\",\"offset\":%u}\n",status,(unsigned)written);fflush(stdout);}
static const char *str(cJSON *j,const char *key){cJSON *v=cJSON_GetObjectItemCaseSensitive(j,key);return cJSON_IsString(v)?v->valuestring:NULL;}
bool panel_video_command(cJSON *j){
 const char *command=str(j,"asset");if(!command)return false;
 if(!access_lock){ack("initializing");return true;}
 xSemaphoreTake(access_lock,portMAX_DELAY);
 if(!mounted){ack("no_card");goto done;}
 if(!strcmp(command,"baud")){
  cJSON *rate=cJSON_GetObjectItemCaseSensitive(j,"rate");
  if(!cJSON_IsNumber(rate)||(rate->valueint!=115200&&rate->valueint!=460800)){ack("invalid");goto done;}
  ack("ready");uart_wait_tx_done(UART_NUM_0,pdMS_TO_TICKS(1000));uart_set_baudrate(UART_NUM_0,rate->valueint);goto done;
 }
 if(!strcmp(command,"info")){ack(upload?"uploading":"ready");goto done;}
 if(!strcmp(command,"begin")){
  const char *name=str(j,"name"),*digest=str(j,"sha256");cJSON *size=cJSON_GetObjectItemCaseSensitive(j,"size");
  if(upload){ack("busy");goto done;}
  if(!allowed_asset(name)||!digest||strlen(digest)!=64||!cJSON_IsNumber(size)||size->valuedouble<16||size->valuedouble>100000000){ack("invalid");goto done;}
  snprintf(destination,sizeof(destination),ROOT"/%s",name);snprintf(partial,sizeof(partial),ROOT"/UPLOAD.TMP");
  struct stat st;if(!stat(destination,&st)||!stat(partial,&st)){ack("exists");goto done;}
  upload=fopen(partial,"wb");if(!upload){ack("write_failed");goto done;}
  expected=(size_t)size->valuedouble;written=0;strcpy(expected_hash,digest);
  mbedtls_sha256_init(&hash);mbedtls_sha256_starts(&hash,0);ack("ready");goto done;
 }
 if(!strcmp(command,"chunk")){
  const char *data=str(j,"data");cJSON *offset=cJSON_GetObjectItemCaseSensitive(j,"offset");uint8_t bytes[1024];size_t length=0;
  if(!upload||!data||!cJSON_IsNumber(offset)||offset->valuedouble!=(double)written){ack("offset");goto done;}
  if(mbedtls_base64_decode(bytes,sizeof(bytes),&length,(const unsigned char *)data,strlen(data))||!length||written+length>expected){ack("invalid");goto done;}
  cJSON *crc=cJSON_GetObjectItemCaseSensitive(j,"crc32");
  if(crc&&(!cJSON_IsNumber(crc)||crc->valuedouble!=(double)esp_crc32_le(0,bytes,length))){ack("checksum_retry");goto done;}
  if(fwrite(bytes,1,length,upload)!=length){ack("write_failed");goto done;}
  mbedtls_sha256_update(&hash,bytes,length);written+=length;ack("ok");goto done;
 }
 if(!strcmp(command,"finish")){
  if(!upload||written!=expected){ack("incomplete");goto done;}
  uint8_t digest[32];char hex[65];mbedtls_sha256_finish(&hash,digest);mbedtls_sha256_free(&hash);
  for(int i=0;i<32;i++)snprintf(hex+i*2,3,"%02x",digest[i]);
  int error=fflush(upload);if(fsync(fileno(upload)))error=-1;if(fclose(upload))error=-1;upload=NULL;
  if(error){ack("write_failed");goto done;}
  if(strcmp(hex,expected_hash)){ack("checksum_failed");goto done;}
  if(rename(partial,destination)){ack("rename_failed");goto done;}
  ack("complete");goto done;
 }
 if(!strcmp(command,"abort")){
  if(upload){fclose(upload);upload=NULL;mbedtls_sha256_free(&hash);unlink(partial);}ack("aborted");goto done;
 }
 ack("unknown");
done:xSemaphoreGive(access_lock);return true;
}
static uint32_t le32(const uint8_t *p){return p[0]|((uint32_t)p[1]<<8)|((uint32_t)p[2]<<16)|((uint32_t)p[3]<<24);}
static uint16_t le16(const uint8_t *p){return p[0]|((uint16_t)p[1]<<8);}
static void player(void *arg){
 (void)arg;vTaskDelay(pdMS_TO_TICKS(7000));
 sdmmc_host_t host=SDMMC_HOST_DEFAULT();host.slot=SDMMC_HOST_SLOT_0;host.max_freq_khz=10000;
 sdmmc_slot_config_t slot=SDMMC_SLOT_CONFIG_DEFAULT();slot.clk=43;slot.cmd=44;slot.d0=39;slot.width=1;slot.flags|=SDMMC_SLOT_FLAG_INTERNAL_PULLUP;
 esp_vfs_fat_sdmmc_mount_config_t cfg={.format_if_mount_failed=false,.max_files=4,.allocation_unit_size=16384};sdmmc_card_t *card;
 esp_err_t err=esp_vfs_fat_sdmmc_mount("/sdcard",&host,&slot,&cfg,&card);
 if(err!=ESP_OK){ESP_LOGW(TAG,"Card unavailable (%s); preserving built-in scene. No formatting performed.",esp_err_to_name(err));vTaskDelete(NULL);return;}
 struct stat st;if(stat(ROOT,&st)&&mkdir(ROOT,0775)){ESP_LOGE(TAG,"Cannot create dedicated asset folder");vTaskDelete(NULL);return;}
 xSemaphoreTake(access_lock,portMAX_DELAY);mounted=true;xSemaphoreGive(access_lock);
 ESP_LOGI(TAG,"SD mounted, %llu MiB. Asset folder: "ROOT,(unsigned long long)card->csd.capacity*card->csd.sector_size/1048576);
 jpeg_decoder_handle_t decoder;jpeg_decode_engine_cfg_t engine={.intr_priority=0,.timeout_ms=100};
 if(jpeg_new_decoder_engine(&engine,&decoder)!=ESP_OK){ESP_LOGE(TAG,"JPEG decoder unavailable");vTaskDelete(NULL);return;}
 jpeg_decode_memory_alloc_cfg_t input_cfg={.buffer_direction=JPEG_DEC_ALLOC_INPUT_BUFFER},output_cfg={.buffer_direction=JPEG_DEC_ALLOC_OUTPUT_BUFFER};
 size_t input_size,output_size[2][2];uint8_t *input=jpeg_alloc_decoder_mem(FRAME_LIMIT,&input_cfg,&input_size),*output[2][2]={{0}};lv_image_dsc_t image[2][2]={0};bool allocated=input!=NULL;
 for(int scene=0;scene<2;scene++)for(int i=0;i<2;i++){
  output[scene][i]=jpeg_alloc_decoder_mem(800*480*2,&output_cfg,&output_size[scene][i]);if(!output[scene][i])allocated=false;
  image[scene][i]=(lv_image_dsc_t){.header={.magic=LV_IMAGE_HEADER_MAGIC,.cf=LV_COLOR_FORMAT_RGB565,.w=800,.h=480,.stride=1600},.data_size=768000,.data=output[scene][i]};
 }
 if(!allocated){ESP_LOGE(TAG,"Video buffers unavailable");for(int s=0;s<2;s++)for(int i=0;i<2;i++)free(output[s][i]);free(input);jpeg_del_decoder_engine(decoder);vTaskDelete(NULL);return;}
 jpeg_decode_cfg_t decode={.output_format=JPEG_DECODE_OUT_FORMAT_RGB565,.rgb_order=JPEG_DEC_RGB_ELEMENT_ORDER_RGB};
 if(panel_movie_run(decoder,input,output[0],output_size[0],image[0])){vTaskDelete(NULL);return;}
 FILE *file=NULL;int active=-1,back[2]={0,0};uint32_t count=0,index=0,fps=15;unsigned frames=0;int64_t measured=esp_timer_get_time();
 int64_t read_us=0,decode_us=0,present_us=0,setup_us=0;
 for(;;){
  int64_t started=esp_timer_get_time();int scene=panel_video_is_night()?1:0;
  xSemaphoreTake(access_lock,portMAX_DELAY);bool busy=upload!=NULL;xSemaphoreGive(access_lock);
  if(busy){vTaskDelay(pdMS_TO_TICKS(100));continue;}
  if(active!=scene){if(file)fclose(file);file=NULL;active=scene;}
  if(!file){
   file=fopen(scene?ROOT"/night.rvj":ROOT"/day.rvj","rb");
   if(!file){vTaskDelay(pdMS_TO_TICKS(1000));continue;}
   uint8_t header[16];if(fread(header,1,16,file)!=16||memcmp(header,"RV01",4)||le16(header+4)!=800||le16(header+6)!=480||le16(header+8)<1||le16(header+8)>30||le32(header+12)<1||le32(header+12)>9000){fclose(file);file=NULL;vTaskDelay(pdMS_TO_TICKS(2000));continue;}
   fps=le16(header+8);count=le32(header+12);index=0;ESP_LOGI(TAG,"Playing %s at requested %lu fps",scene?"night":"day",(unsigned long)fps);
  }
  if(index==count){fseek(file,16,SEEK_SET);index=0;}
  int64_t read_start=esp_timer_get_time();
  uint8_t len[4];uint32_t length=0;if(fread(len,1,4,file)==4)length=le32(len);
  jpeg_decode_picture_info_t info;uint32_t decoded=0;int b=back[scene];
  bool read_ok=length&&length<=FRAME_LIMIT&&fread(input,1,length,file)==length;
  int64_t decode_start=esp_timer_get_time();
  if(!read_ok||jpeg_decoder_get_info(input,length,&info)!=ESP_OK||info.width!=800||info.height!=480||jpeg_decoder_process(decoder,&decode,input,length,output[scene][b],output_size[scene][b],&decoded)!=ESP_OK||decoded!=768000){ESP_LOGW(TAG,"Invalid video frame; retaining last valid image");fclose(file);file=NULL;vTaskDelay(pdMS_TO_TICKS(2000));continue;}
  /* The hardware RGB565 output is byte-swapped relative to LVGL's native
     little-endian pixels. Convert only the decoded video, never the LCD buffer. */
  uint16_t *native_pixels=(uint16_t *)output[scene][b];
  for(size_t p=0;p<800*480;p++)native_pixels[p]=__builtin_bswap16(native_pixels[p]);
  int64_t present_start=esp_timer_get_time();
  if(index==0){uint16_t *pixels=(uint16_t *)output[scene][b];ESP_LOGI(TAG,"Pixel probe scene=%d: %04x %04x %04x %04x",scene,pixels[0],pixels[800*100+100],pixels[800*240+400],pixels[800*400+700]);}
  panel_video_present(&image[scene][b],scene!=0);back[scene]^=1;index++;frames++;
  int64_t now=esp_timer_get_time();
  setup_us+=read_start-started;read_us+=decode_start-read_start;decode_us+=present_start-decode_start;present_us+=now-present_start;
  if(now-measured>10000000){ESP_LOGI(TAG,"Delivered %.1f fps; average ms setup=%.1f read=%.1f decode=%.1f present=%.1f",frames*1000000.0/(now-measured),setup_us/(frames*1000.0),read_us/(frames*1000.0),decode_us/(frames*1000.0),present_us/(frames*1000.0));frames=0;setup_us=read_us=decode_us=present_us=0;measured=now;}
  int wait=1000/fps-(int)((now-started)/1000);vTaskDelay(pdMS_TO_TICKS(wait>0?wait:1));
 }
}
void panel_video_start(void){access_lock=xSemaphoreCreateMutex();if(access_lock)xTaskCreate(player,"panel_video",8192,NULL,2,NULL);}
