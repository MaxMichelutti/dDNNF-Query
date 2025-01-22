main: src/main.cpp src/ddnnf.o
	g++ -std=c++11 -o main src/main.cpp src/ddnnf.o

src/ddnnf.o: src/ddnnf.cpp src/ddnnf.h
	g++ -std=c++11 -c src/ddnnf.cpp -o src/ddnnf.o
