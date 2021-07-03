#ifndef ConfigUpdater_h
#define ConfigUpdater_h

#include "ConfigBase.h"

class ConfigUpdater : public ConfigBase {
  public:
    void show();
    void hide();

  private:
    static lv_obj_t* container1;
    static lv_obj_t* button1;
    static lv_obj_t* label1;
    static lv_obj_t* arc;
    lv_style_t menuStyle;
    static lv_style_t arcStyle;
    lv_style_t style_btn_red;
    static int progressPercent;
    static int last;
    static int totalAssets;
    static int totalProgress;
    static int totalTotal;
    static int updateStep;
    static void info (const char* text, bool error, bool hide_arc);
    static bool updateApp ();
    static void onProgress(int progress, int total);
    static void event_handler(lv_obj_t * obj, lv_event_t event);
};

#endif
