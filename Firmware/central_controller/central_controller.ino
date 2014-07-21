#include "hardware.h"
#include "driver.h"
#include "central_logic.h"

int main (void)
{
    init_ports();
    // TODO For debug purposes only
    // Set baud rate to 115200, but maybe better to 76800 (25)
    USART_init(16);
    
    init_driver();

    init_central_logic();
    load_sides_states();
    //reset_cube();
    
    init_timer1();
    
    for (;;);
    
    return 0;
}

