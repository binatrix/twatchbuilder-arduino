#include "MyWatch.h"
#include "StandardClock.h"

int StandardClockSimple::last_minute;
standard_clock_hand_t* StandardClockSimple::clock_hands;
int StandardClockSimple::num_hands;
lv_obj_t* StandardClockSimple::canvas = nullptr;

lv_obj_t* StandardClockSimple::get_screen() {
  return screen;
}

void StandardClockSimple::add_clock_sprite(TIME_RES res, const lv_img_dsc_t** images, lv_point_t offset) {
  add_clock_sprite(res, LV_COLOR_TRANSP, LV_ALIGN_CENTER, (const void**)images, 0, offset);
}

void StandardClockSimple::add_clock_sprite(TIME_RES res, lv_color_t color, lv_align_t align, const void** images, int images_size, lv_point_t offset) {
  lv_style_init(&clock_hands[hand_index].style);
  lv_style_set_text_color(&clock_hands[hand_index].style, LV_STATE_DEFAULT, color);
  lv_style_set_image_recolor(&clock_hands[hand_index].style, LV_STATE_DEFAULT, color);
  clock_hands[hand_index].image = lv_img_create (canvas, NULL);
  lv_obj_align (clock_hands[hand_index].image, canvas, align, offset.x, offset.y);
  lv_obj_add_style(clock_hands[hand_index].image, LV_OBJ_PART_MAIN, &clock_hands[hand_index].style);
  clock_hands[hand_index].images = images;
  clock_hands[hand_index].images_size = images_size;
  clock_hands[hand_index].resol = res;
  clock_hands[hand_index].align = align;
  clock_hands[hand_index].offset = offset;
  clock_hands[hand_index].type = TYPE_SPRITE;
  hand_index++;
}

void StandardClockSimple::add_clock_text(TIME_RES res, lv_color_t color, lv_align_t align, const char* format, lv_font_t* font, lv_point_t offset) {
  lv_style_init(&clock_hands[hand_index].style);
  lv_style_set_text_font(&clock_hands[hand_index].style, LV_STATE_DEFAULT, font);
  lv_style_set_text_color(&clock_hands[hand_index].style, LV_STATE_DEFAULT, color);
  lv_style_set_image_recolor(&clock_hands[hand_index].style, LV_STATE_DEFAULT, color);
  clock_hands[hand_index].image = lv_label_create(canvas, NULL);
  lv_obj_align(clock_hands[hand_index].image, NULL, align, offset.x, offset.y);
  lv_obj_add_style(clock_hands[hand_index].image, LV_OBJ_PART_MAIN, &clock_hands[hand_index].style);
  clock_hands[hand_index].resol = res;
  clock_hands[hand_index].align = align;
  clock_hands[hand_index].offset = offset;
  clock_hands[hand_index].type = TYPE_TEXT;
  strcpy(clock_hands[hand_index].format, format);
  hand_index++;
}

void StandardClockSimple::add_clock_back(const lv_img_dsc_t* image, lv_point_t offset) {
  add_clock_back(image, LV_ALIGN_CENTER, offset);
}

void StandardClockSimple::add_clock_back(const lv_img_dsc_t* image, lv_align_t align, lv_point_t offset) {
  clock_hands[hand_index].image = lv_img_create (canvas, NULL);
  lv_img_set_src (clock_hands[hand_index].image, (void*)image);
  lv_obj_align (clock_hands[hand_index].image, canvas, align, offset.x, offset.y);
  clock_hands[hand_index].type = TYPE_NONE;
  clock_hands[hand_index].resol = TIME_RES_NONE;
  hand_index++;
}

void StandardClockSimple::add_clock_hand(TIME_RES res, const lv_img_dsc_t* image, lv_point_t offset) {
  add_clock_hand(res, image, offset, {0, 0}, false);
}

void StandardClockSimple::add_clock_hand(TIME_RES res, const lv_img_dsc_t* image, lv_point_t offset, lv_point_t pivot, bool is_shadow) {
  add_clock_hand(res, image, offset, pivot, is_shadow, 0, 0, 0, 0);
}

void StandardClockSimple::add_clock_hand(TIME_RES res, const lv_img_dsc_t* image, lv_point_t offset, lv_point_t pivot, bool is_shadow, int from_low, int from_high, int to_low, int to_high) {
  clock_hands[hand_index].image = lv_img_create (canvas, NULL);
  lv_img_set_src (clock_hands[hand_index].image, (void*)image);
  lv_obj_align (clock_hands[hand_index].image, canvas, LV_ALIGN_CENTER, offset.x, offset.y);
  if (pivot.x >= 0 && pivot.y >= 0) {
    lv_img_set_pivot (clock_hands[hand_index].image, pivot.x, pivot.y);
  }
  if (is_shadow) {
    lv_obj_add_style (clock_hands[hand_index].image, LV_IMG_PART_MAIN, &shadow_style);
  }
  clock_hands[hand_index].resol = res;
  clock_hands[hand_index].from_low = from_low;
  clock_hands[hand_index].from_high = from_high;
  clock_hands[hand_index].to_low = to_low;
  clock_hands[hand_index].to_high = to_high;
  clock_hands[hand_index].scale = (from_low != from_high);
  clock_hands[hand_index].type = TYPE_IMAGE;
  hand_index++;
}

void StandardClockSimple::init(lv_color_t back_color, int n_hands) {
  init(back_color, nullptr, n_hands, 0, nullptr, nullptr);
}

void StandardClockSimple::init(lv_color_t back_color, const lv_img_dsc_t* clock_face, int n_hands) {
  init(back_color, clock_face, n_hands, 0, nullptr, nullptr);
}

void StandardClockSimple::init(lv_color_t back_color, const lv_img_dsc_t* clock_face, int n_hands, void (* post_init)(lv_obj_t*), void (* event_handler)(lv_obj_t* obj, lv_event_t event)) {
  init(back_color, clock_face, n_hands, 0, post_init, event_handler);
}

void StandardClockSimple::init(lv_color_t back_color, const lv_img_dsc_t* clock_face, int n_hands, int n_sprites) {
  init(back_color, clock_face, n_hands, n_sprites, nullptr, nullptr);
}

void StandardClockSimple::init(lv_color_t back_color, const lv_img_dsc_t* clock_face, int n_hands, int n_sprites, void (* post_init)(lv_obj_t*)) {
  init(back_color, clock_face, n_hands, n_sprites, post_init, nullptr);
}

void StandardClockSimple::init(lv_color_t back_color, const lv_img_dsc_t* clock_face, int n_hands, int n_sprites, void (* post_init)(lv_obj_t*), void (* event_handler)(lv_obj_t* obj, lv_event_t event)) {
  hand_index = 0;
  num_hands = 0;
  if (n_hands != 0) {
    num_hands = n_hands + n_sprites;
    clock_hands = (standard_clock_hand_t*) malloc(num_hands * sizeof(standard_clock_hand_t));
  }

  lv_style_init(&main_style);
  lv_style_set_radius(&main_style, LV_OBJ_PART_MAIN, 0);
  lv_style_set_bg_opa(&main_style, LV_OBJ_PART_MAIN, LV_OPA_COVER);
  lv_style_set_border_width(&main_style, LV_OBJ_PART_MAIN, 0);
  lv_style_set_bg_color(&main_style, LV_STATE_DEFAULT, back_color);
  lv_style_set_text_color(&main_style, LV_OBJ_PART_MAIN, back_color);
  lv_style_set_image_recolor(&main_style, LV_OBJ_PART_MAIN, back_color);

  screen = lv_cont_create(lv_scr_act(), NULL);
  lv_obj_set_size(screen, LV_HOR_RES, LV_VER_RES);
  lv_obj_align(screen, NULL, LV_ALIGN_CENTER, 0, 0);
  if (!ISNULL(event_handler)) {
    lv_obj_set_event_cb(screen, event_handler);
  }
  lv_obj_add_style(screen, LV_OBJ_PART_MAIN, &main_style);

  canvas = lv_canvas_create(screen, NULL);
  cbuf = (lv_color_t*)ps_malloc(LV_CANVAS_BUF_SIZE_TRUE_COLOR_ALPHA(CANVAS_WIDTH, CANVAS_HEIGHT));
  lv_canvas_set_buffer(canvas, cbuf, CANVAS_WIDTH, CANVAS_HEIGHT, LV_IMG_CF_TRUE_COLOR_ALPHA);
  lv_obj_align (canvas, NULL, LV_ALIGN_CENTER, 0, 0);
  
  if (!ISNULL(clock_face)) {
    memcpy (cbuf, clock_face->data, clock_face->data_size);
  }

  if (!ISNULL(post_init)) {
    post_init(canvas);
  }

  lv_style_init (&shadow_style);
  lv_style_set_radius (&shadow_style, LV_OBJ_PART_MAIN, 0);
  lv_style_set_image_recolor (&shadow_style, LV_STATE_DEFAULT, LV_COLOR_BLACK);
  lv_style_set_image_recolor_opa (&shadow_style, LV_STATE_DEFAULT, LV_OPA_100);
  lv_style_set_image_opa (&shadow_style, LV_STATE_DEFAULT, 63);  // LV_OPA_25ish!
  lv_style_set_border_width (&shadow_style, LV_OBJ_PART_MAIN, 0);
}

void StandardClockSimple::showBatteryIcon(int i, bool charge, int p) {
  if (ISNULL(canvas)) return;
  int num_images = clock_hands[i].images_size;
  int last_image_index = num_images - 1;
  float p_step = 100.0 / (last_image_index - 1);
  int icon = last_image_index - 1;
  if (charge) {
    icon = last_image_index;
  } else {
    float p2 = 100.0 - p_step;
    for (int j = 0; j < last_image_index - 1; j++) {
      if (p > p2) break;
      icon--;
      p2 -= p_step;
    }
  }
  lv_img_set_src (clock_hands[i].image, clock_hands[i].images[icon]);
  lv_obj_align (clock_hands[i].image, canvas, clock_hands[i].align, clock_hands[i].offset.x, clock_hands[i].offset.y);
}

void StandardClockSimple::showSteps(int i, int st) {
  if (ISNULL(canvas)) return;
  lv_img_set_src (clock_hands[i].image, clock_hands[i].images[st]);
  lv_obj_align (clock_hands[i].image, canvas, clock_hands[i].align, clock_hands[i].offset.x, clock_hands[i].offset.y);
}

int StandardClockSimple::nthdig(int n, int k) {
  while (n--)
    k /= 10;
  return k % 10;
}

void StandardClockSimple::show() {
  if (ISNULL(canvas)) return;
  TTGOClass *ttgo = TTGOClass::getWatch();
  int p = ttgo->power->getBattPercentage();
  int b1 = p == 100 ? 0 : p % 10;
  int b2 = p == 100 ? 0 : p / 10;
  //  Serial.printf("bat: %d %d %d\n", p, b2, b1);
  bool charge = ttgo->power->isChargeing();
  uint32_t steps = ttgo->bma->getCounter();
  // steps = 1234;
  int st1 = nthdig(0, steps);
  int st2 = nthdig(1, steps);
  int st3 = nthdig(2, steps);
  int st4 = nthdig(3, steps);
  int st5 = nthdig(4, steps);

  for (int i = 0; i < num_hands; i++) {
    if (clock_hands[i].resol == TIME_RES_BATTERY) {
      if (clock_hands[i].type == TYPE_TEXT) {
        char buf[50];
        sprintf(buf, clock_hands[i].format, p);
        lv_label_set_text (clock_hands[i].image, buf);
        lv_obj_align(clock_hands[i].image, NULL, clock_hands[i].align, clock_hands[i].offset.x, clock_hands[i].offset.y);
      }
      if (clock_hands[i].type == TYPE_IMAGE) {
        if (clock_hands[i].scale) {
          p = map(p, clock_hands[i].from_low, clock_hands[i].from_high, clock_hands[i].to_low, clock_hands[i].to_high);
        }
        lv_img_set_angle (clock_hands[i].image, 60 * p);
      }
      if (clock_hands[i].type == TYPE_SPRITE) {
        showBatteryIcon(i, charge, p);
      }
    }

    else if (clock_hands[i].resol == TIME_RES_BATTERY_UNIT) {
      if (clock_hands[i].type == TYPE_SPRITE) {
        lv_img_set_src (clock_hands[i].image, clock_hands[i].images[b1]);
        lv_obj_align (clock_hands[i].image, canvas, clock_hands[i].align, clock_hands[i].offset.x, clock_hands[i].offset.y);
      }
    }

    else if (clock_hands[i].resol == TIME_RES_BATTERY_TEN) {
      if (clock_hands[i].type == TYPE_SPRITE) {
        lv_img_set_src (clock_hands[i].image, clock_hands[i].images[b2]);
        lv_obj_align (clock_hands[i].image, canvas, clock_hands[i].align, clock_hands[i].offset.x, clock_hands[i].offset.y);
      }
    }

    else if (clock_hands[i].resol == TIME_RES_STEPS) {

    }

    else if (clock_hands[i].resol == TIME_RES_STEPS_UNIT) {
      if (clock_hands[i].type == TYPE_SPRITE) {
        lv_img_set_src (clock_hands[i].image, clock_hands[i].images[st1]);
        lv_obj_align (clock_hands[i].image, canvas, clock_hands[i].align, clock_hands[i].offset.x, clock_hands[i].offset.y);
      }
    }

    else if (clock_hands[i].resol == TIME_RES_STEPS_TEN) {
      if (clock_hands[i].type == TYPE_SPRITE) {
        lv_img_set_src (clock_hands[i].image, clock_hands[i].images[st2]);
        lv_obj_align (clock_hands[i].image, canvas, clock_hands[i].align, clock_hands[i].offset.x, clock_hands[i].offset.y);
      }
    }

    else if (clock_hands[i].resol == TIME_RES_STEPS_HUNDRED) {
      if (clock_hands[i].type == TYPE_SPRITE) {
        lv_img_set_src (clock_hands[i].image, clock_hands[i].images[st3]);
        lv_obj_align (clock_hands[i].image, canvas, clock_hands[i].align, clock_hands[i].offset.x, clock_hands[i].offset.y);
      }
    }

    else if (clock_hands[i].resol == TIME_RES_STEPS_THOUSAND) {
      if (clock_hands[i].type == TYPE_SPRITE) {
        lv_img_set_src (clock_hands[i].image, clock_hands[i].images[st4]);
        lv_obj_align (clock_hands[i].image, canvas, clock_hands[i].align, clock_hands[i].offset.x, clock_hands[i].offset.y);
      }
    }

    else if (clock_hands[i].resol == TIME_RES_STEPS_TEN_THOUSAND) {
      if (clock_hands[i].type == TYPE_SPRITE) {
        lv_img_set_src (clock_hands[i].image, clock_hands[i].images[st5]);
        lv_obj_align (clock_hands[i].image, canvas, clock_hands[i].align, clock_hands[i].offset.x, clock_hands[i].offset.y);
      }
    }

  }

  task = lv_task_create(lv_update_task, 1000, LV_TASK_PRIO_LOWEST, NULL);
  last_minute = 61; //Reset
  update_time();
}

void StandardClockSimple::hide() {
  if (task) {
    lv_task_del(task);
  }
  LV_OBJ_DEL(canvas);
  LV_OBJ_DEL(screen);
  lv_style_reset(&shadow_style);
  lv_style_reset(&main_style);
  FREE(cbuf);
  if (num_hands != 0) {
    for (int i = 0; i < num_hands; i++) {
      if (clock_hands[i].type == TYPE_TEXT || clock_hands[i].type == TYPE_SPRITE) {
        lv_style_reset(&clock_hands[i].style);
      }
    }
    FREE(clock_hands);
  }
}

void StandardClockSimple::event(WATCH_EVENT event) {
  if (ISNULL(canvas)) return;
  TTGOClass *ttgo = TTGOClass::getWatch();
  for (int i = 0; i < num_hands; i++) {
    if (clock_hands[i].resol == TIME_RES_BATTERY) {
      if (clock_hands[i].type == TYPE_SPRITE) {
        int p = ttgo->power->getBattPercentage();
        switch (event) {
          case EVENT_BATT_CHARGE_START:
            showBatteryIcon(i, true, p);
            break;
          case EVENT_BATT_CHARGE_STOP:
            showBatteryIcon(i, false, p);
            break;
        }
      }
    }
    else if (clock_hands[i].resol == TIME_RES_STEPS_UNIT) {
      if (clock_hands[i].type == TYPE_SPRITE) {
        uint32_t steps = ttgo->bma->getCounter();
        switch (event) {
          case EVENT_STEP_COUNTER:
            showSteps(i, nthdig(0, steps));
            break;
        }
      }
    }

    else if (clock_hands[i].resol == TIME_RES_STEPS_TEN) {
      if (clock_hands[i].type == TYPE_SPRITE) {
        uint32_t steps = ttgo->bma->getCounter();
        switch (event) {
          case EVENT_STEP_COUNTER:
            showSteps(i, nthdig(1, steps));
            break;
        }
      }
    }

    else if (clock_hands[i].resol == TIME_RES_STEPS_HUNDRED) {
      if (clock_hands[i].type == TYPE_SPRITE) {
        uint32_t steps = ttgo->bma->getCounter();
        switch (event) {
          case EVENT_STEP_COUNTER:
            showSteps(i, nthdig(2, steps));
            break;
        }
      }
    }

    else if (clock_hands[i].resol == TIME_RES_STEPS_THOUSAND) {
      if (clock_hands[i].type == TYPE_SPRITE) {
        uint32_t steps = ttgo->bma->getCounter();
        switch (event) {
          case EVENT_STEP_COUNTER:
            showSteps(i, nthdig(3, steps));
            break;
        }
      }
    }

    else if (clock_hands[i].resol == TIME_RES_STEPS_TEN_THOUSAND) {
      if (clock_hands[i].type == TYPE_SPRITE) {
        uint32_t steps = ttgo->bma->getCounter();
        switch (event) {
          case EVENT_STEP_COUNTER:
            showSteps(i, nthdig(4, steps));
            break;
        }
      }
    }
  }
}

void StandardClockSimple::lv_update_task(struct _lv_task_t *data) {
  update_time();
}

void StandardClockSimple::set_text (int i, struct tm info) {
  char buf[150];
  strftime(buf, sizeof(buf), clock_hands[i].format, &info);
  lv_label_set_text (clock_hands[i].image, buf);
  lv_obj_align(clock_hands[i].image, NULL, clock_hands[i].align, clock_hands[i].offset.x, clock_hands[i].offset.y);
}

void StandardClockSimple::update_time() {
  if (ISNULL(canvas)) return;
  time_t now;
  struct tm info;

  //  log_d ("idx=%d", idx);

  time (&now);
  localtime_r (&now, &info);

  int sec  = 60 * info.tm_sec;
  int ss = info.tm_sec;
  int s1 = info.tm_sec % 10;
  int s2 = info.tm_sec / 10;
  uint8_t mon = info.tm_mon;
  int mo1 = (mon + 1) % 10;
  int mo2 = (mon + 1) / 10;
  uint8_t day = info.tm_mday;
  int a0 = day / 10;
  int a1 = day % 10;
  int year = info.tm_year + 1900;
  // year = 1234;
  int y1 = nthdig(0, year);
  int y2 = nthdig(1, year);
  int y3 = nthdig(2, year);
  int y4 = nthdig(3, year);

  // seroutf(PSTR("sec: %d %d %d\r\n"), sec, s2, s1);

  if (info.tm_min != last_minute) {
    last_minute = info.tm_min;

    int hour = (300 * (info.tm_hour % 12)) + (5 * last_minute);
    int h1 = info.tm_hour % 10;
    int h2 = info.tm_hour / 10;
    int min  = 60 * last_minute;
    int m1 = info.tm_min % 10;
    int m2 = info.tm_min / 10;
    uint8_t dow = info.tm_wday;

    //    seroutf(PSTR("hour: %d %d %d\r\n"), hour, h2, h1);
    //    seroutf(PSTR("min: %d %d %d\r\n"), min, m2, m1);

    for (int i = 0; i < num_hands; i++) {
      if (clock_hands[i].resol == TIME_RES_MINUTE) {
        if (clock_hands[i].type == TYPE_IMAGE) {
          lv_img_set_angle (clock_hands[i].image, min);
        }
        if (clock_hands[i].type == TYPE_TEXT) {
          set_text(i, info);
        }
      }

      else if (clock_hands[i].resol == TIME_RES_MINUTE_UNIT) {
        if (clock_hands[i].type == TYPE_SPRITE) {
          lv_img_set_src (clock_hands[i].image, clock_hands[i].images[m1]);
          lv_obj_align (clock_hands[i].image, canvas, clock_hands[i].align, clock_hands[i].offset.x, clock_hands[i].offset.y);
        }
      }

      else if (clock_hands[i].resol == TIME_RES_MINUTE_TEN) {
        if (clock_hands[i].type == TYPE_SPRITE) {
          lv_img_set_src (clock_hands[i].image, clock_hands[i].images[m2]);
          lv_obj_align (clock_hands[i].image, canvas, clock_hands[i].align, clock_hands[i].offset.x, clock_hands[i].offset.y);
        }
      }

      else if (clock_hands[i].resol == TIME_RES_HOUR) {
        if (clock_hands[i].type == TYPE_IMAGE) {
          lv_img_set_angle (clock_hands[i].image, hour);
        }
        if (clock_hands[i].type == TYPE_TEXT) {
          set_text(i, info);
        }
      }

      else if (clock_hands[i].resol == TIME_RES_HOUR_UNIT) {
        if (clock_hands[i].type == TYPE_SPRITE) {
          lv_img_set_src (clock_hands[i].image, clock_hands[i].images[h1]);
          lv_obj_align (clock_hands[i].image, canvas, clock_hands[i].align, clock_hands[i].offset.x, clock_hands[i].offset.y);
        }
      }

      else if (clock_hands[i].resol == TIME_RES_HOUR_TEN) {
        if (clock_hands[i].type == TYPE_SPRITE) {
          lv_img_set_src (clock_hands[i].image, clock_hands[i].images[h2]);
          lv_obj_align (clock_hands[i].image, canvas, clock_hands[i].align, clock_hands[i].offset.x, clock_hands[i].offset.y);
        }
      }

      else if (clock_hands[i].resol == TIME_RES_MONTH) {
        if (clock_hands[i].type == TYPE_SPRITE) {
          lv_img_set_src (clock_hands[i].image, clock_hands[i].images[mon]);
          lv_obj_align (clock_hands[i].image, canvas, clock_hands[i].align, clock_hands[i].offset.x, clock_hands[i].offset.y);
        }
      }

      else if (clock_hands[i].resol == TIME_RES_MONTH_UNIT) {
        if (clock_hands[i].type == TYPE_SPRITE) {
          lv_img_set_src (clock_hands[i].image, clock_hands[i].images[mo1]);
          lv_obj_align (clock_hands[i].image, canvas, clock_hands[i].align, clock_hands[i].offset.x, clock_hands[i].offset.y);
        }
      }

      else if (clock_hands[i].resol == TIME_RES_MONTH_TEN) {
        if (clock_hands[i].type == TYPE_SPRITE) {
          lv_img_set_src (clock_hands[i].image, clock_hands[i].images[mo2]);
          lv_obj_align (clock_hands[i].image, canvas, clock_hands[i].align, clock_hands[i].offset.x, clock_hands[i].offset.y);
        }
      }

      else if (clock_hands[i].resol == TIME_RES_YEAR) {
      }

      else if (clock_hands[i].resol == TIME_RES_YEAR_UNIT) {
        if (clock_hands[i].type == TYPE_SPRITE) {
          lv_img_set_src (clock_hands[i].image, clock_hands[i].images[y1]);
          lv_obj_align (clock_hands[i].image, canvas, clock_hands[i].align, clock_hands[i].offset.x, clock_hands[i].offset.y);
        }
      }

      else if (clock_hands[i].resol == TIME_RES_YEAR_TEN) {
        if (clock_hands[i].type == TYPE_SPRITE) {
          lv_img_set_src (clock_hands[i].image, clock_hands[i].images[y2]);
          lv_obj_align (clock_hands[i].image, canvas, clock_hands[i].align, clock_hands[i].offset.x, clock_hands[i].offset.y);
        }
      }

      else if (clock_hands[i].resol == TIME_RES_YEAR_HUNDRED) {
        if (clock_hands[i].type == TYPE_SPRITE) {
          lv_img_set_src (clock_hands[i].image, clock_hands[i].images[y3]);
          lv_obj_align (clock_hands[i].image, canvas, clock_hands[i].align, clock_hands[i].offset.x, clock_hands[i].offset.y);
        }
      }

      else if (clock_hands[i].resol == TIME_RES_YEAR_THOUSAND) {
        if (clock_hands[i].type == TYPE_SPRITE) {
          lv_img_set_src (clock_hands[i].image, clock_hands[i].images[y4]);
          lv_obj_align (clock_hands[i].image, canvas, clock_hands[i].align, clock_hands[i].offset.x, clock_hands[i].offset.y);
        }
      }

      else if (clock_hands[i].resol == TIME_RES_DAY) {
        if (clock_hands[i].type == TYPE_TEXT) {
          set_text(i, info);
        }
        if (clock_hands[i].type == TYPE_IMAGE) {
          int day2 = day;
          if (clock_hands[i].scale) {
            day2 = map(day2, clock_hands[i].from_low, clock_hands[i].from_high, clock_hands[i].to_low, clock_hands[i].to_high);
          }
          lv_img_set_angle (clock_hands[i].image, 60 * day2);
        }
      }

      else if (clock_hands[i].resol == TIME_RES_WEEKDAY) {
        if (clock_hands[i].type == TYPE_SPRITE) {
          lv_img_set_src (clock_hands[i].image, clock_hands[i].images[dow]);
          lv_obj_align (clock_hands[i].image, canvas, clock_hands[i].align, clock_hands[i].offset.x, clock_hands[i].offset.y);
        }
        if (clock_hands[i].type == TYPE_IMAGE) {
          int dow2 = dow;
          if (clock_hands[i].scale) {
            dow2 = map(dow2, clock_hands[i].from_low, clock_hands[i].from_high, clock_hands[i].to_low, clock_hands[i].to_high);
          }
          lv_img_set_angle (clock_hands[i].image, 60 * dow2);
        }
      }

      else if (clock_hands[i].resol == TIME_RES_DAY_UNIT) {
        if (clock_hands[i].type == TYPE_SPRITE) {
          lv_img_set_src (clock_hands[i].image, clock_hands[i].images[a1]);
          lv_obj_align (clock_hands[i].image, canvas, clock_hands[i].align, clock_hands[i].offset.x, clock_hands[i].offset.y);
        }
      }

      else if (clock_hands[i].resol == TIME_RES_DAY_TEN) {
        if (clock_hands[i].type == TYPE_SPRITE) {
          lv_img_set_src (clock_hands[i].image, clock_hands[i].images[a0]);
          lv_obj_align (clock_hands[i].image, canvas, clock_hands[i].align, clock_hands[i].offset.x, clock_hands[i].offset.y);
        }
      }
    }
  }

  for (int i = 0; i < num_hands; i++) {
    if (clock_hands[i].resol == TIME_RES_SECOND) {
      if (clock_hands[i].type == TYPE_SPRITE) {
        lv_img_set_src (clock_hands[i].image, clock_hands[i].images[ss]);
        lv_obj_align (clock_hands[i].image, canvas, clock_hands[i].align, clock_hands[i].offset.x, clock_hands[i].offset.y);
      }
      if (clock_hands[i].type == TYPE_IMAGE) {
        lv_img_set_angle (clock_hands[i].image, sec);
      }
      if (clock_hands[i].type == TYPE_TEXT) {
        set_text(i, info);
      }
    }

    else if (clock_hands[i].resol == TIME_RES_SECOND_UNIT) {
      if (clock_hands[i].type == TYPE_SPRITE) {
        lv_img_set_src (clock_hands[i].image, clock_hands[i].images[s1]);
        lv_obj_align (clock_hands[i].image, canvas, clock_hands[i].align, clock_hands[i].offset.x, clock_hands[i].offset.y);
      }
    }

    else if (clock_hands[i].resol == TIME_RES_SECOND_TEN) {
      if (clock_hands[i].type == TYPE_SPRITE) {
        lv_img_set_src (clock_hands[i].image, clock_hands[i].images[s2]);
        lv_obj_align (clock_hands[i].image, canvas, clock_hands[i].align, clock_hands[i].offset.x, clock_hands[i].offset.y);
      }
    }

    if (clock_hands[i].resol == TIME_RES_SECOND_BLINK) {
      if (clock_hands[i].type == TYPE_IMAGE) {
        // seroutf(PSTR("sec2 : %d\r\n"), s1 % 2);
        lv_obj_set_hidden (clock_hands[i].image, s1 % 2 == 0);
      }
    }
  }
}
