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
}

void init_timer1(void)
{
    TCCR1B = 1; // set no prescaller
    TIMSK1 = _BV(TOIE1); // enable timer1 over-flow interrupt
    sei(); // enable global interrupts
}

void USART_init(unsigned int ubrr)
{
    /*Set baud rate */
    UBRR0H = (unsigned char) (ubrr >> 8);
    UBRR0L = (unsigned char) ubrr;
    /* Enable double speed and Multi-Processor Slave Mode; 
       only address-frame bytes will be read */
    UCSR0A = _BV(U2X0) /*| _BV(MPCM0)*/;
    /* Enable transmitter, receiver and RX interrupt */
    UCSR0B = _BV(TXEN0) | _BV(RXEN0) | _BV(RXCIE0);
    /* Set frame format: 9data, 1stop bit, no parity bit */
    UCSR0C = /*_BV(UCSZ02) |*/ _BV(UCSZ01) | _BV(UCSZ00); 
}
