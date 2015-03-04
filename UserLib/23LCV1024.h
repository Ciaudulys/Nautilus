#ifndef RAM_H
#define RAM_H

#include "stdint.h"
#include "inc\hw_ssi.h"
#include <23LCV1024_hw.h>




void RAM_Byte_Write(uint32_t addres, uint8_t data);
uint8_t RAM_Byte_Read(uint32_t addres);

void RAM_array_Write(uint32_t StartAdress, uint8_t *data, uint32_t count);
void RAM_array_Read(uint32_t StartAdress, uint8_t *data, uint32_t count);

void RAM_array_DMA_Wr(uint32_t StartAdress, uint8_t *data, uint32_t count);
void RAM_array_DMA_Rd(uint32_t StartAdress, uint8_t *data, uint32_t count);
















#endif
