#ifndef NODEMCU_H
#define NODEMCU_H

//NodeMCU pin mapping
const uint8_t D0   = 16;
const uint8_t D1   = 5;
const uint8_t D2   = 4;
const uint8_t D3   = 0;
const uint8_t D4   = 2;
const uint8_t D5   = 14;
const uint8_t D6   = 12;
const uint8_t D7   = 13;
const uint8_t D8   = 15;
const uint8_t D9   = 3;
const uint8_t D10  = 1;
const uint8_t LED_BUILTIN = 16;

//MUDANDO OS NOMES DOS PINOS
const int LED_PIN = LED_BUILTIN; // the number of the LED pin (digital)
const int ECG_PIN = A0;          // the number of the ECG pin (analog)  => A0
const int PWM_PIN = D10;         // the number of the PWM pin (analog)  => D10

// pins for leads off detection
const int LEADS_OFF_PLUS_PIN = D1;  // the number of the LO+ pin (digital) => D1
const int LEADS_OFF_MINUS_PIN = D2; // the number of the LO- pin (digital) => D2

#endif