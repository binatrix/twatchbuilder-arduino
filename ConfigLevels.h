#ifndef ConfigLevels_h
#define ConfigLevels_h

#include "ConfigBase.h"

class ConfigLevels : public ConfigBase {
  public:
    void save(JsonDocument& doc);
    void show();
    void hide();

  private:
    static void slider_event_cb(lv_obj_t * slider, lv_event_t event);
    static lv_obj_t* slider_label;
    static int cur_level;
    lv_obj_t* slider;
};

#endif
