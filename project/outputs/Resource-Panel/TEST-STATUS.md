# Sakura dashboard test status

## September 23 media update — installed

User confirmed Windows CPU temperature now works and supplied dayidle, nightidle, sleepytime, and wakeuptime MP4s. User explicitly requested each file's own audio, with transition audio allowed to continue into idle playback.

Prepared full 20.1-second idle loops at 10 fps. Both transition videos have black tails beginning at 5.2 seconds; these video tails were removed while full approximately 7.8/9.13-second audio was preserved. All 506 converted JPEG frames and four PCM tracks validated. Originals and conversion manifest are in video/movies-v3.

Player now uses elapsed-time frame selection and per-clip audio. On transition completion, remaining transition audio plays over the idle scene before handing over to idle audio at the corresponding elapsed position. Muting leaves the media clock running. This behavior still needs physical validation.

The first large-packet upload rejected a packet and aborted safely. Added per-packet CRC32 checks, bounded retries, an 8 KB UART receive buffer, and a larger receiver stack. Corrected firmware built/flashed with verification. All eight v3 files transferred and passed SHA256 checks. After restart, the complete v3 video/audio set loaded with 4,687,060 bytes of PSRAM remaining. Initial measured playback is 10.1 fps. Loading completed about 47 seconds after boot (the built-in scene remains available while loading). Live monitoring resumed without reported companion errors. Physical loop/audio-tail confirmation is pending. Prior v2 media is preserved as fallback.

## Windows temperature live verification — 2026-09-20

Libre Hardware Monitor's installed .NET 10 build returned Invalid namespace for WMI. Its local HTTP feed on port 8085 works with the user-selected 0.0.0.0 listener. The companion connects only to 127.0.0.1. This release supplies unit-bearing strings even in RawValue; the reader now handles Celsius/Fahrenheit strings, decimal commas, numeric values, and missing data. CPU distance-to-TjMax values are excluded. Seven sensor tests passed; a live read selected CPU Package at 69.0 C. The persistent companion restarted successfully with this fix. Physical panel temperature confirmation remains pending. A 0.0.0.0 listener may be accessible on the LAN depending on firewall rules.

## Settings / QR update — 2026-09-20

Added SETTINGS with a 225x225 QR code, literal official Libre Hardware Monitor release URL, installation guidance, Close control, and current USB/temperature-received status. The QR image was independently decoded using ZXing and matched the exact intended HTTPS URL. This is a helper-download link; the companion package has no public hosting URL yet.

Added optional background WMI polling to the Windows companion. Tests passed for CPU package preference, AMD identification, excluding GPU/disk sensors, invalid data, and stale-data expiry; the existing three collector tests also passed. Real Libre Hardware Monitor sensor collection and physical QR scanning are not yet verified. Existing video/audio assets are unchanged.

## Current update — 2026-09-20

User confirmed the earlier video's colors were correct and flashing was absent. The procedural stronger-motion preview was rejected and never installed.

User supplied day.mp4 (day to night) and night.mp4 (night to day), and confirmed day.mp4 is the nature-only audio source. Four playback sections have been prepared and all 124 JPEG frames validated. Source and segment details are in video/user-clips/README.md.

The new firmware caches the four clips in PSRAM, plays transitions once, loops the opening sections, and provides a Mute / Sound control for a shared 4.7-second nature loop. It preserves the original artwork clips as fallback. Physical verification of playback, transitions, and sound remains pending.

The literal `f` readings were caused by using LVGL's minimal formatter with CONFIG_LV_USE_FLOAT disabled. Labels now use libc snprintf followed by lv_label_set_text. The Windows collector returned actual CPU/RAM/disk numbers when checked; temperature remains null because this collector does not provide Windows CPU temperature.

Firmware dashboard-user-movies.bin built and flashed with verification. SHA256: 3A49FC45DFC09D237EE76C71BE90BF0D01F50C8CBE1E78DFCA92F1B13BEF8C51. All four video files and nature2.pcm transferred and passed SHA256 verification. After restart, logs confirm all clips cached in PSRAM and nature audio initialized. Daylight playback measures 10.6 fps against the 12 fps target. Live monitoring resumed. User visual/audio confirmation is pending; successful initialization alone does not establish perceived audio quality or display stability.

## Current update — 2026-09-19

- The inserted 32 GB card mounts successfully alongside the C6 radio. Both day and night clips were transferred over USB serial and SHA256 verified. No card formatting or unrelated file removal occurred.
- Full-screen hardware-JPEG playback is implemented. Each clip contains 60 distinct 800x480 frames, requested at 15 fps. These animate the painted environment; the character's standing/sleeping poses still crossfade.
- Initial actual delivery was about 5.3 fps by day and 7 fps by night. Timing measurements found approximately 103 ms card read, 5 ms decode, and 77 ms waiting to read the scene state per daylight frame. The unnecessary scene-state lock wait is now removed using an atomic state.
- User reported inverted-looking video colors in both scenes. Hardware pixel diagnostics identified byte ordering opposite LVGL's native RGB565. The current update converts decoded video pixels only; the LCD and resource text configuration are unchanged. Physical confirmation remains pending.
- The correction was flashed and hash-verified, and monitoring restarted. First daylight timing is 6.8 fps: setup 0.1 ms, card read 111.2 ms, decode plus pixel conversion 26.6 ms, presentation 7.6 ms. Card reading and the conversion are the next optimization targets; 15 fps has not been achieved.
- The RGB driver now uses internal bounce buffers and the matching bounce-frame-complete synchronization. Earlier fixes did not eliminate long-run flashing; stability of this version still requires a longer physical test.
- GameSir G-Touch BLE pairing previously succeeded. The firmware now requests display wake on a successful new HID connection; user confirmation is pending. No input reports were received during the earlier A/Home test. Home-button mapping, gaming handoff, and BC250 relay output remain unfinished.
- Current experimental binary: firmware/dashboard-video-test.bin. Matching sources: video-dashboard-main.c, panel-video.c, panel-bluetooth.c, display-driver-video.c. Earlier dashboard.bin remains the artwork-only fallback.
- Windows resource monitoring runs persistently in the background. Linux hardware validation, native USB video streaming, and USB microphone/speaker operation remain pending.

Earlier entries below are historical and may describe superseded builds.

- Built for Elecrow Advanced 5-inch ESP32-P4 V1.0, ESP-IDF 5.5.4, LVGL 9.1.0.
- Original factory backup remains in ../Panel-Recovery.
- Day/night assets embedded; 18 moving petals, three drifting mist shapes, 2.2-second pose crossfade.
- First artwork build showed incorrect colors with flashing rectangles during redraw, reported by user.
- Found vendor display configuration `swap_bytes=true` with direct frame buffers. The LVGL port swaps the buffer in place during flush. Disabled byte swapping for the native parallel RGB565 display.
- Corrected app built and flashed to COM10 at 0x10000; flash hash verification passed. Awaiting user's visual confirmation of correction and sleep/touch behavior.
- Corrected dashboard.bin SHA256: 94AED0635D86E43342D9A3C3A43E3AA539AADD51902F79189CF1E820ED9EDF46.
- Companion sensor tests: 3 passed. USB connection on Windows confirmed. Linux hardware test remains pending.
- Host reconnect/resume wake behavior implemented; still requires physical observation. Regular telemetry does not intentionally wake manual display sleep.
- Controller: Nova 2 Lite. User has it ready; asked them to select Android Bluetooth mode (yellow, rapid flash) and leave it unpaired. Awaiting confirmation of that mode.
- Windows BLE scanner prepared at ../../work/scan-controller.py; initial scan before pairing readiness found no matching advertisement, which does not establish incompatibility. No pairing or controller firmware change performed.
- No Bluetooth implementation or BC250 power-switch output is enabled in this dashboard build.

Artwork generated with the built-in image generation tool. Original PNGs and full prompts are in art/. Firmware source is dashboard-main.c; vendor display correction is in display-driver.c. Development project remains ../../work/panel-test.

## Update: confirmed display and working Bluetooth scan

User confirmed resource readings are visible and night scene stable after full-frame double buffering, explicit ivory label colors on a lighter pine-green card, and disabling night mist. 36 petals, swaying transparent sakura branch, water ripples, cloud drift added. Day/night backlight now 60%/12%. Persistent monitor manager starts/stops verified Python processes and captures bounded panel-runtime.log.

Bluetooth test initially failed at startup due to internal-RAM SDIO mempool allocation. Resolved using ESP_HOSTED_MEMPOOL_PREFER_SPIRAM, eight-entry TX/RX queues, Bluetooth allocation from PSRAM first, malloc internal threshold 1024 and FreeRTOS 1000 Hz. Factory C6 reports hosted version 2.3.0, BLE/HCI supported. Its controller-init RPC is unsupported, so the test uses the already-running factory HCI controller. Radio now reaches "BT: searching for GameSir" with no C6 firmware rewrite. Updated diagnostic app flashed and verified; current panel runs dashboard-bluetooth-test.bin. dashboard.bin remains the user-verified art-only fallback.

GameSir Nova 2 Lite_G advertised BLE HID UUID 1812 at [controller address omitted]. Windows connected but did not expose HID service through Bleak; no permanent Windows pairing requested. Panel now scans and attempts HID pairing with the GameSir name. Awaiting user re-entry into rapid-flashing mode. No relay/power output or mapped Home wake action enabled yet. panel_bluetooth.c is in the development project; exports panel_bt_start/panel_bt_poll called by main.c. Public dashboard-main.c remains the art-only fallback source, not the current BT test main.

User has a 16 or 32 GB microSD available. No card formatted or files erased. Elecrow V1.0 Lesson12 bsp_sd uses SDMMC slot0, CLK43 CMD44 D039 width1 at10MHz; C6 uses slot1. Card support is not yet added to dashboard. User asks about full-screen animation and BC250 streaming. Earlier advice favoring layers was too categorical: compressed MJPEG with P4 hardware JPEG decoder should be benchmarked from SD/native USB or Wi-Fi. Current UART0 USB telemetry is115200 and not suitable for video. Need native USB port wiring/speed verification before promising streaming. Keep local night loop for host-off operation. Hardware JPEG docs IDF5.5.4 reports isolated800x480 decode253fps, not end-to-end screen performance. No video renderer/decoder implemented yet.
