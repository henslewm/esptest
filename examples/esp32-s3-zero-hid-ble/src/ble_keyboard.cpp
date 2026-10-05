#include "ble_keyboard.h"

#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEHIDDevice.h>
#include <BLECharacteristic.h>
#include <BLESecurity.h>

static BLEHIDDevice* hid = nullptr;
static BLECharacteristic* input = nullptr;
static volatile bool connected = false;

// Standard boot-keyboard report map, report ID 1: 8-byte reports [modifier, reserved, 6 keycodes].
static const uint8_t REPORT_MAP[] = {
  0x05, 0x01,        // Usage Page (Generic Desktop)
  0x09, 0x06,        // Usage (Keyboard)
  0xA1, 0x01,        // Collection (Application)
  0x85, 0x01,        //   Report ID (1)
  0x05, 0x07,        //   Usage Page (Keyboard/Keypad)
  0x19, 0xE0,        //   Usage Minimum (0xE0)
  0x29, 0xE7,        //   Usage Maximum (0xE7)
  0x15, 0x00,        //   Logical Minimum (0)
  0x25, 0x01,        //   Logical Maximum (1)
  0x75, 0x01,        //   Report Size (1)
  0x95, 0x08,        //   Report Count (8)
  0x81, 0x02,        //   Input (Data,Var,Abs) - modifier byte
  0x95, 0x01,        //   Report Count (1)
  0x75, 0x08,        //   Report Size (8)
  0x81, 0x01,        //   Input (Const) - reserved byte
  0x95, 0x06,        //   Report Count (6)
  0x75, 0x08,        //   Report Size (8)
  0x15, 0x00,        //   Logical Minimum (0)
  0x25, 0x65,        //   Logical Maximum (101)
  0x05, 0x07,        //   Usage Page (Keyboard/Keypad)
  0x19, 0x00,        //   Usage Minimum (0)
  0x29, 0x65,        //   Usage Maximum (101)
  0x81, 0x00,        //   Input (Data,Array) - keycodes
  0xC0               // End Collection
};

class ServerCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer* s) override {
    connected = true;
    s->getAdvertising()->stop();  // stop radio chatter once linked (good 2.4GHz neighbor)
  }
  void onDisconnect(BLEServer* s) override {
    connected = false;
    s->getAdvertising()->start();  // re-advertise so the host can auto-reconnect
  }
};

// Map an ASCII char to a HID modifier + keycode. Returns false if unsupported.
static bool asciiToHid(char c, uint8_t& mod, uint8_t& key) {
  mod = 0;
  key = 0;
  if (c >= 'a' && c <= 'z') { key = 0x04 + (c - 'a'); return true; }
  if (c >= 'A' && c <= 'Z') { mod = 0x02; key = 0x04 + (c - 'A'); return true; }  // Left Shift
  if (c >= '1' && c <= '9') { key = 0x1E + (c - '1'); return true; }
  if (c == '0') { key = 0x27; return true; }
  switch (c) {
    case '\n': key = 0x28; return true;  // Enter
    case ' ':  key = 0x2C; return true;
    case '-':  key = 0x2D; return true;
    case '_':  mod = 0x02; key = 0x2D; return true;
    case '=':  key = 0x2E; return true;
    case '+':  mod = 0x02; key = 0x2E; return true;
    case '[':  key = 0x2F; return true;
    case '{':  mod = 0x02; key = 0x2F; return true;
    case ']':  key = 0x30; return true;
    case '}':  mod = 0x02; key = 0x30; return true;
    case '\\': key = 0x31; return true;
    case '|':  mod = 0x02; key = 0x31; return true;
    case ';':  key = 0x33; return true;
    case ':':  mod = 0x02; key = 0x33; return true;
    case '\'': key = 0x34; return true;
    case '"':  mod = 0x02; key = 0x34; return true;
    case '`':  key = 0x35; return true;
    case '~':  mod = 0x02; key = 0x35; return true;
    case ',':  key = 0x36; return true;
    case '<':  mod = 0x02; key = 0x36; return true;
    case '.':  key = 0x37; return true;
    case '>':  mod = 0x02; key = 0x37; return true;
    case '/':  key = 0x38; return true;
    case '?':  mod = 0x02; key = 0x38; return true;
    case '!':  mod = 0x02; key = 0x1E; return true;
    case '@':  mod = 0x02; key = 0x1F; return true;
    case '#':  mod = 0x02; key = 0x20; return true;
    case '$':  mod = 0x02; key = 0x21; return true;
    case '%':  mod = 0x02; key = 0x22; return true;
    case '^':  mod = 0x02; key = 0x23; return true;
    case '&':  mod = 0x02; key = 0x24; return true;
    case '*':  mod = 0x02; key = 0x25; return true;
    case '(':  mod = 0x02; key = 0x26; return true;
    case ')':  mod = 0x02; key = 0x27; return true;
    default:   return false;
  }
}

static void sendReport(uint8_t mod, uint8_t key) {
  uint8_t report[8] = {mod, 0, key, 0, 0, 0, 0, 0};
  input->setValue(report, sizeof(report));
  input->notify();
}

static void releaseKeys() {
  uint8_t report[8] = {0, 0, 0, 0, 0, 0, 0, 0};
  input->setValue(report, sizeof(report));
  input->notify();
}

void bleKeyboardBegin(const char* deviceName) {
  BLEDevice::init(deviceName);
  BLEDevice::setPower(ESP_PWR_LVL_N3);  // lower TX power; board sits next to the BT radio
  BLEServer* server = BLEDevice::createServer();
  server->setCallbacks(new ServerCallbacks());

  hid = new BLEHIDDevice(server);
  input = hid->inputReport(1);  // report ID 1

  hid->manufacturer()->setValue("esptest");
  hid->pnp(0x02, 0x303A, 0x1001, 0x0001);  // USB-IF vendor, our VID/PID, version
  hid->hidInfo(0x00, 0x01);                // country 0, remote-wake
  hid->reportMap((uint8_t*)REPORT_MAP, sizeof(REPORT_MAP));
  hid->startServices();
  hid->setBatteryLevel(100);

  BLESecurity* security = new BLESecurity();
  security->setAuthenticationMode(ESP_LE_AUTH_REQ_SC_BOND);  // bond, Just Works
  security->setCapability(ESP_IO_CAP_NONE);
  security->setInitEncryptionKey(ESP_BLE_ENC_KEY_MASK | ESP_BLE_ID_KEY_MASK);
  security->setRespEncryptionKey(ESP_BLE_ENC_KEY_MASK | ESP_BLE_ID_KEY_MASK);  // keep LTK so Windows reconnects after reboot

  BLEAdvertising* adv = server->getAdvertising();
  adv->setAppearance(0x03C1);  // HID Keyboard
  adv->addServiceUUID(hid->hidService()->getUUID());
  adv->setScanResponse(true);
  adv->setMinInterval(0x0140);  // ~200 ms: much less 2.4GHz airtime than the 20-40 ms default
  adv->setMaxInterval(0x0190);  // ~250 ms
  adv->start();
}

bool bleKeyboardConnected() {
  return connected;
}

void bleKeyboardTypeLine(const char* text) {
  if (!connected || input == nullptr || text == nullptr) {
    return;
  }
  for (const char* p = text; *p; ++p) {
    uint8_t mod, key;
    if (asciiToHid(*p, mod, key)) {
      sendReport(mod, key);
      delay(8);
      releaseKeys();
      delay(8);
    }
  }
  // Enter
  uint8_t mod, key;
  asciiToHid('\n', mod, key);
  sendReport(mod, key);
  delay(8);
  releaseKeys();
  delay(8);
}
