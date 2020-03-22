#include <stdlib.h>
#include <time.h>

#include "header/game.h"

int 
    player_seed[]   = {45, -11, 4, -1, -16, -1, -3, 2, -1},
    opponent_seed[] = {30, -20, 2, -2, -20, -2,  1, 2, -1};

int main() {
    srand(time(NULL));
    
    Data data = init(1, 1, player_seed, opponent_seed);

    play(data);
}

/*
int main() {
    srand(time(NULL));

    Data data;

    int 
        pseed[9] = {45, -11, 4, -1, -16, -1, -3, 2, -1},
        oseed[9] = {};

    for (int i = 0; i < 9; i++) pseed[i] = rand() % (BIAS_MAX - BIAS_MIN + 1) + BIAS_MIN;

    for (int epoch = 0; epoch < 10000; epoch++) {
        int result[] = {0, 0, 0};

        for (int i = 0; i < 9; i++) oseed[i] = rand() % (BIAS_MAX - BIAS_MIN + 1) + BIAS_MIN;

        data = init(2, 2, pseed, oseed);
        game.debug = 0;
        result[1 + play(game)]++;

        if (result[2] < result[0]) *pseed = *oseed;

        printf("Black:%d White:%d Draw:%d\n", result[2], result[0], result[1]);
    }

    for (int i = 0; i < 9; i++) printf("%d\n", pseed[i]);

    return 0;
}
*/