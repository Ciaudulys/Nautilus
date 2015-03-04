#ifndef AFE4490_TM4C123GH6PMI_H
#define AFE4490_TM4C123GH6PMI_H

#include "stdint.h"
#include "stdbool.h"
#include "driverlib\gpio.h"


//AFE4490 A
#define AFE4490A_STE_BASE GPIO_PORTE_BASE
#define AFE4490A_STE_PIN GPIO_PIN_5
#define AFE4490A_STE_lo GPIOPinWrite(AFE4490A_STE_BASE,AFE4490A_STE_PIN,0x00)
#define AFE4490A_STE_hi GPIOPinWrite(AFE4490A_STE_BASE,AFE4490A_STE_PIN,0xFF)

#define AFE4490A_ADC_RDY_BASE GPIO_PORTC_BASE
#define AFE4490A_ADC_RDY_PIN GPIO_PIN_5

//AFE4490 B
#define AFE4490B_STE_BASE GPIO_PORTE_BASE
#define AFE4490B_STE_PIN GPIO_PIN_4
#define AFE4490B_STE_lo GPIOPinWrite(AFE4490B_STE_BASE,AFE4490B_STE_PIN,0x00)
#define AFE4490B_STE_hi GPIOPinWrite(AFE4490B_STE_BASE,AFE4490B_STE_PIN,0xFF)

#define AFE4490B_ADC_RDY_BASE GPIO_PORTC_BASE
#define AFE4490B_ADC_RDY_PIN GPIO_PIN_6

//AFE4490 C
#define AFE4490C_STE_BASE GPIO_PORTD_BASE
#define AFE4490C_STE_PIN GPIO_PIN_1
#define AFE4490C_STE_lo GPIOPinWrite(AFE4490C_STE_BASE,AFE4490C_STE_PIN,0x00)
#define AFE4490C_STE_hi GPIOPinWrite(AFE4490C_STE_BASE,AFE4490C_STE_PIN,0xFF)

#define AFE4490C_ADC_RDY_BASE GPIO_PORTC_BASE
#define AFE4490C_ADC_RDY_PIN GPIO_PIN_7





uint32_t AFE4490_hw_init(void);
uint8_t AFE4490_transferB(uint8_t data);
void AFE4490_delay_ms(uint32_t laikas);
extern void AFE4490_ADC_RDY_int(uint32_t *adresas);
extern unsigned char fram_write_buff(unsigned long pr_addr, unsigned char *buff, unsigned long size);

uint8_t AFE4490_ram_wr(uint32_t start_addr, uint8_t *buff, uint32_t size);

























#endif

