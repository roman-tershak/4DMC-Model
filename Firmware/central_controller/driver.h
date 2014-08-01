#ifndef H_DRIVER
#define H_DRIVER

#include "hardware.h"

#define ISR_TIMEOUT 5208
//#define ISR_TIMEOUT 3906
// With ISR_TIMEOUT = 5208 it will give 48 timer interrupts per second

#define DEBOUNCE_DELAY_1 2
#define DEBOUNCE_DELAY_2 10

#define READ_COMPLETE_SITE_STATE_CYCLES 12 // TODO

#define SW_XL  _BV(0)
#define SW_XR  _BV(1)
#define SW_YL  _BV(2)
#define SW_YR  _BV(3)
#define SW_ZL  _BV(4)
#define SW_ZR  _BV(5)

#define SW_SIDE_NUM 6

#ifdef __cplusplus
 extern "C" {
#endif 

typedef struct 
{
    uint8_t waiting_for_release;
    uint8_t switches;
    uint8_t cycle_ct;
} Switches_Side_State;


void init_driver(void);

ISR (TIMER1_OVF_vect);

#ifdef __cplusplus
}
#endif 

#endif
