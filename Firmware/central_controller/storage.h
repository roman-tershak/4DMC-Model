#ifndef H_STORAGE_LOGIC
#define H_STORAGE_LOGIC

#include <stdio.h>

#define EEPROM_SIZE                   1024

// Used in the storage
#define STATE_DATA_STORAGE_LEN        81


#ifdef __cplusplus
 extern "C" {
#endif 

uint8_t can_save(void);

void save_state(void);

void load_state(void);

#ifdef __cplusplus
}
#endif 

#endif
