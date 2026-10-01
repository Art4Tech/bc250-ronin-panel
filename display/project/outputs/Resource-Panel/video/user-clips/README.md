# User video preparation

Original day.mp4 and night.mp4 are preserved here as day-original.mp4 and night-original.mp4. Their source files in Downloads are unchanged.

Playback plan (12 fps, 800x480):

| State | Source | Source time |
|---|---|---|
| Normal loop | day-original.mp4 | 0–0.75 seconds |
| Sleep transition, once | day-original.mp4 | 0.75–5.125 seconds |
| Sleeping loop | night-original.mp4 | 0–1.5 seconds |
| Wake transition, once | night-original.mp4 | 1.5–5.125 seconds |

These source videos are approximately 5.17 seconds long and begin changing pose/lighting early. The normal loop is necessarily short. The loop boundaries and joins between independently generated videos may remain visible; no claim of seamless character animation is made. No image warping or additional sprites are applied.

The complete 864x480 composition is scaled proportionally to 800x444 and centered with 18-pixel dark borders above and below. This preserves the ronin and tree at the left edge.

Only day-original.mp4 supplies sound. Its first five seconds become a 4.7-second stereo nature loop with a 0.3-second overlap crossfade and 35% gain, converted to 16 kHz/16-bit PCM for the existing speaker configuration. It plays continuously across all four states; Mute / Sound toggles playback. The microphone test button is replaced while this movie mode is active.

All four compressed clips are validated and cached in PSRAM before playback. Existing day.rvj/night.rvj remain available as fallback if the new set is absent or cannot load. No card formatting is used.

CPU/RAM/disk label formatting uses libc snprintf because this LVGL build has float formatting disabled. CPU temperature N/A is a separate limitation of the current Windows sensor collector; values are not fabricated.
