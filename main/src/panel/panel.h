#ifndef PANEL_H
#define PANEL_H
#include "../LEDManager/LEDManager.h"
#include "../RotaryEncoder/RotaryEncoder.h"
#include "../key/key.h"
#include "Arduino.h"
#include "Keyboard.h"
#include "preset.h"

class panel {
  private:
    byte sw = 0;
    byte nsw = 0;
    unsigned int preData = 0;
    unsigned int newData = 0;
    int pos = 0;
    int newPos = 0;
    RotaryEncoder *encoder = nullptr;
    key keys[LEDCount];
    LEDManager LEDMaster;
    const byte lookup[8] = {6, 7, 8, 9, 5, 4, 3, 2};

    /** from
     *              9   8
     *      4   5   6   7
     *      3   2   1   0
     *  to
     *              1   0
     *      5   4   3   2
     *      9   8   7   6
     */

  public:
    // Static member variables for interrupt handling
    static panel *instance; // Pointer to an instance of panel
    static void isr();      // Static member function acting as ISR

    panel(/* args */);
    ~panel();

    void beginKeyboard() {
        Keyboard.begin();
    }

    void endKeyboard() {
        Keyboard.end();
    }

    void init(String keyConfig[10]);
    void updateKeys(String keyConfig[10]);

    byte getState() {
        read_sw();
        return nsw;
    }

    void read_sw();
    void read_keys();

    void monitorKeys();

    void checkPosition();
    void update_encoder();

    // Attach interrupt method
    void attachInterruptToEncoder(int pin) {
        ::attachInterrupt(digitalPinToInterrupt(pin), isr, CHANGE);
    }

    // manage LEDs
    void wellcomeLoop() {
        LEDMaster.LEDloop();
    }

    void errorLoop(){
        LEDMaster.errorLoop();
    }

    void LEDShowTick(byte state) {
        LEDMaster.updateFrame(state);
    }
};

#endif