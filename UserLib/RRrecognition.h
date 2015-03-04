/*
 * RRrecognition.h
 *
 *  Created on: 2013.05.30
 *      Author: Adminas
 */

#ifndef RRRECOGNITION_H_
#define RRRECOGNITION_H_

void RR_Init(void);
unsigned short RRrecognition(signed long Sample, unsigned int SamplingRate);
signed long rrFilter(signed long EkgSample, unsigned char SamplingRate);

#endif /* RRRECOGNITION_H_ */
