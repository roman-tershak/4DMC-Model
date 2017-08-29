#ifndef H_DRIVER
#define H_DRIVER

#include "hardware.h"

#define ISR_TIMEOUT 1302 // @ 8MHz it will give ~100 interrupts per second
/* 
 * With ISR_TIMEOUT = 2604 @ 16MHz it will give ~100 timer interrupts per second
 * 5208 @ 16MHz will give 48 timer interrupts per second
 */

#define DEBOUNCE_DELAY_1  2
#define DEBOUNCE_DELAY_2  10

#define WAITING_FOR_RELEASE  1
#define DOUBLE_CLICK         2

#define READ_COMPLETE_SITE_STATE_CYCLES 16 // TODO

#define SW_XL  _BV(0)
#define SW_XR  _BV(1)
#define SW_YL  _BV(2)
#define SW_YR  _BV(3)
#define SW_ZL  _BV(4)
#define SW_ZR  _BV(5)

#define SW_SIDE_NUM 7

#ifdef __cplusplus
 extern "C" {
#endif 

typedef struct 
{
    uint8_t flags;
    uint8_t switches;
    uint8_t cycle_ct;
} Switches_Side_State;


void init_driver(void);

ISR (TIMER1_OVF_vect);

#ifdef __cplusplus
}
#endif 

#endif
