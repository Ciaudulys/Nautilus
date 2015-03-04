#ifndef afe4490_app_H_
#define afe4490_app_H_

#include <stdint.h>
#include <AFE4490.h>
#include <rtl.h>


typedef struct 
{
	int32_t AFE4490_Mas0[32];
	int32_t AFE4490_Mas1[32];
	int32_t AFE4490_Mas2[32];
	uint8_t cnt;
	uint8_t ArrCnt;
}AFE4490_DataBuff_t;

void AFE4490A_DataPut(AFE4490_Result_t* data, AFE4490_DataBuff_t* DataBuff, OS_ID mbx);
void AFE4490B_DataPut(AFE4490_Result_t* data, AFE4490_DataBuff_t* DataBuff, OS_ID mbx);
void AFE4490C_DataPut(AFE4490_Result_t* data, AFE4490_DataBuff_t* DataBuff, OS_ID mbx);

#endif
