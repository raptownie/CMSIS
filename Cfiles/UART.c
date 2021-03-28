#include "../Headers/UART.h"


void UART4_Init(void){
   
   //UART4  - Rx->PC11; Tx->PC10 */
   /******* Konfiguracja GPIO *******/  
   RCC->AHBENR |= RCC_AHBENR_GPIOCEN;                                         // enabled RCC for GPIOC
   
   // Set AF mode for PC10 and PC11
   GPIOC->MODER |= GPIO_MODER_MODER10_1 | GPIO_MODER_MODER11_1; 
   GPIOC->MODER &= ~GPIO_MODER_MODER10_0 | ~GPIO_MODER_MODER11_0; 
   
   GPIOC->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR10_1 | GPIO_OSPEEDER_OSPEEDR10_0;
   GPIOC->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR11_1 | GPIO_OSPEEDER_OSPEEDR11_0;
   
   GPIOC->AFR[1] |= (5 << 8) | (5 << 12);                                     //UART TX/RX AF5 -> 0b0101 = 5   
   
   /****** Konfiguracja UART4*******/
   RCC->APB1ENR |= RCC_APB1ENR_UART4EN;
   UART4->CR1 &= ~USART_CR1_M0 | ~(1U << 28);
   
   // PLCK1= HCLK/2 = 72MHz/2 = 36Mhz
   UART4->CR1 &= ~USART_CR1_OVER8;                                            // set oversampling 16;
   UART4->BRR = 0xEA6;                                                        // Baudrate = 115200; dla Oversamplingu 16 BRR = USARTDIV = PLCK1/Baudrate = 36000000/115200 = 312 = 0x138
   UART4->CR1 &= ~USART_CR1_PCE;                                              // disabled parity control
   UART4->CR2 &= ~USART_CR2_STOP_0 | ~USART_CR2_STOP_1;                       // set stop = 1bit
   
   // Interrupt RX
   UART4->CR1 |= USART_CR1_RXNEIE;
   NVIC_SetPriority(UART4_IRQn, 2);
   NVIC_EnableIRQ(UART4_IRQn);   
   
   UART4->CR1 |= USART_CR1_TE;                                                //transmiter enabled
   UART4->CR1 |= USART_CR1_RE;                                                //receiver enabled
   UART4->CR1 |= USART_CR1_UE;                                                // enabled UART4
}


void UART4_SendChar (uint8_t znak){
   UART4->TDR = znak;                                                         // wyslanie znaku 
   while (!(UART4->ISR & USART_ISR_TC));                                      //oczekiwanie az transfer zostanie zakonczny
}

void UART4_SendString (char *string){
   while (*string) UART4_SendChar (*string++);
}

uint8_t UART4_GetChar (void){
	while (!(UART4->ISR & USART_ISR_RXNE)){}                                    // wait for RXNE bit to set
 	return (uint8_t)UART4->RDR;                                                // Read the data. When RDR is read RXNE is cleared
}

void UART4_IRQHandler(void){
   
  //nietrzeba clearowac flagi przerwania, robi sie to "automatycznie" po tym jak bufor RX jest pusty
   UART4_SendChar(UART4_GetChar());
}

void UART4_Init_with_DMA(void){
    //UART4  - Rx->PC11; Tx->PC10 */
   /******* Konfiguracja GPIO *******/  
   RCC->AHBENR |= RCC_AHBENR_GPIOCEN;                                         // enabled RCC for GPIOC
   
   // Set AF mode for PC10 and PC11
   GPIOC->MODER |= GPIO_MODER_MODER10_1 | GPIO_MODER_MODER11_1; 
   GPIOC->MODER &= ~GPIO_MODER_MODER10_0 | ~GPIO_MODER_MODER11_0; 
   
   GPIOC->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR10_1 | GPIO_OSPEEDER_OSPEEDR10_0;
   GPIOC->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR11_1 | GPIO_OSPEEDER_OSPEEDR11_0;
   
   GPIOC->AFR[1] |= (5 << 8) | (5 << 12);                                     //UART TX/RX AF5 -> 0b0101 = 5   
   
   /****** Konfiguracja UART4*******/
   RCC->APB1ENR |= RCC_APB1ENR_UART4EN;
   UART4->CR1 &= ~USART_CR1_M0 | ~(1U << 28);
   
   // PLCK1= HCLK/2 = 72MHz/2 = 36Mhz
   UART4->CR1 &= ~USART_CR1_OVER8;                                            // set oversampling 16;
   UART4->BRR = 0xEA6;                                                        // Baudrate = 115200; dla Oversamplingu 16 BRR = USARTDIV = PLCK1/Baudrate = 36000000/115200 = 312 = 0x138
   UART4->CR1 &= ~USART_CR1_PCE;                                              // disabled parity control
   UART4->CR2 &= ~USART_CR2_STOP_0 | ~USART_CR2_STOP_1;                       // set stop = 1bit
   
   //DMA 
   UART4->CR3 |= USART_CR3_DMAT;                                              // enabled DMA transmit
   UART4->CR3 |= USART_CR3_DMAR;
   UART4_DMA_config();

   UART4->CR1 |= USART_CR1_TE;                                                //transmiter enabled
   UART4->CR1 |= USART_CR1_RE;                                                //receiver enabled
   UART4->CR1 |= USART_CR1_UE;                                                // enabled UART4
   
   
   
}

void UART4_DMA_config(void){     
   
   RCC->AHBENR |= RCC_AHBENR_DMA2EN;
      
   // DMA UART4_Tx config - DMA2 channel 5
   DMA2_Channel5->CPAR = (uint32_t)&UART4->TDR;
   DMA2_Channel5->CMAR = (uint32_t)StringToSendUART;   

   DMA2_Channel5 ->CCR &= ~DMA_CCR_MEM2MEM;       // disable Memory 2 memory transfer
   
   DMA2_Channel5->CNDTR = 0x1B;   
   
   DMA2_Channel5->CCR |= DMA_CCR_PL_0;          //priority level - 11 - najwyzszy
   DMA2_Channel5->CCR &= ~DMA_CCR_PL_1;
   
   DMA2_Channel5->CCR &= ~DMA_CCR_MSIZE_0;       // memory size 8 bitow
   DMA2_Channel5->CCR &= ~DMA_CCR_MSIZE_1;
   DMA2_Channel5->CCR &= ~DMA_CCR_PSIZE_0;       // periph size 8bitów
   DMA2_Channel5->CCR &= ~DMA_CCR_PSIZE_1; 
   
   DMA2_Channel5->CCR |= DMA_CCR_MINC;         // Memory inc enabled
   DMA2_Channel5->CCR &= ~DMA_CCR_PINC;         // Periph inc disabled;
   
   DMA2_Channel5->CCR |= DMA_CCR_CIRC;          //circular mode  - dzialanie ciagle
   DMA2_Channel5->CCR |= DMA_CCR_DIR;          // read from memory to periph
 
   DMA2_Channel5->CCR |= DMA_CCR_EN;            //wlaczamy DMA po calej konfiguracji
   
   /************************************************************/
   
   // DMA UART4_RX config - DMA2 channel 3
   DMA2_Channel3->CPAR = (uint32_t)&UART4->RDR;
   DMA2_Channel3->CMAR = (uint32_t)StringToReciveUART;   

   DMA2_Channel3 ->CCR &= ~DMA_CCR_MEM2MEM;       // disable Memory 2 memory transfer
   
   DMA2_Channel3->CNDTR = 0x14;   
   
   DMA2_Channel3->CCR |= DMA_CCR_PL_0;          //priority level - 11 - najwyzszy
   DMA2_Channel3->CCR &= ~DMA_CCR_PL_1;
 
   DMA2_Channel3->CCR &= ~DMA_CCR_MSIZE_0;       // memory size 8 bitow
   DMA2_Channel3->CCR &= ~DMA_CCR_MSIZE_1;
   DMA2_Channel3->CCR &= ~DMA_CCR_PSIZE_0;       // periph size 8 bitów
   DMA2_Channel3->CCR &= ~DMA_CCR_PSIZE_1; 
   
   DMA2_Channel3->CCR |= DMA_CCR_MINC;         // Memory inc enabled
   DMA2_Channel3->CCR &= ~DMA_CCR_PINC;         // Periph inc disabled;
   
   DMA2_Channel3->CCR |= DMA_CCR_CIRC;          //circular mode  - dzialanie ciagle
   DMA2_Channel3->CCR &= ~DMA_CCR_DIR;          // read from periph to memory;
 
   DMA2_Channel3->CCR |= DMA_CCR_EN;            //wlaczamy DMA po calej konfiguracji
   
   
}

void UART4_Init_with_DMA_TIM7(void){
    //UART4  - Rx->PC11; Tx->PC10 */
   /******* Konfiguracja GPIO *******/  
   RCC->AHBENR |= RCC_AHBENR_GPIOCEN;                                         // enabled RCC for GPIOC
   
   // Set AF mode for PC10 and PC11
   GPIOC->MODER |= GPIO_MODER_MODER10_1 | GPIO_MODER_MODER11_1; 
   GPIOC->MODER &= ~GPIO_MODER_MODER10_0 | ~GPIO_MODER_MODER11_0; 
   
   GPIOC->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR10_1 | GPIO_OSPEEDER_OSPEEDR10_0;
   GPIOC->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR11_1 | GPIO_OSPEEDER_OSPEEDR11_0;
   
   GPIOC->AFR[1] |= (5 << 8) | (5 << 12);                                     //UART TX/RX AF5 -> 0b0101 = 5   
   
   /****** Konfiguracja UART4*******/
   RCC->APB1ENR |= RCC_APB1ENR_UART4EN;
   UART4->CR1 &= ~USART_CR1_M0 | ~(1U << 28);
   
   // PLCK1= HCLK/2 = 72MHz/2 = 36Mhz
   UART4->CR1 &= ~USART_CR1_OVER8;                                            // set oversampling 16;
   UART4->BRR = 0xEA6;                                                        // Baudrate = 115200; dla Oversamplingu 16 BRR = USARTDIV = PLCK1/Baudrate = 36000000/115200 = 312 = 0x138
   UART4->CR1 &= ~USART_CR1_PCE;                                              // disabled parity control
   UART4->CR2 &= ~USART_CR2_STOP_0 | ~USART_CR2_STOP_1;                       // set stop = 1bit
   
   //DMA 
   UART4->CR3 |= USART_CR3_DMAT;                                              // enabled DMA transmit
   UART4->CR3 |= USART_CR3_DMAR;
   UART4_DMA_TIM7_config();

   UART4->CR1 |= USART_CR1_TE;                                                //transmiter enabled
   UART4->CR1 |= USART_CR1_RE;                                                //receiver enabled
   UART4->CR1 |= USART_CR1_UE;                                                // enabled UART4
   
   TIM7_config();
}

void UART4_DMA_TIM7_config(void){
     
   
   RCC->AHBENR |= RCC_AHBENR_DMA2EN;
      
   // DMA UART4_Tx config - DMA2 channel 5
   DMA2_Channel5->CPAR = (uint32_t)&UART4->TDR;
   DMA2_Channel5->CMAR = (uint32_t)StringToSendUART;   

   DMA2_Channel5 ->CCR &= ~DMA_CCR_MEM2MEM;       // disable Memory 2 memory transfer
   
   DMA2_Channel5->CNDTR = 0x1B;   
   
   DMA2_Channel5->CCR |= DMA_CCR_PL_0;          //priority level - 11 - najwyzszy
   DMA2_Channel5->CCR &= ~DMA_CCR_PL_1;
   
   DMA2_Channel5->CCR &= ~DMA_CCR_MSIZE_0;       // memory size 8 bitow
   DMA2_Channel5->CCR &= ~DMA_CCR_MSIZE_1;
   DMA2_Channel5->CCR &= ~DMA_CCR_PSIZE_0;       // periph size 8bitów
   DMA2_Channel5->CCR &= ~DMA_CCR_PSIZE_1; 
   
   DMA2_Channel5->CCR |= DMA_CCR_MINC;         // Memory inc enabled
   DMA2_Channel5->CCR &= ~DMA_CCR_PINC;         // Periph inc disabled;
   
   DMA2_Channel5->CCR &= ~DMA_CCR_CIRC;          //circular mode disabled
   DMA2_Channel5->CCR |= DMA_CCR_DIR;          // read from memory to periph
 
   DMA2_Channel5->CCR |= DMA_CCR_EN;            //wlaczamy DMA po calej konfiguracji
   
   /************************************************************/
   
   // DMA UART4_RX config - DMA2 channel 3
   DMA2_Channel3->CPAR = (uint32_t)&UART4->RDR;
   DMA2_Channel3->CMAR = (uint32_t)StringToReciveUART;   

   DMA2_Channel3 ->CCR &= ~DMA_CCR_MEM2MEM;       // disable Memory 2 memory transfer
   
   DMA2_Channel3->CNDTR = 0x14;   
   
   DMA2_Channel3->CCR |= DMA_CCR_PL_0;          //priority level - 11 - najwyzszy
   DMA2_Channel3->CCR &= ~DMA_CCR_PL_1;
 
   DMA2_Channel3->CCR &= ~DMA_CCR_MSIZE_0;       // memory size 8 bitow
   DMA2_Channel3->CCR &= ~DMA_CCR_MSIZE_1;
   DMA2_Channel3->CCR &= ~DMA_CCR_PSIZE_0;       // periph size 8 bitów
   DMA2_Channel3->CCR &= ~DMA_CCR_PSIZE_1; 
   
   DMA2_Channel3->CCR |= DMA_CCR_MINC;         // Memory inc enabled
   DMA2_Channel3->CCR &= ~DMA_CCR_PINC;         // Periph inc disabled;
   
   DMA2_Channel3->CCR &= ~DMA_CCR_CIRC;          //circular mode disabled
   DMA2_Channel3->CCR &= ~DMA_CCR_DIR;          // read from periph to memory;
 
   DMA2_Channel3->CCR |= DMA_CCR_EN;            //wlaczamy DMA po calej konfiguracji
   
   
}
