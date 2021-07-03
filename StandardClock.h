#ifndef StandardClock_h
#define StandardClock_h

enum TIME_RES {
  TIME_RES_NONE,
  TIME_RES_SECOND_UNIT,
  TIME_RES_SECOND_TEN,
  TIME_RES_SECOND,
  TIME_RES_SECOND_BLINK,
  TIME_RES_MINUTE,
  TIME_RES_MINUTE_UNIT,
  TIME_RES_MINUTE_TEN,
  TIME_RES_HOUR,
  TIME_RES_HOUR_UNIT,
  TIME_RES_HOUR_TEN,
  TIME_RES_DAY,
  TIME_RES_DAY_UNIT,
  TIME_RES_DAY_TEN,
  TIME_RES_WEEKDAY,
  TIME_RES_MONTH,
  TIME_RES_MONTH_UNIT,
  TIME_RES_MONTH_TEN,
  TIME_RES_YEAR,
  TIME_RES_YEAR_UNIT,
  TIME_RES_YEAR_TEN,
  TIME_RES_YEAR_HUNDRED,
  TIME_RES_YEAR_THOUSAND,
  TIME_RES_BATTERY,
  TIME_RES_BATTERY_UNIT,
  TIME_RES_BATTERY_TEN,
  TIME_RES_STEPS,
  TIME_RES_STEPS_UNIT,
  TIME_RES_STEPS_TEN,
  TIME_RES_STEPS_HUNDRED,
  TIME_RES_STEPS_THOUSAND,
  TIME_RES_STEPS_TEN_THOUSAND
};

enum ITEM_CLOCK_TYPE {
  TYPE_NONE,
  TYPE_IMAGE,
  TYPE_SPRITE,
  TYPE_TEXT
};

typedef struct {
  lv_obj_t* image;
  const void** images;
  int images_size;
  char format[50];
  int from_low;
  int from_high;
  int to_low;
  int to_high;
  bool scale;
  lv_style_t style;
  lv_align_t align;
  lv_point_t offset;
  TIME_RES resol;
  ITEM_CLOCK_TYPE type;
} standard_clock_hand_t;

class StandardClockSimple {
  public:
    void init(lv_color_t back_color, int n_hands);
    void init(lv_color_t back_color, const lv_img_dsc_t* clock_face, int n_hands);
    void init(lv_color_t back_color, const lv_img_dsc_t* clock_face, int n_hands, void (* post_init)(lv_obj_t*), void (* event_handler)(lv_obj_t* obj, lv_event_t event));
    void init(lv_color_t back_color, const lv_img_dsc_t* clock_face, int n_hands, int n_sprites);
    void init(lv_color_t back_color, const lv_img_dsc_t* clock_face, int n_hands, int n_sprites, void (* post_init)(lv_obj_t*));
    void init(lv_color_t back_color, const lv_img_dsc_t* clock_face, int n_hands, int n_sprites, void (* post_init)(lv_obj_t*), void (* event_handler)(lv_obj_t* obj, lv_event_t event));
    void show();
    void hide();
    void event(WATCH_EVENT event);
    void add_clock_back(const lv_img_dsc_t* image, lv_point_t offset);
    void add_clock_back(const lv_img_dsc_t* image, lv_align_t align, lv_point_t offset);
    void add_clock_hand(TIME_RES res, const lv_img_dsc_t* image, lv_point_t offset);
    void add_clock_hand(TIME_RES res, const lv_img_dsc_t* image, lv_point_t offset, lv_point_t pivot, bool is_shadow);
    void add_clock_hand(TIME_RES res, const lv_img_dsc_t* image, lv_point_t offset, lv_point_t pivot, bool is_shadow, int fromLow, int fromHigh, int toLow, int toHigh);
    void add_clock_text(TIME_RES res, lv_color_t color, lv_align_t align, const char* format, lv_font_t* font, lv_point_t offset);
    void add_clock_sprite(TIME_RES res, const lv_img_dsc_t** images, lv_point_t offset);
    void add_clock_sprite(TIME_RES res, lv_color_t color, lv_align_t align, const void** images, int images_size, lv_point_t offset);
    lv_obj_t* get_screen();
    static void lv_update_task(struct _lv_task_t *data);
    static void update_time();

  private:
    void showBatteryIcon(int i, bool charge, int p);
    void showSteps(int i, int steps);
    static int nthdig(int n, int k);
    static void set_text (int i, struct tm info);
    static standard_clock_hand_t* clock_hands;
    static int num_hands;
    int hand_index = 0;
    lv_obj_t* screen;
    static lv_obj_t* canvas;
    lv_task_t* task;
    lv_color_t* cbuf;
    lv_style_t shadow_style;
    lv_style_t main_style;
    static int last_minute;
};

#endif
