// MEM OK
/*
   IMPORTANTE: Modificar archivo C:\Users\mchav\AppData\Local\Arduino15\packages\esp32\hardware\esp32\1.0.4\libraries\HTTPUpdate\src\HTTPUpdate.cpp
   En Unix: /home/mchavez/.arduino15/packages/esp32/hardware/esp32/1.0.4/libraries/HTTPUpdate/src/HTTPUpdate.cpp
   agregar:
   metodo "HTTPUpdate::handleUpdate", agregar despues de "WiFiClient * tcp = http.getStreamPtr();" lo siguiente: "tcp->setTimeout(30000);"
   metodo "HTTPUpdate::runUpdate", agregar despues de "StreamString error;" lo siguiente: "in.setTimeout(30000);"
   Comentar el contenido del "if" en la linea "if(tcp->peek() != 0xE9) {"
*/
#include "MyWatch.h"
#include "ConfigUpdater.h"
#define NO_GLOBAL_HTTPUPDATE
#include <HTTPClient.h>
#include <HTTPUpdate.h>
#include <ArduinoJson.h>
#include "utils.h"

#define MODO_2

#define UPDATE_URL_1 "http://192.168.0.50/tmp/"
#define UPDATE_URL_2 "http://192.168.0.50/twatch/ota.php"
#define UPDATE_URL_3 "http://twatch2020.ngrok.io/ota.php"
#define UPDATE_URL_5 "https://twatchbuilder.com/tmp/"
#ifdef NO_SSL
#define UPDATE_URL   "http://twatchbuilder.com/ota.php"
#define ASSETS_URL   "http://twatchbuilder.com/spiffs.php"
#else
#define UPDATE_URL   "https://twatchbuilder.com/ota.php"
#define ASSETS_URL   "https://twatchbuilder.com/spiffs.php"
#endif

lv_obj_t* ConfigUpdater::container1;
lv_obj_t* ConfigUpdater::button1;
lv_obj_t* ConfigUpdater::label1;
lv_obj_t* ConfigUpdater::arc;
lv_style_t ConfigUpdater::arcStyle;
int ConfigUpdater::progressPercent = 0;
int ConfigUpdater::last = 0;
int ConfigUpdater::totalAssets = 0;
int ConfigUpdater::totalProgress = 0;
int ConfigUpdater::totalTotal = 0;
int ConfigUpdater::updateStep = 0;

void ConfigUpdater::event_handler(lv_obj_t * obj, lv_event_t event) {
  if (event == LV_EVENT_CLICKED) {
    lv_obj_del(button1);

    arc = lv_arc_create(container1, NULL);
    lv_arc_set_bg_angles(arc, 0, 0);
    lv_arc_set_start_angle(arc, 270);
    lv_arc_set_end_angle(arc, 270);
    lv_obj_set_size(arc, 150, 150);
    lv_obj_align(arc, NULL, LV_ALIGN_CENTER, 0, 0);
    lv_obj_add_style(arc, LV_OBJ_PART_MAIN, &arcStyle);
    lv_obj_add_style(arc, LV_ARC_PART_INDIC, &arcStyle);

    label1 = lv_label_create(container1, NULL);
    lv_label_set_long_mode(label1, LV_LABEL_LONG_BREAK);
    lv_obj_set_size(label1, LV_HOR_RES, LV_VER_RES);
    lv_label_set_align(label1, LV_LABEL_ALIGN_CENTER);

    info("Starting WiFi", false, true);
    if (!wifiOn()) {
      info("WiFi error", true, true);
      delay(3000);
      return;
    }
    info("Please be patience, the update process may be very slow and take several minutes to complete", false, true);
    delay(5000);
    bool restart = updateApp ();
    wifiOff();
    if (restart) {
      ESP.restart();
    }
  }
}

void ConfigUpdater::show() {
  loaded = true;
  lv_style_init(&menuStyle);
  lv_style_set_radius(&menuStyle, LV_OBJ_PART_MAIN, 0);
  //lv_style_set_bg_color(&menuStyle, LV_OBJ_PART_MAIN, LV_COLOR_GRAY);
  lv_style_set_bg_opa(&menuStyle, LV_OBJ_PART_MAIN, LV_OPA_0);
  lv_style_set_border_width(&menuStyle, LV_OBJ_PART_MAIN, 0);
  lv_style_set_text_color(&menuStyle, LV_OBJ_PART_MAIN, LV_COLOR_BLACK);
  lv_style_set_image_recolor(&menuStyle, LV_OBJ_PART_MAIN, LV_COLOR_BLACK);

  lv_style_init(&arcStyle);
  lv_style_set_radius(&arcStyle, LV_OBJ_PART_MAIN, 0);
  lv_style_set_bg_opa(&arcStyle, LV_OBJ_PART_MAIN, LV_OPA_0);
  lv_style_set_border_width(&arcStyle, LV_OBJ_PART_MAIN, 0);
  lv_style_set_line_color(&arcStyle, LV_STATE_DEFAULT, LV_COLOR_RED);
  //  lv_style_set_line_width(&arcStyle, LV_STATE_DEFAULT, 1);
  //  lv_style_set_line_rounded(&arcStyle, LV_STATE_DEFAULT, false);

  container1 = lv_cont_create(lv_scr_act(), nullptr);
  lv_obj_set_size(container1, LV_HOR_RES, LV_VER_RES);
  lv_obj_align(container1, NULL, LV_ALIGN_CENTER, 0, 0);
  lv_obj_add_style(container1, LV_OBJ_PART_MAIN, &menuStyle);

  lv_style_init(&style_btn_red);
  lv_style_set_bg_color(&style_btn_red, LV_STATE_DEFAULT, LV_COLOR_RED);
  lv_style_set_bg_grad_color(&style_btn_red, LV_STATE_DEFAULT, LV_COLOR_MAROON);
  lv_style_set_bg_color(&style_btn_red, LV_STATE_PRESSED, LV_COLOR_MAROON);
  lv_style_set_bg_grad_color(&style_btn_red, LV_STATE_PRESSED, LV_COLOR_RED);
  lv_style_set_text_color(&style_btn_red, LV_STATE_DEFAULT, LV_COLOR_WHITE);
  //  lv_style_set_border_color(&style_btn_red, LV_STATE_DEFAULT, LV_COLOR_RED);

  button1 = lv_btn_create(container1, NULL);
  lv_obj_set_event_cb(button1, event_handler);
  lv_obj_align(button1, NULL, LV_ALIGN_CENTER, 0, 0);
  lv_obj_add_style(button1, LV_OBJ_PART_MAIN, &style_btn_red);
  lv_obj_t* label = lv_label_create(button1, NULL);
  lv_label_set_text(label, "Update");
}

void ConfigUpdater::hide() {
  if (loaded) {
    wifiOff();
    lv_style_reset(&menuStyle);
    lv_style_reset(&arcStyle);
    lv_style_reset(&style_btn_red);
    LV_OBJ_DEL(container1);
  }
  loaded = false;
}

bool ConfigUpdater::updateApp () {
  bool restart = false;
  TTGOClass *ttgo = TTGOClass::getWatch();
  DISABLE_INTERRUPTS()

  if (WiFi.status() == WL_CONNECTED) {
#ifdef NO_SSL
    WiFiClient client;
#else
    WiFiClientSecure client;
    client.setCACert(rootCACertificate);
#endif
    client.setTimeout(TIMEOUT_MS / 1000); // timeout argument is defined in seconds for setTimeout
    HTTPUpdate httpUpdate(TIMEOUT_MS);
    httpUpdate.rebootOnUpdate(false);
    Update.onProgress(onProgress);

    String mac = WiFi.macAddress();
    info("Updating...", false, true);

#ifdef ASSETS_URL
    HTTPClient http;
    DeserializationError error;
    DynamicJsonDocument doc(2048);
    http.begin(client, ASSETS_URL);
    http.setTimeout(TIMEOUT_MS);
    http.setUserAgent("ESP32-http-Update");
    http.addHeader("Content-Type", "application/json");             //Specify content-type header
    http.addHeader("X-ESP32-STA-MAC", mac.c_str());
    int httpResponseCode = http.GET();   //Send the actual POST request
    if (httpResponseCode == 200) {
      error = deserializeJson(doc, http.getStream());
      if (error) {
        log_d("deserializeJson() failed: %s\n", error.f_str());
        String err = "ERROR " + String(error.f_str());
        info(err.c_str(), true, true);
        delay(3000);
      }
    }
    else {
      log_d("[HTTP] GET... failed, error: %s\n", http.errorToString(httpResponseCode).c_str());
      String err = "ERROR " + String(httpResponseCode);
      info(err.c_str(), true, true);
      delay(3000);
    }
    http.end();

    totalAssets = 0;
    totalProgress = 0;
    updateStep = 1;
    JsonArray json;
    if (!error) {
      json = doc.as<JsonArray>();
      serializeJson(json, Serial);
      if (json != NULL) {
        for (JsonObject j : json) {
          const char * name = j["file"].as<char *>();
          if (!SPIFFS.exists(name)) {
            totalAssets += j["size"].as<int>();
          }
        }
      }
    }
    totalTotal = totalAssets;
#endif

#ifdef MODO_1
    mac.replace(":", "");
    mac.toUpperCase();
    String url = UPDATE_URL_5;
    url += "F";
    url += mac;
    url += "/build/twatch.bin";
    log_d("%s\n", url);
    t_httpUpdate_return ret = httpUpdate.update(client, url);
#else
    log_d("%s\n", UPDATE_URL);
    t_httpUpdate_return ret = httpUpdate.update(client, UPDATE_URL);
    //t_httpUpdate_return ret = HTTP_UPDATE_OK;
    //t_httpUpdate_return ret = httpUpdate.update("192.168.0.2", 80, "/esp/update/arduino.php", "optional current version string here");
    //t_httpUpdate_return ret = httpUpdate.update(client, "server", 80, "file.bin");
#endif

#ifdef ASSETS_URL
    updateStep = 2;
    if (!error) {
      for (JsonObject j : json) {
        int size = j["size"].as<int>();
        const char* name = j["file"].as<char *>();
        if (!SPIFFS.exists(name)) {
          seroutf(PSTR("Descargando %s\r\n"), name);
          String url = String(ASSETS_URL);
          url += "?id=";
          url += j["id"].as<char*>();
          log_d("%s\n", url.c_str());
          http.begin(client, url.c_str());
          http.setUserAgent("ESP32-http-Update");
          http.addHeader("X-ESP32-STA-MAC", mac.c_str());
          httpResponseCode = http.GET();
          if (httpResponseCode == HTTP_CODE_OK) {
            downloadFile(&http, name, &totalProgress, totalTotal, onProgress);
          } else {
            log_d("[HTTP] GET... failed, error: %s\n", http.errorToString(httpResponseCode).c_str());
            String err = "ERROR " + String(httpResponseCode);
            info(err.c_str(), true, true);
            delay(3000);
          }
          http.end();
        }
      }
    }
#endif

    switch (ret) {
      case HTTP_UPDATE_FAILED:
        log_d("HTTP_UPDATE_FAILED Error (%d): %s\n", httpUpdate.getLastError(), httpUpdate.getLastErrorString().c_str());
        info("Update failed!", true, true);
        break;

      case HTTP_UPDATE_NO_UPDATES:
        log_d("HTTP_UPDATE_NO_UPDATES\n");
        info("No updates found", false, true);
        break;

      case HTTP_UPDATE_OK:
        log_d("HTTP_UPDATE_OK\n");
        info("OK", false, false);
        DynamicJsonDocument doc(512);
        loadJson(SETTINGS_FILE, doc);
        doc["last_clock"] = -1;
        saveJson(SETTINGS_FILE, doc);
        restart = true;
        delay(3000);
        break;
    }
  }
  ENABLE_INTERRUPTS()
  return restart;
}

void ConfigUpdater::onProgress(int progress, int total) {
  if (updateStep == 1) {
    totalProgress = progress;
    totalTotal = total + totalAssets;
  }
  if (totalTotal == 0) return;
  // Serial.printf("%d %d\r\n", progress, totalTotal);
  progressPercent = PERCENT(progress, totalTotal);
  if (last != progressPercent) {// && progressPercent % 10 == 0) {
    char p[5];
    sprintf(p, "%d%%", progressPercent);
    info(p, false, false);
    lv_arc_set_end_angle(arc, progressPercent * 360 / 100 + 270);
    lv_refr_now(NULL);
  }
  last = progressPercent;
}

void ConfigUpdater::info (const char* text, bool error, bool hide_arc) {
  lv_obj_set_hidden(arc, hide_arc);
  lv_label_set_text(label1, text);
  lv_obj_set_style_local_text_color(label1, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, error ? LV_COLOR_RED : LV_COLOR_MAIN);
  lv_obj_align(label1, NULL, LV_ALIGN_CENTER, 0, 0);
  lv_refr_now(NULL);
  log_d("%s\n", text);
}
