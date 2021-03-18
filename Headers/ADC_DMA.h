#ifndef _ADC_DMA
#define _ADC_DMA

//#include "../Headers/UserSystemInit.h"
#include "stm32f3xx.h"
#include "../Headers/Timers.h"


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

extern float ADC2_Voltage;
extern uint32_t ADC2_Raw_value;
extern uint8_t PWM_CCR1_value;
extern uint16_t PWM_ARR_value;

void GPIO_Init_ADC2 (void);
void ADC1_2_IRQHandler(void);
void ADC2_Init (void);
void ADC_control_PWM_Led (void);
void ADC2_with_DMA_Init (void);
void DMA2_Channel1_IRQHandler (void);


#endif
