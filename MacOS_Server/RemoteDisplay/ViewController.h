//
//  ViewController.h
//  RemoteDisplay
//
//  Created by Laurence Bank on 4/29/20.
//  Copyright © 2020 Laurence Bank. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@interface ViewController : NSViewController
@property (weak) IBOutlet NSImageView *theImage;
@property (weak) IBOutlet NSButton *Button0;
@property (weak) IBOutlet NSButton *Button1;
@property (weak) IBOutlet NSButton *Button2;
+ (void) processBytes:(NSData *)thedata;
+ (int) getButtons;
- (void) showOLED;
- (void) showOLEDNotification:(NSNotification *) notification;
+ (void) showText:(NSString *)name;

enum {
   RD_NOP=0,
   RD_INIT,
   RD_DRAW_TEXT,
   RD_DRAW_LINE,
   RD_DRAW_ELLIPSE,
   RD_DRAW_ICON,
   RD_DRAW_BITMAP,
   RD_DRAW_PIXEL,
   RD_WRITE_PIXELS,
   RD_FILL,
   RD_DRAW_RECT,
   RD_SET_WINDOW,
   RD_SET_ORIENTATION,
   RD_GET_INFO,
   RD_GET_BUTTONS,
   RD_BACKLIGHT,
   RD_ADD_ASSET,
   RD_API_COUNT
};
// Built-in font sizes
enum {
   FONT_6x8 = 0,
   FONT_8x8,
   FONT_12x16,
   FONT_16x16,
   FONT_16x32,
   FONT_COUNT
};
// Error codes returned by getLastError()
enum {
    RD_SUCCESS = 0,
    RD_INVALID_PARAMETER,
    RD_INIT_FAILED,
    RD_BUSY,
    RD_NOT_CONNECTED
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
    RD_LCD_COUNT
};

@end

