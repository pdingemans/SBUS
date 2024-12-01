
#ifndef Timer_h
#define Timer_h

#include <Arduino.h>
class Timer
{
    public:
    Timer();
    void startRepeat(uint16_t repeatTime);
    void startOneShot(uint16_t repeatTime);
    bool isElapsed();
    private:
    unsigned long starttime;
    unsigned int time;
    bool oneshot;
};
#endif