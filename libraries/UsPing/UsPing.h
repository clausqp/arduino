#ifndef __UsPing_h
#define __UsPing_h

/* UsPing.h - Library for handling Paralax UltraSonic Ping))) sensor
   Created by Claus Ørnhøj Petersen, September 15, 2014.
   
   This class handles the process of triggering the Ping)))-sensor, and handling the return echo using 
   interrupt capture of signal transition.
*/


#include "Arduino.h"

class UsPing
{
  public:
    void Init( byte pingPin );

    void Trigger();
    void MarkTimer( int timerOverflow, int timerStart ) ;
    long CalculateCmEchoDistance();
    void EchoResponse(int timerOverflow, int timerCnt);
    long TimeDifference(int timerOverflow, int timerCnt);
    
    int echoTime;

  private:
    byte _pingPin;
    int _intNo;
    int _timerStart;
    int _oflowStart;
};


#endif

