#ifndef _main_h
#define _main_h

enum mimamori_mode : int {
    MIMAMORI_INIT,  // 初期状態
    MIMAMORI_MONITOR,  // みまもり状態
    MIMAMORI_CAMERA,  // カメラ撮影，推論
    MIMAMORI_SEND2ELTRES,  //payloadの更新，ELTRES通信
    MIMAMORI_CHECK,  // みまもり状態の確認
    MIMAMORI_ALARM,  // 目覚まし音再生
    MIMAMORI_ISVOICE,  // マイクから人の声が聞こえているかどうか
    MIMAMORI_START,  // みまもり開始
};

enum mimamori_ch : int {
    STATE_CH_AWAKE,  // 起きている
    STATE_CH_CHECK,  // 起きているが，異常がある
    STATE_CH_ALARM,  // 異常がある
};

#endif // _main_h