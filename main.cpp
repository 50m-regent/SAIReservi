#include<stdio.h>

#define WIDTH 8
#define HEIGHT 8
#define BLACK '*'
#define WHITE 'o'
#define PLACABLE '.'
#define NONE ' '

#define LMASK 0xfefefefefefefefeul
#define RMASK 0x7f7f7f7f7f7f7f7ful
#define HMASK LMASK & RMASK
#define TMASK 0xffffffffffffff00ul
#define BMASK 0x00fffffffffffffful
#define VMASK TMASK & BMASK

class Bitboard {
private:
    unsigned long num;

public:
    Bitboard () {};

    Bitboard (unsigned long x) {num = x;}

    Bitboard (int x, int y) {
        if (
            x < 0 ||
            x >= WIDTH ||
            y < 0 ||
            y >= HEIGHT
        )    num= 0;
        else num = 1lu << (y * WIDTH + x);
    }

    Bitboard operator = (unsigned long x) {
        num = x;
        return *this;
    }

    Bitboard operator ~ () {return Bitboard(~num);}

    Bitboard operator | (Bitboard x) {return Bitboard(num | x.num);}

    Bitboard operator |= (Bitboard x) {
        num |= x.num;
        return *this;
    }

    Bitboard operator & (unsigned long x) {return Bitboard(num & x);}

    Bitboard operator & (Bitboard x) {return Bitboard(num & x.num);}

    Bitboard operator ^= (Bitboard x) {
        num ^= x.num;
        return *this;
    }

    bool operator && (Bitboard x) {return num && x.num;}

    Bitboard operator >> (int x) {return Bitboard(num >> x);}

    Bitboard operator >>= (int x) {
        num >>= x;
        return *this;
    }

    Bitboard operator << (int x) {return Bitboard(num << x);}

    Bitboard operator <<= (int x) {
        num <<= x;
        return *this;
    }

    int count() {
        unsigned long _num = num;
        _num = (_num >> 1  & 0x5555555555555555ul) + (_num & 0x5555555555555555ul);
        _num = (_num >> 2  & 0x3333333333333333ul) + (_num & 0x3333333333333333ul);
        _num = (_num >> 4  & 0x0f0f0f0f0f0f0f0ful) + (_num & 0x0f0f0f0f0f0f0f0ful);
        _num = (_num >> 8  & 0x00ff00ff00ff00fful) + (_num & 0x00ff00ff00ff00fful);
        _num = (_num >> 16 & 0x0000ffff0000fffful) + (_num & 0x0000ffff0000fffful);
        return (_num >> 32) + (_num & 0x00000000fffffffful);
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
        puts("\n");
    }

    Bitboard transfer(
        Bitboard mask,
        int dir
    ) {
        if (dir > 0) num >>= dir;
        else         num <<= -dir;
        num &= mask.num;
        return *this;
    }
};

class Game {
private:
    Bitboard black, white, placable, hand, rev, *p, *o;
    int turn;
    bool pflag;

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

    void flip() {
        int dir[] = {
            -1,
            1,
            1 - WIDTH,
            WIDTH - 1,
            -WIDTH,
            WIDTH,
            -1 - WIDTH,
            WIDTH + 1
        };
        Bitboard
            mask[] = {
                LMASK,
                RMASK,
                RMASK & TMASK,
                LMASK & BMASK,
                TMASK,
                BMASK,
                LMASK & TMASK,
                RMASK & BMASK
            }, rev = 0;

        for (int i = 0; i < 8; i++) {
            Bitboard _rev = 0, trans = hand;

            while ((trans.transfer(mask[i], dir[i])) && trans & *o) _rev |= trans;
            if ((trans & *p).count()) rev |= _rev;
        }

        *p ^= hand | rev;
        *o ^= rev;
    }

    void end() {
        int
            b = black.count(),
            w = white.count();

        puts("Game finished.");

        if      (w < b) puts("Winner: Black");
        else if (w > b) puts("Winner: White");
        else            puts("Draw");
    }

public:
    Game() {
        black = Bitboard(WIDTH / 2 - 1, HEIGHT / 2 - 1) | Bitboard(WIDTH / 2, HEIGHT / 2),
        white = Bitboard(WIDTH / 2, HEIGHT / 2 - 1)     | Bitboard(WIDTH / 2 - 1, HEIGHT / 2),
        placable = 0;

        turn  = WIDTH * HEIGHT,
        pflag = 0;
    }

    void play() {
        while (turn--) {
            if (turn % 2) puts("\nBlack's turn\n"), p = &black, o = &white;
            else          puts("\nWhite's turn\n"), p = &white, o = &black;

            set_placable();
            view();

            if (!placable.count()) {
                if (pflag) {
                    view();
                    end();
                    return;
                } else {
                    pflag = true;
                    puts("Pass");
                    continue;
                }
            } else pflag = 0;

            bool is_first_time = false;

            do {
                char x;
                int y;

                if (is_first_time) puts("Invaild Input.");
                else               is_first_time = true;

                printf("Input<< ");
                scanf("%c%d", &x, &y);

                hand = Bitboard(x - 65, --y);
            } while (!(hand & placable).count());

            flip();
        }
    }
};

int main() {
    Game game;
    game.play();
    return 0;
}