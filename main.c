#include<stdio.h>
#include "stm32f3xx.h"                  // Device header
#include "main.h"

static volatile unsigned long i;				//slowo kluczowe volatile, bez niego kompilator optymalizuje kod, efekt brak opoznienia
void delay(void){
	for ( i=0; i<555555; i++);
}

int main()
{
	//wlaczenie portu E + konfiguracja pinow jako wyjscie
	RCC->AHBENR |= (uint32_t)0x200000;								//Ustawienie 21 bitu na 1 - Podpiecie zegara pod port E
	GPIOE->MODER |= (uint32_t)0x41400000;							//bit 30, 24 i 22 - kolejno PE15, PE11, PE12; rejest MODER - input/output/alternate func/analogmode

	
	//wlaczenie portu A + konfiguracja pinu PA0 jako wejscie
	RCC->AHBENR |= (uint32_t)0x20000;								// podlaczenie sygnalu zegara pod port A
	GPIOA->MODER &= (uint32_t)~0x0003;		// ustawienie PA0 jako wyjscie podciagniete do VCC standardowo.
	
	//GPIOA->PUPDR |= (1<<1);								// pull-up - niepotrzebne, moze byc plywajacoe floating
	
	while(1){		
		
		GPIOE->ODR |= (uint16_t)0x8800;				//wyjscie PE11 i PE15 ustaw na logiczna 1		
	  delay();	
		if (((GPIOA->IDR) & 0x1 ) != (uint16_t)0x0001){	// czy stan niski na PA0?
			GPIOE->ODR &= (uint32_t)~0x1000;			//wyjscie PE12 ustaw logiczne 0
		}
		if (((GPIOA->IDR) & 0x1 ) == (uint16_t)0x0001){ // czy stan wysoki na PA0?
			GPIOE->ODR |= (uint16_t)0x1000; 			//wyjscie PE12 ustaw logiczne 1
		}
		delay();		
		GPIOE->ODR &= (uint16_t)~0x8800;    //wyjscie PE11 i PE15 ustaw na logiczne 0
		delay();
		GPIOE->ODR |= (uint16_t)0x800;			//wyjscie PE11 ustaw logiczne 1
		delay();
		GPIOE->ODR &= (uint16_t)~0x800;   	//wyjscie PE11 ustaw logiczne 0
	  delay();
	}
	
	
}
