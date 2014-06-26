#include "hardware.h"

void init_ports(void)
{
    dir_out(PR1);
    dir_out(PR2);
    dir_out(PR3);
    dir_out(PG1);
    dir_out(PG2);
    dir_out(PG3);
    dir_out(PB1);
    dir_out(PB2);
    dir_out(PB3);

    dir_out(PA);
    dir_out(PB);
    dir_out(PC);
    dir_out(PD);
    dir_out(PE);
    dir_out(PF);
    dir_out(PG);
    dir_out(PH);
    dir_out(PI);
    
    dir_in(RX);
    dir_out(TX);
}

void init_timer1(void)
{
    TCCR1B = _BV(CS10); // set no prescaller
    TIMSK1 = _BV(TOIE1); // enable timer1 over-flow interrupt
    sei(); // enable global interrupts
}

void USART_init(uint16_t ubrr)
{
    /*Set baud rate */
    UBRR0H = (uint8_t) (ubrr >> 8);
    UBRR0L = (uint8_t) ubrr;
    /* Enable double speed and Multi-Processor Slave Mode; 
       only address-frame bytes will be read */
    UCSR0A = _BV(U2X0) | _BV(MPCM0);
    /* Enable transmitter, receiver and RX interrupt and 9-bit data format */
    UCSR0B = _BV(TXEN0) | _BV(RXEN0) | _BV(RXCIE0) | _BV(UCSZ02);
    /* Set frame format: 9data, 1stop bit, no parity bit */
    UCSR0C = _BV(UCSZ01) | _BV(UCSZ00);
}

void USART_transmit(uint8_t data)
{
    /* Wait for empty transmit buffer */
    while ( !(UCSR0A & _BV(UDRE0)) );
    /* Clear TXC0 bit by writing one to its location */
    set_bit(UCSR0A, TXC0);
    
    /* Set dummy 9th bit in TXB8 to zero */
    unset_bit(UCSR0B, TXB80);
    /* Put data into buffer, sends the data */
    UDR0 = data;
    
    /* Wait till the data have been sent */
    while ( !(UCSR0A & _BV(TXC0)) );
}

