#include "MyWatch.h"
#include "audio.h"

AudioPlayer::AudioPlayer() {
  ttgo = TTGOClass::getWatch();
}

AudioPlayer::AudioPlayer(const char* mp3file) {
  ttgo = TTGOClass::getWatch();
  strcpy(mp3audio, mp3file);
}

AudioPlayer::~AudioPlayer() {
  stop();
}

void AudioPlayer::play () {
  play(mp3audio);
}

void AudioPlayer::play (const char* mp3file) {
#ifndef LILYGO_WATCH_2020_V2
  ttgo->disableTouchIRQ();
  active = false;
  if (!SPIFFS.exists(mp3file)) {
    ttgo->enableTouchIRQ();
    return;
  }
  active = true;
  freq = getCpuFrequencyMhz();
  setCpuFrequencyMhz(CPU_FREQ_MAX);
  file = new AudioFileSourceSPIFFS(mp3file);
  id3 = new AudioFileSourceID3(file);
  out = new AudioOutputI2S();
  //External DAC decoding
  out->SetPinout(TWATCH_DAC_IIS_BCK, TWATCH_DAC_IIS_WS, TWATCH_DAC_IIS_DOUT);
  mp3 = new AudioGeneratorMP3();
  mp3->begin(id3, out);
  playing = true;
#endif
}

void AudioPlayer::stop() {
#ifndef LILYGO_WATCH_2020_V2
  if (!ISNULL(mp3) && active) {
    if (mp3->isRunning()) {
      audioStop();
    }
  }
#endif
}

void AudioPlayer::loop() {
#ifndef LILYGO_WATCH_2020_V2
  if (!ISNULL(mp3) && active) {
    if (mp3->isRunning()) {
      if (!mp3->loop()) {
        audioStop();
      }
    }
  }
#endif
}

bool AudioPlayer::isPlaying() {
  return playing;
}

void AudioPlayer::audioStop() {
#ifndef LILYGO_WATCH_2020_V2
  if (!ISNULL(mp3) && active) {
    mp3->stop();
    playing = false;
    delete(mp3);
    delete(out);
    delete(id3);
    delete(file);
    mp3 = NULL;
    out = NULL;
    id3 = NULL;
    file = NULL;
    setCpuFrequencyMhz(freq);
    ttgo->enableTouchIRQ();
  }
#endif
}
