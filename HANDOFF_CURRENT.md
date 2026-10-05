# Current Handoff

- **Prepared:** 2026-10-04
- **From:** Milestone 4 (timestamp + test.txt workflow) session
- **To:** Next session (optional follow-ups / closeout)
- **Branch / verified commit:** main / 1fff624 (Initial commit) per the session-start git snapshot; working tree dirty, no new commit made this session
- **Bootstrap state:** ACTIVE under architecture fingerprint e702f64de79f8c542af1b1f9920443080e3a4aac8a6eb3baa4325dad30112843 (`validate_bootstrap --require-active` confirmed ACTIVE in a prior session)

## Active objective

To develop firmware on an ESP32S3 Zero Waveshare that attaches as HID keyboard & mouse when plugged into USB and then automatically links as a BLE Keyboard. STATUS: COMPLETE — all four milestones done and hardware-confirmed; Definition of Done MET (operator-confirmed).

## Completed and verified

- Milestone 1 (Baseline) complete and hardware-confirmed: root `platformio.ini`, Arduino-ESP32 3.x pinned (ADR-002), `pio run -e dev` SUCCESS; hardware confirmed via `esptool flash-id`. App ELF SHA-256 = 950786bcb4ae5729f009b2e6d8d6373b84fe38df482a0c5365ec71658821307c.
- Milestone 2 (USB HID composite) complete and hardware-confirmed: `src/hid_keyboard.{h,cpp}`, `src/hid_mouse.{h,cpp}`, wired into `src/esptest.ino`. Enumerates as a TinyUSB composite under VID:PID 303A:1001 — HID Keyboard Device (COL01), HID-compliant mouse (COL02), USB CDC serial. Operator confirmed via `Get-PnpDevice` and a typed line into a host field. `pio run -e dev` SUCCESS, flash 12.0%. App ELF SHA-256 = 23af07c0dbb4e69389d3ade5bc76693ff24c14ca3f949e30e3ac5b7038271ad5 (ADR-004).
- Milestone 3 (BLE keyboard link) complete and hardware-confirmed: `src/ble_keyboard.{h,cpp}` using the bundled Bluedroid `BLEHIDDevice` stack with a keyboard-only input report (avoids the core-3.3.12 NimBLE duplicate-UUID bug), wired into `src/esptest.ino`. Advertises as "esptest-BLE-KB" (BLE addr d4:05:92:46:94:25 = base MAC +1), bonds with Windows via Just-Works/ConfirmOnly; operator confirmed it paired and typed into Notepad. Once bonded, Windows auto-reconnects with no input. `pio run -e dev` SUCCESS, flash 21.5%. App ELF SHA-256 = f380eb4ce7ddceaaffb0c43320a25ddaff0d8eb4b2ee9491f9939b90c35f11bb (ADR-005, ADR-006). Mouse-interference incident resolved with gentle 2.4GHz-neighbor BLE settings (R-009).
- Milestone 4 (timestamp + test.txt workflow) complete and hardware-confirmed — DEFINITION OF DONE MET (operator-confirmed): new `src/timestamp.{h,cpp}` (`timestampNow()` emits `YYYY-MM-DD HH:MM:SS` from build-time `__DATE__`/`__TIME__` + uptime; no RTC — drifts and resets on reboot); `src/hid_keyboard.{h,cpp}` gained `hidKeyboardType()`, `hidKeyboardWinR()`, `hidKeyboardCtrlS()` and now types char-by-char with ~14ms spacing so the host registers Shift (ADR-007); `src/esptest.ino` rewritten as a one-shot workflow state machine (waits for BLE link or a 120s USB-only fallback, drives Notepad, saves to a fixed `SAVE_PATH`, handles the overwrite prompt); `src/MODULES.md` updated. On a clean boot with no user input the board auto-reconnected BLE, opened Notepad via Win+R, typed one USB and one BLE line, and saved `C:\Users\hensl\test.txt`. Operator-confirmed contents:
  - `2026-10-04 22:48:11 USB keystroke`
  - `2026-10-04 22:48:12 BLE keystroke`
  `pio run -e dev` SUCCESS, flash 22.4%. App ELF SHA-256 = 905ede5f1dd9d761fd7fc6061dc045eabe925d9202807e1ccd89c2200ce15872 (ADR-007, ADR-008).
- BLE reconnect fix: `security->setRespEncryptionKey(...)` added in `ble_keyboard.cpp` so the bond survives reboot (Windows auto-reconnects); this required removing the old bond and re-pairing once.
- Automatic (click-free) BLE first-pairing solved: a small compiled C# WinRT helper (dotnet SDK 10) with `custom.PairingRequested += (s,e) => e.Accept()` pairs with no click (PAIR-STATUS: Paired); PowerShell's scriptblock handler returns "RejectedByHandler". The helper (`scratchpad/pairtool/Program.cs` + `pair.csproj`) lives only in the session scratchpad and is NOT yet in the repo (ADR-008, OL-011).
- Buttonless re-flash via `tools/flash.py` (USB-CDC 1200bps-touch reboot into the ROM bootloader); after a USB-Serial/JTAG-mode flash, replug once to activate the TinyUSB HID composite.

## Sources relied upon

- User-provided intake in `config/project.json` and `config/bootstrap.json`.
- `docs/PLATFORMIO.md`, `docs/HARDWARE_BASELINE.md`, and `docs/BLE_NOTES.md` in-repo.
- Operator attestation of the clean-boot run and `test.txt` contents (2026-10-04).

## Unresolved risks, contradictions, or unknowns

- `VERIFIED_ON_HARDWARE` is not auto-claimed: operator attested the full workflow and `test.txt` contents, but runtime boot-serial capture is still pending (OL-008). DoD is met on operator attestation; the `VERIFIED_ON_HARDWARE` ladder rung stays operator-reserved (R-007 stays live).
- Timestamps are build-time + uptime (no RTC), so they drift and reset on reboot (R-010).
- The click-free C# pairtool exists only in the scratchpad; it is not vendored into the repo, and no logon auto-pair task is wired (OL-011). The first `dotnet build` took >2 min, which once exceeded the firmware's 120s USB-only fallback.
- ESP32 BLE advertising can interfere with nearby 2.4GHz/BLE peripherals (R-009); mitigations in place. OTG replug-to-activate after a JTAG-mode flash (R-008).

## Exact next action

Optional follow-ups only — the objective is complete and the DoD is met. Vendor the C# pairtool from `scratchpad/pairtool` into the repo (e.g. `tools/pairtool/`), optionally wire a logon scheduled task for zero-touch first-pair, and commit the session's work when authorized.

## Do not assume

- Do not assume the C# pairtool is in the repo — it is not; it lives only in the session scratchpad.
- Do not assume any commit or push occurred this session (none did). Durable records were edited in the working tree; git was not run by this subagent.
- Do not assume `python scripts/validate_project.py` was run — it was not run this session.
- Do not assume `VERIFIED_ON_HARDWARE` is earned; it stays operator-reserved (OL-008 open), and damage-risk physical actions remain operator-reserved.
- Do not assume external connectors are authenticated or that any reference source (SRC-003..006) was verified.
