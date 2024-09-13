#ifndef _DELAY_H
#define _DELAY_H
#include "main.h"

 
extern uint8_t CLOCKFREQ;
extern double CyclePer;



void delay_init(uint8_t clockfreq);
void delay_us(uint16_t us);
void delay_ms(uint16_t ms);
void delay_ns1(uint16_t ns);


#endif





