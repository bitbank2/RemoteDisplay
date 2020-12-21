#include <RemoteDisplay.h>
//#include <Arduino_CRC32.h>

/*
  OV767X - Camera Capture Raw Bytes

  This sketch reads a frame from the OmniVision OV7670 camera
  and writes the bytes to the Serial port. Use the Procesing
  sketch in the extras folder to visualize the camera output.

  Circuit:
    - Arduino Nano 33 BLE board
    - OV7670 camera module:
      - 3.3 connected to 3.3
      - GND connected GND
      - SIOC connected to A5
      - SIOD connected to A4
      - VSYNC connected to 8
      - HREF connected to 10
      - PCLK connected to 12
      - XCLK connected to 9
      - D7 connected to 7
      - D6 connected to 6
      - D5 connected to 5
      - D4 connected to 4
      - D3 connected to 3
      - D2 connected to 2
      - D1 connected to 1 / RX
      - D0 connected to 0 / TX

  This example code is in the public domain.
*/
uint8_t ucTXBuf[1024];
#define CS_PIN -1
#define DC_PIN A3
#define RESET_PIN A2
#define LED_PIN -1
#define MISO_PIN -1
#define MOSI_PIN -1
#define SCK_PIN -1
#include <Arduino_OV767X.h>

BLEDisplay disp;
//SPIDisplay disp;

//Arduino_CRC32 crc32;
int errors = 0;
int count = 0;

int bytesPerFrame;
int iWidth, iHeight;

//byte data[320 * 240 * 2]; // QVGA: 320x240 X 2 bytes per pixel (RGB565)
byte data[160 * 120 * 2]; // QVGA: 320x240 X 2 bytes per pixel (RGB565)

void setup() {
  Serial.begin(115200);
  while (!Serial);

  disp.begin(RD_LCD_ILI9341);
//  disp.begin(RD_LCD_ILI9341, FLAGS_NONE, 8000000, CS_PIN, DC_PIN, RESET_PIN, LED_PIN);
  disp.setOrientation(90);
  disp.fill(0);
  if (!Camera.begin(QQVGA, RGB565, 5)) {
    Serial.println("Failed to initialize camera!");
    while (1);
  }
  iWidth = Camera.width();
  iHeight = Camera.height();
  bytesPerFrame = iWidth * iHeight * Camera.bytesPerPixel();

  // Optionally, enable the test pattern for testing
  // Camera.testPattern();
}

void loop() {
int32_t i, iFrameSize, r, g, b;
uint16_t pixel, *s = (uint16_t *)&data[0];
int32_t iTime;

  iTime = millis();
  Camera.readFrame(data);
  iTime = millis() - iTime;
  Serial.printf("Capture time = %d us\n", iTime);

//  uint32_t const crc32_res = crc32.calc(data, bytesPerFrame);
//  Serial.print("0x");
//  Serial.print(crc32_res, HEX);
//  if (crc32_res != 0x15AB2939 && crc32_res != 0xD3EC95E) {
//    errors++;
//  };

//  count++;
//  Serial.print(" errors:");
//  Serial.print(errors);
//  Serial.print("/");
//  Serial.println(count);

  iFrameSize = iWidth * iHeight;
// average the pixels together and show a single color value
//r = g = b = 0;
//for (i=0; i<iFrameSize; i++)
//{
//   pixel = __builtin_bswap16(s[i]);
//   r += (pixel >> 11);
//   g += ((pixel & 0x7e0) >> 5);
//   b += (pixel & 0x1f);
//}
//  r = r / iFrameSize;
//  g = g / iFrameSize;
//  b = b / iFrameSize;
//  Serial.printf("Average R:%d, G:%d, B:%d\n", r, g, b);
//  Serial.write(data, bytesPerFrame);
//  Serial.flush();
  for (i=0; i<iHeight; i++)
  {
    uint16_t u16Pixels[iWidth];
    uint16_t *s = (uint16_t *)&data[i*iWidth*2];
    disp.setWindow(0,i,iWidth,1);
    // swap byte order
    for (int j=0; j<iWidth; j++) {
       u16Pixels[j] = __builtin_bswap16(*s++);
    }
    disp.writePixels(u16Pixels, iWidth, false);
  }
}
