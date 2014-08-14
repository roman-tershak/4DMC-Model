#ifndef H_STORAGE
#define H_STORAGE

#include <stdio.h>

#define EEPROM_SIZE                   1024

#define SAFETABLE_START_ADDRESS       0x0
#define SAFETABLE_LEN                 48

#define STATE_STORAGE_START_ADDRESS   (SAFETABLE_LEN)
#define STATE_STORAGE_LEN             (EEPROM_SIZE - SAFETABLE_LEN)
#define STATE_BANK_LEN                81
#define STATE_BANK_COUNT              (STATE_STORAGE_LEN / STATE_BANK_LEN)

// Used in the storage
#define STATE_DATA_STORAGE_LEN        (STATE_BANK_LEN)


#ifdef __cplusplus
 extern "C" {
#endif 

uint8_t is_saving(void);

void save_state(void);

uint8_t load_state(void);

#ifdef __cplusplus
}
#endif 

#endif
