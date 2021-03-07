#ifndef _Timers
#define _Timers

#include "../Headers/UserSystemInit.h"
#include "../Headers//EXTI.h"
#include "math.h"

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
