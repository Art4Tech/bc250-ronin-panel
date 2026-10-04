# BC250 Ronin — case design

> [!WARNING]
> **In-progress project: the design and manual are changing rapidly.** Before buying parts, printing, or building a BC250 Ronin now, [contact Arthur](https://artfedderson.tech/#start) to confirm the current revision and unresolved interfaces. The Rev0.3 manual is a provisional first edition; it is not a finalized fabrication or wiring release.

A custom desktop enclosure built around the BC250 board: a sheet-metal backbone, removable magnetic and translucent PCTG covers, and an animated ESP32-P4 resource display at the base.

[![BC250 Ronin case visualization](https://media.githubusercontent.com/media/Art4Tech/bc250-ronin-panel/main/case-design/Product_Renders_2026-10-03_V6/bc250-v6c-cyan.jpg)](https://artfedderson.tech/work/bc250-ronin)

**[Watch “The Computer They Didn't Make” — 89-second unofficial launch film](https://media.githubusercontent.com/media/Art4Tech/bc250-ronin-panel/main/case-design/Product_Renders_2026-10-03_V6/BC250_The_Computer_They_Didnt_Make_V6c.mp4)** · [Full project write-up](https://artfedderson.tech/work/bc250-ronin)

The film and images are CAD-based Blender visualizations. No case parts have been manufactured yet. Rendered features and lighting treatments are design proposals, not proof of completed hardware or firmware integration.

## Start with the case

| Location | Contents |
| --- | --- |
| [Printing and assembly manual — Rev0.3 draft](case-design/manual/README.md) | Illustrated PDF, editable BOM, open decisions, and proposed parts |
| [case-design/](case-design/README.md) | Design guide, download instructions, and portability notes |
| [BC250-Assy.SLDASM](case-design/BC250-Assy.SLDASM) | Main SolidWorks assembly, with component parts alongside it |
| [Backbone flat pattern](case-design/Flat%20pattern%20-%20Backbone.DXF) | Sheet-metal DXF export |
| [Latest film and website imagery](case-design/Product_Renders_2026-10-03_V6/) | V6c 89-second film, two Matchstick boards and untinted frosted PCTG |
| [Earlier V5 delivery](case-design/Product_Renders_2026-09-30_V5/) | Previous films and editable Blender scenes |
| [display/](display/README.md) | Complete display software package and its original README |

Keep the contents of `case-design/` together so relative references stay intact. It includes the saved SolidWorks assembly and 45 part files, neutral CAD files, and all delivered render folders. It is a saved snapshot, not a clean-machine-validated Pack and Go export; see the [case guide](case-design/README.md) before fabrication.

## October 3 lighting revision

The latest rendered assembly replaces the rings and foot strip with two **Rainbow on a Matchstick boards by Blamm**, ten RGB LEDs per board. The revised CAD locations and printed diffusers are reflected in the new film. Untinted transparent PCTG is shown frosted, with 0.20 mm layer texture; the LEDs supply the color. See the [website update](https://artfedderson.tech/blog/bc250-matchstick-lighting-update) for component credits and current integration status. The original SolidWorks snapshot below is preserved and has not been replaced by this media release.

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

## The ESP32-P4 display roadmap

The ESP32-P4 panel already receives CPU, memory and disk readings over USB serial and plays its local animated scenes with audio. The next firmware work gives that small screen four more jobs; **these features are planned, not completed**:

- **Voice companion:** use the panel's onboard microphone and amplified outputs for external speakers as the listening and speaking interface for a companion. The complete voice workflow still needs implementation.
- **ARGB controller:** control the two ten-LED Rainbow on a Matchstick boards from the touch interface, with colors and effects for all twenty LEDs.
- **Gamepad wake of the host:** use the controller to wake or start the BC250. Display connection-wake experiments are separate; they do not yet operate the host's power signal.
- **Touch shortcut pages:** Stream Deck-like button pages that dispatch configured host scripts or commands according to the selected app/program, page or display screen. The context selection, action dispatch and host integration still need to be built.

The [manufacturer audio lesson](https://www.elecrow.com/wiki/5inch_P4_Arduino_11_Playback_After_Recording.html) documents recording and speaker playback hardware. The current Bluetooth work demonstrates scanning/pairing experiments, rather than reliable host wake or controller handoff. Existing local touch controls and sound playback do not yet constitute the voice companion or shortcut system.

## Planned Windows/Linux companion app

The existing [Python resource companion](https://github.com/Art4Tech/bc250-ronin-panel/blob/main/display/project/outputs/Resource-Panel/resource_panel.py) sends host telemetry to the display over USB serial. It is the working resource-monitor helper, rather than the full app described below.

The planned Windows and Linux app will bring the configuration together:

- Advanced LED animations for the two Matchstick boards.
- Installation and activation of resource-monitor modules.
- Touch-button profiles and configured script, command and program integrations, with actions selected by app/program, page or display screen.
- Configuration of the AI-agent portal connection used by the companion workflow.

This full configuration app and its display/host integrations are future work. Existing telemetry and local media playback are not being presented as implementation of those new functions.

## Work on the display

The complete ESP32 firmware, Windows/Linux companion, supplied media, build scripts, and original documentation now live in **[display/](display/README.md)**. Start with that README and run its commands from `display/`. Its internal `project/` and `external-originals/` layout and pinned dependency release are preserved.

Third-party component models, software, and supplied media retain their existing rights. This repository assigns no blanket license to them.
