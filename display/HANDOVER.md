# BC250 / Elecrow resource panel — migration handover

**Repository layout:** the package root referenced below is now `display/`. Open PowerShell there for these commands. The historical validation and private migration-package notes are retained.

Prepared 2026-09-26 for a new **Windows x64 development PC**. All original files on the old PC are preserved. This folder is the copyable package; copy the whole folder, not just the companion. No device was flashed, erased, or reformatted during packaging.

## First use on the new PC

1. Copy this folder to a short local path such as `C:\BC250`. Allow roughly 15 GB of free space for the package plus a fresh build. Avoid OneDrive and very deep folder paths for compilation.
2. Open PowerShell in that folder. Before changing anything, verify the supplied files:
   ```powershell
   & .\project\work\python\cpython-3.12.13-windows-x86_64-none\python.exe .\Verify-Package.py
   ```
   Expect zero failures. The manifest covers the payload, scripts and documentation; it excludes itself and the verification report.
3. Repair the two Python environments for their new location:
   ```powershell
   powershell -NoProfile -ExecutionPolicy Bypass -File .\Setup-Windows.ps1
   ```
   This uses bundled runtimes and packages, without pip downloads or global Python installation. Run it again if the folder moves. It updates the copied environments only. Use `python.exe -m pip`, not their old `pip.exe` launchers, which retain old paths.
4. Plug the display into a USB data port. Keep its existing microSD in place. No reflashing is needed to continue using it. Identify its new serial port:
   ```powershell
   $py='.\project\work\idf-tools\python_env\idf5.5_py3.12_env\Scripts\python.exe'
   & $py .\project\outputs\Resource-Panel\resource_panel.py --list-ports
   ```
   COM10 belonged to the old PC; substitute the detected port below.
5. Start monitoring, then check the display:
   ```powershell
   & .\project\outputs\Resource-Panel\Windows-Monitor.ps1 -Action Start -Port COM7
   # Stop it before flashing or uploading media:
   & .\project\outputs\Resource-Panel\Windows-Monitor.ps1 -Action Stop -Port COM7
   ```
   Start may require the same per-process PowerShell execution-policy invocation as setup. No startup task or service is installed. The original `running.json` is a historical PID record; the controller checks the exact script path and process creation time before acting, so it will not treat an unrelated PID as its companion.

## What is included

* `project/work/`: the entire original development tree, including the full ESP-IDF checkout, vendor checkout, managed components, toolchains, Python environments, standalone Python 3.12, build output, scripts, tests and diagnostic logs. Redundant pip/uv caches are excluded; their installed dependencies are included.
* `project/outputs/`: the complete runbook, companion, firmware versions, factory backup, artwork, prompts, previews, media conversions and status documents.
* `external-originals/Downloads/`: all six user-supplied MP4s, copied directly from Downloads: day, night, dayidle, nightidle, sleepytime, wakeuptime.
* `external-originals/generated-images/`: all five images generated for this task, including earlier versions outside the project folder.
* `dependencies/Python314/`: the original Python 3.14.7 base used by the esptool environment; `dependencies/Git/`: Git for Windows 2.52.0 including its command-line runtime and licenses.
* `dependencies/LibreHardwareMonitor.NET.10.zip` and its extracted application folder, including existing settings. The optional system runtime/driver requirements are below.
* `SOURCE-PROVENANCE.json`: original absolute path, copied path and size for every source file; hashes or prior-copy verification records accompany them. This records the pre-migration source snapshot. `MANIFEST-SHA256.json` describes the final package, including migration scripts. Source files adapted for relocation may therefore intentionally differ between these two records.
* Exact Python package lists and ESP-IDF revision/submodule inventory are at the package root. Licenses supplied with dependencies are retained in their original directories.

## Authoritative source and build

**Edit `project/work/panel-test`, not the exported historical C files in outputs.**

* `main/main.c`: dashboard, settings/QR, resource parsing and scene state.
* `main/panel_movie.c`: current four-clip player, caching, clocks and audio-tail behavior.
* `main/panel_video.c`: SD mount, JPEG decode and serial upload protocol.
* `main/panel_bluetooth.c`: experimental GameSir pairing/reconnect/display wake.
* `peripheral/bsp_illuminate/bsp_illuminate.c`: RGB/LVGL display synchronization and byte order.
* `sdkconfig`, `partitions.csv`, `dependencies.lock`, `main/idf_component.yml`, all CMake files and `managed_components/`: reproducible configuration and resolved source dependencies.

ESP-IDF **5.5.4**, LVGL **9.1.0**, esp_hosted **2.12.12**, esp_wifi_remote **0.16.3**. Target ESP32-P4; 16 MB flash, 32 MB PSRAM; CPU 360 MHz, PSRAM 200 MHz. RISC-V compiler `esp-14.2.0_20260121`, Ninja 1.12.1. The vendor checkout revision is `76dac1b5754d17771f5a15acf0d4606e80164ea2`.

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File .\Build-Firmware.ps1
```

The migration build script uses bundled Git and creates `project/work/panel-test/build-migrated/`, avoiding the archived original build cache's absolute paths. Do not reuse `build/` on the new PC. The original `work/build-panel.ps1` is preserved as historical source and assumes system Git. If you move the package again after building, use a new build directory or remove only the disposable `build-migrated` directory after verifying its path. Do not delete the archived build.

Some historical preview, media and vendor-probing scripts contain the old username or Edge path. The portable media preparation script described below handles current media; archival experimental scripts are not the supported build entry point.

## Firmware and recovery

Installed/current release: `project/outputs/Resource-Panel/firmware/dashboard-movies-v3.bin`.
SHA256: `75AEC525CFAF081945918EC849013312C9163561432A1B804DCBC90FF78F9791`.

The existing board only needs the application at **0x10000**. Stop the companion and any serial terminal first:

```powershell
& .\project\work\panel-tools\Scripts\python.exe -m esptool --chip esp32p4 --port COM7 --baud 460800 write-flash --flash-mode dio --flash-size 16MB --flash-freq 80m 0x10000 .\project\outputs\Resource-Panel\firmware\dashboard-movies-v3.bin
```

Substitute `project/work/panel-test/build-migrated/5inch_Lesson09.bin` only after a successful new build. Restart the companion afterward. Do not erase all flash or flash the C6 radio as part of ordinary development.

Original P4 factory backup: `project/outputs/Panel-Recovery/elecrow-p4-v1-factory-backup.bin` (16 MB), SHA256 `5D5B6C9437E802AB09C00E48050198035139E6D17A66325358690804546D085F`. Its recovery documents are beside it. This is an original factory snapshot, **not a current NVS/bonding backup**, and does not contain C6/STC8 firmware or eFuses. The physical board retains those when moved. Replacing hardware requires additional consideration.

The archived build includes bootloader, partition table, initial OTA data, ELF/map and flasher arguments. Its documented layout is bootloader 0x2000, partition table 0x8000, OTA data 0xd000, app 0x10000. These extra images are for deliberate recovery, not routine app updates. Earlier binaries in outputs are preserved fallbacks/experiments, not the current release.

## Media and SD card

Current assets are in `project/outputs/Resource-Panel/video/movies-v3/`:

| State | Original | Board files |
|---|---|---|
| Awake idle | dayidle.mp4 | day3.rvj, day3.pcm |
| Sleep idle | nightidle.mp4 | nite3.rvj, nite3.pcm |
| Going to sleep | sleepytime.mp4 | sleep3.rvj, sleep3.pcm |
| Waking | wakeuptime.mp4 | wake3.rvj, wake3.pcm |

All eight belong in **`/BC250/` on the microSD**. They are already on the existing display's card. The package contains all project-generated card assets, but is not a byte-for-byte image of the card and does not copy unrelated card contents. No card adapter is needed just to move the display.

The two idle loops are 20.1 seconds; visible transitions are 5.2 seconds, with black video tails removed. Each file uses its **own audio**. The full approximately 7.8/9.13-second transition audio is retained and continues into idle before handing over to idle audio. The obsolete shared nature track belongs to v2 only.

RV01 is a 16-byte little-endian header (`<4sHHHHI`, magic, width, height, fps, reserved, frame count), then length-prefixed JPEG frames. Current frames are 800x480 at 10 fps, with 800x450 content letterboxed by 15 pixels. PCM is signed little-endian 16-bit, stereo, 16 kHz, 35% source gain. JPEG quality 28. All 506 frames were validated. Playback caches about 23.7 MB; loading takes about 47 seconds, while built-in artwork remains visible. Measured playback is about 10 fps.

`Prepare-Media.py` is the relocated version of the current conversion script. Run from the package root with the build Python. It reads the bundled external originals and writes a **new** `media-rebuilt/` directory, preserving all supplied originals and installed assets. It retains the original script separately in project/work. FFmpeg 7.1 is bundled through imageio-ffmpeg.

For serial card uploads use `project/outputs/Resource-Panel/upload_video.py --help`. Stop the companion first. Current firmware supports `--chunk-size 1024`, CRC32 per packet and SHA256 verification on finish. Uploads reject existing destinations and an existing `UPLOAD.TMP`; do not assume overwrite/resume. Existing `work/install-v3-assets.ps1` refers to COM10 and is historical. For a replacement card, a card reader and direct copy of the eight files is simplest. Keep the existing FAT filesystem; don't format a card with data you need.

Rejected procedural preview `preview-motion-v2-standalone.html` distorted the scene and was never installed. Preserve it as history, but use the user's current MP4s for further work.

## Windows temperature monitoring

The companion alone supplies CPU/RAM/disk usage. CPU temperature additionally needs LibreHardwareMonitor running with sensor access, generally elevated. Its app ZIP and extracted files are included, but the **.NET 10 Windows Desktop x64 runtime and PawnIO kernel-driver installation are system prerequisites**, not transferable by copying an app directory. The old PC has WindowsDesktop 10.0.11; the runtime inventory is included. Use the official runtime/helper installation prompts on the new PC if missing.

Enable LibreHardwareMonitor's Remote Web Server on port **8085**. The companion reads `http://127.0.0.1:8085/data.json`; its HTTP client bypasses proxies. The old application bound to `0.0.0.0`, which also listens on other interfaces. Keep access local with Windows Firewall if that is still the available binding. No LAN access is needed for this project.

The installed .NET 10 app's WMI namespace was unavailable; HTTP worked. The reader handles numeric and unit-bearing RawValue, Celsius/Fahrenheit and decimal commas, prefers CPU Package/Tctl/Tdie, excludes GPU/disk and distance-to-TjMax, and expires stale readings after 20 seconds. A background poll avoids blocking USB updates. User confirmed live temperature operation on the old PC.

The on-device Settings QR links to the official LibreHardwareMonitor releases page, not a hosted installer for the companion. Some on-device instructions still describe WMI and need updating to the working HTTP method. No automatic LHM installation or startup was implemented.

## Linux host use

The firmware and SD media do not change. A Linux companion still must be installed and run; it is not plug-and-play telemetry without host software. Copy `project/outputs/Resource-Panel` and create a Linux Python venv (the bundled Windows runtimes cannot run on Linux):

```sh
python3 -m venv .venv
.venv/bin/python -m pip install -r requirements.txt
.venv/bin/python resource_panel.py --list-ports
.venv/bin/python resource_panel.py --port /dev/ttyUSB0
```

Debian/Ubuntu may need `python3-venv` and serial membership in `dialout`; Arch/Omarchy commonly uses `uucp`. Check the device's actual owning group and log out/in after membership changes. Prefer `/dev/serial/by-id/...` for stable naming. Linux temperature collection uses psutil sensors plus thermal-zone fallback, subject to hardware/kernel support. No systemd service is supplied or installed. Physical validation on Linux is still pending.

## Hardware and unfinished work

Elecrow CrowPanel Advanced **5-inch ESP32-P4 V1.0**, 800x480 RGB panel. CH340 USB serial, normal 115200 baud; flashing/uploads 460800 when requested. UART GPIO38/37. Native USB audio/video is not implemented. The speaker/microphone board tests passed, but that does not make it a USB sound card for the host.

RGB clock 18 MHz; internal bounce buffers and correct frame-completion synchronization fixed earlier corruption/flashing. Native display byte swapping is disabled; JPEG video conversion applies its required byte swap separately. Avoid undoing one to fix the other. Resource float labels deliberately use libc snprintf rather than LVGL's formatter, which previously displayed literal `f`.

C6 hosted radio: SDMMC slot1 CLK53/CMD54/D0=52/D1=51/D2=50/D3=49, reset GPIO20; factory hosted firmware 2.3.0 uses a legacy initialization path. SD card: slot0 CLK43/CMD44/D0=39, one-bit 10 MHz. Do not casually change these shared peripheral settings.

GameSir Nova 2 Lite: G-Touch BLE mode Home+RB for 2 seconds (cyan); Home+Screenshot for rapid pairing. It appeared as `GameSir-Nova 2 Lite_G`. BLE scan, pairing and HID connection succeeded; **A/Home input reports did not**, and button-specific wake is unfinished. Firmware wakes the display on a new successful HID connection; reliable physical confirmation is still needed. Android Home+A/yellow mode was not successfully discovered. No BC250 power-switch relay output, standby-power integration or gamepad handoff is implemented.

The intended future chain is standby-powered panel + Bluetooth wake + isolated pulse across the BC250 starter's power-button contacts, with serial telemetry when the host is on. Consult `project/outputs/BC250-Omarchy-Runbook.html` for the hardware plan. Do not wire a GPIO directly to ATX power or infer a completed relay design from this software.

Commander Duo's two temperature probes and lightweight RGB control are **future work**. Linux OpenLinkHub was identified as a possible host-side backend, but no backend or display controls have been implemented or tested. Confirm the exact controller and USB connection, then integrate through the host companion. Keep pump/fan safety independent of display availability.

## Checks and next steps

The user reported the current dashboard working well, including usage and Windows temperature. Historical TEST-STATUS entries contain older pending items and superseded binaries; this handover identifies the current source. Detailed transition audio-tail behavior still merits a specific listening test.

After migration: verify files, run setup, start companion on the new port, confirm live CPU/RAM/disk readings, configure optional LHM temperature, then test sleep/wake and all four clips with sound. Allow a minute after power-up for card caching. Build before editing to establish a baseline. Continue next with controller input reports and power hardware, then Commander Duo integration.

## Excluded or nonportable dependencies

* Windows itself, system .NET runtimes, installed PawnIO/CH340 drivers, registry entries, device bindings, elevation state and firewall rules are **not cloned**. LHM app files are included; its drivers/runtime must be installed on the new PC if needed. Windows may install the CH340 driver automatically; otherwise obtain the manufacturer's driver.
* Edge/Chromium and its browser profile are not copied. Old procedural preview scripts expect system Edge; this is not needed to build firmware, run the companion or convert current MP4s.
* Global Git credentials/configuration, SSH keys, Wi-Fi credentials, unrelated Downloads and unrelated Codex sessions are not part of this project migration. Git program files and project repository metadata are included.
* Live board NVS/bonding, C6/STC8 firmware/eFuses, and unrelated SD files are not read back in this task. The existing hardware retains them. The original P4 factory backup and all known project media are included.
* Linux-native runtimes/packages and future OpenLinkHub/Commander Duo software are not bundled. Exact installed Windows Python package versions are recorded; their installed files are bundled, not a cross-platform wheelhouse.

No old-PC application, original media, source tree or tool installation has been deleted or disabled. Retire the old setup after the new PC passes the above device checks.
