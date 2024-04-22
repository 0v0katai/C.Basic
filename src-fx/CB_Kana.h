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
extern bool g_ext_asc;

/* Indicator of external ASCII mini font bitmap. */
extern bool g_ext_asc_mini;

/* Indicator of external Katakana standard font bitmap. */
extern bool g_ext_kana;

/* Indicator of external Katakana mini font bitmap. */         
extern bool g_ext_kana_mini;

/* Indicator of external Gaiji standard font bitmap. */
extern bool g_ext_gaiji;

/* Indicator of external Gaiji mini font bitmap. */
extern bool g_ext_gaiji_mini;

/* Pointer to the address of ANK standard font data. */
extern unsigned char *p_ext_asc;

/* Pointer to the address of ANK mini font data. */
extern unsigned char *p_ext_asc_mini;

/* Pointer to the address of Katakana and Gaiji standard font data.  */
extern unsigned char *p_ext_kana_gaiji;

/* Pointer to the address of Katakana and Gaiji mini font data. */
extern unsigned char *p_ext_kana_gaiji_mini;

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

/** Main function to clear all external font indicators; and
    restore their corresponding data pointers to the built-in font matrices. */
void ClearExtFontflag();
// int Make_FontFolder();

void LoadExtFontKanafolder(  int flag, char* sname, int folder, int no );        // FONTK8L.bmp -> font 6x8     FONTK6M.bmp -> mini font 6x6
void LoadExtFontGaijifolder( int flag, char* sname, int folder, int no );        // FONTG8L.bmp -> font 6x8     FONTG6M.bmp -> mini font 6x6
void LoadExtFontAnkfolder(   int flag, char* sname, int folder, int no );        // FONTA8L.bmp -> font 6x8     FONTA6M.bmp -> mini font 6x6

void SaveExtFontKana(  int flag, char* sname, int folder, int no, int check );        // font 6x8 -> FONTK8L.bmp    font 6x6 -> FONTK6M.bmp
void SaveExtFontGaiji( int flag, char* sname, int folder, int no, int check );        // font 6x8 -> FONTG8L.bmp    font 6x6 -> FONTG6M.bmp
void SaveExtFontAnk(   int flag, char* sname, int folder, int no, int check );        // font 6x8 -> FONTA8L.bmp    font 6x6 -> FONTA6M.bmp

/* Shortcut function to print an extended/external character in a normal display. */
void KPrintChar( int px, int py, unsigned char *c);

/* Shortcut function to print an extended/external character in a reverse display. */
void KPrintRevChar( int px, int py, unsigned char *c);

/**
 * Subsidiary function to display an extended and/or external character.
 * 
 * @param px x-pixel coordinate of the character
 * @param py y-pixel coordinate of the character
 * @param c hex code of the character (single or multi-byte)
 * @param modify `IMB_WRITEMODIFY` macro (dispbios.h)
 * @return Stores the character bitmap data to VRAM.
 */
void KPrintCharSub( int px, int py, unsigned char *c, int modify );

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