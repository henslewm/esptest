# Facts, Assumptions, Allegations, and Unknowns

## Verified facts

- Project name: esp32test.
- Desired outcome: To develop firmware on an ESP32S3 Zero Waveshare that attaches as HID keyboard & mouse when plugged into USB and then automatically links as a BLE Keyboard..
- Intake was recorded on 2026-10-04.
- Chip (via `esptool flash-id`, 2026-10-04): ESP32-S3 (QFN56) revision v0.2, stock USB-Serial/JTAG.
- Flash (via `esptool flash-id`, 2026-10-04): 4MB embedded quad flash.
- PSRAM (via `esptool flash-id`, 2026-10-04): 2MB embedded quad PSRAM (AP_3v3).
- MAC address (via `esptool flash-id`, 2026-10-04): d4:05:92:46:94:24.
- App ELF SHA-256 from the Milestone 1 baseline build (`pio run -e dev`, 2026-10-04): 950786bcb4ae5729f009b2e6d8d6373b84fe38df482a0c5365ec71658821307c.
- Bootstrap state is ACTIVE under architecture fingerprint e702f64de79f8c542af1b1f9920443080e3a4aac8a6eb3baa4325dad30112843 (validated with `validate_bootstrap --require-active`, 2026-10-04).
- Firmware was flashed to hardware on 2026-10-04; runtime boot-serial capture is still pending and VERIFIED_ON_HARDWARE is not claimed (flashing is not a hardware-verification claim).
- BLE address of the board (2026-10-04): d4:05:92:46:94:25 (base MAC d4:05:92:46:94:24 + 1).
- BLE keyboard "esptest-BLE-KB" paired with Windows and typed its line into Notepad (operator-confirmed, 2026-10-04); three HID keyboards enumerate OK (USB, Logitech K780, our BLE) and Windows auto-reconnects once bonded.
- App ELF SHA-256 from the Milestone 3 BLE build (`pio run -e dev`, 2026-10-04): f380eb4ce7ddceaaffb0c43320a25ddaff0d8eb4b2ee9491f9939b90c35f11bb.
- Definition of Done MET (operator-confirmed, 2026-10-04): on a clean boot with no user input the board auto-reconnected BLE, opened Notepad, and saved `C:\Users\hensl\test.txt` containing one USB line (`2026-10-04 22:48:11 USB keystroke`) and one BLE line (`2026-10-04 22:48:12 BLE keystroke`), typed out only; BLE keyboard visible/bonded in Windows settings and USB keyboard+mouse in Device Manager.
- App ELF SHA-256 from the Milestone 4 build (`pio run -e dev`, 2026-10-04): 905ede5f1dd9d761fd7fc6061dc045eabe925d9202807e1ccd89c2200ce15872 (flash 22.4%).
- Automatic click-free BLE pairing confirmed (2026-10-04) via a compiled C# WinRT helper (dotnet SDK 10): PAIR-STATUS: Paired with no click; PowerShell's `DeviceInformationCustomPairing` scriptblock handler returns "RejectedByHandler" in this PS 5.1/WinRT environment.
- Firmware timestamps are derived from build-time `__DATE__`/`__TIME__` + uptime with no RTC, so they drift and reset on reboot (2026-10-04).

## Working assumptions

- The repository will remain private unless the user changes visibility.
- Connector access is read-only except where `CONNECTOR_PLAN.md` explicitly states otherwise.

## Allegations or disputed claims

- None recorded during project initialization.

## Unknowns requiring resolution

- Which source controls each material disputed or version-sensitive fact.
- Whether all listed external systems are accessible in each selected AI client.
- Which repeated workflows justify custom skills after first use.
