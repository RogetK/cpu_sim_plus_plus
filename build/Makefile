CC = g++
CFLAGS = -Wall -pedantic 
INCLUDES = -I ..

.PHONY: all clean

all: main
	$(CC) $(CFLAGS) $(INCLUDES) -o cpu_sim *.o
	
main: ../main.cpp
	$(CC) $(CFLAGS) $(INCLUDES) -c ../main.cpp 

clean: 
	rm -f *.o
