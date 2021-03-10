#include "../Headers/SPI.h"


void SPI_Gyroskop_Config(void){
   
   //konfiguracja pinow GPIO SCK, MOSI, MISO, CS
   RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
   GPIOA->MODER |= GPIO_MODER_MODER5_1;   // PA5 SCK - set alternate func
   GPIOA->MODER |= GPIO_MODER_MODER6_1;   // PA6 MOSI - set alternate func
   GPIOA->MODER |= GPIO_MODER_MODER7_1;   // PA7 MISO - set alternate func
   
   GPIOA->AFR[0] = 0x55500000;            // AF5 dla PA5,6,7
   
   RCC->AHBENR |= RCC_AHBENR_GPIOEEN;
   GPIOE->MODER |= GPIO_MODER_MODER3_0;   // PE3 CS - set as aoutput
   GPIOE->OTYPER &= ~GPIO_OTYPER_OT_3;    // PE3 output push pull
   GPIOE->ODR &= ~(uint32_t)Pin_3;        // 0 na PE3
   
   //konfiguracja SPI
   RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
   SPI1->CR1 |= SPI_CR1_BR_1;             //konfiguracja zegara SCK - dla ukladu L3GD20 max 10MHz - taktowanie SYSCLK 72MHz -  a wiec dzielnik 8 czyli 010 = 9MHz
   SPI1->CR1 |= SPI_CR1_CPHA;             // jak wynika z doc L3GD20 musi byc opoznienie danych o jeden cykl zegarowy
   SPI1->CR1 |= SPI_CR1_CPOL;             //polaryzacja zegara SCK  - dokumentacja L3GD20 mowi ze musi byc stan wysoki w IDLE
   SPI1->CR1 &= ~SPI_CR1_RXONLY;          // full duplex
   SPI1->CR1 &= ~SPI_CR1_LSBFIRST;        // set MSB frame format (most significant bit)
   SPI1->CR1 |= SPI_CR1_MSTR;             // master configuration
   
   SPI1->CR2 |= SPI_CR2_DS_1 | SPI_CR2_DS_2; // data length transfer 7 bit 0110;
   SPI1->CR2 |= SPI_CR2_SSOE;             //1: SS output is enabled in master mode and when the SPI interface is enabled. 
   SPI1->CR2 &= ~SPI_CR2_FRF;             // Frame format  - 0: SPI Motorola mode   ????? !!brak info w dokumentacji!!
             
}
