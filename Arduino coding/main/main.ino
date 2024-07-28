#include "Arduino.h"
#include "src/memManager/memManager.h"
#include "src/panel/panel.h"

panel panelMaster;    // declare control panel master
memManager memMaster; // declare memory control panel master
byte state;

void setup() {

    Serial.begin(115200);

    // init panel
    state = 0b1111 - panelMaster.getState();
    memMaster.init(state);
    panelMaster.init(memMaster.getKeyConfig());
    panelMaster.beginKeyboard();
    panelMaster.beginMouse();
    panelMaster.attachInterruptToEncoder(rencoderP1);
    panelMaster.attachInterruptToEncoder(rencoderP2);

    unsigned long now = millis();
    while (!Serial) {
        if (millis() - now > 3000) {
            panelMaster.errorLoop();
            break;
        }
    }

    panelMaster.wellcomeLoop();
}

void loop() {

    switch (state) {
    case 0:
        panelMaster.LEDShowTick(state);
        break;
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
        // Serial.println("Ordinary");
        panelMaster.monitorKeys();
        panelMaster.LEDShowTick(state);
        break;

    case 13:

        Serial.println("Flushing EEPROM...");
        Serial.println("!! You are about to erase the whole EEPROM !!");
        Serial.println("!!!!!!! This process is irreversible !!!!!!!!");
        // Uncomment thie following line to enable this feature.

        // memMaster.flushEEPROM();
        Serial.println("Exit flushing EEPROM.\n\tFlip the switch to other states.");

        while ((0b1111 - panelMaster.getState()) == 13) {
            /* can add LED effect */
            delay(5);
        }
        break;
    case 14:
        // if (!Serial) {
        //     break;
        // }
        Serial.println("Print EEPROM...");
        memMaster.printEEPROM();
        Serial.println("Exit print EEPROM.\n\tFlip the switch to other states.");

        while ((0b1111 - panelMaster.getState()) == 14) {
            /* can add LED effect */
            delay(5);
        }
        break;
    case 15:
        // if (!Serial) {
        //     break;
        // }
        Serial.println("Begin load json from PC...");
        unsigned long now = millis();

        unsigned int addr = 0;
        unsigned char size = 0;

        while (1) {
            if ((millis() - now > 10000) && (addr == 0)) {
                break;
            }
            if ((millis() - now > 60000) && (addr != 0)) {
                Serial.println("Fail loadinding within 1 min.\nExist for timeout.");
                break;
            }

            String receiveData = Serial.readStringUntil('\n');

            for (int i = 0; i < receiveData.length(); i++) {
                receiveData.setCharAt(i, byte(receiveData.charAt(i) & 0xFF));
                Serial.print(byte(receiveData.charAt(i) & 0xFF), HEX);
                Serial.print(" ");
            }
            Serial.print(receiveData.length());
            Serial.println();

            if (receiveData) {
                if (receiveData == "done!")
                    break;
                if (receiveData.indexOf("Addr") > -1) {
                    addr = receiveData.substring(6).toInt(); // Skip "Addr: "
                    continue;
                } else if (receiveData.indexOf("Size") > -1) {
                    size = receiveData.substring(6).toInt(); // Skip "Size: "
                    memMaster.setSize(size);
                    continue;
                } else {
                    memMaster.writeEEPROM(addr, receiveData);
                    addr += size;
                }
            }
        }

        Serial.println("Exit load json from PC.\n\tFlip the switch to other states.");
        while ((0b1111 - panelMaster.getState()) == 15) {
            /* can add LED effect */
            delay(5);
        }
        break;
    default:
        break;
    }

    if (state != (0b1111 - panelMaster.getState())) {
        state = 0b1111 - panelMaster.getState();
        memMaster.loadKeyConfig(state);
        panelMaster.updateKeys(memMaster.getKeyConfig());
    }
}
