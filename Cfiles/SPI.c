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
   
   //config
  
   
   
   
   // first run
   /*
   SPI1->CR1 |= SPI_CR1_SPE;
   SPI1->DR = 0xCF; // dummy read
      while (!(SPI1->SR & SPI_SR_TXE));   // czekamy az zakonczy wysylanie
   while (SPI1->SR & SPI_SR_BSY); 
   while(!(SPI1->SR & SPI_SR_RXNE));
   SPI_L3GD2_Read = (uint32_t)SPI1->DR;
   
   SPI1->CR1 &= ~SPI_CR1_SPE;
   */
   
   // end first run
   /*
   SPI_L3GD2_Read = Read_SPI(0x0F);
   
   uint8_t SPI_L3GD2_CTRL_REG1= Read_SPI(0x20);
   uint8_t SPI_L3GD2_Status_reg = Read_SPI(0x27);
   while (SPI_L3GD2_Status_reg == 0){
      SPI_L3GD2_Status_reg = Read_SPI(0x27);
   }
   
   volatile uint8_t SPI_L3GD2_XL = Read_SPI(0x28);
   volatile uint8_t SPI_L3GD2_XH = Read_SPI(0x29);
   volatile uint8_t SPI_L3GD2_YL = Read_SPI(0x2A);
   volatile uint8_t SPI_L3GD2_YH = Read_SPI(0x2B);
   volatile uint8_t SPI_L3GD2_ZL = Read_SPI(0x2C);
   volatile uint8_t SPI_L3GD2_ZH = Read_SPI(0x2D);
*/
   /*
   SPI1->DR |= 0x8F;                      // WHO I AM - adres  0001111b (0x0F) + 0x80 (read bit)
   SPI_L3GD2_Rx = (uint32_t)SPI1->DR;
   SPI_L3GD2_Rx = (uint32_t)SPI1->DR;
   SPI_L3GD2_Rx = (uint32_t)SPI1->DR;
   SPI_L3GD2_Rx = (uint32_t)SPI1->DR;
   SPI_L3GD2_Rx = (uint32_t)SPI1->DR;
   SPI_L3GD2_Rx = (uint32_t)SPI1->DR;
   SPI_L3GD2_Rx = (uint32_t)SPI1->DR;
   SPI_L3GD2_Rx = (uint32_t)SPI1->DR;
   SPI_L3GD2_Rx = (uint32_t)SPI1->DR;
   SPI_L3GD2_Rx = (uint32_t)SPI1->DR;
   SPI_L3GD2_Rx = (uint32_t)SPI1->DR;
   SPI1->CR1 &= ~SPI_CR1_SPE; 
   */
   /*
   // wlaczenie X,Y,Z
   SPI1->DR |= 0x20;
   SPI_L3GD2_Rx = (uint32_t)SPI1->DR;
   SPI_L3GD2_Rx = (uint32_t)SPI1->DR;
   SPI_L3GD2_Rx = (uint32_t)SPI1->DR;
   SPI1->DR |= 0x7;
   SPI_L3GD2_Rx = (uint32_t)SPI1->DR;
   SPI_L3GD2_Rx = (uint32_t)SPI1->DR;
   SPI_L3GD2_Rx = (uint32_t)SPI1->DR;
   */
   

  //testy
  /*
   SPI_L3GD2_Rx = (uint8_t)SPI1->DR;
   SPI1->DR = 0x8F;
   SPI_L3GD2_Rx = (uint8_t)SPI1->DR;
   SPI_L3GD2_Rx = (uint8_t)SPI1->DR;
   SPI_L3GD2_Rx = (uint8_t)SPI1->DR;
   SPI_L3GD2_Rx = (uint8_t)SPI1->DR;
   SPI1->DR |= 0x8F;
   SPI_L3GD2_Rx = (uint8_t)SPI1->DR;
   SPI1->DR |= 0x8F;
   SPI_L3GD2_Rx = (uint8_t)SPI1->DR;
   SPI1->DR |= 0x8F;
   SPI_L3GD2_Rx = (uint8_t)SPI1->DR;
   SPI1->DR |= 0x8F;
   SPI_L3GD2_Rx = (uint8_t)SPI1->DR;
   */
             
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

