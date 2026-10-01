# Windows monitoring setup

Tap SETTINGS on the display. The QR code and printed link lead to the official Libre Hardware Monitor release page:

https://github.com/LibreHardwareMonitor/LibreHardwareMonitor/releases/latest

The code opens a website on the scanning device. If you scan with your phone, send that link to the Windows PC. It does not install anything automatically.

1. Download the appropriate Windows release from that official page and follow its installation instructions, including required sensor components.
2. Run Libre Hardware Monitor as administrator and leave it running. Confirm it shows a CPU Package or AMD CPU temperature.
3. In Options → Remote Web Server, configure 127.0.0.1 (localhost), port 8085, and enable Run. The installed .NET 10 build did not expose the expected WMI namespace on this PC. Keep this folder's USB companion running; it reads the local web sensor feed, with WMI as a fallback for builds that expose it. The display still uses USB, not Wi-Fi.
4. Within about 20 seconds, the panel should show a CPU temperature if a supported sensor is exported. SETTINGS shows whether a CPU temperature is being received. N/A remains appropriate when no valid sensor is available.

The reader prefers CPU Package or Tctl/Tdie, otherwise the highest CPU temperature. It does not substitute SSD/GPU temperatures. Invalid and stale readings are discarded. Real sensor operation still needs verification with Libre Hardware Monitor running on the target PC.

The QR downloads the sensor helper only. Our companion is currently a local package, not a publicly hosted installer. For a new PC, copy this Resource-Panel folder and follow README.md to install Python dependencies and start the companion. A future complete setup QR needs a public download URL for that package.

Neither helper autostart nor administrator approval is silently configured.
