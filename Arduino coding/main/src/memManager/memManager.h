#ifndef memManager_h
#define memManager_h
#include "Arduino.h"
#include <Wire.h>

#define disk1 0x50     // Address of 24LC256 eeprom chip
#define upperAddr 8192 // max address
#define sizeAddr 8190

class memManager {
  private:
    /* data */
    unsigned int address = 0;
    // need to change this assignment to loading the size from EEPROM
    unsigned char size;

    String keyConfig[10];

  public:
    memManager(/* args */);
    ~memManager();

    void init(byte mode = 1);
    void loadKeyConfig(byte mode = 1);

    String *getKeyConfig() {
        return keyConfig;
    }

    String getStr(unsigned int eeaddress);

    void setSize(unsigned char size) {
        // need to change this assignment to loading the size from EEPROM
        size = size;
        writeEEPROM(sizeAddr, size);
    }
    
    void flushEEPROM();

    void writeEEPROM(unsigned int eeaddress, String data);
    void writeEEPROM(unsigned int eeaddress, byte data);
    byte readEEPROM(unsigned int eeaddress, bool filter = false);
    void printEEPROM();

};

#endif