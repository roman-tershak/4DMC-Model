#ifndef H_DRIVER
#define H_DRIVER

#include "hardware.h"
//#include "common.h"

/*---- USART definitions for Multi-Processor Slave Mode ----*/
/* The address of this controller is */
#define MCU_ADDRESS 		0x00 // Each MCU should have a unique 3 bit address
#define MCU_ADDRESS_MASK 	0x07 // 3 lower bits are the MCU address
#define MCU_COMMAND_MASK 	0xF8 // 3 lower bits are the MCU address

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

/* clear_buffer simply writes 0 to all of cube_buf */
void clear_buffer (void);

void fill_buffer (uint8_t r, uint8_t g, uint8_t b);

/* dispaly_buffer copies cube_buf to cube */
void display_buffer (void);

#ifdef __cplusplus
}
#endif 

#endif
