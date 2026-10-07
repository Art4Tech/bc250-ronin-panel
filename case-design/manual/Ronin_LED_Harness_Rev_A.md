# Ronin LED harness — Rev A preparation guide

**Updated:** 2026-10-06 · **Hardware:** Rev A adapter ordered; delivery, assembly inspection and bench tests pending.

You're welcome to build and remix Ronin. This records the intended harness and remaining checks; it is not a tested plug-and-play wiring release. [Ask Arthur](https://artfedderson.tech/#start) if you have questions. Working display touch controls are separate from LED integration, which still needs firmware and physical testing.

## What the adapter does

The 44 × 34 mm adapter takes LED power from the PSU's SATA cable. The ESP32-P4 display keeps its USB power and host connection. A four-wire display lead supplies two data signals, a 3.3 V logic reference and shared ground; **it does not carry LED power**.

Two independent outputs are intended to drive one ten-LED **Rainbow on a Matchstick V1.0 board by Blamm**. This avoids needing a second daisy-chain connector on V1.0. Planned firmware will address twenty individual pixels across two channels. That firmware, startup/fault handling and physical output are not verified yet.

One jumper selects **5 V or 12 V for both outputs together**. Matchstick V1.0 uses **5 V only**. The connector cannot detect a wrong voltage. Future 12 V addressable LEDs also need their pin order and protocol checked; four-wire analog RGB strips are unsupported.

```text
PSU SATA power ── modified extension ── J1 [Ronin Rev A adapter]
                                           ├─ J2 ── Matchstick A, 10 LEDs
                                           └─ J3 ── Matchstick B, 10 LEDs
ESP32-P4 display UART0_OUT ────────────── J4
USB host ─────────────────────────────── display power + control
```

This shows electrical connections, not final routing or connector viewing directions. LED brackets, cable lengths and insulating mounts need fit checks. The integral power-button indicator remains separate.

## Cable parts

Quantities cover one harness, excluding spare terminals. Source links identify parts; availability and final lengths still need checking.

| Part | Quantity | Exact reference / source | Use |
| --- | --- | --- | --- |
| SATA extension | 1 | [StarTech SATAPOWEXT8](https://www.startech.com/en-us/cables/satapowext8) | Replaceable 203.2 mm, 18 AWG donor lead; trim after measuring |
| SATA-input housing | 1 | JST **VHR-3N**, [part reference](https://jlcpcb.com/partdetail/C157899) | 3.96 mm VH, mates with J1 |
| VH terminals | 3 | JST **SVH-21T-P1.1**, [part reference](https://www.digikey.com/en/products/detail/jst-sales-america-inc/SVH-21T-P1-1/527368) | One conductor per contact; 18–22 AWG, insulation OD 1.7–3.0 mm |
| LED housings | 4 | JST **XHP-3**, [part reference](https://jlcpcb.com/partdetail/C144402) | Both ends of two LED leads; 2.50 mm XH |
| Display adapter-end housing | 1 | JST **XHP-4**, [part reference](https://jlcpcb.com/partdetail/C144403) | Mates with J4; 2.50 mm XH |
| XH terminals | 16 | JST **SXH-001T-P0.6**, [part reference](https://jlcpcb.com/partdetail/C140573) | 12 LED + 4 display; 22–28 AWG, insulation OD 0.9–1.9 mm |
| Display-end donor lead | 1 lead | Elecrow **CPC03044C**, [five-pack](https://www.elecrow.com/4-pin-crowtail-cable5-pcs-p-1561.html) | Candidate Crowtail lead; actual fit, wire size and mapping need confirmation |
| LED wire | Length pending | [Alpha Wire 3051](https://www.alphawire.com/products/wire/hook-up-wire/premium/3051/), 22 AWG | Insulation OD 1.575 ± 0.051 mm, within selected XH contact range |
| Selector shunt | 1 | [Harwin M7567-05](https://www.harwin.com/products/M7567-05), 3 A | J5 pins 1–2 for Matchsticks |
| Insulating mounts | 2 sets, dimensions pending | M2.5 standoffs/screws, supplier pending | 2.7 mm board holes; height, screw length and clearance need checks |

Buy spare terminals: a practice crimp costs less than a damaged display. Check the selected contacts in the [JST XH catalog](https://www.jst-mfg.com/product/pdf/eng/eXH.pdf) and [JST VH catalog](https://www.jst-mfg.com/product/pdf/eng/eVH.pdf). “JST connector” alone does not identify a compatible part.

## Read numbers before choosing wire colors

**All tables use electrical pin numbers.** A plug's mating-face view and wire-entry view reverse left and right. Do not copy an unqualified left-to-right photo. On the adapter, identify pad 1 from its square PCB pad and markings; identify the matching housing cavity from the connector drawing or continuity with a mating header. Label ends before inserting terminals.

| Adapter connector | Pin 1 | Pin 2 | Pin 3 | Pin 4 |
| --- | --- | --- | --- | --- |
| **J1**, SATA / VH | SATA +5 V | Ground | SATA +12 V | — |
| **J2**, LED A / XH | Data A | Selected LED supply | Ground | — |
| **J3**, LED B / XH | Data B | Selected LED supply | Ground | — |
| **J4**, display / XH | GPIO48 / A | GPIO47 / B | Display 3.3 V reference | Ground |
| **J5**, selector | +5 V | Selected input | +12 V | — |

Matchstick V1.0 has **pin 1 DIN, pin 2 +5 V, pin 3 GND**. Each LED cable maps 1→1, 2→2, 3→3. DOUT solder pads remain unused. Check actual revisions and markings against [Blamm's upstream design](https://github.com/VoronDesign/Voron-Hardware/tree/36355e8f7d3c6e8d8915fddd307beab8e69d1425/Daylight/Rainbow_on_a_matchstick).

For the **Elecrow Advanced 5-inch ESP32-P4 V1.0**, the supplied manufacturer schematic identifies **J2 UART0_OUT** as **HY2.0-4P-SMT**:

| Display UART0_OUT pin | Signal | Adapter J4 pin |
| --- | --- | --- |
| 1 | GPIO48 | 1 |
| 2 | GPIO47 | 2 |
| 3 | VDD3V3 | 3 |
| 4 | Ground | 4 |

This four-pin connector is **separate from the USB-C UART0 programming socket**. HY2.0 is not a confirmed JST PH part: equal pitch does not prove shell fit. Start with the manufacturer's lead and check fit and every conductor. The [manufacturer V1.0 CAD/schematic](https://github.com/Elecrow-RD/-CrowPanel-Advanced-5inch-ESP32-P4-HMI-AI-Display-800x480-IPS-Touch-Screen/tree/421a2d8c797f04151337ffd3ae03ad50bee0ac28) establishes electrical numbering, not the physical housing's left/right cavity view.

The display multiplexes these signals with an optional wireless interface. **SEL0 must be low** for UART0_OUT; switch labels alone are insufficient because vendor prose and circuit description disagree. The display's secondary STC controller can also affect this routing and needs checking. Confirm routing electrically and prevent two interfaces driving the lines before enabling LED firmware. GPIO47/48 are proposed LED signals based on that circuit; current firmware does not enable them as LED outputs.

## Make one cable at a time

Gather a wire cutter, suitable wire stripper, magnifier, multimeter, labels, heat-shrink and open-barrel crimp tooling. Match the contact and terminal form to the [JST tooling catalog](https://www.jst-mfg.com/product/pdf/eng/eCRIMPING_MACHINES_AND_TOOLS.pdf). The tool must suit the exact contact and wire; a generic “Dupont” crimper is not automatically suitable. Correctly made pre-crimped leads are an alternative. Check the factory display lead's gauge and insulation OD before choosing terminals.

Use the contact/tool instructions for strip length; there is no universal length. Practice first. Following [JST handling guidance](https://www.jst-mfg.com/product/pdf/eng/handling_e.pdf):

1. Strip without nicking or losing strands.
2. Seat all strands in the conductor barrel and insulation in the insulation barrel.
3. Crimp with the specified tool. Inspect both grips and mating section; reject crushed, bent or incomplete crimps.
4. Check retention gently, then insert into the planned cavity until retained. Recheck it cannot slide out.

Do not trim strands to fit oversized wire or put multiple conductors in a single-wire contact. Never use 18 AWG SATA wire in these XH contacts. Meter through a mating test header or breakout rather than oversized probes in female sockets.

Modify a **replaceable SATA extension**, retaining device-side male SATA blades to mate with the PSU plug. Verify which wire reaches +5 V, ground and +12 V, then map to J1. Red/black/yellow are clues, not proof. Individually insulate unused wires and exposed joins. **Do not modify or repin the PSU's modular-end cable:** its pinout depends on the PSU.

Measure lengths with the case assembled enough to represent the route. Keep data beside ground, leave unplugging slack, and anchor cables so they cannot pull on connectors. Clear fan blades, vents, hot heatsinks, sharp edges and magnetic-cover removal paths. Final lengths and tie points belong in the next CAD/manual revision.

## Check before power

With **USB and SATA both disconnected**, meter every cable end-to-end against the numbered tables. Check neighboring pins for shorts, latch orientation and labels. Confirm the modified SATA lead's polarity; verify PSU output voltages separately before connecting the adapter.

Install **one** shunt at J5 **1–2 for 5 V**. Never bridge both sides. To change it, disconnect USB and SATA, allow discharge and meter the rail first. Both LED outputs share the voltage choice. Label this harness **5 V ONLY — Matchstick V1.0**.

First board power-up is a controlled bench test: display/sticks disconnected, current-limited supply. Inspect assembly, check logic/output rails and complete parts omitted by the selected assembly service. Do not bridge a missing fuse. Someone comfortable with a bench supply and meter should perform these checks; a PSU straight into an unknown harness is not the first test.

After firmware is ready, test one dim pixel per output, then channel identity, color order and all ten pixels per stick. Rev A has no LED power-good data gating or verified rail-sensing interface. Software alone has not been shown able to detect SATA power loss. Keep LED signal outputs dormant until a validated sequencing/gating solution and bench acceptance are in place. USB-only, SATA-only and power-off tests must check backfeeding through the data level shifter and display 3.3 V reference as well as the power rails. Load voltage/temperature measurements and strain relief are also required before publishing a validated installation procedure.

**Current stopping point:** adapter ordered and modeled. Display plug fit/cavity orientation, lengths, mounting, mux behavior, LED firmware and bench acceptance remain open. Working application buttons do not establish working LEDs.
