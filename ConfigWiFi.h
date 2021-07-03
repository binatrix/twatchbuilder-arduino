#ifndef ConfigWiFi_h
#define ConfigWiFi_h

#include "ConfigBase.h"

class ConfigWiFi : public ConfigBase {
    typedef struct {
      char ssid[MAX_SSID_LEN];
      char pass[MAX_PASS_LEN];
      bool selected;
      bool deleted;
    } wifi_data_t;

  public:
    void show();
    void hide();

  private:
    static wifi_data_t* wifi_list;
    static wifi_data_t* scan_list;
    static int selected_wifi;
    static int wifi_list_size;
    static int scan_list_size;
    static lv_obj_t* container1;
    static lv_obj_t* label_button2;
    static lv_obj_t* mbox1;
    static lv_obj_t* textarea;
    static lv_obj_t* keyboard;
    static lv_style_t style_btn_red;
    static lv_style_t style_label;
    static lv_style_t style_list;
    static lv_style_t style_container;
    static void screen1();
    static void screen2();
    static void screen3();
    static void load_wifi_list();
    static void save_wifi_list();
    static void scan();
    static bool check_connection(const char* ssid, const char* pass);
    static void button1_event_handler(lv_obj_t* obj, lv_event_t event);
    static void button2_event_handler(lv_obj_t* obj, lv_event_t event);
    static void list1_event_handler(lv_obj_t* obj, lv_event_t event);
    static void list2_event_handler(lv_obj_t* obj, lv_event_t event);
    static void kb_event_handler(lv_obj_t* kb, lv_event_t event);
    static void dialog_event_handler(lv_obj_t* obj, lv_event_t event);
    static int cmpfunc (const void * a, const void * b);
};

#endif
