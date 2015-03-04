#include "stdint.h"
#include "stdio.h"
#include "stdbool.h"
#include "inc\tm4c123gh6pm.h"
#include "inc/hw_gpio.h"
#include "inc/hw_memmap.h"
#include "inc/hw_sysctl.h"
#include "inc/hw_types.h"
#include "inc\hw_ssi.h"
#include "driverlib\gpio.h"
#include "driverlib\ssi.h"
#include "driverlib\pin_map.h"
#include "driverlib\sysctl.h"
#include "driverlib\udma.h"
#include "23LCV1024_hw.h"

extern uint8_t pui8DMAControlTable[1024];

void RAM_hw_init(void)
{

	
	GPIOPinTypeGPIOOutput(RAM_CS_BASE, RAM_CS_PIN);	//nusistaom kaip isejima
	
	RAM_CS_hi;
	
	//SPI CONFIG
	
	HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY;
  HWREG(GPIO_PORTF_BASE + GPIO_O_CR) = 0x00000001;
	HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = 0x00000001;
	
	GPIOPinTypeSSI(GPIO_PORTF_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2); //SSI function to GPIO
	
	GPIOPinConfigure(GPIO_PF0_SSI1RX);
	GPIOPinConfigure(GPIO_PF1_SSI1TX);
	GPIOPinConfigure(GPIO_PF2_SSI1CLK);
	
	GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_2 ,GPIO_STRENGTH_8MA, GPIO_PIN_TYPE_STD);
	SSIDisable(SSI1_BASE);	//Disable for config
	SSIConfigSetExpClk(SSI1_BASE, SysCtlClockGet(), SSI_FRF_MOTO_MODE_0,SSI_MODE_MASTER, 20000000, 8);	//Config SSI1
	HWREG(SSI1_BASE + SSI_O_CR1) |= 0x10;
	SSIEnable(SSI1_BASE);//Enable for communication
	
	
	
	uDMAEnable();
	
	uDMAControlBaseSet(&pui8DMAControlTable[0]);
	
	uDMAChannelAssign(UDMA_CH11_SSI1TX);
	uDMAChannelAssign(UDMA_CH10_SSI1RX);
	
	uDMAChannelAttributeDisable(11, UDMA_ATTR_USEBURST | UDMA_ATTR_ALTSELECT | UDMA_ATTR_HIGH_PRIORITY | UDMA_ATTR_REQMASK);
	uDMAChannelAttributeDisable(10, UDMA_ATTR_USEBURST | UDMA_ATTR_ALTSELECT | UDMA_ATTR_HIGH_PRIORITY | UDMA_ATTR_REQMASK);
	//uDMAChannelAttributeEnable(11,UDMA_ATTR_USEBURST);
	
	uDMAChannelControlSet(11 | UDMA_PRI_SELECT, UDMA_SIZE_8 | UDMA_SRC_INC_8 | UDMA_DST_INC_NONE | UDMA_ARB_4);
	uDMAChannelControlSet(10 | UDMA_PRI_SELECT, UDMA_SIZE_8 | UDMA_SRC_INC_NONE | UDMA_DST_INC_8 | UDMA_ARB_4);
	
	RAM_CS_lo;
	RAM_transferB(0x01);	//write MODE
	RAM_transferB(0x40);	//sequential mode
	RAM_CS_hi;

}
uint8_t RAM_transferB(uint8_t data)
{
	uint8_t tmp1;
	uint32_t PutData = 0;
	
	PutData |= data;
	
	SSIDataPut(SSI1_BASE,PutData);
	SSIDataGet(SSI1_BASE,&PutData);
	tmp1 = 0x000000FF & PutData;
	
	return(tmp1);
}
