#include "driver.h"
#include "hardware.h"
#include <util/delay.h>
#include "common.h"
#include "rotation_logic.h"

#ifdef DEBUG_COLOR_ADJUST
#include "neopixel.h"
#endif

static uint8_t get_rotation_dir(uint8_t switches);

static uint8_t read_switches_debounced();
static uint8_t read_switches();
static uint8_t read_srs_debounced();
static uint8_t read_srs_pin();

static void set_switch_pin(uint8_t pin);
static void reset_switches_pins(void);

/*                     (SW_YR)
 *        
 *                       ^ Y
 *       CENTER          *
 *           (SW_ZL)     *        _ ->X (SW_XR)
 *                  -*_  *  _ -*
 *                    _ -*_
 *           (SW_XL)*    *  -*_
 *                       *      ->Z (SW_ZR)
 *                     (SW_YL)
 */
static const uint8_t SWITCH_TO_ROTATION_MATRIX[64] = 
{
    /* 0x00                                                */   (MOVE_NONE),
    /* 0x01                                          SW_XL */   (ROTATION_Y | DIR_CCW),
    /* 0x02                                  SW_XR         */   (ROTATION_Y | DIR_CW),
    /* 0x03                                  SW_XR | SW_XL */   (MOVE_PERSP),
    /* 0x04                          SW_YL                 */   (ROTATION_X | DIR_CW),
    /* 0x05                          SW_YL         | SW_XL */   (MOVE_PERSP),
    /* 0x06                          SW_YL | SW_XR         */   (MOVE_PERSP),
    /* 0x07                          SW_YL | SW_XR | SW_XL */   (MOVE_PERSP),
    /* 0x08                  SW_YR                         */   (ROTATION_X | DIR_CCW),
    /* 0x09                  SW_YR                 | SW_XL */   (MOVE_PERSP),
    /* 0x0A                  SW_YR         | SW_XR         */   (MOVE_PERSP),
    /* 0x0B                  SW_YR         | SW_XR | SW_XL */   (MOVE_PERSP),
    /* 0x0C                  SW_YR | SW_YL                 */   (MOVE_PERSP),
    /* 0x0D                  SW_YR | SW_YL         | SW_XL */   (MOVE_PERSP),
    /* 0x0E                  SW_YR | SW_YL | SW_XR         */   (MOVE_PERSP),
    /* 0x0F                  SW_YR | SW_YL | SW_XR | SW_XL */   (MOVE_PERSP),
    /* 0x10          SW_ZL |                               */   (ROTATION_Z | DIR_CCW),
    /* 0x11          SW_ZL |                         SW_XL */   (ROTATION_Y | DIR_CCW), 
    /* 0x12          SW_ZL |                 SW_XR         */   (ROTATION_Y | DIR_CW),  
    /* 0x13          SW_ZL |                 SW_XR | SW_XL */   (MOVE_PERSP),           
    /* 0x14          SW_ZL |         SW_YL                 */   (ROTATION_X | DIR_CW),  
    /* 0x15          SW_ZL |         SW_YL         | SW_XL */   (MOVE_PERSP),           
    /* 0x16          SW_ZL |         SW_YL | SW_XR         */   (MOVE_PERSP),           
    /* 0x17          SW_ZL |         SW_YL | SW_XR | SW_XL */   (MOVE_PERSP),           
    /* 0x18          SW_ZL | SW_YR                         */   (ROTATION_X | DIR_CCW), 
    /* 0x19          SW_ZL | SW_YR                 | SW_XL */   (MOVE_PERSP),           
    /* 0x1A          SW_ZL | SW_YR         | SW_XR         */   (MOVE_PERSP),           
    /* 0x1B          SW_ZL | SW_YR         | SW_XR | SW_XL */   (MOVE_PERSP),           
    /* 0x1C          SW_ZL | SW_YR | SW_YL                 */   (MOVE_PERSP),           
    /* 0x1D          SW_ZL | SW_YR | SW_YL         | SW_XL */   (MOVE_PERSP),           
    /* 0x1E          SW_ZL | SW_YR | SW_YL | SW_XR         */   (MOVE_PERSP),           
    /* 0x1F          SW_ZL | SW_YR | SW_YL | SW_XR | SW_XL */   (MOVE_PERSP),           
    /* 0x20  SW_ZR |                                       */   (ROTATION_Z | DIR_CW),
    /* 0x21  SW_ZR |                                 SW_XL */   (ROTATION_Y | DIR_CCW), 
    /* 0x22  SW_ZR |                         SW_XR         */   (ROTATION_Y | DIR_CW),  
    /* 0x23  SW_ZR |                         SW_XR | SW_XL */   (MOVE_PERSP),           
    /* 0x24  SW_ZR |                 SW_YL                 */   (ROTATION_X | DIR_CW),  
    /* 0x25  SW_ZR |                 SW_YL         | SW_XL */   (MOVE_PERSP),           
    /* 0x26  SW_ZR |                 SW_YL | SW_XR         */   (MOVE_PERSP),           
    /* 0x27  SW_ZR |                 SW_YL | SW_XR | SW_XL */   (MOVE_PERSP),           
    /* 0x28  SW_ZR |         SW_YR                         */   (ROTATION_X | DIR_CCW), 
    /* 0x29  SW_ZR |         SW_YR                 | SW_XL */   (MOVE_PERSP),           
    /* 0x2A  SW_ZR |         SW_YR         | SW_XR         */   (MOVE_PERSP),           
    /* 0x2B  SW_ZR |         SW_YR         | SW_XR | SW_XL */   (MOVE_PERSP),           
    /* 0x2C  SW_ZR |         SW_YR | SW_YL                 */   (MOVE_PERSP),           
    /* 0x2D  SW_ZR |         SW_YR | SW_YL         | SW_XL */   (MOVE_PERSP),           
    /* 0x2E  SW_ZR |         SW_YR | SW_YL | SW_XR         */   (MOVE_PERSP),           
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


static volatile Switches_Side_State switches_side_states[SW_SIDE_NUM] = {0};
static volatile uint8_t srs_waiting_for_release = FALSE;

void init_driver(void)
{
    uint8_t i;
    Switches_Side_State *sw_side_state_ptr;
    for (i = 0; i < SW_SIDE_NUM; i++)
    {
        sw_side_state_ptr = &(switches_side_states[i]);

        sw_side_state_ptr->waiting_for_release = FALSE;
        sw_side_state_ptr->switches = 0;
        sw_side_state_ptr->cycle_ct = 0;
    }
}

ISR (TIMER1_OVF_vect)
{
    static uint8_t ct = 0;
    Switches_Side_State *sw_side_state_ptr;

    TCNT1 = (0xFFFF - ISR_TIMEOUT); // set the timeout

    // Handling Software Reset switch
    if (srs_waiting_for_release == FALSE)
    {
        if (read_srs_debounced())
        {
            srs_waiting_for_release = TRUE;
            reset_cube();
        }
    }
    else
    {
    	if (!read_srs_debounced())
            srs_waiting_for_release = FALSE;
    }

    // Handling Rotation side swicthes
    sw_side_state_ptr = &(switches_side_states[ct]);

    if (sw_side_state_ptr->waiting_for_release == FALSE)
    {
        if (sw_side_state_ptr->cycle_ct < READ_COMPLETE_SITE_STATE_CYCLES)
        {
            set_switch_pin(ct);
            // Read switches state for the entire side
            sw_side_state_ptr->switches |= read_switches_debounced();
            if (sw_side_state_ptr->switches)
            {
                // If pressed, then increase the counter. Untill it reaches max
                // gather all pressed side switches since they will not likely be
                // pressed exactly in the same very moment.
                sw_side_state_ptr->cycle_ct++;
            }
            reset_switches_pins();
        }
        else
        {
            // The wait period for switches ended, now start rotation
            if (start_rotation(ct, get_rotation_dir(sw_side_state_ptr->switches)))
            {
                // Start waiting for switch release
                sw_side_state_ptr->waiting_for_release = TRUE;
#ifdef DEBUG_COLOR_ADJUST
                debug_color_adjust(ct, sw_side_state_ptr->switches);
#endif
            }
        }
    }
    else
    {
        // Here we are waiting for the previously pressed switches to be released
        set_switch_pin(ct);
        // Read switches state for the entire side and check if they are released
        if (read_switches_debounced() == 0)
        {
            sw_side_state_ptr->waiting_for_release = FALSE;
            sw_side_state_ptr->switches = 0;
            sw_side_state_ptr->cycle_ct = 0;
        }
        reset_switches_pins();
    }

    // Pass the cycle further down the logic
    handle_cycle();

    ct++; // increase the row
    if (ct >= SW_SIDE_NUM) ct = 0; // reset the counter
}

static uint8_t get_rotation_dir(uint8_t switches)
{
    return SWITCH_TO_ROTATION_MATRIX[switches];
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
    return ~read_pins(SI) & SI_MASK;
}

static uint8_t read_srs_debounced()
{
    if (read_srs_pin())
    {
        _delay_ms(DEBOUNCE_DELAY_2);
        return read_srs_pin();
    }
    return 0;
}

static uint8_t read_srs_pin()
{
    return ~read_pin(SRS) & SRS_MASK;
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
