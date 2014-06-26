#include "save_logic.h"
#include "central_logic.h"
#include "common.h"
#include "mpcomm.h"
#include "safetable.h"
#include "state_storage.h"
#include "driver.h"


#define CANNOT_BE_SAVED (ROTATING)


static uint8_t are_sides_stored(void);
static void copy_back_side_colors(uint8_t *colors_copy_dst);


extern volatile Side_State sides_states[SIDE_COUNT];

static volatile uint8_t is_saving_states = FALSE;

extern uint8_t txc_a, txc_b, txc_c; 
extern uint32_t txc_ct;

uint8_t can_save(void)
{
    uint8_t i;
    if (is_saving_states)
        return FALSE;

    for (i = 0; i < SIDE_COUNT; i++)
    {
        if (sides_states[i].status & CANNOT_BE_SAVED)
            return FALSE;
    }
    return TRUE;
}

void save_state(void)
{
    uint8_t i;
    uint32_t record_num;

    static uint8_t back_side_copy[SIDE_CUBES_COUNT];
    
    // Prevent saving in parallel
    is_saving_states = TRUE;
    // Get the current bank from the safetale
    record_num = read_safetable_record_num();
    record_num++;
    // Send broadcast command to all side controllers to save their state
    send_one_byte_command(MCU_BROADCAST_ADDRESS, USART_STORE_STATE, get_bank_num_for_sides(record_num));
    
    // Make a copy of the hidden side
    copy_back_side_colors(back_side_copy);

    // 'Release' sides that were waiting for saving
    for (i = 0; i < SIDE_COUNT; i++)
    {
        if (sides_states[i].status == WAITING_FOR_SAVING)
            sides_states[i].status = SIDE_IDLE;
    }

    // Enable interrupts, because storing takes long time
    ENABLE_GLOBAL_INTERRUPTS;


    // Store state of the hidden side from the copy
    store_side_state(back_side_copy, get_bank_num_for_hidden_side(record_num));
    
    // Receive aknowledge from all side controllers
    if (are_sides_stored())
    {
        // Make a record about successful storing into the safetable
        store_safetable_record_num(record_num);
    }
    // Unset 'is_saving' indicator
    is_saving_states = FALSE;
}

    const uint8_t colors_1[] = {0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0};
    const uint8_t colors_2[] = {0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7, 4, 3, 4, 3, 4, 3, 4, 3, 4, 3, 4};
/*    const uint8_t colors_3[] = {1, 5, 1, 5, 1, 5, 1, 5, 1, 5, 1, 5, 1, 5, 1, 5, 1, 5, 1, 5, 1, 5, 1, 5, 1, 5, 1};
    const uint8_t colors_4[] = {6, 7, 6, 7, 6, 7, 6, 7, 6, 7, 6, 7, 6, 7, 6, 7, 6, 7, 6, 7, 6, 7, 6, 7, 6, 7, 6};
    const uint8_t colors_5[] = {2, 7, 2, 7, 2, 7, 2, 7, 2, 7, 2, 7, 2, 7, 2, 7, 2, 7, 2, 7, 2, 7, 2, 7, 2, 7, 2};
    const uint8_t colors_6[] = {6, 7, 6, 7, 6, 7, 6, 7, 6, 7, 6, 7, 6, 7, 6, 7, 6, 7, 6, 7, 6, 7, 6, 7, 6, 7, 6};
    const uint8_t colors_7[] = {5, 3, 5, 3, 5, 3, 5, 3, 5, 3, 5, 3, 5, 3, 5, 3, 5, 3, 5, 3, 5, 3, 5, 3, 5, 3, 5};*/


static uint8_t are_sides_stored(void)
{
  static uint8_t ct = 0;
ct++;
    uint8_t i, k, res, st;

    res = 0;
    for (k = 0; k < 2; k++)
    {
//res = 0;
        for (i = 0; i < /*SIDE_CB*/ 2; i++)
        {
            if ((res & _BV(i)) == 0)
            { // Confirmation not received yet
                DISABLE_GLOBAL_INTERRUPTS;
//_delay_ms(10);
                send_no_data_command(GET_MCU_ADDRESS_FROM_SIDE_NUM(i), USART_STORE_CONFIRM);
//_delay_ms(10);
                st = USART_receive(100000 /*TODO 1 million is equal ~1 sec */);
                if (st == USART_STATE_STORED)
                    res |= _BV(i);

                ENABLE_GLOBAL_INTERRUPTS;
            }
        }
        if (res == /*0x7F*/0x3)
        {
            //USART_transmit(0, 0x5A);
            send_side_colors(1, colors_2);
            
            return TRUE;
        }
        _delay_ms(100);
            send_side_colors(0, colors_1);
    }
            send_side_colors(0, colors_1);
//            USART_transmit(0, 0xb4);
            //USART_transmit(0, (ct << 4) + k);
//            USART_transmit(0, 0xB4);
    return FALSE;
}

static void copy_back_side_colors(uint8_t *colors_copy_dst)
{
    uint8_t i;
    uint8_t *back_side_colors = sides_states[SIDE_CB].colors;

    for (i = 0; i < SIDE_CUBES_COUNT; i++)
    {
        colors_copy_dst[i] = back_side_colors[i];
    }
}

