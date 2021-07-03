#include "MyWatch.h"
#include "ConfigNotif.h"

lv_obj_t* ConfigNotif::sw1;
lv_obj_t* ConfigNotif::sw_label;
bool ConfigNotif::notif_shake;

void ConfigNotif::event_handler(lv_obj_t * obj, lv_event_t event) {
  if(event == LV_EVENT_VALUE_CHANGED) {
    notif_shake = lv_switch_get_state(obj);
    printf("State: %s\n", notif_shake ? "On" : "Off");
  }
}

void ConfigNotif::load(JsonDocument& doc) {
  notif_shake = false;
  if (doc.containsKey("notif_shake")) {
    notif_shake = doc["notif_shake"].as<bool>();
  }
}

void ConfigNotif::save(JsonDocument& doc) {
  seroutf(PSTR("CONFIG SAVE: %d\r\n"), notif_shake);
  doc["notif_shake"] = notif_shake;
}

void ConfigNotif::show() {
  loaded = true;
  sw1 = lv_switch_create(lv_scr_act(), NULL);
  lv_obj_align(sw1, NULL, LV_ALIGN_CENTER, 0, 0);
  if (notif_shake)
    lv_switch_on(sw1, LV_ANIM_OFF);
  else
    lv_switch_off(sw1, LV_ANIM_OFF);
  lv_obj_set_event_cb(sw1, event_handler);

  sw_label = lv_label_create(lv_scr_act(), NULL);
  lv_label_set_text(sw_label, "Shake on notification");
  lv_obj_set_auto_realign(sw_label, true);
  lv_obj_set_style_local_text_color(sw_label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_MAIN);
  lv_obj_align(sw_label, sw1, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);
}

void ConfigNotif::hide() {
  if (loaded) {
    LV_OBJ_DEL(sw1);
    LV_OBJ_DEL(sw_label);
  }
  loaded = false;
}
