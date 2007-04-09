CFLAGS=-g -Wall -ansi -pedantic -DDEBUG

all: libasf

libasf: src/main.o src/asf.o src/fileio.o src/header.o src/byteio.o src/data.o src/utf.o src/parse.o src/guid.o
	gcc -o libasf src/*.o

clean:
	rm -f libasf src/*.o
