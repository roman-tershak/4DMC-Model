#ifndef H_ROTATION_LOGIC
#define H_ROTATION_LOGIC

#include "central_logic.h"

#define AXIS_X          0x0
#define AXIS_Y          0x1
#define AXIS_Z          0x2

/* Rotation directions */
// Main axis rotations
#define ROTATION_X      0x0
#define ROTATION_Y      0x2
#define ROTATION_Z      0x4
// Directions
#define DIR_CCW         0x0
#define DIR_CW          0x1
/* Moves */
#define MOVE_NONE       0xFF


#ifdef __cplusplus
 extern "C" {
#endif 

typedef void (*rotation_func_ptr_type)(uint8_t);

rotation_func_ptr_type get_rotation_func_ptr(uint8_t side_num, uint8_t direction);

void do_rotation(uint8_t side_num);

#ifdef __cplusplus
}
#endif 

#endif
