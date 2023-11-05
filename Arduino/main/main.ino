#ifndef _arduino_h
#define _arduino_h
#include <Arduino.h>
#endif
#ifndef _stdio_h
#define _stdio_h
#include <stdio.h>
#endif

#include "clip_eltres.h"
#include "detection.h"
#include "main.h"
#include "sounddetector.h"


const int BUTTON_1 = 4;  // SW4
const int BUTTON_2 = 5;  // SW5
const int BUTTON_3 = 6;  // SW6
const int BUTTON_4 = 7;  // SW7

bool sw1_sts = false;
bool sw2_sts = false;
bool sw3_sts = false;
bool sw4_sts = false;

bool _is_sw(int sw_no) {
    const int sw_sts = digitalRead(sw_no);
    if (sw_sts == LOW) {
        return true;
    } else {
        return false;
    }
}

void is_sw() {
    sw1_sts = _is_sw(BUTTON_1);
    sw2_sts = _is_sw(BUTTON_2);
    sw3_sts = _is_sw(BUTTON_3);
    sw4_sts = _is_sw(BUTTON_4);
}

int mimamori_sts = MIMAMORI_INIT;
int sts_ch = STATE_CH_AWAKE;

uint64_t startTime = 0;
void main_mimamori() {
    Serial.print("[main_mimamori]");
    Serial.println("This is main_mimamori.");
    // input_payload.ch_no = temp_no % 255;
    // input_payload.trriger_1 = 3.14;
    // input_payload.trriger_2 = 1.414;
    delay(500);
    switch (mimamori_sts) {
        case MIMAMORI_START:
            /* code */
            calibration();
            mimamori_sts = MIMAMORI_MONITOR;
            break;
        case MIMAMORI_MONITOR:
            /* code */
            mimamori_sts = MIMAMORI_CAMERA;
            break;
        case MIMAMORI_CAMERA:
            /* nokura code */
            detection();
            Serial.println("detection");
            /* detect "sleep", threshold: 5>(/10) */
            if (countInQueue(&queue, 1) > 5) {
                initQueue(&queue);

                startTime = millis();
                mimamori_sts = MIMAMORI_CHECK;
            }
            // mimamori_sts = MIMAMORI_SEND2ELTRES;
            break;

        case MIMAMORI_CHECK:
            Serial.println("[mimamori]sts: MIMAMORI_CHECK");
            /* code */

            /* Wait for reaction for 10 seconds */
            startTime = millis();
            if (millis() - startTime < 10 * 1000) {
                sts_ch = STATE_CH_CHECK;
                Serial.println("[mimamori]Waiting for your action.");
                // TODO (がんばてね音声再生)
                is_sw();
                delay(100);
                if (sw1_sts) {  // reaction(pushed sw1)
                    Serial.println("[mimamori]sw1_sts: pushed");
                    mimamori_sts = MIMAMORI_MONITOR;
                    break;
                }
            }
            // endloop
            else {
                mimamori_sts = MIMAMORI_ALARM;
                break;
            }

        case MIMAMORI_ALARM:
            Serial.println("[mimamori]sts: MIMAMORI_ALARM");
            // while (true) {
            sts_ch = STATE_CH_ALARM;
            Serial.println("[mimamori]Waiting for your action..");
            // Loop'(反応ない)
            // TODO (アラーム音声再生)
            is_sw();
            delay(100);
            if (sw1_sts) {  // reaction(pushed sw1)
                Serial.println("[mimamori]sw1_sts: pushed");
                mimamori_sts = MIMAMORI_MONITOR;
                // break;
            }
            // }
            break;
            // endloop
        case MIMAMORI_SEND2ELTRES:
            /* do not use (for debug) */
            input_payload.ch_no = 255;
            input_payload.trriger_1 = 3.14;
            input_payload.trriger_2 = 1.14;
            setup_payload();
            EltresAddonBoard.set_payload(payload);
            mimamori_sts = MIMAMORI_MONITOR;
            break;

        default:
            break;
    }
}

void setup() {
    /* init SerialMonitor */
    Serial.begin(115200);

    /** buttom */
    pinMode(BUTTON_1, INPUT_PULLUP);
    pinMode(BUTTON_2, INPUT_PULLUP);
    pinMode(BUTTON_3, INPUT_PULLUP);
    pinMode(BUTTON_4, INPUT_PULLUP);
    /** LED **/
    pinMode(LED0, OUTPUT);
    pinMode(LED1, OUTPUT);
    pinMode(LED2, OUTPUT);
    pinMode(LED3, OUTPUT);
    digitalWrite(LED0, LOW);
    digitalWrite(LED1, LOW);
    digitalWrite(LED2, LOW);
    digitalWrite(LED3, LOW);

   
    /* ELTRES: boot_process */
    init_eltres();

    /* load: AImodel */
    init_AImodel();

    /* init_camera */
    init_camera();

    mimamori_sts = MIMAMORI_START;
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
                input_payload.ch_no = sts_ch;
                input_payload.trriger_1 = 0.0;
                input_payload.trriger_2 = 0.0;
                setup_payload();
                EltresAddonBoard.set_payload(payload);
            }
            if (event_idle) {
                // 送信完了時の処理
                event_idle = false;
                sts_ch = STATE_CH_AWAKE;  // default_sts
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