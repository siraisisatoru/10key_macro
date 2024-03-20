#ifndef preset_h
#define preset_h

// pin definition
// rotary encoder (rencoder)
#define rencoderP1 0
#define rencoderP2 1
// 4 switch
#define TswithA 8
#define TswithB 7
#define TswithC 6
#define TswithD 5

// button 1 2
#define key2Pin 14
#define key1Pin 15
// button 3 10 165pin
#define keyDS A0
#define keyCLK A1
#define keySH A2
#define keyCLK_inh A3
// ^^ end pin definition

// LED
#define outputPin 10  // Digital output pin (default: 7)
#define LEDCount 10   // Number of LEDs to drive (default: 9)
#define brightness 20 // unit %

#define frameDuration 10
#define overflowUpper 3072

#endif