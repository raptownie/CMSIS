// cwiczenie 1 GPIO
#include<stdio.h>
#include "stm32f3xx.h"         // Device header

#define Pin_8 0x100
#define Pin_9 0x200
#define Pin_10 0x400
#define Pin_11 0x800
#define Pin_12 0x1000
#define Pin_13 0x2000
#define Pin_14 0x4000
#define Pin_15 0x8000
#define HSI_Clock 8000000        // bez PLL


//deklaracje funkcji
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
void EXTI0_config(void);
void EXTI0_IRQHandler(void);
void Debouncing_SW_LPF (void);
void GPIO_Init_PWM_TIM1(void);
void PWM_TIM1_C1_Init(uint32_t ARR_value, uint32_t CCR1_value);
void Zmiana_PWM_TIM1_Button(void);
void Zmiana_PWM_TIM1_stopniowo(void);
void EXTI0_config_PWMButton(void);

// deklaracje zmiennych
static uint8_t EXTI0_flag;                            //flaga wykorzystywana do konfiguracji Timera 1 - przerwania od przycisku PA0 - exti0
static volatile uint32_t timer_ms;
static uint8_t FirstRun_GPIO_Init = 0; 
static uint8_t FirstRun_GPIO_PWM_Init = 0;
static uint8_t PWM_temp = 0;
static uint8_t PWM_status_ON = 0;
static uint8_t PWM_ARR_value = 100;
static uint8_t PWM_CCR1_value = 0;

 
int main()
{ 
   // *** Wybor zegara ***
   //HSI_with_PLL();   
   HSE_with_PLL();                                    //taktowanie 72MHz                              
   //HSI_without_PLL();                               //taktowanie z 8MHz
   
   // *** Inicjalizacja GPIO - ledy + przycisk ***
   //GPIO_Init();
   
   // *** TIM1 - licznik advanced, TIM7 - licznik basic (miganie dioda) ***
   //TIM7_config();
   //TIM1_config();

   // *** Przycisk (PA0) miganie diodami - nieblokujace***
   //EXTI0_config();
      
 
   while (1)
   {  
      // *** zabawa z LED ****
      //LEDy_kolo();                                     //noreturn
      //GPIO_zPrzyciskiem();
      //GPIO_Kolko();  
      
      
      // *** PWM ***
      //Zmiana_PWM_TIM1_Button();
      Zmiana_PWM_TIM1_stopniowo();
      
      //Debouncing_SW_LPF();
      
        
   }
   
}

void delay_ms(uint32_t time){
   timer_ms=time;
   while(timer_ms);
}

void GPIO_zPrzyciskiem(void){   
   if (FirstRun_GPIO_Init== 0){
      GPIO_Init();
      FirstRun_GPIO_Init = 1;
   }
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
   if (FirstRun_GPIO_Init == 0){
      GPIO_Init();
      FirstRun_GPIO_Init = 1;
   }
   
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
   if (FirstRun_GPIO_Init == 0){
      GPIO_Init();
      FirstRun_GPIO_Init = 1;
   }
   
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
   
   //flash ustawienie opoznienia 2 cykli
   FLASH->ACR |= FLASH_ACR_LATENCY_1;
  // FLASH->ACR|= 0x2;
  // FLASH->ACR &= (uint16_t)~0x5;
   
   //Wlaczenie sygnalu z PLL jako glownego zrodla taktowania
   RCC->CFGR |= RCC_CFGR_SW_PLL;
   SysTick_Config(72000000/1000);
   
}

void TIM7_config(void){
   /* 
   Notatki basic Timer
   UIF - Update Interrupts flag
   UEV - Update Event
   CK_INT - Internal clock source dla HSI 64Mhz APB1/2 jest 32MHz
   
   */
   if (FirstRun_GPIO_Init == 0){
      GPIO_Init();
      FirstRun_GPIO_Init = 1;
   }
   RCC->APB1ENR |= RCC_APB1ENR_TIM7EN;                // podlaczenie zegara pod TIM7
   TIM7->PSC = 16000-1;                                // dzielnik ustawiony na 8000 - 64MHz/8000=8kHz
   TIM7->ARR = 6000;                                  // 8kHz -8 tys impulsow na 1 sekunde - przerwanie co 5s to 8000*5 =40k
   TIM7->DIER |= TIM_DIER_UIE;                                 // Update interrupt enabled
   NVIC_SetPriority(TIM7_IRQn,1);   
   NVIC_EnableIRQ(TIM7_IRQn);
   TIM7->CR1 |= TIM_CR1_CEN;

   }

void TIM7_IRQHandler(void){
   /*
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
   k>3 ? k=2 : k ;
   */
   GPIOE->ODR ^= (uint16_t)Pin_13;
   TIM7->SR &= ~TIM_CR1_CEN;                        // clearowanie flagi przerwania
  
}   

void TIM1_config(void){
   if (FirstRun_GPIO_Init == 0){
      GPIO_Init();
      FirstRun_GPIO_Init = 1;
   }
   
   RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;                // zegar taktowany 64MHz - bez dzielnika
   TIM1->PSC = 16000-1;                               // Ustawienie prescalera timera 64MHz/16k = 4k
   TIM1->ARR = 13500;                                  // 4kHz -> 4000 impulsow na 1s, a wiec aby okres licznika byl na 2s trzeba wstawic 8000
   TIM1->DIER |= TIM_DIER_UIE;                        // Update interrupt enabled
   NVIC_SetPriority(TIM1_UP_TIM16_IRQn,1);            // Ustawienie priorytetu licznika TIM1
   NVIC_EnableIRQ(TIM1_UP_TIM16_IRQn);                // Wlaczenie przerwania
   if (EXTI0_flag != 1){ 
      // konfigracja kanalow CC1, CC2, CC3
      //konfiguracja kanalu CC1 TIM1
      TIM1->CCR1 =3500;
      TIM1->DIER |= TIM_DIER_CC1IE;
      //konfiguracja kanalu CC2 TIM1
      TIM1->CCR2 =10000;
      TIM1->DIER |= TIM_DIER_CC2IE;
      //konfiguracja kanalu CC3 TIM1
      TIM1->CCR3 =6000;
      TIM1->DIER |= TIM_DIER_CC3IE;
      NVIC_SetPriority(TIM1_CC_IRQn,2);                  // ustawienie priorytetu kanalow licznika TIM1
      NVIC_EnableIRQ(TIM1_CC_IRQn);                      // wlaczenie przerwania od kanalow
      
   }
   
   
   
   TIM1->CR1 |= TIM_CR1_CEN;                          // Wlaczenie licznika TIM1
}

void TIM1_UP_TIM16_IRQHandler(void){ 
     
   static short k=2;
   if (EXTI0_flag == 1){
      
      GPIOE->ODR ^= (uint16_t)Pin_13;
      GPIOE->ODR ^= (uint16_t)Pin_12;
      
      EXTI->PR |= EXTI_PR_PR0;
      TIM1->SR &= ~TIM_SR_UIF; 
      TIM1->CR1 &= ~TIM_CR1_CEN;
  
   }else{
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

          k>3 ? k=2 : k ;
         TIM1->SR &= ~TIM_SR_UIF;  
      }
   }   
}

void TIM1_CC_IRQHandler(void){
  
   static short p1=2;
   static short p2=2;
   static short p3=2;
   volatile uint32_t rejestry = TIM1->SR;
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

void EXTI0_config(void){
   // konfiguracja PA0 jako przerwanie zewnerzne
   SYSCFG->EXTICR[1] |= SYSCFG_EXTICR1_EXTI0_PA;   //podlaczenie PA0 do lini EXTI0; EXTICR1,EXTICR2... zdefiniowane sa jako tablice, dlatego taki zapis
   EXTI->IMR |= EXTI_IMR_MR0;                      // IRM - interrupt mask register - wlaczenie maskowania dla lini EXTI0 
   EXTI->RTSR |= EXTI_RTSR_RT0;                    // RTSR - Raising Trigger Selection register
   NVIC_SetPriority(EXTI0_IRQn, 4);
   NVIC_EnableIRQ(EXTI0_IRQn);
   EXTI0_flag = 1;                                 //flage te wykorzystuje po to by nie musiec wykorzystywac kolejnego licznika (do konfiguracji TIM1)
   TIM1_config();
}

void EXTI0_IRQHandler(void){ 
   // dzialanie nieblokujace z wykorzystaniem licznika TIM1 
   if(((EXTI->PR & EXTI_PR_PR0) == EXTI_PR_PR0) && PWM_status_ON == 0 ){     //sprawdzenie czy zródlem przerwania jest EXTI0
      TIM1->CR1 |= TIM_CR1_CEN;      
      EXTI->PR |= EXTI_PR_PR0;                     // clearowanie flagi przerwania
   }
   //Przycisk ktory zmienia ustawienia PWM
   if(((EXTI->PR & EXTI_PR_PR0) == EXTI_PR_PR0) && PWM_status_ON == 1){     //sprawdzenie czy zródlem przerwania jest EXTI0
      PWM_temp +=(PWM_ARR_value/4);
      if (PWM_temp >=PWM_ARR_value) PWM_temp=0; 
      EXTI->PR |= EXTI_PR_PR0;
   }
   
}

void Debouncing_SW_LPF (void){
   static uint16_t button_pressed = 0;
   static uint16_t button_unpressed = 0;
   static uint16_t debounce_value = 500;
   static uint8_t wait_for_next_press = 0;
   
 
   if (((GPIOA->IDR) & 0x1 ) == (uint16_t)0x0001){
      button_pressed++;
      button_unpressed = 0;
      if (button_pressed > debounce_value && wait_for_next_press == 0){
         GPIOE->ODR ^= (uint16_t)Pin_15; 
         wait_for_next_press = 1;
         
      }             
   } else{
      button_unpressed++;
      button_pressed = 0;
      if (button_unpressed>debounce_value && wait_for_next_press == 1) {         
         wait_for_next_press =0;
      }
   }     
}

void GPIO_Init_PWM_TIM1(void){
   RCC->AHBENR |= RCC_AHBENR_GPIOEEN;
   GPIOE->MODER |= 0x80000;       // alternate func enabled PE9
   GPIOE->AFR[1] |= 0x20;          // ustawienie AF2 dla PE9
}

void PWM_TIM1_C1_Init(uint32_t ARR_value, uint32_t CCR1_value){

   GPIO_Init_PWM_TIM1();
   
   // konfiguracja na podstawie reference manual 20.3.11 PWM mode  str 535
   RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;                // zegar taktowany 72MHz - bez dzielnika

  // TIM1->PSC = 100-1;                               // Tutaj prescaler nie jest potrzebny - im wieksza czestotliwosc tym plynniejsze przejscie - niezauwazalne dla ludzkiego oka
   TIM1->ARR = ARR_value;                             // ustawienie okresu
   TIM1->CCR1 = CCR1_value;                           // wypelnienie impulsu
   TIM1->CCMR1 |= 0x60;                               // 1100: Combined PWM mode 1 - OC1REF has the same behavior as in PWM mode 1.    

   // TIM BDTR - break and dead-time register
   TIM1->BDTR |= TIM_BDTR_MOE;                        // 1: OC and OCN outputs are enabled if their respective enable bits are set (CCxE, CCxNE in TIMx_CCER register).
   TIM1->CCER |= TIM_CCER_CC1E;                       //1: On - OC1 signal is output on the corresponding output pin depending on MOE, OSSI, OSSR, OIS1, OIS1N and CC1NE bits.
   //TIM1->CCER |= TIM_CCER_CC1NE;
   TIM1->BDTR |= TIM_BDTR_OSSR;                        

   TIM1->CR1 |= TIM_CR1_CEN;                          // Wlaczenie licznika TIM1
}

void Zmiana_PWM_TIM1_Button(void){
   if (FirstRun_GPIO_PWM_Init== 0) {
      PWM_TIM1_C1_Init(PWM_ARR_value, PWM_CCR1_value);
      EXTI0_config_PWMButton();
      FirstRun_GPIO_PWM_Init =1;
   }   
   TIM1->CCR1=PWM_temp;
}

void Zmiana_PWM_TIM1_stopniowo(void){
   if (FirstRun_GPIO_PWM_Init== 0) {
      PWM_TIM1_C1_Init(PWM_ARR_value, PWM_CCR1_value);
      FirstRun_GPIO_PWM_Init =1;
   }   
   static uint8_t direction=1;
   delay_ms(30);
   
   //zmiana kierunku
   if (PWM_temp >= PWM_ARR_value|| PWM_temp ==0){
      direction++;
   }
   if (direction%2 == 0) {
      TIM1->CCR1=++PWM_temp;
    
   } else{
      TIM1->CCR1=--PWM_temp;

   }
   
   
}

void EXTI0_config_PWMButton(void){
   // konfiguracja PA0 jako przerwanie zewnerzne
   SYSCFG->EXTICR[1] |= SYSCFG_EXTICR1_EXTI0_PA;   //podlaczenie PA0 do lini EXTI0; EXTICR1,EXTICR2... zdefiniowane sa jako tablice, dlatego taki zapis
   EXTI->IMR |= EXTI_IMR_MR0;                      // IRM - interrupt mask register - wlaczenie maskowania dla lini EXTI0 
   EXTI->RTSR |= EXTI_RTSR_RT0;                    // RTSR - Raising Trigger Selection register
   NVIC_SetPriority(EXTI0_IRQn, 4);
   NVIC_EnableIRQ(EXTI0_IRQn);
   PWM_status_ON=1;

}


