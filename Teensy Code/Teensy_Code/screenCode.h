#ifndef SCREENCODE_H
#define SCREENCODE_H

#include <Arduino.h>
#include "variables.h"

void setupScreen();
void startScreen();
void printCommonBackground();
void updateScreenSOC();
void updateScreenBatteryTemp();
void updateScreenCarSpeed();
void printScreenNumber();
void printScreenTitle(String title, uint8_t number);
void printCommonScreenInfo(String title, uint8_t number);
void bmpDraw(const char *filename, uint8_t x, uint16_t y);
uint16_t read16(File &f);
uint32_t read32(File &f);


#endif
