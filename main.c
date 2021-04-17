// cwiczenie 1 GPIO
#include<stdio.h>
#include "stm32f3xx.h"         // Device header
#include "Headers/UserSystemInit.h"
#include "Headers/Timers.h"
#include "Headers/EXTI.h"
#include "Headers/GPIO_LEDS_Buttons.h"
#include "Headers/ADC_DMA.h"
#include "Headers/SPI.h"
#include "Headers/I2C.h"
#include "Headers/UART.h"
#include <stdio.h>
//#include "Headers/USB_setup.h"


/** USB_LIB**/
#include "USB_LIB/usb_device.h"
#include "USB_LIB/usbd_cdc_if.h"
 
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

//Gyroskop
volatile int8_t SPI_L3GD2_XL;
volatile int8_t SPI_L3GD2_XH;
volatile int8_t SPI_L3GD2_YL;
volatile int8_t SPI_L3GD2_YH;
volatile int8_t SPI_L3GD2_ZL;
volatile int8_t SPI_L3GD2_ZH;
volatile int16_t SPI_L3GD2_X_value;
volatile int16_t SPI_L3GD2_Y_value;
volatile int16_t SPI_L3GD2_Z_value;
int8_t tab_SPI_L3GD20_XYZ_values[6];
const uint8_t tab_SPI_L3GD20_XYZ_adress[6]= {0xE8,0xE9,0xEA,0xEB,0xEC,0xED};  // L3GD20 - XL, XH, YL, YH, ZL, ZH

//Accelero + Magneto
volatile uint8_t Read_value_LSM303DLHC_A;
volatile int8_t I2C_LSM303DLHC_A_XL;
volatile int8_t I2C_LSM303DLHC_A_XH;
volatile int8_t I2C_LSM303DLHC_A_YL;
volatile int8_t I2C_LSM303DLHC_A_YH;
volatile int8_t I2C_LSM303DLHC_A_ZL;
volatile int8_t I2C_LSM303DLHC_A_ZH;
volatile int16_t I2C_LSM303DLHC_A_X_value;
volatile int16_t I2C_LSM303DLHC_A_Y_value;
volatile int16_t I2C_LSM303DLHC_A_Z_value;
static volatile uint8_t Read_value_LSM303DLHC_M;

//UART
char StringToSendUART[40];
char StringToReciveUART[20];
uint8_t SizeOfDataToSendUART4;
uint8_t SizeOfDataToReciveUART4 = 20;

//USB
uint8_t Wiadomosc[40]; // Tablica zawierajaca dane do wyslania
uint8_t *pWiadomosc = &Wiadomosc[0];
uint8_t DlugoscWiadomosci = 0; // Zawiera dlugosc wysylanej wiadomosci

uint8_t OdebranaWiadomosc[40]; // Tablica przechowujaca odebrane dane
uint8_t ReceivedDataFlag = 0; // Flaga informujaca o odebraniu danych

int main()
 { 
    HAL_Init();
    //HAL_Init();
   // *** Wybor zegara ***
   //HSI_with_PLL();   
   HSE_with_PLL();                                    //taktowanie 72MHz                              
   //HSI_without_PLL();                               //taktowanie z 8MHz
    
       /*** USB ***/ 
 //  RCC->APB1ENR |= RCC_APB1ENR_USBEN;
   MX_USB_DEVICE_Init();
 // RCC->APB1ENR |= RCC_APB1ENR_USBEN;

   
   // *** Inicjalizacja GPIO - ledy + przycisk ***
  // GPIO_Init();
   
   // *** TIM1 - licznik advanced - PWM, TIM7 - licznik basic (miganie dioda) ***
   //TIM7_config();
   //TIM1_config();

   // *** Przycisk (PA0) miganie diodami - nieblokujace***
   //EXTI0_config();
    
   //*** SPI GYROSKOP L3GD20 ****/
   //SPI_Config_for_Gyroskop();
   //L3GD20_DMA_Init();
   //L3GD20_Init();
    
   /*** Pomiary ADC ***/
   //ADC2_Init();
   //ADC2_with_DMA_Init(); 
   
   
   /*** I2C Acclereo + Magneto ***/
   //I2C_LSM303DLHC_Init();
   //I2C_LSM303DLHC_Config_Init();  
   
   /*** UART4 Init (Tx - PC10, RX - PC11) ***/
   //UART4_Init();
   //UART4_Init_with_DMA();
   //UART4_Init_with_DMA_TIM7();
   
  // delay_ms(500);

   
   while (1){             
     
//      sprintf(StringToSendUART, "Gyroskop X Value = %5d\r\n", SPI_L3GD2_X_value);
  //    SizeOfDataToSendUART4 = sizeof(StringToSendUART);
      //UART4_SendString(StringToSendUART);
      
      //UART4_SendString(StringToReciveUART);
   
      
      /*** Read Accelerometer Values via I2C - blocking mode ***/
  //    I2C_LSM303DLHC_A_Read_XYZ(); 
      /*** Calculate Gyroskop values via SPI - DMA***/
  //    L3GD20_XYZ_Calculate();     
       
      /*** zabawa z LED ****/
      //LEDy_kolo();                                     //noreturn
      //GPIO_zPrzyciskiem();
      //GPIO_Kolko();  
      //ADC_control_PWM_Led();
       
      /*** USB ***/
      //wyslanie wiadomosci
      delay_ms(10);
      DlugoscWiadomosci = sprintf((char *)pWiadomosc, "Siemasz Krzysiek\n\rWiadomosc z KEILa\n\r");
		CDC_Transmit_FS(Wiadomosc, DlugoscWiadomosci);
      delay_ms(50);
      // Odeslanie odebranych danych przez USB
      if(ReceivedDataFlag == 1){
         ReceivedDataFlag = 0; 
         DlugoscWiadomosci = sprintf((char *)pWiadomosc, "Odebrano: %s\n\r", OdebranaWiadomosc);
         CDC_Transmit_FS(Wiadomosc, DlugoscWiadomosci);
      }
      
      /*** PWM ***/
      //Zmiana_PWM_TIM1_Button();
      //Zmiana_PWM_TIM1_stopniowo();
      
      /*** Software filter low pass - push button debouncing ***/
      Debouncing_SW_LPF();
      
        
   }
   
}

void Error_Handler(void)
{
   
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}




