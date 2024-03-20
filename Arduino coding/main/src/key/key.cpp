#include "key.h"

key::key(/* args */) {
}

key::~key() {
}

void key::initIdx(unsigned char idx) {
    index = idx;
}

void key::exeKey() {
    if (exeMode) {
        //  true : print
        exeKeyPrintln();
    } else {
        //  false : press
        exeKeyPress();
    }
};

void key::exeKeyPrintln() {
    Keyboard.println(printStr);
};

void key::exeKeyPress() {
    // Serial.print(funcKey);
    for (int x = 0; x < 3; x++) {
        Serial.println(funcKey[x]);
        // TODO: check the keybind is '0xFF'
        if (funcKey[x] != 0xFF) {
            Keyboard.press(funcKey[x]);
        }
    }
    Keyboard.releaseAll();
};
