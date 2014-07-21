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

    dir_in(SI0);
    dir_in(SI1);
    dir_in(SI2);
    dir_in(SI3);
    dir_in(SI4);
    dir_in(SI5);

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
    // TODO dir_out(SCH);

    // TODO For debug purposes, should be removed later
    dir_in(RX);
    dir_out(TX);

    // Reset all side color control pins to 1
    // TODO RX res_pin(SCA);
    // TODO TX res_pin(SCB);
    res_pin(SCC);
    res_pin(SCD);
    res_pin(SCE);
    res_pin(SCF);
    res_pin(SCG);
    // TODO SRS temp res_pin(SCH);

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

port_pin_t get_side_led_pin(uint8_t side_num)
{
    switch (side_num)
    {
        case SIDE_XL: return SCA_PIN;
        case SIDE_XR: return SCB_PIN;
        case SIDE_YL: return SCC_PIN;
        case SIDE_YR: return SCD_PIN;
        case SIDE_ZL: return SCE_PIN;
        case SIDE_ZR: return SCF_PIN;
        case SIDE_CF: return SCG_PIN;
        case SIDE_CB: return SCH_PIN;
    }
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
void USART_transmit_16(uint16_t data)
{
    USART_transmit((uint8_t) data);
    USART_transmit((uint8_t) (data >> 8));
}

// TODO For debug purposes only
uint8_t USART_receive()
{
    while (!(UCSR0A & _BV(RXC0)));
    /* Get and return received data from buffer */
    return UDR0;
}


