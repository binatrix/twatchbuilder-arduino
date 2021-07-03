#include "MyWatch.h"
#include "ConfigWiFi.h"
#include <WiFi.h>
#include <ArduinoJson.h>

#define WIFI_FILE "/wifi.json"
#define DUMMY_PASS_2 "jd3wxBvnLcpn"

ConfigWiFi::wifi_data_t* ConfigWiFi::wifi_list;
ConfigWiFi::wifi_data_t* ConfigWiFi::scan_list;
int ConfigWiFi::wifi_list_size = 0;
int ConfigWiFi::scan_list_size = 0;
int ConfigWiFi::selected_wifi;
lv_obj_t* ConfigWiFi::container1;
lv_obj_t* ConfigWiFi::label_button2;
lv_obj_t* ConfigWiFi::mbox1;
lv_obj_t* ConfigWiFi::textarea;
lv_obj_t* ConfigWiFi::keyboard;
lv_style_t ConfigWiFi::style_label;
lv_style_t ConfigWiFi::style_container;
lv_style_t ConfigWiFi::style_btn_red;
lv_style_t ConfigWiFi::style_list;

void ConfigWiFi::show() {
  loaded = true;

  load_wifi_list();

  lv_style_init(&style_container);
  lv_style_set_radius(&style_container, LV_OBJ_PART_MAIN, 0);
  lv_style_set_bg_opa(&style_container, LV_OBJ_PART_MAIN, LV_OPA_0);
  lv_style_set_border_width(&style_container, LV_OBJ_PART_MAIN, 0);
  lv_style_set_bg_color(&style_container, LV_STATE_DEFAULT, LV_COLOR_BLACK);
  lv_style_set_text_color(&style_container, LV_OBJ_PART_MAIN, LV_COLOR_BLACK);
  lv_style_set_image_recolor(&style_container, LV_OBJ_PART_MAIN, LV_COLOR_BLACK);

  container1 = lv_cont_create(lv_scr_act(), NULL);
  lv_obj_set_size(container1, LV_HOR_RES, LV_VER_RES);
  lv_obj_align(container1, NULL, LV_ALIGN_CENTER, 0, 0);
  lv_obj_add_style(container1, LV_OBJ_PART_MAIN, &style_container);

  lv_style_init(&style_btn_red);
  lv_style_set_bg_color(&style_btn_red, LV_STATE_DEFAULT, LV_COLOR_RED);
  lv_style_set_bg_grad_color(&style_btn_red, LV_STATE_DEFAULT, LV_COLOR_MAROON);
  lv_style_set_bg_color(&style_btn_red, LV_STATE_PRESSED, LV_COLOR_MAROON);
  lv_style_set_bg_grad_color(&style_btn_red, LV_STATE_PRESSED, LV_COLOR_RED);
  lv_style_set_text_color(&style_btn_red, LV_STATE_DEFAULT, LV_COLOR_WHITE);

  lv_style_init(&style_label);
  lv_style_set_radius(&style_label, LV_OBJ_PART_MAIN, 0);
  lv_style_set_bg_opa(&style_label, LV_OBJ_PART_MAIN, LV_OPA_0);
  lv_style_set_border_width(&style_label, LV_OBJ_PART_MAIN, 0);
  lv_style_set_text_color(&style_label, LV_OBJ_PART_MAIN, LV_COLOR_MAIN);
  lv_style_set_image_recolor(&style_label, LV_OBJ_PART_MAIN, LV_COLOR_MAIN);

  lv_style_init(&style_list);
  lv_style_copy(&style_list, &style_label);

  screen1();
}

void ConfigWiFi::hide() {
  if (loaded) {
    save_wifi_list();
    LV_OBJ_DEL(mbox1);
    LV_OBJ_DEL(container1);
    LV_OBJ_DEL(textarea);
    LV_OBJ_DEL(keyboard);
    if (wifi_list_size != 0) {
      free(wifi_list);
      wifi_list_size = 0;
    }
    if (scan_list_size != 0) {
      free(scan_list);
      scan_list_size = 0;
    }
    lv_style_reset(&style_btn_red);
    lv_style_reset(&style_label);
    lv_style_reset(&style_list);
    lv_style_reset(&style_container);
  }
  loaded = false;
}

void ConfigWiFi::screen1() {
  lv_obj_t* container2 = lv_cont_create(container1, NULL);
  lv_obj_set_size(container2, LV_HOR_RES, LV_VER_RES - 80);
  lv_obj_align(container2, NULL, LV_ALIGN_IN_TOP_MID, 0, 0);
  lv_obj_add_style(container2, LV_OBJ_PART_MAIN, &style_label);

  lv_obj_t* label1 = lv_label_create(container2, NULL);
  lv_label_set_text(label1, "NONE");
  for (int i = 0; i < wifi_list_size; i++) {
    if (wifi_list[i].selected) {
      lv_label_set_text(label1, wifi_list[i].ssid);
    }
  }
  lv_label_set_align(label1, LV_LABEL_ALIGN_CENTER);
  lv_obj_add_style(label1, LV_OBJ_PART_MAIN, &style_label);
  lv_obj_align(label1, container2, LV_ALIGN_CENTER, 0, 0);

  lv_obj_t* button1 = lv_btn_create(container1, NULL);
  lv_obj_set_event_cb(button1, button1_event_handler);
  lv_obj_add_style(button1, LV_OBJ_PART_MAIN, &style_btn_red);
  lv_obj_align(button1, container2, LV_ALIGN_OUT_BOTTOM_MID, 0, 15);

  lv_obj_t* label2 = lv_label_create(button1, NULL);
  lv_label_set_text(label2, "Change");
}

void ConfigWiFi::screen2() {
  lv_obj_clean(container1);

  lv_obj_t* list1 = lv_list_create(container1, NULL);
  lv_list_set_scrollbar_mode(list1, LV_SCROLLBAR_MODE_AUTO);
  lv_obj_set_size(list1, LV_HOR_RES, LV_VER_RES - 80);
  lv_obj_add_style(list1, LV_OBJ_PART_MAIN, &style_list);
  lv_obj_align(list1, NULL, LV_ALIGN_IN_TOP_MID, 0, 0);
  for (int i = 0; i < wifi_list_size; i++) {
    if (!wifi_list[i].deleted) {
      bool p = wifi_list[i].selected;
      lv_obj_t* btn = lv_list_add_btn(list1, p ? LV_SYMBOL_OK : LV_SYMBOL_CLOSE, wifi_list[i].ssid);
      lv_obj_add_style(btn, LV_OBJ_PART_MAIN, &style_list);
      lv_btn_set_checkable(btn, false);
      lv_obj_set_user_data(btn, (lv_obj_user_data_t)i);
      lv_obj_set_event_cb(btn, list1_event_handler);
      lv_page_glue_obj(btn, true);
      if (p) selected_wifi = i;
    }
  }
  lv_obj_t* button2 = lv_btn_create(container1, NULL);
  lv_obj_set_event_cb(button2, button2_event_handler);
  lv_obj_add_style(button2, LV_OBJ_PART_MAIN, &style_btn_red);
  lv_obj_align(button2, list1, LV_ALIGN_OUT_BOTTOM_MID, 0, 15);

  label_button2 = lv_label_create(button2, NULL);
  lv_label_set_text(label_button2, "Scan");
}

void ConfigWiFi::screen3() {
  lv_label_set_text(label_button2, "Scanning");
  lv_refr_now(NULL);
  scan();
  LV_OBJ_DEL(label_button2);

  lv_obj_clean(container1);

  lv_obj_t* list1 = lv_list_create(container1, NULL);
  lv_list_set_scrollbar_mode(list1, LV_SCROLLBAR_MODE_AUTO);
  lv_obj_set_size(list1, LV_HOR_RES, LV_VER_RES);
  lv_obj_add_style(list1, LV_OBJ_PART_MAIN, &style_list);
  lv_obj_align(list1, NULL, LV_ALIGN_IN_TOP_MID, 0, 0);
  for (int i = 0; i < scan_list_size; i++) {
    lv_obj_t* btn = lv_list_add_btn(list1, LV_SYMBOL_WIFI, scan_list[i].ssid);
    lv_obj_add_style(btn, LV_OBJ_PART_MAIN, &style_list);
    lv_btn_set_checkable(btn, false);
    lv_obj_set_user_data(btn, (lv_obj_user_data_t)i);
    lv_obj_set_event_cb(btn, list2_event_handler);
    lv_page_glue_obj(btn, true);
  }
}

void ConfigWiFi::button1_event_handler(lv_obj_t* obj, lv_event_t event) {
  if (event == LV_EVENT_CLICKED) {
    screen2();
  }
}

void ConfigWiFi::button2_event_handler(lv_obj_t* obj, lv_event_t event) {
  if (event == LV_EVENT_CLICKED) {
    screen3();
  }
}

void ConfigWiFi::list1_event_handler(lv_obj_t* obj, lv_event_t event) {
  if (event == LV_EVENT_LONG_PRESSED) {
    selected_wifi = (int)lv_obj_get_user_data(obj);
    printf("%d\n", selected_wifi);
    static const char* btns[] = { "Set", "Del", "Close", "" };
    mbox1 = lv_msgbox_create(lv_scr_act(), NULL);
    lv_msgbox_set_text(mbox1, "What do you want to do?");
    lv_msgbox_add_btns(mbox1, btns);
    lv_obj_set_width(mbox1, LV_HOR_RES);
    lv_obj_set_event_cb(mbox1, dialog_event_handler);
    lv_obj_align(mbox1, NULL, LV_ALIGN_CENTER, 0, 0);
  }
}

void ConfigWiFi::list2_event_handler(lv_obj_t* obj, lv_event_t event) {
  if (event == LV_EVENT_CLICKED) {
    selected_wifi = (int)lv_obj_get_user_data(obj);
    for (int i = 0; i < wifi_list_size; i++) {
      if (strcmp(wifi_list[i].ssid, scan_list[selected_wifi].ssid) == 0) { //Ya existe la WiFi
        screen2();
        return;
      }
    }

    lv_obj_clean(container1);

    textarea = lv_textarea_create(lv_scr_act(), NULL);
    lv_cont_set_fit2(textarea, LV_FIT_PARENT, LV_FIT_NONE);
    lv_textarea_set_text(textarea, "");
    lv_textarea_set_placeholder_text(textarea, "WiFi Password");
    lv_textarea_set_one_line(textarea, true);
    lv_textarea_set_cursor_hidden(textarea, false);

    keyboard = lv_keyboard_create(lv_scr_act(), NULL);
    //lv_keyboard_set_cursor_manage(keyboard, true);
    lv_keyboard_set_textarea(keyboard, textarea);
    lv_obj_set_event_cb(keyboard, kb_event_handler);
  }
}

void ConfigWiFi::kb_event_handler(lv_obj_t* kb, lv_event_t event) {
  lv_keyboard_def_event_cb(kb, event);
  if (event != LV_EVENT_VALUE_CHANGED && event != LV_EVENT_LONG_PRESSED_REPEAT) return;
  const char* txt = lv_btnmatrix_get_active_btn_text(kb);
  if (txt == NULL) return;
  if (strcmp(txt, LV_SYMBOL_OK) == 0) {
    lv_keyboard_ext_t* ext = (lv_keyboard_ext_t*)lv_obj_get_ext_attr(kb);
    char ssid[MAX_SSID_LEN], pass[MAX_PASS_LEN];
    strcpy(ssid, scan_list[selected_wifi].ssid);
    strcpy(pass, lv_textarea_get_text(ext->ta));
#ifdef DUMMY_PASS
    strcpy22(pass, DUMMY_PASS);
#endif
    if (check_connection(ssid, pass)) {
      wifi_list_size++;
      if (wifi_list_size == 1)
        wifi_list = (wifi_data_t*)malloc(wifi_list_size * sizeof(wifi_data_t));
      else
        wifi_list = (wifi_data_t*)realloc(wifi_list, wifi_list_size * sizeof(wifi_data_t));
      int i = wifi_list_size - 1;
      strcpy(wifi_list[i].ssid, ssid);
      strcpy(wifi_list[i].pass, pass);
      wifi_list[i].deleted = false;
      wifi_list[i].selected = false;
      LV_OBJ_DEL(keyboard);
      LV_OBJ_DEL(textarea);
      screen2();
    } else {
      //static lv_style_t style_modal;
      //lv_style_init(&style_modal);
      //lv_style_set_bg_color(&style_modal, LV_STATE_DEFAULT, LV_COLOR_BLACK);
      //lv_style_set_bg_opa(&style_modal, LV_OBJ_PART_MAIN, LV_OPA_80);
      //lv_obj_t* obj = lv_obj_create(lv_scr_act(), NULL);
      //lv_obj_reset_style_list(obj, LV_OBJ_PART_MAIN);
      //lv_obj_add_style(obj, LV_OBJ_PART_MAIN, &style_modal);
      //lv_obj_set_pos(obj, 0, 0);
      //lv_obj_set_size(obj, LV_HOR_RES, LV_VER_RES);
      static const char* btns2[] = { "OK", "" };
      lv_obj_t* mbox = lv_msgbox_create(lv_scr_act(), NULL);
      lv_msgbox_add_btns(mbox, btns2);
      lv_msgbox_set_text(mbox, "Password error");
      lv_obj_align(mbox, NULL, LV_ALIGN_CENTER, 0, 0);
    }
    return;
  }
  else if (strcmp(txt, LV_SYMBOL_CLOSE) == 0) {
    LV_OBJ_DEL(keyboard);
    LV_OBJ_DEL(textarea);
    screen2();
    return;
  }
}

void ConfigWiFi::dialog_event_handler(lv_obj_t* obj, lv_event_t event) {
  if (event == LV_EVENT_VALUE_CHANGED) {
    char buf[6];
    strcpy(buf, lv_msgbox_get_active_btn_text(obj));
    if (strcmp(buf, "Set") == 0) {
      for (int i = 0; i < wifi_list_size; i++) {
        wifi_list[i].selected = false;
      }
      wifi_list[selected_wifi].selected = true;
    }
    if (strcmp(buf, "Del") == 0) {
      wifi_list[selected_wifi].deleted = true;
    }
    LV_OBJ_DEL(mbox1);
    screen2();
  }
}

void ConfigWiFi::scan() {
  scan_list_size = 0;
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  int n = WiFi.scanNetworks();

  /*WL_CONNECTED: assigned when connected to a WiFi network;
  WL_NO_SHIELD: assigned when no WiFi shield is present;
  WL_IDLE_STATUS: it is a temporary status assigned when WiFi.begin() is called and remains active until the number of attempts expires (resulting in WL_CONNECT_FAILED) or a connection is established (resulting in WL_CONNECTED);
  WL_NO_SSID_AVAIL: assigned when no SSID are available;
  WL_SCAN_COMPLETED: assigned when the scan networks is completed;
  WL_CONNECT_FAILED: assigned when the connection fails for all the attempts;
  WL_CONNECTION_LOST: assigned when the connection is lost;
  WL_DISCONNECTED: assigned when disconnected from a network;
  */
  switch(WiFi.status()){
    case WL_NO_SHIELD: 
          serout(F("No WiFi shield is present."));
          break;
    case WL_IDLE_STATUS: 
          serout(F("Attempting to connect..."));
          break;
    case WL_NO_SSID_AVAIL: 
          serout(F("No SSID available."));
          break;
    case WL_SCAN_COMPLETED: 
          serout(F("Scan Networks is complete."));
          break;
    case WL_CONNECT_FAILED: 
          serout(F("Connection FAILED."));
          break;
    case WL_CONNECTION_LOST: 
          serout(F("Connection LOST."));
          break;
    case WL_DISCONNECTED: 
          serout(F("Device has been DISCONNECTED from the Network."));
          break;
    default: 
          serout(F("UNKNOWN ERROR"));
          break;
  }

  serout(F("scan done"));
  if (n == 0) {
    serout(F("no networks found"));
  } else {
    seroutf(PSTR("%d networks found\r\n"), n);
    scan_list_size = n;
    scan_list = (wifi_data_t*)malloc(scan_list_size * sizeof(wifi_data_t));
    for (int i = 0; i < n; ++i) {
      //      Serial.print(WiFi.SSID(i));
      //      Serial.print(WiFi.RSSI(i));
      //      Serial.println((WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? " " : "*");
      strcpy(scan_list[i].ssid, WiFi.SSID(i).c_str());
    }
  }
  WiFi.scanDelete();
}

void ConfigWiFi::load_wifi_list() {
  wifi_list_size = 0;
  serout(F("Loading"));
  if (!SPIFFS.exists(WIFI_FILE)) {
    return;
  }
  File configFile = SPIFFS.open(WIFI_FILE, "r");
  if (configFile) {
    serout(F("opened config file"));
    DynamicJsonDocument doc(1024);
    DeserializationError error = deserializeJson(doc, configFile);
    if (error) {
      seroutf(PSTR("deserializeJson() failed: %s\r\n"), error.f_str());
      return;
    }
    JsonArray json = doc.as<JsonArray>();
    serout(F("\nparsed json"));
    //serializeJson(json, Serial);
    wifi_list_size = json.size();
    wifi_list = (wifi_data_t *)malloc(wifi_list_size * sizeof(wifi_data_t));
    int i = 0;
    for (JsonObject j : json) {
      strcpy(wifi_list[i].ssid, j["ssid"]);
      strcpy(wifi_list[i].pass, j["pass"]);
      wifi_list[i].selected = j["selected"].as<bool>();
      wifi_list[i].deleted = false;
      i++;
    }
    if (i > 1) {
      qsort(wifi_list, i, sizeof(wifi_list[0]), cmpfunc);
    }
    configFile.close();
  }
}

void ConfigWiFi::save_wifi_list() {
  serout(F("Saving"));
  DynamicJsonDocument doc(1024);
  for (int i = 0; i < wifi_list_size; i++) {
    if (wifi_list[i].deleted == false) {
      JsonObject obj = doc.createNestedObject();
      obj["ssid"] = wifi_list[i].ssid;
      obj["pass"] = wifi_list[i].pass;
      obj["selected"] = wifi_list[i].selected;
    }
  }
  File configFile = SPIFFS.open(WIFI_FILE, "w");
  if (!configFile) {
    serout(F("failed to open config file for writing"));
    return;
  }
  //serializeJson(doc, Serial);
  serializeJson(doc, configFile);
  configFile.close();
  serout(F("saved!!!"));
}

bool ConfigWiFi::check_connection (const char* ssid, const char* pass) {
  if (WiFi.isConnected()) {
    WiFi.disconnect();
  }
  serout(ssid);
  serout(pass);
  WiFi.begin(ssid, pass);
  unsigned long start_millis = millis();
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    if (millis() - start_millis > 10000) {
      return false;
    }
  }
  WiFi.disconnect();
  return true;
}

int ConfigWiFi::cmpfunc (const void * a, const void * b) {
  wifi_data_t* ia = (wifi_data_t*)a;
  wifi_data_t* ib = (wifi_data_t*)b;
//  Serial.printf("ia: %s ib: %s\r\n", ia->ssid, ib->ssid);
  return strcmp(ia->ssid, ib->ssid);
}
