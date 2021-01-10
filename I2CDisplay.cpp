//
// RemoteDisplay
//
// written by Larry Bank
// bitbank@pobox.com
// Project started 12/2/2020
// Original JPEG code written 26+ years ago :)
// The goal of this code is to decode baseline JPEG images
// using no more than 18K of RAM (if sent directly to an LCD display)
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
#include "RemoteDisplay.h"
//
// I2C Display implementation
//
int I2CDisplay::begin(uint16_t u16DisplayType, int SDAPin, int SCLPin, int bBitBang, uint32_t u32Speed)
{
int rc, iDisp;

   iDisp = u16DisplayType - RD_OLED_128x128 + 1;
   rc = obdI2CInit(&_obd, iDisp, -1, 0, 0, (bBitBang == 0), SDAPin, SCLPin, -1, u32Speed); 
   if (rc >= 0) {
      _display_type = u16DisplayType;
      obdSetBackBuffer(&_obd, _buffer);
      return RD_SUCCESS;
   } else {
      return RD_INIT_FAILED;
   }
} /* I2CDisplay::begin() */

int I2CDisplay::dumpBuffer(uint8_t * buffer)
{
    obdDumpBuffer(&_obd, buffer);
    return RD_SUCCESS;
} /* I2CDisplay::dumpBuffer() */
void I2CDisplay::shutdown()
{
    obdPower(&_obd, 0);
} /* I2CDisplay::shutdown() */

int I2CDisplay::setFont(const GFXfont *pFont, int fontIndex)
{
    if (fontIndex >= 0 && fontIndex < MAX_FONT_INDEX) {
        _fonts[fontIndex] = (GFXfont *)pFont;
        return RD_SUCCESS;
    }
    return RD_INVALID_PARAMETER;
} /* setFont() */

int I2CDisplay::setBitmap(uint8_t bitmapIndex, const uint8_t *pBitmap, int iBitmapSize)
{
    if (bitmapIndex >= MAX_BITMAP_INDEX)
        return RD_INVALID_PARAMETER;
    _bitmaps[bitmapIndex] = (uint8_t *)pBitmap;
    return 0;
} /* setBitmap() */

int I2CDisplay::drawBitmap(int x, int y, int bitmapIndex, int stretch)
{
    (void)stretch; // not implemented for OLED displays
    if (bitmapIndex < 0 || bitmapIndex >= MAX_BITMAP_INDEX || _bitmaps[bitmapIndex] == NULL)
        return RD_INVALID_PARAMETER;
    obdLoadBMP(&_obd, _bitmaps[bitmapIndex], x, y, 0);
    return RD_SUCCESS;
} /* drawBitmap() */

int I2CDisplay::drawIcon(int x, int y, int iconIndex, int angle, uint16_t u16FGColor, uint16_t u16BGColor)
{
    if (iconIndex < 0 || iconIndex >= MAX_ICON_INDEX)
        return RD_INVALID_PARAMETER;
    return RD_SUCCESS;
} /* drawIcon() */

int I2CDisplay::fill(uint16_t u16Color)
{
    obdFill(&_obd, (u16Color > 0) ? 0xff:0x00, 1);
    return RD_SUCCESS;
} /* I2CDisplay::fill() */

int I2CDisplay::drawLine(int x1, int y1, int x2, int y2, uint16_t u16Color)
{
    obdDrawLine(&_obd, x1, y1, x2, y2, (uint8_t)(u16Color > 0), 1);
    return RD_SUCCESS;
} /* I2CDisplay::drawLine() */

int I2CDisplay::drawPixel(int x, int y, uint16_t u16Color)
{
    obdSetPixel(&_obd, x, y, (uint8_t)(u16Color > 0), 1);
    return RD_SUCCESS;
} /* I2CDisplay::drawPixel() */

int I2CDisplay::drawText(int x, int y, char *szText, uint8_t u8Font, uint16_t u16FGColor, uint16_t u16BGColor)
{
    obdWriteString(&_obd, 0, x, y/8, szText, u8Font, 0, 1);
    return RD_SUCCESS;
} /* I2CDisplay::drawText() */

int I2CDisplay::drawEllipse(int x, int y, int r1, int r2, uint16_t u16Color, int bFilled)
{
    obdEllipse(&_obd, x, y, r1, r2, (uint8_t)(u16Color > 0), bFilled);
    obdDumpBuffer(&_obd, NULL); // show it
    return RD_SUCCESS;
} /* I2CDisplay::drawEllipse() */

int I2CDisplay::drawRect(int x, int y, int w, int h, uint16_t u16Color, int bFilled)
{
    obdRectangle(&_obd, x, y, x+w-1, y+h-1, u16Color, bFilled);
} /* drawRect() */

int I2CDisplay::setWindow(int x, int y, int w, int h)
{
    _x = x;
    _y = y;
    _w = w;
    _h = h;
    return RD_SUCCESS;
} /* I2CDisplay::setWindow() */

int I2CDisplay::writePixels(uint16_t *pixels, int count, uint8_t bDMA)
{
    (void)pixels;
    (void)count;
    (void)bDMA;
    // DEBUG - missing implementation
    return RD_SUCCESS;
} /* I2CDisplay::writePixels() */

int I2CDisplay::setOrientation(int angle)
{
    // DEBUG
    int i = LCD_ORIENTATION_0;
    if (angle != 0 && angle != 90 && angle != 180 && angle != 270)
        return RD_INVALID_PARAMETER;
    if (angle == 0)
        i = LCD_ORIENTATION_0;
//    else if (angle == 90)
//        i = LCD_ORIENTATION_90;
    else if (angle == 180)
        i = LCD_ORIENTATION_180;
//    else i = LCD_ORIENTATION_270;
//    spilcdSetOrientation(&_lcd, i);
    _orientation = angle;
    return RD_SUCCESS;
} /* I2CDisplay::setOrientation() */

uint16_t I2CDisplay::getButtons()
{
    return _get_buttons();
} /* I2CDisplay::getButtons() */

