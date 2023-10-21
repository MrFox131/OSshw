all:
	gcc -o build/main main.c my_func.c
run: all
	./build/main