#include "hardware.h"

void init_ports(void)
{
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

    dir_in(RX);
    dir_out(TX);

    // Reset all pins to 1
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

void USART_init(uint16_t ubrr)
{
    /*Set baud rate */
    UBRR0H = (uint8_t) (ubrr >> 8);
    UBRR0L = (uint8_t) ubrr;
    /* Enable double speed */
    UCSR0A = _BV(U2X0);
    /* Enable transmitter and receiver and 9-bit data format */
    UCSR0B = _BV(TXEN0) | _BV(RXEN0) | _BV(UCSZ02);
    /* Set frame format: 9data, 1stop bit, no parity bit */
    UCSR0C = _BV(UCSZ01) | _BV(UCSZ00); 
}
uint8_t txc_a, txc_b, txc_c; 
uint32_t txc_ct;
void USART_transmit(uint8_t flag, uint8_t data)
{
    /* Since RX and TX use the same wire, disable receiver */
    unset_bit(UCSR0B, RXEN0);
    /* Wait for empty transmit buffer */
    while ( !(UCSR0A & _BV(UDRE0)) );
    /* Clear TXC0 bit by writing one to its location */
    set_bit(UCSR0A, TXC0);
    
    /* Copy 9th bit to TXB8 */
    if (flag)
        UCSR0B |= _BV(TXB80);
    else
        UCSR0B &= ~_BV(TXB80);
    /* Put data into buffer, sends the data */
    UDR0 = data;
    
    /* Wait till the data have been sent */
    while ( !(UCSR0A & _BV(TXC0)) );
    /* Then enable receiver */
    set_bit(UCSR0B, RXEN0);
}

uint8_t USART_receive(uint32_t timeout)
{
    uint8_t dummy;

    do {
        if (UCSR0A & _BV(RXC0))
        {
            /* Read 9th bit */
            dummy = UCSR0B & _BV(RXB80); // Not really needed
            /* Get and return received data from buffer */
            return UDR0;
        }
    } while (timeout--);    // 0 - once, 1 - two times, max - really long

    return 0;
}


