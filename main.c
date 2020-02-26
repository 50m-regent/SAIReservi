#include<stdio.h>

#define WIDTH 8
#define HEIGHT 8
#define BLACK 'o'
#define WHITE 'x'
#define PLACABLE '.'
#define NONE ' '

#if WIDTH == 4
#define V_MASK 0x6060606060606060ul
#elif WIDTH == 5
#define V_MASK 0x7070707070707070ul
#elif WIDTH == 6
#define V_MASK 0x7878787878787878ul
#elif WIDTH == 7
#define V_MASK 0x7c7c7c7c7c7c7c7cul
#else
#define V_MASK 0x7e7e7e7e7e7e7e7eul
#endif

#if HEIGHT == 4
#define H_MASK 0x00f0f0f0f0f0f000ul
#elif WIDTH == 5
#define H_MASK 0x00f8f8f8f8f8f800ul
#elif WIDTH == 6
#define H_MASK 0x00fcfcfcfcfcfc00ul
#elif WIDTH == 7
#define H_MASK 0x00fefefefefefe00ul
#else
#define H_MASK 0x00ffffffffffff00ul
#endif

unsigned long long c2b(int x, int y) {
    if (x < 0 || x > WIDTH - 1 || y < 0 || y > HEIGHT - 1) return 0;
    else return 1ul << (y * WIDTH + x);
}

int countb(unsigned long long n) {
	n = (n >> 1  & 0x5555555555555555ul) + n & 0x5555555555555555ul;
	n = (n >> 2  & 0x3333333333333333ul) + n & 0x3333333333333333ul;
	n = (n >> 4  & 0x0f0f0f0f0f0f0f0ful) + n & 0x0f0f0f0f0f0f0f0ful;
	n = (n >> 8  & 0x00ff00ff00ff00fful) + n & 0x00ff00ff00ff00fful;
	n = (n >> 16 & 0x0000ffff0000fffful) + n & 0x0000ffff0000fffful;
	return (n >> 32) + n & 0x00000000fffffffful;
}

void view(unsigned long long black, unsigned long long white, unsigned long long placable) {
	putchar(NONE);
	for (int i = 1; i < WIDTH + 1; i++) {
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
    puts("");
	printf("BLACK: %d, WHITE: %d\n", countb(black), countb(white));
}

int main() {
	unsigned long long black, white, placable = 0, hand;
    black = c2b(WIDTH / 2 - 1, WIDTH / 2 - 1) |
		c2b(WIDTH / 2, WIDTH / 2);
	white = c2b(WIDTH / 2 - 1, WIDTH / 2) |
		c2b(WIDTH / 2, WIDTH / 2 - 1);

    int turn = WIDTH * HEIGHT;
    while (turn--) {
		view(black, white, placable);

		int x, y;
        do {
            printf("x<< ");
            scanf("%d", &x);
            printf("y<< ");
            scanf("%d", &y);

			hand = c2b(--x, --y);
			if (turn % 2) black |= hand;
        	else white |= hand;
        } while (!hand);
    }
}