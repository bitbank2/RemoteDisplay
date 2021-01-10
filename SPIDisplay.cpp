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
int SPIDisplay::begin(uint16_t u16DisplayType, uint16_t u16Flags, uint32_t u32Speed, int CS_Pin, int DC_Pin, int RESET_Pin, int LED_Pin)
{
    _display_type = u16DisplayType;
    memset(&_lcd, 0, sizeof(_lcd));
    // For now, assume SPI displays will be color LCD/OLEDs
    if (spilcdInit(&_lcd, u16DisplayType, u16Flags, u32Speed, CS_Pin, DC_Pin, RESET_Pin, LED_Pin, -1, -1, -1))
        return RD_INIT_FAILED;
    return RD_SUCCESS;
} /* SPIDisplay:begin() */

int SPIDisplay::setFont(const GFXfont *pFont, int fontIndex)
{
    if (fontIndex >= 0 && fontIndex < MAX_FONT_INDEX) {
        _fonts[fontIndex] = (GFXfont *)pFont;
        return RD_SUCCESS;
    }
    return RD_INVALID_PARAMETER;
} /* setFont() */

int SPIDisplay::setBitmap(uint8_t bitmapIndex, const uint8_t *pBitmap, int iBitmapSize)
{
    if (bitmapIndex >= MAX_BITMAP_INDEX)
        return RD_INVALID_PARAMETER;
    _bitmaps[bitmapIndex] = (uint8_t *)pBitmap;
    return 0;
} /* setBitmap() */
int SPIDisplay::dumpBuffer(uint8_t * buffer)
{
    // not implemented yet
    (void)buffer;
    return RD_SUCCESS;
} /* SPIDisplay::dumpBuffer() */

void SPIDisplay::shutdown()
{
    spilcdShutdown(&_lcd);
} /* SPIDisplay::shutdown() */

int SPIDisplay::fill(uint16_t u16Color)
{
    spilcdFill(&_lcd, u16Color, DRAW_TO_LCD);
    return RD_SUCCESS;
} /* SPIDisplay::fill() */

int SPIDisplay::drawLine(int x1, int y1, int x2, int y2, uint16_t u16Color)
{
    spilcdDrawLine(&_lcd, x1, y1, x2, y2, u16Color, DRAW_TO_LCD);
    return RD_SUCCESS;
} /* SPIDisplay::drawLine() */

int SPIDisplay::drawPixel(int x, int y, uint16_t u16Color)
{
    spilcdSetPixel(&_lcd, x, y, u16Color, DRAW_TO_LCD);
    return RD_SUCCESS;
} /* SPIDisplay::drawPixel() */

int SPIDisplay::drawBitmap(int x, int y, int bitmapIndex, int stretch)
{
    if (bitmapIndex < 0 || bitmapIndex >= MAX_BITMAP_INDEX || _bitmaps[bitmapIndex] == NULL)
        return RD_INVALID_PARAMETER;
    spilcdDrawBMP(&_lcd, _bitmaps[bitmapIndex], x, y, stretch, 0, DRAW_TO_LCD);
    return RD_SUCCESS;
} /* drawBitmap() */

int SPIDisplay::setWindow(int x, int y, int w, int h)
{
    spilcdSetPosition(&_lcd, x, y, w, h, DRAW_TO_LCD);
    return RD_SUCCESS;
} /* SPIDisplay::setWindow() */

int SPIDisplay::writePixels(uint16_t *pixels, int count, uint8_t bDMA)
{
    spilcdWriteDataBlock(&_lcd, (uint8_t *)pixels, count*2, (bDMA)? (DRAW_TO_LCD | DRAW_WITH_DMA) : DRAW_TO_LCD);
    return RD_SUCCESS;
} /* SPIDisplay::writePixels() */

int SPIDisplay::drawRect(int x, int y, int w, int h, uint16_t u16Color, int bFilled)
{
    spilcdRectangle(&_lcd, x, y, w, h, u16Color, u16Color, bFilled, DRAW_TO_LCD);
    return RD_SUCCESS;
} /* SPIDisplay::drawRect() */

int SPIDisplay::drawText(int x, int y, char *szText, uint8_t u8Font, uint16_t u16FGColor, uint16_t u16BGColor)
{
    if (u8Font < RD_FONT_CUSTOM_0)
        spilcdWriteString(&_lcd, x, y, szText, u16FGColor, u16BGColor, u8Font, DRAW_TO_LCD);
    else // draw custom font
        spilcdWriteStringCustom(&_lcd, _fonts[u8Font - RD_FONT_CUSTOM_0], x, y, szText, u16FGColor, u16BGColor, 1, DRAW_TO_LCD);
    return RD_SUCCESS;
} /* SPIDisplay::drawText() */

int SPIDisplay::drawEllipse(int x, int y, int r1, int r2, uint16_t u16Color, int bFilled)
{
    spilcdEllipse(&_lcd, x, y, r1, r2, u16Color, bFilled, DRAW_TO_LCD);
    return RD_SUCCESS;
} /* SPIDisplay::drawEllipse() */

int SPIDisplay::setOrientation(int angle)
{
    int i = LCD_ORIENTATION_0;
    if (angle != 0 && angle != 90 && angle != 180 && angle != 270)
        return RD_INVALID_PARAMETER;
    if (angle == 0)
        i = LCD_ORIENTATION_0;
    else if (angle == 90)
        i = LCD_ORIENTATION_90;
    else if (angle == 180)
        i = LCD_ORIENTATION_180;
    else i = LCD_ORIENTATION_270;
    spilcdSetOrientation(&_lcd, i);
    _orientation = angle;
    return RD_SUCCESS;
} /* SPIDisplay::setOrientation() */

uint16_t SPIDisplay::getButtons()
{
    return _get_buttons();
} /* SPIDisplay::getButtons()*/

