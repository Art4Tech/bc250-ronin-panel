# Ronin manual — October 6 revision notes

These supplement the frozen Rev0.3 PDF/BOM. They do not silently change its counts or replace the repository's earlier saved CAD. Build and remix Ronin freely; check revisions/open interfaces and [ask Arthur](https://artfedderson.tech/#start) if you have questions.

## Ordered adapter and revised harness

The **Ronin Rev A SATA-powered ESP32 LED adapter** is ordered and appears as one visible component in the working SolidWorks assembly. The 44 × 34 mm board uses SATA LED power; the display stays USB-powered/controlled. One 5/12 V selector supplies both outputs; Matchstick V1.0 requires **5 V**. Two independent channels replace the earlier display-powered, stick-to-stick harness proposal, letting planned firmware address all twenty pixels without daisy-chain connectors absent from V1.0.

Delivery and bench tests are pending. [Harness preparation](Ronin_LED_Harness_Rev_A.md) covers electrical maps, sourced parts, crimps and checks. Actual display plug fit/cavity orientation, mux, lengths, LED brackets and insulating mounts remain provisional. PCB source, Gerbers, manufacturing BOM/CPL and factory files are not included in this public release. Approved images can illustrate the board without publishing those files.

## NVMe arrangement in working CAD

The October 6 live inventory confirms **one NVMe extension board, two `NVME_Extension_Mount` instances, one modeled NVMe extension ribbon and one relocated `SSD_M2_2280`**. Extension board, twin mounts and explicit ribbon route are additions relative to the October 3 render export; SSD placement changed too. The working assembly has 157 unsuppressed instances, 151 visible. These observations do not update frozen Rev0.3 counts or the repository's preserved October 1 CAD snapshot.

The long vertical ribbon leg and extension sit behind the backbone's main sheet. The connector end returns toward the front-side M.2 splitter. This describes the modeled placement, not a collision or bend-radius check.

This route helps locate intended hardware and review service access. It does not prove ribbon bend limits, connector clearance, fit, PCIe performance or final purchase part numbers. World-axis bounds of the tilted assembly are not local part dimensions.

![Current CAD locator of the NVMe extension, modeled ribbon and twin mounts](../media/ronin-nvme-route.jpg)

Pink identifies the ribbon route, lavender the two printed mounts, and teal the SSD/extension. The backbone and BC250 are ghosted for visibility. This is a current-CAD locator, not a cable cut-length, physical fit or storage-link validation.

## Display prototype

The Windows prototype now has native menu/profiles and six reviewed app tiles. Existing-window focus/duplicate suppression passed physical testing; OBS also passed fresh-launch/focus. Repository firmware remains the earlier baseline. See [current status](../../display/CURRENT_STATUS.md). LED output, Linux desktop/Gaming Mode, PC gamepad wake and AI voice remain pending.

## Next-manual followups

| Change | Next revision must record | Acceptance still required |
| --- | --- | --- |
| LED adapter | Installed revision, mounts, selector, assembly photo and route | Received-board inspection; controlled power, voltage, load, heat and backfeeding |
| Independent LED leads | Lengths, real cavity photos, terminals, ties and removal slack | HY2.0 fit/continuity; mux; firmware and pixel/color tests |
| NVMe extension/twin mounts | Exact extension/ribbon source, fasteners and exploded views | Bend/clearance fit, retention and PCIe/NVMe operation |
| Menu/app controls | Paired source/install package, bindings and screenshots | Target Linux GUI, Wayland/Gaming Mode and real launches |
| Cable management | Serviceable ties/anchors and excess PSU routing | Fan/vent clearance, edge protection and cover removal |

Blamm's Matchsticks, Mosfet.Party's EPS Power Adapter/PS_ON Adapter, and MandicReally's single remixed 92 mm fan shroud retain their credits/upstream terms. The power-button indicator remains separate.
