//
// Serial connected (remote) display
//
class UARTDisplay : public RemoteDisplay
{
public:
    UARTDisplay() : RemoteDisplay() {}
    ~UARTDisplay() {}
    int begin(uint16_t u16DisplayType);
    void shutdown();
    int fill(uint16_t u16Color);
    int drawLine(int x1, int y1, int x2, int y2, uint16_t u16Color);
    int drawPixel(int x, int y, uint16_t u16Color);
    int setWindow(int x, int y, int w, int h);
    int writePixels(uint16_t *pixels, int count, uint8_t bDMA);
    int drawRect(int x, int y, int w, int h, uint16_t u16Color, int bFilled);
    int drawText(int x, int y, char *szText, uint8_t u8Font, uint16_t u16FGColor, uint16_t u16BGColor);
    int drawEllipse(int x, int y, int r1, int r2, uint16_t u16Color, int bFilled);
    int setOrientation(int angle);
    int dumpBuffer(uint8_t * buffer);
    uint16_t getButtons();
private:
    int UARTSendVarData(uint16_t *data, int count, void *varData);
    int UARTSend(uint16_t *data, int count);

}; // class UARTDisplay

