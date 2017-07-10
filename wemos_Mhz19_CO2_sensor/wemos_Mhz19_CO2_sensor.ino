#include <stdio.h>
#include <arduino.h>

#include <ThingSpeak.h>
#include <ESP8266WiFi.h>

#include "SoftwareSerial.h"
#include "Mhz19.h"

// Define I/O pins:
const int rxPin = D3;
const int txPin = D2; // -> rx on MHz19



//*** ThingSpeak setup:
static unsigned long channelNumber = 227747;
static const char *writeAPIKey = "F1EKERJ5IDL23QTW";
const char* host = "api.thingspeak.com";
const int httpPort = 80;
const char* serviceName = "Update";

//*** WiFi network setup:
static WiFiClient client;


long nextRead;
SoftwareSerial sensor(rxPin, txPin);


byte cmd[9] = {0xFF,0x01,0x86,0x00,0x00,0x00,0x00,0x00,0x79}; // Man kunne også bare sende denne pakke <-----

bool SendCommand( uint8_t cmd )
{
    // create command buffer
    uint8_t buf[9];
    uint8_t data[] = {0, 0, 0, 0, 0, 0};
    int len = prepare_tx(cmd, data, buf, sizeof(buf));

    // send the command
    sensor.write(buf, len);
}


bool WaitForResponse( uint8_t cmd, uint8_t data[], int timeout )
{
    // wait for response
    long start = millis();
    while ((millis() - start) < timeout) {
        if (sensor.available() > 0) {
            uint8_t b = sensor.read();
            if (process_rx(b, cmd, data)) {
                return true;
            }
        }
    }
    return false;
}


bool ReadTempAndCo2(int *co2, int *temp)
{
    uint8_t data[] = {0, 0, 0, 0, 0, 0};
    SendCommand( 0x86 );
    bool ok = WaitForResponse( 0x86, data, 3000 );
    if (ok) {
        *co2 = (data[0] << 8) + data[1];
        *temp = data[2] - 40;
#if 1
        char raw[32];
        sprintf(raw, "RAW: %02X %02X %02X %02X %02X %02X", data[0], data[1], data[2], data[3], data[4], data[5]);
        Serial.println(raw);
#endif
    }
    return ok;
}



void setup() 
{
    Serial.begin(9600);
    delay(100);
  
    // We start by connecting to a WiFi network

    int idx = 0;
    const char *ssid;
    const char *password;
    do {
        idx++; if (idx > 2) idx = 1;
        switch (idx ) {
          case 1:    
              ssid = "egeCL";
              password = "";
              break;

          case 2:
              ssid = "DSINEXT";
              password = "Ds1N3x7r0x";
              break;

          case 3: 
              ssid = "TelenorA34E93";
              password= "013422EEF5";
              break;
        }
    
        Serial.println();
        Serial.println();
        Serial.print("Connecting to ");
        Serial.println(ssid);
        WiFi.begin( ssid, password);
        for ( int i = 0; WiFi.status() != WL_CONNECTED && i < 20; i++) {
            delay(500);
            Serial.print(".");
        }
        Serial.println();
    } while (WiFi.status() != WL_CONNECTED);

    Serial.print("WiFi connected with IP address: ");
    Serial.println(WiFi.localIP());

    ThingSpeak.begin(client);

    Serial.println( "Waiting 3 mins. for preheating to end" );
    nextRead = millis()+180000;

    sensor.begin(9600);
}




void TransmitData()
{
    int co2, tempC;
    if (ReadTempAndCo2( &co2, &tempC )) {
        Serial.print("CO2: ");
        Serial.print(co2);
        Serial.print(", temp: ");
        Serial.println(tempC );

        ThingSpeak.setField( 1, co2 );
        ThingSpeak.setField( 2, tempC );
        int ret = ThingSpeak.writeFields( channelNumber, writeAPIKey );
        Serial.println( String("Response from thingspeak: ") + ret );
    }
}



void loop() 
{
    if (nextRead < millis()) {
        nextRead = millis() + 180000;
        TransmitData();
    }
    delay(1000);
}



