#include "stdint.h"
#include "stdio.h"
#include "stdbool.h"
#include "inc\tm4c123gh6pm.h"
#include "inc\hw_memmap.h"
#include "driverlib\gpio.h"
#include "driverlib\ssi.h"
#include "driverlib\pin_map.h"
#include "driverlib\sysctl.h"
#include "AFE4490_TM4C123GH6PMI_hw.h"


uint32_t AFE4490_hw_init(void)
{
	uint32_t ret;
	//uint32_t err_code;
	
	ret = 0;
	AFE4490_delay_ms(1000);	//powerup delejus
	//pins config
	GPIOPinTypeGPIOInput(AFE4490A_ADC_RDY_BASE, AFE4490A_ADC_RDY_PIN);	//nusistaom kaip iejima
	GPIOPadConfigSet(AFE4490A_ADC_RDY_BASE, AFE4490A_ADC_RDY_PIN, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD);
	
	GPIOPinTypeGPIOInput(AFE4490B_ADC_RDY_BASE, AFE4490B_ADC_RDY_PIN);	//nusistaom kaip iejima
	GPIOPadConfigSet(AFE4490B_ADC_RDY_BASE, AFE4490B_ADC_RDY_PIN, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD);
	
	GPIOPinTypeGPIOInput(AFE4490C_ADC_RDY_BASE, AFE4490C_ADC_RDY_PIN);	//nusistaom kaip iejima
	GPIOPadConfigSet(AFE4490C_ADC_RDY_BASE, AFE4490C_ADC_RDY_PIN, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD);
	
	GPIOPinTypeGPIOOutput(AFE4490A_STE_BASE, AFE4490A_STE_PIN);	//nusistaom kaip isejima
	GPIOPinTypeGPIOOutput(AFE4490B_STE_BASE, AFE4490B_STE_PIN);	//nusistaom kaip isejima
	GPIOPinTypeGPIOOutput(AFE4490C_STE_BASE, AFE4490C_STE_PIN);	//nusistaom kaip isejima
	
	AFE4490A_STE_hi;	//deselect
	AFE4490B_STE_hi;
	AFE4490C_STE_hi;
	//SPI CONFIG
	
	GPIOPinTypeSSI(GPIO_PORTA_BASE, GPIO_PIN_2 | GPIO_PIN_4 | GPIO_PIN_5); //SSI function to GPIO
	GPIOPadConfigSet(GPIO_PORTA_BASE, GPIO_PIN_2, GPIO_STRENGTH_8MA, GPIO_PIN_TYPE_STD);
	
	GPIOPinConfigure(GPIO_PA2_SSI0CLK);
	GPIOPinConfigure(GPIO_PA4_SSI0RX);
	GPIOPinConfigure(GPIO_PA5_SSI0TX);
	SSIDisable(SSI0_BASE);	//Disable for config
	SSIConfigSetExpClk(SSI0_BASE, SysCtlClockGet(), SSI_FRF_MOTO_MODE_0,SSI_MODE_MASTER, 16000000, 8);	//Config SSI0
	SSIEnable(SSI0_BASE);//Enable for communication
	/*
	while(1)
	{
		AFE4490_transferB(0xAA);
	}
	*/
	return(ret);
}

uint8_t AFE4490_transferB(uint8_t data)
{
	uint8_t tmp1;
	uint32_t PutData = 0;
	
	PutData |= data;
	
	SSIDataPut(SSI0_BASE,PutData);
	SSIDataGet(SSI0_BASE,&PutData);
	tmp1 = 0x000000FF & PutData;
	
	return(tmp1);
}

void AFE4490_delay_ms(uint32_t laikas)
{
	do
	{
		SysCtlDelay(26667);//1 ms velinimas
		laikas--;
	}while(laikas != 0);
}

uint8_t AFE4490_ram_wr(uint32_t start_addr, uint8_t *buff, uint32_t size)
{
	//return(fram_write_buff((unsigned long) start_addr, (unsigned char*) buff, (unsigned long) size));
	//reikes padaryti
	return(0);
}

