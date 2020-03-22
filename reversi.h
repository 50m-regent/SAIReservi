#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<unistd.h>


#define SEARCH_DEPTH 3
#define EPOCH_SIZE 1000
#define BIAS_MIN -32
#define BIAS_MAX 32


// mask for searching
#define LMASK 0xfefefefefefefefeul // Left
#define RMASK 0x7f7f7f7f7f7f7f7ful // Right
#define HMASK LMASK & RMASK        // Horizontal
#define TMASK 0xffffffffffffff00ul // Top
#define BMASK 0x00fffffffffffffful // Bottom
#define VMASK TMASK & BMASK        // Vertical


// 探索方向(ビットシフト数)
const int dir[] = {
    -1,
    1,
    1 - WIDTH,
    WIDTH - 1,
    -WIDTH,
    WIDTH,
    -1 - WIDTH,
    WIDTH + 1
};

// パックマンみたいにならないように端は消す
const unsigned long mask[] = {
    LMASK,
    RMASK,
    RMASK & TMASK,
    LMASK & BMASK,
    TMASK,
    BMASK,
    LMASK & TMASK,
    RMASK & BMASK
};

/*
 * @brief Counts standing bit
 * @param num: 数える対象
 * @return number of standing bit
*/
int countb(unsigned long num);

/*
 * @brief Prints bitboard
 * @param num: print対象
*/
void printb(unsigned long num);

/*
 * @brief View game status
 * @params black:    black's bitboard
 *         white:    white's bitboard
 *         placable: bitboard representing placable cells
*/
void view(Game game);

/*
 * @brief Change coordinate to bitboard
 * @params x: x coordinate (A - H)
 *         y: y coordinate (1 - 8)
 * @return bitboard
*/
unsigned long c2b(
    int x,
    int y
);

/*
 * @brief Get placable cells
 * @params p: player's bitboard
 *         o: opponent's bitboard
 * @return placable cells in bitboard
*/
void set_placable(Game *game);

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
);

/*
 * @brief Flipping pieces
 * @params hand: input cell
 *         *p:   pointer of player bitboard
 *         *o:   pointer of opponent bitboard
 * @return flipped place in bitboard
*/
void flip(Game *game);

/*
 * @brief Called when the game is finished.
 * @params black: black's bitboard
 *         white: white's bitboard
 * @return winner 1: black, -1: white, 0: draw
*/
int end(Game game);

/*
 * @brief Change turn
 * @params *game game data
*/
void change(Game *game);

/*
 * @brief Preparing game data
 * @return game data
*/
Game init();

/*
 * @brief Moving to next turn
 * @params *game: game data pointer
 *         b:     player1
 *         w:     player2
 * @return flag 0: game continues, 1: game ends
*/
int progress(Game *game);