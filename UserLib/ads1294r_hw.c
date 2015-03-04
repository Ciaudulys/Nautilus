
#include "stdint.h"
#include "stdio.h"
#include "stdbool.h"
#include "inc\tm4c123gh6pm.h"
#include "inc\hw_memmap.h"
#include "driverlib\gpio.h"
#include "driverlib\ssi.h"
#include "driverlib\pin_map.h"
#include "driverlib\sysctl.h"
#include "ads1294r.h"


void ADS129X_hw_init(void)
{
	GPIOPinTypeGPIOInput(ADS129X_DRDY_BASE, ADS129X_DRDY_PIN);	//nusistatom kaip iejima
	//GPIOPadConfigSet(ADS129X_DRDY_BASE, ADS129X_DRDY_PIN, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD);
	GPIOIntTypeSet(ADS129X_DRDY_BASE, ADS129X_DRDY_PIN, GPIO_FALLING_EDGE);
	
	GPIOPinTypeGPIOOutput(ADS129X_CS_BASE, ADS129X_CS_PIN);	//nusistaom kaip isejima
	ADS129X_CS_hi;
	
	GPIOPinTypeSSI(GPIO_PORTB_BASE, GPIO_PIN_4 | GPIO_PIN_6 | GPIO_PIN_7); //SSI function to GPIO
	
	GPIOPinConfigure(GPIO_PB4_SSI2CLK);
	GPIOPinConfigure(GPIO_PB6_SSI2RX);
	GPIOPinConfigure(GPIO_PB7_SSI2TX);
	
}


/*
	Inicializuoja SPI
	Kokiu reikia greiciu greiciu
*/
void ADS129X_SPI_init(uint32_t speed)
{
	SSIDisable(SSI2_BASE);	//Disable for config
	SSIConfigSetExpClk(SSI2_BASE, SysCtlClockGet(), SSI_FRF_MOTO_MODE_1,SSI_MODE_MASTER, speed	, 8);	//Config SSI2
	SSIEnable(SSI2_BASE);//Enable for communication
}


/*
	Issiuncia ir gauna SPI0
*/
uint8_t ADS129X_transferB(uint8_t data)
{
	uint8_t tmp1;
	uint32_t PutData = 0;
	
	PutData |= data;
	
	SSIDataPut(SSI2_BASE,PutData);
	SSIDataGet(SSI2_BASE,&PutData);
	tmp1 = 0x000000FF & PutData;
	
	return(tmp1);
}

void ADS129X_DelayUs(uint32_t n)
{
	SysCtlDelay(27*n);
}
