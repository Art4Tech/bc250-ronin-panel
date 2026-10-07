# Ronin display and companion — current prototype

**Updated:** 2026-10-06. This summarizes the newer local prototype. Firmware, binaries and the resource helper elsewhere in this repository remain the earlier published baseline; this documentation update does not replace them.

## Working on the Windows test host

Live USB CPU/memory/disk telemetry, animated local scenes/audio, display Sleep/Wake and mute controls have physical verification. The newer prototype adds a native menu, saved profiles and a two-row, three-column application page:

| | Left | Middle | Right |
| --- | --- | --- | --- |
| Top row | SolidWorks | Steam | Claude |
| Bottom row | PrusaSlicer | OBS | Codex |

The user verified all six buttons bringing existing windows forward without duplicates; the existing-window path also restores minimized windows. OBS passed a fresh-launch/focus check. Other fresh launches have not been separately established. The host checks exact application identity and reports refused focus instead of blindly opening another copy.

In the current prototype, choose **RONIN MENU → Actions → Applications**. Normal startup requires **Enable reviewed tap actions for this session** in the companion. Short, completed releases execute reviewed actions; canceled/incomplete touches do not. One worker owns USB; run the paired companion rather than a second serial helper.

Physical Next/Previous/Home passed, with cached repeat changes faster than first load. A counter-only Bitfocus Satellite proof and guarded handoff are implemented; this does not establish every Bitfocus integration or automatic app-context switching.

The latest change is host-only: installed firmware remains **`ronin-inc3-20261004-r1`**. Verification recorded **302 Windows host tests** and **264 Linux pure-code tests**, plus physical Windows checks. Automated tests do not establish Linux desktop operation or physical LED output.

## Linux is the design baseline

Ronin targets Linux, initially Bazzite on the BC250, with one shared companion, firmware protocol and portable profiles wherever practical. Program paths, sensors, serial permissions, startup and desktop activation are host-specific bindings. A Windows SolidWorks binding does not make SolidWorks available on Linux: unavailable programs need an explicit alternative or remain unavailable.

The Linux X11 activation path uses `wmctrl`/`xprop` and exact process identity. Physical Linux GUI, native Wayland and Bazzite Gaming Mode tests remain pending. Normal operation is not intended to require Elgato Windows/macOS software. Bitfocus is the shared control-backend direction, with the Ronin app retaining sole USB ownership.

## Still being built

| Function | Status |
| --- | --- |
| Two ten-pixel Matchstick channels | Rev A SATA-powered adapter ordered; harness, mux, firmware and physical output unverified. See [harness preparation](../case-design/manual/Ronin_LED_Harness_Rev_A.md). |
| Resource monitor configuration/modules | Live telemetry works; broader module installation/configuration remains to develop. |
| Automatic app-context / held shortcuts | Pending; current reviewed releases and manually selected profiles are narrower. |
| Gamepad host wake/startup | Pending. **Display Sleep/Wake changes the display, not PC power.** |
| AI voice companion | Pending. Mic/speaker hardware and scene audio are not a completed voice workflow; Claude/Codex tiles launch host apps. |
| Linux GUI / hardware / Gaming Mode | Pending physical testing on the target BC250. |

Build and remix Ronin freely. It is evolving quickly; [reach out with questions](https://artfedderson.tech/#start). New source deliveries and tested instructions will have their own revision/evidence. The [Rev0.3 manual](../case-design/manual/README.md) is a frozen first-edition PDF, with newer work in [revision notes](../case-design/manual/REVISION_NOTES_2026-10-06.md).
