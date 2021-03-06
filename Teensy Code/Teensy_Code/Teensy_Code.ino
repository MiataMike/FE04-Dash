//To add the necessary included libraries go to Sketch->Include Library->Add .ZIP Library...
//Filter and get all the necessary libraries
//Make sure Board type selected in tools is a teensy

#include <Arduino.h>
#include "variables.h"
#include "screenCode.h"

//regen
#define lower 40
#define upper 80

unsigned long lastCANMillis;

void setup() 
{
  //Fault Light Setup
  pinMode(AMS_light, OUTPUT);
  pinMode(IMD_light, OUTPUT);
  pinMode(BSPD_light, OUTPUT);
  pinMode(QBAI_light, OUTPUT);
  
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
  pinMode(Ignition_2, INPUT_PULLUP);
  
  //Steering Wheel Setup
  pinMode(left_button, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(left_button), scrollDashLeft, FALLING);
  pinMode(right_button, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(right_button), scrollDashRight, FALLING);

  //regen paddle
  pinMode(Paddle,INPUT_PULLUP);

  //Teensy SD Card Setup
  SD.begin(SDCS);
  pinMode(DRS_flap, INPUT_PULLUP); 
  //attachInterrupt(digitalPinToInterrupt(DRS_flap), drsEngage, FALLING); 
  //attachInterrupt(digitalPinToInterrupt(DRS_flap), drsDisengage, RISING); 
   
  
  //Dash Screen Setup
  setupScreen();
  
  
  //Dash Lights Setup
  setupLights();
  
  //Dash CAN Setup
  CARCAN.begin();
  DAQCAN.begin();
  Serial.begin(9600);


  lastCANMillis=millis();


  tft.fillScreen(HX8357_BLACK);
  printCommonBackground();
  updateDriveMode();
  printUpdates();
}

void loop()
{
  regen_scaled = getRegen(); 
  if (CARCAN.available()) 
  {
    processCARCANFrame();
  }
  
  if(DAQCAN.available())
  {
    processDAQCANFrame();
  }

  printUpdates();

  if(previousdriveActive != driveActive)//If car is turned on update text
  {
    changeDriveMode();
    previousdriveActive=driveActive;
  }   
    
  if(previouslyon != on)//If car is turned on update text
  {
    changeDriveMode();
    previouslyon=on;
  }

  //Update Servo
  if(previousHVSOC != HVSOC && HVSOC != 0 && HVSOC <=100)
  {
    updatesocServo();
  }
  previousHVSOC = HVSOC;
  
  //Update Temperature pixels
  if(previousmaxCellTemp != maxCellTemp)
  {
    updateTempPixels();
  }
  updateTempPixels();
  previousmaxCellTemp = maxCellTemp;
  previouscarSpeed = carSpeed;

  imdLight(IMDfault);
  amsLight(AMSfault);
  bspdLight(BSPDfault);
  qbaiLight(TBPfault);

  
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

/*
  if(!digitalRead(Ignition_1) && !driveActive)
  {
    printBrakeScreen();
  }
  else
  {
    brakeScreen = false;
  }
  */
  previousbrakePosition = brakePosition;
  previouslvVoltageF = lvVoltageF;

  driveModeEnabledLight(driveActive);

  unsigned long timeCurrent=millis();
  if((timeCurrent-lastCANMillis)>10)
  {
    Serial.println(timeCurrent-lastCANMillis);
    sendCARCANFrame();
    lastCANMillis=timeCurrent;
  }
}

void updateDriveMode()
{
  previousdriveMode=driveMode;

  if(!driveActive && !startActive)
  {
    driveMode = digitalRead(SW_bit3);
    driveMode <<= 1;
    driveMode |= digitalRead(SW_bit2);
    driveMode <<= 1;
    driveMode |= digitalRead(SW_bit1);
    driveMode <<= 1;
    driveMode |= digitalRead(SW_bit0);

    //driveMode=4;  //DEBUGGING
    
    fixDriveModeNumber();
  }

  changeDriveMode(); 
}

void fixDriveModeNumber()
{
  if(driveMode == 0){ driveMode = 7; }
  else if(driveMode == 1){ driveMode = 6; }
  else if(driveMode == 2){ driveMode = 5; }
  else if(driveMode == 3){ driveMode = 4; }
  else if(driveMode == 4){ driveMode = 3; }
  else if(driveMode == 5){ driveMode = 2; }
  else if(driveMode == 6){ driveMode = 1; }
  else if(driveMode == 7){ driveMode = 0; }
  else if(driveMode == 8){ driveMode = 11; }
  else if(driveMode == 9){ driveMode = 10; }
  else if(driveMode == 10){ driveMode = 9; }
  else if(driveMode == 11){ driveMode = 8; }
}

void changeDriveMode()
{
  switch(driveMode)
  {
    case 0:
      maxTorque = 270;
      printCommonScreenInfo("Accel(1)");
      break;
    case 1:
      maxTorque = 150;
      printCommonScreenInfo("Skid Pad(2)");
      break;
    case 2:
      maxTorque = 240;
      printCommonScreenInfo("AutoX(3)");
      break;
    case 3:
      maxTorque = 200;
      printCommonScreenInfo("Enduro(4)");
      break;
    case 4:
      maxTorque = 200;
      printCommonScreenInfo("Sunday(5)");
      break;
    case 5:
      maxTorque = 100;
      printCommonScreenInfo("Cpt. Slow(6)");
      break;
    case 6:
      maxTorque = 0;
      printCommonScreenInfo("Pedal Cal(7)");
      break;
    case 7:
      maxTorque = 0;
      printCommonScreenInfo("Extra1");
      break;
    case 8:
      maxTorque = 0;
      printCommonScreenInfo("Extra2");
      break;
    case 9:
      maxTorque = 0;
      printCommonScreenInfo("Extra3");
      break;
    case 10:
      maxTorque = 0;
      printCommonScreenInfo("Extra4");
      break;
    case 11:
      maxTorque = 0;
      printCommonScreenInfo("Extra5");
      break;
    default:
      printCommonScreenInfo("default");
      break;
  }
}

void scrollDashLeft()
{
  if (millis() - Ldebounce > 100)
  { 
     if(dashpage == 1)
        dashpage = 3;
     else
        dashpage--; 
    Ldebounce = millis();
  }
}

void scrollDashRight()
{
  if (millis() - Rdebounce > 100)
  {
    if(dashpage == 3)
      dashpage = 1;
    else
      dashpage++;
    Rdebounce = millis(); 
  }  
}

void imdLight(bool ledOn)
{
  digitalWrite(IMD_light, ledOn);
}

void amsLight(bool ledOn)
{
  digitalWrite(AMS_light, ledOn);
}

void bspdLight(bool ledOn)
{
  digitalWrite(BSPD_light, ledOn);
}

void qbaiLight(bool ledOn)
{
  digitalWrite(QBAI_light, ledOn);
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
  if(driveMode == 3)
  {
    if(maxCellTemp <= 54) { pixels = 1; }
    else if(maxCellTemp <= 55) { pixels = 4; }
    else if(maxCellTemp <= 56) { pixels = 6; }
    else if(maxCellTemp <= 57) { pixels = 8; }
    else if(maxCellTemp <= 58) { pixels = 10; }
    else if(maxCellTemp <= 60) { pixels = 12; }
    temprangechange = true;
  }
  else
  {
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
    temprangechange = false;
  }
    
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

//New CAN STUFF

void sendCARCANFrame()
{
  txmsg.id = 0x24;
  txmsg.len = 2;
  txmsg.buf[0] = packageByteZero();
  txmsg.buf[1] = regenConvert(regen_scaled);
  CARCAN.write(txmsg);
}

uint8_t packageByteZero()
{
  uint8_t buf = 0;
  buf |= !digitalRead(Ignition_1);
  buf <<= 1;
  buf |= !digitalRead(Ignition_2);
  buf <<= 1;
  buf |= DRSRead();
  buf <<= 4;
  buf |= driveMode & 0x0F;


  Serial.println(digitalRead(Ignition_1));
  Serial.println(digitalRead(Ignition_2));
  
  return buf;
}
uint8_t regenConvert(uint8_t regenReading) 
{ 
  Serial.println(regenReading);
  if (regenReading < lower) // lower threshold 
  { 
    return 0; 
  } 
  else if (regenReading > upper) 
  { 
    return maxRegen; 
  } 
  else 
  { 
  Serial.println(((regenReading - lower)*1.0) / (upper-lower) * maxRegen);
  return (regenReading - lower) / (upper-lower) * maxRegen; 
  } 
}

void sendDAQCANFrame()
{
  txmsg.id = 0x00;
  txmsg.len = 8;
  txmsg.buf[0] = 0;
  txmsg.buf[1] = 0;
  txmsg.buf[2] = 0;
  txmsg.buf[3] = 0;
  txmsg.buf[4] = 0;
  txmsg.buf[5] = 0;
  txmsg.buf[6] = 0;
  txmsg.buf[7] = 0;
  DAQCAN.write(txmsg);
}

//int DRSByte()
int DRSRead()
{
  Serial.print("drs");
  Serial.println(digitalRead(DRS_flap));
  if (analogRead(DRS_flap) > 2000)
  {
    return 1;
  }
  else
  {
    return 0;
  }
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

  /* Master Blaster Messages*/
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
    dataByte >>=1;
    if((dataByte & 0x01) == 1){ TBPfault = true; }
    else TBPfault = false;
    dataByte >>=1;
    if((dataByte & 0x01) == 1){ on = true; }
    else on = false;

    lvVoltage = rxmsg.buf[3];
    lvVoltage <<= 8;
    lvVoltage |= rxmsg.buf[4];
    lvVoltageF = (float)lvVoltage / 10;
    lvVoltage = lvVoltage / 10;
    
    carSpeed = rxmsg.buf[5];
    carSpeed <<= 8;
    carSpeed |= rxmsg.buf[6];
    carSpeedF = double(carSpeed)/10;
    carSpeed = carSpeed / 10;
    if(carSpeed > 32767)
    {
      carSpeed = carSpeed - 65536;
    }
    carSpeed = abs(carSpeed);
    brakePosition = rxmsg.buf[7];
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
  else if(rxmsg.id == 0x10)
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
  else if(rxmsg.id == 0x11)
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
  else if(rxmsg.id == 0x12)
  {
    packCurrent = rxmsg.buf[0];
    packCurrent <<= 8;
    packCurrent |= rxmsg.buf[1];
    packCurrentF = (float)packCurrent/10;
  }
  else
  {
    //Serial.print("WTF????");
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

void drsDisengage() 
{ 
  DRSengaged = false;
  return; 
} 

void drsEngage() 
{ 
    DRSengaged = true;
  return; 
} 

uint16_t getRegen() 
{ 
  uint16_t reading;  
  reading = analogRead(Paddle);
  //Serial.println(reading);
  if(reading < groundThreshold) 
  { 
    //grounded (unplugged) 
    return 0; 
  } 
  else if(reading > shortedThreshold) 
  { 
    //SHORTED 
    return 0; 
  } 
  else 
  { 
  return reading/8; 
  } 
} 
 
