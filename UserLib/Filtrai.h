/*
 * filtrai.h
 *
 *  Created on: 2012.09.18
 *      Author: Admin
 */
#ifndef FILTRAI_H_
#define FILTRAI_H_

#include <stdint.h>
#include <stdbool.h>
#include <math.h>

float PpgDcpass(float *outOld, float inSig);

float LowPass3Hz (float in,float *m1);
float LowPass10Hz(float in,float *m1);
float AveragerSpo2(float in,float	*Array);

void calcstat (signed int *a, signed int *avg, float *sd);
signed int middle_of_3(signed int a, signed int b, signed int c);

// RR atpazinimui
uint8_t RR_atpazinimas(signed int value,uint16_t *RRI,uint16_t *ecg_heartrate);
int HRVlpfilter(signed int sampleHRVlp);
int HRVhpfilter(signed int sampleHRVhp);
// Breathing rate atpazinimui
uint8_t respiration_rate(int32_t val,uint16_t *PPI, uint16_t *resp_rate);

#endif /* FILTRAI_H_ */
