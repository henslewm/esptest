# Worked example — ESP32-S3-Zero as USB + BLE HID keyboard/mouse

A complete, hardware-verified starting point for a **Waveshare ESP32-S3-Zero** that:

- enumerates over USB as a **composite TinyUSB HID keyboard + mouse** (Device Manager), and
- advertises as a **BLE HID keyboard** that Windows bonds with and auto-reconnects, and
- with no user input, drives Notepad to create a file and types a date/timestamped line into
  it over **both** the USB and BLE keyboards.

It only emits keystrokes it generates; it never reads or captures host input.

This example exists because getting there hit several non-obvious walls. Each is solved below so
the next project starts from working code. Built and confirmed on hardware with Arduino-ESP32
3.3.12 (pioarduino, ESP-IDF 5.5.5) on Windows 11.

## Layout

```
platformio.ini          build config with every fix baked in
src/
  esptest.ino           setup()/loop() only: the workflow state machine
  build_identity.*      prints the running ELF SHA-256 for evidence
  hid_keyboard.*        USB HID keyboard: Shift-safe typing + Win+R / Ctrl+S
  hid_mouse.*           USB HID mouse
  ble_keyboard.*        Bluedroid BLE HID keyboard + ASCII keymap (gentle advertising)
  timestamp.*           "YYYY-MM-DD HH:MM:SS" from build time + uptime (no RTC)
  MODULES.md            module index
tools/
  flash.py              buttonless flash (1200-baud-touch reboot into bootloader)
  pairtool/             compiled C# helper: click-free Windows BLE pairing
```

## Quick start

1. Identify your hardware (do not trust the board name):
   `python -m esptool flash-id` — set flash size / `psram_type` in `platformio.ini` to match.
2. Edit `SAVE_PATH` in `src/esptest.ino` to an absolute Windows path you want the file saved to.
3. Build: `pio run -e dev`.
4. First flash: hold **BOOT**, plug in USB-C, release BOOT (ROM download mode), then
   `pio run -e dev -t upload`. **Unplug/replug once** afterward (see gotcha 3).
5. Pair BLE once, no clicks: `cd tools/pairtool && dotnet run -c Release -- esptest`.
6. Later reflashes are buttonless: `python tools/flash.py` (then one replug to activate USB).

On a clean boot it auto-reconnects BLE and runs the workflow: Win+R → Notepad → types one USB and
one BLE line → Ctrl+S → saves to `SAVE_PATH`.

## Gotchas this example already solves

1. **HID needs TinyUSB, but the board forces the wrong USB mode.** `esp32-s3-devkitc-1` sets
   `-DARDUINO_USB_MODE=1` (hardware USB-Serial/JTAG), which overrides a plain `build_flags` entry,
   so no custom HID ever appears. Fix: `board_build.extra_flags = -DARDUINO_USB_MODE=0` (OTG).

2. **`FS.h: No such file or directory` building the USB library.** The bundled USB library compiles
   its mass-storage sources, which `#include "FS.h"`, but PlatformIO's LDF does not honor the USB
   lib's `depends=FS`. Fix: a pre-build script (`add_fs_include.py`, wired via
   `extra_scripts = pre:add_fs_include.py`) resolves the framework package directory and appends the
   bundled `FS/src` to `CPPPATH`. This is portable; a hardcoded `-I` works too but isn't.

3. **Flashing + activation in OTG mode.** Once TinyUSB firmware runs, the stock USB-Serial/JTAG
   auto-reset is gone, so `pio run -t upload` can't drop it into the bootloader. `tools/flash.py`
   triggers the Arduino USB-CDC 1200-baud-touch reboot into the ROM bootloader, then uploads.
   Also: a flash done over USB-Serial/JTAG **persists** that USB across the reset, so the board
   stays in JTAG mode until a real power cycle — **unplug/replug once** to activate the HID composite.

4. **USB typing drops Shift when it's fast.** A quick `keyboard.print()` loses the Shift modifier,
   so `:` becomes `;` and capitals become lowercase. Fix: type one character at a time with ~14 ms
   spacing (`hid_keyboard.cpp`).

5. **BLE reconnect + report map.** Use a **keyboard-only** BLE report (mouse stays USB) to avoid a
   core-3.3.12 NimBLE bug that silently drops a second same-UUID input report. Set **both**
   `setInitEncryptionKey` and `setRespEncryptionKey`, or Windows bonds but won't reconnect after a
   reboot. The board's BLE address is the eFuse base MAC **+ 1**.

6. **Be a good 2.4 GHz neighbor.** Default fast advertising at full power can knock other nearby BLE
   devices (e.g. a BLE mouse) offline. This example lowers TX power (`ESP_PWR_LVL_N3`), slows
   advertising to ~200–250 ms, and stops advertising once connected.

7. **Click-free Windows pairing.** Windows must initiate the first bond; it can be scripted, but
   PowerShell's `DeviceInformationCustomPairing` `PairingRequested` scriptblock returns
   `RejectedByHandler` in PS 5.1. A tiny compiled C# WinRT helper whose
   `PairingRequested += (s,e) => e.Accept()` fires reliably — see `tools/pairtool/`. After the first
   bond, Windows auto-reconnects on every boot.

## Verification ladder

- static / build: `pio run -e dev` must pass.
- hardware: `VERIFIED_ON_HARDWARE` is earned by operator attestation, never written in code. Flash,
  observe the HID devices in Device Manager, confirm the BLE bond, and check the saved file. A clean
  compile is not hardware verification.

## Known limitations

- Timestamps come from build time + uptime, so they drift and reset on reboot. For real wall-clock
  time, add WiFi + NTP or have the host send the time over the USB CDC once at startup.
- `SAVE_PATH` in `src/esptest.ino` is the one machine-specific value to set before building (the
   FS include path is now resolved automatically by `add_fs_include.py`).
