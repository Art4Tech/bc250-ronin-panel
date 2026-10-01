# September 23 video set

Sources are preserved as supplied. Mapping:

- dayidle.mp4 → day3.rvj and day3.pcm: 20.1-second normal loop.
- nightidle.mp4 → nite3.rvj and nite3.pcm: 20.1-second sleep loop.
- sleepytime.mp4 → sleep3.rvj: first 5.2 seconds; sleep3.pcm: full approximately 7.8-second audio.
- wakeuptime.mp4 → wake3.rvj: first 5.2 seconds; wake3.pcm: full approximately 9.13-second audio.

Both transition sources contain black footage after 5.2 seconds. Only that video tail was removed; its audio is retained. Transition audio continues over the next idle scene, then the idle sound starts at the corresponding elapsed position. A new sleep/wake request interrupts the previous transition sound. Muting does not intentionally pause the media timeline.

Video: 800x480 RGB565 decoded from JPEG, 10 fps, quality 28. The complete 16:9 picture occupies 800x450 with 15-pixel borders above and below. All original footage in each idle is retained; no warping or new synthetic animation is added. Source loop joins may still be visible.

Audio: individual stereo 16-bit PCM tracks, 16 kHz, 35% gain. Idle tracks are padded to match the 20.1-second converted video loop. All four video/audio pairs total about 23.7 MB, loaded into PSRAM if available. The v2 files are preserved for fallback. New asset filenames prevent overwriting the old set.

The player chooses video frames by elapsed time to preserve transition timing when rendering drops frames. The USB uploader supports 1,024-byte payloads only with the September 23 firmware; older firmware requires the default 256-byte payloads.
