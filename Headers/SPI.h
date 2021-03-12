#ifndef _SPI
#define _SPI
#include "../Headers/UserSystemInit.h"

void SPI_Gyroskop_Config(void);
void Write_SPI(uint8_t reg, uint8_t data);
uint8_t Read_SPI(uint8_t reg);

#endif
