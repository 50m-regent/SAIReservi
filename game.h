#ifndef _GAME_H_
#define _GAME_H_

#include "data.h"

void flip(Data *data);
void change_turn(Data *data);
int finish_game(Data data);
int play(Data data);

#endif