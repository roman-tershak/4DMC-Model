#ifndef H_CENTRAL_LOGIC
#define H_CENTRAL_LOGIC

#include <stdio.h>
#include "common.h"


/* Side states */
#define SIDE_IDLE               _BV(0)
#define WAITING_FOR_ROTATION    _BV(1)
#define ROTATING                _BV(2)


#ifdef __cplusplus
 extern "C" {
#endif 

typedef struct 
{
    volatile uint8_t status;
    volatile uint8_t cycle_ct;
    volatile void *rotation_func_ptr;
    uint8_t colors[SIDE_STICKER_COUNT];
} Side_State;

void load_sides_states(void);
void reset_cube(void);

uint8_t rotation_notify(uint8_t sw_side_num, uint8_t direction);
void rotation_done(uint8_t side_num);
void handle_cycle(void);

void sides_colors_changed(void);

#ifdef __cplusplus
}
#endif 

#endif
