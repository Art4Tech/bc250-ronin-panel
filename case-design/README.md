# BC250 case design

[Project overview](../README.md) · [Display software and its original README](../display/README.md)

> [!NOTE]
> **Work in progress: the design and manual are changing rapidly.** You're welcome to build and remix the case. The [Rev0.3 printing and assembly manual](manual/README.md) records the current revision and open decisions. Mounting, wiring, fit and assembly details still need validation. If you have questions, [ask Arthur](https://artfedderson.tech/#start).

Saved design and rendering snapshot imported on 2026-10-01. The original folder layout is retained so nearby parts, media and relative Blender references stay together.

## Printing and assembly manual

Start with the [Rev0.3 manual and editable companion files](manual/README.md). The PDF records current CAD locators, printing guidance, component evidence, and unresolved build decisions. The CSV and JSON files make the BOM, open items, and proposed parts easy to review and update. The saved October 1 CAD snapshot in this folder and the manual's later source revision must be checked together before fabrication.

## Latest media revision

[Product_Renders_2026-10-03_V6/](Product_Renders_2026-10-03_V6/) contains the new 89-second V6c film and compressed website images. It shows the updated two-board, twenty-LED Matchstick arrangement and frosted untinted PCTG. This is a media release; the October 1 SolidWorks snapshot is retained unchanged. The earlier V5 editable scenes remain available at their original paths.

## Open the design

- `BC250-Assy.SLDASM`: main SolidWorks assembly. The supplied `.SLDPRT` files are alongside it.
- `Flat pattern - Backbone.DXF`: backbone flat-pattern export.
- `.STEP` / `.stp` files: supplied neutral CAD component files.
- `bazziteBC250gui.png`: display reference artwork.
- `Product_Renders_2026-09-30_V5/`: earlier delivered photography and films, including the calm-audio commercial revisions and their editable Blender masters. Start with its `README.txt` and `Preview.html`.
- The original, V2 and V3 render folders preserve earlier deliverables.

The V5 handover states that GUI images, fonts and audio are packed where supported, and image strips use relative paths. Keep `cinematic_assets`, `stills`, and the other companion directories beside the Blender files. No raw render caches or rendering scripts are required to reopen these delivered scenes according to that handover.

The V5 lighting layout is a Blender proposal. Its reversed LED rings and foot strip are not completed mounting changes in the SolidWorks model. Rendered captions and effects do not establish that the panel firmware implements the depicted feature; see the repository's software status.

## Display roadmap

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

## Download and verify

CAD and media binaries are stored with Git LFS. With Git LFS installed:

```powershell
git lfs install
git clone https://github.com/Art4Tech/bc250-ronin-panel.git
cd bc250-ronin-panel
git lfs pull
git lfs fsck
```

`FILES-SHA256.json` lists every imported file, its size and SHA-256. From the repository root, verify the materialized files:

```powershell
$items = Get-Content -LiteralPath .\case-design\FILES-SHA256.json -Raw | ConvertFrom-Json
foreach ($item in $items) {
    $path = Join-Path .\case-design $item.path
    if ((Get-Item -LiteralPath $path).Length -ne $item.bytes -or
        (Get-FileHash -LiteralPath $path -Algorithm SHA256).Hash -ne $item.sha256) {
        throw "Verification failed: $($item.path)"
    }
}
"Verified $($items.Count) imported files."
```

Third-party component models retain their existing rights. No blanket license is assigned by this import.
