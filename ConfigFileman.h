#ifndef ConfigFileman_h
#define ConfigFileman_h

#include "ConfigBase.h"

class ConfigFileman : public ConfigBase {
  public:
    void show();
    void hide();

  private:
    lv_obj_t* list = NULL;
    lv_style_t list_style;
    lv_style_t btn_style;
    int file_count;
    static char** list_files;
    static lv_obj_t* mbox1;
    static lv_obj_t** list_btn;
    void set_btn(lv_obj_t* btn, int i);
    void listDir(fs::FS &fs, const char * dirname, uint8_t levels);
    static int cmpfunc (const void * a, const void * b);
    static void deleteFile(fs::FS &fs, const char * path);
    static void list_event_handler(lv_obj_t* obj, lv_event_t event);
    static void dialog_event_handler(lv_obj_t* obj, lv_event_t event);
};

#endif
