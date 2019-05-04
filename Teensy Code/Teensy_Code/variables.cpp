#include <Arduino.h>
#include "variables.h"

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
bool TBPfault = false;
bool driveActive = false;
bool startActive = false;
bool shutdownActive = false;
uint8_t lvVoltage = 0;
float lvVoltageF = 0;
float previouslvVoltageF = 200;
int16_t carSpeed = 100; //car speed read through CAN bus
int16_t previouscarSpeed = 900;
double carSpeedF = 0; //float
uint8_t brakePosition = 0;
uint8_t previousbrakePosition = 255;

//0x29
uint16_t MCTemp = 0;
float MCTempF = 0;
uint16_t motorTemp = 0;
float motorTempF = 0;
uint16_t batteryTemp = 0;
float batteryTempF = 0;

//0x41
bool prechargeContactor = false;
bool negativeContactor = false;
bool positiveContactor = false;
uint16_t faulted = 0;
uint16_t previousmaxCellTemp = 200;
uint16_t maxCellTemp = 60;
float maxCellTempF = 0;
uint16_t minCellTemp = 0;
float minCellTempF = 0;
uint16_t previousHVSOC = 200;
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

//Dash
uint8_t driveMode = 1;
uint8_t previousdriveMode = 100;
uint8_t dashpage = 1;
uint8_t dashcount = 3;
String previousTitle = "Previous";
bool temprangechange = false;
bool brakeScreen = false;
uint16_t maxTorque = 0;
uint16_t previousmaxTorque = 5000;

//CAN Setup
//FlexCAN CARCAN(500000,0);
//FlexCAN DAQCAN(500000,1);

FlexCAN CARCAN(0);
FlexCAN DAQCAN(1);

CAN_message_t txmsg,rxmsg;

//TFT Screen Setup
HX8357_t3 tft = HX8357_t3(CS, DC);
int16_t tft_width = 0;
int16_t tft_height = 0;
uint8_t rotation = 1;

//Key
bool on = false;
bool previouslyStartActive = false;

//NeoPixels
Adafruit_NeoPixel cdpixels = Adafruit_NeoPixel(NUM_CD_PIXELS,CD_neo_pixel_DI, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel repixels = Adafruit_NeoPixel(NUM_RE_PIXELS,RE_neo_pixel_DI, NEO_GRB + NEO_KHZ800);

//State of Charge Servo
Servo socservo;
uint8_t servoval = 0;

//debounce 
unsigned long Ldebounce = 0; 
unsigned long Rdebounce = 0; 
unsigned long DRSdebounce = 0; 
 
//Regen 
uint8_t regen_scaled = 0;

//DRS
bool DRSengaged;

//Fans
uint8_t highTemp = 50;
uint8_t lowTemp = 30;
float fanPWM = 1;
 
bool secretScreen = false;
bool previouslysecretScreen = false;

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
