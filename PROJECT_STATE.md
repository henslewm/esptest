# Project State

- **Status:** Project objective COMPLETE — Definition of Done MET 2026-10-04 (operator-confirmed); Milestones 1-4 all complete and hardware-confirmed. Bootstrap gate remains ACTIVE under architecture fingerprint e702f64de79f8c542af1b1f9920443080e3a4aac8a6eb3baa4325dad30112843.
- **Last verified:** 2026-10-04
- **Current phase:** Complete — optional follow-ups only
- **Active branch:** main
- **Primary objective:** To develop firmware on an ESP32S3 Zero Waveshare that attaches as HID keyboard & mouse when plugged into USB and then automatically links as a BLE Keyboard.

## Current verified state

- Bootstrap gate ACTIVE under fingerprint e702f64… (ADR-003); routine flashing/monitoring permitted to the agent, damage-risk actions and VERIFIED_ON_HARDWARE claims operator-reserved.
- Milestone 1 (Baseline) complete and hardware-confirmed: root `platformio.ini`, pinned Arduino-ESP32 3.x core (ADR-002), `pio run -e dev` SUCCESS; hardware confirmed via `esptool flash-id` (ESP32-S3 QFN56, 4MB flash, 2MB PSRAM). App ELF SHA-256 = 950786bcb4ae5729f009b2e6d8d6373b84fe38df482a0c5365ec71658821307c.
- Milestone 2 (USB HID composite) complete and hardware-confirmed: `src/hid_keyboard.{h,cpp}`, `src/hid_mouse.{h,cpp}`, wired into `src/esptest.ino`. Enumerates as a TinyUSB composite under VID:PID 303A:1001 (keyboard COL01, mouse COL02, CDC serial); operator confirmed via `Get-PnpDevice` and a typed line into a host field. USB set to OTG/TinyUSB via `board_build.extra_flags` (ADR-004). App ELF SHA-256 = 23af07c0dbb4e69389d3ade5bc76693ff24c14ca3f949e30e3ac5b7038271ad5.
- Milestone 3 (BLE keyboard link) complete and hardware-confirmed: `src/ble_keyboard.{h,cpp}` (bundled Bluedroid `BLEHIDDevice`, keyboard-only report), wired into `src/esptest.ino`; `src/MODULES.md` updated. Advertises as BLE HID keyboard "esptest-BLE-KB" (BLE addr d4:05:92:46:94:25 = base MAC +1), bonded with Windows (Just-Works/ConfirmOnly) and typed into Notepad; three HID keyboards enumerate OK (USB, K780, our BLE); Windows auto-reconnects once bonded (no-input steady state). Gentle 2.4GHz-neighbor BLE settings applied after a mouse-interference incident (ADR-006, R-009). App ELF SHA-256 = f380eb4ce7ddceaaffb0c43320a25ddaff0d8eb4b2ee9491f9939b90c35f11bb (ADR-005). `pio run -e dev` SUCCESS, flash 21.5% of the 3MB app partition.
- Milestone 4 (timestamp + test.txt workflow) complete and hardware-confirmed — DEFINITION OF DONE MET (operator-confirmed): new `src/timestamp.{h,cpp}` (`timestampNow()` from build-time `__DATE__`/`__TIME__` + uptime, no RTC), `src/hid_keyboard.{h,cpp}` extended with `hidKeyboardType()`/`hidKeyboardWinR()`/`hidKeyboardCtrlS()` and char-by-char ~14ms typing (keeps Shift reliable, ADR-007), `src/esptest.ino` rewritten as a one-shot workflow state machine; `src/MODULES.md` updated. On a clean boot with no user input the board auto-reconnected BLE, opened Notepad via Win+R, typed one USB and one BLE timestamped line, and saved `C:\Users\hensl\test.txt` (contents: `2026-10-04 22:48:11 USB keystroke` / `2026-10-04 22:48:12 BLE keystroke`). BLE bond survives reboot via `security->setRespEncryptionKey(...)` (re-pair once). `pio run -e dev` SUCCESS, flash 22.4%. App ELF SHA-256 = 905ede5f1dd9d761fd7fc6061dc045eabe925d9202807e1ccd89c2200ce15872 (ADR-007, ADR-008). `VERIFIED_ON_HARDWARE` stays operator-reserved (OL-008 open); DoD is met on operator attestation.
- Automatic (click-free) BLE first-pairing solved via a compiled C# WinRT helper (dotnet SDK 10, PAIR-STATUS: Paired), currently in the session scratchpad only, not yet in the repo (ADR-008, OL-011).
- Domain: software-hardware; risk: low; sensitivity: private.

## Work completed

- Milestones 1-4 complete and operator-confirmed on hardware (see above). `pio run -e dev` SUCCESS at all four milestones; Milestone 2 flash 12.0%, Milestone 3 21.5%, and Milestone 4 22.4% of the 3MB app partition.
- Milestone 4: `src/timestamp.{h,cpp}` (build-time+uptime timestamp, no RTC), char-by-char ~14ms USB typing for Shift reliability (ADR-007), `src/esptest.ino` one-shot workflow state machine, and `security->setRespEncryptionKey(...)` so the BLE bond survives reboot; produced `C:\Users\hensl\test.txt` with one USB and one BLE timestamped line on a hands-free clean boot (DoD met).
- Three Milestone 2 USB/build fixes documented in `docs/HARDWARE_BASELINE.md`: USB_MODE override via `board_build.extra_flags`, FS include path via `build_flags -I`, and the one-time replug-to-activate after a USB-Serial/JTAG-mode flash.
- Milestone 3 BLE: both HID keyboards (USB and BLE) type; BLE device "esptest-BLE-KB" bonded with Windows and auto-reconnects. A mouse-interference incident was resolved with gentle 2.4GHz-neighbor BLE settings (ADR-006, R-009).
- Buttonless re-flash via `tools/flash.py` (1200bps-touch reboot into ROM bootloader).

## Current focus

- Optional follow-ups only — the project objective is complete and the DoD is met. Vendor the click-free C# pairtool into the repo (`tools/pairtool/`), optionally wire a logon auto-pair scheduled task, and commit the session's work when authorized (OL-011).

## Next three actions

1. Vendor the click-free C# WinRT pairtool from the scratchpad into the repo (e.g. `tools/pairtool/`) (OL-011).
2. Optionally wire the pairtool to a logon scheduled task for fully zero-touch first-pair — must pair before the firmware's 120s USB-only fallback (OL-011).
3. Commit the session's durable-record and firmware changes when authorized (no commit made this session).
