#include "data_structure.h"



void Kontrole_init(uint8_t * mas)
{
	uint32_t * ptr;
	ptr = (uint32_t*)&mas[0];
	
	if(*ptr == Kontrole_check)
	{
		
	}else
	{
		*ptr = Kontrole_check;
		ptr = (uint32_t*)&mas[AFE4490A_sk_idx];
		*ptr = 0;
		ptr = (uint32_t*)&mas[AFE4490B_sk_idx];
		*ptr = 0;
		ptr = (uint32_t*)&mas[AFE4490C_sk_idx];
		*ptr = 0;
		ptr = (uint32_t*)&mas[ADS1298X_sk_idx];
		*ptr = 0;
		ptr = (uint32_t*)&mas[Temp_sk_idx];
		*ptr = 0;
		ptr = (uint32_t*)&mas[Gyro_sk_idx];
		*ptr = 0;
		ptr = (uint32_t*)&mas[Accel_sk_idx];
		*ptr = 0;
		ptr = (uint32_t*)&mas[Kvepavimas_sk_idx];
		*ptr = 0;
		ptr = (uint32_t*)&mas[Slegis_sk_idx];
		*ptr = 0;
		ptr = (uint32_t*)&mas[laiko_zymes_sk_idx];
		*ptr = 0;
		
		ptr = (uint32_t*)&mas[ADS1298X_start_idx];
		*ptr = ADS1298X_flash_start;
		ptr = (uint32_t*)&mas[AFE4490A_start_idx];
		*ptr = AFE4490A_flash_start;
		ptr = (uint32_t*)&mas[AFE4490B_start_idx];
		*ptr = AFE4490B_flash_start;
		ptr = (uint32_t*)&mas[AFE4490C_start_idx];
		*ptr = AFE4490C_flash_start;
		ptr = (uint32_t*)&mas[Temp_start_idx];
		*ptr = Temp_flash_start;
		ptr = (uint32_t*)&mas[Gyro_start_idx];
		*ptr = Gyro_flash_start;
		ptr = (uint32_t*)&mas[Accel_start_idx];
		*ptr = Accel_flash_start;
		ptr = (uint32_t*)&mas[Kvepavimas_start_idx];
		*ptr = Kvepavimas_flash_start;
		ptr = (uint32_t*)&mas[Slegis_start_idx];
		*ptr = Slegis_flash_start;
		ptr = (uint32_t*)&mas[laiko_zymes_start_idx];
		*ptr = laiko_zymes_flash_start;
		
	}
}
































