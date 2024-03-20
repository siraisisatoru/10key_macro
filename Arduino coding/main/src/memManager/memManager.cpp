#include "memManager.h"

memManager::memManager(/* args */) {
    Wire.begin();
}

memManager::~memManager() {
}

void memManager::init(byte mode = 1) {
    size = readEEPROM(sizeAddr);
    loadKeyConfig(mode);
}

void memManager::loadKeyConfig(byte mode = 1) {
    address = size * (mode - 1) * 10;
    for (byte i = 0; i < 10; i++) {
        if (mode >= 0 && mode <= 8) {
            keyConfig[i] = getStr(address + i * size);

        } else {
            keyConfig[i] = "";
        }
    }
}

void memManager::printEEPROM() {
    for (int i = 0; i < upperAddr; i++) {
        Serial.print(char(readEEPROM(i, true)));

        if ((i + 1) % size == 0) {
            Serial.print('\n');
        }
    }
}

void memManager::flushEEPROM() {
    for (int i = 0; i < upperAddr; i++) {
        if (i % 300 == 0) {
            Serial.println(i);
        }
        writeEEPROM(i, 0xFF);
    }
}

void memManager::writeEEPROM(unsigned int eeaddress, String data) {
    for (int i = 0; i < data.length(); i++) {
        writeEEPROM(eeaddress + i, byte(data.charAt(i) & 0xFF));
        Serial.print(byte(data.charAt(i) & 0xFF), HEX);
        Serial.print(" ");
    }
    Serial.println();
}

void memManager::writeEEPROM(unsigned int eeaddress, byte data) {
    Wire.beginTransmission(disk1);
    Wire.write((int)(eeaddress >> 8));   // MSB
    Wire.write((int)(eeaddress & 0xFF)); // LSB
    Wire.write(data);
    Wire.endTransmission();

    delay(5);
}

String memManager::getStr(unsigned int eeaddress) {
    String str = "";
    for (byte i = 0; i < size; i++) {
        str += char(readEEPROM(eeaddress + i, false));
    }
    return str;
}

byte memManager::readEEPROM(unsigned int eeaddress, bool filter = false) {
    byte rdata = 0xFF;

    Wire.beginTransmission(disk1);
    Wire.write((int)(eeaddress >> 8));   // MSB
    Wire.write((int)(eeaddress & 0xFF)); // LSB
    Wire.endTransmission();

    Wire.requestFrom(disk1, 1);

    if (Wire.available())
        rdata = Wire.read();

    if (filter) {
        return isAscii(rdata) ? rdata : rdata == 0xFF ? '-'
                                                      : '?';
    } else {
        return rdata;
    }
}
