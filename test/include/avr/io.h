#pragma once

#include <math.h>
#include <stdint.h>

#define PROGMEM

#define strcmp_P strcmp

typedef unsigned int UINT;

extern uint8_t TCCR1A;
extern uint8_t TCCR1B;
extern uint8_t TCCR3B;

extern uint8_t TIMSK1;
extern uint8_t TIMSK3;
#define TOIE1 0
#define OCIE3A 0

extern uint8_t TCNT1;
#define COM1A1 0
#define COM1A0 0
#define COM1B1 0
#define WGM12 0
#define WGM32 0
#define WGM10 0
#define CS10 0
#define CS31 0

extern uint8_t OCR1A;
extern uint8_t OCR1B;
extern uint16_t OCR3A;

extern uint8_t DDRB;
extern uint8_t DDRC;

extern uint8_t PORTC;
