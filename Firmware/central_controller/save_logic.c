#include "save_logic.h"
#include "central_logic.h"
#include "common.h"
#include "mpcomm.h"
#include "safetable.h"
#include "state_storage.h"
#include "driver.h"
#include "utils.h"


#define CANNOT_BE_SAVED (ROTATING)


static uint8_t are_sides_stored(void);


extern volatile Side_State sides_states[SIDE_COUNT];

static volatile uint8_t is_saving_states = FALSE;


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

    static uint8_t back_side_copy_packed[SIDE_CUBES_COUNT];
    
    // Prevent saving in parallel
    is_saving_states = TRUE;
    // Get the current bank from the safetale
    record_num = read_safetable_record_num();
    record_num++;
    // Send broadcast command to all side controllers to save their state
    send_one_byte_command(MCU_BROADCAST_ADDRESS, USART_STORE_STATE, get_bank_num_for_sides(record_num));
    
    // Make a copy of the hidden side
    pack_colors(sides_states[SIDE_CB].colors, back_side_copy_packed);

    // 'Release' sides that were waiting for saving
    for (i = 0; i < SIDE_COUNT; i++)
    {
        if (sides_states[i].status == WAITING_FOR_SAVING)
            sides_states[i].status = SIDE_IDLE;
    }

    // Enable interrupts, because storing takes long time
    ENABLE_GLOBAL_INTERRUPTS;


    // Store state of the hidden side from the copy
    store_side_state(back_side_copy_packed, get_bank_num_for_hidden_side(record_num));
    
    // Receive aknowledge from all side controllers
    if (are_sides_stored())
    {
        // Make a record about successful storing into the safetable
        store_safetable_record_num(record_num);
    }
    // Unset 'is_saving' indicator
    is_saving_states = FALSE;
}

static uint8_t are_sides_stored(void)
{
    uint8_t sn, k, status;

    status = 0;

    for (k = 0; k < 2; k++)
    {
        _delay_ms(200);

        for (sn = 0; sn < SIDE_CB; sn++)
        {
            if ((status & _BV(sn)) == 0)
            { // Confirmation not received yet
                DISABLE_GLOBAL_INTERRUPTS;

                send_no_data_command(GET_MCU_ADDRESS_FROM_SIDE_NUM(sn), USART_STORE_CONFIRM);

                if (USART_receive(100000 /*TODO*/) == USART_STATE_STORED)
                    status |= _BV(sn);

                ENABLE_GLOBAL_INTERRUPTS;
            }
        }
        if (status == 0x7F)
            return TRUE;
    }
    return FALSE;
}
