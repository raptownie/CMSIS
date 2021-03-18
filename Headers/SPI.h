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

void SPI_Config_for_Gyroskop(void);
void Write_SPI(uint8_t reg, uint8_t data);
uint8_t Read_SPI(uint8_t reg);
void L3GD20_Init(void);

#endif
