#include "CB_io.h"
#include "keysc.h"

void Keyboard_ClrBuffer();
int BackLight(int n);

int CB_Getkey() {
    if (Recent_code && keydown(Recent_code)) return Recent_code;

    int code = getkey_clz();
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
