#include <stdio.h>
#include "dodawanie.h"
#include "Dzielenie.h"
#include "Mnozenie.h"
#include "Odejmowanie.h"


int main()
{
    int a=5;
    int b=30;


    printf("Wynik dodawania a+b = %d\n", dodaj(a,b));
    printf("Wynik odejmowanie a-b = %d\n", odejmij(a,b));
    printf("Wynik mnozenie a*b = %d\n", pomnoz(a,b));
	
    printf("Wynik dzielenie a/b = %f\n", podziel(b,b));
	while(1);



    return 1;
}
