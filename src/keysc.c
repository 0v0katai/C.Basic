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

static uint8_t const MPM_KEYMAP[30] = {
    KEY_ON,       KEY_HOME,  KEY_LTAB, KEY_UP,   KEY_RTAB,    KEY_PGUP,
    KEY_SETTINGS, KEY_EXIT,  KEY_LEFT, KEY_OK,   KEY_RIGHT,   KEY_PGDOWN,
    KEY_SHIFT,    KEY_ALPHA, KEY_VARS, KEY_DOWN, KEY_CATALOG, KEY_TOOLS,
    KEY_XTT,      KEY_FRAC,  KEY_SQRT, KEY_POW,  KEY_SQUARE,  KEY_LN,
    KEY_COMMA,    KEY_SIN,   KEY_COS,  KEY_TAN,  KEY_LPAR,    KEY_RPAR
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
        if (MPM && col > 4) {
            for (int i = 0; i < sizeof(MPM_KEYMAP); i++) {
                if (key == MPM_KEYMAP[i]) {
                    row = 9 - i / 6;
                    col = 1 << (6 - i % 6);
                    break;
                }
            }
        }
    }

    return iokbd_row(row) & col;
}

int getkey_value(int lsb, int row) {
    if (lsb == 1 && row == 0) return KEY_AC;
    if (IsSH3 == 2) return SLIM_KEYMAP[72 - lsb * 9 - row];
    if (MPM && row > 4) return MPM_KEYMAP[61 - row * 6 - lsb];
    return lsb * 10 + row;
}

int getkey_all(uint8_t *scan) {
    int count = 0;
    uint8_t row_data[10];
    for (int row = 0; row < 10; row++)
        row_data[row] = iokbd_row(row);
    for (int i = 0; i < 10; i++) {
        while (row_data[i]) {
            int lsb = __builtin_ffs(row_data[i]);
            if (lsb) scan[count++] = getkey_value(lsb, i);
            row_data[i] &= ~(1 << (lsb - 1));
        }
    }
    return count;
}

/* Precedence: row 0 to 9, LSB */
int getkey_lsb() {
    for (int row = 0; row < 10; row++) {
        int c = iokbd_row(row);
        int lsb = __builtin_ffs(c);
        if (lsb == 0) continue;
        return getkey_value(lsb, row);
    }
    return 0;
}
