#ifndef ConfigNotif_h
#define ConfigNotif_h

#include "ConfigBase.h"

class ConfigNotif : public ConfigBase {
  public:
    void load(JsonDocument& doc);
    void save(JsonDocument& doc);
    void show();
    void hide();

  private:
    static lv_obj_t* sw1;
    static lv_obj_t* sw_label;
    static bool notif_shake;
    static void event_handler(lv_obj_t * obj, lv_event_t event);
};

#endif
