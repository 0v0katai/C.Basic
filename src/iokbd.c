#include "keysc.h"
#include <gint/mpu/pfc.h>

static void iokbd_delay() {
    for (int i = 0; i < 10; i++)
        __asm__("nop");
}

int iokbd_7705(int row) {
    int orig_PBCR = SH7705_PFC.PBCR;
    int orig_PMCR = SH7705_PFC.PMCR;
    int orig_PBDR = SH7705_PFC.PBDR;
    int orig_PMDR = SH7705_PFC.PMDR;

    /* This will enable output (01) on @row, input (10) everywhere else */
    uint16_t ctrl_mask = 0x0003 << ((row & 7) * 2);
    /* Enable output (0) on @row, input (1) everywhere else */
    uint8_t data_mask = ~(1 << (row & 7));

    /* When row < 8, the associated bits are in port B */
    if (row < 8) {
        /* Set @row as output in port B; port M is unused */
        SH7705_PFC.PBCR = 0xaaaa ^ ctrl_mask;
        SH7705_PFC.PMCR = (SH7705_PFC.PMCR & 0xff00) | 0x00aa;
        iokbd_delay();

        /* Set @row to 0, everything else to 1 */
        SH7705_PFC.PBDR = data_mask;
        SH7705_PFC.PMDR = (SH7705_PFC.PMDR & 0xf0) | 0x0f;
        iokbd_delay();
    }
    /* When row >= 8, the associated bits are in port M */
    else {
        /* Set @row as output in port M; port B is unused */
        SH7705_PFC.PBCR = 0xaaaa;
        SH7705_PFC.PMCR = (SH7705_PFC.PMCR & 0xff00) | (0x00aa ^ ctrl_mask);
        iokbd_delay();

        /* Set @row to 0, everything else to 1 */
        SH7705_PFC.PBDR = 0xff;
        SH7705_PFC.PMDR = SH7705_PFC.PMDR & data_mask;
        iokbd_delay();
    }

    /* Now read the input data from the keyboard! */
    uint8_t input = ~SH7705_PFC.PADR;
    iokbd_delay();

    /* Reset the port configuration */
    SH7705_PFC.PBCR = orig_PBCR;
    SH7705_PFC.PMCR = orig_PMCR;
    iokbd_delay();

    /* Now also reset the data registers. This was forgotten from SimLo's
       CheckKeyRow() and blows up everything. */
    SH7705_PFC.PBDR = orig_PBDR;
    SH7705_PFC.PMDR = orig_PMDR;
    iokbd_delay();

    return input;
}

int iokbd_7305(int row) {
    return SH7305_IOKBD.matrix[row ^ 1];
}
