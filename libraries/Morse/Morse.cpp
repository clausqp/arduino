
#include "morse.h"

// int ledPin = 33;
// const int dotDelay = 200; // ms

char* letters[] = {
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


char* numbers[] = {
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

// Constructor
void Morse::Init( int ledPin, int dotDelayMs )
{
    _ledPin = ledPin;
    _dotDelay = dotDelayMs;
}


void Morse::DotOrDash( char dotOrDash )
{
    digitalWrite( _ledPin, HIGH );
    if (dotOrDash == '.') {
        delay( _dotDelay );
    } else {
        delay( _dotDelay * 3 );  
    }
    digitalWrite( _ledPin, LOW );
    delay( _dotDelay );
}



void Morse::SendMorseSequence( char *sequence )
{
    for (int i = 0; sequence[i] != 0; i++) {
        DotOrDash( sequence[i] );
    }
    delay( _dotDelay * 3 ); // Gap between letters
}



void Morse::SendMorseLetter( char letter )
{
    if ('a' <= letter && letter <= 'z') {
        SendMorseSequence( letters[ letter - 'a' ] );
    } else if ('A' <= letter && letter <= 'Z') {
        SendMorseSequence( letters[ letter - 'A' ] );
    } else if ('0' <= letter && letter <= '9') {
        SendMorseSequence( numbers[ letter - '0' ] ); 
    } else if (letter == ' ') {
        delay( _dotDelay*4 ); // gap between words
    }
}


void Morse::SendMorse( char *sequence )
{
    for (int i = 0; sequence[i] != 0; i++) {
        SendMorseLetter( sequence[i] );
    }
}

/*
void Morse::SendMorse( __FlashStringHelper *sequence )
{
    for (int i = 0; sequence[i] != 0; i++) {
        SendMorseLetter( sequence[i] );
    }
}


void Morse::SendMorse( const __FlashStringHelper *sequence )
{
    for (int i = 0; sequence[i] != 0; i++) {
        SendMorseLetter( sequence[i] );
    }
}
*/



