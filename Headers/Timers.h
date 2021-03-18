#ifndef _Timers
#define _Timers

#include "../Headers/EXTI.h"
#include "../Headers/UserSystemInit.h"
#include "stm32f3xx.h" 
#include "math.h"


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

extern uint8_t PWM_CCR1_value;
extern uint16_t PWM_ARR_value;
extern uint16_t PWM_temp;
extern uint8_t FirstRun_GPIO_Init;
extern uint8_t FirstRun_GPIO_PWM_Init;
extern uint8_t EXTI0_flag;

void TIM7_config(void);
void TIM7_IRQHandler(void);
void TIM1_config(void);
void TIM1_UP_TIM16_IRQHandler(void);
void TIM1_CC_IRQHandler(void);
void GPIO_Init_PWM_TIM1(void);
void PWM_TIM1_C1_Init(uint32_t ARR_value, uint32_t CCR1_value);
void Zmiana_PWM_TIM1_Button(void);
void Zmiana_PWM_TIM1_stopniowo(void);



#endif
