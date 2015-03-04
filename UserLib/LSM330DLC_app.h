#ifndef LSM330DLC_app_H_
#define LSM330DLC_app_H_

#include <stdint.h>
#include <LSM330DLC.h>
#include <rtl.h>


typedef struct 
{
	uint16_t Mas0[256];
	uint16_t Mas1[256];
	uint8_t ArrCnt;
	uint8_t cnt;
	uint8_t Mas0Status :1;
	uint8_t Mas1Status :1;
}LSM330DLC_DataBuff_t;

void LSM330DLC_AccelDataPut(LSM330DLC_AccelData_t* data, LSM330DLC_DataBuff_t* DataBuff);
void LSM330DLC_GyroDataPut(LSM330DLC_GyroData_t* data, LSM330DLC_DataBuff_t* DataBuff);
void LSM330DLC_DataPut(LSM330DLC_AccelData_t * AccelData, LSM330DLC_GyroData_t* GyroData, LSM330DLC_DataBuff_t* DataBuff);

#endif
