#include <stdio.h>

#include "game.h"
#include "data.h"
#include "bitboard.h"
#include "player.h"

void flip(Data *data) {
    bitboard rev = 0;

    for (int i = 0; i < 8; i++) {
        bitboard _rev = 0, trans = game->hand;

        while ((trans = transfer(trans, mask[i], dir[i])) && trans & data->opponent.board) _rev |= trans;
        if (trans & data->player.board) rev |= _rev;
    }

    data->player.board   ^= data->hand | rev;
    data->opponent.board ^= rev;
}

void change_turn(Data *data) {
    Player *tmp = data->player;
    data->player   = data->opponent,
    data->opponent = tmp;
    
    data->turn++;
}

int finish_game(Data data) {
    int
        black_count = count_standing_bit(data.black),
        white_count = count_standing_bit(data.white);

    if (data.debug) puts("Game finished.");

    if (black_count > white_count) {
        if (data.debug) puts("Winner: Black");
        return 1;
    } else if (black_count < white_count) {
        if (data.debug) puts("Winner: White");
        return -1;
    } else {
        if (data.debug) puts("Draw");
        return 0;
    }
}

int play(Data data) {
    while (true) {
        set_placable(&data);

        if(data.debug) {
            data.turn % 2 ? puts("\nWhite's turn\n") : puts("\nBlack's turn\n");
            view(game);
        }

        if (!data.placable) {
            data.have_passed++;
            if (data.have_passed == 1) {
                if (data.debug) puts("Pass");
                continue;
            } else {
                if (data.debug) view(data);
                return finish_game(data);
            }
        } else data.have_passed = 0;

        if (data.player.player_type == 0)      get_human_input(&data);
        else if (game.player.player_type == 1) get_random_input(&data);
        else if (game.player.player_type == 2) get_ai_input(&game, 0);

        flip(&data);
        change_turn(&data);
    }

    return 0;
}