#include <Arduino.h>
#include "SBus.h"
#include "Timer.h"
// #include <Servo.h>

Timer mytimer;
constexpr uint8_t LANDINGLIGHTS = 10;
constexpr uint8_t TAILLIGHTS = 11;
constexpr uint8_t NAVLIGHTS = 12;

//  bit info 

uint32_t usecs = 1000;
void setup()
{
  // Serial.begin(100000);
  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(13, OUTPUT);

  // Serial.println("starting the sbus decoder");

  // mytimer.startRepeat(10); // start de timer zodat die na 500 msec elapsed geeft en zichzelf weer start daarna
  uint32_t a = micros();
}
int val;
int laststate;
void loop()
{
  SBus sBus(Serial);
  // call this method to read the data from the serial input
  // btw: you need to invert the sbus signal to be able to receive it correctly.
  while (true)
  {

    SBus::SIGNAL_STATUS status = sBus.UpdateChannels();

    if (status == SBus::SIGNAL_STATUS::OK)
    {

      laststate = status;
      // https://github.com/bolderflight/SBUS
      // frsky : minimal value 172, max value 1811
      // kanaal 0 : aileron kanaal (althans bij mijn zender en ontvanger)

      // de volgende code vertaald de waarde van channels[0] naar een waarde tussen 0 en 255
      // en stuurt dit vervolgens naar de LED
      // gevolg: LED he calderheid is afhankelijk van de stand van de stuurknuppel
      // int val = map (sBus.getChannels()[0], 172, 1811, 1000, 2000);
      // Serial.println("TO");
      uint16_t newval = sBus.getChannels()[0];
      // we use 10 bits to switch the leds on or off
      // bit 0 : landing lights
      // bit 1 : tail lights
      // bit 2 : nav lights
      // bit 3..9 : not used
      
      newval = map(newval, 172, 1811, 0, 1023);

      if (newval != val)
      {
        digitalWrite(13, newval > 512);
                digitalWrite(10, newval > 512);
                        digitalWrite(11, newval > 512);
        val = newval;
        // Serial.println((val>>1)+1);
        // servo2.writeMicroseconds(val);
      } // analogWrite(9, val);
    }
    else
    {
      // toggle the led on pin 13 after a timer elapsed
      // hier heeft de ontvanger laten weten dat er iets niet in orde is
      // het blijkt wel dat er af en toe een communicatie LOST tussendoor komt
      // deze kunnen we dan gevoeglijk negeren

      digitalWrite(13, !digitalRead(13));
    }
  }
}
