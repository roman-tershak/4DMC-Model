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

#define SW_XL  0x1
#define SW_XR  0x2
#define SW_YL  0x4
#define SW_YR  0x8
#define SW_ZL  0x10
#define SW_ZR  0x20

#define SW_XLR_YLR  (SW_XL | SW_XR | SW_YL |SW_YR)

#define SW_SIDE_XL  0
#define SW_SIDE_XR  1
#define SW_SIDE_YL  2
#define SW_SIDE_YR  3
#define SW_SIDE_ZL  4
#define SW_SIDE_ZR  5
#define SW_SIDE_CF  6
#define SW_SIDE_CB  7

#define SW_SIDE_NUM 7

#ifdef __cplusplus
 extern "C" {
#endif 

typedef struct 
{
    uint8_t flags;
    uint8_t switches;
    uint8_t cycle_ct;
}
Switches_Side_State;


void init_driver(void);

ISR (TIMER1_OVF_vect);

#ifdef __cplusplus
}
#endif 

#endif
