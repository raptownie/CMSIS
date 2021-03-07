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
   
   static float ADC2_Voltage;
   ADC2_Voltage = ((float)ADC2_Raw_value*3.3f)/4096.0f;     // przeliczenie wartosci RAW na napiecie   
   TIM1->CCR1= (uint32_t)((PWM_ARR_value *ADC2_Voltage)/3.3f);        // przeliczenie zmierzonego napiecia na procentowa wartosc wzgledem 3.3V   
 

}

void ADC2_with_DMA_Init (void){
   GPIO_Init_ADC2();                                  //init GPIO pod ADC
   PWM_TIM1_C1_Init(PWM_ARR_value, PWM_CCR1_value);   //Init PWM
   
   RCC->AHBENR |= RCC_AHBENR_ADC12EN;              //podlaczenie zegara do ADC1/ADC2
   
   ADC2->CFGR |= ADC_CFGR_CONT;                  // cont conv mode
   ADC2->SQR1 |= ADC_SQR1_SQ1_0;                    // set 8 bit  ->SQ1 set to channel 1 -> PA4
   //ADC2->SMPR1 |= ADC_SMPR1_SMP1_1;
   //ADC2->SMPR1 |= ADC_SMPR1_SMP1_0;
   //ADC2->CFGR |= ADC_CFGR_OVRMOD;
   ADC2->CFGR |= ADC_CFGR_AUTDLY;                 //AUTODELAY - automatyczne opoznienie pomiaru
   ADC2->CFGR &= ~ADC_CFGR_RES_0;                 // rozdzielczosc 12bitow - bity RES 00
   ADC2->CFGR &= ~ADC_CFGR_RES_1;
   
   ADC2->CFGR |= ADC_CFGR_DMAEN;                   // Wlaczenie trigera DMA dla ADC2           
   ADC2->CFGR |= ADC_CFGR_DMACFG;
   
   //konfiguracja przerwania 
  // ADC2->IER |= ADC_IER_ADRDYIE;                //wlaczenie przerwania od ADC
  // ADC2->IER |= ADC_IER_EOCIE;                  //przerwanie kiedy ADC zakonczy przetwarzanie End of convert - flaga przerwania clearuje sie po odczytaniu danych
   //NVIC_SetPriority(ADC1_2_IRQn,2);
   //NVIC_EnableIRQ(ADC1_2_IRQn);
   
   ADC2->CR |= ADC_CR_ADEN;                     // ADC enable control
   ADC2->CR |= ADC_CR_ADSTART;                  //ADC start of regular conversion (Note: Software is allowed to set ADSTART only when ADEN=1 and ADDIS=0)

   
   // konfiguracja DMA - musze uzyc DMA2 kanal 1 pocniewaz tak wynika z mapy polaczen - triger DMA dla ADC2 wyzwala wlasnie ten kanal
   RCC->CFGR |= RCC_AHBENR_DMA2EN;
   
      // zdefiniowanie skad dokad ma przepisywac dane
      /*
   static volatile uint32_t *pADCDR = &ADC2->DR;           // rejest DR - data register ADC2 - czyli RAW value z rejestru przetwornika
   static uint32_t *ADCRAW = &ADC2_Raw_value;    //   
   DMA2_Channel1->CPAR = (uint32_t)pADCDR;
   DMA2_Channel1->CMAR = (uint32_t)ADCRAW;
   */
    DMA2_Channel1->CPAR = (uint32_t)&ADC2->DR;
   DMA2_Channel1->CMAR = (uint32_t)&ADC2_Raw_value;
   
   DMA2_Channel1->CNDTR = 0x1;   
   
  // DMA2_Channel1->CCR |= DMA_CCR_PL_0;          //priority level - 11 - najwyzszy
 //  DMA2_Channel1->CCR |= DMA_CCR_PL_1;
   
   DMA2_Channel1->CCR |= DMA_CCR_MSIZE_0;       // memory size 16 bitow
   DMA2_Channel1->CCR &= ~DMA_CCR_MSIZE_1;
   DMA2_Channel1->CCR |= DMA_CCR_PSIZE_0;       // periph size 16bitów
   DMA2_Channel1->CCR &= ~DMA_CCR_PSIZE_1; 
   
   DMA2_Channel1->CCR |= DMA_CCR_CIRC;          //circular mode  - dzialanie ciagle
   DMA2_Channel1->CCR &= ~DMA_CCR_DIR;
   //DMA2_Channel1->CCR |= DMA_CCR_HTIE;
   //DMA2_Channel1->CCR |= DMA_CCR_TCIE;
   //DMA2_Channel1->CCR |=DMA_CCR_TEIE;
   
  // NVIC_SetPriority(DMA2_Channel1_IRQn, 0);
 // NVIC_EnableIRQ(DMA2_Channel1_IRQn);
   
   
 
   DMA2_Channel1->CCR |= DMA_CCR_EN;            //wlaczamy DMA po calej konfiguracji  
   

   
}


