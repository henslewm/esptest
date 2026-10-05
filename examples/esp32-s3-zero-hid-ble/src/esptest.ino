// esptest — Waveshare ESP32-S3-Zero
// Milestone 4: with no user input, drive the host to create test.txt and type date/timestamped
// lines into it over BOTH the USB and BLE keyboards. The USB keyboard opens Notepad (Win+R),
// both keyboards type a timestamped line, then the USB keyboard saves the file as test.txt.
// Also enumerates a USB HID mouse. The .ino holds only setup()/loop(); see src/MODULES.md.

#include <Arduino.h>
#include "USB.h"
#include "build_identity.h"
#include "hid_keyboard.h"
#include "hid_mouse.h"
#include "ble_keyboard.h"
#include "timestamp.h"

enum FlowState { FLOW_WAIT, FLOW_RUN, FLOW_DONE };
static FlowState flow = FLOW_WAIT;

static const uint32_t BLE_LINK_SETTLE_MS = 3000; // require BLE stably connected this long before running
static const uint32_t BLE_FALLBACK_MS = 120000;  // if BLE never links, proceed USB-only (allows time to re-pair)
static uint32_t bleConnectedSince = 0;           // millis() when the current BLE link came up (0 = down)
static const char* SAVE_PATH = "C:\\Users\\YOURNAME\\test.txt";

static void runTestFileWorkflow(bool withBle) {
  char ts[32];
  char line[80];

  Serial.println("[flow] opening Notepad via USB (Win+R)");
  hidKeyboardWinR();
  delay(700);
  hidKeyboardTypeLine("notepad");  // Run dialog: launch Notepad
  delay(2500);                     // wait for Notepad to open and take focus

  timestampNow(ts, sizeof(ts));
  snprintf(line, sizeof(line), "%s USB keystroke", ts);
  Serial.printf("[flow] USB typing: %s\n", line);
  hidKeyboardTypeLine(line);

  if (withBle) {
    delay(250);
    timestampNow(ts, sizeof(ts));
    snprintf(line, sizeof(line), "%s BLE keystroke", ts);
    Serial.printf("[flow] BLE typing: %s\n", line);
    bleKeyboardTypeLine(line);
  } else {
    Serial.println("[flow] BLE not linked; USB-only line written");
  }

  delay(400);
  Serial.printf("[flow] saving as %s via USB (Ctrl+S)\n", SAVE_PATH);
  hidKeyboardCtrlS();
  delay(1800);                 // wait for the Save As dialog
  hidKeyboardType(SAVE_PATH);  // full path in the filename field (deterministic location)
  delay(300);
  hidKeyboardTypeLine("");     // Enter: save
  delay(900);
  hidKeyboardTypeLine("");     // Enter again: confirm overwrite prompt if the file exists
  Serial.println("[flow] done");
}

void setup() {
  Serial.begin(115200);
  delay(300);
  Serial.println("[boot] esptest: USB HID + BLE keyboard + test.txt workflow");
  printBuildIdentity();

  hidKeyboardBegin();
  hidMouseBegin();
  USB.begin();
  Serial.println("[usb] HID keyboard + mouse started");

  bleKeyboardBegin("esptest-BLE-KB");
  Serial.println("[ble] advertising as 'esptest-BLE-KB'");
  hidMouseNudge();  // prove the mouse interface is live, once
}

void loop() {
  // Track how long the BLE link has been stably up, measured from the connect event,
  // so a late auto-reconnect gets the same settle window as a boot-time link.
  bool bleNow = bleKeyboardConnected();
  if (bleNow && bleConnectedSince == 0) {
    bleConnectedSince = millis();
  } else if (!bleNow) {
    bleConnectedSince = 0;
  }

  if (flow == FLOW_WAIT) {
    bool bleSettled = bleNow && (millis() - bleConnectedSince >= BLE_LINK_SETTLE_MS);
    bool fallback = millis() >= BLE_FALLBACK_MS;
    if (bleSettled || fallback) {
      flow = FLOW_RUN;
    }
  }

  if (flow == FLOW_RUN) {
    runTestFileWorkflow(bleKeyboardConnected());
    flow = FLOW_DONE;
  }

  delay(50);
}
