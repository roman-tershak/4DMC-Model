#include "safetable.h"

uint32_t read_safetable_record_num(void)
{
    uint8_t i;
    uint32_t record_num_max;
    static uint32_t safetable[SAFETABLE_RECORD_COUNT];
    // Read the whole safetable
    eeprom_read_block((void*) safetable, (const void*) SAFETABLE_START_ADDRESS, SAFETABLE_LEN);
    // Find the max record num
    record_num_max = 0;
    for (i = 0; i < SAFETABLE_RECORD_COUNT; i++)
    {
        if (safetable[i] > record_num_max)
            record_num_max = safetable[i];
    }
    return record_num_max;
}

void store_safetable_record_num(uint32_t record_num)
{
    uint8_t *record_num_address = SAFETABLE_START_ADDRESS + 
            (record_num % SAFETABLE_RECORD_COUNT) * SAFETABLE_RECORD_LEN;
    // record_num's are writen sequentionally
    eeprom_write_dword((uint32_t*) record_num_address, record_num);
}

