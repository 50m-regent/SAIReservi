#ifndef _BITBOARD_H_
#define _BITBOARD_H_

typedef unsigned long long bitboard;

int count_standing_bit(bitboard x);
void print_board(bitboard x);
bitboard coordinate_to_bitboard(int x, int y);
bitboard transfer_bit(bitboard hand, bitboard mask, int direction);

#endif