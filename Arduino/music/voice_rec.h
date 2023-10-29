#ifdef voice_rec_h
#define voice_rec_h

#define MP3_FILE_NAME "Sound.mp3"

#define MP3_VOLUME -160
/*The "volume" parameter allows you to adjust the volume within a range of -
    1020 to 120. The specified value is represented in 1
        10th of a decibel(dB).For example,
    if you set it to 15,
    the actual volume will be 1.5 dB.
*/

// extern SDClass theSD;
// extern AudioClass *theAudio;
// extern File myFile;
extern bool ErrEnd;

void audio_attention_cb(const ErrorAttentionParam *atprm);
void setupAudioPlayer();
void loopAudioPlayer();

#endif  // voice_rec_h