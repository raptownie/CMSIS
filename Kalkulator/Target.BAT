gcc.exe -Wall -g  -c Dodawanie.c -o obj\Debug\Dodawanie.o
gcc.exe -Wall -g  -c Dzielenie.c -o obj\Debug\Dzielenie.o
gcc.exe -Wall -g  -c main.c -o obj\Debug\main.o
gcc.exe -Wall -g  -c Mnozenie.c -o obj\Debug\Mnozenie.o
gcc.exe -Wall -g  -c Odejmowanie.c -o obj\Debug\Odejmowanie.o
gcc.exe  -o bin\Debug\Kalkulator.exe obj\Debug\Dodawanie.o obj\Debug\Dzielenie.o obj\Debug\main.o obj\Debug\Mnozenie.o obj\Debug\Odejmowanie.o  
