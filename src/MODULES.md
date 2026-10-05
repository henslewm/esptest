# Modules index — esptest firmware

The `.ino` is not a module; it holds only `setup()`/`loop()`. Open only the modules
that own the state you change, and update this index in the same change
(`.claude/rules/05-modular-code.md`).

| Module | Owns state | Header | Public functions | Notes |
|---|---|---|---|---|
| build_identity | none (reads app descriptor) | `build_identity.h` | `printBuildIdentity()` | Prints running firmware's ELF SHA-256 for hardware-evidence binding. |
| hid_keyboard | `USBHIDKeyboard` instance | `hid_keyboard.h` | `hidKeyboardBegin()`, `hidKeyboardType()`, `hidKeyboardTypeLine()`, `hidKeyboardWinR()`, `hidKeyboardCtrlS()` | USB HID keyboard emit, plus Win+R / Ctrl+S for driving Notepad. |
| hid_mouse | `USBHIDMouse` instance | `hid_mouse.h` | `hidMouseBegin()`, `hidMouseNudge()` | USB HID mouse emit; nudge is self-cancelling. |
| ble_keyboard | Bluedroid BLE HID server + keymap | `ble_keyboard.h` | `bleKeyboardBegin()`, `bleKeyboardConnected()`, `bleKeyboardTypeLine()` | BLE HID keyboard, single input report, Just-Works bonding, gentle advertising. |
| timestamp | none (build-time + uptime) | `timestamp.h` | `timestampNow()` | `"YYYY-MM-DD HH:MM:SS"`; no RTC or network clock. |

The test.txt workflow (open Notepad via USB, type timestamped lines over USB and BLE,
save as test.txt) is orchestrated by the loop state machine in `esptest.ino`, not a
separate module.
