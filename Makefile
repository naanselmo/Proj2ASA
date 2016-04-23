all: mooshak

mooshak: mooshak.cpp
	g++ -O3 -ansi -Wall mooshak.cpp

botnet: graph.o place.o branch.o main.cpp
	g++ -O3 -ansi -Wall main.cpp graph.o place.o branch.o -lm

graph.o: graph.cpp graph.hpp branch.o place.o
	g++ -O3 -ansi -Wall -g -c graph.cpp -lm

place.o: place.cpp place.hpp
	g++ -O3 -ansi -Wall -g -c place.cpp -lm

branch.o: branch.cpp branch.hpp
	g++ -O3 -ansi -Wall -g -c branch.cpp -lm

clean:
	rm *.o *.out
