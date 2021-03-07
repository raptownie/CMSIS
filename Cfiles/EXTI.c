#include "../Headers/EXTI.h"

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

void EXTI0_config_PWMButton(void){
   // konfiguracja PA0 jako przerwanie zewnerzne
   SYSCFG->EXTICR[1] |= SYSCFG_EXTICR1_EXTI0_PA;   //podlaczenie PA0 do lini EXTI0; EXTICR1,EXTICR2... zdefiniowane sa jako tablice, dlatego taki zapis
   EXTI->IMR |= EXTI_IMR_MR0;                      // IRM - interrupt mask register - wlaczenie maskowania dla lini EXTI0 
   EXTI->RTSR |= EXTI_RTSR_RT0;                    // RTSR - Raising Trigger Selection register
   NVIC_SetPriority(EXTI0_IRQn, 4);
   NVIC_EnableIRQ(EXTI0_IRQn);
   PWM_status_ON=1;
}

