#pragma once

// BLE HID keyboard (bundled Bluedroid stack). Single keyboard input report, Just-Works
// bonding so Windows can pair once and auto-reconnect. Call bleKeyboardBegin() in setup();
// bleKeyboardConnected() reflects an active link; typeLine() emits text + Enter when connected.
void bleKeyboardBegin(const char* deviceName);
bool bleKeyboardConnected();
void bleKeyboardTypeLine(const char* text);
