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
static BLEUUID serviceUUID("0000fea0-1234-1000-8000-00805f9b34fb"); //Service
static BLEUUID dataUUID("0000fea1-0000-1000-8000-00805f9b34fb"); // data characteristic
//static BLEUUID nameUUID("0000fea2-0000-1000-8000-00805f9b34fb"); // name characteristic
static std::string VD_BLE_Name = "RemoteDisplay";
char Scanned_Name[32];
String Scanned_Address;
static BLEScanResults foundDevices;
static BLEAddress *Server_BLE_Address;
static volatile boolean paired = false; //boolean variable to togge light
static BLEServer *pServer;
static BLEScan *pBLEScan;
static BLEService *pService;
static BLERemoteCharacteristic *pCharacteristicData;
#endif // HAL_ESP32_HAL_H_

// Bluetooth support for Adafruit nrf52 boards
#ifdef ARDUINO_NRF52_ADAFRUIT
#include <bluefruit.h>
const uint8_t myServiceUUID[16] = {0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x34, 0x12, 0xa0, 0xfe, 0x00, 0x00};
const uint8_t myDataUUID[16] = {0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x34, 0x12, 0xa1, 0xfe, 0x00, 0x00};
//#define myServiceUUID 0xFEA0
//#define myDataUUID 0xFEA1
static int isConnected;
BLEClientCharacteristic myDataChar(myDataUUID);
BLEClientService myService(myServiceUUID);

/**
 * Callback invoked when an connection is established
 * @param conn_handle
 */
static void connect_callback(uint16_t conn_handle)
{
//  Serial.println("Connected!");
//  Serial.print("Discovering FEA0 Service ... ");
 
  // If FEA0 is not found, disconnect and return
  if ( !myService.discover(conn_handle) )
  {
//    Serial.println("Found NONE");
    // disconect since we couldn't find our service
    Bluefruit.disconnect(conn_handle);
    return;
  }
 
  // Once FEA0 service is found, we continue to discover its characteristics
  if ( !myDataChar.discover() )
  {
    // Data char is mandatory, if it is not found (valid), then disconnect
//    Serial.println("Data characteristic is mandatory but not found");
    Bluefruit.disconnect(conn_handle);
    return;
  }
    isConnected = 1; // success!
} /* connect_callback() */
/**
 * Callback invoked when a connection is dropped
 * @param conn_handle
 * @param reason
 */
static void disconnect_callback(uint16_t conn_handle, uint8_t reason)
{
  (void) conn_handle;
  (void) reason;
    isConnected = 0;
//  Serial.println("Disconnected");
} /* disconnect_callback() */

static void scan_callback(ble_gap_evt_adv_report_t* report)
{
//    Serial.printf("found something %s\n", report->data.p_data);
//  if (Bluefruit.Scanner.checkReportForUuid(report, myServiceUUID))
    char *name = (char *)report->data.p_data;
    int i;
    for (i=0; i<report->data.len; i++)
      if (name[i] == 'R') break; // "parse" for the name in the adv data
  if (name && memcmp(&name[i], "RemoteDisplay", 13) == 0)
  {
//      Serial.println("Found RemoteDisplay");
      Bluefruit.Scanner.stop();
//      Serial.print("RemoteDisplay UUID detected. Connecting ... ");
      Bluefruit.Central.connect(report);
  }
  else // keep looking
  {
    // For Softdevice v6: after received a report, scanner will be paused
    // We need to call Scanner resume() to continue scanning
    Bluefruit.Scanner.resume();
  }
} /* scan_callback() */

static void notify_callback(BLEClientCharacteristic* chr, uint8_t* data, uint16_t len)
{
} /* notify_callback() */

#endif // Adafruit nrf52

// Bluetooth support for Nano 33 BLE
#ifdef ARDUINO_ARDUINO_NANO33BLE
#include <ArduinoBLE.h>

static BLEDevice peripheral;
static BLEService prtService;
static BLECharacteristic pCharacteristicData;
#endif // Nano 33 BLE
#ifdef HAL_ESP32_HAL_H_
// Called for each device found during a BLE scan by the client
class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks
{
    void onResult(BLEAdvertisedDevice advertisedDevice) {
      Serial.printf("Scan Result: %s \n", advertisedDevice.toString().c_str());
      if (Scanned_Name[0] == 0 && strcmp(VD_BLE_Name.c_str(), advertisedDevice.getName().c_str()) == 0) { // this is what we want
        Server_BLE_Address = new BLEAddress(advertisedDevice.getAddress());
        Scanned_Address = Server_BLE_Address->toString().c_str();
        strcpy(Scanned_Name, advertisedDevice.getName().c_str());
        Serial.printf("Found what we're looking for!\n");
        pBLEScan->stop(); // stop scanning
      }
    }
};

// When the scan has found the BLE server device name we're looking for, we try to connect
static bool connectToserver (BLEAddress pAddress)
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
uint16_t BLEDisplay::BLEReceive()
{
    uint16_t value = 0;
#ifdef HAL_ESP32_HAL_H_
    value = pCharacteristicData->readUInt16();
#endif
#ifdef ARDUINO_ARDUINO_NANO33BLE
    pCharacteristicData.readValue(&value, 2);
#endif
#ifdef ARDUINO_NRF52_ADAFRUIT
    myDataChar.read((void *)&value, (uint16_t)2);
#endif
    return value;
} /* BLEReceive() */

int BLEDisplay::BLESendVarData(uint16_t *data, int count, void *varData)
{
    uint8_t ucTemp[256];
    static int iCounter = 0;
    int iSize = count*2 + data[1];
    
    data[0] |= (uint16_t)(iSize << 8); // total packet size in bytes
    iCounter++; // count packets to know when we need to ask for a response
    // If we send too many without responses, the BLE lib will hang
    memcpy(ucTemp, data, count*sizeof(uint16_t)); // non-payload part
    memcpy(&ucTemp[count*sizeof(uint16_t)], varData, data[1]); // var payload
#ifdef HAL_ESP32_HAL_H_
    pCharacteristicData->writeValue(ucTemp, iSize, (iCounter & 0x3f) == 0);
#endif
#ifdef ARDUINO_ARDUINO_NANO33BLE
    pCharacteristicData.writeValue(ucTemp, iSize, (iCounter & 0x3f) == 0);
#endif
#ifdef ARDUINO_NRF52_ADAFRUIT
    myDataChar.write((const void *)ucTemp, (uint16_t)iSize);
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
#ifdef ARDUINO_NRF52_ADAFRUIT
    myDataChar.write((const void *)data, (uint16_t)(count+1)*sizeof(uint16_t));
#endif
    return RD_SUCCESS;
} /* BLESend() */

int BLEDisplay::begin(uint16_t display_type)
{
    _display_type = display_type;
    _bConnected = 0;
#ifdef ARDUINO_NRF52_ADAFRUIT
    isConnected = 0;
    // Initialize Bluefruit with maximum connections as Peripheral = 0, Central = 1
    // SRAM usage required by SoftDevice will increase dramatically with number of connections
    Bluefruit.begin(0, 1);
    /* Set the device name */
    Bluefruit.setName("Bluefruit52");
    /* Set the LED interval for blinky pattern on BLUE LED */
    Bluefruit.setConnLedInterval(250);
    Bluefruit.configCentralBandwidth(BANDWIDTH_MAX);
    myService.begin(); // start my client service
    // Initialize client characteristics of VirtualDisplay.
    // Note: Client Chars will be added to the last service that is begin()ed.
    myDataChar.setNotifyCallback(notify_callback);
    myDataChar.begin();
    // Callbacks for Central
    Bluefruit.Central.setConnectCallback(connect_callback);
    Bluefruit.Central.setDisconnectCallback(disconnect_callback);
    /* Start Central Scanning
     * - Enable auto scan if disconnected
     * - Filter out packet with a min rssi
     * - Interval = 100 ms, window = 50 ms
     * - Use active scan (used to retrieve the optional scan response adv packet)
     * - Start(0) = will scan forever since no timeout is given
     */
    Bluefruit.Scanner.setRxCallback(scan_callback);
    Bluefruit.Scanner.restartOnDisconnect(true);
//    Bluefruit.Scanner.filterRssi(-72);
//    Bluefruit.Scanner.filterUuid(myServiceUUID);
    Bluefruit.Scanner.setInterval(160, 80);       // in units of 0.625 ms
    Bluefruit.Scanner.useActiveScan(true);        // Request scan response data
    Bluefruit.Scanner.start(0);                   // 0 = Don't stop scanning after n seconds
    {
        int iTimeout = 0; // scan / try for up to 10 seconds
        while (iTimeout < 10 && isConnected == 0)
        {
            iTimeout++;
            delay(1000);
        }
        if (isConnected == 1)
        {
            _bConnected = 1;
            delay(2000); // wait for things to settle
        }
    }
#endif // Adafruit nrf52
#ifdef HAL_ESP32_HAL_H_
    pCharacteristicData = NULL;
    BLEDevice::init("ESP32BLE");
    Scanned_Name[0] = 0;
    pBLEScan = BLEDevice::getScan(); //create new scan
    pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks()); //Call the class that is defined above
    pBLEScan->setActiveScan(true); //active scan uses more power, but get results faster
   foundDevices = pBLEScan->start(3); //Scan for 3 seconds to find the Fitness band

  while (!paired && foundDevices.getCount() >= 1)
  {
    if (strcmp(Scanned_Name,VD_BLE_Name.c_str()) == 0) // found the device we want
    {
//      pBLEScan->stop(); // stop scanning
      yield();
      Serial.println("Found Device :-)... connecting to Server as client");
      Scanned_Name[0] = 0; // don't reconnect until we scan again
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
    }
    else
    {
        return RD_NOT_CONNECTED;
    }
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
           Serial.println("discovered attributes");
          if (peripheral.discoverService("0000fea0-1234-1000-8000-00805f9b34fb"))
          {
             Serial.println("Discovered fea0 service");
             prtService = peripheral.service("0000fea0-1234-1000-8000-00805f9b34fb"); // get the remote display service
             if (prtService)
//             if (1)
             {
                Serial.println("Got the service");
                pCharacteristicData = prtService.characteristic("0000fea1-1234-1000-8000-00805f9b34fb");
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
#endif // Nano33
    
#if defined( HAL_ESP32_HAL_H_ ) || defined( ARDUINO_ARDUINO_NANO33BLE ) || defined( ARDUINO_NRF52_ADAFRUIT )
    if (_bConnected)
    {
        uint16_t u16Tmp[4];
        u16Tmp[0] = RD_INIT;
        u16Tmp[1] = display_type;
        return BLESend(u16Tmp, 2); // send to the remote server
    }
    
    return RD_NOT_CONNECTED;
#endif
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

// transmit font data to the display server
int BLEDisplay::setFont(const GFXfont *pFont, int fontIndex)
{
    int iMaxOffset, iMaxIndex;
    int rc, i, iSize, iCount;
    uint16_t u16Tmp[8];
    GFXglyph *glyph;
    uint8_t *pData;
    // figure out the bitmap image data size
    glyph = pFont->glyph; // glyph table
    iMaxOffset = 0;
    // the data may not be in order (it probably is)
    iSize = pFont->last - pFont->first;
    for (i=0; i < iSize; i++) {
        if (glyph[i].bitmapOffset > iMaxOffset) {
            iMaxOffset = glyph[i].bitmapOffset;
            iMaxIndex = i;
        }
    } // for i
    // Now that we have the max bitmap offset, calculate the last bitmap byte
//    Serial.printf("max bitmap offset = %d\n", iMaxOffset);
    iSize = glyph[iMaxIndex].width * glyph[iMaxIndex].height;
    iSize = (iSize + 7)/8; // number of bytes
    iMaxOffset += iSize; // we now know how much bitmap data to send
    // send the font info first to allow the server to allocate memory
    u16Tmp[0] = RD_SET_FONT_INFO;
    u16Tmp[1] = fontIndex;
    u16Tmp[2] = pFont->first;    ///< ASCII extents (first char)
    u16Tmp[3] = pFont->last;     ///< ASCII extents (last char)
    u16Tmp[4] = pFont->yAdvance; ///< Newline distance (y axis)
    u16Tmp[5] = (uint16_t)iMaxOffset; // bitmap size
    u16Tmp[6] = (uint16_t)(iMaxOffset >> 16);
    rc = BLESend(u16Tmp, 7);

    iCount = (iMaxOffset + MAX_DATA_BLOCK-1) / MAX_DATA_BLOCK; // send max 230 bytes per block
//    Serial.printf("About to send font bitmap size %d, blocks: %d\n", iMaxOffset, iCount);
    pData = (uint8_t *)pFont->bitmap; // start of bitmap data
    // Transmit the font's bitmap data first
    rc = RD_SUCCESS;
    for (i=0; i<iCount && rc == RD_SUCCESS; i++) {
        u16Tmp[0] = RD_SET_FONT_BITMAP;
        u16Tmp[4] = fontIndex;
        u16Tmp[2] = i; // current block
        u16Tmp[3] = iCount; // total blocks
        iSize = (iMaxOffset > MAX_DATA_BLOCK) ? MAX_DATA_BLOCK : iMaxOffset;
        u16Tmp[1] = iSize; // payload size
        rc = BLESendVarData(u16Tmp, 5, (void *)pData); // send to the remote server
        pData += iSize;
        iMaxOffset -= iSize;
    } // for each block of bitmap data
    // Now transmit the font index data
    iMaxOffset = pFont->last - pFont->first; // number of entries
    iMaxOffset *= sizeof(GFXglyph); // might be packed structure or not
    pData = (uint8_t *)pFont->glyph;
    iCount = (iMaxOffset + MAX_DATA_BLOCK-1) / MAX_DATA_BLOCK; // send max 230 bytes per block
//    Serial.printf("About to send font index size %d, blocks: %d\n", iMaxOffset, iCount);
    for (i=0; i<iCount && rc == RD_SUCCESS; i++) {
        u16Tmp[0] = RD_SET_FONT_INDEX;
        iSize = (iMaxOffset > MAX_DATA_BLOCK) ? MAX_DATA_BLOCK : iMaxOffset;
        u16Tmp[1] = iSize; // payload size
        u16Tmp[2] = (i | (iCount << 8)); // current block / total blocks
        u16Tmp[3] = fontIndex; // || (sizeof(GFXglyph) << 8); // send structure size too because it can be 7 or 8
        rc = BLESendVarData(u16Tmp, 4, (void *)pData); // send to the remote server
        pData += iSize;
        iMaxOffset -= iSize;
    } // for each block of bitmap data
    return rc;
} /* BLEDisplay::setFont() */

int BLEDisplay::setBitmap(uint8_t bitmapIndex, const uint8_t *pBitmap, int iBitmapSize)
{
    int i, iSize, iCount, rc = RD_SUCCESS;
    uint8_t *pData;
    uint16_t u16Tmp[6];
    
    iCount = (iBitmapSize + MAX_DATA_BLOCK-1) / MAX_DATA_BLOCK; // send max 230 bytes per block
    pData = (uint8_t *)pBitmap;
    // Transmit the font's bitmap data first
    rc = RD_SUCCESS;
    for (i=0; i<iCount && rc == RD_SUCCESS; i++) {
        u16Tmp[0] = RD_SET_BITMAP;
        u16Tmp[4] = bitmapIndex;
        u16Tmp[2] = i; // current block
        u16Tmp[3] = iCount; // total blocks
        iSize = (iBitmapSize > MAX_DATA_BLOCK) ? MAX_DATA_BLOCK : iBitmapSize;
        u16Tmp[1] = iSize; // payload size
        rc = BLESendVarData(u16Tmp, 5, (void *)pData); // send to the remote server
        pData += iSize;
        iBitmapSize -= iSize;
    } // for each block of bitmap data

    return rc;
} /* setBitmap() */

int BLEDisplay::drawBitmap(int x, int y, int bitmapIndex, int stretch)
{
    uint16_t u16Tmp[6];
    
    if (bitmapIndex < 0 || bitmapIndex >= MAX_BITMAP_INDEX)
        return RD_INVALID_PARAMETER;
    u16Tmp[0] = RD_DRAW_BITMAP;
    u16Tmp[1] = (uint16_t)bitmapIndex;
    u16Tmp[2] = (uint16_t)x;
    u16Tmp[3] = (uint16_t)y;
    u16Tmp[4] = (uint16_t)stretch;
    return BLESend(u16Tmp, 5);
} /* drawBitmap() */

int BLEDisplay::drawIcon(int x, int y, int iconIndex, int angle, uint16_t u16FGColor, uint16_t u16BGColor)
{
    return RD_SUCCESS;
} /* drawIcon() */

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

int BLEDisplay::writePixels(uint16_t *pixels, int count, uint8_t bDMA)
{
     uint16_t u16Tmp[8];
     if (!_bConnected)
         return RD_NOT_CONNECTED;
     while (count >= 64) // don't send more than 64 at a time
     {
         u16Tmp[0] = RD_WRITE_PIXELS;
         u16Tmp[1] = 128; // payload size in bytes
         u16Tmp[2] = (uint16_t)bDMA;
         BLESendVarData(u16Tmp, 3, (void *)pixels); // send to the remote server
         pixels += 64;
         count -= 64;
     }
    if (count > 0)
    {
        u16Tmp[0] = RD_WRITE_PIXELS;
        u16Tmp[1] = count*2; // payload size in bytes
        u16Tmp[2] = (uint16_t)bDMA;
        BLESendVarData(u16Tmp, 3, (void *)pixels); // send to the remote
    }
    return RD_SUCCESS;
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
     u16Tmp[1] = strlen(szText)+1; // payload size in bytes
     u16Tmp[2] = (uint16_t)x;
     u16Tmp[3] = (uint16_t)y;
     u16Tmp[4] = (uint16_t)u8Font;
     u16Tmp[5] = u16FGColor;
     u16Tmp[6] = u16BGColor;
     return BLESendVarData(u16Tmp, 7, szText); // send to the remote server
 } /* drawText() */

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

uint16_t BLEDisplay::getButtons()
{
    return BLEReceive();
} /* BLEDisplay::getButtons() */

int BLEDisplay::dumpBuffer(uint8_t * buffer)
{
    if (_bpp == 1) {
        uint16_t u16Tmp[4];
        int iCount = (_width * _height) / 8;
        u16Tmp[0] = RD_DUMP_BUFFER;
        u16Tmp[1] = iCount/2; // in terms of uint16_t's
        return BLESendVarData(u16Tmp, 2, (void *)buffer);
    } else {
        return RD_NOT_SUPPORTED;
    }
} /* BLEDisplay::dumpBuffer() */

