#ifndef data_structure_H
#define data_structure_H

#include "stdint.h"


#define Kontrole_check 0x50281567

//sektoriu adresai
#define Kontrole_flash 0
#define laiko_zymes_flash_start 1
#define laiko_zymes_flash_end 8
#define ADS1298X_flash_start 9
#define ADS1298X_flash_end 5515755
#define AFE4490A_flash_start 5515756
#define AFE4490A_flash_end 6205223
#define AFE4490B_flash_start 6205224
#define AFE4490B_flash_end 6894691
#define AFE4490C_flash_start 6894692
#define AFE4490C_flash_end 7584159
#define Gyro_flash_start 7584160
#define Gyro_flash_end 7610635
#define Accel_flash_start 7610636
#define Accel_flash_end 7637111
#define Temp_flash_start 7637112
#define Temp_flash_end 7672412
#define Kvepavimas_flash_start 7672413
#define Kvepavimas_flash_end 7690062
#define Slegis_flash_start 7690063
#define Slegis_flash_end 7690239

//kotroles sektoriaus indeksai
#define Check_bit_idx 0
#define ADS1298X_start_idx 4
#define AFE4490A_start_idx 8
#define AFE4490B_start_idx 12
#define AFE4490C_start_idx 16
#define Gyro_start_idx 20
#define Accel_start_idx 24
#define Temp_start_idx 28
#define Kvepavimas_start_idx 32
#define Slegis_start_idx 36
#define laiko_zymes_start_idx 40

#define ADS1298X_sk_idx 44
#define AFE4490A_sk_idx 48
#define AFE4490B_sk_idx 52
#define AFE4490C_sk_idx 56
#define Gyro_sk_idx 60
#define Accel_sk_idx 64
#define Temp_sk_idx 68
#define Kvepavimas_sk_idx 72
#define Slegis_sk_idx 76
#define laiko_zymes_sk_idx 80




void Kontrole_init(uint8_t * mas);

















#endif
