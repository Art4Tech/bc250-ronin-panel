/**
 * @file bsp_mic.c
 * @brief Teaching source for 5inch_P4_IDF_11_Playback_After_Recording.
 *
 * This file is part of the CrowPanel Advanced 5-inch ESP32-P4 course.
 * The comments explain module responsibilities and observable behavior
 * without changing the original program logic.
 */

/*————————————————————————————————————————Header file declaration————————————————————————————————————————*/
#include "bsp_mic.h"    // Include the microphone module header file
/*——————————————————————————————————————Header file declaration end——————————————————————————————————————*/

/*——————————————————————————————————————————Variable declaration—————————————————————————————————————————*/
i2s_chan_handle_t rx_chan;    // Global I2S receive channel handle for microphone
/*————————————————————————————————————————Variable declaration end———————————————————————————————————————*/

/*—————————————————————————————————————————Functional function———————————————————————————————————————————*/

esp_err_t mic_init()    // Initialize the microphone I2S channel
{
    esp_err_t err = ESP_OK;    // Initialize error variable

    i2s_chan_config_t rx_chan_cfg = {    // Configure I2S channel parameters
        .id = I2S_NUM_0,                 // Use I2S port 0
        .role = I2S_ROLE_MASTER,         // Set as master
        .dma_desc_num = 6,               // Number of DMA descriptors
        .dma_frame_num = 256,            // Number of frames per DMA descriptor
        .auto_clear_after_cb = true,     // Auto-clear DMA after callback
        .auto_clear_before_cb = true,    // Auto-clear DMA before callback
        .allow_pd = false,               // Disallow power-down mode
        .intr_priority = 0,              // Interrupt priority
    };
    err = i2s_new_channel(&rx_chan_cfg, NULL, &rx_chan);    // Create I2S receive channel
    if (err != ESP_OK)
        return err;    // Return if failed

    i2s_pdm_rx_config_t pdm_rx_cfg = {    // Configure PDM receive parameters
        .clk_cfg = {                       // Clock configuration
            .sample_rate_hz = MIC_SAMPLE_RATE,    // Set sample rate 16kHz
            .clk_src = I2S_CLK_SRC_DEFAULT,       // Use default clock source
            .mclk_multiple = I2S_MCLK_MULTIPLE_256, // MCLK multiplier
            .dn_sample_mode = I2S_PDM_DSR_8S,       // Downsample mode
            .bclk_div = 8,                           // Bit clock divider
        },
        /* The data bit-width of PDM mode is fixed to 16 */
        .slot_cfg = {                   // Slot configuration
            .data_bit_width = I2S_DATA_BIT_WIDTH_16BIT,    // 16-bit data width
            .slot_bit_width = I2S_SLOT_BIT_WIDTH_AUTO,     // Slot bit width auto
            .slot_mode = I2S_SLOT_MODE_MONO,              // Mono mode
            .slot_mask = I2S_PDM_SLOT_LEFT,              // Left channel used
            .hp_en = true,                               // Enable high-pass filter
            .hp_cut_off_freq_hz = 35.5,                  // High-pass cutoff frequency
            .amplify_num = 1,                            // Amplification factor
        },
        .gpio_cfg = {                   // GPIO configuration
            .clk = MIC_GPIO_CLK,        // Clock pin
            .din = MIC_GPIO_SDIN2,      // Data input pin
            .invert_flags = {
                .clk_inv = false,      // Clock polarity not inverted
            },
        },
    };
    err = i2s_channel_init_pdm_rx_mode(rx_chan, &pdm_rx_cfg);    // Initialize PDM RX mode
    if (err != ESP_OK)
        return err;    // Return if failed
    err = i2s_channel_enable(rx_chan);    // Enable the RX channel
    if (err != ESP_OK)
        return err;    // Return if failed
    return err;    // Return success
}


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
