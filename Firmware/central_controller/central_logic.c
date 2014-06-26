#include "central_logic.h"
#include "common.h"
#include "driver.h"
#include "mpcomm.h"
#include "rotation_logic.h"
#include "save_logic.h"
#include "utils.h"


/* Side states checks */
#define IS_SIDE_ROTATING_OR_WAITING    (WAITING_FOR_ROTATION | ROTATING | WAITING_FOR_SAVING)
#define CAN_START_ROTATION  (SIDE_IDLE | WAITING_FOR_SAVING)


static uint8_t can_side_rotate(uint8_t side_num, Side_State *state_ptr);
static void send_side_colors(uint8_t side_num, uint8_t *colors);


static const uint8_t DEPENDENCY_MATRIX[] = 
{
    (_BV(SIDE_YL) | _BV(SIDE_YR) | _BV(SIDE_ZL) | _BV(SIDE_ZR) | _BV(SIDE_CF) | _BV(SIDE_CB)), // For SIDE_XL
    (_BV(SIDE_YL) | _BV(SIDE_YR) | _BV(SIDE_ZL) | _BV(SIDE_ZR) | _BV(SIDE_CF) | _BV(SIDE_CB)), // For SIDE_XR
    (_BV(SIDE_XL) | _BV(SIDE_XR) | _BV(SIDE_ZL) | _BV(SIDE_ZR) | _BV(SIDE_CF) | _BV(SIDE_CB)), // For SIDE_YL
    (_BV(SIDE_XL) | _BV(SIDE_XR) | _BV(SIDE_ZL) | _BV(SIDE_ZR) | _BV(SIDE_CF) | _BV(SIDE_CB)), // For SIDE_YR
    (_BV(SIDE_XL) | _BV(SIDE_XR) | _BV(SIDE_YL) | _BV(SIDE_YR) | _BV(SIDE_CF) | _BV(SIDE_CB)), // For SIDE_ZL
    (_BV(SIDE_XL) | _BV(SIDE_XR) | _BV(SIDE_YL) | _BV(SIDE_YR) | _BV(SIDE_CF) | _BV(SIDE_CB)), // For SIDE_ZR
    (_BV(SIDE_XL) | _BV(SIDE_XR) | _BV(SIDE_YL) | _BV(SIDE_YR) | _BV(SIDE_ZL) | _BV(SIDE_ZR))  // For SIDE_CF
};


volatile Side_State sides_states[SIDE_COUNT];


uint8_t start_rotation(uint8_t sw_side_num, uint8_t direction)
{
    Side_State *state_ptr;

    if (direction == MOVE_NONE)
        return TRUE;

    state_ptr = &(sides_states[direction == MOVE_PERSP ? SIDE_CF : sw_side_num]);

    if (state_ptr->status & CAN_START_ROTATION)
    {
        state_ptr->cycle_ct = 0;
        state_ptr->rotation_func_ptr = get_rotation_func_ptr(sw_side_num, direction);
        state_ptr->status = WAITING_FOR_ROTATION;

        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

void handle_cycle(void)
{
    Side_State *state_ptr;
    uint8_t side_num;

    for (side_num = 0; side_num < SIDE_CB; side_num++)
    {
        state_ptr = &(sides_states[side_num]);

        if (state_ptr->status & IS_SIDE_ROTATING_OR_WAITING)
        {
            switch (state_ptr->status)
            {
            case WAITING_FOR_ROTATION:

                if (can_side_rotate(side_num, state_ptr))
                {
                    state_ptr->status = ROTATING;
                    state_ptr->cycle_ct = 0;
                }
                else
                {
                    state_ptr->cycle_ct++;
                }
                break;

            case ROTATING:
                
                // rotation_logic.c
                rotation_cycle(side_num, state_ptr);
                break;

            case WAITING_FOR_SAVING:

                // save_logic.c
                if (can_save())
                {
                    save_state();
                }
                break;
            }
        }
    }
}

static uint8_t can_side_rotate(uint8_t side_num, Side_State *state_ptr)
{
    uint8_t sn, dependencies;

    // Check neighboring sides if they are not rotating
    dependencies = DEPENDENCY_MATRIX[side_num];
    for (sn = 0; sn < SIDE_CB; sn++)
    {
        // Is the side being checked dependant on 'sn'th side?
        // Is 'sn'th side rotating?
        if (dependencies | _BV(sn))  // if sn == side_num it will be false
        {
            // Cannot rotate this (side_num'th) side if there is...
            if (sides_states[sn].status == ROTATING)
                return FALSE; // ...at least one other side is rotating
            if (sides_states[sn].status == WAITING_FOR_ROTATION && 
                state_ptr->cycle_ct < sides_states[sn].cycle_ct)
                return FALSE; // ...at least one other side is waiting for rotation and has higher cycle_ct
        }
    }
    return TRUE;
}

void notify_sides_changed(void)
{
    uint8_t sn;
    Side_State *state_ptr;

    for (sn = 0; sn < SIDE_CB; sn++)
    {
        state_ptr = &(sides_states[sn]);

        if (state_ptr->colors_changed)
        {
            DISABLE_GLOBAL_INTERRUPTS;
            // Send new colors to side controller
            send_side_colors(sn, state_ptr->colors);

            ENABLE_GLOBAL_INTERRUPTS;
            // Clear colors_changed
            state_ptr->colors_changed = FALSE;
        }
    }
}

/*
 * | 1 | 0 || 3 | 2 || 5 | 4 |
 */
static void send_side_colors(uint8_t side_num, uint8_t *colors)
{
    static uint8_t colors_packed[SIDE_STATE_DATA_LEN];

    pack_colors(colors, colors_packed);

    send_multi_byte_command(GET_MCU_ADDRESS_FROM_SIDE_NUM(side_num), USART_DISPLAY_COLORS, colors_packed, SIDE_STATE_DATA_LEN);
}

