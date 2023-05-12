CC = g++
FLAGS = -Wall -lpthread
FILES = hw2.cpp hw2_output.c 

all: hw2
	
hw2:
	$(CC) -o $@ $(FILES) $(FLAGS)

clean:
	rm -f hw2
