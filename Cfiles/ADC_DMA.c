#include "../Headers/ADC_DMA.h"


void GPIO_Init_ADC2 (void){
   RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
   GPIOA->MODER |= 0x300;              // PA4 analog mode
   ////cos jeszcze????
}

void ADC2_Init (void){
   GPIO_Init_ADC2();                                  //init GPIO pod ADC
   PWM_TIM1_C1_Init(PWM_ARR_value, PWM_CCR1_value);   //Init PWM
   
   RCC->AHBENR |= RCC_AHBENR_ADC12EN;              //podlaczenie zegara do ADC1/ADC2
   
   ADC2->CFGR |= ADC_CFGR_CONT;                  // cont conv mode
   ADC2->SQR1 |= ADC_SQR1_SQ1_0;                    // set 8 bit  ->SQ1 set to channel 1 -> PA4
   //ADC2->SMPR1 |= ADC_SMPR1_SMP1_1;
  // ADC2->SMPR1 |= ADC_SMPR1_SMP1_0;
  // ADC2->CFGR |= ADC_CFGR_OVRMOD;
   ADC2->CFGR |= ADC_CFGR_AUTDLY;                 //AUTODELAY - automatyczne opoznienie pomiaru
   ADC2->CFGR &= ~ADC_CFGR_RES_0;                 // rozdzielczosc 12bitow - bity RES 00
   ADC2->CFGR &= ~ADC_CFGR_RES_1;
   
   //konfiguracja przerwania
   //ADC2->CFGR |= ADC_CFGR_EXTSEL_1;
   ADC2->IER |= ADC_IER_ADRDYIE;                //wlaczenie przerwania od ADC
   //ADC2->IER |= ADC_IER_OVRIE;
   ADC2->IER |= ADC_IER_EOCIE;                  //przerwanie kiedy ADC zakonczy przetwarzanie End of convert
   NVIC_SetPriority(ADC1_2_IRQn,2);
   NVIC_EnableIRQ(ADC1_2_IRQn);
   
   ADC2->CR |= ADC_CR_ADEN;                     // ADC enable control
   ADC2->CR |= ADC_CR_ADSTART;                  //ADC start of regular conversion (Note: Software is allowed to set ADSTART only when ADEN=1 and ADDIS=0)
}

void ADC1_2_IRQHandler(void){
   
  ADC2_Raw_value = ADC2->DR;
   
  ADC2->ISR |= ADC_ISR_EOC;
}

void ADC_control_PWM_Led (void){

   static float ADC2_Voltage=0;
   ADC2_Voltage = ((float)ADC2_Raw_value*3.3f)/4096.0f;     // przeliczenie wartosci RAW na napiecie
   TIM1->CCR1= (uint32_t)((100 *ADC2_Voltage)/3.3f);        // przeliczenie zmierzonego napiecia na procentowa wartosc wzgledem 3.3V   

}
