#include "hardware.h"

void init_ports(void)
{
	// Initializing switches pins
    dir_out(SPA);
    dir_out(SPB);
    dir_out(SPC);
    dir_out(SPD);
    dir_out(SPE);
    dir_out(SPF);

    dir_in(SI1);
    dir_in(SI2);
    dir_in(SI3);
    dir_in(SI4);
    dir_in(SI5);
    dir_in(SI6);

    // Software RESET pin
    dir_in(SRS);

    // Initializing side color control pins
    // TODO RX dir_out(SCA);
    // TODO TX dir_out(SCB);
    dir_out(SCC);
    dir_out(SCD);
    dir_out(SCE);
    dir_out(SCF);
    dir_out(SCG);
    // TODO SRS dir_out(SCH);

    // TODO For debug purposes, should be removed later
    dir_in(RX);
    dir_out(TX);

    // Reset all side color control pins to 1
    // TODO RX set_pin(SCA);
    // TODO TX set_pin(SCB);
    set_pin(SCC);
    set_pin(SCD);
    set_pin(SCE);
    set_pin(SCF);
    set_pin(SCG);
    // TODO SRS set_pin(SCH);

    // Reset all swicthes pins to 1
    set_pin(SPA);
    set_pin(SPB);
    set_pin(SPC);
    set_pin(SPD);
    set_pin(SPE);
    set_pin(SPF);
}

void init_timer1(void)
{
    // TODO select the correct mode
    TCCR1B = _BV(CS11) | _BV(CS10); // set prescaller to 64
    // With ISR_TIMEOUT = 5208 it will give 48 timer interrupts per second

    TIMSK1 = _BV(TOIE1); // enable timer1 over-flow interrupt
    sei(); // enable global interrupts
}

// TODO for debug purposes only
void USART_init(uint16_t ubrr)
{
    /*Set baud rate */
    UBRR0H = (uint8_t) (ubrr >> 8);
    UBRR0L = (uint8_t) ubrr;
    /* Enable double speed */
    UCSR0A = _BV(U2X0);
    /* Enable transmitter and receiver and 8-bit data format */
    UCSR0B = _BV(TXEN0) | _BV(RXEN0);
    /* Set frame format: 8data, 1stop bit, no parity bit */
    UCSR0C = _BV(UCSZ01) | _BV(UCSZ00);
}

// TODO for debug purposes only
void USART_transmit(uint8_t data)
{
    /* Since RX and TX use the same wire, disable receiver */
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

// TODO For debug purposes only
uint8_t USART_receive()
{
    while (!(UCSR0A & _BV(RXC0)));
    /* Get and return received data from buffer */
    return UDR0;
}


