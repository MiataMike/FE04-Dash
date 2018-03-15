#ifndef VARIABLES_H
#define VARIABLES_H

#include <SPI.h>
#include <Adafruit_NeoPixel.h>
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_GFX.h>
#include <HX8357_t3.h>
#include <FlexCAN.h>
#include <kinetis_flexcan.h>
#include <SD.h>
#include <string.h>

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
extern bool driveActive;
extern bool startActive;
extern bool shutdownActive;
extern int carSpeed; //car speed read through CAN bus 
extern double carSpeedF; //float

//0x29
extern uint16_t MCTemp;
extern float MCTempF;
extern uint16_t motorTemp;
extern float motorTempF;
extern uint16_t batteryTemp;
extern float batteryTempF;

//0x41
extern bool chargingMode;
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

extern uint8_t driveMode;
extern uint8_t dashpage;
extern uint8_t dashcount;

//CAN Setup
extern FlexCAN CARCAN;
extern FlexCAN DAQCAN;
extern CAN_message_t msg,rxmsg;

//TFT Screen Setup
extern HX8357_t3 tft;
extern int16_t tft_width;
extern int16_t tft_height;
extern uint8_t rotation;
extern bool display_on;
extern bool razzleMode;
extern bool previouslyrazzleMode;

//Key
extern bool on;
extern bool starting;
extern bool previouslyon;
extern uint8_t previousdriveMode;
extern String previousTitle;

extern const unsigned char STlogo [] PROGMEM;

#endif
