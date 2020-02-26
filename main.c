#include<stdio.h>

#define WIDTH 8
#define HEIGHT 8
#define BLACK 'o'
#define WHITE 'x'
#define PLACABLE '.'
#define NONE ' '

long unsigned int c2b(int x, int y) {
    if (x < 0 || x > 7 || y < 0 || y > 7) return 0;
    else return 1lu << (x * 8 + y);
}

void view(long unsigned int black, long unsigned int white) {
    for (int i = 0; i < WIDTH * HEIGHT; i++) {
        if (black >> i & 1) putchar(BLACK);
        else if (white >> i & 1) putchar(WHITE);
        else putchar(NONE);
        putchar(NONE);

        if (!((i + 1) % 8)) putchar('\n');
    }
    putchar('\n');
}

int main() {
    long unsigned int black = c2b(3, 3) | c2b(4, 4), white = c2b(3, 4) | c2b(4, 3);
    view(black, white);

    int turn = 64;
    while (turn--) {
        int x, y;

        do {
            printf("x<< ");
            scanf("%d", &x);
            printf("y<< ");
            scanf("%d", &y);
        } while (x < 0 || x > 7 || y < 0 || y > 7);

        long unsigned int hand = c2b(x, y);
        if (turn % 2) black |= hand;
        else white |= hand;

        view(black, white);
    }
}