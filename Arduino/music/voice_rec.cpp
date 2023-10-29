#include <Audio.h>
#include <SDHCI.h>

#include "audio_player.h"

#define DEBUG


bool is_debug() {
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

SDClass theSD;
AudioClass *theAudio;

File myFile;

bool ErrEnd = false;

void check_sd_card() {
    /* Initialize SD */
    while (!theSD.begin()) {
        /* wait until SD card is mounted. */
        Serial.println("Insert SD card.");
    }
}

void init_theAudio() {

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

    err_t err = theAudio->initPlayer(AudioClass::Player0, AS_CODECTYPE_MP3,
                                     "/mnt/sd0/BIN", AS_SAMPLINGRATE_AUTO,
                                     AS_CHANNEL_STEREO);

    /* Verify player initialize */
    if (err != AUDIOLIB_ECODE_OK) {
        printf("Player0 initialize error\n");
        exit(1);
    }
}

void open_VoiceFile() {
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

void init_VoiceRec(){
    init_rheAudio();
    open_VoiceFile();

    /* Main volume set to -16.0 dB */
    theAudio->setVolume(MP3_VOLUME);
    theAudio->startPlayer(AudioClass::Player0);
}