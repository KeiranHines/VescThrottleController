#include "utils.h"
#include <Arduino.h>

void printSerial(const char *header, unsigned char *data, int len)
{
    Serial.print(header);
    for (int i = 0; i <= len; i++)
    {
        Serial.print(data[i]);
        Serial.print(" ");
    }
    Serial.println("");
}
void printSerial(const char *header, std::string data, std::size_t len)
{
    for (int i = 0; i < data.length(); i++)
    {
        Serial.print(data[i], DEC);
        Serial.print(" ");
    }
}

float mapFloat(float x, float in_min, float in_max, float out_min, float out_max)
{
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}