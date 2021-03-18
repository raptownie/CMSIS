#ifndef _GPIO_LEDS_Buttons
#define _GPIO_LEDS_Buttons

#include "stm32f3xx.h" 
#include "../Headers/UserSystemInit.h"
//#include "../Headers/Timers.h"

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

extern uint8_t FirstRun_GPIO_Init;
void GPIO_zPrzyciskiem(void);
void GPIO_Kolko(void);
void LEDy_kolo (void)__attribute__((noreturn));
void Debouncing_SW_LPF (void);

#endif
