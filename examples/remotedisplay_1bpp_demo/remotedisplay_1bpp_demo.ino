//
// RemoteDisplay demo
//
#include <RemoteDisplay.h>

//BLEDisplay disp;
//SPIDisplay disp;
I2CDisplay disp;
#define CS_PIN -1
#define DC_PIN A3
#define RESET_PIN A2
#define LED_PIN -1
int button_list[] = {15, 16}; // 2 push buttons

void setup() {
//  Serial.begin(115200);
//  while (!Serial) {};

  disp.defineButtons(button_list, 2, LOW);
  disp.begin(RD_OLED_128x64);
//  disp.begin(RD_LCD_ILI9341, FLAGS_NONE, 8000000, CS_PIN, DC_PIN, RESET_PIN, LED_PIN);
  disp.setOrientation(0);
} /* setup() */

void loop() {
int x = 0;

  disp.fill(0);
  disp.drawText(0, 0, (char *)"0 Circles", FONT_12x16, 0x7e0, 0);
  disp.drawText(0, 16, (char *)"1 Lines", FONT_12x16, 0xffe0, 0);
  while (x == 0)
  {
    x = disp.getButtons();
    delay(100);
  }
  disp.fill(0);
  if (x & 1) // button 0
  {
    for (x=1; x<32; x++)
    {
      disp.drawEllipse(64, 32, x, x, 0x7e0, 0); 
    }
  }
  else if (x & 2) // button 1
  {
    for (x=0; x<127; x += 2)
    {
      disp.drawLine(x, 0, 127-x, 63, 0xffe0);
    }
    for (x=0; x<63; x += 2)
    {
      disp.drawLine(127, x, 0, 63-x, 0xffe0);
    }
  }
  delay(2000);
} /* loop() */
