#ifndef H_STATE_STORAGE
#define H_STATE_STORAGE

#include <stdio.h>

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
