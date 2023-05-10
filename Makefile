CC = g++
FLAGS = -Wall -lpthread -std=c++11
FILES = hw2.cpp hw2_output.c 

all: hw2
	
hw2:
	$(CC) $(FLAGS) -o $@ $(FILES)

clean:
	rm -f hw2
