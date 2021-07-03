#ifndef audio_h
#define audio_h

#include "AudioFileSourceSPIFFS.h"
#include "AudioFileSourceID3.h"
#include "AudioGeneratorMP3.h"
#include "AudioOutputI2S.h"

class AudioPlayer {
  public:
    AudioPlayer();
    AudioPlayer(const char* mp3file);
    ~AudioPlayer();
    void play ();
    void play (const char* mp3file);
    void stop();
    void loop();
    bool isPlaying();

  private:
    void audioStop();
    AudioGeneratorMP3 *mp3 = NULL;
    AudioFileSourceSPIFFS *file = NULL;
    AudioOutputI2S *out = NULL;
    AudioFileSourceID3 *id3 = NULL;
    uint32_t freq;
    bool playing = false;
    bool active = false;
    char mp3audio[50];
    TTGOClass *ttgo;
};

#endif
