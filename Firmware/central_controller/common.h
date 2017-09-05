#ifndef H_COMMON
#define H_COMMON

#define FALSE 0x0
#define TRUE 0x1

#ifndef NULL
#define NULL 0x0
#endif

/* Cube geometry */
#define SIDE_XL  0
#define SIDE_XR  1
#define SIDE_YL  2
#define SIDE_YR  3
#define SIDE_ZL  4
#define SIDE_ZR  5
#define SIDE_CF  6
#define SIDE_CB  7

#define SIDE_COUNT          8
#define SIDE_STICKER_COUNT  8
#define STICKER_COUNT       (SIDE_COUNT * SIDE_STICKER_COUNT)

#define LIGHT_SIDE_COUNT    (SIDE_COUNT)

/* Colors */
#define MAIN_COLOR_COUNT        8
#define ALL_COLOR_COUNT         16

/* Debug directives */
//#define DEBUG_COLOR_ADJUST
#define USART_DEBUG
//#define DEBUG_DONT_STORE

#endif
