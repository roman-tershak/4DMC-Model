#include "central_logic.h"
#include "common.h"
#include "driver.h"
#include "mpcomm.h"
#include "rotation_logic.h"
#include "save_logic.h"


/* Side states checks */
#define IS_SIDE_ROTATING_OR_WAITING    (WAITING_FOR_ROTATION | ROTATING | WAITING_FOR_SAVING)
#define CAN_START_ROTATION  (SIDE_IDLE | WAITING_FOR_SAVING)


static uint8_t can_side_rotate(uint8_t side_num);
//static void send_side_colors(uint8_t side_num, uint8_t *colors);


static const uint8_t DEPENDENCY_MATRIX[] = 
{
    (_BV(SIDE_YL) | _BV(SIDE_YR) | _BV(SIDE_ZL) | _BV(SIDE_ZR) | _BV(SIDE_CF) | _BV(SIDE_CB)), // For SIDE_XL
    (_BV(SIDE_YL) | _BV(SIDE_YR) | _BV(SIDE_ZL) | _BV(SIDE_ZR) | _BV(SIDE_CF) | _BV(SIDE_CB)), // For SIDE_XR
    (_BV(SIDE_XL) | _BV(SIDE_XR) | _BV(SIDE_ZL) | _BV(SIDE_ZR) | _BV(SIDE_CF) | _BV(SIDE_CB)), // For SIDE_YL
    (_BV(SIDE_XL) | _BV(SIDE_XR) | _BV(SIDE_ZL) | _BV(SIDE_ZR) | _BV(SIDE_CF) | _BV(SIDE_CB)), // For SIDE_YR
    (_BV(SIDE_XL) | _BV(SIDE_XR) | _BV(SIDE_YL) | _BV(SIDE_YR) | _BV(SIDE_CF) | _BV(SIDE_CB)), // For SIDE_ZL
    (_BV(SIDE_XL) | _BV(SIDE_XR) | _BV(SIDE_YL) | _BV(SIDE_YR) | _BV(SIDE_CF) | _BV(SIDE_CB)), // For SIDE_ZR
    0,
    0
};


volatile Side_State sides_states[SIDE_COUNT];


uint8_t is_side_rotating_or_waiting(uint8_t side_num)
{
    return (sides_states[side_num].status & IS_SIDE_ROTATING_OR_WAITING);
}

uint8_t start_rotation(uint8_t side_num, uint8_t direction)
{
    Side_State *state_ptr = &(sides_states[side_num]);

    if (state_ptr->status & CAN_START_ROTATION)
    {
        state_ptr->cycle_ct = 0;
        state_ptr->rotation_func_ptr = get_rotation_func_ptr(direction);
        state_ptr->phase_len = get_rotation_phase_len(direction);
        state_ptr->status = WAITING_FOR_ROTATION;

        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

void handle_cycle(uint8_t side_num)
{
    Side_State *state_ptr = &(sides_states[side_num]);

    switch (state_ptr->status)
    {
    case WAITING_FOR_ROTATION:

        if (can_side_rotate(side_num))
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

void notify_sides_changed(void)
{
    uint8_t i;
    Side_State *state_ptr;

    for (i = 0; i < SIDE_CB; i++)
    {
        state_ptr = &(sides_states[i]);

        if (state_ptr->colors_changed)
        {
            DISABLE_GLOBAL_INTERRUPTS;
            // Send new colors to side controller
            send_side_colors(i, state_ptr->colors);

            ENABLE_GLOBAL_INTERRUPTS;
            // Clear colors_changed
            state_ptr->colors_changed = FALSE;
        }
    }
}

static uint8_t can_side_rotate(uint8_t side_num)
{
    uint8_t i, dependencies;

    // Check neighboring sides if they are not rotating
    dependencies = DEPENDENCY_MATRIX[side_num];
    for (i = 0; i < SIDE_COUNT; i++)
    {
        // Is the side being checked dependant on 'i'th side?
        // Is 'i'th side rotating?
        if (dependencies | _BV(i))  // if i == side_num it will be false
        {
            // Cannot rotate this (side_num'th) side if there is...
            if (sides_states[i].status == ROTATING)
                return FALSE; // ...at least one other side is rotating
            if (sides_states[i].status == WAITING_FOR_ROTATION && 
                sides_states[side_num].cycle_ct < sides_states[i].cycle_ct)
                return FALSE; // ...at least one other side is waiting for rotation and has higher cycle_ct
        }
    }
    return TRUE;
}

/*
 * | 1 | 0 || 3 | 2 || 5 | 4 |
 */
/*static */void send_side_colors(uint8_t side_num, uint8_t *colors)
{
    uint8_t i, j;
    static uint8_t colors_packed[SIDE_STATE_LEN];

    for (i = 0, j = 0; i < SIDE_CUBES_COUNT; i++)
    {
        if (i % 2)
        {
            colors_packed[j] |= (colors[i] << 4);
            j++;
        }
        else
        {
            colors_packed[j] = colors[i];
        }
    }
    send_multi_byte_command(GET_MCU_ADDRESS_FROM_SIDE_NUM(side_num), USART_DISPLAY_COLORS, colors_packed, SIDE_STATE_LEN);
}

