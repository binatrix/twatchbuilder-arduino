#include "MyWatch.h"
#include "notif.h"
#include <BLE2902.h>

bool Notification::deviceConnected = false;
char* Notification::buffer;
notif_info_t* Notification::notifArray;
int Notification::notif_con;
Ticker Notification::ticker1;
void (* Notification::onNotification)();
void (* Notification::onConnected)();
void (* Notification::onDisconnected)();

/*
  void notif_handler(lv_obj_t *obj, lv_event_t event) {
  if (event == LV_EVENT_VALUE_CHANGED) {
    lv_coord_t y = ((lv_tileview_ext_t *)lv_obj_get_ext_attr(appView))->act_id.y;
    Serial.println(y);
    if (y == notif_con) {
      lv_obj_del(appView);
    }
  }
  if (event == LV_EVENT_LONG_PRESSED) {  //!  Event callback Is in here
    int i = (int)lv_obj_get_user_data(obj);
    LV_OBJ_DEL(appCont[i]);
    notif_con--;
    if (notif_con == 0) {
      lv_obj_del(appView);
    }
  }
  }
*/

class MyCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      std::string rxValue = pCharacteristic->getValue();
      if (rxValue.length() > 0) {
        Notification::ticker1.detach();
        if (Notification::buffer == NULL) {
          Notification::buffer = (char*) malloc(sizeof(char) * 512);
          Notification::buffer[0] = '\0';
        }
        for (int i = 0; i < rxValue.length(); i++) {
          char c = rxValue[i];
          int len = strlen(Notification::buffer);
          //Serial.println(len);
          Notification::buffer[len] = c;
          Notification::buffer[len + 1] = '\0';
          //Serial.println(Notification::buffer);
          if (c == '\r') {
            Serial.printf_P(PSTR("notificando %s\r\n"), Notification::buffer);
            Notification::addNotification(Notification::buffer);
            FREE(Notification::buffer);
          }
        }
      }
    }
};

class MyServerCallback : public BLEServerCallbacks {
    void onConnect(BLEServer *pServer) {
      Serial.println(F("onConnect"));
      Notification::deviceConnected = true;
      if (Notification::onConnected != NULL) {
        Notification::onConnected();
      }
    }

    void onDisconnect(BLEServer *pServer) {
      Serial.println(F("onDisconnect"));
      Notification::deviceConnected = false;
      if (Notification::onDisconnected != NULL) {
        Notification::onDisconnected();
      }
      pServer->startAdvertising();
    }
};

void Notification::addNotification(const char* str) {
  notif_con++;
  if (notif_con == 1) {
    notifArray = (notif_info_t *) malloc(sizeof(notif_info_t) * notif_con);
  } else {
    notifArray = (notif_info_t *) realloc(notifArray, sizeof(notif_info_t) * notif_con);
  }
  int i = notif_con - 1;
  DynamicJsonDocument doc(JSON_SIZE);
  DeserializationError error = deserializeJson(doc, str);
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    strcpy(notifArray[i].title, "Error");
    strcpy(notifArray[i].body, error.c_str());
    strcpy(notifArray[i].app, "error");
  } 
  else {
    strcpy(notifArray[i].title, doc["title"].as<char*>());
    strcpy(notifArray[i].body, doc["body"].as<char*>());
    strcpy(notifArray[i].app, doc["package"].as<char*>());
  }
  ticker1.once_ms(100, onNotification);
}

void Notification::hideNotifications() {
  if (notif_con == 0) return;
  Serial.println(F("hide Notifications"));
  LV_OBJ_DEL(container);
  free(appCont);
  free(valid_pos);
  free(notifArray);
  lv_style_reset(&listStyle);
  lv_style_reset(&textStyle);
  notif_con = 0;
  print_mem();
}

void Notification::showNotifications() {
  print_mem();
  if (notif_con == 0) return;

  if (container == NULL) {
    lv_style_init(&listStyle);
    lv_style_set_radius(&listStyle, LV_OBJ_PART_MAIN, 0);
    lv_style_set_bg_opa(&listStyle, LV_OBJ_PART_MAIN, LV_OPA_0);
    //    lv_style_set_bg_color(&listStyle, LV_OBJ_PART_MAIN, LV_COLOR_BLACK);
    lv_style_set_border_width(&listStyle, LV_OBJ_PART_MAIN, 0);
    lv_style_set_text_color(&listStyle, LV_OBJ_PART_MAIN, LV_COLOR_BLACK);
    lv_style_set_image_recolor(&listStyle, LV_OBJ_PART_MAIN, LV_COLOR_BLACK);

    lv_style_init(&textStyle);
    lv_style_copy(&textStyle, &listStyle);
    lv_style_set_bg_color(&textStyle, LV_OBJ_PART_MAIN, LV_COLOR_BLACK);
    lv_style_set_bg_opa(&textStyle, LV_OBJ_PART_MAIN, LV_OPA_70);
    lv_style_set_text_font(&textStyle, LV_STATE_DEFAULT, &lv_font_montserrat_22);
  }
  else {
    LV_OBJ_DEL(container);
    free(appCont);
    free(valid_pos);
  }

  container = lv_cont_create(lv_scr_act(), NULL);
  lv_obj_set_size(container, LV_HOR_RES, LV_VER_RES);
  lv_obj_align(container, NULL, LV_ALIGN_CENTER, 0, 0);
  lv_obj_add_style(container, LV_OBJ_PART_MAIN, &listStyle);

  appCont = (lv_obj_t **) malloc(sizeof(lv_obj_t *) * notif_con);

  valid_pos = (lv_point_t *) malloc(sizeof(lv_point_t) * notif_con);
  for (int i = 0; i < notif_con; i++) {
    valid_pos[i].x = 0;
    valid_pos[i].y = i;
  }

  appView = lv_tileview_create(container, NULL);
  lv_obj_set_size(appView, LV_HOR_RES, LV_VER_RES);
  lv_tileview_set_valid_positions(appView, valid_pos, notif_con);
  lv_tileview_set_edge_flash(appView, true);
  lv_obj_align(appView, NULL, LV_ALIGN_CENTER, 0, 0);
  lv_obj_add_style(appView, LV_OBJ_PART_MAIN, &listStyle);
  //  lv_obj_set_event_cb(appView, notif_handler);
  //  lv_page_set_scrlbar_mode(appView, LV_SCRLBAR_MODE_OFF);

  lv_coord_t _w = lv_obj_get_width(appView);
  lv_coord_t _h = lv_obj_get_height(appView);

  for (int i = 0; i < notif_con; i++) {
    appCont[i] = lv_cont_create(appView, NULL);
    lv_obj_add_style(appCont[i], LV_OBJ_PART_MAIN, &textStyle);
    lv_obj_set_size(appCont[i], _w, _h);
    lv_obj_set_user_data(appCont[i], (lv_obj_user_data_t)i);
    //    lv_obj_set_click(appCont[i], true);
    //    lv_obj_set_event_cb(appCont[i], notif_handler);

    i == 0 ? lv_obj_align(appCont[i], NULL, LV_ALIGN_CENTER, 0, 0) : lv_obj_align(appCont[i], appCont[i - 1], LV_ALIGN_OUT_BOTTOM_MID, 0, 0);
    lv_tileview_add_element(appView, appCont[i]);

    lv_obj_t* app = lv_label_create(appCont[i], NULL);
    lv_obj_set_pos(app, 10, 10);
    lv_obj_add_style(app, LV_OBJ_PART_MAIN, &textStyle);
    lv_page_glue_obj(app, true);
    lv_obj_set_width(app, LV_HOR_RES);
    lv_label_set_text(app, notifArray[i].app);
    lv_obj_set_style_local_text_color(app, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_SILVER);

    lv_obj_t* title = lv_label_create(appCont[i], NULL);
    lv_obj_align(title, app, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 10);
    //lv_label_set_long_mode(title, LV_LABEL_LONG_CROP);
    lv_obj_add_style(title, LV_OBJ_PART_MAIN, &textStyle);
    lv_page_glue_obj(title, true);
    lv_obj_set_width(title, LV_HOR_RES);
    lv_label_set_text(title, notifArray[i].title);
    lv_obj_set_style_local_text_color(title, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_YELLOW);

    lv_obj_t* body = lv_label_create(appCont[i], NULL);
    lv_obj_align(body, app, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 40);
    lv_label_set_long_mode(body, LV_LABEL_LONG_BREAK);
    lv_obj_add_style(body, LV_OBJ_PART_MAIN, &textStyle);
    lv_page_glue_obj(body, true);
    lv_obj_set_width(body, LV_HOR_RES);
    lv_label_set_text(body, notifArray[i].body);
    lv_obj_set_style_local_text_color(body, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);

    // lv_cont_set_fit(appCont[i], LV_FIT_TIGHT);
    //lv_cont_set_layout(appCont[i], LV_LAYOUT_GRID);
  }

  //  lv_tileview_set_tile_act(appView, 0, 0, LV_ANIM_OFF);
}

void Notification::setupBLE(void) {
  BLEDevice::init("T-WATCH");
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallback);
  BLEService *pService = pServer->createService(SERVICE_UUID);

  pCharacteristic = pService->createCharacteristic(
                      CHARACTERISTIC_UUID,
                      BLECharacteristic::PROPERTY_WRITE_NR  |
                      BLECharacteristic::PROPERTY_NOTIFY
                    );
  pCharacteristic->addDescriptor(new BLE2902());
  pCharacteristic->setCallbacks(new MyCallbacks());
  pService->start();

  BLEAdvertising *pAdvertising = pServer->getAdvertising();
  pAdvertising->start();
}

bool Notification::hasNotifications() {
  return notif_con > 0;
}

bool Notification::isConnected() {
  return deviceConnected;
}

void Notification::sendNotification(char* msg) {
  Serial.printf_P(PSTR("send notif: %s\r\n"), msg);
  pCharacteristic->setValue((std::string)msg);
  pCharacteristic->notify();
}
