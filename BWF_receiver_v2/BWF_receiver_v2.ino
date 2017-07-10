/*
Avkodning av slingsändare.
Design av Per-Ola Johansson 2012-2014
Mottagare ansluts till pin0 på ArduinoUno
Inne/Ute/OK-signal på pin 11,12,13
mer info på www.pojpoj.se 

2017-05-23: Now using software serial, to free USB on Serial
*/
 


#include <Arduino.h>
#include "Bwf.h"
#include <SoftwareSerial.h>
 
 
// initialize the library with the numbers of the interface pins

byte timer = 0; // sekundräknare, nollas när den är inne, nollas inte om ute eller ingen signal
long previousMillis = 0;

const int insidePin = 11;
const int outsidePin = 12;
const int errorPin = 13;

Bwf bwfSensor;

void setup() 
{
    pinMode( errorPin, OUTPUT );   // OK/Error LED
    pinMode( outsidePin, OUTPUT ); // outside LED
    pinMode( insidePin, OUTPUT );  // inside LED
    
    Serial.begin( 9600, SERIAL_8E2 ); // 8bit, even parity, 2 stop bits
    // for debugging!
    Serial.println( "BWF receiver v2.2" );

    digitalWrite( errorPin, LOW );

    bwfSensor.Init( &Serial );
}


bool isError = false;

 
void loop() 
{
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis > 1000) {
        previousMillis = currentMillis;
        timer = timer + 1;
        if (bwfSensor.state == 1) {
          Serial.print( " *IN* ");
          
        } else if (bwfSensor.state == 2) {
          Serial.print( " *OUT* " );
        } else if (isError) {
          Serial.print( " *ERR* " );
        }
        Serial.println( String("Inside:") + bwfSensor.insideCnt + ", outside: " + bwfSensor.outsideCnt + ", crap: " + bwfSensor.crapCnt + ", timer: " + timer );
    }


    if (bwfSensor.CheckEvents()) {
        // Turn off control pins:
        digitalWrite( insidePin, LOW );
        digitalWrite( outsidePin, LOW );
        if (bwfSensor.state == 1) {
            digitalWrite( errorPin, LOW ); // Signal found, no error
            digitalWrite( insidePin, HIGH ); // Signal we're inside
            timer = 0;
        } else if (bwfSensor.state == 2) {
            digitalWrite( outsidePin, HIGH ); // Signal we're outside
        }
    }

    
    if (timer > 5) { 
        digitalWrite( errorPin, HIGH ); // escaped, or no signal
        isError = true;
        digitalWrite( insidePin, LOW );
        digitalWrite( outsidePin, LOW );
    }
}

