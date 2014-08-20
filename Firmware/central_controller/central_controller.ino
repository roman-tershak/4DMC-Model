#include "common.h"
#include "hardware.h"
#include "driver.h"
#include "central_logic.h"

int main (void)
{
    init_ports();

#ifdef USART_DEBUG
    // For debug purposes only
    USART_init(16);  // 57600 @8MHz
	/*
	 * 115200 - 16 @ 16MHz, 8 @ 8MHz; 76800 - 25 @ 16MHz
	 */
#endif
    
    init_driver();

    load_sides_states();
    
    init_timer1();
    
    for (;;);
    
    return 0;
}

