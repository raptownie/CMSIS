#ifndef _UserSystemInit
#define _UserSystemInit

#include "stm32f3xx.h" 

#define HSI_Clock 8000000        // bez PLL
#define Pin_8 0x100
#define Pin_9 0x200
#define Pin_10 0x400
#define Pin_11 0x800
#define Pin_12 0x1000
#define Pin_13 0x2000
#define Pin_14 0x4000
#define Pin_15 0x8000

static volatile uint32_t timer_ms;
static uint8_t EXTI0_flag; 
static uint8_t FirstRun_GPIO_PWM_Init = 0;
static uint8_t FirstRun_GPIO_Init = 0; 
static uint8_t PWM_status_ON = 0;
static uint16_t PWM_temp = 0;
static uint16_t PWM_ARR_value = 2047;
static uint8_t PWM_CCR1_value = 0;
static uint32_t ADC2_Raw_value=0;
static float ADC2_Voltage=0;


void GPIO_Init(void);
void SysTick_Init(void);
void delay_ms(uint32_t time);
void HSI_without_PLL(void);
void HSI_with_PLL(void);
void HSE_with_PLL(void);
void SysTick_Handler(void);

#endif
