/* *********************************************************
 * IR PWM RGB LED
 *
 * 2012 Moritz Kobel, http://www.kobelnet.ch
 *
 * Change color using the function buttons on a Sony RM-U265 remote
 *
 ***********************************************************
 */

/* Based on several libraries + examples:
 * http://www.elcojacobs.com/shiftpwm/
 * http://www.arcfn.com/2009/08/multi-protocol-infrared-remote-library.html
 * http://wiki.play-zone.ch/index.php?title=Arduino_Fernbedienung
 */
 
 
/* *************
 * PWM Init
 * ************/
const int ShiftPWM_latchPin=8;
#define SHIFTPWM_NOSPI
const int ShiftPWM_dataPin = 11;
const int ShiftPWM_clockPin = 12;

const bool ShiftPWM_invertOutputs = false; 
const bool ShiftPWM_balanceLoad = false;

#include <ShiftPWM.h>   // include ShiftPWM.h after setting the pins!

unsigned char maxBrightness = 128;
unsigned char pwmFrequency = 50;
int numRegisters = 3;
int numRGBleds = numRegisters*8/3;

/* *************
 * IR Init
 * ************/
#include <IRremote.h>

// start receiver on pin 13
IRrecv irrecv(13);


void setup(){
  Serial.begin(9600);

  ShiftPWM.SetAmountOfRegisters(numRegisters);
  ShiftPWM.SetPinGrouping(8); 
  
  ShiftPWM.Start(pwmFrequency,maxBrightness);
  ShiftPWM.SetAll(0);
  
  irrecv.enableIRIn();
}

void loop() {
  decode_results results;
 
  // receive?
  if (irrecv.decode(&results)) {
 
    // Debug Value 
    Serial.println(results.value, HEX);
 
    // Set color
    switch ( results.value ) {
      case 0x441:  //button:video
        ShiftPWM.SetAllRGB(255, 0, 0);
        break;
      case 0x561: //button:tv
        ShiftPWM.SetAllRGB(0, 255, 0);
        break;      
      case 0xC41: //button:tape/md
        ShiftPWM.SetAllRGB(0, 0, 255);
        break;      
      case 0xA41: //button:cd
        ShiftPWM.SetAllRGB(96, 128, 0);
        break;     
      case 0x841: //button:tuner
        ShiftPWM.SetAllRGB(0, 128, 128);
        break;      
      case 0x041: //button:phono
        ShiftPWM.SetAllRGB(96, 0, 128);
        break;  
      default:
        ShiftPWM.SetAll(0);
    }
    
    
    irrecv.resume(); // continue receiving
  }
}



