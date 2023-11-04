#ifndef _main_h
#define _main_h

enum mimamori_mode : int {
    MIMAMORI_INIT,  // 初期状態
    MIMAMORI_MONITOR,  // みまもり状態
    MIMAMORI_CAMERA,  // カメラ撮影，推論
    MIMAMORI_SEND2ELTRES,  //payloadの更新，ELTRES通信
    MIMAMORI_ALARM,  // mp3音源の再生
    MIMAMORI_ISVOICE,  // マイクから人の声が聞こえているかどうか
    MIMAMORI_START,  // みまもり開始
};

#endif // _main_h