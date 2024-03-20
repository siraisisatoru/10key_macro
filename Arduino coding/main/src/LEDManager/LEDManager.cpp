
#include "LEDManager.h"

LEDManager::LEDManager() : LED(LEDCount) {
    LED.setOutput(outputPin);
    timebuffer = millis();

    // Assign functions to the array
    illuminationList[0] = &LEDManager::colorWipeFrame;
    illuminationList[1] = &LEDManager::rainbowFrame;                   // whole panel rainbow same color
    illuminationList[2] = &LEDManager::rainbowCycleFrame;              // whole panel rainbow spread wheel to all keys
    illuminationList[3] = &LEDManager::rainbowCycleRingGraduallyFrame; // whole panel rainbow spread wheel to all keys
    illuminationList[4] = &LEDManager::rainbowCycleRingFrame;          // whole panel rainbow spread wheel to all keys
    illuminationList[5] = &LEDManager::theaterChaseFrame;
    illuminationList[6] = &LEDManager::theaterChaseRainbowFrame;
    illuminationList[7] = &LEDManager::fadeFrame;
};

LEDManager::~LEDManager() {
}

void LEDManager::cRGBWrap(uint8_t r, uint8_t g, uint8_t b, cRGB &newValue) {
    newValue.r = r * brightness / 100;
    newValue.g = g * brightness / 100;
    newValue.b = b * brightness / 100;
}

void LEDManager::cRGBWrap(uint8_t r, uint8_t g, uint8_t b, cRGB &newValue, uint8_t locBrightness) {
    newValue.r = r * locBrightness / 100;
    newValue.g = g * locBrightness / 100;
    newValue.b = b * locBrightness / 100;
}

cRGB LEDManager::Wheel(byte WheelPos) {
    // Input a value 0 to 255 to get a color value.
    // The colours are a transition r - g - b - back to r.
    cRGB tempValue;

    WheelPos = 255 - WheelPos;
    if (WheelPos < 85) {
        cRGBWrap(255 - WheelPos * 3, 0, WheelPos * 3, tempValue); // assign color to 'value'

        return tempValue;
    }
    if (WheelPos < 170) {
        WheelPos -= 85;
        cRGBWrap(0, WheelPos * 3, 255 - WheelPos * 3, tempValue);
        return tempValue;
    }
    WheelPos -= 170;
    cRGBWrap(WheelPos * 3, 255 - WheelPos * 3, 0, tempValue);
    return tempValue;
}

void LEDManager::valueLEDs(bool update = false) {
    // !!    Drect flush LEDs with color stored in value   !!
    // !! Need to update value prior calling this function !!
    for (byte i = 0; i < LEDCount; i++) {
        LED.set_crgb_at(i, update ? updateValue : value);
    }
    LED.sync(); // Sends the data to the LEDs
}

void LEDManager::clearLEDs() {
    // Fill the dots one after the other with a color
    cRGBWrap(0, 0, 0, value); // assign color to 'value'
    valueLEDs();
}

void LEDManager::updateFrame(byte mode = 0) {

    switch (mode) {
    case 1:
        /* code */
        // break;
    case 2:
        /* code */
        // break;
    case 0:
    default:
        if (illIdx >= 0 && illIdx < illSize && illuminationList[illIdx] != nullptr) {
            (this->*illuminationList[illIdx])(); // Call function using pointer to member function
        }
        break;
    }
}

void LEDManager::resetCounters() {
    illIdx = (illIdx + 1) % illSize;
    step = 0;
    updateIdx = 0;
    updateI = 0;
    updateJ = 0;
    updateK = 0;
    manual = false;
    fadeIObool = false;
    fadeSteps = 100;
    fadeBrightnessFactor = 100.f / fadeSteps;
}

void LEDManager::colorWipeFrame() {
    // Check time duration
    if ((millis() - timebuffer) < (frameDuration * durationFactor[illIdx])) {
        return;
    }
    timebuffer = millis();
    // Execute function

    if (!manual) {
        cRGBWrap(255, 0, 0, value); // assign color to 'value'
    }

    LED.set_crgb_at(updateIdx, value);
    LED.sync(); // Sends the data to the LEDs

    updateIdx++;
    bool finish = updateIdx > LEDCount;
    if (finish) {
        clearLEDs();
    }

    // Update counters
    step++;
    if ((step > overflowUpper) || finish) {
        // counter overflow
        resetCounters();
    }
};

void LEDManager::rainbowFrame() {
    // Check time duration
    if ((millis() - timebuffer) < (frameDuration * durationFactor[illIdx])) {
        return;
    }
    timebuffer = millis();
    // Execute function

    for (byte i = 0; i < LEDCount; i++) {
        LED.set_crgb_at(i, Wheel((i + updateJ) & 255));
    }
    LED.sync(); // Sends the data to the LEDs

    updateJ++;

    bool finish = updateJ >= 255;
    if (finish) {
        clearLEDs();
    }

    // Update counters
    step++;
    if ((step > overflowUpper) || finish) {
        // counter overflow
        resetCounters();
    }
};

void LEDManager::rainbowCycleFrame() {
    // Check time duration
    if ((millis() - timebuffer) < (frameDuration * durationFactor[illIdx])) {
        return;
    }
    timebuffer = millis();
    // Execute function

    for (byte i = 0; i < LEDCount; i++) {
        LED.set_crgb_at(i, Wheel(((i * 256 / LEDCount) + updateJ) & 255));
    }
    LED.sync(); // Sends the data to the LEDs

    updateJ++;

    bool finish = updateJ >= 256 * 3;
    if (finish) {
        clearLEDs();
    }

    // Update counters
    step++;
    if ((step > overflowUpper) || finish) {
        // counter overflow
        resetCounters();
    }
};

void LEDManager::rainbowCycleRingGraduallyFrame() {
    // Check time duration
    if ((millis() - timebuffer) < (frameDuration * durationFactor[illIdx])) {
        return;
    }
    timebuffer = millis();
    // Execute function

    for (byte i = 0; i <= updateI; i++) {
        LED.set_crgb_at(LEDsequence[i], Wheel(((i * 256 / LEDCount) + updateJ) & 255));
    }
    LED.sync(); // Sends the data to the LEDs
    updateJ--;

    if (updateJ % 50 == 0) {
        updateI = (updateI + 1) % LEDCount;
        if (updateI == 0) {
            clearLEDs();
            updateK++;
        }
    }

    bool finish = updateK == 1; // modify this to loop more than 1 times
    if (finish) {
        clearLEDs();
    }

    // Update counters
    step++;
    if ((step > overflowUpper) || finish) {
        // counter overflow
        resetCounters();
    }
};

void LEDManager::rainbowCycleRingFrame() {
    // Check time duration
    if ((millis() - timebuffer) < (frameDuration * durationFactor[illIdx])) {
        return;
    }
    timebuffer = millis();
    // Execute function

    for (byte i = 0; i < LEDCount; i++) {
        LED.set_crgb_at(LEDsequence[i], Wheel(((i * 256 / LEDCount) + updateJ) & 255));
    }
    LED.sync(); // Sends the data to the LEDs
    updateJ++;

    bool finish = updateJ > (256 * 3); // 65535 - 256 * 10 = 62975 (3 full loops)
    if (finish) {
        clearLEDs();
    }

    // Update counters
    step++;
    if ((step > overflowUpper) || finish) {
        // counter overflow
        resetCounters();
    }
};

void LEDManager::theaterChaseFrame() {
    // Check time duration
    if ((millis() - timebuffer) < (frameDuration * durationFactor[illIdx])) {
        return;
    }
    timebuffer = millis();
    // Execute function

    // clearLEDs();

    if (!manual) {
        cRGBWrap(255, 0, 0, value); // assign color to 'value'
    }

    for (byte i = 0; i < LEDCount; i = i + 3) {
        LED.set_crgb_at(i + updateI, value);
    }
    LED.sync(); // Sends the data to the LEDs

    cRGBWrap(0, 0, 0, value); // assign color to 'value'
    for (byte i = 0; i < LEDCount; i = i + 3) {
        LED.set_crgb_at(i + updateI, value);
    }

    updateI = (updateI + 1) % 3;
    // Serial.println(updateI);
    if (updateI == 0) {
        updateJ++;
    }

    bool finish = updateJ >= 10;
    if (finish) {
        clearLEDs();
    }

    // Update counters
    step++;
    if ((step > overflowUpper) || finish) {
        // counter overflow
        resetCounters();
    }
};

void LEDManager::theaterChaseRainbowFrame() {
    // Check time duration
    if ((millis() - timebuffer) < (frameDuration * durationFactor[illIdx])) {
        return;
    }
    timebuffer = millis();
    // Execute function

    // clearLEDs();

    for (byte i = 0; i < LEDCount; i = i + 3) {
        LED.set_crgb_at(i + updateI, Wheel((((i + updateI) * 256 / LEDCount) + updateJ) & 255));
    }
    LED.sync(); // Sends the data to the LEDs

    cRGBWrap(0, 0, 0, value); // assign color to 'value'
    for (byte i = 0; i < LEDCount; i = i + 3) {
        LED.set_crgb_at(i + updateI, value);
    }

    updateI = (updateI + 1) % 3;
    // Serial.println(updateI);
    if (updateI == 0) {
        updateJ += 10;
    }

    bool finish = updateJ >= 256;
    if (finish) {
        clearLEDs();
    }

    // Update counters
    step++;
    if ((step > overflowUpper) || finish) {
        // counter overflow
        resetCounters();
    }
};

void LEDManager::fadeFrame() {

    // Check time duration
    if ((millis() - timebuffer) < (frameDuration * durationFactor[7])) {
        return;
    }
    timebuffer = millis();
    // Execute function

    if (!manual) {
        cRGBWrap(255, 255, 0, value); // assign color to 'value'
    }

    /**
     * Presets
     * 1. value                     {undefine}
     * 2. fadeIObool                {false}         [True: I, false: O]
     * 3. fadeSteps(opt)            {100}
     * 4. fadeBrightnessFactor(opt) {100/fadeSteps}
     * ** configure the speed of one loop in `durationFactor`
     */

    float locBrightness = (fadeIObool ? updateI : (fadeSteps - updateI)) * fadeBrightnessFactor;
    // Serial.print(locBrightness );
    // Serial.println(fadeIObool );

    cRGBWrap(value.r, value.g, value.b, updateValue, uint8_t(locBrightness));
    valueLEDs(true);

    updateI = updateI += 1;

    bool finish = updateI == fadeSteps;
    // if (finish) {
    //     clearLEDs();
    // }

    // Update counters
    step++;
    if ((step > overflowUpper) || finish) {
        // counter overflow
        resetCounters();
    }
}

void LEDManager::LEDloop() {

    clearLEDs();

    resetCounters();
    illIdx = 3;
    while (1) {
        rainbowCycleRingGraduallyFrame();
        if (illIdx != 3) {
            break;
        }
    }

    // fade in
    resetCounters();
    cRGBWrap(255, 255, 255, value); // assign color to 'value'
    manual = true;
    fadeIObool = true;
    illIdx = 7;
    while (1) {
        fadeFrame();
        if (illIdx != 7) {
            break;
        }
    }

    // fade out
    cRGBWrap(255, 255, 255, value); // assign color to 'value'
    manual = true;
    fadeIObool = false;
    illIdx = 7;
    while (1) {
        fadeFrame();
        if (illIdx != 7) {
            break;
        }
    }
}

void LEDManager::errorLoop() {
    cRGBWrap(255, 0, 0, value); // assign color to 'value'
    manual = true;
    fadeIObool = true;
    fadeSteps = 50;
    illIdx = 7;
    while (1) {
        fadeFrame();
        if (illIdx != 7) {
            break;
        }
    }
    cRGBWrap(255, 0, 0, value); // assign color to 'value'
    manual = true;
    fadeIObool = false;
    fadeSteps = 50;
    illIdx = 7;
    while (1) {
        fadeFrame();
        if (illIdx != 7) {
            break;
        }
    }

    cRGBWrap(255, 0, 255, value); // assign color to 'value'
    manual = true;
    fadeIObool = true;
    fadeSteps = 50;
    illIdx = 7;
    while (1) {
        fadeFrame();
        if (illIdx != 7) {
            break;
        }
    }
    cRGBWrap(255, 0, 255, value); // assign color to 'value'
    manual = true;
    fadeIObool = false;
    fadeSteps = 50;
    illIdx = 7;
    while (1) {
        fadeFrame();
        if (illIdx != 7) {
            break;
        }
    }

    cRGBWrap(255, 0, 0, value); // assign color to 'value'
    manual = true;
    fadeIObool = true;
    fadeSteps = 50;
    illIdx = 7;
    while (1) {
        fadeFrame();
        if (illIdx != 7) {
            break;
        }
    }
    cRGBWrap(255, 0, 0, value); // assign color to 'value'
    manual = true;
    fadeIObool = false;
    fadeSteps = 50;
    illIdx = 7;
    while (1) {
        fadeFrame();
        if (illIdx != 7) {
            break;
        }
    }

    cRGBWrap(255, 0, 255, value); // assign color to 'value'
    manual = true;
    fadeIObool = true;
    fadeSteps = 50;
    illIdx = 7;
    while (1) {
        fadeFrame();
        if (illIdx != 7) {
            break;
        }
    }
    cRGBWrap(255, 0, 255, value); // assign color to 'value'
    manual = true;
    fadeIObool = false;
    fadeSteps = 50;
    illIdx = 7;
    while (1) {
        fadeFrame();
        if (illIdx != 7) {
            break;
        }
    }
}

/* Frame template
    // Check time duration
    if ((millis() - timebuffer) < (frameDuration * durationFactor[illIdx])) {
        return;
    }
    timebuffer = millis();
    // Execute function

    if (!manual) {
        cRGBWrap(255, 0, 0, value); // assign color to 'value'
    }

    updateI = (updateI + 1) % 3;
    // Serial.println(updateI);
    if (updateI == 0) {
        updateJ++;
    }

    bool finish = updateJ >= 10;
    if (finish) {
        clearLEDs();
    }

    // Update counters
    step++;
    if ((step > overflowUpper) || finish) {
        // counter overflow
        resetCounters();
    }
*/
