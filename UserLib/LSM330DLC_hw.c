
#include "stdint.h"
#include "stdio.h"
#include "stdbool.h"
#include "inc\tm4c123gh6pm.h"
#include "inc\hw_memmap.h"
#include "driverlib\gpio.h"
#include "driverlib\ssi.h"
#include "driverlib\pin_map.h"
#include "driverlib\sysctl.h"
#include "LSM330DLC.h"


void LSM330DLC_hw_init(void)
{
	GPIOPinTypeGPIOInput(LSM330DLC_A_INT_BASE, LSM330DLC_A_INT_PIN);	//nusistatom kaip iejima
	GPIOPinTypeGPIOInput(LSM330DLC_G_INT_BASE, LSM330DLC_G_INT_PIN);	//nusistatom kaip iejima
	//GPIOPadConfigSet(ADS129X_DRDY_BASE, ADS129X_DRDY_PIN, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD);
	//GPIOIntTypeSet(LSM330DLC_G_INT_BASE, LSM330DLC_G_INT_PIN, GPIO_FALLING_EDGE);
	//GPIOIntTypeSet(LSM330DLC_A_INT_BASE, LSM330DLC_A_INT_PIN, GPIO_FALLING_EDGE);
	
	GPIOPinTypeGPIOOutput(LSM330DLC_CS_A_BASE, LSM330DLC_CS_A_PIN);	//nusistaom kaip isejima
	LSM330DLC_CS_A_hi;
	GPIOPinTypeGPIOOutput(LSM330DLC_CS_G_BASE, LSM330DLC_CS_G_PIN);	//nusistaom kaip isejima
	LSM330DLC_CS_G_hi;
	
	GPIOPinTypeSSI(GPIO_PORTB_BASE, GPIO_PIN_4 | GPIO_PIN_6 | GPIO_PIN_7); //SSI function to GPIO
	
	GPIOPinConfigure(GPIO_PB4_SSI2CLK);
	GPIOPinConfigure(GPIO_PB6_SSI2RX);
	GPIOPinConfigure(GPIO_PB7_SSI2TX);
	
	GPIOPadConfigSet(GPIO_PORTB_BASE, GPIO_PIN_4, GPIO_STRENGTH_8MA, GPIO_PIN_TYPE_STD);
	
	LSM330DLC_SPI_init(1000000);
	
}


/*
	Inicializuoja SPI
	Kokiu reikia greiciu greiciu
*/
void LSM330DLC_SPI_init(uint32_t speed)
{
	SSIDisable(SSI2_BASE);	//Disable for config
	SSIConfigSetExpClk(SSI2_BASE, SysCtlClockGet(), SSI_FRF_MOTO_MODE_3,SSI_MODE_MASTER, speed	, 8);	//Config SSI2
	SSIEnable(SSI2_BASE);//Enable for communication
}


/*
	Issiuncia ir gauna
*/
uint8_t LSM330DLC_transferB(uint8_t data)
{
	uint8_t tmp1;
	uint32_t PutData = 0;
	
	PutData |= data;
	
	SSIDataPut(SSI2_BASE,PutData);
	SSIDataGet(SSI2_BASE,&PutData);
	tmp1 = 0x000000FF & PutData;
	
	return(tmp1);
}

void LSM330DLC_DelayUs(uint32_t n)
{
	SysCtlDelay(27*n);
}








