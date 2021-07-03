#include "MyWatch.h"
#include "utils.h"
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <ArduinoJson.h>

#define WIFI_FILE "/wifi.json"

//#define WLAN_SSID "VTR-8322701"
//#define WLAN_PASS "jd3wxBvnLcpn"

WiFiMulti wifiMulti;

bool wifiOn() {
  char ssid[MAX_SSID_LEN];
  char pass[MAX_PASS_LEN];
  TTGOClass *ttgo = TTGOClass::getWatch();
  ttgo->disableTouchIRQ();
  if (!SPIFFS.exists(WIFI_FILE)) {
    ttgo->enableTouchIRQ();
    return false;
  }
  File configFile = SPIFFS.open(WIFI_FILE, "r");
  if (configFile) {
    serout(F("opened config file"));
    DynamicJsonDocument doc(1024);
    DeserializationError error = deserializeJson(doc, configFile);
    if (error) {
      seroutf(PSTR("deserializeJson() failed: %s\r\n"), error.f_str());
      ttgo->enableTouchIRQ();
      return false;
    }
    JsonArray json = doc.as<JsonArray>();
    //    serializeJson(json, Serial);
    int i = 0;
    for (JsonObject j : json) {
      if (j["selected"].as<bool>()) {
        strcpy(ssid, j["ssid"]);
        strcpy(pass, j["pass"]);
      }
      i++;
    }
    configFile.close();
  }
  ttgo->enableTouchIRQ();
  serout(ssid);
  serout(pass);
  if (ssid == "") return false;

  wifiOff();
  WiFi.mode(WIFI_STA);
  unsigned long start_millis = millis();
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    if (millis() - start_millis > 10000) {
      return false;
    }
  }
  serout(F("Connected"));
  return true;
}

void wifiOn2() {
  wifiMulti.addAP("VTR-8322701", "jd3wxBvnLcpn");
  //  wifiMulti.addAP("VTR-8322701_EXT", "jd3wxBvnLcpn");
  wifiMulti.addAP("Marcelo", "npcqlhi2018");
  //  print("INICIANDO WIFI", LV_COLOR_GREEN);
  serout(F("Starting Wifi"));
  while (wifiMulti.run() != WL_CONNECTED) { // Wait for the Wi-Fi to connect: scan for Wi-Fi networks, and connect to the strongest of the networks above
    delay(500);
    Serial.print(".");
  }
}

void wifiOff() {
  if (WiFi.status() == WL_CONNECTED) {
    WiFi.disconnect();
  }
}

// void syncTime2 () {
//   char *ntpServer = NTS_SERVER;
//   int gmtOffset_sec = TZ_OFFSET * 3600;
//   Serial.println(gmtOffset_sec);
//   int daylightOffset_sec = DST_OFFSET * 3600;
//   Serial.println(daylightOffset_sec);
//   TTGOClass *ttgo = TTGOClass::getWatch();
//   PCF8563_Class* rtc = ttgo->rtc;;
//   wifiOn();
//   configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
//   struct tm timeinfo;
//   if (!getLocalTime(&timeinfo)) {
//     Serial.println("Failed to obtain time, Restart in 3 seconds");
//     delay(3000);
//   }
//   Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
//   rtc->syncToRtc();
//   rtc->syncToSystem();
//   wifiOff();
// }

void syncTime () {
  WiFiUDP ntpUDP;
  NTPClient timeClient(ntpUDP);
  TTGOClass* ttgo = TTGOClass::getWatch();
  PCF8563_Class* rtc = ttgo->rtc;;
  rtc->check();
  serout(F("time1"));
  wifiOn();
  timeClient.begin();
  timeClient.setTimeOffset((TZ_OFFSET + DST_OFFSET) * 3600);
  while (!timeClient.update()) {
    timeClient.forceUpdate();
  }
  time_t rawtime = timeClient.getEpochTime();
  struct tm * ti;
  ti = localtime (&rawtime);
  uint16_t year = ti->tm_year + 1900;
  uint8_t month = ti->tm_mon + 1;
  uint8_t day = ti->tm_mday;
  uint8_t hours = ti->tm_hour;
  uint8_t minutes = ti->tm_min;
  uint8_t seconds = ti->tm_sec;
  rtc->setDateTime(year, month, day, hours, minutes, seconds);
  serout(F("time2"));
  serout(ti, "%A, %B %d %Y %H:%M:%S");
  rtc->syncToSystem();
  wifiOff();
}

bool loadJson(const char* file, JsonDocument &json) {
  bool ok = false;
  TTGOClass *ttgo = TTGOClass::getWatch();
  ttgo->disableTouchIRQ();
  if (SPIFFS.exists(file)) {
    File configFile = SPIFFS.open(file, "r");
    if (configFile) {
      DeserializationError err = deserializeJson(json, configFile);
      if (err) {
        seroutf(PSTR("deserializeJson() failed: %s\r\n"), err.f_str());
      }
      ok = (err == DeserializationError::Ok);
      configFile.close();
    }
  }
  ttgo->enableTouchIRQ();
  return !ok;
}

void saveJson(const char* file, JsonDocument &json) {
  TTGOClass *ttgo = TTGOClass::getWatch();
  ttgo->disableTouchIRQ();
  File configFile = SPIFFS.open(file, "w");
  if (configFile) {
    serializeJson(json, configFile);
    configFile.close();
  }
  ttgo->enableTouchIRQ();
}

bool downloadFile(const char* url, const char* filename, const char* contentType) {
#ifndef ENABLE_BLUETOOTH
    WiFiClientSecure client;
    client.setCACert(rootCACertificate);
#else
    WiFiClient client;
#endif
  client.setTimeout(TIMEOUT_MS / 1000); // timeout argument is defined in seconds for setTimeout
  HTTPClient http;
  http.begin(client, url);
  http.setUserAgent("T-WATCH-2020");
  if (contentType != NULL) {
    http.addHeader("Content-Type", contentType);
  }
  int httpResponseCode = http.GET();   //Send the actual POST request
  // Serial.printf("Response: %d\r\n", httpResponseCode);
  if (httpResponseCode != HTTP_CODE_OK) {
    http.end();
    seroutf(PSTR("[HTTP] GET... failed, error: %s\r\n"), http.errorToString(httpResponseCode).c_str());
    return false;
  }
  bool res = downloadFile(&http, filename);
  http.end();
  return res;
}

bool downloadFile(HTTPClient* http, const char* filename) {
  return downloadFile(http, filename, nullptr, 0, nullptr);
}

bool downloadFile(HTTPClient* http, const char* filename, int* progress, int total, void (* onProgress)(int progress, int total)) {
  File f = SPIFFS.open(filename, "w");
  if (!f) return false;
  int len = http->getSize();
  int total0 = (total != 0 ? total : len);
  int progress0 = 0;
  if (ISNOTNULL(progress)) {
    progress0 = *progress;
  }
  uint8_t buff[HTTP_TCP_BUFFER_SIZE] = { 0 };
  WiFiClient* stream = http->getStreamPtr();
  stream->setTimeout(TIMEOUT_MS);
  while (http->connected() && (len > 0 || len == -1)) {
    size_t size = stream->available();
    if (size) {
      int c = stream->readBytes(buff, ((size > sizeof(buff)) ? sizeof(buff) : size));
      f.write(buff, c);
      if (len > 0) {
        len -= c;
      }
      progress0 += c;
      if (ISNOTNULL(onProgress)) {
        onProgress(progress0, total0);
      }
    }
    delay(1);
  }
  if (ISNOTNULL(progress)) {
    *progress = progress0;
  }
  // http.writeToStream(&f);
  f.close();
  return true;
}
