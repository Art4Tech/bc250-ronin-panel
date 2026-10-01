# USB Resource Panel

Elecrow CrowPanel Advanced 5-inch ESP32-P4 V1.0. Connect the computer to the panel's **UART0 USB-C** port with a data cable. No Wi-Fi is used.

## Current features

- Total CPU utilization, RAM utilization and used/total GiB.
- Used percentage of the host's system filesystem (not all attached drives).
- CPU temperature when a supported Linux sensor is available. The displayed Linux value is the hottest reading in the selected CPU sensor group.
- Host name and operating system.
- Readings clear after five seconds without valid updates.
- The on-panel microphone/speaker recording test remains available.

This firmware does **not** yet provide USB microphone/speaker endpoints, Bluetooth gamepad reception, GPU utilization, fan monitoring, or power-button control. The audio button runs a local recording/playback test only. Windows CPU temperature needs an additional hardware-monitor integration; this version shows Unavailable. It does not use the ESP32's own temperature as the PC temperature.

## Windows setup

Install Python 3 if necessary, then open a terminal in this folder:

```powershell
py -m venv .venv
.\.venv\Scripts\python.exe -m pip install -r requirements.txt
.\.venv\Scripts\python.exe resource_panel.py --list-ports
.\.venv\Scripts\python.exe resource_panel.py --port COM10
```

COM10 was the port on the development PC. Confirm the port on each computer. Keep the terminal open; Ctrl+C stops monitoring. Close serial monitors before running this program. No administrator privileges or network access are needed during normal monitoring.

## Debian, Ubuntu, Raspberry Pi OS, or Omarchy

Install Python 3 with venv/pip support through the distribution's package manager. For Debian/Ubuntu this is normally `sudo apt install python3-venv python3-pip`; Omarchy/Arch uses `sudo pacman -S python python-pip`.

```sh
python3 -m venv .venv
.venv/bin/python -m pip install -r requirements.txt
.venv/bin/python resource_panel.py --list-ports
.venv/bin/python resource_panel.py --port /dev/serial/by-id/YOUR-PANEL
```

Use the actual stable /dev/serial/by-id path when available; otherwise use the listed /dev/ttyUSB device. If access is denied, check its owning group with `ls -l /dev/ttyUSB*`. Add your user to the owning serial-access group (commonly dialout on Debian/Ubuntu or uucp on Arch), then log out and back in. Do not make the device world-writable.

The same script supports Windows and Linux; only Windows has been tested on physical hardware so far. Linux sensor availability depends on the board, kernel and exposed hwmon/thermal drivers.

## Checks

1. Start the companion; readings and host name should appear within two seconds.
2. Compare RAM usage with the host's monitor. Sampling and accounting may differ from Windows Task Manager.
3. Stop the companion. Within five seconds the panel should show Disconnected and clear its readings.
4. Restart it; readings should return. Unplugging/replugging is retried automatically when the port path remains the same.
5. Move the panel to another supported computer and run its local companion. No firmware change is required.

Use `--once` to inspect one sample without USB. Use `--disk D:\\` on Windows or `--disk /mnt/data` on Linux to monitor another filesystem. Use `--seconds 30` for a timed test.

The companion sends resource numbers and the host name over the USB cable. It does not open a network listener or send commands to the computer. It is not installed as an automatic startup service.

## Recovery

The adjacent Panel-Recovery folder contains the original 16 MB factory flash backup and its checksum. App-only images are not substitutes for that full backup. Preserve recovery files before modifying firmware.

## Convenient Windows start/stop

The panel's SETTINGS button now shows a verified QR code and readable link to the official Libre Hardware Monitor downloads, plus USB/CPU-temperature status. See [Windows-Setup.md](Windows-Setup.md). The companion optionally reads CPU temperatures from Libre Hardware Monitor's local WMI feed; keep windows_temperature.py beside resource_panel.py. No network listener is required. The QR links to the sensor helper, not an installer for this companion.

Run Windows-Monitor.ps1 -Action Start -Port COM10 to start monitoring in the background, and Windows-Monitor.ps1 -Action Stop to stop it. It uses this folder's .venv, or the existing development environment on this PC. It does not register a startup task. The monitor retries if USB is unplugged. Logs and a process record are kept beside the script.

## Sakura mountain refuge

The dashboard embeds matching day/night paintings, 36 drifting petals, a gently swaying foreground sakura branch, subtle water ripples, slow cloud drift and daytime mist. The ronin's poses crossfade over 2.2 seconds; this is not a skeletal or frame-by-frame character animation. Original artwork and built-in image-generation prompts are in `art/`.

Sleep Display hides the resource overlay, shows the sleeping ronin and dims the backlight from 60% to 12%. The panel stays powered and touch-sensitive. Tap anywhere to return to daylight. This button does not shut down the host computer.

The companion marks its first packet after opening USB, or after a sending gap longer than five seconds, as a wake event. The display also wakes when valid telemetry resumes after a five-second gap. Ordinary updates leave a manually sleeping display asleep. Restarting the companion therefore wakes the display too; this is a host-connection indication, not a verified BC250 power-state signal. Powering off the panel's USB supply prevents it from showing the night scene or receiving Bluetooth.

Test with the companion running: press Sleep Display, wait at least ten seconds, then tap to wake. Press Sleep again, stop the companion for more than five seconds and restart it: daylight should return automatically. Repeat during a transition to check responsiveness. Confirm the resource text stays readable and petals move smoothly. The local audio test remains available.

The experimental video firmware plays day/night MJPEG clips from microSD and includes GameSir BLE testing. The GameSir has paired in G-Touch mode; waking the display on a new controller connection is implemented but still needs a physical test. Home-button reports and BC250 power-relay operation are not implemented. No Wi-Fi connection is needed. See TEST-STATUS.md for current limitations; dashboard.bin remains the earlier artwork-only fallback.
