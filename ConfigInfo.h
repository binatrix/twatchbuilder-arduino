#ifndef ConfigInfo_h
#define ConfigInfo_h

#include "ConfigBase.h"

class ConfigInfo : public ConfigBase {
  public:
    void show();
    void hide();

  private:
    lv_obj_t* label;
};

#endif
