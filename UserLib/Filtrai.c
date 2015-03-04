/*
 * Filtrai.c
 *
 *  Created on: 2012.09.18
 *      Author: Admin
 */

#include "Filtrai.h"


float PpgDcpass(float *outOld, float inSig) //signed short PpgDcRemoval(float *outOld, signed short inSig)
{
  //float InNew = inSig;
  //float Out;
  *outOld = ((0.9937*(*outOld)) + (0.0063*inSig)); // 0.5Hz Filtravimo funkcija
  //Out = inSig - *outOld;
  return *outOld;
}

#define b0  ((float)(0.0003460413427))
#define b1  ((float)(0.0006920826854))
#define b2  ((float)(0.0003460413427))
#define a1  ((float)(-1.946697593))
#define a2  ((float)(0.9480817318))

float LowPass3Hz(float in,float *m1){ // 3Hz
  register float w = in - (a1*(m1[0])) - (a2*(m1[1]));
  register float out = (b1*(m1[0])) + (b2*(m1[1])) + (b0*w);
  m1[1] = m1[0];
  m1[0] = w;
  return out;
}

// //%b=[0.003621681593, 0.007243363187, 0.003621681593];
// //%a=[ 1, -1.822694898,   0.8371816278];

#define b00  ((float)(0.00362168159))
#define b10  ((float)(0.007243363187))
#define b20  ((float)(0.003621681593))
#define a10  ((float)(-1.822694898))
#define a20  ((float)(0.8371816278))

float LowPass10Hz(float in,float *m1){ // 10Hz
  register float w = in - (a10*(m1[0])) - (a20*(m1[1]));
  register float out = (b10*(m1[0])) + (b20*(m1[1])) + (b00*w);
  m1[1] = m1[0];
  m1[0] = w;
  return out;
}

float AveragerSpo2(float in,float  *Array){
  register float out = (Array[0]+Array[1]+Array[2]+in)/4;
  Array[2] = Array[1];
  Array[1] = Array[0];
  Array[0] = in;
  return out;
}

void calcstat (signed int *a, signed int *avg, float *sd){
  float Calc=0.0;
  signed int sum;

  sum = a[0]+a[1]+a[2]+a[3]+a[4];
  *avg = sum / 5.0;

  Calc += (float)( a[0] - *avg) * (float)( a[0] - *avg);
  Calc += (float)( a[1] - *avg) * (float)( a[1] - *avg);
  Calc += (float)( a[2] - *avg) * (float)( a[2] - *avg);
  Calc += (float)( a[3] - *avg) * (float)( a[3] - *avg);
  Calc += (float)( a[4] - *avg) * (float)( a[4] - *avg);
  *sd = sqrt((double)Calc/5.0);
  //*sd = sqrtf(Calc/5.0);
}

signed int middle_of_3(signed int a, signed int b, signed int c){
  signed int middle;

 if ((a <= b) && (a <= c))
 {
   middle = (b <= c) ? b : c;
 }
 else if ((b <= a) && (b <= c))
 {
   middle = (a <= c) ? a : c;
 }
 else
 {
   middle = (a <= b) ? a : b;
 }
 return middle;
}
//#define MEDIAN_FILTER_SIZE 5 /* Size of the data buffer; length of the sequence. */
//#define STOPPER 0 /* Smaller than any datum */
//
//int median_filter(int datum){
//  struct pair
//  {
//    struct pair   *point;                            /* Pointers forming list linked in sorted order */
//    int  value;                                      /* Values to sort */
//  };
//  static struct pair mbuffer[MEDIAN_FILTER_SIZE] = {}; /* Buffer of nwidth pairs */
//  static struct pair *datpoint = mbuffer;              /* Pointer into circular buffer of data */
//  static struct pair small = {NULL, STOPPER};          /* Chain stopper */
//  static struct pair big = {&small, 0};                /* Pointer to head (largest) of linked list.*/
//
//  struct pair *successor;                              /* Pointer to successor of replaced data item */
//  struct pair *scan;                                   /* Pointer used to scan down the sorted list */
//  struct pair *scanold;                                /* Previous value of scan */
//  struct pair *median;                                 /* Pointer to median */
//  uint16_t i;
//
//  if (datum == STOPPER)
//  {
//    datum = STOPPER + 1;                             /* No stoppers allowed. */
//  }
//
//  if ( (++datpoint - mbuffer) >= MEDIAN_FILTER_SIZE)
//  {
//    datpoint = mbuffer;                               /* Increment and wrap data in pointer.*/
//  }
//
//  datpoint->value = datum;                           /* Copy in new datum */
//  successor = datpoint->point;                       /* Save pointer to old value's successor */
//  median = &big;                                     /* Median initially to first in chain */
//  scanold = NULL;                                    /* Scanold initially null. */
//  scan = &big;                                       /* Points to pointer to first (largest) datum in chain */
//
//  /* Handle chain-out of first item in chain as special case */
//  if (scan->point == datpoint)
//  {
//    scan->point = successor;
//  }
//  scanold = scan;                                     /* Save this pointer and   */
//  scan = scan->point ;                                /* step down chain */
//
//  /* Loop through the chain, normal loop exit via break. */
//  for (i = 0 ; i < MEDIAN_FILTER_SIZE; ++i)
//  {
//    /* Handle odd-numbered item in chain  */
//    if (scan->point == datpoint)
//    {
//      scan->point = successor;                      /* Chain out the old datum.*/
//    }
//
//    if (scan->value < datum)                        /* If datum is larger than scanned value,*/
//    {
//      datpoint->point = scanold->point;             /* Chain it in here.  */
//      scanold->point = datpoint;                    /* Mark it chained in. */
//      datum = STOPPER;
//    };
//
//    /* Step median pointer down chain after doing odd-numbered element */
//    median = median->point;                       /* Step median pointer.  */
//    if (scan == &small)
//    {
//      break;                                      /* Break at end of chain  */
//    }
//    scanold = scan;                               /* Save this pointer and   */
//    scan = scan->point;                           /* step down chain */
//
//    /* Handle even-numbered item in chain.  */
//    if (scan->point == datpoint)
//    {
//      scan->point = successor;
//    }
//
//    if (scan->value < datum)
//    {
//      datpoint->point = scanold->point;
//      scanold->point = datpoint;
//      datum = STOPPER;
//    }
//
//    if (scan == &small)
//    {
//      break;
//    }
//
//    scanold = scan;
//    scan = scan->point;
//  }
//  return median->value;
//}

//----------------------------------------------------- --FILTRAI RRI ATPAZINIMUI-------------------------------------------------------------------
/* RR variables and defines */
volatile unsigned short  rri_jj;
volatile unsigned char   rri_radau;
volatile unsigned short  RRI;
volatile unsigned char   rri_valid = false;
volatile signed int      ekgdata[3];
// ekgdata[0],ekgdata[1],ekgdata[2]
// previous   current    future    values

volatile unsigned int rri_maxnew = 0;
volatile unsigned int rri_max = 0;

volatile unsigned int rri_counter;
volatile unsigned int rri_slenkstis;
volatile unsigned int rri_slenkstis1;
volatile signed int    rri_nejautra;

volatile unsigned int rri_x1new;
volatile unsigned int rri_x1old;
volatile unsigned int rri_x2new;
volatile static unsigned int rri_x2old;

volatile unsigned int rri_ii;
volatile unsigned int rri_xx;
volatile static unsigned int rri_x1;
volatile static unsigned int rri_x2;

// Highpass FIR filter coefficients for 63 taps to filter < 11Hz
const int coeffsHRVhp[32] =
{27,  28,  30,  33,  36,  39,  40,  38,  33,  22,  4,  -22,  -57,  -102,  -158,  -225,  -302,  -390,  -487,  -592,  -702,  -816,  -931,  -1045,  -1154,  -1255,  -1347,  -1425,  -1489,  -1536,  -1565,  31220};
//{-1,  -4,  -7,  -12,  -19,  -28,  -40,  -55,  -75,  -98,  -126,  -159,  -197,  -239,  -286,  -337,  -391,  -448,  -508,  -568,  -629,  -690,  -749,  -805,  -857,  -905,  -947,  -983,  -1011,  -1032,  -1044,  31720  };

// Lowpass FIR filter coefficients for 17 taps to filter > 30Hz
const int coeffsHRVlp[9] =
{16,  100,  349,  875,  1703,  2729,  3742,  4489,  4764};
//{-685, -537, 399, 3801, 1085, 2116, 3245, 4130, 4466};

volatile unsigned int beat;
/* END of RR section */

//---------------------------------------------------------- HIGH PASS FILTER -----------------------------------------------------------------------
int HRVhpfilter(signed int sampleHRVhp)
{
  static signed int bufHRVhp[128];
  static signed int offsetHRVhp = 0;
  long long z;
  signed int i;

  bufHRVhp[offsetHRVhp] = sampleHRVhp;

  //z = signed_multiplication(coeffsHRVhp[31], bufHRVhp[(offsetHRVhp - 32) & 0x7F]);
  z = (int64_t)coeffsHRVhp[31] * (int64_t)(bufHRVhp[(offsetHRVhp - 32) & 0x7F]);
  for (i = 0;  i < 31;  i++){
    //z += signed_multiplication(coeffsHRVhp[i], bufHRVhp[(offsetHRVhp - i) & 0x7F] + bufHRVhp[(offsetHRVhp - 64 + i) & 0x7F]);
    z +=(int64_t)coeffsHRVhp[i] * ((int64_t)bufHRVhp[(offsetHRVhp - i) & 0x7F] + (int64_t)bufHRVhp[(offsetHRVhp - 64 + i) & 0x7F]);
  }
  offsetHRVhp = (offsetHRVhp + 1) & 0x7F;
  return  z >> 15;
}

//---------------------------------------------------------- LOW PASS FILTER -----------------------------------------------------------------------
int HRVlpfilter(signed int sampleHRVlp)
{
  static signed int bufHRVlp[32];
  static signed int offsetHRVlp = 0;
  long long z;
  signed int i;
	
	static uint16_t MeanCnt = 0;
	static int32_t Mean;
	static int64_t Accu;
	
	if(MeanCnt < 256) //Pati pradzia
	{
		Accu += sampleHRVlp;
		MeanCnt++;
	}else
	{
		Mean = Accu >> 8;
		Accu -= Mean;
		Accu += sampleHRVlp;
		sampleHRVlp -= Mean;
	}

  bufHRVlp[offsetHRVlp] = sampleHRVlp;

  //z = signed_multiplication(coeffsHRVlp[8], bufHRVlp[(offsetHRVlp - 8) & 0x1F]);
  z = (int64_t)coeffsHRVlp[8]*(int64_t)(bufHRVlp[(offsetHRVlp - 8) & 0x1F]);
  for (i = 0;  i < 8;  i++){
    //z += signed_multiplication(coeffsHRVlp[i], bufHRVlp[(offsetHRVlp - i) & 0x1F] + bufHRVlp[(offsetHRVlp - 16 + i) & 0x1F]);
    z += (int64_t)coeffsHRVlp[i]*(int64_t)(bufHRVlp[(offsetHRVlp - i) & 0x1F] + bufHRVlp[(offsetHRVlp - 16 + i) & 0x1F]);
  }
  offsetHRVlp = (offsetHRVlp + 1) & 0x1F;
  return  z >> 15;
}
//---------------------------------------------------------- LOW PASS FILTER -----------------------------------------------------------------------
int filterlp(signed int sample)
{
  //Lowpass FIR filter coefficients for 17 taps to filter > 30Hz
  static const signed int coeffslp[9] = {0, -27, 8, 222, -16, -434, 16, 1304, 2032};

  static signed int buflp[32];                                                         // Reserve 32 loactions for circular buffering
  static signed int offsetlp = 0;
  long long z;
  signed int i;
  buflp[offsetlp] = sample;
  //z = signed_multiplication(coeffslp[8], buflp[(offsetlp - 8) & 0x1F]);
  z = coeffslp[8]*(buflp[(offsetlp - 8) & 0x1F]);
  for (i = 0;  i < 8;  i++){
    //z += signed_multiplication(coeffslp[i], buflp[(offsetlp - i) & 0x1F] + buflp[(offsetlp - 16 + i) & 0x1F]);
    z += coeffslp[i]*(buflp[(offsetlp - i) & 0x1F] + buflp[(offsetlp - 16 + i) & 0x1F]);
  }
  offsetlp = (offsetlp + 1) & 0x1F;
  return  z >> 12;                                                              // Return filter output
}

int AD50HzFilter(int AD50sample)
{
  long Out;

  static int In;
  static int In1;
  static int In2;
  static int Out1;
  static int Out2;

  In = AD50sample;
  Out = (1011*(long)In - 1636*(long)In1 + 1011*(long)In2 + 1636*(long)Out1 - 998*(long)Out2) >> 10;

  In2 = In1;
  In1 = In;
  Out2 = Out1;
  Out1 = Out;

  return Out;                                                                   // Return filter output
}

uint8_t RR_atpazinimas(signed int value,uint16_t *RRI,uint16_t *ecg_heartrate)
{
  unsigned short rrrr;
	
  //  signed int Datain;

  //value = AD50HzFilter(value/32);
  //  Datain = filterlp(Datain);
  //  Datain = value;
  rri_counter++;                                   // rr slenkscio nulinimui
  //  ekgdata[1] = HRVlpfilter(Datain);
  ekgdata[1] = HRVlpfilter(value);//32);//256);
  //DAT.ekg1 = ekgdata[1]+32768;
  ekgdata[1] = HRVhpfilter(ekgdata[1]);            //AD filtras, 20 koeficientu
  //ekgdata[1] = ekgdata[1]/256; // down scaling to 16 bits
  if(ekgdata[1] < 0){
    ekgdata[1] = 0;
  }
  
  //DAT.ekg1 = ekgdata[1]+15000;
  ekgdata[0] = ekgdata[2];                         //pries tai buvusi reiksme
  ekgdata[2] = ekgdata[1];
  rri_nejautra++;
  //  DAT.accx = ekgdata[1];
  //  DAT.accy = rri_slenkstis;
  //  DAT.accz = rri_slenkstis1;
  //------- ieskom pradines didziausios reiksmes (pradinio slenkscio)-----------
  if(rri_jj < 500)
  {
    if(ekgdata[0] < ekgdata[2])
    {
      rri_maxnew = ekgdata[2];
      if(rri_maxnew > rri_max)
      {
        rri_max = rri_maxnew;
        rri_xx = rri_ii;                              // isimenam maksimumo vieta
      }
    }
    rri_jj++;
    rri_slenkstis = rri_max >> 1;
    rri_slenkstis1 = rri_max >> 2;

    rri_ii = rri_xx;
    rri_radau = 1;
    RRI = 0;
  }else
  { //--------------------------radom pradini slenksti-------------------------
    if(rri_nejautra > 100)
    {
      if(rri_radau == 1)
      {
        rri_counter = 0;
        rri_valid = true;
        //beat = 5; // 10ms multiple
        rri_nejautra = 0;
        rri_radau = 0;
      }
      if(ekgdata[2] > rri_slenkstis1)                   // 1/4 max
      {
        if(ekgdata[0] < ekgdata[2])
        {
          rri_maxnew = ekgdata[2];
          rri_xx = rri_ii;
        }else
        {
          rri_x2old = rri_x2new;
          rri_x2new = rri_xx;
          rri_slenkstis1 = rri_maxnew >> 2;                  //slenksti mazinam 4 kartus
        }
        if(ekgdata[2] > rri_slenkstis)                       //ar reikðme didesne uþ 1/2 max?
        {
          if(ekgdata[0] < ekgdata[2])
          {
            rri_maxnew = ekgdata[2];
            rri_xx = rri_ii;
          }else
          {
            rri_x1old = rri_x1new;
            rri_x1new = rri_xx;
            rri_slenkstis  = rri_maxnew >> 1;              //maþinam slenksti dvigubai
            rri_radau = 1;
            rrrr = (rri_x1new - rri_x1old);
            if(rrrr < 100)
            {
              rri_radau = 0;
            }else{
              *RRI  = rrrr;
              *ecg_heartrate = 30000/(rri_x1new -rri_x1old);           //30000/atskaitu tarp RR skaicius
            }
          }
        }
      }
    }
    rri_ii++;
  }
  if(rri_counter > 2000)  // tikrinam ar per 10 sek pavyko atrasti RR intervala
  {
    rri_counter = 0;
    rri_jj = 0;

    rri_maxnew = 0;
    ekgdata[2] = 0;
    ekgdata[0] = 0;
    rri_max = 0;
    rri_xx = 0;
    rri_ii = 0;
    rri_slenkstis = 100;
    rri_slenkstis1 = 95;
    *RRI = 0;
    *ecg_heartrate = 0;
  }
  //if (rri_slenkstis <1000) {  rri_slenkstis = 1000; DAT.ecg_heartrate = 0; };
  //if (rri_slenkstis1 < 950){ rri_slenkstis1 = 950; };
  if (rri_radau != 0)
    return 1;  // jei rasta
  else
    return 0;
}

// Fs=500Hz
//#define b010  ((double)(0.394433639694380e-6F))
//#define b011  ((double)(0.788867279388761e-6F))
//#define b012  ((double)(0.394433639694380e-6F))
//#define a011  ((double)(-1.998222847292))
//#define a012  ((double)(0.9982244250264))
// Fs=50Hz
#define b010  ((double)( 0.391302053991682e-4F))
#define b011  ((double)( 0.782604107983365e-4F))
#define b012  ((double)( 0.391302053991682e-4F))
#define a011  ((double)(-1.982228929792529))
#define a012  ((double)( 0.982385450614126))

double LowPass0_1Hz(double in01,double *m01){ // 0.1Hz
  register double w01 = in01 - (a011*(m01[0])) - (a012*(m01[1]));
  register double out01 = (b011*(m01[0])) + (b012*(m01[1])) + (b010*w01);
  m01[1] = m01[0];
  m01[0] = w01;
  return out01;
}

// Fs=500Hz
//#define b060  ((double)(0.141367939777148e-4F))
//#define b061  ((double)(0.282735879554297e-4F))
//#define b062  ((double)(0.141367939777148e-4F))
//#define a061    ((double)(-1.989337181173716))
//#define a062  ((double)(0.989393728349627))
// Fs=50Hz
#define b060  ((double)( 0.001348711948356))
#define b061  ((double)( 0.002697423896713))
#define b062  ((double)( 0.001348711948356))
#define a061    ((double)(-1.893464146361827))
#define a062  ((double)( 0.898858994155252))

double LowPass0_6Hz(double in,double *yn){ // 0.6Hz
  register double w = in - (a061*(yn[0])) - (a062*(yn[1]));
  register double out = (b061*(yn[0])) + (b062*(yn[1])) + (b060*w);
  yn[1] = yn[0];
  yn[0] = w;
  return out;
}
float Averageof4(int32_t in,int32_t  *Array){
  register int32_t out = (Array[0]+Array[1]+Array[2]+in)/4;
  Array[2] = Array[1];
  Array[1] = Array[0];
  Array[0] = in;
  return out;
}

//#define b060i  ((int64_t)(237))
//#define b061i  ((int64_t)(474))
//#define b062i  ((int64_t)(237))
//#define a060i   ((int64_t)(16777216))
//#define a061i   ((int64_t)(-33375540))
//#define a062i  ((int64_t)(16599272))
//
//int32_t LowPass0_6Hzint(int32_t in,int64_t *yn){ // 0.6Hz
////  register int64_t w = (in*a060i) - (a061i*(yn[0])) - (a062i*(yn[1]));
//  register int64_t w = ((int64_t)in*a060i);
//           w-= (a061i*(yn[0]));
//           w-= (a062i*(yn[1]));
////  register int64_t out = (b061i*(yn[0])) + (b062i*(yn[1])) + (b060i*w);
//  register int64_t out = (b061i*(yn[0]));
//           out+= (b062i*(yn[1]));
//           out+= (b060i*w);
//  yn[1] = yn[0];
//  yn[0] = w;
//  return (int32_t)(out>>48);// 2^(24-1)*2 gal?
//}

#define low_signal_threshold    10

volatile float nmax;
double lp[2];
double in,dc;
double hp[2];
int32_t vidf[4];
int32_t ind,ppind,ind2,decval,valp;
signed char sign, sign_prv,cnt;

uint8_t respiration_rate(int32_t val,uint16_t *PPI, uint16_t *resp_rate)
{
  uint8_t flag = 0;
//  static volatile float nmax;
//  //  static float lp[2];
//  static double lp[2];
//  //static int32_t dc;
//  static double in,dc;
//  //static int64_t hp[2];
//  static double hp[2];
//  static int32_t vidf[4];
//  static int32_t ind,ppind,ind2,decval,valp;
//  static signed char sign, sign_prv,cnt;
  
  if(cnt == 0){
    decval = val;
  }else{
  decval += val;
  }
  decval = decval/2;
  cnt++;
  if (cnt >= 10){ // decimation of 10
    cnt = 0;
    in = (double)decval;
    dc = LowPass0_1Hz(decval, &hp[0]);
    //  DAT.ekg1  = (unsigned short)dc;
    in = in - dc;
    in = LowPass0_6Hz(in, &lp[0]);
    //DAT.ekg2  = (unsigned short)(in+9000);
    //  DAT.ekg1  = (unsigned short)(in+9000);
    val = in;
    if ((val > valp+low_signal_threshold)||(val < valp-low_signal_threshold)){ // low signal threshold
      valp = val;
    }else{
      val = valp;
    }
    // sign detection
    //DAT.ekg1  = (unsigned short)(val+9000);
    if (val > 0){
      sign = 1;
    }else if (val == 0){
      sign = 0;
    }else{
      sign = -1;
    }
    //DAT.ekg2  = sign + 10;
    // zero cross detector
    if ((sign < sign_prv)&&(sign == -1)){
      nmax = 0;
      //dc = Averageof4(ppind, &vidf[0]);
      *resp_rate = (unsigned short)(50*60/ppind); //dc;
      flag = 1;
      ind = ind2;
      ind2 = 0;
      ppind = 0;
    }else if ((sign > sign_prv)||(sign == 1)){
      if (val >= nmax){
        nmax = val;
        ppind = ind;
        ind2 = 0;
      }
    };
    ind++;
    ind2++;
    sign_prv = sign;
    if (ind >= 500){ //10s*50Hz
      *resp_rate = 0;
    }
  }else{
    //DAT.ekg1  = (unsigned short)(valp+9000);
    //DAT.ekg2  = sign + 10;
  }
  return flag;
}
