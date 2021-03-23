#ifndef _I2C
#define _I2C

#include "stm32f3xx.h"
#include "../Headers/UserSystemInit.h"

void I2C_LSM303DLHC_Init(void);
uint8_t I2C_Read(uint8_t SlaveAdress, uint8_t RegisterToRead, uint8_t BytesToRead);
void I2C_Write(uint8_t SlaveAdress, uint8_t RegisterToWrite, uint8_t Data, uint8_t BytesToWrite);
void I2C_LSM303DLHC_Config_Init (void);
void I2C_LSM303DLHC_A_Read_XYZ(void);

extern volatile uint8_t Read_value_LSM303DLHC_A;
extern volatile int8_t I2C_LSM303DLHC_A_XL;
extern volatile int8_t I2C_LSM303DLHC_A_XH;
extern volatile int8_t I2C_LSM303DLHC_A_YL;
extern volatile int8_t I2C_LSM303DLHC_A_YH;
extern volatile int8_t I2C_LSM303DLHC_A_ZL;
extern volatile int8_t I2C_LSM303DLHC_A_ZH;
extern volatile int16_t I2C_LSM303DLHC_A_X_value;
extern volatile int16_t I2C_LSM303DLHC_A_Y_value;
extern volatile int16_t I2C_LSM303DLHC_A_Z_value;

#endif
