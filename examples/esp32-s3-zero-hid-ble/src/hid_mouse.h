#pragma once

// USB HID mouse (TinyUSB). Owns the USBHIDMouse instance. Call hidMouseBegin() before
// USB.begin(); nudge() makes a small, self-cancelling move so the device is observable
// without disturbing the pointer.
void hidMouseBegin();
void hidMouseNudge();
