#include "key.h"

key::key(/* args */) {
}

key::~key() {
}

void key::initIdx(unsigned char idx) {
    index = idx;
}

void key::exeKey(bool trigger) {

    if (trigger) {
        // press

        switch (exeMode) {
        case 0:
        case 2:
            exeKeyPress(exeMode == 0);
            break;
        case 1:
            exeKeyPrintln();
            break;

        default:
            break;
        }
    } else {
        // release
        if (exeMode == 2) {
            exeKeyRelease();
        }
    }
};

void key::exeKeyPrintln() {
    Keyboard.println(printStr);
};

void key::exeKeyPress(bool once) {
    // Serial.print(funcKey);
    for (int x = 0; x < 3; x++) {
        Serial.println(funcKey[x]);
        // TODO: check the keybind is '0xFF'
        if (funcKey[x] != 0xFF) {
            Keyboard.press(funcKey[x]);
        }
    }
    if (once) {
        Keyboard.releaseAll();
    }
};

void key::exeKeyRelease() {
    for (int x = 0; x < 3; x++) {
        Serial.println(funcKey[x]);
        // TODO: check the keybind is '0xFF'
        if (funcKey[x] != 0xFF) {
            Keyboard.release(funcKey[x]);
        }
    }
};
