#include<stdio.h>

#define WIDTH 8
#define HEIGHT 7
#define BLACK 'o'
#define WHITE 'x'
#define PLACABLE '.'
#define NONE ' '

unsigned long c2b(int x, int y) {
    if (x < 0 || x > WIDTH - 1 || y < 0 || y > HEIGHT - 1) return 0;
    else return 1ul << (y * WIDTH + x);
}

int count(unsigned long bitset) {
	bitset = (bitset >> 1  & 0x5555555555555555ul) + bitset & 0x5555555555555555ul;
	bitset = (bitset >> 2  & 0x3333333333333333ul) + bitset & 0x3333333333333333ul;
	bitset = (bitset >> 4  & 0x0f0f0f0f0f0f0f0ful) + bitset & 0x0f0f0f0f0f0f0f0ful;
	bitset = (bitset >> 8  & 0x00ff00ff00ff00fful) + bitset & 0x00ff00ff00ff00fful;
	bitset = (bitset >> 16 & 0x0000ffff0000fffful) + bitset & 0x0000ffff0000fffful;
	return (bitset >> 32) + bitset & 0x00000000fffffffful;
}

void view(unsigned long black, unsigned long white, unsigned long placable) {
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
	printf("BLACK: %d, WHITE: %d\n", count(black), count(white));
}

int main() {
	unsigned long black, white, placable = 0, hand;
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