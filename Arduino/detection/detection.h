/*
detection(int count) : 引数には何。回sleepを検知したときに終了するか(10回以内)。
ex)num = 5,二秒ごとに状態検知(sleep or awake)し、5回sleepと検知されたときにプログラムを終了する。
キューで前十回分だけ保存する。

calibration() : カメラの位置を決めるための初期設定。背景の認識→顔の認識
背景を映した時に、顔として誤認識されないかをチェック。
顔を映した時に、顔としてにんしきされるかをチェック。 
detection()と違い,count数は5で固定している（変更可）。

 */

#include <SDHCI.h>
#include <DNNRT.h>
#include <stdio.h>
#include <Camera.h>

//キューに格納できるデータ数+1
#define MAX_NUM (10+1)
//sleepのカウント数
#define sleepNumber 5

// AIモデルへの入力サイズ
#define INPUT_WIDTH             (28)
#define INPUT_HEIGHT            (28)
// 拡縮比率（２のべき乗）
#define INPUT_RESIZE_RATIO      (2)
// 撮影サイズ
#define PICTURE_WIDTH           (CAM_IMGSIZE_QVGA_H)
#define PICTURE_HEIGTH          (CAM_IMGSIZE_QVGA_V)
// 切り取りサイズ
#define PICTURE_CLIP_WIDTH      (INPUT_WIDTH * INPUT_RESIZE_RATIO)
#define PICTURE_CLIP_HEIGHT     (INPUT_HEIGHT * INPUT_RESIZE_RATIO)

// drawBoxでしかつかわない
#define CAM_CLIP_W 112 //DNN_IMGのn倍であること(clipAndResizeImageByHWの制約)
#define CAM_CLIP_H 112 //DNN_IMGのn倍であること(clipAndResizeImageByHWの制約)

#define CAM_CLIP_X 104
#define CAM_CLIP_Y 64

// x,y方向にどれだけ分割して学習させるか
#define devided 3

#define LINE_THICKNESS 5

//TFT設定
#include <SPI.h>
#include "Adafruit_ILI9341.h"
/* LCD Settings */
#define TFT_RST 8
#define TFT_DC  9
#define TFT_CS  10

//　DNNRTクラスのインスタンス
extern DNNRT dnnrt;
// SDカードクラスのインスタンス
extern SDClass SD;
// AIモデルの入力データ用領域
extern DNNVariable dnn_input;
extern Adafruit_ILI9341 tft;

void detection(int lim_sleep);
void calibration();

void putStringOnLcd(String str, int color);
void drawBox(uint16_t* imgBuf);


/* キュー構造体 awake:0, sleep:1 */
typedef struct QUEUE {
    /* データの最後尾 */
    int tail;
    /* データの先頭 */
    int head;
    /* キューされているデータ */
    int data[MAX_NUM];
} QUEUE_T;


void initQueue(QUEUE_T *queue);
void enqueue(QUEUE_T *queue, int input);
void dequeue(QUEUE_T *queue);
int getQueueSize(const QUEUE_T *queue);
int countInQueue(const QUEUE_T *queue, int target);
