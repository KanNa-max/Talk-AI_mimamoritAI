#ifndef _clip_eltres_h
#define _clip_eltres_h
#include <stdint.h>
#include <Arduino.h>
#include <EltresAddonBoard.h>

/* Program Status */
#define PROGRAM_STS_INIT (0)     // プログラム内部状態：初期状態
#define PROGRAM_STS_RUNNING (1)  // プログラム内部状態：起動中
#define PROGRAM_STS_STOPPING (2)  //  プログラム内部状態：終了処理中
#define PROGRAM_STS_STOPPED (3)  // プログラム内部状態：終了

typedef struct {
    int ch_no;
    float trriger_1;
    float trriger_2;
} Payload;
extern Payload input_payload;

/* Function Declarations */
// void eltres_event_cb(eltres_board_event event);
// void gga_event_cb(const eltres_board_gga_info *gga_info);
// int get_gnss_time(void);
void init_eltres(void);
// void send_eltres(void);
void setup_payload(void);

/* Global Variables */
extern int program_sts;

extern bool gnss_recevie_timeout;
extern bool event_send_ready;
extern bool event_idle;

extern uint64_t last_change_blink_time;
extern uint8_t payload[16];
extern eltres_board_gga_info last_gga_info;


#endif // _clip_eltres_h