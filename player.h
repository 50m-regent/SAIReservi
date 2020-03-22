#ifndef _PLAYER_H_
#define _PLAYER_H_

#include "bitboard.h"

typedef struct {
    int player_type, *board_score;
    bitboard board;
} Player;

#endif