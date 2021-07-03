#include "MyWatch.h"
#include "ConfigFileman.h"

lv_obj_t* ConfigFileman::mbox1;
lv_obj_t** ConfigFileman::list_btn = NULL;
char** ConfigFileman::list_files = NULL;

void ConfigFileman::dialog_event_handler(lv_obj_t* obj, lv_event_t event) {
  if (event == LV_EVENT_VALUE_CHANGED) {
    char buf[6];
    strcpy(buf, lv_msgbox_get_active_btn_text(obj));
    if (strcmp(buf, "Yes") == 0) {
      int sel_item = (int)lv_obj_get_user_data(obj);
      lv_obj_t * label = lv_obj_get_child(list_btn[sel_item], NULL);
      const char * txt = lv_label_get_text(label);
      deleteFile(SPIFFS, txt);
      lv_obj_del(list_btn[sel_item]);
    }
    LV_OBJ_DEL(mbox1);
  }
}

void ConfigFileman::list_event_handler(lv_obj_t* obj, lv_event_t event) {
  if (event == LV_EVENT_LONG_PRESSED) {
    int sel_item = (int)lv_obj_get_user_data(obj);
    static const char* btns[] = { "Yes", "No", "" };
    mbox1 = lv_msgbox_create(lv_scr_act(), NULL);
    lv_msgbox_set_text(mbox1, "Delete file?");
    lv_msgbox_add_btns(mbox1, btns);
    lv_obj_set_width(mbox1, LV_HOR_RES);
    lv_obj_set_event_cb(mbox1, dialog_event_handler);
    lv_obj_set_user_data(mbox1, (lv_obj_user_data_t)sel_item);
    lv_obj_align(mbox1, NULL, LV_ALIGN_CENTER, 0, 0);
  }
  if (event == LV_EVENT_CLICKED) {
    int sel_item = (int)lv_obj_get_user_data(obj);
    lv_obj_t * label = lv_obj_get_child(list_btn[sel_item], NULL);
    const char * txt = lv_label_get_text(label);
    const char* exts[] = { ".json", ".txt" };
    int exts_len = 2;
    for(int i=0; i<exts_len; i++) {
      char* p = strstr(txt, exts[i]);
      if (p) {
        File f = SPIFFS.open(txt, "r");
        if (!f) {
          serout(F("file open failed"));
        }
        else {
          seroutf(PSTR("reading %s\r\n"), txt);
          while (f.available()) {
            String s = f.readStringUntil('\n');
            serout(s);
          }
          f.close();
          serout(F("end"));
        }
      }
    }
  }
}

void ConfigFileman::set_btn(lv_obj_t* btn, int i) {
  lv_obj_add_style(btn, LV_OBJ_PART_MAIN, &btn_style);
  lv_btn_set_checkable(btn, false);
  lv_obj_set_user_data(btn, (lv_obj_user_data_t)i);
  lv_obj_set_event_cb(btn, list_event_handler);
  lv_page_glue_obj(btn, true);
}

void ConfigFileman::deleteFile(fs::FS &fs, const char * path) {
  // Serial.printf("Deleting file: %s\r\n", path);
  if (fs.remove(path)) {
    // Serial.println("- file deleted");
  } else {
    // Serial.println("- delete failed");
  }
}

void ConfigFileman::listDir(fs::FS &fs, const char * dirname, uint8_t levels) {
  // Serial.printf("Listing directory: %s\r\n", dirname);

  File root = fs.open(dirname);
  if (!root) {
    // Serial.println("- failed to open directory");
    return;
  }
  if (!root.isDirectory()) {
    // Serial.println(" - not a directory");
    return;
  }

  File file = root.openNextFile();
  int i = 0;
  while (file) {
    if (file.isDirectory()) {
      // Serial.print("  DIR : ");
      // Serial.println(file.name());
      //      if (levels) {
      //        listDir(fs, file.name(), levels - 1);
      //      }
    } else {
      // Serial.print("  FILE: ");
      // Serial.print(file.name());
      // Serial.print("\tSIZE: ");
      // Serial.println(file.size());
    }
    if (list_files == NULL) {
      list_files = (char**)malloc(sizeof(char*));
    } else {
      list_files = (char**)realloc(list_files, (i + 1) * sizeof(char*));
    }
    list_files[i] = new char[40];
    strcpy(list_files[i], file.name());
    i++;
    file = root.openNextFile();
  }

  file_count = i;
  if (file_count > 1) {
    qsort(list_files, file_count, sizeof(list_files[0]), cmpfunc);
  }
  for (i = 0; i < file_count; i++) {
    // Serial.println(list_files[i]);
    if (list_btn == NULL) {
      list_btn = (lv_obj_t**)malloc(sizeof(lv_obj_t*));
    } else {
      list_btn = (lv_obj_t**)realloc(list_btn, (i + 1) * sizeof(lv_obj_t*));
    }
    list_btn[i] = lv_list_add_btn(list, LV_SYMBOL_FILE, list_files[i]);
    set_btn(list_btn[i], i);
  }
}

void ConfigFileman::show() {
  loaded = true;
  lv_style_init(&list_style);
  lv_style_set_radius(&list_style, LV_OBJ_PART_MAIN, 0);
  lv_style_set_bg_opa(&list_style, LV_OBJ_PART_MAIN, LV_OPA_0);
  lv_style_set_border_width(&list_style, LV_OBJ_PART_MAIN, 0);
  lv_style_set_text_color(&list_style, LV_OBJ_PART_MAIN, LV_COLOR_MAIN);
  lv_style_set_image_recolor(&list_style, LV_OBJ_PART_MAIN, LV_COLOR_MAIN);

  lv_style_init(&btn_style);
  lv_style_copy(&btn_style, &list_style);
  lv_style_set_bg_color(&btn_style, LV_OBJ_PART_MAIN, LV_COLOR_BLACK);
  lv_style_set_bg_opa(&btn_style, LV_OBJ_PART_MAIN, LV_OPA_40);

  list = lv_list_create(lv_scr_act(), NULL);
  lv_list_set_scrollbar_mode(list, LV_SCROLLBAR_MODE_AUTO); //LV_SCROLLBAR_MODE_OFF);
  lv_obj_set_size(list, LV_HOR_RES, LV_VER_RES);
  lv_obj_add_style(list, LV_OBJ_PART_MAIN, &list_style);
  lv_obj_align(list, NULL, LV_ALIGN_CENTER, 0, 0);

  listDir(SPIFFS, "/", 0);
}

void ConfigFileman::hide() {
  if (loaded) {
    lv_list_clean(list);
    LV_OBJ_DEL(list);
    LV_OBJ_DEL(mbox1);
    lv_style_reset(&list_style);
    lv_style_reset(&btn_style);
    FREE(list_btn);
    for (int i = 0; i < file_count; i++) {
      delete list_files[i];
    }
    FREE(list_files);
  }
  loaded = false;
}

int ConfigFileman::cmpfunc (const void * a, const void * b) {
  return strcmp(*(char**)a, *(char**)b);
}
