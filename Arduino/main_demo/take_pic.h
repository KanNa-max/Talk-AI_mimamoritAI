#ifndef CAMERA_HEADER_H
#define CAMERA_HEADER_H

#define BAUDRATE (115200)
#define TOTAL_PICTURE_COUNT (500)
#define CAP_INTERVAL (1)
#define NAME_KEY "OCT1017"


void CamSetup();
void CamTakePic();

#endif // CAMERA_HEADER_H
