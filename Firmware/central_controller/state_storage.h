#ifndef H_STATE_STORAGE
#define H_STATE_STORAGE

#include <avr/eeprom.h>
#include "safetable.h"

#define STATE_STORAGE_LEN             256
#define STATE_BANK_LEN                14
#define SIDES_BANK_COUNT              (STATE_STORAGE_LEN / STATE_BANK_LEN)

#define HIDDEN_STORAGE_START_ADDRESS  (SAFETABLE_LEN)
#define HIDDEN_STORAGE_LEN            STATE_STORAGE_LEN
#define HIDDEN_BANK_COUNT             (HIDDEN_STORAGE_LEN / STATE_BANK_LEN)

#ifdef __cplusplus
 extern "C" {
#endif 

void read_side_state(uint8_t *side_state_dst, uint8_t bank_num);

void store_side_state(uint8_t *side_state_src, uint8_t bank_num);

uint8_t get_bank_num_for_sides(uint32_t record_num);

uint8_t get_bank_num_for_hidden_side(uint32_t record_num);

#ifdef __cplusplus
}
#endif 

#endif
