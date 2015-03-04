#include "stdint.h"
#include "stdbool.h"
#include "inc\tm4c123gh6pm.h"
#include "inc\hw_memmap.h"
#include "driverlib\sysctl.h"
#include "driverlib\gpio.h"
#include "driverlib\pin_map.h"
#include "driverlib\ssi.h"
#include "driverlib\timer.h"
#include "driverlib\uart.h"
#include "driverlib\adc.h"
#include "driverlib\hibernate.h"
#include "board.h"

void board_init(void)
{
	uint32_t BaterijosMorkos, Rezultatas;
	uint32_t cnt;
	
	SysCtlClockSet(SYSCTL_USE_PLL | SYSCTL_MAIN_OSC_DIS | SYSCTL_OSC_INT | SYSCTL_SYSDIV_2_5); //uzsiklokinam 80 MHz
	
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);	//ijungiam E porta
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);	//ijungiam C porta
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);	//ijungiam F porta
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);	//ijungiam D porta
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);	//ijungiam B porta
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);	//ijungiam A porta
	SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI2);		//CLK i SSI2
	SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI0);		//CLK i SSI0
	SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI1);		//CLK i SSI1
	SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI3);		//CLK i SSI1
	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);		//CLK i TIMER0
	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1);		//CLK i TIMER1
	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART1);		//CLK i UART1
	SysCtlPeripheralEnable(SYSCTL_PERIPH_UDMA);		//CLK i uDMA
	SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);		//CLK i uDMA
	
	
	//SUPPLY_SNS pin'o konfiguracija
	GPIOPinTypeGPIOInput(SUPPLY_SNS_BASE, SUPPLY_SNS_PIN);	//nusistaom kaip iejima
	GPIOPadConfigSet(SUPPLY_SNS_BASE, SUPPLY_SNS_PIN, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);	//ijungiam pullup
	//CHG_STATUS pin'o konfiguracija
	GPIOPinTypeGPIOInput(CHG_STATUS_BASE, CHG_STATUS_PIN);	//nusistaom kaip iejima
	GPIOPadConfigSet(CHG_STATUS_BASE, CHG_STATUS_PIN, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);	//ijungiam pullup
	//T_CS pin'o konfiguracija
	
	
	//Mag SW
	GPIOPinTypeGPIOInput(MagSwBase, MagSwPin);	//nusistaom kaip iejima
	GPIOPadConfigSet(MagSwBase, MagSwPin, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);	//ijungiam pullup
	
	//LED
	GPIOPinTypeGPIOOutput(LedBase,LedPin);
	LedOFF;
	//SdMode
	GPIOPinTypeGPIOOutput(SdModeBase,SdModePin);
	SdModeLow;
	//ExternalSync
	
	
	/*
	GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE,GPIO_PIN_4);
	while(1)
	{
		GPIOPinWrite(GPIO_PORTB_BASE,GPIO_PIN_4,0x00);
		GPIOPinWrite(GPIO_PORTB_BASE,GPIO_PIN_4,0xFF);
	}
	*/
	//SSI2 konfiguracija (EKG, ACC/GYRO, T)
	
	GPIOPinTypeSSI(GPIO_PORTB_BASE, GPIO_PIN_4 | GPIO_PIN_6 | GPIO_PIN_7); //SSI function to GPIO
	
	GPIOPinConfigure(GPIO_PB4_SSI2CLK);
	GPIOPinConfigure(GPIO_PB6_SSI2RX);
	GPIOPinConfigure(GPIO_PB7_SSI2TX);
	SSIDisable(SSI2_BASE);	//Disable for config
	SSIConfigSetExpClk(SSI2_BASE, SysCtlClockGet(), SSI_FRF_MOTO_MODE_2,SSI_MODE_MASTER, 2500000, 8);	//Config SSI1
	SSIEnable(SSI2_BASE);//Enable for communication
	
	
	UARTDisable(UART1_BASE);	//Disable for config
	UARTConfigSetExpClk(UART1_BASE, SysCtlClockGet(), 115000, (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE) );	//Config UART1
	UARTEnable(UART1_BASE);//Enable for communication
	
	GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_3);
	
	ADCSequenceConfigure(ADC0_BASE, 0, ADC_TRIGGER_PROCESSOR, 0);
	ADCSequenceStepConfigure(ADC0_BASE, 0, 0, ADC_CTL_IE | ADC_CTL_END | ADC_CTL_CH0);
	ADCSequenceEnable(ADC0_BASE, 0);
	
	BaterijosMorkos = 0;
	for(cnt = 0; cnt<128;cnt++)
	{
		ADCProcessorTrigger(ADC0_BASE, 0);
		while(!ADCIntStatus(ADC0_BASE, 0, false))
		{
		}
		ADCSequenceDataGet(ADC0_BASE, 0, &Rezultatas);
		BaterijosMorkos += Rezultatas;
	}
	BaterijosMorkos /= 128;
	if(BaterijosMorkos < 2762)
	{
		while(MagSwON)//ar pridetas magnetas
		{
			BoardMinimumInit(); //Checkina baterijos krovima
		}
		HibernateRequest();//liepa Hibernate moduliui isjungti maitinima
	}
	
	
}	

void BoardMinimumInit(void)
{
	uint32_t Rezultatas;
	
	SysCtlClockSet(SYSCTL_USE_PLL | SYSCTL_MAIN_OSC_DIS | SYSCTL_OSC_INT | SYSCTL_SYSDIV_12_5); //uzsiklokinam 80 MHz
	
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);	//ijungiam E porta
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);	//ijungiam C porta
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);	//ijungiam B porta
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);	//ijungiam C porta
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);	//ijungiam C porta
	SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);		//CLK i ADC0
	
	//SUPPLY_SNS pin'o konfiguracija
	GPIOPinTypeGPIOInput(SUPPLY_SNS_BASE, SUPPLY_SNS_PIN);	//nusistaom kaip iejima
	GPIOPadConfigSet(SUPPLY_SNS_BASE, SUPPLY_SNS_PIN, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);	//ijungiam pullup
	//CHG_STATUS pin'o konfiguracija
	GPIOPinTypeGPIOInput(CHG_STATUS_BASE, CHG_STATUS_PIN);	//nusistaom kaip iejima
	GPIOPadConfigSet(CHG_STATUS_BASE, CHG_STATUS_PIN, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);	//ijungiam pullup
	//LED
	GPIOPinTypeGPIOOutput(LedBase,LedPin);
	LedOFF;
	//SdMode
	GPIOPinTypeGPIOOutput(SdModeBase,SdModePin);
	SdModeLow;
	//Mag SW
	GPIOPinTypeGPIOInput(MagSwBase, MagSwPin);	//nusistaom kaip iejima
	GPIOPadConfigSet(MagSwBase, MagSwPin, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);	//ijungiam pullup
	
	GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_3);
	
	ADCSequenceConfigure(ADC0_BASE, 0, ADC_TRIGGER_PROCESSOR, 0);
	ADCSequenceStepConfigure(ADC0_BASE, 0, 0, ADC_CTL_IE | ADC_CTL_END | ADC_CTL_CH0);
	ADCSequenceEnable(ADC0_BASE, 0);
	
	while(SUPPLY_ON)
	{
		SdModeHi;
		if(CHARGING)
		{
			LedON;
			SysCtlDelay(2000000);
			LedOFF;
			SysCtlDelay(2000000);
			
			ADCProcessorTrigger(ADC0_BASE, 0);
			while(!ADCIntStatus(ADC0_BASE, 0, false))
			{
			}
			ADCSequenceDataGet(ADC0_BASE, 0, &Rezultatas);
			
			Rezultatas = Rezultatas;
		
		}else
		{
			LedON;
		}
	}
	SdModeLow;
}

void temperaturos_spi_init(void)
{
	SSIDisable(SSI2_BASE);	//Disable for config
	SSIConfigSetExpClk(SSI2_BASE, SysCtlClockGet(), SSI_FRF_MOTO_MODE_2,SSI_MODE_MASTER, 2500000, 8);	//Config SSI1
	SSIEnable(SSI2_BASE);//Enable for communication
}

void TempDataPut(Temp_Data_t* data, TempDataBuff_t* DataBuff)
{

	switch(DataBuff->ArrCnt)
		{
			case 0:
				DataBuff->Mas0[DataBuff->cnt] = data->Temp1;
				DataBuff->cnt++;
				DataBuff->Mas0[DataBuff->cnt] = data->Temp2;
				DataBuff->cnt++;
				DataBuff->Mas0[DataBuff->cnt] = data->Temp3;
				DataBuff->cnt++;
				DataBuff->Mas0[DataBuff->cnt] = data->Temp4;
				DataBuff->cnt++;
			break;
			case 1:
				DataBuff->Mas1[DataBuff->cnt] = data->Temp1;
				DataBuff->cnt++;
				DataBuff->Mas1[DataBuff->cnt] = data->Temp2;
				DataBuff->cnt++;
				DataBuff->Mas1[DataBuff->cnt] = data->Temp3;
				DataBuff->cnt++;
				DataBuff->Mas1[DataBuff->cnt] = data->Temp4;
				DataBuff->cnt++;
			break;
		}
		
		if (DataBuff->cnt == 256 )
		{
			DataBuff->cnt = 0;
			switch(DataBuff->ArrCnt)
			{
				case 0:
					DataBuff->Mas0Status = 1;
				break;
				case 1:
					DataBuff->Mas1Status = 1;
				break;
			}
			DataBuff->ArrCnt++;
			if(DataBuff->ArrCnt == 2)
			{
				DataBuff->ArrCnt = 0;
			}
			
		}
}

void TimeStampDataPut(DataCnt_t * data, TimeStampDataBuff_t * DataBuff)
{
	uint8_t i;
	uint8_t *ptr;
	
	ptr = (uint8_t*)(data);
	
	switch(DataBuff->ArrCnt)
		{
			case 0:
				if(DataBuff->Mas0[0] < 30)
				{
					for(i = 0; i < 16; i++)
					{
						DataBuff->Mas0[DataBuff->Cnt + i] = *ptr;
						ptr++;
					}
					DataBuff->Mas0[0] += 1;
				}
				
			break;
			case 1:
				if(DataBuff->Mas1[0] < 30)
				{
					for(i = 0; i < 16; i++)
					{
						DataBuff->Mas1[DataBuff->Cnt + i] = *ptr;
						ptr++;
					}
					DataBuff->Mas1[0] += 1;
				}
			break;
		}
		
		DataBuff->Cnt += 16;
		/*
		if (DataBuff->Cnt == 497 )
		{
			DataBuff->Cnt = 1;
			switch(DataBuff->ArrCnt)
			{
				case 0:
					DataBuff->Mas0Status = 1;
					DataBuff->Mas1[0] = 0;
				break;
				case 1:
					DataBuff->Mas1Status = 1;
				DataBuff->Mas0[0] = 0;
				break;
			}
			DataBuff->ArrCnt++;
			if(DataBuff->ArrCnt == 2)
			{
				DataBuff->ArrCnt = 0;
			}
			
		}*/
}


