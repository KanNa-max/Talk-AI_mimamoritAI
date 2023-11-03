#include <stdio.h>
// #include "main.h"
#include "clip_eltres.h"

int temp_no = 0;
void main_mimamori() {
  temp_no ++;
  Serial.println("This is main_mimamori.");
  input_payload.ch_no = temp_no%255;
  input_payload.trriger_1 = 3.14;
  input_payload.trriger_2 = 1.414;
  delay(3000);
  
}

void setup() {
  init_eltres();
}

void loop() {
  switch (program_sts) {
    case PROGRAM_STS_RUNNING:
      // プログラム内部状態：起動中
      if (gnss_recevie_timeout) {
        // GNSS電波受信タイムアウト（GNSS受信エラー）
        uint64_t now_time = millis();
        if ((now_time - last_change_blink_time) >= 1000) {
          last_change_blink_time = now_time;
        }
      } else if (event_send_ready) {
        // 送信直前通知時の処理
        event_send_ready = false;
        setup_payload();
        EltresAddonBoard.set_payload(payload);
      }
      /* ELTRES停止 */
      if (false) {  // 終了条件(未設定)
        program_sts = PROGRAM_STS_STOPPING;
        Serial.println("stop sending.");
        break;
      }
      break;
    // ----------------------------------------
    /* プログラム内部状態：終了処理中 */
    case PROGRAM_STS_STOPPING:
      // ELTRES停止処理(注意：この処理を行わないとELTRESが自動送信し続ける)
      EltresAddonBoard.end();
      program_sts = PROGRAM_STS_STOPPED;
      break;
    // ----------------------------------------
    /* プログラム内部状態：終了 */
    case PROGRAM_STS_STOPPED:
      break;
  }
  main_mimamori();
  // 次のループ処理まで100ミリ秒待機
  delay(100);
}