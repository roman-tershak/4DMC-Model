#include "mpcomm.h"
#include "hardware.h"

void send_no_data_command(uint8_t address, uint8_t command)
{

    // The address and command in the address byte
    USART_transmit(USART_ADDRESS_BYTE, (address | command));
}

void send_one_byte_command(uint8_t address, uint8_t command, uint8_t data)
{
    // The address and command in the address byte
    USART_transmit(USART_ADDRESS_BYTE, (address | command));
    // Send one byte data
    USART_transmit(USART_DATA_BYTE, data);
}

void send_multi_byte_command(uint8_t address, uint8_t command, uint8_t *data_ptr, uint8_t data_len)
{
    uint8_t i;
    // Send address and command in the 1st byte
    USART_transmit(USART_ADDRESS_BYTE, (address | command));
    // Send the data
    for (i = 0; i < data_len; i++)
    {
        USART_transmit(USART_DATA_BYTE, *(data_ptr + i));
    }
}

