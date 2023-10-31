#include <Audio.h>
#include <SDHCI.h>
#include <stdio.h>
#include <time.h>

#include "voice_rec.h"



#define DEBUG

bool is_debug(void) {
#ifdef DEBUG
    return true;
#endif
    return false;
}
void print_debug(const char *str) {
    if (is_debug()) {
        Serial.println(str);
    }
}

int get_EraTime(void) {
    time_t now = time(NULL);
    struct tm *pnow = localtime(&now);
    int EraTime = pnow->tm_hour * 100 + pnow->tm_min + pnow->tm_sec;
    return EraTime;
}

SDClass theSD;
AudioClass *theAudio;

File myFile;

//  * When audio internal error occurs, this function will be called back.
bool ErrEnd = false;
static void audio_attention_cb(const ErrorAttentionParam *atprm) {
    puts("Attention!");

    if (atprm->error_code >= AS_ATTENTION_CODE_WARNING) {
        ErrEnd = true;
    }
}

void check_sd_card(void) {
    /* Initialize SD */
    while (!theSD.begin()) {
        /* wait until SD card is mounted. */
        Serial.println("Insert SD card.");
    }
}

void init_theAudio(void) {
    // start audio system
    theAudio = AudioClass::getInstance();
    theAudio->begin(audio_attention_cb);
    printf("initialization Audio Library");

    /* Set clock mode to normal */
    theAudio->setRenderingClockMode(AS_CLKMODE_NORMAL);

    /* Set output device to speaker with first argument.
       AS_SETPLAYER_OUTPUTDEVICE_SPHP:
       Set speaker driver mode to LineOut with second argument.
       AS_SP_DRV_MODE_LINEOUT:
     */
    theAudio->setPlayerMode(AS_SETPLAYER_OUTPUTDEVICE_SPHP,
                            AS_SP_DRV_MODE_LINEOUT);

    /* Set init parameter
    - "id": This parameter allows you to select the Player ID. In this sample,
        AudioClass::Player0 is used.

    - "codec": You specify the codec type, which is MP3 in this
    case(AS_CODECTYPE_MP3).

    - "codec_path": This parameter specifies the location where the DSP file is
    installed. If you are using a microSD card, you would typically
    specify"/mnt/sd0/BIN," and if you are using SPI-Flash, you would specify
    "/mnt/spif/BIN."

    - "fs": This parameter is used to specify the sampling rate.
    For MP3, it upports sampling rates like 32kHz (AS_SAMPLINGRATE_32000),
    44.1kHz AS_SAMPLINGRATE_44100),
    and 48kHz (AS_SAMPLINGRATE_48000).
    You can also use S_SAMPLINGRATE_AUTO to automatically detect the sampling
    rate.

    - "channel": You specify whether the audio is in mono (AS_CHANNEL_MONO) or
    stereo (AS_CHANNEL_STEREO).
     */
    err_t err = theAudio->initPlayer(AudioClass::Player0, AS_CODECTYPE_MP3,
                                     "/mnt/sd0/BIN", AS_SAMPLINGRATE_AUTO,
                                     AS_CHANNEL_STEREO);

    /* Verify player initialize */
    if (err != AUDIOLIB_ECODE_OK) {
        printf("Player0 initialize error\n");
        exit(1);
    }
}

void open_VoiceFile(char* mp3_file_path) {
    check_sd_card();

    /* Open file placed on SD card */
    myFile = theSD.open(MP3_FILE_NAME);

    /* Verify file open */
    if (!myFile) {
        printf("File open error\n");
        exit(1);
    }
    printf("Open! 0x%08lx\n", (uint32_t)myFile);

    /* Send first frames to be decoded */
    err_t err = theAudio->writeFrames(AudioClass::Player0, myFile);

    if ((err != AUDIOLIB_ECODE_OK) && (err != AUDIOLIB_ECODE_FILEEND)) {
        printf("File Read Error! =%d\n", err);
        myFile.close();
        exit(1);
    }
    printf("Seccess: open_VoiceFile()\n");
}

void init_VoiceRec(char* mp3_file_path) {
    init_theAudio();
    open_VoiceFile(mp3_file_path);

    /* Main volume set to -16.0 dB */
    theAudio->setVolume(MP3_VOLUME);
    theAudio->startPlayer(AudioClass::Player0);
}

void stop_player() {
    theAudio->stopPlayer(AudioClass::Player0);
    // myFile.close();
    theAudio->setReadyMode();
    theAudio->end();
}

// static const int32_t stop_time = MAX_PLAY_TIME;
void play_Voice(int stop_time) {
    int init_eratime = get_EraTime();
    while (get_EraTime() - init_eratime < stop_time) {
        /* Send new frames to decode in a loop until file ends */
        int err = theAudio->writeFrames(AudioClass::Player0, myFile);

        /*  Tell when player file ends */
        if (err == AUDIOLIB_ECODE_FILEEND) {
            printf("Main player File End!\n");
            stop_player();
            return;
        }

        /* Show error code from player and stop */
        if (err | ErrEnd) {
            printf("Main player error code: %d\n", err);
            stop_player();
            exit(1);
        }
        usleep(40000);
    }
    /* defalt */
    printf("Main player Stop Time!\n");
    stop_player();
}


/**REC**/
// static const int32_t recoding_time = RECORD_TIME;
// static const int32_t recoding_bitrate = 96000;
// static const int32_t recoding_byte_per_second = (recoding_bitrate / 8);  // Bytes per second
// static const int32_t recoding_size = recoding_byte_per_second * recoding_time;  // Total recording size


