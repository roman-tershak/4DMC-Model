#include "hardware.h"
#include "driver.h"

int main (void)
{
    init_ports();
    // Set baud rate to 115200, but maybe better to 76800 (25)
    USART_init(16);
    
    clear_buffer();
    // TODO remove fill_buffer
    fill_buffer(5, 50, 5);
    init_timer1(); //set up the timer and start the main interrupt

    for (;;);
    return 0;
}


