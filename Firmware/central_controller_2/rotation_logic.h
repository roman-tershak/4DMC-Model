#ifndef H_ROTATION_LOGIC
#define H_ROTATION_LOGIC

#include "central_logic.h"

/* Moves */
#define MOVE_NONE       0x0
#define MOVE_PERSP      0x1

/* Rotation directions */
// Main axis
#define ROTATION_X      0x2
#define ROTATION_Y      0x4
#define ROTATION_Z      0x6
// Vertices axis
#define ROTATION_XYZ    0x8
#define ROTATION_XY_Z   0xA
#define ROTATION_X_YZ   0xC
#define ROTATION_X_Y_Z  0xE
// Side axis
#define ROTATION_XY     0x10
#define ROTATION_X_Y    0x12
#define ROTATION_XZ     0x14
#define ROTATION_X_Z    0x16
#define ROTATION_YZ     0x18
#define ROTATION_Y_Z    0x1A
// Directions
#define DIR_CCW         0x0
#define DIR_CW          0x1

/* Rotation phases */
//#define ROTATION_PHASE_ONE_CYCLES  2
//#define ROTATION_PHASE_ONE_2_CYCLES  3
//#define ROTATION_PHASE_TWO_CYCLES  4
//#define ROTATION_PHASE_FINAL_CYCLES  6

#ifdef __cplusplus
 extern "C" {
#endif 

typedef enum
{
	LEFT, RIGHT
} Orientation;

typedef void* (*rotation_func_ptr_type)(uint8_t, Side_State*);

uint8_t get_rotation_phase_len(uint8_t direction);

rotation_func_ptr_type get_rotation_func_ptr(uint8_t direction);

void rotation_cycle(uint8_t side_num, Side_State *state_ptr);

#ifdef __cplusplus
}
#endif 

#endif
