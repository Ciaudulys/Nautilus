#ifndef ads1296r_app_H_
#define ads1296r_app_H_

#include <stdint.h>
#include <ads1294r.h>
#include <rtl.h>

//ADS1294R


typedef struct 
{
	int32_t ADS1296R_Mas0[32];
	int32_t ADS1296R_Mas1[32];
	int32_t ADS1296R_Mas2[32];
	uint8_t cnt;
	uint8_t ArrCnt;
}ADS1296R_DataBuff_t;

void ADS1296_DataPut(ADS1296_RDATA_result_t* data, ADS1296R_DataBuff_t* DataBuff, OS_ID mbx);

#endif
