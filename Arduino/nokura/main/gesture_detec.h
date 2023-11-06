
#include "ge.h"



extern uint8_t buffer[28 * 28] __attribute__((aligned(16)));


//   下の関数を使う前に入れる初期化関数、終わったら→dnnrt.end();いれる　detecも同様
void init_gesture();

// mainに書かない
bool gesture_detec();

// limに設定した回数（最大５）だけジェスチャーを認識したらtrue, 例）count_gesture(2);
bool count_gesture(int lim);
