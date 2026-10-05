#include "timestamp.h"

#include <Arduino.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>

static time_t baseTime = 0;

// Convert the compiler's __DATE__ ("Mmm dd yyyy") and __TIME__ ("HH:MM:SS") to a time_t.
static time_t compileTime() {
  const char* d = __DATE__;
  const char* t = __TIME__;
  static const char* months = "JanFebMarAprMayJunJulAugSepOctNovDec";
  char mon[4] = {d[0], d[1], d[2], '\0'};
  const char* p = strstr(months, mon);
  int month = (p != nullptr) ? (int)((p - months) / 3) : 0;

  struct tm tmv;
  memset(&tmv, 0, sizeof(tmv));
  tmv.tm_year = atoi(d + 7) - 1900;
  tmv.tm_mon = month;
  tmv.tm_mday = atoi(d + 4);  // day may be space-padded; atoi handles it
  tmv.tm_hour = atoi(t);
  tmv.tm_min = atoi(t + 3);
  tmv.tm_sec = atoi(t + 6);
  tmv.tm_isdst = -1;
  return mktime(&tmv);
}

void timestampNow(char* buf, size_t len) {
  if (baseTime == 0) {
    baseTime = compileTime();
  }
  time_t now = baseTime + (time_t)(millis() / 1000UL);
  struct tm tmv;
  localtime_r(&now, &tmv);
  strftime(buf, len, "%Y-%m-%d %H:%M:%S", &tmv);
}
