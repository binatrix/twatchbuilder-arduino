#ifndef notif_h
#define notif_h

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

#define SERVICE_UUID        "34d927a9-0ac5-4b82-8051-f5a330f1649f"
#define CHARACTERISTIC_UUID "3bb3e1a8-df61-4292-8edd-7c66d53edaae"
#define JSON_SIZE 600

typedef struct {
  char title[150];
  char body[250];
  char app[150];
} notif_info_t;

class Notification {
  public:
    Notification(void (* onBtConnected)(), void (* onBtDisconnected)(), void (* onBtNotification)()) {
      notif_con = 0;
      buffer = NULL;
      onConnected = onBtConnected;
      onDisconnected = onBtDisconnected;
      onNotification = onBtNotification;
    }
    void showNotifications();
    void hideNotifications();
    void setupBLE(void);
    bool hasNotifications();
    bool isConnected();
    void sendNotification(char* msg);
    static void addNotification(const char* str);
    static char* buffer;
    static Ticker ticker1;
    static bool deviceConnected;
    static void (* onNotification)();
    static void (* onConnected)();
    static void (* onDisconnected)();

  private:
    BLEServer* pServer = NULL;
    BLECharacteristic* pCharacteristic = NULL;
    lv_obj_t* container = NULL;
    lv_obj_t **appCont;
    lv_obj_t *appView;
    lv_point_t *valid_pos;
    lv_style_t listStyle, textStyle;
    static notif_info_t* notifArray;
    static int notif_con;
};

#endif
