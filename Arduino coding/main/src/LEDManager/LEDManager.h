#ifndef LEDMANAGER_H
#define LEDMANAGER_H
#include "../WS2812/WS2812.h"
#include "../WS2812/cRGB.h"
#include "../panel/preset.h"
#include "Arduino.h"

class LEDManager {
  private:
    WS2812 LED;
    cRGB value;
    unsigned long timebuffer = 0;
    unsigned short step = 0;

    // Need same size 1/3
    const unsigned char illSize = 8;
    // Need same size 2/3
    const float durationFactor[8] = {20, 1, 1, 1, 1, 5, 5, 1};
    const unsigned char LEDsequence[LEDCount] = {1, 0, 2, 6, 7, 8, 9, 5, 4, 3};
    /**
     *              1   0
     *      5   4   3   2
     *      9   8   7   6
     */

    // const unsigned char manualModeSequence[10] = {0, 0, 0, 0, 0, 0, 1, 2, 3, 4};

    unsigned short updateIdx = 0; // a temporary storage for LED index
    unsigned short updateI = 0;   // a temporary storage for dummy variable I
    unsigned short updateJ = 0;   // a temporary storage for dummy variable J
    unsigned short updateK = 0;   // a temporary storage for dummy variable K
    cRGB updateValue;             // a temporary storage for dummy RGB value

    unsigned short fadeSteps = 100;
    float fadeBrightnessFactor = 100.f / fadeSteps;
    bool fadeIObool = false;

    byte illIdx = 0;
    bool manual = false;

  public:
    typedef void (LEDManager::*FunctionPtr)();

    // Need same size 3/3
    FunctionPtr illuminationList[8];


    LEDManager();
    ~LEDManager();

    void cRGBWrap(uint8_t r, uint8_t g, uint8_t b, cRGB &value);
    void cRGBWrap(uint8_t r, uint8_t g, uint8_t b, cRGB &value, uint8_t locBrightness);

    cRGB Wheel(byte WheelPos);

    void valueLEDs(bool update = false);
    void clearLEDs();

    void updateFrame(byte mode = 0);

    void resetCounters();

    void colorWipeFrame();
    void rainbowFrame();
    void rainbowCycleFrame();
    void rainbowCycleRingGraduallyFrame();
    void rainbowCycleRingFrame();
    void theaterChaseFrame();
    void theaterChaseRainbowFrame();
    void fadeFrame();

    void LEDloop();
    void errorLoop();
};

#endif