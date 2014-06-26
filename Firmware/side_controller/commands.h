#ifndef H_COMMANDS
#define H_COMMANDS

#include "driver.h"

/*---- USART commands ----*/
/* Commands use lower bits in address byte */
#define MCU_COMMAND_MASK		0x0F

#define USART_NO_COMMAND                0x00
#define USART_INIT_LOAD			0x01
#define USART_DISPLAY_COLORS            0x02
#define USART_STORE_STATE		0x03
#define USART_STORE_CONFIRM		0x04

#define USART_STATE_STORED		0x0E    // output command sent as a confirmation that state has been stored

/* Side state array length */
#define SIDE_STATE_DATA_LEN		14


#ifdef __cplusplus
 extern "C" {
#endif 

void handle_usart_command(uint8_t command, uint8_t data, uint8_t data_ct);

uint8_t get_command_data_len(uint8_t command);

#ifdef __cplusplus
}
#endif 

#endif
