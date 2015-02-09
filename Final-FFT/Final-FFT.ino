#include <Time.h>
#include <avr/pgmspace.h>
#include <Arduino.h>
#include <math.h>

//assuming 16 bit signed ints
const int MAXNDATA = 256;
const int INT_UNITY = 128;
const int UNITY_EXP = 7;

int val[MAXNDATA];
int valI[MAXNDATA];
unsigned long t1;
unsigned long t2;
unsigned long time;
double interval;
int cutoff[4]={0,5,25,95};

prog_int16_t SINE8BIT[] PROGMEM= {
0, 3, 6, 9, 12, 15, 18, 21, 
24, 28, 31, 34, 37, 40, 43, 46, 
48, 51, 54, 57, 60, 63, 65, 68, 
71, 73, 76, 78, 81, 83, 85, 88, 
90, 92, 94, 96, 98, 100, 102, 104, 
106, 108, 109, 111, 112, 114, 115, 117, 
118, 119, 120, 121, 122, 123, 124, 124, 
125, 126, 126, 127, 127, 127, 127, 127, 

127, 127, 127, 127, 127, 127, 126, 126, 
125, 124, 124, 123, 122, 121, 120, 119, 
118, 117, 115, 114, 112, 111, 109, 108, 
106, 104, 102, 100, 98, 96, 94, 92, 
90, 88, 85, 83, 81, 78, 76, 73, 
71, 68, 65, 63, 60, 57, 54, 51, 
48, 46, 43, 40, 37, 34, 31, 28, 
24, 21, 18, 15, 12, 9, 6, 3, 

0, -3, -6, -9, -12, -15, -18, -21, 
-24, -28, -31, -34, -37, -40, -43, -46, 
-48, -51, -54, -57, -60, -63, -65, -68, 
-71, -73, -76, -78, -81, -83, -85, -88, 
-90, -92, -94, -96, -98, -100, -102, -104, 
-106, -108, -109, -111, -112, -114, -115, -117, 
-118, -119, -120, -121, -122, -123, -124, -124, 
-125, -126, -126, -127, -127, -127, -127, -127
}; //arduino

prog_int16_t BLACKMAN[] PROGMEM = {
  1, 1, 1, 1, 1, 1, 1, 1,
  1, 2, 2, 2, 2, 2, 3, 3,
  3, 3, 4, 4, 4, 5, 5, 5,
  6, 6, 7, 7, 8, 8, 9, 10,
  10, 11, 12, 12, 13, 14, 15, 15,
  16, 17, 18, 19, 20, 21, 22, 23,
  24, 25, 27, 28, 29, 30, 32, 33,
  34, 36, 37, 38, 40, 41, 43, 44,
  46, 48, 49, 51, 52, 54, 56, 57,
  59, 61, 63, 64, 66, 68, 70, 71,
  73, 75, 77, 78, 80, 82, 84, 85,
  87, 89, 91, 92, 94, 96, 97, 99,
  100, 102, 103, 105, 106, 108, 109, 110,
  112, 113, 114, 115, 116, 117, 118, 119,
  120, 121, 122, 123, 123, 124, 125, 125,
  126, 126, 126, 127, 127, 127, 127, 127,
  127, 127, 127, 127, 126, 126, 126, 125,
  125, 124, 123, 123, 122, 121, 120, 119,
  118, 117, 116, 115, 114, 113, 112, 110,
  109, 108, 106, 105, 103, 102, 100, 99,
  97, 96, 94, 92, 91, 89, 87, 85,
  84, 82, 80, 78, 77, 75, 73, 71,
  70, 68, 66, 64, 63, 61, 59, 57,
  56, 54, 52, 51, 49, 48, 46, 44,
  43, 41, 40, 38, 37, 36, 34, 33,
  32, 30, 29, 28, 27, 25, 24, 23,
  22, 21, 20, 19, 18, 17, 16, 15,
  15, 14, 13, 12, 12, 11, 10, 10,
  9, 8, 8, 7, 7, 6, 6, 5,
  5, 5, 4, 4, 4, 3, 3, 3,
  3, 2, 2, 2, 2, 2, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1
};

int doFFT(int dataR[], int dataI[], int num);
int MInt8(int a, int b);
int fillAllNotes(int data[]);


void setup()
{
  pinMode(A0,INPUT);
  pinMode(9,OUTPUT);
  pinMode(10,OUTPUT);
  pinMode(11,OUTPUT);
  Serial.begin(9600);
  Serial.println("hi");
  /*
  for (int i=0; i<MAXNDATA; i++)
  {
    valI[i] = 0;
  }
  
  fillAllNotes(val);
  
  interval = 0.1;
  int nData = 256;
  t1 = millis();
  doFFT(val,valI,nData);
  t2 = millis();
  //Serial.println(t2-t1);
  
  double weightedTotal[3]={0,0,0};
  for (int i=0; i<nData; i++)
  {
    double real = ((double) val[i])/INT_UNITY;
    double imag = ((double) valI[i])/INT_UNITY;
    double mag = pow(pow(real,2)+pow(imag,2),0.5);
    double freq = ((double) i)/interval*pow(10,3)/nData;
    for (int k=0; k<3; k++)
    {
      if ((i>=cutoff[k]) && (i<cutoff[k+1]))
      {
        weightedTotal[k]+=mag;//*TOTALSCALE/i;
        break;
      }
    }
    /*
    Serial.print("   ");
    Serial.print(freq,5);
    Serial.print("\t");
    Serial.print(mag,5);
    Serial.print("\t");
    Serial.print("\n");
    
  }
  analogWrite(9,(int) (weightedTotal[0]*32));
  analogWrite(10,(int) (weightedTotal[1]*32));
  analogWrite(11,(int) (weightedTotal[2]*32));
  
  Serial.print("          ");
  Serial.print(weightedTotal[0]);
  Serial.print(",");
  Serial.print(weightedTotal[1]);
  Serial.print(",");
  Serial.print(weightedTotal[2]);
  */
}

void loop()
{
  for (int i=0; i<256; i++)
  {
    val[i] = (analogRead(A0)-512)>>2;
    valI[i] = 0;
  }
  
  for (int i=0; i<256; i++)
  {
    val[i] = MInt8(val[i], pgm_read_word_near(BLACKMAN + i));
  }
  //finding range then midpoint
  
  doFFT(val,valI,256);
  
  double weightedTotal[3]={0,0,0};
  
  for (int i=0; i<256; i++)
  {
    double real = ((double) val[i])/INT_UNITY;
    double imag = ((double) valI[i])/INT_UNITY;
    double mag = pow(pow(real,2)+pow(imag,2),0.5);
    for (int k=0; k<3; k++)
    {
      if ((i>=cutoff[k]) && (i<cutoff[k+1]))
      {
        weightedTotal[k]+=mag;//*TOTALSCALE/i;
        break;
      }
    }
  }
  
  //analogWrite(9,(int) (weightedTotal[0]*64));
  //analogWrite(10,(int) (weightedTotal[1]*64));
  //analogWrite(11,(int) (weightedTotal[2]*64));
  
  analogWrite(9,(int) 1.66557*exp(0.3837*1.5*weightedTotal[0]*10)/2);
  analogWrite(10,(int) 1.66557*exp(0.3837*2.5*weightedTotal[1]*10)/2);
  analogWrite(11,(int) 1.66557*exp(0.3837*1.5*weightedTotal[2]*10)/2);
  
  //take magnitudes in an array 128
  //frequency = sampling rate * i / 128
  // sampling rate = 1/time
  
}

int fillAllNotes(int data[])
{
	for (int i = 0; i<MAXNDATA; i++)
	{
		double val1 = 0;
		int temp = 88;
		for (int j = 1; j<=88; j++)
		{
			val1 += sin(pow(1.05946309435929526456,j-49)*440*2*M_PI*i/10000);
		}
		val1 *= INT_UNITY/30;
		data[i]=(int) val1;
	}
	return MAXNDATA;
}

int MInt8(int a, int b)
{
  long c = (((long) a) * ((long) b));
  c >>= (UNITY_EXP-1);
  long g=c&1;
  return (int) ((c>>1) + g);
}

int doFFT(int dataR[], int dataI[], int num)
{
  if (num<1)
    return -1;
  int nDataTest=num;
  int expsize=0;
  while(nDataTest!=1)
  {
    if ((nDataTest&1)==1)
    {
      //cout<<"Bad data quantity";
      return -1;
    }
    nDataTest>>=1;
    expsize++;
  }

  if (num==1)
    return 0;
  for (int i=0; i<num; i++)
  {
    int g=4;
    int i0=i;
    while (g<=num)
    {
      int iS=i0%g;
      int iS2=(iS*2)%(g-1);
      if (iS2)
        i0+=(iS2-iS);
      g <<= 1;
    }
    if (i<i0)
    {
      int temp=dataR[i];
      dataR[i]=dataR[i0];
      dataR[i0]=temp;
      temp=dataI[i];
      dataI[i]=dataI[i0];
      dataI[i0]=temp;
    }
  }
  
  //cerr<<endl;
  int g=2;
  int gExp=1;
  while (g<=num)
  {
    for (int i=0; i<num; i+=g)
    {
      //cout<<i<<"---"<<endl;
      for (int j=0; j<((g>>1)); j++)
      {
        int evenR=dataR[i+j];
        int evenI=dataI[i+j];
        int oddR=dataR[i+j+((g>>1))];
        int oddI=dataI[i+j+((g>>1))];

        //cout<<"|"<<j<<"|"<<i+j<<"|"<<i+j+g/2<<"|"<<evenR<<","<<evenI<<","<<oddR<<","<<oddI<<","<<endl;

        int factor = (MAXNDATA*j)>>gExp;
        //cout<<((double) factor)/INT_UNITY<<endl;
        //int sine = -SINE8BIT[factor];
        int sine = -pgm_read_word_near(SINE8BIT + factor);
        //int cosine = SINE8BIT[( factor + (MAXNDATA>>2) ) % MAXNDATA];
        int cosine = pgm_read_word_near(SINE8BIT + (( factor + (MAXNDATA>>2) ) % MAXNDATA));
        int XoddR = MInt8(oddR,cosine)-MInt8(oddI,sine);
        int XoddI = MInt8(oddR,sine)+MInt8(oddI,cosine);
        //cout<<">"<<cosine<<">"<<sine<<">"<<XoddR<<">"<<XoddI<<endl;
        //int XoddR=Re(cos(-factor)+isin(-factor))(oddR+ioddI)=oddR*cos(-2pik/n)-sin(-2pik/n)*oddI;
        //int XoddI=Im(cos(-factor)+isin(-factor))(oddR+ioddI)=sin(-2pik/n)*oddR+cos(-2pik/n)*oddI
        //exp(-2piik/n)=cos(-2pik/n)+isin(-2pik/n)
        dataR[i+j]= (evenR+XoddR)>>1;
        dataI[i+j]= (evenI+XoddI)>>1;
        //cout<<"<"<<evenR+XoddR<<endl;
        dataR[i+j+(g>>1)]= (evenR-XoddR)>>1;
        dataI[i+j+(g>>1)]= (evenI-XoddI)>>1;
        //cout<<"|"<<dataR[i+j]<<"|"<<dataI[i+j]<<"|"<<dataR[i+j+g/2]<<"|"<<dataI[i+j+g/2]<<endl;
      }
    }
    //cout<<"||---------------------"<<endl;
    g<<=1;
    gExp++;
  }
  //for (int i = 0; i<num; i++)
  //	cout<<dataR[i]<<","<<dataI[i]<<endl;
  return 0;
}

