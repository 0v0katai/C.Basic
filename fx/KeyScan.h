//
// fx-9860G/II series KeyScan
//
// original source by SuperH-based fx calculators
//
// this modified source is written by sentaro21
//
//
//    06    05   04   03   02   01   00        (SH4A)
//--------------------------------------
//0B                  DIAG     OSUPD     0B    A44B000B
//0A                                     0A    A44B000A
//09  F1    F2   F3   F4   F5   F6       09    A44B0009
//08  SHIFT OPTN VARS MENU Å©  Å™        08    A44B0008
//07  ALPHA ^2   ^    EXIT Å´   Å®       07    A44B0007
//06  XTT   log  ln   sin  cos  tan      06    A44B0006
//05  ab/c  F<>D  (    )    ,   Å®       05    A44B0005
//04  7     8    9    DEL                04    A44B0004
//03  4     5    6    x    div           03    A44B0003
//02  1     2    3    +    -             02    A44B0002
//01  0     .    EXP  (-)  EXE           01    A44B0001
//00                                 AC  00    A44B0000
//--------------------------------------
//    06    05   04   03   02   01   00        (SH4A)

/*
#define KB_1ST_ROW    9
#define KB_2ND_ROW    8
#define KB_3RD_ROW    7
#define KB_4TH_ROW    6
#define KB_5TH_ROW    5
#define KB_6TH_ROW    4
#define KB_7TH_ROW    3
#define KB_8TH_ROW    2
#define KB_9TH_ROW    1
#define KB_0TH_ROW    0

#define KB_F1_ROW     KB_1ST_ROW
#define KB_SHIFT_ROW  KB_2ND_ROW
#define KB_ALPHA_ROW  KB_3RD_ROW
#define KB_XTT_ROW    KB_4TH_ROW
#define KB_ABC_ROW    KB_5TH_ROW
#define KB_7_ROW      KB_6TH_ROW
#define KB_4_ROW      KB_7TH_ROW
#define KB_1_ROW      KB_8TH_ROW
#define KB_0_ROW      KB_9TH_ROW
#define KB_AC_ROW     KB_0TH_ROW

#define KB_1ST_COL    0x40
#define KB_2ND_COL    0x20
#define KB_3RD_COL    0x10
#define KB_4TH_COL    0x08
#define KB_5TH_COL    0x04
#define KB_6TH_COL    0x02
#define KB_7TH_COL    0x01
#define KB_AC_COL     0x00

#define KB_ALPHA_COL  KB_1ST_COL
#define KB_SQUARE_COL KB_2ND_COL
#define KB_POW_COL    KB_3RD_COL
#define KB_EXIT_COL   KB_4TH_COL
#define KB_DOWN_COL   KB_5TH_COL
#define KB_RIGHT_COL  KB_6TH_COL
*/

#define KEYSC_F1 		0x409
#define KEYSC_F2 		0x209
#define KEYSC_F3 		0x109
#define KEYSC_F4 		0x089
#define KEYSC_F5		0x049
#define KEYSC_F6 		0x029

#define KEYSC_SHIFT 	0x408
#define KEYSC_OPTN 		0x208
#define KEYSC_VARS 		0x108
#define KEYSC_MENU 		0x088
#define KEYSC_LEFT 		0x048
#define KEYSC_UP 		0x028

#define KEYSC_ALPHA 	0x407
#define KEYSC_SQUARE 	0x207
#define KEYSC_POW 		0x107
#define KEYSC_EXIT 		0x087
#define KEYSC_DOWN 		0x047
#define KEYSC_RIGHT 	0x027

#define KEYSC_XTT 		0x406
#define KEYSC_LOG 		0x206
#define KEYSC_LN 		0x106
#define KEYSC_SIN 		0x086
#define KEYSC_COS 		0x046
#define KEYSC_TAN 		0x026

#define KEYSC_FRAC 		0x405
#define KEYSC_FD 		0x205
#define KEYSC_LPAR 		0x105
#define KEYSC_RPAR 		0x085
#define KEYSC_COMMA 	0x045
#define KEYSC_STORE 	0x025

#define KEYSC_7 		0x404
#define KEYSC_8 		0x204
#define KEYSC_9 		0x104
#define KEYSC_DEL 		0x084

#define KEYSC_4 		0x403
#define KEYSC_5 		0x203
#define KEYSC_6 		0x103
#define KEYSC_MULT 		0x083
#define KEYSC_DIV 		0x043

#define KEYSC_1 		0x402
#define KEYSC_2 		0x202
#define KEYSC_3 		0x102
#define KEYSC_PLUS 		0x082
#define KEYSC_MINUS 	0x042

#define KEYSC_0 		0x401
#define KEYSC_DP 		0x201
#define KEYSC_EXP 		0x101
#define KEYSC_PMINUS	0x081
#define KEYSC_EXE 		0x041

#define KEYSC_AC 		0x010 // 

int KeyScanDown(int keyscan_code);

int CB_Getkey();
int CB_Getkey1();

int KeyCheckAC();
int KeyCheckEXE();
int KeyCheckEXIT();

