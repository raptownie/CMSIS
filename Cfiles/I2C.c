#include "../Headers/I2C.h"

//adres for accelerometer 0x19
//adress for magneto 0x1E


void I2C_LSM303DLHC_Init(void){
   
   //configuracja GPIOB
   //pb6 - SCL; pb7 - SDA
   RCC->AHBENR |= RCC_AHBENR_GPIOBEN;
   
   // ustawienie pb6 i pb7 w trybie alternate function
   GPIOB->MODER &= ~GPIO_MODER_MODER6_0;
   GPIOB->MODER |= GPIO_MODER_MODER6_1;
   GPIOB->MODER &= ~GPIO_MODER_MODER7_0;
   GPIOB->MODER |= GPIO_MODER_MODER7_1;   
   
   //ustawienie AF4 dla PB6 i PB7 - I2C
   GPIOB->AFR[0] |= 0x44000000;
   
   // OTYPER musza byc ustawione jako open drain
   GPIOB->OTYPER |= GPIO_OTYPER_OT_6;
   GPIOB->OTYPER |= GPIO_OTYPER_OT_7;
   
   //ustawienie predkosci na najwolniejsza
   GPIOB->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR6_0;
   GPIOB->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR7_0;
   
   //No pullup/pulldown
   GPIOB->PUPDR &= ~GPIO_PUPDR_PUPDR6_0;
   GPIOB->PUPDR &= ~GPIO_PUPDR_PUPDR6_1;
   GPIOB->PUPDR &= ~GPIO_PUPDR_PUPDR7_0;
   GPIOB->PUPDR &= ~GPIO_PUPDR_PUPDR7_1;
      
   //configuracja I2C
   RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;   
   I2C1->TIMINGR = (uint32_t)0x00200208;     //z wykorzystaniem kalkulatora od ST   
   I2C1->CR1 |= I2C_CR1_PE;
}


void I2C_Write(uint8_t SlaveAdress, uint8_t RegisterToWrite, uint8_t Data, uint8_t BytesToWrite){
   
   I2C1->CR2 |= (uint8_t)(SlaveAdress<<1); // adres urzadzenia z ktorym chcemy pogadac, przesuwamy o 1 zeby zrobic miejsce dla bitu R/W
   I2C1->CR2 &= ~I2C_CR2_RD_WRN;             // ustawiamy WRITE
   I2C1->CR2 |= (uint32_t)((BytesToWrite+1) << 16)  ;//ile bajtow chcemy wgrac (adres + 
   
   I2C1->CR2 |= I2C_CR2_START;
   while (I2C1->CR2 & I2C_CR2_START); 
   
   while (!(I2C1->ISR & I2C_ISR_TXE));
   I2C1->TXDR = RegisterToWrite;
   
   while (!(I2C1->ISR & I2C_ISR_TXE));
   I2C1->TXDR = Data;
   
   while (!(I2C1->ISR & I2C_ISR_TXE));
   
   
   I2C1->CR2 |= I2C_CR2_STOP;
   while (I2C1->CR2 & I2C_CR2_STOP);
   I2C1->CR2 &= ~(uint32_t)((BytesToWrite+1) << 16);         // super wazne bo inaczej powoduje problem przy odczycie; przed tym rejestr CR2
   I2C1->CR2 &= ~(uint8_t)(SlaveAdress<<1);
}

uint8_t I2C_Read(uint8_t SlaveAdress, uint8_t RegisterToRead, uint8_t BytesToRead){
   
   /*konfiguracja ramki READ
   START -> SlaveAdress + Write -> Slave Acknowlage -> Slave Register to read -> Slave Acknowlage->
   
   */
   I2C1->CR2 |= (uint8_t)(SlaveAdress<<1); // adres urzadzenia z ktorym chcemy pogadac, przesuwamy o 1 zeby zrobic miejsce dla bitu R/W
   I2C1->CR2 &= ~I2C_CR2_RD_WRN;             //Przy odczycie najpierw musimy odczytac
   I2C1->CR2 |= (1 << 16);//ile bajtow chcemy wgrac - ile adresow
   
   I2C1->CR2 |= I2C_CR2_START;
   while (I2C1->CR2 & I2C_CR2_START);  // Start bit jest resetowane przez HW, czekamy zatem az bedzie zresetowane, przed kolejnym krokiem
   while (!(I2C1->ISR & I2C_ISR_TXE));
   I2C1->TXDR = RegisterToRead;
   while (!(I2C1->ISR & I2C_ISR_TXE)); // czekamy az buffor Transmit bedzie pusty, az dane beda wyslane
   
   //restart SR
   I2C1->CR2 |= (uint8_t)(SlaveAdress<<1);
   I2C1->CR2 |= I2C_CR2_RD_WRN;
   I2C1->CR2 &= ~(uint32_t)(1 << 16);                      // negacja poprzedniego wpisu
   I2C1->CR2 |= (uint32_t)(BytesToRead << 16);   // ile bajtow chcemy odczytac
   I2C1->CR2 |= I2C_CR2_START;
   while (I2C1->CR2 & I2C_CR2_START);
   while (!(I2C1->ISR & I2C_ISR_TXE));
   
   while (I2C1->ISR & I2C_ISR_RXNE);     
  
   while (!(I2C1->ISR & I2C_ISR_TC));
   uint8_t Read =(uint8_t)I2C1->RXDR;   
   while (!(I2C1->ISR & I2C_ISR_TC));
   I2C1->CR2 |= I2C_CR2_STOP;
   while (I2C1->CR2 & I2C_CR2_STOP);
   
   I2C1->CR2 &= ~(uint8_t)(SlaveAdress<<1);
   I2C1->CR2 &= ~(uint32_t)(BytesToRead << 16);
   return (uint8_t)Read;
}

void I2C_LSM303DLHC_Config_Init (void){
  Read_value_LSM303DLHC_A = (uint8_t)I2C_Read(Accelerometer_Adress,0x20,1);
  I2C_Write(Accelerometer_Adress,0x20, 0x97,1);
  Read_value_LSM303DLHC_A = (uint8_t)I2C_Read(Accelerometer_Adress,0x20,1);
   
}

void I2C_LSM303DLHC_A_Read_XYZ(void){
   I2C_LSM303DLHC_A_XL = (int8_t)I2C_Read(Accelerometer_Adress,0x28,1);
   I2C_LSM303DLHC_A_XH = (int8_t)I2C_Read(Accelerometer_Adress,0x29,1);  
   I2C_LSM303DLHC_A_X_value = (int16_t)(I2C_LSM303DLHC_A_XL | (I2C_LSM303DLHC_A_XH << 8));    
      
   I2C_LSM303DLHC_A_YL = (int8_t)I2C_Read(Accelerometer_Adress,0x2A,1);    
   I2C_LSM303DLHC_A_YH = (int8_t)I2C_Read(Accelerometer_Adress,0x2B,1);   
   I2C_LSM303DLHC_A_Y_value = (int16_t)(I2C_LSM303DLHC_A_YL | (I2C_LSM303DLHC_A_YH << 8));     
      
   I2C_LSM303DLHC_A_ZL = (int8_t)I2C_Read(Accelerometer_Adress,0x2C,1);
   I2C_LSM303DLHC_A_ZH = (int8_t)I2C_Read(Accelerometer_Adress,0x2D,1);
   I2C_LSM303DLHC_A_Z_value = (int16_t)(I2C_LSM303DLHC_A_ZL | (I2C_LSM303DLHC_A_ZH << 8));   
}

