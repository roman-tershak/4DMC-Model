#ifndef H_COMMON
#define H_COMMON

#define FALSE 0x0
#define TRUE 0x1

#define NULL 0x0

/* Cube geometry */
#define SIDE_XL  0
#define SIDE_XR  1
#define SIDE_YL  2
#define SIDE_YR  3
#define SIDE_ZL  4
#define SIDE_ZR  5
#define SIDE_CF  6
#define SIDE_CB  7

#define SIDE_COUNT        8
#define SIDE_CUBES_COUNT  27

#define LIGHT_SIDE_COUNT  (SIDE_COUNT)

/* Colors */
#define MAIN_COLOR_COUNT        8
#define ALL_COLOR_COUNT         16

/* All LEDs go in pairs */
#define DOUBLE_COLOR_LEDS
#define CB_ABSENT_CUBIE_NUM     13

/* Debug directives */
//#define DEBUG_COLOR_ADJUST
//#define USART_DEBUG
//#define DEBUG_DONT_STORE

#endif
