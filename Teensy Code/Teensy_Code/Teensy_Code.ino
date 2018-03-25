//To add the necessary included libraries go to Sketch->Include Library->Add .ZIP Library...
//Filter and get all the necessary libraries
//Make sure Board type selected in tools is a teensy

#include <Arduino.h>
#include "variables.h"
#include "screenCode.h"

bool reverseMode = false;

void setup() 
{
  //Fault Light Setup
  pinMode(AMS_light, OUTPUT);
  pinMode(IMD_light, OUTPUT);
  pinMode(BSPD_light, OUTPUT);
  pinMode(TPS_light, OUTPUT);
  
  //Servo Setup
  pinMode(servo_PWM, OUTPUT);
  
  //Drive Mode Switch Setup
  pinMode(SW_bit0, INPUT);
  pinMode(SW_bit1, INPUT);
  pinMode(SW_bit2, INPUT);
  pinMode(SW_bit3, INPUT);
  pinMode(GS_pin, INPUT);
  attachInterrupt(digitalPinToInterrupt(GS_pin), updateDriveMode, RISING);
  
  //Ignition Switch Setup
  pinMode(Ignition_1, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(Ignition_1), displayDriveMode, CHANGE);
  pinMode(Ignition_2, INPUT_PULLUP);
  
  //Steering Wheel Setup
  pinMode(left_button, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(left_button), scrollDashLeft, FALLING);
  pinMode(right_button, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(right_button), scrollDashRight, FALLING);

  //Teensy SD Card Setup
  SD.begin(SDCS);
  
  //Dash Screen Setup
  setupScreen();
  displayDriveMode();
  
  //Dash Lights Setup
  setupLights();
  
  //Dash CAN Setup
  CARCAN.begin();
  DAQCAN.begin();
}

void loop()
{
  if(CARCAN.available())
  {
    processCARCANFrame();
  }
  
  if(DAQCAN.available())
  {
    processDAQCANFrame();
  }
  
  if(on && dashpage == 1)
  {
    if(!previouslyon)        //If it was on the start logo, erase screen
    {
      if(driveMode != 11)
      {
         tft.fillScreen(HX8357_BLACK);
         printCommonBackground();
      }
      changeDriveMode();
      previouslyon = true;
    }
    else if(driveMode == 11)  //If it is in razzle mode, update image to dog
    {
      changeDriveMode();
    }
    else                        //If any other drive mode, update screen
    {
     if(previousdriveMode == 11)      
     {
       tft.fillScreen(HX8357_BLACK);  //blank screen
       updateTempPixels();            //reset pixels to current battery temp
     }
     printCommonBackground();         //update background
     changeDriveMode();               //updates everything else on screen
    }
  }
  else if(on && dashpage != 1)
  {
    if(driveMode == 11)
    {
      dashpage = 1;
    }
    else if(!previouslyon)
    {
      tft.fillScreen(HX8357_BLACK);
      printCommonBackground();
      changeDashPage();
      previouslyon = true;
    }
    else
    {
      printCommonBackground();
      changeDashPage();
    }
  }
  else if(!on && previouslyon)
  {
    startScreen();
    previouslyon = false;
  }
  
  //Update Servo
  if(previousHVSOC != HVSOC)
  {
    updatesocServo();
  }
  previousHVSOC = HVSOC;
  
  //Update Temperature pixels
  if((previousmaxCellTemp != maxCellTemp) && (driveMode != 11))
  {
    updateTempPixels();
  }
  previousmaxCellTemp = maxCellTemp;

  //Update fault lights
  if(driveMode != 11 || !on)
  {
    imdLight(IMDfault);
    amsLight(AMSfault);
    bspdLight(BSPDfault);
  }
  
  if(HVSOC <= 20)
  {
    repixels.setPixelColor(1, 255,0,0);
    repixels.show();
  }
  else
  {
    repixels.setPixelColor(1, 0,0,0);
    repixels.show();
  }

  driveModeEnabledLight(driveActive);
  sendCARCANFrame();
}

void displayDriveMode()
{
  on = !digitalRead(Ignition_1);
  driveMode = digitalRead(SW_bit3);
  driveMode <<= 1;
  driveMode |= digitalRead(SW_bit2);
  driveMode <<= 1;
  driveMode |= digitalRead(SW_bit1);
  driveMode <<= 1;
  driveMode |= digitalRead(SW_bit0);
  fixDriveModeNumber();
}

void updateDriveMode()
{
  if(driveActive || startActive)
  {
    driveMode = digitalRead(SW_bit3);
    driveMode <<= 1;
    driveMode |= digitalRead(SW_bit2);
    driveMode <<= 1;
    driveMode |= digitalRead(SW_bit1);
    driveMode <<= 1;
    driveMode |= digitalRead(SW_bit0);
    fixDriveModeNumber();
  }
}

void fixDriveModeNumber()
{
  if(driveMode == 0){ driveMode = 10;}
  else if(driveMode == 1){ driveMode = 11; }
  else { driveMode -= 2; }
}

void changeDriveMode()
{
  switch(driveMode)
  {
    case 0:
      printScreenTitle("Acceleration", 0);
      printScreenNumber();
      if(previousdriveMode == 11)
      {
        qbaiLight(OFF);
      }
      previousdriveMode = 0;
      break;
    case 1:
      printScreenTitle("Skid Pad", 1);
      printScreenNumber();
      if(previousdriveMode == 11)
      {
        qbaiLight(OFF);
      }
      previousdriveMode = 1;
      break;
    case 2:
      printScreenTitle("Autocross", 2);
      printScreenNumber();
      if(previousdriveMode == 11)
      {
        qbaiLight(OFF);
      }
      previousdriveMode = 2;
      break;
    case 3:
      printScreenTitle("Endurance", 3);
      printScreenNumber();
      if(previousdriveMode == 11)
      {
        qbaiLight(OFF);
      }
      previousdriveMode = 3;
      break;
    case 4:
      printScreenTitle("Sunday Driving", 4);
      printScreenNumber();
      if(previousdriveMode == 11)
      {
        qbaiLight(OFF);
      }
      previousdriveMode = 4;
      break;
    case 5:
      printScreenTitle("Granny Mode", 5);
      printScreenNumber();
      if(previousdriveMode == 11)
      {
        qbaiLight(OFF);
      }
      previousdriveMode = 5;
      break;
    case 6:
      if(reverseMode)
      {
        printScreenTitle("Reverse", 6);
      }
      else
      {
        printScreenTitle("Extra0", 6);
      }
      printScreenNumber();
      if(previousdriveMode == 11)
      {
        qbaiLight(OFF);
      }
      previousdriveMode = 6;
      break;
    case 7:
      printScreenTitle("Extra", 7);
      printScreenNumber();
      if(previousdriveMode == 11)
      {
        qbaiLight(OFF);
      }
      previousdriveMode = 7;
      break;
    case 8:
      printScreenTitle("Extra1", 8);
      printScreenNumber();
      if(previousdriveMode == 11)
      {
        qbaiLight(OFF);
      }
      previousdriveMode = 8;
      break;
    case 9:
      printScreenTitle("Extra2", 9);
      printScreenNumber();
      if(previousdriveMode == 11)
      {
        qbaiLight(OFF);
      }
      previousdriveMode = 9;
      break;
    case 10:
      printScreenTitle("Extra3", 10);
      printScreenNumber();
      if(previousdriveMode == 11)
      {
        qbaiLight(OFF);
      }
      previousdriveMode = 10;
      break;
    case 11:
      if((previousdriveMode != 11) || !previouslyon)
      {
        bmpDraw("rnd.bmp", 0, 0);
        for(uint8_t i = 0; i < NUM_CD_PIXELS; i++)
        {
          cdpixels.setPixelColor(i, 0,0,0);
        }
        for(uint8_t i = 0; i < NUM_RE_PIXELS; i++)
        {
          repixels.setPixelColor(i, 0,0,0);
        }
        cdpixels.show();
        repixels.show();
      }
      previousdriveMode = 11;
      razzleMode();
      break;
    default:
      printScreenTitle("default", 69);
      printScreenNumber();
      if(previousdriveMode == 11)
      {
        qbaiLight(OFF);
      }
      previousdriveMode = 69;
      break;
  }
}

void scrollDashLeft()
{
  if(driveMode != 11)
  {
    if(dashpage == 1){ dashpage = 3; }
    else{ dashpage--; }
  }
}

void scrollDashRight()
{
  if(driveMode != 11)
  {
    if(dashpage == 3){ dashpage = 1; }
    else{ dashpage++; }
  }
}

void changeDashPage()
{
  switch(dashpage)
  {
    case 2:
      printScreenTitle("Faults", 12);
      printScreenNumber();
      break;
    case 3:
      printScreenTitle("CAN Messages", 13);
      printScreenNumber();
      break;
  }
}

void imdLight(bool on)
{
  digitalWrite(IMD_light, on);
}

void amsLight(bool on)
{
  digitalWrite(AMS_light, on);
}

void bspdLight(bool on)
{
  digitalWrite(BSPD_light, on);
}

void qbaiLight(bool on)
{
  digitalWrite(TPS_light, on);
}

void driveModeEnabledLight(bool enabled)
{
  if(enabled){ repixels.setPixelColor(0, 0,0,255); }
  else if(startActive){ repixels.setPixelColor(0, 255,0,0); }
  else{ repixels.setPixelColor(0, 0,255,0); }
  repixels.show();
}

void updatesocServo()
{
  socservo.attach(servo_PWM);
  if(HVSOC < 26)
  {
    servoval = map(HVSOC, 0, 25, 19, 51);
    socservo.write(servoval);
    delay(250);
  }
  else if(HVSOC < 51)
  {
    servoval = map(HVSOC, 26, 50, 52, 90);
    socservo.write(servoval);
    delay(250);
  }
  else if(HVSOC < 76)
  {
    servoval = map(HVSOC, 51, 75, 91, 135);
    socservo.write(servoval);
    delay(250);
  }
  else
  {
    servoval = map(HVSOC, 76, 100, 136, 175);
    socservo.write(servoval);
    delay(250);
  }
  socservo.detach();
}

void updateTempPixels()
{
  for(uint8_t i = 0; i < NUM_CD_PIXELS; i++)
  {
    cdpixels.setPixelColor(i, 0,0,0);
  }
  cdpixels.show();
  
  uint8_t pixels = 0;
  if(maxCellTemp <= 0) { pixels = 1; }
  else if(maxCellTemp <=5) { pixels = 2; }
  else if(maxCellTemp <=10) { pixels = 3; }
  else if(maxCellTemp <=16) { pixels = 4; }
  else if(maxCellTemp <=21) { pixels = 5; }
  else if(maxCellTemp <=27) { pixels = 6; }
  else if(maxCellTemp <=32) { pixels = 7; }
  else if(maxCellTemp <=38) { pixels = 8; }
  else if(maxCellTemp <=43) { pixels = 9; }
  else if(maxCellTemp <=49) { pixels = 10; }
  else if(maxCellTemp <=54) { pixels = 11; }
  else { pixels = 12; }
  
  switch(pixels)
  {
    case 12:
      cdpixels.setPixelColor(11, 255,0,0);
    case 11:
      cdpixels.setPixelColor(10, 255,0,0);
    case 10:
      cdpixels.setPixelColor(9, 255,0,0);
    case 9:
      cdpixels.setPixelColor(8, 255,50,0);
    case 8:
      cdpixels.setPixelColor(7, 253,255,18);
    case 7:
      cdpixels.setPixelColor(6, 170,255,0);
    case 6:
      cdpixels.setPixelColor(5, 0,255,0);
    case 5:
      cdpixels.setPixelColor(4, 0,255,117);
    case 4:
      cdpixels.setPixelColor(3, 0,223,247);
    case 3:
      cdpixels.setPixelColor(2, 0,126,255);
    case 2:
      cdpixels.setPixelColor(1, 0,65,255);
    case 1:
      cdpixels.setPixelColor(0, 0,0,255);    
  }
  cdpixels.show();
}


//CAN Stuff
void sendCARCANFrame()
{
  txmsg.id = 0x24;
  txmsg.len = 8;
  txmsg.buf[0] = 0;
  txmsg.buf[1] = ignitionByte();
  txmsg.buf[2] = 2;
  txmsg.buf[3] = 3;
  txmsg.buf[4] = 4;
  txmsg.buf[5] = 5;
  txmsg.buf[6] = 6;
  txmsg.buf[7] = 7;
  CARCAN.write(txmsg);
}

uint8_t ignitionByte()
{
  uint8_t buf = 0;
  if(driveMode == 11){ buf |= 0; }
  else {buf |= !digitalRead(Ignition_2); }
  buf <<= 1;
  buf |= !on;
  return buf;
}

void sendDAQCANFrame()
{
  txmsg.id = 0x00;
  txmsg.len = 8;
  txmsg.buf[0] = 0;
  txmsg.buf[1] = 1;
  txmsg.buf[2] = 2;
  txmsg.buf[3] = 3;
  txmsg.buf[4] = 4;
  txmsg.buf[5] = 5;
  txmsg.buf[6] = 6;
  txmsg.buf[7] = 7;
  DAQCAN.write(txmsg);
}

void processCARCANFrame()
{
  CARCAN.read(rxmsg);
  
  //Pedal Board
  uint8_t dataByte = 0;
  if(rxmsg.id == 0x20)
  {
    throttleOneRaw = rxmsg.buf[0];
    throttleOneRaw <<= 8;
    throttleOneRaw |= rxmsg.buf[1];
    throttleOneF = (float)(throttleOneRaw-256)/18.3;
    throttleTwoRaw = rxmsg.buf[2];
    throttleTwoRaw <<= 8;
    throttleTwoRaw |= rxmsg.buf[3];
    throttleTwoF = (float)(throttleTwoRaw-256)/18.3;
    throttleDiff = abs(throttleOneF-throttleTwoF);
    
    brakePressureFrontRaw = rxmsg.buf[4];
    brakePressureFrontRaw <<= 8;
    brakePressureFrontRaw |= rxmsg.buf[5];
    brakePressureFrontF = (float)(brakePressureFrontRaw-256)/10;
    if(brakePressureFrontF >= 13){ brakePressureBool = true; }
    else brakePressureBool = false;
    if(brakePressureFrontF >= brakeMax){ brakeMax = brakePressureFrontF; }
    brakePressureRearRaw = rxmsg.buf[6];
    brakePressureRearRaw <<= 8;
    brakePressureRearRaw |= rxmsg.buf[7];
    brakePressureRearF = (float)(brakePressureRearRaw-256)/10;
  }

  //Bridge Card Messages
  else if(rxmsg.id == 0x28)
  {
    dataByte = rxmsg.buf[0];
    if((dataByte & 0x01) == 1){ BSPDfault = true; }
    else BSPDfault = false;
    dataByte >>= 1;
    if((dataByte & 0x01) == 1){ driveActive = true; }
    else driveActive = false;
    dataByte >>=1;
    if((dataByte & 0x01) == 1){ startActive = true; }
    else startActive = false;
    dataByte >>=1;
    if((dataByte & 0x01) == 1){ IMDfault = true; }
    else IMDfault = false;
    dataByte >>=1;
    if((dataByte & 0x01) == 1){ AMSfault = true; }
    else AMSfault = false;
    dataByte >>=1;
    if((dataByte & 0x01) == 1){ shutdownActive = false; }
    else shutdownActive = true;
    carSpeed = rxmsg.buf[5];
    carSpeed <<= 8;
    carSpeed |= rxmsg.buf[6];
    carSpeedF = double(carSpeed)/10;
  }
  else if(rxmsg.id == 0x29)
  {
    MCTemp = rxmsg.buf[0];
    MCTemp <<= 8;
    MCTemp |= rxmsg.buf[1];
    MCTempF = (float)MCTemp/10;
    motorTemp = rxmsg.buf[2];
    motorTemp <<= 8;
    motorTemp |= rxmsg.buf[3];
    motorTempF = (float)motorTemp/10;
    batteryTemp = rxmsg.buf[4];
    batteryTemp <<= 8;
    batteryTemp |= rxmsg.buf[5];
    batteryTempF = (float)batteryTemp/10;
  }

  //AMS Messages
  else if(rxmsg.id == 0x41)
  {
    faulted = rxmsg.buf[0] << 1;
    dataByte = rxmsg.buf[1];
    faulted |= dataByte >> 7;
    dataByte >>=1;
    if((dataByte & 0x01) == 1){ prechargeContactor = true; }
    else prechargeContactor = false;
    dataByte >>=1;
    if((dataByte & 0x01) == 1){ negativeContactor = true; }
    else negativeContactor = false;
    dataByte >>=1;
    if((dataByte & 0x01) == 1){ positiveContactor = true; }
    else positiveContactor = false;
    maxCellTemp = rxmsg.buf[2];
    maxCellTemp <<= 8;
    maxCellTemp |= rxmsg.buf[3];
    maxCellTempF = (float)maxCellTemp/10;
    maxCellTemp = maxCellTemp/10;
    minCellTemp = rxmsg.buf[4];
    minCellTemp <<= 8;
    minCellTemp |= rxmsg.buf[5];
    minCellTempF = (float)minCellTemp/10;      
    HVSOC = rxmsg.buf[6];
    HVSOC <<= 8;
    HVSOC |= rxmsg.buf[7];
    HVSOCF = (float)HVSOC/10;
    HVSOC = HVSOC/10;
  }
  else if(rxmsg.id == 0x42)
  {
    packVoltage = rxmsg.buf[0];
    packVoltage <<= 8;
    packVoltage |= rxmsg.buf[1];
    packVoltageF = (float)packVoltage/10;
    vehicleVoltage = rxmsg.buf[2];
    vehicleVoltage <<= 8;
    vehicleVoltage |= rxmsg.buf[3];
    vehicleVoltageF = (float)vehicleVoltage/10;
    maxCellVoltage = rxmsg.buf[4];
    maxCellVoltage <<= 8;
    maxCellVoltage |= rxmsg.buf[5];
    maxCellVoltageF = (float)maxCellVoltage/1000;
    minCellVoltage = rxmsg.buf[6];
    minCellVoltage <<= 8;
    minCellVoltage |= rxmsg.buf[7];
    minCellVoltageF = (float)minCellVoltage/1000; 
  }
  else if(rxmsg.id == 0x43)
  {
    packCurrent = rxmsg.buf[0];
    packCurrent <<= 8;
    packCurrent |= rxmsg.buf[1];
    packCurrentF = (float)packCurrent/10;
  }
  else
  {
    Serial.print("WTF????");
  }
}

void processDAQCANFrame()
{
  DAQCAN.read(rxmsg);
}

void setupLights()
{
  cdpixels.begin();
  repixels.begin();
  loopPixel(cdpixels.Color(150,150,0));
  loopPixel(cdpixels.Color(0,150,0));
  loopPixel(cdpixels.Color(0,150,150));
  loopPixel(cdpixels.Color(0,0,150));
  loopPixel(cdpixels.Color(150,0,150));
  loopPixel(cdpixels.Color(150,0,0));
}

void loopPixel(uint32_t color )
{
  for(int i=0;i<NUM_CD_PIXELS;i++)
  {
    cdpixels.setPixelColor(i, color);
    cdpixels.show();
    delay(25);
  }
}

void razzleMode()
{
  qbaiLight(ON);
  delay(razzledelay);
  bspdLight(ON);
  delay(razzledelay);
  amsLight(ON);
  delay(razzledelay);
  
  qbaiLight(OFF);
  delay(razzledelay);
  imdLight(ON);
  delay(razzledelay);
  
  bspdLight(OFF);
  delay(razzledelay);
  cdpixels.setPixelColor(0, 207,0,82);
  cdpixels.show();
  delay(razzledelay);
  
  amsLight(OFF);
  delay(razzledelay);
  cdpixels.setPixelColor(1, 143,0,158);
  cdpixels.show();
  delay(razzledelay);
  
  imdLight(OFF);
  delay(razzledelay);
  cdpixels.setPixelColor(2, 103,0,209);
  cdpixels.show();
  delay(razzledelay);

  cdpixels.setPixelColor(0, 0,0,0);
  cdpixels.show();
  delay(razzledelay);
  cdpixels.setPixelColor(3, 42,0,255);
  cdpixels.show();
  delay(razzledelay);

  cdpixels.setPixelColor(1, 0,0,0);
  cdpixels.show();
  delay(razzledelay);
  cdpixels.setPixelColor(4, 0,0,255);
  cdpixels.show();
  delay(razzledelay);

  cdpixels.setPixelColor(2, 0,0,0);
  cdpixels.show();
  delay(razzledelay);
  cdpixels.setPixelColor(5, 0,115,116);
  cdpixels.show();
  delay(razzledelay);

  cdpixels.setPixelColor(3, 0,0,0);
  cdpixels.show();
  delay(razzledelay);
  cdpixels.setPixelColor(6, 0,153,98);
  cdpixels.show();
  delay(razzledelay);

  cdpixels.setPixelColor(4, 0,0,0);
  cdpixels.show();
  delay(razzledelay);
  cdpixels.setPixelColor(7, 0,173,0);
  cdpixels.show();
  delay(razzledelay);

  cdpixels.setPixelColor(5, 0,0,0);
  cdpixels.show();
  delay(razzledelay);
  cdpixels.setPixelColor(8, 0,255,0);
  cdpixels.show();
  delay(razzledelay);

  cdpixels.setPixelColor(6, 0,0,0);
  cdpixels.show();
  delay(razzledelay);
  cdpixels.setPixelColor(9, 199,255,0);
  cdpixels.show();
  delay(razzledelay);

  cdpixels.setPixelColor(7, 0,0,0);
  cdpixels.show();
  delay(razzledelay);
  cdpixels.setPixelColor(10, 253,255,0);
  cdpixels.show();
  delay(razzledelay);

  cdpixels.setPixelColor(8, 0,0,0);
  cdpixels.show();
  delay(razzledelay);
  cdpixels.setPixelColor(11, 255,185,0);
  cdpixels.show();
  delay(razzledelay);

  cdpixels.setPixelColor(9, 0,0,0);
  cdpixels.show();
  delay(razzledelay);
  repixels.setPixelColor(0, 255,185,0);
  repixels.show();
  delay(razzledelay);

  cdpixels.setPixelColor(10, 0,0,0);
  cdpixels.show();
  delay(razzledelay);
  repixels.setPixelColor(1, 0,0,255);
  repixels.show();
  delay(razzledelay);

  cdpixels.setPixelColor(11, 0,0,0);
  cdpixels.show();
  delay(razzledelay);
  qbaiLight(ON);
  delay(razzledelay);

  repixels.setPixelColor(0, 0,0,0);
  repixels.show();
  delay(razzledelay);
  bspdLight(ON);
  delay(razzledelay);

  repixels.setPixelColor(1, 0,0,0);
  repixels.show();
  delay(razzledelay);
  amsLight(ON);
}

void fillSTlogo()
{
  for(int16_t x = -35; x < tft_width; x+=68)
  {
    for(int16_t y = 0; y < tft_height; y+=64)
    {
      tft.drawBitmap(x,y,STlogo, 128, 64, HX8357_BLACK);
    }
  }
}

