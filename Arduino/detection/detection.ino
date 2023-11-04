#include "detection.h"

/**
   @brief セットアップ処理
*/
// 　DNNRTクラスのインスタンス
DNNRT dnnrt;
// SDカードクラスのインスタンス
SDClass SD;
// AIモデルの入力データ用領域
DNNVariable dnn_input(INPUT_WIDTH* INPUT_HEIGHT);
Adafruit_ILI9341 tft = Adafruit_ILI9341(&SPI, TFT_DC, TFT_CS, TFT_RST);

void setup() {
    int ret;
    CamErr cam_err;

    Serial.begin(115200);
    while (!Serial) {
        ;  // シリアルモニタ接続待ち
    }
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
    calibration();
}

/**
   @brief ループ処理
*/
void loop() {
    detection(5);
    Serial.println("sleep");
}
