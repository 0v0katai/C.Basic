/* *****************************************************************************
 * CB_Kana.c -- Font display routine
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

#include "CB.h"
#include "CB_font.h"
#include "CB_fontmini.h"

bool g_ext_asc         = false;
bool g_ext_kana        = false;
bool g_ext_gaiji       = false;
bool g_ext_asc_mini    = false;
bool g_ext_kana_mini   = false;
bool g_ext_gaiji_mini  = false;

unsigned char *p_ext_asc;
unsigned char *p_ext_asc_mini;
unsigned char *p_ext_gaiji;
unsigned char *p_ext_gaiji_mini;
unsigned char *p_ext_kana;
unsigned char *p_ext_kana_mini;

/* Subsidiary function to display an extended and/or external standard character. */
void KPrintCharSub(int px, int py, unsigned char *c, int modify) {
    DISPGRAPH std_char;
    GRAPHDATA std_font;

    /* Single-byte character */
    int sbchar = c[0];
    /* Multi-byte character */
    int mbchar = sbchar << 8 | c[1];

    if (sbchar <= 0x7E)
        /* displays an external ASCII (ANK) character */
        std_font.pBitmap = p_ext_asc + (mbchar-0x20)*8;

    else if ((0xE741 <= mbchar) && (mbchar <= 0xE77E))
        /* displays a character in the range `E741` to `E77E` */
        std_font.pBitmap = font_e7[mbchar-0xE740];
    
    else if (g_ext_gaiji && (0xFF80 <= mbchar) && (mbchar <= 0xFF9F))
        /* displays an external Gaiji character in the range `FF80` to `FF9F` */
        std_font.pBitmap = p_ext_gaiji + (mbchar-0xFF80)*8;
    else if (g_ext_kana && (0xFFA0 <= mbchar) && (mbchar <= 0xFFDF))
        /* displays an external Gaiji character in the range `FF80` to `FF9F` */
        std_font.pBitmap = p_ext_kana + (mbchar-0xFFA0)*8;
    else if (mbchar == 0xFFE0)
        std_font.pBitmap = font_ffe0;
    else if (mbchar == 0xFFE1)
        std_font.pBitmap = font_ffe1;
    else if (mbchar == 0xFFE2)
        std_font.pBitmap = font_ffe2;
    else
        /* for invalid characters */
        std_font.pBitmap = font_unknown;

    std_font.width = 6;
    std_font.height = 8;
    std_char.x = px;
    std_char.y = py;
    std_char.GraphData = std_font;
    std_char.WriteModify = modify;
    std_char.WriteKind = IMB_WRITEKIND_OVER;
    Bdisp_WriteGraph_VRAM(&std_char);
}

void KPrintChar(int px, int py, unsigned char *c) {
    KPrintCharSub(px, py, c, IMB_WRITEMODIFY_NORMAL);
}

void KPrintRevChar(int px, int py, unsigned char *c) {
    KPrintCharSub(px, py, c, IMB_WRITEMODIFY_REVERCE);
}

unsigned char* _char_mini_font_lookup(unsigned char *str, int ext_flag) {
    /* Single-byte character */
    int sbchar = str[0];
    /* Multi-byte character */
    int mbchar = sbchar << 8 | str[1];

    if ((sbchar != 0x7F) && (sbchar <= 0xDF)) {
        if (ext_flag && g_ext_asc_mini && (0x20 <= sbchar) && (sbchar <= 0x7E))
            return p_ext_asc_mini + (sbchar-0x20)*8;
        else
            return font_asc_mini[sbchar];
    }

    else if (mbchar == 0x7F50)
        return font_7f50_mini;
    else if (mbchar == 0x7F53)
        return font_7f53_mini;
    else if (mbchar == 0x7F54)
        return font_7f54_mini;
    else if (mbchar == 0x7FC7)
        return font_7fc7_mini;
    
    else if ((0xE500 <= mbchar) && (mbchar <= 0xE5DF))
        return font_e5_mini[mbchar-0xE500];
    
    else if ((0xE600 <= mbchar) && (mbchar <= 0xE6DF))
        return font_e6_mini[mbchar-0xE600];
    
    else if ((0xE740 <= mbchar) && (mbchar <= 0xE77E))
        return font_e7_mini[mbchar-0xE740];
    
    else if (g_ext_gaiji_mini && (0xFF80 <= mbchar) && (mbchar <= 0xFF9F))
        return p_ext_gaiji_mini + (mbchar-0xFF80)*8;
    
    else if (g_ext_kana_mini && (0xFFA0 <= mbchar) && (mbchar <= 0xFFDF))
        return p_ext_kana_mini + (mbchar-0xFFA0)*8;

    else if (mbchar == 0xFFE0)
        return font_ffe0_mini;

    else if (mbchar == 0xFFE1)
        return font_ffe1_mini;

    else if (mbchar == 0xFFE2)
        return font_ffe2_mini;

    else
        return font_unknown_mini;
}

int KPrintCharMini(int px, int py, unsigned char *str, int mode, int ext_flag) {
    DISPGRAPH mini_char;
    GRAPHDATA mini_font;
    unsigned char *font_data;

    font_data = _char_mini_font_lookup(str, ext_flag);
    mini_font.width = font_data[0];
    if (px + mini_font.width > 128)
        mini_font.width = 127-px;
    mini_font.height = 6;
    mini_font.pBitmap = font_data+2;
    mini_char.x = px;
    mini_char.y = py;
    mini_char.GraphData = mini_font;

    /**
     *  `MINI_` macro   | Value | Write modify  | Write kind
     *  ---             | ---   | ---           | ---
     *  MINI_OVER       | 0x10  | Normal    (1) | Over  (1)
     *  MINI_OR         | 0x11  | Normal    (1) | Or    (2)
     *  MINI_REV        | 0x12  | Reverse   (2) | Over  (1)
     *  MINI_REVOR      | 0x13  | Reverse   (2) | Or    (2)
     */
    mini_char.WriteModify = (mode >= 0x12) + 1;
    mini_char.WriteKind = (mode % 2) + 1;

    if (mini_font.width > 1)
        Bdisp_WriteGraph_VRAM(&mini_char);
    return mini_font.width;
}

//---------------------------------------------------------------------------------------------

void CB_PrintMini(int px, int py, const unsigned char *str, int mode, int ext_flag) {
    unsigned char mod,kind;
    int i;
    int c=(char)*str;
    while ( c ) {
        i=KPrintCharMini( px, py, str++ , mode, ext_flag);
        if ( (c==0x7F)||(c==0xFFFFFFF9)||(c==0xFFFFFFE5)||(c==0xFFFFFFE6)||(c==0xFFFFFFE7)||(c==0xFFFFFFFF) )  str++;
        px+=i;
        if ( px>127 ) break;
        c=(char)*str;
    }
}

int CB_PrintMiniC(int px, int py, const unsigned char *str, int mode, int ext_flag) {
    return KPrintCharMini(px, py, str, mode, ext_flag);
}

int CB_PrintMiniLength( unsigned char *str, int extflag ) {
    return _char_mini_font_lookup(str, extflag)[0];
}

int CB_PrintMiniLengthStr( unsigned char *str, int extflag ){
    int i,length=0;
    int c=(char)*str;
    while ( c ) {
        i=CB_PrintMiniLength( str++, extflag );
        if ( (c==0x7F)||(c==0xFFFFFFF9)||(c==0xFFFFFFE5)||(c==0xFFFFFFE6)||(c==0xFFFFFFE7)||(c==0xFFFFFFFF) )  str++;
        length+=i;
        c=(char)*str;
    }
    return length;
}

// void font_init(
//     unsigned char** font_pointer,
//     int font_count
// ) {
//     int font_size = 8;

//     *font_pointer == NULL;
//     free(*font_pointer);
//     *font_pointer = malloc(font_size * font_count);
//     if (*font_pointer == NULL)
//         Abort();
// }

void ClearExtFontflag() {
    g_ext_asc         = false;
    g_ext_kana        = false;
    g_ext_gaiji       = false;
    g_ext_asc_mini    = false;
    g_ext_kana_mini   = false;
    g_ext_gaiji_mini  = false;
    memcpy((char*)p_ext_asc,              (char*)font_asc,             95*8);
    memcpy((char*)p_ext_asc_mini,         (char*)font_asc_mini+32*8,   95*8);
    // font_init(&p_ext_kana,          64);
    // font_init(&p_ext_kana_mini,     64);
    // font_init(&p_ext_gaiji,         32);
    // font_init(&p_ext_gaiji_mini,    32);
}

void ReadExtFont(){
    LoadExtFontAnk(   3, "", -1 );            // FONTA8L.bmp -> font 6x8     FONTA6M.bmp -> mini font 6x6
    LoadExtFontGaiji( 3, "", -1 );            // FONTG8L.bmp -> font 6x8     FONTG6M.bmp -> mini font 6x6
    LoadExtFontKana(  3, "", -1 );            // FONTK8L.bmp -> font 6x8     FONTK6M.bmp -> mini font 6x6
}

int CB_GetFontSub( char *SRC, char *cstr, int *orgflag, int getmode ) {
    int opcode;
    int c=SRC[ExecPtr];
    if ( c == ')' ) { ExecPtr++; return -1; }
    *orgflag=0; if ( c == '@' ) { ExecPtr++; *orgflag=1; }
    c=CB_IsStr( SRC, ExecPtr );
    if ( c ) {    // string
        CB_GetLocateStr( SRC, cstr, 256-1 );        // String -> buffer    return 
        GetOpcodeLen( cstr, 0, &opcode );
    } else {    // expression
        opcode = CB_EvalInt( SRC );
        if ( getmode == 0 ) {
            if ( opcode == 0 ) { ClearExtFontflag(); return -2; }
            if ( opcode == 1 ) { 
                if (EnableExtFont==0) OkMSGstr2("Please Enable","Extfont by Setup"); else ReadExtFont();
                return -2;
             }
        }
        if ( opcode>0xFF ) { 
            cstr[0]=(opcode>>8) & 0xFF;
            cstr[1]=(opcode) & 0xFF;
            cstr[2]='\0';
        } else {
            cstr[0]=opcode;
            cstr[1]='\0';
        }
    }
    return opcode & 0xFFFF;
}
int CB_GetFont( char *SRC ){    // GetFont(0xFFA0)->Mat C
    int c;
    int x,y,px,py;
    int reg,i;
    int mptr;
    int ElementSize=1;
    unsigned short CharNo;
    int width=6,height=8;
    char vbuf[16*8];
    char *vram=(char*)PictAry[0];
    unsigned char cstr[256];
    int orgflag;

    c = CB_GetFontSub( SRC, (char*)cstr, &orgflag, 1 ) ;
    if ( c == -1 ) goto exit;
    if ( SRC[ExecPtr] == ')' ) ExecPtr++;
    if ( SRC[ExecPtr] == 0x0E ) {  // -> Mat C
            ExecPtr++;
            width=6;
            MatrixOprand( SRC, &reg, &x, &y );
            if ( ErrorNo == NoMatrixArrayERR ) {     // No Matrix Array
                ErrorNo=0;    // error cancel
            }
            DimMatrixSub( reg, 1, width, height,  ElementSize ) ;    // 1bit
            if ( ErrorNo ) return; // error

            memcpy( vbuf, vram, 16*8 );
            if ( cstr==NULL )    CB_PrintC( 1, 1, (unsigned char *)" " );
            else            CB_PrintC_ext( 1, 1, cstr, orgflag==0 );
            x=1;
            y=1;
            i=x;
            for ( py=0; py<8 ; py++) {
                x=i;
                for ( px=0; px<6 ; px++) WriteMatrixInt( reg, x++, y, BdispGetPointVRAM2(px,py) );
                y++;
            }
            memcpy( vram, vbuf, 16*8);
    } else { CB_Error(SyntaxERR);}    // Syntax error
  exit:
    return (g_ext_kana!=0)*4 + (g_ext_gaiji!=0)*2 + (g_ext_asc!=0);
}

int CB_GetFontMini( char *SRC ){    // GetFont(0xFFA0)->Mat C
    int c;
    int x,y,px,py;
    int reg,i;
    int mptr;
    int ElementSize=1;
    unsigned short CharNo;
    int width=6,height=6;
    char vbuf[16*8];
    char *vram=(char*)PictAry[0]+384*24*2;
    unsigned char cstr[256];
    int orgflag;

    c = CB_GetFontSub( SRC, (char*)cstr, &orgflag, 1 ) ;
    if ( c == -1 ) goto exit;
    if ( SRC[ExecPtr] == ')' ) ExecPtr++;
    if ( SRC[ExecPtr] == 0x0E ) {  // -> Mat C
            ExecPtr++;
            width=CB_PrintMiniLength( cstr, (orgflag==0) );
            if ( width<2 ) width=2;
            MatrixOprand( SRC, &reg, &x, &y );
            if ( ErrorNo == NoMatrixArrayERR ) {     // No Matrix Array
                ErrorNo=0;    // error cancel
            }
            DimMatrixSub( reg, 1, width, height, ElementSize ) ;    // 1bit
            if ( ErrorNo ) return; // error
            
            memcpy( vbuf, vram, 16*8 );
            if ( cstr==NULL ) {
                                CB_PrintMiniC(0, 0, (unsigned char *)" ", MINI_OVER, false);
            } else {
                if ( orgflag )    CB_PrintMiniC(0, 0, cstr, MINI_OVER, false);
                else            CB_PrintMiniC(0, 0, cstr, MINI_OVER, true);
            }
            x=1;
            y=1;
            i=x;
            for ( py=0; py<6 ; py++) {
                x=i;
                for ( px=0; px<width ; px++) WriteMatrixInt( reg, x++, y, BdispGetPointVRAM2(px,py) );
                y++;
            }
            memcpy( vram, vbuf, 16*8 );
    } else { CB_Error(SyntaxERR); }    // Syntax error
  exit:
    return (g_ext_kana_mini!=0)*4 + (g_ext_gaiji_mini!=0)*2 + (g_ext_asc_mini!=0);
}


char* CB_SetFontSub( char *SRC, int *reg, int mini ) {
    int CharNo;
    int width,height;
    unsigned char cstr[256];
    int orgflag;
    char *fontptr;

    CharNo=CB_GetFontSub( SRC, (char*)cstr, &orgflag, 0 );
    if ( CharNo == -1 ) { CB_Error(SyntaxERR); return 0; }    // Syntax error
    if ( CharNo == -2 ) { return 0; }    // SetFont 0   SetFont 1
    if ( CharNo < 0xFF ) {
        if ( ( CharNo < 0x20 ) || ( CharNo >= 0x7F ) ) {  CB_Error(OutOfDomainERR); return NULL; } // Out of Domain error
        CharNo&=0xFF;
        CharNo-=0x20;
        if ( mini )    { fontptr=(char*)p_ext_asc_mini+(7+1)*CharNo; g_ext_asc_mini = 1; }
        else        { fontptr=(char*)p_ext_asc+(8)*CharNo;        g_ext_asc = 1; }
    } else {
        CharNo &= 0xFFFF;
        if ((CharNo < 0xFF80) || (CharNo > 0xFFE2)) {
            CB_Error(OutOfDomainERR);
            return NULL;
        }
        CharNo &= 0xFF;
        CharNo -= 0x80;
        if (mini) {
            if (CharNo < 0x20) {
                fontptr = (char*)p_ext_gaiji_mini + 8*CharNo;
                g_ext_gaiji_mini = true;
            } else {
                fontptr = (char*)p_ext_kana_mini + 8*(CharNo-0x20);
                g_ext_kana_mini = true;
            }
        } else {
            if (CharNo < 0x20) {
                fontptr = (char*)p_ext_gaiji + 8*CharNo;
                g_ext_gaiji = true;
            } else {
                fontptr = (char*)p_ext_kana + 8*(CharNo-0x20);
                g_ext_kana = true;
            }
        }
    }
    if ( SRC[ExecPtr] != ',' ) { CB_Error(SyntaxERR); return NULL; }  // Syntax error
    ExecPtr++;
    MatrixOprand( SRC, &(*reg), &width, &height );
    if ( ErrorNo ) return NULL; // error
    if ( MatAry[*reg].ElementSize != 1 ) {  CB_Error(ArraySizeERR); return NULL; } // ArraySizeERR
    return fontptr;
}

void CB_SetFont( char *SRC ){    // SetFont 0xFFA0,Mat C
    int x,y,px,py;
    int reg;
    char *matptr,*fontptr;

    fontptr=CB_SetFontSub( SRC, &reg, 0 );
    if ( ( fontptr==NULL ) || ( ErrorNo ) ) return ;
    if ( EnableExtFont==0 ) return ;
    matptr=(char*)MatAry[reg].Adrs;
    for ( py=0; py<8 ; py++) {
        *(fontptr++)=*(matptr++);
    }
}

void CB_SetFontMini( char *SRC ){    // SetFont 0xFFA0,Mat C
    int x,y,px,py;
    int reg;
    int width,height;
    char *matptr,*fontptr;
    
    fontptr=CB_SetFontSub( SRC, &reg, 1 );
    if ( ( fontptr==NULL ) || ( ErrorNo ) ) return ;
    if ( EnableExtFont==0 ) return ;
    matptr=(char*)MatAry[reg].Adrs;
    width=MatAry[reg].SizeA;
    *(fontptr++)=width;
    *(fontptr++)=0;
    for ( py=0; py<6 ; py++) {
        *(fontptr++)=*(matptr++);
    }
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
int ReadBmpHeader( unsigned char *filebase, int *bfOffBits, int *biWidth, int *biHeight );    // 1:1 bit mono bmp file   24:24bit   0:no bmp
void DecodeBmp2mem( char *buffer2, char *buffer, int width, int height );    //    bmpformat(buffer) -> bmp(buffer2)

void FontCopy( char *font,  int reg,  int mx, int my, int dx, int dy ) {    // buffer[x,y]  -> font  1bit data
    int i,j;
    int x,y;
    int xbyte;
    
    xbyte= (dx+7)/8;
    for ( y=0; y<dy; y++ ) {    //
        for ( x=0; x<dx; x++ ) {
            if ( ReadMatrixInt( reg, x+mx, y+my ) ) {
                font[y*xbyte+(x>>3)] |= 128>>(x&7);        // set 1
            } else {
                font[y*xbyte+(x>>3)] &= ~(128>>(x&7));    // set 0
            }
        }
    }
}
int FontWidth( int reg, int mx, int cx, int cy ) {    // buffer[x,y] 1bit data -> width
    int x,y,my,dx;
    my=cy*8;

    for ( x=1; x<6; x++ ) {
        if ( ReadMatrixInt( reg, x+mx, my ) ) break ; 
    }
    return x;
}

int LoadExtFontKana_sub( char* name, char *font, int line ){        // LFONTK.bmp -> font 18x24
    char * FilePtr;
    int offset,ptr=0,size=0,bit;
    int width,height;
    volatile int i,x,y,dx,dy;
    int reg=Mattmpreg;
    char* matptr;
    char fname[64];
    char buf[32],buf2[32];

    if ( name[0]=='/' ) FilePtr = CB_LoadSub( name, 0, &size, "bmp" ) ;
    else    FilePtr = Load1st2nd( name, "FONT", "bmp" );
    if ( ErrorNo == CantFindFileERR ) ErrorNo=0;     // Cancel CantFindFileERR
    if ( FilePtr == NULL ) return 0;

    bit=ReadBmpHeader( (unsigned char*)FilePtr, &offset, &width, &height );
    if ( ( bit != 1 ) || ( width > 8*16 ) || ( height > 8*line ) ) { return 0; }
    DimMatrixSub( reg, 1, width, height, 0 ) ;    // ElementSize=1  base=0
    if ( ErrorNo ) { return 0; } // error
    matptr=(char*)MatAry[reg].Adrs;
    DecodeBmp2mem( matptr , FilePtr+offset, width, height );    //    bmpformat -> bmp

//    memset( font, 0, line*16*8 );
    i=0;
    for ( y=0; y<line; y++ ) {
        for ( x=0; x<16; x++ ) {
            FontCopy( font+i, reg, x*6,y*8,6,8);
            i+=(8);
        }
    }
    DeleteMatrix( reg );
    return 1;
}

int LoadExtFontKanaMini_sub( char* name, char *font, int line ){            // MFONTK.bmp -> font ?x18
    char * FilePtr;
    int offset,ptr=0,size,bit;
    int width,height;
    volatile int i,x,y,dx,dy,mx;
    int reg=Mattmpreg;
    char* matptr;
    char fname[64];
    char buf[32],buf2[32];

    if ( name[0]=='/' ) FilePtr = CB_LoadSub( name, 0, &size, "bmp" ) ;
    else    FilePtr = Load1st2nd( name, "FONT", "bmp" );
    if ( ErrorNo == CantFindFileERR ) ErrorNo=0;     // Cancel CantFindFileERR
    if ( FilePtr == NULL ) return 0;

    bit=ReadBmpHeader( (unsigned char*)FilePtr, &offset, &width, &height );
    if ( ( bit != 1 ) || ( width > 8*16 ) || ( height > 8*line ) ) { return 0; }
    DimMatrixSub( reg, 1, width, height, 0 ) ;    // ElementSize=1  base=0
    if ( ErrorNo ) { return 0; } // error
    matptr=(char*)MatAry[reg].Adrs;
    DecodeBmp2mem( matptr , FilePtr+offset, width, height );    //    bmpformat -> bmp

    for ( y=0; y<line; y++ ) {
        mx=0;
        for ( x=0; x<16; x++ ) {
            *(font)=4;
            dx=FontWidth( reg, mx, x, y );
            if ( dx<2 ) break;
            if ( dx>6 ) dx=6;
            *(font)=dx;
            FontCopy( font+2, reg, mx, y*8+2, dx, 6);
            mx += dx;
            font += (2+6);
        }
    }
    DeleteMatrix( reg );
    return 1;
}

const char fontfolderFX[][8]={"","/FONT/"};

void LoadExtFontKanafolder( int flag, char* sname, int folder, int no ){        // FONTK8L.bmp -> font 6x8     FONTK6M.bmp -> mini font 6x6
    char sname2[32];
    strcpy( sname2, sname );
    if ( EnableExtFont==0 ) return ;
    if ( sname[0]=='\0' ) {
            if ( ( no < 0 )||( 9 < no ) )    sprintf(sname2 ,"%sFONTK8L",   fontfolderFX[folder] );
            else                            sprintf(sname2 ,"%sFONTK8L%d", fontfolderFX[folder], no );
    }
    if (flag & 1)
        g_ext_kana = LoadExtFontKana_sub(sname2, (char*)p_ext_kana, 4);
    
    if ( sname[0]=='\0' ) {
            if ( ( no < 0 )||( 9 < no ) )    sprintf(sname2 ,"%sFONTK6M",   fontfolderFX[folder] );
            else                            sprintf(sname2 ,"%sFONTK6M%d", fontfolderFX[folder], no);
    }
    if (flag & 2)
        g_ext_kana_mini = LoadExtFontKanaMini_sub(sname2, (char*)p_ext_kana_mini, 4);
}

void LoadExtFontGaijifolder( int flag, char* sname, int folder, int no ){        // FONTG8L.bmp -> font 6x8     FONTG6M.bmp -> mini font 6x6
    char sname2[32];
    strcpy( sname2, sname );
    if ( EnableExtFont==0 ) return ;
    if ( sname[0]=='\0' ) {
            if ( ( no < 0 )||( 9 < no ) )    sprintf(sname2 ,"%sFONTG8L",   fontfolderFX[folder] );
            else                            sprintf(sname2 ,"%sFONTG8L%d", fontfolderFX[folder], no );
    }
    if (flag & 1)
        g_ext_gaiji = LoadExtFontKana_sub(sname2, (char*)p_ext_gaiji, 2);

    if ( sname[0]=='\0' ) {
            if ( ( no < 0 )||( 9 < no ) )    sprintf(sname2 ,"%sFONTG6M",   fontfolderFX[folder] );
            else                            sprintf(sname2 ,"%sFONTG6M%d", fontfolderFX[folder], no);
    }
    if (flag & 2)
        g_ext_gaiji_mini = LoadExtFontKanaMini_sub(sname2, (char*)p_ext_gaiji_mini, 2);
}

void LoadExtFontAnkfolder( int flag, char* sname, int folder, int no ){        // FONTA8L.bmp -> font 6x8     FONTA6M.bmp -> mini font 6x6
    char sname2[32];
    strcpy( sname2, sname );
    if ( EnableExtFont==0 ) return ;
    g_ext_asc = 1;
    g_ext_asc_mini = 1;
    if ( sname[0]=='\0' ) {
            if ( ( no < 0 )||( 9 < no ) )    sprintf(sname2 ,"%sFONTA8L",   fontfolderFX[folder] );
            else                            sprintf(sname2 ,"%sFONTA8L%d", fontfolderFX[folder], no);
    }
    if ( flag & 1 ) g_ext_asc = LoadExtFontKana_sub( sname2, (char*)p_ext_asc, 6 );
    
    if ( sname[0]=='\0' ) {
            if ( ( no < 0 )||( 9 < no ) )    sprintf(sname2 ,"%sFONTA6M",   fontfolderFX[folder] );
            else                            sprintf(sname2 ,"%sFONTA6M%d", fontfolderFX[folder], no);
    }
    if ( flag & 2 ) g_ext_asc_mini = LoadExtFontKanaMini_sub( sname2, (char*)p_ext_asc_mini, 6 );
}

void LoadExtFontKana( int flag, char* sname, int no ){        // LFONTK.bmp -> font 18x24
    LoadExtFontKanafolder(flag, sname, 0, no );
}
void LoadExtFontGaiji( int flag, char* sname, int no ){        // LFONTK.bmp -> font 18x24
    LoadExtFontGaijifolder( flag, sname, 0, no );
}
void LoadExtFontAnk( int flag, char* sname, int no ){        // LFONTK.bmp -> font 18x24
    LoadExtFontAnkfolder( flag, sname, 0, no );
}

//------------------------------------------------------------------------------
void FontCopy2( char *font,  int reg,  int mx, int my, int dx, int dy ) {    // font  1bit data  -> Mat[x,y]
    int i,j;
    int x,y;
    int xbyte;
    
    xbyte= (dx+7)/8;
    for ( y=0; y<dy; y++ ) {    //
        for ( x=0; x<dx; x++ ) {
            WriteMatrixInt( reg, x+mx, y+my , ( font[y*xbyte+(x>>3)] & (128>>(x&7)) ) != 0 );
        }
    }
}

void SaveExtFontKana_sub( char* sname, char *font, int line, int check ){        // LFONTK.bmp -> font 18x24
    char * FilePtr;
    int ptr=0,size=0,bit;
    int width,height;
    volatile int i,x,y;
    int reg=Mattmpreg;
    int bfOffBits = 0x3E;
    int bfSize;
    char bmpbuffer[1024];
    int base=0;
    int ElementSize=1;
    
    width =6*16;
    height=8*line;
    bit=1;
    DimMatrixSub( reg, ElementSize, width, height, base ) ;
    if ( ErrorNo ) return ; // error

    i=0;
    for ( y=0; y<line; y++ ) {
        for ( x=0; x<16; x++ ) {
            FontCopy2( font+i, reg, x*6,y*8,6,8);    // Fontdata -> Mat
            i+=(8);
        }
    }
    
    bfSize = ((((width+7)/8)+3)/4)*4 *height +bfOffBits;    //  1bit
    memset( bmpbuffer, 0x00, 1024 );
    WriteBmpHeader( bmpbuffer, width, height );
    EncodeMat2Bmp( reg , bmpbuffer+bfOffBits , width, height );    // Mat -> bmpbuffer
    CB_SaveSub( sname, bmpbuffer, bfSize, check, "bmp" );
}

void SaveExtFontKanaMini_sub( char* sname, char *font, int line, int check ){            // MFONTK.bmp -> font ?x18
    char * FilePtr;
    int ptr=0,size,bit;
    int width,height;
    volatile int i,x,y,dx,mx;
    int reg=Mattmpreg;
    int bfOffBits = 0x3E;
    int bfSize;
    char bmpbuffer[1024];
    int base=0;
    int ElementSize=1;

    width= 6*17+2;
    height=8*line;
    bit=1;
    DimMatrixSub( reg, ElementSize, width, height, base ) ;
    if ( ErrorNo ) return ; // error

    i=0;
    for ( y=0; y<line; y++ ) {
        mx=0;
        WriteMatrixInt( reg, 0, y*8 , 1 );
        for ( x=0; x<16; x++ ) {
            WriteMatrixInt( reg, mx+1, y*8 , 0 );
            dx = (int)*font;
            if ( dx<2 ) dx=2;
            if ( dx>6 ) dx=6;
            WriteMatrixInt( reg, mx+dx,   y*8 , 1 );
            WriteMatrixInt( reg, mx+dx+1, y*8 , 1 );
            FontCopy2( font+2, reg, mx, y*8+2, dx, 6);    // Fontdata -> Mat
            mx += dx;
            font += (2+6);
        }
    }
    bfSize = ((((width+7)/8)+3)/4)*4 *height +bfOffBits;    //  1bit
    memset( bmpbuffer, 0x00, 1024 );
    WriteBmpHeader( bmpbuffer, width, height );
    EncodeMat2Bmp( reg , bmpbuffer+bfOffBits , width, height );    // Mat -> bmpbuffer
    CB_SaveSub( sname, bmpbuffer, bfSize, check, "bmp" );
}

void SaveExtFontKana( int flag, char* sname, int folder, int no, int check ){        // font 6x8 -> FONTK8L.bmp    font 6x6 -> FONTK6M.bmp
    char sname2[32];
    if ( EnableExtFont==0 ) return ;
    strcpy( sname2, sname );
    if ( sname[0]=='\0' ) {
            if ( ( no < 0 )||( 9 < no ) )    sprintf(sname2 ,"%sFONTK8L",   fontfolderFX[folder] );
            else                            sprintf(sname2 ,"%sFONTK8L%d", fontfolderFX[folder], no );
    }
    if ((flag & 1) && (g_ext_kana))
        SaveExtFontKana_sub(sname2, (char*)p_ext_kana, 5, check);
    
    if ( sname[0]=='\0' ) {
            if ( ( no < 0 )||( 9 < no ) )    sprintf(sname2 ,"%sFONTK6M",   fontfolderFX[folder] );
            else                            sprintf(sname2 ,"%sFONTK6M%d", fontfolderFX[folder], no);
    }
    if ((flag & 2) && (g_ext_kana_mini))
        SaveExtFontKanaMini_sub(sname2, (char*)p_ext_kana_mini, 5, check);
}

void SaveExtFontGaiji( int flag, char* sname, int folder, int no, int check ){        // font 6x8 -> FONTG8L.bmp    font 6x6 -> FONTG6M.bmp
    char sname2[32];
    if ( EnableExtFont==0 ) return ;
    strcpy( sname2, sname );
    if ( sname[0]=='\0' ) {
            if ( ( no < 0 )||( 9 < no ) )    sprintf(sname2 ,"%sFONTG8L",   fontfolderFX[folder] );
            else                            sprintf(sname2 ,"%sFONTG8L%d", fontfolderFX[folder], no );
    }
    if ((flag & 1) && (g_ext_gaiji))
        SaveExtFontKana_sub(sname2, (char*)p_ext_gaiji, 2, check);
    
    if ( sname[0]=='\0' ) {
            if ( ( no < 0 )||( 9 < no ) )    sprintf(sname2 ,"%sFONTG6M",   fontfolderFX[folder] );
            else                            sprintf(sname2 ,"%sFONTG6M%d", fontfolderFX[folder], no);
    }
    if ((flag & 2) && (g_ext_gaiji_mini))
        SaveExtFontKanaMini_sub(sname2, (char*)p_ext_gaiji_mini, 2, check);
}

void SaveExtFontAnk( int flag, char* sname, int folder, int no, int check ){            // font 6x8 -> FONTA8L.bmp    font 6x6 -> FONTA6M.bmp
    char sname2[32];
    if ( EnableExtFont==0 ) return ;
    strcpy( sname2, sname );
    if ( sname[0]=='\0' ) {
            if ( ( no < 0 )||( 9 < no ) )    sprintf(sname2 ,"%sFONTA8L",   fontfolderFX[folder] );
            else                            sprintf(sname2 ,"%sFONTA8L%d", fontfolderFX[folder], no );
    }
    if ( ( flag & 1 ) && ( g_ext_asc ) ) SaveExtFontKana_sub( sname2, (char*)p_ext_asc, 6, check );
    
    if ( sname[0]=='\0' ) {
            if ( ( no < 0 )||( 9 < no ) )    sprintf(sname2 ,"%sFONTA6M",   fontfolderFX[folder] );
            else                            sprintf(sname2 ,"%sFONTA6M%d", fontfolderFX[folder], no);
    }
    if ( ( flag & 2 ) && ( g_ext_asc_mini ) ) SaveExtFontKanaMini_sub( sname2, (char*)p_ext_asc_mini, 6, check );
}
