#include "hardware.h"
#include "common.h"

void init_ports(void)
{
    // Initializing switches pins
    dir_out(SPA);
    dir_out(SPB);
    dir_out(SPC);
    dir_out(SPD);
    dir_out(SPE);
    dir_out(SPF);
    dir_out(SPG);

    dir_in(SI0);
    dir_in(SI1);
    dir_in(SI2);
    dir_in(SI3);
    dir_in(SI4);
    dir_in(SI5);

    // Software RESET pin
    dir_in(SRS);

    // Initializing color control pin
    dir_out(LED_COLOR);

#ifdef USART_DEBUG
    // For debug purposes
    dir_in(RX);
    dir_out(TX);
#endif

    // Reset LED color pin to 0
    res_pin(LED_COLOR);

    // Reset all swicthes pins to 1
    set_pin(SPA);
    set_pin(SPB);
    set_pin(SPC);
    set_pin(SPD);
    set_pin(SPE);
    set_pin(SPF);
    set_pin(SPG);

    // Enable pull-ups for all SI pins and for SRS.
    // PUD bit in MCUCR register by default is 0, meaning that all pull-ups are enabled by default.
    // For activating pull-ups PORTxn are used. They need to be set to 1.
    set_pin(SI0);
    set_pin(SI1);
    set_pin(SI2);
    set_pin(SI3);
    set_pin(SI4);
    set_pin(SI5);

    set_pin(SRS);

    // Reset all D pins to 0
#ifndef USART_DEBUG
    res_pin(PD0);  // RX
    res_pin(PD1);  // TX
#endif
    res_pin(PD3);
    res_pin(PD4);
    res_pin(PD5);
    res_pin(PD6);
}

void init_timer1(void)
{
    TCCR1B = _BV(CS11) | _BV(CS10); // set prescaller to 64
    TIMSK1 = _BV(TOIE1); // enable timer1 over-flow interrupt
    ENABLE_GLOBAL_INTERRUPTS(); // enable global interrupts
}


#ifdef USART_DEBUG

void USART_init(uint16_t ubrr)
{
    /* Set baud rate */
    UBRR0H = (uint8_t) (ubrr >> 8);
    UBRR0L = (uint8_t) ubrr;
    /* Enable double speed */
    UCSR0A = _BV(U2X0);
    /* Enable transmitter and receiver and 8-bit data format */
    UCSR0B = _BV(TXEN0) | _BV(RXEN0);
    /* Set frame format: 8data, 1stop bit, no parity bit */
    UCSR0C = _BV(UCSZ01) | _BV(UCSZ00);
}

void USART_transmit(uint8_t data)
{
    /* Since RX and TX may use the same wire (in previous schema edition), disable receiver */
    unset_bit(UCSR0B, RXEN0);
    /* Wait for empty transmit buffer */
    while ( !(UCSR0A & _BV(UDRE0)) );
    /* Clear TXC0 bit by writing one to its location */
    set_bit(UCSR0A, TXC0);
    
    /* Put data into buffer, sends the data */
    UDR0 = data;
    
    /* Wait till the data have been sent */
    while ( !(UCSR0A & _BV(TXC0)) );
    /* Then enable receiver */
    set_bit(UCSR0B, RXEN0);
}
void USART_transmit_16(uint16_t data)
{
    USART_transmit((uint8_t) data);
    USART_transmit((uint8_t) (data >> 8));
}

uint8_t USART_receive()
{
    while (!(UCSR0A & _BV(RXC0)));
    /* Get and return received data from buffer */
    return UDR0;
}

#endif
