/* *****************************************************************************
 * CB_Kana.h -- Header for Japanese font display routine
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

extern char ExtCharAnkFX;                   /* Indicator of Ank standard font. (0,1)        */
extern char ExtCharKanaFX;                  /* Indicator of Katakana standard font. (0,1)   */
extern char ExtCharGaijiFX;                 /* Indicator of Gaiji standard font. (0,1)      */
extern char ExtCharAnkMiniFX;               /* Indicator of Ank mini font. (0,1)            */
extern char ExtCharKanaMiniFX;              /* Indicator of Katakana mini font. (0,1)       */
extern char ExtCharGaijiMiniFX;             /* Indicator of Gaiji mini font. (0,1)          */

extern unsigned char *ExtAnkFontFX;         /* Ank standard font character set backup? */
extern unsigned char *ExtAnkFontFXmini;     /* Ank mini font character set backup? */
extern unsigned char *ExtKanaFontFX;        /* Katakana and Gaiji standard font character sets backup? */
extern unsigned char *ExtKanaFontFXmini;    /* Katakana and Gaiji mini font character sets backup? */

extern const unsigned char Font00[][8];
extern const unsigned char KanaFont[][8];
extern const unsigned char Fontmini[][8];
extern const unsigned char KanaFontmini[][8];

void ClearExtFontflag();
int Make_FontFoloder();
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
