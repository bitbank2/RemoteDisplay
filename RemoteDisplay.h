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
#include <OneBitDisplay.h>
#include <bb_spi_lcd.h>
#endif
#include "rd_constants.h"
//
// RemoteDisplay
// Written by Larry Bank
// Copyright (c) 2020 BitBank Software, Inc.
// 
// Designed to allow remote or local displays and input
// for easy prototyping without owning the final hardware
//
//
// The base class which implements a few methods
// and leaves the rest as virtual to implement in each specific
// use case
//
class RemoteDisplay
{
  public:
    RemoteDisplay() {_bConnected = false;}
    ~RemoteDisplay() {}
    virtual int begin() {return 0;}
    virtual void shutdown() {}
    virtual void defineButtons(int *list, int count, int active)
    { if (count > MAX_BUTTONS) count = MAX_BUTTONS;
        memcpy(_buttons, list, sizeof(int)*count);
        _button_active = active;
        _button_count = count;
        for (int i=0; i<count; i++) {
            pinMode(_buttons[i], (active == LOW) ? INPUT_PULLUP : INPUT);
        }
    }
    virtual int fill(uint16_t u16Color);
    virtual int drawLine(int x1, int y1, int x2, int y2, uint16_t u16Color);
    virtual int drawPixel(int x, int y, uint16_t u16Color);
    virtual int setWindow(int x, int y, int w, int h);
    virtual int writePixels(uint16_t *pixels, int count, uint8_t bDMA);
    virtual int drawRect(int x, int y, int w, int h, uint16_t u16Color, int bFilled);
    virtual int drawText(int x, int y, char *szText, uint8_t u8Font, uint16_t u16FGColor, uint16_t u16BGColor);
    virtual int drawEllipse(int x, int y, int r1, int r2, uint16_t u16Color, int bFilled);
    virtual int setOrientation(int angle) {_orientation = angle; return RD_SUCCESS;};
    virtual int dumpBuffer(uint8_t * buffer);
    virtual int setFont(const GFXfont *pFont, int fontIndex);
    virtual int setBitmap(uint8_t bitmapIndex, const uint8_t *pBitmap, int iBitmapSize);
    virtual int drawBitmap(int x, int y, int bitmapIndex, int stretch);
    virtual int drawIcon(int x, int y, int iconIndex, int angle, uint16_t u16FGColor, uint16_t u16BGColor);
    int getWidth() {return _width;};
    int getHeight() {return _height;};
    int getBpp() {return _bpp;};
    int getOrientation() {return _orientation;};
    virtual uint16_t getButtons();
    
  protected:
    int _display_type;
    int _width;
    int _height;
    int _orientation;
    int _bpp;
    bool _bConnected;
    int _buttons[MAX_BUTTONS], _button_active, _button_count;
    uint16_t crc_16(uint8_t *data, size_t size);
    uint16_t _get_buttons(void);

}; // class RemoteDisplay

// Child classes of RemoteDisplay
#include "BLEDisplay.h"
#include "UARTDisplay.h"
#include "I2CDisplay.h"
#include "SPIDisplay.h"
#include "ESLDisplay.h"

#endif // __REMOTEDISPLAY__
