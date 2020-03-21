#include "reversi.h"

int countb(unsigned long num) {
    num = (num >> 1  & 0x5555555555555555ul) + (num & 0x5555555555555555ul);
    num = (num >> 2  & 0x3333333333333333ul) + (num & 0x3333333333333333ul);
    num = (num >> 4  & 0x0f0f0f0f0f0f0f0ful) + (num & 0x0f0f0f0f0f0f0f0ful);
    num = (num >> 8  & 0x00ff00ff00ff00fful) + (num & 0x00ff00ff00ff00fful);
    num = (num >> 16 & 0x0000ffff0000fffful) + (num & 0x0000ffff0000fffful);
    return (num >> 32) + (num & 0x00000000fffffffful);
}

void printb(unsigned long num) {
    // あの上のあれ
    putchar(' ');
    for (int i = 0; i < WIDTH; i++) printf(" %c", 'A' + i);

    // loop for 64 bits
    for (int i = 0; i < WIDTH * HEIGHT; i++) {
        if (!(i % WIDTH)) printf("\n%d ", i / WIDTH + 1);

        if (num >> i & 1) putchar(PLACABLE);
        else              putchar(NONE);
        putchar(NONE);
    }
    putchar('\n');
}

void view(Game game) {
    putchar(' ');
    for (int i = 0; i < WIDTH; i++) printf(" %c", 'A' + i);

    for (int i = 0; i < WIDTH * HEIGHT; i++) {
        if (!(i % WIDTH)) printf("\n%d ", i / WIDTH + 1);

        if (game.black >> i & 1)         putchar(BLACK);
        else if (game.white >> i & 1)    putchar(WHITE);
        else if (game.placable >> i & 1) putchar(PLACABLE);
        else                        putchar(NONE);
        putchar(NONE);
    }
    puts("\n");

    printf("BLACK %d : %d WHITE\n", countb(game.black), countb(game.white));
}

unsigned long c2b(int x, int y) {
    if (
        x < 0 ||
        x >= WIDTH ||
        y < 0 ||
        y >= HEIGHT
    )    return 0;
    else return 1lu << (y * WIDTH + x);
}

void set_placable(Game *game) {
    unsigned long p, o;
    if (game->turn % 2) p = game->black, o = game->white;
    else                p = game->white, o = game->black;

    unsigned long
        hm = o & HMASK,
        vm = o & VMASK,
        dw = hm & vm,

        hb  = hm & (p >> 1 | p << 1),
        vb  = vm & (p >> WIDTH | p << WIDTH),
        db1 = dw & (p >> (WIDTH + 1) | p << (WIDTH + 1)),
        db2 = dw & (p >> (WIDTH - 1) | p << (WIDTH - 1));
    
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

    game->placable = ~(p | o) & (hb | vb | db1 | db2);
}

unsigned long transfer(unsigned long hand, unsigned long mask, int dir) {
    if (dir > 0) hand >>= dir;
    else         hand <<= -dir;
    return mask & hand;
}

void flip(Game *game) {
    unsigned long rev = 0, *p, *o;

    if (game->turn % 2) p = &game->black, o = &game->white;
    else                p = &game->white, o = &game->black;

    for (int i = 0; i < 8; i++) {
        unsigned long _rev = 0, trans = game->hand;

        while ((trans = transfer(trans, mask[i], dir[i])) && trans & *o) _rev |= trans;
        if (trans & *p) rev |= _rev;
    }

    *p ^= game->hand | rev;
    *o ^= rev;
}

void end(Game game) {
    int
        b = countb(game.black),
        w = countb(game.white);

    puts("Game finished.");

    if      (w < b) puts("Winner: Black");
    else if (w > b) puts("Winner: White");
    else            puts("Draw");
}

Game init(int ptype, int otype) {
    Game game;

    game.black = c2b(WIDTH / 2 - 1, HEIGHT / 2 - 1) | c2b(WIDTH / 2, HEIGHT / 2),
    game.white = c2b(WIDTH / 2, HEIGHT / 2 - 1)     | c2b(WIDTH / 2 - 1, HEIGHT / 2),
    game.placable = 0;

    game.turn = game.pflag = 0;

    game.p.board = &game.black, game.o.board = &game.white,

    game.p.type = ptype,
    game.o.type = otype;
    
    return game;
}

void get_human_input(Game *game) {
    // 初回かどうか
    int is_first_time = 0;

    // 入力処理
    do {
        char x;
        int y;

        // 初回じゃなければ警告
        if (is_first_time) puts("Invaild Input.");
        else               is_first_time++;

        printf("Input<< ");
        scanf("%c%d", &x, &y);
        
        game->hand = c2b(x - 65, --y);
    } while (!(game->hand & game->placable)); // 有効手じゃない間ループ
}

void get_random_input(Game *game) {
    // 入力処理
    do {
        char x;
        int y;

        x = rand() % 8;
        y = rand() % 8;
        
        game->hand = c2b(x, y);
    } while (!(game->hand & game->placable)); // 有効手じゃない間ループ
}

int main() {
    srand(time(NULL));

    Game game = init(1, 1);

    // ずっとループ
    while (++game.turn) {
        // 黒のターン
        if (game.turn % 2) puts("\nBlack's turn\n");
        // 白のターン
        else               puts("\nWhite's turn\n");

        set_placable(&game);
        view(game);

        // パス処理
        if (!game.placable) {
            game.pflag++;
            if (game.pflag == 1) {
                puts("Pass");
                continue;
            } else {
                view(game);
                end(game);
                return 0;
            }
        } else game.pflag = 0;

        if (game.p.type == 0) {
            get_human_input(&game);
        } else if (game.p.type == 1) {
            get_random_input(&game);
        } else {

        }

        flip(&game);

        // ターンチェンジ
        unsigned long *tmp = game.p.board;
        game.p.board = game.o.board,
        game.o.board = tmp;

        game.p.type += game.o.type,
        game.o.type  = game.p.type - game.o.type,
        game.p.type -= game.o.type;
    }
}