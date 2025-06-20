CC=gcc
CFLAGS=-Wall -Iinclude

SRC=src
OBJ=main.o lexer.o parser.o interpreter.o

all: rody

%.o: $(SRC)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

rody: $(OBJ)
	$(CC) $(CFLAGS) -o rody $(OBJ)

clean:
	rm -f *.o rody
