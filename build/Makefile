CC = g++
CFLAGS = -Wall -pedantic -ggdb
INCLUDES = -I ..

.PHONY: clean

all: main
	$(CC) $(CFLAGS) $(INCLUDES) -o cpu_sim *.o
	
main: ../main.cpp
	$(CC) $(CFLAGS) $(INCLUDES) -c ../main.cpp 

clean: 
	rm -f *.o
