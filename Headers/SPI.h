#ifndef _SPI
#define _SPI
//#include "../Headers/UserSystemInit.h"
#include "stm32f3xx.h" 

#define HSI_Clock 8000000        // bez PLL
#define Pin_3 0x8
#define Pin_5 0x20
#define Pin_8 0x100
#define Pin_9 0x200
#define Pin_10 0x400
#define Pin_11 0x800
#define Pin_12 0x1000
#define Pin_13 0x2000
#define Pin_14 0x4000
#define Pin_15 0x8000

static volatile uint16_t SPI_L3GD2_WAI;
static volatile uint16_t SPI_L3GD2_CTRL1;
extern uint32_t SPI_L3GD2_Read;
extern int8_t tab_SPI_L3GD20_XYZ_values[6];
const extern uint8_t tab_SPI_L3GD20_XYZ_adress[6];
extern volatile int8_t SPI_L3GD2_XL;
extern volatile int8_t SPI_L3GD2_XH;
extern volatile int8_t SPI_L3GD2_YL;
extern volatile int8_t SPI_L3GD2_YH;
extern volatile int8_t SPI_L3GD2_ZL;
extern volatile int8_t SPI_L3GD2_ZH;
extern volatile int16_t X_value;
extern volatile int16_t Y_value;
extern volatile int16_t Z_value;

void L3GD20_XYZ_Calculate(void);
void SPI_Config_for_Gyroskop(void);
void Write_SPI(uint8_t reg, uint8_t data);
uint8_t Read_SPI(uint8_t reg);
void L3GD20_Init(void);
void L3GD20_DMA_Init(void);
void SPI_Config_for_Gyroskop_DMA(void);
void DMA1_Channel2_IRQHandler(void);

#endif
