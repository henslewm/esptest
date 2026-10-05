#include "hid_keyboard.h"

#include <Arduino.h>
#include "USB.h"
#include "USBHIDKeyboard.h"

static USBHIDKeyboard keyboard;

void hidKeyboardBegin() {
  keyboard.begin();
}

// Type one char at a time with spacing so the host reliably registers the Shift
// modifier on shifted characters (fast bursts drop Shift: ':' -> ';', 'U' -> 'u').
static void typeText(const char* text) {
  if (text == nullptr) {
    return;
  }
  for (const char* p = text; *p != '\0'; ++p) {
    keyboard.write((uint8_t)*p);
    delay(14);
  }
}

void hidKeyboardType(const char* text) {
  typeText(text);
}

void hidKeyboardTypeLine(const char* text) {
  typeText(text);
  keyboard.write('\n');  // Enter
  delay(14);
}

void hidKeyboardWinR() {
  keyboard.press(KEY_LEFT_GUI);
  keyboard.press('r');
  delay(40);
  keyboard.releaseAll();
  delay(10);
}

void hidKeyboardCtrlS() {
  keyboard.press(KEY_LEFT_CTRL);
  keyboard.press('s');
  delay(40);
  keyboard.releaseAll();
  delay(10);
}
