#include <Arduino.h>
#include "check_buttun.h"

const int BUTTON_1 = 4;  // SW4
const int BUTTON_2 = 5;  // SW5
const int BUTTON_3 = 6;  // SW6
const int BUTTON_4 = 7;  // SW7
bool sw1_sts = false;
bool sw2_sts = false;
bool sw3_sts = false;
bool sw4_sts = false;

void init_buttom() {
    /** buttom */
    pinMode(BUTTON_1, INPUT_PULLUP);
    pinMode(BUTTON_2, INPUT_PULLUP);
    pinMode(BUTTON_3, INPUT_PULLUP);
    pinMode(BUTTON_4, INPUT_PULLUP);
}
void init_LED() {
    /** LED **/
    pinMode(LED0, OUTPUT);
    pinMode(LED1, OUTPUT);
    pinMode(LED2, OUTPUT);
    pinMode(LED3, OUTPUT);
    digitalWrite(LED0, LOW);
    digitalWrite(LED1, LOW);
    digitalWrite(LED2, LOW);
    digitalWrite(LED3, LOW);
}

bool _is_sw(int sw_no) {
    int sw_sts = digitalRead(sw_no);
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
