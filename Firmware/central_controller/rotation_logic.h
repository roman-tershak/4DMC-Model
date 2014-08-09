#ifndef H_ROTATION_LOGIC
#define H_ROTATION_LOGIC

#include "central_logic.h"

#define AXIS_X          0x0
#define AXIS_Y          0x1
#define AXIS_Z          0x2

/* Rotation directions */
// Main axis rotations
#define ROTATION_X      (AXIS_X << 1)
#define ROTATION_Y      (AXIS_Y << 1)
#define ROTATION_Z      (AXIS_Z << 1)
// Directions
#define DIR_CCW         0x0
#define DIR_CW          0x1
/* Moves */
#define MOVE_PERSP      0x6		// This should go right after the last rotation direction (ROTATION_Z | DIR_CW)
#define MOVE_NONE       0xFF


#ifdef __cplusplus
 extern "C" {
#endif 

typedef uint8_t (*rotation_func_ptr_type)(uint8_t);

rotation_func_ptr_type get_rotation_func_ptr(uint8_t side_num, uint8_t direction);

void rotation_cycle(uint8_t side_num);

void change_phase_cycle_counters(uint8_t faster, uint8_t slower);

#ifdef __cplusplus
}
#endif 

#endif
