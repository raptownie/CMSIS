#ifndef _GPIO_LEDS_Buttons
#define _GPIO_LEDS_Buttons

#include "../Headers/UserSystemInit.h"
#include "../Headers/Timers.h"

void GPIO_zPrzyciskiem(void);
void GPIO_Kolko(void);
void LEDy_kolo (void)__attribute__((noreturn));
void Debouncing_SW_LPF (void);

#endif
