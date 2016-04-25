#ifndef Morse_h
#define Morse_h

/* Morse.h - Library for sending morse code
   Created by Claus Ørnhøj Petersen, juni 6, 2015.
   
*/


#include "Arduino.h"


class AsyncMorse
{
  public:
    void Init( int ledPin, int dotDelayMs );

    void StartSendMorse( char *sequence );
    
    void Update();
	
  private:
    enum States { DotOrDash, Pause, NextCh };
    int speakerPin;
    int dotDelay;
    long nextTimeUpdate;
    const char *nextDotOrDash;
    char *nextChar;
    States curState;
    
    const char* GetNextMorseLetter( char letter );
    
};

#endif


