#ifndef H_CENTRAL_LOGIC
#define H_CENTRAL_LOGIC

#include <stdio.h>
#include <avr/io.h>
#include "hardware.h"
#include "common.h"


/* Colors */
#define MAIN_COLOR_COUNT        8
#define ALL_COLOR_COUNT         16

/* Side states */
#define SIDE_IDLE               _BV(0)
#define WAITING_FOR_ROTATION    _BV(1)
#define ROTATING                _BV(2)
#define WAITING_FOR_SAVING      _BV(3)


#ifdef __cplusplus
 extern "C" {
#endif 

typedef struct 
{
    uint8_t status;
    uint8_t cycle_ct;
    void *rotation_func_ptr;
    uint8_t colors[SIDE_CUBES_COUNT];
    uint8_t colors_changed;
} Side_State;

void init_central_logic(void);

void load_sides_states(void);

void reset_sides_states(void);

uint8_t start_rotation(uint8_t sw_side_num, uint8_t direction);

void handle_cycle(void);

void sides_colors_changed(void);

#ifdef __cplusplus
}
#endif 

#endif
