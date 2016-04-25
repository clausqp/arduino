

#include "UsPing.h"
  

// ---==* Defines *==---
#define TIMER_US_PER_TICK   4 // = 16MHz / 64 cycles per tick
#define TIMER_OVERFLOW_US   TIMER_US_PER_TICK * 65536 // timer1 is 16bit
#define CM_PER_SEC          34030  // Speed of sound


void UsPing::Init( byte pingPin )
{
    _pingPin = pingPin;  
    pinMode( _pingPin, OUTPUT);    // Set trigger pin as output
    echoTime = -1;
}

  
/* Function to trigger a pulse and start timing */
void UsPing::Trigger()
{
    echoTime = -1;      // clear any current time
    _oflowStart = -1;    // ignore INT1 for now

    // Serial.print("!");
    // COP says: for using same pin:
    pinMode(_pingPin, OUTPUT);    // Set trigger pin as output
  
    digitalWrite(_pingPin, LOW);      // Send 2us LOW pulse to ensure we get a nice clean pulse
    delayMicroseconds(2);

    digitalWrite(_pingPin, HIGH);    // Ping is triggered by a HIGH pulse of 2 or more microseconds
    delayMicroseconds(10);
    digitalWrite(_pingPin, LOW);      // End the ping

    // COP says: for using same pin:
    delayMicroseconds(2);
    pinMode(_pingPin, INPUT);        // Set echo pin as input
    delayMicroseconds(5);
    int val = digitalRead(_pingPin); // get signal value
    while(val == LOW) { // Loop until pin reads a high value
      val = digitalRead(_pingPin);
    }
    // Serial.print("#");
}


void UsPing::MarkTimer( int timerOverflow, int timerStart ) 
{
    _timerStart = timerStart;                    // Get the current time
    _oflowStart = timerOverflow;
}


// Skotske "proclaimers" - walk 500 miles



/* Interrupt Routines */
void UsPing::EchoResponse(int timerOverflow, int timerCnt) 
{
    // Only grab the time difference if we have triggered the pulse (ie the oflow_start is > 0)
    if (_oflowStart != -1) {
        echoTime = TimeDifference( timerOverflow, timerCnt);
    }
}


/* Function to return the time difference */
long UsPing::TimeDifference( int timerOverflow, int timerCnt ) 
{
    long us;
    us = (timerOverflow - _oflowStart) * 65536;  // overflows every 65k ticks, so nbr of overflows*65k
    us += (timerCnt - _timerStart);               // and add the timer remainder
    us *= TIMER_US_PER_TICK;                    // convert it to microseconds
    return us;
}


long UsPing::CalculateCmEchoDistance()
{
    // Divide echo_time by 1000000 in three steps for efficiency
    long cm = ((echoTime/100) * (CM_PER_SEC/100))/100;
    return cm/2; // divide by 2 for one way (not the return trip) 
}



