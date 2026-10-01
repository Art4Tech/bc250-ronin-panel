# Public repository contents and private migration boundary

The panel package now lives in `display/`. Paths in the case-design section below are relative to the repository root; the [case-design guide](../case-design/README.md) is alongside this package.

## Case-design addition (2026-10-01)

`case-design/` adds the saved SolidWorks assembly and component files, neutral CAD files, display reference image, and all four delivered render folders (original, V2, V3 and V5). CAD and media binaries use Git LFS. `case-design/FILES-SHA256.json` records the imported files and their verified SHA-256 hashes. SolidWorks lock files and local `.codex` state are excluded. The original design folder is preserved. Third-party component models retain their existing rights; this addition assigns no blanket license to them.

This is a snapshot of saved files, not unsaved SolidWorks edits or a validated SolidWorks Pack and Go export. External assembly references have not been resolved by reopening the model on a clean machine. See the case-design README for portability limits.

## Panel software and migration package

The public repository contains the full application source, companion, managed component source, project configuration, firmware application versions, user-supplied media originals, generated artwork, conversion tools, tests, hardware runbook and handover. Large SDK/toolchain/runtime dependencies are attached to the pinned GitHub release rather than committed as Git objects.

The separate local migration package preserves the complete old setup, including the following **not published** items:

* Raw factory/full-flash recovery backups, which can contain device state outside the application.
* Serial/runtime/build logs, PID records and personal-machine telemetry.
* Original source-provenance manifest with personal absolute filesystem paths.
* Old build cache/objects, duplicate pip/uv/download caches, local editor state and repository reflogs.
* LibreHardwareMonitor's machine-specific settings. Obtain the app from its official release link in Settings or the handover; the local package also contains the original ZIP.

Account credentials, GitHub sign-in state, SSH keys and unrelated user files are never part of either project publication. Source copies generalize the old Windows username/path and controller address in documentation. Bundled Python environment configuration is deliberately reset and repaired by `Setup-Windows.ps1` after download.

Windows/.NET/PawnIO/USB-driver installations are not portable application folders. The new machine may need those system dependencies. Edge is optional for historical browser previews; it is not required for current video conversion or firmware builds.

The existing physical display retains its card and NVS/bonding state when moved. This repository is not a clone of every flash partition or every file on its microSD card.
