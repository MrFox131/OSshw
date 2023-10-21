ifeq ($(OS),Windows_NT) 
    binary := ./build/main.exe
else
    binary := ./build/main
endif

build/my_func.o: my_func.c
	g++ -c -Wall -g ./my_func.c -o build/my_func.o

build/main.o: main.c
	g++ -c -Wall -g ./main.c -o build/main.o


all: build/main.o build/my_func.o
	g++ -o $(binary) build/main.o build/my_func.o

run: all
	$(binary)