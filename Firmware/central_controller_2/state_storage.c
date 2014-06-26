#include "state_storage.h"

void read_side_state(uint8_t *side_state_dst, uint8_t bank_num)
{
    // Calculate the address of the side state being read
    uint8_t *bank_state_address = HIDDEN_STORAGE_START_ADDRESS + (bank_num * STATE_BANK_LEN);

    eeprom_read_block((void*) side_state_dst, (const void*) bank_state_address, STATE_BANK_LEN);
}

void store_side_state(uint8_t *side_state_src, uint8_t bank_num)
{
    // Calculate the address of the side state being writen
    uint8_t *bank_state_address = HIDDEN_STORAGE_START_ADDRESS + (bank_num * STATE_BANK_LEN);

    eeprom_write_block((const void*) side_state_src, (void*) bank_state_address, STATE_BANK_LEN);
}

uint8_t get_bank_num_for_sides(uint32_t record_num)
{
    return (uint8_t) (record_num % SIDES_BANK_COUNT);
}

uint8_t get_bank_num_for_hidden_side(uint32_t record_num)
{
    return (uint8_t) (record_num % HIDDEN_BANK_COUNT);
}

