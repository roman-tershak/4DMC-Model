#include "driver.h"

volatile uint8_t cube[9][9] = {{0}}; // actual display RAM
volatile uint8_t cube_buf[9][9] = {{0}}; // display buffer

const uint8_t COLOR_MATRIX[9][3] = 
{
    {0, 0, 0},      // switched off
    {30, 0, 0},     // red
    {0, 50, 0},     // green
    {0, 0, 25},     // blue
    {50, 50, 0},    // yellow
    {0, 20, 10},    // light blue
    {30, 00, 25},   // pink
    {50, 07, 02},   // orange ...
    {50, 50, 50}    // white
};

const uint8_t LED_MATRIX[27][4] =
{
    // The first layer in 3x3x3 cube
    {R1, G1, B1, A}, {R1, G1, B1, B}, {R1, G1, B1, C},
    {R2, G2, B2, A}, {R2, G2, B2, B}, {R2, G2, B2, C},
    {R3, G3, B3, A}, {R3, G3, B3, B}, {R3, G3, B3, C},


    // The second layer in 3x3x3 cube
    {R1, G1, B1, D}, {R1, G1, B1, E}, {R1, G1, B1, F},
    {R2, G2, B2, D}, {R2, G2, B2, E}, {R2, G2, B2, F},
    {R3, G3, B3, D}, {R3, G3, B3, E}, {R3, G3, B3, F},
    

    // The third layer in 3x3x3 cube
    {R1, G1, B1, G}, {R1, G1, B1, H}, {R1, G1, B1, I},
    {R2, G2, B2, G}, {R2, G2, B2, H}, {R2, G2, B2, I},
    {R3, G3, B3, G}, {R3, G3, B3, H}, {R3, G3, B3, I}
};


ISR (TIMER1_OVF_vect)
{
    static uint8_t ct = 0, state = 1, mode = 1;

    if (mode)
    {
        TCNT1 = (0xFFFF - ISR_TIMEOUT); //set the timer so it overflows faster

        if (cube[R1][ct] >= state) set_pin(PR1);
        if (cube[G1][ct] >= state) set_pin(PG1);
        if (cube[B1][ct] >= state) set_pin(PB1);
        
        if (cube[R2][ct] >= state) set_pin(PR2);
        if (cube[G2][ct] >= state) set_pin(PG2);
        if (cube[B2][ct] >= state) set_pin(PB2);
        
        if (cube[R3][ct] >= state) set_pin(PR3);
        if (cube[G3][ct] >= state) set_pin(PG3);
        if (cube[B3][ct] >= state) set_pin(PB3);

        //turn the cube back on
        switch (ct) {
        case 0: set_pin(PA); break;
        case 1: set_pin(PB); break;
        case 2: set_pin(PC); break;
        case 3: set_pin(PD); break;
        case 4: set_pin(PE); break;
        case 5: set_pin(PF); break;
        case 6: set_pin(PG); break;
        case 7: set_pin(PH); break;
        case 8: set_pin(PI); break;
        }

        mode = 0;
    }
    else
    {
        TCNT1 = (0xFFFF - OFF_DELAY);

        res_pin(PR1);
        res_pin(PR2);
        res_pin(PR3);
        res_pin(PG1);
        res_pin(PG2);
        res_pin(PG3);
        res_pin(PB1);
        res_pin(PB2);
        res_pin(PB3);

        res_pin(PA);
        res_pin(PB);
        res_pin(PC);
        res_pin(PD);
        res_pin(PE);
        res_pin(PF);
        res_pin(PG);
        res_pin(PH);
        res_pin(PI);
 
        ct++; //increase the row
        if (ct > 8) ct = 0; //reset
        
        state += 3; //increase the color
        if (state > MAX_COLOR) state -= MAX_COLOR; //compare color overflow

        mode = 1;
    }
}

/*ISR(USART_RX_vect)
{
    static uint8_t rx_ct = 0;

    // Move this into a separate function, possibly static?
    uint8_t data, command;
    uint8_t r, g, b, ri, gi, bi, col;
    
    data = UDR0;

    // Check the 9th bit if it is an address or data
    if (bit_is_set(UCSRB, RXB8))
    { // This is an address byte
        if ((data & MCU_ADDRESS_MASK) == MCU_ADDRESS)
        { // We are called
            // Get command (5 higher bits)
            command = data & MCU_COMMAND_MASK;
            rx_ct = get_command_data_len(command);
            if (rx_ct)
            { // we are going to accept data for the command
                // clear MPCM0 bit
                unset_bit(UCSR0A, MPCM0);
            }
        }
        else
        { // Not our address, ignore it
            // Set MPCM0 bit, just in case
            set_bit(UCSR0A, MPCM0);
            return;
        }
    }
    else
    { // Here we have command data
        // Read data to somewhere TODO


        
        if (--rx_ct == 0)
        { // All data have been read, set MPCM0 bit again
            set_bit(UCSR0A, MPCM0);
        }
    }
    // Get R, G, B from matrix by color code
    r = COLOR_MATRIX[data][0];
    g = COLOR_MATRIX[data][1];
    b = COLOR_MATRIX[data][2];
    // Get R, G, B LED indexes by LED number
    ri = LED_MATRIX[rx_ct][0];
    gi = LED_MATRIX[rx_ct][1];
    bi = LED_MATRIX[rx_ct][2];
    col = LED_MATRIX[rx_ct][3];
    // Fill buffer with the color
    cube_buf[ri][col] = r;
    cube_buf[gi][col] = g;
    cube_buf[bi][col] = b;
    
    rx_ct++;
    if (rx_ct >= PKG_LEN)
    {
        display_buffer();
        rx_ct = 0;
    }
}

uint8_t get_command_data_len(uint8_t command)
{
    switch (command)
    {
        case USART_INIT_LOAD:
        case USART_STORE_STATE: 
            return 1;
    }
}*/

ISR(USART_RX_vect)
{
    static uint8_t rx_byte_ct = 0;
    static unsigned char intencity = 0;
    
    intencity = UDR0;
    
    switch (rx_byte_ct++)
    {
    case 0:
        clear_buffer();
        cube_buf[R1][A] = intencity; 
        break;
    case 1: 
        cube_buf[G1][A] = intencity; 
        break;
    case 2: 
        cube_buf[B1][A] = intencity;
        break;
    case 3:
        cube_buf[R1][B] = intencity; 
        break;
    case 4: 
        cube_buf[G1][B] = intencity; 
        break;
    case 5: 
        cube_buf[B1][B] = intencity;
        break;
    case 6:
        cube_buf[R1][C] = intencity; 
        break;
    case 7: 
        cube_buf[G1][C] = intencity; 
        break;
    case 8: 
        cube_buf[B1][C] = intencity;
        display_buffer();  // no break here
    default:
        rx_byte_ct = 0;
        break;
    }
}

void clear_buffer(void)
{
    uint8_t i;
    for (i = 0; i < 9; i++)
    {
        cube_buf[R1][i] = 0;
        cube_buf[G1][i] = 0;
        cube_buf[B1][i] = 0;
        cube_buf[R2][i] = 0;
        cube_buf[G2][i] = 0;
        cube_buf[B2][i] = 0;
        cube_buf[R3][i] = 0;
        cube_buf[G3][i] = 0;
        cube_buf[B3][i] = 0;
    }
}

void fill_buffer(uint8_t r, uint8_t g, uint8_t b)
{
    uint8_t i;
    for (i = 0; i < 9; i++)
    {
        cube_buf[R1][i] = cube_buf[R2][i] = cube_buf[R3][i] = r;
        cube_buf[G1][i] = cube_buf[G2][i] = cube_buf[G3][i] = g;
        cube_buf[B1][i] = cube_buf[B2][i] = cube_buf[B3][i] = b;
    }
}

void display_buffer(void)
{
    uint8_t i;
    for (i = 0; i < 9; i++)
    {
        cube[R1][i] = cube_buf[R1][i];
        cube[G1][i] = cube_buf[G1][i];
        cube[B1][i] = cube_buf[B1][i];
        cube[R2][i] = cube_buf[R2][i];
        cube[G2][i] = cube_buf[G2][i];
        cube[B2][i] = cube_buf[B2][i];
        cube[R3][i] = cube_buf[R3][i];
        cube[G3][i] = cube_buf[G3][i];
        cube[B3][i] = cube_buf[B3][i];
    }
}
