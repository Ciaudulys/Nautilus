#include <AFE4490_app.h>

void AFE4490A_DataPut(AFE4490_Result_t* data, AFE4490_DataBuff_t* DataBuff, OS_ID mbx)
{	
	
	switch(DataBuff->ArrCnt)
		{
			case 0:
				DataBuff->AFE4490_Mas0[DataBuff->cnt] = data->LED1;
				DataBuff->cnt++;
				DataBuff->AFE4490_Mas0[DataBuff->cnt] = data->LED2;
				DataBuff->cnt++;
			
			break;
			case 1:
				DataBuff->AFE4490_Mas1[DataBuff->cnt] = data->LED1;
				DataBuff->cnt++;
				DataBuff->AFE4490_Mas1[DataBuff->cnt] = data->LED2;
				DataBuff->cnt++;
			break;
			case 2:
				DataBuff->AFE4490_Mas2[DataBuff->cnt] = data->LED1;
				DataBuff->cnt++;
				DataBuff->AFE4490_Mas2[DataBuff->cnt] = data->LED2;
				DataBuff->cnt++;
			break;
		}
		
		if (DataBuff->cnt == 32 )
		{
			DataBuff->cnt = 0;
			switch(DataBuff->ArrCnt)
			{
				case 0:
					os_mbx_send (mbx, (uint32_t*)0xA0000000, 0xFFFF);	//uzpildytas nulinis AFE4490 masyvas
				break;
				case 1:
					os_mbx_send (mbx, (uint32_t*)0xA0000001, 0xFFFF);	//uzpildytas pirmas AFE4490 masyvas
				break;
				case 2:
					os_mbx_send (mbx, (uint32_t*)0xA0000002, 0xFFFF);	//uzpildytas antras AFE4490 masyvas
				break;
			}
			DataBuff->ArrCnt++;
			if(DataBuff->ArrCnt == 3)
			{
				DataBuff->ArrCnt = 0;
			}
			
		}
}

void AFE4490B_DataPut(AFE4490_Result_t* data, AFE4490_DataBuff_t* DataBuff, OS_ID mbx)
{	
	
	switch(DataBuff->ArrCnt)
		{
			case 0:
				DataBuff->AFE4490_Mas0[DataBuff->cnt] = data->LED1;
				DataBuff->cnt++;
				DataBuff->AFE4490_Mas0[DataBuff->cnt] = data->LED2;
				DataBuff->cnt++;
			break;
			case 1:
				DataBuff->AFE4490_Mas1[DataBuff->cnt] = data->LED1;
				DataBuff->cnt++;
				DataBuff->AFE4490_Mas1[DataBuff->cnt] = data->LED2;
				DataBuff->cnt++;
			break;
			case 2:
				DataBuff->AFE4490_Mas2[DataBuff->cnt] = data->LED1;
				DataBuff->cnt++;
				DataBuff->AFE4490_Mas2[DataBuff->cnt] = data->LED2;
				DataBuff->cnt++;
			break;
		}
		
		if (DataBuff->cnt == 32 )
		{
			DataBuff->cnt = 0;
			switch(DataBuff->ArrCnt)
			{
				case 0:
					os_mbx_send (mbx, (uint32_t*)0xB0000000, 0xFFFF);	//uzpildytas nulinis AFE4490 masyvas
				break;
				case 1:
					os_mbx_send (mbx, (uint32_t*)0xB0000001, 0xFFFF);	//uzpildytas pirmas AFE4490 masyvas
				break;
				case 2:
					os_mbx_send (mbx, (uint32_t*)0xB0000002, 0xFFFF);	//uzpildytas antras AFE4490 masyvas
				break;
			}
			DataBuff->ArrCnt++;
			if(DataBuff->ArrCnt == 3)
			{
				DataBuff->ArrCnt = 0;
			}
			
		}
}

void AFE4490C_DataPut(AFE4490_Result_t* data, AFE4490_DataBuff_t* DataBuff, OS_ID mbx)
{	
	
	switch(DataBuff->ArrCnt)
		{
			case 0:
				DataBuff->AFE4490_Mas0[DataBuff->cnt] = data->LED1;
				DataBuff->cnt++;
				DataBuff->AFE4490_Mas0[DataBuff->cnt] = data->LED2;
				DataBuff->cnt++;
			break;
			case 1:
				DataBuff->AFE4490_Mas1[DataBuff->cnt] = data->LED1;
				DataBuff->cnt++;
				DataBuff->AFE4490_Mas1[DataBuff->cnt] = data->LED2;
				DataBuff->cnt++;
			break;
			case 2:
				DataBuff->AFE4490_Mas2[DataBuff->cnt] = data->LED1;
				DataBuff->cnt++;
				DataBuff->AFE4490_Mas2[DataBuff->cnt] = data->LED2;
				DataBuff->cnt++;
			break;
		}
		
		if (DataBuff->cnt == 32 )
		{
			DataBuff->cnt = 0;
			switch(DataBuff->ArrCnt)
			{
				case 0:
					os_mbx_send (mbx, (uint32_t*)0xC0000000, 0xFFFF);	//uzpildytas nulinis AFE4490 masyvas
				break;
				case 1:
					os_mbx_send (mbx, (uint32_t*)0xC0000001, 0xFFFF);	//uzpildytas pirmas AFE4490 masyvas
				break;
				case 2:
					os_mbx_send (mbx, (uint32_t*)0xC0000002, 0xFFFF);	//uzpildytas antras AFE4490 masyvas
				break;
			}
			DataBuff->ArrCnt++;
			if(DataBuff->ArrCnt == 3)
			{
				DataBuff->ArrCnt = 0;
			}
			
		}
}
