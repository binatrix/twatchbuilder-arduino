#ifndef Utils_h
#define Utils_h

#include <WiFiMulti.h>
#include <HTTPClient.h>

bool wifiOn();
void wifiOff();
void syncTime();
bool loadJson(const char* file, JsonDocument &json);
void saveJson(const char* file, JsonDocument &json);
bool downloadFile(const char* url, const char* filename, const char* contentType);
bool downloadFile(HTTPClient* http, const char* filename);
bool downloadFile(HTTPClient* http, const char* filename, int* progress, int total, void (* onProgress)(int progress, int total));

#endif
