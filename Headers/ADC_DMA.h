#ifndef _ADC_DMA
#define _ADC_DMA

#include "../Headers/UserSystemInit.h"
#include "../Headers/Timers.h"

void GPIO_Init_ADC2 (void);
void ADC1_2_IRQHandler(void);
void ADC2_Init (void);
void ADC_control_PWM_Led (void);

#endif
