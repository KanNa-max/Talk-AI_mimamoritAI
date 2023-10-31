#include <stdio.h>
#include <stdlib.h>

#include "main_music.h"
#include "voice_rec.h"


void setup() {
  init_VoiceRec(MP3_FILE_NAME);
}

void loop() {

  play_Voice(MAX_PLAY_TIME);
  printf("stop: first`play_voice()`\n");
  init_VoiceRec(MP3_FILE_NAME);

  play_Voice(MAX_PLAY_TIME);
  init_VoiceRec(MP3_FILE_NAME);
  return;
}
