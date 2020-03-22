#ifndef _DATA_H_
#define _DATA_H_

#include "bitboard.h"
#include "player.h"

typedef struct {
    bitboard *black, *white, placable, hand, rev;
    int turn, have_passed, debug_mode;
    Player *player, *opponent;
} Data;

Data init(int player_type, int opponent_type, int *player_board_score, int *opponent_board_score);
void view_game_status(Data data);
void set_placable(Data *data);

#endif