#ifndef _UART
#define _UART
#include "stm32f3xx.h"

void UART4_Init(void);
void UART4_SendChar (uint8_t znak);

void UART4_SendString (char *string);
uint8_t UART4_GetChar (void);
void UART4_IRQHandler(void);

#endif
