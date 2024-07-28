#include "panel.h"

// Initialize static member variable
panel *panel::instance = nullptr;

// Static member function definition
void panel::isr() {
    // Call non-static member function through an instance
    instance->checkPosition();
}

panel::panel(/* args */) {
    pinMode(keyCLK, OUTPUT);
    pinMode(keySH, OUTPUT);
    pinMode(keyDS, INPUT);
    pinMode(keyCLK_inh, OUTPUT);
    pinMode(key2Pin, INPUT_PULLUP);
    pinMode(key1Pin, INPUT_PULLUP);

    pinMode(TswithA, INPUT_PULLUP);
    pinMode(TswithB, INPUT_PULLUP);
    pinMode(TswithC, INPUT_PULLUP);
    pinMode(TswithD, INPUT_PULLUP);

    encoder = new RotaryEncoder(rencoderP1, rencoderP2, RotaryEncoder::LatchMode::TWO03);
}

panel::~panel() {
}

void panel::init(String keyConfig[10]) {
    for (unsigned char i = 0; i < LEDCount; i++) {

        keys[i].initIdx(i);
    }
    updateKeys(keyConfig);
}

void panel::updateKeys(String keyConfig[10]) {
    char configList[3];
    Keyboard.releaseAll();
    for (unsigned char i = 0; i < LEDCount; i++) {
        configList[0] = keyConfig[i].charAt(1);
        configList[1] = keyConfig[i].charAt(2);
        configList[2] = keyConfig[i].charAt(3);
        keys[i].setFuncKey(configList);
        keys[i].setPrintStr(keyConfig[i].substring(4));
        keys[i].setMode(keyConfig[i].charAt(0) - '0');
        // keys[i].exeKeyPrintln();
        // keys[i].exeKeyPress();
    }
}

void panel::read_sw() {
    bitWrite(nsw, 3, digitalRead(TswithA));
    bitWrite(nsw, 2, digitalRead(TswithB));
    bitWrite(nsw, 1, digitalRead(TswithC));
    bitWrite(nsw, 0, digitalRead(TswithD));
}

void panel::read_keys() {
    digitalRead(key2Pin) == 0 ? bitWrite(newData, 0, 1) : bitWrite(newData, 0, 0);
    digitalRead(key1Pin) == 0 ? bitWrite(newData, 1, 1) : bitWrite(newData, 1, 0);
    digitalWrite(keyCLK_inh, HIGH);
    digitalWrite(keySH, LOW);
    digitalWrite(keyCLK, LOW);
    digitalWrite(keyCLK, HIGH);
    digitalWrite(keySH, HIGH);
    digitalWrite(keyCLK, LOW);
    digitalWrite(keyCLK, HIGH);
    digitalWrite(keyCLK_inh, LOW);
    for (int i = 0; i < 8; i++) {
        digitalWrite(keyCLK, LOW);
        // digitalRead(keyDS) == 1 ? bitWrite(newData, i, 1)
        //                         : bitWrite(newData, i, 0);
        bitWrite(newData, lookup[i], digitalRead(keyDS));

        digitalWrite(keyCLK, HIGH);
    }
}

void panel::monitorKeys() {

    // backup the old data
    preData = newData;
    pos = newPos;
    // load/check new data
    read_keys();
    read_sw();
    update_encoder();

    if (preData != newData) {
        Serial.println(newData, BIN);
        for (int i = 0; i < LEDCount; i++) {
            if ((newData & (1 << i)) && !(preData & (1 << i))) {
                keys[i].exeKey(true);
            } else if (!(newData & (1 << i)) && (preData & (1 << i))) {
                keys[i].exeKeyRelease();
            }
        }
    }
    if (pos != newPos) {
        Serial.print("pos:");
        Serial.print(newPos);
        Serial.print(" dir:");
        Serial.println((int)(encoder->getDirection()));
        pos = newPos;
    }
    if (sw != nsw) {
        Serial.println(nsw, BIN);
        sw = nsw;
    }
}

void panel::checkPosition() {
    encoder->tick(); // just call tick() to check the state.
}

void panel::update_encoder() {
    encoder->tick(); // just call tick() to check the state.
    newPos = encoder->getPosition();
}
