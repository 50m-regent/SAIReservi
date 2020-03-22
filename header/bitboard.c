#include <stdio.h>

#include "bitboard.h"
#include "const.h"

int count_standing_bit(bitboard x) {
    x = (x >> 1  & 0x5555555555555555ul) + (x & 0x5555555555555555ul);
    x = (x >> 2  & 0x3333333333333333ul) + (x & 0x3333333333333333ul);
    x = (x >> 4  & 0x0f0f0f0f0f0f0f0ful) + (x & 0x0f0f0f0f0f0f0f0ful);
    x = (x >> 8  & 0x00ff00ff00ff00fful) + (x & 0x00ff00ff00ff00fful);
    x = (x >> 16 & 0x0000ffff0000fffful) + (x & 0x0000ffff0000fffful);
    x = (x >> 32 & 0x00000000fffffffful) + (x & 0x00000000fffffffful);
    return x;
}

void print_board(bitboard num) {
    putchar(' ');
    for (int i = 0; i < WIDTH; i++) printf(" %c", 'A' + i);

    for (int i = 0; i < WIDTH * HEIGHT; i++) {
        if (!(i % WIDTH)) printf("\n%d ", i / WIDTH + 1);

        if (num >> i & 1) putchar(PLACABLE);
        else              putchar(NONE);
        putchar(NONE);
    }
    putchar('\n');
}

bitboard coordinate_to_bitboard(int x, int y) {
    if (x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT) return 0;
    else                                             return 1lu << (y * WIDTH + x);
}

bitboard transfer_bit(bitboard hand, bitboard mask, int direction) {
    if (direction > 0) hand >>= direction;
    else               hand <<= -direction;
    
    return mask & hand;
}