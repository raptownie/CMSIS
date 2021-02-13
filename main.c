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



void GPIO_Init(void);
void GPIO_zPrzyciskiem(void);
void GPIO_Kolko(void);
void SysTick_Init(void);
void SysTick_Handler(void);
void delay_ms(uint32_t time);
void HSI_without_PLL(void);
void HSI_with_PLL(void);
void HSE_with_PLL(void);
void TIM7_config(void);
void TIM7_IRQHandler(void);

static volatile uint32_t timer_ms;
   
int main()
{ 
   
   /* SysTick notatki
   W stmach zawsze po resecie zródlem sygnalu jest HSI (high speed internal), czyli wewnetrzny generator szybkich przebiegow, dla STM32F3 to 8MHz.
   Czestotliwosc ta moze byc zmieniona przez petle PLL (podzielona przez 2), nastepnie pomnozona.
   Sygnalem zegara systemowego moze byc rowniez HSE - czyli high speed external, zewnetrzny oscylator kwarc.   
   Dokumentacja funkcji SysTick_config() i rejestrow https://developer.arm.com/documentation/dui0552/a/cortex-m3-peripherals/system-timer--systick
   */   

   HSI_with_PLL();
   //HSE_with_PLL();       //to narazie nie dziala
  // HSI_without_PLL();
  // TIM7_config();
   
   GPIO_Init(); 
   
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
   RCC->AHBENR |= (uint32_t)0x200000;                 //Ustawienie 21 bitu na 1 - Podpiecie zegara pod port E
   GPIOE->MODER |= (uint32_t)0x55550000;              //bit 30,28,26,24,22,20,18 i 16 - PE8-15 jako wyjscie; rejest MODER - input/output/alternate func/analogmode
   GPIOE->OTYPER = 0x0;                               // wyjscie ustawione jako open dran
   RCC->AHBENR |= (uint32_t)0x20000;                  // podlaczenie sygnalu zegara pod port A
   GPIOA->MODER &= (uint32_t)~0x0003;                 // ustawienie PA0 jako wyjscie podciagniete do VCC standardowo.
}

void SysTick_Handler(void){
   if (timer_ms) timer_ms--;
}

void HSI_without_PLL(void){
   SysTick_Config(HSI_Clock/1000);                    // ustawienie przerwania co 1ms
}

void HSI_with_PLL(void){
   
   // Max taktowanie to 64MHz
   RCC->CR &= (uint32_t)~0x1000000;                    //RCC->CR &=~RCC_CR_PLLON;                           // wylaczenie PLL

   //konfigracja PLL
   RCC->CFGR |= RCC_CFGR_PLLSRC_HSI_DIV2;             // select PLL clock source 00: HSI/2 used as PREDIV1 entry and PREDIV1 forced to div by 2.
   RCC->CFGR |= RCC_CFGR_PLLMUL16;                    // PLL input clock x12
      
   RCC->CR |= RCC_CR_PLLON;                           //wlaczenie PLL
   
   RCC->CFGR |=RCC_CFGR_PPRE1_DIV2;                   //APB1 div /2 - max32MHz wiec trzeba podzielic
   
   //flash ustawienie opoznienia 2 cykli konieczne przy taktowaniu 48-72MHz - bez tego hardfoult "acces memory..."
   FLASH->ACR|= 0x2;
   FLASH->ACR &= (uint16_t)~0x5;
   
   //Ustawienie zegarow APB1, APB2, USB, I2C, SYSCLK
   RCC->CFGR &= (uint32_t)~0x400000;                  //RCC->CFGR |= RCC_CFGR_USBPRE_DIV1_5;   nok
   RCC->CFGR &= (uint32_t)~0x800000;                  //RCC->CFGR |=RCC_CFGR_I2SSRC_SYSCLK ;   nok
   RCC->CFGR |= 0x400;                                //RCC->CFGR |=RCC_CFGR_PPRE1_DIV2;   nok
   RCC->CFGR &= (uint32_t)~0x300;      
   RCC->CFGR &= (uint32_t)~0x2000;                    //RCC->CFGR |= RCC_CFGR_PPRE2_DIV1;   nok
   RCC->CFGR &= (uint32_t)~0x80;                      //RCC->CFGR |=RCC_CFGR_HPRE_DIV1;   nok
   
   RCC->CFGR |= RCC_CFGR_SW_PLL ;                     // System clock switch 01 : PLL selectes as system clock  
 
   SysTick_Config(64000000/1000);                     //ustawienie przerwania co 1ms
}

void HSE_with_PLL(void){
   
   //to narazie nie dziala
   RCC->CR|=0x40000;                   // RCC->CR |= RCC_CR_HSEBYP;   ok
   RCC->CFGR &= (uint32_t)~0x2000;     //RCC->CFGR |= RCC_CFGR_PLLXTPRE_HSE_PREDIV_DIV1;   nok
   RCC->CR |= 0x1;                     //RCC->CR |= RCC_CR_HSION;   ok
   RCC->CFGR |= 0x10000;               //RCC->CFGR |= RCC_CFGR_PLLSRC_HSE_PREDIV;   ok
   RCC->CFGR |= 0x1C0000;              //RCC->CFGR |= RCC_CFGR_PLLMUL9;   ok
   RCC->CFGR &= (uint32_t)~0x200000;
   RCC->CR |= 0x1000000;               //RCC->CR |= RCC_CR_PLLON;   
   
   RCC->CFGR &= (uint32_t)~0x400000;   //RCC->CFGR |= RCC_CFGR_USBPRE_DIV1_5;   nok
   RCC->CFGR &= (uint32_t)~0x800000;   //RCC->CFGR |= RCC_CFGR_I2SSRC_SYSCLK ;   nok
   RCC->CFGR |= 0x400;                 //RCC->CFGR |= RCC_CFGR_PPRE1_DIV2;   nok
   RCC->CFGR &= (uint32_t)~0x300;      
   RCC->CFGR &= (uint32_t)~0x2000;     //RCC->CFGR |= RCC_CFGR_PPRE2_DIV1;   nok
   RCC->CFGR &= (uint32_t)~0x80;       //RCC->CFGR |= RCC_CFGR_HPRE_DIV1;   nok
   
   //flash ustawienie opoznienia 2 cykli
   FLASH->ACR|= 0x2;
   FLASH->ACR &= (uint16_t)~0x5;
   
   RCC->CFGR |= 0x2;                   //RCC->CFGR |= RCC_CFGR_SW_PLL;   ok
   RCC->CFGR &= (uint32_t)~0x1;
   SysTick_Config(72000000/1000);
   
}

void TIM7_config(void){
   /* 
   Notatki basic Timer
   UIF - Update Interrupts flag
   UEV - Update Event
   CK_INT - Internal clock source 24MHz APB1
   
   */
   RCC->APB1ENR |= RCC_APB1ENR_TIM7EN; 
   TIM7->CR1 |= TIM_CR1_CEN;
   TIM7->PSC = 2000; // dzielnik ustawiony na 2000
   TIM7->CNT = 60000; // przerwanie co 5s  
   }

void TIM7_IRQHandler(void){
  static short k=2;
   if (k%2 ==0){
      GPIOE->ODR |= (uint16_t)Pin_13;
      k++;
   } else{
      GPIOE->ODR &= (uint16_t)~Pin_13;
      k=2;
   }
   TIM7->SR &= ~TIM_SR_UIF;
  
}   
