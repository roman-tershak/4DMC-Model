#include "rotation_logic.h"
#include "common.h"
#include "hardware.h"

#define DIM(color) (0x8 | (color))
#define UNDIM(color) ((color) & 0x7)

typedef enum
{
    dim, undim, no_change
} Dim_Action;


extern volatile Side_State sides_states[SIDE_COUNT];


static const uint8_t rotation_phase_cycles[][3] = 
{
    {0, 16, 32},  // The slowest set
    {0, 11, 22},  // Middle
    {0, 7, 14},   // Fast
    {0, 4, 8}     // The fastest set
};

static uint8_t rotation_phase_1_cycles =  0;
static uint8_t rotation_phase_2_cycles = 16;
static uint8_t rotation_phase_f_cycles = 32;


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
 *                 y                     z  X   (side_xr)
 * (side_xl)              |Z              \ |
 *          ___Y          |____Y           \|___Y
 *         |\              \ (side_cf)
 *         | \Z             \X
 *         vX
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
    /* SIDE_ZR, AXIS_Z */ {SIDE_CB, SIDE_YR, SIDE_CF, SIDE_YL, SIDE_XR, SIDE_XL}

    /* SIDE_CF */
    /* SIDE_CB */ 
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
 *            |  6      7      8
 *            |
 *            15     16     17
 *            |  3      4      5
 *          24|    25     26
 *            12     13     14
 *            |  0      1      2
 *          21  ---22-----23---->X
 *           / 9     10     11
 *          /
 *         /18     19     20
 *        Z
 */

static const uint8_t ROTATION_X_CCW_INDEXES[] = 
{
    0, 3, 6, 15, 24, 21, 18, 9,     // XL
    1, 4, 7, 16, 25, 22, 19, 10,    // XM
    2, 5, 8, 17, 26, 23, 20, 11     // XR
};
static const uint8_t ROTATION_X_CW_INDEXES[] = 
{
    0, 9, 18, 21, 24, 15, 6, 3,     // XL
    1, 10, 19, 22, 25, 16, 7, 4,    // XM
    2, 11, 20, 23, 26, 17, 8, 5     // XR
};
static const uint8_t ROTATION_Y_CCW_INDEXES[] = 
{
    0, 9, 18, 19, 20, 11, 2, 1,     // YL
    3, 12, 21, 22, 23, 14, 5, 4,    // YM
    6, 15, 24, 25, 26, 17, 8, 7     // YR
};
static const uint8_t ROTATION_Y_CW_INDEXES[] = 
{
    0, 1, 2, 11, 20, 19, 18, 9,     // YL
    3, 4, 5, 14, 23, 22, 21, 12,    // YM
    6, 7, 8, 17, 26, 25, 24, 15     // YR
};
static const uint8_t ROTATION_Z_CCW_INDEXES[] = 
{
    0, 1, 2, 5, 8, 7, 6, 3,         // ZL
    9, 10, 11, 14, 17, 16, 15, 12,  // ZM
    18, 19, 20, 23, 26, 25, 24, 21  // ZR
};
static const uint8_t ROTATION_Z_CW_INDEXES[] = 
{
    0, 3, 6, 7, 8, 5, 2, 1,         // ZL
    9, 12, 15, 16, 17, 14, 11, 10,  // ZM
    18, 21, 24, 25, 26, 23, 20, 19  // ZR
};

static void rotate_1_side_level(uint8_t *cl, uint8_t *indexes, Dim_Action dim_action)
{
    uint8_t tc, i;

    switch (dim_action)
    {
    case dim:

        tc = cl[indexes[0]];
        for (i = 0; i < 7; i++)
        {
            cl[indexes[i]] = DIM(cl[indexes[i + 1]]);
        }
        cl[indexes[7]] = DIM(tc);
        break;

    case undim:

        for (i = 0; i < 8; i++)
        {
            cl[indexes[i]] = UNDIM(cl[indexes[i]]);
        }
        break;

    case no_change:

        tc = cl[indexes[0]];
        for (i = 0; i < 7; i++)
        {
            cl[indexes[i]] = cl[indexes[i + 1]];
        }
        cl[indexes[7]] = tc;
        break;
    }
}

static void rotate_1_side(uint8_t *colors, uint8_t *indexes, Dim_Action dim_action)
{
    rotate_1_side_level(colors, indexes, dim_action);
    rotate_1_side_level(colors, (indexes + 8), dim_action);
    rotate_1_side_level(colors, (indexes + 16), dim_action);
}


/* Functions for rotating a round layer adjacent to a particular side according to a given direction  */

/*
 *                                                                  y   x                   
 *                                                                   \ |                    
 *                                                                    \|                    
 *                                                               z<----                     
 *             ^Y                                         x___                              
 *             |    ~3<------                                 |\                            
 *             | 6      7      8                              | \z                          
 *             |                                              y                     z  X    
 *            15     16     17                                       |Z              \ |    
 *             | 3      4      5  ^4~                  ___Y          |____Y           \|___Y
 *          24 |   25     26     /                    |\              \                     
 *         2^ 12     13     14  /                     | \Z             \X                   
 *         /   | 0      1      2                      vX                                    
 *        / 21  ---22-----23---->X                                           ^Y             
 *           / 9     10     11                                               |              
 *          /                                                                |              
 *         /18     19     20                                        X         ---->X        
 *        Z     1<------                                            |         \             
 *                                                                  |___Z      Z            
 *                                                                   \                      
 *                                                                    \Y                    
 */
static void rotate_adjacent_layer_x_ccw(uint8_t side_num)
{
    uint8_t *uc, *rc, *dc, *lc;
    uint8_t t1, t2, t3;

    uc = get_adjacent_up_side(side_num, AXIS_X);
    rc = get_adjacent_right_side(side_num, AXIS_X);
    dc = get_adjacent_down_side(side_num, AXIS_X);
    lc = get_adjacent_left_side(side_num, AXIS_X);

    t1=uc[ 0]; uc[ 0]=uc[ 1]; uc[ 1]=uc[ 2]; 
    t2=uc[ 9]; uc[ 9]=uc[10]; uc[10]=uc[11]; 
    t3=uc[18]; uc[18]=uc[19]; uc[19]=uc[20];

    uc[ 2]=rc[0]; rc[0]=rc[ 9]; rc[ 9]=rc[18]; 
    uc[11]=rc[3]; rc[3]=rc[12]; rc[12]=rc[21]; 
    uc[20]=rc[6]; rc[6]=rc[15]; rc[15]=rc[24];

    rc[24]=dc[ 6]; dc[ 6]=dc[ 7]; dc[ 7]=dc[ 8];
    rc[21]=dc[15]; dc[15]=dc[16]; dc[16]=dc[17];
    rc[18]=dc[24]; dc[24]=dc[25]; dc[25]=dc[26];

    dc[ 8]=lc[2]; lc[2]=lc[11]; lc[11]=lc[20]; lc[20]=t3;
    dc[17]=lc[5]; lc[5]=lc[14]; lc[14]=lc[23]; lc[23]=t2; 
    dc[26]=lc[8]; lc[8]=lc[17]; lc[17]=lc[26]; lc[26]=t1;
}
/* 
 *                                                                         y   x                   
 *                                                                          \ |                    
 *                                                                           \|                    
 *                  ------->3                                           z<----                     
 *             ^Y                                                x___                              
 *             |                                                     |\                            
 *             | 6      7      8                                     | \z                          
 *             |                                                     y                     z  X    
 *            15     16     17                                              |Z              \ |    
 *             | 3      4      5       /                      ___Y          |____Y           \|___Y
 *          24 |   25     26          /                      |\              \                     
 *         /  12     13     14     ~2V                       | \Z             \X                   
 *        /    | 0      1      2                             vX                                    
 *     ~4V  21  ---22-----23---->X                                                  ^Y             
 *           / 9     10     11                                                      |              
 *          /                                                                       |              
 *         /18     19     20                                               X         ---->X        
 *        Z                                                                |         \             
 *              ------->1                                                  |___Z      Z            
 *                                                                          \                      
 *                                                                           \Y                    
 */
static void rotate_adjacent_layer_x_cw(uint8_t side_num)
{
    uint8_t *uc, *rc, *dc, *lc;
    uint8_t t1, t2, t3;

    uc = get_adjacent_up_side(side_num, AXIS_X);
    rc = get_adjacent_right_side(side_num, AXIS_X);
    dc = get_adjacent_down_side(side_num, AXIS_X);
    lc = get_adjacent_left_side(side_num, AXIS_X);

    t1=uc[ 2]; uc[ 2]=uc[ 1]; uc[ 1]=uc[ 0];
    t2=uc[11]; uc[11]=uc[10]; uc[10]=uc[ 9];
    t3=uc[20]; uc[20]=uc[19]; uc[19]=uc[18];

    uc[18]=lc[20]; lc[20]=lc[11]; lc[11]=lc[2];
    uc[ 9]=lc[23]; lc[23]=lc[14]; lc[14]=lc[5];
    uc[ 0]=lc[26]; lc[26]=lc[17]; lc[17]=lc[8];
    
    lc[2]=dc[ 8]; dc[ 8]=dc[ 7]; dc[ 7]=dc[ 6];
    lc[5]=dc[17]; dc[17]=dc[16]; dc[16]=dc[15];
    lc[8]=dc[26]; dc[26]=dc[25]; dc[25]=dc[24];
    
    dc[24]=rc[18]; rc[18]=rc[ 9]; rc[ 9]=rc[0]; rc[0]=t1;
    dc[15]=rc[21]; rc[21]=rc[12]; rc[12]=rc[3]; rc[3]=t2;
    dc[ 6]=rc[24]; rc[24]=rc[15]; rc[15]=rc[6]; rc[6]=t3;
}
/* 
 *                                                                          y   x                   
 *                                                                           \ |                    
 *                                                                            \|                    
 *                                                                       z<----                     
 *                                                                x___                              
 *             ^Y                                                     |\                            
 *             |                                                      | \z                          
 *             | 6 ^~2  7      8                                      y                     z  X    
 *        1^   |   |                                                         |Z              \ |    
 *         |  15   | 16     17      ^3                         ___Y          |____Y           \|___Y
 *         |   | 3 |    4      5    |                         |\              \                     
 *         |24 |   25     26        |                         | \Z             \X                   
 *            12     13     14      |                         vX                                    
 *             | 0      1      2                                                     ^Y             
 *          21  ---22-----23---->X                                                   |              
 *           / 9 ^~4 10     11                                                       |              
 *          /    |                                                          X         ---->X        
 *         /18   | 19     20                                                |         \             
 *        Z      |                                                          |___Z      Z            
 *                                                                           \                      
 *                                                                            \Y                    
 */
static void rotate_adjacent_layer_y_ccw(uint8_t side_num)
{
    uint8_t *uc, *rc, *dc, *lc;
    uint8_t t1, t2, t3;

    uc = get_adjacent_up_side(side_num, AXIS_Y);
    rc = get_adjacent_right_side(side_num, AXIS_Y);
    dc = get_adjacent_down_side(side_num, AXIS_Y);
    lc = get_adjacent_left_side(side_num, AXIS_Y);

    t1=uc[ 6]; uc[ 6]=uc[ 3]; uc[ 3]=uc[ 0];
    t2=uc[15]; uc[15]=uc[12]; uc[12]=uc[ 9];
    t3=uc[24]; uc[24]=uc[21]; uc[21]=uc[18];

    uc[18]=rc[6]; rc[6]=rc[3]; rc[3]=rc[0];
    uc[ 9]=rc[7]; rc[7]=rc[4]; rc[4]=rc[1];
    uc[ 0]=rc[8]; rc[8]=rc[5]; rc[5]=rc[2];

    rc[0]=dc[ 8]; dc[ 8]=dc[ 5]; dc[ 5]=dc[ 2];
    rc[1]=dc[17]; dc[17]=dc[14]; dc[14]=dc[11];
    rc[2]=dc[26]; dc[26]=dc[23]; dc[23]=dc[20];

    dc[20]=lc[24]; lc[24]=lc[21]; lc[21]=lc[18]; lc[18]=t1;
    dc[11]=lc[25]; lc[25]=lc[22]; lc[22]=lc[19]; lc[19]=t2;
    dc[ 2]=lc[26]; lc[26]=lc[23]; lc[23]=lc[20]; lc[20]=t3;
}
/* 
 *                                                                      y   x                   
 *                                                                       \ |                    
 *                                                                        \|                    
 *                                                                   z<----                     
 *                                                            x___                              
 *             ^Y                                                 |\                            
 *             |                                                  | \z                          
 *             | 6 |    7      8                                  y                     z  X    
 *             |   |                                                     |Z              \ |    
 *         |  15   | 16     17                             ___Y          |____Y           \|___Y
 *         |   | 3 V4~  4      5    |                     |\              \                     
 *         |24 |   25     26        |                     | \Z             \X                   
 *        1V  12     13     14      |                     vX                                    
 *             | 0      1      2    V~3                                          ^Y             
 *          21  ---22-----23---->X                                               |              
 *           / 9     10     11                                                   |              
 *          /    |                                                      X         ---->X        
 *         /18   | 19     20                                            |         \             
 *        Z      |                                                      |___Z      Z            
 *               V2                                                      \                      
 *                                                                        \Y                    
 */
static void rotate_adjacent_layer_y_cw(uint8_t side_num)
{
    uint8_t *uc, *rc, *dc, *lc;
    uint8_t t1, t2, t3;

    uc = get_adjacent_up_side(side_num, AXIS_Y);
    rc = get_adjacent_right_side(side_num, AXIS_Y);
    dc = get_adjacent_down_side(side_num, AXIS_Y);
    lc = get_adjacent_left_side(side_num, AXIS_Y);

    t1=uc[ 0]; uc[ 0]=uc[ 3]; uc[ 3]=uc[ 6];
    t2=uc[ 9]; uc[ 9]=uc[12]; uc[12]=uc[15];
    t3=uc[18]; uc[18]=uc[21]; uc[21]=uc[24];

    uc[ 6]=lc[18]; lc[18]=lc[21]; lc[21]=lc[24];
    uc[15]=lc[19]; lc[19]=lc[22]; lc[22]=lc[25];
    uc[24]=lc[20]; lc[20]=lc[23]; lc[23]=lc[26];

    lc[26]=dc[ 2]; dc[ 2]=dc[ 5]; dc[ 5]=dc[ 8];
    lc[25]=dc[11]; dc[11]=dc[14]; dc[14]=dc[17];
    lc[24]=dc[20]; dc[20]=dc[23]; dc[23]=dc[26];

    dc[ 8]=rc[0]; rc[0]=rc[3]; rc[3]=rc[6]; rc[6]=t3;
    dc[17]=rc[1]; rc[1]=rc[4]; rc[4]=rc[7]; rc[7]=t2;
    dc[26]=rc[2]; rc[2]=rc[5]; rc[5]=rc[8]; rc[8]=t1;
}
/* 
 *                                                                     y   x                   
 *                                                                      \ |                    
 *                                                                       \|                    
 *                   ^4~                                            z<----                     
 *                  /                                        x___                              
 *             ^Y  /                                             |\                            
 *             |  /                                              | \z                          
 *             | 6      7      8                                 y                     z  X    
 *             |   1<-------                                            |Z              \ |    
 *            15     16     17                            ___Y          |____Y           \|___Y
 *             | 3      4      5                         |\              \                     
 *          24 |   25     26                             | \Z             \X                   
 *            12     13     14                           vX                                    
 *             | 0      1      2                                                ^Y             
 *          21  ---22-----23---->X                                              |              
 *           / 9     10     11                                                  |              
 *          /    ^2                                                    X         ---->X        
 *         /18  /  19     20                                           |         \             
 *        Z    /                                                       |___Z      Z            
 *            /                                                         \                      
 *           ~3<-------                                                  \Y                    
 * 
 */
static void rotate_adjacent_layer_z_ccw(uint8_t side_num)
{
    uint8_t *uc, *rc, *dc, *lc;
    uint8_t t1, t2, t3;

    uc = get_adjacent_up_side(side_num, AXIS_Z);
    rc = get_adjacent_right_side(side_num, AXIS_Z);
    dc = get_adjacent_down_side(side_num, AXIS_Z);
    lc = get_adjacent_left_side(side_num, AXIS_Z);

    t1=uc[0]; uc[0]=uc[1]; uc[1]=uc[2]; 
    t2=uc[3]; uc[3]=uc[4]; uc[4]=uc[5]; 
    t3=uc[6]; uc[6]=uc[7]; uc[7]=uc[8];

    uc[2]=rc[0]; rc[0]=rc[ 9]; rc[ 9]=rc[18]; 
    uc[5]=rc[1]; rc[1]=rc[10]; rc[10]=rc[19]; 
    uc[8]=rc[2]; rc[2]=rc[11]; rc[11]=rc[20];

    rc[20]=dc[18]; dc[18]=dc[19]; dc[19]=dc[20];
    rc[19]=dc[21]; dc[21]=dc[22]; dc[22]=dc[23];
    rc[18]=dc[24]; dc[24]=dc[25]; dc[25]=dc[26];

    dc[20]=lc[6]; lc[6]=lc[15]; lc[15]=lc[24]; lc[24]=t3;
    dc[23]=lc[7]; lc[7]=lc[16]; lc[16]=lc[25]; lc[25]=t2; 
    dc[26]=lc[8]; lc[8]=lc[17]; lc[17]=lc[26]; lc[26]=t1;
}
/* 
 *                                                                      y   x                   
 *                                                                       \ |                    
 *                                                                        \|                    
 *                   /                                               z<----                     
 *                  /                                         x___                              
 *             ^Y  /                                              |\                            
 *             |  V~2                                             | \z                          
 *             | 6      7      8                                  y                     z  X    
 *             |   ------->1                                             |Z              \ |    
 *            15     16     17                             ___Y          |____Y           \|___Y
 *             | 3      4      5                          |\              \                     
 *          24 |   25     26                              | \Z             \X                   
 *            12     13     14                            vX                                    
 *             | 0      1      2                                                 ^Y             
 *          21  ---22-----23---->X                                               |              
 *           / 9     10     11                                                   |              
 *          /    /                                                      X         ---->X        
 *         /18  /  19     20                                            |         \             
 *        Z    /                                                        |___Z      Z            
 *            V~4                                                        \                      
 *                                                                        \Y
 *        ------->3
 * 
 */
static void rotate_adjacent_layer_z_cw(uint8_t side_num)
{
    uint8_t *uc, *rc, *dc, *lc;
    uint8_t t1, t2, t3;

    uc = get_adjacent_up_side(side_num, AXIS_Z);
    rc = get_adjacent_right_side(side_num, AXIS_Z);
    dc = get_adjacent_down_side(side_num, AXIS_Z);
    lc = get_adjacent_left_side(side_num, AXIS_Z);

    t1=uc[2]; uc[2]=uc[1]; uc[1]=uc[0]; 
    t2=uc[5]; uc[5]=uc[4]; uc[4]=uc[3]; 
    t3=uc[8]; uc[8]=uc[7]; uc[7]=uc[6];

    uc[6]=lc[24]; lc[24]=lc[15]; lc[15]=lc[6]; 
    uc[3]=lc[25]; lc[25]=lc[16]; lc[16]=lc[7]; 
    uc[0]=lc[26]; lc[26]=lc[17]; lc[17]=lc[8];

    lc[6]=dc[20]; dc[20]=dc[19]; dc[19]=dc[18];
    lc[7]=dc[23]; dc[23]=dc[22]; dc[22]=dc[21];
    lc[8]=dc[26]; dc[26]=dc[25]; dc[25]=dc[24];

    dc[24]=rc[18]; rc[18]=rc[ 9]; rc[ 9]=rc[0]; rc[0]=t1;
    dc[21]=rc[19]; rc[19]=rc[10]; rc[10]=rc[1]; rc[1]=t2; 
    dc[18]=rc[20]; rc[20]=rc[11]; rc[11]=rc[2]; rc[2]=t3;
}


/* 
 *                                                                             Z
 *                                                                          -------
 *                                                                             |
 *                                                                               (side_zr)
 *                                                                          y   x
 *                                                                           \ |
 *                                                                            \|
 *                    Y________                          (side_yr)       z<----
 *                        |                                        x___
 *                 ^Y                                                  |\
 *                 |                                                   | \z
 *                 | 6      7      8                                   y                    z  X   (side_xr)   \
 *                 |                                  (side_xl)              |Z              \ |             ---\Y
 *                15     16     17                             ___Y          |____Y           \|___Y             \
 *                 | 3      4      5        /                 |\              \ (side_cf)
 *              24 |   25     26        ___/                  | \Z             \X
 *                12     13     14        /                   vX
 *                 | 0      1      2     /X                                          ^Y
 *              21  ---22-----23---->X                                               |
 *               / 9     10     11                                                   |
 *              /                                                           X         ---->X
 *             /18 |   19     20                                            |         \  (side_yl)
 *            Z    |                                                        |___Z      Z
 *                 |/                                             (side_zl)  \               \|
 *                 |                                                          \Y              |
 *                 |Z                                                                         |X
 */
static void move_middle_layer_x_l_to_r()
{
    uint8_t *bc, *tc, *fc, *oc;
    uint8_t t1, t2, t3;

    tc = sides_states[SIDE_CF].colors;
    fc = sides_states[SIDE_YL].colors;
    oc = sides_states[SIDE_CB].colors;
    bc = sides_states[SIDE_YR].colors;

    t1=tc[ 5]; tc[ 5]=tc[ 4]; tc[ 4]=tc[ 3];
    t2=tc[14]; tc[14]=tc[13]; tc[13]=tc[12];
    t3=tc[23]; tc[23]=tc[22]; tc[22]=tc[21];

    tc[ 3]=bc[25]; bc[25]=bc[16]; bc[16]=bc[7];
    tc[12]=bc[22]; bc[22]=bc[13]; bc[13]=bc[4];
    tc[21]=bc[19]; bc[19]=bc[10]; bc[10]=bc[1];

    bc[7]=oc[23]; oc[23]=oc[22]; oc[22]=oc[21];
    bc[4]=oc[14]; oc[14]=oc[13]; oc[13]=oc[12];
    bc[1]=oc[ 5]; oc[ 5]=oc[ 4]; oc[ 4]=oc[ 3];

    oc[21]=fc[19]; fc[19]=fc[10]; fc[10]=fc[1]; fc[1]=t1;
    oc[12]=fc[22]; fc[22]=fc[13]; fc[13]=fc[4]; fc[4]=t2;
    oc[ 3]=fc[25]; fc[25]=fc[16]; fc[16]=fc[7]; fc[7]=t3;
}

/* 
 *                                                                             Z
 *                                                                          -------
 *                                                                             |
 *                                                                               (side_zr)
 *                                                                          y   x
 *                                                                           \ |
 *                                                                            \|
 *                    Y________                          (side_yr)       z<----
 *                        |                                        x___
 *                 ^Y                                                  |\
 *                 |                                                   | \z
 *                 | 6      7      8                                   y                    z  X   (side_xr)   \
 *                 |                                  (side_xl)              |Z              \ |             ---\Y
 *                15     16     17                             ___Y          |____Y           \|___Y             \
 *                 | 3      4      5        /                 |\              \ (side_cf)
 *              24 |   25     26        ___/                  | \Z             \X
 *                12     13     14        /                   vX
 *                 | 0      1      2     /X                                          ^Y
 *              21  ---22-----23---->X                                               |
 *               / 9     10     11                                                   |
 *              /                                                           X         ---->X
 *             /18 |   19     20                                            |         \  (side_yl)
 *            Z    |                                                        |___Z      Z
 *                 |/                                             (side_zl)  \               \|
 *                 |                                                          \Y              |
 *                 |Z                                                                         |X
 */
static void move_middle_layer_x_r_to_l()
{
    uint8_t *bc, *tc, *fc, *oc;
    uint8_t t1, t2, t3;

    tc = sides_states[SIDE_CF].colors;
    fc = sides_states[SIDE_YL].colors;
    oc = sides_states[SIDE_CB].colors;
    bc = sides_states[SIDE_YR].colors;

    t1=tc[ 3]; tc[ 3]=tc[ 4]; tc[ 4]=tc[ 5];
    t2=tc[12]; tc[12]=tc[13]; tc[13]=tc[14];
    t3=tc[21]; tc[21]=tc[22]; tc[22]=tc[23];

    tc[ 5]=fc[1]; fc[1]=fc[10]; fc[10]=fc[19];
    tc[14]=fc[4]; fc[4]=fc[13]; fc[13]=fc[22];
    tc[23]=fc[7]; fc[7]=fc[16]; fc[16]=fc[25];

    fc[19]=oc[21]; oc[21]=oc[22]; oc[22]=oc[23];
    fc[22]=oc[12]; oc[12]=oc[13]; oc[13]=oc[14];
    fc[25]=oc[ 3]; oc[ 3]=oc[ 4]; oc[ 4]=oc[ 5];

    oc[23]=bc[7]; bc[7]=bc[16]; bc[16]=bc[25]; bc[25]=t1;
    oc[14]=bc[4]; bc[4]=bc[13]; bc[13]=bc[22]; bc[22]=t2;
    oc[ 5]=bc[1]; bc[1]=bc[10]; bc[10]=bc[19]; bc[19]=t3;
}

/* 
 *                                                                             Z
 *                                                                          -------
 *                                                                             |
 *                                                                               (side_zr)
 *                                                                          y   x
 *                          Y2                                               \ |
 *                         /                                                  \|
 *                   Y1___/____                          (side_yr)       z<----
 *                       /|                                        x___
 *                 ^Y   /                                              |\
 *                 |                                                   | \z                                     |Y2
 *                 | 6      7      8                                   y                    z  X   (side_xr)   \|
 *                 |                                  (side_xl)              |Z              \ |             ---\
 *                15     16     17                             ___Y          |____Y           \|___Y            |\
 *                 | 3      4      5        /                 |\              \ (side_cf)                       | Y1
 *              24 |   25     26        ___/                  | \Z             \X
 *                12     13     14        /                   vX
 *                 | 0      1      2     /X                                          ^Y
 *              21  ---22-----23---->X                                               |
 *               / 9     10     11                                                   |
 *              /                                                           X         ---->X
 *             /18 |   19     20                                            |         \  (side_yl)
 *            Z    |                                                        |___Z      Z
 *                 |/                                             (side_zl)  \               \|
 *                 |                                                          \Y              |
 *                 |Z                                                                         |X
 */
static void move_middle_layer_y_l_to_r()
{
    uint8_t *bc, *tc, *fc, *oc;
    uint8_t t1, t2, t3;

    tc = sides_states[SIDE_CF].colors;
    fc = sides_states[SIDE_XR].colors;
    oc = sides_states[SIDE_CB].colors;
    bc = sides_states[SIDE_XL].colors;

    t1=tc[15]; tc[15]=tc[12]; tc[12]=tc[ 9];
    t2=tc[16]; tc[16]=tc[13]; tc[13]=tc[10];
    t3=tc[17]; tc[17]=tc[14]; tc[14]=tc[11];

    tc[ 9]=bc[ 7]; bc[ 7]=bc[ 4]; bc[ 4]=bc[ 1];
    tc[10]=bc[16]; bc[16]=bc[13]; bc[13]=bc[10];
    tc[11]=bc[25]; bc[25]=bc[22]; bc[22]=bc[19];

    bc[ 1]=oc[17]; oc[17]=oc[14]; oc[14]=oc[11];
    bc[10]=oc[16]; oc[16]=oc[13]; oc[13]=oc[10];
    bc[19]=oc[15]; oc[15]=oc[12]; oc[12]=oc[ 9];

    oc[11]=fc[25]; fc[25]=fc[22]; fc[22]=fc[19]; fc[19]=t1;
    oc[10]=fc[16]; fc[16]=fc[13]; fc[13]=fc[10]; fc[10]=t2;
    oc[ 9]=fc[ 7]; fc[ 7]=fc[ 4]; fc[ 4]=fc[ 1]; fc[ 1]=t3;
}

/* 
 *                                                                             Z
 *                                                                          -------
 *                                                                             |
 *                                                                               (side_zr)
 *                                                                          y   x
 *                          Y2                                               \ |
 *                         /                                                  \|
 *                   Y1___/____                          (side_yr)       z<----
 *                       /|                                        x___
 *                 ^Y   /                                              |\
 *                 |                                                   | \z                                     |Y2
 *                 | 6      7      8                                   y                    z  X   (side_xr)   \|
 *                 |                                  (side_xl)              |Z              \ |             ---\
 *                15     16     17                             ___Y          |____Y           \|___Y            |\
 *                 | 3      4      5        /                 |\              \ (side_cf)                       | Y1
 *              24 |   25     26        ___/                  | \Z             \X
 *                12     13     14        /                   vX
 *                 | 0      1      2     /X                                          ^Y
 *              21  ---22-----23---->X                                               |
 *               / 9     10     11                                                   |
 *              /                                                           X         ---->X
 *             /18 |   19     20                                            |         \  (side_yl)
 *            Z    |                                                        |___Z      Z
 *                 |/                                             (side_zl)  \               \|
 *                 |                                                          \Y              |
 *                 |Z                                                                         |X
 */
static void move_middle_layer_y_r_to_l()
{
    uint8_t *bc, *tc, *fc, *oc;
    uint8_t t1, t2, t3;

    tc = sides_states[SIDE_CF].colors;
    fc = sides_states[SIDE_XR].colors;
    oc = sides_states[SIDE_CB].colors;
    bc = sides_states[SIDE_XL].colors;

    t1=tc[ 9]; tc[ 9]=tc[12]; tc[12]=tc[15];
    t2=tc[10]; tc[10]=tc[13]; tc[13]=tc[16];
    t3=tc[11]; tc[11]=tc[14]; tc[14]=tc[17];

    tc[15]=fc[19]; fc[19]=fc[22]; fc[22]=fc[25];
    tc[16]=fc[10]; fc[10]=fc[13]; fc[13]=fc[16];
    tc[17]=fc[ 1]; fc[ 1]=fc[ 4]; fc[ 4]=fc[ 7];

    fc[25]=oc[11]; oc[11]=oc[14]; oc[14]=oc[17];
    fc[16]=oc[10]; oc[10]=oc[13]; oc[13]=oc[16];
    fc[ 7]=oc[ 9]; oc[ 9]=oc[12]; oc[12]=oc[15];

    oc[17]=bc[ 1]; bc[ 1]=bc[ 4]; bc[ 4]=bc[ 7]; bc[ 7]=t1;
    oc[16]=bc[10]; bc[10]=bc[13]; bc[13]=bc[16]; bc[16]=t2;
    oc[15]=bc[19]; bc[19]=bc[22]; bc[22]=bc[25]; bc[25]=t3;
}

/* 
 *                                                                             Z
 *                                                                          -------
 *                                                                             |
 *                                                                               (side_zr)
 *                                                                          y   x
 *                                                                           \ |
 *                                                                            \|
 *                    Y________                          (side_yr)       z<----
 *                        |                                        x___
 *                 ^Y                                                  |\
 *                 |                                                   | \z
 *                 | 6      7      8                                   y                    z  X   (side_xr)   \
 *                 |                                  (side_xl)              |Z              \ |             ---\Y
 *                15     16     17                             ___Y          |____Y           \|___Y             \
 *                 | 3      4      5        /                 |\              \ (side_cf)
 *              24 |   25     26        ___/                  | \Z             \X
 *                12     13     14        /                   vX
 *                 | 0      1      2     /X                                          ^Y
 *              21  ---22-----23---->X                                               |
 *               / 9     10     11                                                   |
 *              /                                                           X         ---->X
 *             /18 |   19     20                                            |         \  (side_yl)
 *            Z    |                                                        |___Z      Z
 *                 |/                                             (side_zl)  \               \|
 *                 |                                                          \Y              |
 *                 |Z                                                                         |X
 */
static void move_middle_layer_z_l_to_r()
{
    uint8_t *bc, *tc, *fc, *oc;
    uint8_t t1, t2, t3;

    tc = sides_states[SIDE_CF].colors;
    fc = sides_states[SIDE_ZR].colors;
    oc = sides_states[SIDE_CB].colors;
    bc = sides_states[SIDE_ZL].colors;

    t1=tc[19]; tc[19]=tc[10]; tc[10]=tc[1];
    t2=tc[22]; tc[22]=tc[13]; tc[13]=tc[4];
    t3=tc[25]; tc[25]=tc[16]; tc[16]=tc[7];

    tc[1]=bc[ 5]; bc[ 5]=bc[ 4]; bc[ 4]=bc[ 3];
    tc[4]=bc[14]; bc[14]=bc[13]; bc[13]=bc[12];
    tc[7]=bc[23]; bc[23]=bc[22]; bc[22]=bc[21];

    bc[ 3]=oc[25]; oc[25]=oc[16]; oc[16]=oc[7];
    bc[12]=oc[22]; oc[22]=oc[13]; oc[13]=oc[4];
    bc[21]=oc[19]; oc[19]=oc[10]; oc[10]=oc[1];

    oc[7]=fc[23]; fc[23]=fc[22]; fc[22]=fc[21]; fc[21]=t1;
    oc[4]=fc[14]; fc[14]=fc[13]; fc[13]=fc[12]; fc[12]=t2;
    oc[1]=fc[ 5]; fc[ 5]=fc[ 4]; fc[ 4]=fc[ 3]; fc[ 3]=t3;
}

/* 
 *                                                                             Z
 *                                                                          -------
 *                                                                             |
 *                                                                               (side_zr)
 *                                                                          y   x
 *                                                                           \ |
 *                                                                            \|
 *                    Y________                          (side_yr)       z<----
 *                        |                                        x___
 *                 ^Y                                                  |\
 *                 |                                                   | \z
 *                 | 6      7      8                                   y                    z  X   (side_xr)   \
 *                 |                                  (side_xl)              |Z              \ |             ---\Y
 *                15     16     17                             ___Y          |____Y           \|___Y             \
 *                 | 3      4      5        /                 |\              \ (side_cf)
 *              24 |   25     26        ___/                  | \Z             \X
 *                12     13     14        /                   vX
 *                 | 0      1      2     /X                                          ^Y
 *              21  ---22-----23---->X                                               |
 *               / 9     10     11                                                   |
 *              /                                                           X         ---->X
 *             /18 |   19     20                                            |         \  (side_yl)
 *            Z    |                                                        |___Z      Z
 *                 |/                                             (side_zl)  \               \|
 *                 |                                                          \Y              |
 *                 |Z                                                                         |X
 */
static void move_middle_layer_z_r_to_l()
{
    uint8_t *bc, *tc, *fc, *oc;
    uint8_t t1, t2, t3;

    tc = sides_states[SIDE_CF].colors;
    fc = sides_states[SIDE_ZR].colors;
    oc = sides_states[SIDE_CB].colors;
    bc = sides_states[SIDE_ZL].colors;

    t1=tc[1]; tc[1]=tc[10]; tc[10]=tc[19];
    t2=tc[4]; tc[4]=tc[13]; tc[13]=tc[22];
    t3=tc[7]; tc[7]=tc[16]; tc[16]=tc[25];

    tc[19]=fc[21]; fc[21]=fc[22]; fc[22]=fc[23];
    tc[22]=fc[12]; fc[12]=fc[13]; fc[13]=fc[14];
    tc[25]=fc[ 3]; fc[ 3]=fc[ 4]; fc[ 4]=fc[ 5];

    fc[23]=oc[7]; oc[7]=oc[16]; oc[16]=oc[25];
    fc[14]=oc[4]; oc[4]=oc[13]; oc[13]=oc[22];
    fc[ 5]=oc[1]; oc[1]=oc[10]; oc[10]=oc[19];

    oc[25]=bc[ 3]; bc[ 3]=bc[ 4]; bc[ 4]=bc[ 5]; bc[ 5]=t1;
    oc[22]=bc[12]; bc[12]=bc[13]; bc[13]=bc[14]; bc[14]=t2;
    oc[19]=bc[21]; bc[21]=bc[22]; bc[22]=bc[23]; bc[23]=t3;
}

/* Some auxiliary functions */

static Dim_Action get_dim_action(uint8_t cycle_ct)
{
    if (cycle_ct == rotation_phase_1_cycles) return dim;
    if (cycle_ct == rotation_phase_2_cycles) return undim;
    if (cycle_ct == rotation_phase_f_cycles) return no_change;
    return no_change;
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
    rotate_1_side_level(bc, (indexes_bf + 16), dim_action);
    rotate_1_side_level(fc, indexes_bf, dim_action);
}


/* Main rotation and perspective moving functions */

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

/*
 *             Cube sides orientation
 * 
 * 
 *                   l to r ^    |
 *                          |    |
 *                          |    |
 *                          |    V r to l
 *
 *                             Z
 *                          -------
 *                             |
 * 
 *                               (side_zr)
 *                          y   x
 *                           \ |
 *                            \|
 *       (side_yr)       z<----
 *                 x___
 *                     |\
 *                     | \z
 *                     y                    z  X   (side_xr)   \     -----> l to r
 *    (side_xl)              |Z              \ |             ---\Y
 *             ___Y          |____Y           \|___Y             \     <----- r to l
 *            |\              \ (side_cf)
 *            | \Z             \X
 *            vX
 *                                   ^Y
 *                                   |
 *                                   |
 *                          X         ---->X
 *                          |         \  (side_yl)
 *                          |___Z      Z
 *                (side_zl)  \               \|
 *                            \Y              |
 *                                            |X  \
 *                                                 \
 *                                                  V l to r
 *                                       r to l ^
 *                                               \
 *                                                \
 */
static void move_persp_x_dir_l_to_r(uint8_t side_num)
{
    Dim_Action dim_action = get_dim_action(sides_states[side_num].cycle_ct);

    rotate_1_side(sides_states[SIDE_XR].colors, ROTATION_X_CW_INDEXES, dim_action);
    rotate_1_side(sides_states[SIDE_ZR].colors, ROTATION_Z_CW_INDEXES, dim_action);
    rotate_1_side(sides_states[SIDE_XL].colors, ROTATION_X_CW_INDEXES, dim_action);
    rotate_1_side(sides_states[SIDE_ZL].colors, ROTATION_Z_CW_INDEXES, dim_action);

    move_middle_layer_x_l_to_r();

    rotate_adjacent_layer_x_cw(SIDE_XR);
    rotate_adjacent_layer_x_cw(SIDE_XL);
}
static void move_persp_x_dir_r_to_l(uint8_t side_num)
{
    Dim_Action dim_action = get_dim_action(sides_states[side_num].cycle_ct);

    rotate_1_side(sides_states[SIDE_XR].colors, ROTATION_X_CCW_INDEXES, dim_action);
    rotate_1_side(sides_states[SIDE_ZR].colors, ROTATION_Z_CCW_INDEXES, dim_action);
    rotate_1_side(sides_states[SIDE_XL].colors, ROTATION_X_CCW_INDEXES, dim_action);
    rotate_1_side(sides_states[SIDE_ZL].colors, ROTATION_Z_CCW_INDEXES, dim_action);

    move_middle_layer_x_r_to_l();

    rotate_adjacent_layer_x_ccw(SIDE_XR);
    rotate_adjacent_layer_x_ccw(SIDE_XL);
}
static void move_persp_y_dir_l_to_r(uint8_t side_num)
{
    Dim_Action dim_action = get_dim_action(sides_states[side_num].cycle_ct);

    rotate_1_side(sides_states[SIDE_ZR].colors, ROTATION_Y_CCW_INDEXES, dim_action);
    rotate_1_side(sides_states[SIDE_YL].colors, ROTATION_Y_CCW_INDEXES, dim_action);
    rotate_1_side(sides_states[SIDE_ZL].colors, ROTATION_Y_CCW_INDEXES, dim_action);
    rotate_1_side(sides_states[SIDE_YR].colors, ROTATION_Y_CCW_INDEXES, dim_action);

    move_middle_layer_y_l_to_r();

    rotate_adjacent_layer_y_ccw(SIDE_ZR);
    rotate_adjacent_layer_y_ccw(SIDE_ZL);
}
static void move_persp_y_dir_r_to_l(uint8_t side_num)
{
    Dim_Action dim_action = get_dim_action(sides_states[side_num].cycle_ct);

    rotate_1_side(sides_states[SIDE_ZR].colors, ROTATION_Y_CW_INDEXES, dim_action);
    rotate_1_side(sides_states[SIDE_YL].colors, ROTATION_Y_CW_INDEXES, dim_action);
    rotate_1_side(sides_states[SIDE_ZL].colors, ROTATION_Y_CW_INDEXES, dim_action);
    rotate_1_side(sides_states[SIDE_YR].colors, ROTATION_Y_CW_INDEXES, dim_action);

    move_middle_layer_y_r_to_l();

    rotate_adjacent_layer_y_cw(SIDE_ZR);
    rotate_adjacent_layer_y_cw(SIDE_ZL);
}
static void move_persp_z_dir_l_to_r(uint8_t side_num)
{
    Dim_Action dim_action = get_dim_action(sides_states[side_num].cycle_ct);

    rotate_1_side(sides_states[SIDE_YL].colors, ROTATION_X_CW_INDEXES, dim_action);
    rotate_1_side(sides_states[SIDE_XR].colors, ROTATION_Z_CW_INDEXES, dim_action);
    rotate_1_side(sides_states[SIDE_YR].colors, ROTATION_X_CW_INDEXES, dim_action);
    rotate_1_side(sides_states[SIDE_XL].colors, ROTATION_Z_CW_INDEXES, dim_action);

    move_middle_layer_z_l_to_r();

    rotate_adjacent_layer_x_cw(SIDE_YL);
    rotate_adjacent_layer_x_cw(SIDE_YR);
}
static void move_persp_z_dir_r_to_l(uint8_t side_num)
{
    Dim_Action dim_action = get_dim_action(sides_states[side_num].cycle_ct);

    rotate_1_side(sides_states[SIDE_YL].colors, ROTATION_X_CCW_INDEXES, dim_action);
    rotate_1_side(sides_states[SIDE_XR].colors, ROTATION_Z_CCW_INDEXES, dim_action);
    rotate_1_side(sides_states[SIDE_YR].colors, ROTATION_X_CCW_INDEXES, dim_action);
    rotate_1_side(sides_states[SIDE_XL].colors, ROTATION_Z_CCW_INDEXES, dim_action);

    move_middle_layer_z_r_to_l();

    rotate_adjacent_layer_x_ccw(SIDE_YL);
    rotate_adjacent_layer_x_ccw(SIDE_YR);
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
    /* SIDE_XL , (MOVE_PERSP)            */ move_persp_y_dir_l_to_r,

    /* SIDE_XR , (ROTATION_X | DIR_CCW)  */ rotate_x_dir_ccw,
    /* SIDE_XR , (ROTATION_X | DIR_CW)   */ rotate_x_dir_cw,
    /* SIDE_XR , (ROTATION_Y | DIR_CCW)  */ rotate_z_dir_cw,
    /* SIDE_XR , (ROTATION_Y | DIR_CW)   */ rotate_z_dir_ccw,
    /* SIDE_XR , (ROTATION_Z | DIR_CCW)  */ rotate_y_dir_ccw,
    /* SIDE_XR , (ROTATION_Z | DIR_CW)   */ rotate_y_dir_cw,
    /* SIDE_XR , (MOVE_PERSP)            */ move_persp_y_dir_r_to_l,

    /* SIDE_YL , (ROTATION_X | DIR_CCW)  */ rotate_x_dir_ccw,
    /* SIDE_YL , (ROTATION_X | DIR_CW)   */ rotate_x_dir_cw,
    /* SIDE_YL , (ROTATION_Y | DIR_CCW)  */ rotate_y_dir_ccw,
    /* SIDE_YL , (ROTATION_Y | DIR_CW)   */ rotate_y_dir_cw,
    /* SIDE_YL , (ROTATION_Z | DIR_CCW)  */ rotate_z_dir_ccw,
    /* SIDE_YL , (ROTATION_Z | DIR_CW)   */ rotate_z_dir_cw,
    /* SIDE_YL , (MOVE_PERSP)            */ move_persp_x_dir_r_to_l,

    /* SIDE_YR , (ROTATION_X | DIR_CCW)  */ rotate_x_dir_ccw,
    /* SIDE_YR , (ROTATION_X | DIR_CW)   */ rotate_x_dir_cw,
    /* SIDE_YR , (ROTATION_Y | DIR_CCW)  */ rotate_y_dir_cw,
    /* SIDE_YR , (ROTATION_Y | DIR_CW)   */ rotate_y_dir_ccw,
    /* SIDE_YR , (ROTATION_Z | DIR_CCW)  */ rotate_z_dir_cw,
    /* SIDE_YR , (ROTATION_Z | DIR_CW)   */ rotate_z_dir_ccw,
    /* SIDE_YR , (MOVE_PERSP)            */ move_persp_x_dir_l_to_r,

    /* SIDE_ZL , (ROTATION_X | DIR_CCW)  */ rotate_y_dir_cw,
    /* SIDE_ZL , (ROTATION_X | DIR_CW)   */ rotate_y_dir_ccw,
    /* SIDE_ZL , (ROTATION_Y | DIR_CCW)  */ rotate_z_dir_ccw,
    /* SIDE_ZL , (ROTATION_Y | DIR_CW)   */ rotate_z_dir_cw,
    /* SIDE_ZL , (ROTATION_Z | DIR_CCW)  */ rotate_x_dir_cw,
    /* SIDE_ZL , (ROTATION_Z | DIR_CW)   */ rotate_x_dir_ccw,
    /* SIDE_ZL , (MOVE_PERSP)            */ move_persp_z_dir_l_to_r,

    /* SIDE_ZR , (ROTATION_X | DIR_CCW)  */ rotate_y_dir_cw,
    /* SIDE_ZR , (ROTATION_X | DIR_CW)   */ rotate_y_dir_ccw,
    /* SIDE_ZR , (ROTATION_Y | DIR_CCW)  */ rotate_z_dir_cw,
    /* SIDE_ZR , (ROTATION_Y | DIR_CW)   */ rotate_z_dir_ccw,
    /* SIDE_ZR , (ROTATION_Z | DIR_CCW)  */ rotate_x_dir_ccw,
    /* SIDE_ZR , (ROTATION_Z | DIR_CW)   */ rotate_x_dir_cw,
    /* SIDE_ZR , (MOVE_PERSP)            */ move_persp_z_dir_r_to_l
};


/* Public functions */
rotation_func_ptr_type get_rotation_func_ptr(uint8_t side_num, uint8_t direction)
{
    return ROTATION_FUNC_MATRIX[side_num * 7 + direction];
}

/* The main rotation function and its counterparts */
static uint8_t can_do_rotation_cycle(uint8_t cycle_ct)
{
    if (cycle_ct == rotation_phase_1_cycles ||
        cycle_ct == rotation_phase_2_cycles ||
        cycle_ct == rotation_phase_f_cycles)
        return TRUE;
    else
        return FALSE;
}

void change_phase_cycle_counters(uint8_t faster, uint8_t slower)
{
    static uint8_t index = 0;
    int8_t index_d;

    if (faster || slower)
    {
        index_d = faster ? (index < 3 ? 1 : 0) : (index > 0 ? -1 : 0);

        if (index_d != 0)
        {
            index += index_d;

            rotation_phase_1_cycles = rotation_phase_cycles[index][0];
            rotation_phase_2_cycles = rotation_phase_cycles[index][1];
            rotation_phase_f_cycles = rotation_phase_cycles[index][2];
        }
    }
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

