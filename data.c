#include <stdio.h>

#include "data.h"
#include "const.h"
#include "bitboard.h"

Data init(int player_type, int opponent_type, int *player_board_score, int *opponent_board_score) {
    Data data;

    data.turn = data.have_passed = 0;
    data.debug_mode = 1;

    data.player->board =
        coordinate_to_bitboard(WIDTH / 2 - 1, HEIGHT / 2 - 1) |
        coordinate_to_bitboard(WIDTH / 2, HEIGHT / 2),
    data.opponent->board =
        coordinate_to_bitboard(WIDTH / 2, HEIGHT / 2 - 1) |
        coordinate_to_bitboard(WIDTH / 2 - 1, HEIGHT / 2);

    data.player->player_type   = player_type,
    data.opponent->player_type = opponent_type;

    data.player->board_score   = player_board_score;
    data.opponent->board_score = opponent_board_score;

    data.black = &data.player->board;
    data.white = &data.opponent->board;
    
    return data;
}

void view_game_status(Data data) {
    putchar(' ');
    for (int i = 0; i < WIDTH; i++) printf(" %c", 'A' + i);

    for (int i = 0; i < WIDTH * HEIGHT; i++) {
        if (!(i % WIDTH)) printf("\n%d ", i / WIDTH + 1);

        if (*data.black >> i & 1)         putchar(BLACK);
        else if (*data.white >> i & 1)    putchar(WHITE);
        else if (data.placable >> i & 1)  putchar(PLACABLE);
        else                              putchar(NONE);
        putchar(NONE);
    }
    puts("\n");

    printf("BLACK %d : %d WHITE\n", count_standing_bit(*data.black), count_standing_bit(*data.white));
}

void set_placable(Data *data) {
    bitboard
        hm = data->player->board & HMASK,
        vm = data->opponent->board & VMASK,
        dw = hm & vm,

        hb  = hm & (data->player->board >> 1 | data->player->board << 1),
        vb  = vm & (data->player->board >> WIDTH | data->player->board << WIDTH),
        db1 = dw & (data->player->board >> (WIDTH + 1) | data->player->board << (WIDTH + 1)),
        db2 = dw & (data->player->board >> (WIDTH - 1) | data->player->board << (WIDTH - 1));
    
    for (int i = 0; i < 5; i++) {
        hb  |= hm & (hb >> 1 | hb << 1);
        vb  |= vm & (vb >> WIDTH | vb << WIDTH);
        db1 |= dw & (db1 >> (WIDTH + 1) | db1 << (WIDTH + 1));
        db2 |= dw & (db2 >> (WIDTH - 1) | db2 << (WIDTH - 1));
    }

    hb  = hb >> 1 | hb << 1;
    vb  = vb >> WIDTH | vb << WIDTH;
    db1 = db1 >> (WIDTH + 1) | db1 << (WIDTH + 1);
    db2 = db2 >> (WIDTH - 1) | db2 << (WIDTH - 1);

    data->placable = ~(data->player->board | data->opponent->board) & (hb | vb | db1 | db2);
}