#ifndef __UTILS_H__
#define __UTILS_H__

#include <sstream>

void printSerial(const char *header, unsigned char *data, int len);
void printSerial(const char *header, std::string data, std::size_t len);
float mapFloat(float x, float in_min, float in_max, float out_min, float out_max);
#endif // __UTILS_H__