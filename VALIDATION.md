# Migration validation

Validated during preparation on Windows x64:

* Public source and media push succeeded; a fresh anonymous clone completed.
* 2,154 text files passed a scan for common credential/private-key patterns before publication. Runtime logs, PID state, raw flash recovery backups and personal path provenance were excluded from publication.
* All 13 dependency ZIPs passed SHA256 and ZIP CRC checks. GitHub's recorded SHA256 digest and size matched every local archive. An anonymous release download also matched its expected hash.
* The relocated Python 3.12 environment imported companion, media and ESP-IDF packages successfully; the relocated Python 3.14 environment ran esptool 5.4.0.
* Seven Windows temperature tests and three collector tests passed using the relocated Python. The older collector test runner requires `PYTHONPATH` to include `project/outputs/Resource-Panel`.
* A complete clean ESP32-P4 firmware build succeeded using only the relocated SDK, compilers, Git and Python environments. The application occupied `0x339630` bytes with 18% of its partition free. The rebuilt binary was not flashed; the previously installed v3 release remains the recommended known device build.

The complete offline package retains private recovery/history data not present here. No board flash, SD reformat or physical wiring change was performed as part of migration. A new PC still needs a live-device check of its own USB port, temperatures and media playback.
