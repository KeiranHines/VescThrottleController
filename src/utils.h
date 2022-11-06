#ifndef UTILS_H
#define UTILS_H

#include <sstream>
#include <Arduino.h>
#include "config.h"

extern HardwareSerial *debugSerial;
void printSerial(const char *header, unsigned char *data, int len);
void printSerial(const char *header, std::string data, std::size_t len);
float mapFloat(float x, float in_min, float in_max, float out_min, float out_max);
#endif // UTILS_H