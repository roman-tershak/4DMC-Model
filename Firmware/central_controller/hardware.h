#ifndef H_HARDWARE
#define H_HARDWARE

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
#define SPA_MASK _BV(PORTB0)

#define SPB_PORT PORTB
#define SPB_DDR DDRB
#define SPB_MASK _BV(PORTB1)

#define SPC_PORT PORTB
#define SPC_DDR DDRB
#define SPC_MASK _BV(PORTB2)

#define SPD_PORT PORTB
#define SPD_DDR DDRB
#define SPD_MASK _BV(PORTB3)

#define SPE_PORT PORTB
#define SPE_DDR DDRB
#define SPE_MASK _BV(PORTB4)

#define SPF_PORT PORTB
#define SPF_DDR DDRB
#define SPF_MASK _BV(PORTB5)


// Switches input ports. They read a side switches states in the switches matrix
#define SI0_PIN PINC
#define SI0_DDR DDRC
#define SI0_MASK _BV(PORTC0)

#define SI1_PIN PINC
#define SI1_DDR DDRC
#define SI1_MASK _BV(PORTC1)

#define SI2_PIN PINC
#define SI2_DDR DDRC
#define SI2_MASK _BV(PORTC2)

#define SI3_PIN PINC
#define SI3_DDR DDRC
#define SI3_MASK _BV(PORTC3)

#define SI4_PIN PINC
#define SI4_DDR DDRC
#define SI4_MASK _BV(PORTC4)

#define SI5_PIN PINC
#define SI5_DDR DDRC
#define SI5_MASK _BV(PORTC5)

#define SI_PIN PINC
#define SI_MASK (SI0_MASK | SI1_MASK | SI2_MASK | SI3_MASK | SI4_MASK | SI5_MASK)


// Software Reset input pin
// TODO use PORTC6 (RESET) ?
#define SRS_PIN PIND
#define SRS_DDR DDRD
#define SRS_MASK _BV(PORTD7)


// Side color control outputs. They send control sequences of bytes to side LED's
#define SCA_PORT PORTD
#define SCA_DDR DDRD
#define SCA_MASK _BV(PORTD0)
#define SCA_PIN PORTD0

#define SCB_PORT PORTD
#define SCB_DDR DDRD
#define SCB_MASK _BV(PORTD1)
#define SCB_PIN PORTD1

#define SCC_PORT PORTD
#define SCC_DDR DDRD
#define SCC_MASK _BV(PORTD2)
#define SCC_PIN PORTD2

#define SCD_PORT PORTD
#define SCD_DDR DDRD
#define SCD_MASK _BV(PORTD3)
#define SCD_PIN PORTD3

#define SCE_PORT PORTD
#define SCE_DDR DDRD
#define SCE_MASK _BV(PORTD4)
#define SCE_PIN PORTD4

#define SCF_PORT PORTD
#define SCF_DDR DDRD
#define SCF_MASK _BV(PORTD5)
#define SCF_PIN PORTD5

#define SCG_PORT PORTD
#define SCG_DDR DDRD
#define SCG_MASK _BV(PORTD6)
#define SCG_PIN PORTD6

#define SCH_PORT PORTD
#define SCH_DDR DDRD
#define SCH_MASK _BV(PORTD7)
#define SCH_PIN PORTD7

#define SC_PORT PORTD
#define SC_DDR DDRD
#define SC_MASK (SCA_MASK | SCB_MASK | SCC_MASK | SCD_MASK | SCE_MASK | SCF_MASK | SCG_MASK | SCH_MASK)

/* Enable/disable interrupts */
#define ENABLE_GLOBAL_INTERRUPTS()  sei()
#define DISABLE_GLOBAL_INTERRUPTS()  cli()


#ifdef __cplusplus
 extern "C" {
#endif

typedef uint8_t port_pin_t;

/* Hardware Functions */
void init_ports(void);
void init_timer1(void);
port_pin_t get_side_led_pin(uint8_t side_num);


#ifdef USART_DEBUG
// For debug purposes only

#define USART_TRANSMIT_BYTE(data) USART_transmit(data)
#define USART_TRANSMIT_WORD(data) USART_transmit_16(data)

void USART_init(uint16_t ubrr);
void USART_transmit(uint8_t data);
void USART_transmit_16(uint16_t data);
uint8_t USART_receive(void);

#else

#define USART_TRANSMIT_BYTE(data)
#define USART_TRANSMIT_WORD(data)

#endif	// End of ifdef USART_DEBUG

#ifdef __cplusplus
}
#endif 

#endif
