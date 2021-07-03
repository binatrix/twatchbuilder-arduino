#include "Settings.h"

typedef struct {
  const char *name;
  void *icon;
  const char* id;
  void (* load)(JsonDocument& doc);
  void (* save)(JsonDocument& doc);
  void (* event)(WATCH_EVENT event);
  void (* show)();
  void (* hide)();
  void (* loop)();
  bool clock;
  bool sleep;
} lv_menu_config_t;

lv_menu_config_t apps[] = {
  { .name = "Settings",
    .icon = (void *) &updater_icon,
    .id = nullptr,
    .load = nullptr,
    .save = nullptr,
    .event = nullptr,
    .show = Settings_show,
    .hide = Settings_hide,
    .loop = Settings_loop,
    .clock = false,
    .sleep = false
  },
};
