#include <stdio.h>

#include "game.h"
#include "const.h"
#include "data.h"
#include "bitboard.h"
#include "player.h"
#include "input.h"

void flip(Data *data) {
    const int search_directions[] = {
        -1,
        1,
        1 - WIDTH,
        WIDTH - 1,
        -WIDTH,
        WIDTH,
        -1 - WIDTH,
        WIDTH + 1
    };

    const bitboard mask[] = {
        LMASK,
        RMASK,
        RMASK & TMASK,
        LMASK & BMASK,
        TMASK,
        BMASK,
        LMASK & TMASK,
        RMASK & BMASK
    };

    bitboard rev = 0;

    for (int i = 0; i < 8; i++) {
        bitboard _rev = 0, trans = data->hand;

        while ((trans = transfer_bit(trans, mask[i], search_directions[i])) && trans & data->opponent->board) _rev |= trans;
        if (trans & data->player->board) rev |= _rev;
    }

    data->player->board   ^= data->hand | rev;
    data->opponent->board ^= rev;
}

void change_turn(Data *data) {
    Player *tmp = data->player;
    data->player   = data->opponent,
    data->opponent = tmp;
    
    data->turn++;
}

int finish_game(Data data) {
    int
        black_count = count_standing_bit(*data.black),
        white_count = count_standing_bit(*data.white);

    if (data.debug_mode) puts("Game finished.");

    if (black_count > white_count) {
        if (data.debug_mode) puts("Winner: Black");
        return 1;
    } else if (black_count < white_count) {
        if (data.debug_mode) puts("Winner: White");
        return -1;
    } else {
        if (data.debug_mode) puts("Draw");
        return 0;
    }
}

int play(Data data) {
    while (1) {
        set_placable(&data);

        if(data.debug_mode) {
            data.turn % 2 ? puts("\nWhite's turn\n") : puts("\nBlack's turn\n");
            view_game_status(data);
        }

        if (!data.placable) {
            data.have_passed++;
            if (data.have_passed == 1) {
                if (data.debug_mode) puts("Pass");
                continue;
            } else return finish_game(data);
        } else data.have_passed = 0;

        if (data.player->player_type == 0)      get_human_input(&data);
        else if (data.player->player_type == 1) get_random_input(&data);
        // else if (data.player->player_type == 2) get_ai_input(&data, 0);

        flip(&data);
        change_turn(&data);
    }

    return 0;
}