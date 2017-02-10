CC=gcc
CFLAGS=-O3 -march=native $(shell pkg-config --cflags gtk+-3.0 json-glib-1.0)
OBJ=weather.o beheader.o json-glib-example.o

all: weather json-glib-example

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

weather: weather.o beheader.o
	$(CC) weather.o beheader.o -o weather $(shell pkg-config --libs gtk+-3.0) -export-dynamic

json-glib-example: json-glib-example.o
	$(CC) json-glib-example.o -o json-glib-example $(shell pkg-config --libs gtk+-3.0 json-glib-1.0) -export-dynamic

clean:
	rm weather json-glib-example $(OBJ)
