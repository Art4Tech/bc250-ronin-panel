BC250 — V2 PRODUCT SHOWCASE / CORRECTED LED OCCLUSION

Deliverables
• BC250_60s_Showcase.mp4 — 60 seconds, 1920 × 1080, 24 fps, H.264.
• stills — seven 16-bit PNG masters, 2400 × 2400 or 2600 × 1800.
• share-ready — matching full-resolution JPEGs, quality 95.
• BC250_Showcase_V2.blend — complete editable assembly, animation and captions.
• BC250_Showcase_Cycles.blend — original Cycles animation scene for alternate offline rendering.
• BC250_LED_Studio.blend — separate still-photography studio with the updated assembly and lighting.

Photo treatments
01 Cyan / dark hero
02 Amber / rear three-quarter
03 Magenta / dark hero
04 Mint / bright studio
05 Addressable RGB / rear three-quarter
06 Addressable RGB / display and power-button detail
07 Addressable RGB / PCTG surface detail

Film timing
00–03 Opening hero
03–10 Front view and 5.3-inch touch display / ESP32 / LED control / resource monitoring
10–16 Right side and four USB ports
16–22 Rear view and Wi-Fi 7 / BLE 5.3 via NVMe splitter
22–28 Elevated flyover and NVMe in the cooling airflow
28–34 Left side and Dual Arctic P9 PWM PST fans
34–41 Magnetic cover removal and reassembly
41–46 Bright studio and gamepad startup caption
46–49 Studio fades to dark
49–51 The two RGB arrays wake
51–55 Display wakes and the rainbow pattern moves through individual pixels
55–58 Display goes to sleep; LEDs dim to 8 percent
58–60 Dimmed hero and fade to black for looping

Blender scenes
01 | Dark studio
02 | Bright studio
03 | 60 second flyover — camera, lighting, RGB pixels, screen and covers
04 | Final film + feature captions — editable text and fades over Scene 03

Import and rendering notes
The current SolidWorks assembly was read without saving or changing the CAD files. All 79 visible component instances are represented. Both 24-pixel NeoPixel rings and ten additional magnets were imported. Changed geometry on the backbone and front, top and rear covers was also refreshed. Other existing mesh data was checked against the new export.

The ECAD ring models provide package footprints. Small raised packages and emitting diffusers were added at the 48 original CAD footprint centers, in the original component transforms. The older concept LED strip was removed. All 48 light helpers now cast shadows and sit just above the added diffuser surfaces, outside the PCB. The prior helper overlap with the PCB and the shadow bypass have been removed. The opaque PCB and aluminum backbone block light; visible illumination passes through actual gaps and perforations. LED intensity is composed for product photography and is not a calibrated prediction of physical lumen output.

The power-button ring and rear indicators remain illuminated. The original GUI image is packed in the Blender files. Its backlight, status indicator and sleep behavior are animated; this is not a live recording of the controller software.

Translucent PCTG keeps the 0.20 mm procedural surface layers. Wall panels use upright print orientation; the top cover is oriented as standing on its rear edge. These are surface-shader approximations rather than slicer toolpaths or simulated internal infill. Covers move for an illustrative exploded view and return to their imported positions.

As in V1, SolidWorks could not tessellate four small curved faces on the two fan components (about 330 square millimeters per fan). The remaining visible fan geometry is present. No new export warnings appeared.

Feature specifications and the fan/display wording follow the owner's supplied descriptions. Both stills and the film use Blender Cycles with ray-traced shadows and indirect light. Stills use 96 samples; animation uses 16 samples during motion and 48 on stationary holds, with denoising. All 48 emitting surfaces and matching shadow-casting helpers retain independent RGB animation. Both animation project files contain the corrected Cycles scene. Fonts and GUI artwork are packed. No external Python scripts are required to edit or render the saved scene.


