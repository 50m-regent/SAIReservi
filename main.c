#include<stdio.h>

#define WIDTH 8
#define HEIGHT 8

long unsigned int c2b(int x, int y) {
    if (x < 0 || x > 7 || y < 0 || y > 7) return 0;

    long unsigned int bitset = 1;

    bitset <<= (x * 8);
    bitset <<= y;

    return bitset;
}

void printb(long unsigned int bitset) {
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            if (bitset >> (x + y * WIDTH) & 1) putchar('1');
            else putchar('0');
            putchar(' ');
        }
        putchar('\n');
    }
    putchar('\n');
}

typedef struct Board {
    long unsigned int black, white;
} Board;

Board create_board() {
    Board board;
    board.black = c2b(3, 3) | c2b(4, 4);
    board.white = c2b(3, 4) | c2b(4, 3);

    return board;
}

int main() {
    Board board = create_board();
}