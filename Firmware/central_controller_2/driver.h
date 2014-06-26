#ifndef H_DRIVER
#define H_DRIVER

#include "hardware.h"

#define DEBOUNCE_DELAY_1 2
#define DEBOUNCE_DELAY_2 10

#define READ_COMPLETE_SITE_STATE_CYCLES 2 // TODO

#define SW_XL  _BV(0)
#define SW_XR  _BV(1)
#define SW_YL  _BV(2)
#define SW_YR  _BV(3)
#define SW_ZL  _BV(4)
#define SW_ZR  _BV(5)

#define SW_SIDE_NUM 6

#define ENABLE_GLOBAL_INTERRUPTS  sei()
#define DISABLE_GLOBAL_INTERRUPTS  cli()

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
