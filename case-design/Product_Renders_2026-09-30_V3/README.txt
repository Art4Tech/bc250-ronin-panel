BC250 — DIRECTIONAL RGB SHOWCASE / V3

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
