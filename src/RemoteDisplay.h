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
   RD_DRAW_TEXT,
   RD_DRAW_LINE,
   RD_DRAW_ELLIPSE,
   RD_DRAW_ICON,
   RD_DRAW_BITMAP,
   RD_DRAW_PIXEL,
   RD_FILL,
   RD_DRAW_RECT,
   RD_SET_CURSOR,
   RD_GET_INFO,
   RD_GET_BUTTONS,
   RD_BACKLIGHT,
   RD_API_COUNT
};

// Display capabilities bits
// a virtual display can implement 1 or more types of displays
#define DISPLAY_TYPE_OLED_1BPP   0x00000001
#define DISPLAY_TYPE_OLED_4BPP   0x00000002
#define DISPLAY_TYPE_OLED_16BPP  0x00000004
#define DISPLAY_TYPE_LCD_1BPP    0x00000008
#define DISPLAY_TYPE_LCD_16BPP   0x00000010

// Display chipset types
#define DISPLAY_CTRL_NONE        0
#define DISPLAY_CTRL_SSD1306     0x00010000
#define DISPLAY_CTRL_SH1106      0x00020000
#define DISPLAY_CTRL_SH1107      0x00040000
#define DISPLAY_CTRL_UC1701      0x00080000
#define DISPLAY_CTRL_HX1230      0x00100000
#define DISPLAY_CTRL_ST7735      0x00200000
#define DISPLAY_CTRL_ST7789      0x00400000
#define DISPLAY_CTRL_ILI9341     0x00800000
#define DISPLAY_CTRL_ILI9342     0x01000000
#define DISPLAY_CTRL_ILI9486     0x02000000
#define DISPLAY_CTRL_ILI9225     0x04000000
#define DISPLAY_CTRL_HX8357      0x08000000
#define DISPLAY_CTRL_SSD1331     0x10000000
#define DISPLAY_CTRL_SSD1351     0x20000000
#define DISPLAY_CTRL_SSD1283A    0x40000000

// Error codes returned by getLastError()
enum {
    RD_SUCCESS = 0,
    RD_INVALID_PARAMETER,
    RD_INIT_FAILED,
    RD_BUSY,
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
    RemoteDisplay() {}
    ~RemoteDisplay() {}
    virtual int begin() {}
    virtual void shutdown()
    virtual int drawLine(int x1, int y1, int x2, int y2, uint16_t u16Color);
    virtual int drawPixel(int x, int y, uint16_t u16Color);
    virtual int setWindow(int x, int y, int w, int h);
    virtual int writePixels(void *pixels, int count, uint8_t bDMA);
    virtual int drawRect(int x1, int y1, int x2, int y2, uint16_t u16Color);
    virtual int drawFilledRect(int x1, int y1, int x2, int y2, uint16_t u16Color);
    virtual int drawText(int x, int y, char *szText, uint8_t u8Font, uint16_t u16FGColor, uint16_t u16BGColor);
    virtual int drawEllipse(int x, int y, int r1, int r2, uint16_t u16Color);
    virtual int drawFilledEllipse(int x, int y, int r1, int r2, uint16_t u16Color);
    virtual void setOrientation(int angle);
    int getWidth() {return width;};
    int getHeight() {return height;};
    int getBpp() {return bpp;};
    int getOrientation() {return orientation;};
    
  private:
    int width, height;
    int orientation;
    int bpp;
    uint16_t __crc_16(uint8_t *data, size_t size);
}; // class RemoteDisplay

class BLEDisplay : public RemoteDisplay
{
public:
    BLEDisplay() : RemoteDisplay() {}
    ~BLEDisplay() {}
    int begin(uint32_t u32Capabilities, uint16_t width, uint16_t height);

}; // class BLEDisplay

class UARTDisplay : public RemoteDisplay
{
public:
    UARTDisplay() : RemoteDisplay() {}
    ~UARTDisplay() {}
    int begin(uint32_t u32Capabilities, uint16_t width, uint16_t height, uint8_t uint32_t u32Speed);

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
