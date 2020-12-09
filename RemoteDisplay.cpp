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
#ifdef HAL_ESP32_HAL_H_
// Bluetooth support
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
static BLEUUID serviceUUID("0000fea0-0000-1000-8000-00805f9b34fb"); //Service
static BLEUUID dataUUID("0000fea1-0000-1000-8000-00805f9b34fb"); // data characteristic
static BLEUUID nameUUID("0000fea2-0000-1000-8000-00805f9b34fb"); // name characteristic
std::string VD_BLE_Name = "RemoteDisplay";
char Scanned_BLE_Name[32];
String Scanned_BLE_Address;
BLEScanResults foundDevices;
static BLEAddress *Server_BLE_Address;
volatile boolean paired = false; //boolean variable to togge light
BLEServer *pServer;
BLEScan *pBLEScan;
BLEService *pService;
BLERemoteCharacteristic *pCharacteristicData;
#endif // HAL_ESP32_HAL_H_

// Bluetooth support for Nano 33 BLE
#ifdef ARDUINO_ARDUINO_NANO33BLE
#include <ArduinoBLE.h>

static BLEDevice peripheral;
static BLEService prtService;
static BLECharacteristic pCharacteristicData;
#endif // Nano 33 BLE

static const uint16_t __crc_16_table[256] PROGMEM =
{
    0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50A5, 0x60C6, 0x70E7,
    0x8108, 0x9129, 0xA14A, 0xB16B, 0xC18C, 0xD1AD, 0xE1CE, 0xF1EF,
    0x1231, 0x0210, 0x3273, 0x2252, 0x52B5, 0x4294, 0x72F7, 0x62D6,
    0x9339, 0x8318, 0xB37B, 0xA35A, 0xD3BD, 0xC39C, 0xF3FF, 0xE3DE,
    0x2462, 0x3443, 0x0420, 0x1401, 0x64E6, 0x74C7, 0x44A4, 0x5485,
    0xA56A, 0xB54B, 0x8528, 0x9509, 0xE5EE, 0xF5CF, 0xC5AC, 0xD58D,
    0x3653, 0x2672, 0x1611, 0x0630, 0x76D7, 0x66F6, 0x5695, 0x46B4,
    0xB75B, 0xA77A, 0x9719, 0x8738, 0xF7DF, 0xE7FE, 0xD79D, 0xC7BC,
    0x48C4, 0x58E5, 0x6886, 0x78A7, 0x0840, 0x1861, 0x2802, 0x3823,
    0xC9CC, 0xD9ED, 0xE98E, 0xF9AF, 0x8948, 0x9969, 0xA90A, 0xB92B,
    0x5AF5, 0x4AD4, 0x7AB7, 0x6A96, 0x1A71, 0x0A50, 0x3A33, 0x2A12,
    0xDBFD, 0xCBDC, 0xFBBF, 0xEB9E, 0x9B79, 0x8B58, 0xBB3B, 0xAB1A,
    0x6CA6, 0x7C87, 0x4CE4, 0x5CC5, 0x2C22, 0x3C03, 0x0C60, 0x1C41,
    0xEDAE, 0xFD8F, 0xCDEC, 0xDDCD, 0xAD2A, 0xBD0B, 0x8D68, 0x9D49,
    0x7E97, 0x6EB6, 0x5ED5, 0x4EF4, 0x3E13, 0x2E32, 0x1E51, 0x0E70,
    0xFF9F, 0xEFBE, 0xDFDD, 0xCFFC, 0xBF1B, 0xAF3A, 0x9F59, 0x8F78,
    0x9188, 0x81A9, 0xB1CA, 0xA1EB, 0xD10C, 0xC12D, 0xF14E, 0xE16F,
    0x1080, 0x00A1, 0x30C2, 0x20E3, 0x5004, 0x4025, 0x7046, 0x6067,
    0x83B9, 0x9398, 0xA3FB, 0xB3DA, 0xC33D, 0xD31C, 0xE37F, 0xF35E,
    0x02B1, 0x1290, 0x22F3, 0x32D2, 0x4235, 0x5214, 0x6277, 0x7256,
    0xB5EA, 0xA5CB, 0x95A8, 0x8589, 0xF56E, 0xE54F, 0xD52C, 0xC50D,
    0x34E2, 0x24C3, 0x14A0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405,
    0xA7DB, 0xB7FA, 0x8799, 0x97B8, 0xE75F, 0xF77E, 0xC71D, 0xD73C,
    0x26D3, 0x36F2, 0x0691, 0x16B0, 0x6657, 0x7676, 0x4615, 0x5634,
    0xD94C, 0xC96D, 0xF90E, 0xE92F, 0x99C8, 0x89E9, 0xB98A, 0xA9AB,
    0x5844, 0x4865, 0x7806, 0x6827, 0x18C0, 0x08E1, 0x3882, 0x28A3,
    0xCB7D, 0xDB5C, 0xEB3F, 0xFB1E, 0x8BF9, 0x9BD8, 0xABBB, 0xBB9A,
    0x4A75, 0x5A54, 0x6A37, 0x7A16, 0x0AF1, 0x1AD0, 0x2AB3, 0x3A92,
    0xFD2E, 0xED0F, 0xDD6C, 0xCD4D, 0xBDAA, 0xAD8B, 0x9DE8, 0x8DC9,
    0x7C26, 0x6C07, 0x5C64, 0x4C45, 0x3CA2, 0x2C83, 0x1CE0, 0x0CC1,
    0xEF1F, 0xFF3E, 0xCF5D, 0xDF7C, 0xAF9B, 0xBFBA, 0x8FD9, 0x9FF8,
    0x6E17, 0x7E36, 0x4E55, 0x5E74, 0x2E93, 0x3EB2, 0x0ED1, 0x1EF0
};

uint16_t RemoteDisplay::crc_16(uint8_t *data, size_t size)
{
    uint16_t crc = 0xFFFF;

    for (size_t i = 0; i < size; i++) crc = pgm_read_word_near(__crc_16_table + (((crc >> 8) ^ data[i]) & 0xff)) ^ (crc << 8);

    return crc;
} /* crc_16() */

#ifdef HAL_ESP32_HAL_H_
// Called for each device found during a BLE scan by the client
class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks
{
    void onResult(BLEAdvertisedDevice advertisedDevice) {
      Serial.printf("Scan Result: %s \n", advertisedDevice.toString().c_str());
      if (Scanned_BLE_Name[0] == 0 && strcmp(VD_BLE_Name.c_str(), advertisedDevice.getName().c_str()) == 0) { // this is what we want
        Server_BLE_Address = new BLEAddress(advertisedDevice.getAddress());
        Scanned_BLE_Address = Server_BLE_Address->toString().c_str();
        strcpy(Scanned_BLE_Name, advertisedDevice.getName().c_str());
        Serial.printf("Found what we're looking for!\n");
        pBLEScan->stop(); // stop scanning
      }
    }
};

// When the scan has found the BLE server device name we're looking for, we try to connect
bool connectToserver (BLEAddress pAddress)
{
    BLEClient*  pClient  = BLEDevice::createClient();
    Serial.println(" - Created client");

    // Connect to the BLE Server.
    pClient->connect(pAddress, BLE_ADDR_TYPE_RANDOM); // needed for iOS/Android/MacOS
    Serial.print(" - Connected to "); Serial.println(VD_BLE_Name.c_str());
    // Obtain a reference to the service we are after in the remote BLE server.
    BLERemoteService* pRemoteService = pClient->getService(serviceUUID);
    Serial.println("Returned from getService()");
    if (pRemoteService != NULL)
    {
      Serial.println(" - Found our service");
      if (pClient->isConnected())
      {
        Serial.println(" - We're connected");
    // Obtain a reference to the characteristic in the service of the remote BLE server.
        pCharacteristicData = pRemoteService->getCharacteristic(dataUUID);
        if (pCharacteristicData != nullptr)
        {
          Serial.println(" - Found our data characteristic");
        }
        return true;
      } // client is connected
   } // if remote service is not NULL
   return false;
} /* connectToserver() */
#endif // ESP32
//
// BLE implementation
//
int BLEDisplay::BLESendVarData(uint16_t *data, int count, void *varData)
{
    uint8_t ucTemp[512];
    int iSize = count * sizeof(uint16_t) + data[1];
    memcpy(ucTemp, data, count*sizeof(uint16_t)); // non-payload part
    memcpy(&ucTemp[count*sizeof(uint16_t)], varData, data[1]); // var payload
#ifdef HAL_ESP32_HAL_H_
    pCharacteristicData->writeValue(ucTemp, iSize, false);
#endif
#ifdef ARDUINO_ARDUINO_NANO33BLE
    pCharacteristicData.writeValue(ucTemp, iSize, false);
#endif
    return RD_SUCCESS;
} /* BLESendVarData() */

int BLEDisplay::BLESend(uint16_t *data, int count)
{
    data[count] = crc_16((uint8_t *)data, count * sizeof(uint16_t));
#ifdef HAL_ESP32_HAL_H_
    pCharacteristicData->writeValue((uint8_t *)data, (count+1)*sizeof(uint16_t), false);
#endif
#ifdef ARDUINO_ARDUINO_NANO33BLE
    pCharacteristicData.writeValue((uint8_t *)data, (count+1)*sizeof(uint16_t), false);
#endif

    return RD_SUCCESS;
} /* BLESend() */

int BLEDisplay::begin(uint16_t display_type)
{
//    _width = width;
//    _height = _height;
    _bConnected = false;
#ifdef HAL_ESP32_HAL_H_
    pCharacteristicData = NULL;
    BLEDevice::init("ESP32BLE");
    Scanned_BLE_Name[0] = 0;
    pBLEScan = BLEDevice::getScan(); //create new scan
    pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks()); //Call the class that is defined above
    pBLEScan->setActiveScan(true); //active scan uses more power, but get results faster
   foundDevices = pBLEScan->start(3); //Scan for 3 seconds to find the Fitness band

  while (!paired && foundDevices.getCount() >= 1)
  {
    if (strcmp(Scanned_BLE_Name,VD_BLE_Name.c_str()) == 0) // found the device we want
    {
//      pBLEScan->stop(); // stop scanning
      yield();
      Serial.println("Found Device :-)... connecting to Server as client");
      Scanned_BLE_Name[0] = 0; // don't reconnect until we scan again
      if (connectToserver(*Server_BLE_Address))
      {
      paired = true;
      break;
      }
    }
  }
    if (pCharacteristicData != nullptr)
    {
        _bConnected = 1;
        return RD_SUCCESS;
    }
  return RD_NOT_CONNECTED;
#endif // ESP32

#ifdef ARDUINO_ARDUINO_NANO33BLE
    long lTime;
    bool bFound = false;
    BLE.begin();
    BLE.setLocalName("Nano33BLE");
    Serial.println("About to start scan");
    BLE.scanForName("RemoteDisplay", true);
    lTime = millis();
    while (!bFound && (millis() - lTime) < 5000L)
    {
    // check if a peripheral has been discovered
        peripheral = BLE.available();
        if (peripheral)
        {
        // discovered a peripheral, print out address, local name, and advertised service
            Serial.print("Found ");
            Serial.print(peripheral.address());
            Serial.print(" '");
            Serial.print(peripheral.localName());
            Serial.print("' ");
            Serial.print(peripheral.advertisedServiceUuid());
            Serial.println();
            if (memcmp(peripheral.localName().c_str(), "RemoteDisplay", 14) == 0)
            { // found the one we're looking for
               BLE.stopScan();
               bFound = 1;
            } // found it in scan
        } // peripheral located
        else
        {
           delay(50);
        }
    } // while scanning
    if (bFound)
    {
       // Connect to the BLE Server.
       Serial.println("connection attempt...");
       if (peripheral.connect())
       {
          Serial.println("Connected!");
          peripheral.discoverAttributes();
          if (peripheral.discoverService("fea0"))
          {
             Serial.println("Discovered fea0 service");
             prtService = peripheral.service("0000fea0-0000-1000-8000-00805f9b34fb"); // get the virtual display service
             if (prtService)
//             if (1)
             {
                Serial.println("Got the service");
                pCharacteristicData = prtService.characteristic("0000fea1-0000-1000-8000-00805f9b34fb");
                if (pCharacteristicData)
                {
                    Serial.println("Got the characteristics");
                    Serial.print("Properties = 0x");
                    Serial.println(pCharacteristicData.properties(), HEX);
                    _bConnected = 1;
                }
             } else {
               Serial.println("Couldn't get service");
             }
          }
       }
    }
    if (_bConnected)
    {
        uint16_t u16Tmp[4];
        u16Tmp[0] = RD_INIT;
        u16Tmp[1] = display_type;
        return BLESend(u16Tmp, 2); // send to the remote server
    }
#endif // Nano33
    
    return RD_NOT_CONNECTED;
} /* begin() */
int BLEDisplay::fill(uint16_t u16Color)
{
    uint16_t u16Tmp[4];
    if (!_bConnected)
        return RD_NOT_CONNECTED;
    u16Tmp[0] = RD_FILL;
    u16Tmp[1] = u16Color;
    return BLESend(u16Tmp, 2); // send to the remote server
} /* fill() */

int BLEDisplay::drawLine(int x1, int y1, int x2, int y2, uint16_t u16Color)
{
    uint16_t u16Tmp[8];
    if (!_bConnected)
        return RD_NOT_CONNECTED;
    u16Tmp[0] = RD_DRAW_LINE;
    u16Tmp[1] = (uint16_t)x1;
    u16Tmp[2] = (uint16_t)y1;
    u16Tmp[3] = (uint16_t)x2;
    u16Tmp[4] = (uint16_t)y2;
    u16Tmp[5] = u16Color;
    return BLESend(u16Tmp, 6); // send to the remote server
} /* drawLine() */

int BLEDisplay::drawPixel(int x, int y, uint16_t u16Color)
{
    uint16_t u16Tmp[8];
    if (!_bConnected)
        return RD_NOT_CONNECTED;
    u16Tmp[0] = RD_DRAW_PIXEL;
    u16Tmp[1] = (uint16_t)x;
    u16Tmp[2] = (uint16_t)y;
    u16Tmp[3] = u16Color;
    return BLESend(u16Tmp, 4); // send to the remote server
} /* drawPixel() */

int BLEDisplay::setWindow(int x, int y, int w, int h)
{
    uint16_t u16Tmp[8];
    if (!_bConnected)
        return RD_NOT_CONNECTED;
    u16Tmp[0] = RD_SET_WINDOW;
    u16Tmp[1] = (uint16_t)x;
    u16Tmp[2] = (uint16_t)y;
    u16Tmp[3] = (uint16_t)w;
    u16Tmp[4] = (uint16_t)h;
    return BLESend(u16Tmp, 5); // send to the remote server
} /* setWindow() */

int BLEDisplay::writePixels(void *pixels, int count, uint8_t bDMA)
{
     uint16_t u16Tmp[8];
     if (!_bConnected)
         return RD_NOT_CONNECTED;
     u16Tmp[0] = RD_WRITE_PIXELS;
     u16Tmp[1] = (uint16_t)count*2; // payload size in bytes
     u16Tmp[2] = (uint16_t)bDMA;
     return BLESendVarData(u16Tmp, 3, pixels); // send to the remote server
 } /* writePixels() */

int BLEDisplay::drawRect(int x, int y, int w, int h, uint16_t u16Color, int bFilled)
{
    uint16_t u16Tmp[8];
    if (!_bConnected)
        return RD_NOT_CONNECTED;
    u16Tmp[0] = RD_DRAW_RECT;
    u16Tmp[1] = (uint16_t)x;
    u16Tmp[2] = (uint16_t)y;
    u16Tmp[3] = (uint16_t)w;
    u16Tmp[4] = (uint16_t)h;
    u16Tmp[5] = u16Color;
    u16Tmp[6] = (uint16_t)bFilled;
    return BLESend(u16Tmp, 7); // send to the remote server
} /* drawRect() */

int BLEDisplay::drawText(int x, int y, char *szText, uint8_t u8Font, uint16_t u16FGColor, uint16_t u16BGColor)
{
     uint16_t u16Tmp[8];
     if (!_bConnected)
         return RD_NOT_CONNECTED;
     u16Tmp[0] = RD_DRAW_TEXT;
     u16Tmp[1] = strlen(szText); // payload size in bytes
     u16Tmp[2] = (uint16_t)x;
     u16Tmp[3] = (uint16_t)y;
     u16Tmp[4] = (uint16_t)u8Font;
     u16Tmp[5] = u16FGColor;
     u16Tmp[6] = u16BGColor;
     return BLESendVarData(u16Tmp, 7, szText); // send to the remote server
 } /* writePixels() */

int BLEDisplay::drawEllipse(int x, int y, int r1, int r2, uint16_t u16Color, int bFilled)
{
    uint16_t u16Tmp[8];
    if (!_bConnected)
        return RD_NOT_CONNECTED;
    u16Tmp[0] = RD_DRAW_ELLIPSE;
    u16Tmp[1] = (uint16_t)x;
    u16Tmp[2] = (uint16_t)y;
    u16Tmp[3] = (uint16_t)r1;
    u16Tmp[4] = (uint16_t)r2;
    u16Tmp[5] = u16Color;
    u16Tmp[6] = bFilled;
    return BLESend(u16Tmp, 7); // send to the remote server
} /* drawEllipse() */

int BLEDisplay::setOrientation(int angle)
{
    int rc;
    uint16_t u16Tmp[4];
    if (!_bConnected)
        return RD_NOT_CONNECTED;
    u16Tmp[0] = RD_SET_ORIENTATION;
    u16Tmp[1] = (uint16_t)angle;
    rc = BLESend(u16Tmp, 2); // send to the remote server
    if (rc == RD_SUCCESS)
       _orientation = angle;
    return rc;
} /* setOrientation() */
//
// UART implementation
//
int UARTDisplay::begin(uint32_t u32Capabilities, uint16_t width, uint16_t height, uint32_t u32Speed)
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
