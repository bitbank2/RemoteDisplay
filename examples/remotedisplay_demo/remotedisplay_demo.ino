//
// RemoteDisplay demo
//
#include <RemoteDisplay.h>
#include <JPEGDEC.h>
#include "/Users/laurencebank/Documents/Arduino/libraries/JPEGDEC/test_images/st_peters.h"

BLEDisplay disp;
//SPIDisplay disp;
#define CS_PIN -1
#define DC_PIN A3
#define RESET_PIN A2
#define LED_PIN -1
//int button_list[] = {35,39};
int button_list[] = {A6, A7}; // 2 push buttons
JPEGDEC jpeg;

void JPEGDraw(JPEGDRAW *pDraw)
{
  disp.setWindow(pDraw->x, pDraw->y, pDraw->iWidth, pDraw->iHeight);
  disp.writePixels(pDraw->pPixels, pDraw->iWidth * pDraw->iHeight, true); // Use DMA
//  return 1; // continue decoding
} /* JPEGDraw() */


void setup() {
  Serial.begin(115200);
  while (!Serial) {};

  disp.defineButtons(button_list, 2, LOW);
  disp.begin(RD_LCD_ILI9341);
//  disp.begin(RD_LCD_ILI9341, FLAGS_NONE, 8000000, CS_PIN, DC_PIN, RESET_PIN, LED_PIN);
  disp.setOrientation(0);
} /* setup() */

void loop() {
int x = 0;

  disp.fill(0);
  disp.drawText(0, 64, (char *)"0 = Circles", FONT_12x16, 0x7e0, 0);
  disp.drawText(0, 96, (char *)"1 = Lines", FONT_12x16, 0xffe0, 0);
  disp.drawText(0, 128, (char *)"2 = JPEG", FONT_12x16, 0xffff, 0);
  while (x == 0)
  {
    x = disp.getButtons();
    delay(100);
  }
  disp.fill(0);
  if (x & 1) // button 0
  {
    for (x=1; x<120; x++)
    {
      disp.drawEllipse(120, 160, x, x, 0x7e0, 0); 
    }
  }
  else if (x & 2) // button 1
  {
    for (x=0; x<239; x += 2)
    {
      disp.drawLine(x, 0, 239-x, 319, 0xffe0);
    }
    for (x=0; x<319; x += 2)
    {
      disp.drawLine(239, x, 0, 319-x, 0xffe0);
    }
  }
  else if (x & 4) // button 2
  {
     if (jpeg.openFLASH((uint8_t *)st_peters, sizeof(st_peters), JPEGDraw))
    {
//      lTime = micros();
 //     jpeg.setMaxOutputSize(1);
      if (jpeg.decode(0,0,0))
      {
//        lTime = micros() - lTime;
//        Serial.printf("%d x %d image, decode time = %d us\n", jpeg.getWidth() >> i, jpeg.getHeight() >> i, (int)lTime);
      }
      jpeg.close();
    }
  }
  delay(2000);
} /* loop() */
