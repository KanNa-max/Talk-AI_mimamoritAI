/*
 *  eltres_sample_gps.ino - GPS情報を送信するサンプルプログラム
 *  Copyright 2022 CRESCO LTD.
 */
/**
 * 説明
 *   本プログラムは、ELTRESアドオンボードからGPS情報(緯度、経度、日時)を取得し、
 *   その値をCLIP Viewer
 * LiteのGPSペイロードのフォーマット仕様に合わせてをELTRES送信します。
 *   ELTRESエラー時を除き、初回GNSS電波受信後にELTRES送信を開始し継続します。
 *   GPSペイロードに設定した値はシリアルモニタで確認することができます。
 *   また、ELTRESエラー時を除き、1秒毎にGGA情報をシリアルモニタへ出力します。
 *   ELTRESエラー時は異常終了状態でループします。
 *   LEDの動きについては補足事項を確認してください。
 *
 * 動作環境
 *   Arduino IDEを利用するSpresense Arduino 開発環境(2.3.0)上で利用できます。
 *   以下のArduinoライブラリを利用します。
 *     ・ELTRESアドオンボード用ライブラリ(1.1.0)
 *
 * 利用方法
 *   1. ELTRESアドオンボード用ライブラリをArduino IDEにインストールします。
 *   2. 本プログラムをArduino IDEで開き、Spresenseに書き込みます。
 *   3. 電源を入れてプログラムを起動します。
 *      シリアルモニタを確認したい場合、Arduino IDEと接続し、
 *      Arduino
 * IDEの上部メニューから「ツール」->「シリアルモニタ」を選択し、シリアルモニタを起動します。
 *      ・GPSペイロードに設定した値は"[setup_payload_gps]"から始まる出力で確認できます。
 *      ・GGA情報は"[gga]"から始まる出力で確認できます。
 *
 * 注意事項
 *   本プログラムは、ELTRES送信間隔が1分であることを前提としてます。
 *   3分契約の場合、EltresAddonBoard.begin()の引数 ELTRES_BOARD_SEND_MODE_1MIN
 * を ELTRES_BOARD_SEND_MODE_3MIN に変更してください。
 *
 * 補足事項
 *   本プログラムは、下記のようにLEDを用いて状態を表現します。
 *     LED0：[プログラム状態] 起動中：点灯、正常・異常終了：消灯
 *     LED1：[GNSS電波状態] 受信中：点灯、未受信：消灯
 *     LED2：[ELTRES状態] 送信中：点灯、待機中：消灯
 *     LED3：[エラー状態]
 * GNSS受信エラー：点滅、ELTRESエラー：点灯、エラー無し：消灯
 *           ※GNSS受信エラー発生時はGNSS電波受信するまで待ち続けます。（LED0は点灯、LED3は点滅）
 *           ※ELTRESエラー発生時は異常終了状態でループします。（LED0は消灯、LED3は点灯）
 */
#include <EltresAddonBoard.h>

// PIN定義：LED(プログラム状態)
#define LED_RUN PIN_LED0
// PIN定義：LED(GNSS電波状態)
#define LED_GNSS PIN_LED1
// PIN定義：LED(ELTRES状態)
#define LED_SND PIN_LED2
// PIN定義：LED(エラー状態)
#define LED_ERR PIN_LED3

// プログラム内部状態：初期状態
#define PROGRAM_STS_INIT (0)
// プログラム内部状態：起動中
#define PROGRAM_STS_RUNNING (1)
// プログラム内部状態：終了
#define PROGRAM_STS_STOPPED (3)

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
// 最新のGGA情報
eltres_board_gga_info last_gga_info;

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
 * @brief GGA情報受信コールバック
 * @param gga_info GGA情報のポインタ
 */
void gga_event_cb(const eltres_board_gga_info *gga_info) {
    Serial.print("[gga]");
    last_gga_info = *gga_info;
    if (gga_info->m_pos_status) {
        // 測位状態
        // GGA情報をシリアルモニタへ出力
        Serial.print("utc: ");
        Serial.println((const char *)gga_info->m_utc);
        Serial.print("lat: ");
        Serial.print((const char *)gga_info->m_n_s);
        Serial.print((const char *)gga_info->m_lat);
        Serial.print(", lon: ");
        Serial.print((const char *)gga_info->m_e_w);
        Serial.println((const char *)gga_info->m_lon);
        Serial.print("pos_status: ");
        Serial.print(gga_info->m_pos_status);
        Serial.print(", sat_used: ");
        Serial.println(gga_info->m_sat_used);
        Serial.print("hdop: ");
        Serial.print(gga_info->m_hdop);
        Serial.print(", height: ");
        Serial.print(gga_info->m_height);
        Serial.print(" m, geoid: ");
        Serial.print(gga_info->m_geoid);
        Serial.println(" m");
    } else {
        // 非測位状態
        // "invalid data"をシリアルモニタへ出力
        Serial.println("invalid data.");
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
    eltres_board_result ret = EltresAddonBoard.begin(
        ELTRES_BOARD_SEND_MODE_1MIN, eltres_event_cb, gga_event_cb);
    if (ret != ELTRES_BOARD_RESULT_OK) {
        // ELTRESエラー発生
        digitalWrite(LED_RUN, LOW);
        digitalWrite(LED_ERR, HIGH);
        program_sts = PROGRAM_STS_STOPPED;
        Serial.print("cannot start eltres board (");
        Serial.print(ret);
        Serial.println(").");
    } else {
        // 正常
        program_sts = PROGRAM_STS_RUNNING;
    }
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
                setup_payload_gps();
                // 送信ペイロードの設定
                EltresAddonBoard.set_payload(payload);
            }
            break;

        case PROGRAM_STS_STOPPED:
            // プログラム内部状態：終了
            break;
    }
    // 次のループ処理まで100ミリ秒待機
    delay(100);
}

/**
 * @brief GPSペイロード設定
 */
void setup_payload_gps() {
    String lat_string = String((char *)last_gga_info.m_lat);
    String lon_string = String((char *)last_gga_info.m_lon);
    int index;
    uint32_t gnss_time;
    uint32_t utc_time;

    // GNSS時刻(epoch秒)の取得
    EltresAddonBoard.get_gnss_time(&gnss_time);
    // UTC時刻を計算（閏秒補正）
    utc_time = gnss_time - 18;

    // 設定情報をシリアルモニタへ出力
    Serial.print("[setup_payload_gps]");
    Serial.print("lat:");
    Serial.print(lat_string);
    Serial.print(",lon:");
    Serial.print(lon_string);
    Serial.print(",utc:");
    Serial.print(utc_time);
    Serial.print(",pos:");
    Serial.print(last_gga_info.m_pos_status);
    Serial.println();

    // ペイロード領域初期化
    memset(payload, 0x00, sizeof(payload));
    // ペイロード種別[GPSペイロード]設定
    payload[0] = 0x81;
    // 緯度設定
    index = 0;
    payload[1] =
        (uint8_t)(((lat_string.substring(index, index + 1).toInt() << 4) +
                   lat_string.substring(index + 1, index + 2).toInt()) &
                  0xff);
    index += 2;
    payload[2] =
        (uint8_t)(((lat_string.substring(index, index + 1).toInt() << 4) +
                   lat_string.substring(index + 1, index + 2).toInt()) &
                  0xff);
    index += 2;
    index += 1;  // skip "."
    payload[3] =
        (uint8_t)(((lat_string.substring(index, index + 1).toInt() << 4) +
                   lat_string.substring(index + 1, index + 2).toInt()) &
                  0xff);
    index += 2;
    payload[4] =
        (uint8_t)(((lat_string.substring(index, index + 1).toInt() << 4) +
                   lat_string.substring(index + 1, index + 2).toInt()) &
                  0xff);
    // 経度設定
    index = 0;
    payload[5] =
        (uint8_t)(lon_string.substring(index, index + 1).toInt() & 0xff);
    index += 1;
    payload[6] =
        (uint8_t)(((lon_string.substring(index, index + 1).toInt() << 4) +
                   lon_string.substring(index + 1, index + 2).toInt()) &
                  0xff);
    index += 2;
    payload[7] =
        (uint8_t)(((lon_string.substring(index, index + 1).toInt() << 4) +
                   lon_string.substring(index + 1, index + 2).toInt()) &
                  0xff);
    index += 2;
    index += 1;  // skip "."
    payload[8] =
        (uint8_t)(((lon_string.substring(index, index + 1).toInt() << 4) +
                   lon_string.substring(index + 1, index + 2).toInt()) &
                  0xff);
    index += 2;
    payload[9] =
        (uint8_t)(((lon_string.substring(index, index + 1).toInt() << 4) +
                   lon_string.substring(index + 1, index + 2).toInt()) &
                  0xff);
    // 時刻(EPOCH秒)設定
    payload[10] = (uint8_t)((utc_time >> 24) & 0xff);
    payload[11] = (uint8_t)((utc_time >> 16) & 0xff);
    payload[12] = (uint8_t)((utc_time >> 8) & 0xff);
    payload[13] = (uint8_t)(utc_time & 0xff);
    // 拡張用領域(0固定)設定
    payload[14] = 0x00;
    // 品質設定
    payload[15] = last_gga_info.m_pos_status;
}
