# Full-screen motion test

Open preview.html to preview the animated painting locally. These loops animate water, clouds, blossoms and the canopy from the existing artwork. The ronin remains in the standing or sleeping pose; this is not a frame-by-frame sit/stand character animation.

Each day.rvj/night.rvj contains 60 distinct baseline JPEG frames at 800×480, requesting 15 fps. The panel's P4 hardware JPEG decoder renders them from the card, retaining the live resource overlay and touch sleep/wake controls. Actual delivered FPS is logged as PANEL_VIDEO and must be measured on hardware with Bluetooth and SD activity running.

Files go in /BC250/day.rvj and /BC250/night.rvj on the card. The uploader refuses to overwrite existing clips or temporary files. It verifies SHA-256 on the panel and only renames the temporary file after a complete, matching transfer. No formatting is performed. Existing unrelated card files are preserved.

Stop Windows-Monitor.ps1 before running ../upload_video.py day.rvj night.rvj with the development Python environment. The uploader briefly changes serial speed to 460800 for transfer, then returns it to 115200. Restart Windows-Monitor.ps1 afterward. Do not remove the card or disconnect power during transfer. These RVJ files use our small frame container, not an AVI/MP4 container; arbitrary MP4 files do not play directly.

RV01 container: 16-byte little-endian header: magic RV01, uint16 width, uint16 height, uint16 fps, uint16 reserved, uint32 frame count. Each frame: uint32 JPEG byte length followed by JPEG data. Accepted dimensions are exactly 800×480, 1–30 fps, at most 9000 frames, and at most 256 KiB per compressed frame.

With no valid clip, the built-in scene remains available. Native USB live video streaming from the host is not implemented yet. This first test verifies local decode, display refresh, and SD throughput before adding a streaming transport.
