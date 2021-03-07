#include "../Headers/UserSystemInit.h"

void delay_ms(uint32_t time){
   timer_ms=time;
   while(timer_ms);
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

   RCC->CR |= RCC_CR_HSEBYP;                          
   RCC->CR |= RCC_CR_HSEON;
   
   uint16_t i=0;                                      //zmienna pomocnicza - zliczenie cykli po jakim czasie zegar HSE bedzie gotowy 
   while ((RCC->CR & RCC_CR_HSERDY) != 0x20000) i++;  // oczekiwanie az zordlo HSE bedzie stabilne - zmienna i pokazuje mi jak dlugo trwala 
   
   // konfig pentli PLL
   RCC->CR &= ~RCC_CR_PLLON; 
   RCC->CFGR |= RCC_CFGR_PLLXTPRE_HSE_PREDIV_DIV1; 
   RCC->CFGR |= RCC_CFGR_PLLSRC_HSE_PREDIV;
   RCC->CFGR |= RCC_CFGR_PLLMUL9;   
   RCC->CR |= RCC_CR_PLLON;
   
   uint16_t w=0;
   while ((RCC->CR & RCC_CR_PLLRDY) != 0x2000000) w++;
   
   //Ustawienie zegarow APB1, APB2, USB, I2C, SYSCLK
   RCC->CFGR |= RCC_CFGR_USBPRE_DIV1_5;
   RCC->CFGR |= RCC_CFGR_I2SSRC_SYSCLK ;
   RCC->CFGR |= RCC_CFGR_PPRE1_DIV2;
   RCC->CFGR |= RCC_CFGR_PPRE2_DIV1;
   RCC->CFGR |= RCC_CFGR_HPRE_DIV1;
   
   //Ustawienie dzielnika 4 na ADC 1 i 2
   RCC->CFGR2 |= RCC_CFGR2_ADCPRE12_DIV4;
   
   //flash ustawienie opoznienia 2 cykli
   FLASH->ACR |= FLASH_ACR_LATENCY_1;
  // FLASH->ACR|= 0x2;
  // FLASH->ACR &= (uint16_t)~0x5;
   
   //Wlaczenie sygnalu z PLL jako glownego zrodla taktowania
   RCC->CFGR |= RCC_CFGR_SW_PLL;
   SysTick_Config(72000000/1000);
   
}
