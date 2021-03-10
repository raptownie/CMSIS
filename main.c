// cwiczenie 1 GPIO
#include<stdio.h>
#include "stm32f3xx.h"         // Device header
#include "Headers/UserSystemInit.h"
#include "Headers/Timers.h"
#include "Headers/EXTI.h"
#include "Headers/GPIO_LEDS_Buttons.h"
#include "Headers/ADC_DMA.h"
#include "Headers/SPI.h"
 
int main()
{ 
   // *** Wybor zegara ***
   //HSI_with_PLL();   
   HSE_with_PLL();                                    //taktowanie 72MHz                              
   //HSI_without_PLL();                               //taktowanie z 8MHz
   
   // *** Inicjalizacja GPIO - ledy + przycisk ***
  // GPIO_Init();
   
   // *** TIM1 - licznik advanced, TIM7 - licznik basic (miganie dioda) ***
   //TIM7_config();
   //TIM1_config();

   // *** Przycisk (PA0) miganie diodami - nieblokujace***
   //EXTI0_config();
      SPI_Gyroskop_Config();
 //ADC2_Init();
   //ADC2_with_DMA_Init();
   while (1)
   {  
      //delay_ms(100);
        //ADC2_Raw_value = ADC2->DR;
      // *** zabawa z LED ****
      //LEDy_kolo();                                     //noreturn
      //GPIO_zPrzyciskiem();
      //GPIO_Kolko();  
      //ADC_control_PWM_Led();
       
      
      // *** PWM ***
      //Zmiana_PWM_TIM1_Button();
      //Zmiana_PWM_TIM1_stopniowo();
      
      //Debouncing_SW_LPF();
      
        
   }
   
}




