#ifndef ENCODER_H
#define ENCODER_H
#include <Arduino.h>
#include <Mouse.h>

class encoder {
  private:
    /* data */
  public:
    encoder(/* args */);
    ~encoder();

    void scroll(signed char val) {
        // Serial.println(up ? "up" : "down");
        Mouse.move(0, 0, val);
    };
};

#endif