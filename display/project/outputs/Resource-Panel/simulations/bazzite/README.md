# Bazzite preview

UI mockup only, not flashed firmware. Open bazzite-interactive-preview.html; the LED button opens a working local interaction preview with power, colors, brightness and effect selection. No hardware connection is made.

The PNG active area is exactly 800x480. The black bezel is horizontally centered and raised vertically: 3.5mm top / 7.5mm bottom with a modeled 108x65.3mm active area. Overall size is 120.7x76.3mm, as requested. Native PNGs remain 800x480; the bezel illustration applies slight vertical scaling. Print the HTML at Actual size / 100% for physical dimensions; on-screen size depends on monitor scaling.

Font: DroidSansM Nerd Font Mono, from https://github.com/ryanoasis/nerd-fonts/tree/master/patched-fonts/DroidSansMono . Font license is included under fonts/. The HTML embeds its font and video frame for offline viewing.

Readings are simulated light-desktop values, not Bazzite benchmarks. The planned count is 48 RGB pixels (two 24-pixel rings). Exact NeoPixel product, connector and a confirmed free ESP32-P4 GPIO are required before implementing its output.
