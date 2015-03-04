/*
 * RRrecognition.c
 *
 *  Created on: 2013.05.30
 *      Author: Adminas
 */

#include "RRrecognition.h"
//----------- Globalūs kintamieji ------------------------
//signed short  Datain;         		//EKG atskaita
volatile unsigned char RWaveFound;      		//Rastas R dantelis
volatile unsigned short int  RRI;           		//Atskaitų skaičius tarp R dantelių

volatile unsigned long rrMax;           		//Pradinis maksimumas
volatile unsigned long rrMaxNew;        		//Naujas maksimumas
volatile unsigned long rrMaxTemp;       		//Laikinas maksimumas
volatile unsigned long rrMaxOld;        		//Buvęs maksimumas

//volatile unsigned long rri_counter;   //Diskretizavimo dažniu dirbantis skaitiklis
volatile unsigned long rrThreshold;
volatile unsigned long rrSkipping;

volatile unsigned long rrIndexNew;           //Naujas R dantelio indeksas
volatile unsigned long rrIndexOld;           //Buvusio R dantelio indeksas
volatile unsigned long rrIndexTemp;          //Laikinas R dantelio indeksas

volatile unsigned long rrInd;       //Algoritmo skaitiklis

//volatile signed long EkgSampleTemp;          //Laikina EKG atskaita
volatile unsigned long EkgSampleNew;           //Nauja EKG atskaita
volatile unsigned long EkgSampleOld;           //Buvusi EKG atskaita

volatile unsigned long Multiplier;           //Daugiklis. Nustatomas atsižvelgiant į diskretizavimo dažnį
//volatile unsigned char SamplingRate;         //Jei 1, tai Fd = 512, jei 0, tai Fd = 256
volatile unsigned char restart;

extern volatile unsigned int ebeat;
void RR_Init(void){
	rrMax = 0;                    //Pradinis maksimumas
	rrMaxNew = 0;                 //Naujas maksimumas
	rrMaxTemp = 0;                //Laikinas maksimumas
	rrMaxOld = 0;                 //Buvęs maksimumas

	//	rri_counter = 0;
	rrThreshold = 0;              //Slenkstis
	rrSkipping = 0;               //Nejautros intervalo skaitiklis

	rrIndexNew = 0;               //Naujas R dantelio indeksas
	rrIndexOld = 0;               //Buvusio R dantelio indeksas
	rrIndexTemp = 0;              //Laikinas R dantelio indeksas

	rrInd = 0;                    //Algoritmo skaitiklis

	//EkgSampleTemp = 0;            //Laikina EKG atskaita
	EkgSampleNew = 0;             //Laikina EKG atskaita
	EkgSampleOld = 0;
	restart = 0;
}

//---------------------------------------RR atpažinimo funkcija-----------------------------------------
unsigned short RRrecognition(signed long Sample, unsigned int SamplingRate)
{
	signed long ECGSample;
	static signed short ecg_heartrate;
	//if(SamplingRate == 1)                       //Jei SamplingRate = 1, tai Fd = 512, jei 0, tai Fd = 256
	Multiplier = 100;
	//else
	//	Multiplier = 50;

	//DAT.ekg1 = Sample;
	ECGSample = rrFilter(Sample, SamplingRate);
	if(ECGSample < 0)
		ECGSample = 0;                          //Neigiamas reikšmes lyginam 0.
	//DAT.ekg1 = ECGSample;

	EkgSampleOld = EkgSampleNew;                //Naują atskaitą išsaugom kaip buvusią
	EkgSampleNew = ECGSample;               //Esama atskaita išsaugom kaip naują

	//DAT.ekg3 = rrThreshold;

	if((rrInd < Multiplier*5)||(restart != 0)){ //Ieškom pradinio slenksčio 1.5 sek. intervale
		if(EkgSampleNew > EkgSampleOld){        //Tikrinam ar nauja atskaita didesne už buvusią
			rrMaxTemp = EkgSampleNew;           //Jei didesnė, tai nauja atskaita tampa laikinu maksimumu
			if(rrMaxTemp > rrMax){              //Ar laikinas maksimumas didesnis už esamą maksimumą?
				rrMax = rrMaxTemp;              //Jei taip, tai laikinas maksimumas tampa pradiniu maksimumu
				rrIndexOld = rrInd;             //Įsimenam maksimumo vieta
				if (restart == 1){
					if (rrInd > (rrIndexNew+500)){
						restart = 0;
					}
				}
			}
		}

		rrMaxOld = rrMax;                       //Esama maksimumą išsaugom kaip buvusi maksimumą (reikės tolimesniam apdorojimui)
		rrInd = rrInd + 1;
		rrThreshold = rrMax >> 1;               //Nustatome slenkstį pagal naują maksimumą (pusę maksimumo)
		rrSkipping = Multiplier*5 - rrIndexOld; //Nustatom kiek atskaitų praleisti po aptikto R dantelio
	}else{                                      //Pradinis slenkstis rastas, toliau ieškom R dantelių
		if(rrSkipping>Multiplier){              //Ar praeitas nejautros etapas?
			if(EkgSampleNew > rrThreshold){     //Ar naujoji atskaita didesnė už slenkstį?
				if(EkgSampleNew>EkgSampleOld){  //Ar naujoji atskaita didesnė už prieš tai buvusią atskaitą?
					rrMax = EkgSampleNew;       //Jei taip, tai naujoji atskaita tampa nauju maksimumu
					rrIndexNew = rrInd;         //Įsimenam naujo maksimumo vietą
				}
				else
				{
					RWaveFound = 1;             //Jei ne, tai radom R dantelį
					rrThreshold = (rrMaxOld + rrMax) >> 2; //Nustatom naują slenkstį. Imam 1/2 naujo maksimumo ir 1/2 prieš tai buvusio maksimumo
					rrMaxOld = rrMax;           //Naująjį maksimumą nustatom kaip buvusį

					RRI = rrIndexNew - rrIndexOld; //Skaičiuojam RR intervalo trukmę
					ecg_heartrate = 500*60/(rrIndexNew - rrIndexOld)+1;
					if (ecg_heartrate > 270){restart = 1;}; // test reset
					//ebeat = 5; // 10ms multiple
					//Gaunam atskaitų skaičių tarp RR dantelių
					//Norint gauti RR intervalo ilgį reikia dar padauginti iš 3.906, kai Fd = 256 Hz arba iš 1.953, kai Fd = 512 Hz
					rrIndexOld = rrIndexNew;    //Nauja R dantelio vieta (indeksą) išsaugom kaip buvusį
					rrSkipping = 0;             //Pradedam iš naujo skaičiuoti po rasto R dantelio praleidžiamu atskaitų skaičių
				}
			}
		}
		rrSkipping = rrSkipping + 1;            //Nejautros intervalo po rasto R dantelio skaitiklis
		rrInd = rrInd + 1;                      //Algoritmo skaitiklis
	}
	if(rrInd > (rrIndexNew+1200))   // tikrinam ar per 4*Fs sek pavyko atrasti RR intervala
	{
		rrIndexNew = rrInd;
		rrIndexOld = rrInd;
		//rrThreshold = rrThreshold >> 1;
		restart = 1;
		if (rrThreshold < 110){ rrThreshold = 110;};
		rrMax = 220;
		rrMaxOld = 220;
	}

	if(RWaveFound == 1){
		RWaveFound = 0;                         //Išvalom kintamąjį
		return ecg_heartrate;                             //Jei R dantelis rastas, tai gražinam RR intervalą
	}else{
		return 0;                               //Jei R dantelis nerastas, tai gražinam 0
	}
}


signed long rrFilter(signed long EkgSample, unsigned char SamplingRate) //512Hz: SamplingRate = 1, 256Hz: SamplingRate == 0, EKGSample - EKG įėjimo atskaita
{
	signed static long long InLP;               //Žemųjų dažnių filtro įėjimo atskaita
	signed static long long In1LP;              //Žemųjų dažnių filtro įėjimas suvėlintas 1 atskaita
	signed static long long In2LP;              //Žemųjų dažnių filtro įėjimas suvėlintas 2 atskaitomis
	signed static long long Out1LP;             //Žemųjų dažnių filtro išėjimas suvėlintas 1 ataskaita
	signed static long long Out2LP;             //Žemųjų dažnių filtro išėjimas suvėlintas 2 atskaitomis
	signed long long OutLP = 0;                          //Žemųjų dažnių filtro išėjimo atskaita
	signed static long long In1HP;              //Aukštųjų dažnių filtro įėjimas suvėlintas 1 atskaita
	signed static long long In2HP;              //Aukštųjų dažnių filtro įėjimas suvėlintas 2 atskaitomis
	signed static long long Out1HP;             //Aukštųjų dažnių filtro išėjimas suvėlintas 1 ataskaita
	signed static long long Out2HP;             //Aukštųjų dažnių filtro išėjimas suvėlintas 2 ataskaitomis
	signed long long OutHP = 0;                          //Aukštųjų dažnių filtro išėjimo atskaita

	InLP = EkgSample;                           //EKG atskaita - įėjimas žemųjų dažnių filtrą

	//switch(SamplingRate){                       //Pagal diskretizavimo dažnį parenkame atitinkamus filtro koeficientus žemųjų dažnių filtrui
	//case 0:
	//	//256Hz Žemųjų dažnių NIR filtras
	//	OutLP = (1865*(signed long)InLP + 2420*(signed long)In1LP + 1865*(signed long)In2LP + 17259*(signed long)Out1LP - 7776*(signed long)Out2LP) >> 14;
	//	break;
	//case 1:
	//512Hz Žemųjų dažnių NIR filtras
	OutLP = (875*(signed long long)InLP + 79*(signed long long)In1LP + 875*(signed long long)In2LP + 25411*(signed long long)Out1LP - 11080*(signed long long)Out2LP) >> 14;
	//	break;
	//}

	In2LP = In1LP;                              //Išsaugome suvėlintas atskaitas, kurios bus naudingos tolimesnėms atskaitoms filtruoti
	In1LP = InLP;
	Out2LP = Out1LP;
	Out1LP = OutLP;

	//switch(SamplingRate){                       //Pagal diskretizavimo dažnį parenkame atitinkamus filtro koeficientus aukštųjų dažnių filtrui
	//case 0:
	//	//256Hz Aukštųjų dažnių NIR filtras
	//	OutHP = (12526*(signed long)OutLP - 25042*(signed long)In1HP + 12526*(signed long)In2HP + 27545*(signed long)Out1HP - 12278*(signed long)Out2HP) >> 14;
	//	break;
	//case 1:
	//512Hz Aukštųjų dažnių NIR filtras
	OutHP = (13552*(signed long long)OutLP - 27101*(signed long long)In1HP + 13552*(signed long long)In2HP + 30260 *(signed long long)Out1HP - 14174*(signed long long)Out2HP) >> 14;
	//	break;
	//}

	In2HP = In1HP;                              //Išsaugome suvėlintas atskaitas, kurios bus naudingos tolimesnėms atskaitoms filtruoti
	In1HP = OutLP;
	Out2HP = Out1HP;
	Out1HP = OutHP;

	OutHP = OutHP*(-1);                         //Invertuojam išėjimą (R atpažinimo algoritmo specifika)

	return OutHP;                               //Nufiltruota atskaita
}
