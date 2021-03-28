#ifndef _UART
#define _UART
#include "stm32f3xx.h"

/*** Variables ***/
extern char StringToSendUART[40];
extern char StringToReciveUART[20];

/*** Function prototypes ***/
void UART4_Init(void);
void UART4_SendChar (uint8_t znak);
void UART4_SendString (char *string);
uint8_t UART4_GetChar (void);
void UART4_IRQHandler(void);
void UART4_Init_with_DMA(void);
void UART4_DMA_config(void);
void UART4_DMA_TIM7_config(void);
void UART4_Init_with_DMA_TIM7(void);
extern void TIM7_config(void);

#endif
