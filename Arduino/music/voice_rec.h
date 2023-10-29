#ifdef voice_rec_h
#define voice_rec_h

#define MP3_FILE_NAME "Sound.mp3"
#define RECORD_FILE_NAME "Sound_record.mp3"
#define MAX_PLAY_TIME 10  // seconds
#define RECORD_TIME 5     // seconds

#define MP3_VOLUME -160
/*The "volume" parameter allows you to adjust the volume within a range of -
    1020 to 120. The specified value is represented in 1
        10th of a decibel(dB).For example,
    if you set it to 15,
    the actual volume will be 1.5 dB.
*/

// 曜日を表す列挙型
extern enum status_voice {
    PLAY,    // 0
    REC,    // 1
    ELSE,   // 2
};

// extern SDClass theSD;
// extern AudioClass *theAudio;
// extern File myFile;
// extern bool ErrEnd;

void setupAudioPlayer();
void loopAudioPlayer();

#endif  // voice_rec_h