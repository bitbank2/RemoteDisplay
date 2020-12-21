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
  if (Bluefruit.Scanner.checkReportForUuid(report, myServiceUUID))
  {
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
    uint8_t ucTemp[512];
    static int iCounter = 0;
    int iSize = count*2 + data[1];
    
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
    if (iSize > 20) // only write with response supports large packets
    {
        myDataChar.write_resp((const void *)ucTemp, (uint16_t)iSize);
    }
    else
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
    Bluefruit.Scanner.filterUuid(myServiceUUID);
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
    // not implemented yet
    (void)buffer;
    return RD_SUCCESS;
} /* BLEDisplay::dumpBuffer() */
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
//
// I2C Display implementation
//
int I2CDisplay::begin(uint16_t u16DisplayType, int SDAPin, int SCLPin, int bBitBang, uint32_t u32Speed)
{
    (void)u32Speed; // DEBUG
    (void)SDAPin;
    (void)SCLPin;
    (void)bBitBang;

    _display_type = u16DisplayType;
    return RD_SUCCESS;
} /* begin() */

int SPIDisplay::begin(uint16_t u16DisplayType, uint16_t u16Flags, uint32_t u32Speed, int CS_Pin, int DC_Pin, int RESET_Pin, int LED_Pin)
{
    _display_type = u16DisplayType;
    memset(&_lcd, 0, sizeof(_lcd));
    // For now, assume SPI displays will be color LCD/OLEDs
    if (spilcdInit(&_lcd, u16DisplayType, u16Flags, u32Speed, CS_Pin, DC_Pin, RESET_Pin, LED_Pin, -1, -1, -1))
        return RD_INIT_FAILED;
    return RD_SUCCESS;
} /* SPIDisplay:begin() */

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
    spilcdWriteString(&_lcd, x, y, szText, u16FGColor, u16BGColor, u8Font, DRAW_TO_LCD);
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
    uint16_t buttons = 0;

    for (int i=0; i<_button_count; i++)
        if (digitalRead(_buttons[i]) == _button_active)
            buttons |= (1 << i);
    
    return buttons;
} /* SPIDisplay::getButtons()*/
