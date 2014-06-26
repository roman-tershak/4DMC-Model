#include "commands.h"
#include "state_storage.h"

static void handle_init_load_command(uint8_t data);
static void handle_display_colors_command(uint8_t data, uint8_t data_ct);
static void handle_store_state_command(uint8_t data);
static void handle_store_confirm_command(void);

static volatile uint8_t side_state[SIDE_STATE_DATA_LEN] = {0};
static volatile uint8_t side_state_stored = 0;

void handle_usart_command(uint8_t command, uint8_t data, uint8_t data_ct)
{
    switch (command)
    {
    case USART_INIT_LOAD:
        handle_init_load_command(data);
        break;
    case USART_DISPLAY_COLORS:
        handle_display_colors_command(data, data_ct);
        break;
    case USART_STORE_STATE:
        handle_store_state_command(data);
        break;
    case USART_STORE_CONFIRM:
        handle_store_confirm_command();
        break;
    }
}

uint8_t get_command_data_len(uint8_t command)
{
    switch (command)
    {
    case USART_INIT_LOAD:
    case USART_STORE_STATE:
        return 1;
    case USART_DISPLAY_COLORS:
        return SIDE_STATE_DATA_LEN;
    case USART_STORE_CONFIRM:
        return 0;
    default:
        return 0;
    }
}

static void handle_init_load_command(uint8_t data)
{
    uint8_t i;
    // Read data from storage by bank number
    read_side_state(side_state, data);
    // Display this state
    display_state(side_state);
    // Trasmit the state to the central MCU
    for (i = 0; i < SIDE_STATE_DATA_LEN; i++)
    {
        USART_transmit(side_state[i]);
    }
}

static void handle_display_colors_command(uint8_t data, uint8_t data_ct)
{
    // Accept data byte
    side_state[(data_ct - 1)] = data;
    // Is it the last data byte?
    if (data_ct >= SIDE_STATE_DATA_LEN)
    {
        display_state(side_state);
    }
}

static void handle_store_state_command(uint8_t data)
{
    static uint8_t side_state_copy[SIDE_STATE_DATA_LEN] = {0};
    uint8_t i;
    // State is not stored yet
    side_state_stored = 0;
    // Since writing to EEPROM takes long time, the main timer (timer1) must be enabled 
    // during this time in order to prevend blinking of LEDS
    // So make a copy of state and store it
    for (i = 0; i < SIDE_STATE_DATA_LEN; i++)
    {
        side_state_copy[i] = side_state[i];
    }
    sei();
    // Store state into storage (EEPROM) into the given bank
    store_side_state(side_state_copy, data);
    // Now State is stored
    side_state_stored = 1;
}

static void handle_store_confirm_command(void)
{
//    _delay_ms(5);
//fill_buffer(10, 0, 10);
    USART_transmit(USART_STATE_STORED | side_state_stored);
}

