CC=gcc
CFLAGS=-O3 -march=native $(shell pkg-config --cflags gtk+-3.0)
OBJ=weather.o

all: weather

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

weather: weather.o
	$(CC) $(OBJ) -o weather $(shell pkg-config --libs gtk+-3.0) -export-dynamic

clean:
	rm weather $(OBJ)
