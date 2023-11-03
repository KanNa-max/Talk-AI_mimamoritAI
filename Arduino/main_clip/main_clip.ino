#include <stdio.h>
#include "check_gnss.h"

// #include <Arduino.h>  // Arduino基本ヘッダーファイル




void setup() { init_eltres(); }

void loop() {
    input_payload.ch_no = 5;
    input_payload.trriger_1 = 1.23;
    input_payload.trriger_2 = 3.54;
    send_eltres();
    sleep(3000);
    Serial.println("Wait for a time");
}