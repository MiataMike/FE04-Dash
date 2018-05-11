#include "screenCode.h"

void setupScreen()
{
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
}

void startScreen()
{
  bmpDraw("logo.bmp", 0, 0);
}

void printCommonBackground()
{
  if(!previouslyon || previousdriveMode == 11 || previousdriveMode == 10 || previouslybrakeScreen != brakeScreen)
  {
    tft.drawRect(0,0,tft_width,tft_height,HX8357_GREEN);
    tft.drawFastHLine(0,75,tft_width,HX8357_GREEN);
    tft.drawFastVLine(tft_width/4,0,75,HX8357_GREEN);
    tft.drawFastVLine(3*(tft_width/4),0,75,HX8357_GREEN);
    tft.drawFastHLine(0,tft_height-50,tft_width,HX8357_GREEN);
    tft.drawFastVLine(tft_width/4,tft_height-50,50,HX8357_GREEN);
    tft.drawFastVLine(3*(tft_width/4),tft_height-50,50,HX8357_GREEN);
    //tft.drawFastVLine(tft_width/2,0,tft_height,HX8357_GREEN);

    tft.setTextColor(HX8357_GREEN);
    tft.setCursor(43,60);
    tft.setTextSize(2);
    tft.println("SOC");

    tft.setCursor(367,60);
    tft.setTextSize(2);
    tft.println("Batt Temp");

    tft.setCursor((tft_width/2)-17,150);
    tft.setTextSize(2);
    tft.print("MPH");

    tft.setCursor(18,tft_height-45);
    tft.setTextSize(2);
    tft.print("Max Trq");
    
    tft.setCursor(tft_width-65,tft_height-45);
    tft.setTextSize(2);
    tft.print("LV");
  }
  
  if(previousHVSOC != HVSOC || !previouslyon || previousdriveMode == 11 || previousdriveMode == 10 || previouslybrakeScreen != brakeScreen)
  {
    updateScreenSOC();
  }
  
  if(previousmaxCellTemp != maxCellTemp || !previouslyon || previousdriveMode == 11 || previousdriveMode == 10 || previouslybrakeScreen != brakeScreen)
  {
    updateScreenBatteryTemp();
  }

  if(previouscarSpeed != carSpeed || !previouslyon || previousdriveMode == 11 || previousdriveMode == 10 || previouslybrakeScreen != brakeScreen)
  {
    updateScreenCarSpeed();
  }

  if(previouslvVoltage != lvVoltage || !previouslyon || previousdriveMode == 11 || previousdriveMode == 10 || previouslybrakeScreen != brakeScreen)
  {
    updateScreenLVVoltage();
  }
}

void updateScreenSOC()
{
  tft.setTextSize(4);
  tft.setTextColor(HX8357_BLACK);
  if(previousHVSOC >= 100){ tft.setCursor(15,20); }
  else if(previousHVSOC < 100 && previousHVSOC > 9){ tft.setCursor(30,20); }
  else if(previousHVSOC < 10){ tft.setCursor(40,20); }
  tft.print(previousHVSOC);
  tft.print("%");
  tft.setTextColor(HX8357_GREEN);
  if(HVSOC >= 100){ tft.setCursor(15,20); }
  else if(HVSOC < 100 && HVSOC > 9){ tft.setCursor(30,20); }
  else if(HVSOC < 10){ tft.setCursor(40,20); }
  tft.print(HVSOC);
  tft.print("%"); 
}

void updateScreenBatteryTemp()
{
  tft.setTextSize(4);
  tft.setTextColor(HX8357_BLACK);
  if(previousmaxCellTemp >= 100){ tft.setCursor(3*(tft_width/4)+15,20); }
  else if(previousmaxCellTemp < 100 && previousmaxCellTemp > 9){ tft.setCursor(3*(tft_width/4)+30,20); }
  else if(previousmaxCellTemp < 10){ tft.setCursor(3*(tft_width/4)+40,20); }
  tft.print(previousmaxCellTemp);
  tft.print("C");
  tft.setTextColor(HX8357_GREEN);
  if(maxCellTemp >= 100){ tft.setCursor(3*(tft_width/4)+15,20); }
  else if(maxCellTemp < 100 && maxCellTemp > 9){ tft.setCursor(3*(tft_width/4)+30,20); }
  else if(maxCellTemp < 10){ tft.setCursor(3*(tft_width/4)+40,20); }
  tft.print(maxCellTemp);
  tft.print("C"); 
}

void updateScreenCarSpeed()
{
  tft.setTextSize(8);
  tft.setTextColor(HX8357_BLACK);
  if(previouscarSpeed >= 100){ tft.setCursor((tft_width/2)-67,85); }
  else if(previouscarSpeed < 100 && previouscarSpeed > 9){ tft.setCursor((tft_width/2)-43,85); }
  else if(previouscarSpeed < 10){ tft.setCursor((tft_width/2)-20,85); }
  tft.print(previouscarSpeed);
  tft.setTextColor(HX8357_GREEN);
  if(carSpeed >= 100){ tft.setCursor((tft_width/2)-67,85); }
  else if(carSpeed < 100 && carSpeed > 9){ tft.setCursor((tft_width/2)-43,85); }
  else if(carSpeed < 10){ tft.setCursor((tft_width/2)-20,85); }
  tft.print(carSpeed);
}

void updateScreenLVVoltage()
{
  tft.setTextSize(3);
  tft.setTextColor(HX8357_BLACK);
  if(previouslvVoltage > 9){ tft.setCursor(3*(tft_width/4)+40,tft_height-25); }
  else if(previouslvVoltage < 10){ tft.setCursor(3*(tft_width/4)+48,tft_height-25); }
  tft.print(previouslvVoltage);
  tft.print("V");
  tft.setTextColor(HX8357_GREEN);
  if(lvVoltage > 9){ tft.setCursor(3*(tft_width/4)+40,tft_height-25); }
  else if(lvVoltage < 10){ tft.setCursor(3*(tft_width/4)+48,tft_height-25); }
  tft.print(lvVoltage);
  tft.print("V"); 
}

void updateScreenMaxTorque()
{
  tft.setTextSize(3);
  tft.setTextColor(HX8357_BLACK);
  if(previousmaxTorque > 99){ tft.setCursor(15,tft_height-25); }
  else if(previousmaxTorque < 100 && maxTorque > 9){ tft.setCursor(26,tft_height-25); }
  else if(previousmaxTorque < 10){ tft.setCursor(38,tft_height-25); }
  tft.print(previousmaxTorque);
  tft.print("Nm");
  tft.setTextColor(HX8357_GREEN);
  if(maxTorque > 99){ tft.setCursor(15,tft_height-25); }
  else if(maxTorque < 100 && maxTorque > 9){ tft.setCursor(26,tft_height-25); }
  else if(maxTorque < 10){ tft.setCursor(38,tft_height-25); }
  tft.print(maxTorque);
  tft.print("Nm");
}

void printScreenNumber()
{
  tft.setCursor((tft_width/2)-10,tft_height-35);
  tft.setTextSize(4);
  tft.setTextColor(HX8357_BLACK);
  tft.println(previousdriveMode);
  if(driveActive){ tft.setTextColor(HX8357_BLUE); }
  else if(startActive){ tft.setTextColor(HX8357_RED); }
  else{ tft.setTextColor(HX8357_GREEN); }
  tft.setCursor((tft_width/2)-10,tft_height-35);
  tft.println(driveMode);
}

void printScreenTitle(String title, uint8_t number)
{
  tft.setCursor((tft_width/2)-70,30);
  tft.setTextSize(3);
  tft.fillRect(2+(tft_width/4),2,(tft_width/2)-2,73,HX8357_BLACK);
  if(driveActive){ tft.setTextColor(HX8357_BLUE); }
  else if(startActive){ tft.setTextColor(HX8357_RED); }
  else{ tft.setTextColor(HX8357_GREEN); }
  switch(number)
  {
    case 0:
      tft.setCursor(tft_width/4 + 13, 30); //GOOD
      break;
    case 1:
      tft.setCursor(tft_width/4 + 50, 30); //GOOD
      break;
    case 2:
      tft.setCursor(tft_width/4 + 40, 30); //GOOD
      break;
    case 3:
      tft.setCursor(tft_width/4 + 40, 30); //GOOD
      break;
    case 4: //THIS ONE IS SPECIAL
      break;
    case 5:
      tft.setCursor(tft_width/4 + 22, 30); //GOOD
      break;
    case 6:
      tft.setCursor(tft_width/4 + 60, 30); //GOOD
      break;
    case 7:
      tft.setCursor(tft_width/4 + 65, 30); //GOOD
      break;
    case 8:
      tft.setCursor(tft_width/4 + 65, 30); //GOOD
      break;
    case 9:
      tft.setCursor(tft_width/4 + 65, 30); //GOOD
      break;
    case 10:
      tft.setCursor(tft_width/4 + 65, 30); //GOOD
      break;
    case 11:
      tft.setCursor(tft_width/4 + 5, 30); //GOOD
      break;
    case 12:
      tft.setCursor(tft_width/4 + 65, 30); //GOOD
      break;
    case 13:
      tft.setCursor(tft_width/4 + 15, 30); //GOOD
      break;
    default:
      tft.setCursor(tft_width/4 + 15, 30);
      break;
  }
  if(number != 4){ tft.println(title); }
  else
  {
    tft.setCursor(tft_width/4 + 63, 13);
    tft.println("Sunday");
    tft.setCursor(tft_width/4 + 54, 43);
    tft.println("Driving");      
  }
}

void printCommonScreenInfo(String title, uint8_t number)
{
  if(previousTitle != title || !previouslyon || previousdriveMode == 11 || previousdriveMode == 10 || previouslyStartActive != startActive || previouslybrakeScreen != brakeScreen)
  {
    printScreenTitle(title, number);
    previousTitle = title;
  }
  if(previousdriveMode != driveMode || !previouslyon || previousdriveMode == 11 || previousdriveMode == 10 || previouslyStartActive != startActive || previouslybrakeScreen != brakeScreen)
  {
    printScreenNumber();
    previousdriveMode = driveMode;
    updateScreenMaxTorque();
    previousmaxTorque = maxTorque;
  }
  if(previouslyStartActive != startActive){ previouslyStartActive = startActive; }
}

void printBrakeScreen()
{
  uint8_t brakePositionBuffer;
  if(!previouslybrakeScreen)
  {
    tft.fillRect(55,tft_height/2-40,370,100,HX8357_RED);
    tft.setTextColor(HX8357_BLACK);
    if(vehicleVoltage > 10)
    {
      tft.setTextSize(4);
      tft.setCursor(105, tft_height/2-30);
      tft.println("Press Brake");
      tft.drawRect(75,tft_height/2+5,330,50,HX8357_BLACK);
      tft.drawFastVLine(158,tft_height/2+5,50,HX8357_BLACK);
      tft.drawFastVLine(240,tft_height/2+5,50,HX8357_BLACK);
      tft.drawFastVLine(323,tft_height/2+5,50,HX8357_BLACK);
    }
    else
    {
      tft.setTextSize(6);
      tft.setCursor(145, tft_height/2-15);
      tft.println("HV OFF");
    }
    brakeScreen = true;
    previouslybrakeScreen = true;
  }
  if(previousbrakePosition != brakePosition && vehicleVoltage > 10)
  {
    brakePositionBuffer = brakePosition;
    if(brakePositionBuffer > 30){ brakePositionBuffer = 30; }
    if(brakePositionBuffer < previousbrakePosition && brakePosition < 30)
    {
      tft.fillRect(75,tft_height/2+5,previousbrakePosition*11,50,HX8357_RED);
      tft.drawRect(75,tft_height/2+5,330,50,HX8357_BLACK);
      tft.drawFastVLine(158,tft_height/2+5,50,HX8357_BLACK);
      tft.drawFastVLine(240,tft_height/2+5,50,HX8357_BLACK);
      tft.drawFastVLine(323,tft_height/2+5,50,HX8357_BLACK);
    }
    tft.fillRect(75,tft_height/2+5,brakePositionBuffer*11,50,HX8357_BLACK);
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

  uint16_t awColors[tft_width];  // hold colors for one row at a time...

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
