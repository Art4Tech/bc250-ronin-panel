BC250 — V5 / INDEPENDENT LED OUTPUT

The 48 ring LEDs have 20 times their V3 output; the 12-pixel foot strip has 8 times its V3 output. This is 10 times and 4 times, respectively, the V4 preview output. LED direction, geometry, studio exposure, colors and animation timing are retained.



Lighting revision
The two imported 24-pixel rings now face the sheet-metal backbone in the Blender proposal. Their light bounces into real gaps and perforations; the PCB and backbone remain opaque. Only the forward lens faces emit. Package sides and backs do not emit, and the previous omnidirectional RGB light helpers have been removed.

An 80 mm strip with 12 individual RGB LED packages sits at the foot of the board and points upward along the component side. It uses lower output than the rings and follows their color treatments, animated rainbow, wake-up and dimming. There are 60 individually animated RGB emitters in total. Neutral white is included alongside cyan, magenta, mint and rainbow.

The ring orientation and foot strip are proposed physical layout changes represented in Blender. Original SolidWorks files were not changed. The strip is a rendering concept, not a completed CAD mounting design. Intensity is composed for photography, not a calibrated prediction of physical LED output.

Deliverables
BC250_60s_Directional_RGB.mp4 — 60 seconds, 1920 × 1080, 24 fps, H.264.
BC250_Showcase_Directional.blend — complete editable assembly, camera, lighting, animation and feature captions.
BC250_Directional_Studio.blend — separate still-photography studio.
stills — eight full-resolution 16-bit PNG masters.
share-ready — matching full-resolution JPEGs, quality 95.
Preview.html — local photo gallery and movie player.
Photo_Contact_Sheet.jpg — photo overview.

Photo treatments
01 Cyan / dark hero
02 Neutral white / rear three-quarter
03 Magenta / dark hero
04 Mint / bright studio
05 Addressable RGB / rear three-quarter
06 Addressable RGB / display and power-button detail
07 Addressable RGB / PCTG surface detail
08 Neutral white / dark hero

Film timing
00–03 Opening hero
03–10 Front view, 5.3-inch touch display, integrated ESP32, LED control and resource monitoring
10–16 Right side and four USB ports; neutral-white LEDs
16–22 Rear view and Wi-Fi 7 / BLE 5.3 via NVMe splitter; mint LEDs
22–28 Elevated flyover and NVMe in cooling airflow; magenta LEDs
28–34 Left side and Dual Arctic P9 PWM PST fans; neutral-white LEDs
34–41 Magnetic cover removal and reassembly; rainbow LEDs
41–46 Bright studio and gamepad startup
46–49 Studio fades to dark
49–51 Rings and foot strip wake in sequence
51–55 Display wakes while the rainbow pattern moves through individual LEDs
55–58 Display sleeps; RGB emitters dim to 8 percent
58–60 Dimmed hero and fade to black for looping

Blender scenes
01 | Dark studio
02 | Bright studio
03 | 60 second flyover — camera, RGB, studio lighting, screen and cover movement
04 | Final film + feature captions — editable captions and fades over Scene 03

Source and materials
All 79 visible SolidWorks component instances are represented. Both 24-pixel rings and ten additional magnets were imported from the refreshed assembly, together with changed backbone and front/top/rear-cover geometry. Raised LED packages and lens surfaces were added at the ring CAD footprints. The original GUI and fonts are packed. GUI animation represents backlight/status behavior rather than a recording of the software.

Translucent PCTG retains procedural 0.20 mm layers with upright panel print orientation. These surface layers approximate FDM appearance; internal infill and slicer toolpaths are not simulated. Cover movement is an illustrative exploded view returning to the imported positions. As in the previous deliverable, four small curved fan faces failed SolidWorks tessellation, about 330 square millimeters per fan; the other visible fan geometry is present.

Rendering
Cycles with ray-traced shadows and indirect illumination. Stills use 96 samples; the film uses 16 during motion and 48 on stationary holds, with denoising. Identical static film states reuse an identical rendered frame. The 60-pixel RGB animation, studio transitions, screen wake/sleep and original feature captions remain baked into the Blender files. No external scripts are required for playback or editing.

COMMERCIAL FILMS

BC250_Cinematic_Commercial.mp4
The first requested commercial: the 60-second all-sides camera tour with five close-up cutaways, animated feature captions, the magnetic-cover demonstration, and the studio / RGB / display wake-and-sleep finale. The close-ups are 2304 x 1296 Cycles renders with depth of field and subtle editorial push-ins. The cooling close-up temporarily removes the front cover for visibility; its caption states that the cover is removed.
BC250_Cinematic_Commercial.blend is the editable source. It contains the original full 3D tour, macro cameras, image cutaways, typography and audio.

BC250_Made_To_Be_Seen.mp4
The second requested commercial, with a separate creative treatment: 'Made to Be Seen'. A new grazing-light macro and low hero camera move open the film; a tighter editorial sequence moves through touch control, connectivity, airflow, magnetic covers and three lighting personalities. The studio-to-night sequence returns before the final BC250 signature. It has a new original score and effects mix.
BC250_Made_To_Be_Seen.blend is the editable master. It includes the original tour scene and the new eight-second light-study scene, using live 3D Scene strips rather than requiring the cached render frames.
BC250_Director_Light_Study.blend is the standalone new cinematography setup. The case and component mesh coordinates and topology were hashed before and after its preparation; all matched. No product meshes were added or edited. Cameras, studio lamps, LED output animation, editing, typography and sound were changed. Product surface materials and the physical LED occlusion are retained.

Both commercials are 60 seconds, 1920 x 1080, 24 fps, H.264 with 48 kHz stereo AAC audio at 256 kbps. BC250_60s_Directional_RGB.mp4 is the additional silent all-sides showcase.

The music and sound design were synthesised specifically for these films, with no third-party recordings or sampled songs. The two 60-second 48 kHz / 24-bit stereo WAV masters are included in cinematic_assets. Audio peaks and durations were checked numerically; the film containers were checked for both video and AAC audio tracks.

The new moving macro uses 48 Cycles samples, and the new moving hero uses 32, with denoising. The existing tour uses the sampling described above. The new eight-second shot sequence is reused for the closing hero; the rest of the second film uses a fresh edit of the accurate V5 tour and photographs.

PORTABLE PROJECTS
Keep the cinematic_assets and stills directories alongside the Blender files. GUI images, fonts and audio are packed where Blender supports it; VSE image strips use relative paths to the included images. No rendering scripts or raw frame caches are needed to reopen and edit the projects. The live tour scenes include compositor-based AgX finishing followed by a Standard inverse conversion, with the scene view set to Standard, so the editable scene strips retain the same photographic look as the final films. Comparison frames were rendered directly from both commercial projects and checked against the decoded exports. The videos are ready to share directly.

Preview.html and Photos.html play all three videos and display the eight photographs. The verification reports record video frame counts, dimensions, durations, audio tracks, and unchanged geometry. Original SolidWorks files remain untouched.

CALM AUDIO REVISION
Both commercials now have calm replacement mixes. Every noise-based transition sweep and scratchy mechanical click has been removed. The second film's noise-based hi-hat is also removed. The music retains its previous level; the remaining soft LED/display tones are approximately 14 dB quieter. No gain normalization was applied to compensate for the removed effects.

BC250_Cinematic_Commercial_Calm_Audio.mp4
BC250_Made_To_Be_Seen_Calm_Audio.mp4

The original video streams were copied without re-encoding and their hashes match exactly. The canonical commercial MP4 filenames also contain the revised mixes. Both editable commercial Blender files and their packed WAV masters have been updated. The silent all-sides showcase and photographs are unchanged.
