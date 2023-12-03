#makefile
CC := g++
C_FLAGS := -std=c++17 -Wall
SRC_FILES:= main.cpp poly.cpp poly.h

Target := test

all:
	$(CC) $(SRC_FILES) $(C_FLAGS) -g -o $(Target)

test:
	

Valgrind:
	valgrind --leak-check=full --show-leak-kinds=all ./$(Target)