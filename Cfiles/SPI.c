#include "../Headers/SPI.h"


void SPI_Config_for_Gyroskop(void){
   
   //konfiguracja pinow GPIO SCK, MOSI, MISO, CS
   RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
   GPIOA->MODER |= GPIO_MODER_MODER5_1;   // PA5 SCK - set alternate func
   GPIOA->MODER |= GPIO_MODER_MODER6_1;   // PA6 MOSI - set alternate func
   GPIOA->MODER |= GPIO_MODER_MODER7_1;   // PA7 MISO - set alternate func
   
   GPIOA->AFR[0] = 0x55500000;            // AF5 dla PA5,6,7
   
   RCC->AHBENR |= RCC_AHBENR_GPIOEEN;
   GPIOE->MODER |= GPIO_MODER_MODER3_0;   // PE3 CS - set as aoutput
   GPIOE->OTYPER &= ~GPIO_OTYPER_OT_3;    // PE3 output push pull
   GPIOE->ODR |= (uint32_t)Pin_3;
   
   //konfiguracja SPI
   RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
   SPI1->CR1 |= SPI_CR1_BR_2 | SPI_CR1_BR_1 | SPI_CR1_BR_0;             //konfiguracja zegara SCK - dla ukladu L3GD20 max 10MHz - taktowanie SYSCLK 72MHz -  a wiec dzielnik 8 czyli 010 = 9MHz
   SPI1->CR1 |= SPI_CR1_CPHA;             // jak wynika z doc L3GD20 musi byc opoznienie danych o jeden cykl zegarowy
   SPI1->CR1 |= SPI_CR1_CPOL;             //polaryzacja zegara SCK  - dokumentacja L3GD20 mowi ze musi byc stan wysoki w IDLE
   SPI1->CR1 &= ~SPI_CR1_RXONLY;          // full duplex
  // SPI1->CR1 |= SPI_CR1_BIDIMODE;       // half duplex
   SPI1->CR1 &= ~SPI_CR1_LSBFIRST;        // set MSB frame format (most significant bit)
   SPI1->CR1 |= SPI_CR1_MSTR;             // master configuration
  //SPI1->CR1 |= SPI_CR1_SSM;              // software slave managment
   
   SPI1->CR2 |= SPI_CR2_DS_0 | SPI_CR2_DS_1 | SPI_CR2_DS_2; // data length transfer 8 bit 0111;
   SPI1->CR2 |= SPI_CR2_SSOE;             //1: SS output is enabled in master mode and when the SPI interface is enabled. 
   SPI1->CR2 &= ~SPI_CR2_FRF;             // Frame format  - 0: SPI Motorola mode   ????? !!brak info w dokumentacji!!
   SPI1->CR2 |= SPI_CR2_FRXTH;            //FIFO reception threshold 8bit
   
   //SPI1->CR1 |= SPI_CR1_SPE;              //SPI enabled
   
  // GPIOE->ODR &= ~(uint32_t)Pin_3;        // Slave Selector SS 
     GPIOE->ODR |= (uint32_t)Pin_3;        // Slave Selector SS disable
}


//ta funkcja jeszcze nie dziala
void Write_SPI(uint8_t reg, uint8_t data){
   
   GPIOE->ODR &= ~(uint32_t)Pin_3;  
   SPI1->CR1 |= SPI_CR1_SPE;  
   while (!(SPI1->SR & SPI_SR_TXE));  
   reg= (reg | 0x40);
   SPI1->DR = (uint32_t)((data<<8) | reg);
   while (!(SPI1->SR & SPI_SR_TXE));
   while (SPI1->SR & SPI_SR_BSY);
   while(!(SPI1->SR & SPI_SR_RXNE));
   SPI1->CR1 &= ~SPI_CR1_SPE;
   GPIOE->ODR |= (uint32_t)Pin_3;        // Slave Selector SS 
}

uint8_t Read_SPI(uint8_t reg){
      GPIOE->ODR &= ~(uint32_t)Pin_3;        // Slave Selector SS 
   SPI1->CR1 |= SPI_CR1_SPE;           // wlaczamy SPI
   while (SPI1->SR & SPI_SR_BSY);      // czy SPI zajety?
   while (!(SPI1->SR & SPI_SR_TXE));      // sprawdzenie czy kolejka TX jest pusta
   SPI1->DR = (0xC0|reg);              // trzeba wlaczyc BIT 8 i 7  (8-read, 7-When 0 do not increment address; when 1 increment address in multiple reading.
   while (!(SPI1->SR & SPI_SR_TXE));   // czekamy az zakonczy wysylanie
   while (SPI1->SR & SPI_SR_BSY);      // sprawdzamy czy SPI jest zajety
  // SPI1->DR= 0x00;                     // dummy read
   while(!(SPI1->SR & SPI_SR_RXNE));   // czekamy na odpowiedz
   volatile uint8_t rd = (uint8_t) SPI1->DR;     // za pierwszym razem odczytuje FF, wiec dodalem odczyt przed wlasciwym odczytem zeby zdjac z kolejki FIFO  
   while(!(SPI1->SR & SPI_SR_RXNE));   // znowu sprawdzamy czy jest jakas dana do odczytu w buforze
   uint8_t r= (uint8_t)SPI1->DR;       // odczyt wlasciwy
   SPI1->CR1 &= ~SPI_CR1_SPE;          //wylaczamy spi
      GPIOE->ODR |= (uint32_t)Pin_3;        // Slave Selector SS 
   return r;
}

void L3GD20_Init(void){
   
   SPI_L3GD2_WAI = Read_SPI(0x0F);    
      
   // axis xyz on, power on
   Write_SPI(0x20, 0xF);      
   SPI_L3GD2_CTRL1 = Read_SPI(0x20);

   Write_SPI(0x23, 0xF0);    // output registers not updated until MSb and LSb readinge; set 500 DPS

}

void L3GD20_DMA_Init(void){
   
   SPI_Config_for_Gyroskop_DMA();
   
   RCC->AHBENR |= RCC_AHBENR_DMA1EN;
      
   // DMA RX config - DMA1 channel 2
   DMA1_Channel2->CPAR = (uint32_t)&SPI1->DR;
   DMA1_Channel2->CMAR = (uint32_t)tab_SPI_L3GD20_XYZ_values;   

   DMA1_Channel2 ->CCR &= ~DMA_CCR_MEM2MEM;       // disable Memory 2 memory transfer
   
   DMA1_Channel2->CNDTR = 0x6;   
   
   DMA1_Channel2->CCR |= DMA_CCR_PL_0;          //priority level - 11 - najwyzszy
   DMA1_Channel2->CCR &= ~DMA_CCR_PL_1;
   
   //W przypadku ramki 8 bitow danych po SPI trzeba ustawic memory size 16 bitow
   DMA1_Channel2->CCR &= ~DMA_CCR_MSIZE_0;       // memory size 16 bitow
   DMA1_Channel2->CCR &= ~DMA_CCR_MSIZE_1;
   DMA1_Channel2->CCR &= ~DMA_CCR_PSIZE_0;       // periph size 16bitów
   DMA1_Channel2->CCR &= ~DMA_CCR_PSIZE_1; 
   
   DMA1_Channel2->CCR |= DMA_CCR_MINC;         // Memory inc enabled
   DMA1_Channel2->CCR &= ~DMA_CCR_PINC;         // Periph inc disabled;
   
   DMA1_Channel2->CCR |= DMA_CCR_CIRC;          //circular mode  - dzialanie ciagle
   DMA1_Channel2->CCR &= ~DMA_CCR_DIR;          // read from periph to memory;
 
   DMA1_Channel2->CCR |= DMA_CCR_EN;            //wlaczamy DMA po calej konfiguracji
   
//******************************************************************************
   // DMA TX config - DMA1 channel 3
   DMA1_Channel3->CPAR = (uint32_t)&SPI1->DR;
   DMA1_Channel3->CMAR = (uint32_t)tab_SPI_L3GD20_XYZ_adress;  

   DMA1_Channel3 ->CCR &= ~DMA_CCR_MEM2MEM;       // disable Memory 2 memory transfer
   
   DMA1_Channel3->CNDTR = 0x6;   
   
   DMA1_Channel3->CCR |= DMA_CCR_PL_0;          //priority level - 11 - najwyzszy
   DMA1_Channel3->CCR &= ~DMA_CCR_PL_1;
   
   //W przypadku ramki 8 bitow danych po SPI trzeba ustawic memory size 16 bitow
   DMA1_Channel3->CCR &= ~DMA_CCR_MSIZE_0;       // memory size 8 bitow
   DMA1_Channel3->CCR &= ~DMA_CCR_MSIZE_1;
   DMA1_Channel3->CCR &= ~DMA_CCR_PSIZE_0;       // periph size 8bitów
   DMA1_Channel3->CCR &= ~DMA_CCR_PSIZE_1; 
   
   DMA1_Channel3->CCR |= DMA_CCR_MINC;         // Memory inc enabled
   DMA1_Channel3->CCR |= DMA_CCR_PINC;         // Periph inc disabled;
   
   DMA1_Channel3->CCR |= DMA_CCR_CIRC;          //circular mode  - dzialanie ciagle
   DMA1_Channel3->CCR |= DMA_CCR_DIR;          // read from memory to periph
   


   DMA1_Channel3->CCR |= DMA_CCR_EN;            //wlaczamy DMA po calej konfiguracji
   
   SPI1->CR1 |= SPI_CR1_SPE;
   GPIOE->ODR &= ~(uint32_t)Pin_3;        // Slave Selector SS 
   
}

void SPI_Config_for_Gyroskop_DMA(void){
      //konfiguracja pinow GPIO SCK, MOSI, MISO, CS
   RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
   GPIOA->MODER |= GPIO_MODER_MODER5_1;   // PA5 SCK - set alternate func
   GPIOA->MODER |= GPIO_MODER_MODER6_1;   // PA6 MOSI - set alternate func
   GPIOA->MODER |= GPIO_MODER_MODER7_1;   // PA7 MISO - set alternate func
   
   GPIOA->AFR[0] = 0x55500000;            // AF5 dla PA5,6,7
   
   RCC->AHBENR |= RCC_AHBENR_GPIOEEN;
   GPIOE->MODER |= GPIO_MODER_MODER3_0;   // PE3 CS - set as aoutput
   GPIOE->OTYPER &= ~GPIO_OTYPER_OT_3;    // PE3 output push pull
   GPIOE->ODR |= (uint32_t)Pin_3;
   
   //konfiguracja SPI
   RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
   SPI1->CR1 |= SPI_CR1_BR_2 | SPI_CR1_BR_1 | SPI_CR1_BR_0;             //konfiguracja zegara SCK - dla ukladu L3GD20 max 10MHz - taktowanie SYSCLK 72MHz -  a wiec dzielnik 8 czyli 010 = 9MHz
   SPI1->CR1 |= SPI_CR1_CPHA;             // jak wynika z doc L3GD20 musi byc opoznienie danych o jeden cykl zegarowy
   SPI1->CR1 |= SPI_CR1_CPOL;             //polaryzacja zegara SCK  - dokumentacja L3GD20 mowi ze musi byc stan wysoki w IDLE
   SPI1->CR1 &= ~SPI_CR1_RXONLY;          // full duplex  
   SPI1->CR1 &= ~SPI_CR1_LSBFIRST;        // set MSB frame format (most significant bit)
   SPI1->CR1 |= SPI_CR1_MSTR;             // master configuration
   
   SPI1->CR2 |= SPI_CR2_DS_0 | SPI_CR2_DS_1 | SPI_CR2_DS_2; // data length transfer 8 bit 0111;
   SPI1->CR2 |= SPI_CR2_SSOE;             //1: SS output is enabled in master mode and when the SPI interface is enabled. 
   SPI1->CR2 &= ~SPI_CR2_FRF;             // Frame format  - 0: SPI Motorola mode   ????? !!brak info w dokumentacji!!
   SPI1->CR2 |= SPI_CR2_FRXTH;            //FIFO reception threshold 8bit
       
   //konfiguracja L3GD20
   SPI_L3GD2_WAI = Read_SPI(0x0F);  //WHO AM I        
   Write_SPI(0x23, 0xF0);    // output registers not updated until MSb and LSb readinge; set 500 DPS
   Write_SPI(0x2E, 0x60);     //stream to FIFO mode - bez roznicy
   Write_SPI(0x24, 0x40);     //enabled fifo mode
   Write_SPI(0x20, 0xF);            // axis xyz on, power on 
   SPI_L3GD2_CTRL1 = Read_SPI(0x20);
   
   SPI1->CR2 |= SPI_CR2_RXDMAEN;
   SPI1->CR2 |= SPI_CR2_TXDMAEN;
   SPI1->CR1 &= ~SPI_CR1_SPE;              //SPI disabled
   
   
   GPIOE->ODR &= ~(uint32_t)Pin_3;        // Slave Selector SS 
  
    
}

void L3GD20_XYZ_Calculate(void){
   //*** kalkulacja XYZ Gyroskopu, z wykorzystaniem DMA***
   SPI_L3GD2_YL = (int8_t)tab_SPI_L3GD20_XYZ_values[0];   
   SPI_L3GD2_YH = (int8_t)tab_SPI_L3GD20_XYZ_values[1];   
   Y_value = (int16_t)(SPI_L3GD2_YL |(SPI_L3GD2_YH << 8));
      
   SPI_L3GD2_ZL = (int8_t)tab_SPI_L3GD20_XYZ_values[2];  
   SPI_L3GD2_ZH = (int8_t)tab_SPI_L3GD20_XYZ_values[3];   
   Z_value = (int16_t)(SPI_L3GD2_ZL |(SPI_L3GD2_ZH << 8));
  
   SPI_L3GD2_XL = (int8_t)tab_SPI_L3GD20_XYZ_values[4];     
   SPI_L3GD2_XH = (int8_t)tab_SPI_L3GD20_XYZ_values[5];      
   X_value = (int16_t)(SPI_L3GD2_XL | (SPI_L3GD2_XH << 8)); 
  //********************************************************   
}




