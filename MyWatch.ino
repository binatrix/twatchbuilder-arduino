#include <WiFi.h>
#include <ArduinoJson.h>
#include "MyWatch.h"
#ifdef ENABLE_ALARM
#include "audio.h"
#endif
#include "utils.h"
// #include <esp_core_dump.h>

#define DEV_MODE_2
#define ENABLE_LOG_2

#ifndef ENABLE_BLUETOOTH
// Descargar cer en base64 (R3, nivel2)
const char rootCACertificate[] PROGMEM =
  "-----BEGIN CERTIFICATE-----\n" \
  "MIIEZTCCA02gAwIBAgIQQAF1BIMUpMghjISpDBbN3zANBgkqhkiG9w0BAQsFADA/\n" \
  "MSQwIgYDVQQKExtEaWdpdGFsIFNpZ25hdHVyZSBUcnVzdCBDby4xFzAVBgNVBAMT\n" \
  "DkRTVCBSb290IENBIFgzMB4XDTIwMTAwNzE5MjE0MFoXDTIxMDkyOTE5MjE0MFow\n" \
  "MjELMAkGA1UEBhMCVVMxFjAUBgNVBAoTDUxldCdzIEVuY3J5cHQxCzAJBgNVBAMT\n" \
  "AlIzMIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAuwIVKMz2oJTTDxLs\n" \
  "jVWSw/iC8ZmmekKIp10mqrUrucVMsa+Oa/l1yKPXD0eUFFU1V4yeqKI5GfWCPEKp\n" \
  "Tm71O8Mu243AsFzzWTjn7c9p8FoLG77AlCQlh/o3cbMT5xys4Zvv2+Q7RVJFlqnB\n" \
  "U840yFLuta7tj95gcOKlVKu2bQ6XpUA0ayvTvGbrZjR8+muLj1cpmfgwF126cm/7\n" \
  "gcWt0oZYPRfH5wm78Sv3htzB2nFd1EbjzK0lwYi8YGd1ZrPxGPeiXOZT/zqItkel\n" \
  "/xMY6pgJdz+dU/nPAeX1pnAXFK9jpP+Zs5Od3FOnBv5IhR2haa4ldbsTzFID9e1R\n" \
  "oYvbFQIDAQABo4IBaDCCAWQwEgYDVR0TAQH/BAgwBgEB/wIBADAOBgNVHQ8BAf8E\n" \
  "BAMCAYYwSwYIKwYBBQUHAQEEPzA9MDsGCCsGAQUFBzAChi9odHRwOi8vYXBwcy5p\n" \
  "ZGVudHJ1c3QuY29tL3Jvb3RzL2RzdHJvb3RjYXgzLnA3YzAfBgNVHSMEGDAWgBTE\n" \
  "p7Gkeyxx+tvhS5B1/8QVYIWJEDBUBgNVHSAETTBLMAgGBmeBDAECATA/BgsrBgEE\n" \
  "AYLfEwEBATAwMC4GCCsGAQUFBwIBFiJodHRwOi8vY3BzLnJvb3QteDEubGV0c2Vu\n" \
  "Y3J5cHQub3JnMDwGA1UdHwQ1MDMwMaAvoC2GK2h0dHA6Ly9jcmwuaWRlbnRydXN0\n" \
  "LmNvbS9EU1RST09UQ0FYM0NSTC5jcmwwHQYDVR0OBBYEFBQusxe3WFbLrlAJQOYf\n" \
  "r52LFMLGMB0GA1UdJQQWMBQGCCsGAQUFBwMBBggrBgEFBQcDAjANBgkqhkiG9w0B\n" \
  "AQsFAAOCAQEA2UzgyfWEiDcx27sT4rP8i2tiEmxYt0l+PAK3qB8oYevO4C5z70kH\n" \
  "ejWEHx2taPDY/laBL21/WKZuNTYQHHPD5b1tXgHXbnL7KqC401dk5VvCadTQsvd8\n" \
  "S8MXjohyc9z9/G2948kLjmE6Flh9dDYrVYA9x2O+hEPGOaEOa1eePynBgPayvUfL\n" \
  "qjBstzLhWVQLGAkXXmNs+5ZnPBxzDJOLxhF2JIbeQAcH5H0tZrUlo5ZYyOqA7s9p\n" \
  "O5b85o3AM/OJ+CktFBQtfvBhcJVd9wvlwPsk+uyOy2HI7mNxKKgsBTt375teA2Tw\n" \
  "UdHkhVNcsAKX1H7GNNLOEADksd86wuoXvg==\n" \
  "-----END CERTIFICATE-----\n;";
#endif

enum {
  Q_EVENT_BMA_INT,
  Q_EVENT_AXP_INT,
  Q_EVENT_RTC_INT,
};
#define WATCH_FLAG_SLEEP_MODE   _BV(1)
#define WATCH_FLAG_SLEEP_EXIT   _BV(2)
#define WATCH_FLAG_BMA_IRQ      _BV(3)
#define WATCH_FLAG_AXP_IRQ      _BV(4)
#define WATCH_FLAG_RTC_IRQ      _BV(5)

#ifdef USE_BACKGROUND
LV_IMG_DECLARE(background);
#endif

TTGOClass *ttgo = nullptr;
#ifdef LILYGO_WATCH_2020_V22
Air530* gps = nullptr;
#endif
lv_obj_t *screen = nullptr;
lv_obj_t *appContainer = nullptr;
lv_style_t menuStyle;
lv_obj_t* appLabel1;
bool notif_shake = false;
#ifdef ENABLE_STATUSBAR
lv_style_t barStyle;
int g_power = 0;
bool g_charge = false;
char g_date[6], g_time[6];
unsigned long time_millis = 0;
#endif
#ifdef ENABLE_ALARM
AudioPlayer* __player;
#endif

#ifdef ENABLE_BLUETOOTH
#include "notif.h"
lv_obj_t* btIcon = NULL;

void onBtNotification();
void onBtConnect();
void onBtDisconnect();

Notification bt_notif(onBtConnect, onBtDisconnect, onBtNotification);

void onBtConnect() {
  if (screen == NULL) return;
  btIcon = lv_img_create(screen, NULL);
  lv_img_set_src(btIcon, LV_SYMBOL_BLUETOOTH);
  lv_obj_align(btIcon, NULL, LV_ALIGN_IN_TOP_LEFT, 0, 0);
  lv_obj_set_style_local_image_recolor(btIcon, LV_OBJ_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);
}

void onBtDisconnect() {
  LV_OBJ_DEL(btIcon);
}

void onBtNotification () {
  bt_notif.showNotifications();
#ifndef LILYGO_WATCH_2020_V2
#ifdef ENABLE_MOTOR
  if (notif_shake) {
    ttgo->shake();
  }
#endif
#endif
  serout(F("on notification"));
  int p = 0;
  while (p == 0) {
    p = ttgo->power->getBattPercentage();
    delay(10);
  }
  char buf[21];
  sprintf_P(buf, PSTR("version:%s"), FIRMWARE_VERSION);
  bt_notif.sendNotification(buf);
  sprintf_P(buf, PSTR("battery:%d"), p);
  bt_notif.sendNotification(buf);
}
#endif

#include "Apps.h"
int appIndex = 0;
int appLength = sizeof(apps) / sizeof(apps[0]);
int lastClock = -1;
int bl_level = 255;
bool menuActive = false;
// unsigned long alarm_millis = 0;
uint32_t freq;

QueueHandle_t g_event_queue_handle = NULL;
bool lenergy = false;
bool rtcIrq = false;
void do_sleep();

#define ISR_FUNC(Q_EVENT_INT) [] { \
    BaseType_t xHigherPriorityTaskWoken = pdFALSE; \
    uint8_t data = Q_EVENT_INT; \
    xQueueSendFromISR(g_event_queue_handle, &data, &xHigherPriorityTaskWoken); \
    if (xHigherPriorityTaskWoken) \
    { \
      portYIELD_FROM_ISR (); \
    } \
  }

void log(const char* buf) {
#ifdef ENABLE_LOG
  ttgo->disableTouchIRQ();
  File logf = SPIFFS.open("/log.txt", "a");
  if (logf) {
    time_t now;
    struct tm info;
    time (&now);
    localtime_r (&now, &info);
    char timeStringBuff[20];
    strftime(timeStringBuff, sizeof(timeStringBuff), "%F %T", &info);
    logf.printf("%s:%s\n", timeStringBuff, buf);
    logf.close();
  }
  ttgo->enableTouchIRQ();
#endif
}

void print_mem() {
  seroutf(PSTR("%d\r\n"), esp_get_free_heap_size());
}

void showApp(int i) {
  if (i < 0) return;
  if (apps[i].id != nullptr && apps[i].load != nullptr) {
    ttgo->disableTouchIRQ();
    char buf[25];
    sprintf_P(buf, PSTR("/apps/%s.json"), apps[i].id);
    if (SPIFFS.exists(buf)) {
      File configFile = SPIFFS.open(buf, "r");
      if (configFile) {
        DynamicJsonDocument doc(512);
        DeserializationError error = deserializeJson(doc, configFile);
        if (!error) {
          apps[i].load(doc);
        }
        configFile.close();
      }
    }
    ttgo->enableTouchIRQ();
  }
  apps[i].show();
}

void hideApp(int i) {
  if (i < 0) return;
  if (apps[i].id != nullptr && apps[i].save != nullptr) {
    ttgo->disableTouchIRQ();
    DynamicJsonDocument doc(512);
    apps[i].save(doc);
    char buf[25];
    sprintf_P(buf, PSTR("/apps/%s.json"), apps[i].id);
    File configFile = SPIFFS.open(buf, "w");
    if (configFile) {
      serializeJson(doc, configFile);
      configFile.close();
    }
    ttgo->enableTouchIRQ();
  }
  apps[i].hide();
  if (i == 0) {
    readSettings();
  }
}

void loopApp(int i) {
  if (i < 0) return;
  if (apps[i].loop != nullptr) {
    apps[i].loop();
  }
}

void eventApp(int i, WATCH_EVENT event) {
  if (i < 0) return;
  if (apps[i].event != nullptr) {
    apps[i].event(event);
  }
}

void setup() {
  // esp_core_dump_init();
  // esp_core_dump_to_flash();

  Serial.begin(115200);
  serout(FIRMWARE_VERSION);

#ifdef ENABLE_BLUETOOTH
  bt_notif.setupBLE();
#endif

  g_event_queue_handle = xQueueCreate(20, sizeof(uint8_t));

  ttgo = TTGOClass::getWatch();
  ttgo->begin();

  serout(F("Turn on the IRQ used"));
  // Turn on the IRQ used
  ttgo->power->adc1Enable(AXP202_BATT_VOL_ADC1 | AXP202_BATT_CUR_ADC1 | AXP202_VBUS_VOL_ADC1 | AXP202_VBUS_CUR_ADC1, AXP202_ON);
  ttgo->power->enableIRQ(AXP202_PEK_SHORTPRESS_IRQ | AXP202_VBUS_REMOVED_IRQ | AXP202_VBUS_CONNECT_IRQ | AXP202_CHARGING_FINISHED_IRQ | AXP202_CHARGING_IRQ, AXP202_ON);
  ttgo->power->clearIRQ();

  serout(F("Turn off unused power"));
  // Turn off unused power
  ttgo->power->setPowerOutPut(AXP202_EXTEN, AXP202_OFF);
  ttgo->power->setPowerOutPut(AXP202_DCDC2, AXP202_OFF);
  ttgo->power->setPowerOutPut(AXP202_LDO3, AXP202_OFF);
  ttgo->power->setPowerOutPut(AXP202_LDO4, AXP202_OFF);

  serout(F("lvgl_begin"));
  ttgo->lvgl_begin();
#ifdef LILYGO_WATCH_2020_V22
  ttgo->trunOnGPS();
  gps = ttgo->gps_begin();
  gps->setNormalMode();
  delay(500);
#endif
#ifndef LILYGO_WATCH_2020_V2
#ifdef ENABLE_MOTOR
  serout(F("ttgo->motor_begin"));
  ttgo->motor_begin();
#endif
#ifdef ENABLE_AUDIO
  serout(F("ttgo->enableLDO3"));
  ttgo->enableLDO3();
#endif
#endif
  serout(F("ttgo->bma->begin"));
  ttgo->bma->begin();
  ttgo->bma->attachInterrupt();
#ifdef ENABLE_ACCEL
  serout(F("Accel parameter structure"));
  // Accel parameter structure
  Acfg cfg;
  /*!
      Output data rate in Hz, Optional parameters:
          - BMA4_OUTPUT_DATA_RATE_0_78HZ
          - BMA4_OUTPUT_DATA_RATE_1_56HZ
          - BMA4_OUTPUT_DATA_RATE_3_12HZ
          - BMA4_OUTPUT_DATA_RATE_6_25HZ
          - BMA4_OUTPUT_DATA_RATE_12_5HZ
          - BMA4_OUTPUT_DATA_RATE_25HZ
          - BMA4_OUTPUT_DATA_RATE_50HZ
          - BMA4_OUTPUT_DATA_RATE_100HZ
          - BMA4_OUTPUT_DATA_RATE_200HZ
          - BMA4_OUTPUT_DATA_RATE_400HZ
          - BMA4_OUTPUT_DATA_RATE_800HZ
          - BMA4_OUTPUT_DATA_RATE_1600HZ
  */
  cfg.odr = BMA4_OUTPUT_DATA_RATE_100HZ;
  /*!
      G-range, Optional parameters:
          - BMA4_ACCEL_RANGE_2G
          - BMA4_ACCEL_RANGE_4G
          - BMA4_ACCEL_RANGE_8G
          - BMA4_ACCEL_RANGE_16G
  */
  cfg.range = BMA4_ACCEL_RANGE_2G;
  /*!
      Bandwidth parameter, determines filter configuration, Optional parameters:
          - BMA4_ACCEL_OSR4_AVG1
          - BMA4_ACCEL_OSR2_AVG2
          - BMA4_ACCEL_NORMAL_AVG4
          - BMA4_ACCEL_CIC_AVG8
          - BMA4_ACCEL_RES_AVG16
          - BMA4_ACCEL_RES_AVG32
          - BMA4_ACCEL_RES_AVG64
          - BMA4_ACCEL_RES_AVG128
  */
  cfg.bandwidth = BMA4_ACCEL_NORMAL_AVG4;

  /*! Filter performance mode , Optional parameters:
      - BMA4_CIC_AVG_MODE
      - BMA4_CONTINUOUS_MODE
  */
  cfg.perf_mode = BMA4_CONTINUOUS_MODE;

  // Configure the BMA423 accelerometer
  ttgo->bma->accelConfig(cfg);

  // Enable BMA423 accelerometer
  // Warning : Need to use steps, you must first enable the accelerometer
  ttgo->bma->enableAccel();
#endif

  serout(F("BMA423_INT1"));
  pinMode(BMA423_INT1, INPUT);
  attachInterrupt(BMA423_INT1, ISR_FUNC(Q_EVENT_BMA_INT), RISING);

  serout(F("AXP202_INT"));
  pinMode(AXP202_INT, INPUT_PULLUP);
  attachInterrupt(AXP202_INT, ISR_FUNC(Q_EVENT_AXP_INT), FALLING);

#ifdef ENABLE_ACCEL
  serout(F("BMA423_STEP_CNTR"));
  ttgo->bma->enableFeature(BMA423_STEP_CNTR, true);
  // ttgo->bma->enableFeature(BMA423_TILT, true);
  serout(F("BMA423_WAKEUP"));
  ttgo->bma->enableFeature(BMA423_WAKEUP, true);
  serout(F("enableStepCountInterrupt"));
  ttgo->bma->enableStepCountInterrupt();
  // ttgo->bma->enableTiltInterrupt();
  serout(F("enableWakeupInterrupt"));
  ttgo->bma->enableWakeupInterrupt();
#endif

  serout(F("rtc->check"));
  ttgo->rtc->check();
  ttgo->rtc->syncToSystem();
#ifdef ENABLE_ALARM
  serout(F("RTC_INT_PIN"));
  pinMode(RTC_INT_PIN, INPUT_PULLUP);
  attachInterrupt(RTC_INT_PIN, ISR_FUNC(Q_EVENT_RTC_INT), FALLING);
  __player = new AudioPlayer("/alarm.mp3");
#else
  serout(F("disableAlarm"));
  ttgo->rtc->disableAlarm();
#endif

  serout(F("get screen"));
  screen = lv_scr_act();

  serout(F("menuStyle"));
  lv_style_init(&menuStyle);
  lv_style_set_radius(&menuStyle, LV_OBJ_PART_MAIN, 0);
  lv_style_set_bg_color(&menuStyle, LV_OBJ_PART_MAIN, LV_COLOR_GRAY);
  lv_style_set_bg_opa(&menuStyle, LV_OBJ_PART_MAIN, LV_OPA_0);
  lv_style_set_border_width(&menuStyle, LV_OBJ_PART_MAIN, 0);
  lv_style_set_text_color(&menuStyle, LV_OBJ_PART_MAIN, LV_COLOR_MAIN);
  lv_style_set_image_recolor(&menuStyle, LV_OBJ_PART_MAIN, LV_COLOR_MAIN);
  lv_style_set_pad_top(&menuStyle, LV_OBJ_PART_MAIN, 0);
  lv_style_set_pad_bottom(&menuStyle, LV_OBJ_PART_MAIN, 0);
  lv_style_set_pad_left(&menuStyle, LV_OBJ_PART_MAIN, 0);
  lv_style_set_pad_right(&menuStyle, LV_OBJ_PART_MAIN, 0);
  lv_style_set_pad_inner(&menuStyle, LV_OBJ_PART_MAIN, 0);

#ifdef ENABLE_STATUSBAR
  lv_style_init(&barStyle);
  lv_style_copy(&barStyle, &menuStyle);
  lv_style_set_bg_opa(&barStyle, LV_OBJ_PART_MAIN, LV_OPA_20);
#endif
  
#ifdef USE_BACKGROUND
  serout(F("background"));
  lv_obj_t *img_bin = lv_img_create(screen, NULL);  /*Create an image object*/
  lv_img_set_src(img_bin, &background);
  lv_obj_align(img_bin, NULL, LV_ALIGN_CENTER, 0, 0);
#endif

  serout(F("app label"));
  appLabel1 = lv_label_create(screen, NULL);
  lv_label_set_long_mode(appLabel1, LV_LABEL_LONG_BREAK);
  lv_obj_set_size(appLabel1, LV_HOR_RES, LV_VER_RES);
  lv_label_set_align(appLabel1, LV_LABEL_ALIGN_CENTER);
  lv_obj_set_hidden(appLabel1, true);

#ifdef BL_LEVEL
  seroutf(PSTR("bl->adjust %d\r\n"), BL_LEVEL);
  ttgo->bl->adjust(BL_LEVEL);
#endif
  serout(F("ttgo->openBL"));
  ttgo->openBL();
  serout(F("ttgo->disableTouchIRQ"));
  ttgo->disableTouchIRQ();
  serout(F("SPIFFS.begin"));
  if (!SPIFFS.begin(true)) {
    printInfo("ERROR SPIFFS", true);
    while (true);
  }

#ifdef ENABLE_FORMAT
  if (!SPIFFS.format()) {
    printInfo("ERROR FORMAT", true);
    while (true);
  }
#endif
  // if (!SPIFFS.exists("/apps/")) {
  //   if (SPIFFS.mkdir("/apps/")) {
  //     serout(F("Dir created"));
  //   } else {
  //     serout(F("mkdir failed"));
  //   }
  // }
  serout(F("readSettings"));
  readSettings();
  ttgo->enableTouchIRQ();

  appIndex = lastClock;
  if (appIndex != -1) {
    serout(F("showApp"));
    showApp(appIndex);
  } else {
    serout(F("showAppList"));
    showAppList();
  }
  lv_disp_trig_activity(NULL);
  if (bl_level > 0) {
    seroutf(PSTR("bl_level 2 = %d\r\n"), bl_level);
    ttgo->bl->adjust(bl_level);
  } else {
#ifdef BL_LEVEL
    seroutf(PSTR("bl_level 3 = %d\r\n"), BL_LEVEL);
    ttgo->bl->adjust(BL_LEVEL);
#endif
  }
  serout(F("end setup"));
}

void printTime() {
  serout(F("SET TIME"));
  time_t tnow = time(nullptr);
  serout(ctime(&tnow));
  char buff[20];
  strftime(buff, 20, "%Y-%m-%d %H:%M:%S", localtime(&tnow));
}

#ifdef ENABLE_STATUSBAR
void getTime() {
  time_t tnow = time(nullptr);
//  strftime(g_datetime, 20, "%H:%M %Y-%m-%d", localtime(&tnow));
  strftime(g_date, 6, "%m/%d", localtime(&tnow));
  strftime(g_time, 6, "%H:%M", localtime(&tnow));
  g_power = ttgo->power->getBattPercentage();
  g_charge = ttgo->power->isChargeing();
  time_millis = millis();
}
#endif

void loop() {
#ifdef ENABLE_STATUSBAR
  if (time_millis == 0 || millis() - time_millis > 1000) {
    getTime();
  }
#endif
  bool  rlst;
  uint8_t data;
  if (xQueueReceive(g_event_queue_handle, &data, 5 / portTICK_RATE_MS) == pdPASS) {
    switch (data) {
      case Q_EVENT_BMA_INT:
        serout(F("Q_EVENT_BMA_IRQ"));
        do {
          rlst =  ttgo->bma->readInterrupt();
        } while (!rlst);
#ifdef ENABLE_ACCEL
        if (ttgo->bma->isStepCounter()) {
          eventApp(appIndex, EVENT_STEP_COUNTER);
        }
        // if (ttgo->bma->isTilt()) {
        //   showAppList();
        // }
        if (ttgo->bma->isDoubleClick()) {
          if (lenergy) {
            // do_sleep();
          } else {
            showAppList();
          }
        }
#endif
        break;

      case Q_EVENT_AXP_INT:
        serout(F("Q_EVENT_AXP_INT"));
        ttgo->power->readIRQ();
        if (ttgo->power->isVbusPlugInIRQ()) {
          serout(F("Event Plug"));
          eventApp(appIndex, EVENT_USB_PLUG);
          eventApp(appIndex, EVENT_BATT_CHARGE_START);
        }
        if (ttgo->power->isVbusRemoveIRQ()) {
          serout(F("Event UnPlug"));
          eventApp(appIndex, EVENT_USB_UNPLUG);
        }
        if (ttgo->power->isChargingDoneIRQ()) {
          serout(F("Event Charge Done"));
          eventApp(appIndex, EVENT_BATT_CHARGE_STOP);
        }
        if (ttgo->power->isPEKShortPressIRQ()) {
          serout(F("Event Press"));
          if (lenergy) {
            do_sleep();
          } else {
            showAppList();
          }
          serout(F("clearIRQ"));
          ttgo->power->clearIRQ();
#ifdef ENABLE_ALARM
          if (rtcIrq) {
            serout(F("alarm stop"));
            __player->stop();
          }
#endif
          rtcIrq = false;
          return;
        }
        ttgo->power->clearIRQ();
        break;

#ifdef ENABLE_ALARM
      case Q_EVENT_RTC_INT:
        serout(F("Q_EVENT_RTC_IRQ"));
        //        detachInterrupt(RTC_INT_PIN);
        ttgo->rtc->resetAlarm();
        rtcIrq = true;
        break;
#endif

      default:
        break;
    }
  }

#ifndef LILYGO_WATCH_2020_V2
#ifdef ENABLE_ALARM
  if (rtcIrq) {
    if (!__player->isPlaying()) {
      __player->play();
    }
    __player->loop();
#ifdef ENABLE_MOTOR_22
    if (millis() - alarm_millis > 1000) {
      ttgo->shake();
      alarm_millis = millis();
    }
#endif
  }
#endif
#endif

#ifdef DEV_MODE
  lv_task_handler();
  if (menuActive) return;
  if (appIndex != -1) {
    loopApp(appIndex);
  }
#else

#ifdef ENABLE_BLUETOOTH
  bool bluetooth_active = (bt_notif.hasNotifications() || bt_notif.isConnected());
#else
  bool bluetooth_active = false;
#endif

  if (lv_disp_get_inactive_time(NULL) < DEFAULT_SCREEN_TIMEOUT || rtcIrq || bluetooth_active) {
    lv_task_handler();
    if (menuActive) return;
    if (appIndex != -1) {
      if (!apps[appIndex].sleep) {
        lv_disp_trig_activity(NULL);
      }
      loopApp(appIndex);
    }
  }
  else {
    do_sleep();
  }
#endif
}

void do_sleep() {
  serout(F("do_sleep"));
  print_mem();

  if (ttgo->bl->isOn()) {
    serout(F("01"));
    ttgo->closeBL();
    serout(F("02"));
    ttgo->stopLvglTick();
    serout(F("03"));
    ttgo->disableTouchIRQ();
    serout(F("03a"));
    ttgo->bma->enableStepCountInterrupt(false);
    serout(F("04"));
    ttgo->displaySleep();
    serout(F("05"));
    if (WiFi.isConnected()) WiFi.disconnect();
    serout(F("06"));
    WiFi.mode(WIFI_OFF);
    serout(F("07"));
    log("sleeping");
    // ttgo->power->clearIRQ();
    lenergy = true;
    serout(F("08"));
    saveSettings();
    serout(F("09"));
    if (!menuActive) {
      serout(F("10"));
      if (appIndex != -1) {
        serout(F("11"));
        hideApp(appIndex);
      }
    }
    else {
      serout(F("12"));
      if (lastClock != -1) {
        serout(F("13"));
        lv_obj_del(appContainer);
        menuActive = false;
      }
    }
#ifdef LILYGO_WATCH_2020_V22
    //gps->sleep();
    gps->setTrackingMode();
#endif
    serout(F("14"));
    lv_refr_now(NULL);
    serout(F("15"));
    gpio_wakeup_enable ((gpio_num_t)AXP202_INT, GPIO_INTR_LOW_LEVEL);
    serout(F("16"));
#ifdef ENABLE_ACCEL
    gpio_wakeup_enable ((gpio_num_t)BMA423_INT1, GPIO_INTR_HIGH_LEVEL);
    serout(F("17"));
#endif
#ifdef ENABLE_ALARM
    gpio_wakeup_enable ((gpio_num_t)RTC_INT_PIN, GPIO_INTR_LOW_LEVEL);
    serout(F("18"));
#endif
    esp_sleep_enable_gpio_wakeup ();
    serout(F("19"));
    // freq = getCpuFrequencyMhz();
    // serout(F("20"));
    // setCpuFrequencyMhz(CPU_FREQ_MIN);
    serout(F("21"));
    //delay(100);
    esp_err_t ret = esp_light_sleep_start();
    serout(F("sleeping!!"));
    yield();     // <== NEW!
    if (ret != ESP_OK) {
      seroutf(PSTR("light_sleep error: %d\r\n"), ret); // likely WiFi wasn't off
    } else {
      serout(F("light sleep OK"));
    }
  }
  else {
    serout(F("waking up"));
    serout(F("23"));
    // setCpuFrequencyMhz(freq);
    serout(F("24"));
    ttgo->displayWakeup();
    serout(F("25"));
    ttgo->rtc->syncToSystem();
    serout(F("26"));
    // readSettings();
    serout(F("27"));
    appIndex = lastClock;
    serout(F("28"));
    if (appIndex != -1) {
      serout(F("29"));
      showApp(appIndex);
    }
    serout(F("30"));
    ttgo->bl->adjust(bl_level);
    ttgo->openBL();
    serout(F("31"));
    ttgo->startLvglTick();
    serout(F("32"));
    lv_disp_trig_activity(NULL);
    // lv_refr_now(NULL);
    serout(F("32a"));
    ttgo->enableTouchIRQ();
    serout(F("33"));
    ttgo->bma->enableStepCountInterrupt(true);
#ifdef LILYGO_WATCH_2020_V22
    gps->wakeup();
    delay(2000);
    /*
        AIR530_HOT_START
        AIR530_WARM_START
        AIR530_COLD_START
        Call restart to start the gps module
    */
    gps->restart(AIR530_HOT_START);
    serout(F("34"));
#endif
    serout(F("35"));
    lenergy = false;
  }
}

void icon_handler(lv_obj_t *obj, lv_event_t event) {
  if (event == LV_EVENT_CLICKED) {  //!  Event callback Is in here
    menuActive = false;
    int i = (int)lv_obj_get_user_data(obj);
    //serout(i);
    appIndex = i;
    if (apps[appIndex].clock) {
      lastClock = appIndex;
    }
    lv_obj_del(appContainer);
    lv_refr_now(NULL);
    showApp(i);
  }
}

void showAppList() {
  lv_disp_trig_activity(NULL);

#ifdef ENABLE_BLUETOOTH
  if (bt_notif.hasNotifications()) {
    bt_notif.hideNotifications();
    return;
  }
#endif

  print_mem();

  if (menuActive && lastClock == -1) return;

  if (menuActive) {
    lv_obj_del(appContainer);
    lv_refr_now(NULL);
    menuActive = false;
    appIndex = lastClock;
    if (appIndex != -1) {
      showApp(appIndex);
    }
    return;
  }

  menuActive = true;
  if (appIndex != -1) {
    hideApp(appIndex);
  }

  appContainer = lv_cont_create(screen, NULL);
  lv_obj_set_size(appContainer, LV_HOR_RES, LV_VER_RES);
  lv_obj_align(appContainer, NULL, LV_ALIGN_CENTER, 0, 0);
  lv_obj_add_style(appContainer, LV_OBJ_PART_MAIN, &menuStyle);

#ifdef ENABLE_STATUSBAR
  int barHeight = 16;
  getTime();

  lv_obj_t *appBar = lv_cont_create(appContainer, NULL);
  lv_obj_add_style(appBar, LV_OBJ_PART_MAIN, &barStyle);
  lv_obj_set_size(appBar, LV_HOR_RES, barHeight);
  lv_obj_align(appBar, appContainer, LV_ALIGN_IN_TOP_MID, 0, 0);

  lv_obj_t *labelDate = lv_label_create(appBar, NULL);
  lv_label_set_text(labelDate, g_date);
  lv_obj_align(labelDate, appBar, LV_ALIGN_IN_LEFT_MID, 3, 0);

  lv_obj_t *labelTime = lv_label_create(appBar, NULL);
  lv_label_set_text(labelTime, g_time);
  lv_obj_align(labelTime, appBar, LV_ALIGN_CENTER, 0, 0);

  lv_obj_t *labelBat = lv_label_create(appBar, NULL);
  if (g_charge) {
    lv_label_set_text(labelBat, LV_SYMBOL_CHARGE);
  } else {
    if (g_power >= 90) {
      lv_label_set_text(labelBat, LV_SYMBOL_BATTERY_FULL);
    } else if (g_power >= 75) {
      lv_label_set_text(labelBat, LV_SYMBOL_BATTERY_3);
    } else if (g_power >= 50) {
      lv_label_set_text(labelBat, LV_SYMBOL_BATTERY_2);
    } else if (g_power >= 25) {
      lv_label_set_text(labelBat, LV_SYMBOL_BATTERY_1);
    } else {
      lv_label_set_text(labelBat, LV_SYMBOL_BATTERY_EMPTY);
    }
  }
  lv_obj_align(labelBat, appBar, LV_ALIGN_IN_RIGHT_MID, -3, 0);
#endif

  int n_con = ((appLength - 1) / 4) + 1;

  static lv_obj_t **appIcons = new lv_obj_t *[appLength];
  static lv_point_t *valid_pos = new lv_point_t [n_con];

  for (int i = 0; i < n_con; i++) {
    valid_pos[i].x = i;
    valid_pos[i].y = 0;
  }

  lv_obj_t *appView = lv_tileview_create(appContainer, NULL);
  lv_tileview_set_valid_positions(appView, valid_pos, n_con);
  lv_tileview_set_edge_flash(appView, true);
  // lv_obj_set_size(appView, LV_HOR_RES, LV_VER_RES - barHeight);
  // lv_obj_align(appView, appBar, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);
  lv_obj_align(appView, NULL, LV_ALIGN_CENTER, 0, 0);
  lv_page_set_scrlbar_mode(appView, LV_SCRLBAR_MODE_OFF);
  lv_obj_add_style(appView, LV_OBJ_PART_MAIN, &menuStyle);

  lv_coord_t _w = lv_obj_get_width(appView);
  lv_coord_t _h = lv_obj_get_height(appView);

  static lv_obj_t **appCont = new lv_obj_t *[n_con];;
  for (int i = 0; i < n_con; i++) {
    appCont[i] = lv_cont_create(appView, NULL);
    lv_obj_set_size(appCont[i], _w, _h);
    lv_obj_align(appCont[i], NULL, LV_ALIGN_CENTER, 0, 0);
    lv_obj_add_style(appCont[i], LV_OBJ_PART_MAIN, &menuStyle);
    //lv_cont_set_fit(appCont[i], LV_FIT_TIGHT);
    lv_cont_set_layout(appCont[i], LV_LAYOUT_GRID);
  }

  int j = 0;
  for (int i = 0; i < appLength; i++) {
    if (i > 0 && i % 4 == 0) {
      j == 0 ? lv_obj_align(appCont[j], NULL, LV_ALIGN_CENTER, 0, 0) : lv_obj_align(appCont[j], appCont[j - 1], LV_ALIGN_OUT_RIGHT_MID, 0, 0);
      lv_tileview_add_element(appView, appCont[j]);
      j++;
    }
    appIcons[i] = lv_cont_create(appCont[j], NULL);
    lv_obj_add_style(appIcons[i], LV_OBJ_PART_MAIN, &menuStyle);
    lv_coord_t _w2 = lv_obj_get_width(appCont[j]) / 2;
    lv_coord_t _h2 = lv_obj_get_height(appCont[j]) / 2;
    lv_obj_set_size(appIcons[i], _w2, _h2);
    lv_page_glue_obj(appIcons[i], true);

    lv_obj_t *img = lv_img_create(appIcons[i], NULL);
    lv_obj_set_size(img, 72, 72);
    lv_img_set_src(img, apps[i].icon);
    lv_obj_align(img, appIcons[i], LV_ALIGN_CENTER, 0, 0);

    lv_obj_t *label = lv_label_create(appIcons[i], NULL);
    lv_label_set_text(label, apps[i].name);
    lv_obj_align(label, img, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);

    lv_obj_set_user_data(appIcons[i], (lv_obj_user_data_t)i);
    lv_obj_set_click(appIcons[i], true);
    lv_obj_set_event_cb(appIcons[i], icon_handler);
  }
  j == 0 ? lv_obj_align(appCont[j], NULL, LV_ALIGN_CENTER, 0, 0) : lv_obj_align(appCont[j], appCont[j - 1], LV_ALIGN_OUT_RIGHT_MID, 0, 0);
  lv_tileview_add_element(appView, appCont[j]);

  int n_idx = (appIndex / 4);
  lv_tileview_set_tile_act(appView, n_idx, 0, LV_ANIM_OFF);
}

void saveSettings () {
  DynamicJsonDocument doc(512);
  loadJson(SETTINGS_FILE, doc);
  doc["last_clock"] = lastClock;
  saveJson(SETTINGS_FILE, doc);
  serout(F("saved!!!"));
}

void readSettings () {
  serout(F("readSettings"));
  DynamicJsonDocument doc(512);
  lastClock = -1;
#ifdef BL_LEVEL
  bl_level = BL_LEVEL;
  seroutf(PSTR("def bl_level == %d\r\n"), BL_LEVEL);
#endif
  if (loadJson(SETTINGS_FILE, doc)) return;
  // serializeJson(doc, Serial);
  if (doc.containsKey("last_clock")) {
    lastClock = doc["last_clock"].as<int>();
  }
  if (lastClock >= appLength)
    lastClock = -1;
  if (doc.containsKey("backlight")) {
    serout(F("has backlight"));
    bl_level = doc["backlight"].as<int>();
    seroutf(PSTR("bl_level 1 = %d\r\n"), bl_level);
  }
  if (doc.containsKey("notif_shake")) {
    notif_shake = doc["notif_shake"].as<bool>();
  }
  if (bl_level == 0) {
    serout(F("bl_level == 0"));
    bl_level = 255;
  }
}

void printInfo(const char* text, bool error) {
  lv_obj_set_hidden(appLabel1, false);
  lv_label_set_text(appLabel1, text);
  lv_obj_set_style_local_text_color(appLabel1, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, error ? LV_COLOR_RED : LV_COLOR_MAIN);
  lv_obj_align(appLabel1, NULL, LV_ALIGN_CENTER, 0, 0);
  lv_refr_now(NULL);
  serout(text);
}

void messageBox(const char* text) {
  static const char * btns[] = {"OK", ""};
  lv_obj_t * mbox1 = lv_msgbox_create(screen, NULL);
  lv_msgbox_set_text(mbox1, text);
  lv_msgbox_add_btns(mbox1, btns);
  lv_obj_set_width(mbox1, 120);
  lv_obj_align(mbox1, NULL, LV_ALIGN_CENTER, 0, 0);
  serout(text);
}
