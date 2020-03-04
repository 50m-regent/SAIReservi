#include<stdio.h>

#define WIDTH 8
#define HEIGHT 8
#define BLACK '*'
#define WHITE 'o'
#define PLACABLE '.'
#define NONE ' '

// horizontal mask for searching placable cell
#if WIDTH == 4
#define HMASK 0x6060606060606060ul
#elif WIDTH == 5
#define HMASK 0x7070707070707070ul
#elif WIDTH == 6
#define HMASK 0x7878787878787878ul
#elif WIDTH == 7
#define HMASK 0x7c7c7c7c7c7c7c7cul
#else
#define HMASK 0x7e7e7e7e7e7e7e7eul
#endif

// vertical mask for searching placable cell
#if HEIGHT == 4
#define VMASK 0x00f0f0f0f0f0f000ul
#elif HEIGHT == 5
#define VMASK 0x00f8f8f8f8f8f800ul
#elif HEIGHT == 6
#define VMASK 0x00fcfcfcfcfcfc00ul
#elif HEIGHT == 7
#define VMASK 0x00fefefefefefe00ul
#else
#define VMASK 0x00ffffffffffff00ul
#endif

// full mask for searching placable cell
#define DMASK HMASK & VMASK

int countb(unsigned long num) {
    num = (num >> 1  & 0x5555555555555555ul) + (num & 0x5555555555555555ul);
    num = (num >> 2  & 0x3333333333333333ul) + (num & 0x3333333333333333ul);
    num = (num >> 4  & 0x0f0f0f0f0f0f0f0ful) + (num & 0x0f0f0f0f0f0f0f0ful);
    num = (num >> 8  & 0x00ff00ff00ff00fful) + (num & 0x00ff00ff00ff00fful);
    num = (num >> 16 & 0x0000ffff0000fffful) + (num & 0x0000ffff0000fffful);
    return (num >> 32) + (num & 0x00000000fffffffful);
}

void printb(unsigned long num) {
    putchar(' ');
    for (int i = 1; i <= WIDTH; i++) {
        printf(" %d", i);
    }

    for (int i = 0; i < WIDTH * HEIGHT; i++) {
        if (!(i % WIDTH)) printf("\n%d ", i / WIDTH + 1);
        if (num >> i & 1) putchar(BLACK);
        else putchar(NONE);
        putchar(NONE);
    }
    putchar('\n');
}

void view(unsigned long black, unsigned long white, unsigned long placable) {
    putchar(' ');
    for (int i = 1; i <= WIDTH; i++) {
        printf(" %d", i);
    }

    for (int i = 0; i < WIDTH * HEIGHT; i++) {
        if (!(i % WIDTH)) printf("\n%d ", i / WIDTH + 1);
        if (black >> i & 1) putchar(BLACK);
        else if (white >> i & 1) putchar(WHITE);
        else if (placable >> i & 1) putchar(PLACABLE);
        else putchar(NONE);
        putchar(NONE);
    }
    putchar('\n');
    printf("BLACK %d : %d WHITE\n", countb(black), countb(white));
}

unsigned long c2b(int x, int y) {
    if (x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT) return 0;
    else return 1lu << (y * WIDTH + x);
}

unsigned long get_placable(unsigned long p1, unsigned long p2) {
    unsigned long
        hw = p2 & HMASK,
        vw = p2 & VMASK,
        dw = p2 & DMASK,

        lb  = hw & (p1 >> 1),
        rb  = hw & (p1 << 1),
        tb  = vw & (p1 >> WIDTH),
        bb  = vw & (p1 << WIDTH),
        ltb = dw & (p1 >> WIDTH + 1),
        lbb = dw & (p1 << WIDTH - 1),
        rtb = dw & (p1 >> WIDTH - 1),
        rbb = dw & (p1 << WIDTH + 1);
    
    for (int i = 0; i < 5; i++) {
        lb  |= hw & (lb  >> 1);
        rb  |= hw & (rb  << 1);
        tb  |= vw & (tb  >> WIDTH);
        bb  |= vw & (bb  << WIDTH);
        ltb |= dw & (ltb >> WIDTH + 1);
        lbb |= dw & (lbb << WIDTH - 1);
        rtb |= dw & (rtb >> WIDTH - 1);
        rbb |= dw & (rbb << WIDTH + 1);
    }

    lb  >>= 1,
    rb  <<= 1,
    tb  >>= WIDTH,
    bb  <<= WIDTH,
    ltb >>= WIDTH + 1,
    lbb <<= WIDTH - 1,
    rtb >>= WIDTH - 1,
    rbb <<= WIDTH + 1;

    return ~(p1 | p2) & (lb | rb | tb | bb | ltb | lbb | rtb | rbb);
}

int main() {
    unsigned long 
        black = c2b(WIDTH / 2 - 1, HEIGHT / 2 - 1) | c2b(WIDTH / 2, HEIGHT / 2),
        white = c2b(WIDTH / 2 - 1, HEIGHT / 2)     | c2b(WIDTH / 2, HEIGHT / 2 - 1),
        placable = 0, hand, turn = WIDTH * HEIGHT;

    while (turn--) {
        if (turn % 2) placable = get_placable(black, white);
        else placable = get_placable(white, black);

        view(black, white, placable);

        int x, y;
        do {
            printf("x<< ");
            scanf("%d", &x);
            printf("y<< ");
            scanf("%d", &y);
            x--, y--;
            hand = c2b(x, y);
        } while (!(hand & placable));

        if (turn % 2) black |= hand;
        else white |= hand;
    }
}