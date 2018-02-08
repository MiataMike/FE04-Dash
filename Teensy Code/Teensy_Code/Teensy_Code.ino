//To add the necessary included libraries go to Sketch->Include Library->Manage Libraries...
//Filter and get all the necessary libraries

#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>
#include <Adafruit_HX8357.h>

void setup() 
{
  //Setting up Pins
#define D_neo_pixel_DI 2; //Dash 

#define CAN_TX 3;
#define CAN_RX 4;
 
#define AMS_light 5;
#define IMD_light 6;
#define BSPD_light 7;
#define TPS_light 8;

#define DC 9;
#define CS 10;
#define MOSI 11;
#define MISO 12;

#define Paddle A13;
#define up_button 24;
#define down_button 25;
#define left_button 26;
#define right_button 27;
#define DRS_flap 28;

#define servo_PWM 22;
#define RE_neo_pixel_DI 21; //Right ear

#define RJ45_L1 20;
#define RJ45_L2 19;

#define SW_bit3 17;
#define SW_bit2 16;
#define SW_bit1 15;
#define SW_bit0 14;

#define spare_5 37;
#define spare_4 36;
#define spare_3 35;
#define spare_2 34;
#define spare_1 33;

}

void loop() {
  // put your main code here, to run repeatedly:

}
