#ifndef _CONST_H_
#define _CONST_H_

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

#endif