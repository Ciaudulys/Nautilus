#include <LSM330DLC_app.h>


void LSM330DLC_AccelDataPut(LSM330DLC_AccelData_t* data, LSM330DLC_DataBuff_t* DataBuff)
{	
	
	switch(DataBuff->ArrCnt)
		{
			case 0:
				DataBuff->Mas0[DataBuff->cnt] = data->X;
				DataBuff->cnt++;
				DataBuff->Mas0[DataBuff->cnt] = data->Y;
				DataBuff->cnt++;
				DataBuff->Mas0[DataBuff->cnt] = data->Z;
				DataBuff->cnt++;
			break;
			case 1:
				DataBuff->Mas1[DataBuff->cnt] = data->X;
				DataBuff->cnt++;
				DataBuff->Mas1[DataBuff->cnt] = data->Y;
				DataBuff->cnt++;
				DataBuff->Mas1[DataBuff->cnt] = data->Z;
				DataBuff->cnt++;
			break;
		}
		
		if (DataBuff->cnt == 255 )
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

void LSM330DLC_GyroDataPut(LSM330DLC_GyroData_t* data, LSM330DLC_DataBuff_t* DataBuff)
{
	switch(DataBuff->ArrCnt)
		{
			case 0:
				DataBuff->Mas0[DataBuff->cnt] = data->Roll;
				DataBuff->cnt++;
				DataBuff->Mas0[DataBuff->cnt] = data->Pitch;
				DataBuff->cnt++;
				DataBuff->Mas0[DataBuff->cnt] = data->Yaw;
				DataBuff->cnt++;
			break;
			case 1:
				DataBuff->Mas1[DataBuff->cnt] = data->Roll;
				DataBuff->cnt++;
				DataBuff->Mas1[DataBuff->cnt] = data->Pitch;
				DataBuff->cnt++;
				DataBuff->Mas1[DataBuff->cnt] = data->Yaw;
				DataBuff->cnt++;
			break;
		}
		
		if (DataBuff->cnt == 255 )
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

void LSM330DLC_DataPut(LSM330DLC_AccelData_t * AccelData, LSM330DLC_GyroData_t* GyroData, LSM330DLC_DataBuff_t* DataBuff)
{
	switch(DataBuff->ArrCnt)
		{
			case 0:
				DataBuff->Mas0[DataBuff->cnt] = AccelData->X;
				DataBuff->cnt++;
				DataBuff->Mas0[DataBuff->cnt] = AccelData->Y;
				DataBuff->cnt++;
				DataBuff->Mas0[DataBuff->cnt] = AccelData->Z;
				DataBuff->cnt++;
				DataBuff->Mas0[DataBuff->cnt] = GyroData->Roll;
				DataBuff->cnt++;
				DataBuff->Mas0[DataBuff->cnt] = GyroData->Pitch;
				DataBuff->cnt++;
				DataBuff->Mas0[DataBuff->cnt] = GyroData->Yaw;
				DataBuff->cnt++;
			break;
			case 1:
				DataBuff->Mas1[DataBuff->cnt] = AccelData->X;
				DataBuff->cnt++;
				DataBuff->Mas1[DataBuff->cnt] = AccelData->Y;
				DataBuff->cnt++;
				DataBuff->Mas1[DataBuff->cnt] = AccelData->Z;
				DataBuff->cnt++;
				DataBuff->Mas1[DataBuff->cnt] = GyroData->Roll;
				DataBuff->cnt++;
				DataBuff->Mas1[DataBuff->cnt] = GyroData->Pitch;
				DataBuff->cnt++;
				DataBuff->Mas1[DataBuff->cnt] = GyroData->Yaw;
				DataBuff->cnt++;
			break;
		}
		
		if (DataBuff->cnt == 240)
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
