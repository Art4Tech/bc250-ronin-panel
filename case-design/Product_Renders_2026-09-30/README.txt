BC250 PRODUCT STUDIO — 30 September 2026

Open BC250_Product_Studio.blend in Blender 5.2 or newer.
The GUI image is packed into the project; no texture relinking is required.

DELIVERABLES (16-bit PNG masters; JPEG versions in share-ready)
01_BC250_Dark_Hero.png — dark front / left three-quarter, 2400 x 2400
02_BC250_Bright_Studio.png — bright front / right three-quarter, 2400 x 2400
03_BC250_Rear_Illumination.png — rear view with the two illuminated buttons, 2400 x 2400
04_BC250_Display_Detail.png — original backlit GUI and front detail, 2600 x 1800
05_BC250_PCTG_Layers.png — rear vents, printed texture and button lighting, 2600 x 1800
06_BC250_Animated_Studio_Loop.mp4 — four-second seamless 1080 x 1080 H.264 loop at 24 fps

BLENDER SCENES
01 | Dark studio
02 | Bright studio
03 | Animated studio loop

The CAD assembly, lighting additions and photographic studio are in separate
collections. Component and body names are retained. The model uses meters;
the interface is configured to display millimeters. Stills use Cycles,
adaptive sampling and denoising. The project supports AMD HIP GPU rendering.

PRINTED PCTG
Purple translucent appearances have been translated to rough transmission,
volume absorption and procedural ridges at 0.20 mm pitch. Wall panels use
assembled vertical as the print direction. The top cover stands on its rear
edge. Change each "Print direction" empty to adjust the build direction;
change the layer-pitch math node and bump distance in the PCTG materials
to adjust layer height and prominence. This is a visual surface model, not
a slicer toolpath simulation, volumetric infill or a measured filament match.

LIGHTING AND ANIMATION
A concept LED strip is placed beneath the BC250 PCB with soft internal fill.
The existing left power-ring diffuser and rear power/reset button geometry
have emissive finishes. The original GUI is textured at its SolidWorks sketch
picture coordinates. The animation adds a small live-status pulse and gentle
backlight variation, plus a slow camera move; it is not a live software feed.

IMPORT NOTE
67 visible components were imported from the open BC250-Assy assembly,
using 2,281,689 source triangles. Source CAD files were not saved or altered.
SolidWorks did not provide mesh triangles for four small curved surfaces on
the two 92 mm fans (approximately 330 square millimeters per fan). A second
CAD tessellation method also failed. The affected faces are omitted and
listed in the Blender project's READ ME text. Enclosure and display meshes
were unaffected. CAD material colors and assignments were translated to
Blender's physically based shaders; the shading systems are not identical.

