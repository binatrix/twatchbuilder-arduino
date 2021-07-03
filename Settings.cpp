#include "MyWatch.h"
#include "ConfigUpdater.h"
#include "ConfigInfo.h"
#include "ConfigWiFi.h"
#include "ConfigTimeSync.h"
#include "ConfigFileman.h"
#include "ConfigLevels.h"
#include "ConfigNotif.h"
#include "utils.h"

lv_style_t settings_style_list;
lv_style_t settings_btn_style;
lv_obj_t* settings_list;
lv_obj_t* btn_updater;
lv_obj_t* btn_info;
lv_obj_t* btn_wifi;
lv_obj_t* btn_time_sync;
lv_obj_t* btn_restart;
lv_obj_t* btn_fileman;
lv_obj_t* btn_levels;
lv_obj_t* btn_notif;
ConfigUpdater* conf_updater = nullptr;
ConfigInfo* conf_info = nullptr;
ConfigWiFi* conf_wifi = nullptr;
ConfigTimeSync* conf_time_sync = nullptr;
ConfigFileman* conf_fileman = nullptr;
ConfigLevels* conf_levels = nullptr;
ConfigNotif* conf_notif = nullptr;
DynamicJsonDocument doc_settings(512);

void settings_btn_event_handler(lv_obj_t * obj, lv_event_t event) {
  if (event == LV_EVENT_CLICKED) {
    bool error = loadJson(SETTINGS_FILE, doc_settings);
    serout(F("SETTINGS LOAD"));
    serializeJson(doc_settings, Serial);
    lv_obj_set_hidden(settings_list, true);
    serout(F("event"));
    if (obj == btn_wifi) {
      conf_wifi = new ConfigWiFi();
      conf_wifi->show();
    }
    if (obj == btn_info) {
      conf_info = new ConfigInfo();
      conf_info->show();
    }
    if (obj == btn_updater) {
      conf_updater = new ConfigUpdater();
      conf_updater->show();
    }
    if (obj == btn_time_sync) {
      conf_time_sync = new ConfigTimeSync();
      conf_time_sync->show();
    }
    if (obj == btn_fileman) {
      conf_fileman = new ConfigFileman();
      conf_fileman->show();
    }
    if (obj == btn_levels) {
      conf_levels = new ConfigLevels();
      if (!error) {
        conf_levels->load(doc_settings);
      }
      conf_levels->show();
    }
    if (obj == btn_notif) {
      conf_notif = new ConfigNotif();
      if (!error) {
        conf_notif->load(doc_settings);
      }
      conf_notif->show();
    }
    if (obj == btn_restart) {
      ESP.restart();
    }
  }
}

lv_obj_t* settings_add_btn(lv_obj_t *list, const void *img_src, const char *txt) {
  lv_obj_t* btn = lv_list_add_btn(list, img_src, txt);
  lv_obj_add_style(btn, LV_OBJ_PART_MAIN, &settings_btn_style);
  lv_btn_set_checkable(btn, false);
  //  lv_obj_set_user_data(btn, (lv_obj_user_data_t)i);
  lv_obj_set_event_cb(btn, settings_btn_event_handler);
  lv_page_glue_obj(btn, true);
  return btn;
}

void Settings_show() {
  //  Serial.println(esp_get_free_heap_size());

  lv_style_init(&settings_style_list);
  lv_style_set_radius(&settings_style_list, LV_OBJ_PART_MAIN, 0);
  lv_style_set_bg_opa(&settings_style_list, LV_OBJ_PART_MAIN, LV_OPA_0);
  lv_style_set_border_width(&settings_style_list, LV_OBJ_PART_MAIN, 0);
  lv_style_set_bg_color(&settings_style_list, LV_STATE_DEFAULT, LV_COLOR_MAIN);
  lv_style_set_text_color(&settings_style_list, LV_OBJ_PART_MAIN, LV_COLOR_MAIN);
  lv_style_set_image_recolor(&settings_style_list, LV_OBJ_PART_MAIN, LV_COLOR_MAIN);

  lv_style_init(&settings_btn_style);
  lv_style_copy(&settings_btn_style, &settings_style_list);
  lv_style_set_bg_opa(&settings_btn_style, LV_OBJ_PART_MAIN, LV_OPA_0);

  settings_list = lv_list_create(lv_scr_act(), NULL);
  lv_list_set_scrollbar_mode(settings_list, LV_SCROLLBAR_MODE_AUTO); //LV_SCROLLBAR_MODE_OFF);
  lv_obj_set_size(settings_list, LV_HOR_RES, LV_VER_RES);
  lv_obj_add_style(settings_list, LV_OBJ_PART_MAIN, &settings_style_list);
  lv_obj_align(settings_list, NULL, LV_ALIGN_CENTER, 0, 0);

  btn_info = settings_add_btn(settings_list, LV_SYMBOL_WARNING, "Information");
  btn_wifi = settings_add_btn(settings_list, LV_SYMBOL_WIFI, "WiFi");
  btn_updater = settings_add_btn(settings_list, LV_SYMBOL_DOWNLOAD, "Updater");
  btn_time_sync = settings_add_btn(settings_list, LV_SYMBOL_REFRESH, "Time");
  btn_fileman = settings_add_btn(settings_list, LV_SYMBOL_SD_CARD, "Files");
  btn_levels = settings_add_btn(settings_list, LV_SYMBOL_LOOP, "Backlight");
  btn_notif = settings_add_btn(settings_list, LV_SYMBOL_BELL, "Notifications");
  btn_restart = settings_add_btn(settings_list, LV_SYMBOL_HOME, "Restart");
}

void Settings_hide() {
  LV_OBJ_DEL(settings_list);
  lv_style_reset(&settings_style_list);
  lv_style_reset(&settings_btn_style);
  bool save = false;
  if (conf_updater != nullptr) {
    conf_updater->hide();
    delete conf_updater;
    conf_updater = nullptr;
  }
  if (conf_info != nullptr) {
    conf_info->hide();
    delete conf_info;
    conf_info = nullptr;
  }
  if (conf_wifi != nullptr) {
    conf_wifi->hide();
    delete conf_wifi;
    conf_wifi = nullptr;
  }
  if (conf_time_sync != nullptr) {
    conf_time_sync->hide();
    delete conf_time_sync;
    conf_time_sync = nullptr;
  }
  if (conf_fileman != nullptr) {
    conf_fileman->hide();
    delete conf_fileman;
    conf_fileman = nullptr;
  }
  if (conf_levels != nullptr) {
    conf_levels->save(doc_settings);
    conf_levels->hide();
    delete conf_levels;
    conf_levels = nullptr;
    save = true;
  }
  if (conf_notif != nullptr) {
    conf_notif->save(doc_settings);
    conf_notif->hide();
    delete conf_notif;
    conf_notif = nullptr;
    save = true;
  }
  if (save) {
    serout(F("SETTINGS SAVE"));
    serializeJson(doc_settings, Serial);
    saveJson(SETTINGS_FILE, doc_settings);
  }
  //  Serial.println(esp_get_free_heap_size());
}

void Settings_loop() {
  if (conf_updater != nullptr) {
    conf_updater->loop();
  }
  if (conf_info != nullptr) {
    conf_info->loop();
  }
  if (conf_wifi != nullptr) {
    conf_wifi->loop();
  }
  if (conf_time_sync != nullptr) {
    conf_time_sync->loop();
  }
  if (conf_fileman != nullptr) {
    conf_fileman->loop();
  }
  if (conf_levels != nullptr) {
    conf_levels->loop();
  }
  if (conf_notif != nullptr) {
    conf_notif->loop();
  }
}
