# eenvoudige makefile
all: hoofd.o game.o gobord.o basicfunctions.o globaldata.o
	g++ -Wall -o taran4 globaldata.o basicfunctions.o gobord.o game.o hoofd.o
globaldata.o: globaldata.cpp globaldata.h
	g++ -Wall -c globaldata.cpp
basicfunctions.o: basicfunctions.cpp basicfunctions.h
	g++ -Wall -c basicfunctions.cpp
gobord.o: gobord.cpp gobord.h
	g++ -Wall -c gobord.cpp
game.o: game.cpp game.h
	g++ -Wall -c game.cpp
hoofd.o: hoofd.cpp gobord.h
	g++ -Wall -c hoofd.cpp
clean:
	rm -f *.o taran4
