#include "stdint.h"
#include "stdio.h"
#include "stdbool.h"
#include "inc\tm4c123gh6pm.h"
#include "inc\hw_memmap.h"
#include "inc\hw_types.h"
#include "driverlib\gpio.h"
#include "driverlib\ssi.h"
#include "driverlib\pin_map.h"
#include "driverlib\sysctl.h"
#include "driverlib\udma.h"
#include "driverlib\interrupt.h"
#include "23LCV1024.h"
#include <rtl.h>

extern uint8_t SSI1_flegas;

void RAM_Byte_Write(uint32_t addres, uint8_t data)
{
	RAM_CS_lo;
	RAM_transferB(0x02);	//sequential write CMD
	RAM_transferB((uint8_t)(addres >>16));	//MSB
	RAM_transferB((uint8_t)(((addres & 0x00FFFF) >>8)));	//MidSB
	RAM_transferB((uint8_t)(((addres & 0x0000FF))));	//LSB
	RAM_transferB(data);
	RAM_CS_hi;
}

uint8_t RAM_Byte_Read(uint32_t addres)
{
	uint8_t ret;
	
	RAM_CS_lo;
	RAM_transferB(0x03);	//sequential read CMD
	RAM_transferB((uint8_t)(addres >>16));	//MSB
	RAM_transferB((uint8_t)(((addres & 0x00FFFF) >>8)));	//MidSB
	RAM_transferB((uint8_t)(((addres & 0x0000FF))));	//LSB
	ret = RAM_transferB(0);
	RAM_CS_hi;
	return(ret);
}

void RAM_array_Write(uint32_t StartAdress, uint8_t *data, uint32_t count)
{
	uint32_t i;
	
	RAM_CS_lo;
	RAM_transferB(0x02);	//sequential write CMD
	RAM_transferB((uint8_t)(StartAdress >>16));	//MSB
	RAM_transferB((uint8_t)(((StartAdress & 0x00FFFF) >>8)));	//MidSB
	RAM_transferB((uint8_t)(((StartAdress & 0x0000FF))));	//LSB
	for (i = 0; i < count; i++)
	{
		RAM_transferB(*data);
		data++;
	}
	RAM_CS_hi;
}

void RAM_array_Read(uint32_t StartAdress, uint8_t *data, uint32_t count)
{
	uint32_t i;
	
	RAM_CS_lo;
	RAM_transferB(0x03);	//sequential read CMD
	RAM_transferB((uint8_t)(StartAdress >>16));	//MSB
	RAM_transferB((uint8_t)(((StartAdress & 0x00FFFF) >>8)));	//MidSB
	RAM_transferB((uint8_t)(((StartAdress & 0x0000FF))));	//LSB
	for (i = 0; i < count; i++)
	{
		*data = RAM_transferB(0);
		data++;
	}
	RAM_CS_hi;
}

void RAM_array_DMA_Wr(uint32_t StartAdress, uint8_t *data, uint32_t count)
{
	uint32_t tmp;
	uint32_t dummy;
	
	tsk_lock();
	
	uDMAChannelControlSet(11 | UDMA_PRI_SELECT, UDMA_SIZE_8 | UDMA_SRC_INC_8 | UDMA_DST_INC_NONE | UDMA_ARB_4);
	
	uDMAChannelTransferSet(11 | UDMA_PRI_SELECT, UDMA_MODE_BASIC, data, (void *)(SSI1_BASE + SSI_O_DR), count);
	uDMAChannelEnable(11);
	
	RAM_CS_lo;
	
	RAM_transferB(0x02);	//sequential write CMD
	RAM_transferB((uint8_t)(StartAdress >>16));	//MSB
	RAM_transferB((uint8_t)(((StartAdress & 0x00FFFF) >>8)));	//MidSB
	RAM_transferB((uint8_t)(((StartAdress & 0x0000FF))));	//LSB
	
	
	SSIDMAEnable(SSI1_BASE,SSI_DMA_TX);
	SSIIntEnable(SSI1_BASE,SSI_TXFF);
	
	tsk_unlock();
	
	//while(1);
	/*
	do
	{
		tmp = uDMAChannelSizeGet(11 | UDMA_PRI_SELECT);
	}while(tmp != 0);
	
	while(SSIBusy(SSI1_BASE));
	*/
	/*
	while(SSI1_flegas == 0);
	SSI1_flegas = 0;
	*/
	os_evt_wait_or(0x0001,0xFFFF);
	RAM_CS_hi;
	SSIDMADisable(SSI1_BASE,SSI_DMA_TX);
	SSIDMADisable(SSI1_BASE,SSI_DMA_RX);
	SSIIntDisable(SSI1_BASE,SSI_TXFF);
	
	do
	{
	tmp = SSIDataGetNonBlocking(SSI1_BASE,&dummy);
	}while (tmp != 0);
	
}

void RAM_array_DMA_Rd(uint32_t StartAdress, uint8_t *data, uint32_t count)
{
	uint32_t tmp1;
	uint32_t dummy = 0xFF;
	uint8_t MSB, MidSB, LSB;
	
	tsk_lock();
	IntMasterDisable();
	
	uDMAChannelControlSet(11 | UDMA_PRI_SELECT, UDMA_SIZE_8 | UDMA_SRC_INC_NONE | UDMA_DST_INC_NONE | UDMA_ARB_4);
	
	uDMAChannelTransferSet(11 | UDMA_PRI_SELECT, UDMA_MODE_BASIC, &dummy, (void *)(SSI1_BASE + SSI_O_DR), count);
	uDMAChannelTransferSet(10 | UDMA_PRI_SELECT, UDMA_MODE_BASIC,(uint32_t *)(SSI1_BASE + SSI_O_DR), data, count);
	uDMAChannelEnable(10);
	uDMAChannelEnable(11);
	
	RAM_CS_lo;
	MSB = StartAdress >> 16;
	MidSB = StartAdress >>8;
	LSB = StartAdress;
	
	RAM_transferB(0x03);	//sequential read CMD
	RAM_transferB(MSB);	//MSB
	RAM_transferB(MidSB);	//MidSB
	RAM_transferB(LSB);	//LSB
	
	SSIDMAEnable(SSI1_BASE,SSI_DMA_RX);
	SSIDMAEnable(SSI1_BASE,SSI_DMA_TX);
	SSIIntEnable(SSI1_BASE,SSI_TXFF);
	
	IntMasterEnable();
	tsk_unlock();
	
	//while(1);
	/*
	do
	{
		tmp = uDMAChannelSizeGet(11 | UDMA_PRI_SELECT);
	}while(tmp != 0);
	
	while(SSIBusy(SSI1_BASE));
	*/
	/*
	while(SSI1_flegas == 0);
	SSI1_flegas = 0;
	*/
	os_evt_wait_or(0x0001,0xFFFF);
	RAM_CS_hi;
	SSIDMADisable(SSI1_BASE,SSI_DMA_TX);
	SSIDMADisable(SSI1_BASE,SSI_DMA_RX);
	SSIIntDisable(SSI1_BASE,SSI_TXFF);
	
	do
	{
	tmp1 = SSIDataGetNonBlocking(SSI1_BASE,&dummy);
	}while (tmp1 != 0);
	
}
	



