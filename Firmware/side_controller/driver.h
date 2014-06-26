#ifndef H_DRIVER
#define H_DRIVER

#include "hardware.h"

/*---- USART definitions for Multi-Processor Slave Mode ----*/
/* The address of this controller is */
/* 4 higher bits */
#define MCU_ADDRESS             (1 << 4) // Each MCU should have a unique 4 bit address
#define MCU_ADDRESS_MASK        0xF0 // 4 higher bits are the MCU address
#define MCU_BROADCAST_ADDRESS   0xF0

/* Number of RGB LEDs */
#define SIDE_LED_COUNT      27

/* Max Color */
#define MAX_COLOR 	50

/* Number of different colors */
#define NUM_OF_COLORS       16

#define ISR_TIMEOUT 500 // TODO
//#define ISR_TIMEOUT 250
//#define OFF_DELAY 150
//#define OFF_DELAY 75

/* LED ports -> cube indexes */
#define R1 0
#define G1 1
#define B1 2
#define R2 3
#define G2 4
#define B2 5
#define R3 6
#define G3 7
#define B3 8

#define A 0
#define B 1
#define C 2
#define D 3
#define E 4
#define F 5
#define G 6
#define H 7
#define I 8

#ifdef __cplusplus
 extern "C" {
#endif 

/* This is the most used function. It handles the task of turning
the variable cube into the colors on the display. It is an
interrupt so you never call it. It is called when needed automatically. */
ISR (TIMER1_OVF_vect);

/* clear_buffer simply writes 0 to all of cube */
void clear_buffer (void);

void display_state (uint8_t *side_state);

void fill_buffer(uint8_t r, uint8_t g, uint8_t b);

#ifdef __cplusplus
}
#endif 

#endif
