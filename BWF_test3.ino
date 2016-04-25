/*
BWF test



*/

//#define USE_SAMPLES
#ifdef  USE_SAMPLES
#define    AVE_DIFF_SIZE    140
#define    SAMPLES_SIZE      (2048-(AVE_DIFF_SIZE*2)-500)/2
#else
#define    AVE_DIFF_SIZE    200
#endif
#define    AVERAGE_COUNT    500

int bwfAmplPin = 1;  // BWF amplitude pin
int bwfPeakPin = 3;  // BWF peak hold pin
int bwfPin  = 6;    // BWF input pin 
int ledHasSignalPin = 2;     // Inside LED (green) output pin
int ledOutsidePin = 3;     // Outside LED (red) output pin

 
int cnt5 = 0;
long sum5 = 0;



#ifdef    USE_SAMPLES
const int sSize = SAMPLES_SIZE;
int samples[sSize];
int ptr = 0;
#endif


long lastMillis = 0;

void setup() 
{
    Serial.begin(9600); 
    Serial.println("Start BWF test" );

    pinMode(ledHasSignalPin, OUTPUT);
    pinMode(ledOutsidePin, OUTPUT);
    pinMode(bwfPin, INPUT_PULLUP );
    pinMode(4, INPUT_PULLUP );
    pinMode(5, INPUT_PULLUP );
        
    digitalWrite( ledOutsidePin, HIGH );
    digitalWrite( ledHasSignalPin, HIGH );
    delay( 1000 );
    
    digitalWrite( ledHasSignalPin, LOW );
    digitalWrite( ledOutsidePin, LOW );

    Serial.println("Running..." );
}



/*
* Digital Signal (inside):
* --------. .----------
*         | |
*         | |
*         | |
*         '-'
* Analog signal (inside):
*         *   
*         **  
*         * *  
*         *  *  
*  -------*   *-----------
*
* Digital Signal (inside):
* --------.      .----------
*         |      |
*         |      |
*         |      |
*         '------'
* Analog signal (outside):
*         *  * 
*         ** ** 
*         * ** * 
*         *     *  
*  -------*      *-----------
*                 
* we want to trigger on falling edge, then read analog amplitude a number of times to ensure that the amplitude is positive
* If positive we will be inside the fence
*/


bool lastPulse = false;
long pulseStart = 0;
long pulseEnd = 0;


long lastPulseTime = 0;


long endEcho = 0;
long endPause = 0;
int state = 0;

void loop() 
{
    int dig =  digitalRead( 5 );
    
    if (lastMillis+500 < millis()) {
        Serial.println( "." );
        lastMillis = millis();
    }
  
    int cntLow = 0;
    int cnt = 0;  
    while (digitalRead( bwfPin ) == 0) {
        if (++cntLow > 1024) break;
    }
    pulseStart = micros();
    while (digitalRead( bwfPin ) == 1) cnt++;
    pulseEnd = micros();

    bool hasPulse = false;
    if (cnt > 1) {
        digitalWrite( ledHasSignalPin, HIGH );
        lastPulseTime = millis();
        hasPulse = true;
    } else {
        if (lastPulseTime+100 < millis()) {
            digitalWrite( ledHasSignalPin, LOW );
            digitalWrite( ledOutsidePin, LOW );
        }
    }
    
    if (hasPulse) {
        // Get average pulsewidth:
        if (cnt5 >= 5) {
            sum5 -= sum5/5;
        } else {
            cnt5++;
        }
        int pWidth = pulseEnd-pulseStart;
        sum5 += pWidth;
        int avgWidth = sum5/cnt5;
        switch (state) {
            case 0: // Start echo
                digitalWrite( ledOutsidePin, HIGH );
                endEcho = millis() + 20 + cnt; //avgWidth/2;
                state = 1;
                break;
            case 1: // wait for echo time to pass
                if (endEcho <= millis()) {
                    digitalWrite( ledOutsidePin, LOW );
                    endPause = millis() + cnt; //avgWidth/3;
                    state = 2;
                }
                break;
            case 2: // wait for pause time to pass
                if (endPause <= millis()) {
                    state = 0;
                    /** /
                    Serial.print( cnt );
                    Serial.print( "/" );
                    Serial.print( pWidth );
                    Serial.print( "/" );
                    Serial.print( avgWidth );
                    Serial.print( "  " );
                    /**/
                }
                break;
        }
    }
 }





