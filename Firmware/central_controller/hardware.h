#ifndef H_HARDWARE
#define H_HARDWARE

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <util/delay.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define ISR_TIMEOUT 5208

/* Macros */
#define set_bit(REG, BIT) ((REG) |= _BV(BIT))
#define unset_bit(REG, BIT) ((REG) &= ~_BV(BIT))

#define set_pin(type) (type ## _PORT |= type ## _MASK)
#define res_pin(type) (type ## _PORT &= ~type ## _MASK)

#define read_pin(type) (type ## _PIN & type ## _MASK)
#define read_pins(type) (type ## _PIN & type ## _MASK)

#define dir_out(type) (type ## _DDR |= type ## _MASK)
#define dir_in(type) (type ## _DDR &= ~type ## _MASK)

/* Pin Definitions */
#define RX_PORT PORTD
#define RX_DDR DDRD
#define RX_MASK _BV(PORTD0)

#define TX_PORT PORTD
#define TX_DDR DDRD
#define TX_MASK _BV(PORTD1)


#define SPA_PORT PORTD
#define SPA_DDR DDRD
#define SPA_MASK _BV(PORTD2)

#define SPB_PORT PORTD
#define SPB_DDR DDRD
#define SPB_MASK _BV(PORTD3)

#define SPC_PORT PORTD
#define SPC_DDR DDRD
#define SPC_MASK _BV(PORTD4)

#define SPD_PORT PORTD
#define SPD_DDR DDRD
#define SPD_MASK _BV(PORTD5)

#define SPE_PORT PORTD
#define SPE_DDR DDRD
#define SPE_MASK _BV(PORTD6)

#define SPF_PORT PORTD
#define SPF_DDR DDRD
#define SPF_MASK _BV(PORTD7)


#define SI1_PIN PINC
#define SI1_DDR DDRC
#define SI1_MASK _BV(PORTC0)

#define SI2_PIN PINC
#define SI2_DDR DDRC
#define SI2_MASK _BV(PORTC1)

#define SI3_PIN PINC
#define SI3_DDR DDRC
#define SI3_MASK _BV(PORTC2)

#define SI4_PIN PINC
#define SI4_DDR DDRC
#define SI4_MASK _BV(PORTC3)

#define SI5_PIN PINC
#define SI5_DDR DDRC
#define SI5_MASK _BV(PORTC4)

#define SI6_PIN PINC
#define SI6_DDR DDRC
#define SI6_MASK _BV(PORTC5)

#define SI_PIN PINC
#define SI_MASK (SI1_MASK | SI2_MASK | SI3_MASK | SI4_MASK | SI5_MASK | SI6_MASK)

#ifdef __cplusplus
 extern "C" {
#endif

/* Hardware Functions */
void init_ports(void);
void init_timer1(void);
void USART_init(uint16_t ubrr);
void USART_transmit(uint8_t data);
uint8_t USART_receive(void);

#ifdef __cplusplus
}
#endif 

#endif
