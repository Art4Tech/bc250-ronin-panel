from pathlib import Path
p=Path('work/panel-test/peripheral/bsp_mic/bsp_mic.c')
s=p.read_text(encoding='utf-8-sig')
s=s[:s.index('esp_err_t mic_read_to_audio')]+r'''
esp_err_t mic_read_to_audio(size_t seconds)
{
 if (seconds < 1 || seconds > 10) return ESP_ERR_INVALID_ARG;
 size_t bytes = seconds * BYTE_RATE, received = 0, written = 0;
 int16_t *mono = heap_caps_malloc(bytes, MALLOC_CAP_SPIRAM);
 int16_t *stereo = heap_caps_malloc(bytes * 2, MALLOC_CAP_SPIRAM);
 if (!mono || !stereo) { free(mono); free(stereo); return ESP_ERR_NO_MEM; }
 esp_err_t err = set_Audio_ctrl(false);
 if (err != ESP_OK) goto done;
 MIC_INFO("Recording %u seconds", (unsigned)seconds);
 err = i2s_channel_read(rx_chan, mono, bytes, &received, 7000);
 if (err != ESP_OK) goto done;
 if (received != bytes) { err = ESP_ERR_INVALID_SIZE; goto done; }
 int peak = 0;
 for (size_t i=0; i<bytes/2; ++i) {
  int value = mono[i]; if(value < 0) value = -value;
  if(value > peak) peak = value;
 }
 MIC_INFO("Recorded bytes=%u peak=%d",(unsigned)received,peak);
 // Limit gain and playback peak for the initial speaker test.
 float gain = peak > 0 ? 4096.0f / peak : 1.0f;
 if(gain > 8.0f) gain = 8.0f;
 for (size_t i=0; i<bytes/2; ++i) {
  int16_t sample = (int16_t)(mono[i] * gain);
  stereo[2*i] = sample; stereo[2*i+1] = sample;
 }
 err = set_Audio_ctrl(true);
 if (err != ESP_OK) goto done;
 MIC_INFO("Playing recorded audio");
 err = i2s_channel_write(get_audio_handle(),stereo,bytes*2,&written,7000);
 if(err == ESP_OK && written != bytes*2) err = ESP_ERR_INVALID_SIZE;
 // Let the final DMA frames leave the output before disabling the amplifier.
 vTaskDelay(pdMS_TO_TICKS(150));
done:
 set_Audio_ctrl(false);
 free(mono); free(stereo);
 return err;
}
'''
p.write_text(s,encoding='utf-8')
