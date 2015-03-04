#ifndef USER_BOARD_H
#define USER_BOARD_H
#include "stdint.h"
#include "stdbool.h"

#include "driverlib\gpio.h"

//***************isorinio maitinimo jutimas
#define SUPPLY_SNS_BASE GPIO_PORTA_BASE
#define SUPPLY_SNS_PIN GPIO_PIN_6
#define SUPPLY_ON !((GPIOPinRead(SUPPLY_SNS_BASE,SUPPLY_SNS_PIN)) & (SUPPLY_SNS_PIN))

//**********krovimas
#define CHG_STATUS_BASE GPIO_PORTC_BASE
#define CHG_STATUS_PIN GPIO_PIN_4
#define CHARGING !((GPIOPinRead(CHG_STATUS_BASE,SUPPLY_SNS_PIN)) & (CHG_STATUS_PIN))


//Magnetinis jungiklis
#define MagSwBase GPIO_PORTA_BASE
#define MagSwPin GPIO_PIN_1
#define MagSwON ((GPIOPinRead(MagSwBase,MagSwPin)) & (MagSwPin))

//LED
#define LedBase GPIO_PORTC_BASE
#define LedPin GPIO_PIN_2
#define LedON GPIOPinWrite(LedBase, LedPin, 0xFF);
#define LedOFF GPIOPinWrite(LedBase, LedPin, 0x00);

//SD reader MODE
#define SdModeBase GPIO_PORTB_BASE
#define SdModePin GPIO_PIN_1
#define SdModeHi GPIOPinWrite(SdModeBase, SdModePin, 0xFF)
#define SdModeLow GPIOPinWrite(SdModeBase, SdModePin, 0x00)

//SD reader BUSY
#define SdBusyBase GPIO_PORTA_BASE
#define SdBusyPin GPIO_PIN_7
#define SdBusy (!((GPIOPinRead(SdBusyBase,SdBusyPin)) & (SdBusyPin)))



//BLTH
#define BlthBase GPIO_PORTB_BASE
#define BlthPin GPIO_PIN_1

//Baterijos itampa
#define BatVoBase GPIO_PORTE_BASE
#define BatVoPint GPIO_PIN_4

typedef struct
{
	uint16_t Temp1;
	uint16_t Temp2;
	uint16_t Temp3;
	uint16_t Temp4;
}Temp_Data_t;

typedef struct
{
	uint16_t Mas0[256];
	uint16_t Mas1[256];
	uint8_t Mas0Status :1;
	uint8_t Mas1Status :1;
	uint8_t ArrCnt;
	uint16_t cnt;
}TempDataBuff_t;

typedef struct
{
	uint32_t ADS1294R;
	uint32_t AFE4490A;
	uint32_t AFE4490B;
	uint32_t AFE4490C;
	uint8_t TempMas0Status :1;
	uint8_t TempMas1Status :1;
	uint8_t ImuMas0Status :1;
	uint8_t ImuMas1Status :1;
	uint8_t TimeStamp0Status :1;
	uint8_t TimeStamp1Status :1;
}Occupied_t;

typedef struct
{
	uint8_t Mas0[512]; //Reiksmiu masyvas
	uint8_t Mas1[512]; //Reiksmiu masyvas
	uint8_t Cnt; //Indekso skaitliukas
	uint8_t ArrCnt; //Masyvo skaitliukas
	uint8_t Mas0Status : 1; //Busenos bitas
	uint8_t Mas1Status : 1; //Busenos bitas
}TimeStampDataBuff_t;

typedef struct
{
	uint32_t EcgDataCnt;
	uint32_t PpgDataCnt ;
	uint32_t ImuDataCnt;
	uint32_t TempDataCnt ;
}DataCnt_t;

extern void board_init(void);
extern void temperaturos_spi_init(void);
extern void BoardMinimumInit(void);

void TempDataPut(Temp_Data_t* data, TempDataBuff_t* DataBuff);
void TimeStampDataPut(DataCnt_t * data, TimeStampDataBuff_t * DataBuff);


#endif
