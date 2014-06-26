#ifndef H_STATE_STORAGE
#define H_STATE_STORAGE

#include <avr/eeprom.h>

#define STATE_STORAGE_START_ADDRESS  0x0
#define STATE_BANK_LEN               14

#ifdef __cplusplus
 extern "C" {
#endif 

void read_side_state(uint8_t *side_state_dst, uint8_t bank_num);

void store_side_state(uint8_t *side_state_src, uint8_t bank_num);

#ifdef __cplusplus
}
#endif 

#endif
