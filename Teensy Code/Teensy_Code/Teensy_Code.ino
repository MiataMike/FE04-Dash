//To add the necessary included libraries go to Sketch->Include Library->Manage Libraries...
//Filter and get all the necessary libraries
//Make sure Board type selected in tools is a teensy

#include <SPI.h>
#include <Adafruit_NeoPixel.h>
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_GFX.h>
#include <HX8357_t3.h>
#include <FlexCAN.h>
#include <kinetis_flexcan.h>
#include <SD.h>

//Setting up Pins
#define D_neo_pixel_DI 2 //Dash 

#define CARCAN_TX 3  //CAN0
#define CARCAN_RX 4

#define DAQCAN_TX 33  //CAN1
#define DAQCAN_RX 34
 
#define AMS_light 5 //Left ear
#define IMD_light 6
#define BSPD_light 7
#define TPS_light 8

#define DC 9
#define CS 10
#define MOSI 11
#define MISO 12
#define CLK 13
#define SDCS BUILTIN_SDCARD

#define Paddle A13
#define up_button 24
#define down_button 25
#define left_button 26
#define right_button 27
#define DRS_flap 28

#define servo_PWM 22 //Right ear
#define RE_neo_pixel_DI 21

#define RJ45_L1 20
#define RJ45_L2 19

#define Ignition_1 35
#define Ignition_2 36

#define SW_bit0 14
#define SW_bit1 15
#define SW_bit2 16
#define SW_bit3 17

#define spare_1 37

//0x20
uint16_t throttleOneRaw = 0;
float throttleOneF = 0;
uint16_t throttleTwoRaw = 0;
float throttleTwoF = 0;
uint16_t brakePressureFrontRaw = 0;
float brakePressureFrontF = 0;
uint16_t brakePressureRearRaw = 0;
float brakePressureRearF = 0;
bool brakePressureBool = false;
float brakeMax = 0;
bool throttlePlaus = false;
float throttleDiff = 0;

//0x28
bool BSPDfault = false; //statuses
bool IMDfault = false;
bool AMSfault = false;
bool driveActive = false;
bool startActive = false;
bool shutdownActive = false;
int carSpeed = 0; //car speed read through CAN bus 
double carSpeedF = 0; //float

//0x29
uint16_t MCTemp = 0;
float MCTempF = 0;
uint16_t motorTemp = 0;
float motorTempF = 0;
uint16_t batteryTemp = 0;
float batteryTempF = 0;

//0x41
bool chargingMode = false;
bool prechargeContactor = false;
bool negativeContactor = false;
bool positiveContactor = false;
uint16_t faulted = 0;
uint16_t maxCellTemp = 0;
float maxCellTempF = 0;
uint16_t minCellTemp = 0;
float minCellTempF = 0;
uint16_t HVSOC = 0; //High Voltage State Of Charge
float HVSOCF = 0; //float for gauges

//0x42
uint16_t packVoltage = 0;
float packVoltageF = 0;
uint16_t vehicleVoltage = 0;
float vehicleVoltageF = 0;
uint16_t maxCellVoltage = 0;
float maxCellVoltageF = 0;
uint16_t minCellVoltage = 0;
float minCellVoltageF = 0;

//0x43
uint16_t packCurrent = 0;
float packCurrentF = 0;

uint8_t driveMode = 1;
uint8_t driveModeCount = 12;
uint8_t previousdriveMode  = 100;

//CAN Setup
FlexCAN CARCAN(0);
FlexCAN DAQCAN(1);
static CAN_message_t msg,rxmsg;

//TFT Screen Setup
HX8357_t3 tft = HX8357_t3(CS, DC);
int16_t tft_width = 0;
int16_t tft_height = 0;
uint8_t rotation = 3;
bool display_on = false;

//Key
bool on = false;
bool starting = false;
bool previouslyon = false;
uint8_t previousScreenNumber = 100;

const unsigned char STlogo [] PROGMEM = {
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xE0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3C, 0xF8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xE0, 0x0C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x80, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x01, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x1E, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x7F, 0xA1, 0xF8, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0xFC, 0xC3, 0xEC, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0xF8, 0x87, 0xC4, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0xF3, 0x87, 0x84, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x7E, 0x06, 0x18, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x42, 0x33, 0xE0, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x43, 0xC0, 0x00, 0x80, 0x00, 0x07, 0xC0, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0xC0, 0x07, 0xF8, 0x80, 0x00, 0x1F, 0xE0, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x80, 0x00, 0x3F, 0xD8, 0x1C, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x80, 0x00, 0x63, 0xC7, 0x7E, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x80, 0x00, 0x61, 0x27, 0x9E, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x80, 0x00, 0xC1, 0x37, 0x9E, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x80, 0x00, 0xC1, 0xFF, 0x9A, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x80, 0x00, 0x81, 0xFC, 0x62, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x28, 0xC0, 0x01, 0x83, 0xDC, 0x62, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x28, 0x40, 0x03, 0x03, 0xC7, 0xE6, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x28, 0x41, 0xFF, 0x06, 0x47, 0x1E, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x28, 0x6E, 0x0E, 0x0C, 0x3F, 0x1E, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0xC0, 0x00, 0x28, 0x30, 0x0C, 0x18, 0x7F, 0x3A, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x00, 0x28, 0x23, 0x18, 0x1F, 0xF1, 0xC2, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x00, 0x28, 0x2D, 0xFF, 0x80, 0xF9, 0xC2, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x60, 0x00, 0x28, 0x03, 0x00, 0xE7, 0xC7, 0xE6, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 0x28, 0x04, 0x00, 0x31, 0xE0, 0xFC, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 0x28, 0x00, 0x00, 0x10, 0x30, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x00, 0x66, 0x00, 0x00, 0x18, 0x70, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x00, 0xC1, 0x00, 0x00, 0x0F, 0xE0, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x00, 0x84, 0x80, 0x00, 0x0F, 0x80, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x01, 0x6B, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x01, 0xB8, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x80, 0x00, 0x00, 0x00, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC0, 0x00, 0x00, 0x00, 0xE0, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x60, 0x00, 0x00, 0x01, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3C, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0F, 0xC0, 0x01, 0xFE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0xFF, 0xF9, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x80, 0x09, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0x80, 0x09, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x8D, 0x83, 0xF9, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x69, 0x01, 0x19, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x3B, 0x00, 0xC1, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x82, 0x00, 0x61, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC6, 0x00, 0x39, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3C, 0x00, 0x0F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

void setup() 
{
  pinMode(AMS_light, OUTPUT);
  pinMode(IMD_light, OUTPUT);
  pinMode(BSPD_light, OUTPUT);
  pinMode(TPS_light, OUTPUT);
  pinMode(servo_PWM, OUTPUT);
  pinMode(SW_bit0, INPUT);
  attachInterrupt(digitalPinToInterrupt(SW_bit0), updateDriveMode, CHANGE);
  pinMode(SW_bit1, INPUT);
  pinMode(SW_bit2, INPUT);
  pinMode(SW_bit3, INPUT);
  pinMode(Ignition_1, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(Ignition_1), displayDriveMode, CHANGE);
  pinMode(Ignition_2, INPUT_PULLUP);

  SD.begin(SDCS);
  
  tft.begin(HX8357D);
  if(rotation == 1 || rotation == 3)
  {
    tft_width = tft.height();
    tft_height = tft.width();
  }
  else if(rotation == 2 || rotation == 4)
  {
    tft_width = tft.width();
    tft_height = tft.height();
  }
  tft.setRotation(rotation);
  startScreen();
  
  CARCAN.begin(500000);
  DAQCAN.begin(500000);
}

void loop()
{
  if(on)
  {
    if(!previouslyon)
    {
      tft.fillScreen(HX8357_BLACK);
      printTopBar();
      changeDriveMode();
    }
    else
    {
      changeDriveMode();
    }
    previouslyon = true;
  }
  else if(!on && previouslyon)
  {
    startScreen();
    previouslyon = false;
  }
  
  if(CARCAN.available())
  {
    CARCAN.read(rxmsg);
    processCARCANFrame(rxmsg);
  }
  
  if(DAQCAN.available())
  {
    DAQCAN.read(rxmsg);
    processDAQCANFrame(rxmsg);
  }
}

void startScreen()
{
    bmpDraw("logo.bmp", 0, 0);
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
}

void updateDriveMode()
{
  driveMode = digitalRead(SW_bit3);
  driveMode <<= 1;
  driveMode |= digitalRead(SW_bit2);
  driveMode <<= 1;
  driveMode |= digitalRead(SW_bit1);
  driveMode <<= 1;
  driveMode |= digitalRead(SW_bit0);
}

void changeDriveMode()
{
  switch(driveMode)
  {
    case 10:
      //mode 0
      printScreenNumber(0);
      break;
    case 9:
      //mode 1
      printScreenNumber(1);
      break;
    case 8:
      //mode 2
      printScreenNumber(2);
      break;
    case 7:
      //mode 3
      printScreenNumber(3);
      break;
    case 6:
      //mode 4
      printScreenNumber(4);
      break;
    case 5:
      //mode 5
      printScreenNumber(5);
      break;
    case 4:
      //mode 6
      printScreenNumber(6);
      break;
    case 3:
      //mode 7
      printScreenNumber(7);
      break;
    case 2:
      //mode 8
      printScreenNumber(8);
      break;
    case 1:
      //mode 9
      printScreenNumber(9);
      break;
    case 0:
      //mode 10
      printScreenNumber(10);
      break;
    case 11:
      //mode 11
      printScreenNumber(11);
      break;
    default:
      //FUCK??
      tft.fillScreen(HX8357_BLACK);
      tft.setCursor(40,40);
      tft.setTextColor(HX8357_WHITE);
      tft.setTextSize(14);
      tft.println("DON'T");
      tft.setCursor(40,200);
      tft.setTextSize(13);
      tft.println("PANIC");
      tft.setCursor(400,200);
      tft.println("!");
      break;
  }
}

void printTopBar()
{
  tft.drawRect(0,0,tft_width,75,HX8357_GREEN);
  tft.drawFastVLine(tft_width/4,0,75,HX8357_GREEN);
  tft.drawFastVLine(3*(tft_width/4),0,75,HX8357_GREEN);
}

void printScreenNumber(uint8_t number)
{
  tft.setCursor((tft_width/2)-25,tft_height-35);
  tft.setTextSize(4);
  tft.setTextColor(HX8357_BLACK);
  if(previousScreenNumber != number || !previouslyon)
  {
    tft.println(previousScreenNumber);
    tft.setTextColor(HX8357_GREEN);
    tft.setCursor((tft_width/2)-25,tft_height-35);
    tft.println(number);
    previousScreenNumber = number;
  }
}

void amsLight(bool on)
{
  digitalWrite(AMS_light, on);
}

void imdLight(bool on)
{
  digitalWrite(IMD_light, on);
}

void bspdLight(bool on)
{
  digitalWrite(BSPD_light, on);
}

void tpsLight(bool on)
{
  digitalWrite(TPS_light, on);
}

void updatesocServo(int var)  //What should I pass it??
{
  digitalWrite(servo_PWM, HIGH);
  delayMicroseconds(1800);    //Update the delays
  digitalWrite(20,LOW);
  delay(10);
}


//CAN Stuff
void sendCARCANFrame()
{
  msg.id = 0x24;
  msg.len = 8;
  msg.buf[0] = 0;
  msg.buf[1] = 1;
  msg.buf[2] = 2;
  msg.buf[3] = 3;
  msg.buf[4] = 4;
  msg.buf[5] = 5;
  msg.buf[6] = 6;
  msg.buf[7] = 7;
  CARCAN.write(msg);
}

void sendDAQCANFrame()
{
  msg.id = 0x00;
  msg.len = 8;
  msg.buf[0] = 0;
  msg.buf[1] = 1;
  msg.buf[2] = 2;
  msg.buf[3] = 3;
  msg.buf[4] = 4;
  msg.buf[5] = 5;
  msg.buf[6] = 6;
  msg.buf[7] = 7;
  DAQCAN.write(msg);
}

void processCARCANFrame(CAN_message_t f)
{
  //Pedal Board
  uint8_t dataByte = 0;
  if(f.id == 0x20)
  {
    throttleOneRaw = f.buf[0];
    throttleOneRaw <<= 8;
    throttleOneRaw |= f.buf[1];
    throttleOneF = (float)(throttleOneRaw-256)/18.3;
    throttleTwoRaw = f.buf[2];
    throttleTwoRaw <<= 8;
    throttleTwoRaw |= f.buf[3];
    throttleTwoF = (float)(throttleTwoRaw-256)/18.3;
    throttleDiff = abs(throttleOneF-throttleTwoF);
    
    brakePressureFrontRaw = f.buf[4];
    brakePressureFrontRaw <<= 8;
    brakePressureFrontRaw |= f.buf[5];
    brakePressureFrontF = (float)(brakePressureFrontRaw-256)/10;
    if(brakePressureFrontF >= 13){ brakePressureBool = true; }
    else brakePressureBool = false;
    if(brakePressureFrontF >= brakeMax){ brakeMax = brakePressureFrontF; }
    brakePressureRearRaw = f.buf[6];
    brakePressureRearRaw <<= 8;
    brakePressureRearRaw |= f.buf[7];
    brakePressureRearF = (float)(brakePressureRearRaw-256)/10;
  }

  //Bridge Card Messages
  else if(f.id == 0x28)
  {
    dataByte = f.buf[0];
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
    carSpeed = f.buf[5];
    carSpeed <<= 8;
    carSpeed |= f.buf[6];
    carSpeedF = double(carSpeed)/10;
  }
  else if(f.id == 0x29)
  {
    MCTemp = f.buf[0];
    MCTemp <<= 8;
    MCTemp |= f.buf[1];
    MCTempF = (float)MCTemp/10;
    motorTemp = f.buf[2];
    motorTemp <<= 8;
    motorTemp |= f.buf[3];
    motorTempF = (float)motorTemp/10;
    batteryTemp = f.buf[4];
    batteryTemp <<= 8;
    batteryTemp |= f.buf[5];
    batteryTempF = (float)batteryTemp/10;
  }

  //AMS Messages
  else if(f.id == 0x41)
  {
    faulted = f.buf[0] << 1;
    dataByte = f.buf[1];
    faulted |= dataByte >> 7;
    if((dataByte & 0x01) == 1){ chargingMode = true; }
    else chargingMode = false;
    dataByte >>=1;
    if((dataByte & 0x01) == 1){ prechargeContactor = true; }
    else prechargeContactor = false;
    dataByte >>=1;
    if((dataByte & 0x01) == 1){ negativeContactor = true; }
    else negativeContactor = false;
    dataByte >>=1;
    if((dataByte & 0x01) == 1){ positiveContactor = true; }
    else positiveContactor = false;
    maxCellTemp = f.buf[2];
    maxCellTemp <<= 8;
    maxCellTemp |= f.buf[3];
    maxCellTempF = (float)maxCellTemp/10;
    minCellTemp = f.buf[4];
    minCellTemp <<= 8;
    minCellTemp |= f.buf[5];
    minCellTempF = (float)minCellTemp/10;      
    HVSOC = f.buf[6];
    HVSOC <<= 8;
    HVSOC |= f.buf[7];
    HVSOCF = (float)HVSOC/10;   
  }
  else if(f.id == 0x42)
  {
    packVoltage = f.buf[0];
    packVoltage <<= 8;
    packVoltage |= f.buf[1];
    packVoltageF = (float)packVoltage/10;
    vehicleVoltage = f.buf[2];
    vehicleVoltage <<= 8;
    vehicleVoltage |= f.buf[3];
    vehicleVoltageF = (float)vehicleVoltage/10;
    maxCellVoltage = f.buf[4];
    maxCellVoltage <<= 8;
    maxCellVoltage |= f.buf[5];
    maxCellVoltageF = (float)maxCellVoltage/1000;
    minCellVoltage = f.buf[6];
    minCellVoltage <<= 8;
    minCellVoltage |= f.buf[7];
    minCellVoltageF = (float)minCellVoltage/1000; 
  }
  else if(f.id == 0x43)
  {
    packCurrent = f.buf[0];
    packCurrent <<= 8;
    packCurrent |= f.buf[1];
    packCurrentF = (float)packCurrent/10;
  }
  else
  {
    Serial.print("WTF????");
  }
}

void processDAQCANFrame(CAN_message_t f)
{
  
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

#define BUFFPIXEL 80


//===========================================================
// Try Draw using writeRect
void bmpDraw(const char *filename, uint8_t x, uint16_t y) {

  File     bmpFile;
  int      bmpWidth, bmpHeight;   // W+H in pixels
  uint8_t  bmpDepth;              // Bit depth (currently must be 24)
  uint32_t bmpImageoffset;        // Start of image data in file
  uint32_t rowSize;               // Not always = bmpWidth; may have padding
  uint8_t  sdbuffer[3*BUFFPIXEL]; // pixel buffer (R+G+B per pixel)
  uint16_t buffidx = sizeof(sdbuffer); // Current position in sdbuffer
  boolean  goodBmp = false;       // Set to true on valid header parse
  boolean  flip    = true;        // BMP is stored bottom-to-top
  int      w, h, row, col;
  uint8_t  r, g, b;
  uint32_t pos = 0, startTime = millis();

  uint16_t awColors[480];  // hold colors for one row at a time...

  if((x >= tft.width()) || (y >= tft.height())) return;

  Serial.println();
  Serial.print(F("Loading image '"));
  Serial.print(filename);
  Serial.println('\'');

  // Open requested file on SD card
  if (!(bmpFile = SD.open(filename))) {
    Serial.print(F("File not found"));
    return;
  }

  // Parse BMP header
  if(read16(bmpFile) == 0x4D42) { // BMP signature
    Serial.print(F("File size: ")); Serial.println(read32(bmpFile));
    (void)read32(bmpFile); // Read & ignore creator bytes
    bmpImageoffset = read32(bmpFile); // Start of image data
    Serial.print(F("Image Offset: ")); Serial.println(bmpImageoffset, DEC);
    // Read DIB header
    Serial.print(F("Header size: ")); Serial.println(read32(bmpFile));
    bmpWidth  = read32(bmpFile);
    bmpHeight = read32(bmpFile);
    if(read16(bmpFile) == 1) { // # planes -- must be '1'
      bmpDepth = read16(bmpFile); // bits per pixel
      Serial.print(F("Bit Depth: ")); Serial.println(bmpDepth);
      if((bmpDepth == 24) && (read32(bmpFile) == 0)) { // 0 = uncompressed

        goodBmp = true; // Supported BMP format -- proceed!
        Serial.print(F("Image size: "));
        Serial.print(bmpWidth);
        Serial.print('x');
        Serial.println(bmpHeight);

        // BMP rows are padded (if needed) to 4-byte boundary
        rowSize = (bmpWidth * 3 + 3) & ~3;

        // If bmpHeight is negative, image is in top-down order.
        // This is not canon but has been observed in the wild.
        if(bmpHeight < 0) {
          bmpHeight = -bmpHeight;
          flip      = false;
        }

        // Crop area to be loaded
        w = bmpWidth;
        h = bmpHeight;
        if((x+w-1) >= tft.width())  w = tft.width()  - x;
        if((y+h-1) >= tft.height()) h = tft.height() - y;

        for (row=0; row<h; row++) { // For each scanline...

          // Seek to start of scan line.  It might seem labor-
          // intensive to be doing this on every line, but this
          // method covers a lot of gritty details like cropping
          // and scanline padding.  Also, the seek only takes
          // place if the file position actually needs to change
          // (avoids a lot of cluster math in SD library).
          if(flip) // Bitmap is stored bottom-to-top order (normal BMP)
            pos = bmpImageoffset + (bmpHeight - 1 - row) * rowSize;
          else     // Bitmap is stored top-to-bottom
            pos = bmpImageoffset + row * rowSize;
          if(bmpFile.position() != pos) { // Need seek?
            bmpFile.seek(pos);
            buffidx = sizeof(sdbuffer); // Force buffer reload
          }

          for (col=0; col<w; col++) { // For each pixel...
            // Time to read more pixel data?
            if (buffidx >= sizeof(sdbuffer)) { // Indeed
              bmpFile.read(sdbuffer, sizeof(sdbuffer));
              buffidx = 0; // Set index to beginning
            }

            // Convert pixel from BMP to TFT format, push to display
            b = sdbuffer[buffidx++];
            g = sdbuffer[buffidx++];
            r = sdbuffer[buffidx++];
            awColors[col] = tft.color565(r,g,b);
          } // end pixel
          tft.writeRect(0, row, w, 1, awColors);
        } // end scanline
        Serial.print(F("Loaded in "));
        Serial.print(millis() - startTime);
        Serial.println(" ms");
      } // end goodBmp
    }
  }

  bmpFile.close();
  if(!goodBmp) Serial.println(F("BMP format not recognized."));
}



// These read 16- and 32-bit types from the SD card file.
// BMP data is stored little-endian, Arduino is little-endian too.
// May need to reverse subscript order if porting elsewhere.

uint16_t read16(File &f) {
  uint16_t result;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read(); // MSB
  return result;
}

uint32_t read32(File &f) {
  uint32_t result;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read();
  ((uint8_t *)&result)[2] = f.read();
  ((uint8_t *)&result)[3] = f.read(); // MSB
  return result;
}
