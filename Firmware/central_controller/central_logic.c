#include "central_logic.h"
#include "common.h"
#include "driver.h"
#include "rotation_logic.h"
#include "storage.h"
#include "neopixel.h"

#define SLOWER_IDLE_CYCLE_SPAN 200
#define SAVE_IDLE_CYCLE_SPAN   150

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


static const uint8_t DEPENDENCY_MATRIX[] = 
{
    (_BV(SIDE_YL) | _BV(SIDE_YR) | _BV(SIDE_ZL) | _BV(SIDE_ZR) | _BV(SIDE_CF) | _BV(SIDE_CB)), // For SIDE_XL
    (_BV(SIDE_YL) | _BV(SIDE_YR) | _BV(SIDE_ZL) | _BV(SIDE_ZR) | _BV(SIDE_CF) | _BV(SIDE_CB)), // For SIDE_XR
    (_BV(SIDE_XL) | _BV(SIDE_XR) | _BV(SIDE_ZL) | _BV(SIDE_ZR) | _BV(SIDE_CF) | _BV(SIDE_CB)), // For SIDE_YL
    (_BV(SIDE_XL) | _BV(SIDE_XR) | _BV(SIDE_ZL) | _BV(SIDE_ZR) | _BV(SIDE_CF) | _BV(SIDE_CB)), // For SIDE_YR
    (_BV(SIDE_XL) | _BV(SIDE_XR) | _BV(SIDE_YL) | _BV(SIDE_YR) | _BV(SIDE_CF) | _BV(SIDE_CB)), // For SIDE_ZL
    (_BV(SIDE_XL) | _BV(SIDE_XR) | _BV(SIDE_YL) | _BV(SIDE_YR) | _BV(SIDE_CF) | _BV(SIDE_CB)), // For SIDE_ZR
    (_BV(SIDE_XL) | _BV(SIDE_XR) | _BV(SIDE_YL) | _BV(SIDE_YR) | _BV(SIDE_ZL) | _BV(SIDE_ZR)), // For SIDE_CF
    (_BV(SIDE_XL) | _BV(SIDE_XR) | _BV(SIDE_YL) | _BV(SIDE_YR) | _BV(SIDE_ZL) | _BV(SIDE_ZR))  // For SIDE_CB
};


/*
 *   This is one side cubies local orientation. Each side has its own orientation of XYZ axes
 *
 *                                                                      v
 *                                                                      |
 *                                                                      |
 *                                                                     .*
 *                                                                   . * .
 *            ^Y                                                   .  * .
 *            |                                                  .   *     .
 *            |  2          3                                  .    *   .
 *            |                                              .   1 *    4 _ -*
 *            |                                            .      @  _-@-   *
 *            |6          7                              .      _*--    .  @ 3  .
 *            |  0          1                          .   _ -- *  .       .*
 *              -------------->X                     . _--   . *             *.   .
 *           /                                     ._-  .          *      *      .
 *          /  4          5                    <--*                    @ 2          .
 *         Z
 *                                                                                                  (side_zr)
 *                                                                                             y   x
 *                                                 _ 13                                         \ |
 *                                 ____    ----     **                                           \| 
 *                  6__  ------                    *                       (side_yr)        z<----
 *                 *                              *                                  x___
 *                *                              *    *                                  |\
 *               *                       _12    14                                       | \z
 *              *              7 _ ----   *   --*                                        y                     z  X   (side_xr)      x  (side_cb, inverted)
 *             5__  -----      *         * |           *                 (side_xl)              |Z              \ |                   \
 *             *              * |     __15                                        ___Y          |____Y           \|___Y           Y~___\
 *                           4_ -----     _11    *                               |\              \ (side_cf)                            |
 *                              8_  ---  | *            *                        | \Z             \X                                    |
 *              *            |  *         *                                      vX                                                     vZ
 *                             *      ___16       *  ___ 10                                             ^Y
 *                            3_ ---____    -----       *                                               |
 *               *      9__  -----                     *                                                |
 *                     *                           *  *                                        X         ---->X
 *                    *                              *                                         |         \  (side_yl)
 *                *  *                          ___ 1                                          |___Z      Z
 *                  *         _____    -----                                         (side_zl)  \
 *                 2_ -----                                                                      \Y
 *
 */
static const uint8_t LED_TO_STICKERS_MATRIX[STICKER_COUNT][2] = 
{
    {SIDE_ZL, 6}, {SIDE_CB, 4}, {SIDE_XR, 2}, {SIDE_YL, 5}, // corner 1
    {SIDE_ZL, 2}, {SIDE_XL, 5}, {SIDE_CB, 6}, {SIDE_YL, 4}, // corner 2
    {SIDE_YL, 0}, {SIDE_CF, 1}, {SIDE_ZL, 3}, {SIDE_XL, 7}, // corner 3
    {SIDE_YL, 2}, {SIDE_ZR, 4}, {SIDE_CF, 5}, {SIDE_XL, 6}, // corner 4
    {SIDE_XL, 4}, {SIDE_CB, 2}, {SIDE_YL, 6}, {SIDE_ZR, 5}, // corner 5
    {SIDE_XL, 0}, {SIDE_YR, 1}, {SIDE_CB, 3}, {SIDE_ZR, 7}, // corner 6
    {SIDE_YR, 5}, {SIDE_CF, 4}, {SIDE_ZR, 6}, {SIDE_XL, 2}, // corner 7
    {SIDE_, }, {SIDE_, }, {SIDE_, }, {SIDE_, }, // corner 8
    {SIDE_, }, {SIDE_, }, {SIDE_, }, {SIDE_, }, // corner 9
    {SIDE_, }, {SIDE_, }, {SIDE_, }, {SIDE_, }, // corner 10
    {SIDE_, }, {SIDE_, }, {SIDE_, }, {SIDE_, }, // corner 11
    {SIDE_, }, {SIDE_, }, {SIDE_, }, {SIDE_, }, // corner 12
    {SIDE_, }, {SIDE_, }, {SIDE_, }, {SIDE_, }, // corner 13
    {SIDE_, }, {SIDE_, }, {SIDE_, }, {SIDE_, }, // corner 14
    {SIDE_, }, {SIDE_, }, {SIDE_, }, {SIDE_, }, // corner 15
    {SIDE_, }, {SIDE_, }, {SIDE_, }, {SIDE_, }  // corner 16
};

volatile Side_State sides_states[SIDE_COUNT];

static uint8_t faster = FALSE;
static uint8_t slower = FALSE;
static uint16_t slower_cycle_counter = SLOWER_IDLE_CYCLE_SPAN;
static uint16_t save_cycle_counter = SAVE_IDLE_CYCLE_SPAN;


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
        case SIDE_XL: return BLUE;
        case SIDE_XR: return LIGHT_BLUE;
        case SIDE_YL: return GREEN;
        case SIDE_YR: return YELLOW;
        case SIDE_ZL: return ORANGE;
        case SIDE_ZR: return RED;
        case SIDE_CF: return PINK;
        case SIDE_CB: return PURPLE;
        default: return 0;
    }
}

uint8_t rotation_notify(uint8_t sw_side_num, uint8_t direction)
{
    Side_State *state_ptr;

    if (direction == MOVE_NONE)
        return TRUE;

    state_ptr = &(sides_states[sw_side_num]);

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
        faster = TRUE;
//      USART_TRANSMIT_BYTE(0xfc);

        return FALSE;
    }
}

void handle_cycle(void)
{
    Side_State *state_ptr;
    uint8_t side_num, idle_cycle, rotating;

    idle_cycle = TRUE;
    rotating = FALSE;

    for (side_num = 0; side_num < SIDE_COUNT; side_num++)
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
                    faster = TRUE;
//                  USART_TRANSMIT_BYTE(0xfc);
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
                    save_cycle_counter = 0;
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
    for (sn = 0; sn < SIDE_COUNT; sn++)
    {
        // Is the side being checked dependant on 'sn'th side?
        // Is 'sn'th side rotating?
        if (dependencies | _BV(sn))  // if sn == side_num it will be false
        {
            status = sides_states[sn].status;
            // Cannot rotate this side (side_num'th) if there is...
            if (status == ROTATING)
                return FALSE; // ...at least one neighboring side rotating
            if (status == WAITING_FOR_ROTATION && 
                state_ptr->cycle_ct < sides_states[sn].cycle_ct)
                return FALSE; // ...at least one other side waiting for rotation and has higher cycle_ct
        }
    }
    return TRUE;
}

static void handle_idle_cycle(uint8_t idle_cycle, uint8_t rotating)
{
    if (idle_cycle)
    {
        slower_cycle_counter++;
        if (slower_cycle_counter > SLOWER_IDLE_CYCLE_SPAN)
        {
            slower = TRUE;
//          USART_TRANSMIT_BYTE(0x51);
            slower_cycle_counter = 0;
        }
    }
    else
    {
        slower_cycle_counter = 0;
    }

    if (!rotating)
    {
        change_phase_cycle_counters(faster, slower);
        faster = FALSE;
        slower = FALSE;

        if (save_cycle_counter < SAVE_IDLE_CYCLE_SPAN) save_cycle_counter++;
    }
    else
    {
        save_cycle_counter = 0;
    }
}


static uint8_t can_save(void)
{
    uint8_t i;

    if (is_saving() || save_cycle_counter < SAVE_IDLE_CYCLE_SPAN)
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
    // In-memory buffer for all stickers colors
    static uint8_t color_buff[STICKER_COUNT];

#ifdef USART_DEBUG
#ifndef DEBUG_COLOR_ADJUST
    // uint8_t ok = TRUE;
#endif
#endif

    uint8_t sn, i, *buff_ptr, *led2st_ptr;
    uint8_t pin_mask = _BV(get_side_led_pin(0));

    buff_ptr = color_buff;
    led2st_ptr = LED_TO_STICKERS_MATRIX[0];

    for (i = 0; i < STICKER_COUNT; i++)
    {
        *buff_ptr++ = sides_states[*led2st_ptr++].colors[*led2st_ptr++];
    }

    light_color_buff(pin_mask, color_buff, STICKER_COUNT);


#ifdef USART_DEBUG
#ifndef DEBUG_COLOR_ADJUST
    for (sn = 0; sn < LIGHT_SIDE_COUNT; sn++)
    {
        // Send new colors to side
        light_side_color(sn, sides_states[sn].colors);

        // if (!send_colors_to_usart(sn, sides_states[sn].colors))
        //     ok = FALSE;

    }
    // if (ok)
    // {
    //     USART_transmit(0xff);
    //     USART_transmit(0x0C);
    //     USART_transmit(0xE1);
    //     USART_transmit(0xff);
    // }
#endif
#endif

}
