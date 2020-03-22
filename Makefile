PROGRAM = main
OBJS = header/bitboard.o header/input.o header/data.o header/game.o 
CC = gcc

$(PROGRAM): $(PROGRAM).o $(OBJS)
	$(CC) $(PROGRAM).c -o $(PROGRAM) $(OBJS)

clean:
	rm -f *.o header/*.o