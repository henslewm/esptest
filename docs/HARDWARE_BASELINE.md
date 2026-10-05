# Hardware baseline — confirmed

Source: `python -m esptool flash-id` on COM7, 2026-10-04. Recorded here (not in
`config/bootstrap.json`, which is fingerprint-bound) so the confirmed values do not
invalidate the approved package.

| Property | Value |
|---|---|
| Chip | ESP32-S3 (QFN56), revision v0.2 |
| Cores | Dual-core + LP core, 240 MHz |
| Radios | Wi-Fi, Bluetooth 5 (LE) |
| Flash | 4 MB embedded, quad (XMC), manufacturer 0x46, device 0x4016 |
| PSRAM | 2 MB embedded, quad (AP_3v3), 3.3 V |
| Crystal | 40 MHz |
| MAC | d4:05:92:46:94:24 |
| Stock USB mode | USB-Serial/JTAG (VID:PID 303A:1001) |

## Build implications
- PSRAM is quad, so `board_build.psram_type = qspi` with `-DBOARD_HAS_PSRAM`.
- 4 MB flash → `huge_app.csv` partition (3 MB app + 1 OTA) fits; no custom table needed yet.
- HID requires TinyUSB, so `-DARDUINO_USB_MODE=0` (USB-OTG). This changes the USB
  enumeration identity away from the stock USB-Serial/JTAG device; recovery is via the
  BOOT button into ROM download mode.
- Confirmed against OL-004.

## Flashing (buttonless)

In OTG/HID mode the stock USB-Serial/JTAG auto-reset is gone, so `pio run -t upload`
cannot drop a running firmware into the bootloader by itself. Use the helper, which
performs the Arduino-ESP32 USB-CDC 1200bps-touch reboot into the ROM bootloader and
then uploads:

```
python tools/flash.py
```

Verified working 2026-10-04 with no button press. Fallback only if the running firmware
is crashed/old and cannot honor the touch: unplug, hold BOOT, replug, release BOOT, then
`pio run -e dev -t upload` (the board enumerates as "USB JTAG/serial debug unit").

## Baseline build evidence

- Build: `pio run -e dev` SUCCESS (core 3.3.12, IDF 5.5.5); flash 11.7% of 3 MB app, RAM 16.7%.
- App ELF SHA-256 (from `esptool image-info` on firmware.bin): `950786bcb4ae5729f009b2e6d8d6373b84fe38df482a0c5365ec71658821307c`.
- Flashed to hardware 2026-10-04. Runtime boot-serial confirmation (the `[build] app_elf_sha256=...`
  line) is pending an operator-observed capture; VERIFIED_ON_HARDWARE is not claimed.

## USB mode gotchas (ESP32-S3, TinyUSB HID)

Two things that cost a debugging cycle on Milestone 2; keep them in mind:

1. **The board manifest forces `-DARDUINO_USB_MODE=1`** (hardware USB-Serial/JTAG), which overrides
   a plain `build_flags` entry, so the firmware never switches to TinyUSB and no HID appears. Set OTG
   mode with `board_build.extra_flags = -DARDUINO_USB_MODE=0` (it replaces the manifest's extra_flags).
2. **Flashing over USB-Serial/JTAG persists that USB across the reset** (so the COM port survives), which
   keeps the board in JTAG mode and blocks the app's TinyUSB HID. After such a flash, **unplug/replug
   once** to let the firmware enumerate as the HID composite. The 1200bps-touch path (`tools/flash.py`)
   starts from the running OTG app instead.

## Milestone 2 — USB HID composite (keyboard + mouse)

- Build ELF SHA-256: `23af07c0dbb4e69389d3ade5bc76693ff24c14ca3f949e30e3ac5b7038271ad5`.
- Operator-observed: the firmware typed `esptest USB HID OK uptime_ms=10015` into a host text field
  ~10s after boot (USB HID keyboard confirmed emitting).
- Enumeration under VID:PID 303A:1001: USB Composite Device with HID Keyboard Device (COL01),
  HID-compliant mouse (COL02), and USB Serial Device (CDC). Satisfies the DoD's "USB keyboard/mouse
  visible in Device Manager."
