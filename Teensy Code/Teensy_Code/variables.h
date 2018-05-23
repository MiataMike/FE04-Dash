#ifndef VARIABLES_H
#define VARIABLES_H

#include <SPI.h>
#include <Adafruit_NeoPixel.h>
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_GFX.h>
#include <HX8357_t3.h>
#include <FlexCAN_t3.h>
#include <kinetis_flexcan_t3.h>
#include <SD.h>
#include <string.h>
#include <Servo.h>

//Setting up Pins
#define CD_neo_pixel_DI 2 //Dash
#define NUM_CD_PIXELS 12

#define CARCAN_TX 3  //CAN0
#define CARCAN_RX 4

#define DAQCAN_TX 33  //CAN1
#define DAQCAN_RX 34
 
#define AMS_light 5 //Left ear
#define IMD_light 6
#define BSPD_light 7
#define QBAI_light 8

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
#define NUM_RE_PIXELS 2

#define RJ45_L1 20
#define RJ45_L2 19

#define Ignition_1 35
#define Ignition_2 36

#define SW_bit0 14
#define SW_bit1 15
#define SW_bit2 16
#define SW_bit3 17
#define GS_pin 37

#define ON true
#define OFF false

//0x20
extern uint16_t throttleOneRaw;
extern float throttleOneF;
extern uint16_t throttleTwoRaw;
extern float throttleTwoF;
extern uint16_t brakePressureFrontRaw;
extern float brakePressureFrontF;
extern uint16_t brakePressureRearRaw;
extern float brakePressureRearF;
extern bool brakePressureBool;
extern float brakeMax;
extern bool throttlePlaus;
extern float throttleDiff;

//0x28
extern bool BSPDfault; //statuses
extern bool IMDfault;
extern bool AMSfault;
extern bool TBPfault; //Throttle Brake Plausibility (QBAI)
extern bool driveActive;
extern bool startActive;
extern bool shutdownActive;
extern uint8_t lvVoltage;
extern float lvVoltageF;
extern float previouslvVoltageF;
extern int16_t carSpeed; //car speed read through CAN bus
extern int16_t previouscarSpeed;
extern double carSpeedF; //float
extern uint8_t brakePosition;
extern uint8_t previousbrakePosition;

//0x29
extern uint16_t MCTemp;
extern float MCTempF;
extern uint16_t motorTemp;
extern float motorTempF;
extern uint16_t batteryTemp;
extern float batteryTempF;

//0x41
extern bool prechargeContactor;
extern bool negativeContactor;
extern bool positiveContactor;
extern uint16_t faulted;
extern uint16_t previousmaxCellTemp;
extern uint16_t maxCellTemp;
extern float maxCellTempF;
extern uint16_t minCellTemp;
extern float minCellTempF;
extern uint16_t previousHVSOC;
extern uint16_t HVSOC; //High Voltage State Of Charge
extern float HVSOCF; //float for gauges

//0x42
extern uint16_t packVoltage;
extern float packVoltageF;
extern uint16_t vehicleVoltage;
extern float vehicleVoltageF;
extern uint16_t maxCellVoltage;
extern float maxCellVoltageF;
extern uint16_t minCellVoltage;
extern float minCellVoltageF;

//0x43
extern uint16_t packCurrent;
extern float packCurrentF;

//Dash
extern uint8_t driveMode;
extern uint8_t previousdriveMode;
extern uint8_t dashpage;
extern uint8_t previousdashpage;
extern uint8_t dashcount;
extern String previousTitle;
extern uint8_t razzledelay;
extern bool temprangechange;
extern bool brakeScreen;
extern bool previouslybrakeScreen;
extern uint16_t maxTorque;
extern uint16_t previousmaxTorque;

//CAN Setup
extern FlexCAN CARCAN;
extern FlexCAN DAQCAN;
extern CAN_message_t txmsg,rxmsg;

//TFT Screen Setup
extern HX8357_t3 tft;
extern int16_t tft_width;
extern int16_t tft_height;
extern uint8_t rotation;

//Key
extern bool on;
extern bool previouslyon;
extern bool previouslyStartActive;

//NeoPixels
extern Adafruit_NeoPixel cdpixels;
extern Adafruit_NeoPixel repixels;

//State of Charge Servo
extern Servo socservo;
extern uint8_t servoval;

extern bool secretScreen;
extern bool previouslysecretScreen;

//debounce 
extern unsigned long Ldebounce; 
extern unsigned long Rdebounce; 
extern unsigned long DRSdebounce; 
 
//regen 
#define shortedThreshold 69420 
#define groundThreshold 0 
#define maxRegen 90 
extern uint8_t regen_scaled; 

extern const unsigned char STlogo [] PROGMEM;

#endif
