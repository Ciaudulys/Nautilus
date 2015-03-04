#ifndef ADS1298R_HW_H
#define ADS1298R_HW_H

#include <stdint.h>

#define ADS129X_CS_BASE GPIO_PORTB_BASE
#define ADS129X_CS_PIN GPIO_PIN_0
#define ADS129X_CS_lo GPIOPinWrite(ADS129X_CS_BASE,ADS129X_CS_PIN,0x00)
#define ADS129X_CS_hi GPIOPinWrite(ADS129X_CS_BASE,ADS129X_CS_PIN,0xFF)

#define ADS129X_DRDY_BASE 	GPIO_PORTB_BASE
#define ADS129X_DRDY_PIN		GPIO_PIN_3

void ADS129X_hw_init(void);
void ADS129X_SPI_init(uint32_t speed);
void ADS129X_DelayUs(uint32_t n);
uint8_t ADS129X_transferB(uint8_t data);

#endif
