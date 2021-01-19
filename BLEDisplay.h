//
// Bluetooth Low Energy (remote)
//
class BLEDisplay : public RemoteDisplay
{
public:
    BLEDisplay() : RemoteDisplay() {}
    ~BLEDisplay() {}
    int begin(uint16_t display_type);
    int fill(uint16_t u16Color);
    int drawLine(int x1, int y1, int x2, int y2, uint16_t u16Color);
    int drawPixel(int x, int y, uint16_t u16Color);
    int setWindow(int x, int y, int w, int h);
    int writePixels(uint16_t *pixels, int count, uint8_t bDMA);
    int dumpBuffer(uint8_t * buffer);
    int drawRect(int x, int y, int w, int h, uint16_t u16Color, int bFilled);
    int drawBitmap(int x, int y, int bitmapIndex, int stretch);
    int drawIcon(int x, int y, int iconIndex, int angle, uint16_t u16FGColor, uint16_t u16BGColor);
    int drawText(int x, int y, char *szText, uint8_t u8Font, uint16_t u16FGColor, uint16_t u16BGColor);
    int drawEllipse(int x, int y, int r1, int r2, uint16_t u16Color, int bFilled);
    int setFont(const GFXfont *pFont, int fontIndex);
    int setBitmap(uint8_t bitmapIndex, const uint8_t *pBitmap, int iBitmapSize);
    int setOrientation(int angle);
    uint16_t getButtons();
private:
    int BLESendVarData(uint16_t *data, int count, void *varData);
    int BLESend(uint16_t *data, int count);
    uint16_t BLEReceive();
}; // class BLEDisplay

