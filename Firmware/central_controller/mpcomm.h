#ifndef H_MPCOMM
#define H_MPCOMM

#include <stdio.h>

/*---- USART commands ----*/
/* 4 higher bits are the MCU address */
#define MCU_ADDRESS_MASK        0xF0
/* Commands use lower bits in address byte */
#define MCU_COMMAND_MASK        0x0F

#define MCU_BROADCAST_ADDRESS   0xF0

/*---- Macros ----*/
#define GET_MCU_ADDRESS_FROM_SIDE_NUM(side_num) ((side_num) << 4)

#ifdef __cplusplus
 extern "C" {
#endif

void send_no_data_command(uint8_t address, uint8_t command);

void send_one_byte_command(uint8_t address, uint8_t command, uint8_t data);

void send_multi_byte_command(uint8_t address, uint8_t command, uint8_t *data_ptr, uint8_t data_len);

#ifdef __cplusplus
}
#endif 

#endif
