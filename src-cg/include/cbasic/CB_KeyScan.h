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
//08  SHIFT OPTN VARS MENU ��  ��        08    A44B0008
//07  ALPHA ^2   ^    EXIT ��   ��       07    A44B0007
//06  XTT   log  ln   sin  cos  tan      06    A44B0006
//05  ab/c  F<>D  (    )    ,   ��       05    A44B0005
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

int iokbd_7305( int row );
int GetKey_DisableMenu( int *key ) ;
int GetKey_DisableMenu_pushpop( int *key ) ;
int GetKey_DisableMenuCatalog( int *key ) ;
int GetKey_DisableCatalog( int *key ) ;
int CB_KeyCodeCnvt( unsigned int key ) ;			// CasioBasic Getkey SDK compatible

int CB_Getkey();
int CB_Getkey0();
int CB_Getkey1(int sdkcode);
int CB_Getkey2(int sdkcode);
int CB_GetkeyN( int n, int disableCatalog, int sdkcode ) ;			// CasioBasic Getkey 

int BackLight( int n );			// 0:off  1:on   2:xor
void KeyRecover() ;