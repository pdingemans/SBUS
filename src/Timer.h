
#ifndef Timer_h
#define Timer_h

#include <Arduino.h>
class Timer
{
    public:
    Timer();
    void startRepeat(unsigned int repeatTime);
    void startOneShot(unsigned int repeatTime);
    bool isElapsed();
    private:
    unsigned long starttime;
    unsigned int time;
    bool oneshot;
};
#endif