#ifndef AFE4490h
#define AFE4490h

#include "stdint.h"
#include "AFE4490_TM4C123GH6PMI_hw.h"

//AFE4490 registers
#define CONTROL0 0x00
#define LED2STC 0x01
#define LED2ENDC 0x02
#define LED2LEDSTC 0x03
#define LED2LEDENDC 0x04
#define ALED2STC 0x05
#define ALED2ENDC 0x06
#define LED1STC 0x07
#define LED1ENDC 0x08
#define LED1LEDSTC 0x09
#define LED1LEDENDC 0x0A
#define ALED1STC 0x0B
#define ALED1ENDC 0x0C
#define LED2CONVST 0x0D
#define LED2CONVEND 0x0E
#define ALED2CONVST	0x0F
#define ALED2CONVEND 0x10
#define LED1CONVST 0x11
#define LED1CONVEND 0x12
#define ALED1CONVST 0x13
#define ALED1CONVEND 0x14
#define ADCRSTSTCT0 0x15
#define ADCRSTENDCT0 0x16
#define ADCRSTSTCT1 0x17
#define ADCRSTENDCT1 0x18
#define ADCRSTSTCT2 0x19
#define ADCRSTENDCT2 0x1A
#define ADCRSTSTCT3 0x1B
#define ADCRSTENDCT3 0x1C
#define PRPCOUNT 0x1D
#define CONTROL1 0x1E
#define SPARE1 0x1F
#define TIAGAIN 0x20
#define TIA_AMB_GAIN 0x21
#define LEDCNTRL 0x22
#define CONTROL2 0x23
#define SPARE2 0x24
#define SPARE3 0x25
#define SPARE4 0x26
#define RESERVED1 0x27
#define RESERVED2 0x28
#define ALARM 0x29
#define LED2VAL 0x2A
#define ALED2VAL 0x2B
#define LED1VAL 0x2C
#define ALED1VAL 0x2D
#define LED2_ALED2VAL 0x2E
#define LED1_ALED1VAL 0x2F
#define DIAG 0x30
//AFE registers bits
//CONTROL0
#define CONTROL0_SPI_READ_pos 0
#define CONTROL0_TIM_COUNT_RST_pos 1
#define CONTROL0_DIAG_EN_pos 2
#define CONTROL0_SW_RST_pos 3
//CONTROL1
#define CONTROL1_NUMAV_pos 0
#define CONTROL1_TIMEREN_pos 8
#define CONTROL1_CLKALMPIN_pos 9
//TIAGAIN
#define TIAGAIN_RF_LED1_pos 0
#define TIAGAIN_CF_LED1_pos 3
#define TIAGAIN_ENSEPGAN_pos 15

#define TIAGAIN_CF_LED1_5pf 0
#define TIAGAIN_CF_LED1_inc_5pf 1
#define TIAGAIN_CF_LED1_inc_15pf 2
#define TIAGAIN_CF_LED1_inc_25pf 4
#define TIAGAIN_CF_LED1_inc_50pf 8
#define TIAGAIN_CF_LED1_inc_150pf 16

#define TIAGAIN_RF_LED1_500k 0
#define TIAGAIN_RF_LED1_250k 1
#define TIAGAIN_RF_LED1_100k 2
#define TIAGAIN_RF_LED1_50k 3
#define TIAGAIN_RF_LED1_25k 4
#define TIAGAIN_RF_LED1_10k 5
#define TIAGAIN_RF_LED1_1M 6
#define TIAGAIN_RF_LED1_0 7

//TIA_AMB_GAIN
#define TIA_AMB_GAIN_RF_LED2_pos 0
#define TIA_AMB_GAIN_CF_LED2_pos 3
#define TIA_AMB_GAIN_STG2GAIN_pos 8
#define TIA_AMB_GAIN_STAGE2EN_pos 14
#define TIA_AMB_GAIN_FLTRCNRSEL_pos 15
#define TIA_AMB_GAIN_AMBDAC_pos 16

#define TIA_AMB_GAIN_AMBDA_0uA 0
#define TIA_AMB_GAIN_AMBDA_1uA 1
#define TIA_AMB_GAIN_AMBDA_2uA 2
#define TIA_AMB_GAIN_AMBDA_3uA 3
#define TIA_AMB_GAIN_AMBDA_4uA 4
#define TIA_AMB_GAIN_AMBDA_5uA 5
#define TIA_AMB_GAIN_AMBDA_6uA 6
#define TIA_AMB_GAIN_AMBDA_7uA 7
#define TIA_AMB_GAIN_AMBDA_8uA 8
#define TIA_AMB_GAIN_AMBDA_9uA 9
#define TIA_AMB_GAIN_AMBDA_10uA 10

#define TIA_AMB_GAIN_STG2GAIN_1 0
#define TIA_AMB_GAIN_STG2GAIN_1_5 1
#define TIA_AMB_GAIN_STG2GAIN_2 2
#define TIA_AMB_GAIN_STG2GAIN_3 3
#define TIA_AMB_GAIN_STG2GAIN_4 4

#define TIA_AMB_GAIN_CF_LED2_5pf 0
#define TIA_AMB_GAIN_CF_LED2_inc_5pf 1
#define TIA_AMB_GAIN_CF_LED2_inc_15pf 2
#define TIA_AMB_GAIN_CF_LED2_inc_25pf 4
#define TIA_AMB_GAIN_CF_LED2_inc_50pf 8
#define TIA_AMB_GAIN_CF_LED2_inc_150pf 16

#define TIA_AMB_GAIN_RF_LED2_500k 0
#define TIA_AMB_GAIN_RF_LED2_250k 1
#define TIA_AMB_GAIN_RF_LED2_100k 2
#define TIA_AMB_GAIN_RF_LED2_50k 3
#define TIA_AMB_GAIN_RF_LED2_25k 4
#define TIA_AMB_GAIN_RF_LED2_10k 5
#define TIA_AMB_GAIN_RF_LED2_1M 6
#define TIA_AMB_GAIN_RF_LED2_0 7

//LEDCNTRL
#define LEDCNTRL_LED2_pos 0
#define LEDCNTRL_LED1_pos 8
#define LEDCNTRL_LED_RANGE_pos 16
//CONTROL2
#define CONTROL2_PDNAFE_pos 0
#define CONTROL2_PDNRX_pos 1
#define CONTROL2_PDNTX_pos 2
#define CONTROL2_EN_SLOW_DIAG_pos 8
#define CONTROL2_XTALDIS_pos 9
#define CONTROL2_DIGOUT_TRISTATE_pos 10
#define CONTROL2_TXBRGMOD_pos 11
#define CONTROL2_EN_ADC_BYP_pos 15
#define CONTROL2_RST_CLK_ON_PD_ALM_pos 16
#define CONTROL2_TX_REF_pos 17

#define CONTROL2_TX_REF_750mV 0
#define CONTROL2_TX_REF_500mV 1
#define CONTROL2_TX_REF_1V 2


//ALARM
#define ALARM_ALMPINCLKEN_pos 7
//DIAG
#define DIAG_INPSCLED_pos 0
#define DIAG_INNSCLED_pos 1
#define DIAG_INPSCGND_pos 2
#define DIAG_INNSCGND_pos 3
#define DIAG_PDSC_pos 4
#define DIAG_PDOC_pos 5
#define DIAG_OUTNSHGND_pos 6
#define DIAG_OUTPSHGND_pos 7
#define DIAG_LEDSC_pos 8
#define DIAG_LED2OPEN_pos 9
#define DIAG_LED1OPEN_pos 10
#define DIAG_LED_ALM_pos 11
#define DIAG_PD_ALM_pos 12


#define AFE4490C_hiZ_ON AFE4490C_wr_reg(CONTROL2,(1 << CONTROL2_DIGOUT_TRISTATE_pos) | (CONTROL2_TX_REF_500mV << CONTROL2_TX_REF_pos))
#define AFE4490C_hiZ_OFF AFE4490C_wr_reg(CONTROL2,(CONTROL2_TX_REF_500mV << CONTROL2_TX_REF_pos))

#define AFE4490B_hiZ_ON AFE4490B_wr_reg(CONTROL2,(1 << CONTROL2_DIGOUT_TRISTATE_pos) | (CONTROL2_TX_REF_500mV << CONTROL2_TX_REF_pos))
#define AFE4490B_hiZ_OFF AFE4490B_wr_reg(CONTROL2,(CONTROL2_TX_REF_500mV << CONTROL2_TX_REF_pos))

#define AFE4490A_hiZ_ON AFE4490A_wr_reg(CONTROL2,(1 << CONTROL2_DIGOUT_TRISTATE_pos) | (CONTROL2_TX_REF_500mV << CONTROL2_TX_REF_pos))
#define AFE4490A_hiZ_OFF AFE4490A_wr_reg(CONTROL2,(CONTROL2_TX_REF_500mV << CONTROL2_TX_REF_pos))


typedef struct
{
uint32_t reg_CONTROL0;	
uint32_t reg_LED2STC;	
uint32_t reg_LED2ENDC;
uint32_t reg_LED2LEDSTC;
uint32_t reg_LED2LEDENDC;
uint32_t reg_ALED2STC;
uint32_t reg_ALED2ENDC;
uint32_t reg_LED1STC;
uint32_t reg_LED1ENDC;
uint32_t reg_LED1LEDSTC;
uint32_t reg_LED1LEDENDC;
uint32_t reg_ALED1STC;
uint32_t reg_ALED1ENDC;
uint32_t reg_LED2CONVST;
uint32_t reg_LED2CONVEND;
uint32_t reg_ALED2CONVST;
uint32_t reg_ALED2CONVEND;
uint32_t reg_LED1CONVST;
uint32_t reg_LED1CONVEND;
uint32_t reg_ALED1CONVST;
uint32_t reg_ALED1CONVEND;
uint32_t reg_ADCRSTSTCT0;
uint32_t reg_ADCRSTENDCT0;
uint32_t reg_ADCRSTSTCT1;
uint32_t reg_ADCRSTENDCT1;
uint32_t reg_ADCRSTSTCT2;
uint32_t reg_ADCRSTENDCT2;
uint32_t reg_ADCRSTSTCT3;
uint32_t reg_ADCRSTENDCT3;
uint32_t reg_PRPCOUNT;
uint32_t reg_CONTROL1;
uint32_t reg_SPARE1;
uint32_t reg_TIAGAIN;
uint32_t reg_TIA_AMB_GAIN;
uint32_t reg_LEDCNTRL;
uint32_t reg_CONTROL2;
uint32_t reg_SPARE2;
uint32_t reg_SPARE3;
uint32_t reg_SPARE4;
uint32_t reg_RESERVED1;
uint32_t reg_RESERVED2;
uint32_t reg_ALARM;
uint32_t reg_LED2VAL;
uint32_t reg_ALED2VAL;
uint32_t reg_LED1VAL;
uint32_t reg_ALED1VAL;
uint32_t reg_LED2_ALED2VAL;
uint32_t reg_LED1_ALED1VAL;
uint32_t reg_DIAG;
} AFE4490_param_t;

typedef struct
{
	int32_t LED1;
	int32_t LED2;
}AFE4490_Result_t;

extern uint8_t AFE4490_transferB(uint8_t);
extern void AFE4490_delay_ms(uint32_t laikas);
extern uint8_t AFE4490_ram_wr(uint32_t start_addr, uint8_t *buff, uint32_t size);

uint8_t AFE4490_calc_Cf(uint32_t *rodykle_Rf, uint32_t *rodykle_sample_t);
uint32_t AFE4490_calc_I(uint8_t LED1_I, uint8_t LED2_I, uint32_t *rodykle_ref_V);

uint32_t AFE4490A_rd_reg(uint8_t adrr);
void AFE4490A_rd_EN(void);	//perjungiam i skaityma
void AFE4490A_wr_EN(void);	//perjungiam i rasyma
void AFE4490A_wr_reg(uint8_t adrr, uint32_t data);
void AFE4490A_init(AFE4490_param_t *pointer);
void AFE4490A_RdData(AFE4490_Result_t* data);
uint32_t AFE4490A_diag(void);

uint32_t AFE4490B_rd_reg(uint8_t adrr);
void AFE4490B_rd_EN(void);	//perjungiam i skaityma
void AFE4490B_wr_EN(void);	//perjungiam i rasyma
void AFE4490B_wr_reg(uint8_t adrr, uint32_t data);
void AFE4490B_init(AFE4490_param_t *pointer);
void AFE4490B_RdData(AFE4490_Result_t* data);
uint32_t AFE4490B_diag(void);

uint32_t AFE4490C_rd_reg(uint8_t adrr);
void AFE4490C_rd_EN(void);	//perjungiam i skaityma
void AFE4490C_wr_EN(void);	//perjungiam i rasyma
void AFE4490C_wr_reg(uint8_t adrr, uint32_t data);
void AFE4490C_init(AFE4490_param_t *pointer);
void AFE4490C_RdData(AFE4490_Result_t* data);
uint32_t AFE4490C_diag(void);


#endif
