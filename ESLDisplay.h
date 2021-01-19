lass ESLDisplay : public RemoteDisplay
{
public:
    ESLDisplay() : RemoteDisplay() {}
    ~ESLDisplay() {}
    int begin(uint16_t u16DisplayType, int SDAPin = -1, int SCLPin = -1, int bBitBang = 0, uint32_t u32Speed = 400000);
    void shutdown();
    int fill(uint16_t u16Color);
    int drawLine(int x1, int y1, int x2, int y2, uint16_t u16Color);
    int drawPixel(int x, int y, uint16_t u16Color);
    int setWindow(int x, int y, int w, int h);
    int writePixels(uint16_t *pixels, int count, uint8_t bDMA);
    int drawRect(int x, int y, int w, int h, uint16_t u16Color, int bFilled);
    int drawText(int x, int y, char *szText, uint8_t u8Font, uint16_t u16FGColor, uint16_t u16BGColor);
    int drawBitmap(int x, int y, int bitmapIndex, int stretch);
    int drawIcon(int x, int y, int iconIndex, int angle, uint16_t u16FGColor, uint16_t u16BGColor);
    int drawEllipse(int x, int y, int r1, int r2, uint16_t u16Color, int bFilled);
    int setFont(const GFXfont *pFont, int fontIndex);
    int setBitmap(uint8_t bitmapIndex, const uint8_t *pBitmap, int iBitmapSize);
    int setOrientation(int angle);
    int dumpBuffer(uint8_t * buffer);
    uint16_t getButtons();
private:
    OBDISP _obd;
    GFXfont *_fonts[MAX_FONT_INDEX];
    uint8_t *_bitmaps[MAX_BITMAP_INDEX];
    uint8_t *_icons[MAX_ICON_INDEX];
    int _x, _y, _w, _h; // current window info
    uint8_t _buffer[1024]; // DEBUG

}; // class ESLDisplay

