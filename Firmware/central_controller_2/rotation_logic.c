#include "rotation_logic.h"
#include "common.h"

#define DEEM(color) (0x8 | (color))
#define UNDEEM(color) ((color) & 0x7)

/* Static functions declarations */
static rotation_func_ptr_type move_none(uint8_t side_num, Side_State *state_ptr);

static rotation_func_ptr_type rotation_x_dir_ccw_1(uint8_t side_num, Side_State *state_ptr);
static rotation_func_ptr_type rotation_x_dir_ccw_2(uint8_t side_num, Side_State *state_ptr);
static rotation_func_ptr_type rotation_x_dir_ccw_f(uint8_t side_num, Side_State *state_ptr);
static rotation_func_ptr_type rotation_x_dir_cw_1(uint8_t side_num, Side_State *state_ptr);
static rotation_func_ptr_type rotation_x_dir_cw_2(uint8_t side_num, Side_State *state_ptr);
static rotation_func_ptr_type rotation_x_dir_cw_f(uint8_t side_num, Side_State *state_ptr);

static rotation_func_ptr_type rotation_y_dir_ccw_1(uint8_t side_num, Side_State *state_ptr);
static rotation_func_ptr_type rotation_y_dir_ccw_2(uint8_t side_num, Side_State *state_ptr);
static rotation_func_ptr_type rotation_y_dir_ccw_f(uint8_t side_num, Side_State *state_ptr);
static rotation_func_ptr_type rotation_y_dir_cw_1(uint8_t side_num, Side_State *state_ptr);
static rotation_func_ptr_type rotation_y_dir_cw_2(uint8_t side_num, Side_State *state_ptr);
static rotation_func_ptr_type rotation_y_dir_cw_f(uint8_t side_num, Side_State *state_ptr);

static rotation_func_ptr_type rotation_z_dir_ccw_1(uint8_t side_num, Side_State *state_ptr);
static rotation_func_ptr_type rotation_z_dir_ccw_2(uint8_t side_num, Side_State *state_ptr);
static rotation_func_ptr_type rotation_z_dir_ccw_f(uint8_t side_num, Side_State *state_ptr);
static rotation_func_ptr_type rotation_z_dir_cw_1(uint8_t side_num, Side_State *state_ptr);
static rotation_func_ptr_type rotation_z_dir_cw_2(uint8_t side_num, Side_State *state_ptr);
static rotation_func_ptr_type rotation_z_dir_cw_f(uint8_t side_num, Side_State *state_ptr);


/*static rotation_func_ptr_type rotation_xyz_dir_ccw_1(uint8_t side_num, Side_State *state_ptr);
static rotation_func_ptr_type rotation_xyz_dir_ccw_f(uint8_t side_num, Side_State *state_ptr);
static rotation_func_ptr_type rotation_xyz_dir_cw_1(uint8_t side_num, Side_State *state_ptr);
static rotation_func_ptr_type rotation_xyz_dir_cw_f(uint8_t side_num, Side_State *state_ptr);

static rotation_func_ptr_type rotation_xy_z_dir_ccw_1(uint8_t side_num, Side_State *state_ptr);
static rotation_func_ptr_type rotation_xy_z_dir_ccw_f(uint8_t side_num, Side_State *state_ptr);
static rotation_func_ptr_type rotation_xy_z_dir_cw_1(uint8_t side_num, Side_State *state_ptr);
static rotation_func_ptr_type rotation_xy_z_dir_cw_f(uint8_t side_num, Side_State *state_ptr);

static rotation_func_ptr_type rotation_x_yz_dir_ccw_1(uint8_t side_num, Side_State *state_ptr);
static rotation_func_ptr_type rotation_x_yz_dir_ccw_f(uint8_t side_num, Side_State *state_ptr);
static rotation_func_ptr_type rotation_x_yz_dir_cw_1(uint8_t side_num, Side_State *state_ptr);
static rotation_func_ptr_type rotation_x_yz_dir_cw_f(uint8_t side_num, Side_State *state_ptr);

static rotation_func_ptr_type rotation_x_y_z_dir_ccw_1(uint8_t side_num, Side_State *state_ptr);
static rotation_func_ptr_type rotation_x_y_z_dir_ccw_f(uint8_t side_num, Side_State *state_ptr);
static rotation_func_ptr_type rotation_x_y_z_dir_cw_1(uint8_t side_num, Side_State *state_ptr);
static rotation_func_ptr_type rotation_x_y_z_dir_cw_f(uint8_t side_num, Side_State *state_ptr);


static rotation_func_ptr_type rotation_xy_dir_ccw_1(uint8_t side_num, Side_State *state_ptr);
static rotation_func_ptr_type rotation_xy_dir_ccw_f(uint8_t side_num, Side_State *state_ptr);
static rotation_func_ptr_type rotation_xy_dir_cw_1(uint8_t side_num, Side_State *state_ptr);
static rotation_func_ptr_type rotation_xy_dir_cw_f(uint8_t side_num, Side_State *state_ptr);

static rotation_func_ptr_type rotation_x_y_dir_ccw_1(uint8_t side_num, Side_State *state_ptr);
static rotation_func_ptr_type rotation_x_y_dir_ccw_f(uint8_t side_num, Side_State *state_ptr);
static rotation_func_ptr_type rotation_x_y_dir_cw_1(uint8_t side_num, Side_State *state_ptr);
static rotation_func_ptr_type rotation_x_y_dir_cw_f(uint8_t side_num, Side_State *state_ptr);

static rotation_func_ptr_type rotation_xz_dir_ccw_1(uint8_t side_num, Side_State *state_ptr);
static rotation_func_ptr_type rotation_xz_dir_ccw_f(uint8_t side_num, Side_State *state_ptr);
static rotation_func_ptr_type rotation_xz_dir_cw_1(uint8_t side_num, Side_State *state_ptr);
static rotation_func_ptr_type rotation_xz_dir_cw_f(uint8_t side_num, Side_State *state_ptr);

static rotation_func_ptr_type rotation_x_z_dir_ccw_1(uint8_t side_num, Side_State *state_ptr);
static rotation_func_ptr_type rotation_x_z_dir_ccw_f(uint8_t side_num, Side_State *state_ptr);
static rotation_func_ptr_type rotation_x_z_dir_cw_1(uint8_t side_num, Side_State *state_ptr);
static rotation_func_ptr_type rotation_x_z_dir_cw_f(uint8_t side_num, Side_State *state_ptr);

static rotation_func_ptr_type rotation_yz_dir_ccw_1(uint8_t side_num, Side_State *state_ptr);
static rotation_func_ptr_type rotation_yz_dir_ccw_f(uint8_t side_num, Side_State *state_ptr);
static rotation_func_ptr_type rotation_yz_dir_cw_1(uint8_t side_num, Side_State *state_ptr);
static rotation_func_ptr_type rotation_yz_dir_cw_f(uint8_t side_num, Side_State *state_ptr);

static rotation_func_ptr_type rotation_y_z_dir_ccw_1(uint8_t side_num, Side_State *state_ptr);
static rotation_func_ptr_type rotation_y_z_dir_ccw_f(uint8_t side_num, Side_State *state_ptr);
static rotation_func_ptr_type rotation_y_z_dir_cw_1(uint8_t side_num, Side_State *state_ptr);
static rotation_func_ptr_type rotation_y_z_dir_cw_f(uint8_t side_num, Side_State *state_ptr);
*/

static rotation_func_ptr_type move_persp_1(uint8_t side_num, Side_State *state_ptr);
static rotation_func_ptr_type move_persp_2(uint8_t side_num, Side_State *state_ptr);
static rotation_func_ptr_type move_persp_f(uint8_t side_num, Side_State *state_ptr);

static uint8_t is_side_left(uint8_t side_num);
static uint8_t get_adjacent_left_side(uint8_t side_num);
static uint8_t get_adjacent_right_side(uint8_t side_num);
static uint8_t get_adjacent_down_side(uint8_t side_num);
static uint8_t get_adjacent_up_side(uint8_t side_num);

static void test_1(uint8_t side_num, Side_State *state_ptr);
static void test_2(uint8_t side_num, Side_State *state_ptr);
static void test_3(uint8_t side_num, Side_State *state_ptr);


/*
 *                   Z
 *                  |___Y
 *                   \
 *                    vX
 *               z y
 *                \|
 *             x<-- 
 *                    z           ^X
 *  Z ___          y\|            |
 *       |\        ----->x        |___Z
 *       | Y         |\            \
 *       vX           |Y            Y
 *                     ---->X
 *                     \
 *                      Z
 *                 x^
 *                   \___Y
 *                   |
 *                    Z
 *
 */
static const uint8_t ADJACENT_SIDES_MATRIX[][4] = 
{
    /* SIDE_XL */ {SIDE_ZR, SIDE_ZL, SIDE_YR, SIDE_YL},
    /* SIDE_XR */ {SIDE_ZL, SIDE_ZR, SIDE_YR, SIDE_YL},
    /* SIDE_YL */ {SIDE_XL, SIDE_XR, SIDE_ZL, SIDE_ZR},
    /* SIDE_YR */ {SIDE_XR, SIDE_XL, SIDE_ZL, SIDE_ZR},
    /* SIDE_ZL */ {SIDE_YL, SIDE_YR, SIDE_XL, SIDE_XR},
    /* SIDE_ZR */ {SIDE_YR, SIDE_YL, SIDE_XL, SIDE_XR}
    /* SIDE_CF */ 
    /* SIDE_CB */ 
};

static const Orientation ORIENTATION_MATRIX[] = 
{
    /* SIDE_XL */ RIGHT,
    /* SIDE_XR */ LEFT,
    /* SIDE_YL */ RIGHT,
    /* SIDE_YR */ LEFT,
    /* SIDE_ZL */ RIGHT,
    /* SIDE_ZR */ LEFT,
    /* SIDE_CF */ RIGHT,
    /* SIDE_CB */ LEFT
};

static const rotation_func_ptr_type ROTATION_FUNC_MATRIX_1[/*TODO*/] =
{
    /* 0x00             MOVE_NONE     */  move_none,
    /* 0x01             MOVE_PERSP    */  move_persp_1,
    
    /* 0x02  ROTATION_X     | DIR_CCW */  rotation_x_dir_ccw_1,
    /* 0x03  ROTATION_X     | DIR_CW  */  rotation_x_dir_cw_1,
    /* 0x04  ROTATION_Y     | DIR_CCW */  rotation_y_dir_ccw_1,
    /* 0x05  ROTATION_Y     | DIR_CW  */  rotation_y_dir_cw_1,
    /* 0x06  ROTATION_Z     | DIR_CCW */  rotation_z_dir_ccw_1,
    /* 0x07  ROTATION_Z     | DIR_CW  */  rotation_z_dir_cw_1//,

    // /* 0x08  ROTATION_XYZ   | DIR_CCW */  rotation_xyz_dir_ccw_1,
    // /* 0x09  ROTATION_XYZ   | DIR_CW  */  rotation_xyz_dir_cw_1,
    // /* 0x0A  ROTATION_XY_Z  | DIR_CCW */  rotation_xy_z_dir_ccw_1,
    // /* 0x0B  ROTATION_XY_Z  | DIR_CW  */  rotation_xy_z_dir_cw_1,
    // /* 0x0C  ROTATION_X_YZ  | DIR_CCW */  rotation_x_yz_dir_ccw_1,
    // /* 0x0D  ROTATION_X_YZ  | DIR_CW  */  rotation_x_yz_dir_cw_1,
    // /* 0x0E  ROTATION_X_Y_Z | DIR_CCW */  rotation_x_y_z_dir_ccw_1,
    // /* 0x0F  ROTATION_X_Y_Z | DIR_CW  */  rotation_x_y_z_dir_cw_1,

    // /* 0x10  ROTATION_XY    | DIR_CCW */  rotation_xy_dir_ccw_1,
    // /* 0x11  ROTATION_XY    | DIR_CW  */  rotation_xy_dir_cw_1,
    // /* 0x12  ROTATION_X_Y   | DIR_CCW */  rotation_x_y_dir_ccw_1,
    // /* 0x13  ROTATION_X_Y   | DIR_CW  */  rotation_x_y_dir_cw_1,
    // /* 0x14  ROTATION_XZ    | DIR_CCW */  rotation_xz_dir_ccw_1,
    // /* 0x15  ROTATION_XZ    | DIR_CW  */  rotation_xz_dir_cw_1,
    // /* 0x16  ROTATION_X_Z   | DIR_CCW */  rotation_x_z_dir_ccw_1,
    // /* 0x17  ROTATION_X_Z   | DIR_CW  */  rotation_x_z_dir_cw_1,
    // /* 0x18  ROTATION_YZ    | DIR_CCW */  rotation_yz_dir_ccw_1,
    // /* 0x19  ROTATION_YZ    | DIR_CW  */  rotation_yz_dir_cw_1,
    // /* 0x1A  ROTATION_Y_Z   | DIR_CCW */  rotation_y_z_dir_ccw_1,
    // /* 0x1B  ROTATION_Y_Z   | DIR_CW  */  rotation_y_z_dir_cw_1
};


extern volatile Side_State sides_states[SIDE_COUNT];


/* Public functions */
uint8_t get_rotation_phase_len(uint8_t direction)
{
    if (direction >= MOVE_PERSP && direction < ROTATION_XYZ)
        return 3;
    else if (direction >= ROTATION_XYZ && direction <= (ROTATION_Y_Z | DIR_CW))
        return 3;
    else
        return 0;
}

rotation_func_ptr_type get_rotation_func_ptr(uint8_t direction)
{
    return ROTATION_FUNC_MATRIX_1[direction];
}

void rotation_cycle(uint8_t side_num, Side_State *state_ptr)
{
    state_ptr->cycle_ct++;

    if (state_ptr->cycle_ct >= state_ptr->phase_len)
    {
        state_ptr->cycle_ct = 0;

        if (state_ptr->rotation_func_ptr != NULL)
        {
            state_ptr->rotation_func_ptr = ((rotation_func_ptr_type) state_ptr->rotation_func_ptr)(side_num, state_ptr);
            // state_ptr->colors_changed are set by rotation function
 
            notify_sides_changed();

            if (state_ptr->rotation_func_ptr == NULL)
                state_ptr->status = WAITING_FOR_SAVING;
        }
        else
        {
            state_ptr->status = SIDE_IDLE;
        }
    }
}

static rotation_func_ptr_type move_none(uint8_t side_num, Side_State *state_ptr)
{
    return NULL;
}


static rotation_func_ptr_type rotation_x_dir_ccw_1(uint8_t side_num, Side_State *state_ptr)
{
    test_1(side_num, state_ptr);
    return rotation_x_dir_ccw_2;
}
static rotation_func_ptr_type rotation_x_dir_ccw_2(uint8_t side_num, Side_State *state_ptr)
{
    test_2(side_num, state_ptr);
    return rotation_x_dir_ccw_f;
}
static rotation_func_ptr_type rotation_x_dir_ccw_f(uint8_t side_num, Side_State *state_ptr)
{
    test_3(side_num, state_ptr);
    return NULL;
}

static rotation_func_ptr_type rotation_x_dir_cw_1(uint8_t side_num, Side_State *state_ptr)
{
    test_1(side_num, state_ptr);
    return rotation_x_dir_cw_2;
}
static rotation_func_ptr_type rotation_x_dir_cw_2(uint8_t side_num, Side_State *state_ptr)
{
    test_2(side_num, state_ptr);
    return rotation_x_dir_cw_f;
}
static rotation_func_ptr_type rotation_x_dir_cw_f(uint8_t side_num, Side_State *state_ptr)
{
    test_3(side_num, state_ptr);
    return NULL;
}

static rotation_func_ptr_type rotation_y_dir_ccw_1(uint8_t side_num, Side_State *state_ptr)
{
    test_1(side_num, state_ptr);
    return rotation_y_dir_ccw_2;
}
static rotation_func_ptr_type rotation_y_dir_ccw_2(uint8_t side_num, Side_State *state_ptr)
{
    test_2(side_num, state_ptr);
    return rotation_y_dir_ccw_f;
}
static rotation_func_ptr_type rotation_y_dir_ccw_f(uint8_t side_num, Side_State *state_ptr)
{
    test_3(side_num, state_ptr);
    return NULL;
}

static rotation_func_ptr_type rotation_y_dir_cw_1(uint8_t side_num, Side_State *state_ptr)
{
    test_1(side_num, state_ptr);
    return rotation_y_dir_cw_2;
}
static rotation_func_ptr_type rotation_y_dir_cw_2(uint8_t side_num, Side_State *state_ptr)
{
    test_2(side_num, state_ptr);
    return rotation_y_dir_cw_f;
}
static rotation_func_ptr_type rotation_y_dir_cw_f(uint8_t side_num, Side_State *state_ptr)
{
    test_3(side_num, state_ptr);
    return NULL;
}

static rotation_func_ptr_type rotation_z_dir_ccw_1(uint8_t side_num, Side_State *state_ptr)
{
    test_1(side_num, state_ptr);
    return rotation_z_dir_ccw_2;
}
static rotation_func_ptr_type rotation_z_dir_ccw_2(uint8_t side_num, Side_State *state_ptr)
{
    test_2(side_num, state_ptr);
    return rotation_z_dir_ccw_f;
}
static rotation_func_ptr_type rotation_z_dir_ccw_f(uint8_t side_num, Side_State *state_ptr)
{
    test_3(side_num, state_ptr);
    return NULL;
}

static rotation_func_ptr_type rotation_z_dir_cw_1(uint8_t side_num, Side_State *state_ptr)
{
    test_1(side_num, state_ptr);
    return rotation_z_dir_cw_2;
}
static rotation_func_ptr_type rotation_z_dir_cw_2(uint8_t side_num, Side_State *state_ptr)
{
    test_2(side_num, state_ptr);
    return rotation_z_dir_cw_f;
}
static rotation_func_ptr_type rotation_z_dir_cw_f(uint8_t side_num, Side_State *state_ptr)
{
    test_3(side_num, state_ptr);
    return NULL;
}

/*static rotation_func_ptr_type rotation_xyz_dir_ccw(uint8_t side_num, Side_State *state_ptr)
{
}
static rotation_func_ptr_type rotation_xyz_dir_cw(uint8_t side_num, Side_State *state_ptr)
{
}

static rotation_func_ptr_type rotation_xy_z_dir_ccw(uint8_t side_num, Side_State *state_ptr)
{
}
static rotation_func_ptr_type rotation_xy_z_dir_cw(uint8_t side_num, Side_State *state_ptr)
{
}

static rotation_func_ptr_type rotation_x_yz_dir_ccw(uint8_t side_num, Side_State *state_ptr)
{
}
static rotation_func_ptr_type rotation_x_yz_dir_cw(uint8_t side_num, Side_State *state_ptr)
{
}

static rotation_func_ptr_type rotation_x_y_z_dir_ccw(uint8_t side_num, Side_State *state_ptr)
{
}
static rotation_func_ptr_type rotation_x_y_z_dir_cw(uint8_t side_num, Side_State *state_ptr)
{
}

static rotation_func_ptr_type rotation_xy_dir_ccw(uint8_t side_num, Side_State *state_ptr)
{
}
static rotation_func_ptr_type rotation_xy_dir_cw(uint8_t side_num, Side_State *state_ptr)
{
}

static rotation_func_ptr_type rotation_x_y_dir_ccw(uint8_t side_num, Side_State *state_ptr)
{
}
static rotation_func_ptr_type rotation_x_y_dir_cw(uint8_t side_num, Side_State *state_ptr)
{
}

static rotation_func_ptr_type rotation_xz_dir_ccw(uint8_t side_num, Side_State *state_ptr)
{
}
static rotation_func_ptr_type rotation_xz_dir_cw(uint8_t side_num, Side_State *state_ptr)
{
}

static rotation_func_ptr_type rotation_x_z_dir_ccw(uint8_t side_num, Side_State *state_ptr)
{
}
static rotation_func_ptr_type rotation_x_z_dir_cw(uint8_t side_num, Side_State *state_ptr)
{
}

static rotation_func_ptr_type rotation_yz_dir_ccw(uint8_t side_num, Side_State *state_ptr)
{
}
static rotation_func_ptr_type rotation_yz_dir_cw(uint8_t side_num, Side_State *state_ptr)
{
}

static rotation_func_ptr_type rotation_y_z_dir_ccw(uint8_t side_num, Side_State *state_ptr)
{
}
static rotation_func_ptr_type rotation_y_z_dir_cw(uint8_t side_num, Side_State *state_ptr)
{
}*/

static rotation_func_ptr_type move_persp_1(uint8_t side_num, Side_State *state_ptr)
{
    return move_persp_2;
}
static rotation_func_ptr_type move_persp_2(uint8_t side_num, Side_State *state_ptr)
{
    return move_persp_f;
}
static rotation_func_ptr_type move_persp_f(uint8_t side_num, Side_State *state_ptr)
{
    return NULL;
}

static uint8_t is_side_left(uint8_t side_num)
{
    return ORIENTATION_MATRIX[side_num] == LEFT;
}
static uint8_t get_adjacent_left_side(uint8_t side_num)
{
    return ADJACENT_SIDES_MATRIX[side_num][0];
}
static uint8_t get_adjacent_right_side(uint8_t side_num)
{
    return ADJACENT_SIDES_MATRIX[side_num][1];
}
static uint8_t get_adjacent_down_side(uint8_t side_num)
{
    return ADJACENT_SIDES_MATRIX[side_num][2];
}
static uint8_t get_adjacent_up_side(uint8_t side_num)
{
    return ADJACENT_SIDES_MATRIX[side_num][3];
}
static const uint8_t colors_1[] = {0, 0, 0, 
                                   1, 1, 1, 
                                   2, 2, 2, 0, 0, 0, 
                                            1, 1, 1, 
                                            2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0};
static const uint8_t colors_2[] = {0, 0, 1, 
                                   0, 1, 2, 
                                   1, 2, 2, 0, 0, 1, 
                                            0, 1, 2, 
                                            1, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0};
static const uint8_t colors_3[] = {0, 1, 2, 
                                   0, 1, 2, 
                                   0, 1, 2, 0, 1, 2, 
                                            0, 1, 2, 
                                            0, 1, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0};
static void test_1(uint8_t side_num, Side_State *state_ptr)
{
    uint8_t i, *colors;
    colors = state_ptr->colors;
    for (i = 0; i < SIDE_CUBES_COUNT; i++)
        colors[i] = colors_1[i];
    state_ptr->colors_changed = TRUE;
}
static void test_2(uint8_t side_num, Side_State *state_ptr)
{
    uint8_t i, *colors;
    colors = state_ptr->colors;
    for (i = 0; i < SIDE_CUBES_COUNT; i++)
        colors[i] = colors_2[i];
    state_ptr->colors_changed = TRUE;
}
static void test_3(uint8_t side_num, Side_State *state_ptr)
{
    uint8_t i, *colors;
    colors = state_ptr->colors;
    for (i = 0; i < SIDE_CUBES_COUNT; i++)
        colors[i] = colors_3[i];
    state_ptr->colors_changed = TRUE;
}



