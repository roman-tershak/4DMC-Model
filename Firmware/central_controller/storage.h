#ifndef H_STORAGE_LOGIC
#define H_STORAGE_LOGIC

#include <stdio.h>

// Used in the storage
#define SIDE_STATE_DATA_LEN  14


#ifdef __cplusplus
 extern "C" {
#endif 

uint8_t can_save(void);

void save_state(void);

#ifdef __cplusplus
}
#endif 

#endif
