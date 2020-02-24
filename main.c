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

int main() {
    long unsigned int black = c2b(3, 3) | c2b(4, 4), white = c2b(3, 4) | c2b(4, 3);

    while (1) {
        int x, y;
        scanf("x<< %d", &x);
        scanf("y<< %d", &x);
    }
}