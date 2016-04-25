
#include "Arduino.h"
#include "asyncmorse.h"

// int ledPin = 33;
// const int dotDelay = 200; // ms

const char* const letters[] = {
    ".-",    // A
    "-...",  // B
    "-.-.",   // C
    "-..", // D
    ".", // E
    "..-.", // F
    "--.", // G
    "....", // H
    "..", // I
    ".---", // J
    "-.-", // K
    ".-..", // L
    "--", // M
    "-.", // N
    "---", // O
    ".--.", // P
    "--.-", // Q
    ".-.", // R
    "...", // S
    "-", // T
    "..-", // U
    "...-", // V
    ".--", // W
    "-..-", // X
    "-.--", // Y
    "--.." // Z
};


const char* numbers[] = {
    "-----",  // 0
    ".----",  // 1
    "..---",  // 2
    "...--",  // 3
    "....-",  // 4
    "-....",  // 5
    "--...",  // 6
    "---..",  // 7
    "----.",  // 8
    "-----"   // 9
};

const char *empty = "\0\0";

// Constructor
void AsyncMorse::Init( int pin, int dotDelayMs )
{
    speakerPin = pin;
    dotDelay = dotDelayMs;
    nextTimeUpdate = 0;
    nextDotOrDash = NULL;
    nextChar = NULL;
    curState = Pause;
}


void AsyncMorse::Update()
{
    if ((nextTimeUpdate == 0) || (nextTimeUpdate > millis())) {
        return; // Nothing to do
    }

    switch (curState) {
        case DotOrDash:
            //Serial.print( *nextDotOrDash );
            digitalWrite( speakerPin, HIGH );
            // Character time delay:
            if (*nextDotOrDash == '.') {
                nextTimeUpdate = millis() + dotDelay;
            } else {
                nextTimeUpdate = millis() + dotDelay*3;
            }
            curState = Pause; // next state
            break;
        case Pause:
            digitalWrite( speakerPin, LOW );
            // Betwen character delay:
            nextTimeUpdate = millis() + dotDelay;
            // fall through...
        case NextCh:
            //Serial.print( "/NC/" );
            nextDotOrDash++;
            if (*nextDotOrDash == 0) {
                // End of dot/dash, do next letter:
                nextChar++;
                if (*nextChar == 0) {
                    // End of sequence, stop
                    nextTimeUpdate = 0;
                    nextChar = NULL;
                    nextDotOrDash = NULL;
                    //Serial.print( "/done/" );
                    return;
                }
                if (*nextChar == ' ') {
                    nextTimeUpdate = millis() + dotDelay*6;  // Gap between words = 1+6
                    curState = NextCh;
                    nextDotOrDash = empty;
                    //Serial.print( "/#/" );
                    return;
                }
                nextTimeUpdate = millis() + dotDelay*2;  // Gap between letters = 1+2
                nextDotOrDash = GetNextMorseLetter( *nextChar );
                curState = DotOrDash;
                //Serial.print( "/Ch:" );
                //Serial.print( *nextChar );
                //Serial.print( "/" );
                return;
            }
            curState = DotOrDash;
            break;
    }
}


const char* AsyncMorse::GetNextMorseLetter( char letter )
{
    if ('a' <= letter && letter <= 'z') {
        return letters[ letter - 'a' ];
    } else if ('A' <= letter && letter <= 'Z') {
        return letters[ letter - 'A' ];
    } else if ('0' <= letter && letter <= '9') {
        return numbers[ letter - '0' ]; 
    } else { // if (letter == ' ') {
        return NULL;
      // delay( dotDelay*4 ); // gap between words
    }
}


void AsyncMorse::StartSendMorse( char *sequence )
{
    nextTimeUpdate = millis()-1;
    nextChar = sequence;
    nextDotOrDash = GetNextMorseLetter( *nextChar );
    curState = DotOrDash;
    Update();
}




