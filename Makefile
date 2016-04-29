

# Compiler
C = g++ -std=c++0x

# Flags
CFLAGS = -c

client: packet.o client.cpp
	$(C) packet.o client.cpp -o client

server: packet.o server.cpp
	$(C) packet.o server.cpp -o server

packet.o: packet.h packet.cpp
	$(C) $(CFLAGS) packet.cpp

all: client server packet.o

clean:
	rm -Rf *.o client
	rm -Rf *.o server
	rm -Rf *.o
