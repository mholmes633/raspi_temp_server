#include "dht22.h"
#include <iostream>
#include <wiringPi.h>

#define MAXTIMINGS 85
#define LED 0

static int DHTPIN = 7;
static int dht22_dat[5] = { 0,0,0,0,0 };
static float t = {};
static float h = {};

static uint8_t sizecvt(const int read)
{
    /* digitalRead() and friends from wiringpi are defined as returning a value
    < 256. However, they are returned as int() types. This is a safety function */

    if (read > 255 || read < 0)
    {
        printf("Invalid data from wiringPi library\n");
        exit(EXIT_FAILURE);
    }
    return (uint8_t)read;
}

dht22::dht22()
{

}

dht22::~dht22()
{
    std::cout << "dht22 destructor called" << std::endl;
}

int dht22::readDHT22(bool Celsius)
{
    uint8_t laststate = HIGH;
    uint8_t counter = 0;
    uint8_t j = 0, i;
    //static bool LED_out = 0;

    dht22_dat[0] = dht22_dat[1] = dht22_dat[2] = dht22_dat[3] = dht22_dat[4] = 0;

    pinMode(LED, OUTPUT); // set LED GPIO to output
    digitalWrite(LED, HIGH);
    delay(1000);
    digitalWrite(LED, LOW);

    /*
    if (LED_out == 0) {
        LED_out = 1;
        digitalWrite(LED, LOW);
    }
    else {
        LED_out = 0;
        digitalWrite(LED, HIGH);
    }
    */

    
    // pull pin down for 18 milliseconds
    pinMode(DHTPIN, OUTPUT);
    digitalWrite(DHTPIN, HIGH);
    delay(500);
    digitalWrite(DHTPIN, LOW);
    delay(20);
    // prepare to read the pin
    pinMode(DHTPIN, INPUT);

    // detect change and read data
    for (i = 0; i < MAXTIMINGS; i++) {
        counter = 0;
        while (sizecvt(digitalRead(DHTPIN)) == laststate) {
            counter++;
            delayMicroseconds(2);
            if (counter == 255) {
                break;
            }
        }
        laststate = sizecvt(digitalRead(DHTPIN));

        if (counter == 255) break;

        // ignore first 3 transitions
        if ((i >= 4) && (i % 2 == 0)) {
            // shove each bit into the storage bytes
            dht22_dat[j / 8] <<= 1;
            if (counter > 16)
                dht22_dat[j / 8] |= 1;
            j++;
        }
    }

    // check we read 40 bits (8bit x 5 ) + verify checksum in the last byte
    // print it out if data is good
    if ((j >= 40) &&
        (dht22_dat[4] == ((dht22_dat[0] + dht22_dat[1] + dht22_dat[2] + dht22_dat[3]) & 0xFF))) {

        h = (float)dht22_dat[0] * 256 + (float)dht22_dat[1];
        h /= 10;
        t = (float)(dht22_dat[2] & 0x7F) * 256 + (float)dht22_dat[3];
        t /= (float)10.0;
        if ((dht22_dat[2] & 0x80) != 0)  t *= -1;

        if (Celsius)
            this->temperature = t;
        else
            this->temperature = (float)(t * 1.8 + 32);

        this->humidity = h;
        return 0;
    }
    else
    {
        this->temperature = -1.0;
        this->humidity = -1.0;
        return 1;
    }

    this->temperature = -1.0;
    this->humidity = -1.0;
    return 1;
}

float dht22::getTemperature()
{
    return this->temperature;
}

float dht22::getHumidity()
{
    return this->humidity;
}
