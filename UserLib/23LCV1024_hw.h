#ifndef RAM_hw_H
#define RAM_hw_H

#include "stdint.h"

#define RAM_CS_BASE GPIO_PORTA_BASE
#define RAM_CS_PIN GPIO_PIN_0
#define RAM_CS_lo GPIOPinWrite(RAM_CS_BASE,RAM_CS_PIN,0x00)
#define RAM_CS_hi GPIOPinWrite(RAM_CS_BASE,RAM_CS_PIN,0xFF)

void RAM_hw_init(void);
uint8_t RAM_transferB(uint8_t);


















#endif
