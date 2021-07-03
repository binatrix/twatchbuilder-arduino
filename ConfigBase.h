#ifndef ConfigBase_h
#define ConfigBase_h

class ConfigBase {
  public:
    virtual void load(JsonDocument& doc) {}
    virtual void save(JsonDocument& doc) {}
    virtual void show() {}
    virtual void hide() {}
    virtual void loop() {}

  protected:
    bool loaded = false;
};

#endif
