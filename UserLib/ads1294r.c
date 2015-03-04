#include "ads1294r.h"
#include "stdint.h"
#include "stdio.h"
#include "stdbool.h"
#include "inc\tm4c123gh6pm.h"
#include "inc\hw_memmap.h"
#include "driverlib\gpio.h"
#include "driverlib\ssi.h"
#include "driverlib\pin_map.h"
#include "driverlib\sysctl.h"

uint8_t ADS129X_RegRead(uint8_t addr)
{
	uint8_t tmp1;
	
	ADS129X_CS_lo;
	ADS129X_transferB(ADS129X_RREG|(0x1F&addr));
	ADS129X_DelayUs(2);
	ADS129X_transferB(0);
	ADS129X_DelayUs(2);
	tmp1 =  ADS129X_transferB(0);
	ADS129X_DelayUs(2);
	ADS129X_CS_hi;
	
	return tmp1;
}

void ADS129X_RegWrite(uint8_t addr, uint8_t value)
{
	ADS129X_CS_lo;
	ADS129X_transferB(ADS129X_WREG|(0x1F&addr));
	ADS129X_DelayUs(2);
	ADS129X_transferB(0);
	ADS129X_DelayUs(2);
	ADS129X_transferB(value);
	ADS129X_DelayUs(2);
	ADS129X_CS_hi;
}

void ADS129X_RegReadAll(ADS129X_reg_t *registrai)
{
	unsigned char tmp;
	ADS129X_CS_lo;
	
	ADS129X_transferB(ADS129X_RREG);
	ADS129X_transferB(0x19);					// 26
	for(tmp=0; tmp<26; tmp++)
	{
		*((unsigned char *)registrai + tmp) = ADS129X_transferB(0);
	}
	ADS129X_CS_hi;
}

void ADS129X_RegWriteAll(ADS129X_reg_t *registrai)
{
	uint8_t tmp;
	ADS129X_CS_lo;
	
	ADS129X_transferB(ADS129X_WREG);
	ADS129X_transferB(0x19);					// 26
	for(tmp=0; tmp<26; tmp++)
	{
		ADS129X_transferB(*((uint8_t *)registrai + tmp));
	}
	ADS129X_CS_hi;
}

void ADS129X_Cmd(uint8_t com)
{
	ADS129X_CS_lo;
	ADS129X_transferB(com);
	ADS129X_DelayUs(10);
	ADS129X_CS_hi;
}

void ADS129X_CmdReset(void)
{
	ADS129X_Cmd(ADS129X_RESET);
	ADS129X_DelayUs(20000);
}

void ADS129X_CmdSdatac(void)
{
	ADS129X_Cmd(ADS129X_SDATAC);
	ADS129X_DelayUs(2);
}

void ADS1294_DataRead(ADS1294_RDATA_result_t *struktura)
{	
	uint8_t* ptr;
	uint8_t i;
	uint8_t j;
	
	ptr = (uint8_t*)&struktura->CH1;
	ptr -= 4;
	
	ADS129X_CS_lo;

	for(i = 0; i < 3; i++)
	{
		*(ptr + (2-i)) = ADS129X_transferB(0);
	}
		
	ptr = (uint8_t*)&struktura->CH1;
	for(j = 0; j < 4; j++)
	{
		for(i = 0; i < 3; i++)
		{
			*(ptr + (3-i)) = ADS129X_transferB(0);
		}
		ptr += 4;
	}

	ADS129X_CS_hi;
	
	struktura->CH1 /= 256;
	struktura->CH2 /= 256;
	struktura->CH3 /= 256;
	struktura->CH4 /= 256;
}

void ADS1296_DataRead(ADS1296_RDATA_result_t *struktura)
{	
	uint8_t* ptr;
	uint8_t i;
	uint8_t j;
	
	ptr = (uint8_t*)&struktura->CH1;
	ptr -= 4;
	
	ADS129X_CS_lo;

	for(i = 0; i < 3; i++)
	{
		*(ptr + (2-i)) = ADS129X_transferB(0);
	}
		
	ptr = (uint8_t*)&struktura->CH1;
	for(j = 0; j < 6; j++)
	{
		for(i = 0; i < 3; i++)
		{
			*(ptr + (3-i)) = ADS129X_transferB(0);
		}
		ptr += 4;
	}

	ADS129X_CS_hi;
	
	struktura->CH1 /= 256;
	struktura->CH2 /= 256;
	struktura->CH3 /= 256;
	struktura->CH4 /= 256;
	struktura->CH5 /= 256;
	struktura->CH6 /= 256;
}

void ADS1294r_init(ADS129X_reg_t* registrai)
{
//	uint32_t laikinas;
	
	ADS129X_hw_init(); //pinu inicializacija
	ADS129X_SPI_init(1000000); //nustatymu siuntimui
	
	ADS129X_CmdReset(); //tegu persikrauna
	
	ADS129X_CmdSdatac();
	
	ADS129X_RegWrite(ADS129X_CONFIG3,0xC0);
	ADS129X_RegWrite(ADS129X_CONFIG1,0x84);	//2kSPS
	//laikinas = ADS129X_RegRead(ADS129X_CONFIG1);
	ADS129X_RegWrite(ADS129X_CONFIG2,0x10);
	/*********************** Pasiduodam testini signala********
	ADS129X_RegWrite(ADS129X_CH1SET,0x05);
	ADS129X_RegWrite(ADS129X_CH2SET,0x05);
	ADS129X_RegWrite(ADS129X_CH3SET,0x05);
	ADS129X_RegWrite(ADS129X_CH4SET,0x05);
	*/
	
	/*********************** Prisijungiam elektrodus**********/
	ADS129X_RegWrite(ADS129X_CH1SET,0x00);	//nomrmalus kanalas
	//ADS129X_RegWrite(ADS129X_CH1SET,0x40);	//kvepavimui
	ADS129X_RegWrite(ADS129X_CH2SET,0x00);
	ADS129X_RegWrite(ADS129X_CH3SET,0x00);
	ADS129X_RegWrite(ADS129X_CH4SET,0x00);
	
	//RLD
	ADS129X_RegWrite(ADS129X_RLD_SENSP,0x0E);
	ADS129X_RegWrite(ADS129X_RLD_SENSN,0x0E);
	ADS129X_RegWrite(ADS129X_CONFIG3,0xCC); //Turn on RLD amplifier, set internal RLDREF voltage
	
	ADS129X_RegWrite(ADS129X_RESP,((1<<7)|(1<<6)|(1<<5)|0x02));
  //ADS129X_RegWrite(ADS129X_CONFIG4,1 << 5);
	
	
	/************ Uztrumpinami, triuksmo matavimui
	ADS129X_RegWrite(ADS129X_CH1SET,0x01);
	ADS129X_RegWrite(ADS129X_CH2SET,0x01);
	ADS129X_RegWrite(ADS129X_CH3SET,0x01);
	ADS129X_RegWrite(ADS129X_CH4SET,0x01);
	*/
	
	ADS129X_Cmd(ADS129X_RDATAC); //Data continous
	ADS129X_Cmd(ADS129X_START);	//startuojam
	
	ADS129X_SPI_init(20000000);	//nuskaitymo sparta
	
}

void ADS1296r_init(ADS129X_reg_t* registrai)
{
	//uint32_t laikinas;
	
	ADS129X_hw_init(); //pinu inicializacija
	ADS129X_SPI_init(1000000); //nustatymu siuntimui
	
	ADS129X_CmdReset(); //tegu persikrauna
	
	ADS129X_CmdSdatac();
	
	ADS129X_RegWrite(ADS129X_CONFIG3,0xC0);
	ADS129X_RegWrite(ADS129X_CONFIG1,0x84);	//2kSPS
	//laikinas = ADS129X_RegRead(ADS129X_CONFIG1);
	ADS129X_RegWrite(ADS129X_CONFIG2,0x10);
	/*********************** Pasiduodam testini signala*******
	ADS129X_RegWrite(ADS129X_CH1SET,0x05);
	ADS129X_RegWrite(ADS129X_CH2SET,0x05);
	ADS129X_RegWrite(ADS129X_CH3SET,0x05);
	ADS129X_RegWrite(ADS129X_CH4SET,0x05);
	ADS129X_RegWrite(ADS129X_CH5SET,0x05);
	ADS129X_RegWrite(ADS129X_CH6SET,0x05);*/
	
	
	/*********************** Prisijungiam elektrodus**********/
	ADS129X_RegWrite(ADS129X_CH1SET,0x00);	//nomrmalus kanalas
	//ADS129X_RegWrite(ADS129X_CH1SET,0x40);	//kvepavimui
	ADS129X_RegWrite(ADS129X_CH2SET,0x00);
	ADS129X_RegWrite(ADS129X_CH3SET,0x00);
	ADS129X_RegWrite(ADS129X_CH4SET,0x00);
	ADS129X_RegWrite(ADS129X_CH5SET,0x00);
	ADS129X_RegWrite(ADS129X_CH6SET,0x00);
	
	//RLD
	ADS129X_RegWrite(ADS129X_RLD_SENSP,0x0E);
	ADS129X_RegWrite(ADS129X_RLD_SENSN,0x0E);
	ADS129X_RegWrite(ADS129X_CONFIG3,0xCC); //Turn on RLD amplifier, set internal RLDREF voltage
	
	ADS129X_RegWrite(ADS129X_RESP,((1<<7)|(1<<6)|(1<<5)|0x02));
  //ADS129X_RegWrite(ADS129X_CONFIG4,1 << 5);
	
	
	/************ Uztrumpinami, triuksmo matavimui
	ADS129X_RegWrite(ADS129X_CH1SET,0x01);
	ADS129X_RegWrite(ADS129X_CH2SET,0x01);
	ADS129X_RegWrite(ADS129X_CH3SET,0x01);
	ADS129X_RegWrite(ADS129X_CH4SET,0x01);
	*/
	
	ADS129X_Cmd(ADS129X_RDATAC); //Data continous
	ADS129X_Cmd(ADS129X_START);	//startuojam
	
	ADS129X_SPI_init(20000000);	//nuskaitymo sparta
	
}

