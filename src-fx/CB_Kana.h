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

extern char ExtCharAnkFX;                   /* Indicator of external ANK standard font bitmap.        (0,1) */
extern char ExtCharAnkMiniFX;               /* Indicator of external ANK mini font bitmap.            (0,1) */
extern char ExtCharKanaFX;                  /* Indicator of external Katakana standard font bitmap.   (0,1) */
extern char ExtCharKanaMiniFX;              /* Indicator of external Katakana mini font bitmap.       (0,1) */
extern char ExtCharGaijiFX;                 /* Indicator of external Gaiji standard font bitmap.      (0,1) */
extern char ExtCharGaijiMiniFX;             /* Indicator of external Gaiji mini font bitmap.          (0,1) */

extern unsigned char *ExtAnkFontFX;         /* Pointer to the address of ANK standard font data.               */
extern unsigned char *ExtAnkFontFXmini;     /* Pointer to the address of ANK mini font data.                   */
extern unsigned char *ExtKanaFontFX;        /* Pointer to the address of Katakana & Gaiji standard font data.  */
extern unsigned char *ExtKanaFontFXmini;    /* Pointer to the address of Katakana & Gaiji mini font data.      */

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
// #define HEXCHAR_0X0E    0x0E
#define HEXCHAR_EXP     0x0F
#define HEXCHAR_LTEQ    0x10
#define HEXCHAR_NEQ     0x11
#define HEXCHAR_MTEQ    0x12
#define HEXCHAR_IMPLY   0x13
// #define HEXCHAR_0X14    0x14
// #define HEXCHAR_0X15    0x15
// #define HEXCHAR_0X16    0x16
// #define HEXCHAR_0X17    0x17
// #define HEXCHAR_0X18    0x18
// #define HEXCHAR_0X19    0x19
#define HEXCHAR_HEXA    0x1A
#define HEXCHAR_HEXB    0x1B
#define HEXCHAR_HEXC    0x1C
#define HEXCHAR_HEXD    0x1D
#define HEXCHAR_HEXE    0x1E
#define HEXCHAR_HEXF    0x1F

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

/**
 * Shortcut for `KPrintCharSub(px, py, c, IMB_WRITEMODIFY_NORMAL)`.
 * 
 * @param px (0..383)
 * @param py (-24..191)
 * @param c Character hex code.
 */
void KPrintChar( int px, int py, unsigned char *c);

/**
 * Shortcut for `KPrintCharSub(px, py, c, IMB_WRITEMODIFY_REVERCE)`.
 * 
 * @param px (0..383)
 * @param py (-24..191)
 * @param c Character hex code.
 */
void KPrintRevChar( int px, int py, unsigned char *c);

int KPrintCharMini( int px, int py, unsigned char *str, int mode ) ; // カナ対応 PrintMini

int CB_GetFont( char *SRC );        // DotChar(0xFFA0)->Mat C
int CB_GetFontMini( char *SRC );    // DotChar(0xFFA0)->Mat C
void CB_SetFont( char *SRC );        // SetFont 0xFFA0,Mat C
void CB_SetFontMini( char *SRC );    // SetFont 0xFFA0,Mat C

void ReadExtFont();
