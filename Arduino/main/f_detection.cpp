#include "detection.h"


    QUEUE_T queue;


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

void drawBox(uint16_t *imgBuf) {
    for (int x = CAM_CLIP_X; x < CAM_CLIP_X + CAM_CLIP_W; ++x) {
        for (int n = 0; n < LINE_THICKNESS; ++n) {
            *(imgBuf + PICTURE_WIDTH * (CAM_CLIP_Y + n) + x) = ILI9341_RED;
            *(imgBuf + PICTURE_WIDTH * (CAM_CLIP_Y + CAM_CLIP_H - 1 - n) + x) =
                ILI9341_RED;
        }
    }
    for (int y = CAM_CLIP_Y; y < CAM_CLIP_Y + CAM_CLIP_H; ++y) {
        for (int n = 0; n < LINE_THICKNESS; ++n) {
            *(imgBuf + PICTURE_WIDTH * y + CAM_CLIP_X + n) = ILI9341_RED;
            *(imgBuf + PICTURE_WIDTH * y + CAM_CLIP_X + CAM_CLIP_W - 1 - n) =
                ILI9341_RED;
        }
    }
}

/* キューを初期化する関数 */
void initQueue(QUEUE_T *queue) {
    /* キューを空に設定 */
    queue->head = 0;
    queue->tail = -1;
}

/* ENQUEUEする関数 */
void enqueue(QUEUE_T *queue, int input) {
    /* キューが満杯なら何もせず関数終了 */
    if ((queue->tail + 2) % MAX_NUM == queue->head) {
        printf("キューが満杯でENQUEUEできません\n");
        return;
    }

    /* データをデータの最後尾の１つ後ろに格納 */
    queue->data[(queue->tail + 1) % MAX_NUM] = input;

    /* データの最後尾を１つ後ろに移動 */
    queue->tail = (queue->tail + 1) % MAX_NUM;
}

/* DEQUEUEする関数 */
void dequeue(QUEUE_T *queue) {
    int ret = 0;

    /* キューが空なら何もせずに関数終了 */
    if ((queue->tail + 1) % MAX_NUM == queue->head) {
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

void preview_queue(const QUEUE_T *queue) {
    int index = queue->head;
    for (int i = 0; i < getQueueSize(queue); i++) {
        Serial.print(queue->data[index]);
        index = (index + 1) % MAX_NUM;
    }
    Serial.println();
}



void CamCB(CamImage img) {
    if (!img.isAvailable()) {
        Serial.println("Image is not available. Try again");
        return;
    }
}

void detection() {
 
    String gStrResult;
    int awake_count = 0;

    // キューに11以上入っていたらデキューする
    if (getQueueSize(&queue) == queue_size) {
        dequeue(&queue);
    }
    if (detec() == 0) {
        gStrResult = String("awake");
        Serial.println("awake");
        enqueue(&queue, 0);
        // Serial.println(awake_value);
    } else {
        gStrResult = String("sleep");
        Serial.println("sleep");
        enqueue(&queue, 1);
    }

    putStringOnLcd(gStrResult, ILI9341_YELLOW);
}
