// MEM OK
#include "MyWatch.h"
#include "ConfigInfo.h"
#include <WiFi.h>

void ConfigInfo::show() {
  loaded = true;
  label = lv_label_create(lv_scr_act(), nullptr);
  lv_obj_set_style_local_text_color(label, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_MAIN);
  lv_obj_align(label, NULL, LV_ALIGN_IN_TOP_LEFT, 10, 10);

  String mac = "F" + WiFi.macAddress();
  mac.replace(":", "");
  int total_bytes = SPIFFS.totalBytes();
  int used_bytes = SPIFFS.usedBytes();
  int perc_used_bytes = 100 * used_bytes / total_bytes;
  int free_bytes = total_bytes - used_bytes;
  int perc_free_bytes = 100 - perc_used_bytes;

  TTGOClass *ttgo = TTGOClass::getWatch();
  int power = ttgo->power->getBattPercentage();
  bool charge = ttgo->power->isChargeing();

  char str_info[350];
  sprintf(str_info,
          "Device ID:\n%s\n\n" \
          "Firmware:\n%s\n\n" \
          "Battery:\n" \
          "Percentage: %d%%\n\n" \
          "Storage:\n" \
          "Total bytes: %d\n" \
          "Used bytes: %d (%d%%)\n" \
          "Free bytes: %d (%d%%)\n\n",
          mac.c_str(), FIRMWARE_VERSION,
          power,
          total_bytes, used_bytes, perc_used_bytes, free_bytes, perc_free_bytes);

  lv_label_set_text(label, str_info);
}

void ConfigInfo::hide() {
  if (loaded) {
    LV_OBJ_DEL(label);
  }
  loaded = false;
}
