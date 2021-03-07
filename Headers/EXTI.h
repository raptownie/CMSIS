#ifndef _EXTI
#define _EXTI

#include "../Headers/UserSystemInit.h"
#include "../Headers/Timers.h"

void EXTI0_config(void);
void EXTI0_IRQHandler(void);
void EXTI0_config_PWMButton(void);

#endif
