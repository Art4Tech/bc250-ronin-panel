#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdatomic.h>
#include <sys/stat.h>
#include "esp_heap_caps.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "driver/jpeg_decode.h"
#include "bsp_audio.h"
#include "lvgl.h"
extern bool panel_video_is_night(void);
extern void panel_movie_present(const lv_image_dsc_t *,bool);
static atomic_bool muted;
static uint8_t *audio_data[4];static size_t audio_size[4];
static SemaphoreHandle_t audio_lock;
static int requested_audio;static int64_t audio_epoch;
static unsigned audio_generation;
static void audio_request(int state,int64_t epoch){xSemaphoreTake(audio_lock,portMAX_DELAY);requested_audio=state;audio_epoch=epoch;audio_generation++;xSemaphoreGive(audio_lock);}
bool panel_movie_toggle_audio(void){bool next=!atomic_load(&muted);atomic_store(&muted,next);return next;}
static uint32_t movie_u32(const uint8_t *p){return p[0]|((uint32_t)p[1]<<8)|((uint32_t)p[2]<<16)|((uint32_t)p[3]<<24);}
static uint16_t movie_u16(const uint8_t *p){return p[0]|((uint16_t)p[1]<<8);}
typedef struct {uint8_t *data;size_t size;uint32_t count,fps;uint32_t *offset;} clip_t;
static uint8_t *load(const char *name,size_t *length){
 char path[96];snprintf(path,sizeof(path),"/sdcard/BC250/%s",name);struct stat st;
 if(stat(path,&st)||st.st_size<16||st.st_size>14000000)return NULL;
 uint8_t *data=heap_caps_malloc(st.st_size,MALLOC_CAP_SPIRAM);if(!data)return NULL;
 FILE *f=fopen(path,"rb");if(!f){free(data);return NULL;}
 bool ok=fread(data,1,st.st_size,f)==st.st_size;fclose(f);if(!ok){free(data);return NULL;}*length=st.st_size;return data;
}
static bool load_clip(const char *name,clip_t *c){
 c->data=load(name,&c->size);if(!c->data)return false;
 if(memcmp(c->data,"RV01",4)||movie_u16(c->data+4)!=800||movie_u16(c->data+6)!=480)return false;
 c->fps=movie_u16(c->data+8);c->count=movie_u32(c->data+12);
 if(c->fps<1||c->fps>30||!c->count||c->count>9000)return false;
 c->offset=calloc(c->count,sizeof(uint32_t));if(!c->offset)return false;
 size_t p=16;for(uint32_t i=0;i<c->count;i++){if(p+4>c->size)return false;uint32_t n=movie_u32(c->data+p);if(!n||n>256*1024||p+4+n>c->size)return false;c->offset[i]=p;p+=4+n;}return p==c->size;
}
static void nature(void *unused){
 (void)unused;size_t pos=0;bool amp=false;int active=-1;unsigned seen=0;
 for(;;){
  xSemaphoreTake(audio_lock,portMAX_DELAY);int desired=requested_audio;int64_t epoch=audio_epoch;unsigned generation=audio_generation;xSemaphoreGive(audio_lock);
  bool tail=active>=2&&desired<2&&pos<audio_size[active];
  if(active<0||(!tail&&(seen!=generation||pos>=audio_size[active]))){
   active=desired;seen=generation;int64_t elapsed=esp_timer_get_time()-epoch;
   pos=((size_t)(elapsed>0?elapsed/1000:0)*64)%audio_size[active];pos-=pos%4;
  }
  bool enabled=!atomic_load(&muted);if(enabled!=amp){set_Audio_ctrl(enabled);amp=enabled;}
  /* Continue the media clock while muted, so sound resumes in sync. */
  if(!enabled){pos+=2048;if(active<2)pos%=audio_size[active];vTaskDelay(pdMS_TO_TICKS(32));continue;}
  size_t n=audio_size[active]-pos;if(n>2048)n=2048;size_t written=0;
  esp_err_t err=i2s_channel_write(get_audio_handle(),audio_data[active]+pos,n,&written,1000);
  if(err!=ESP_OK||!written){ESP_LOGW("PANEL_MOVIE","Audio stopped: %s",esp_err_to_name(err));set_Audio_ctrl(false);vTaskDelete(NULL);return;}
  pos+=written;if(active<2)pos%=audio_size[active];
 }
}
bool panel_movie_run(jpeg_decoder_handle_t decoder,uint8_t *input,uint8_t **output,size_t *sizes,lv_image_dsc_t *images){
 clip_t clips[4]={0};const char *names[]={"day3.rvj","nite3.rvj","sleep3.rvj","wake3.rvj"};const char *sounds[]={"day3.pcm","nite3.pcm","sleep3.pcm","wake3.pcm"};
 const char *old_names[]={"day2.rvj","nite2.rvj","sleep2.rvj","wake2.rvj"};bool ok=false;int version=3;
 for(int attempt=0;attempt<2;attempt++){
  ok=true;version=attempt?2:3;
  for(int i=0;i<4;i++){
   if(!load_clip(attempt?old_names[i]:names[i],&clips[i])){ok=false;break;}
   audio_data[i]=load(attempt?"nature2.pcm":sounds[i],&audio_size[i]);if(!audio_data[i]||audio_size[i]%4){ok=false;break;}
  }
  if(ok)break;
  for(int i=0;i<4;i++){free(clips[i].data);free(clips[i].offset);free(audio_data[i]);audio_data[i]=NULL;memset(&clips[i],0,sizeof(clips[i]));}
 }
 if(!ok){ESP_LOGW("PANEL_MOVIE","Movie assets incomplete or memory unavailable; preserving artwork");return false;}
 audio_lock=xSemaphoreCreateMutex();if(!audio_lock)return false;
 ESP_LOGI("PANEL_MOVIE","V%d clips/audio cached; PSRAM remaining %u bytes",version,(unsigned)heap_caps_get_free_size(MALLOC_CAP_SPIRAM));
 jpeg_decode_cfg_t cfg={.output_format=JPEG_DECODE_OUT_FORMAT_RGB565,.rgb_order=JPEG_DEC_RGB_ELEMENT_ORDER_RGB};
 int state=panel_video_is_night()?1:0,back=0;uint32_t index=0;bool target=state==1;int64_t measured=esp_timer_get_time(),epoch=measured;unsigned frames=0;int previous=-1;
 audio_request(state,epoch);if(xTaskCreate(nature,"nature_audio",4096,NULL,3,NULL)!=pdPASS)ESP_LOGW("PANEL_MOVIE","Audio task unavailable");
 for(;;){
  int64_t started=esp_timer_get_time();bool desired=panel_video_is_night();
  if(desired!=target){target=desired;state=desired?2:3;epoch=started;previous=-1;audio_request(state,epoch);ESP_LOGI("PANEL_MOVIE","Transition: %s",desired?"day to night":"night to day");}
  clip_t *c=&clips[state];index=(uint32_t)((started-epoch)*c->fps/1000000);
  if(state>=2&&index>=c->count){epoch+=((int64_t)c->count*1000000/c->fps);state=target?1:0;c=&clips[state];index=(uint32_t)((started-epoch)*c->fps/1000000);previous=-1;audio_request(state,epoch);ESP_LOGI("PANEL_MOVIE","Loop: %s; transition audio tail may continue",target?"sleeping":"daylight");}
  index%=c->count;if((int)index==previous){vTaskDelay(pdMS_TO_TICKS(2));continue;}
  uint32_t p=c->offset[index],length=movie_u32(c->data+p);memcpy(input,c->data+p+4,length);
  jpeg_decode_picture_info_t info;uint32_t decoded=0;
  esp_err_t err=jpeg_decoder_get_info(input,length,&info);
  if(err==ESP_OK&&info.width==800&&info.height==480)err=jpeg_decoder_process(decoder,&cfg,input,length,output[back],sizes[back],&decoded);
  if(err!=ESP_OK||decoded!=768000){ESP_LOGE("PANEL_MOVIE","Frame decode failed; retaining previous frame");vTaskDelay(pdMS_TO_TICKS(100));continue;}
  uint16_t *pixels=(uint16_t *)output[back];for(size_t n=0;n<384000;n++)pixels[n]=__builtin_bswap16(pixels[n]);
  panel_movie_present(&images[back],state<2);back^=1;previous=index;frames++;
  int64_t now=esp_timer_get_time();if(now-measured>10000000){ESP_LOGI("PANEL_MOVIE","Delivered %.1f fps",frames*1000000.0/(now-measured));frames=0;measured=now;}
  int wait=1000/c->fps-(int)((now-started)/1000);vTaskDelay(pdMS_TO_TICKS(wait>0?wait:1));
 }
}
