#include "SBus.h"
#include "Timer.h"
#include <Servo.h>
SBus sBus;
Timer mytimer;
Servo servo1;
Servo servo2;
Servo servo3;
uint32_t usecs = 1000;
void setup() 
{
  pinMode (9, OUTPUT);
  pinMode (13, OUTPUT);
  sBus.begin();
  Serial.println("starting the sbus decoder");

  mytimer.startRepeat(10); // start de timer zodat die na 500 msec elapsed geeft en zichzelf weer start daarna
  uint32_t a = micros();
  servo1.attach(12);
  servo2.attach(11);
  servo3.attach(10);
  servo1.writeMicroseconds(1000 );
  servo2.writeMicroseconds(1510);
  servo3.writeMicroseconds(1520);
  

}

void loop() {
  // call this method to read the data from the serial input
  // btw: you need to invert the sbus signal to be able to receive it correctly.
  SBus::SIGNAL_STATUS status =  sBus.UpdateChannels();
  
  // de volgende software checked de status.
  // als de status OK is, is er geldige data aanwezig en kunnen de kanalen uitgelezen worden.
  
  int delta = 2000;
  if (delta <= -20015|| delta >= -1985)
  {
   //Serial.println(delta); 
    
  }
  if (mytimer.isElapsed())
  {
      usecs=(usecs+20)%(2010-990)+990;
      //Serial.println(usecs); 
     
  }
  if (status == SBus::SIGNAL_STATUS::OK)
  {
    
    // https://github.com/bolderflight/SBUS
    // frsky : minimal value 172, max value 1811
    // kanaal 0 : aileron kanaal (althans bij mijn zender en ontvanger)

    // de volgende code vertaald de waarde van channels[0] naar een waarde tussen 0 en 255
    // en stuurt dit vervolgens naar de LED
    // gevolg: LED helderheid is afhankelijk van de stand van de stuurknuppel
    int val = map (sBus.getChannels()[0], 172, 1811, 1000, 2000);

    Serial.println(sBus.getChannels()[1]); 
 servo2.writeMicroseconds(val);
    //analogWrite(9, val);

  }
 else 
 {
  // toggle the led on pin 13 after a timer elapsed
   // hier heeft de ontvanger laten weten dat er iets niet in orde is
   // het blijkt wel dat er af en toe een communicatie LOST tussendoor komt
   // deze kunnen we dan gevoeglijk negeren

 }

 // volgende code gebruikt de timer om iets te doen wanneer die afloopt
 //if(mytimer.isElapsed()) 
 {
   //Serial.println("elapsed");
   
 }
}
