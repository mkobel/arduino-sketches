//***************************************************************
// 8-RGB-LED via 3 74HC595 Shift Register                       *
//***************************************************************
// Author: Moritz Kobel, moritz@kobelnet.ch
// Date  : Okt 2012

// Notes : currently work in progress... my first arduino project


// code based on:
//**************************************************************//
//  Name    : shiftOutCode, Predefined Dual Array Style         //
//  Author  : Carlyn Maw, Tom Igoe                              //
//  Date    : 25 Oct, 2006                                      //
//  Version : 1.0                                               //
//  Notes   : Code for using a 74HC595 Shift Register           //
//          : to count from 0 to 255                            //
//****************************************************************
#define NUM_REGISTERS 3
#define NUM_LED 8


//Pin connected to ST_CP of all the 74HC595
int latchPin = 8;
//Pin connected to SH_CP of all the 74HC595
int clockPin = 12;
////Pin connected to DS of the first (red) 74HC595
int dataPin = 11; // 11



//holders for infromation you're going to pass to shifting function
byte dataRED;
byte dataGREEN;
byte dataBLUE;

void setup() {
  //set pins to output because they are addressed in the main loop
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  Serial.begin(9600);

  
}

void loop() {

   int r[NUM_LED];
   int g[NUM_LED];
   int b[NUM_LED];
  
 
   for (int j = 0; j < 100; j++) {
       for ( int l = 0 ; l < NUM_LED; l++ )
       {
           r[l] = l;
           g[l] = NUM_LED - l;
           b[l] = 0; //(l+l) % numLED;
       }
       showRGB(r,g,b);
       delay(0);
   }
  
}

/**
 * Software-based PWM
 * provide a value between 0 and 255 for each color channel
 */
void showRGB(int r[], int g[], int b[])
{
  int maxVAL = 8;
  int ar[NUM_LED];
  int ag[NUM_LED];
  int ab[NUM_LED];
  byte br;
  byte bg;
  byte bb;
  for ( int i = 0 ; i < NUM_LED ; i++ )
  {
    ar[i] = map(r[i], 0, NUM_LED, 0, maxVAL); 
    ag[i] = map(g[i], 0, NUM_LED, 0, maxVAL); 
    ab[i] = map(b[i], 0, NUM_LED, 0, maxVAL);  
    Serial.print("Show r/g/b: ");
    Serial.print(ar[i]);
    Serial.print("/");
    Serial.print(ag[i]);
    Serial.print("/");
    Serial.print(ab[i]);    
    Serial.println(";");
  }
  int add = 1;
  for ( int c = 0 ; c < maxVAL ; c++ )
  {
    br = 0;
    bg = 0;
    bb = 0;

    for ( int i = 0 ; i < NUM_LED ; i++ )
    {
        if ( ar[i] > c )
          br |= add;
        if ( ag[i] > c )
          bg |= add;
        if ( ab[i] > c )
          bb |= add;
    } 
    Serial.print("Show mixed r/g/b ");
    showMixedRGB(br,bg,bb);
    Serial.print(br,HEX);
    Serial.print("/");
    Serial.print(bg,HEX);
    Serial.print("/");
    Serial.print(bb,HEX); 
    Serial.println(";");
    //delay(1);
    add = add<<1;
  }
}

/**
 * If all the channels share the same resistor, red has the biggest current
 * an therefore only red will be illuminated
 * Therefore only one channel is powered a time
 */
void showMixedRGB(byte r, byte g, byte b)
{
  
  byte data[NUM_REGISTERS];
  
  // R=0,G=1,B=2
  data[0] = r;
  data[1] = 0x00;
  data[2] = 0x00;       
  delay(1);
  sendDataToRegisters(dataPin, clockPin, latchPin, data);       
  data[0] = 0x00;
  data[1] = g;
  data[2] = 0x00; 
  delay(1);  
  sendDataToRegisters(dataPin, clockPin, latchPin, data);       
  data[0] = 0x00;
  data[1] = 0x00;
  data[2] = b;       
  sendDataToRegisters(dataPin, clockPin, latchPin, data);       
  delay(1);
  
}


void sendDataToRegisters(int myDataPin, int myClockPin, int myLatchPin, byte data[]) {
  
  digitalWrite(myLatchPin, LOW); 
  for ( int i = NUM_REGISTERS-1 ; i >= 0  ; i-- )
  {
     shiftOut(myDataPin,myClockPin,MSBFIRST,data[i]);
  }
  digitalWrite(myLatchPin, HIGH);
  
}

