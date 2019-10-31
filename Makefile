# Set compiler to use
CC=g++
CFLAGS=-g -I. -fpermissive
LDFLAGS=
DEPS=Matrix_e.h
OBJS=Main.o Matrix_e.o ttymodes.o
DEBUG=0

all:: Main.exe

Main.exe: $(OBJS)
	$(CC) -o $@ $^ -std=c++11 $(CFLAGS) $(LDFLAGS)

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

%.o: %.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

clean: 
	rm -f *.exe *.o *~ *.stackdump
