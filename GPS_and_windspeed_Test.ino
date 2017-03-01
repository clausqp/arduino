/* This project was made to measure speed of a vehiecle while recording the number of tics a windspeed sensor emits pr. second.
 * The idea is to corelate vehiecle speeds with a specific number of tics. It might not work :S 
 * It did work :)
 */


#include <SoftwareSerial.h>
#include "CopTinyGps.h"

// måske kan vi bruge: #include <YetAnotherPcInt.h> ???

long lat,lon; // create variable for latitude and longitude object

const int windSpeedPin = 3; // kan kun være pin 2 eller 3, vi skal bruge et HW irq.
const int dumpGpsCharsPin = 2;

const int gpsRxDataPin = 10; // Kan ikke være en pin på samme port som windSpeedPin, det giver falske interupts! dvs. pin > 8!
const int gpsTxDataPin = 11; // NC

const int ledPin = 13;

CopTinyGps gps; // create gps object
SoftwareSerial gpsSerial( gpsRxDataPin, gpsTxDataPin, true ); // create gps sensor connection (RxPin, TxPin, inverseLogic)


long timeToResteCount = 0;

volatile int windCount = 0;
volatile byte state = LOW;


void blink() 
{
    windCount++;
    state = !state;
}



// prints val with number of decimal places determine by precision
// NOTE: precision is 1 followed by the number of zeros for the desired number of decimial places
// example: printDouble( 3.1415, 2 ); // prints 3.14 (two decimal places)
void printDouble( double val, byte precision)
{
    Serial.print (int(val));  //prints the int part
    if (precision > 0) {
        Serial.print("."); // print the decimal point
        unsigned long frac;
        unsigned long mult = 1;
        byte padding = precision -1;
        while (precision--) {
            mult *=10;
        }
        if (val >= 0) {
            frac = (val - int(val)) * mult;
        } else {
            frac = (int(val)- val ) * mult;
        }
        unsigned long frac1 = frac;
        while (frac1 /= 10) {
            padding--;
        }
        while (padding--) {
            Serial.print("0");
        }
        Serial.print(frac,DEC) ;
    }
}


void setup() 
{
    Serial.begin(9600); // connect serial
    gpsSerial.begin( 4800 ); // connect gps sensor

    pinMode( ledPin, OUTPUT );
    pinMode( windSpeedPin, INPUT_PULLUP );
    pinMode( dumpGpsCharsPin, INPUT_PULLUP );

    int intr = digitalPinToInterrupt(windSpeedPin);
    Serial.println( intr);
    
    attachInterrupt(digitalPinToInterrupt(windSpeedPin), blink, CHANGE);
  
    Serial.println( "GPS Hacking..." );
    timeToResteCount = millis() + 1000; // 1 sec.
}


bool firstTime = true;
long flat,flon;

long linieNo = 0;
bool startTag  = false;
int tagLen = 0;

long lastCharReceived = 0;

void loop() 
{
    digitalWrite(ledPin, state);

    if (timeToResteCount < millis()) {
        timeToResteCount = millis() + 1000; // 1 sec.
        Serial.print( "Count: "); Serial.println( windCount );
        windCount = 0;

        unsigned long chars;
        unsigned short good_sentences;
        unsigned short failed_cs;
        gps.stats( &chars, &good_sentences, &failed_cs);
        Serial.print( String("Chars=" ) + chars  + ", good sentences=" + good_sentences + ", failed cs=" + failed_cs );
        
        bool gpsDataGood;
        unsigned short numSats;
        if (lastCharReceived+1000 > millis()) {
            gps.Info( &gpsDataGood, &numSats );
            Serial.print( String(", good data=" ) + gpsDataGood + ", numb.sats=" + numSats );
        } else {
            gpsDataGood = false;
            numSats  = 0;
            Serial.print( ", ingen gps data" );
        }
        Serial.println();
    }

    gps.debug = (digitalRead( dumpGpsCharsPin ) == LOW);
    
    if (gpsSerial.available()) { // check for gps data
        char ch = gpsSerial.read();
        lastCharReceived = millis();

        /** /
        if (! ('0' <= ch && ch <= '9')) {
         Serial.print( ch );
        }
        /* */

        /**/
        if (digitalRead( dumpGpsCharsPin ) == LOW) {
            Serial.print( ch );
        }
        /* */

        /*
        if (ch == '$') {
            startTag = true;
            tagLen = 0;
        }

        if (startTag) {
            Serial.print( ch );
            tagLen++;
            if (tagLen == 5) {
                startTag = false;
            } else if (('0' <= ch && ch <= '9')) {
                startTag = false;
            }
        }
        /* */

        unsigned long fixAge;
        if (gps.encode( ch )) { // encode gps data
            gps.get_position(&lat,&lon, &fixAge); // get latitude and longitude
            if (firstTime) {
                flat = lat;
                flon = lon;
                firstTime = false;
            }
            linieNo++;

            Serial.print( " ");
            Serial.print( linieNo );

            // display position
            Serial.print(" Position:");
            Serial.print(" lat: ");Serial.print(lat); Serial.print( " " ); Serial.print( lat-flat );// print latitude
            Serial.print(" lon: ");Serial.print(lon); Serial.print( " " ); Serial.print( lon-flon );// print longitude
            Serial.print(", speed: " ); 
            
            // 100th of a knot: Serial.print( gps.speed() ); // Speed from gps

            double m_pr_s = gps.f_speed_mps(); 
            printDouble( m_pr_s, 2 );
            Serial.print( " m/s, " );

            double km_pr_h = gps.f_speed_kmph();
            printDouble( km_pr_h, 3 );
            Serial.print( " km/h," );
            
            Serial.print(" count: " ); Serial.print( windCount ); // wind speed counter
            Serial.println();
        }
    }
}




