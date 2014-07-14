#ifndef H_STATE_STORAGE
#define H_STATE_STORAGE

#include <avr/eeprom.h>
#include "safetable.h"
#include "storage.h"

#define STATE_STORAGE_START_ADDRESS   (SAFETABLE_LEN)
#define STATE_STORAGE_LEN             (EEPROM_SIZE - SAFETABLE_LEN)
#define STATE_BANK_LEN                81
#define SIDES_BANK_COUNT              (STATE_STORAGE_LEN / STATE_BANK_LEN)


#ifdef __cplusplus
 extern "C" {
#endif 

void read_side_states(uint8_t *side_state_dst, uint8_t bank_num);

void store_side_states(uint8_t *side_state_src, uint8_t bank_num);

uint8_t get_bank_num_storage(uint32_t record_num);

#ifdef __cplusplus
}
#endif 

#endif
