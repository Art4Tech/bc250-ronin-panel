# Planned two-ring connection

Concept only: no GPIO has been assigned or firmware output enabled.
The proposed rings sound like Adafruit 1586, 24 RGB pixels, nominal 66 mm diameter. Confirm the exact SKU before ordering connectors (RGBW requires different pixel configuration).

```text
Confirmed-free P4 GPIO -> 74AHCT125 (5 V) -> 330 ohm -> Ring 1 DIN
Ring 1 DOUT --------------------------------------> Ring 2 DIN
Regulated 5 V supply --------+---------------------> Ring 1 5V
                            +---------------------> Ring 2 5V
Common GND: supply, P4, level shifter, both rings
```

Data is daisy chained; power is parallel. Connect the level shifter enable appropriately (74AHCT125 /OE low), and add its supply decoupling. Put a 500–1000 uF capacitor rated at least 6.3 V across the LED supply near the rings, observing polarity.

Budget conservatively up to 2.88 A at 5 V for 48 RGB pixels at full white (60 mA per pixel); actual draw depends on revision and brightness. Supply the LEDs from a suitably rated 5 V PSU branch, not a GPIO or an unverified display-board power header. Do not join independent USB and PSU 5 V sources. Common ground is required. Standby power for the panel does not imply enough standby capacity for the LEDs; initially plan LEDs off while the PC is off.

These rings use solder pads. A matching locking pigtail is useful, but it is not automatically compatible with the Elecrow connector. Verify connector current rating, wire gauge, polarity and pin order. Separate power feeds to each ring are preferable to passing all LED current through an undersized connector. Add strain relief. Board V1.0 schematic and existing pin assignments must be reviewed before choosing the GPIO or board-side pigtail.

Sources:
- https://www.adafruit.com/product/1586
- https://learn.adafruit.com/adafruit-neopixel-uberguide/basic-connections
- https://learn.adafruit.com/adafruit-neopixel-uberguide/powering-neopixels

The HTML preview provides power, brightness, palette and effect selection. It does not drive LEDs. The current device firmware remains unchanged.
