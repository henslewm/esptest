#pragma once

// USB HID keyboard (TinyUSB). Owns the USBHIDKeyboard instance. Call hidKeyboardBegin()
// before USB.begin(). typeLine() emits text + Enter; type() emits text only. winR() and
// ctrlS() send the Windows Run and Save shortcuts for driving Notepad.
void hidKeyboardBegin();
void hidKeyboardType(const char* text);
void hidKeyboardTypeLine(const char* text);
void hidKeyboardWinR();
void hidKeyboardCtrlS();
