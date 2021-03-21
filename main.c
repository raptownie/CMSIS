// cwiczenie 1 GPIO
#include<stdio.h>
#include "stm32f3xx.h"         // Device header
#include "Headers/UserSystemInit.h"
#include "Headers/Timers.h"
#include "Headers/EXTI.h"
#include "Headers/GPIO_LEDS_Buttons.h"
#include "Headers/ADC_DMA.h"
#include "Headers/SPI.h"
 
volatile uint32_t timer_ms;
uint8_t EXTI0_flag; 
uint8_t FirstRun_GPIO_PWM_Init = 0;
uint8_t FirstRun_GPIO_Init = 0; 
uint8_t PWM_status_ON = 0;
uint16_t PWM_temp;
uint16_t PWM_ARR_value = 2047;
uint8_t PWM_CCR1_value = 0;
uint32_t ADC2_Raw_value=0;
float ADC2_Voltage=0; 
uint32_t SPI_L3GD2_Read;

volatile int8_t SPI_L3GD2_XL;
volatile int8_t SPI_L3GD2_XH;
volatile int8_t SPI_L3GD2_YL;
volatile int8_t SPI_L3GD2_YH;
volatile int8_t SPI_L3GD2_ZL;
volatile int8_t SPI_L3GD2_ZH;
volatile int16_t X_value;
volatile int16_t Y_value;
volatile int16_t Z_value;


int8_t tab_SPI_L3GD20_XYZ_values[6];
const uint8_t tab_SPI_L3GD20_XYZ_adress[6]= {0xE8,0xE9,0xEA,0xEB,0xEC,0xED};  // L3GD20 - XL, XH, YL, YH, ZL, ZH


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
   //SPI_Config_for_Gyroskop();
   L3GD20_DMA_Init();
 //ADC2_Init();
   //ADC2_with_DMA_Init(); 
   //L3GD20_Init();
  
   while (1){        
       delay_ms(50); 
      
   L3GD20_XYZ_Calculate();     
/*
  
   SPI_L3GD2_YL = (int8_t)Read_SPI(0x2A);   
   SPI_L3GD2_YH = (int8_t)Read_SPI(0x2B);   
   Y_value = (int16_t)(SPI_L3GD2_YL |(SPI_L3GD2_YH << 8));
   //Y_value += (SPI_L3GD2_YL << 8);
   SPI_L3GD2_ZL = (int8_t)Read_SPI(0x2C);  
   SPI_L3GD2_ZH = (int8_t)Read_SPI(0x2D);   
   Z_value = (int16_t)(SPI_L3GD2_ZL |(SPI_L3GD2_ZH <<8));
  // Z_value += (SPI_L3GD2_ZL <<8);
     SPI_L3GD2_XL = (int8_t)Read_SPI(0x28);     
   SPI_L3GD2_XH = (int8_t)Read_SPI(0x29);      
   X_value = (int16_t)(SPI_L3GD2_XL | (SPI_L3GD2_XH << 8));  
    // X_value += (SPI_L3GD2_XL << 8);   
      */
      //delay_ms(100);
        //ADC2_Raw_value = ADC2->DR;
      // *** zabawa z LED ****
      //LEDy_kolo();                                     //noreturn
      //GPIO_zPrzyciskiem();
      //GPIO_Kolko();  
      //ADC_control_PWM_Led();
       
      
      // *** PWM ***
      //Zmiana_PWM_TIM1_Button();
      Zmiana_PWM_TIM1_stopniowo();
      
      //Debouncing_SW_LPF();
      
        
   }
   
}






