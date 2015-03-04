#include "stdint.h"
#include "stdio.h"
#include "stdbool.h"
#include "inc\tm4c123gh6pm.h"
#include "inc\hw_memmap.h"
#include "driverlib\gpio.h"
#include "driverlib\ssi.h"
#include "driverlib\pin_map.h"
#include "driverlib\sysctl.h"
#include "AFE4490.h"


uint8_t AFE4490_calc_Cf(uint32_t *rodykle_Rf, uint32_t *rodykle_sample_t)
{
	uint8_t ret;
	float Cf;
	float laiko_konstanta;
	uint32_t Rf;
	uint32_t tmp;
	
	tmp = (*rodykle_Rf) & 0x0007;
	switch(tmp)
	{
		case 0:
		Rf = 500E3;
		break;
		case 1:
		Rf = 250E3;
		break;
		case 2:
		Rf = 100E3;
		break;
		case 3:
		Rf = 50E3;
		break;
		case 4:
		Rf = 25E3;
		break;
		case 5:
		Rf = 10E3;
		break;
		case 6:
		Rf = 1E6;
		break;
		case 7:
		Rf = 0;
		break;
	}
	
	laiko_konstanta = 0.00000025 * (*rodykle_sample_t) * 0.1;
	Cf = laiko_konstanta/Rf;
	
	ret = 0;
	Cf -= (float)5E-12; //talpa bent 5 pF
	
	if(Cf > (float)150E-12)
	{
		Cf -= (float)150E-12;
		ret |= TIAGAIN_CF_LED1_inc_150pf;
	}
	if(Cf > (float)50E-12)
	{
		Cf -= (float)50E-12;
		ret |= TIAGAIN_CF_LED1_inc_50pf;
	}
	if(Cf > (float)25E-12)
	{
		Cf -= (float)25E-12;
		ret |= TIAGAIN_CF_LED1_inc_25pf;
	}
	if(Cf > (float)15E-12)
	{
		Cf -= (float)15E-12;
		ret |= TIAGAIN_CF_LED1_inc_15pf;
	}
	if(Cf > (float)5E-12)
	{
		Cf -= (float)5E-12;
		ret |= TIAGAIN_CF_LED1_inc_5pf;
	}
	
	
	
	return(ret);
}


uint32_t AFE4490_calc_I(uint8_t LED1_I, uint8_t LED2_I, uint32_t *rodykle_ref_V)
{
	uint32_t ret = 0;
	uint8_t full_scale_current;
	uint32_t tmp;
	uint8_t tmp2;

	tmp = (*rodykle_ref_V >> CONTROL2_TX_REF_pos) & 0x03;
	
	switch(tmp)
	{
		case 0:	//TX_REF = 0.75V
		case 3:	//TX_REF = 0.75V
			if((LED1_I <= 75) && (LED2_I <= 75))
			{
				ret = (1 << LEDCNTRL_LED_RANGE_pos);
				full_scale_current = 75;
			}else
			{
				ret = (2 << LEDCNTRL_LED_RANGE_pos);
				full_scale_current = 150;
			}
		break;
		case 1: //TX_REF = 0.5V
			if((LED1_I <= 50) && (LED2_I <= 50))
			{
				ret = (1 << LEDCNTRL_LED_RANGE_pos);
				full_scale_current = 50;
			}else
			{
				ret = (2 << LEDCNTRL_LED_RANGE_pos);
				full_scale_current = 100;
			}
		break;
		case 2: //TX_REF = 1V
			if((LED1_I <= 100) && (LED2_I <= 100))
			{
				ret = (1 << LEDCNTRL_LED_RANGE_pos);
				full_scale_current = 100;
			}else
			{
				ret = (2 << LEDCNTRL_LED_RANGE_pos);
				full_scale_current = 200;
			}
		break;	
	}
	
	tmp2 = (LED1_I * 256)/full_scale_current;
	ret |= tmp2 << LEDCNTRL_LED1_pos;
		
	tmp2 = (LED2_I * 256)/full_scale_current;
	ret |= tmp2 << LEDCNTRL_LED2_pos;
	
	return(ret);
}

uint32_t AFE4490A_rd_reg(uint8_t adrr)
{
	uint32_t ret = 0;
	uint8_t MSB, MidSB, LSB;
	
	AFE4490A_rd_EN();
	
	AFE4490A_STE_lo;
	AFE4490_transferB(adrr);
	MSB = AFE4490_transferB(0xFF);	//MSB
	MidSB = AFE4490_transferB(0xFF);	//MidSB
	LSB = AFE4490_transferB(0xFF);	//LSB
	AFE4490A_STE_hi;
	
	AFE4490A_wr_EN();
	
	ret = (MSB<<16)|(MidSB<<8)|(LSB);
	return(ret);
}

void AFE4490A_rd_EN(void)
{
	AFE4490A_STE_lo;	//select AFE4490
	AFE4490_transferB(CONTROL0);	//write to CONTROL0
	AFE4490_transferB(0);	//MSB
	AFE4490_transferB(0);	//MidSB
	AFE4490_transferB(1);	//LSB
	AFE4490A_STE_hi;	//deselect AFE4490
}

void AFE4490A_wr_EN(void)
{
	AFE4490A_STE_lo;	//select AFE4490
	AFE4490_transferB(CONTROL0);	//write to CONTROL0
	AFE4490_transferB(0);	//MSB
	AFE4490_transferB(0);	//MidSB
	AFE4490_transferB(0);	//LSB
	AFE4490A_STE_hi;	//deselect AFE4490
}

void AFE4490A_wr_reg(uint8_t adrr, uint32_t data)
{
	AFE4490A_wr_EN();
	AFE4490A_STE_lo;	//select AFE4490
	AFE4490_transferB(adrr);	//write to CONTROL0
	AFE4490_transferB((uint8_t)(data >>16));	//MSB
	AFE4490_transferB((uint8_t)(((data & 0x00FFFF) >>8)));	//MidSB
	AFE4490_transferB((uint8_t)(((data & 0x0000FF))));	//LSB
	AFE4490A_STE_hi;	//deselect AFE4490 
}

void AFE4490A_init(AFE4490_param_t *pointer)
{
	AFE4490A_wr_EN();	//alow writes to AFE4490
	//counters
	AFE4490A_wr_reg(LED2STC,pointer->reg_LED2STC);
	AFE4490A_wr_reg(LED2ENDC,pointer->reg_LED2ENDC);
	AFE4490A_wr_reg(LED2LEDSTC,pointer->reg_LED2LEDSTC);
	AFE4490A_wr_reg(LED2LEDENDC,pointer->reg_LED2LEDENDC);
	AFE4490A_wr_reg(ALED2STC,pointer->reg_ALED2STC);
	AFE4490A_wr_reg(ALED2ENDC,pointer->reg_ALED2ENDC);
	AFE4490A_wr_reg(LED1STC,pointer->reg_LED1STC);
	AFE4490A_wr_reg(LED1ENDC,pointer->reg_LED1ENDC);
	AFE4490A_wr_reg(LED1LEDSTC,pointer->reg_LED1LEDSTC);
	AFE4490A_wr_reg(LED1LEDENDC,pointer->reg_LED1LEDENDC);
	AFE4490A_wr_reg(ALED1STC,pointer->reg_ALED1STC);
	AFE4490A_wr_reg(ALED1ENDC,pointer->reg_ALED1ENDC);
	AFE4490A_wr_reg(LED2CONVST,pointer->reg_LED2CONVST);
	AFE4490A_wr_reg(LED2CONVEND,pointer->reg_LED2CONVEND);
	AFE4490A_wr_reg(ALED2CONVST,pointer->reg_ALED2CONVST);
	AFE4490A_wr_reg(ALED2CONVEND,pointer->reg_ALED2CONVEND);
	AFE4490A_wr_reg(LED1CONVST,pointer->reg_LED1CONVST);
	AFE4490A_wr_reg(LED1CONVEND,pointer->reg_LED1CONVEND);
	AFE4490A_wr_reg(ALED1CONVST,pointer->reg_ALED1CONVST);
	AFE4490A_wr_reg(ALED1CONVEND,pointer->reg_ALED1CONVEND);
	AFE4490A_wr_reg(ADCRSTSTCT0,pointer->reg_ADCRSTSTCT0);
	AFE4490A_wr_reg(ADCRSTENDCT0,pointer->reg_ADCRSTENDCT0);
	AFE4490A_wr_reg(ADCRSTSTCT1,pointer->reg_ADCRSTSTCT1);
	AFE4490A_wr_reg(ADCRSTENDCT1,pointer->reg_ADCRSTENDCT1);
	AFE4490A_wr_reg(ADCRSTSTCT2,pointer->reg_ADCRSTSTCT2);
	AFE4490A_wr_reg(ADCRSTENDCT2,pointer->reg_ADCRSTENDCT2);
	AFE4490A_wr_reg(ADCRSTSTCT3,pointer->reg_ADCRSTSTCT3);
	AFE4490A_wr_reg(ADCRSTENDCT3,pointer->reg_ADCRSTENDCT3);
	AFE4490A_wr_reg(PRPCOUNT,pointer->reg_PRPCOUNT);	//period
	//counters end
	
	AFE4490A_wr_reg(TIAGAIN,pointer->reg_TIAGAIN);
	AFE4490A_wr_reg(TIA_AMB_GAIN,pointer->reg_TIA_AMB_GAIN);
	AFE4490A_wr_reg(LEDCNTRL,pointer->reg_LEDCNTRL);
	AFE4490A_wr_reg(CONTROL2,pointer->reg_CONTROL2);
	AFE4490A_wr_reg(CONTROL1,0x000101);	//timers ON
	AFE4490A_rd_EN();
}

uint32_t AFE4490A_diag(void)
{
	AFE4490A_wr_reg(CONTROL0,(1 << CONTROL0_DIAG_EN_pos));
	AFE4490_delay_ms(100);
	AFE4490A_rd_EN();
	return(AFE4490A_rd_reg(DIAG));
}

uint32_t AFE4490B_rd_reg(uint8_t adrr)
{
	uint32_t ret = 0;
	uint8_t MSB, MidSB, LSB;
	
	AFE4490B_rd_EN();
	
	AFE4490B_STE_lo;
	AFE4490_transferB(adrr);
	MSB = AFE4490_transferB(0xFF);	//MSB
	MidSB = AFE4490_transferB(0xFF);	//MidSB
	LSB = AFE4490_transferB(0xFF);	//LSB
	AFE4490B_STE_hi;
	
	AFE4490B_wr_EN();
	
	ret = (MSB<<16)|(MidSB<<8)|(LSB);
	return(ret);
}

void AFE4490B_rd_EN(void)
{
	AFE4490B_STE_lo;	//select AFE4490
	AFE4490_transferB(CONTROL0);	//write to CONTROL0
	AFE4490_transferB(0);	//MSB
	AFE4490_transferB(0);	//MidSB
	AFE4490_transferB(1);	//LSB
	AFE4490B_STE_hi;	//deselect AFE4490
}

void AFE4490B_wr_EN(void)
{
	AFE4490B_STE_lo;	//select AFE4490
	AFE4490_transferB(CONTROL0);	//write to CONTROL0
	AFE4490_transferB(0);	//MSB
	AFE4490_transferB(0);	//MidSB
	AFE4490_transferB(0);	//LSB
	AFE4490B_STE_hi;	//deselect AFE4490
}

void AFE4490B_wr_reg(uint8_t adrr, uint32_t data)
{
	AFE4490B_wr_EN();
	AFE4490B_STE_lo;	//select AFE4490
	AFE4490_transferB(adrr);	//write to CONTROL0
	AFE4490_transferB((uint8_t)(data >>16));	//MSB
	AFE4490_transferB((uint8_t)(((data & 0x00FFFF) >>8)));	//MidSB
	AFE4490_transferB((uint8_t)(((data & 0x0000FF))));	//LSB
	AFE4490B_STE_hi;	//deselect AFE4490 
}

void AFE4490B_init(AFE4490_param_t *pointer)
{
	AFE4490B_wr_EN();	//alow writes to AFE4490
	//counters
	AFE4490B_wr_reg(LED2STC,pointer->reg_LED2STC);
	AFE4490B_wr_reg(LED2ENDC,pointer->reg_LED2ENDC);
	AFE4490B_wr_reg(LED2LEDSTC,pointer->reg_LED2LEDSTC);
	AFE4490B_wr_reg(LED2LEDENDC,pointer->reg_LED2LEDENDC);
	AFE4490B_wr_reg(ALED2STC,pointer->reg_ALED2STC);
	AFE4490B_wr_reg(ALED2ENDC,pointer->reg_ALED2ENDC);
	AFE4490B_wr_reg(LED1STC,pointer->reg_LED1STC);
	AFE4490B_wr_reg(LED1ENDC,pointer->reg_LED1ENDC);
	AFE4490B_wr_reg(LED1LEDSTC,pointer->reg_LED1LEDSTC);
	AFE4490B_wr_reg(LED1LEDENDC,pointer->reg_LED1LEDENDC);
	AFE4490B_wr_reg(ALED1STC,pointer->reg_ALED1STC);
	AFE4490B_wr_reg(ALED1ENDC,pointer->reg_ALED1ENDC);
	AFE4490B_wr_reg(LED2CONVST,pointer->reg_LED2CONVST);
	AFE4490B_wr_reg(LED2CONVEND,pointer->reg_LED2CONVEND);
	AFE4490B_wr_reg(ALED2CONVST,pointer->reg_ALED2CONVST);
	AFE4490B_wr_reg(ALED2CONVEND,pointer->reg_ALED2CONVEND);
	AFE4490B_wr_reg(LED1CONVST,pointer->reg_LED1CONVST);
	AFE4490B_wr_reg(LED1CONVEND,pointer->reg_LED1CONVEND);
	AFE4490B_wr_reg(ALED1CONVST,pointer->reg_ALED1CONVST);
	AFE4490B_wr_reg(ALED1CONVEND,pointer->reg_ALED1CONVEND);
	AFE4490B_wr_reg(ADCRSTSTCT0,pointer->reg_ADCRSTSTCT0);
	AFE4490B_wr_reg(ADCRSTENDCT0,pointer->reg_ADCRSTENDCT0);
	AFE4490B_wr_reg(ADCRSTSTCT1,pointer->reg_ADCRSTSTCT1);
	AFE4490B_wr_reg(ADCRSTENDCT1,pointer->reg_ADCRSTENDCT1);
	AFE4490B_wr_reg(ADCRSTSTCT2,pointer->reg_ADCRSTSTCT2);
	AFE4490B_wr_reg(ADCRSTENDCT2,pointer->reg_ADCRSTENDCT2);
	AFE4490B_wr_reg(ADCRSTSTCT3,pointer->reg_ADCRSTSTCT3);
	AFE4490B_wr_reg(ADCRSTENDCT3,pointer->reg_ADCRSTENDCT3);
	AFE4490B_wr_reg(PRPCOUNT,pointer->reg_PRPCOUNT);	//period
	//counters end
	
	AFE4490B_wr_reg(TIAGAIN,pointer->reg_TIAGAIN);
	AFE4490B_wr_reg(TIA_AMB_GAIN,pointer->reg_TIA_AMB_GAIN);
	AFE4490B_wr_reg(LEDCNTRL,pointer->reg_LEDCNTRL);
	AFE4490B_wr_reg(CONTROL2,pointer->reg_CONTROL2);
	AFE4490B_wr_reg(CONTROL1,0x000101);	//timers ON
	AFE4490B_rd_EN();
}

uint32_t AFE4490B_diag(void)
{
	AFE4490B_wr_reg(CONTROL0,(1 << CONTROL0_DIAG_EN_pos));
	AFE4490_delay_ms(100);
	AFE4490B_rd_EN();
	return(AFE4490B_rd_reg(DIAG));
}

uint32_t AFE4490C_rd_reg(uint8_t adrr)
{
	uint32_t ret = 0;
	uint8_t MSB, MidSB, LSB;
	
	AFE4490C_rd_EN();
	
	AFE4490C_STE_lo;
	AFE4490_transferB(adrr);
	MSB = AFE4490_transferB(0xFF);	//MSB
	MidSB = AFE4490_transferB(0xFF);	//MidSB
	LSB = AFE4490_transferB(0xFF);	//LSB
	AFE4490C_STE_hi;
	
	AFE4490C_wr_EN();
	
	ret = (MSB<<16)|(MidSB<<8)|(LSB);
	return(ret);
}

void AFE4490C_rd_EN(void)
{
	AFE4490C_STE_lo;	//select AFE4490
	AFE4490_transferB(CONTROL0);	//write to CONTROL0
	AFE4490_transferB(0);	//MSB
	AFE4490_transferB(0);	//MidSB
	AFE4490_transferB(1);	//LSB
	AFE4490C_STE_hi;	//deselect AFE4490
}

void AFE4490C_wr_EN(void)
{
	AFE4490C_STE_lo;	//select AFE4490
	AFE4490_transferB(CONTROL0);	//write to CONTROL0
	AFE4490_transferB(0);	//MSB
	AFE4490_transferB(0);	//MidSB
	AFE4490_transferB(0);	//LSB
	AFE4490C_STE_hi;	//deselect AFE4490
}

void AFE4490C_wr_reg(uint8_t adrr, uint32_t data)
{
	AFE4490C_wr_EN();
	AFE4490C_STE_lo;	//select AFE4490
	AFE4490_transferB(adrr);	//write to CONTROL0
	AFE4490_transferB((uint8_t)(data >>16));	//MSB
	AFE4490_transferB((uint8_t)(((data & 0x00FFFF) >>8)));	//MidSB
	AFE4490_transferB((uint8_t)(((data & 0x0000FF))));	//LSB
	AFE4490C_STE_hi;	//deselect AFE4490 
}

void AFE4490C_init(AFE4490_param_t *pointer)
{
	AFE4490C_wr_EN();	//alow writes to AFE4490
	//counters
	AFE4490C_wr_reg(LED2STC,pointer->reg_LED2STC);
	AFE4490C_wr_reg(LED2ENDC,pointer->reg_LED2ENDC);
	AFE4490C_wr_reg(LED2LEDSTC,pointer->reg_LED2LEDSTC);
	AFE4490C_wr_reg(LED2LEDENDC,pointer->reg_LED2LEDENDC);
	AFE4490C_wr_reg(ALED2STC,pointer->reg_ALED2STC);
	AFE4490C_wr_reg(ALED2ENDC,pointer->reg_ALED2ENDC);
	AFE4490C_wr_reg(LED1STC,pointer->reg_LED1STC);
	AFE4490C_wr_reg(LED1ENDC,pointer->reg_LED1ENDC);
	AFE4490C_wr_reg(LED1LEDSTC,pointer->reg_LED1LEDSTC);
	AFE4490C_wr_reg(LED1LEDENDC,pointer->reg_LED1LEDENDC);
	AFE4490C_wr_reg(ALED1STC,pointer->reg_ALED1STC);
	AFE4490C_wr_reg(ALED1ENDC,pointer->reg_ALED1ENDC);
	AFE4490C_wr_reg(LED2CONVST,pointer->reg_LED2CONVST);
	AFE4490C_wr_reg(LED2CONVEND,pointer->reg_LED2CONVEND);
	AFE4490C_wr_reg(ALED2CONVST,pointer->reg_ALED2CONVST);
	AFE4490C_wr_reg(ALED2CONVEND,pointer->reg_ALED2CONVEND);
	AFE4490C_wr_reg(LED1CONVST,pointer->reg_LED1CONVST);
	AFE4490C_wr_reg(LED1CONVEND,pointer->reg_LED1CONVEND);
	AFE4490C_wr_reg(ALED1CONVST,pointer->reg_ALED1CONVST);
	AFE4490C_wr_reg(ALED1CONVEND,pointer->reg_ALED1CONVEND);
	AFE4490C_wr_reg(ADCRSTSTCT0,pointer->reg_ADCRSTSTCT0);
	AFE4490C_wr_reg(ADCRSTENDCT0,pointer->reg_ADCRSTENDCT0);
	AFE4490C_wr_reg(ADCRSTSTCT1,pointer->reg_ADCRSTSTCT1);
	AFE4490C_wr_reg(ADCRSTENDCT1,pointer->reg_ADCRSTENDCT1);
	AFE4490C_wr_reg(ADCRSTSTCT2,pointer->reg_ADCRSTSTCT2);
	AFE4490C_wr_reg(ADCRSTENDCT2,pointer->reg_ADCRSTENDCT2);
	AFE4490C_wr_reg(ADCRSTSTCT3,pointer->reg_ADCRSTSTCT3);
	AFE4490C_wr_reg(ADCRSTENDCT3,pointer->reg_ADCRSTENDCT3);
	AFE4490C_wr_reg(PRPCOUNT,pointer->reg_PRPCOUNT);	//period
	//counters end
	
	AFE4490C_wr_reg(TIAGAIN,pointer->reg_TIAGAIN);
	AFE4490C_wr_reg(TIA_AMB_GAIN,pointer->reg_TIA_AMB_GAIN);
	AFE4490C_wr_reg(LEDCNTRL,pointer->reg_LEDCNTRL);
	AFE4490C_wr_reg(CONTROL2,pointer->reg_CONTROL2);
	AFE4490C_wr_reg(CONTROL1,0x000101);	//timers ON
	AFE4490C_rd_EN();
}

uint32_t AFE4490C_diag(void)
{
	AFE4490C_wr_reg(CONTROL0,(1 << CONTROL0_DIAG_EN_pos));
	AFE4490_delay_ms(100);
	AFE4490C_rd_EN();
	return(AFE4490C_rd_reg(DIAG));
}

void AFE4490A_RdData(AFE4490_Result_t* data)
{
	uint32_t LED1reiksme;
	uint32_t LED2reiksme;
	uint32_t perstumta;
	
	LED1reiksme = AFE4490A_rd_reg(LED1VAL);
	LED2reiksme = AFE4490A_rd_reg(LED2VAL);
	
	perstumta = LED1reiksme << 10;
	data->LED1 = (int32_t)perstumta;
	data->LED1 /= 1024;
	
	perstumta = LED2reiksme << 10;
	data->LED2 = (int32_t)perstumta;
	data->LED2 /= 1024;
}

void AFE4490B_RdData(AFE4490_Result_t* data)
{
	uint32_t LED1reiksme;
	uint32_t LED2reiksme;
	uint32_t perstumta;
	
	LED1reiksme = AFE4490B_rd_reg(LED1VAL);
	LED2reiksme = AFE4490B_rd_reg(LED2VAL);
	
	perstumta = LED1reiksme << 10;
	data->LED1 = (int32_t)perstumta;
	data->LED1 /= 1024;
	
	perstumta = LED2reiksme << 10;
	data->LED2 = (int32_t)perstumta;
	data->LED2 /= 1024;
}

void AFE4490C_RdData(AFE4490_Result_t* data)
{
	uint32_t LED1reiksme;
	uint32_t LED2reiksme;
	uint32_t perstumta;
	
	LED1reiksme = AFE4490C_rd_reg(LED1VAL);
	LED2reiksme = AFE4490C_rd_reg(LED2VAL);
	
	perstumta = LED1reiksme << 10;
	data->LED1 = (int32_t)perstumta;
	data->LED1 /= 1024;
	
	perstumta = LED2reiksme << 10;
	data->LED2 = (int32_t)perstumta;
	data->LED2 /= 1024;
}

