#include "hid_mouse.h"

#include <Arduino.h>
#include "USB.h"
#include "USBHIDMouse.h"

static USBHIDMouse mouse;

void hidMouseBegin() {
  mouse.begin();
}

void hidMouseNudge() {
  // Move a few pixels and back so the mouse is provably live but the pointer ends put.
  mouse.move(4, 0);
  delay(40);
  mouse.move(-4, 0);
}
