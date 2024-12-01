 #include "Timer.h"

  
Timer::Timer()
{
    starttime=millis();
    time = 0;  
    oneshot=true;
}
void Timer::startRepeat(uint16_t repeatTime)
{
    starttime=millis();
    oneshot=false;
    time = repeatTime;
}
void Timer::startOneShot(uint16_t repeatTime)
{
    starttime=millis();
    oneshot=true;
    time = repeatTime;
}
bool Timer::isElapsed()
{
    bool elapsed = ((millis()-starttime)>time);
    if (elapsed && !oneshot)
    {
        startRepeat(time);
    }
    return elapsed;
}
    