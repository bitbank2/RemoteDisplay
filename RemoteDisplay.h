//
// Copyright 2020 BitBank Software, Inc. All Rights Reserved.
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//    http://www.apache.org/licenses/LICENSE-2.0
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//===========================================================================
//
#ifndef __REMOTEDISPLAY__
#define __REMOTEDISPLAY__
#if defined( __MACH__ ) || defined( __LINUX__ ) || defined( __MCUXPRESSO )
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#define memcpy_P memcpy
#define PROGMEM
#else
#include <Arduino.h>
#endif
//
// RemoteDisplay
// Written by Larry Bank
// Copyright (c) 2020 BitBank Software, Inc.
// 
// Designed to allow remote or local displays and input
// for easy prototyping without owning the final hardware
//

/* Defines and variables */
enum {
   RD_NOP=0,
   RD_INIT,
   RD_DRAW_TEXT,
   RD_DRAW_LINE,
   RD_DRAW_ELLIPSE,
   RD_DRAW_ICON,
   RD_DRAW_BITMAP,
   RD_DRAW_PIXEL,
   RD_WRITE_PIXELS,
   RD_FILL,
   RD_DRAW_RECT,
   RD_SET_WINDOW,
   RD_SET_ORIENTATION,
   RD_GET_INFO,
   RD_GET_BUTTONS,
   RD_BACKLIGHT,
   RD_ADD_ASSET,
   RD_API_COUNT
};

// Supported display types
enum {
    RD_LCD_INVALID=0,
    // Color LCDs/OLEDs
    RD_LCD_ILI9341, // 240x320
    RD_LCD_ILI9225, // 176x220
    RD_LCD_HX8357, // 320x480
    RD_LCD_ST7735R, // 128x160
    RD_LCD_ST7735S, // 80x160 with offset of 24,0
    RD_LCD_ST7735S_B, // 80x160 with offset of 26,2
    RD_LCD_SSD1331,
    RD_LCD_SSD1351,
    RD_LCD_ILI9342, // 320x240 IPS
    RD_LCD_ST7789, // 240x320
    RD_LCD_ST7789_240,  // 240x240
    RD_LCD_ST7789_135, // 135x240
    RD_LCD_ST7789_NOCS, // 240x240 without CS, vertical offset of 80, MODE3
    RD_LCD_SSD1283A, // 132x132
    RD_LCD_ILI9486, // 320x480
    // Monochrome LCDs/OLEDs
    RD_OLED_128x128,
    RD_OLED_128x32,
    RD_OLED_128x64,
    RD_OLED_132x64,
    RD_OLED_64x32,
    RD_OLED_96x16,
    RD_OLED_72x40,
    RD_LCD_UC1701,
    RD_LCD_UC1609,
    RD_LCD_HX1230,
    RD_LCD_NOKIA5110,
    RD_SHARP_144x168,
    RD_SHARP_400x240,
    RD_LCD_COUNT
};

// Error codes returned by getLastError()
enum {
    RD_SUCCESS = 0,
    RD_INVALID_PARAMETER,
    RD_INIT_FAILED,
    RD_BUSY,
    RD_NOT_CONNECTED
};

// Built-in font sizes
enum {
   FONT_6x8 = 0,
   FONT_8x8,
   FONT_12x16,
   FONT_16x16,
   FONT_16x32,
   FONT_COUNT
};

//
// The JPEGDEC class wraps portable C code which does the actual work
//
class RemoteDisplay
{
  public:
    RemoteDisplay() {_bConnected = false;}
    ~RemoteDisplay() {}
    virtual int begin() {return 0;}
    virtual void shutdown() {}
    virtual int fill(uint16_t u16Color) {return 0;};
    virtual int drawLine(int x1, int y1, int x2, int y2, uint16_t u16Color) {return 0;};
    virtual int drawPixel(int x, int y, uint16_t u16Color) {return 0;};
    virtual int setWindow(int x, int y, int w, int h) {return 0;};
    virtual int writePixels(void *pixels, int count, uint8_t bDMA) {return 0;};
    virtual int drawRect(int x, int y, int w, int h, uint16_t u16Color, int bFilled) {return 0;};
    virtual int drawText(int x, int y, char *szText, uint8_t u8Font, uint16_t u16FGColor, uint16_t u16BGColor) {return 0;};
    virtual int drawEllipse(int x, int y, int r1, int r2, uint16_t u16Color, int bFilled) {return 0;};
    virtual int setOrientation(int angle) {_orientation = angle; return RD_SUCCESS;};
    int getWidth() {return _width;};
    int getHeight() {return _height;};
    int getBpp() {return _bpp;};
    int getOrientation() {return _orientation;};
    
  protected:
    int _width;
    int _height;
    int _orientation;
    int _bpp;
    bool _bConnected;
    uint16_t crc_16(uint8_t *data, size_t size);
}; // class RemoteDisplay

class BLEDisplay : public RemoteDisplay
{
public:
    BLEDisplay() : RemoteDisplay() {}
    ~BLEDisplay() {}
    int begin(uint16_t display_type);
    int fill(uint16_t u16Color);
    int drawLine(int x1, int y1, int x2, int y2, uint16_t u16Color);
    int drawPixel(int x, int y, uint16_t u16Color);
    int setWindow(int x, int y, int w, int h);
    int writePixels(void *pixels, int count, uint8_t bDMA);
    int drawRect(int x, int y, int w, int h, uint16_t u16Color, int bFilled);
    int drawText(int x, int y, char *szText, uint8_t u8Font, uint16_t u16FGColor, uint16_t u16BGColor);
    int drawEllipse(int x, int y, int r1, int r2, uint16_t u16Color, int bFilled);
    int setOrientation(int angle);
private:
    int BLESendVarData(uint16_t *data, int count, void *varData);
    int BLESend(uint16_t *data, int count);
}; // class BLEDisplay

class UARTDisplay : public RemoteDisplay
{
public:
    UARTDisplay() : RemoteDisplay() {}
    ~UARTDisplay() {}
    int begin(uint32_t u32Capabilities, uint16_t width, uint16_t height, uint32_t u32Speed);

}; // class UARTDisplay

class I2CDisplay : public RemoteDisplay
{
public:
    I2CDisplay() : RemoteDisplay() {}
    ~I2CDisplay() {}
    int begin(uint32_t u32Capabilities, uint16_t width, uint16_t height, uint8_t SDAPin, uint8_t SCLPin, uint8_t bBitBang, uint32_t u32Speed);

}; // class I2CDisplay

class SPIDisplay : public RemoteDisplay
{
public:
    SPIDisplay() : RemoteDisplay() {}
    ~SPIDisplay() {}
    int begin(uint32_t u32Capabilities, uint16_t width, uint16_t height, uint8_t CS_Pin, uint8_t DC_Pin, uint8_t RESET_Pin, uint8_t LED_Pin, uint32_t u32Speed);

}; // class SPIDisplay

#endif // __REMOTEDISPLAY__
