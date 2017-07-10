#ifndef Bwf_h
#define Bwf_h

#include <Arduino.h>
#include "Bwf.h"


Bwf::Bwf()
{
    lastSeenInside = 0;
    lastSeenOutside = 0;
    isInside = false;
    isOutside = false;
    isError = false;
    insideCnt = 0;
    outsideCnt = 0;
    crapCnt = 0;
}


void Bwf::Init( Stream *out )
{
    bwfSerial = out;
    state = 0;
}


bool Bwf::CheckEvents()
{
    bool anyEvents = false;
    // send data only when you receive data:
    while (bwfSerial->available() > 0) {
        // Turn off control pins:
        // read the incoming byte:
        byte incomingByte = bwfSerial->read();
        if (incomingByte == 107) { // 107 is inside fence
            isInside = true;
            isError = false;
            insideCnt++;
            state = 1;
            lastSeenInside = millis();
            Serial.print( "In " );
        } else
        if (incomingByte == 171) { // 171 is outside fence
            isOutside = true;
            outsideCnt++;
            state = 2;
            lastSeenOutside = millis();
            Serial.print( "Out " );
        } else {
            crapCnt++;
            Serial.print( String("'" + (char)incomingByte) + "'(" + incomingByte + ") " );
        }
        anyEvents = true;
    }
    return anyEvents;
}


bool IsError()
{
    return false; // (millis()-lastSeenInside > 5000) && (millis()-lastSeenOutside > 5000);
}


bool Bwf::IsInside()
{
    return isInside;
}


bool Bwf::IsOutside()
{
    return isOutside;
}


#endif // Bwf_h
