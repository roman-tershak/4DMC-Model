#include "state_storage.h"
#include "storage.h"
#include <avr/eeprom.h>


void read_side_states(uint8_t *side_state_dst, uint8_t bank_num)
{
    // Calculate the address of the side state being read
    uint8_t *bank_state_address = STATE_STORAGE_START_ADDRESS + (bank_num * STATE_BANK_LEN);

    eeprom_read_block((void*) side_state_dst, (const void*) bank_state_address, STATE_BANK_LEN);
}

void store_side_states(uint8_t *side_state_src, uint8_t bank_num)
{
    // Calculate the address of the side state being writen
    uint8_t *bank_state_address = STATE_STORAGE_START_ADDRESS + (bank_num * STATE_BANK_LEN);

    eeprom_write_block((const void*) side_state_src, (void*) bank_state_address, STATE_BANK_LEN);
}

uint8_t get_bank_num_storage(uint32_t record_num)
{
    return (uint8_t) (record_num % STATE_BANK_COUNT);
}

