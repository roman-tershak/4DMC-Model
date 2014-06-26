#ifndef H_HARDWARE
#define H_HARDWARE

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

/* Macros */
#define set_bit(REG, BIT) ((REG) |= _BV(BIT))
#define unset_bit(REG, BIT) ((REG) &= ~_BV(BIT))

// Pin macros
#define set_pin(PIN) (PIN ## _PORT |= PIN ## _MASK)
#define res_pin(PIN) (PIN ## _PORT &= ~PIN ## _MASK)

#define dir_out(PIN) (PIN ## _DDR |= PIN ## _MASK)
#define dir_in(PIN) (PIN ## _DDR &= ~PIN ## _MASK)

/* Pin Definitions */
#define RX_PORT PORTD
#define RX_DDR DDRD
#define RX_MASK _BV(PORTD0)

#define TX_PORT PORTD
#define TX_DDR DDRD
#define TX_MASK _BV(PORTD1)


#define PA_PORT PORTC
#define PA_DDR DDRC
#define PA_MASK _BV(PORTC0)

#define PB_PORT PORTC
#define PB_DDR DDRC
#define PB_MASK _BV(PORTC1)

#define PC_PORT PORTC
#define PC_DDR DDRC
#define PC_MASK _BV(PORTC2)

#define PD_PORT PORTC
#define PD_DDR DDRC
#define PD_MASK _BV(PORTC3)

#define PE_PORT PORTC
#define PE_DDR DDRC
#define PE_MASK _BV(PORTC4)

#define PF_PORT PORTC
#define PF_DDR DDRC
#define PF_MASK _BV(PORTC5)

#define PG_PORT PORTD
#define PG_DDR DDRD
#define PG_MASK _BV(PORTD2)

#define PH_PORT PORTD
#define PH_DDR DDRD
#define PH_MASK _BV(PORTD3)

#define PI_PORT PORTD
#define PI_DDR DDRD
#define PI_MASK _BV(PORTD4)


#define PR1_PORT PORTD
#define PR1_DDR DDRD
#define PR1_MASK _BV(PORTD5)

#define PR2_PORT PORTD
#define PR2_DDR DDRD
#define PR2_MASK _BV(PORTD6)

#define PR3_PORT PORTD
#define PR3_DDR DDRD
#define PR3_MASK _BV(PORTD7)

#define PG1_PORT PORTB
#define PG1_DDR DDRB
#define PG1_MASK _BV(PORTB0)

#define PG2_PORT PORTB
#define PG2_DDR DDRB
#define PG2_MASK _BV(PORTB1)

#define PG3_PORT PORTB
#define PG3_DDR DDRB
#define PG3_MASK _BV(PORTB2)

#define PB1_PORT PORTB
#define PB1_DDR DDRB
#define PB1_MASK _BV(PORTB3)

#define PB2_PORT PORTB
#define PB2_DDR DDRB
#define PB2_MASK _BV(PORTB4)

#define PB3_PORT PORTB
#define PB3_DDR DDRB
#define PB3_MASK _BV(PORTB5)


#ifdef __cplusplus
 extern "C" {
#endif

/* Hardware Functions */
void init_ports(void);
void init_timer1(void);
void USART_init(uint16_t ubrr);
void USART_transmit(uint8_t data);

#ifdef __cplusplus
}
#endif 


#endif
