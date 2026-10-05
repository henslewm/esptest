#pragma once
#include <stddef.h>

// Writes "YYYY-MM-DD HH:MM:SS" into buf. Time base is the firmware build time
// (__DATE__/__TIME__) plus uptime, since the board has no RTC or network clock.
void timestampNow(char* buf, size_t len);
