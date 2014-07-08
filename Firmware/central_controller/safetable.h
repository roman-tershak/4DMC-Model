#ifndef H_SAFETABLE
#define H_SAFETABLE

#include <avr/eeprom.h>

#define SAFETABLE_START_ADDRESS  0x0
#define SAFETABLE_RECORD_LEN     4
#define SAFETABLE_RECORD_COUNT   12
#define SAFETABLE_LEN            (SAFETABLE_RECORD_LEN * SAFETABLE_RECORD_COUNT)

#ifdef __cplusplus
 extern "C" {
#endif 

uint32_t read_safetable_record_num(void);

void store_safetable_record_num(uint32_t record_num);

#ifdef __cplusplus
}
#endif 

#endif
