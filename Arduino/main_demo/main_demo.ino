#include <stdio.h>
#include <stdlib.h>

#include "main_demo.h"
#include "append.h"
#include "take_pic.h"


void setup() {
  // put your setup code here, to run once:
  append_fan();
  CamSetup();  
}

void loop() {
  // put your main code here, to run repeatedly:
  CamTakePic();
}
