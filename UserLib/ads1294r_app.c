#include <ads1294r_app.h>


void ADS129X_DataPut(ADS1294_RDATA_result_t* data, ADS1294R_DataBuff_t* DataBuff, OS_ID mbx)
{	
	
	switch(DataBuff->ArrCnt)
		{
			case 0:
				DataBuff->ADS1294R_Mas0[DataBuff->cnt] = data->CH1;
				DataBuff->cnt++;
				DataBuff->ADS1294R_Mas0[DataBuff->cnt] = data->CH2;
				DataBuff->cnt++;
				DataBuff->ADS1294R_Mas0[DataBuff->cnt] = data->CH3;
				DataBuff->cnt++;
				DataBuff->ADS1294R_Mas0[DataBuff->cnt] = data->CH4;
				DataBuff->cnt++;
			break;
			case 1:
				DataBuff->ADS1294R_Mas1[DataBuff->cnt] = data->CH1;
				DataBuff->cnt++;
				DataBuff->ADS1294R_Mas1[DataBuff->cnt] = data->CH2;
				DataBuff->cnt++;
				DataBuff->ADS1294R_Mas1[DataBuff->cnt] = data->CH3;
				DataBuff->cnt++;
				DataBuff->ADS1294R_Mas1[DataBuff->cnt] = data->CH4;
				DataBuff->cnt++;
			break;
			case 2:
				DataBuff->ADS1294R_Mas2[DataBuff->cnt] = data->CH1;
				DataBuff->cnt++;
				DataBuff->ADS1294R_Mas2[DataBuff->cnt] = data->CH2;
				DataBuff->cnt++;
				DataBuff->ADS1294R_Mas2[DataBuff->cnt] = data->CH3;
				DataBuff->cnt++;
				DataBuff->ADS1294R_Mas2[DataBuff->cnt] = data->CH4;
				DataBuff->cnt++;
			break;
		}
		
		if (DataBuff->cnt == 32 )
		{
			DataBuff->cnt = 0;
			switch(DataBuff->ArrCnt)
			{
				case 0:
					os_mbx_send (mbx, (uint32_t*)0xE0000000, 0xFFFF);	//uzpildytas nulinis ADS129X masyvas
				break;
				case 1:
					os_mbx_send (mbx, (uint32_t*)0xE0000001, 0xFFFF);	//uzpildytas pirmas ADS129X masyvas
				break;
				case 2:
					os_mbx_send (mbx, (uint32_t*)0xE0000002, 0xFFFF);	//uzpildytas antras ADS129X masyvas
				break;
			}
			DataBuff->ArrCnt++;
			if(DataBuff->ArrCnt == 3)
			{
				DataBuff->ArrCnt = 0;
			}
			
		}
}

