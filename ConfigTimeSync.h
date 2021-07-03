#ifndef ConfigTimeSync_h
#define ConfigTimeSync_h

#include "ConfigBase.h"

class ConfigTimeSync : public ConfigBase {
  public:
    void show();
    void hide();

  private:
    static lv_obj_t* container1;
    static lv_obj_t* button1;
    static lv_obj_t* label1;
    lv_style_t menuStyle;
    lv_style_t style_btn_red;
    static void info (const char* text, bool error);
    static void event_handler(lv_obj_t * obj, lv_event_t event);
};

#endif
