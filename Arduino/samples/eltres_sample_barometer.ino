/*
 *  eltres_sample_barometer.ino - ローム Add-On Board SPRESENSE-SENSOR-EVK-701 上の気圧センサ BM1383AGLV での
 *                                気圧情報を送信するサンプルプログラム
 *  Copyright 2022 CRESCO LTD.
 */
/**
 * 説明
 *   本プログラムは、ローム Add-On Board SPRESENSE-SENSOR-EVK-701 に搭載された気圧センサ BM1383AGLV を使って、
 *   気圧をを取得し、その値をCLIP Viewer Liteの気圧圧力照度距離ペイロードのフォーマット仕様に合わせてELTRES送信します。
 *   ELTRESエラー、センサ初期設定エラー時を除き、初回GNSS電波受信後にELTRES送信を開始し継続します。
 *   気圧圧力照度距離ペイロードに設定した値はシリアルモニタで確認することができます。
 *   また、ELTRESエラー時、センサ初期設定エラーを除き、100ミリ秒毎に取得した気圧をシリアルモニタへ出力します。
 *   ELTRESエラー、センサ初期設定エラー時は異常終了状態でループします。
 *   LEDの動きについては補足事項を確認してください。
 * 
 * 動作環境
 *   Arduino IDEを利用するSpresense Arduino 開発環境(2.3.0)上で利用できます。
 *   以下のArduinoライブラリを利用します。
 *     ・ELTRESアドオンボード用ライブラリ(1.1.0)
 *     ・Arduino library for BM1383AGLV
 *       https://github.com/RohmSemiconductor/Arduino
 * 
 * 利用方法
 *   1. ELTRESアドオンボード用ライブラリをArduino IDEにインストールします。
 *   2. Arduino library for BM1383AGLV をArduino IDEにインストールします。
 *      以下の手順でインストールできます。
 *      2-1. 以下のローム製センサのArduino用ライブラリのURLにアクセスし、ファイルを取得します。
 *             https://github.com/RohmSemiconductor/Arduino
 *           緑色の「Code」ボタンをクリックし、「Download ZIP」をクリックすることでダウンロードできます。
 *      2-2. ダウンロードしたファイルをZIP展開します。
 *      2-3. 展開したフォルダ内に、フォルダ「BM1383AGLV」が含まれています。
 *           このフォルダをZIP圧縮します。
 *           ※Windows10では、フォルダを右クリックし、
 *            「送信」-> 「圧縮 (zip 形式) フォルダー」と選択することで、 ZIP圧縮できます。
 *      2-4. Arduino IDEの上部メニューから「スケッチ」->「.ZIP形式のライブラリをインストール」を選択します。
 *      2-5. ファイル選択用のウインドウが起動するので、先ほどZIP圧縮により作成したZIPファイルを選択し、
 *           「開く」をクリックします。
 *   3. 本プログラムをArduino IDEで開き、Spresenseに書き込みます。
 *   4. 電源を入れてプログラムを起動します。
 *      シリアルモニタを確認したい場合、Arduino IDEと接続し、
 *      Arduino IDEの上部メニューから「ツール」->「シリアルモニタ」を選択し、シリアルモニタを起動します。
 *      ・気圧圧力照度距離ペイロードに設定した値は"[setup_payload_barometer]"から始まる出力で確認できます。
 *      ・取得した気圧は"[measure]"から始まる出力で確認できます。
 * 
 * 注意事項
 *   本プログラムは、ELTRES送信間隔が1分であることを前提としてます。
 *   3分契約の場合、EltresAddonBoard.begin()の引数 ELTRES_BOARD_SEND_MODE_1MIN を
 *   ELTRES_BOARD_SEND_MODE_3MIN に変更してください。
 * 
 * 補足事項
 *   本プログラムは、下記のようにLEDを用いて状態を表現します。
 *     LED0：[プログラム状態] 起動中：点灯、正常・異常終了：消灯
 *     LED1：[GNSS電波状態] 受信中：点灯、未受信：消灯
 *     LED2：[ELTRES状態] 送信中：点灯、待機中：消灯
 *     LED3：[エラー状態] GNSS受信エラー：点滅、ELTRESエラー、センサ初期設定エラー：点灯、エラー無し：消灯
 *           ※GNSS受信エラー発生時はGNSS電波受信するまで待ち続けます。（LED0は点灯、LED3は点滅）
 *           ※ELTRESエラー、センサ初期設定エラー発生時は異常終了状態でループします。（LED0は消灯、LED3は点灯）
 * 
 * 製品情報
 *   ローム Add-On Board SPRESENSE-SENSOR-EVK-701
 *   https://www.rohm.co.jp/support/spresense-add-on-board
 * 
 * 参考情報
 *   気圧センサ（BM1383AGLV 気圧センサIC）の詳細仕様については下記を参照してください。
 *     https://www.rohm.co.jp/products/sensors-mems/pressure-sensor-ics/bm1383aglv-product
 */
#include <EltresAddonBoard.h>

#include <Wire.h>
// BM1383AGLV用ライブラリのインクルード
#include <BM1383AGLV.h>

// PIN定義：LED(プログラム状態)
#define LED_RUN PIN_LED0
// PIN定義：LED(GNSS電波状態)
#define LED_GNSS PIN_LED1
// PIN定義：LED(ELTRES状態)
#define LED_SND PIN_LED2
// PIN定義：LED(エラー状態)
#define LED_ERR PIN_LED3

// プログラム内部状態：初期状態
#define PROGRAM_STS_INIT      (0)
// プログラム内部状態：起動中
#define PROGRAM_STS_RUNNING   (1)
// プログラム内部状態：終了
#define PROGRAM_STS_STOPPED   (3)

// プログラム内部状態
int program_sts = PROGRAM_STS_INIT;
// GNSS電波受信タイムアウト（GNSS受信エラー）発生フラグ
bool gnss_recevie_timeout = false;
// 点滅処理で最後に変更した時間
uint64_t last_change_blink_time = 0;
// イベント通知での送信直前通知（5秒前）受信フラグ
bool event_send_ready = false;
// ペイロードデータ格納場所
uint8_t payload[16];

// BM1383AGLV用インスタンス
BM1383AGLV bm1383aglv;
// 最新値（気圧）
float last_pressure = 0;

/**
 * @brief イベント通知受信コールバック
 * @param event イベント種別
 */
void eltres_event_cb(eltres_board_event event) {
  switch (event) {
  case ELTRES_BOARD_EVT_GNSS_TMOUT:
    // GNSS電波受信タイムアウト
    Serial.println("gnss wait timeout error.");
    gnss_recevie_timeout = true;
    break;
  case ELTRES_BOARD_EVT_IDLE:
    // アイドル状態
    Serial.println("waiting sending timings.");
    digitalWrite(LED_SND, LOW);
    break;
  case ELTRES_BOARD_EVT_SEND_READY:
    // 送信直前通知（5秒前）
    Serial.println("Shortly before sending, so setup payload if need.");
    event_send_ready = true;
    break;
  case ELTRES_BOARD_EVT_SENDING:
    // 送信開始
    Serial.println("start sending.");
    digitalWrite(LED_SND, HIGH);
    break;
  case ELTRES_BOARD_EVT_GNSS_UNRECEIVE:
    // GNSS電波未受信
    Serial.println("gnss wave has not been received.");
    digitalWrite(LED_GNSS, LOW);
    break;
  case ELTRES_BOARD_EVT_GNSS_RECEIVE:
    // GNSS電波受信
    Serial.println("gnss wave has been received.");
    digitalWrite(LED_GNSS, HIGH);
    gnss_recevie_timeout = false;
    break;
  case ELTRES_BOARD_EVT_FAULT:
    // 内部エラー発生
    Serial.println("internal error.");
    break;
  }
}

/**
 * @brief setup()関数
 */
void setup() {
  // シリアルモニタ出力設定
  Serial.begin(115200);

  // LED初期設定
  pinMode(LED_RUN, OUTPUT);
  digitalWrite(LED_RUN, HIGH);
  pinMode(LED_GNSS, OUTPUT);
  digitalWrite(LED_GNSS, LOW);
  pinMode(LED_SND, OUTPUT);
  digitalWrite(LED_SND, LOW);
  pinMode(LED_ERR, OUTPUT);
  digitalWrite(LED_ERR, LOW);

  // ELTRES起動処理
  eltres_board_result ret = EltresAddonBoard.begin(ELTRES_BOARD_SEND_MODE_1MIN,eltres_event_cb, NULL);
  if (ret != ELTRES_BOARD_RESULT_OK) {
    // ELTRESエラー発生
    digitalWrite(LED_RUN, LOW);
    digitalWrite(LED_ERR, HIGH);
    program_sts = PROGRAM_STS_STOPPED;
    Serial.print("cannot start eltres board (");
    Serial.print(ret);
    Serial.println(").");
    return;
  }

  // 気圧センサ初期設定
  Wire.begin();
  byte rc;
  rc = bm1383aglv.init();
  if (rc != 0) {
    // センサ初期設定エラー
    EltresAddonBoard.end();
    digitalWrite(LED_RUN, LOW);
    digitalWrite(LED_ERR, HIGH);
    program_sts = PROGRAM_STS_STOPPED;
    Serial.print("cannnot start sensor.(");
    Serial.print(rc);
    Serial.println(")");
    return;
  }
  // 正常
  program_sts = PROGRAM_STS_RUNNING;
}

/**
 * @brief loop()関数
 */
void loop() {

  switch (program_sts) {
    case PROGRAM_STS_RUNNING:
      // プログラム内部状態：起動中
      if (gnss_recevie_timeout) {
        // GNSS電波受信タイムアウト（GNSS受信エラー）時の点滅処理
        uint64_t now_time = millis();
        if ((now_time - last_change_blink_time) >= 1000) {
          last_change_blink_time = now_time;
          bool set_value = digitalRead(LED_ERR);
          bool next_value = (set_value == LOW) ? HIGH : LOW;
          digitalWrite(LED_ERR, next_value);
        }
      } else {
        digitalWrite(LED_ERR, LOW);
      }

      if (event_send_ready) {
        // 送信直前通知時の処理
        event_send_ready = false;
        setup_payload_barometer(last_pressure);
        // 送信ペイロードの設定
        EltresAddonBoard.set_payload(payload);
      }

      // 気圧センサから測定値を取得し、最新値を更新
      measure_barometer();
      break;
     
    case PROGRAM_STS_STOPPED:
      // プログラム内部状態：終了
      break;
  }

  // 次のループ処理まで100ミリ秒待機
  delay(100);
}

/**
 * @brief 気圧圧力照度距離ペイロード設定（気圧のみ利用）
 * @param pressure 気圧
 */
void setup_payload_barometer(float pressure) {

  // 設定情報をシリアルモニタへ出力
  Serial.print("[setup_payload_barometer]");
  Serial.print("pressure:");
  Serial.print(pressure, 6);
  Serial.print(" hPa");
  Serial.println();

  // ペイロード領域初期化
  memset(payload, 0x00, sizeof(payload));
  // ペイロード種別[気圧圧力照度距離ペイロード]設定
  payload[0] = 0x85;
  // 気圧
  uint32_t raw;
  raw = *((uint32_t*)&pressure);
  payload[1] = (uint8_t)((raw >> 24) & 0xff);
  payload[2] = (uint8_t)((raw >> 16) & 0xff);
  payload[3] = (uint8_t)((raw >> 8) & 0xff);
  payload[4] = (uint8_t)((raw >> 0) & 0xff);
}

/**
 * @brief 気圧センサから気圧を取得し、最新値を更新
 */
void measure_barometer() {
  byte rc;
  float pressure, temp;

  // 気圧センサから測定値を取得
  rc = bm1383aglv.get_val(&pressure, &temp);
  if (rc != 0) {
    Serial.print("cannot read measurement (");
    Serial.print(rc);
    Serial.println(")");
    return;
  }
  // 最新値の更新
  last_pressure = pressure;

  // 最新値をシリアルモニタへ出力
  Serial.print("[measure]pressure:");
  Serial.print(last_pressure, 6);
  Serial.print(" hPa");
  Serial.println();
}