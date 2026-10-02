# BC250 Ronin — case design

A custom desktop enclosure built around the BC250 board: a sheet-metal backbone, removable magnetic and translucent PCTG covers, and an animated ESP32-P4 resource display at the base.

[![BC250 Ronin case visualization](https://media.githubusercontent.com/media/Art4Tech/bc250-ronin-panel/main/case-design/Product_Renders_2026-09-30_V5/stills/01_Cyan_Dark_Hero.png)](https://artfedderson.tech/work/bc250-ronin)

**[Watch “Made to Be Seen” — 60-second case film with calm audio](https://media.githubusercontent.com/media/Art4Tech/bc250-ronin-panel/main/case-design/Product_Renders_2026-09-30_V5/BC250_Made_To_Be_Seen_Calm_Audio.mp4)** · [Full project write-up](https://artfedderson.tech/work/bc250-ronin)

The film and images are CAD-based Blender visualizations. No case parts have been manufactured yet. Rendered features and lighting treatments are design proposals, not proof of completed hardware or firmware integration.

## Start with the case

| Location | Contents |
| --- | --- |
| [case-design/](case-design/README.md) | Design guide, download instructions, and portability notes |
| [BC250-Assy.SLDASM](case-design/BC250-Assy.SLDASM) | Main SolidWorks assembly, with component parts alongside it |
| [Backbone flat pattern](case-design/Flat%20pattern%20-%20Backbone.DXF) | Sheet-metal DXF export |
| [Latest renders and films](case-design/Product_Renders_2026-09-30_V5/) | V5 stills, films, and editable Blender scenes |
| [display/](display/README.md) | Complete display software package and its original README |

Keep the contents of `case-design/` together so relative references stay intact. It includes the saved SolidWorks assembly and 45 part files, neutral CAD files, and all delivered render folders. It is a saved snapshot, not a clean-machine-validated Pack and Go export; see the [case guide](case-design/README.md) before fabrication.

## Current build status

- Machine internals are built and validated, with an initial cooling check complete.
- Software and the resource display are set up and working.
- Case fabrication is next. Fit and cooling need checking again inside the finished enclosure.
- Gamepad startup remains unfinished. The proposed LED layout still needs physical mounting and control integration.

Approximately **$375** has been spent so far, including the required filament. The remaining sheet-metal piece is estimated at **$275** for one unit, for an expected total around **$650**. A run of at least five targets a reduction of at least 60% in the sheet-metal cost (about $110 or less per piece); this is a target, not a confirmed quote for the whole machine.

## Get the complete design

CAD, Blender, and case media files use **Git LFS**. Install Git LFS, then:

```powershell
git lfs install
git clone https://github.com/Art4Tech/bc250-ronin-panel.git
cd bc250-ronin-panel
git lfs pull
git lfs fsck
```

Use a Git LFS checkout for the complete case assets. The [case-design README](case-design/README.md) includes SHA-256 verification instructions for the imported snapshot.

## Work on the display

The complete ESP32 firmware, Windows/Linux companion, supplied media, build scripts, and original documentation now live in **[display/](display/README.md)**. Start with that README and run its commands from `display/`. Its internal `project/` and `external-originals/` layout and pinned dependency release are preserved.

Third-party component models, software, and supplied media retain their existing rights. This repository assigns no blanket license to them.
