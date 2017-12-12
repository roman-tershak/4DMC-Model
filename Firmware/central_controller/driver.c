#include "driver.h"
#include "hardware.h"
#include <util/delay.h>
#include "common.h"
#include "rotation_logic.h"

#ifdef DEBUG_COLOR_ADJUST
#include "neopixel.h"
#endif

#define DEBUG_LOG(STEP, WHAT) deb[i++] = (uint16_t) (STEP) | (WHAT); if (i >= 25) {i = 0;} else {};

static uint8_t read_switches_debounced(uint8_t switch_row);
static uint8_t read_switches();
static uint8_t read_srs_debounced();
static uint8_t read_srs_pin();

static void set_switch_pin_to_zero(uint8_t pin);
static void reset_switches_pins_to_one(void);

static void get_rotation_side_and_dir(uint8_t swn, uint8_t switches_i, uint8_t back_side_ind, 
        uint8_t *swn_out_ptr, uint8_t *dir_out);


static volatile uint8_t srs_waiting_for_release = FALSE;


void init_driver(void)
{
}

ISR (TIMER1_OVF_vect)
{
    static volatile uint8_t cycle_ct = 0;
    static volatile uint8_t switches_initial = 0;

    uint8_t switches, next_side, rotate, rotating_side, rotation_dir;

    TCNT1 = (0xFFFF - ISR_TIMEOUT); // set the timeout

    // Handling Software Reset switch
    if (!srs_waiting_for_release)
    {
        if (read_srs_debounced())
        {
            srs_waiting_for_release = TRUE;
            reset_cube();
        }
    }
    else
    {
        if (read_srs_debounced() == 0)
            srs_waiting_for_release = FALSE;
    }

    rotate = FALSE;

    if (switches = read_switches_debounced(cycle_ct))  // switch(es) are pressed
    {
        if (cycle_ct < READ_SITE_SWITCH_STATE_MAX_CYCLES)  // and not waiting for them just to be released
        {
            if (!switches_initial)
                switches_initial = switches;

            cycle_ct++;
        }
        else if (switches_initial)
        {
            // The wait period for switches has finished, now start rotation
            get_rotation_side_and_dir(cycle_ct, switches_initial, TRUE, &rotating_side, &rotation_dir);
            rotate = TRUE;
            switches_initial = 0;
        }

        next_side = FALSE;
    }
    else  // switches are not pressed
    {
        if (switches_initial)
        {
            // The pressed switch(es) was released, now start rotation
            get_rotation_side_and_dir(cycle_ct, switches_initial, FALSE, &rotating_side, &rotation_dir);
            rotate = TRUE;
            switches_initial = 0;
        }
        if (cycle_ct)
        {
            cycle_ct = 0;
        }

        next_side = TRUE;
    }

    if (rotate)
    {
DISABLE_GLOBAL_INTERRUPTS();
        rotate_side(rotating_side, rotation_dir);
ENABLE_GLOBAL_INTERRUPTS();

#ifdef DEBUG_COLOR_ADJUST
        debug_color_adjust(cycle_ct, switches);
#endif
    }

    if (next_side)
    {
        cycle_ct++; // next sensor set/side
        if (cycle_ct >= SW_SIDE_NUM) cycle_ct = 0; // going in round cycle
    }

    // Pass the cycle tick further down the logic
    handle_cycle();
}

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
    /* 0x03                                  SW_XR | SW_XL */   (MOVE_NONE),
    /* 0x04                          SW_YL                 */   (ROTATION_X | DIR_CW),
    /* 0x05                          SW_YL         | SW_XL */   (MOVE_NONE),
    /* 0x06                          SW_YL | SW_XR         */   (MOVE_NONE),
    /* 0x07                          SW_YL | SW_XR | SW_XL */   (MOVE_NONE),
    /* 0x08                  SW_YR                         */   (ROTATION_X | DIR_CCW),
    /* 0x09                  SW_YR                 | SW_XL */   (MOVE_NONE),
    /* 0x0A                  SW_YR         | SW_XR         */   (MOVE_NONE),
    /* 0x0B                  SW_YR         | SW_XR | SW_XL */   (MOVE_NONE),
    /* 0x0C                  SW_YR | SW_YL                 */   (MOVE_NONE),
    /* 0x0D                  SW_YR | SW_YL         | SW_XL */   (MOVE_NONE),
    /* 0x0E                  SW_YR | SW_YL | SW_XR         */   (MOVE_NONE),
    /* 0x0F                  SW_YR | SW_YL | SW_XR | SW_XL */   (MOVE_NONE),
    /* 0x10          SW_ZL |                               */   (ROTATION_Z | DIR_CCW),
    /* 0x11          SW_ZL |                         SW_XL */   (MOVE_NONE), 
    /* 0x12          SW_ZL |                 SW_XR         */   (MOVE_NONE),  
    /* 0x13          SW_ZL |                 SW_XR | SW_XL */   (MOVE_NONE),           
    /* 0x14          SW_ZL |         SW_YL                 */   (MOVE_NONE),  
    /* 0x15          SW_ZL |         SW_YL         | SW_XL */   (MOVE_NONE),           
    /* 0x16          SW_ZL |         SW_YL | SW_XR         */   (MOVE_NONE),           
    /* 0x17          SW_ZL |         SW_YL | SW_XR | SW_XL */   (MOVE_NONE),           
    /* 0x18          SW_ZL | SW_YR                         */   (MOVE_NONE), 
    /* 0x19          SW_ZL | SW_YR                 | SW_XL */   (MOVE_NONE),           
    /* 0x1A          SW_ZL | SW_YR         | SW_XR         */   (MOVE_NONE),           
    /* 0x1B          SW_ZL | SW_YR         | SW_XR | SW_XL */   (MOVE_NONE),           
    /* 0x1C          SW_ZL | SW_YR | SW_YL                 */   (MOVE_NONE),           
    /* 0x1D          SW_ZL | SW_YR | SW_YL         | SW_XL */   (MOVE_NONE),           
    /* 0x1E          SW_ZL | SW_YR | SW_YL | SW_XR         */   (MOVE_NONE),           
    /* 0x1F          SW_ZL | SW_YR | SW_YL | SW_XR | SW_XL */   (MOVE_NONE),           
    /* 0x20  SW_ZR |                                       */   (ROTATION_Z | DIR_CW),
    /* 0x21  SW_ZR |                                 SW_XL */   (MOVE_NONE), 
    /* 0x22  SW_ZR |                         SW_XR         */   (MOVE_NONE),  
    /* 0x23  SW_ZR |                         SW_XR | SW_XL */   (MOVE_NONE),           
    /* 0x24  SW_ZR |                 SW_YL                 */   (MOVE_NONE),  
    /* 0x25  SW_ZR |                 SW_YL         | SW_XL */   (MOVE_NONE),           
    /* 0x26  SW_ZR |                 SW_YL | SW_XR         */   (MOVE_NONE),           
    /* 0x27  SW_ZR |                 SW_YL | SW_XR | SW_XL */   (MOVE_NONE),           
    /* 0x28  SW_ZR |         SW_YR                         */   (MOVE_NONE), 
    /* 0x29  SW_ZR |         SW_YR                 | SW_XL */   (MOVE_NONE),           
    /* 0x2A  SW_ZR |         SW_YR         | SW_XR         */   (MOVE_NONE),           
    /* 0x2B  SW_ZR |         SW_YR         | SW_XR | SW_XL */   (MOVE_NONE),           
    /* 0x2C  SW_ZR |         SW_YR | SW_YL                 */   (MOVE_NONE),           
    /* 0x2D  SW_ZR |         SW_YR | SW_YL         | SW_XL */   (MOVE_NONE),           
    /* 0x2E  SW_ZR |         SW_YR | SW_YL | SW_XR         */   (MOVE_NONE),           
    /* 0x2F  SW_ZR |         SW_YR | SW_YL | SW_XR | SW_XL */   (MOVE_NONE),           
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

/* 
 *               Sensors orientation
 * 
 * 
 *                          (side_zr)
 *                         Z
 *                        |___Y
 *                         \
 *                          vX
 *           (side_yr) z y
 *                      \|
 *                   x<-- 
 * (side_xl)                               ^X (side_xr)       |Z
 *      Z ___                |Z (side_cf)  |                  |
 *           |\              |             |___Z        ~X<---
 *           | Y              --->Y         \                  \
 *           vX               \              Y        (side_cb) \Y
 *                             X  |Y
 *                                |
 *                                 ---->X
 *                       x^        \   (side_yl)
 *                         \___Y    \Z
 *                         |
 *               (side_zl)  Z
 * 
 */
static const uint8_t SWITCH_TO_ROTATION_MATRIX_CB[] =
{
    /* SW_SIDE_XL */
    /* 0x00                                */   (MOVE_NONE),
    /* 0x01                          SW_XL */   (ROTATION_Y | DIR_CCW),
    /* 0x02                  SW_XR         */   (ROTATION_Y | DIR_CW),
    /* 0x03                  SW_XR | SW_XL */   (MOVE_NONE),
    /* 0x04          SW_YL                 */   (ROTATION_Z | DIR_CCW),
    /* 0x05          SW_YL         | SW_XL */   (MOVE_NONE),
    /* 0x06          SW_YL | SW_XR         */   (MOVE_NONE),
    /* 0x07          SW_YL | SW_XR | SW_XL */   (MOVE_NONE),
    /* 0x08  SW_YR                         */   (ROTATION_Z | DIR_CW),
    /* 0x09  SW_YR                 | SW_XL */   (MOVE_NONE),
    /* 0x0A  SW_YR         | SW_XR         */   (MOVE_NONE),
    /* 0x0B  SW_YR         | SW_XR | SW_XL */   (MOVE_NONE),
    /* 0x0C  SW_YR | SW_YL                 */   (MOVE_NONE),
    /* 0x0D  SW_YR | SW_YL         | SW_XL */   (MOVE_NONE),
    /* 0x0E  SW_YR | SW_YL | SW_XR         */   (MOVE_NONE),
    /* 0x0F  SW_YR | SW_YL | SW_XR | SW_XL */   (MOVE_NONE),

    /* SW_SIDE_XR */
    /* 0x10                                */   (MOVE_NONE),
    /* 0x11                          SW_XL */   (ROTATION_Y | DIR_CCW),
    /* 0x12                  SW_XR         */   (ROTATION_Y | DIR_CW),
    /* 0x13                  SW_XR | SW_XL */   (MOVE_NONE),
    /* 0x14          SW_YL                 */   (ROTATION_Z | DIR_CW),
    /* 0x15          SW_YL         | SW_XL */   (MOVE_NONE),
    /* 0x16          SW_YL | SW_XR         */   (MOVE_NONE),
    /* 0x17          SW_YL | SW_XR | SW_XL */   (MOVE_NONE),
    /* 0x18  SW_YR                         */   (ROTATION_Z | DIR_CCW),
    /* 0x19  SW_YR                 | SW_XL */   (MOVE_NONE),
    /* 0x1A  SW_YR         | SW_XR         */   (MOVE_NONE),
    /* 0x1B  SW_YR         | SW_XR | SW_XL */   (MOVE_NONE),
    /* 0x1C  SW_YR | SW_YL                 */   (MOVE_NONE),
    /* 0x1D  SW_YR | SW_YL         | SW_XL */   (MOVE_NONE),
    /* 0x1E  SW_YR | SW_YL | SW_XR         */   (MOVE_NONE),
    /* 0x1F  SW_YR | SW_YL | SW_XR | SW_XL */   (MOVE_NONE),

    /* SW_SIDE_YL */
    /* 0x20                                */   (MOVE_NONE),
    /* 0x21                          SW_XL */   (ROTATION_Z | DIR_CCW),
    /* 0x22                  SW_XR         */   (ROTATION_Z | DIR_CW),
    /* 0x23                  SW_XR | SW_XL */   (MOVE_NONE),
    /* 0x24          SW_YL                 */   (ROTATION_X | DIR_CW),
    /* 0x25          SW_YL         | SW_XL */   (MOVE_NONE),
    /* 0x26          SW_YL | SW_XR         */   (MOVE_NONE),
    /* 0x27          SW_YL | SW_XR | SW_XL */   (MOVE_NONE),
    /* 0x28  SW_YR                         */   (ROTATION_X | DIR_CCW),
    /* 0x29  SW_YR                 | SW_XL */   (MOVE_NONE),
    /* 0x2A  SW_YR         | SW_XR         */   (MOVE_NONE),
    /* 0x2B  SW_YR         | SW_XR | SW_XL */   (MOVE_NONE),
    /* 0x2C  SW_YR | SW_YL                 */   (MOVE_NONE),
    /* 0x2D  SW_YR | SW_YL         | SW_XL */   (MOVE_NONE),
    /* 0x2E  SW_YR | SW_YL | SW_XR         */   (MOVE_NONE),
    /* 0x2F  SW_YR | SW_YL | SW_XR | SW_XL */   (MOVE_NONE),

    /* SW_SIDE_YR */
    /* 0x30                                */   (MOVE_NONE),
    /* 0x31                          SW_XL */   (ROTATION_Z | DIR_CCW),
    /* 0x32                  SW_XR         */   (ROTATION_Z | DIR_CW),
    /* 0x33                  SW_XR | SW_XL */   (MOVE_NONE),
    /* 0x34          SW_YL                 */   (ROTATION_X | DIR_CCW),
    /* 0x35          SW_YL         | SW_XL */   (MOVE_NONE),
    /* 0x36          SW_YL | SW_XR         */   (MOVE_NONE),
    /* 0x37          SW_YL | SW_XR | SW_XL */   (MOVE_NONE),
    /* 0x38  SW_YR                         */   (ROTATION_X | DIR_CW),
    /* 0x39  SW_YR                 | SW_XL */   (MOVE_NONE),
    /* 0x3A  SW_YR         | SW_XR         */   (MOVE_NONE),
    /* 0x3B  SW_YR         | SW_XR | SW_XL */   (MOVE_NONE),
    /* 0x3C  SW_YR | SW_YL                 */   (MOVE_NONE),
    /* 0x3D  SW_YR | SW_YL         | SW_XL */   (MOVE_NONE),
    /* 0x3E  SW_YR | SW_YL | SW_XR         */   (MOVE_NONE),
    /* 0x3F  SW_YR | SW_YL | SW_XR | SW_XL */   (MOVE_NONE),

    /* SW_SIDE_ZL */
    /* 0x40                                */   (MOVE_NONE),
    /* 0x41                          SW_XL */   (ROTATION_X | DIR_CCW),
    /* 0x42                  SW_XR         */   (ROTATION_X | DIR_CW),
    /* 0x43                  SW_XR | SW_XL */   (MOVE_NONE),
    /* 0x44          SW_YL                 */   (ROTATION_Y | DIR_CCW),
    /* 0x45          SW_YL         | SW_XL */   (MOVE_NONE),
    /* 0x46          SW_YL | SW_XR         */   (MOVE_NONE),
    /* 0x47          SW_YL | SW_XR | SW_XL */   (MOVE_NONE),
    /* 0x48  SW_YR                         */   (ROTATION_Y | DIR_CW),
    /* 0x49  SW_YR                 | SW_XL */   (MOVE_NONE),
    /* 0x4A  SW_YR         | SW_XR         */   (MOVE_NONE),
    /* 0x4B  SW_YR         | SW_XR | SW_XL */   (MOVE_NONE),
    /* 0x4C  SW_YR | SW_YL                 */   (MOVE_NONE),
    /* 0x4D  SW_YR | SW_YL         | SW_XL */   (MOVE_NONE),
    /* 0x4E  SW_YR | SW_YL | SW_XR         */   (MOVE_NONE),
    /* 0x4F  SW_YR | SW_YL | SW_XR | SW_XL */   (MOVE_NONE),

    /* SW_SIDE_ZR */
    /* 0x50                                */   (MOVE_NONE),
    /* 0x51                          SW_XL */   (ROTATION_X | DIR_CCW),
    /* 0x52                  SW_XR         */   (ROTATION_X | DIR_CW),
    /* 0x53                  SW_XR | SW_XL */   (MOVE_NONE),
    /* 0x54          SW_YL                 */   (ROTATION_Y | DIR_CW),
    /* 0x55          SW_YL         | SW_XL */   (MOVE_NONE),
    /* 0x56          SW_YL | SW_XR         */   (MOVE_NONE),
    /* 0x57          SW_YL | SW_XR | SW_XL */   (MOVE_NONE),
    /* 0x58  SW_YR                         */   (ROTATION_Y | DIR_CCW),
    /* 0x59  SW_YR                 | SW_XL */   (MOVE_NONE),
    /* 0x5A  SW_YR         | SW_XR         */   (MOVE_NONE),
    /* 0x5B  SW_YR         | SW_XR | SW_XL */   (MOVE_NONE),
    /* 0x5C  SW_YR | SW_YL                 */   (MOVE_NONE),
    /* 0x5D  SW_YR | SW_YL         | SW_XL */   (MOVE_NONE),
    /* 0x5E  SW_YR | SW_YL | SW_XR         */   (MOVE_NONE),
    /* 0x5F  SW_YR | SW_YL | SW_XR | SW_XL */   (MOVE_NONE),
};

static void get_rotation_side_and_dir(uint8_t swn, uint8_t switches_i, uint8_t back_side_ind, 
        uint8_t *swn_out_ptr, uint8_t *dir_out)
{
    if (back_side_ind &&
            swn < SW_SIDE_CF &&    // All switch sides but CF and CB
            switches_i < SW_ZL)      // Only XL, XR and YL, YR switches
    {
        switches_i &= SW_XLR_YLR;  // Clear ZL, ZR bits
        switches_i |= swn << 4;    // Put swicthes side num into higher bits, this is needed for lookup in 
                                   // SWITCH_TO_ROTATION_MATRIX_CB matrix
        
        *swn_out_ptr = SW_SIDE_CB;  // CB side is rotating
        *dir_out = SWITCH_TO_ROTATION_MATRIX_CB[switches_i];  // get rotation direction from this matrix
    }
    else
    {
        *swn_out_ptr = swn;
        *dir_out = SWITCH_TO_ROTATION_MATRIX[switches_i];
    }
}

static uint8_t read_switches_debounced(uint8_t switch_row)
{
    uint8_t state;

    set_switch_pin_to_zero(switch_row);

    state = read_switches();
    _delay_ms(DEBOUNCE_DELAY_1);
    state |= read_switches();

    if (state)
    {
        _delay_ms(DEBOUNCE_DELAY_2);
        // If during debounce delay switches (one or more) 
        // where pressed, then consider this as really pressed (debounced) switch(es)
        state &= read_switches();
    }
    reset_switches_pins_to_one();

    return state;
}

static uint8_t read_switches()
{
    // Read inverted values
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
    // Read inverted SRS bit
    return ~read_pin(SRS) & SRS_MASK;
}

static void set_switch_pin_to_zero(uint8_t pin)
{
    // Set one pin to 0 (the one that will be scanned)
    switch (pin) {
    case 0: res_pin(SPA); break;
    case 1: res_pin(SPB); break;
    case 2: res_pin(SPC); break;
    case 3: res_pin(SPD); break;
    case 4: res_pin(SPE); break;
    case 5: res_pin(SPF); break;
    case 6: res_pin(SPG); break;
    }
}

static void reset_switches_pins_to_one(void)
{
    // Reset all pins to 1
    set_pin(SPA);
    set_pin(SPB);
    set_pin(SPC);
    set_pin(SPD);
    set_pin(SPE);
    set_pin(SPF);
    set_pin(SPG);
}
