#include<iostream>

#define WIDTH 8      // width of board
#define HEIGHT 8     // height (same as width)
#define BLACK '*'    // what represents black's piece
#define WHITE 'o'    // white
#define PLACABLE '.' // placable cells
#define NONE ' '     // for spaces

// mask for searching
#define LMASK 0xfefefefefefefefeul // Left
#define RMASK 0x7f7f7f7f7f7f7f7ful // Right
#define HMASK LMASK & RMASK        // Horizontal
#define TMASK 0xffffffffffffff00ul // Top
#define BMASK 0x00fffffffffffffful // Bottom
#define VMASK TMASK & BMASK        // Vertical

class Bitboard {
public:
    unsigned long num;

    Bitboard();

    Bitboard(int x, int y) {
        if (
            x < 0 ||
            x >= WIDTH ||
            y < 0 ||
            y >= HEIGHT
        )    num= 0;
        else num = 1lu << (y * WIDTH + x);
    }

    Bitboard operator = (int x) {
        num = x;
        return *this;
    }

    Bitboard operator ~ () {
        num = ~num;
        return *this;
    }

    Bitboard operator | (Bitboard x) {
        num |= x.num;
        return *this;
    }

    Bitboard operator |= (Bitboard x) {
        num |= x.num;
        return *this;
    }

    Bitboard operator & (int x) {
        num &= x;
        return *this;
    }

    Bitboard operator & (Bitboard x) {
        num &= x.num;
        return *this;
    }

    Bitboard operator >> (int x) {
        num >>= x;
        return *this;
    }

    Bitboard operator << (int x) {
        num <<= x;
        return *this;
    }

    int count() {
        num = (num >> 1  & 0x5555555555555555ul) + (num & 0x5555555555555555ul);
        num = (num >> 2  & 0x3333333333333333ul) + (num & 0x3333333333333333ul);
        num = (num >> 4  & 0x0f0f0f0f0f0f0f0ful) + (num & 0x0f0f0f0f0f0f0f0ful);
        num = (num >> 8  & 0x00ff00ff00ff00fful) + (num & 0x00ff00ff00ff00fful);
        num = (num >> 16 & 0x0000ffff0000fffful) + (num & 0x0000ffff0000fffful);
        return (num >> 32) + (num & 0x00000000fffffffful);
    }

    void print() {
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
};

class Game {
public:
    Bitboard black, white, placable, hand, rev, *p, *o;
    int turn;
    bool pflag;

    Game() {
        black = Bitboard(WIDTH / 2 - 1, HEIGHT / 2 - 1) | Bitboard(WIDTH / 2, HEIGHT / 2),
        white = Bitboard(WIDTH / 2, HEIGHT / 2 - 1)     | Bitboard(WIDTH / 2 - 1, HEIGHT / 2),
        placable = 0;

        turn  = WIDTH * HEIGHT,
        pflag = 0;
    }

    void view() {
        putchar(' ');
        for (int i = 0; i < WIDTH; i++) printf(" %c", 'A' + i);

        for (int i = 0; i < WIDTH * HEIGHT; i++) {
            if (!(i % WIDTH)) printf("\n%d ", i / WIDTH + 1);

            if ((black >> i & 1).count())         putchar(BLACK);
            else if ((white >> i & 1).count())    putchar(WHITE);
            else if ((placable >> i & 1).count()) putchar(PLACABLE);
            else                                  putchar(NONE);
            putchar(NONE);
        }
        puts("\n");

        printf("BLACK %d : %d WHITE\n", black.count(), white.count());
    }

    void set_placable() {
        Bitboard
            hm = *o & HMASK,
            vm = *o & VMASK,
            dw = hm & vm,

            hb  = hm & (*p >> 1 | *p << 1),
            vb  = vm & (*p >> WIDTH | *p << WIDTH),
            db1 = dw & (*p >> (WIDTH + 1) | *p << (WIDTH + 1)),
            db2 = dw & (*p >> (WIDTH - 1) | *p << (WIDTH - 1));
        
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

        placable = ~(*p | *o) & (hb | vb | db1 | db2);
    }
};

/*
 * @brief Transfer cell for searching
 * @params hand: input cell
 *         mask: mask to prevent searching outside of board
 *         dir:  searching direction
 * @return transfered bitboard
*/
unsigned long transfer(
    unsigned long hand,
    unsigned long mask,
    int dir
) {
    if (dir > 0) hand >>= dir;
    else         hand <<= -dir;
    return mask & hand;
}

/*
 * @brief Flipping pieces
 * @params hand: input cell
 *         *p:   pointer of player bitboard
 *         *o:   pointer of opponent bitboard
 * @return flipped place in bitboard
*/
void flip(Game *game) {
    int dir[] = {
        // 探索方向(ビットシフト数)
        -1,
        1,
        1 - WIDTH,
        WIDTH - 1,
        -WIDTH,
        WIDTH,
        -1 - WIDTH,
        WIDTH + 1
    };
    unsigned long
        // パックマンみたいにならないように端は消す
        mask[] = {
            LMASK,
            RMASK,
            RMASK & TMASK,
            LMASK & BMASK,
            TMASK,
            BMASK,
            LMASK & TMASK,
            RMASK & BMASK
        }, rev = 0, *p, *o;

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

/*
 * @brief Called when the game is finished.
 * @params black: black's bitboard
 *         white: white's bitboard
*/
void end(Game game) {
    int
        b = countb(game.black),
        w = countb(game.white);

    puts("Game finished.");

    if      (w < b) puts("Winner: Black");
    else if (w > b) puts("Winner: White");
    else            puts("Draw");
}

void init(Game *game) {
    game->black = c2b(WIDTH / 2 - 1, HEIGHT / 2 - 1) | c2b(WIDTH / 2, HEIGHT / 2),
    game->white = c2b(WIDTH / 2, HEIGHT / 2 - 1)     | c2b(WIDTH / 2 - 1, HEIGHT / 2),
    game->placable = 0;

    game->turn  = WIDTH * HEIGHT,
    game->pflag = 0;
}

int main() {
    Game game;

    init(&game);

    // ずっとループ
    while (game.turn--) {
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

            game.hand = c2b(x - 65, --y);
            scanf("%c", &x);
        } while (!(game.hand & game.placable)); // 有効手じゃない間ループ

        // 反転処理
        flip(&game);
    }
}