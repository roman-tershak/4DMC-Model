#ifndef H_CENTRAL_LOGIC
#define H_CENTRAL_LOGIC

#include <stdio.h>
#include <avr/io.h>

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

/* Side states */
#define SIDE_IDLE               _BV(0)
#define WAITING_FOR_ROTATION    _BV(1)
#define ROTATING                _BV(2)
#define WAITING_FOR_SAVING      _BV(3)

/* Inter-MCU communication commands */
#define USART_INIT_LOAD         0x01
#define USART_DISPLAY_COLORS    0x02
#define USART_STORE_STATE       0x03
#define USART_STORE_CONFIRM     0x04
#define USART_STATE_STORED      0x0F    // output command sent as a confirmation that state has been stored

// Use in inter-MCU communication and storages
#define SIDE_STATE_DATA_LEN  14


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

uint8_t start_rotation(uint8_t sw_side_num, uint8_t direction);

void handle_cycle(void);

void notify_sides_changed(void);

#ifdef __cplusplus
}
#endif 

#endif
