#ifndef VESC_UART_H
#define VESC_UART_H

#include <Arduino.h>
#include "freertos/semphr.h"

#ifndef PACKET_MAX_PL_LEN
#define PACKET_MAX_PL_LEN 512
#endif
#define BUFFER_LEN (PACKET_MAX_PL_LEN + 8)

/**
 * Wrapper class for VescUart wrapping Hardware serial
 * to enable Vesc specific command methods.
 */
class VescUart : public HardwareSerial
{
public:
    VescUart(int uart_nr);
    void writeCurrentRel(float current);
    void alive();

private:
    int PackSendPayload(unsigned char *data, unsigned int len);
    xSemaphoreHandle mutex;
};

#endif // VESC_UART_H