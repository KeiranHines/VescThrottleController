#include "VescUart.h"
#include "buffer.h"
#include "datatypes.h"
#include "crc.h"
#include "config.h"
#include "utils.h"

VescUart::VescUart(int uart_nr) : HardwareSerial(uart_nr) {
    mutex = xSemaphoreCreateRecursiveMutex();
}

void VescUart::alive()
{
    uint8_t payload[1];
    payload[0] = COMM_ALIVE;
    PackSendPayload(payload, 1);
}

void VescUart::writeCurrentRel(float current)
{
    int32_t index = 0;
    constexpr int PAYLOAD_SIZE =5;
    uint8_t payload[PAYLOAD_SIZE];

    payload[index++] = COMM_SET_CURRENT_REL;
    buffer_append_float32(payload, current, 1e5, &index); // TODO Confirm scaling. and if value should be passed as 0-1 or 0-100
    PackSendPayload(payload, PAYLOAD_SIZE);
}

int VescUart::PackSendPayload(unsigned char *data, unsigned int len)
{
    if (len == 0 || len > PACKET_MAX_PL_LEN)
    {
        return 0;
    }

    int b_ind = 0;
    xSemaphoreTakeRecursive(mutex, portMAX_DELAY);
    unsigned char tx_buffer[BUFFER_LEN];
    if (len <= UINT8_MAX)
    {
        tx_buffer[b_ind++] = 2;
        tx_buffer[b_ind++] = len;
    }
    else if (len <= UINT16_MAX)
    {
        tx_buffer[b_ind++] = 3;
        tx_buffer[b_ind++] = len >> 8;
        tx_buffer[b_ind++] = len & 0xFF;
    }
    else
    {
        tx_buffer[b_ind++] = 4;
        tx_buffer[b_ind++] = len >> 16;
        tx_buffer[b_ind++] = (len >> 8) & 0x0F;
        tx_buffer[b_ind++] = len & 0xFF;
    }

    memcpy(tx_buffer + b_ind, data, len);
    b_ind += len;

    unsigned short crc = crc16(data, len);
    tx_buffer[b_ind++] = (uint8_t)(crc >> 8);
    tx_buffer[b_ind++] = (uint8_t)(crc & 0xFF);
    tx_buffer[b_ind++] = 3;

#ifdef DEBUG
    printSerial("Compiled buffer: ", tx_buffer, b_ind);
#endif
    HardwareSerial::write(tx_buffer, b_ind);
    xSemaphoreGiveRecursive(mutex);
    return b_ind;
}