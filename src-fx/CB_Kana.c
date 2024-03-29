/* *****************************************************************************
 * CB_Kana.c -- C.Basic Japanese font display routine
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

/* Initialization */

char ExtCharAnkFX=0;
char ExtCharKanaFX=0;
char ExtCharGaijiFX=0;
char ExtCharAnkMiniFX=0;
char ExtCharKanaMiniFX=0;
char ExtCharGaijiMiniFX=0;

unsigned char *ExtAnkFontFX;            // Ext Ank font
unsigned char *ExtAnkFontFXmini;        // Ext Ank font mini
unsigned char *ExtKanaFontFX;           // Ext Kana & Gaiji font
unsigned char *ExtKanaFontFXmini;       // Ext Kana & Gaiji font mini

/**
 * Subsidiary function to display a Katakana character.
 * 
 * @param px (0..383)
 * @param py (-24..191)
 * @param c Extended character.
 * @param modify `WRITEMODIFY` modifier.
 */
void KPrintCharSub( int px, int py, unsigned char *c, int modify ) {
    DISPGRAPH kfont;
    GRAPHDATA kfont_info;
    int a,b;

    b= (*c);
    a= (b<<8) + c[1];
     if ( ( ( ExtCharKanaFX  )&&( 0xFFA0 <= a )&&( a <= 0xFFEF ) ) ||
         ( ( ExtCharGaijiFX )&&( 0xFF80 <= a )&&( a <= 0xFF9F ) ) ||
         ( ( ExtCharAnkFX   )&&( 0x20 <= b   )&&( b < 0x7F    ) ) ) {
        if ( ( 0x20 <= b )&&( b < 0x7F ) ) {
            kfont_info.pBitmap = ExtAnkFontFX + (b-0x20)*8;
        } else {
            a &= 0xFF;
            a -= 0x80;
            if ( a>=0x70 ) goto err ;
            kfont_info.pBitmap = ExtKanaFontFX + a*8;
        }
    } else {
        if ( ( 0xE741 <= a )&&( a <= 0xE77E ) ) {
            a &= 0xFF;
            a -= 0x40;
            kfont_info.pBitmap = FontE7[a];
        } else 
        if ( ( 0xFF80 <= a )&&( a <= 0xFFE2 ) ) {
            a &= 0xFF;
            a -= 0x80;
            kfont_info.pBitmap = KanaFont[a];
        } else {
          err:
            locate (px/6+1,py/8+1);
            if ( modify == IMB_WRITEMODIFY_NORMAL ) PrintC( (unsigned char*)"@" );
            else                                    PrintRevC( (unsigned char*)"@" );
            return ;
        }
    }
    kfont_info.width = 6;
    kfont_info.height = 8;
    kfont.x = px;
    kfont.y = py;
    kfont.GraphData = kfont_info;
    kfont.WriteModify = modify;
    kfont.WriteKind = IMB_WRITEKIND_OVER;
    Bdisp_WriteGraph_VRAM(&kfont);
}

/**
 * Shortcut for `KPrintCharSub(px, py, c, IMB_WRITEMODIFY_NORMAL)`.
 * 
 * @param px (0..383)
 * @param py (-24..191)
 * @param c Extended character.
 */
void KPrintChar( int px, int py, unsigned char *c) {
    KPrintCharSub( px, py, c, IMB_WRITEMODIFY_NORMAL);
}

/**
 * Shortcut for `KPrintCharSub(px, py, c, IMB_WRITEMODIFY_REVERCE)`.
 * 
 * @param px (0..383)
 * @param py (-24..191)
 * @param c Extended character.
 */
void KPrintRevChar( int px, int py, unsigned char *c) {
    KPrintCharSub( px, py, c, IMB_WRITEMODIFY_REVERCE);
}

/// @brief ni
/// @param px
/// @param py 
/// @param str 
/// @param mode 
/// @return test
int KPrintCharMini( int px, int py, unsigned char *str, int mode ) { // カナ対応 PrintMini
    DISPGRAPH kfont;
    GRAPHDATA kfont_info;
    unsigned char *font;
    int a,b,c;
    int extflag = mode & 0xFF00;
    mode &= 0xFF;
    font=Fontmini[0];
    a= str[0];
     if ( a < 0x7F ) {
        if ( ( extflag ) && ( ExtCharAnkMiniFX ) && ( 0x20 <= a ) ) font= ExtAnkFontFXmini + (a-0x20)*8;
        else font=Fontmini[a];
     } else 
    if ( a == 0xFF )  {
        c = str[1];
        if ( ( 0xA0 <= c ) && ( c <= 0xEF ) ) {
            if ( ( extflag ) && ( ExtCharKanaMiniFX ) ) font= ExtKanaFontFXmini + (c-0x80)*8;
            else font=KanaFontmini[c-0x80];
        } else
        if ( ( 0x80 <= c ) && ( c <= 0x9F ) ) {
            if ( ( extflag ) && ( ExtCharGaijiMiniFX ) ) font= ExtKanaFontFXmini + (c-0x80)*8;
            else  font=KanaFontmini[c-0x80];
        } else goto err;
    } else
    if ( a == 0x7F )  {
        c = str[1];
        if ( c == 0x50 ) font=Fontmini7F50;
        else
        if ( c == 0x53 ) font=Fontmini7F53;
        else
        if ( c == 0x54 ) font=Fontmini7F54;
        else
        if ( c == 0xC7 ) font=Fontmini7FC7;
        else goto err;
    } else
    if ( a == 0xE5 )  {
        c = str[1];
        if ( c <= 0xDF ) font=FontminiE5[c];
        else goto err;
    } else
    if ( a == 0xE6 )  {
        c = str[1];
        if ( c <= 0xDF ) font=FontminiE6[c];
        else goto err;
    } else
    if ( a == 0xE7 )  {
        c = str[1];
        if ( ( 0x40 <= c ) && ( c <= 0x7E ) ) font=FontminiE7[c-0x40];
        else goto err;
    } else
    if ( ( 0x80 <= a ) && ( a <= 0xDF ) ) font=Fontmini80[a-0x80];
    else {
      err:
        font=Fontmini['@'];
    }
    
    kfont_info.width = font[0];
    if ( mode==0x21 ) goto exit;
    switch ( mode ) {
        case MINI_OVER:
            kfont.WriteModify = IMB_WRITEMODIFY_NORMAL;
            kfont.WriteKind = IMB_WRITEKIND_OVER;
            break;
        case MINI_REV:
            kfont.WriteModify = IMB_WRITEMODIFY_REVERCE;
            kfont.WriteKind = IMB_WRITEKIND_OVER;
            break;
        case MINI_OR:
            kfont.WriteModify = IMB_WRITEMODIFY_NORMAL;
            kfont.WriteKind = IMB_WRITEKIND_OR;
            break;
        case MINI_REVOR:
            kfont.WriteModify = IMB_WRITEMODIFY_REVERCE;
            kfont.WriteKind = IMB_WRITEKIND_OR;
            break;
    }
    kfont_info.height = 6;
    kfont_info.pBitmap = font+2;
    kfont.x = px;
    kfont.y = py;
    kfont.GraphData = kfont_info;
    if ( px+kfont_info.width > 128 ) kfont_info.width = 127-px;
    if ( kfont_info.width > 1 ) Bdisp_WriteGraph_VRAM(&kfont);
  exit:
    return kfont_info.width;
}

//---------------------------------------------------------------------------------------------

void CB_PrintMini( int px, int py, const unsigned char *str, int mode){
    unsigned char mod,kind;
    int i;
    int c=(char)*str;
    while ( c ) {
        i=KPrintCharMini( px, py, str++ , mode );
        if ( (c==0x7F)||(c==0xFFFFFFF9)||(c==0xFFFFFFE5)||(c==0xFFFFFFE6)||(c==0xFFFFFFE7)||(c==0xFFFFFFFF) )  str++;
        px+=i;
        if ( px>127 ) break;
        c=(char)*str;
    }
}
int CB_PrintMiniC( int px, int py, const unsigned char *str, int mode){
    return KPrintCharMini( px, py, str , mode );
}

int CB_PrintMiniLength( unsigned char *str, int extflag ){
    return KPrintCharMini( 0, 0, str , 0x21+ extflag*0x100 );
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

//---------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------
void ClearExtFontflag() {
    ExtCharAnkFX=0;                // 0:Normal     1:Ext Ank  font FX
    ExtCharKanaFX=0;            // 0:Normal     1:Ext Kana  font FX
    ExtCharGaijiFX=0;            // 0:Normal     1:Ext Gaiji font FX
    ExtCharAnkMiniFX=0;            // 0:Normal     1:Ext Ank  font FX
    ExtCharKanaMiniFX=0;        // 0:Normal     1:Ext Kana  font FX
    ExtCharGaijiMiniFX=0;        // 0:Normal     1:Ext Gaiji font FX
    memcpy( (char*)ExtAnkFontFX,     (char*)Font00  +32*8,  96*8 );        // Ank Font copy   FX font 
    memcpy( (char*)ExtAnkFontFXmini, (char*)Fontmini+32*8,  96*8 );        // Ank mini copy   FX font 
    memcpy( (char*)ExtKanaFontFX,    (char*)KanaFont    ,  112*8 );        // Ank Font copy   FX font 
    memcpy( (char*)ExtKanaFontFXmini,(char*)KanaFontmini,  112*8 );        // Ank mini copy   FX font 
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
    return (ExtCharKanaFX!=0)*4 + (ExtCharGaijiFX!=0)*2 + (ExtCharAnkFX!=0);
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
                                CB_PrintMiniC( 0, 0, (unsigned char *)" ", MINI_OVER );
            } else {
                if ( orgflag )    CB_PrintMiniC( 0, 0, cstr, MINI_OVER );
                else            CB_PrintMiniC( 0, 0, cstr, MINI_OVER | 0x100 );
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
    return (ExtCharKanaMiniFX!=0)*4 + (ExtCharGaijiMiniFX!=0)*2 + (ExtCharAnkMiniFX!=0);
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
        if ( mini )    { fontptr=(char*)ExtAnkFontFXmini+(7+1)*CharNo; ExtCharAnkMiniFX = 1; }
        else        { fontptr=(char*)ExtAnkFontFX+(8)*CharNo;        ExtCharAnkFX = 1; }
    } else {
        CharNo&=0xFFFF;
        if ( ( 0xFF80 > CharNo ) || ( 0xFFE3 < CharNo ) ) {  CB_Error(OutOfDomainERR); return NULL; } // Out of Domain error
        CharNo&=0xFF;
        CharNo-=0x80;
        if ( mini )    {
            fontptr=(char*)ExtKanaFontFXmini+(7+1)*CharNo;
            if ( CharNo< 0x20 ) ExtCharGaijiMiniFX = 1; else ExtCharKanaMiniFX = 1;
        } else {
            fontptr=(char*)ExtKanaFontFX+(8)*CharNo;
            if ( CharNo< 0x20 ) ExtCharGaijiFX = 1; else ExtCharKanaFX = 1;
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
    if ( flag & 1 ) ExtCharKanaFX  = LoadExtFontKana_sub( sname2, (char*)ExtKanaFontFX+(8)*32, 5 );
    
    if ( sname[0]=='\0' ) {
            if ( ( no < 0 )||( 9 < no ) )    sprintf(sname2 ,"%sFONTK6M",   fontfolderFX[folder] );
            else                            sprintf(sname2 ,"%sFONTK6M%d", fontfolderFX[folder], no);
    }
    if ( flag & 2 ) ExtCharKanaMiniFX  = LoadExtFontKanaMini_sub( sname2, (char*)ExtKanaFontFXmini+(1+7)*32, 5 );
}

void LoadExtFontGaijifolder( int flag, char* sname, int folder, int no ){        // FONTG8L.bmp -> font 6x8     FONTG6M.bmp -> mini font 6x6
    char sname2[32];
    strcpy( sname2, sname );
    if ( EnableExtFont==0 ) return ;
    if ( sname[0]=='\0' ) {
            if ( ( no < 0 )||( 9 < no ) )    sprintf(sname2 ,"%sFONTG8L",   fontfolderFX[folder] );
            else                            sprintf(sname2 ,"%sFONTG8L%d", fontfolderFX[folder], no );
    }
    if ( flag & 1 ) ExtCharGaijiFX = LoadExtFontKana_sub( sname2, (char*)ExtKanaFontFX, 2 );

    if ( sname[0]=='\0' ) {
            if ( ( no < 0 )||( 9 < no ) )    sprintf(sname2 ,"%sFONTG6M",   fontfolderFX[folder] );
            else                            sprintf(sname2 ,"%sFONTG6M%d", fontfolderFX[folder], no);
    }
    if ( flag & 2 ) ExtCharGaijiMiniFX = LoadExtFontKanaMini_sub( sname2, (char*)ExtKanaFontFXmini, 2 );
}

void LoadExtFontAnkfolder( int flag, char* sname, int folder, int no ){        // FONTA8L.bmp -> font 6x8     FONTA6M.bmp -> mini font 6x6
    char sname2[32];
    strcpy( sname2, sname );
    if ( EnableExtFont==0 ) return ;
    ExtCharAnkFX = 1;
    ExtCharAnkMiniFX = 1;
    if ( sname[0]=='\0' ) {
            if ( ( no < 0 )||( 9 < no ) )    sprintf(sname2 ,"%sFONTA8L",   fontfolderFX[folder] );
            else                            sprintf(sname2 ,"%sFONTA8L%d", fontfolderFX[folder], no);
    }
    if ( flag & 1 ) ExtCharAnkFX = LoadExtFontKana_sub( sname2, (char*)ExtAnkFontFX, 6 );
    
    if ( sname[0]=='\0' ) {
            if ( ( no < 0 )||( 9 < no ) )    sprintf(sname2 ,"%sFONTA6M",   fontfolderFX[folder] );
            else                            sprintf(sname2 ,"%sFONTA6M%d", fontfolderFX[folder], no);
    }
    if ( flag & 2 ) ExtCharAnkMiniFX = LoadExtFontKanaMini_sub( sname2, (char*)ExtAnkFontFXmini, 6 );
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
    if ( ( flag & 1 ) && ( ExtCharKanaFX ) ) SaveExtFontKana_sub( sname2, (char*)ExtKanaFontFX+(8)*32, 5, check );
    
    if ( sname[0]=='\0' ) {
            if ( ( no < 0 )||( 9 < no ) )    sprintf(sname2 ,"%sFONTK6M",   fontfolderFX[folder] );
            else                            sprintf(sname2 ,"%sFONTK6M%d", fontfolderFX[folder], no);
    }
    if ( ( flag & 2 ) && ( ExtCharKanaMiniFX ) ) SaveExtFontKanaMini_sub( sname2, (char*)ExtKanaFontFXmini+(1+7)*32, 5, check );
}

void SaveExtFontGaiji( int flag, char* sname, int folder, int no, int check ){        // font 6x8 -> FONTG8L.bmp    font 6x6 -> FONTG6M.bmp
    char sname2[32];
    if ( EnableExtFont==0 ) return ;
    strcpy( sname2, sname );
    if ( sname[0]=='\0' ) {
            if ( ( no < 0 )||( 9 < no ) )    sprintf(sname2 ,"%sFONTG8L",   fontfolderFX[folder] );
            else                            sprintf(sname2 ,"%sFONTG8L%d", fontfolderFX[folder], no );
    }
    if ( ( flag & 1 ) && ( ExtCharGaijiFX ) ) SaveExtFontKana_sub( sname2, (char*)ExtKanaFontFX, 2, check );
    
    if ( sname[0]=='\0' ) {
            if ( ( no < 0 )||( 9 < no ) )    sprintf(sname2 ,"%sFONTG6M",   fontfolderFX[folder] );
            else                            sprintf(sname2 ,"%sFONTG6M%d", fontfolderFX[folder], no);
    }
    if ( ( flag & 2 ) && ( ExtCharGaijiMiniFX ) ) SaveExtFontKanaMini_sub( sname2, (char*)ExtKanaFontFXmini, 2, check );
}

void SaveExtFontAnk( int flag, char* sname, int folder, int no, int check ){            // font 6x8 -> FONTA8L.bmp    font 6x6 -> FONTA6M.bmp
    char sname2[32];
    if ( EnableExtFont==0 ) return ;
    strcpy( sname2, sname );
    if ( sname[0]=='\0' ) {
            if ( ( no < 0 )||( 9 < no ) )    sprintf(sname2 ,"%sFONTA8L",   fontfolderFX[folder] );
            else                            sprintf(sname2 ,"%sFONTA8L%d", fontfolderFX[folder], no );
    }
    if ( ( flag & 1 ) && ( ExtCharAnkFX ) ) SaveExtFontKana_sub( sname2, (char*)ExtAnkFontFX, 6, check );
    
    if ( sname[0]=='\0' ) {
            if ( ( no < 0 )||( 9 < no ) )    sprintf(sname2 ,"%sFONTA6M",   fontfolderFX[folder] );
            else                            sprintf(sname2 ,"%sFONTA6M%d", fontfolderFX[folder], no);
    }
    if ( ( flag & 2 ) && ( ExtCharAnkMiniFX ) ) SaveExtFontKanaMini_sub( sname2, (char*)ExtAnkFontFXmini, 6, check );
}
