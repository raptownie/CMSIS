#include "../Headers/Timers.h"



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
   TIM1->CCER |= TIM_CCER_CC1E;                       // 1: On - OC1 signal is output on the corresponding output pin depending on MOE, OSSI, OSSR, OIS1, OIS1N and CC1NE bits.
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
   static uint16_t Tab_PWM_temp[100];
   if (FirstRun_GPIO_PWM_Init== 0) {
      PWM_TIM1_C1_Init(PWM_ARR_value, PWM_CCR1_value);
      static uint16_t exponenta = 0;
      for (int i=0; Tab_PWM_temp[i]<=PWM_ARR_value; i++, exponenta++)
         Tab_PWM_temp[i]=4*(uint16_t)(exponenta*exponenta);
         
      
      FirstRun_GPIO_PWM_Init =1;
   }   

   static uint8_t direction=1;
   static uint8_t p=0;
   delay_ms(30);
   
   //zmiana kierunku
   if (PWM_temp >= PWM_ARR_value|| PWM_temp ==0){
      direction++;
   }
   if (direction%2 == 0) {
      p++;
      PWM_temp = Tab_PWM_temp[p];
      TIM1->CCR1=PWM_temp;
    
   } else{
       p--;
      PWM_temp = Tab_PWM_temp[p];
      TIM1->CCR1=PWM_temp;
   }   
}



void GPIO_Init_PWM_TIM1(void){
   RCC->AHBENR |= RCC_AHBENR_GPIOEEN;
   GPIOE->MODER |= 0x80000;       // alternate func enabled PE9
   GPIOE->AFR[1] |= 0x20;          // ustawienie AF2 dla PE9
}
