#include "MyWatch.h"
#include "ConfigLevels.h"

lv_obj_t* ConfigLevels::slider_label;
int ConfigLevels::cur_level;

void ConfigLevels::slider_event_cb(lv_obj_t * slider, lv_event_t event) {
  if (event == LV_EVENT_VALUE_CHANGED) {
    TTGOClass *ttgo = TTGOClass::getWatch();
    static char buf[4]; /* max 3 bytes for number plus 1 null terminating byte */
    cur_level = lv_slider_get_value(slider);
    ttgo->bl->adjust(cur_level);
    snprintf(buf, 4, "%u", cur_level);
    lv_label_set_text(slider_label, buf);
  }
}

void ConfigLevels::save(JsonDocument& doc) {
  seroutf(PSTR("CONFIG SAVE: %d\r\n"), cur_level);
  doc["backlight"] = cur_level;
}

void ConfigLevels::show() {
  TTGOClass *ttgo = TTGOClass::getWatch();
  cur_level = ttgo->bl->getLevel();
  loaded = true;
  slider = lv_slider_create(lv_scr_act(), NULL);
  lv_obj_set_width(slider, LV_HOR_RES - 80);
  lv_obj_align(slider, NULL, LV_ALIGN_CENTER, 0, 0);
  lv_obj_set_event_cb(slider, slider_event_cb);
  lv_slider_set_range(slider, 1, 255);
  lv_slider_set_value(slider, cur_level, LV_ANIM_OFF);

  slider_label = lv_label_create(lv_scr_act(), NULL);
  lv_label_set_text(slider_label, "0");
  lv_obj_set_auto_realign(slider_label, true);
  lv_obj_set_style_local_text_color(slider_label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_MAIN);
  lv_obj_align(slider_label, slider, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);
  char buf[4]; /* max 3 bytes for number plus 1 null terminating byte */
  snprintf(buf, 4, "%u", cur_level);
  lv_label_set_text(slider_label, buf);
}

void ConfigLevels::hide() {
  if (loaded) {
    LV_OBJ_DEL(slider);
    LV_OBJ_DEL(slider_label);
  }
  loaded = false;
}
