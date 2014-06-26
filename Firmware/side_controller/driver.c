#include "driver.h"
#include "commands.h"

const uint8_t COLOR_MATRIX[NUM_OF_COLORS][3] = 
{
    {30,  0,  0},   // red
    { 0, 25,  0},   // green
    { 0,  0, 25},   // blue
    {50, 50,  0},   // yellow
    //{ 0, 20, 10},   // light blue
    {0, 15, 5},
    {30,  0, 25},   // pink
    {50,  7,  2},   // orange ...
    {50, 50, 50},   // white
    {10,  0,  0},   // semi-red
    { 0, 20,  0},   // semi-green
    { 0,  0,  8},   // semi-blue
    {20, 20,  0},   // semi-yellow
    { 0,  7,  4},   // semi-light blue
    {10,  0,  8},   // semi-pink
    {20,  2,  1},   // semi-orange ...
    {20, 20, 20}    // semi-white
};

const uint8_t LED_MATRIX[SIDE_LED_COUNT][4] =
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


static volatile uint8_t cube[9][9] = {{0}}; // actual display RAM

ISR (TIMER1_OVF_vect)
{
    static uint8_t ct = 0, state = 1;

    TCNT1 = (0xFFFF - ISR_TIMEOUT); // set the timer so it overflows faster

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

    if (cube[R1][ct] >= state) set_pin(PR1);
    if (cube[G1][ct] >= state) set_pin(PG1);
    if (cube[B1][ct] >= state) set_pin(PB1);
    
    if (cube[R2][ct] >= state) set_pin(PR2);
    if (cube[G2][ct] >= state) set_pin(PG2);
    if (cube[B2][ct] >= state) set_pin(PB2);

    if (cube[R3][ct] >= state) set_pin(PR3);
    if (cube[G3][ct] >= state) set_pin(PG3);
    if (cube[B3][ct] >= state) set_pin(PB3);

    // turn the column in matrix
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

    ct++; // increase the row
    if (ct > 8) ct = 0; // reset
    
    state += 3; // increase the color
    if (state > MAX_COLOR) state -= MAX_COLOR;

}

ISR(USART_RX_vect)
{
    static uint8_t rx_ct = 0, command, command_data_len;
    //static uint8_t commbyte = 1;

    // Must be read before reading the low bits from UDRn
    uint8_t addr_flag = bit_is_set(UCSR0B, RXB80);
    uint8_t rx_byte = UDR0;
	
    uint8_t address;

    // Check the 9th bit if it is an address or data
    if (addr_flag /*commbyte*/)
    { // This is an address byte
	    address = rx_byte & MCU_ADDRESS_MASK;
        if (address == MCU_ADDRESS || address == MCU_BROADCAST_ADDRESS)
        { // We are called
            // Get command from address byte
            command = rx_byte & MCU_COMMAND_MASK;
            command_data_len = get_command_data_len(command);
            rx_ct = 0;

            if (command_data_len > 0)
            { // we are going to accept data for the command in subsequent isr calls
                // clear MPCM0 bit
                unset_bit(UCSR0A, MPCM0);
                //commbyte = 0;
            }
            else
            {
                // command without any data
                handle_usart_command(command, rx_byte, 0);
            }
        }
        else
        { // Not our address, ignore it
            // Set MPCM0 bit, just in case
            set_bit(UCSR0A, MPCM0);
  //          command = USART_NO_COMMAND;
            //commbyte = 1;
        }
    }
    else
    {
        if (++rx_ct >= command_data_len)
        { // All data have been read, set MPCM0 bit again
            set_bit(UCSR0A, MPCM0);
            //commbyte = 1;
//            command = USART_NO_COMMAND;
        }
        // Here we have command data
        handle_usart_command(command, rx_byte, rx_ct);
    }
}

void clear_buffer(void)
{
    uint8_t i;
    for (i = 0; i < 9; i++)
    {
        cube[R1][i] = 0;
        cube[G1][i] = 0;
        cube[B1][i] = 0;
        cube[R2][i] = 0;
        cube[G2][i] = 0;
        cube[B2][i] = 0;
        cube[R3][i] = 0;
        cube[G3][i] = 0;
        cube[B3][i] = 0;
    }
}

/*
 * | 1 | 0 || 3 | 2 || 5 | 4 |
 */
void display_state(uint8_t *side_state)
{
    uint8_t i, j, cc, ri, gi, bi, col;
    for (i = 0, j = 0; i < SIDE_LED_COUNT; i++)
    {
        cc = side_state[j];
        if ((i % 2) == 1)
        {
            cc >>= 4;
            j++;
        }
        cc &= 0x0F;
        
        // Get R, G, B LED indexes by LED number
        ri = LED_MATRIX[i][0];
        gi = LED_MATRIX[i][1];
        bi = LED_MATRIX[i][2];
        col = LED_MATRIX[i][3];
        // Get R, G, B intencity from matrix by color code
        // and fill buffer with the color
        cube[ri][col] = COLOR_MATRIX[cc][0];
        cube[gi][col] = COLOR_MATRIX[cc][1];
        cube[bi][col] = COLOR_MATRIX[cc][2];
    }
}

// TODO test function, should be removed later
void fill_buffer(uint8_t r, uint8_t g, uint8_t b)
{
    uint8_t i;
    for (i = 0; i < 9; i++)
    {
        cube[R1][i] = cube[R2][i] = cube[R3][i] = r;
        cube[G1][i] = cube[G2][i] = cube[G3][i] = g;
        cube[B1][i] = cube[B2][i] = cube[B3][i] = b;
    }
}


