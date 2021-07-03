#ifndef MyWatch_h
#define MyWatch_h

#include "defines.h"

#define FIRMWARE_VERSION  "1.0.13"

#ifndef LILYGO_WATCH_2020_V2
#ifdef ENABLE_MOTOR
#define LILYGO_WATCH_HAS_MOTOR       //Use Motor module 
#endif
#undef STANDARD_BACKPLANE
#define EXTERNAL_DAC_BACKPLANE
#endif
#define LILYGO_WATCH_LVGL                   //To use LVGL, you need to enable the macro LVGL
#define NTS_SERVER "pool.ntp.org"

#define CPU_FREQ_MIN     10
#define CPU_FREQ_NORM    80
#define CPU_FREQ_WIFI    80
#define CPU_FREQ_MEDIUM 160
#define CPU_FREQ_MAX    240

#define ENABLE_INTERRUPTS() \
  ttgo->enableTouchIRQ(); \
  ttgo->bma->enableStepCountInterrupt(true);

#define DISABLE_INTERRUPTS() \
  ttgo->disableTouchIRQ(); \
  ttgo->bma->enableStepCountInterrupt(false);

#define LV_OBJ_DEL(obj) \
  if (obj != NULL && obj != nullptr) { \
    lv_obj_del(obj); \
    obj = nullptr; \
  }

#define FREE(obj) \
  if (obj != NULL && obj != nullptr) { \
    free(obj); \
    obj = nullptr; \
  }

#define ISNULL(obj) (obj == NULL || obj == nullptr)
#define ISNOTNULL(obj) (obj != NULL && obj != nullptr)
#define ARRAY_SIZE(array) (sizeof(array) / sizeof(array[0]))
#define PERCENT(a,b) ((100 * a) / b)
#define NO_SSL

#ifdef ENABLE_SERIAL_DEBUG
#define serout(...) Serial.println(__VA_ARGS__)
#define seroutf(...) Serial.printf_P(__VA_ARGS__)
#else
#define serout(...)
#define seroutf(...)
#endif

#include <SPIFFS.h>
#include <LilyGoWatch.h>
#include <ArduinoJson.h>

extern const char rootCACertificate[] PROGMEM;
void print_mem();
void printInfo(const char* text, bool error);
void messageBox(const char* text);

enum WATCH_EVENT {
  EVENT_USB_PLUG,
  EVENT_USB_UNPLUG,
  EVENT_BATT_CHARGE_START,
  EVENT_BATT_CHARGE_STOP,
  EVENT_STEP_COUNTER
};

#define SETTINGS_FILE "/settings.json"
#define TIMEOUT_MS 30000

#endif
