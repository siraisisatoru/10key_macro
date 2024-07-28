#ifndef KEY_H
#define KEY_H
#include "Keyboard.h"
#include <Arduino.h>

class key {
  private:
    /* data */
    unsigned char index = 255; // default index 255
    String printStr = "";      // default string ""
    char funcKey[3];

    unsigned char exeMode = 0;

  public:
    key(/* args */);
    ~key();

    void initIdx(unsigned char idx);

    void setPrintStr(String str) {
        printStr = str;
    };

    void setFuncKey(char keyList[]) {
        for (char i = 0; i < 3; i++) {
            funcKey[i] = keyList[i];
        }
    };

    void setMode(unsigned char mode) { exeMode = mode; }

    void exeKey(bool trigger);

    void exeKeyPrintln();
    void exeKeyPress(bool once);
    void exeKeyRelease();
};

#endif