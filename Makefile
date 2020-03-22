PROGRAM = main
OBJS = bitboard.o data.o game.o input.o
CC = gcc

$(PROGRAM): $(PROGRAM).o $(OBJS)
	$(CC) $(PROGRAM).c -o $(PROGRAM) $(OBJS)