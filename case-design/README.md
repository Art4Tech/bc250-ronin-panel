# Ronin case design

![Ronin project logo](branding/ronin-brand-banner.jpg)

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

## October 6 working-CAD changes

The current working assembly includes one visible **Ronin Rev A SATA LED adapter**, one NVMe extension board, two printed extension mounts, one modeled extension ribbon and a relocated M.2 2280 SSD. The adapter has been ordered; delivery and bench acceptance are pending. The explicit ribbon route and mounts are modeled proposals awaiting physical fit, retention and PCIe checks.

![Ordered LED adapter design visualization](media/ronin-led-adapter-top.jpg)

Visualization from ordered fabrication layers and current CAD, not a physical photograph. Some packages are simplified; the optional single 5 V shunt is illustrated. This documentation/media update does not replace the saved October 1 assembly or publish native PCB/manufacturing files. See [manual revision notes](manual/REVISION_NOTES_2026-10-06.md) and [harness preparation](manual/Ronin_LED_Harness_Rev_A.md).

![Ronin Rev A adapter underside with exposed copper logo and board name](media/ronin-led-adapter-bottom.jpg)

Ordered copper/mask artwork visualized in Blender with a gold ENIG finish. This is a design visualization, not an assembled-board photograph.

![Current CAD locator of the NVMe extension, modeled ribbon and twin mounts](media/ronin-nvme-route.jpg)

Pink identifies the ribbon route, lavender the two printed mounts, and teal the SSD/extension. The backbone and BC250 are ghosted for visibility. This is a current-CAD locator, not a cable cut-length, physical fit or storage-link validation.

## Display progress

The current local Windows prototype has verified USB telemetry, scene/audio controls, a native menu/profiles and six application tiles. Existing app focus and duplicate suppression passed; OBS also passed fresh-launch focus. The repository retains an earlier software baseline. [Current status](../display/CURRENT_STATUS.md) explains the distinction and the Linux-first shared workflow.

Physical Linux GUI/Wayland/Gaming Mode, Matchstick output, gamepad host wake and AI voice remain pending. The two independent LED channels use SATA power; the display stays USB-powered. Matchstick V1.0 requires the adapter's **5 V** selection. Final wiring, mounts and firmware must be validated before applying power.

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
