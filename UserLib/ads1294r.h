/*
	Modifikavimai:
	2014-05-02				Jei egzistuoja failas, neraso ant virsaus, o pavadinime numeriuka pripliusuoja
										SD_FS_PC - iraso ir i SD ir siuncia i PC, SD_FS tik i SD
	
	
	
	*********************************************
	Pridejimas prie projekto:
	#include "ads1298r.h"
	
	Prideti prie projekto c failus:
	ads1298r.c
	ads1298r_hw.c
	
	C/C++ nustatymuose prie Include paths nurodyti inc aplanko adresa pvz \..\..\EKG_ADS1298r\inc;
	main.h faile deklaruoti pinus
	#define ADS1298R_CS					30	
	#define ADS1298R_CLK				28
	#define ADS1298R_DATA_INT		24
	#define ADS1298R_MISO				25
	#define ADS1298R_MOSI				29
	**********************************************
	
	Naudojimo pavyzdys:
	
	ads1298r_init();						// inicializuoti GPIO, SPI, kai kuriuos ADS registrus
	
	ADS_UART.ADS_DEST = SD_FS;  // duomenu kryptis - SD kortele su failu sistema
	
	startas();									// pradedami matavimai
	stopas(); 									// baigiami matavimai
	
	ads1298r_data_switch();			// isrenka ir iraso/nusiuncia duomenis, si funkcija turi buti daznai iskvieciama
	
	
	*************************************************
*/
#ifndef ADS1294R_H
#define ADS1294R_H

#include <stdint.h>
#include "ads1294r_hw.h"


extern unsigned long ads_mem_ind;



// System Commands
#define ADS129X_WAKEUP			0x02	// Wake-up from standby mode
#define ADS129X_STANDBY		0x04	// Enter standby mode
#define ADS129X_RESET			0x06	// Reset the device
#define ADS129X_START			0x08	// Start/restart (synchronize) conversions
#define ADS129X_STOP				0x0A	// Stop conversion

// Data Read Commands
#define ADS129X_RDATAC			0x10	// Enable Read Data Continuous mode
#define ADS129X_SDATAC			0x11	// Stop Read Data Continuously mode
#define ADS129X_RDATA			0x12	// Read data by command; supports multiple read back.

// Register Read Commands
#define ADS129X_RREG				0x20	// Read n nnnn registers starting at address r rrrr
#define ADS129X_WREG				0x40	// Write n nnnn registers starting at address r rrrr

// Registers addresses
#define ADS129X_ID						0x00
#define ADS129X_CONFIG1			0x01
#define ADS129X_CONFIG2			0x02
#define ADS129X_CONFIG3			0x03
#define ADS129X_LOFF					0x04
#define ADS129X_CH1SET				0x05
#define ADS129X_CH2SET				0x06
#define ADS129X_CH3SET				0x07
#define ADS129X_CH4SET				0x08
#define ADS129X_CH5SET				0x09
#define ADS129X_CH6SET				0x0A
#define ADS129X_CH7SET				0x0B
#define ADS129X_CH8SET				0x0C
#define ADS129X_RLD_SENSP		0x0D
#define ADS129X_RLD_SENSN		0x0E
#define ADS129X_LOFF_SENSP		0x0F
#define ADS129X_LOFF_SENSN		0x10
#define ADS129X_LOFF_FLIP		0x11
#define ADS129X_LOFF_STATP		0x12
#define ADS129X_LOFF_STATN		0x13
#define ADS129X_GPIO					0x14
#define ADS129X_PACE					0x15
#define ADS129X_RESP					0x16
#define ADS129X_CONFIG4			0x17
#define ADS129X_WCT1					0x18
#define ADS129X_WCT2					0x19

typedef struct
{
	uint8_t DataRate 	:3; 
	uint8_t Always0		:2;
	uint8_t ClkEn 		:1;					//	CLK connection. 0 = Oscillator clock output disabled (default) 1 = Oscillator clock output enabled
	uint8_t DaisyEn 	:1; 			// Daisy-chain/multiple readback mode. 0 = Daisy-chain mode (default) 1 = Multiple readback mode
	uint8_t HighRate 	:1; 			// High-Resolution/Low-Power mode. 0 = Low-Power mode (default) 1 = High-Resolution mode
}ADS129X_config1_t;

// CONFIG1: Configuration Register 1
#define ADS129X_config1_DataRate_32k_16kLP			0x00 //	Output data rate 32kSPS - In Low Power Mode 16kSPS
#define ADS129X_config1_DataRate_16k_8kLP			0x01 //	Output data rate 16kSPS - In Low Power Mode 8kSPS
#define ADS129X_config1_DataRate_8k_4kLP				0x02 //	Output data rate 8kSPS - In Low Power Mode 4kSPS
#define ADS129X_config1_DataRate_4k_2kLP				0x03 //	Output data rate 4kSPS - In Low Power Mode 2kSPS
#define ADS129X_config1_DataRate_2k_1kLP				0x04 //	Output data rate 2kSPS - In Low Power Mode 1kSPS
#define ADS129X_config1_DataRate_1k_500LP			0x05 //	Output data rate 1kSPS - In Low Power Mode 500SPS
#define ADS129X_config1_DataRate_500_250LP			0x06 //	Output data rate 500SPS - In Low Power Mode 250SPS

// CONFIG2: Configuration Register 2
typedef struct
{
	uint8_t TestFreq 	:2; //	Test signal frequency.
	uint8_t TestAmp 	:1;	//	Test signal amplitude. 0 = 1 × –(VREFP – VREFN)/2.4mV (default) 1 = 2 × –(VREFP – VREFN)/2.4mV
	uint8_t Always0	 	:1;					
	uint8_t IntTest	 	:1; // 	TEST source. 0 = Test signals are driven externally (default) 1 = Test signals are generated internally			
	uint8_t WctChop	 	:1;	// 	WCT chopping scheme. 0 = Chopping frequency varies, 1 = Chopping frequency constant at fMOD/16
	uint8_t Always00	:2;
}ADS129X_config2_t;

#define ADS129X_config2_TestFreq_0		0x00 //	Test signal frequency. 00 = Pulsed at fCLK/221 (default)
#define ADS129X_config2_TestFreq_1		0x01 //	Test signal frequency. 01 = Pulsed at fCLK/220
#define ADS129X_config2_TestFreq_2		0x02 //	Test signal frequency. 10 = Not used
#define ADS129X_config2_TestFreq_3		0x03 //	Test signal frequency. 11 = At dc

// CONFIG3: Configuration Register 3
typedef struct
{
	uint8_t RLDLeadOffStatus 	:1; //	RLD lead-off status. 0 = RLD is connected (default) 1 = RLD is not connected
	uint8_t RLDLeadSenseOff 	:1;	//	RLD sense function. 0 = RLD sense is disabled (default) 1 = RLD sense is enabled
	uint8_t RLDBufferPower 		:1;	//	RLD buffer power. 0 = RLD buffer is powered down (default) 1 = RLD buffer is enabled
	uint8_t RLDRefSource 			:1;	//	RLDREF signal. 0 = RLDREF signal fed externally (default) 1 = RLDREF signal (AVDD – AVSS)/2 generated internally
	uint8_t RLDMeasurement		:1; //	RLD measurement. 0 = Open (default) 1 = RLD_IN signal is routed to the channel that has the MUX_Setting 010 (VREF)
	uint8_t RefVoltage	 			:1;	//	Reference voltage. 0 = VREFP is set to 2.4V (default) 1 = VREFP is set to 4V (use only with a 5V analog supply)
	uint8_t Always1 					:1;	//	Must always be set to '1' ****************
	uint8_t PDRefBuff 				:1;	//	0 = Power-down internal reference buffer (default) 1 = Enable internal reference buffer
}ADS129X_config3_t;

// LOFF: Lead-Off Control Register
// The Lead-Off Control Register configures the Lead-Off detection operation.
typedef struct
{
	uint8_t LeadOffFrequency 								:2; //	These bits determine the frequency of lead-off detect for each channel.
	uint8_t LeadOffCurrentMagnitude 				:2;	//	These bits determine the magnitude of current for the current lead-off mode.
	uint8_t LeadOffDetectionMode						:1;	//	This bit determines the lead-off detection mode.
	uint8_t LeadOffComparatorThreshold			:3;	//	Lead-off comparator threshold
}ADS129X_LeadOffControl_t;

#define ADS129X_LeadOff_COMP_TH_95				0x00 // Lead-off comparator threshold. 000 = 95% (default)
#define ADS129X_LeadOff_COMP_TH_92				0x01 // Lead-off comparator threshold. 001 = 92.5%
#define ADS129X_LeadOff_COMP_TH_90				0x02 // Lead-off comparator threshold. 010 = 90%
#define ADS129X_LeadOff_COMP_TH_97				0x03 // Lead-off comparator threshold. 011 = 87.5%
#define ADS129X_LeadOff_COMP_TH_85				0x04 // Lead-off comparator threshold. 100 = 85%
#define ADS129X_LeadOff_COMP_TH_80				0x05 // Lead-off comparator threshold. 101 = 80%
#define ADS129X_LeadOff_COMP_TH_75				0x06 // Lead-off comparator threshold. 110 = 75%
#define ADS129X_LeadOff_COMP_TH_70				0x07 // Lead-off comparator threshold. 111 = 70%

#define ADS129X_LEAD_OFF_6nA				0x00 // Lead-off current magnitude. 00 = 6nA (default)
#define ADS129X_LEAD_OFF_12nA			0x01 // Lead-off current magnitude. 01 = 12nA
#define ADS129X_LEAD_OFF_18nA			0x02 // Lead-off current magnitude. 10 = 18nA
#define ADS129X_LEAD_OFF_24nA			0x03 // Lead-off current magnitude. 11 = 24nA
#define ADS129X_LeadOffFrequencyAc	0x01 // Lead-off frequency. 01 = AC lead-off detection at fDR/4
#define ADS129X_LeadOffFrequencyDc	0x03 // Lead-off frequency. 11 = DC lead-off detection turned on

// CHnSET: Individual Channel Settings (n = 1 : 8)
// The CH[1:8]SET Control Register configures the power mode, PGA gain, and multiplexer settings channels. See
// the Input Multiplexer section for details. CH[2:8]SET are similar to CH1SET, corresponding to the respective
// channels.
typedef struct
{
	uint8_t Mux													:3; //	These bits determine the channel input selection.
	uint8_t Always0											:1;	//	Always write '0'
	uint8_t PgaGain											:3;	//	These bits determine the PGA gain setting.
	uint8_t PowerDown										:1;	//	This bit determines the channel power mode for the corresponding channel.
}ADS129X_ChannelSettings_t;

#define ADS129X_ChannelSettings_GAIN_6												0x00 // PGA gain. 000 = 6 (default)
#define ADS129X_ChannelSettings_GAIN_1												0x01 // PGA gain. 001 = 1
#define ADS129X_ChannelSettings_GAIN_2												0x02 // PGA gain. 010 = 2
#define ADS129X_ChannelSettings_GAIN_3												0x03 // PGA gain. 011 = 3
#define ADS129X_ChannelSettings_GAIN_4												0x04 // PGA gain. 100 = 4
#define ADS129X_ChannelSettings_GAIN_8												0x05 // PGA gain. 101 = 8
#define ADS129X_ChannelSettings_GAIN_12											0x06 // PGA gain. 110 = 12
#define ADS129X_ChannelSettings_MUX_NormalElectrode					0x00 // Channel input. 000 = Normal electrode input (default)
#define ADS129X_ChannelSettings_MUX_Shorted									0x01 // Channel input. 001 = Input shorted (for offset or noise measurements)
#define ADS129X_ChannelSettings_MUX_RldMeasurements					0x02 // Channel input. 010 = Used in conjunction with RLD_MEAS bit for RLD measurements
#define ADS129X_ChannelSettings_MUX_SupplyMeasurement				0x03 // Channel input. 011 = MVDD for supply measurement
#define ADS129X_ChannelSettings_MUX_TemperatureSensor				0x04 // Channel input. 100 = Temperature sensor
#define ADS129X_ChannelSettings_MUX_TestSignal								0x05 // Channel input. 101 = Test signal
#define ADS129X_ChannelSettings_MUX_RLD_DRP									0x06 // Channel input. 110 = RLD_DRP (positive electrode is the driver)
#define ADS129X_ChannelSettings_MUX_RLD_DRN									0x07 // Channel input. 111 = RLD_DRN (negative electrode is the driver)

// 	RLD_SENSP
//	This register controls the selection of the positive signals from each channel for right leg drive derivation.
typedef struct
{
	uint8_t Rld1P			:1;
	uint8_t Rld2P			:1;
	uint8_t Rld3P			:1;
	uint8_t Rld4P			:1;
	uint8_t Rld5P			:1;
	uint8_t Rld6P			:1;
	uint8_t Rld7P			:1;
	uint8_t Rld8P			:1;
}ADS129X_RldSensP_t;

// 	RLD_SENSP
//	This register controls the selection of the negative signals from each channel for right leg drive derivation.
typedef struct
{
	uint8_t Rld1N			:1;
	uint8_t Rld2N			:1;
	uint8_t Rld3N			:1;
	uint8_t Rld4N			:1;
	uint8_t Rld5N			:1;
	uint8_t Rld6N			:1;
	uint8_t Rld7N			:1;
	uint8_t Rld8N			:1;
}ADS129X_RldSensN_t;


// 	LOFF_SENSP
//	This register selects the positive side from each channel for lead-off detection.
typedef struct
{
	uint8_t Loff1P			:1;
	uint8_t Loff2P			:1;
	uint8_t Loff3P			:1;
	uint8_t Loff4P			:1;
	uint8_t Loff5P			:1;
	uint8_t Loff6P			:1;
	uint8_t Loff7P			:1;
	uint8_t Loff8P			:1;
}ADS129X_LoffSensP_t;

// 	LOFF_SENSN
//	This register selects the negative side from each channel for lead-off detection.
typedef struct
{
	uint8_t Loff1N			:1;
	uint8_t Loff2N			:1;
	uint8_t Loff3N			:1;
	uint8_t Loff4N			:1;
	uint8_t Loff5N			:1;
	uint8_t Loff6N			:1;
	uint8_t Loff7N			:1;
	uint8_t Loff8N			:1;
}ADS129X_LoffSensN_t;

// 	LOFF_FLIP
//	This register controls the direction of the current used for lead-off derivation.
typedef struct
{
	uint8_t LoffFlip1			:1;
	uint8_t LoffFlip2			:1;
	uint8_t LoffFlip3			:1;
	uint8_t LoffFlip4			:1;
	uint8_t LoffFlip5			:1;
	uint8_t LoffFlip6			:1;
	uint8_t LoffFlip7			:1;
	uint8_t LoffFlip8			:1;
}ADS129X_LoffFlip_t;

// 	LOFF_STATP (Read-Only Register)
//	This register stores the status of whether the positive electrode on each channel is on or off.
typedef struct
{
	uint8_t In1P_off			:1;
	uint8_t In2P_off			:1;
	uint8_t In3P_off			:1;
	uint8_t In4P_off			:1;
	uint8_t In5P_off			:1;
	uint8_t In6P_off			:1;
	uint8_t In7P_off			:1;
	uint8_t In8P_off			:1;
}ADS129X_LoffStatP_t;

// 	LOFF_STATN (Read-Only Register)
//	This register stores the status of whether the negative electrode on each channel is on or off.
typedef struct
{
	uint8_t In1N_off			:1;
	uint8_t In2N_off			:1;
	uint8_t In3N_off			:1;
	uint8_t In4N_off			:1;
	uint8_t In5N_off			:1;
	uint8_t In6N_off			:1;
	uint8_t In7N_off			:1;
	uint8_t In8N_off			:1;
}ADS129X_LoffStatN_t;

// 	GPIO: General-Purpose I/O Register
//	The General-Purpose I/O Register controls the action of the three GPIO pins.
typedef struct
{
	uint8_t GPIOControl		:4;	//	These bits determine if the corresponding GPIOD pin is an input or output. 0 = Output, 1 = Input (default)
	uint8_t GPIOData			:4;	// 	These bits are used to read and write data to the GPIO ports.
}ADS129X_GPIO_t;

// PACE: PACE Detect Register
// This register provides the PACE controls that configure the channel signal used to feed the external PACE detect circuitry.
typedef struct
{
	uint8_t PaceDetectBuffer		:1;	// 	PACE detect buffer. 0 = PACE detect buffer turned off (default) 1 = PACE detect buffer turned on	
	uint8_t PaceOddChannels			:2;	// 	These bits control the selection of the odd number channels available on TEST_PACE_OUT2
	uint8_t PaceEvenChannels		:2;	//	These bits control the selection of the even number channels available on TEST_PACE_OUT1.
	uint8_t Always0							:3;	//	Always 0
}ADS129X_Pace_t;


#define PaceEvenCh2		0x00 // PACE even channels. 00 = Channel 2 (default)
#define PaceEvenCh4		0x01 // PACE even channels. 01 = Channel 4
#define PaceEvenCh6		0x02 // PACE even channels. 10 = Channel 6
#define PaceEvenCh8		0x03 // PACE even channels. 11 = Channel 8
#define PaceOddCh1		0x00 // PACE odd channels. 00 = Channel 1 (default)
#define PaceOddCh3		0x01 // PACE odd channels. 01 = Channel 3
#define PaceOddCh5		0x02 // PACE odd channels. 10 = Channel 5
#define PaceOddCh7		0x03 // PACE odd channels. 11 = Channel 7

// RESP: Respiration Control Register
// This register provides the controls for the respiration circuitry
typedef struct
{
	uint8_t RespControl							:2;	
	uint8_t RespPhase								:3;	
	uint8_t Always1									:1;	
	uint8_t RespModulationEnable		:1;	
	uint8_t RespDemodulationEnable	:1;
}ADS129X_Resp_t;

#define ADS129X_RespPhase_22									0x00 // Respiration phase. 000 = 22.5° (default)
#define ADS129X_RespPhase_45									0x01 // Respiration phase. 001 = 45°
#define ADS129X_RespPhase_67									0x02 // Respiration phase. 010 = 67.5°
#define ADS129X_RespPhase_90									0x03 // Respiration phase. 011 = 90°
#define ADS129X_RespPhase_112								0x04 // Respiration phase. 100 = 112.5°
#define ADS129X_RespPhase_135								0x05 // Respiration phase. 101 = 135°
#define ADS129X_RespPhase_157								0x06 // Respiration phase. 110 = 157.5°
#define ADS129X_RespControlOff								0x00 // Respiration control. 00 = No respiration (default)
#define ADS129X_RespControlExternalResp			0x01 // Respiration control. 00 = No respiration (default)
#define ADS129X_RespControlInternalSignals		0x02 // Respiration control. 10 = Internal respiration with internal signals
#define ADS129X_RespCOntrolUserSignals				0x03 // Respiration control. 11 = Internal respiration with user-generated signals

// CONFIG4: Configuration Register 4
typedef struct
{
	uint8_t Always0											:1;	
	uint8_t LeadOffComparatorPowerOff		:1;	// Lead-off comparator power-down. 0 = Lead-off comparators disabled (default) 1 = Lead-off comparators enabled
	uint8_t ConnectWctToRld							:1;	// Connects the WCT to the RLD. 0 = WCT to RLD connection off (default) 1 = WCT to RLD connection on
	uint8_t SingleShotConvEn						:1;	// Single-shot conversion. 0 = Continuous conversion mode (default) 1 = Single-shot mode	
	uint8_t Always00											:1;	
	uint8_t RespirationModFreq 					:3;	// Respiration modulation frequency.
}ADS129X_Config4_t;

#define ADS129X_RespirationModFreq_64k				0x00 // Respiration modulation frequency. 000 = 64kHz modulation clock
#define ADS129X_RespirationModFreq_32k				0x01 // Respiration modulation frequency. 001 = 32kHz modulation clock
#define ADS129X_RespirationModFreq_16k				0x02 // Respiration modulation frequency. 010 = 16kHz square wave on GPIO3 and GPIO04. Output on GPIO4 is 180 degree out of phase with GPIO3.
#define ADS129X_RespirationModFreq_8k				0x03 // Respiration modulation frequency. 011 = 8kHz square wave on GPIO3 and GPIO04. Output on GPIO4 is 180 degree out of phase with GPIO3.
#define ADS129X_RespirationModFreq_4k				0x04 // Respiration modulation frequency. 100 = 4kHz square wave on GPIO3 and GPIO04. Output on GPIO4 is 180 degree out of phase with GPIO3
#define ADS129X_RespirationModFreq_2k				0x05 // Respiration modulation frequency. 101 = 2kHz square wave on GPIO3 and GPIO04. Output on GPIO4 is 180 degree out of phase with GPIO3
#define ADS129X_RespirationModFreq_1k				0x06 // Respiration modulation frequency. 110 = 1kHz square wave on GPIO3 and GPIO04. Output on GPIO4 is 180 degree out of phase with GPIO3.
#define ADS129X_RespirationModFreq_500				0x07 // Respiration modulation frequency. 111 = 500Hz square wave on GPIO3 and GPIO04. Output on GPIO4 is 180 degree out of phase with GPIO3

// WCT1: Wilson Central Terminal and Augmented Lead Control Register
// The WCT1 control register configures the device WCT circuit channel selection and the augmented leads
typedef struct
{
	uint8_t WctAmpChASel								:3;	//	WCT amplifier A channel selection;
	uint8_t PowerDownWctA								:1;	// 	Power-down WCTA. 0 = Powered down (default) 1 = Powered on
	uint8_t aVRCh4EnableWctToNegInCh4		:1;	//	Enable (WCTB + WCTB)/2 to the negative input of channel 4 0 = Disabled (default) 1 = Enabled
	uint8_t aVRCh7EnableWctToNegInCh7		:1;	// 	Enable (WCTB + WCTB)/2 to the negative input of channel 7 0 = Disabled (default) 1 = Enabled
	uint8_t aVRCh5EnableWctToNegInCh5		:1;	//	Enable (WCTA + WCTB)/2 to the negative input of channel 5 0 = Disabled (default) 1 = Enabled
	uint8_t aVRCh6EnableWctToNegInCh6 	:1;	// 	Enable (WCTA + WCTB)/2 to the negative input of channel 6 0 = Disabled (default) 1 = Enabled
}ADS129X_WctControl1_t;

#define ADS129X_WctAmpChASel_Ch1P				0x00 // WCT amplifier A channel selection; typically connected to RA electrode. 000 = Channel 1 positive input connected to WCTA amplifier (default)
#define ADS129X_WctAmpChASel_Ch1N				0x01 // WCT amplifier A channel selection; typically connected to RA electrode. 001 = Channel 1 negative input connected to WCTA amplifier
#define ADS129X_WctAmpChASel_Ch2P				0x02 // WCT amplifier A channel selection; typically connected to RA electrode. 010 = Channel 2 positive input connected to WCTA amplifier
#define ADS129X_WctAmpChASel_Ch2N				0x03 // WCT amplifier A channel selection; typically connected to RA electrode. 011 = Channel 2 negative input connected to WCTA amplifier
#define ADS129X_WctAmpChASel_Ch3P				0x04 // WCT amplifier A channel selection; typically connected to RA electrode. 100 = Channel 3 positive input connected to WCTA amplifier
#define ADS129X_WctAmpChASel_Ch3N				0x05 // WCT amplifier A channel selection; typically connected to RA electrode. 101 = Channel 3 negative input connected to WCTA amplifier
#define ADS129X_WctAmpChASel_Ch4P				0x06 // WCT amplifier A channel selection; typically connected to RA electrode. 110 = Channel 4 positive input connected to WCTA amplifier
#define ADS129X_WctAmpChASel_Ch4N				0x07 // WCT amplifier A channel selection; typically connected to RA electrode. 111 = Channel 4 negative input connected to WCTA amplifier

// WCT2: Wilson Central Terminal Control Register
// The WCT2 configuration register configures the device WCT circuit channel selection.
typedef struct
{
	uint8_t WctAmpCChSel		:3;	//	WCT amplifier C channel selection; typically connected to LL electrode.
	uint8_t WctAmpBChSel		:3;	// 	WCT amplifier B channel selection; typically connected to LA electrode.
	uint8_t PowerDownWctB		:1;	//	Enable (WCTB + WCTB)/2 to the negative input of channel 4 0 = Disabled (default) 1 = Enabled
	uint8_t PowerDownWctA		:1;	// 	Enable (WCTB + WCTB)/2 to the negative input of channel 7 0 = Disabled (default) 1 = Enabled
	
}ADS129X_WctControl2_t;

#define ADS129X_WctAmpChBSel_Ch1P				0x00 // WCT amplifier B channel selection; typically connected to LA electrode. 000 = Channel 1 positive input connected to WCTB amplifier (default)
#define ADS129X_WctAmpChBSel_Ch1N				0x01 // WCT amplifier B channel selection; typically connected to LA electrode. 000 = Channel 1 negative input connected to WCTB amplifier
#define ADS129X_WctAmpChBSel_Ch2P				0x02 // WCT amplifier B channel selection; typically connected to LA electrode. 000 = Channel 2 positive input connected to WCTB amplifier
#define ADS129X_WctAmpChBSel_Ch2N				0x03 // WCT amplifier B channel selection; typically connected to LA electrode. 000 = Channel 2 negative input connected to WCTB amplifier
#define ADS129X_WctAmpChBSel_Ch3P				0x04 // WCT amplifier B channel selection; typically connected to LA electrode. 000 = Channel 3 positive input connected to WCTB amplifier
#define ADS129X_WctAmpChBSel_Ch3N				0x05 // WCT amplifier B channel selection; typically connected to LA electrode. 000 = Channel 3 negative input connected to WCTB amplifier
#define ADS129X_WctAmpChBSel_Ch4P				0x06 // WCT amplifier B channel selection; typically connected to LA electrode. 000 = Channel 4 positive input connected to WCTB amplifier
#define ADS129X_WctAmpChBSel_Ch4N				0x07 // WCT amplifier B channel selection; typically connected to LA electrode. 000 = Channel 4 negative input connected to WCTB amplifier
#define ADS129X_WctAmpChCSel_Ch1P				0x00 // WCT amplifier C channel selection; typically connected to LL electrode. 000 = Channel 1 positive input connected to WCTC amplifier (default)
#define ADS129X_WctAmpChCSel_Ch1N				0x01 // WCT amplifier C channel selection; typically connected to LL electrode. 001 = Channel 1 negative input connected to WCTC amplifier
#define ADS129X_WctAmpChCSel_Ch2P				0x02 // WCT amplifier C channel selection; typically connected to LL electrode. 010 = Channel 2 positive input connected to WCTC amplifier
#define ADS129X_WctAmpChCSel_Ch2N				0x03 // WCT amplifier C channel selection; typically connected to LL electrode. 011 = Channel 2 negative input connected to WCTC amplifier
#define ADS129X_WctAmpChCSel_Ch3P				0x04 // WCT amplifier C channel selection; typically connected to LL electrode. 100 = Channel 3 positive input connected to WCTC amplifier
#define ADS129X_WctAmpChCSel_Ch3N				0x05 // WCT amplifier C channel selection; typically connected to LL electrode. 101 = Channel 3 negative input connected to WCTC amplifier
#define ADS129X_WctAmpChCSel_Ch4P				0x06 // WCT amplifier C channel selection; typically connected to LL electrode. 110 = Channel 4 positive input connected to WCTC amplifier
#define ADS129X_WctAmpChCSel_Ch4N				0x07 // WCT amplifier C channel selection; typically connected to LL electrode. 111 = Channel 4 negative input connected to WCTC amplifier

typedef struct
{
	uint8_t ID;
	ADS129X_config1_t CONFIG1;
	ADS129X_config2_t CONFIG2;
	ADS129X_config3_t CONFIG3;
	ADS129X_LeadOffControl_t LOFF;
	ADS129X_ChannelSettings_t CH1SET;
	ADS129X_ChannelSettings_t CH2SET;
	ADS129X_ChannelSettings_t CH3SET;
	ADS129X_ChannelSettings_t CH4SET;
	ADS129X_ChannelSettings_t CH5SET;
	ADS129X_ChannelSettings_t CH6SET;
	ADS129X_ChannelSettings_t CH7SET;
	ADS129X_ChannelSettings_t CH8SET;
	ADS129X_RldSensP_t RLD_SENSP;
	ADS129X_RldSensN_t RLD_SENSN;
	ADS129X_LoffSensP_t LOFF_SENSP;
	ADS129X_LoffSensN_t LOFF_SENSN;
	ADS129X_LoffFlip_t LOFF_FLIP;
	ADS129X_LoffStatP_t LOFF_STATP;
	ADS129X_LoffStatN_t LOFF_STATN;
	ADS129X_GPIO_t GPIO;
	ADS129X_Pace_t PACE;
	ADS129X_Resp_t RESP;
	ADS129X_Config4_t CONFIG4;
	ADS129X_WctControl1_t WCT1;
	ADS129X_WctControl2_t WCT2;
}ADS129X_reg_t;


typedef struct
{
	uint8_t CONST;
	uint8_t LOFF_STATP;
	uint8_t LOFF_STATN;
	uint8_t GPIO;
	
	uint32_t CH1;
	uint32_t CH2;
	uint32_t CH3;
	uint32_t CH4;
	uint32_t CH5;
	uint32_t CH6;
	uint32_t CH7;
	uint32_t CH8;
} ADS1298_RDATA_result;

typedef struct
{
	uint32_t Gpio 								:4;
	uint32_t LeadOffFaultStatusN 	:8;
	uint32_t LeadOffFaultStatusP 	:8;
	uint32_t Const 								:4;
	uint32_t Dummy 								:8;
	
	int32_t CH1;
	int32_t CH2;
	int32_t CH3;
	int32_t CH4;
} ADS1294_RDATA_result_t;

typedef struct
{
	uint32_t Gpio 								:4;
	uint32_t LeadOffFaultStatusN 	:8;
	uint32_t LeadOffFaultStatusP 	:8;
	uint32_t Const 								:4;
	uint32_t Dummy 								:8;
	
	int32_t CH1;
	int32_t CH2;
	int32_t CH3;
	int32_t CH4;
	int32_t CH5;
	int32_t CH6;
} ADS1296_RDATA_result_t;
						
						

void ADS1294r_init(ADS129X_reg_t* registrai);
void ADS1296r_init(ADS129X_reg_t* registrai);

uint8_t ADS129X_RegRead(uint8_t addr);
void ADS129X_RegWrite(uint8_t addr, uint8_t value);
void ADS129X_RegReadAll(ADS129X_reg_t *registrai);
void ADS129X_RegWriteAll(ADS129X_reg_t *registrai);
void ADS129X_Cmd(uint8_t com);
void ADS129X_CmdReset(void);
void ADS129X_CmdSdatac(void);
void ADS1294_DataRead(ADS1294_RDATA_result_t *struktura);
void ADS1296_DataRead(ADS1296_RDATA_result_t *struktura);



#endif

