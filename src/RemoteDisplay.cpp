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

int BLEDisplay::begin(uint32_t u32Capabilities, uint16_t width, uint16_t height)
{
    return RD_SUCCESS;
} /* begin() */

int UARTDisplay::begin(uint32_t u32Capabilities, uint16_t width, uint16_t height, uint8_t uint32_t u32Speed)
{
    return RD_SUCCESS;
} /* begin() */

int I2CDisplay::begin(uint32_t u32Capabilities, uint16_t width, uint16_t height, uint8_t SDAPin, uint8_t SCLPin, uint8_t bBitBang, uint32_t u32Speed)
{
    return RD_SUCCESS;
} /* begin() */

int SPIDisplay::begin(uint32_t u32Capabilities, uint16_t width, uint16_t height, uint8_t CS_Pin, uint8_t DC_Pin, uint8_t RESET_Pin, uint8_t LED_Pin, uint32_t u32Speed)
{
    return RD_SUCCESS;
} /* begin() */
