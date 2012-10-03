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

//Pin connected to ST_CP of 74HC595
int latchPin = 8;
//Pin connected to SH_CP of 74HC595
int clockPin = 12;
////Pin connected to DS of 74HC595
int dataPin = 11; // 11


// Amount of LED
int numLED = 8;

//holders for infromation you're going to pass to shifting function
byte dataRED;
byte dataGREEN;
byte dataBLUE;
byte dataArrayRED[10];
byte dataArrayGREEN[10];
byte dataArrayBLUE[10];

void setup() {
  //set pins to output because they are addressed in the main loop
  pinMode(latchPin, OUTPUT);
  Serial.begin(9600);

  //Arduino doesn't seem to have a way to write binary straight into the code 
  //so these values are in HEX.  Decimal would have been fine, too. 
  dataArrayRED[0] = 0xFF; //11111111
  dataArrayRED[1] = 0xFE; //11111110
  dataArrayRED[2] = 0xFC; //11111100
  dataArrayRED[3] = 0xF8; //11111000
  dataArrayRED[4] = 0xF0; //11110000
  dataArrayRED[5] = 0xE0; //11100000
  dataArrayRED[6] = 0xC0; //11000000
  dataArrayRED[7] = 0x80; //10000000
  dataArrayRED[8] = 0x00; //00000000
  dataArrayRED[9] = 0xE0; //11100000

  //Arduino doesn't seem to have a way to write binary straight into the code 
  //so these values are in HEX.  Decimal would have been fine, too. 
  dataArrayGREEN[0] = 0xFF; //11111111
  dataArrayGREEN[1] = 0x7F; //01111111
  dataArrayGREEN[2] = 0x3F; //00111111
  dataArrayGREEN[3] = 0x1F; //00011111
  dataArrayGREEN[4] = 0x0F; //00001111
  dataArrayGREEN[5] = 0x07; //00000111
  dataArrayGREEN[6] = 0x03; //00000011
  dataArrayGREEN[7] = 0x01; //00000001
  dataArrayGREEN[8] = 0x00; //00000000
  dataArrayGREEN[9] = 0x07; //00000111
  
 
  dataArrayBLUE[0] = 0x07; //00000111
  dataArrayBLUE[1] = 0x03; //00000011
  dataArrayBLUE[2] = 0x01; //00000001
  dataArrayBLUE[3] = 0x00; //00000000
  dataArrayBLUE[4] = 0x07; //00000111
  dataArrayBLUE[5] = 0xFF; //11111111
  dataArrayBLUE[6] = 0x7F; //01111111
  dataArrayBLUE[7] = 0x3F; //00111111
  dataArrayBLUE[8] = 0x1F; //00011111
  dataArrayBLUE[9] = 0x0F; //00001111

  //function that blinks all the LEDs
  //gets passed the number of blinks and the pause time
  //blinkAll_2Bytes(2,500); 
}

void loop() {


  /*for (int j = 0; j < 10; j++) {
    //load the light sequence you want from array
    dataRED = dataArrayRED[j];
    dataGREEN = dataArrayGREEN[j];
    dataBLUE = dataArrayBLUE[j];
    //ground latchPin and hold low for as long as you are transmitting
    digitalWrite(latchPin, LOW);
    //move 'em out
    shiftOut(dataPin, clockPin, dataBLUE);   
    shiftOut(dataPin, clockPin, dataGREEN);   
    shiftOut(dataPin, clockPin, dataRED);
    //return the latch pin high to signal chip that it 
    //no longer needs to listen for information
    digitalWrite(latchPin, HIGH);
    delay(10);
  }*/
  
   int r[numLED];
   int g[numLED];
   int b[numLED];
  
   
   
  
   for (int j = 0; j < 100; j++) {
       for ( int l = 0 ; l < numLED; l++ )
       {
           r[l] = l;
           g[l] = 0; //numLED-l;
           b[l] = 0; //(l+l) % numLED;
       }
       showRGB(r,g,b);
       delay(0);
   }
  
}

void showRGB(int r[], int g[], int b[])
{
  for (int i=0; i<numLED; i++)
  {
    byte ro = 0;
    byte go = 0;
    byte bo = 0;
    
    
    Serial.print("r=");
    for ( int j=0; j<numLED ; j++ )
    {        
        if ( r[j] < i ) {
          ro |= (0x01<<j);
          Serial.print("1");
        } else {
          Serial.print("0");
        }
       
       
    }
    Serial.print("g=");
    for ( int j=0; j<numLED ; j++ )
    {        
        if ( g[j] < i ) {
          go |= (0x01<<j);
          Serial.print("1");
        } else {
          Serial.print("0");
        }
    }
    Serial.print("b=");
    for ( int j=0; j<numLED ; j++ )
    {        
       
        if ( b[j] < i ) {
          bo |= (0x01<<j);
          Serial.print("1");
        } else {
          Serial.print("0");
        }
    }
    Serial.println(";");
   
    digitalWrite(latchPin, LOW);    
    Serial.print("write out:");
    //shiftOutDist(dataPin, clockPin, bo);
    //shiftOutDist(dataPin, clockPin, go);
    shiftOutDist(dataPin, clockPin, ro);
    Serial.println("done");
    digitalWrite(latchPin, HIGH);
    delay(500);
  }
}

// the heart of the program
void shiftOutDist(int myDataPin, int myClockPin, byte myDataOut) {
  // This shifts 8 bits out MSB first, 
  //on the rising edge of the clock,
  //clock idles low

  //internal function setup
  int i=0;
  int pinState;
  pinMode(myClockPin, OUTPUT);
  pinMode(myDataPin, OUTPUT);

  //clear everything out just in case to
  //prepare shift register for bit shifting
  digitalWrite(myDataPin, 0);
  digitalWrite(myClockPin, 0);

  //for each bit in the byte myDataOut�
  //NOTICE THAT WE ARE COUNTING DOWN in our for loop
  //This means that %00000001 or "1" will go through such
  //that it will be pin Q0 that lights. 
  for (i=7; i>=0; i--)  {
    digitalWrite(myClockPin, 0);

    //if the value passed to myDataOut and a bitmask result 
    // true then... so if we are at i=6 and our value is
    // %11010100 it would the code compares it to %01000000 
    // and proceeds to set pinState to 1.
    if ( myDataOut & (1<<i) ) {
      pinState= 1;
    }
    else {	
      pinState= 0;
    }

    //Sets the pin to HIGH or LOW depending on pinState
    digitalWrite(myDataPin, pinState);
    Serial.print(pinState);
    //register shifts bits on upstroke of clock pin  
    digitalWrite(myClockPin, 1);
    //zero the data pin after shift to prevent bleed through
    digitalWrite(myDataPin, 0);
  }

  //stop shifting
  digitalWrite(myClockPin, 0);
}

/*
// the heart of the program
void shiftOut( byte dataR, byte dataG, byte dataB) {
  // This shifts 8 bits out MSB first, 
  //on the rising edge of the clock,
  //clock idles low

  //internal function setup
  int i=0;
  int pinStateR;
  int pinStateG;
  int pinStateB;  
  pinMode(clockPin, OUTPUT);
  pinMode(dataPinR, OUTPUT);
  pinMode(dataPinG, OUTPUT);
  pinMode(dataPinB, OUTPUT);  

  //clear everything out just in case to
  //prepare shift register for bit shifting
  
  digitalWrite(dataPinR, LOW);
  digitalWrite(dataPinG, LOW);
  digitalWrite(dataPinB, LOW);  
  digitalWrite(clockPin, LOW);

  //for each bit in the byte myDataOut�
  //NOTICE THAT WE ARE COUNTING DOWN in our for loop
  //This means that %00000001 or "1" will go through such
  //that it will be pin Q0 that lights. 
  for (i=7; i>=0; i--)  {
    digitalWrite(clockPin, LOW);

    //if the value passed to myDataOut and a bitmask result 
    // true then... so if we are at i=6 and our value is
    // %11010100 it would the code compares it to %01000000 
    // and proceeds to set pinState to 1.
    if ( dataR & (1<<i) ) {
      pinStateR= 1;
    }
    else {	
      pinStateR= 0;
    }
    if ( dataG & (1<<i) ) {
      pinStateG= 1;
    }
    else {	
      pinStateG= 0;
    }
    if ( dataB & (1<<i) ) {
      pinStateB= 1;
    }
    else {	
      pinStateB= 0;
    }

    //Sets the pin to HIGH or LOW depending on pinState
    digitalWrite(dataPinR, pinStateR);
    digitalWrite(dataPinG, pinStateG);
    digitalWrite(dataPinB, pinStateB);    
    //register shifts bits on upstroke of clock pin  
    digitalWrite(clockPin, HIGH);
    //zero the data pin after shift to prevent bleed through
    digitalWrite(dataPinR, LOW);
    digitalWrite(dataPinG, LOW);
    digitalWrite(dataPinB, LOW);
  }

  //stop shifting
  digitalWrite(clockPin, LOW);
}*/


//blinks the whole register based on the number of times you want to 
//blink "n" and the pause between them "d"
//starts with a moment of darkness to make sure the first blink
//has its full visual effect.
/*void blinkAll_2Bytes(int n, int d) {
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, 0);
  shiftOut(dataPin, clockPin, 0);
  shiftOut(dataPin, clockPin, 0);
  digitalWrite(latchPin, HIGH);
  delay(200);
  for (int x = 0; x < n; x++) {
    digitalWrite(latchPin, LOW);
    shiftOut(dataPin, clockPin, 255);
    shiftOut(dataPin, clockPin, 0);
    shiftOut(dataPin, clockPin, 0);
    digitalWrite(latchPin, HIGH);
    delay(d);
    digitalWrite(latchPin, LOW);
    shiftOut(dataPin, clockPin, 0);
    shiftOut(dataPin, clockPin, 255);
    shiftOut(dataPin, clockPin, 0);    
    digitalWrite(latchPin, HIGH);
    delay(d);
    digitalWrite(latchPin, LOW);
    shiftOut(dataPin, clockPin, 0);
    shiftOut(dataPin, clockPin, 0);
    shiftOut(dataPin, clockPin, 255);    
    digitalWrite(latchPin, HIGH);
    delay(d);
  }
}*/

