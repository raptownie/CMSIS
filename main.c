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
void LEDy_kolo (void)__attribute__((noreturn));
void SysTick_Init(void);
void SysTick_Handler(void);
void delay_ms(uint32_t time);
void HSI_without_PLL(void);
void HSI_with_PLL(void);
void HSE_with_PLL(void);
void TIM7_config(void);
void TIM7_IRQHandler(void);
void TIM1_config(void);
void TIM1_UP_TIM16_IRQHandler(void);
void TIM1_CC_IRQHandler(void);

static volatile uint32_t timer_ms;
   
int main()
{ 
   HSI_with_PLL();
   //HSE_with_PLL();                                  //to narazie nie dziala
  // HSI_without_PLL();
   TIM7_config();
   //TIM1_config();
      
   GPIO_Init(); 
  
   while(1);                                          //potrzebne do dzialania przerwan od timera, inaczej program konczy dzialanie nic sie nie dzieje
   //LEDy_kolo();                                     //noreturn
   //GPIO_zPrzyciskiem();
   //GPIO_Kolko();  
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

void LEDy_kolo(void){
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
   }
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
      /* SysTick notatki
   W stmach zawsze po resecie zródlem sygnalu jest HSI (high speed internal), czyli wewnetrzny generator szybkich przebiegow, dla STM32F3 to 8MHz.
   Czestotliwosc ta moze byc zmieniona przez petle PLL (podzielona przez 2), nastepnie pomnozona.
   Sygnalem zegara systemowego moze byc rowniez HSE - czyli high speed external, zewnetrzny oscylator kwarc.   
   Dokumentacja funkcji SysTick_config() i rejestrow https://developer.arm.com/documentation/dui0552/a/cortex-m3-peripherals/system-timer--systick
   */      
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
   RCC->CFGR |= RCC_CFGR_PLLMUL16;                    // PLL input clock x16
      
   RCC->CR |= RCC_CR_PLLON;                           //wlaczenie PLL
   
   RCC->CFGR |=RCC_CFGR_PPRE1_DIV2;                   //APB1 div /2 - max32MHz wiec trzeba podzielic
   
   //flash ustawienie opoznienia 2 cykli konieczne przy taktowaniu 48-72MHz - bez tego hardfoult "acces memory..."
   FLASH->ACR|= 0x2;
   FLASH->ACR &= (uint16_t)~0x5;
   
   //Ustawienie zegarow APB1, APB2, USB, I2C, SYSCLK
   RCC->CFGR |= RCC_CFGR_USBPRE_DIV1_5;  
   RCC->CFGR |=RCC_CFGR_I2SSRC_SYSCLK ;   
   RCC->CFGR |=RCC_CFGR_PPRE1_DIV2;       
   RCC->CFGR |= RCC_CFGR_PPRE2_DIV1;   
   RCC->CFGR |=RCC_CFGR_HPRE_DIV1;   
   
   
   /*
      RCC->CFGR &= (uint32_t)~0x400000;                  //RCC->CFGR |= RCC_CFGR_USBPRE_DIV1_5;   nok
   RCC->CFGR &= (uint32_t)~0x800000;                  //RCC->CFGR |=RCC_CFGR_I2SSRC_SYSCLK ;   nok
   RCC->CFGR |= 0x400;                                //RCC->CFGR |=RCC_CFGR_PPRE1_DIV2;   nok
   RCC->CFGR &= (uint32_t)~0x300;      
   RCC->CFGR &= (uint32_t)~0x2000;                    //RCC->CFGR |= RCC_CFGR_PPRE2_DIV1;   nok
   RCC->CFGR &= (uint32_t)~0x80;                      //RCC->CFGR |=RCC_CFGR_HPRE_DIV1;   nok
   */
   
   RCC->CFGR |= RCC_CFGR_SW_PLL ;                     // System clock switch 01 : PLL selectes as system clock  
 
   SysTick_Config(64000000/1000);                     //ustawienie przerwania co 1ms
}

void HSE_with_PLL(void){
   
   //to narazie nie dziala - brak zewnetrznego sygnalu zegarowego
   RCC->CR|=0x40000;                                  // RCC->CR |= RCC_CR_HSEBYP;   ok
   RCC->CFGR &= (uint32_t)~0x2000;                    //RCC->CFGR |= RCC_CFGR_PLLXTPRE_HSE_PREDIV_DIV1;   nok
   RCC->CR |= 0x1;                                    //RCC->CR |= RCC_CR_HSION;   ok
   RCC->CFGR |= 0x10000;                              //RCC->CFGR |= RCC_CFGR_PLLSRC_HSE_PREDIV;   ok
   RCC->CFGR |= 0x1C0000;                             //RCC->CFGR |= RCC_CFGR_PLLMUL9;   ok
   RCC->CFGR &= (uint32_t)~0x200000;
   RCC->CR |= 0x1000000;                              //RCC->CR |= RCC_CR_PLLON;   
   
   RCC->CFGR &= (uint32_t)~0x400000;                  //RCC->CFGR |= RCC_CFGR_USBPRE_DIV1_5;   nok
   RCC->CFGR &= (uint32_t)~0x800000;                  //RCC->CFGR |= RCC_CFGR_I2SSRC_SYSCLK ;   nok
   RCC->CFGR |= 0x400;                                //RCC->CFGR |= RCC_CFGR_PPRE1_DIV2;   nok
   RCC->CFGR &= (uint32_t)~0x300;      
   RCC->CFGR &= (uint32_t)~0x2000;                    //RCC->CFGR |= RCC_CFGR_PPRE2_DIV1;   nok
   RCC->CFGR &= (uint32_t)~0x80;                      //RCC->CFGR |= RCC_CFGR_HPRE_DIV1;   nok
   
   //flash ustawienie opoznienia 2 cykli
   FLASH->ACR|= 0x2;
   FLASH->ACR &= (uint16_t)~0x5;
   
   RCC->CFGR |= 0x2;                                  //RCC->CFGR |= RCC_CFGR_SW_PLL;   ok
   RCC->CFGR &= (uint32_t)~0x1;
   SysTick_Config(72000000/1000);
   
}

void TIM7_config(void){
   /* 
   Notatki basic Timer
   UIF - Update Interrupts flag
   UEV - Update Event
   CK_INT - Internal clock source dla HSI 64Mhz APB1/2 jest 32MHz
   
   */
   RCC->APB1ENR |= RCC_APB1ENR_TIM7EN;                // podlaczenie zegara pod TIM7
   TIM7->PSC = 8000-1;                                // dzielnik ustawiony na 8000 - 64MHz/8000=8kHz
//   TIM7->CNT = 0x20000;                             // ??????? co robi ten rejestr?
   TIM7->ARR =40000;                                  // 8kHz -8 tys impulsow na 1 sekunde - przerwanie co 5s to 8000*5 =40k
   TIM7->DIER |= 0x1;                                 // Update interrupt enabled
   NVIC_SetPriority(TIM7_IRQn,1);   
   NVIC_EnableIRQ(TIM7_IRQn);
   TIM7->CR1 |= TIM_CR1_CEN;

   }

void TIM7_IRQHandler(void){
  static short k;
  
   if (k%2 ==0){
      
      GPIOE->ODR |= (uint16_t)Pin_13;
      GPIOE->ODR &= (uint16_t)~Pin_12;
      k++;

   } else{
      GPIOE->ODR |= (uint16_t)Pin_12;
      GPIOE->ODR &= (uint16_t)~Pin_13;

      k++;
   }
   k==2 ? k=2 : k ;
   TIM7->SR &= (uint16_t)~0x1;                        // clearowanie flagi przerwania
  
}   

void TIM1_config(void){
   RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;                // zegar taktowany 64MHz - bez dzielnika
   TIM1->PSC = 16000-1;                               // Ustawienie prescalera timera 64MHz/16k = 4k
   TIM1->ARR = 8000;                                  // 4kHz -> 4000 impulsow na 1s, a wiec aby okres licznika byl na 2s trzeba wstawic 8000
   TIM1->DIER |= TIM_DIER_UIE;                        // Update interrupt enabled
   NVIC_SetPriority(TIM1_UP_TIM16_IRQn,1);            // Ustawienie priorytetu licznika TIM1
   NVIC_EnableIRQ(TIM1_UP_TIM16_IRQn);                // Wlaczenie przerwania
   // konfigracja kanalow CC1, CC2, CC3
   //konfiguracja kanalu CC1 TIM1
   TIM1->CCR1 =5000;
   TIM1->DIER |= TIM_DIER_CC1IE;
   //konfiguracja kanalu CC2 TIM1
   TIM1->CCR2 =2000;
   TIM1->DIER |= TIM_DIER_CC2IE;
   //konfiguracja kanalu CC3 TIM1
   TIM1->CCR3 =6000;
   TIM1->DIER |= TIM_DIER_CC3IE;
   NVIC_SetPriority(TIM1_CC_IRQn,2);                  // ustawienie priorytetu kanalow licznika TIM1
   NVIC_EnableIRQ(TIM1_CC_IRQn);                      // wlaczenie przerwania od kanalow
   
   TIM1->CR1 |= TIM_CR1_CEN;                          // Wlaczenie licznika TIM1
}

void TIM1_UP_TIM16_IRQHandler(void){ 
   static short k=2;
   if(((TIM1->SR) & 0x1) == 0x1){
  
      if (k%2 ==0){      
         GPIOE->ODR |= (uint16_t)Pin_13;
         GPIOE->ODR &= (uint16_t)~Pin_12;
         k++;
      } else{
         GPIOE->ODR |= (uint16_t)Pin_12;
         GPIOE->ODR &= (uint16_t)~Pin_13;
         k++;
      }
      k>3 ? k=2 : k ;
      TIM1->SR &= ~TIM_SR_UIF;                        // Czyszczenie flagi glownego przerwania TIM1       

   }
}

void TIM1_CC_IRQHandler(void){
  
   static short p1=2;
   static short p2=2;
   static short p3=2;
   if(((TIM1->SR) & TIM_SR_CC1IF) == 0x2){            // sprawdzenie czy przerwanie wywolujace funkcje pochodzi od CC1
      if (p1%2 ==0){      
         GPIOE->ODR |= (uint16_t)Pin_15;
         GPIOE->ODR &= (uint16_t)~Pin_14;
         p1++;
      } else{
         GPIOE->ODR |= (uint16_t)Pin_14;
         GPIOE->ODR &= (uint16_t)~Pin_15;
         p1++;
      }
      p1>3 ? p1=2 : p1 ;
      TIM1-> SR &= ~TIM_SR_CC1IF;                     // Czyszczenie flagi przerwania od CC1
   }
   if(((TIM1->SR) & TIM_SR_CC2IF) == 0x4){            // sprawdzenie czy przerwanie wywolujace funkcje pochodzi od CC2
      if (p2%2 ==0){      
         GPIOE->ODR |= (uint16_t)Pin_10;
         GPIOE->ODR &= (uint16_t)~Pin_11;
         p2++;
      } else{
         GPIOE->ODR |= (uint16_t)Pin_11;
         GPIOE->ODR &= (uint16_t)~Pin_10;
         p2++;
      }
      p2>3 ? p2=2 : p2 ;
      TIM1-> SR &= ~TIM_SR_CC2IF;                     // Czyszczenie flagi przerwania od CC2
   }
      if(((TIM1->SR) & TIM_SR_CC3IF) == 0x8){         // sprawdzenie czy przerwanie wywolujace funkcje pochodzi od CC3
      if (p3%2 ==0){      
         GPIOE->ODR |= (uint16_t)Pin_8;
         GPIOE->ODR &= (uint16_t)~Pin_9;
         p3++;
      } else{
         GPIOE->ODR |= (uint16_t)Pin_9;
         GPIOE->ODR &= (uint16_t)~Pin_8;
         p3++;
      }
      p3>3 ? p3=2 : p3 ;
      TIM1-> SR &= ~TIM_SR_CC3IF;                     // Czyszczenie flagi przerwania od CC3                  
   }

}
