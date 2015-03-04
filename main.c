#include "stdint.h"
#include "stdio.h"
#include "stdbool.h"
#include "inc\tm4c123gh6pm.h"
#include "inc\hw_memmap.h"
#include "inc\hw_ssi.h"
#include "inc\hw_types.h"
#include "inc/hw_gpio.h"
#include "driverlib\sysctl.h"
#include "driverlib\gpio.h"
#include "driverlib\interrupt.h"
#include "driverlib\pwm.h"
#include "driverlib\ssi.h"
#include "driverlib\pin_map.h"
#include "driverlib\timer.h"
#include "driverlib\uart.h"
#include "driverlib\udma.h"
#include "driverlib\adc.h"
#include "AFE4490.h"
#include "AFE4490_app.h"
#include "23LCV1024.h"
#include "SD_card\ff.h"
#include "SD_card\diskio.h"
#include "ads1294r.h"
#include "ads1296r_app.h"
#include "LSM330DLC.h"
#include "LSM330DLC_app.h"
#include "driverlib\hibernate.h"
#include <rtl.h>
#include "board.h"
#include "data_structure.h"
//#include "RRrecognition.h"
#include "Filtrai.h"


//global variables
uint32_t tmp; //bendros paskirties kintamasis
uint32_t min_msg = 10; //Message box pilnumo tikrinimas
uint8_t SD_darbinis_mas0[512]; //Masyvas per kuri vyksta operacijos su SRAM
volatile uint8_t ovf_detect = 0; //Circullar masyvo persipildymas
volatile uint32_t MagSwCnt = 0;	//magnetinio mygtuko skaitliukas
volatile uint32_t LedBlinkCnt = 0; //LED mirksejimo skaitliukas
volatile uint8_t SynchroLed = 0;
FATFS FileSystem; //Failu sistemos struktura
FIL DataFile; //Failas


//Trumpo duomenu kaupimo strukturos
ADS1296R_DataBuff_t ADS1296R_DataBuff;
AFE4490_DataBuff_t AFE4490A_DataBuff;
AFE4490_DataBuff_t AFE4490B_DataBuff;
AFE4490_DataBuff_t AFE4490C_DataBuff;
LSM330DLC_DataBuff_t ImuDataBuff;
TempDataBuff_t TempDataBuff;
TimeStampDataBuff_t TimeStampBuff;

//debuginimui
//uint32_t ovfl_check;

//DMA valdymo lentele
__align(1024) uint8_t pui8DMAControlTable[1024];


// RTX duomenys
OS_TID ID_Temperaturos_matavimas, ID_RAM_worker, ID_SD_worker; //Uzduociu ID
OS_TID ID_DuomenuSurinkimas, ID_HrSkaiciavimas, ID_BatCheck; //uzduociu ID
OS_MUT MUT_SSI2, MUT_FreshData;; //MUTEX objektai
os_mbx_declare(RAM_mb, 10);//SRAM funkcijos pasto dezute
os_mbx_declare(SD_mb, 4);//SD koteles aptarnavimo pasto dezute
os_mbx_declare(Temp_mb,6);//Temperaturos duomenu perdavimo dezute
os_mbx_declare(Events_mb,4);//Ivykiu, reset arba power off, pasto dezute
os_mbx_declare(TimeStampMb,4);//laiko ivikiu pasto dezute
os_mbx_declare(HrMb,4); //HR skaiciavimo MB



//Apskrito masyvo rodykles bei konstantos
//EKG
uint32_t ADS129X_ptr_read, ADS129X_ptr_write;
#define ADS129X_RAM_start 0x00
#define ADS129X_RAM_end 0x17300
//AFE4490A
uint32_t AFE4490A_ptr_read, AFE4490A_ptr_write;
#define AFE4490A_RAM_start 0x17300
#define AFE4490A_RAM_end 0x1A200
//AFE4490B
uint32_t AFE4490B_ptr_read, AFE4490B_ptr_write;
#define AFE4490B_RAM_start 0x1A200
#define AFE4490B_RAM_end 0x1D100
//AFE4490C
uint32_t AFE4490C_ptr_read, AFE4490C_ptr_write;
#define AFE4490C_RAM_start 0x1D100
#define AFE4490C_RAM_end 0x20000


//Tkrinimas ar speja buferint duomenis
//RAM worker
uint8_t AFE4490A_paskutinis_mas, AFE4490B_paskutinis_mas, AFE4490C_paskutinis_mas, ADS129X_paskutinis_mas;
//duomenu nuskaitymo
uint8_t AFE4490A_einamas_mas, AFE4490B_einamas_mas, AFE4490C_einamas_mas, ADS129X_einamas_mas;

//pagalbines funkcijos
AFE4490_param_t pulsoks_param; //AFE4490 registru struktura
uint32_t RAM_uzimta(uint32_t wr_ptr, uint32_t rd_ptr, uint32_t start_addr,uint32_t end_addr); //Funkcija ramu uzimtumui tikrint
uint8_t FileWrite(FIL *file, uint32_t msg, uint32_t uzimta);//Universali rasymo i faila funkcija
void GetOccupied(Occupied_t *Occupied);//nustato ramu uzimtuma

//Pertrauktys
void temperaturos_tick(void);//Temperaturos timer'io tickas
void PortAIntHandler(void);//A porto pertrauktis
uint8_t SSI1_flegas = 0; //DMA perkelimo galas
void SSI1Handler(void); //SSI DMA pertrauktis
uint8_t SSI3_flegas = 0; //DMA perkelimo galas
void SSI3Handler(void); //SSI DMA pertrauktis
void PortBIntHandler(void);//B porto pertrauktis (EKG)
void PortEIntHandler(void);//E porto pertrauktis
void SD_tick(void);//SD tickas timeoutais

void AFE4490ABC_reg_init(void); //AFE4490 registru inicializacija

void LedBlink(uint32_t Delay);//LED mirksinimas

//uzduotys
__task void Temperaturos_matavimas (void); //Temperaturos matavimo uzduotis

__task void RAM_worker(void);//RAMU naudojimo uzduotis
__task void SD_worker(void);//SD aptarnavimo uzduotis

__task void DuomenuSurinkimas(void);//Duomenu surinkimo uzduotis
__task void HrSkaiciavimas(void);
__task void BatCheck(void);

ADS129X_reg_t ADS1294R_reg;//EKG chipo struktura


int main(void)
{
	volatile uint32_t dummy;//siaip kintamasis
	//volatile uint32_t kiekSkaityt;
	uint32_t skaitliukas; //skaitliukas inizializacija
	FRESULT Result;//Failu sistemos rezultatas
	volatile DSTATUS DiskoBusena;
	volatile DRESULT DiskoAtsakas;
	char FileName[15];//failo pavadinimas
	
	//LedOFF;
	LedBlinkCnt = 0;//inicializacija mirksejimo
	
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);	//ijungiam B porta
	HWREG(GPIO_PORTC_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY;
  HWREG(GPIO_PORTC_BASE + GPIO_O_CR) = 0x00000004;
	HWREG(GPIO_PORTC_BASE + GPIO_O_LOCK) = 0x00000004;
	GPIOPinTypeGPIOOutput(LedBase,LedPin);//pasidarom isejimu
	LedON;//ijungiam LED
	SysCtlDelay(20000000);//Palaukiam ~1,5 sekundes, kad isbedos butu galima prisijungti prie uC
	LedOFF;//isjungiam LED
	
	
	SysCtlPeripheralEnable(SYSCTL_PERIPH_HIBERNATE); //CLK i Hibernation
	if(!HibernateIsActive())
	{//kazkoks kitas resetas, pvz pirma kart power on arba debug resetas
		BoardMinimumInit(); //Checkina baterijos krovima
		//jeigu istrauke pakroveja uzmigdo
		HibernateEnableExpClk(SysCtlClockGet()); //Ijungiamas hibernate CLK
		HibernateClockConfig(HIBERNATE_OSC_LOWDRIVE);//Naudojamas vidinis RC
		HibernateWakeSet(HIBERNATE_WAKE_PIN);//pazadinimo slayga - wake isvadas
		//net nemegina eit miegot kol bent viena is wake up salygu aktyvios
		while(MagSwON)//tikrina magnetini mygtuka
		{
			BoardMinimumInit(); //Checkina baterijos krovima
		}
		HibernateRequest();//liepa Hibernate moduliui isjungti maitinima
		while(1);//Maitinima isjungia is slow clk srities, todel uC dar siek tiek paveikia
	}else
	{//is po hiberneitinimo
		BoardMinimumInit(); //Checkina baterijos krovima
		SysCtlDelay(20000); //Velinimas, kad butu pasibaige pereinamieji procesai
		if(!MagSwON)
		{//jeigu nepridedas magnetas
			LedOFF;//LED uzgesta
			SysCtlDelay(2000000);//velinimas, kad butu uzgeses kuri laika
			LedON;//LED uzsiziebia
			SysCtlDelay(2000000);//Velinimas kad butu uzsiziebes
			LedOFF;//LED uzgesta
			SysCtlDelay(2000000); //Velinimas, kad pabutu uzgeses
			//jeigu istrauke pakroveja uzmigdo
			HibernateEnableExpClk(SysCtlClockGet());//Ijungimas hibernate CLK
			HibernateClockConfig(HIBERNATE_OSC_LOWDRIVE);//Naudojamas vidinis RC
			HibernateWakeSet(HIBERNATE_WAKE_PIN);//pazadinimo salyga - wake pin'as
			//net nemegina eit miegot kol bent viena is wake up salygu aktyvios
			while(MagSwON)//ar pridetas magnetas
			{
				BoardMinimumInit(); //Checkina baterijos krovima
			}
			HibernateRequest();//liepa Hibernate moduliui isjungti maitinima
			while(1);//Maitinima isjungia is slow clk srities, todel uC dar siek tiek paveikia
		}
	}
	//ateina iki cia jeigu reseto metu (power on/debug/prog) buvo stabiliai pridetas magnetas
	while(MagSwON)//Laukiam, kad atitrauktu magneta ir prietaisas startuotu
	{
		LedON;//Ijungia LED
		SysCtlDelay(200000);//Pasviecia LED
		LedOFF;//isjungia LED
		SysCtlDelay(200000);//Pasviecia LED
	}
	
	HibernateEnableExpClk(SysCtlClockGet());//Ijungiamas hibernate blokas
	HibernateClockConfig(HIBERNATE_OSC_LOWDRIVE);//Vidinis RC
	HibernateWakeSet(HIBERNATE_WAKE_PIN);//Prikelimo saltinis Wake isvadas
	
	board_init();//plokstes inicializacija - uC CLK, ijungiama periferija 
	
	
	TempDataBuff.ArrCnt = 0;//pradzioj 0
	TempDataBuff.cnt = 0;//pradzioj 0
	TempDataBuff.Mas0Status = 0;//pradzioj 0
	TempDataBuff.Mas1Status = 0;//pradzioj 0
	
	ImuDataBuff.ArrCnt = 0;//pradzioj 0
	ImuDataBuff.cnt = 0;//pradzioj 0
	ImuDataBuff.Mas0Status = 0;//pradzioj 0
	ImuDataBuff.Mas1Status = 0;//pradzioj 0
	
	ADS1296R_DataBuff.ArrCnt = 0;//pradzioj 0
	ADS1296R_DataBuff.cnt = 0;//pradzioj 0
	AFE4490A_DataBuff.ArrCnt = 0;//pradzioj 0
	AFE4490A_DataBuff.cnt = 0;//pradzioj 0
	AFE4490B_DataBuff.ArrCnt = 0;//pradzioj 0
	AFE4490B_DataBuff.cnt = 0;//pradzioj 0
	AFE4490C_DataBuff.ArrCnt = 0;//pradzioj 0
	AFE4490C_DataBuff.cnt = 0;//pradzioj 0
	TimeStampBuff.ArrCnt = 0;//pradzioj 0
	TimeStampBuff.Cnt = 1;	//nes pirmame baite irasyta kiek time stampu
	TimeStampBuff.Mas0Status = 0;//pradzioj 0
	TimeStampBuff.Mas1Status = 0;//pradzioj 0
	
	
	ADS129X_ptr_read = ADS129X_RAM_start; //rodykles pradine reiksme
	ADS129X_ptr_write = ADS129X_RAM_start; //rodykles pradine reiksme
	
	AFE4490A_ptr_read = AFE4490A_RAM_start;//rodykles pradine reiksme
	AFE4490A_ptr_write = AFE4490A_RAM_start;//rodykles pradine reiksme
	
	AFE4490B_ptr_read = AFE4490B_RAM_start;//rodykles pradine reiksme
	AFE4490B_ptr_write = AFE4490B_RAM_start;//rodykles pradine reiksme
	
	AFE4490C_ptr_read = AFE4490C_RAM_start;//rodykles pradine reiksme
	AFE4490C_ptr_write = AFE4490C_RAM_start;//rodykles pradine reiksme
	
	AFE4490A_paskutinis_mas = 0xFF;//Pradine reiksme per didele, kodas paskui sutvarko
	AFE4490B_paskutinis_mas = 0xFF;//Pradine reiksme per didele, kodas paskui sutvarko
	AFE4490C_paskutinis_mas = 0xFF;//Pradine reiksme per didele, kodas paskui sutvarko
	ADS129X_paskutinis_mas = 0xFF;//I have no idea what I'm  doing
	
	AFE4490A_einamas_mas = 0;//Pradeda nuo 0linio masyvo
	AFE4490B_einamas_mas = 0;//Pradeda nuo 0linio masyvo
	AFE4490C_einamas_mas = 0;//Pradeda nuo 0linio masyvo
	ADS129X_einamas_mas = 0;//Pradeda nuo 0linio masyvo
	
	TimerConfigure(TIMER0_BASE,TIMER_CFG_PERIODIC);//Temperaturos lopas skaitliukas
	TimerLoadSet(TIMER0_BASE, TIMER_A, 80000000/200);//Danznis 200 Hz
	TimerIntEnable(TIMER0_BASE,TIMER_TIMA_TIMEOUT);//timeout pertrauktis
	TimerIntRegister(TIMER0_BASE,TIMER_A,temperaturos_tick);//aptarnavimo funkcija
	
	TimerConfigure(TIMER1_BASE,TIMER_CFG_PERIODIC);//SD skaitliukas
	TimerLoadSet(TIMER1_BASE, TIMER_A, 80000000/100);//Daznis 100 Hz
	TimerIntEnable(TIMER1_BASE,TIMER_TIMA_TIMEOUT);//Timeout pertrauktis
	TimerIntRegister(TIMER1_BASE,TIMER_A,SD_tick);//aptarnavimo funkcija
	TimerEnable(TIMER1_BASE,TIMER_A);//iskarto ijungiam 
	
	GPIOIntRegister(GPIO_PORTA_BASE, PortAIntHandler);
	GPIOIntRegister(GPIO_PORTB_BASE, PortBIntHandler);
	GPIOIntRegister(GPIO_PORTE_BASE, PortEIntHandler);
	
	
	GPIOPinTypeGPIOInput(AFE4490A_ADC_RDY_BASE,AFE4490A_ADC_RDY_PIN);
	GPIOIntTypeSet(AFE4490A_ADC_RDY_BASE, AFE4490A_ADC_RDY_PIN, GPIO_RISING_EDGE);
	
	GPIOPinTypeGPIOInput(AFE4490B_ADC_RDY_BASE,AFE4490B_ADC_RDY_PIN);
	GPIOIntTypeSet(AFE4490B_ADC_RDY_BASE, AFE4490B_ADC_RDY_PIN, GPIO_RISING_EDGE);
	
	GPIOPinTypeGPIOInput(AFE4490C_ADC_RDY_BASE,AFE4490C_ADC_RDY_PIN);
	GPIOIntTypeSet(AFE4490C_ADC_RDY_BASE, AFE4490C_ADC_RDY_PIN, GPIO_RISING_EDGE);
	
	//GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE,GPIO_PIN_1);
	//GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_1, 0x00);
	
	SSIIntRegister(SSI1_BASE, SSI1Handler); //SSI1 pertrauktis - ramai
	SSIIntRegister(SSI3_BASE, SSI3Handler); 
	
	
	
	
	AFE4490_hw_init();
	
	AFE4490B_hiZ_OFF;
	AFE4490A_hiZ_OFF;
	AFE4490C_hiZ_OFF;
	
	
	AFE4490ABC_reg_init();
	
	RAM_hw_init();
	
	ADS1296r_init(&ADS1294R_reg);
	//RR_Init();
	 
	//LSM330DLC_init();
	
	do
	{
		LedON;
		Result = f_mount(&FileSystem,"",1);
		LedBlinkCnt++;
		if(LedBlinkCnt > 100)
		{
			HibernateRequest();
			while(1);
		}
	}while(Result != FR_OK);
	
	skaitliukas = 1;
	LedBlinkCnt = 0;
	do
	{
		LedON;
		sprintf(FileName,"Data%u.bin",skaitliukas);
		Result = f_open(&DataFile,FileName,FA_WRITE | FA_CREATE_NEW);
		if(Result == FR_EXIST)
		{
			skaitliukas++;
		}else
		{
			LedBlinkCnt++; //Jeigu yra toks failo pavadinimas nepliusuojam
		}
		if(LedBlinkCnt > 100)
		{
			HibernateRequest();
			while(1);
		}
	}while(Result != FR_OK);
	
	
	
	LedOFF;
	LedBlinkCnt = 0;
	
	do
	{
		tmp = AFE4490A_diag();
		LedBlink(2000000);
		LedBlinkCnt++;
		if(LedBlinkCnt > 20)
		{
			HibernateRequest();
		}
	}while(tmp != 0);
	
	LedOFF;
	LedBlinkCnt = 0;
	
	do
	{
		tmp = AFE4490B_diag();
		LedBlink(2000000);
		LedBlinkCnt++;
		if(LedBlinkCnt > 20)
		{
			HibernateRequest();
		}
	}while(tmp != 0);
	
	LedOFF;
	LedBlinkCnt = 0;
	
	do
	{
		tmp = AFE4490C_diag();
		LedBlink(2000000);
		LedBlinkCnt++;
		if(LedBlinkCnt > 20)
		{
			HibernateRequest();
		}
	}while(tmp != 0);
	
	LedOFF;
	LedBlinkCnt = 0;
	
	AFE4490ABC_reg_init();
	
	
	os_sys_init_prio(Temperaturos_matavimas,255);
	
	
	while(1);
	while(1)
	{
		
		
	}
}

__task void Temperaturos_matavimas (void)
{
	
	
	
	ID_Temperaturos_matavimas = os_tsk_self();
	os_mbx_init (&RAM_mb, sizeof(RAM_mb));
	os_mbx_init(&SD_mb, sizeof(SD_mb));
	os_mbx_init(&Temp_mb, sizeof(Temp_mb));
	os_mbx_init(&Events_mb,sizeof(Events_mb));
	os_mbx_init(&TimeStampMb,sizeof(TimeStampMb));
	os_mbx_init(&HrMb,sizeof(HrMb));
	
	os_mut_init (&MUT_SSI2); //EKG,temperatura ir kiti dalykai
	
	ID_RAM_worker = os_tsk_create(RAM_worker,4);
	ID_SD_worker = os_tsk_create(SD_worker,3);
	ID_DuomenuSurinkimas = os_tsk_create(DuomenuSurinkimas,6);
	ID_HrSkaiciavimas = os_tsk_create(HrSkaiciavimas,5);
	ID_BatCheck = os_tsk_create(BatCheck,2);
	
	
	os_tsk_prio_self(6);
	
	
	TimerEnable(TIMER0_BASE,TIMER_A);
	
	
	
	while(1)
	{
		os_evt_wait_or(0x0001,0xFFFF);
		
		os_mut_wait(&MUT_SSI2,0xFFFF);
		//GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_1, 0xFF);
		os_mut_release(&MUT_SSI2);
		temperaturos_spi_init();
		
		
	}
	
}



__task void RAM_worker(void)
{
	void *msg;
	uint32_t dummy;
	
	
	while(1)
	{
		os_mbx_wait (RAM_mb, &msg, 0xffff);
		
		dummy = (uint32_t) msg;
		dummy = dummy;
		
		
		switch (dummy)
		{
			//AFE4490A
			case 0xA0000000:
				RAM_array_DMA_Wr(AFE4490A_ptr_write,(uint8_t*)&AFE4490A_DataBuff.AFE4490_Mas0[0],128);
				AFE4490A_ptr_write += 128;
				if(AFE4490A_ptr_write == AFE4490A_ptr_read)
					{
						ovf_detect = 1;
					}
				if(AFE4490A_ptr_write == AFE4490A_RAM_end)
					{
						AFE4490A_ptr_write = AFE4490A_RAM_start;
					}
				AFE4490A_paskutinis_mas = 0;
				break;
			case 0xA0000001:
				RAM_array_DMA_Wr(AFE4490A_ptr_write,(uint8_t*)&AFE4490A_DataBuff.AFE4490_Mas1[0],128);
				AFE4490A_ptr_write += 128;
				if(AFE4490A_ptr_write == AFE4490A_ptr_read)
					{
						ovf_detect = 1;
					}
				if(AFE4490A_ptr_write == AFE4490A_RAM_end)
					{
						AFE4490A_ptr_write = AFE4490A_RAM_start;
					}
				AFE4490A_paskutinis_mas = 1;
				break;
			case 0xA0000002:
				RAM_array_DMA_Wr(AFE4490A_ptr_write,(uint8_t*)&AFE4490A_DataBuff.AFE4490_Mas2[0],128);
				AFE4490A_ptr_write += 128;
				if(AFE4490A_ptr_write == AFE4490A_ptr_read)
					{
						ovf_detect = 1;
					}
				if(AFE4490A_ptr_write == AFE4490A_RAM_end)
					{
						AFE4490A_ptr_write = AFE4490A_RAM_start;
					}
				AFE4490A_paskutinis_mas = 2;
				break;
			//AFE4490B	
			case 0xB0000000:
				RAM_array_DMA_Wr(AFE4490B_ptr_write,(uint8_t*)&AFE4490B_DataBuff.AFE4490_Mas0[0],128);
				AFE4490B_ptr_write += 128;
				if(AFE4490B_ptr_write == AFE4490B_ptr_read)
					{
						ovf_detect = 1;
					}
				if(AFE4490B_ptr_write == AFE4490B_RAM_end)
				{
					AFE4490B_ptr_write = AFE4490B_RAM_start;
				}
				AFE4490B_paskutinis_mas = 0;
				break;
			case 0xB0000001:
				RAM_array_DMA_Wr(AFE4490B_ptr_write,(uint8_t*)&AFE4490B_DataBuff.AFE4490_Mas1[0],128);
				AFE4490B_ptr_write += 128;
				if(AFE4490B_ptr_write == AFE4490B_ptr_read)
					{
						ovf_detect = 1;
					}
				if(AFE4490B_ptr_write == AFE4490B_RAM_end)
					{
						AFE4490B_ptr_write = AFE4490B_RAM_start;
					}
				AFE4490B_paskutinis_mas = 1;
				break;
			case 0xB0000002:
				RAM_array_DMA_Wr(AFE4490B_ptr_write,(uint8_t*)&AFE4490B_DataBuff.AFE4490_Mas2[0],128);
				AFE4490B_ptr_write += 128;
				if(AFE4490B_ptr_write == AFE4490B_ptr_read)
					{
						ovf_detect = 1;
					}
				if(AFE4490B_ptr_write == AFE4490B_RAM_end)
					{
						AFE4490B_ptr_write = AFE4490B_RAM_start;
					}
				AFE4490B_paskutinis_mas = 2;
				break;
				//AFE4490C	
			case 0xC0000000:
				RAM_array_DMA_Wr(AFE4490C_ptr_write,(uint8_t*)&AFE4490C_DataBuff.AFE4490_Mas0[0],128);
				AFE4490C_ptr_write += 128;
				if(AFE4490C_ptr_write == AFE4490C_ptr_read)
					{
						ovf_detect = 1;
					}
				if(AFE4490C_ptr_write == AFE4490C_RAM_end)
					{
						AFE4490C_ptr_write = AFE4490C_RAM_start;
					}
				AFE4490C_paskutinis_mas = 0;
				break;
			case 0xC0000001:
				RAM_array_DMA_Wr(AFE4490C_ptr_write,(uint8_t*)&AFE4490C_DataBuff.AFE4490_Mas1[0],128);
				AFE4490C_ptr_write += 128;
				if(AFE4490C_ptr_write == AFE4490C_ptr_read)
					{
						ovf_detect = 1;
					}
				if(AFE4490C_ptr_write == AFE4490C_RAM_end)
					{
						AFE4490C_ptr_write = AFE4490C_RAM_start;
					}
				AFE4490C_paskutinis_mas = 1;
				break;
			case 0xC0000002:
				RAM_array_DMA_Wr(AFE4490C_ptr_write,(uint8_t*)&AFE4490C_DataBuff.AFE4490_Mas2[0],128);
				AFE4490C_ptr_write += 128;
				if(AFE4490C_ptr_write == AFE4490C_ptr_read)
					{
						ovf_detect = 1;
					}
				if(AFE4490C_ptr_write == AFE4490C_RAM_end)
					{
						AFE4490C_ptr_write = AFE4490C_RAM_start;
					}
				AFE4490C_paskutinis_mas = 2;
				break;
					//ADS129X
			case 0xE0000000:
				RAM_array_DMA_Wr(ADS129X_ptr_write,(uint8_t*)&ADS1296R_DataBuff.ADS1296R_Mas0[0],128);
				ADS129X_ptr_write += 128;
				if(ADS129X_ptr_write == ADS129X_ptr_read)
					{
						ovf_detect = 1;
					}
				if(ADS129X_ptr_write == ADS129X_RAM_end)
					{
						ADS129X_ptr_write = ADS129X_RAM_start;
					}
				ADS129X_paskutinis_mas = 0;
				break;
			case 0xE0000001:
				RAM_array_DMA_Wr(ADS129X_ptr_write,(uint8_t*)&ADS1296R_DataBuff.ADS1296R_Mas1[0],128);
				ADS129X_ptr_write += 128;
				if(ADS129X_ptr_write == ADS129X_ptr_read)
					{
						ovf_detect = 1;
					}
				if(ADS129X_ptr_write == ADS129X_RAM_end)
					{
						ADS129X_ptr_write = ADS129X_RAM_start;
					}
				ADS129X_paskutinis_mas = 1;
				break;
			case 0xE0000002:
				RAM_array_DMA_Wr(ADS129X_ptr_write,(uint8_t*)&ADS1296R_DataBuff.ADS1296R_Mas2[0],128);
				ADS129X_ptr_write += 128;
				if(ADS129X_ptr_write == ADS129X_ptr_read)
					{
						ovf_detect = 1;
					}
				if(ADS129X_ptr_write == ADS129X_RAM_end)
					{
						ADS129X_ptr_write = ADS129X_RAM_start;
					}
				ADS129X_paskutinis_mas = 2;
				break;
					
			case 0xDA000000:	//AFE4490A readas SD kortelei
				//GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_1, 0xFF);
				RAM_array_Read(AFE4490A_ptr_read,(uint8_t*)&SD_darbinis_mas0[0],128);	//ciumpam pirma bloka
				AFE4490A_ptr_read += 128;
				if(AFE4490A_ptr_read == AFE4490A_RAM_end)
				{
					AFE4490A_ptr_read = AFE4490A_RAM_start;
				}
				
				
				RAM_array_Read(AFE4490A_ptr_read,(uint8_t*)&SD_darbinis_mas0[128],128); //ciumpam antra bloka
				AFE4490A_ptr_read += 128;
				if(AFE4490A_ptr_read == AFE4490A_RAM_end)
				{
					AFE4490A_ptr_read = AFE4490A_RAM_start;
				}
				
				RAM_array_Read(AFE4490A_ptr_read,(uint8_t*)&SD_darbinis_mas0[256],128); //ciumpam trecia bloka
				AFE4490A_ptr_read += 128;
				if(AFE4490A_ptr_read == AFE4490A_RAM_end)
				{
					AFE4490A_ptr_read = AFE4490A_RAM_start;
				}
				
				RAM_array_Read(AFE4490A_ptr_read,(uint8_t*)&SD_darbinis_mas0[384],128); //ciumpam ketvirta bloka
				AFE4490A_ptr_read += 128;
				if(AFE4490A_ptr_read == AFE4490A_RAM_end)
				{
					AFE4490A_ptr_read = AFE4490A_RAM_start;
				}
				
				os_mbx_send(SD_mb,(uint32_t*)0xDA000000,0xFFFF);
				break;
			case 0xDB000000:	//AFE4490B readas SD kortelei
				RAM_array_Read(AFE4490B_ptr_read,(uint8_t*)&SD_darbinis_mas0[0],128);	//ciumpam pirma bloka
				AFE4490B_ptr_read += 128;
				if(AFE4490B_ptr_read == AFE4490B_RAM_end)
				{
					AFE4490B_ptr_read = AFE4490B_RAM_start;
				}
				
				RAM_array_Read(AFE4490B_ptr_read,(uint8_t*)&SD_darbinis_mas0[128],128); //ciumpam antra bloka
				AFE4490B_ptr_read += 128;
				if(AFE4490B_ptr_read == AFE4490B_RAM_end)
				{
					AFE4490B_ptr_read = AFE4490B_RAM_start;
				}
				
				RAM_array_Read(AFE4490B_ptr_read,(uint8_t*)&SD_darbinis_mas0[256],128); //ciumpam trecia bloka
				AFE4490B_ptr_read += 128;
				if(AFE4490B_ptr_read == AFE4490B_RAM_end)
				{
					AFE4490B_ptr_read = AFE4490B_RAM_start;
				}
				
				RAM_array_Read(AFE4490B_ptr_read,(uint8_t*)&SD_darbinis_mas0[384],128); //ciumpam ketvirta bloka
				AFE4490B_ptr_read += 128;
				if(AFE4490B_ptr_read == AFE4490B_RAM_end)
				{
					AFE4490B_ptr_read = AFE4490B_RAM_start;
				}
				
				os_mbx_send(SD_mb,(uint32_t*)0xDB000000,0xFFFF);
				break;
				case 0xDC000000:	//AFE4490C readas SD kortelei
					RAM_array_Read(AFE4490C_ptr_read,(uint8_t*)&SD_darbinis_mas0[0],128);	//ciumpam pirma bloka
					AFE4490C_ptr_read += 128;
					if(AFE4490C_ptr_read == AFE4490C_RAM_end)
					{
						AFE4490C_ptr_read = AFE4490C_RAM_start;
					}
					
					RAM_array_Read(AFE4490C_ptr_read,(uint8_t*)&SD_darbinis_mas0[128],128); //ciumpam antra bloka
					AFE4490C_ptr_read += 128;
					if(AFE4490C_ptr_read == AFE4490C_RAM_end)
					{
						AFE4490C_ptr_read = AFE4490C_RAM_start;
					}
					
					RAM_array_Read(AFE4490C_ptr_read,(uint8_t*)&SD_darbinis_mas0[256],128); //ciumpam trecia bloka
					AFE4490C_ptr_read += 128;
					if(AFE4490C_ptr_read == AFE4490C_RAM_end)
					{
						AFE4490C_ptr_read = AFE4490C_RAM_start;
					}
					
					RAM_array_Read(AFE4490C_ptr_read,(uint8_t*)&SD_darbinis_mas0[384],128); //ciumpam ketvirta bloka
					AFE4490C_ptr_read += 128;
					if(AFE4490C_ptr_read == AFE4490C_RAM_end)
					{
						AFE4490C_ptr_read = AFE4490C_RAM_start;
					}
					
					os_mbx_send(SD_mb,(uint32_t*)0xDC000000,0xFFFF);
				break;
					case 0xDE000000:	//ADS129X readas SD kortelei
					RAM_array_Read(ADS129X_ptr_read,(uint8_t*)&SD_darbinis_mas0[0],128);	//ciumpam pirma bloka
					ADS129X_ptr_read += 128;
					if(ADS129X_ptr_read == ADS129X_RAM_end)
					{
						ADS129X_ptr_read = ADS129X_RAM_start;
					}
					
					RAM_array_Read(ADS129X_ptr_read,(uint8_t*)&SD_darbinis_mas0[128],128); //ciumpam antra bloka
					ADS129X_ptr_read += 128;
					if(ADS129X_ptr_read == ADS129X_RAM_end)
					{
						ADS129X_ptr_read = ADS129X_RAM_start;
					}
					
					RAM_array_Read(ADS129X_ptr_read,(uint8_t*)&SD_darbinis_mas0[256],128); //ciumpam trecia bloka
					ADS129X_ptr_read += 128;
					if(ADS129X_ptr_read == ADS129X_RAM_end)
					{
						ADS129X_ptr_read = ADS129X_RAM_start;
					}
					
					RAM_array_Read(ADS129X_ptr_read,(uint8_t*)&SD_darbinis_mas0[384],128); //ciumpam ketvirta bloka
					ADS129X_ptr_read += 128;
					if(ADS129X_ptr_read == ADS129X_RAM_end)
					{
						ADS129X_ptr_read = ADS129X_RAM_start;
					}
					
					os_mbx_send(SD_mb,(uint32_t*)0xDE000000,0xFFFF);
				break;
		}
		
	}
}

__task void SD_worker(void)
{
	
	Occupied_t Occupied;
	
	uint8_t status;
	uint16_t kontroles_wr = 0;
	uint16_t i = 0;
	uint8_t WriteResult;
		
	void *msg;
	void *msg2;
	
	
	while(1)
	{
		
		
		if( i > 0)
		{
			if( (i % 8) == 0) //tikrinam ar astuntas kartas
			{
				do
				{
					GetOccupied(&Occupied);
					WriteResult = FileWrite(&DataFile,0xDA000000,Occupied.AFE4490A);
					if(WriteResult != 0)
					{
						os_dly_wait(1);
					}
					
				}while(WriteResult != 0);
				
				do
				{
					GetOccupied(&Occupied);
					WriteResult = FileWrite(&DataFile,0xDB000000,Occupied.AFE4490B);
					if(WriteResult != 0)
					{
						os_dly_wait(1);
					}
					
				}while(WriteResult != 0);
				
				do
				{
					GetOccupied(&Occupied);
					WriteResult = FileWrite(&DataFile,0xDC000000,Occupied.AFE4490C);
					if(WriteResult != 0)
					{
						os_dly_wait(1);
					}
					
				}while(WriteResult != 0);
			}
			if( (i % 100) == 0)
			{	//IMU irasymas
				do
				{
					GetOccupied(&Occupied);
					if(Occupied.ImuMas0Status)
					{
						WriteResult = FileWrite(&DataFile,0x000100A0,512);
					}else if(Occupied.ImuMas1Status)
					{
						WriteResult = FileWrite(&DataFile,0x000100A1,512);
					}else
					{
						WriteResult = 1;
					}
					
					if(WriteResult != 0)
					{
						os_dly_wait(1);
					}
					
				}while(WriteResult != 0);
				
			}
			if ( (i % 800) == 0)
			{
				//temperaturos irasymas
				do
				{
					GetOccupied(&Occupied);
					if(Occupied.TempMas0Status)
					{
						WriteResult = FileWrite(&DataFile,0x000100E0,512);
					}else if(Occupied.TempMas1Status)
					{
						WriteResult = FileWrite(&DataFile,0x000100E1,512);
					}else
					{
						WriteResult = 1;
					}
					
					if(WriteResult != 0)
					{
						os_dly_wait(1);
					}
					
				}while(WriteResult != 0);
				//TimeStampu irasymas
				
				do
				{
					GetOccupied(&Occupied);
					if(Occupied.TimeStamp0Status)
					{
						WriteResult = FileWrite(&DataFile,0x000100F0,512);
					}else if(Occupied.TimeStamp1Status)
					{
						WriteResult = FileWrite(&DataFile,0x000100F1,512);
					}else
					{
						WriteResult = 1;
					}
					
					if(WriteResult != 0)
					{
						os_dly_wait(1);
					}
					
				}while(WriteResult != 0);
				i = 0;
			}
			/*if ( (i % 500) == 0)
			{
				//TimeStampuIrasymas
				do
				{
					GetOccupied(&Occupied);
					if(Occupied.TimeStamp0Status)
					{
						//WriteResult = FileWrite(&DataFile,0x000100E0,512);
					}else if(Occupied.TimeStamp0Status)
					{
						//WriteResult = FileWrite(&DataFile,0x000100E1,512);
					}else
					{
						WriteResult = 1;
					}
					
					if(WriteResult != 0)
					{
						os_dly_wait(1);
					}
					
				}while(WriteResult != 0);
				i = 0;
			}*/
		}
		do
		{
			GetOccupied(&Occupied);
			WriteResult = FileWrite(&DataFile,0xDE000000,Occupied.ADS1294R);
			if(WriteResult != 0)
			{
				os_dly_wait(1);
			}
		}while(WriteResult != 0);
		i++;
		//ECG irasymas jeigu pavyksta pliusuojam i
		
		
		kontroles_wr++;
		
		if(kontroles_wr > 50)
		{
			kontroles_wr = 0;
			f_sync(&DataFile);
			if(os_mbx_check(Events_mb) <= 3)
			{
				os_mbx_wait (Events_mb, &msg2, 0xffff);
				
				switch((uint32_t)msg2)
				{
					case 0x00000001:
						f_close(&DataFile);
						f_mount(NULL,"",0);
						while(MagSwON)
						{
							BoardMinimumInit(); //Checkina baterijos krovima
						}
						HibernateRequest();
						while(1);
						break;
					case 0x00000002:
						f_close(&DataFile);
						f_mount(NULL,"",0);
						SysCtlReset();
						break;
				}
			}
		}
		
	}
	
}


__task void DuomenuSurinkimas(void)
{
	ADS1296_RDATA_result_t ADS1296R_Data;
	AFE4490_Result_t AFE4490A_Data;
	AFE4490_Result_t AFE4490B_Data;
	AFE4490_Result_t AFE4490C_Data;
	LSM330DLC_AccelData_t AccelData;
	LSM330DLC_GyroData_t GyroData;
	Temp_Data_t TempData;
	
	uint32_t FakeData = 0;
	uint8_t PpgCounter = 0;
	uint8_t ImuCounter = 0;
	uint8_t TempCounter = 0;
	uint16_t TimeStampCounter = 0;
	uint8_t RRcounter = 0;
	
	DataCnt_t DataCnt;
	
	void *msg;
	
	
	DataCnt.EcgDataCnt = 0;
	DataCnt.ImuDataCnt = 0;
	DataCnt.PpgDataCnt = 0;
	DataCnt.TempDataCnt = 0;
	
	TempData.Temp1 = 0;
	TempData.Temp2 = 0;
	
	TempData.Temp3 = 0;
	TempData.Temp4 = 0;
	
	GPIOIntEnable(GPIO_PORTB_BASE, ADS129X_DRDY_PIN); //ijungiam ECG interupta
	while(1)
	{
		os_evt_wait_or(0x0001,0xFFFF);
		PpgCounter++;
		ImuCounter++;
		TempCounter++;
		TimeStampCounter++;
		RRcounter++;
		
		if(ImuCounter == 40) //50Hz
		{
			os_mut_wait(&MUT_SSI2,0xFFFF);
			
			ADS129X_SPI_init(20000000);
			ADS1296_DataRead(&ADS1296R_Data);
			ADS1296R_Data.CH1 = FakeData;
			FakeData++;
			ADS1296_DataPut(&ADS1296R_Data,&ADS1296R_DataBuff, RAM_mb);
			DataCnt.EcgDataCnt++;
			
			
			AccelData.X = 0;
			AccelData.Y = 0;
			AccelData.Z = 0;
			GyroData.Pitch = 0;
			GyroData.Roll = 0;
			GyroData.Yaw = 0;
			LSM330DLC_DataPut(&AccelData,&GyroData,&ImuDataBuff);
			DataCnt.ImuDataCnt++;
			ImuCounter = 0;
			os_mut_release(&MUT_SSI2);
		}else //2kHz
		{
			os_mut_wait(&MUT_SSI2,0xFFFF);
			ADS129X_SPI_init(20000000);
			ADS1296_DataRead(&ADS1296R_Data);
			ADS1296R_Data.CH1 = FakeData;
			FakeData++;
			ADS1296_DataPut(&ADS1296R_Data,&ADS1296R_DataBuff, RAM_mb);
			DataCnt.EcgDataCnt++;
			os_mut_release(&MUT_SSI2);
		}
		/*
		if(RRcounter == 4)
		{
			os_mbx_send(HrMb,(void *)ADS1296R_Data.CH3,0xFFFF);
			FakeData = TempData.Temp2;
			os_mbx_send(HrMb,(void *)FakeData,0xFFFF);
			RRcounter = 0;
		}
		*/
		
		if(PpgCounter == 2) //1kHz
		{
			AFE4490A_RdData(&AFE4490A_Data);
			AFE4490A_DataPut(&AFE4490A_Data,&AFE4490A_DataBuff,RAM_mb);
			
			AFE4490B_RdData(&AFE4490B_Data);
			AFE4490B_DataPut(&AFE4490B_Data,&AFE4490B_DataBuff,RAM_mb);
			
			AFE4490C_RdData(&AFE4490C_Data);
			AFE4490C_DataPut(&AFE4490C_Data,&AFE4490C_DataBuff,RAM_mb);
			
			DataCnt.PpgDataCnt++;
			PpgCounter = 0;
		}
		/*
		if(os_mbx_check(Temp_mb) <= 4) //Pasiimam asinchroninius duomenis
		{
			
			TempData.Temp1 = 0;
			TempData.Temp2 = 0;
			
			TempData.Temp3 = 0;
			TempData.Temp4 = 0;
		}
		*/
		
		
		if(TempCounter == 200) //10 Hz
		{
			TempDataPut(&TempData,&TempDataBuff);
			DataCnt.TempDataCnt++;
			TempCounter = 0;
		}
		//GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_1, 0x00);
		
		if(os_mbx_check(TimeStampMb) < 4)
		{
			os_mbx_wait (TimeStampMb, &msg, 0xffff);
			TimeStampDataPut(&DataCnt,&TimeStampBuff);
		}
		if(TimeStampCounter == 12800)//6400 ms
		{
			TimeStampCounter = 0;
			TimeStampBuff.Cnt = 1;
			switch(TimeStampBuff.ArrCnt)
			{
				case 0:
					TimeStampBuff.Mas0Status = 1;
					TimeStampBuff.Mas1[0] = 0;
				break;
				case 1:
					TimeStampBuff.Mas1Status = 1;
					TimeStampBuff.Mas0[0] = 0;
				break;
			}
			TimeStampBuff.ArrCnt++;
			if(TimeStampBuff.ArrCnt == 2)
			{
				TimeStampBuff.ArrCnt = 0;
			}
		}
	}
}

__task void HrSkaiciavimas(void)
{
	void *msg;
	int32_t dummy;
	uint32_t dummy2;
	uint8_t HR, tmp;
	uint16_t HR2,RRi;
	uint16_t Temperatura;
	char tekstas[20];
	
	uint32_t OsTime, LastSentTime, LastHrTime;
	
	OsTime = 0;
	LastSentTime = 0;
	LastHrTime = 0;
	Temperatura = 25;
	
	while(1)
	{
		os_mbx_wait (HrMb, &msg, 0xffff);
		
		dummy = (int32_t) msg;
		dummy = dummy;
		
		//tmp = RRrecognition(dummy/4,1);
		tmp = RR_atpazinimas(dummy,&RRi,&HR2);
		OsTime = os_time_get();
		
		os_mbx_wait (HrMb, &msg, 0xffff);
		dummy2 = (uint32_t) msg;
		dummy2 = dummy2;
		Temperatura = dummy2;
		
		if(tmp == 1)
		{
			HR = tmp;
			LastHrTime = OsTime;
		}
		
		
		if( (OsTime - LastSentTime) >= 1000)
		{
			LastSentTime = OsTime;
			if( (OsTime - LastHrTime) >=2000) //Seniai rado RR intervala
			{
				sprintf(&tekstas[0],"#@%d?!%u",HR2,Temperatura);
				UARTPrint(UART1_BASE,(uint8_t*)&tekstas[0]);
				UARTPrint(UART1_BASE,(uint8_t*)"\n\r");
			}else //HR verte jaunesne nei 2s
			{
				sprintf(&tekstas[0],"#@%d!%u",HR2,Temperatura);
				UARTPrint(UART1_BASE,(uint8_t*)&tekstas[0]);
				UARTPrint(UART1_BASE,(uint8_t*)"\n\r");
				
			}
		}
		
	}
}

__task void BatCheck(void)
{
	uint32_t Rezultatas = 0;
	uint32_t LowBatCnt = 0;
	while(1)
	{
		os_dly_wait(100);
		ADCProcessorTrigger(ADC0_BASE, 0);
		while(!ADCIntStatus(ADC0_BASE, 0, false))
		{
		}
		ADCSequenceDataGet(ADC0_BASE, 0, &Rezultatas);
		if(Rezultatas < 2762)
		{
			LowBatCnt++;
		}else
		{
			LowBatCnt = 0;
		}
		
		if(LowBatCnt > 10)
		{
			os_mbx_send (&Events_mb,(void *)0x00000001,0xFFFF);
		}
		
		
	}
}


void AFE4490ABC_reg_init(void)
{
	uint32_t teempe;
	
	pulsoks_param.reg_LED2STC = 0x000BB8; //LED2 sampling start
	pulsoks_param.reg_LED2ENDC = 0x000F9E; //LED2 sampling end
	pulsoks_param.reg_LED2LEDSTC = 0x000BB8;	//LED2 pulse start
	pulsoks_param.reg_LED2LEDENDC = 0x000F9F; //LED2 pulse end
	pulsoks_param.reg_ALED2STC = 0x000000;	//LED2 ambient sampling start
	pulsoks_param.reg_ALED2ENDC = 0x0003E6; //LED2 ambient sampling end
	
	pulsoks_param.reg_LED1STC = 0x0003E8;	//LED1 sampling start
	pulsoks_param.reg_LED1ENDC = 0x0007CE; //LED1 sampling end
	pulsoks_param.reg_LED1LEDSTC = 0x0003E8; //LED1 pulse start
	pulsoks_param.reg_LED1LEDENDC = 0x0007CF; //LED1 pulse end
	pulsoks_param.reg_ALED1STC = 0x0007D0; //LED1 ambient sampling start
	pulsoks_param.reg_ALED1ENDC = 0x000BB6; //LED1 ambient sampling start
	
	pulsoks_param.reg_LED2CONVST = 0x000002; //LED2 conversation start
	pulsoks_param.reg_LED2CONVEND = 0x0003E7; //LED2 conversation end
	pulsoks_param.reg_ALED2CONVST = 0x0003EA; //LED2 ambient conversation start
	pulsoks_param.reg_ALED2CONVEND = 0x0007CF; //LED2 ambient conversation end
	pulsoks_param.reg_LED1CONVST = 0x0007D2; //LED1 conversation start
	pulsoks_param.reg_LED1CONVEND = 0x000BB7; //LED1 conversation end
	pulsoks_param.reg_ALED1CONVST = 0x000BBA; //LED1 ambient conversation start
	pulsoks_param.reg_ALED1CONVEND = 0x000F9F; //LED1 ambient conversation end
	
	pulsoks_param.reg_ADCRSTSTCT0 = 0x000000; //ADC reset 0 start
	pulsoks_param.reg_ADCRSTENDCT0 = 0x000000; //ADC reset 0 end
	pulsoks_param.reg_ADCRSTSTCT1 = 0x0003E8; //ADC reset 1 start
	pulsoks_param.reg_ADCRSTENDCT1 = 0x0003E8; //ADC reset 1 end
	pulsoks_param.reg_ADCRSTSTCT2 = 0x0007D0; //ADC reset 2 start
	pulsoks_param.reg_ADCRSTENDCT2 = 0x0007D0; //ADC reset 2 end
	pulsoks_param.reg_ADCRSTSTCT3 = 0x000BB8; //ADC reset 3 start
	pulsoks_param.reg_ADCRSTENDCT3 = 0x000BB8; //ADC reset 3 end
	
	pulsoks_param.reg_PRPCOUNT = 0x000F9F; //pulse repetition period
	
	pulsoks_param.reg_CONTROL1 = (0 << CONTROL1_CLKALMPIN_pos) | (0 << CONTROL1_TIMEREN_pos) | (1 << CONTROL1_NUMAV_pos); //Timer disabled, 0 averages
	
	pulsoks_param.reg_TIAGAIN = 0;
  //pulsoks_param.reg_TIAGAIN |= (AFE4490_calc_Cf(&pulsoks_param.reg_TIAGAIN,&pulsoks_param.reg_ALED2ENDC) << TIAGAIN_CF_LED1_pos); //suskaiciuojam ir irasom Cf reiksme
	
	pulsoks_param.reg_TIA_AMB_GAIN = 0;
	//pulsoks_param.reg_TIA_AMB_GAIN |= (AFE4490_calc_Cf(&pulsoks_param.reg_TIA_AMB_GAIN,&pulsoks_param.reg_ALED2ENDC) << TIA_AMB_GAIN_CF_LED2_pos); //suskaiciuojam ir irasom Cf reiksme
	
	pulsoks_param.reg_CONTROL2 = 0x020000;
	pulsoks_param.reg_LEDCNTRL = 0x011E1E;
	
	AFE4490A_wr_reg(CONTROL0, 1 << 3); //resetas
	AFE4490_delay_ms(100);
	AFE4490B_wr_reg(CONTROL0, 1 << 3); //resetas
	AFE4490_delay_ms(100);
	AFE4490C_wr_reg(CONTROL0, 1 << 3); //resetas
	AFE4490_delay_ms(100);
	
	AFE4490A_init(&pulsoks_param);
	teempe = AFE4490A_rd_reg(LED2STC);
	AFE4490B_init(&pulsoks_param);
	teempe = AFE4490B_rd_reg(LED2STC);
	AFE4490C_init(&pulsoks_param);
	teempe = AFE4490C_rd_reg(LED2STC);
	
	teempe--;
	
	
}

uint32_t RAM_uzimta(uint32_t wr_ptr, uint32_t rd_ptr, uint32_t start_addr,uint32_t end_addr)
{
	uint32_t uzimta;
	if(wr_ptr >= rd_ptr)
	{
		uzimta = wr_ptr - rd_ptr;
	}else
	{
		uzimta = (wr_ptr - start_addr + end_addr) - rd_ptr;
	}
	return(uzimta);
}

void LedBlink(uint32_t Delay)
{
	LedON;
	SysCtlDelay(Delay);
	LedOFF;
	SysCtlDelay(Delay);
}

uint8_t FileWrite(FIL *file, uint32_t msg, uint32_t uzimta)
{
	uint16_t BytesWritten;
	uint32_t Dummy;
	uint8_t ret = 0;
	FRESULT Result;
	
	
	if(uzimta < 512)
	{
		ret = 1;
	}
	if(msg & 0x00010000)
	{//letu dalyku irasymas i flasha
		switch(msg)
		{//IMU irasymas
			case 0x000100A0:
				Result = f_write(file,(uint8_t *)&ImuDataBuff.Mas0[0],512,(UINT*)&BytesWritten);
				if (Result != FR_OK)
				{
					ret = 2;
				}else if (BytesWritten != 512)
				{
					ret = 3;
				}else
				{
					tsk_lock();
					ImuDataBuff.Mas0Status = 0;
					tsk_unlock();
					ret = 0;
				}
				break;
			case 0x000100A1: //IMU irasymas
				Result = f_write(file,(uint8_t *)&ImuDataBuff.Mas1[0],512,(UINT*)&BytesWritten);
				if (Result != FR_OK)
				{
					ret = 2;
				}else if (BytesWritten != 512)
				{
					ret = 3;
				}else
				{
					tsk_lock();
					ImuDataBuff.Mas1Status = 0;
					tsk_unlock();
					ret = 0;
				}
				break;
			case 0x000100E0: //Temperaturos irasymas
				Result = f_write(file,(uint8_t *)&TempDataBuff.Mas0[0],512,(UINT*)&BytesWritten);
				if (Result != FR_OK)
				{
					ret = 2;
				}else if (BytesWritten != 512)
				{
					ret = 3;
				}else
				{
					tsk_lock();
					TempDataBuff.Mas0Status = 0;
					tsk_unlock();
					ret = 0;
				}
				break;
			case 0x000100E1: //Temperaturos irasymas
				Result = f_write(file,(uint8_t *)&TempDataBuff.Mas1[0],512,(UINT*)&BytesWritten);
				if (Result != FR_OK)
				{
					ret = 2;
				}else if (BytesWritten != 512)
				{
					ret = 3;
				}else
				{
					tsk_lock();
					TempDataBuff.Mas1Status = 0;
					tsk_unlock();
					ret = 0;
				}
				break;
				
				case 0x000100F0: //TimeStampu irasymas
				Result = f_write(file,(uint8_t *)&TimeStampBuff.Mas0[0],512,(UINT*)&BytesWritten);
				if (Result != FR_OK)
				{
					ret = 2;
				}else if (BytesWritten != 512)
				{
					ret = 3;
				}else
				{
					tsk_lock();
					TimeStampBuff.Mas0Status = 0;
					tsk_unlock();
					ret = 0;
				}
				break;
				
				case 0x000100F1: //TimeStampu irasymas
				Result = f_write(file,(uint8_t *)&TimeStampBuff.Mas1[0],512,(UINT*)&BytesWritten);
				if (Result != FR_OK)
				{
					ret = 2;
				}else if (BytesWritten != 512)
				{
					ret = 3;
				}else
				{
					tsk_lock();
					TimeStampBuff.Mas1Status = 0;
					tsk_unlock();
					ret = 0;
				}
				break;
			default:
				break;
		}
		
	}else if (ret == 0)
	{
		os_mbx_send (RAM_mb, (uint32_t*)msg, 0xFFFF);	//liepia ramu taskui nuskaityt ka reikia i SD_darbinis_mas0
		os_mbx_wait (SD_mb, (void *)Dummy, 0xffff); //palaukiam kol is ramu istrauks duomenis
		
		Result = f_write(file,(uint8_t *)&SD_darbinis_mas0[0],512,(UINT*)&BytesWritten);

		if (Result != FR_OK)
		{
			ret = 2;
		}else if (BytesWritten != 512)
		{
			ret = 3;
		}else
		{
			ret = 0;
		}
		
	}
	return ret;
}

void GetOccupied(Occupied_t *Occupied)
{
	uint32_t local_ADS129X_ptr_read, local_ADS129X_ptr_write;
	uint32_t local_AFE4490A_ptr_read, local_AFE4490A_ptr_write;
	uint32_t local_AFE4490B_ptr_read, local_AFE4490B_ptr_write;
	uint32_t local_AFE4490C_ptr_read, local_AFE4490C_ptr_write;
	
	tsk_lock();
	local_ADS129X_ptr_read = ADS129X_ptr_read;
	local_ADS129X_ptr_write = ADS129X_ptr_write;
	local_AFE4490A_ptr_read = AFE4490A_ptr_read;
	local_AFE4490A_ptr_write = AFE4490A_ptr_write;
	local_AFE4490B_ptr_read = AFE4490B_ptr_read;
	local_AFE4490B_ptr_write = AFE4490B_ptr_write;
	local_AFE4490C_ptr_read = AFE4490C_ptr_read;
	local_AFE4490C_ptr_write = AFE4490C_ptr_write;
	Occupied->TempMas0Status = TempDataBuff.Mas0Status;
	Occupied->TempMas1Status = TempDataBuff.Mas1Status;
	Occupied->ImuMas0Status = ImuDataBuff.Mas0Status;
	Occupied->ImuMas1Status = ImuDataBuff.Mas1Status;
	Occupied->TimeStamp0Status = TimeStampBuff.Mas0Status;
	Occupied->TimeStamp1Status = TimeStampBuff.Mas1Status;
	tsk_unlock();
	
	Occupied->ADS1294R = RAM_uzimta(local_ADS129X_ptr_write,local_ADS129X_ptr_read,ADS129X_RAM_start,ADS129X_RAM_end);
	Occupied->AFE4490A = RAM_uzimta(local_AFE4490A_ptr_write,local_AFE4490A_ptr_read,AFE4490A_RAM_start, AFE4490A_RAM_end);
	Occupied->AFE4490B = RAM_uzimta(local_AFE4490B_ptr_write,local_AFE4490B_ptr_read,AFE4490B_RAM_start, AFE4490B_RAM_end);
	Occupied->AFE4490C = RAM_uzimta(local_AFE4490C_ptr_write,local_AFE4490C_ptr_read,AFE4490C_RAM_start, AFE4490C_RAM_end);
}

void temperaturos_tick(void){
	uint8_t MagSw;
	
	
	TimerIntClear(TIMER0_BASE,TIMER_TIMA_TIMEOUT);
	if(SUPPLY_ON)
	{
		isr_mbx_send (&Events_mb,(void *)0x0000002);
		//SysCtlReset();
	}
	
	MagSw = MagSwON;
	if(MagSw)
	{
		MagSwCnt++;
	}else
	{
		if(MagSwCnt > 1200)
		{
			isr_mbx_send (&Events_mb,(void *)0x00000001);
		}else if (MagSwCnt > 200)
		{
			isr_mbx_send(&TimeStampMb,(void *)0x00000001);
		}
		MagSwCnt = 0;
	}
	
	LedBlinkCnt++;
	
	if(MagSw)
	{
		if(MagSwCnt > 1205)
		{
			LedOFF;
		}else if(MagSwCnt > 205)
		{
			LedON;
		}else
		{
			switch(LedBlinkCnt)
			{
				case 1:
					LedON;
					break;
				case 10:
					LedOFF;
					break;
				case 20:
					LedBlinkCnt = 0;
					break;
				default:
					break;
			}
		}
		if(LedBlinkCnt > 21)
		{
			LedBlinkCnt = 0;
		}
		
	}else
	{
		if(SynchroLed == 1)
		{
			LedBlinkCnt = 0;
			LedON;
			SynchroLed = 2;
		}else if(SynchroLed == 2)
		{
			if(LedBlinkCnt > 300)
			{
				SynchroLed = 0;
				LedBlinkCnt = 0;
			}
		}else
		{
			switch(LedBlinkCnt)
			{
				case 1:
					LedON;
					break;
				case 10:
					LedOFF;
					break;
				case 20:
					LedON;
					break;
				case 30:
					LedOFF;
					break;
				case 200:
					LedBlinkCnt = 0;
					break;
				default:
					break;
			}
		}
	}
	
	isr_evt_set(0x0001,ID_Temperaturos_matavimas);
}


void PortAIntHandler(void)
{
	
	
	GPIOIntClear(GPIO_PORTA_BASE,0xFFFFFFFF);
	
}

void PortBIntHandler(void)
{
	uint32_t IntStatus;
	
	IntStatus = GPIOIntStatus(GPIO_PORTB_BASE,1);
	
	GPIOIntClear(GPIO_PORTB_BASE,0xFFFFFFFF);
	
	if(IntStatus & ADS129X_DRDY_PIN)
	{
		isr_evt_set(0x0001,ID_DuomenuSurinkimas);
	}
	
	
}

void PortEIntHandler(void)
{
	
	GPIOIntClear(GPIO_PORTE_BASE,0xFFFFFFFF);
	
}

void SSI1Handler(void)
{
	//SSI1_flegas = 1;
	isr_evt_set(0x00001,ID_RAM_worker);
}
	
void SSI3Handler(void)
{
	//SSI3_flegas = 1;
	isr_evt_set(0x00001,ID_SD_worker);
}

void SD_tick(void)
{
	TimerIntClear(TIMER1_BASE,TIMER_TIMA_TIMEOUT);
	disk_timerproc();
}


