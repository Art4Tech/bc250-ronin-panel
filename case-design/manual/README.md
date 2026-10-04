# BC250 Ronin printing and assembly manual

> [!WARNING]
> **In-progress project — changing rapidly. Contact Arthur before building now.** Before purchasing parts, printing, or starting assembly, [contact Arthur](https://artfedderson.tech/#start) to confirm the latest design and open decisions. Rev0.3 is a draft for review and iteration, with provisional CAD locator illustrations. It is not a finalized fabrication, wiring, or assembly release.

**Revision:** Rev0.3 · **Status:** provisional first edition · **Publication date:** 2026-10-04

[Open the illustrated PDF](Rev_0p3/BC250_Ronin_Build_Manual_Rev_0p3.pdf)

## Editable companion files

| File set | CSV | JSON | Purpose |
| --- | --- | --- | --- |
| CAD inventory / provisional BOM | [CAD inventory](Rev_0p3/BC250_CAD_BOM_Rev_0p3.csv) | [CAD inventory](Rev_0p3/BC250_CAD_BOM_Rev_0p3.json) | 34 CAD source rows covering 83 active assembly instances, with sourcing and verification status |
| Open items | [Open items](Rev_0p3/BC250_Open_Items_Rev_0p3.csv) | [Open items](Rev_0p3/BC250_Open_Items_Rev_0p3.json) | 21 original unresolved fit, hardware, mounting, routing, and validation decisions |
| Proposed hardware / context | [Proposed hardware](Rev_0p3/BC250_Proposed_Hardware_Rev_0p3.csv) | [Proposed hardware](Rev_0p3/BC250_Proposed_Hardware_Rev_0p3.json) | 59 proposal, context, reuse, alternative, and routing rows requiring confirmation |

Unconfirmed quantities, specifications, supplier part numbers, and interfaces remain explicitly open. A file present in the CAD folder is not evidence that the part is used in the active assembly. Match the manual's CAD source/revision notes to the design you intend to build; the repository's original saved CAD snapshot is described in the [case-design guide](../README.md).

These are public companion derivatives for this manual revision. They preserve modeled counts, stable IDs, proposal quantities, specifications, and unresolved purchase fields. The proposal register includes existing hardware reuse and alternatives; the final purchase BOM remains unfinished.

## CAD revision used by this draft

Rev0.3 records a native CAD export with 83 active instances and 34 unique CAD source paths. The saved `BC250-Assy.SLDASM` was observed on **2026-10-03 at 21:11:25 UTC**, with SHA-256 `9373a60ab27ae99628ebbc556cbeacaa1fa84fe42846cb7406cfa16978e2c861`. The export SHA-256 is `6629e8ec61feadab36cd367e2baddc19778b2425403b07549a4b3b1a90aa8ac1`. The export postdates that saved assembly; this does not prove every referenced component is unchanged.

The [assembly preserved in the repository](../BC250-Assy.SLDASM) is the earlier imported snapshot, with Git LFS SHA-256 `271cf7703d2a2d1c0b56d6f5e30ba2481672cdabf75e36cca64796add342484a`. It differs from the saved assembly observed for this manual. **Confirm the current CAD and parts with Arthur before fabrication.** This manual publication does not update that preserved CAD snapshot.

## How to use this draft

1. Read the revision status and unresolved decisions in the PDF before selecting parts or committing to a print.
2. Check the BOM's modeled, sourced, and physically verified evidence for each component.
3. Treat printed orientations, mounting/routing concepts, and current-CAD locators as provisional wherever indicated.
4. Confirm final wiring and mechanical interfaces before applying power or following an assembly sequence.

The design uses two ten-LED **Rainbow on a Matchstick boards by Blamm**; the power-button indicator is separate. LED mounting and harness details still need confirmation. The **Mosfet.Party EPS Power Adapter and PS_ON Adapter** remain subject to variant, mounting, and harness confirmation. Credit for the single remixed 92 mm fan shroud belongs to **MandicReally**. Component manufacturer sources and remaining verification work are recorded in the manual and companion data.

Live USB resource telemetry has been physically confirmed. New LED control, touch shortcuts, gamepad host wake, and AI/voice functions remain planned until verified. See the [display package](../../display/README.md) and [project overview](../../README.md) for software status.

## Keeping the revision current

Mechanical, electronics, printing, display, and host-software changes must update the relevant manual/BOM entries or record a concrete documentation follow-up for the next revision. Keep proposed parts separate from confirmed BOM entries. Refresh figures from the stated assembly source when CAD changes; final exploded views and detailed assembly order depend on resolving the remaining interfaces.

The illustrations and manual prose are original project work. Voron's documentation organization was a clarity reference; no Voron artwork, diagrams, icons, page assets, or prose are reused, and this project is not affiliated with Voron. Third-party components and models retain their existing rights; this manual adds no blanket license to them.
