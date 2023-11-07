#include "gesture_detec.h"
#include "detection.h"


uint8_t buffer[28 * 28] __attribute__((aligned(16)));

void init_gesture() {
  int ret;
  // AIモデルファイル読み込み
  File nnbfile = SD.open("gesture.nnb");
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
  Serial.println("[gesture_detec.cpp:init_gesture()] AI gesture is loaded.");
}



bool gesture_detec() {
  CamErr cam_err;
  CamImage coverted;
  String gStrResult;
  float* input_buffer;
  int label;
  unsigned int i;
  // カメラ撮影

  CamImage camImage = theCamera.takePicture();
  if (!camImage.isAvailable()) {
    // 撮影失敗
    Serial.println("CAMERA take picture failure.");
    return;
  }

  // 画像の切り取りと縮小処理
  int lefttop_x = 0;
  int lefttop_y = 0;
  int rightbottom_x = 111; //223
  int rightbottom_y = 111;  //223
  cam_err = camImage.clipAndResizeImageByHW(coverted,
                                            lefttop_x, lefttop_y, rightbottom_x, rightbottom_y,
                                            INPUT_WIDTH, INPUT_HEIGHT);
  if (cam_err != CAM_ERR_SUCCESS) {
    Serial.print("CAMERA resize failure. : ");
    Serial.println(cam_err);
    return;
  }


  //GE.shrink(coverted, buffer);
  // input_buffer = dnn_input.data();
  // for (i = 0; i < 28 * 28; i++)
  // {
  //   input_buffer[i] = (float)buffer[i];
  // }
  cam_err = coverted.convertPixFormat(CAM_IMAGE_PIX_FMT_GRAY);
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
  // label = 3;
  // for (i = 0; i < output.size(); i++)
  // {
  //   if (output[i] > 0.4)
  //   {
  //     label = i;
  //   }
  // }
  label = 3; //
  for (i = 0; i < output.size(); i++) {
    if (output[i] > 0.6) {
      label = i;
      //Serial.println(output[i]);
    }
  }
  Serial.print("[gesture_detec.cpp:gesture_detec()]:");
    Serial.println(label);

  // Serial.print("[gesture_detec.cpp:gesture_detec()]");
  // Serial.println(label);
  if (label == 0) {
    gStrResult = String("hand:") + String(label);
  } else {
    gStrResult = String("no hand:") + String(label);
  }
  // クラスの予測結果を取得

  // 画面にクラスの予測結果を表示
  putStringOnLcd(gStrResult, ILI9341_YELLOW);

  camImage.convertPixFormat(CAM_IMAGE_PIX_FMT_RGB565);
  uint16_t* imgBuf = (uint16_t*)camImage.getImgBuff();
  drawBox(imgBuf, CAM_CLIP_X_ges, CAM_CLIP_Y_ges);
  tft.drawRGBBitmap(0, 0, (uint16_t*)camImage.getImgBuff(), 320, 224);

  if (label == 0) {
    return true;
  } else {
    return false;
  }
}

bool count_gesture(int lim) {
  int count = 0;
  for (int i = 0; i < 5; i++) {
    if (gesture_detec() == true) {
      count++;
    }
  }
  if (count >= lim) {
    Serial.print("[gesture_detec.cpp:gesture_detec()]");
    Serial.println("true");
    return true;
  } else {
    Serial.print("[gesture_detec.cpp:gesture_detec()]");
    Serial.println("false");
    return false;
  }
}
