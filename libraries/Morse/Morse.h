#ifndef Morse_h
#define Morse_h

/* Morse.h - Library for sending morse code
   Created by Claus Ørnhøj Petersen, juni 6, 2015.
   
*/


#include "Arduino.h"

class Morse
{
  public:
    // Morse();
	void Init( int ledPin, int dotDelayMs );

	void SendMorse( char *sequence );
	//void SendMorse( __FlashStringHelper *sequence );
	//void SendMorse( const __FlashStringHelper *sequence );
    void SendMorseLetter( char letter );
	
  private:
    int _ledPin;
    int _dotDelay;
    
    void DotOrDash( char dotOrDash );
    void SendMorseSequence( char *sequence );
};

#endif

