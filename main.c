// cwiczenie 1 GPIO
#include<stdio.h>
#include "stm32f3xx.h"                  // Device header
#include "main.h"
#define Pin_8 0x100
#define Pin_9 0x200
#define Pin_10 0x400
#define Pin_11 0x800
#define Pin_12 0x1000
#define Pin_13 0x2000
#define Pin_14 0x4000
#define Pin_15 0x8000

//
static volatile unsigned long i;				//slowo kluczowe volatile, bez niego kompilator optymalizuje kod, efekt brak opoznienia
void delay(void){
	for ( i=0; i<133332; i++);
}

void GPIO_zPrzyciskiem(void){	
		GPIOE->ODR |= (uint16_t)(Pin_11+Pin_15);				//wyjscie PE11 i PE15 ustaw na logiczna 1		
	  delay();	
		if (((GPIOA->IDR) & 0x1 ) != (uint16_t)0x0001){	// czy stan niski na PA0?
			GPIOE->ODR &= (uint32_t)~0x1000;			//wyjscie PE12 ustaw logiczne 0
		}
		if (((GPIOA->IDR) & 0x1 ) == (uint16_t)0x0001){ // czy stan wysoki na PA0?
			GPIOE->ODR |= (uint16_t)0x1000; 			//wyjscie PE12 ustaw logiczne 1
		}
		delay();		
		GPIOE->ODR &= (uint16_t)~(Pin_11+Pin_15);    //wyjscie PE11 i PE15 ustaw na logiczne 0
		delay();
		GPIOE->ODR |= (uint16_t)Pin_11;			//wyjscie PE11 ustaw logiczne 1
		delay();
		GPIOE->ODR &= (uint16_t)~Pin_11;   	//wyjscie PE11 ustaw logiczne 0
	  delay();
}

void GPIO_Kolko(void){
	

		GPIOE->ODR |= (uint32_t)(Pin_15+Pin_14+Pin_13);
		delay();
		GPIOE->ODR &= (uint32_t)~Pin_15;
		delay();
		GPIOE->ODR |= (uint32_t)(Pin_14+Pin_13+Pin_12);
		delay();
		GPIOE->ODR &= (uint32_t)~Pin_14;
		delay();
		GPIOE->ODR |= (uint32_t)(Pin_13+Pin_12+Pin_11);	
		delay();
		GPIOE->ODR &= (uint32_t)~Pin_13;
		delay();
		GPIOE->ODR |= (uint32_t)(Pin_12+Pin_11+Pin_10);	
		delay();
		GPIOE->ODR &= (uint32_t)~Pin_12;
		delay();
		GPIOE->ODR |= (uint32_t)(Pin_11+Pin_10+Pin_9);
		delay();
		GPIOE->ODR &= (uint32_t)~Pin_11;
		delay();
		GPIOE->ODR |= (uint32_t)(Pin_10+Pin_9+Pin_8);
		delay();
		GPIOE->ODR &= (uint32_t)~Pin_10;
		delay();
		GPIOE->ODR |= (uint32_t)(Pin_9+Pin_8+Pin_15);
		delay();
		GPIOE->ODR &= (uint32_t)~Pin_9;
		delay();
		GPIOE->ODR |= (uint32_t)(Pin_8+Pin_15+Pin_14);
		delay();
		GPIOE->ODR &= (uint32_t)~Pin_8;		
		delay();
}

void GPIO_Init(void){
	//wlaczenie portu E + konfiguracja pinow jako wyjscie
	RCC->AHBENR |= (uint32_t)0x200000;								//Ustawienie 21 bitu na 1 - Podpiecie zegara pod port E
	GPIOE->MODER |= (uint32_t)0x55550000;							//bit 30,28,26,24,22,20,18 i 16 - PE8-15 jako wyjscie; rejest MODER - input/output/alternate func/analogmode
	GPIOE->OTYPER = 0x0;															// wyjscie ustawione jako open dran
	//GPIOE->PUPDR |= 0xAAAA0000;													// rezystory pulldown
		//wlaczenie portu A + konfiguracja pinu PA0 jako wejscie
	RCC->AHBENR |= (uint32_t)0x20000;								// podlaczenie sygnalu zegara pod port A
	GPIOA->MODER &= (uint32_t)~0x0003;		// ustawienie PA0 jako wyjscie podciagniete do VCC standardowo.
}

int main()
{
	
	GPIO_Init();
	
	uint16_t temp= 0x100;
	GPIOE->ODR |= temp;
	
	while(1){	
		//diody na plytce discovery swieca sie w kolko jedna po drugiej
		temp=(uint16_t)temp << 1;	
		GPIOE->ODR = temp;		
		if (temp == 0x8000) {
			delay();
			temp = 0x100;
			GPIOE->ODR = temp;
		}
		delay();
		//
				
	//GPIO_zPrzyciskiem();
	//	GPIO_Kolko();
		
	
	}
	
	
}
