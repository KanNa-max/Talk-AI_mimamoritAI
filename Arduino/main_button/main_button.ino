#include "check_buttun.h"

void setup() {
    /* init SerialMonitor */
    Serial.begin(115200);
    init_buttom();
    init_LED();
}

void loop() {
    is_sw();
    // delay(300);
    if (sw1_sts) {
        digitalWrite(LED0, HIGH);
        delay(300);
        digitalWrite(LED0, LOW);
    }
    if (sw2_sts) {
        digitalWrite(LED1, HIGH);
        delay(300);
        digitalWrite(LED1, LOW);
    }
    if (sw3_sts) {
        digitalWrite(LED2, HIGH);
        delay(300);
        digitalWrite(LED2, LOW);
    }
    if (sw4_sts) {
        digitalWrite(LED3, HIGH);
        delay(300);
        digitalWrite(LED3, LOW);
    }
    
}