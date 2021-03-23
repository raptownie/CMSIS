#ifndef _UserSystemInit
#define _UserSystemInit

#include "stm32f3xx.h"

#define HSI_Clock 8000000        // bez PLL
#define Pin_3 0x8
#define Pin_5 0x20
#define Pin_8 0x100
#define Pin_9 0x200
#define Pin_10 0x400
#define Pin_11 0x800
#define Pin_12 0x1000
#define Pin_13 0x2000
#define Pin_14 0x4000
#define Pin_15 0x8000
#define Accelerometer_Adress 0x19
#define Magneto_Adress 0x1E

extern volatile uint32_t timer_ms;

void GPIO_Init(void);
void SysTick_Init(void);
void delay_ms(uint32_t time);
void HSI_without_PLL(void);
void HSI_with_PLL(void);
void HSE_with_PLL(void);
void SysTick_Handler(void);

#endif
