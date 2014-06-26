#include "hardware.h"
#include "driver.h"

int main (void)
{
    
    init_ports();
    // TODO set baud rate to 115200
    USART_init(34);
    
    clear_buffer();
    init_timer1(); //set up the timer and start the main interrupt

    fill_buffer(50, 50, 50);
    display_buffer();

    for (;;);
    return 0;
}


