# GameSir display-wake test

Current evidence: the Nova 2 Lite_G paired and opened its BLE HID service on the panel. A/Home presses in that mode did not produce input reports. Android/yellow mode has not yet appeared in the BLE scan. This does not establish that the controller is Classic-only in every mode.

The next firmware wakes the display when a GameSir connection successfully opens. It does not interpret Home-button packets from an already-connected controller, and it does not operate any BC250 power-switch output.

After firmware installation is confirmed:

1. Keep the display awake. Hold Home + RB for two seconds to select G-Touch mode (cyan). If needed, hold Home + Screenshot for three seconds to enter rapid-flashing pairing mode. These are the combinations in GameSir's current official manual; firmware variations may differ.
2. Wait for the display's status to say GameSir connected. Pairing can take several seconds.
3. Turn the controller off and wait for the panel to report disconnected. Put the display to sleep using its touch button.
4. Turn the controller back on using Home in the same mode. Successful reconnection should wake the display. This is the test still requiring physical confirmation.

Simply putting the display to sleep while leaving the controller connected will not create a new connection event. A single Home press in that situation has no mapped wake action yet.

Bluetooth handoff to the BC250's separate adapter is not implemented. Do not assume the controller can be connected to both hosts simultaneously or that the same mode will suit PC gaming. The relay and standby-power wiring are separate future tests.

Official mode reference: https://gamesir.com/support/manuals/gamesir-nova-2-lite
