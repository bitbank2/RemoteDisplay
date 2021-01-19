//
// SPI (locally connected)
//
class SPIDisplay : public RemoteDisplay
{
public:
    SPIDisplay() : RemoteDisplay() {}
    ~SPIDisplay() {}
    int begin(uint16_t u16LCDType, uint16_t u16Flags, uint32_t u32Speed, int CS_Pin, int DC_Pin, int RESET_Pin, int LED_Pin);
    void shutdown();
    int fill(uint16_t u16Color);
    int drawLine(int x1, int y1, int x2, int y2, uint16_t u16Color);
    int drawPixel(int x, int y, uint16_t u16Color);
    int setWindow(int x, int y, int w, int h);
    int writePixels(uint16_t *pixels, int count, uint8_t bDMA);
    int drawRect(int x, int y, int w, int h, uint16_t u16Color, int bFilled);
    int drawText(int x, int y, char *szText, uint8_t u8Font, uint16_t u16FGColor, uint16_t u16BGColor);
    int drawEllipse(int x, int y, int r1, int r2, uint16_t u16Color, int bFilled);
    int drawBitmap(int x, int y, int bitmapIndex, int stretch);
    int setFont(const GFXfont *pFont, int fontIndex);
    int setBitmap(uint8_t bitmapIndex, const uint8_t *pBitmap, int iBitmapSize);
    int setOrientation(int angle);
    int dumpBuffer(uint8_t * buffer);
    uint16_t getButtons();
private:
    SPILCD _lcd;
    GFXfont *_fonts[MAX_FONT_INDEX];
    uint8_t *_bitmaps[MAX_BITMAP_INDEX];
    uint8_t *_icons[MAX_ICON_INDEX];

}; // class SPIDisplay

