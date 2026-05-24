#include "CB.h"

int CB_GetkeyM() {
    uint8_t key_data[55] = {0};
    int c = max(1, getkey_all(key_data));

    dspflag = 4;
    NewMatListAns(c, 1, 1, 8);      // List Ans[c].b
    for (int i = 0; i < c; i++)
        WriteMatrix(CB_MatListAnsreg, i + 1, 1, key_data[i]);
    return 1;
}

int CB_Getkey() {
    if (Recent_code && keydown(Recent_code)) return Recent_code;

    int code = getkey_lsb();
    Recent_code = code;

    if (code && Getkey_shift) {
        Getkey_shift = 0;
        if (code == KEY_OPTN) {
            Keyboard_ClrBuffer();
            if (IsSH3 == 0) BackLight(2);	// SH4 only
        }
    }
    if (code == KEY_SHIFT) Getkey_shift = 1;
    return code;
}
