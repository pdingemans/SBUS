 #include "Timer.h"

  
Timer::Timer()
{
    starttime=micros();
    time = 0;  
    oneshot=true;
}
void Timer::startRepeat(unsigned int repeatTime)
{
    starttime=micros();
    oneshot=false;
    time = repeatTime;
}
void Timer::startOneShot(unsigned int repeatTime)
{
    starttime=micros();
    oneshot=true;
    time = repeatTime;
}
bool Timer::isElapsed()
{
    bool elapsed = (micros()-starttime>time);
    if (elapsed && !oneshot)
    {
        startRepeat(time);
    }
    return elapsed;
}
    