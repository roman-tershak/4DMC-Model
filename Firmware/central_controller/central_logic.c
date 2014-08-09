#include "central_logic.h"
#include "common.h"
#include "driver.h"
#include "rotation_logic.h"
#include "storage.h"
#include "neopixel.h"

#define SLOWER_IDLE_CYCLE_SPAN 200

/* Side states checks */
#define IS_SIDE_ROTATING_OR_WAITING    (WAITING_FOR_ROTATION | ROTATING | WAITING_FOR_SAVING)
#define CAN_ROTATE  (SIDE_IDLE | WAITING_FOR_SAVING)
#define CANNOT_BE_SAVED (ROTATING)


static uint8_t can_start_rotation(uint8_t side_num, Side_State *state_ptr);
static uint8_t can_save(void);

static void reset_sides_colors(void);
static void reset_sides_states(void);
static uint8_t get_initial_color_for_side(uint8_t side_num);

static void handle_idle_cycle(uint8_t idle_cycle, uint8_t rotating);
static void move_faster(void);
static void move_slower(void);


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

static uint8_t faster = FALSE;
static uint8_t slower = FALSE;


void load_sides_states(void)
{
    if (!load_state())
        reset_sides_colors();

    reset_sides_states();
    sides_colors_changed();
}

void reset_cube(void)
{
    reset_sides_colors();
    reset_sides_states();
    sides_states[0].status = WAITING_FOR_SAVING;
    sides_colors_changed();
}

static void reset_sides_colors(void)
{
    uint8_t sn, c, color;
    uint8_t *colors;

    for (sn = 0; sn < SIDE_COUNT; sn++)
    {
        color = get_initial_color_for_side(sn);

        colors = sides_states[sn].colors;
        for (c = 0; c < SIDE_CUBES_COUNT; c++)
        {
            colors[c] = color;
        }
    }
}

static void reset_sides_states(void)
{
    uint8_t i;

    for (i = 0; i < SIDE_COUNT; i++)
    {
        sides_states[i].status = SIDE_IDLE;
    }
    
}

static uint8_t get_initial_color_for_side(uint8_t side_num)
{
    switch (side_num)
    {
        case SIDE_XL: return 0;
        case SIDE_XR: return 1;
        case SIDE_YL: return 2;
        case SIDE_YR: return 3;
        case SIDE_ZL: return 4;
        case SIDE_ZR: return 5;
        case SIDE_CF: return 6;
        case SIDE_CB: return 7;
        default: return 0;
    }
}

uint8_t rotation_notify(uint8_t sw_side_num, uint8_t direction)
{
    Side_State *state_ptr;

    if (direction == MOVE_NONE)
        return TRUE;

    state_ptr = &(sides_states[direction == MOVE_PERSP ? SIDE_CF : sw_side_num]);

    if (state_ptr->status & CAN_ROTATE)
    {
        state_ptr->cycle_ct = 0;
        state_ptr->rotation_func_ptr = get_rotation_func_ptr(sw_side_num, direction);
        state_ptr->status = WAITING_FOR_ROTATION;

        return TRUE;
    }
    else
    {
        // Cannot start rotation right now, need to rotate faster
        move_faster();

        return FALSE;
    }
}

void handle_cycle(void)
{
    Side_State *state_ptr;
    uint8_t side_num;
    uint8_t idle_cycle = TRUE, rotating = FALSE;

    for (side_num = 0; side_num < SIDE_CB; side_num++)
    {
        state_ptr = &(sides_states[side_num]);

        if (state_ptr->status & IS_SIDE_ROTATING_OR_WAITING)
        {
            switch (state_ptr->status)
            {
            case WAITING_FOR_ROTATION:

                if (can_start_rotation(side_num, state_ptr))
                {
                    state_ptr->status = ROTATING;
                    state_ptr->cycle_ct = 0;
                }
                else
                {
                    state_ptr->cycle_ct++;
                    // Cannot start rotation right now, need to rotate faster
                    move_faster();
                }
                break;

            case ROTATING:
                
                // rotation_logic.c
                rotation_cycle(side_num);
                rotating = TRUE;
                break;

            case WAITING_FOR_SAVING:

                // save_logic.c
                if (can_save())
                {
                    save_state();
                }
                break;
            }

            idle_cycle = FALSE;
        }
    }

    handle_idle_cycle(idle_cycle, rotating);
}

void rotation_done(uint8_t side_num)
{
    sides_states[side_num].status = WAITING_FOR_SAVING;
}

static uint8_t can_start_rotation(uint8_t side_num, Side_State *state_ptr)
{
    uint8_t sn, dependencies, status;

    // Check neighboring sides if they are not rotating
    dependencies = DEPENDENCY_MATRIX[side_num];
    for (sn = 0; sn < SIDE_CB; sn++)
    {
        // Is the side being checked dependant on 'sn'th side?
        // Is 'sn'th side rotating?
        if (dependencies | _BV(sn))  // if sn == side_num it will be false
        {
            status = sides_states[sn].status;
            // Cannot rotate this side (side_num'th) if there is...
            if (status == ROTATING)
                return FALSE; // ...at least one other side rotating
            if (status == WAITING_FOR_ROTATION && 
                state_ptr->cycle_ct < sides_states[sn].cycle_ct)
                return FALSE; // ...at least one other side waiting for rotation and has higher cycle_ct
        }
    }
    return TRUE;
}

static void handle_idle_cycle(uint8_t idle_cycle, uint8_t rotating)
{
    static uint16_t slower_cycle_counter = SLOWER_IDLE_CYCLE_SPAN;

    if (idle_cycle)
    {
        if (slower_cycle_counter > 0)
        {
            slower_cycle_counter--;
        }
        else
        {
            move_slower();
            slower_cycle_counter = SLOWER_IDLE_CYCLE_SPAN;
        }
    }
    else
    {
        slower_cycle_counter = SLOWER_IDLE_CYCLE_SPAN;
    }

    if (!rotating)
    {
        change_phase_cycle_counters(faster, slower);
        faster = FALSE;
        slower = FALSE;
    }
}

static void move_faster(void)
{
    faster = TRUE;
    USART_TRANSMIT_BYTE(0xfc);
}
static void move_slower(void)
{
    slower = TRUE;
    USART_TRANSMIT_BYTE(0xf5);
}


static uint8_t can_save(void)
{
    uint8_t i;

    if (is_saving())
        return FALSE;

    for (i = 0; i < SIDE_COUNT; i++)
    {
        if (sides_states[i].status & CANNOT_BE_SAVED)
            return FALSE;
    }
    return TRUE;
}

#ifdef USART_DEBUG
static uint8_t send_colors_to_usart(uint8_t sn, uint8_t *colors)
{
    uint8_t c, ok;

    ok = TRUE;
    USART_transmit(0xff);

    for (c = 0; c < SIDE_CUBES_COUNT; c++)
    {
        USART_transmit(colors[c]);
        if (colors[c] != sn)
            ok = FALSE;
    }
    return ok;
}
#endif

void sides_colors_changed(void)
{
    uint8_t sn, c;

#ifdef USART_DEBUG_
#ifndef DEBUG_COLOR_ADJUST
    uint8_t ok = TRUE;
#endif
#endif

    for (sn = 0; sn < SIDE_CB; sn++)
    {
        // Send new colors to side
        light_side_color(sn, sides_states[sn].colors);

#ifdef USART_DEBUG_
#ifndef DEBUG_COLOR_ADJUST
        if (!send_colors_to_usart(sn, sides_states[sn].colors))
            ok = FALSE;
#endif
#endif

    }

#ifdef USART_DEBUG_
#ifndef DEBUG_COLOR_ADJUST
    if (ok)
    {
        USART_transmit(0xff);
        USART_transmit(0x0C);
        USART_transmit(0xE1);
        USART_transmit(0xff);
    }
#endif
#endif

}


