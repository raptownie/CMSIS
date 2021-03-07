#include "../Headers/GPIO_LEDS_Buttons.h"

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
