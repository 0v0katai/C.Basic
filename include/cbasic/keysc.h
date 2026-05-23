#ifndef CBASIC_KEYSC_H
#define CBASIC_KEYSC_H

#include <gint/defs/types.h>

enum {
    KEY_F1     = 79,
    KEY_F2     = 69,
    KEY_F3     = 59,
    KEY_F4     = 49,
    KEY_F5     = 39,
    KEY_F6     = 29,

    KEY_SHIFT  = 78,
    KEY_OPTN   = 68,
    KEY_VARS   = 58,
    KEY_MENU   = 48,
    KEY_LEFT   = 38,
    KEY_UP     = 28,

    KEY_ALPHA  = 77,
    KEY_SQUARE = 67,
    KEY_POW    = 57,
    KEY_EXIT   = 47,
    KEY_DOWN   = 37,
    KEY_RIGHT  = 27,

    KEY_XTT    = 76,
    KEY_LOG    = 66,
    KEY_LN     = 56,
    KEY_SIN    = 46,
    KEY_COS    = 36,
    KEY_TAN    = 26,

    KEY_FRAC   = 75,
    KEY_FD     = 65,
    KEY_LPAR   = 55,
    KEY_RPAR   = 45,
    KEY_COMMA  = 35,
    KEY_STORE  = 25,

    KEY_7      = 74,
    KEY_8      = 64,
    KEY_9      = 54,
    KEY_DEL    = 44,

    KEY_4      = 73,
    KEY_5      = 63,
    KEY_6      = 53,
    KEY_MULT   = 43,
    KEY_DIV    = 33,

    KEY_1      = 72,
    KEY_2      = 62,
    KEY_3      = 52,
    KEY_PLUS   = 42,
    KEY_MINUS  = 32,

    KEY_0      = 71,
    KEY_DP     = 61,
    KEY_EXP    = 51,
    KEY_PMINUS = 41,
    KEY_EXE    = 31,
    
    /* 3.0 TODO: change to 10 */
    KEY_AC     = 34,

    /* fx-9860G Slim only */
    KEY_LIGHT  = 21,
    KEY_HELP   = 11,

};

typedef volatile struct {
    array_union(uint8_t, matrix,
        uint8_t row1, row0;
        uint8_t row3, row2;
        uint8_t row5, row4;
        uint8_t row7, row6;
        uint8_t row9, row8;
    );
} GPACKED(2) SH7305_iokbd_t;

#define SH7305_IOKBD (*((SH7305_iokbd_t *)0xa44b0000))

int iokbd_7705(int row);
int iokbd_7305(int row);
extern int (* iokbd_row)(int);

int keydown(int key);
int getkey_clz();

extern uint8_t Recent_code;
extern char Getkey_shift;

#endif /* CBASIC_KEYSC_H */
