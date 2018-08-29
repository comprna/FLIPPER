EXEC=flipper    	# name of executable is run
CC=g++      	# compile with g++
CFLAGS=-Wall -Wextra -std=c++11 -O3 -pthread

all: $(EXEC)

$(EXEC): main.cpp adapters.o fasta.o nw.o polya.o utils.o fl.o
	$(CC) -o $(EXEC) $(CFLAGS) main.cpp adapters.o fasta.o nw.o polya.o utils.o fl.o

utils.o: utils.cpp
	$(CC) -c $(CFLAGS) utils.cpp

fasta.o: fasta.cpp
	$(CC) -c $(CFLAGS) fasta.cpp

polya.o: polya.cpp
	$(CC) -c $(CFLAGS) polya.cpp

nw.o: nw.cpp 
	$(CC) -c $(CFLAGS) nw.cpp

adapters.o: nw.cpp adapters.cpp
	$(CC) -c $(CFLAGS) adapters.cpp nw.cpp

fl.o: fl.cpp fasta.cpp adapters.cpp utils.cpp polya.cpp
	$(CC) -c $(CFLAGS) fl.cpp fasta.cpp adapters.cpp utils.cpp polya.cpp

clean: 
	rm -f *.o
	rm -f $(EXEC)
	rm -f *log.txt