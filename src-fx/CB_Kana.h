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

/* Indicator of external ASCII standard font bitmap. */
extern bool g_external_asc;

/* Indicator of external ASCII mini font bitmap. */
extern bool g_external_asc_mini;

/* Indicator of external Katakana standard font bitmap. */
extern bool g_external_kana;

/* Indicator of external Katakana mini font bitmap. */         
extern bool g_external_kana_mini;

/* Indicator of external Gaiji standard font bitmap. */
extern bool g_external_gaiji;

/* Indicator of external Gaiji mini font bitmap. */
extern bool g_external_gaiji_mini;

/* Pointer to the address of ANK standard font data. */
extern unsigned char *p_external_asc;

/* Pointer to the address of ANK mini font data. */
extern unsigned char *p_external_asc_mini;

/* Pointer to the address of Katakana and Gaiji standard font data.  */
extern unsigned char *p_external_kana_gaiji;

/* Pointer to the address of Katakana and Gaiji mini font data. */
extern unsigned char *p_external_kana_gaiji_mini;

extern const unsigned char font_asc[][8];
extern const unsigned char font_asc_mini[][8];
extern const unsigned char font_80_mini[][8];
extern const unsigned char font_7f50_mini[8];
extern const unsigned char font_7f53_mini[8];
extern const unsigned char font_7f54_mini[8];
extern const unsigned char font_7fc7_mini[8];
extern const unsigned char font_e5_mini[][8];
extern const unsigned char font_e6_mini[][8];
extern const unsigned char font_e7[][8];
extern const unsigned char font_e7_mini[][8];
extern const unsigned char font_kana[][8];
extern const unsigned char font_kana_mini[][8];

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
#define HEXCHAR_f1      0x14
#define HEXCHAR_f2      0x15
#define HEXCHAR_f3      0x16
#define HEXCHAR_f4      0x17
#define HEXCHAR_f5      0x18
#define HEXCHAR_f6      0x19
#define HEXCHAR_HEXA    0x1A
#define HEXCHAR_HEXB    0x1B
#define HEXCHAR_HEXC    0x1C
#define HEXCHAR_HEXD    0x1D
#define HEXCHAR_HEXE    0x1E
#define HEXCHAR_HEXF    0x1F

/* Clears all external font indicators and restores to the original font data. */
void ClearExtFontflag();

// int Make_FontFolder();

/**
 * 
 * 
 * @param flag 
 * @param sname 
 * @param folder 
 * @param no 
 */
void LoadExtFontKanafolder(  int flag, char* sname, int folder, int no );        // FONTK8L.bmp -> font 6x8     FONTK6M.bmp -> mini font 6x6
void LoadExtFontGaijifolder( int flag, char* sname, int folder, int no );        // FONTG8L.bmp -> font 6x8     FONTG6M.bmp -> mini font 6x6
void LoadExtFontAnkfolder(   int flag, char* sname, int folder, int no );        // FONTA8L.bmp -> font 6x8     FONTA6M.bmp -> mini font 6x6

void SaveExtFontKana(  int flag, char* sname, int folder, int no, int check );        // font 6x8 -> FONTK8L.bmp    font 6x6 -> FONTK6M.bmp
void SaveExtFontGaiji( int flag, char* sname, int folder, int no, int check );        // font 6x8 -> FONTG8L.bmp    font 6x6 -> FONTG6M.bmp
void SaveExtFontAnk(   int flag, char* sname, int folder, int no, int check );        // font 6x8 -> FONTA8L.bmp    font 6x6 -> FONTA6M.bmp

/**
 * Shortcut for `KPrintCharSub(px, py, c, IMB_WRITEMODIFY_NORMAL)`.
 * 
 * @param px (0..383)
 * @param py (-24..191)
 * @param c Character hex code
 */
void KPrintChar( int px, int py, unsigned char *c);

/**
 * Shortcut for `KPrintCharSub(px, py, c, IMB_WRITEMODIFY_REVERCE)`.
 * 
 * @param px (0..383)
 * @param py (-24..191)
 * @param c Character hex code
 */
void KPrintRevChar( int px, int py, unsigned char *c);

/**
 * Main function to display a mini character.
 * 
 * @param px (0..383)
 * @param py (-24..191)
 * @param str Character hex code
 * @param mode `MINI` macro (dispbios.h)
 * @return The value of the first index of the font data array.
 */
int KPrintCharMini( int px, int py, unsigned char *str, int mode ) ; // カナ対応 PrintMini

int CB_GetFont( char *SRC );        // DotChar(0xFFA0)->Mat C
int CB_GetFontMini( char *SRC );    // DotChar(0xFFA0)->Mat C
void CB_SetFont( char *SRC );        // SetFont 0xFFA0,Mat C
void CB_SetFontMini( char *SRC );    // SetFont 0xFFA0,Mat C

/* Reads external font bitmaps in the current folder. */
void ReadExtFont();

void LoadExtFontKana(  int flag, char* sname, int no );            // FONTK8L.bmp -> font 6x8     FONTK6M.bmp -> mini font 6x6
void LoadExtFontGaiji( int flag, char* sname, int no );            // FONTG8L.bmp -> font 6x8     FONTG6M.bmp -> mini font 6x6
void LoadExtFontAnk(   int flag, char* sname, int no );            // FONTA8L.bmp -> font 6x8     FONTA6M.bmp -> mini font 6x6