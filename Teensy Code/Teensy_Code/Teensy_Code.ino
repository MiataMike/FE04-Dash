//To add the necessary included libraries go to Sketch->Include Library->Manage Libraries...
//Filter and get all the necessary libraries

#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_NeoPixel.h>
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_HX8357.h>
#include <FlexCAN.h>
#include <kinetis_flexcan.h>

//Setting up Pins
#define D_neo_pixel_DI 2 //Dash 

#define CAN_TX 3  //Only Pins for flexCan are 3 and 4
#define CAN_RX 4
 
#define AMS_light 5 //Left ear
#define IMD_light 6
#define BSPD_light 7
#define TPS_light 8

#define DC 9
#define CS 10
#define MOSI 11
#define MISO 12

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

#define Ignition_1 33
#define Ignition_2 34

#define SW_bit0 14
#define SW_bit1 15
#define SW_bit2 16
#define SW_bit3 17

#define spare_1 35
#define spare_2 36
#define spare_3 37

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

uint8_t dash = 1;//start on dash 1
uint8_t dashCount = 5;//total number of dashes

//CAN setup
FlexCAN CANbus(500000);
static CAN_message_t msg,rxmsg;

void setup() 
{
  pinMode(AMS_light, OUTPUT);
  pinMode(IMD_light, OUTPUT);
  pinMode(BSPD_light, OUTPUT);
  pinMode(TPS_light, OUTPUT);
  pinMode(servo_PWM, OUTPUT);
  pinMode(up_button, INPUT);
  pinMode(down_button, INPUT);
  pinMode(left_button, INPUT);
  pinMode(right_button, INPUT);
  
  CANbus.begin();

}

void loop()
{
  
  CANbus.read(rxmsg);
  processFrame(rxmsg);
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

void sendFrame()
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
  CANbus.write(msg);
}

void processFrame(CAN_message_t f)
{
  //Pedal Board
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
    uint8_t dataByte = f.buf[0];
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

