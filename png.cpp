#include "MyWatch.h"
#include "utils.h"
#include <HTTPClient.h>

#ifdef NO_SSL
#define PNG_URL "http://twatchbuilder.com/download_image.php?u="
#else
#define PNG_URL "https://twatchbuilder.com/download_image.php?u="
#endif

bool loadPNG(const char* file, int w, int h, uint8_t* data, lv_img_dsc_t* img) {
  File f = SPIFFS.open(file, "r");
  if (!f) return false;
  size_t size = f.size();
  FREE(data);
  data = (uint8_t *) ps_malloc(size);
  int bytes = f.readBytes((char *)data, size);
  f.close();
  // Serial.print("Size:");
  // Serial.println(size);
  // Serial.print("Bytes read:");
  // Serial.println(bytes);
  img->header.always_zero = 0;
  img->header.w = w;
  img->header.h = h;
  img->data_size = size;
  img->header.cf = LV_IMG_CF_RAW_ALPHA;
  img->data = data;
  return true;
}

bool downloadPNG(const char* url, const char* filename, int width, int height, uint8_t* data, lv_img_dsc_t* img, int* progress, int total, void (* onProgress)(int progress, int total)) {
  String url2 = F(PNG_URL);
  url2 += url;
  if (width != 0) url2 += "&w=" + String(width);
  if (height != 0) url2 += "&h=" + String(height);
  serout(url2);
#ifndef NO_SSL
    WiFiClientSecure client;
    client.setCACert(rootCACertificate);
#else
    WiFiClient client;
#endif
  client.setTimeout(TIMEOUT_MS / 1000); // timeout argument is defined in seconds for setTimeout
  HTTPClient http;
  String mac = WiFi.macAddress();
  http.begin(client, url2.c_str());
  http.setUserAgent("ESP32-http-Update");
  http.addHeader("Content-Type", "application/json");
  http.addHeader("X-ESP32-STA-MAC", mac.c_str());
  int httpResponseCode = http.GET();
  if (httpResponseCode != HTTP_CODE_OK) {
    seroutf(PSTR("Error on GET json: %d\r\n"), httpResponseCode);
    http.end();
    return false;
  }
  DynamicJsonDocument doc(512);
  DeserializationError error = deserializeJson(doc, http.getStream());
  if (error) {
    seroutf(PSTR("deserializeJson() failed: %s\r\n"), error.f_str());
    return false;
  }
  http.begin(doc["url"]);
  http.addHeader("Content-Type", "image/png");
  httpResponseCode = http.GET();
  if (httpResponseCode != HTTP_CODE_OK) {
    seroutf(PSTR("Error on GET image: %d\r\n"), httpResponseCode);
    http.end();
    return false;
  }
  bool res = downloadFile(&http, filename, progress, total, onProgress);
  http.end();
  if (res == false) return false;
  res = loadPNG(filename, doc["width"], doc["height"], data, img);
  return res;
}

bool downloadPNG(const char* url, const char* filename, int width, int height, uint8_t* data, lv_img_dsc_t* img) {
  return downloadPNG(url, filename, width, height, data, img, nullptr, 0, nullptr);
}
