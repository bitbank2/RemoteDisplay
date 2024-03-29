#define MAX_BUTTONS 4
#define MAX_FONT_INDEX 4
#define MAX_BITMAP_INDEX 4
#define MAX_ICON_INDEX 16
#define MAX_DATA_BLOCK 230
/* Defines and variables */
enum {
   RD_NOP=0,
   RD_INIT,
   RD_DRAW_TEXT,
   RD_DRAW_LINE,
   RD_DRAW_ELLIPSE,
   RD_DRAW_BITMAP,
   RD_DRAW_ICON,
   RD_DRAW_PIXEL,
   RD_WRITE_PIXELS,
   RD_DUMP_BUFFER,
   RD_FILL,
   RD_DRAW_RECT,
   RD_SET_WINDOW,
   RD_SET_ORIENTATION,
   RD_GET_INFO,
   RD_GET_BUTTONS,
   RD_BACKLIGHT,
   RD_SET_FONT_BITMAP,
   RD_SET_FONT_INDEX,
   RD_SET_FONT_INFO,
   RD_SET_BITMAP,
   RD_SET_ICON,
   RD_API_COUNT
};

// Supported display types
enum {
    RD_LCD_INVALID=0,
    // Color LCDs/OLEDs
    RD_LCD_ILI9341, // 240x320
    RD_LCD_ILI9225, // 176x220
    RD_LCD_HX8357, // 320x480
    RD_LCD_ST7735R, // 128x160
    RD_LCD_ST7735S, // 80x160 with offset of 24,0
    RD_LCD_ST7735S_B, // 80x160 with offset of 26,2
    RD_LCD_SSD1331,
    RD_LCD_SSD1351,
    RD_LCD_ILI9342, // 320x240 IPS
    RD_LCD_ST7789, // 240x320
    RD_LCD_ST7789_240,  // 240x240
    RD_LCD_ST7789_135, // 135x240
    RD_LCD_ST7789_NOCS, // 240x240 without CS, vertical offset of 80, MODE3
    RD_LCD_SSD1283A, // 132x132
    RD_LCD_ILI9486, // 320x480
    // Monochrome LCDs/OLEDs
    RD_OLED_128x128,
    RD_OLED_128x32,
    RD_OLED_128x64,
    RD_OLED_132x64,
    RD_OLED_64x32,
    RD_OLED_96x16,
    RD_OLED_72x40,
    RD_LCD_UC1701,
    RD_LCD_UC1609,
    RD_LCD_HX1230,
    RD_LCD_NOKIA5110,
    RD_SHARP_144x168,
    RD_SHARP_400x240,
    RD_EPOP_50,
    RD_EPOP_500,
    RD_EPOP_900,
    RD_LCD_COUNT
};

// Error codes returned by getLastError()
enum {
    RD_SUCCESS = 0,
    RD_INVALID_PARAMETER,
    RD_INIT_FAILED,
    RD_BUSY,
    RD_NOT_CONNECTED,
    RD_NOT_SUPPORTED
};

// Built-in font sizes
enum {
    RD_FONT_6x8 = 0,
    RD_FONT_8x8,
    RD_FONT_12x16,
    RD_FONT_16x16,
    RD_FONT_16x32,
    RD_FONT_CUSTOM_0,
    RD_FONT_CUSTOM_1,
    RD_FONT_CUSTOM_2,
    RD_FONT_CUSTOM_3,
    RD_FONT_COUNT
};


