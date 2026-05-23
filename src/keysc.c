#include "CB.h"

int (*iokbd_row)(int) = iokbd_7305;

static uint8_t const SLIM_KEYMAP[54] = {
    KEY_MENU,  KEY_F1,     KEY_F2,    KEY_F3,    KEY_F4,  KEY_F5,  KEY_F6,   KEY_EXIT,   0,
    KEY_XTT,   KEY_LOG,    KEY_LN,    KEY_SIN,   KEY_COS, KEY_TAN, KEY_FRAC, KEY_FD,     KEY_LPAR,
    KEY_SHIFT, KEY_SQUARE, KEY_COMMA, KEY_STORE, KEY_7,   KEY_8,   KEY_9,    KEY_DEL,    KEY_RPAR,
    KEY_ALPHA, KEY_POW,    KEY_OPTN,  KEY_VARS,  KEY_4,   KEY_5,   KEY_6,    KEY_MULT,   KEY_DIV,
    0,         KEY_LEFT,   KEY_DOWN,  KEY_HELP,  KEY_1,   KEY_2,   KEY_3,    KEY_PLUS,   KEY_EXE,
    0,         KEY_UP,     KEY_RIGHT, KEY_LIGHT, KEY_0,   KEY_DP,  KEY_EXP,  KEY_PMINUS, KEY_MINUS
};

int keydown_ac() {
    int result = 0;
    int n = Waitcount;
    if (n <= 0) return iokbd_row(0) == 0x01;
    if (n > 1) n *= BREAKCOUNT;
    if (IsSH3 == 0) n *= 5; /* SH4 adjust */
    while (n) {
        result = iokbd_row(0) == 0x01;
        if (result) break;
        n--;
    }
    return result;
}

int keydown(int key) {
    if (key == KEY_AC) return keydown_ac();

    int row = 0, col = 0;
    if (IsSH3 == 2) {
        for (int i = 0; i < sizeof(SLIM_KEYMAP); i++) {
            if (key == SLIM_KEYMAP[i]) {
                row = 9 - i % 9;
                col = 1 << (6 - i / 9);
                break;
            }
        }
    } else {
        row = key % 10;
        col = 1 << (key / 10) >> 1;
    }

    return iokbd_row(row) & col;
}

/* Precedence: row 0 to 9, high-to-low bit */
int getkey_clz() {
    for (int row = 0; row < 10; row++) {
        int c = iokbd_row(row);
        if (c) {
            if (c == 0x01 && row == 0) return KEY_AC;
            int bit_pos = 32 - __builtin_clz(c);
            return IsSH3 == 2 ? SLIM_KEYMAP[72 - bit_pos * 9 - row] : bit_pos * 10 + row;
        }
    }

    return 0;
}
