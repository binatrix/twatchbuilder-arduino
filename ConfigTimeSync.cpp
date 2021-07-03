// MEM OK
#include "MyWatch.h"
#include "ConfigTimeSync.h"
#include "utils.h"

lv_obj_t* ConfigTimeSync::container1;
lv_obj_t* ConfigTimeSync::button1;
lv_obj_t* ConfigTimeSync::label1;

void ConfigTimeSync::event_handler(lv_obj_t * obj, lv_event_t event) {
  if (event == LV_EVENT_CLICKED) {
    lv_obj_del(button1);
    label1 = lv_label_create(container1, NULL);
    lv_label_set_long_mode(label1, LV_LABEL_LONG_BREAK);
    //    lv_obj_set_width(label1, CANVAS_WIDTH);
    lv_obj_set_size(label1, LV_HOR_RES, LV_VER_RES);
    lv_label_set_align(label1, LV_LABEL_ALIGN_CENTER);
    info("Starting WiFi", false);
    if (!wifiOn()) {
      info("WiFi error", true);
      delay(3000);
      return;
    }
    info("Syncing...", false);
    syncTime();
    wifiOff();
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) {
      Serial.println("Failed to obtain time, Restart in 3 seconds");
      return;
    }
    //Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
    char timeStringBuff[50]; //50 chars should be enough
    strftime(timeStringBuff, sizeof(timeStringBuff), "Current time:\n%A, %B %d %Y\n%H:%M:%S", &timeinfo);
    info(timeStringBuff, false);
    delay(5000);
    ESP.restart();
  }
}

void ConfigTimeSync::show() {
  loaded = true;
  lv_style_init(&menuStyle);
  lv_style_set_radius(&menuStyle, LV_OBJ_PART_MAIN, 0);
  //lv_style_set_bg_color(&menuStyle, LV_OBJ_PART_MAIN, LV_COLOR_GRAY);
  lv_style_set_bg_opa(&menuStyle, LV_OBJ_PART_MAIN, LV_OPA_0);
  lv_style_set_border_width(&menuStyle, LV_OBJ_PART_MAIN, 0);
  lv_style_set_text_color(&menuStyle, LV_OBJ_PART_MAIN, LV_COLOR_BLACK);
  lv_style_set_image_recolor(&menuStyle, LV_OBJ_PART_MAIN, LV_COLOR_BLACK);

  container1 = lv_cont_create(lv_scr_act(), nullptr);
  lv_obj_set_size(container1, LV_HOR_RES, LV_VER_RES);
  lv_obj_align(container1, NULL, LV_ALIGN_CENTER, 0, 0);
  lv_obj_add_style(container1, LV_OBJ_PART_MAIN, &menuStyle);

  lv_style_init(&style_btn_red);
  lv_style_set_bg_color(&style_btn_red, LV_STATE_DEFAULT, LV_COLOR_RED);
  lv_style_set_bg_grad_color(&style_btn_red, LV_STATE_DEFAULT, LV_COLOR_MAROON);
  lv_style_set_bg_color(&style_btn_red, LV_STATE_PRESSED, LV_COLOR_MAROON);
  lv_style_set_bg_grad_color(&style_btn_red, LV_STATE_PRESSED, LV_COLOR_RED);
  lv_style_set_text_color(&style_btn_red, LV_STATE_DEFAULT, LV_COLOR_WHITE);

  button1 = lv_btn_create(container1, NULL);
  lv_obj_set_event_cb(button1, event_handler);
  lv_obj_align(button1, NULL, LV_ALIGN_CENTER, 0, 0);
  lv_obj_add_style(button1, LV_OBJ_PART_MAIN, &style_btn_red);

  lv_obj_t* label = lv_label_create(button1, NULL);
  lv_label_set_text(label, "Sync Time");
}

void ConfigTimeSync::hide() {
  if (loaded) {
    lv_style_reset(&menuStyle);
    lv_style_reset(&style_btn_red);
    LV_OBJ_DEL(container1);
  }
  loaded = false;
}

void ConfigTimeSync::info (const char* text, bool error) {
  lv_label_set_text(label1, text);
  lv_obj_set_style_local_text_color(label1, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, error ? LV_COLOR_RED : LV_COLOR_MAIN);
  lv_obj_align(label1, NULL, LV_ALIGN_CENTER, 0, 0);
  lv_refr_now(NULL);
  Serial.println(text);
}
