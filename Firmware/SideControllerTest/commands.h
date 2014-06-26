#ifndef H_COMMANDS
#define H_COMMANDS

/*---- USART commands ----*/
/* Commands use 5 higher bits in address byte */
#define USART_INIT_LOAD 	0x00
#define USART_STORE_STATE 	0x10
#define USART_STATE_STORED	0x18	// output command sent aa a confirmation on USART_STORE_STATE

#define PKG_START 0xFF
#define PKG_LEN   27

#define R1 0
#define G1 1
#define B1 2
#define R2 3
#define G2 4
#define B2 5
#define R3 6
#define G3 7
#define B3 8

#define A 0
#define B 1
#define C 2
#define D 3
#define E 4
#define F 5
#define G 6
#define H 7
#define I 8

#ifdef __cplusplus
 extern "C" {
#endif 

void display_colors();

#ifdef __cplusplus
}
#endif 

#endif
