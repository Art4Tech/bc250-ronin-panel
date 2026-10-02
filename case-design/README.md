# BC250 case design

[Project overview](../README.md) · [Display software and its original README](../display/README.md)

Saved design and rendering snapshot imported on 2026-10-01. The original folder layout is retained so nearby parts, media and relative Blender references stay together.

## Open the design

- `BC250-Assy.SLDASM`: main SolidWorks assembly. The supplied `.SLDPRT` files are alongside it.
- `Flat pattern - Backbone.DXF`: backbone flat-pattern export.
- `.STEP` / `.stp` files: supplied neutral CAD component files.
- `bazziteBC250gui.png`: display reference artwork.
- `Product_Renders_2026-09-30_V5/`: latest delivered photography and films, including the calm-audio commercial revisions and their editable Blender masters. Start with its `README.txt` and `Preview.html`.
- The original, V2 and V3 render folders preserve earlier deliverables.

The V5 handover states that GUI images, fonts and audio are packed where supported, and image strips use relative paths. Keep `cinematic_assets`, `stills`, and the other companion directories beside the Blender files. No raw render caches or rendering scripts are required to reopen these delivered scenes according to that handover.

The V5 lighting layout is a Blender proposal. Its reversed LED rings and foot strip are not completed mounting changes in the SolidWorks model. Rendered captions and effects do not establish that the panel firmware implements the depicted feature; see the repository's software status.

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
