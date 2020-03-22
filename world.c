#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define WIDTH  8
#define HEIGHT 8

#define BLACK    '*'
#define WHITE    'o'
#define PLACABLE '.'
#define NONE     ' '

#define LMASK 0xfefefefefefefefeul
#define RMASK 0x7f7f7f7f7f7f7f7ful
#define HMASK LMASK & RMASK
#define TMASK 0xffffffffffffff00ul
#define BMASK 0x00fffffffffffffful
#define VMASK TMASK & BMASK

typedef unsigned long long bitboard;

typedef struct {
    int player_type, *board_score;
    bitboard board;
} Player;

typedef struct {
    bitboard *black, *white, placable, hand, rev;
    int turn, have_passed, debug_mode;
    Player *player, *opponent;
} Data;

int count_standing_bit(bitboard x);
void print_board(bitboard x);
bitboard coordinate_to_bitboard(int x, int y);
bitboard transfer_bit(bitboard hand, bitboard mask, int direction);
Data init(int player_type, int opponent_type, int *player_board_score, int *opponent_board_score);
void view_game_status(Data data);
void set_placable(Data *data);
void flip(Data *data);
void change_turn(Data *data);
int finish_game(Data data);
int play(Data data);
int evaluate(Data data);
void get_human_input(Data *data);
void get_random_input(Data *data);

int count_standing_bit(bitboard x) {
    x = (x >> 1  & 0x5555555555555555ul) + (x & 0x5555555555555555ul);
    x = (x >> 2  & 0x3333333333333333ul) + (x & 0x3333333333333333ul);
    x = (x >> 4  & 0x0f0f0f0f0f0f0f0ful) + (x & 0x0f0f0f0f0f0f0f0ful);
    x = (x >> 8  & 0x00ff00ff00ff00fful) + (x & 0x00ff00ff00ff00fful);
    x = (x >> 16 & 0x0000ffff0000fffful) + (x & 0x0000ffff0000fffful);
    x = (x >> 32 & 0x00000000fffffffful) + (x & 0x00000000fffffffful);
    return x;
}

void print_board(bitboard num) {
    putchar(' ');
    for (int i = 0; i < WIDTH; i++) printf(" %c", 'A' + i);

    for (int i = 0; i < WIDTH * HEIGHT; i++) {
        if (!(i % WIDTH)) printf("\n%d ", i / WIDTH + 1);

        if (num >> i & 1) putchar(PLACABLE);
        else              putchar(NONE);
        putchar(NONE);
    }
    putchar('\n');
}

bitboard coordinate_to_bitboard(int x, int y) {
    if (x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT) return 0;
    else                                             return 1lu << (y * WIDTH + x);
}

bitboard transfer_bit(bitboard hand, bitboard mask, int direction) {
    if (direction > 0) hand >>= direction;
    else               hand <<= -direction;
    
    return mask & hand;
}

Data init(int player_type, int opponent_type, int *player_board_score, int *opponent_board_score) {
    Data data;

    data.turn = data.have_passed = 0;
    data.debug_mode = 1;

    Player player, opponent;

    player.board =
        coordinate_to_bitboard(WIDTH / 2 - 1, HEIGHT / 2 - 1) |
        coordinate_to_bitboard(WIDTH / 2, HEIGHT / 2),
    opponent.board =
        coordinate_to_bitboard(WIDTH / 2, HEIGHT / 2 - 1) |
        coordinate_to_bitboard(WIDTH / 2 - 1, HEIGHT / 2);

    player.player_type   = player_type,
    opponent.player_type = opponent_type;

    player.board_score   = player_board_score;
    opponent.board_score = opponent_board_score;

    data.player   = &player,
    data.opponent = &opponent;

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

int main() {
    srand(time(NULL));

    int 
        player_seed[]   = {45, -11, 4, -1, -16, -1, -3, 2, -1},
        opponent_seed[] = {30, -20, 2, -2, -20, -2,  1, 2, -1};
    
    Data data = init(0, 1, player_seed, opponent_seed);

    play(data);
}