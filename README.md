# Ronin: A BC250 desktop

> [!NOTE]
> **Work in progress: the design and manual are changing rapidly.** You're welcome to build and remix Ronin. Check the revision notes and open decisions as you go. Rev0.3 is a provisional first edition, with fabrication and wiring details still to validate. If you have questions, [ask Arthur](https://artfedderson.tech/#start).

![Ronin project logo](case-design/branding/ronin-brand-banner.jpg)

A custom desktop enclosure built around the BC250 board: a sheet-metal backbone, magnetically attached covers printed in transparent PCTG with a frosted finish, and an animated ESP32-P4 resource display at the base.

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
| [display/](display/README.md) | Published display baseline and current prototype status |
| [Rev A harness guide](case-design/manual/Ronin_LED_Harness_Rev_A.md) | Sourced connectors, numbered electrical maps, crimping and unverified interfaces |
| [October 6 manual updates](case-design/manual/REVISION_NOTES_2026-10-06.md) | Ordered adapter, new NVMe working-CAD route and next-revision checks |

Keep the contents of `case-design/` together so relative references stay intact. It includes the saved SolidWorks assembly and 45 part files, neutral CAD files, and all delivered render folders. It is a saved snapshot, not a clean-machine-validated Pack and Go export; see the [case guide](case-design/README.md) before fabrication.

## October 3 lighting revision

The latest rendered assembly replaces the rings and foot strip with two **Rainbow on a Matchstick boards by Blamm**, ten RGB LEDs per board. The revised CAD locations and printed diffusers are reflected in the new film. Untinted transparent PCTG is shown frosted, with 0.20 mm layer texture; the LEDs supply the color. See the [website update](https://artfedderson.tech/blog/bc250-matchstick-lighting-update) for component credits and current integration status. The original SolidWorks snapshot below is preserved and has not been replaced by this media release.

## October 6 hardware and display update

The **Ronin Rev A SATA-powered ESP32 LED adapter is ordered** and modeled in the current working CAD. It supplies two independent ten-pixel Matchstick channels while the display keeps USB power/control. One selector chooses 5 V or 12 V for both ports; **Matchstick V1.0 requires 5 V**. Delivery, bench checks, LED firmware and the real harness remain pending. Start with the [harness preparation guide](case-design/manual/Ronin_LED_Harness_Rev_A.md).

![Ronin Rev A LED adapter design visualization](case-design/media/ronin-led-adapter-top.jpg)

Design visualization from ordered fabrication layers and current CAD, not a physical-board photograph. Some component packages are simplified; the single 5 V selector shunt is illustrative. Native PCB/manufacturing files are not part of this public update.

![Ronin Rev A adapter underside with exposed copper logo and board name](case-design/media/ronin-led-adapter-bottom.jpg)

Ordered copper/mask artwork visualized in Blender with a gold ENIG finish. This is a design visualization, not an assembled-board photograph.

The working assembly now models an NVMe extension board, two printed extension mounts and an explicit ribbon route to the relocated SSD. Fit, retention and PCIe operation need physical testing. The repository's earlier CAD snapshot is unchanged; [October 6 revision notes](case-design/manual/REVISION_NOTES_2026-10-06.md) track the newer working CAD separately.

![Current CAD locator of the NVMe extension, modeled ribbon and twin mounts](case-design/media/ronin-nvme-route.jpg)

Pink identifies the ribbon route, lavender the two printed mounts, and teal the SSD/extension. The backbone and BC250 are ghosted for visibility. This is a current-CAD locator, not a cable cut-length, physical fit or storage-link validation.

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

## Display and companion progress

The Windows prototype now has a native menu, profiles and six touch application tiles. Existing SolidWorks, Steam, Claude, PrusaSlicer, OBS and Codex windows come forward without duplicates; OBS also passed fresh-launch focus. USB resource telemetry and scene/audio controls work. [Current prototype status](display/CURRENT_STATUS.md) separates this local tested work from the earlier software package published here.

Linux, initially Bazzite, remains the design baseline. The shared app/protocol and portable profiles keep the workflow consistent; installed programs, sensors and desktop activation need host-specific bindings. Physical Linux/Wayland/Gaming Mode tests, Matchstick output, gamepad host wake and AI voice remain pending. Display Sleep/Wake affects the display, not PC power. Claude/Codex tiles open host apps; they are not the voice companion.

## Work on the display

The published baseline ESP32 firmware, resource companion, media, build scripts and original documentation live in **[display/](display/README.md)**. Start with that README and run its commands from `display/`. Its internal `project/` and `external-originals/` layout and pinned dependency release are preserved.

Third-party component models, software, and supplied media retain their existing rights. This repository assigns no blanket license to them.
