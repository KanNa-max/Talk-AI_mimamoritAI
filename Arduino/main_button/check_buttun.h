#ifndef _check_button_h
#define _check_button_h


// enum momamori_sw : int {
//     SW_1, // ボタン１
//     SW_2, // ボタン２
//     SW_3, // ボタン３
//     SW_4, // ボタン４
// }
extern bool sw1_sts;
extern bool sw2_sts;
extern bool sw3_sts;
extern bool sw4_sts;

void init_buttom();
void init_LED();
void is_sw();


#endif // _check_button_h