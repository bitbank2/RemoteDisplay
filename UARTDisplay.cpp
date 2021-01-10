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
// UART implementation
//
int UARTDisplay::UARTSendVarData(uint16_t *data, int count, void *varData)
{
    uint8_t ucTemp[512];
    int iSize = count*2 + data[1];
    memcpy(ucTemp, data, count*sizeof(uint16_t)); // non-payload part
    memcpy(&ucTemp[count*sizeof(uint16_t)], varData, data[1]); // var payload
    // Serial needs to know the length first since it buffers the read
    Serial.write(0xff); // start with a couple of sync bytes
    Serial.write(0xff);
    Serial.write((uint8_t)iSize); // low byte of length
    Serial.write((uint8_t)(iSize >> 8)); // high byte
    Serial.write(ucTemp, iSize);
    Serial.flush(); // wait for transmission to complete
    return RD_SUCCESS;
} /* UARTSendVarData() */

int UARTDisplay::UARTSend(uint16_t *data, int count)
{
    data[count] = crc_16((uint8_t *)data, count * sizeof(uint16_t));
    // Serial needs to know the length first since it buffers the read
    Serial.write(0xff); // start with a couple of sync bytes
    Serial.write(0xff);
    Serial.write(count * 2); // low byte of length
    Serial.write((uint8_t)0); // high byte
    Serial.write((uint8_t *)data, (count+1)*sizeof(uint16_t));
    Serial.flush(); // wait for transmission to complete
    return RD_SUCCESS;
} /* BLESend() */

int UARTDisplay::begin(uint16_t u16DisplayType)
{
    uint16_t u16Tmp[4];

    _display_type = u16DisplayType;
    Serial.begin(115200); // this baud rate should work for all situations
    while (!Serial) {delay(10);}; // wait until fully initialized
    // Send the remote server the type of display we want
    u16Tmp[0] = RD_INIT;
    u16Tmp[1] = u16DisplayType;
    return UARTSend(u16Tmp, 2);
} /* UARTDisplay::begin() */

void UARTDisplay::shutdown()
{
} /* UARTDisplay::shutdown() */

int UARTDisplay::fill(uint16_t u16Color)
{
    uint16_t u16Tmp[4];
    u16Tmp[0] = RD_FILL;
    u16Tmp[1] = u16Color;
    return UARTSend(u16Tmp, 2); // send to the remote server
}
int UARTDisplay::drawLine(int x1, int y1, int x2, int y2, uint16_t u16Color)
{
    uint16_t u16Tmp[8];
    u16Tmp[0] = RD_DRAW_LINE;
    u16Tmp[1] = (uint16_t)x1;
    u16Tmp[2] = (uint16_t)y1;
    u16Tmp[3] = (uint16_t)x2;
    u16Tmp[4] = (uint16_t)y2;
    u16Tmp[5] = u16Color;
    return UARTSend(u16Tmp, 6); // send to the remote server
} /* UARTDisplay::drawLine() */

int UARTDisplay::drawPixel(int x, int y, uint16_t u16Color)
{
    uint16_t u16Tmp[8];
    u16Tmp[0] = RD_DRAW_PIXEL;
    u16Tmp[1] = (uint16_t)x;
    u16Tmp[2] = (uint16_t)y;
    u16Tmp[3] = u16Color;
    return UARTSend(u16Tmp, 4); // send to the remote server
} /* UARTDisplay::drawPixel() */

int UARTDisplay::setWindow(int x, int y, int w, int h)
{
    uint16_t u16Tmp[8];
    u16Tmp[0] = RD_SET_WINDOW;
    u16Tmp[1] = (uint16_t)x;
    u16Tmp[2] = (uint16_t)y;
    u16Tmp[3] = (uint16_t)w;
    u16Tmp[4] = (uint16_t)h;
    return UARTSend(u16Tmp, 5); // send to the remote server
} /* UARTDisplay::setWindow() */

int UARTDisplay::writePixels(uint16_t *pixels, int count, uint8_t bDMA)
{
    uint16_t u16Tmp[8];
    u16Tmp[0] = RD_WRITE_PIXELS;
    u16Tmp[1] = (uint16_t)count*2; // payload size in bytes
    u16Tmp[2] = (uint16_t)bDMA;
    return UARTSendVarData(u16Tmp, 3, pixels); // send to the remote server
} /* UARTDisplay::writePixels() */

int UARTDisplay::dumpBuffer(uint8_t * buffer)
{
    if (_bpp == 1) {
        uint16_t u16Tmp[4];
        int iCount = (_width * _height) / 8;
        u16Tmp[0] = RD_DUMP_BUFFER;
        u16Tmp[1] = iCount/2; // in terms of uint16_t's
        return UARTSendVarData(u16Tmp, 2, (void *)buffer);
    } else {
        return RD_NOT_SUPPORTED;
    }
} /* UARTDisplay::dumpBuffer() */

int UARTDisplay::drawRect(int x, int y, int w, int h, uint16_t u16Color, int bFilled)
{
    uint16_t u16Tmp[8];
    u16Tmp[0] = RD_DRAW_RECT;
    u16Tmp[1] = (uint16_t)x;
    u16Tmp[2] = (uint16_t)y;
    u16Tmp[3] = (uint16_t)w;
    u16Tmp[4] = (uint16_t)h;
    u16Tmp[5] = u16Color;
    u16Tmp[6] = (uint16_t)bFilled;
    return UARTSend(u16Tmp, 7); // send to the remote server
} /* UARTDisplay::drawRect() */

int UARTDisplay::drawText(int x, int y, char *szText, uint8_t u8Font, uint16_t u16FGColor, uint16_t u16BGColor)
{
     uint16_t u16Tmp[8];
     u16Tmp[0] = RD_DRAW_TEXT;
     u16Tmp[1] = strlen(szText); // payload size in bytes
     u16Tmp[2] = (uint16_t)x;
     u16Tmp[3] = (uint16_t)y;
     u16Tmp[4] = (uint16_t)u8Font;
     u16Tmp[5] = u16FGColor;
     u16Tmp[6] = u16BGColor;
     return UARTSendVarData(u16Tmp, 7, szText); // send to the remote server
 } /* UARTDisplay::drawText() */

int UARTDisplay::drawEllipse(int x, int y, int r1, int r2, uint16_t u16Color, int bFilled)
{
    uint16_t u16Tmp[8];
    u16Tmp[0] = RD_DRAW_ELLIPSE;
    u16Tmp[1] = (uint16_t)x;
    u16Tmp[2] = (uint16_t)y;
    u16Tmp[3] = (uint16_t)r1;
    u16Tmp[4] = (uint16_t)r2;
    u16Tmp[5] = u16Color;
    u16Tmp[6] = bFilled;
    return UARTSend(u16Tmp, 7); // send to the remote server
} /* UARTDisplay::drawEllipse() */

int UARTDisplay::setOrientation(int angle)
{
    int rc;
    uint16_t u16Tmp[4];
    u16Tmp[0] = RD_SET_ORIENTATION;
    u16Tmp[1] = (uint16_t)angle;
    rc = UARTSend(u16Tmp, 2); // send to the remote server
    if (rc == RD_SUCCESS)
       _orientation = angle;
    return rc;
} /* UARTDisplay::setOrientation() */

uint16_t UARTDisplay::getButtons()
{
    // Need to explicitly ask for the buttons over serial
    uint16_t u16Tmp[4];
    uint8_t buttons = 0;
    u16Tmp[0] = RD_GET_BUTTONS;
    UARTSend(u16Tmp, 1); // send to the remote server
    buttons = Serial.read(); // get 1 byte returned
    return buttons;

} /* UARTDisplay::getButtons() */

