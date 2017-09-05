#ifndef H_HARDWARE
#define H_HARDWARE

// The Internal Calibrated RC Oscillator is used. This allows to increase a number of pins
// that can be used as IO pins, as well as to reduce a number of electronic parties.
#undef F_CPU
#define F_CPU 8000000

#include <avr/io.h>
#include <avr/interrupt.h>
#include "common.h"

/* Macros */
#define set_bit(REG, BIT) ((REG) |= _BV(BIT))
#define unset_bit(REG, BIT) ((REG) &= ~_BV(BIT))

#define set_bit_mask(REG, BIT_MASK) ((REG) |= (BIT_MASK))
#define unset_bit_mask(REG, BIT_MASK) ((REG) &= ~(BIT_MASK))

#define set_pin(type) (type ## _PORT |= type ## _MASK)
#define res_pin(type) (type ## _PORT &= ~type ## _MASK)

#define read_pin(type) (type ## _PIN & type ## _MASK)
#define read_pins(type) (type ## _PIN & type ## _MASK)

#define dir_out(type) (type ## _DDR |= type ## _MASK)
#define dir_in(type) (type ## _DDR &= ~type ## _MASK)

/* Pin Definitions */
#ifdef USART_DEBUG
// For debug purposes, should not be used in prod
#define RX_PORT PORTD
#define RX_DDR DDRD
#define RX_MASK _BV(PORTD0)

#define TX_PORT PORTD
#define TX_DDR DDRD
#define TX_MASK _BV(PORTD1)
#endif


// Switches output ports. They enable a side switches scan in the switches matrix
#define SPA_PORT PORTB
#define SPA_DDR DDRB
#define SPA_MASK _BV(PORTB0)  // B0

#define SPB_PORT PORTB
#define SPB_DDR DDRB
#define SPB_MASK _BV(PORTB1)  // B1

#define SPC_PORT PORTB
#define SPC_DDR DDRB
#define SPC_MASK _BV(PORTB2)  // B2

#define SPD_PORT PORTB
#define SPD_DDR DDRB
#define SPD_MASK _BV(PORTB3)  // B3

#define SPE_PORT PORTB
#define SPE_DDR DDRB
#define SPE_MASK _BV(PORTB4)  // B4

#define SPF_PORT PORTB
#define SPF_DDR DDRB
#define SPF_MASK _BV(PORTB5)  // B5

#define SPG_PORT PORTD
#define SPG_DDR DDRD
#define SPG_MASK _BV(PORTD7)  // D7


// Switches input ports. They read a side switches states in the switches matrix
#define SI0_PIN PINC
#define SI0_PORT PORTC
#define SI0_DDR DDRC
#define SI0_MASK _BV(PORTC0)  // C0, XL

#define SI1_PIN PINC
#define SI1_PORT PORTC
#define SI1_DDR DDRC
#define SI1_MASK _BV(PORTC1)  // C1, XR

#define SI2_PIN PINC
#define SI2_PORT PORTC
#define SI2_DDR DDRC
#define SI2_MASK _BV(PORTC2)  // C2, YL

#define SI3_PIN PINC
#define SI3_PORT PORTC
#define SI3_DDR DDRC
#define SI3_MASK _BV(PORTC3)  // C3, YR

#define SI4_PIN PINC
#define SI4_PORT PORTC
#define SI4_DDR DDRC
#define SI4_MASK _BV(PORTC4)  // C4, ZL

#define SI5_PIN PINC
#define SI5_PORT PORTC
#define SI5_DDR DDRC
#define SI5_MASK _BV(PORTC5)  // C5, ZR

#define SI_PIN PINC
#define SI_MASK (SI0_MASK | SI1_MASK | SI2_MASK | SI3_MASK | SI4_MASK | SI5_MASK)


// Software Reset input pin
#define SRS_PIN PINB
#define SRS_PORT PORTB
#define SRS_DDR DDRB
#define SRS_MASK _BV(PORTB7)  // B7


// Color control output pin. It sends control sequences of bytes to side LED's
#define LED_COLOR_PORT PORTD
#define LED_COLOR_DDR DDRD
#define LED_COLOR_MASK _BV(PORTD2)  // Choose one of D2(32), D3(1), D4(2), D5(9), D6(10)
#define LED_COLOR_PIN PORTD2

// PORT D pins.
#define PD0_PORT PORTD
#define PD0_DDR DDRD
#define PD0_MASK _BV(PORTD0)
#define PD0_PIN PORTD0

#define PD1_PORT PORTD
#define PD1_DDR DDRD
#define PD1_MASK _BV(PORTD1)
#define PD1_PIN PORTD1

// #define PD2_PORT PORTD
// #define PD2_DDR DDRD
// #define PD2_MASK _BV(PORTD2)
// #define PD2_PIN PORTD2

#define PD3_PORT PORTD
#define PD3_DDR DDRD
#define PD3_MASK _BV(PORTD3)
#define PD3_PIN PORTD3

#define PD4_PORT PORTD
#define PD4_DDR DDRD
#define PD4_MASK _BV(PORTD4)
#define PD4_PIN PORTD4

#define PD5_PORT PORTD
#define PD5_DDR DDRD
#define PD5_MASK _BV(PORTD5)
#define PD5_PIN PORTD5

#define PD6_PORT PORTD
#define PD6_DDR DDRD
#define PD6_MASK _BV(PORTD6)
#define PD6_PIN PORTD6

#define PD_PORT PORTD
#define PD_DDR DDRD
#define PD_MASK (PD0_MASK | PD1_MASK | PD3_MASK | PD4_MASK | PD5_MASK | PD6_MASK)
// #define PD_MASK (PD0_MASK | PD1_MASK | PD2_MASK | PD3_MASK | PD4_MASK | PD5_MASK | PD6_MASK)

/* Enable/disable interrupts */
#define ENABLE_GLOBAL_INTERRUPTS()  sei()
#define DISABLE_GLOBAL_INTERRUPTS()  cli()


#ifdef __cplusplus
 extern "C" {
#endif

/* Hardware Functions */
void init_ports(void);
void init_timer1(void);


#ifdef USART_DEBUG
// For debug purposes only

#define USART_TRANSMIT_BYTE(data) USART_transmit(data)
#define USART_TRANSMIT_WORD(data) USART_transmit_16(data)

void USART_init(uint16_t ubrr);
void USART_transmit(uint8_t data);
void USART_transmit_16(uint16_t data);
uint8_t USART_receive(void);

void USART_transmit_buff(uint8_t *buff, uint8_t len);

#else

#define USART_TRANSMIT_BYTE(data)
#define USART_TRANSMIT_WORD(data)

#endif	// End of ifdef USART_DEBUG

#ifdef __cplusplus
}
#endif 

#endif
