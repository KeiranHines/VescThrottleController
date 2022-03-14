#include "utils.h"

void printSerial(const char *header, unsigned char *data, int len)
{
    debugSerial->print(header);
    for (int i = 0; i <= len; i++)
    {
        debugSerial->print(data[i]);
        debugSerial->print(" ");
    }
    debugSerial->println("");
}
void printSerial(const char *header, std::string data, std::size_t len)
{
    for (int i = 0; i < data.length(); i++)
    {
        debugSerial->print(data[i], DEC);
        debugSerial->print(" ");
    }
}

float mapFloat(float x, float in_min, float in_max, float out_min, float out_max)
{
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}