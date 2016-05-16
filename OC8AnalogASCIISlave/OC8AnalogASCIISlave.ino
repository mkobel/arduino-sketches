

/*
 * OC RelayDuino 8 Analog ASCII Slave
 * For KTA-225
 * by Ocean Controls www.oceancontrols.com.au
 *
 * Controls 
 *  -8 Relays (4 With PWM Out (Should only be used with SSR's, the SSR Switching time is 1ms))
 *  -8 Analog Ins (or Non Isolated Digitals)
 */
#include <string.h> //Use the string Library
#include <ctype.h>

const int stackSize = 10;
const int numInputs = 8; 

//void readdips(void);
void printaddr(char x);
void setbaud(char Mybaud);
bool mostEq(int anreadings[numInputs][stackSize], int input, int value);
bool mostLt(int anreadings[numInputs][stackSize], int input, int value);
bool mostGt(int anreadings[numInputs][stackSize], int input, int value);

#define ANIN1 0   // Analog 1 is connected to Arduino Analog In 0
#define ANIN2 1   // Analog 2 is connected to Arduino Analog In 1
#define ANIN3 2   // Analog 3 is connected to Arduino Analog In 2
#define ANIN4 3   // Analog 4 is connected to Arduino Analog In 3
#define ANIN5 4   // Analog 5 is connected to Arduino Analog In 4
#define ANIN6 5   // Analog 6 is connected to Arduino Analog In 5
#define ANIN7 6   // Analog 7 is connected to Arduino Analog In 6
#define ANIN8 7   // Analog 8 is connected to Arduino Analog In 7
int Analogs[8] = {ANIN1, ANIN2, ANIN3, ANIN4, ANIN5, ANIN6, ANIN7, ANIN8};

#define REL1 2  // Relay 1 is connected to Arduino Digital 2
#define REL2 3  // Relay 2 is connected to Arduino Digital 3 PWM
#define REL3 4  // Relay 3 is connected to Arduino Digital 4
#define REL4 5  // Relay 4 is connected to Arduino Digital 5 PWM
#define REL5 6  // Relay 5 is connected to Arduino Digital 6 PWM
#define REL6 7  // Relay 6 is connected to Arduino Digital 7
#define REL7 8  // Relay 7 is connected to Arduino Digital 8
#define REL8 9  // Relay 8 is connected to Arduino Digital 9 PWM
int Relays[8] = {REL1, REL2, REL3, REL4, REL5, REL6, REL7, REL8};
int PWMAble[4] = {2, 4, 5, 8};
/*
#define OI1 15 // Opto-Isolated Input 1 is connected to Arduino Analog 1 which is Digital 15
#define OI2 16 // Opto-Isolated Input 2 is connected to Arduino Analog 2 which is Digital 16
#define OI3 17 // Opto-Isolated Input 3 is connected to Arduino Analog 3 which is Digital 17
#define OI4 18 // Opto-Isolated Input 4 is connected to Arduino Analog 4 which is Digital 18
int Optos[4] = {OI1, OI2, OI3, OI4};
*/
#define TXEN 10 // RS-485 Transmit Enable is connected to Arduino Digital 10

char i = 0, j = 0, k = 0, Rxchar, Rxenable, Rxptr, Cmdcomplete, R;
char Rxbuf[15];
char adrbuf[3], cmdbuf[3], valbuf[12];
char rxaddress, Unitaddress, Unitbaud;
char Dip[5], Dop[9];
char Hold, Mask, Analog =1, Imax;
int val = 0, Param;     

int anreadings[numInputs][stackSize];
char x;

int modeL = 0;
int modeR = 0;
int powerL = 0;
int powerR = 0;
int valueChanged = 0;

void setup() 
{
  analogReference(EXTERNAL); // use the AREF pin for the reference voltage so we can select between 5v or 3.3v
  for (i=0;i<8;i++)
  {
    pinMode(Relays[i], OUTPUT);  // declare the relay pin as an output
  }  
 /* for (i=0;i<4;i++)
  {
    pinMode(Optos[i], INPUT);  // declare the relay pin as an output
  } */
  pinMode(TXEN, OUTPUT);
  //Read Address, Baud and Parity from EEPROM Here  
  Unitaddress = 0;
  Unitbaud = 4;
//  Unitparity = EEPROM.read(2);//Parity currently not available
  setbaud(Unitbaud);// start serial port
   
  UCSR0A=UCSR0A |(1 << TXC0); //Clear Transmit Complete Flag
  digitalWrite(TXEN, HIGH);   //Enable Transmit
  delay(1);                   //Let 485 chip go into Transmit Mode
  Serial.println("Ocean Controls");
  Serial.println("KTA-225 v1.1");
  while (!(UCSR0A & (1 << TXC0)));    //Wait for Transmit to finish
  digitalWrite(TXEN,LOW);               //Turn off transmit enable

  //We want the switching freq to be pretty slow as the turn on time for the SSR's is 1ms
  //TCCR0B = TCCR0B & 0b11111101; // Arduino pins 5+6 61Hz 
  //TCCR1B = TCCR1B & 0b11111101; // Arduino pins 9+10 30Hz
  //TCCR2B = TCCR2B & 0b11111111; // Arduino pins 11+3 30Hz (stuck at 490Hz have to check this)

  resetAnreadings();
}

void loop() 
{


    for ( j =0; j<numInputs ; j++ )
    {        
      // stack erneuern
      for ( i = 1 ; i<stackSize ; i++ )
      {
        anreadings[j][i] = anreadings[j][i-1];
      }    
      anreadings[j][0] = analogRead(Analogs[j]);

      /*Serial.print(j, DEC);
      Serial.print(" = ");
      Serial.print(anreadings[j-1], DEC);
      Serial.println();*/
    }

  
    valueChanged = 0;

    // Schalter abfragen
    if ( mostEq(anreadings,7,0) )
    {
      if ( powerL == 0 ) {
        powerL = 1;
      } else {
        powerL = 0;
      }
      valueChanged = 1;
    }
    if ( mostEq(anreadings,6,0) )
    {
      if ( powerR == 0 ) {
        powerR = 1;
      } else {
        powerR = 0;
      }
      valueChanged = 1;
    }

    // JS abfragen
    if ( mostLt(anreadings,5,200) || mostLt(anreadings,4,200) ) {
      modeL = modeL + 1;
      if ( modeL > 6 )
        modeL = 0;
      valueChanged = 1;
    }
     if ( mostGt(anreadings,5,700) || mostGt(anreadings,4,700) ) {
      modeL = modeL - 1;
      if ( modeL < 0 )
        modeL = 6;
      valueChanged = 1;
    }
    if ( mostLt(anreadings,3,200)   ) {
      modeR = modeR + 1;
      if ( modeR > 2 )
        modeR = 0;
      valueChanged = 1;
    }
    if (  mostGt(anreadings,3,700)  ) {
      modeR = modeR - 1;
      if ( modeR < 0 )
        modeR = 2;
      valueChanged = 1;
    }

    if ( valueChanged == 1 ) 
    {
      resetAnreadings();
      Serial.print("value changed: LEFT:");
      Serial.print(powerL,DEC);
      Serial.print("#");
      Serial.print(modeL,DEC);
      Serial.print(" RIGHT:");
      Serial.print(powerR,DEC);
      Serial.print("#");
      Serial.println(modeR,DEC);
      if ( powerL == 1 ) {
        switch ( modeL ) {
          case 0:
            digitalWrite(Relays[0],LOW);
            digitalWrite(Relays[1],LOW);        
            digitalWrite(Relays[2],HIGH);        
            break;
          case 1:
            digitalWrite(Relays[0],LOW);
            digitalWrite(Relays[1],HIGH);        
            digitalWrite(Relays[2],LOW);        
            break;  
          case 2:
            digitalWrite(Relays[0],HIGH);
            digitalWrite(Relays[1],LOW);        
            digitalWrite(Relays[2],LOW);        
            break;  
          case 3:
            digitalWrite(Relays[0],HIGH);
            digitalWrite(Relays[1],LOW);        
            digitalWrite(Relays[2],HIGH);        
            break;
          case 4:
            digitalWrite(Relays[0],LOW);
            digitalWrite(Relays[1],HIGH);        
            digitalWrite(Relays[2],HIGH);        
            break;  
          case 5:
            digitalWrite(Relays[0],HIGH);
            digitalWrite(Relays[1],HIGH);        
            digitalWrite(Relays[2],LOW);        
            break; 
          case 6:
            digitalWrite(Relays[0],HIGH);
            digitalWrite(Relays[1],HIGH);        
            digitalWrite(Relays[2],HIGH);        
            break;    
        }
      } else {
        digitalWrite(Relays[0],LOW);
        digitalWrite(Relays[1],LOW);
        digitalWrite(Relays[2],LOW);
      }
      if ( powerR == 1 ) {
        switch ( modeR ) {
          case 0:
            digitalWrite(Relays[3],LOW);
            digitalWrite(Relays[4],HIGH);        
            break;
          case 1:
            digitalWrite(Relays[3],HIGH);
            digitalWrite(Relays[4],HIGH);        
            break;  
          case 2:
            digitalWrite(Relays[3],HIGH);
            digitalWrite(Relays[4],LOW);        
            break;  
        }
      } else {
        digitalWrite(Relays[3],LOW);
        digitalWrite(Relays[4],LOW);        
      }
    }
    
    
    delay(300);

      
}//end loop




bool mostEq(int anreadings[numInputs][stackSize], int input, int value) {

    int match = 0;
 
    for ( k = 0 ; k<stackSize ; k++ )
    {
      //Serial.print(anreadings[input][k],DEC);
      if ( anreadings[input][k] == value && anreadings[input][k] != -1 )
        match++;
    }
    //Serial.print("match: ");
    //Serial.println(match,DEC);
    if ( match > 8 )
      return true;
    else
      return false;
  }
bool mostLt(int anreadings[numInputs][stackSize], int input, int value) {
  int match = 0;
    for ( k = 0 ; k<stackSize ; k++ )
    {
      if ( anreadings[input][k] < value && anreadings[input][k] != -1  )
        match++;
    }
    if ( match > 8 )
      return true;
    else
      return false;
  }
bool mostGt(int anreadings[numInputs][stackSize], int input, int value) {
  int match = 0;
    for ( k = 0 ; k<stackSize ; k++ )
    {
      if ( anreadings[input][k] > value  && anreadings[input][k] != -1 )
        match++;
    }
    if ( match > 8 )
      return true;
    else
      return false;
  }

void resetAnreadings() {
  for ( i=0; i<numInputs ; i++ )
  {
    for ( j=0; j<stackSize ; j++ ) 
    {
      anreadings[i][j] = -1;  
    }
  }
}

void setbaud(char Mybaud)
{
   switch (Mybaud)
   {
    case 1 : Serial.begin(1200);
      break;
    case 2 : Serial.begin(2400);
      break;     
    case 3 : Serial.begin(4800);
      break;
    case 4 : Serial.begin(9600);
      break;
    case 5 : Serial.begin(14400);
      break;
    case 6 : Serial.begin(19200);
      break;
    case 7 : Serial.begin(28800);
      break;
    case 8 : Serial.begin(38400);
      break;
    case 9 : Serial.begin(57600);
      break;
    case 10 : Serial.begin(115200);
      break;
    default:  Serial.begin(9600);
      break;
   }
}
