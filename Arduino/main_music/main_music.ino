#include <stdio.h>
#include <stdlib.h>

// #include "main_music.h"
#include "voice_rec.h"

void setup() {
  Serial.begin(115200);
  init_SDcard();
  // init_VoicePlay(MP3_FILE_NAME);  // "Sound.mp3"
}

void loop() {
  // init_VoicePlay(MP3_FILE_NAME, MP3_VOLUME);
  // play_Voice(MAX_PLAY_TIME);  // 10sec
  // Serial.println("stop: first play_voice()");
  // delay(1000);

  init_VoicePlay("MUSIC/zunda_01.mp3", 100);
  play_Voice(MAX_PLAY_TIME);
  Serial.println("stop: second play_voice()");
  delay(1000);

  init_VoicePlay("MUSIC/zunda_02.mp3", 100);
  play_Voice(MAX_PLAY_TIME);
  Serial.println("stop: second play_voice()");
  delay(1000);

  init_VoicePlay("MUSIC/zunda_03.mp3", 100);
  play_Voice(MAX_PLAY_TIME);
  Serial.println("stop: second play_voice()");
  delay(1000);

  init_VoicePlay("MUSIC/zunda_04.mp3", 100);
  play_Voice(MAX_PLAY_TIME);
  Serial.println("stop: second play_voice()");
  delay(1000);

  init_VoicePlay("MUSIC/zunda_05.mp3", 100);
  play_Voice(MAX_PLAY_TIME);
  Serial.println("stop: second play_voice()");
  delay(1000);

  init_VoicePlay("MUSIC/zunda_06.mp3", 100);
  play_Voice(MAX_PLAY_TIME);
  Serial.println("stop: second play_voice()");
  delay(1000);

  delay(5000);
}
