#include "detection.h"

// 　DNNRTクラスのインスタンス
DNNRT dnnrt;
// SDカードクラスのインスタンス
SDClass SD;
// AIモデルの入力データ用領域
DNNVariable dnn_input(INPUT_WIDTH* INPUT_HEIGHT);
Adafruit_ILI9341 tft = Adafruit_ILI9341(&SPI, TFT_DC, TFT_CS, TFT_RST);

void init_AImodel() {
    int ret;
    tft.begin();
    tft.setRotation(3);
    // AIモデルファイル読み込み
    File nnbfile = SD.open("model.nnb");
    if (nnbfile == NULL) {
        // ファイル無しエラー
        Serial.print("nnb is not found");
        exit(0);
    }
    // DNNRTライブラリ（AIモデルでの判定を行うライブラリ）の初期設定
    ret = dnnrt.begin(nnbfile);
    if (ret < 0) {
        // DNNRT開始エラー
        Serial.print("DNNRT initialization failure.: ");
        Serial.println(ret);
        exit(0);
    }
    nnbfile.close();
}

void init_camera() {
    CamErr cam_err;
    // カメラの初期設定
    cam_err = theCamera.begin(0);
    if (cam_err != CAM_ERR_SUCCESS) {
        // カメラ開始エラー
        Serial.print("CAMERA initialization failure.: ");
        Serial.println(cam_err);
        exit(0);
    }
    // 撮影パラメタ（サイズ、形式）設定
    cam_err = theCamera.setStillPictureImageFormat(
        PICTURE_WIDTH, PICTURE_HEIGTH, CAM_IMAGE_PIX_FMT_YUV422);
    if (cam_err != CAM_ERR_SUCCESS) {
        // カメラ設定エラー
        Serial.print("CAMERA set parameters failure.: ");
        Serial.println(cam_err);
        exit(0);
    }
}

// detec()-> awake:0 sleep:1を返す
int detec() {
    CamErr cam_err;
    CamImage coverted;

    String gStrResult;
    int awake_count = 0;

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
            cam_err = camImage.clipAndResizeImageByHW(
                coverted, lefttop_x, lefttop_y, rightbottom_x, rightbottom_y,
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
                    input_data[h * INPUT_WIDTH + w] =
                        camera_buf[h * INPUT_WIDTH + w] / 255.0;
                }
            }
            dnnrt.inputVariable(dnn_input, 0);
            // 推論実行
            dnnrt.forward();
            // 推論結果取得
            DNNVariable output = dnnrt.outputVariable(0);
            float value = output[0];
            // 結果表示
            // Serial.print("[recognition] door is ");
            if (value < detec_level) {
                // Serial.print("awake.");
                awake_count = 1;
            } else {
                // Serial.print("sleep. ");
            }
            // Serial.print("value: ");
            // Serial.println(value);
            //      Serial.println(")");
        }
    }

    camImage.convertPixFormat(CAM_IMAGE_PIX_FMT_RGB565);
    uint16_t* imgBuf = (uint16_t*)camImage.getImgBuff();
    drawBox(imgBuf);
    tft.drawRGBBitmap(0, 0, (uint16_t*)camImage.getImgBuff(), 320, 224);

    putStringOnLcd(gStrResult, ILI9341_YELLOW);

    if (awake_count == 1) {
        gStrResult = String("[calibration] awake");
        // Serial.print("awake");

        return 0;
    } else {
        gStrResult = String("[calibration] sleep");
        // Serial.print("sleep");
        return 1;
    }
}

// detec()-> awake:0 sleep:1を返す
void calibration() {
    int count = 0;

    int results;

    Serial.println("[calibration] Calibration start!");

    // 顔（瞳）検出

    Serial.println("[calibration] 顔を映してください");

    /* キューを初期化 */
    initQueue(&queue);
    while (count < 5) {
        // Serial.println(getQueueSize(&queue));
        //  キューに11以上入っていたらデキューする
        if (getQueueSize(&queue) == queue_size) {
            dequeue(&queue);
        }
        if (detec() == 1) {
            enqueue(&queue, 1);
        } else {
            enqueue(&queue, 0);
        }

        count = countInQueue(&queue, 0);
        Serial.print("[calibration] stage1: check your face  ");
        Serial.print(count);
        Serial.print("/5  [");
        preview_queue(&queue);
        Serial.println("]");
    }
    Serial.println("[calibration] done!");

    // 背景
    Serial.println("[calibration] 背景を映してください");

    /* キューを初期化 */
    initQueue(&queue);

    count = 0;
    while (count < 5) {
        // キューに11以上入っていたらデキューする
        if (getQueueSize(&queue) == queue_size) {
            dequeue(&queue);
        }
        if (detec() == 1) {
            enqueue(&queue, 1);
        } else {
            enqueue(&queue, 0);
        }

        count = countInQueue(&queue, 1);
        Serial.print("[calibration] stage2: check your background  ");
        Serial.print(count);
        Serial.print("/5  [");
        preview_queue(&queue);
        Serial.println("]");
    }
    Serial.println("[calibration] done!");
    //

    Serial.println("[calibration] もう一度顔を映してください");

    /* キューを初期化 */
    initQueue(&queue);

    count = 0;
    while (count < 2) {
        // キューに11以上入っていたらデキューする
        if (getQueueSize(&queue) == queue_size) {
            dequeue(&queue);
        }
        if (detec() == 1) {
            enqueue(&queue, 1);
        } else {
            enqueue(&queue, 0);
        }

        count = countInQueue(&queue, 0);
        Serial.print("[calibration] stage3: check your face again  ");
        Serial.print(count);
        Serial.print("/2  [");
        preview_queue(&queue);
        Serial.println("]");
    }
    Serial.println("[calibration] done!");

    /* キューを初期化 */
    initQueue(&queue);

    Serial.println("[calibration] Calibration end!");
}
