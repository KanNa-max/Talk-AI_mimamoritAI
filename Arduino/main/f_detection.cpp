#include "detection.h"


void putStringOnLcd(String str, int color) {
  int len = str.length();
  tft.fillRect(0, 224, 320, 240, ILI9341_BLACK);
  tft.setTextSize(2);
  int sx = 160 - len / 2 * 12;
  if (sx < 0) sx = 0;
  tft.setCursor(sx, 225);
  tft.setTextColor(color);
  tft.println(str);
}

void drawBox(uint16_t* imgBuf) {

  for (int x = CAM_CLIP_X; x < CAM_CLIP_X + CAM_CLIP_W; ++x) {
    for (int n = 0; n < LINE_THICKNESS; ++n) {
      *(imgBuf + PICTURE_WIDTH * (CAM_CLIP_Y + n) + x)              = ILI9341_RED;
      *(imgBuf + PICTURE_WIDTH * (CAM_CLIP_Y + CAM_CLIP_H - 1 - n) + x) = ILI9341_RED;
    }
  }
  for (int y = CAM_CLIP_Y; y < CAM_CLIP_Y + CAM_CLIP_H; ++y) {
    for (int n = 0; n < LINE_THICKNESS; ++n) {
      *(imgBuf + PICTURE_WIDTH * y + CAM_CLIP_X + n)                = ILI9341_RED;
      *(imgBuf + PICTURE_WIDTH * y + CAM_CLIP_X + CAM_CLIP_W - 1 - n) = ILI9341_RED;
    }
  }
}

/* キューを初期化する関数 */
void initQueue(QUEUE_T *queue){

    /* キューを空に設定 */
    queue->head = 0;
    queue->tail = -1;
}

/* ENQUEUEする関数 */
void enqueue(QUEUE_T *queue, int input){

    /* キューが満杯なら何もせず関数終了 */
    if((queue->tail + 2) % MAX_NUM == queue->head){
        printf("キューが満杯でENQUEUEできません\n");
        return;
    }

    /* データをデータの最後尾の１つ後ろに格納 */
    queue->data[(queue->tail + 1) % MAX_NUM] = input;

    /* データの最後尾を１つ後ろに移動 */
    queue->tail = (queue->tail + 1) % MAX_NUM;
}

/* DEQUEUEする関数 */
void dequeue(QUEUE_T *queue){
    int ret = 0;

    /* キューが空なら何もせずに関数終了 */
    if((queue->tail + 1) % MAX_NUM == queue->head){
        printf("キューが空です\n");
        return -1;
    }

    /* データの先頭からデータを取得 */
    ret = queue->data[queue->head];

    /* データの先頭を１つ後ろにずらす */
    queue->head = (queue->head + 1) % MAX_NUM;

}

/* キュー内のデータ数を取得する関数 */
int getQueueSize(const QUEUE_T *queue) {
    if (queue->tail >= queue->head) {
        return queue->tail - queue->head + 1;
    } else {
        return MAX_NUM - (queue->head - queue->tail) + 1;
    }
}

/* キュー内の特定の数字の個数を調べる関数 */
int countInQueue(const QUEUE_T *queue, int target) {
    int count = 0;
    int index = queue->head;

    for (int i = 0; i < getQueueSize(queue); i++) {
        if (queue->data[index] == target) {
            count++;
        }
        index = (index + 1) % MAX_NUM;
    }

    return count;
}
/*
void putStringOnLcd(String str, int color) {
  int len = str.length();
  tft.fillRect(0, 224, 320, 240, ILI9341_BLACK);
  tft.setTextSize(2);
  int sx = 160 - len / 2 * 12;
  if (sx < 0) sx = 0;
  tft.setCursor(sx, 225);
  tft.setTextColor(color);
  tft.println(str);
}

void drawBox(uint16_t* imgBuf) {

  for (int x = CAM_CLIP_X; x < CAM_CLIP_X + CAM_CLIP_W; ++x) {
    for (int n = 0; n < LINE_THICKNESS; ++n) {
      *(imgBuf + PICTURE_WIDTH * (CAM_CLIP_Y + n) + x)              = ILI9341_RED;
      *(imgBuf + PICTURE_WIDTH * (CAM_CLIP_Y + CAM_CLIP_H - 1 - n) + x) = ILI9341_RED;
    }
  }
  for (int y = CAM_CLIP_Y; y < CAM_CLIP_Y + CAM_CLIP_H; ++y) {
    for (int n = 0; n < LINE_THICKNESS; ++n) {
      *(imgBuf + PICTURE_WIDTH * y + CAM_CLIP_X + n)                = ILI9341_RED;
      *(imgBuf + PICTURE_WIDTH * y + CAM_CLIP_X + CAM_CLIP_W - 1 - n) = ILI9341_RED;
    }
  }
}
*/
void CamCB(CamImage img) {

  if (!img.isAvailable()) {
    Serial.println("Image is not available. Try again");
    return;
  }


}


void detection(int lim_sleep) {
  
  int count_sleep = 0;
  char stat[] = "awake";
  QUEUE_T queue;

  Serial.println("検知開始");

    /* キューを初期化 */
  initQueue(&queue);

  while (count_sleep < lim_sleep) {
    count_sleep = countInQueue(&queue, 1);
    Serial.println(count_sleep);
    CamErr cam_err;
    CamImage coverted;
    String gStrResult;
    int awake_count = 0;
    int awake_value;
    int sleep_value;

    // カメラ撮影

    CamImage camImage = theCamera.takePicture();
    if (!camImage.isAvailable()) {
      // 撮影失敗
      Serial.println("CAMERA take picture failure.");
      return;
    }


    for (int y_shift = 0; y_shift < devided; y_shift++) {
      for (int x_shift = 0; x_shift < devided; x_shift++) {

        // 画像の切り取りと縮小処理
        int lefttop_x = CAM_CLIP_X + x_shift * PICTURE_CLIP_WIDTH / 2;
        int lefttop_y = CAM_CLIP_Y + y_shift * PICTURE_CLIP_HEIGHT / 2;
        int rightbottom_x = lefttop_x + PICTURE_CLIP_WIDTH - 1;
        int rightbottom_y = lefttop_y + PICTURE_CLIP_HEIGHT - 1;
        cam_err = camImage.clipAndResizeImageByHW(coverted,
                  lefttop_x, lefttop_y, rightbottom_x, rightbottom_y,
                  INPUT_WIDTH, INPUT_HEIGHT);
        if (cam_err != CAM_ERR_SUCCESS) {
          Serial.print("CAMERA resize failure. : ");
          Serial.println(cam_err);
          return;
        }
        // グレースケール形式への変換
        cam_err = coverted.convertPixFormat(CAM_IMAGE_PIX_FMT_GRAY);
        if (cam_err != CAM_ERR_SUCCESS) {
          Serial.print("CAMERA convert format failure. : ");
          Serial.println(cam_err);
          return;
        }

        // AIモデルの入力データ設定
        float* input_data = dnn_input.data();
        uint8_t* camera_buf = coverted.getImgBuff();
        for (int h = 0; h < INPUT_HEIGHT; h++) {
          for (int w = 0; w < INPUT_WIDTH; w++) {
            input_data[h * INPUT_WIDTH + w] = camera_buf[h * INPUT_WIDTH + w] / 255.0;
          }
        }
        dnnrt.inputVariable(dnn_input, 0);
        // 推論実行
        dnnrt.forward();
        // 推論結果取得
        DNNVariable output = dnnrt.outputVariable(0);
        float value = output[0];

        // 結果表示
        //Serial.print("[recognition] door is ");
        if (value < 0.7) {
          //Serial.print("awake.");
          awake_count = 1;

          awake_value = value;

        } else {
          //Serial.print("sleep. ");
          sleep_value = value;

        }
//        Serial.print(" ( value: ");
//        Serial.print(value);
//        Serial.println(")");

      }
    }
        // キューに11以上入っていたらデキューする
        if(getQueueSize(&queue) == MAX_NUM){
          dequeue(&queue);
        }
    if (awake_count == 1) {
      gStrResult = String("awake:") + String(awake_value);
      Serial.println("awake");
      enqueue(&queue, 0); 
      //Serial.println(awake_value);
    }
    else {
      gStrResult = String("sleep:") + String(sleep_value);
      Serial.println("sleep");
      enqueue(&queue, 1); 
    }

    camImage.convertPixFormat(CAM_IMAGE_PIX_FMT_RGB565);
    uint16_t* imgBuf = (uint16_t*)camImage.getImgBuff();
    drawBox(imgBuf);
    tft.drawRGBBitmap(0, 0, (uint16_t *)camImage.getImgBuff(), 320, 224);

    putStringOnLcd(gStrResult, ILI9341_YELLOW);
  }
  *stat = "sleep";
}
