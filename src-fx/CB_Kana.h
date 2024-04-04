/* *****************************************************************************
 * CB_Kana.h -- Header for font display routine
 * Copyright (C) 2015-2024 Sentaro21 <sentaro21@pm.matrix.jp>
 *
 * This file is part of C.Basic.
 * C.Basic is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2.0 of the License,
 * or (at your option) any later version.
 *
 * C.Basic is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with C.Basic; if not, see <https://www.gnu.org/licenses/>.
 * ************************************************************************** */

/* ANK means "Alpha, Numeric and Katakana" in JIS encoding system. */

extern char ExtCharAnkFX;                   /* Indicator of ANK standard font. (0,1)        */
extern char ExtCharKanaFX;                  /* Indicator of Katakana standard font. (0,1)   */
extern char ExtCharGaijiFX;                 /* Indicator of Gaiji standard font. (0,1)      */
extern char ExtCharAnkMiniFX;               /* Indicator of ANK mini font. (0,1)            */
extern char ExtCharKanaMiniFX;              /* Indicator of Katakana mini font. (0,1)       */
extern char ExtCharGaijiMiniFX;             /* Indicator of Gaiji mini font. (0,1)          */

extern unsigned char *ExtAnkFontFX;         /* Pointer to the address of ANK standard font data. */
extern unsigned char *ExtAnkFontFXmini;     /* Pointer to the address of ANK mini font data.  */
extern unsigned char *ExtKanaFontFX;        /* Pointer to the address of Katakana standard font data. */
extern unsigned char *ExtKanaFontFXmini;    /* Pointer to the address of Katakana mini font data. */

extern const unsigned char Font00[][8];
extern const unsigned char Fontmini[][8];
extern const unsigned char Fontmini80[][8];
extern const unsigned char Fontmini7F50[8];
extern const unsigned char Fontmini7F53[8];
extern const unsigned char Fontmini7F54[8];
extern const unsigned char Fontmini7FC7[8];
extern const unsigned char FontminiE5[][8];
extern const unsigned char FontminiE6[][8];
extern const unsigned char FontE7[][8];
extern const unsigned char FontminiE7[][8];
extern const unsigned char KanaFont[][8];
extern const unsigned char KanaFontmini[][8];

#define HEXCHAR_NULL    0x00
#define HEXCHAR_FEMTO   0x01
#define HEXCHAR_PICO    0x02
#define HEXCHAR_NANO    0x03
#define HEXCHAR_MICRO   0x04
#define HEXCHAR_MILLI   0x05
#define HEXCHAR_KILO    0x06
#define HEXCHAR_MEGA    0x07
#define HEXCHAR_GIGA    0x08
#define HEXCHAR_TERA    0x09
#define HEXCHAR_PETA    0x0A
#define HEXCHAR_EXA     0x0B
#define HEXCHAR_DISP    0x0C
#define HEXCHAR_CR      0x0D
#define HEXCHAR_0X0E    0x0E
#define HEXCHAR_EXP     0x0F
#define HEXCHAR_LTEQ    0x10
#define HEXCHAR_NEQ     0x11
#define HEXCHAR_MTEQ    0x12
#define HEXCHAR_IMPLY   0x13
#define HEXCHAR_0X14    0x14
#define HEXCHAR_0X15    0x15
#define HEXCHAR_0X16    0x16
#define HEXCHAR_0X17    0x17
#define HEXCHAR_0X18    0x18
#define HEXCHAR_0X19    0x19
#define HEXCHAR_HEXA    0x1A
#define HEXCHAR_HEXB    0x1B
#define HEXCHAR_HEXC    0x1C
#define HEXCHAR_HEXD    0x1D
#define HEXCHAR_HEXE    0x1E
#define HEXCHAR_HEXF    0x1F
#define HEXCHAR_SPACE   0x20
#define HEXCHAR_EXCL    0x21
#define HEXCHAR_QUOTE   0x22
#define HEXCHAR_HASH    0x23
#define HEXCHAR_$       0x24
#define HEXCHAR_PER     0x25
#define HEXCHAR_CAND    0x26
#define HEXCHAR_COMME   0x27
#define HEXCHAR_LRBRC   0x28
#define HEXCHAR_RRBRC   0x29
#define HEXCHAR_ASTER   0x2A
#define HEXCHAR_PLUS    0x2B
#define HEXCHAR_COMMA   0x2C
#define HEXCHAR_MINUS   0x2D
#define HEXCHAR_DOT     0x2E
#define HEXCHAR_SLSH    0x2F
#define HEXCHAR_0       0x30
#define HEXCHAR_1       0x31
#define HEXCHAR_2       0x32
#define HEXCHAR_3       0x33
#define HEXCHAR_4       0x34
#define HEXCHAR_5       0x35
#define HEXCHAR_6       0x36
#define HEXCHAR_7       0x37
#define HEXCHAR_8       0x38
#define HEXCHAR_9       0x39
#define HEXCHAR_COLN    0x3A
#define HEXCHAR_SCOLN   0x3B
#define HEXCHAR_LT      0x3C
#define HEXCHAR_EQ      0x3D
#define HEXCHAR_MT      0x3E
#define HEXCHAR_QUERY   0x3F
#define HEXCHAR_AT      0x40
#define HEXCHAR_A       0x41
#define HEXCHAR_B       0x42
#define HEXCHAR_C       0x43
#define HEXCHAR_D       0x44
#define HEXCHAR_E       0x45
#define HEXCHAR_F       0x46
#define HEXCHAR_G       0x47
#define HEXCHAR_H       0x48
#define HEXCHAR_I       0x49
#define HEXCHAR_J       0x4A
#define HEXCHAR_K       0x4B
#define HEXCHAR_L       0x4C
#define HEXCHAR_M       0x4D
#define HEXCHAR_N       0x4E
#define HEXCHAR_O       0x4F
#define HEXCHAR_P       0x50
#define HEXCHAR_Q       0x51
#define HEXCHAR_R       0x52
#define HEXCHAR_S       0x53
#define HEXCHAR_T       0x54
#define HEXCHAR_U       0x55
#define HEXCHAR_V       0x56
#define HEXCHAR_W       0x57
#define HEXCHAR_X       0x58
#define HEXCHAR_Y       0x59
#define HEXCHAR_Z       0x5A
#define HEXCHAR_LSBRC   0x5B
#define HEXCHAR_BSLSH   0x5C
#define HEXCHAR_RSBRC   0x5D
#define HEXCHAR_CARET   0x5E
#define HEXCHAR__       0x5F
#define HEXCHAR_GRAVE   0x60
#define HEXCHAR_a       0x61
#define HEXCHAR_b       0x62
#define HEXCHAR_c       0x63
#define HEXCHAR_d       0x64
#define HEXCHAR_e       0x65
#define HEXCHAR_f       0x66
#define HEXCHAR_g       0x67
#define HEXCHAR_h       0x68
#define HEXCHAR_i       0x69
#define HEXCHAR_j       0x6A
#define HEXCHAR_k       0x6B
#define HEXCHAR_l       0x6C
#define HEXCHAR_m       0x6D
#define HEXCHAR_n       0x6E
#define HEXCHAR_o       0x6F
#define HEXCHAR_p       0x70
#define HEXCHAR_q       0x71
#define HEXCHAR_r       0x72
#define HEXCHAR_s       0x73
#define HEXCHAR_t       0x74
#define HEXCHAR_u       0x75
#define HEXCHAR_v       0x76
#define HEXCHAR_w       0x77
#define HEXCHAR_x       0x78
#define HEXCHAR_y       0x79
#define HEXCHAR_z       0x7A
#define HEXCHAR_LCBRC   0x7B
#define HEXCHAR_BSLSH   0x7C
#define HEXCHAR_BAR     0x7D
#define HEXCHAR_TILDE   0x7E

/* 
   
   */
void ClearExtFontflag();

// int Make_FontFolder();

void LoadExtFontKanafolder(  int flag, char* sname, int folder, int no );        // FONTK8L.bmp -> font 6x8     FONTK6M.bmp -> mini font 6x6
void LoadExtFontGaijifolder( int flag, char* sname, int folder, int no );        // FONTG8L.bmp -> font 6x8     FONTG6M.bmp -> mini font 6x6
void LoadExtFontAnkfolder(   int flag, char* sname, int folder, int no );        // FONTA8L.bmp -> font 6x8     FONTA6M.bmp -> mini font 6x6
void LoadExtFontKana(  int flag, char* sname, int no );            // FONTK8L.bmp -> font 6x8     FONTK6M.bmp -> mini font 6x6
void LoadExtFontGaiji( int flag, char* sname, int no );            // FONTG8L.bmp -> font 6x8     FONTG6M.bmp -> mini font 6x6
void LoadExtFontAnk(   int flag, char* sname, int no );            // FONTA8L.bmp -> font 6x8     FONTA6M.bmp -> mini font 6x6

void SaveExtFontKana(  int flag, char* sname, int folder, int no, int check );        // font 6x8 -> FONTK8L.bmp    font 6x6 -> FONTK6M.bmp
void SaveExtFontGaiji( int flag, char* sname, int folder, int no, int check );        // font 6x8 -> FONTG8L.bmp    font 6x6 -> FONTG6M.bmp
void SaveExtFontAnk(   int flag, char* sname, int folder, int no, int check );        // font 6x8 -> FONTA8L.bmp    font 6x6 -> FONTA6M.bmp

void KPrintChar( int px, int py, unsigned char *c) ; //カナ一文字表示
void KPrintRevChar( int px, int py, unsigned char *c) ; //カナ一文字表示

int KPrintCharMini( int px, int py, unsigned char *str, int mode ) ; // カナ対応 PrintMini

int CB_GetFont( char *SRC );        // DotChar(0xFFA0)->Mat C
int CB_GetFontMini( char *SRC );    // DotChar(0xFFA0)->Mat C
void CB_SetFont( char *SRC );        // SetFont 0xFFA0,Mat C
void CB_SetFontMini( char *SRC );    // SetFont 0xFFA0,Mat C

void ReadExtFont();
