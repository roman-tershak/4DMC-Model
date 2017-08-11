#include "rotation_logic.h"
#include "common.h"
#include "hardware.h"

#define DIM(color) (0x8 | (color))
#define UNDIM(color) ((color) & 0x7)

typedef enum
{
    MoveAndDim, Undim
} Dim_Action;


extern volatile Side_State sides_states[SIDE_COUNT];


static const uint8_t rotation_phase_cycles[][2] = 
{
    {0, 16},  // The slowest set
    {0, 11},  // Middle
    {0, 7},   // Fast
    {0, 4}     // The fastest set
};

static uint8_t rotation_phase_1_cycles = rotation_phase_cycles[0][0];
static uint8_t rotation_phase_f_cycles = rotation_phase_cycles[0][1];


/* Functions for retrieving adjacent sides */

/*
 *                            (side_zr)
 *                       y   x
 *                        \ |
 *                         \| 
 *   (side_yr)        z<----
 *             x___
 *                 |\
 *                 | \z
 *                 y                     z  X   (side_xr)      x  (side_cb)
 * (side_xl)              |Z              \ |                   \
 *          ___Y          |____Y           \|___Y                \ ___Y
 *         |\              \ (side_cf)                            |
 *         | \Z             \X                                    |
 *         vX                                                     vZ
 *                                ^Y
 *                                |
 *                                |
 *                       X         ---->X
 *                       |         \  (side_yl)
 *                       |___Z      Z
 *             (side_zl)  \
 *                         \Y
 *
 */
static const uint8_t ADJACENT_SIDES_MATRIX[][6] = 
{
    //                        Up     Right    Down     Left     Back     Front
    /* SIDE_XL, AXIS_X */ {SIDE_CF, SIDE_YL, SIDE_CB, SIDE_YR, SIDE_ZR, SIDE_ZL},
    /* SIDE_XL, AXIS_Y */ {SIDE_YL, SIDE_ZL, SIDE_YR, SIDE_ZR, SIDE_CB, SIDE_CF},
    /* SIDE_XL, AXIS_Z */ {SIDE_ZL, SIDE_CF, SIDE_ZR, SIDE_CB, SIDE_YR, SIDE_YL},

    /* SIDE_XR, AXIS_X */ {SIDE_CB, SIDE_YR, SIDE_CF, SIDE_YL, SIDE_ZL, SIDE_ZR},
    /* SIDE_XR, AXIS_Y */ {SIDE_YR, SIDE_ZR, SIDE_YL, SIDE_ZL, SIDE_CF, SIDE_CB},
    /* SIDE_XR, AXIS_Z */ {SIDE_ZR, SIDE_CB, SIDE_ZL, SIDE_CF, SIDE_YL, SIDE_YR},

    /* SIDE_YL, AXIS_X */ {SIDE_ZR, SIDE_CB, SIDE_ZL, SIDE_CF, SIDE_XL, SIDE_XR},
    /* SIDE_YL, AXIS_Y */ {SIDE_CB, SIDE_XR, SIDE_CF, SIDE_XL, SIDE_ZL, SIDE_ZR},
    /* SIDE_YL, AXIS_Z */ {SIDE_XR, SIDE_ZR, SIDE_XL, SIDE_ZL, SIDE_CF, SIDE_CB},

    /* SIDE_YR, AXIS_X */ {SIDE_ZL, SIDE_CF, SIDE_ZR, SIDE_CB, SIDE_XR, SIDE_XL},
    /* SIDE_YR, AXIS_Y */ {SIDE_CF, SIDE_XL, SIDE_CB, SIDE_XR, SIDE_ZR, SIDE_ZL},
    /* SIDE_YR, AXIS_Z */ {SIDE_XL, SIDE_ZL, SIDE_XR, SIDE_ZR, SIDE_CB, SIDE_CF},

    /* SIDE_ZL, AXIS_X */ {SIDE_YL, SIDE_XR, SIDE_YR, SIDE_XL, SIDE_CB, SIDE_CF},
    /* SIDE_ZL, AXIS_Y */ {SIDE_XR, SIDE_CF, SIDE_XL, SIDE_CB, SIDE_YR, SIDE_YL},
    /* SIDE_ZL, AXIS_Z */ {SIDE_CF, SIDE_YL, SIDE_CB, SIDE_YR, SIDE_XL, SIDE_XR},

    /* SIDE_ZR, AXIS_X */ {SIDE_YR, SIDE_XL, SIDE_YL, SIDE_XR, SIDE_CF, SIDE_CB},
    /* SIDE_ZR, AXIS_Y */ {SIDE_XL, SIDE_CB, SIDE_XR, SIDE_CF, SIDE_YL, SIDE_YR},
    /* SIDE_ZR, AXIS_Z */ {SIDE_CB, SIDE_YR, SIDE_CF, SIDE_YL, SIDE_XR, SIDE_XL},

    /* SIDE_CF, AXIS_X */ {SIDE_XR, SIDE_ZR, SIDE_XL, SIDE_ZL, SIDE_YR, SIDE_YL},
    /* SIDE_CF, AXIS_Y */ {SIDE_ZR, SIDE_YL, SIDE_ZL, SIDE_YR, SIDE_XL, SIDE_XR},
    /* SIDE_CF, AXIS_Z */ {SIDE_YL, SIDE_XR, SIDE_YR, SIDE_XL, SIDE_ZL, SIDE_ZR},

    /* SIDE_CB, AXIS_X */ {SIDE_XL, SIDE_ZL, SIDE_XR, SIDE_ZR, SIDE_YL, SIDE_YR},
    /* SIDE_CB, AXIS_Y */ {SIDE_ZL, SIDE_YR, SIDE_ZR, SIDE_YL, SIDE_XR, SIDE_XL},
    /* SIDE_CB, AXIS_Z */ {SIDE_YR, SIDE_XL, SIDE_YL, SIDE_XR, SIDE_ZR, SIDE_ZL}
};

static uint8_t* get_adjacent_matrix_indexex(uint8_t side_num, uint8_t orientation)
{
    return ADJACENT_SIDES_MATRIX[ side_num * 3 + orientation ];
}

static uint8_t* get_adjacent_up_side(uint8_t side_num, uint8_t orientation)
{
    uint8_t index = get_adjacent_matrix_indexex(side_num, orientation)[0];
    return sides_states[index].colors;
}
static uint8_t* get_adjacent_right_side(uint8_t side_num, uint8_t orientation)
{
    uint8_t index = get_adjacent_matrix_indexex(side_num, orientation)[1];
    return sides_states[index].colors;
}
static uint8_t* get_adjacent_down_side(uint8_t side_num, uint8_t orientation)
{
    uint8_t index = get_adjacent_matrix_indexex(side_num, orientation)[2];
    return sides_states[index].colors;
}
static uint8_t* get_adjacent_left_side(uint8_t side_num, uint8_t orientation)
{
    uint8_t index = get_adjacent_matrix_indexex(side_num, orientation)[3];
    return sides_states[index].colors;
}
static uint8_t* get_adjacent_back_side(uint8_t side_num, uint8_t orientation)
{
    uint8_t index = get_adjacent_matrix_indexex(side_num, orientation)[4];
    return sides_states[index].colors;
}
static uint8_t* get_adjacent_front_side(uint8_t side_num, uint8_t orientation)
{
    uint8_t index = get_adjacent_matrix_indexex(side_num, orientation)[5];
    return sides_states[index].colors;
}

/* Indexes and functions for rotating one side or just one side layer without adjacent cubies */

/* 
 *   This is one side cubies local orientation. Each side has its own orientation of XYZ axes
 * 
 *            ^Y
 *            |
 *            |  2          3
 *            |
 *            |
 *            |6          7
 *            |  0          1
 *              -------------->X
 *           /
 *          /  4          5
 *         Z
 */

static const uint8_t ROTATION_X_CCW_INDEXES[] = 
{
    0, 2, 6, 4,    // XL
    1, 3, 7, 5     // XR
};
static const uint8_t ROTATION_X_CW_INDEXES[] = 
{
    0, 4, 6, 2,    // XL
    1, 5, 7, 3     // XR
};
static const uint8_t ROTATION_Y_CCW_INDEXES[] = 
{
    0, 4, 5, 1,    // YL
    2, 6, 7, 3     // YR
};
static const uint8_t ROTATION_Y_CW_INDEXES[] = 
{
    0, 1, 5, 4,    // YL
    2, 3, 7, 6     // YR
};
static const uint8_t ROTATION_Z_CCW_INDEXES[] = 
{
    0, 1, 3, 2,    // ZL
    4, 5, 7, 6     // ZR
};
static const uint8_t ROTATION_Z_CW_INDEXES[] = 
{
    0, 2, 3, 1,    // ZL
    4, 6, 7, 5     // ZR
};

static void rotate_1_side_level(uint8_t *cl, uint8_t *indexes, Dim_Action dim_action)
{
    uint8_t tc, i;

    switch (dim_action)
    {
    case MoveAndDim:

        tc = cl[indexes[0]];
        for (i = 0; i < 3; i++)
        {
            cl[indexes[i]] = DIM(cl[indexes[i + 1]]);
        }
        cl[indexes[3]] = DIM(tc);
        break;

    case Undim:

        for (i = 0; i <= 3; i++)
        {
            cl[indexes[i]] = UNDIM(cl[indexes[i]]);
        }
        break;
    }
}

static void rotate_1_side(uint8_t *colors, uint8_t *indexes, Dim_Action dim_action)
{
    rotate_1_side_level(colors, indexes, dim_action);
    rotate_1_side_level(colors, (indexes + 4), dim_action);
}


/* Functions for rotating a round layer adjacent to a particular side according to a given direction  */

/*
 *                                                                  y   x                   
 *                                                                   \ |                    
 *                                                                    \|                    
 *                                                               z<----                     
 *             ^Y                                         x___                              
 *             |    ~3<------                                 |\                            
 *             | 2             3                              | \z                          
 *             |                                              y                     z  X    
 *             |                                                     |Z              \ |    
 *             |                  ^4~                  ___Y          |____Y           \|___Y
 *           6 |           7     /                    |\              \                     
 *         2^  |                /                     | \Z             \X                   
 *         /   | 0             1                      vX                                    
 *        /     ---------------->X                                           ^Y             
 *           /                                                               |              
 *          /                                                                |              
 *         / 4             5                                        X         ---->X        
 *        Z     1<------                                            |         \             
 *                                                                  |___Z      Z            
 *                                                                   \                      
 *                                                                    \Y                    
 */
static void rotate_adjacent_layer_x_ccw(uint8_t side_num)
{
    uint8_t *uc, *rc, *dc, *lc;
    uint8_t t1, t2;

    uc = get_adjacent_up_side(side_num, AXIS_X);
    rc = get_adjacent_right_side(side_num, AXIS_X);
    dc = get_adjacent_down_side(side_num, AXIS_X);
    lc = get_adjacent_left_side(side_num, AXIS_X);

    t1=uc[0]; uc[0]=uc[1]; 
    t2=uc[4]; uc[4]=uc[5];

    uc[1]=rc[0]; rc[0]=rc[4]; 
    uc[5]=rc[2]; rc[2]=rc[6];

    rc[6]=dc[2]; dc[2]=dc[3];
    rc[4]=dc[6]; dc[6]=dc[7];

    dc[3]=lc[1]; lc[1]=lc[5]; lc[5]=t2;
    dc[7]=lc[3]; lc[3]=lc[7]; lc[7]=t1;
}
/* 
 *                                                                         y   x                   
 *                                                                          \ |                    
 *                                                                           \|                    
 *                  ------->3                                           z<----                     
 *             ^Y                                                x___                              
 *             |                                                     |\                            
 *             | 2             3                                     | \z                          
 *             |                                                     y                     z  X    
 *             |                                                            |Z              \ |    
 *             |                       /                      ___Y          |____Y           \|___Y
 *           6 |           7          /                      |\              \                     
 *         /   |                   ~2V                       | \Z             \X                   
 *        /    | 0             1                             vX                                    
 *     ~4V      ---------------->X                                                  ^Y             
 *           /                                                                      |              
 *          /                                                                       |              
 *         / 4             5                                               X         ---->X        
 *        Z                                                                |         \             
 *              ------->1                                                  |___Z      Z            
 *                                                                          \                      
 *                                                                           \Y                    
 */
static void rotate_adjacent_layer_x_cw(uint8_t side_num)
{
    uint8_t *uc, *rc, *dc, *lc;
    uint8_t t1, t2;

    uc = get_adjacent_up_side(side_num, AXIS_X);
    rc = get_adjacent_right_side(side_num, AXIS_X);
    dc = get_adjacent_down_side(side_num, AXIS_X);
    lc = get_adjacent_left_side(side_num, AXIS_X);

    t1=uc[1]; uc[1]=uc[0];
    t2=uc[5]; uc[5]=uc[4];

    uc[4]=lc[5]; lc[5]=lc[1];
    uc[0]=lc[7]; lc[7]=lc[3];
    
    lc[1]=dc[3]; dc[3]=dc[2];
    lc[3]=dc[7]; dc[7]=dc[6];
    
    dc[6]=rc[4]; rc[4]=rc[0]; rc[0]=t1;
    dc[2]=rc[6]; rc[6]=rc[2]; rc[2]=t2;
}
/* 
 *                                                                          y   x                   
 *                                                                           \ |                    
 *                                                                            \|                    
 *                                                                       z<----                     
 *                                                                x___                              
 *             ^Y                                                     |\                            
 *             |                                                      | \z                          
 *             | 2 ^~2         3                                      y                     z  X    
 *        1^   |   |                                                         |Z              \ |    
 *         |   |   |                ^3                         ___Y          |____Y           \|___Y
 *         |   |   |                |                         |\              \                     
 *         | 6 |           7        |                         | \Z             \X                   
 *             |                    |                         vX                                    
 *             | 0             1                                                     ^Y             
 *              ---------------->X                                                   |              
 *           /   ^~4                                                                 |              
 *          /    |                                                          X         ---->X        
 *         / 4   |         5                                                |         \             
 *        Z      |                                                          |___Z      Z            
 *                                                                           \                      
 *                                                                            \Y                    
 */
static void rotate_adjacent_layer_y_ccw(uint8_t side_num)
{
    uint8_t *uc, *rc, *dc, *lc;
    uint8_t t1, t2;

    uc = get_adjacent_up_side(side_num, AXIS_Y);
    rc = get_adjacent_right_side(side_num, AXIS_Y);
    dc = get_adjacent_down_side(side_num, AXIS_Y);
    lc = get_adjacent_left_side(side_num, AXIS_Y);

    t1=uc[2]; uc[2]=uc[0];
    t2=uc[6]; uc[6]=uc[4];

    uc[4]=rc[2]; rc[2]=rc[0];
    uc[0]=rc[3]; rc[3]=rc[1];

    rc[0]=dc[3]; dc[3]=dc[1];
    rc[1]=dc[7]; dc[7]=dc[5];

    dc[5]=lc[6]; lc[6]=lc[4]; lc[4]=t1;
    dc[1]=lc[7]; lc[7]=lc[5]; lc[5]=t2;
}
/* 
 *                                                                      y   x                   
 *                                                                       \ |                    
 *                                                                        \|                    
 *                                                                   z<----                     
 *                                                            x___                              
 *             ^Y                                                 |\                            
 *             |                                                  | \z                          
 *             | 2 |           3                                  y                     z  X    
 *             |   |                                                     |Z              \ |    
 *         |   |   |                                       ___Y          |____Y           \|___Y
 *         |   |   V4~              |                     |\              \                     
 *         | 6 |           7        |                     | \Z             \X                   
 *        1V   |                    |                     vX                                    
 *             | 0             1    V~3                                          ^Y             
 *              ---------------->X                                               |              
 *           /                                                                   |              
 *          /    |                                                      X         ---->X        
 *         / 4   |         5                                            |         \             
 *        Z      |                                                      |___Z      Z            
 *               V2                                                      \                      
 *                                                                        \Y                    
 */
static void rotate_adjacent_layer_y_cw(uint8_t side_num)
{
    uint8_t *uc, *rc, *dc, *lc;
    uint8_t t1, t2;

    uc = get_adjacent_up_side(side_num, AXIS_Y);
    rc = get_adjacent_right_side(side_num, AXIS_Y);
    dc = get_adjacent_down_side(side_num, AXIS_Y);
    lc = get_adjacent_left_side(side_num, AXIS_Y);

    t1=uc[0]; uc[0]=uc[2];
    t2=uc[4]; uc[4]=uc[6];

    uc[2]=lc[4]; lc[4]=lc[6];
    uc[6]=lc[5]; lc[5]=lc[7];

    lc[7]=dc[1]; dc[1]=dc[3];
    lc[6]=dc[5]; dc[5]=dc[7];

    dc[3]=rc[0]; rc[0]=rc[2]; rc[2]=t2;
    dc[7]=rc[1]; rc[1]=rc[3]; rc[3]=t1;
}
/* 
 *                                                                     y   x                   
 *                                                                      \ |                    
 *                                                                       \|                    
 *                   ^4~                                            z<----                     
 *                  /                                        x___                              
 *             ^Y  /                                             |\                            
 *             |  /                                              | \z                          
 *             | 2             3                                 y                     z  X    
 *             |   1<-------                                            |Z              \ |    
 *             |                                          ___Y          |____Y           \|___Y
 *             |                                         |\              \                     
 *           6 |           7                             | \Z             \X                   
 *             |                                         vX                                    
 *             | 0             1                                                ^Y             
 *              ---------------->X                                              |              
 *           /                                                                  |              
 *          /    ^2                                                    X         ---->X        
 *         / 4  /          5                                           |         \             
 *        Z    /                                                       |___Z      Z            
 *            /                                                         \                      
 *           ~3<-------                                                  \Y                    
 * 
 */
static void rotate_adjacent_layer_z_ccw(uint8_t side_num)
{
    uint8_t *uc, *rc, *dc, *lc;
    uint8_t t1, t2;

    uc = get_adjacent_up_side(side_num, AXIS_Z);
    rc = get_adjacent_right_side(side_num, AXIS_Z);
    dc = get_adjacent_down_side(side_num, AXIS_Z);
    lc = get_adjacent_left_side(side_num, AXIS_Z);

    t1=uc[0]; uc[0]=uc[1]; 
    t2=uc[2]; uc[2]=uc[3];

    uc[1]=rc[0]; rc[0]=rc[4]; 
    uc[3]=rc[1]; rc[1]=rc[5];

    rc[5]=dc[4]; dc[4]=dc[5];
    rc[4]=dc[6]; dc[6]=dc[7];

    dc[5]=lc[2]; lc[2]=lc[6]; lc[6]=t2;
    dc[7]=lc[3]; lc[3]=lc[7]; lc[7]=t1;
}
/* 
 *                                                                      y   x                   
 *                                                                       \ |                    
 *                                                                        \|                    
 *                   /                                               z<----                     
 *                  /                                         x___                              
 *             ^Y  /                                              |\                            
 *             |  V~2                                             | \z                          
 *             | 2             3                                  y                     z  X    
 *             |   ------->1                                             |Z              \ |    
 *             |                                           ___Y          |____Y           \|___Y
 *             |                                          |\              \                     
 *           6 |           7                              | \Z             \X                   
 *             |                                          vX                                    
 *             | 0             1                                                 ^Y             
 *              ---------------->X                                               |              
 *           /                                                                   |              
 *          /    /                                                      X         ---->X        
 *         / 4  /          5                                            |         \             
 *        Z    /                                                        |___Z      Z            
 *            V~4                                                        \                      
 *                                                                        \Y
 *        ------->3
 * 
 */
static void rotate_adjacent_layer_z_cw(uint8_t side_num)
{
    uint8_t *uc, *rc, *dc, *lc;
    uint8_t t1, t2;

    uc = get_adjacent_up_side(side_num, AXIS_Z);
    rc = get_adjacent_right_side(side_num, AXIS_Z);
    dc = get_adjacent_down_side(side_num, AXIS_Z);
    lc = get_adjacent_left_side(side_num, AXIS_Z);

    t1=uc[1]; uc[1]=uc[0]; 
    t2=uc[3]; uc[3]=uc[2];

    uc[2]=lc[6]; lc[6]=lc[2]; 
    uc[0]=lc[7]; lc[7]=lc[3];

    lc[2]=dc[5]; dc[5]=dc[4];
    lc[3]=dc[7]; dc[7]=dc[6];

    dc[6]=rc[4]; rc[4]=rc[0]; rc[0]=t1;
    dc[4]=rc[5]; rc[5]=rc[1]; rc[1]=t2;
}


/* Some auxiliary functions */

static Dim_Action get_dim_action(uint8_t cycle_ct)
{
    if (cycle_ct == rotation_phase_1_cycles) 
        return MoveAndDim;
    else 
        return Undim;
}

static void rotate(uint8_t side_num, uint8_t *indexes_m, uint8_t *indexes_bf, uint8_t rotation_axis, void (*rotate_adjacent_layer_ptr)(uint8_t side_num))
{
    Side_State *state_ptr;
    uint8_t *bc, *fc;
    Dim_Action dim_action;

    state_ptr = &(sides_states[side_num]);
    dim_action = get_dim_action(state_ptr->cycle_ct);

    rotate_1_side(state_ptr->colors, indexes_m, dim_action);

    rotate_adjacent_layer_ptr(side_num);

    bc = get_adjacent_back_side(side_num, rotation_axis);
    fc = get_adjacent_front_side(side_num, rotation_axis);
    rotate_1_side_level(bc, (indexes_bf + 4), dim_action);
    rotate_1_side_level(fc, indexes_bf, dim_action);
}


/* Main rotation functions */

/*
 *                     y   x
 *                      \ |
 *                       \| 
 *                  z<----
 *           x___
 *               |\
 *               | \z
 *               y                     z  X
 *                      |Z              \ |
 *        ___Y          |____Y           \|___Y
 *       |\              \
 *       | \Z             \X
 *       vX
 *                              ^Y
 *                              |
 *                              |
 *                     X         ---->X
 *                     |         \
 *                     |___Z      Z
 *                      \
 *                       \Y
 *
 */
static void rotate_x_dir_ccw(uint8_t side_num)
{
    rotate(side_num, ROTATION_X_CCW_INDEXES, ROTATION_Z_CCW_INDEXES, AXIS_X, rotate_adjacent_layer_x_ccw);
}
static void rotate_x_dir_cw(uint8_t side_num)
{
    rotate(side_num, ROTATION_X_CW_INDEXES, ROTATION_Z_CW_INDEXES, AXIS_X, rotate_adjacent_layer_x_cw);
}
static void rotate_y_dir_ccw(uint8_t side_num)
{
    rotate(side_num, ROTATION_Y_CCW_INDEXES, ROTATION_Y_CCW_INDEXES, AXIS_Y, rotate_adjacent_layer_y_ccw);
}
static void rotate_y_dir_cw(uint8_t side_num)
{
    rotate(side_num, ROTATION_Y_CW_INDEXES, ROTATION_Y_CW_INDEXES, AXIS_Y, rotate_adjacent_layer_y_cw);
}
static void rotate_z_dir_ccw(uint8_t side_num)
{
    rotate(side_num, ROTATION_Z_CCW_INDEXES, ROTATION_X_CCW_INDEXES, AXIS_Z, rotate_adjacent_layer_z_ccw);
}
static void rotate_z_dir_cw(uint8_t side_num)
{
    rotate(side_num, ROTATION_Z_CW_INDEXES, ROTATION_X_CW_INDEXES, AXIS_Z, rotate_adjacent_layer_z_cw);
}

/* Matrix of rotation and perspective moving functions */

/* 
 *               Sensors orientation                                         Cube sides orientation
 *                                     
 * 
 *                          (side_zr)                                                          (side_zr)                  
 *                         Z                                                              y   x                           
 *                        |___Y                                                            \ |                            
 *                         \                                                                \|                            
 *                          vX                                        (side_yr)        z<----                             
 *           (side_yr) z y                                                      x___                                      
 *                      \|                                                          |\                                    
 *                   x<--                                                           | \z                                  
 * (side_xl)                               ^X (side_xr)                             y                     z  X   (side_xr)
 *      Z ___                              |                        (side_xl)              |Z              \ |            
 *           |\                            |___Z                             ___Y          |____Y           \|___Y        
 *           | Y            |Y              \                               |\              \ (side_cf)                   
 *           vX             |                Y                              | \Z             \X                           
 *                           ---->X                                         vX                                            
 *                           \   (side_yl)                                                         ^Y                     
 *                            Z                                                                    |                      
 *                       x^                                                                        |                      
 *                         \___Y                                                          X         ---->X                
 *                         |                                                              |         \  (side_yl)          
 *               (side_zl)  Z                                                             |___Z      Z                    
 *                                                                              (side_zl)  \                              
 *                                                                                          \Y                            
 *                                     
 */

static const rotation_func_ptr_type ROTATION_FUNC_MATRIX[] = 
{
    /* SIDE_XL , (ROTATION_X | DIR_CCW)  */ rotate_x_dir_ccw,
    /* SIDE_XL , (ROTATION_X | DIR_CW)   */ rotate_x_dir_cw,
    /* SIDE_XL , (ROTATION_Y | DIR_CCW)  */ rotate_z_dir_ccw,
    /* SIDE_XL , (ROTATION_Y | DIR_CW)   */ rotate_z_dir_cw,
    /* SIDE_XL , (ROTATION_Z | DIR_CCW)  */ rotate_y_dir_cw,
    /* SIDE_XL , (ROTATION_Z | DIR_CW)   */ rotate_y_dir_ccw,

    /* SIDE_XR , (ROTATION_X | DIR_CCW)  */ rotate_x_dir_ccw,
    /* SIDE_XR , (ROTATION_X | DIR_CW)   */ rotate_x_dir_cw,
    /* SIDE_XR , (ROTATION_Y | DIR_CCW)  */ rotate_z_dir_cw,
    /* SIDE_XR , (ROTATION_Y | DIR_CW)   */ rotate_z_dir_ccw,
    /* SIDE_XR , (ROTATION_Z | DIR_CCW)  */ rotate_y_dir_ccw,
    /* SIDE_XR , (ROTATION_Z | DIR_CW)   */ rotate_y_dir_cw,

    /* SIDE_YL , (ROTATION_X | DIR_CCW)  */ rotate_x_dir_ccw,
    /* SIDE_YL , (ROTATION_X | DIR_CW)   */ rotate_x_dir_cw,
    /* SIDE_YL , (ROTATION_Y | DIR_CCW)  */ rotate_y_dir_ccw,
    /* SIDE_YL , (ROTATION_Y | DIR_CW)   */ rotate_y_dir_cw,
    /* SIDE_YL , (ROTATION_Z | DIR_CCW)  */ rotate_z_dir_ccw,
    /* SIDE_YL , (ROTATION_Z | DIR_CW)   */ rotate_z_dir_cw,

    /* SIDE_YR , (ROTATION_X | DIR_CCW)  */ rotate_x_dir_ccw,
    /* SIDE_YR , (ROTATION_X | DIR_CW)   */ rotate_x_dir_cw,
    /* SIDE_YR , (ROTATION_Y | DIR_CCW)  */ rotate_y_dir_cw,
    /* SIDE_YR , (ROTATION_Y | DIR_CW)   */ rotate_y_dir_ccw,
    /* SIDE_YR , (ROTATION_Z | DIR_CCW)  */ rotate_z_dir_cw,
    /* SIDE_YR , (ROTATION_Z | DIR_CW)   */ rotate_z_dir_ccw,

    /* SIDE_ZL , (ROTATION_X | DIR_CCW)  */ rotate_y_dir_cw,
    /* SIDE_ZL , (ROTATION_X | DIR_CW)   */ rotate_y_dir_ccw,
    /* SIDE_ZL , (ROTATION_Y | DIR_CCW)  */ rotate_z_dir_ccw,
    /* SIDE_ZL , (ROTATION_Y | DIR_CW)   */ rotate_z_dir_cw,
    /* SIDE_ZL , (ROTATION_Z | DIR_CCW)  */ rotate_x_dir_cw,
    /* SIDE_ZL , (ROTATION_Z | DIR_CW)   */ rotate_x_dir_ccw,

    /* SIDE_ZR , (ROTATION_X | DIR_CCW)  */ rotate_y_dir_cw,
    /* SIDE_ZR , (ROTATION_X | DIR_CW)   */ rotate_y_dir_ccw,
    /* SIDE_ZR , (ROTATION_Y | DIR_CCW)  */ rotate_z_dir_cw,
    /* SIDE_ZR , (ROTATION_Y | DIR_CW)   */ rotate_z_dir_ccw,
    /* SIDE_ZR , (ROTATION_Z | DIR_CCW)  */ rotate_x_dir_ccw,
    /* SIDE_ZR , (ROTATION_Z | DIR_CW)   */ rotate_x_dir_cw
};


/* Public functions */
rotation_func_ptr_type get_rotation_func_ptr(uint8_t side_num, uint8_t direction)
{
    return ROTATION_FUNC_MATRIX[side_num * 6 + direction];
}

/* The main rotation function and its counterparts */
static uint8_t can_do_rotation_cycle(uint8_t cycle_ct)
{
    if (cycle_ct == rotation_phase_1_cycles ||
        cycle_ct == rotation_phase_f_cycles)
        return TRUE;
    else
        return FALSE;
}

void rotation_cycle(uint8_t side_num)
{
    Side_State *state_ptr = &(sides_states[side_num]);

    if (can_do_rotation_cycle(state_ptr->cycle_ct))
    {
        ((rotation_func_ptr_type) state_ptr->rotation_func_ptr)(side_num);
        sides_colors_changed();
    }
    state_ptr->cycle_ct++;

    if (state_ptr->cycle_ct > rotation_phase_f_cycles)
        rotation_done(side_num);
}

void change_phase_cycle_counters(uint8_t faster, uint8_t slower)
{
    static uint8_t index = 0;
    int8_t index_d;

    if (faster || slower)
    {
        // Faster takes the priority over slower
        index_d = faster ? (index < 3 ? 1 : 0) : (index > 0 ? -1 : 0);

        if (index_d != 0)
        {
            index += index_d;

            rotation_phase_1_cycles = rotation_phase_cycles[index][0];
            rotation_phase_f_cycles = rotation_phase_cycles[index][1];
        }
    }
}

