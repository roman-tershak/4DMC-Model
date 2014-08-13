#include "common.h"
#include "hardware.h"
#include "driver.h"
#include "central_logic.h"

int main (void)
{
    init_ports();

#ifdef USART_DEBUG
    // For debug purposes only
    // Set baud rate to 115200, but maybe better to 76800 (25 @16MHz)
    // USART_init(16);  // @16MHz
    USART_init(8);  // @8MHz
#endif
    
    init_driver();

    load_sides_states();
    
    init_timer1();
    
    for (;;);
    
    return 0;
}

