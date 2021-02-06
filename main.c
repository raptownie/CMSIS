// cwiczenie 1 GPIO
#include<stdio.h>
#include "stm32f3xx.h"         // Device header
#include "main.h"
#define Pin_8 0x100
#define Pin_9 0x200
#define Pin_10 0x400
#define Pin_11 0x800
#define Pin_12 0x1000
#define Pin_13 0x2000
#define Pin_14 0x4000
#define Pin_15 0x8000
#define HSI_Clock 8000000        // bez PLL
#define HSI_Clock_PLL 24000000   // z PLL


void GPIO_Init(void);
void GPIO_zPrzyciskiem(void);
void GPIO_Kolko(void);
void SysTick_Init(void);
void SysTick_Handler(void);
void delay_ms(uint32_t time);
void HSI_without_PLL(void);
void HSI_with_PLL(void);



static volatile uint32_t timer_ms;
   
int main()
{

   GPIO_Init();
   
   
   
   /* SysTick notatki
   W stmach zawsze po resecie zródlem sygnalu jest HSI (high speed internal), czyli wewnetrzny generator szybkich przebiegow, dla STM32F3 to 8MHz.
   Czestotliwosc ta moze byc zmieniona przez petle PLL (podzielona przez 2), nastepnie pomnozona.
   Sygnalem zegara systemowego moze byc rowniez HSE - czyli high speed external, zewnetrzny oscylator kwarc.   
   */   
   HSI_with_PLL();
  // HSI_without_PLL();
   uint16_t temp= 0x100;
   GPIOE->ODR |= temp;
   
   while(1){   
   //diody na plytce discovery swieca sie w kolko jedna po drugiej
   temp <<= 1;   
   GPIOE->ODR = temp;   
   if (temp == 0x8000) {
      delay_ms(5000);
      temp = 0x100;
      GPIOE->ODR = temp;
   }
   delay_ms(5000);
   
      
   //GPIO_zPrzyciskiem();
   //GPIO_Kolko();
   
   
   }
   
   
}
void delay_ms(uint32_t time){
   timer_ms=time;
   while(timer_ms);
}

void GPIO_zPrzyciskiem(void){   
   GPIOE->ODR |= (uint16_t)(Pin_11+Pin_15);           //wyjscie PE11 i PE15 ustaw na logiczna 1   
   delay_ms(1000);   
   if (((GPIOA->IDR) & 0x1 ) != (uint16_t)0x0001){    // czy stan niski na PA0?
      GPIOE->ODR &= (uint32_t)~0x1000;                //wyjscie PE12 ustaw logiczne 0
   }
   if (((GPIOA->IDR) & 0x1 ) == (uint16_t)0x0001){    // czy stan wysoki na PA0?
      GPIOE->ODR |= (uint16_t)0x1000;                 //wyjscie PE12 ustaw logiczne 1
   }
   delay_ms(1000);   
   GPIOE->ODR &= (uint16_t)~(Pin_11+Pin_15);          //wyjscie PE11 i PE15 ustaw na logiczne 0
   delay_ms(1000);
   GPIOE->ODR |= (uint16_t)Pin_11;                    //wyjscie PE11 ustaw logiczne 1
   delay_ms(1000);
   GPIOE->ODR &= (uint16_t)~Pin_11;                   //wyjscie PE11 ustaw logiczne 0
   delay_ms(1000);
}

void GPIO_Kolko(void){
   GPIOE->ODR |= (uint32_t)(Pin_15+Pin_14+Pin_13);
   delay_ms(250);
   GPIOE->ODR &= (uint32_t)~Pin_15;
   delay_ms(250);
   GPIOE->ODR |= (uint32_t)(Pin_14+Pin_13+Pin_12);
   delay_ms(250);
   GPIOE->ODR &= (uint32_t)~Pin_14;
   delay_ms(250);
   GPIOE->ODR |= (uint32_t)(Pin_13+Pin_12+Pin_11);   
   delay_ms(250);
   GPIOE->ODR &= (uint32_t)~Pin_13;
   delay_ms(250);
   GPIOE->ODR |= (uint32_t)(Pin_12+Pin_11+Pin_10);   
   delay_ms(250);
   GPIOE->ODR &= (uint32_t)~Pin_12;
   delay_ms(250);
   GPIOE->ODR |= (uint32_t)(Pin_11+Pin_10+Pin_9);
   delay_ms(250);
   GPIOE->ODR &= (uint32_t)~Pin_11;
   delay_ms(250);
   GPIOE->ODR |= (uint32_t)(Pin_10+Pin_9+Pin_8);
   delay_ms(250);
   GPIOE->ODR &= (uint32_t)~Pin_10;
   delay_ms(250);
   GPIOE->ODR |= (uint32_t)(Pin_9+Pin_8+Pin_15);
   delay_ms(250);
   GPIOE->ODR &= (uint32_t)~Pin_9;
   delay_ms(250);
   GPIOE->ODR |= (uint32_t)(Pin_8+Pin_15+Pin_14);
   delay_ms(250);
   GPIOE->ODR &= (uint32_t)~Pin_8;   
   delay_ms(250);
}

void GPIO_Init(void){
   //wlaczenie portu E + konfiguracja pinow jako wyjscie
   RCC->AHBENR |= (uint32_t)0x200000;              //Ustawienie 21 bitu na 1 - Podpiecie zegara pod port E
   GPIOE->MODER |= (uint32_t)0x55550000;           //bit 30,28,26,24,22,20,18 i 16 - PE8-15 jako wyjscie; rejest MODER - input/output/alternate func/analogmode
   GPIOE->OTYPER = 0x0;                            // wyjscie ustawione jako open dran
   //GPIOE->PUPDR |= 0xAAAA0000;                   // rezystory pulldown
   //wlaczenie portu A + konfiguracja pinu PA0 jako wejscie
   RCC->AHBENR |= (uint32_t)0x20000;               // podlaczenie sygnalu zegara pod port A
   GPIOA->MODER &= (uint32_t)~0x0003;              // ustawienie PA0 jako wyjscie podciagniete do VCC standardowo.
}

void SysTick_Init(void){
   /*
   SysTick->CTRL=5;
   SysTick->LOAD = 4;
   SysTick->VAL=5;
   SysTick->CALIB=5;
   */
   
}

void SysTick_Handler(void){
   if (timer_ms) timer_ms--;
}
void HSI_without_PLL(void){
   SysTick_Config(HSI_Clock/1000); // ustawienie przerwania co 1ms
}

void HSI_with_PLL(void){
   RCC->CR &= (uint32_t)~0x2000000;             // PLL unlocked
   RCC->CR &=(uint32_t)~0x1000000;              // wylaczenie PLL
   RCC->CFGR &=(uint32_t)~0x1;                  // System clock switch 01 : PLL selectes as system clock
   RCC->CFGR |= 0x2; 
   RCC->CFGR |= 0x8;                            //10: PLL used as system clock
   RCC->CFGR &= (uint32_t)~0x4;
   RCC->CFGR &=(uint32_t)~0x18000;              // select PLL clock source 00: HSI/2 used as PREDIV1 entry and PREDIV1 forced to div by 2.
   RCC->CFGR &= (uint32_t)~0x2C0000;            // PLL input clock x6
   RCC->CFGR |= (uint32_t)0x100000;
   RCC->CR |= (uint32_t)0x1000000;              //wlaczenie PLL
   RCC->CR |= (uint32_t)0x2000000;              // PLL locked
   SysTick_Config(HSI_Clock_PLL/1000);          //ustawienie przerwania co 1ms
}
