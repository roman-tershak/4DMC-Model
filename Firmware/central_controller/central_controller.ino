#include "hardware.h"
#include "driver.h"
#include "central_logic.h"

int main (void)
{
    init_ports();
    
    init_driver();
    init_central_logic();
    
    load_sides_states();
    
    init_timer1();
    
    for (;;);
    
    return 0;
}

