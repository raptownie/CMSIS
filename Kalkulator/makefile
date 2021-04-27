#lista obiektów w projekcie
OBJECTS = Main.o Dodawanie.o Odejmowanie.o Dzielenie.o Mnozenie.o 

#lokalizacja pliku exe
EXEDIR = bin\Debug

#lokalizacja do plikow .obiektów
ODIR = obj\Debug

#lokalizacja plikow *.o
OBJECTSDIR = $(ODIR)\Main.o $(ODIR)\Dodawanie.o $(ODIR)\Odejmowanie.o \
			$(ODIR)\Dzielenie.o $(ODIR)\Mnozenie.o

#flagi kompilacji
CFLAGS = -Wall -g -c

$(EXEDIR)\kalkulator.exe: $(OBJECTSDIR)
	gcc.exe -o $(EXEDIR)\kalkulator-makefile.exe $(OBJECTSDIR)
	echo "Kompilacja zakonczona"
	
$(ODIR)\Main.o: Main.c Dodawanie.h Odejmowanie.h Dzielenie.h Mnozenie.h
	gcc.exe $(CFLAGS) main.c -o $(ODIR)\Main.o
	
$(ODIR)\Dodawanie.o: Dodawanie.c Dodawanie.h
	gcc.exe $(CFLAGS) Dodawanie.c -o $(ODIR)\Dodawanie.o

$(ODIR)\Odejmowanie.o: Odejmowanie.c Odejmowanie.h
	gcc.exe $(CFLAGS) Odejmowanie.c -o $(ODIR)\Odejmowanie.o
	
$(ODIR)\Dzielenie.o: Dzielenie.c Dzielenie.h
	gcc.exe $(CFLAGS) Dzielenie.c -o $(ODIR)\Dzielenie.o

$(ODIR)\Mnozenie.o: Mnozenie.c Mnozenie.h
	gcc.exe $(CFLAGS) Mnozenie.c -o $(ODIR)\Mnozenie.o
	
.PHONY: clean
clean:
	-del $(ODIR)