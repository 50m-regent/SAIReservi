#include <stdio.h>
#include <stdlib.h>

#include "input.h"
#include "const.h"

int evaluate(Data data) {
    /*
        scoreは大きさ9の配列のポインタ
        0 1 2 3 3 2 1 0
        1 4 5 6 6 5 4 1
        2 5 7 8 8 7 5 2
        3 6 8 x x 8 6 3
        3 6 8 x x 8 6 3
        2 5 7 8 8 7 5 2
        1 4 5 6 6 5 4 1
        0 1 2 3 3 2 1 0
        みたいに盤面のますごとの点数を格納しておく。
    */
    int *points, black = 0, white = 0;

    if (data.turn % 2) {
        int _points[] = {
            data.opponent->board_score[0], data.opponent->board_score[1], data.opponent->board_score[2], data.opponent->board_score[3], data.opponent->board_score[3], data.opponent->board_score[2], data.opponent->board_score[1], data.opponent->board_score[0],
            data.opponent->board_score[1], data.opponent->board_score[4], data.opponent->board_score[5], data.opponent->board_score[6], data.opponent->board_score[6], data.opponent->board_score[5], data.opponent->board_score[4], data.opponent->board_score[1],
            data.opponent->board_score[2], data.opponent->board_score[5], data.opponent->board_score[7], data.opponent->board_score[8], data.opponent->board_score[8], data.opponent->board_score[7], data.opponent->board_score[5], data.opponent->board_score[2],
            data.opponent->board_score[3], data.opponent->board_score[6], data.opponent->board_score[8],                             0,                             0, data.opponent->board_score[8], data.opponent->board_score[6], data.opponent->board_score[3]
        };
        points = _points;
    } else {
        int _points[] = {
            data.player->board_score[0], data.player->board_score[1], data.player->board_score[2], data.player->board_score[3], data.player->board_score[3], data.player->board_score[2], data.player->board_score[1], data.player->board_score[0],
            data.player->board_score[1], data.player->board_score[4], data.player->board_score[5], data.player->board_score[6], data.player->board_score[6], data.player->board_score[5], data.player->board_score[4], data.player->board_score[1],
            data.player->board_score[2], data.player->board_score[5], data.player->board_score[7], data.player->board_score[8], data.player->board_score[8], data.player->board_score[7], data.player->board_score[5], data.player->board_score[2],
            data.player->board_score[3], data.player->board_score[6], data.player->board_score[8],                           0,                           0, data.player->board_score[8], data.player->board_score[6], data.player->board_score[3]
        };
        points = _points;
    }

    for (int i = 0; i < WIDTH * HEIGHT; i++) {
        if (*data.black >> i & 1)      black += i < 32 ? points[i] : points[64 - i];
        else if (*data.white >> i & 1) white += i < 32 ? points[i] : points[64 - i];
    }

    return black - white;
}

void get_human_input(Data *data) {
    int is_first_time = 0;

    do {
        char x;
        int y;

        if (is_first_time) puts("Invaild Input.");
        else               is_first_time++;

        printf("Input<< ");
        scanf("%c%d", &x, &y);
        
        data->hand = coordinate_to_bitboard(x - 65, --y);
    } while (!(data->hand & data->placable));
}

void get_random_input(Data *data) {
    do {
        char x;
        int y;

        x = rand() % 8;
        y = rand() % 8;
        
        data->hand = coordinate_to_bitboard(x, y);
    } while (!(data->hand & data->placable));
}