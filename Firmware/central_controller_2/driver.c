#include "driver.h"
#include "common.h"
#include "rotation_logic.h"

static uint8_t get_rotation_dir(uint8_t switches);

static uint8_t read_switches_debounced();
static uint8_t read_switches();

static void set_switch_pin(uint8_t pin);
static void reset_switches_pins(void);

/*                     (SW_YR)
 *        
 *                       ^ Y
 *       CENTRE          *
 *           (SW_ZL)     *        _ ->X (SW_XR)
 *                  -*_  *  _ -*
 *                    _ -*_
 *           (SW_XL)*    *  -*_
 *                       *      ->Z (SW_ZR)
 *                     (SW_YL)
 */
static const uint8_t SWITCH_ROTATION_MATRIX[64] = 
{
    /* 0x00                                                */   (MOVE_NONE),
    /* 0x01                                          SW_XL */   (ROTATION_Y | DIR_CCW),     // M
    /* 0x02                                  SW_XR         */   (ROTATION_Y | DIR_CW),      // M
    /* 0x03                                  SW_XR | SW_XL */   (MOVE_PERSP),
    /* 0x04                          SW_YL                 */   (ROTATION_X | DIR_CW),      // M
    /* 0x05                          SW_YL         | SW_XL */   (ROTATION_X_Y | DIR_CW),    // S
    /* 0x06                          SW_YL | SW_XR         */   (ROTATION_XY | DIR_CW),     // S
    /* 0x07                          SW_YL | SW_XR | SW_XL */   (ROTATION_X | DIR_CW),      // M2
    /* 0x08                  SW_YR                         */   (ROTATION_X | DIR_CCW),     // M
    /* 0x09                  SW_YR                 | SW_XL */   (ROTATION_XY | DIR_CCW),    // S
    /* 0x0A                  SW_YR         | SW_XR         */   (ROTATION_X_Y | DIR_CCW),   // S
    /* 0x0B                  SW_YR         | SW_XR | SW_XL */   (ROTATION_X | DIR_CCW),     // M2
    /* 0x0C                  SW_YR | SW_YL                 */   (MOVE_PERSP),
    /* 0x0D                  SW_YR | SW_YL         | SW_XL */   (ROTATION_Y | DIR_CCW),     // M2
    /* 0x0E                  SW_YR | SW_YL | SW_XR         */   (ROTATION_Y | DIR_CW),      // M2
    /* 0x0F                  SW_YR | SW_YL | SW_XR | SW_XL */   (MOVE_PERSP),
    /* 0x10          SW_ZL |                               */   (ROTATION_Z | DIR_CCW),     // M
    /* 0x11          SW_ZL |                         SW_XL */   (ROTATION_YZ | DIR_CCW),    // S
    /* 0x12          SW_ZL |                 SW_XR         */   (ROTATION_Y_Z | DIR_CW),    // S
    /* 0x13          SW_ZL |                 SW_XR | SW_XL */   (MOVE_PERSP),
    /* 0x14          SW_ZL |         SW_YL                 */   (ROTATION_X_Z | DIR_CW),    // S
    /* 0x15          SW_ZL |         SW_YL         | SW_XL */   (ROTATION_X_Y_Z | DIR_CW),  // V
    /* 0x16          SW_ZL |         SW_YL | SW_XR         */   (ROTATION_XY_Z | DIR_CW),   // V
    /* 0x17          SW_ZL |         SW_YL | SW_XR | SW_XL */   (ROTATION_X_Z | DIR_CW),    // S2
    /* 0x18          SW_ZL | SW_YR                         */   (ROTATION_XZ | DIR_CCW),    // S
    /* 0x19          SW_ZL | SW_YR                 | SW_XL */   (ROTATION_XYZ | DIR_CCW),   // V
    /* 0x1A          SW_ZL | SW_YR         | SW_XR         */   (ROTATION_X_YZ | DIR_CCW),  // V
    /* 0x1B          SW_ZL | SW_YR         | SW_XR | SW_XL */   (ROTATION_XZ | DIR_CCW),    // S2
    /* 0x1C          SW_ZL | SW_YR | SW_YL                 */   (MOVE_PERSP),
    /* 0x1D          SW_ZL | SW_YR | SW_YL         | SW_XL */   (ROTATION_YZ | DIR_CCW),    // S2
    /* 0x1E          SW_ZL | SW_YR | SW_YL | SW_XR         */   (ROTATION_Y_Z | DIR_CW),    // S2
    /* 0x1F          SW_ZL | SW_YR | SW_YL | SW_XR | SW_XL */   (MOVE_PERSP),
    /* 0x20  SW_ZR |                                       */   (ROTATION_Z | DIR_CW),      // M
    /* 0x21  SW_ZR |                                 SW_XL */   (ROTATION_Y_Z | DIR_CCW),   // S
    /* 0x22  SW_ZR |                         SW_XR         */   (ROTATION_YZ | DIR_CW),     // S
    /* 0x23  SW_ZR |                         SW_XR | SW_XL */   (MOVE_PERSP),
    /* 0x24  SW_ZR |                 SW_YL                 */   (ROTATION_XZ | DIR_CW),     // S
    /* 0x25  SW_ZR |                 SW_YL         | SW_XL */   (ROTATION_X_YZ | DIR_CW),   // V
    /* 0x26  SW_ZR |                 SW_YL | SW_XR         */   (ROTATION_XYZ | DIR_CW),    // V
    /* 0x27  SW_ZR |                 SW_YL | SW_XR | SW_XL */   (ROTATION_XZ | DIR_CW),     // S2
    /* 0x28  SW_ZR |         SW_YR                         */   (ROTATION_X_Z | DIR_CCW),   // S
    /* 0x29  SW_ZR |         SW_YR                 | SW_XL */   (ROTATION_XY_Z | DIR_CCW),  // V
    /* 0x2A  SW_ZR |         SW_YR         | SW_XR         */   (ROTATION_X_Y_Z | DIR_CCW), // V
    /* 0x2B  SW_ZR |         SW_YR         | SW_XR | SW_XL */   (ROTATION_X_Z | DIR_CCW),   // S2
    /* 0x2C  SW_ZR |         SW_YR | SW_YL                 */   (MOVE_PERSP),
    /* 0x2D  SW_ZR |         SW_YR | SW_YL         | SW_XL */   (ROTATION_Y_Z | DIR_CCW),   // S2
    /* 0x2E  SW_ZR |         SW_YR | SW_YL | SW_XR         */   (ROTATION_YZ | DIR_CW),     // S2
    /* 0x2F  SW_ZR |         SW_YR | SW_YL | SW_XR | SW_XL */   (MOVE_PERSP),
    /* 0x30  SW_ZR | SW_ZL |                               */   (MOVE_NONE),
    /* 0x31  SW_ZR | SW_ZL |                         SW_XL */   (MOVE_NONE),
    /* 0x32  SW_ZR | SW_ZL |                 SW_XR         */   (MOVE_NONE),
    /* 0x33  SW_ZR | SW_ZL |                 SW_XR | SW_XL */   (MOVE_NONE),
    /* 0x34  SW_ZR | SW_ZL |         SW_YL                 */   (MOVE_NONE),
    /* 0x35  SW_ZR | SW_ZL |         SW_YL         | SW_XL */   (MOVE_NONE),
    /* 0x36  SW_ZR | SW_ZL |         SW_YL | SW_XR         */   (MOVE_NONE),
    /* 0x37  SW_ZR | SW_ZL |         SW_YL | SW_XR | SW_XL */   (MOVE_NONE),
    /* 0x38  SW_ZR | SW_ZL | SW_YR                         */   (MOVE_NONE),
    /* 0x39  SW_ZR | SW_ZL | SW_YR                 | SW_XL */   (MOVE_NONE),
    /* 0x3A  SW_ZR | SW_ZL | SW_YR         | SW_XR         */   (MOVE_NONE),
    /* 0x3B  SW_ZR | SW_ZL | SW_YR         | SW_XR | SW_XL */   (MOVE_NONE),
    /* 0x3C  SW_ZR | SW_ZL | SW_YR | SW_YL                 */   (MOVE_NONE),
    /* 0x3D  SW_ZR | SW_ZL | SW_YR | SW_YL         | SW_XL */   (MOVE_NONE),
    /* 0x3E  SW_ZR | SW_ZL | SW_YR | SW_YL | SW_XR         */   (MOVE_NONE),
    /* 0x3F  SW_ZR | SW_ZL | SW_YR | SW_YL | SW_XR | SW_XL */   (MOVE_NONE),
};


volatile Switches_Side_State switches_side_states[SW_SIDE_NUM] = {0};


void init_driver(void)
{
    uint8_t i;
    Switches_Side_State *switches_side_state_ptr;
    for (i = 0; i < SW_SIDE_NUM; i++)
    {
        switches_side_state_ptr = &(switches_side_states[i]);

        switches_side_state_ptr->waiting_for_release = FALSE;
        switches_side_state_ptr->switches = 0;
        switches_side_state_ptr->cycle_ct = 0;
    }
}

ISR (TIMER1_OVF_vect)
{
    static uint8_t ct = 0;
    Switches_Side_State *side_state_ptr;

    TCNT1 = (0xFFFF - ISR_TIMEOUT); // set the timeout

    side_state_ptr = &(switches_side_states[ct]);

    if (side_state_ptr->waiting_for_release == FALSE)
    {
        if (side_state_ptr->cycle_ct < READ_COMPLETE_SITE_STATE_CYCLES)
        {
            set_switch_pin(ct);
            // Read switches state for the entire side
            side_state_ptr->switches |= read_switches_debounced();
            if (side_state_ptr->switches)
            {
                // If pressed, then increase the counter. Untill it reaches max
                // gather all pressed side switches since they will not likely be
                // pressed exactly in the same very moment.
                side_state_ptr->cycle_ct++;
            }
            reset_switches_pins();
        }
        else
        {
            // The wait period for switches ended, now start rotation
            if (start_rotation(ct, get_rotation_dir(side_state_ptr->switches)))
            {
                // Start waiting for switch release
                side_state_ptr->waiting_for_release = TRUE;
            }
        }
    }
    else
    {
      //USART_transmit(0, '!');
        // Here we are waiting for the previously pressed switches to be released
        set_switch_pin(ct);
        // Read switches state for the entire side and check if they are released
        if (read_switches_debounced() == 0)
        {
            side_state_ptr->waiting_for_release = FALSE;
            side_state_ptr->switches = 0;
            side_state_ptr->cycle_ct = 0;
        }
        reset_switches_pins();
    }


    if (is_side_rotating_or_waiting(ct))
    {
        handle_cycle(ct);
    }
    //USART_transmit(0, 60 + ct);


    ct++; // increase the row
    if (ct >= SW_SIDE_NUM) ct = 0; // reset
}

static uint8_t get_rotation_dir(uint8_t switches)
{
    return SWITCH_ROTATION_MATRIX[switches];
}

static uint8_t read_switches_debounced()
{
    uint8_t state;

    state = read_switches();
    _delay_ms(DEBOUNCE_DELAY_1);
    state |= read_switches();

    if (state)
    {
        _delay_ms(DEBOUNCE_DELAY_2);
        // If during debounce delay switches (one or more) 
        // where pressed, then consider this as really pressed (debounced) switch(es)
        return state & read_switches();
    }
    return 0;
}

static uint8_t read_switches()
{
    // TODO This depends on port pins layout - may need to change
    return ~read_pins(SI) & 0x3;
}

static void set_switch_pin(uint8_t pin)
{
    // Set one pin to 0 (the one that will be scanned)
    switch (pin) {
    case 0: res_pin(SPA); break;
    case 1: res_pin(SPB); break;
    case 2: res_pin(SPC); break;
    case 3: res_pin(SPD); break;
    case 4: res_pin(SPE); break;
    case 5: res_pin(SPF); break;
    }
}

static void reset_switches_pins(void)
{
    // Reset all pins to 1
    set_pin(SPA);
    set_pin(SPB);
    set_pin(SPC);
    set_pin(SPD);
    set_pin(SPE);
    set_pin(SPF);
}
