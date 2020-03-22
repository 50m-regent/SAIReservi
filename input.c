#include <stdio.h>

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

    if (game->turn % 2) {
        int _points[] = {
            game->p.seed[0], game->p.seed[1], game->p.seed[2], game->p.seed[3], game->p.seed[3], game->p.seed[2], game->p.seed[1], game->p.seed[0],
            game->p.seed[1], game->p.seed[4], game->p.seed[5], game->p.seed[6], game->p.seed[6], game->p.seed[5], game->p.seed[4], game->p.seed[1],
            game->p.seed[2], game->p.seed[5], game->p.seed[7], game->p.seed[8], game->p.seed[8], game->p.seed[7], game->p.seed[5], game->p.seed[2],
            game->p.seed[3], game->p.seed[6], game->p.seed[8],               0,               0, game->p.seed[8], game->p.seed[6], game->p.seed[3]
        };
        points = _points;
    } else {
        int _points[] = {
            game->o.seed[0], game->o.seed[1], game->o.seed[2], game->o.seed[3], game->o.seed[3], game->o.seed[2], game->o.seed[1], game->o.seed[0],
            game->o.seed[1], game->o.seed[4], game->o.seed[5], game->o.seed[6], game->o.seed[6], game->o.seed[5], game->o.seed[4], game->o.seed[1],
            game->o.seed[2], game->o.seed[5], game->o.seed[7], game->o.seed[8], game->o.seed[8], game->o.seed[7], game->o.seed[5], game->o.seed[2],
            game->o.seed[3], game->o.seed[6], game->o.seed[8],               0,               0, game->o.seed[8], game->o.seed[6], game->o.seed[3]
        };
        points = _points;
    }

    for (int i = 0; i < WIDTH * HEIGHT; i++) {
        if (game->black >> i & 1)      black += i < 32 ? points[i] : points[64 - i];
        else if (game->white >> i & 1) white += i < 32 ? points[i] : points[64 - i];
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