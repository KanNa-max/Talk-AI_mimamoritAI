#include "take_pic.h"

#include <Camera.h>
#include <SDHCI.h>
#include <stdio.h>
#include <stdlib.h>

SDClass theSD;

int gen_random() {
    int random_number;
    // srand((unsigned int)clock());  // 乱数のシードを設定
    // 0から999までの乱数を生成
    random_number = rand() % 1000;
    // printf("Random number: %d\n", random_number);
    return random_number;
}

int NAME_ID = gen_random();
// int pic_cnt = 0;

/**
 * Print error message
 */

void printError(enum CamErr err) {
    Serial.print("Error: ");
    switch (err) {
        case CAM_ERR_NO_DEVICE:
            Serial.println("No Device");
            break;
        case CAM_ERR_ILLEGAL_DEVERR:
            Serial.println("Illegal device error");
            break;
        case CAM_ERR_ALREADY_INITIALIZED:
            Serial.println("Already initialized");
            break;
        case CAM_ERR_NOT_INITIALIZED:
            Serial.println("Not initialized");
            break;
        case CAM_ERR_NOT_STILL_INITIALIZED:
            Serial.println("Still picture not initialized");
            break;
        case CAM_ERR_CANT_CREATE_THREAD:
            Serial.println("Failed to create thread");
            break;
        case CAM_ERR_INVALID_PARAM:
            Serial.println("Invalid parameter");
            break;
        case CAM_ERR_NO_MEMORY:
            Serial.println("No memory");
            break;
        case CAM_ERR_USR_INUSED:
            Serial.println("Buffer already in use");
            break;
        case CAM_ERR_NOT_PERMITTED:
            Serial.println("Operation not permitted");
            break;
        default:
            break;
    }
}

/**
 * Callback from Camera library when video frame is captured.
 */

void CamCB(CamImage img) {
    /* Check the img instance is available or not. */

    if (img.isAvailable()) {
        /* If you want RGB565 data, convert image data format to RGB565 */

        img.convertPixFormat(CAM_IMAGE_PIX_FMT_RGB565);

        /* You can use image data directly by using getImgSize() and
         * getImgBuff(). for displaying image to a display, etc. */

        Serial.print("Image data size = ");
        Serial.print(img.getImgSize(), DEC);
        Serial.print(" , ");

        Serial.print("buff addr = ");
        Serial.print((unsigned long)img.getImgBuff(), HEX);
        Serial.println("");
    } else {
        Serial.println("Failed to get video stream image");
    }
}

/**
 * @brief Initialize camera
 */
void CamSetup() {
    CamErr err;

    /* Open serial communications and wait for port to open */

    Serial.begin(BAUDRATE);
    while (!Serial) {
        ; /* wait for serial port to connect. Needed for native USB port only */
    }

    /* Initialize SD */
    while (!theSD.begin()) {
        /* wait until SD card is mounted. */
        Serial.println("Insert SD card.");
    }

    /* begin() without parameters means that
     * number of buffers = 1, 30FPS, QVGA, YUV 4:2:2 format */

    Serial.println("Prepare camera");
    err = theCamera.begin();
    if (err != CAM_ERR_SUCCESS) {
        printError(err);
    }

    /* Start video stream.
     * If received video stream data from camera device,
     *  camera library call CamCB.
     */

    Serial.println("Start streaming");
    err = theCamera.startStreaming(true, CamCB);
    if (err != CAM_ERR_SUCCESS) {
        printError(err);
    }

    /* Auto white balance configuration */

    Serial.println("Set Auto white balance parameter");
    err = theCamera.setAutoWhiteBalanceMode(CAM_WHITE_BALANCE_DAYLIGHT);
    if (err != CAM_ERR_SUCCESS) {
        printError(err);
    }

    /* Set parameters about still picture.
     * In the following case, QUADVGA and JPEG.
     */

    Serial.println("Set still picture format");
    err = theCamera.setStillPictureImageFormat(
        CAM_IMGSIZE_QUADVGA_H, CAM_IMGSIZE_QUADVGA_V, CAM_IMAGE_PIX_FMT_JPG);
    if (err != CAM_ERR_SUCCESS) {
        printError(err);
    }
    /** LED **/
    pinMode(LED0, OUTPUT);
    digitalWrite(LED0, HIGH);
    pinMode(LED1, OUTPUT);
    digitalWrite(LED1, LOW);
}

/**
 * @brief Take picture with format JPEG per second
 */

void CamTakePic() {
    int pic_cnt;
    for (pic_cnt = 0; pic_cnt < TOTAL_PICTURE_COUNT;
         pic_cnt++) {
        // digitalWrite(LED1, HIGH);
        // delay(100);
        // digitalWrite(LED1, LOW);
        // delay(100);

        // // 現在の時間を取得
        // time_t current_time;
        // struct tm *time_info;
        // time(&current_time);
        // time_info = localtime(&current_time);
        // strftime(time_string, sizeof(time_string), "%Y%m%d_%H%M%S",
        // time_info);

        sleep(CAP_INTERVAL); /* wait for CAP_INTERVAL second */

        /* You can change the format of still picture at here also, if you want.
         */

        /* theCamera.setStillPictureImageFormat(
         *   CAM_IMGSIZE_HD_H,
         *   CAM_IMGSIZE_HD_V,
         *   CAM_IMAGE_PIX_FMT_JPG);
         */

        /* This sample code can take pictures in every one second from starting.
         */

        if (pic_cnt < TOTAL_PICTURE_COUNT) {
            digitalWrite(LED1, HIGH);
            /* Take still picture.
             * Unlike video stream(startStreaming) , this API wait to receive
             * image data from camera device.
             */

            Serial.println("call takePicture()");
            CamImage img = theCamera.takePicture();

            /* Check availability of the img instance. */
            /* If any errors occur, the img is not available. */

            if (img.isAvailable()) {
                /* Create file name */

                char filename[16] = {0};
                // sprintf(filename,"PICT%03d.JPG", pic_cnt);
                sprintf(filename, "out/PICT_%s_%03d_%03d.JPG", NAME_KEY,
                        NAME_ID, pic_cnt);

                Serial.print("Save taken picture as ");
                Serial.print(filename);
                Serial.println("");

                /* Remove the old file with the same file name as new created
                 * file, and create new file.
                 */

                theSD.remove(filename);
                File myFile = theSD.open(filename, FILE_WRITE);
                myFile.write(img.getImgBuff(), img.getImgSize());
                myFile.close();
            } else {
                /* The size of a picture may exceed the allocated memory size.
                 * Then, allocate the larger memory size and/or decrease the
                 * size of a picture. [How to allocate the larger memory]
                 * - Decrease jpgbufsize_divisor specified by
                 * setStillPictureImageFormat()
                 * - Increase the Memory size from Arduino IDE tools Menu
                 * [How to decrease the size of a picture]
                 * - Decrease the JPEG quality by setJPEGQuality()
                 */

                Serial.println("Failed to take picture");
            }
            digitalWrite(LED1, LOW);
        } else if (pic_cnt == TOTAL_PICTURE_COUNT) {
            Serial.println("End.");
            theCamera.end();
            digitalWrite(LED0, LOW);
        }

        // pic_cnt++;
        Serial.print("pic_cnt: success");
    }
}
