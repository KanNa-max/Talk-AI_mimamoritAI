#ifndef _arduino_h
#define _arduino_h
#include <Arduino.h>
#endif
#include "clip_eltres.h"

/* Program Status */
int program_sts = PROGRAM_STS_INIT;

/* Flagment */
bool gnss_recevie_timeout =
  false;                        // GNSS電波受信タイムアウト（GNSS受信エラー）発生フラグ
bool event_send_ready = false;  // ELTRES送信準備完了フラグ
bool event_idle = false;        // ELTRES送信アイドル状態フラグ

/* LED Blink and Communication Status */
uint64_t last_change_blink_time = 0;  // 点滅処理で最後に変更した時間
int send_count = 0;
uint8_t payload[16];                  // ペイロードデータ格納場所
eltres_board_gga_info last_gga_info;  // 最新のGGA情報
Payload input_payload;
void setup_payload(Payload payload);

/**
 * @brief イベント通知受信コールバック
 * @param event イベント種別
 */
void eltres_event_cb(eltres_board_event event) {
  Serial.print("[eltres]");
  switch (event) {
    case ELTRES_BOARD_EVT_GNSS_TMOUT:
      // GNSS電波受信タイムアウト
      Serial.println("gnss wait timeout error.");
      gnss_recevie_timeout = true;
      break;
    case ELTRES_BOARD_EVT_IDLE:
      // アイドル状態
      Serial.println("waiting sending timings.");
      event_idle = true;
      break;
    case ELTRES_BOARD_EVT_SEND_READY:
      // 送信直前通知（5秒前）
      Serial.println("Shortly before sending, so setup payload if need.");
      event_send_ready = true;
      break;
    case ELTRES_BOARD_EVT_SENDING:
      // 送信開始
      Serial.println("start sending.");
      break;
    case ELTRES_BOARD_EVT_GNSS_UNRECEIVE:
      // GNSS電波未受信
      Serial.println("gnss wave has not been received.");
      break;
    case ELTRES_BOARD_EVT_GNSS_RECEIVE:
      // GNSS電波受信
      Serial.println("gnss wave has been received.");
      gnss_recevie_timeout = false;
      break;
    case ELTRES_BOARD_EVT_FAULT:
      // 内部エラー発生
      Serial.println("internal error.");
      break;
  }
}

/**
 * @brief GGA情報受信コールバック
 * @param gga_info GGA情報のポインタ
 */
void gga_event_cb(const eltres_board_gga_info *gga_info) {
  // Serial.print("[gga]");
  last_gga_info = *gga_info;
  /* 測位状態 (GGA情報出力) */
  if (gga_info->m_pos_status) {
    Serial.print("utc: ");
    Serial.print((const char *)gga_info->m_utc);  // UTC(hhmmss.ss形式)
    // ----------------------------------------
    Serial.print(", lat: ");
    Serial.print(
      (const char *)gga_info->m_n_s);             // 北緯 / 南緯(N: 北緯, S: 南緯)
    Serial.print((const char *)gga_info->m_lat);  // 緯度(dddmm.mmmm形式)
    Serial.print(", lon: ");
    Serial.print(
      (const char *)gga_info->m_e_w);               // 東経 / 西経(E: 東経, W: 西経)
    Serial.print((const char *)gga_info->m_lon);  // 経度(dddmm.mmmm形式)
    // ----------------------------------------
    Serial.print(", pos_status: ");
    Serial.print(gga_info->m_pos_status);
    // GPS 位置品質指標
    //   0: 非測位状態
    //   1: 測位状態
    //   2: 測位状態，かつ DGPS による補正が有効
    //   6: DR (Dead Reckoning)による自律測位
    Serial.print(", sat_used: ");
    Serial.print(gga_info->m_sat_used);  // 使用衛星数
    // ----------------------------------------
    Serial.print(", hdop: ");
    Serial.print(gga_info->m_hdop);  // HDOP (水平精度低下度)
    Serial.print(", height: ");
    Serial.print(gga_info->m_height);  // 標高、単位:m
    Serial.print(" m, geoid: ");
    Serial.print(gga_info->m_geoid);  // ジオイド高、単位:m
    Serial.println(" m");
  }
  /* 非測位状態 */
  else {
    // Serial.println("invalid data.");
  }
}

/**
 * @brief プログラム終了処理
 */
void init_eltres(void) {
  /* ELTRES: boot_process */
  eltres_board_result ret = EltresAddonBoard.begin(
    ELTRES_BOARD_SEND_MODE_1MIN, eltres_event_cb, gga_event_cb);
  if (ret != ELTRES_BOARD_RESULT_OK) {
    // ELTRESエラー発生
    program_sts = PROGRAM_STS_STOPPED;
    Serial.print("cannot start eltres board (");
    Serial.print(ret);
    Serial.println(").");
  } else {
    // 正常
    program_sts = PROGRAM_STS_RUNNING;
    Serial.println("Success: init_eltres");
  }
}

// void send_eltres(void) {
//   uint32_t gnss_time;
//   int32_t remaining;
//   bool temp_f = true;
//   while (temp_f) {
//     switch (program_sts) {
//       /* プログラム内部状態：初期状態 */
//       case PROGRAM_STS_RUNNING:
//         /*  GNSS電波受信タイムアウト（GNSS受信エラー） */
//         if (gnss_recevie_timeout) {
//           uint64_t now_time =
//             millis();  // 実行してからの経過時間をミリ秒で取得
//           if ((now_time - last_change_blink_time) >= 1000) {
//             last_change_blink_time = now_time;
//           }
//         }
//         /* 送信直前通知時の処理, 送信ペイロードの設定*/
//         if (event_send_ready) {
//           event_send_ready = false;
//           send_count += 1;
//           setup_payload(input_payload);
//           EltresAddonBoard.set_payload(payload);
//         }
//         /* 次の送信に向けた準備 */
//         if (event_idle) {
//           event_idle = false;

//           // if (send_count > 2) {
//           //   // 規定回数
//           //   program_sts = PROGRAM_STS_STOPPING;
//           //   Serial.println("stop sending.");
//           //   break;
//           // }
//           // GNSS時刻(epoch秒)の取得
//           Serial.println("----------");
//           EltresAddonBoard.get_gnss_time(&gnss_time);
//           Serial.print("gnss time: ");
//           Serial.print(gnss_time);
//           Serial.println(" sec");
//           // 次送信までの残り時間の取得
//           EltresAddonBoard.get_remaing_time(&remaining);
//           Serial.print("remaining time: ");
//           Serial.print(remaining);
//           Serial.println(" sec");
//         }
//         /* ELTRES停止 */
//         if (false) {  // 終了条件(未設定)
//           program_sts = PROGRAM_STS_STOPPING;
//           Serial.println("stop sending.");
//           break;
//         }
//         break;
//       // ----------------------------------------
//       /* プログラム内部状態：終了処理中 */
//       case PROGRAM_STS_STOPPING:
//         // ELTRES停止処理(注意：この処理を行わないとELTRESが自動送信し続ける)
//         EltresAddonBoard.end();
//         program_sts = PROGRAM_STS_STOPPED;
//         break;
//       // ----------------------------------------
//       /* プログラム内部状態：終了 */
//       case PROGRAM_STS_STOPPED:
//         break;
//     }
//     delay(3000);
//     Serial.print("Loop");
//   }
//   Serial.print("end_Loop");
// }


/**
 * @brief ペイロード設定
 */
void setup_payload() {
  int ch_no = input_payload.ch_no;
  float trriger_1 = input_payload.trriger_1;
  float trriger_2 = input_payload.trriger_2;

  uint32_t gnss_time;
  uint32_t utc_time;
  // GNSS時刻(epoch秒)の取得
  EltresAddonBoard.get_gnss_time(&gnss_time);
  // UTC時刻を計算（閏秒補正）
  utc_time = gnss_time - 18;

  // 設定情報をシリアルモニタへ出力
  Serial.print("[setup_payload_gps]");
  Serial.print("Ch.:");
  Serial.print(ch_no);
  Serial.print(",trrigee1:");
  Serial.print(trriger_1);
  Serial.print(",trriger2:");
  Serial.print(trriger_2);
  Serial.print(",utc:");
  Serial.print(utc_time);
  Serial.println();

  // ペイロード領域初期化
  memset(payload, 0x00, sizeof(payload));
  // ペイロード種別[トリガー検出ペイロード]設定
  payload[0] = 0x89;
  // CH 番号(0~255)
  payload[1] = (uint8_t)(ch_no & 0xff);
  // トリガー 1 の閾値(0~100)
  uint32_t raw;
  raw = *((uint32_t *)&trriger_1);
  payload[2] = (uint8_t)((raw >> 24) & 0xff);
  payload[3] = (uint8_t)((raw >> 16) & 0xff);
  payload[4] = (uint8_t)((raw >> 8) & 0xff);
  payload[5] = (uint8_t)((raw >> 0) & 0xff);
  // トリガー 2 の閾値(0~100)
  raw = *((uint32_t *)&trriger_2);
  payload[6] = (uint8_t)((raw >> 24) & 0xff);
  payload[7] = (uint8_t)((raw >> 16) & 0xff);
  payload[8] = (uint8_t)((raw >> 8) & 0xff);
  payload[9] = (uint8_t)((raw >> 0) & 0xff);
  // 時刻(EPOCH秒)設定
  payload[10] = (uint8_t)((utc_time >> 24) & 0xff);
  payload[11] = (uint8_t)((utc_time >> 16) & 0xff);
  payload[12] = (uint8_t)((utc_time >> 8) & 0xff);
  payload[13] = (uint8_t)((utc_time >> 0) & 0xff);
  // 拡張用領域(0固定)設定
  payload[14] = 0x00;
  payload[15] = 0x00;
}
