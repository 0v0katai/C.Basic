extern "C" {

//  ������̃T�C�g���6x8�J�i�t�H���g�f�[�^����ѕ\�����[�`���𕔕��I�Ɉ��p�����Ă��������Ă��܂��B
//	http://www.d-b.ne.jp/~k-kita/fx_sdk_005.html
//
#include "prizm.h"
#include "CBP.h"
#include "CBP_kfont.h"
#include "CBP_kfontminimini.h"
#include "CBP_kfontminiminiBold.h"
//------------------------------------------------------------------------------
char GBcode=0;			// GB font enable:1		Full GB code:2
char ExtCharAnk=0;				// 0:Normal 	1:Ext Ank   font 
char ExtCharKana=0;				// 0:Normal 	1:Ext Kana  font 
char ExtCharGaiji=0;			// 0:Normal 	1:Ext Gaiji font 
char ExtCharAnkMini=0;				// 0:Normal 	1:Ext Ank   font 
char ExtCharKanaMini=0;				// 0:Normal 	1:Ext Kana  font 
char ExtCharGaijiMini=0;			// 0:Normal 	1:Ext Gaiji font 
static unsigned char ExtAnkFont[96][24*3];			// Ext Ank font
static unsigned char ExtAnkFontmini[96][1+18*2];	// Ext Ank font
static unsigned char ExtKanaFont[128][24*3];		// Ext Kana & Gaiji font
static unsigned char ExtKanaFontmini[128][1+18*2];	// Ext Kana & Gaiji font

char ExtCharAnkFX=0;			// 0:Normal 	1:Ext Ank  font FX
char ExtCharKanaFX=0;			// 0:Normal 	1:Ext Kana  font FX
char ExtCharGaijiFX=0;			// 0:Normal 	1:Ext Gaiji font FX
char ExtCharAnkMiniFX=0;			// 0:Normal 	1:Ext Ank  font FX
char ExtCharKanaMiniFX=0;			// 0:Normal 	1:Ext Kana  font FX
char ExtCharGaijiMiniFX=0;			// 0:Normal 	1:Ext Gaiji font FX
static unsigned char ExtAnkFontFX[96][8];			// Ext Ank font FX
static unsigned char ExtAnkFontFXmini[96][1+7];		// Ext Ank font FX
static unsigned char ExtKanaFontFX[128][8];			// Ext Kana & Gaiji font FX
static unsigned char ExtKanaFontFXmini[128][1+7];	// Ext Kana & Gaiji font FX
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// Standard font 18x24  fx(6x8)
//------------------------------------------------------------------------------

int RevMode( int mode ) {
	int a=mode&0xFF;
	if (  mode & 0x10000000 ) {
		switch ( a ) {
			case MINI_OVER:
				a = MINI_REV;
				break;
			case MINI_REV:
				a = MINI_OVER;
				break;
			case MINI_OR:
				a = MINI_REVOR;
				break;
			case MINI_REVOR:
				a = MINI_OR;
				break;
			default:
				break;
		}
	}
	return (mode&0x00FFFF00) + a;
}
int GBmodeChar( int *a, int *b, int mode ){	// GB mode in
	*a=0xE5; *b=0x9E;
	return RevMode( mode | 0x10000000 );
}

int KPrintCharSub( int px, int py, unsigned char *str, int mode) { //	gaiji �J�i�ꕶ���\��	px,py  (0,-24)-(383,191)
    DISPGRAPH kfont;
    GRAPHDATA kfont_info;
    unsigned short mb_glyph_no;
    unsigned short glyph_info;
    int a,b;
	int color=CB_ColorIndex;
	int extflag= mode & 0xF00;
	int dy =(mode>>16)&0xFF;
	int fx1dotfontmode=0, reduce=0;
	if ( CB_ColorIndex < 0 ) CB_ColorIndex=0x0000;
	if ( mode & 0x1000 ) fx1dotfontmode=1;
	kfont_info.height = 24;
	a = str[0];
	b = str[1];
	if ( (fx1dotfontmode==0)&&(CB_G1MorG3M==3) ) {
		if ( (a==0x03)&&(b==0xA8) ) mode = GBmodeChar(&a,&b,mode);		// GB code pre byte
		else
		if ( (a==0x5C)&&(str[2]==0x5C)&&(((0xE5<=b)&&(b<=0xE7))||(b==0x7F)) ) {	// escape GB font
			a=b;
			b=str[3];
			mode = RevMode( mode );
		} else
		if ( GBcode ) {		// GB font 
			if ( (a==0x5C) && CheckEscape1(b) ) {	// escape GB font
				a=b;
				mode = RevMode( mode );
			} else
			if ( IsGBCode2(a,b) ) {
				mb_glyph_no = (a<<8) + b ;
				kfont_info.pBitmap=(unsigned char *)GetGBGlyphPtr( mb_glyph_no, &glyph_info );
				if ( glyph_info == 0 ) kfont_info.pBitmap=(unsigned char *)GetGBGlyphPtr( 0xA1F9, &glyph_info );	// ��
				kfont_info.width  = glyph_info ;
				if ( mode & 0x20000000 ) { kfont_info.width = 18; reduce=1; }
		    	fx1dotfontmode=1;
		    	goto next;
		    }
		}
		if ( a < 0x7F ) {
			if ( (extflag) && ( (ExtCharAnk)&&( 0x20<=a ) ) ) kfont_info.pBitmap = (unsigned char *)ExtAnkFont[a-0x20];
			else kfont_info.pBitmap=(unsigned char *)GetGlyphPtr( a );
		} else
		if ( ( 0x80 <= a ) && ( a <= 0xDF ) ) kfont_info.pBitmap=(unsigned char *)GetGlyphPtr( a );
		else
		if ( a == 0x7F )  {
			kfont_info.pBitmap=(unsigned char *)GetGlyphPtr_7F( b );
		} else
		if ( a == 0xE5 )  {
			kfont_info.pBitmap=(unsigned char *)GetGlyphPtr_E5( b );
		} else
		if ( a == 0xE6 )  {
			kfont_info.pBitmap=(unsigned char *)GetGlyphPtr_E6( b );
		} else
		if ( a == 0xE7 )  {
			kfont_info.pBitmap=(unsigned char *)GetGlyphPtr_E7( b );
		} else
		if ( a == 0xFF ) {
			if ( ((ExtCharGaiji)&&(0x80<=b)&&(b<=0x9F)) || ((ExtCharKana)&&(0xA0<=b)&&(b<=0xEF)) )  kfont_info.pBitmap = (unsigned char *)ExtKanaFont[b-0x80];
			else 
			if ( ( 0x80 <= b )&&( b <= 0xEF ) ) { kfont_info.pBitmap = (unsigned char *)KanaFont[b-0x80]; goto fxent; }
			else goto err;
		} else errcg: kfont_info.pBitmap=(unsigned char *)GetGlyphPtr( '@' );
	    kfont_info.width  = 18;
	    fx1dotfontmode=1;
	    goto next;
	} else {
		if ( (a==0x03)&&(b==0xA8) ) mode = GBmodeChar(&a,&b,mode);		// GB code pre byte
		else
		if ( (a==0x5C)&&(str[2]==0x5C)&&(((0xE5<=b)&&(b<=0xE7))||(b==0x7F)) ) {	// escape GB font
			a=b;
			b=str[3];
			mode = RevMode( mode );
		} else
		if ( GBcode ) {
			if ( (a==0x5C) && CheckEscape1(b) ) {	// escape GB font
				a=b;
				mode = RevMode( mode );
			} else
			if ( IsGBCode2(a,b) )  a='*';		// GB font
		}
		if ( a < 0x7F ) {
			if ( (extflag) && ( (ExtCharAnkFX)&&( 0x20<=a ) ) ) kfont_info.pBitmap = (unsigned char *)ExtAnkFontFX[a-0x20];
			else kfont_info.pBitmap=(unsigned char *)Font00[a];
		} else
		if ( ( 0x80 <= a ) && ( a <= 0xDF ) ) kfont_info.pBitmap=(unsigned char *)Font80[a-0x80];
		else
		if ( a == 0x7F )  {
				if ( b == 0x50 ) kfont_info.pBitmap=(unsigned char *)Font7F50;
				else
				if ( b == 0x53 ) kfont_info.pBitmap=(unsigned char *)Font7F53;
				else
				if ( b == 0x54 ) kfont_info.pBitmap=(unsigned char *)Font7F54;
				else
				if ( b == 0xC7 ) kfont_info.pBitmap=(unsigned char *)Font7FC7;
				else goto err;
		} else
		if ( a == 0xE5 )  {
				kfont_info.pBitmap=(unsigned char *)FontE5[b];
		} else
		if ( a == 0xE6 )  {
				kfont_info.pBitmap=(unsigned char *)FontE6[b];
		} else
		if ( a == 0xE7 )  {
				if ( ( 0x40 <= b ) && ( b <= 0x7E ) ) kfont_info.pBitmap=(unsigned char *)FontE7[b-0x40];
				else goto err;
		} else
		if ( a == 0xFF ) {
				if ( ((ExtCharGaijiFX)&&(0x80<=b)&&(b<=0x9F)) || ((ExtCharKanaFX)&&(0xA0<=b)&&(b<=0xEF)) )  kfont_info.pBitmap = (unsigned char *)ExtKanaFontFX[b-0x80];
				else
				if ( ( 0x80 <= b )&&( b <= 0xEF ) ) kfont_info.pBitmap = (unsigned char *)KanaFont[b-0x80];
				else goto err;
		}
		else {
		  err:
			kfont_info.pBitmap=(unsigned char *)Font00['@'];
		}
	  fxent:
	    kfont_info.width  = 6;
	    kfont_info.height = 8;
	    dy = 0;
	}
  next:
	kfont.x = px;
	kfont.y = py;
	if ( ( dy ) && ( dy<24 ) ) kfont_info.height = dy;
	kfont.GraphData = kfont_info;
	mode &= 0xFF;
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
	if ( CB_ColorIndex < 0 ) CB_ColorIndex=0x0000;
	if ( fx1dotfontmode ) {
	    if ( px+kfont_info.width > 384 ) kfont_info.width=384-px; 
	    if ( kfont_info.width > 0 ) {
			if ( reduce )	Bdisp_WriteGraph_VRAM_CG_1bit_75(&kfont);
			else 			Bdisp_WriteGraph_VRAM_CG_1bit(&kfont);
		}
	} else {
 	   	if ( px/3+kfont_info.width > 127 ) kfont_info.width=127-px/3;
 	  	if ( kfont_info.width > 0 ) Bdisp_WriteGraph_VRAM_fx_xy1(&kfont);
	}
  exit:
	CB_ColorIndex=color;
	if ( fx1dotfontmode ) return kfont_info.width ; else return kfont_info.width *3;
}
//---------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------
// mini font 5~16x18  fx(2x6~6x6)
//---------------------------------------------------------------------------------------------
int KPrintCharMiniFX( int px, int py, unsigned char *str, int mode ) { // �J�i�Ή� PrintMini FX or CG (0,-24)-(383,191)
    DISPGRAPH kfont;
    GRAPHDATA kfont_info;
    unsigned char *font;
    int a,b;
    int extflag ;
    extflag = mode & 0xFF00;
	a = str[0];
	b = str[1];
	if ( (a==0x03)&&(b==0xA8) ) mode = GBmodeChar(&a,&b,mode);	// GB code pre byte
	else
	if ( (a==0x5C)&&(str[2]==0x5C)&&(((0xE5<=b)&&(b<=0xE7))||(b==0x7F)) ) {	// escape GB font
		a=b;
		b=str[3];
		mode = RevMode( mode );
	} else
	if ( GBcode ) {
		if ( (a==0x5C) && CheckEscape1(b) ) {	// escape GB font
			a=b;
			mode = RevMode( mode );
		} else if ( IsGBCode2(a,b) )  a='*';		// GB font
	}
	if ( a < 0x7F ) {
		if ( ( extflag ) && ( ExtCharAnkMiniFX ) && ( 0x20 <= a ) ) font=(unsigned char *)ExtAnkFontFXmini[a-0x20];
		else font=(unsigned char *)Fontmini[a];
 	} else 
	if ( a == 0xFF )  {
		if ( ( 0xA0 <= b ) && ( b <= 0xEF ) ) {
			if ( ( extflag ) && ( ExtCharKanaMiniFX ) ) font=(unsigned char *)ExtKanaFontFXmini[b-0x80];
			else font=(unsigned char *)KanaFontmini[b-0x80];
		} else
		if ( ( 0x80 <= b ) && ( b <= 0x9F ) ) {
			if ( ( extflag ) && ( ExtCharGaijiMiniFX ) ) font=(unsigned char *)ExtKanaFontFXmini[b-0x80];
			else font=(unsigned char *)KanaFontmini[b-0x80];
		} else goto err;
	} else
	if ( a == 0x7F )  {
		if ( b == 0x50 ) font=(unsigned char *)Fontmini7F50;
		else
		if ( b == 0x53 ) font=(unsigned char *)Fontmini7F53;
		else
		if ( b == 0x54 ) font=(unsigned char *)Fontmini7F54;
		else
		if ( b == 0xC7 ) font=(unsigned char *)Fontmini7FC7;
		else goto err;
	} else
	if ( a == 0xE5 )  {
		font=(unsigned char *)FontminiE5[b];
	} else
	if ( a == 0xE6 )  {
		font=(unsigned char *)FontminiE6[b];
	} else
	if ( a == 0xE7 )  {
		if ( ( 0x40 <= b ) && ( b <= 0x7E ) ) font=(unsigned char *)FontminiE7[b-0x40];
		else goto err;
	} else
	if ( ( 0x80 <= a ) && ( a <= 0xDF ) ) font=(unsigned char *)Fontmini80[a-0x80];
	else {
	  err:
		font=(unsigned char *)Fontmini['@'];
	}
  fxent:
    kfont_info.width = font[0];
    mode &= 0xFF;
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
    kfont.y = py-0;
    kfont.GraphData = kfont_info;
    if ( px/3+kfont_info.width > 127 ) kfont_info.width=127-px/3;
    if ( kfont_info.width > 0 ) Bdisp_WriteGraph_VRAM_fx_xy1(&kfont);
  exit:
    return kfont_info.width*3;
}

int KPrintCharMiniCG( int px, int py, unsigned char *str, int mode ) { // �J�i�Ή� PrintMini CG (0,-24)-(383,191)
    DISPGRAPH kfont;
    GRAPHDATA kfont_info;
    unsigned short mb_glyph_no;
    unsigned short glyph_info;
    unsigned char *font;
    int a,b;
    int extflag = mode & 0xFF00;
    int dy = mode>>16;
    unsigned char spc[]={0,0,0,0};
    int gb=0;
    int reduce=0;
	if ( ( dy<=0 ) || ( dy>18 ) ) dy=18;
	a= str[0];
	b= str[1];
	if ( (a==0x03)&&(b==0xA8) ) mode = GBmodeChar(&a,&b,mode);	// GB code pre byte
	else
	if ( (a==0x5C)&&(str[2]==0x5C)&&(((0xE5<=b)&&(b<=0xE7))||(b==0x7F)) ) {	// escape GB font
		a=b;
		b=str[3];
		mode = RevMode( mode );
	} else
	if ( GBcode ) {		// GB mini font 
		if ( (a==0x5C) && CheckEscape1(b) ) {	// escape GB font
			a=b;
			mode = RevMode( mode );
		} else
		if ( IsGBCode2(a,b) ) {
			mb_glyph_no = (a<<8) + b ;
			kfont_info.pBitmap=(unsigned char *)GetGBMiniGlyphPtr( mb_glyph_no, &glyph_info );
			if ( glyph_info == 0 ) kfont_info.pBitmap=(unsigned char *)GetGBMiniGlyphPtr( 0xA1F9, &glyph_info );	// ��
			kfont_info.width = glyph_info ;
			if ( mode & 0x20000000 ) { kfont_info.width = 12; reduce=1; }
		    kfont_info.height = 16;
			gb=dy-16; if ( gb<0 ) gb=0;
		    goto next3;
	    }
	}
	if ( ( 0x20 <= a )  && ( a < 0x7F ) && ( ExtCharAnkMini ) && ( extflag ) ) {
		font=(unsigned char *)ExtAnkFontmini[a-0x20];
		goto next;
	} else
	if ( a == 0xFF ) {
		if ( ((0x80<=b)&&(b<=0x9F)&&(ExtCharGaijiMini)) || ((0xA0<=b)&&(b<=0xEF)&&(ExtCharKanaMini)) ) font=(unsigned char *)ExtKanaFontmini[b-0x80];
		else return KPrintCharMiniFX( px, py, str, mode );
	} else {
		if ( (a==0x7F)||(a==0xF9)||(a==0xE5)||(a==0xE6)||(a==0xE7) ) mb_glyph_no = (a<<8) + b;
		else mb_glyph_no = a;
	  minicg:
		kfont_info.pBitmap=(unsigned char *)GetMiniGlyphPtr_MB( mb_glyph_no, &glyph_info );
		kfont_info.width = glyph_info ;
		goto next2;
	}
  next:
    kfont_info.width = font[0];
    kfont_info.pBitmap = font+1;
  next2:
    kfont_info.height = dy;
  next3:
    mode &= 0xFF;
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
    kfont.x = px;
    kfont.y = py;
    kfont.GraphData = kfont_info;
    if ( px+kfont_info.width > 384 ) kfont_info.width=384-px; 
    if ( kfont_info.width > 0 ) {
		if ( reduce )	Bdisp_WriteGraph_VRAM_CG_1bit_75(&kfont);
		else			Bdisp_WriteGraph_VRAM_CG_1bit(&kfont);
	}
    if ( gb ) {
		kfont.y = py+16;
		kfont_info.pBitmap = spc;
		kfont_info.height  = gb;
		kfont.GraphData = kfont_info;
		Bdisp_WriteGraph_VRAM_CG_1bit(&kfont);
	}
  exit:
    return kfont_info.width;
}

int KPrintCharMini( int px, int py, unsigned char *str, int mode ) { // �J�i�Ή� PrintMini FX (0,-24)-(383,191)
	if ( CB_G1MorG3M != 1 ) return KPrintCharMiniCG( px, py, str, mode );
	return KPrintCharMiniFX( px, py, str, mode );
}

//---------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------
// minimini font 6x10~10x10
//---------------------------------------------------------------------------------------------
int KPrintCharMiniMini( int px, int py, unsigned char *str, int mode ) { // PrintMiniMini (0,-24)-(383,191)
    DISPGRAPH kfont;
    GRAPHDATA kfont_info;
    unsigned char *font;
    int a,b;
    int extflag = mode & 0xFF00;
    int dy = mode>>16;
    unsigned short mb_glyph_no;
    unsigned short glyph_info;
	a= str[0];
	b= str[1];
	if ( (a==0x03)&&(b==0xA8) ) { mode = GBmodeChar(&a,&b,mode); goto mfont; }	// GB code pre byte
	if ( (a==0x5C)&&(str[2]==0x5C)&&(((0xE5<=b)&&(b<=0xE7))||(b==0x7F)) ) {	// escape GB font
		a=b;
		b=str[3];
		mb_glyph_no = (a<<8)+b;
		mode = RevMode( mode );
		goto mfont;
	}
	if ( GBcode ) {
		if ( (a==0x5C) && CheckEscape1(b) ) {	// escape GB font
			mb_glyph_no=b;
			mode = RevMode( mode );
			goto mment;
		} else 
		if ( IsGBCode2(a,b) ) { 
			mb_glyph_no='*';		// GB font 
			goto mment;
		}
	}
	mb_glyph_no=(unsigned short)GetOpcodeGB( (char*)str, 0 );
  mment:
	font=(unsigned char *)GetMiniMiniGlyphPtr_MB( mb_glyph_no, &glyph_info );
	if ( ( glyph_info ) && ( a != 0xE6 ) ) {
		kfont_info.width = glyph_info;
		kfont_info.pBitmap = font;
	} else {
	  mfont:
		if ( a < 0x7F ) font=(unsigned char *)MiniMiniFont00[a];
		else
		if ( ( 0x80 <= a ) && ( a <= 0xDF ) ) font=(unsigned char *)MiniMiniFont80[a-0x80];
		else 
		if ( a == 0x7F )  {
			if ( b == 0x50 ) font=(unsigned char *)MiniMiniFont7F50;
			else
			if ( b == 0x53 ) font=(unsigned char *)MiniMiniFont7F53;
			else
			if ( b == 0x54 ) font=(unsigned char *)MiniMiniFont7F54;
			else
			if ( b == 0xC7 ) font=(unsigned char *)MiniMiniFont7FC7;
			else goto err;
		} else
		if ( a == 0xE5 )  {
			font=(unsigned char *)MiniMiniFontE5[b];
		} else
		if ( a == 0xE6 )  {
			font=(unsigned char *)MiniMiniFontE6[b];
		} else
		if ( a == 0xE7 )  {
			if ( ( 0x40 <= b ) && ( b <= 0x7E ) ) font=(unsigned char *)MiniMiniFontE7[b-0x40];
			else goto err;
		} else
		if ( a == 0xFF ) {
			if ( ( 0x80 <= b )&&( b <= 0xEF ) ) font=(unsigned char *)MiniMiniKanaFont[b-0x80];
			else goto err;
		}
		else {
		  err:
			font=(unsigned char *)MiniMiniFont00['@'];
		}
		kfont_info.width = font[0];
		kfont_info.pBitmap = font+1;
	}
    mode &= 0xFF;
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
	if ( ( dy<=0 ) || ( dy>10 ) ) dy=10;
    kfont_info.height = dy;
    kfont.x = px;
    kfont.y = py-0;
    kfont.GraphData = kfont_info;
    if ( px+kfont_info.width > 384 ) kfont_info.width=384-px; 
    if ( kfont_info.width > 0 ) Bdisp_WriteGraph_VRAM_CG_1bit(&kfont);
  exit:
    return kfont_info.width;
}

int KPrintCharMiniMiniBold( int px, int py, unsigned char *str, int mode ) { // PrintMiniMini (0,-24)-(383,191) Bold
    DISPGRAPH kfont;
    GRAPHDATA kfont_info;
    unsigned char *font;
    int a,b,c;
    int extflag = mode & 0xFF00;
    int dy = mode>>16;
    unsigned short mb_glyph_no;
    unsigned short glyph_info;
	a= str[0];
	b= str[1];
	if ( (a==0x03)&&(b==0xA8) ) { mode = GBmodeChar(&a,&b,mode); goto mfont; }	// GB code pre byte
	if ( (a==0x5C)&&(str[2]==0x5C)&&(((0xE5<=b)&&(b<=0xE7))||(b==0x7F)) ) {	// escape GB font
		a=b;
		b=str[3];
		mb_glyph_no = (a>>8)+b;
		mode = RevMode( mode );
		goto mfont;
	} else
	if ( GBcode ) {
		if ( (a==0x5C) && CheckEscape1(b) ) {	// escape GB font
			mb_glyph_no=b;
			mode = RevMode( mode );
			goto mment;
		} else 
		if ( IsGBCode2(a,b) ) { 
			mb_glyph_no='*';		// GB font 
			goto mment;
		}
	}
	mb_glyph_no=(unsigned short)GetOpcodeGB( (char*)str, 0 );
  mment:
	font=(unsigned char *)GetBoldMiniMiniGlyphPtr_MB( mb_glyph_no, &glyph_info );
	if ( glyph_info ) {
		kfont_info.width = glyph_info;
		kfont_info.pBitmap = font;
	} else {
	  mfont:
		if ( a < 0x7F ) font=(unsigned char *)MiniMiniBoldFont00[a];
		else
		if ( ( 0x80 <= a ) && ( a <= 0xDF ) ) font=(unsigned char *)MiniMiniBoldFont80[a-0x80];
		else 
		if ( a == 0x7F )  {
			if ( b == 0x50 ) font=(unsigned char *)MiniMiniBoldFont7F50;
			else
			if ( b == 0x53 ) font=(unsigned char *)MiniMiniBoldFont7F53;
			else
			if ( b == 0x54 ) font=(unsigned char *)MiniMiniBoldFont7F54;
			else
			if ( b == 0xC7 ) font=(unsigned char *)MiniMiniBoldFont7FC7;
			else goto err;
		} else
		if ( a == 0xE5 )  {
			font=(unsigned char *)MiniMiniBoldFontE5[b];
		} else
		if ( a == 0xE6 )  {
			font=(unsigned char *)MiniMiniBoldFontE6[b];
		} else
		if ( a == 0xE7 )  {
			if ( ( 0x40 <= b ) && ( b <= 0x7E ) ) font=(unsigned char *)MiniMiniBoldFontE7[b-0x40];
			else goto err;
		} else
		if ( a == 0xFF ) {
			if ( ( 0x80 <= b )&&( b <= 0xEF ) ) font=(unsigned char *)MiniMiniKanaFont[b-0x80];
			else goto err;
		}
		else {
		  err:
			font=(unsigned char *)MiniMiniFont00['@'];
		}
		kfont_info.width = font[0];
		kfont_info.pBitmap = font+1;
	}
    mode &= 0xFF;
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
	if ( ( dy<=0 ) || ( dy>10 ) ) dy=10;
    kfont_info.height = dy;
    kfont.x = px;
    kfont.y = py-0;
    kfont.GraphData = kfont_info;
    if ( px+kfont_info.width > 384 ) kfont_info.width=384-px; 
    if ( kfont_info.width > 0 ) Bdisp_WriteGraph_VRAM_CG_1bit(&kfont);
  exit:
    return kfont_info.width;
}

int KPrintCharMiniMiniFX( int px, int py, unsigned char *str, int mode ) { // PrintMiniMiniFX (0,-24)-(383,191) fx 6*8dot font
    DISPGRAPH kfont;
    GRAPHDATA kfont_info;
    unsigned char *font;
    int extflag = mode & 0xFF00;
    int dy = mode>>16;
    int a,b;
	a= str[0];
	b= str[1];
	if ( (a==0x03)&&(b==0xA8) ) { mode = GBmodeChar(&a,&b,mode); goto mfont; }	// GB code pre byte
	if ( (a==0x5C)&&(str[2]==0x5C)&&(((0xE5<=b)&&(b<=0xE7))||(b==0x7F)) ) {	// escape GB font
		a=b;
		b=str[3];
		mode = RevMode( mode );
		goto mfont;
	} else
	if ( GBcode ) {
		if ( (a==0x5C) && CheckEscape1(b) ) {	// escape GB font
			a=b;
			mode = RevMode( mode );
		} else 
		if ( IsGBCode2(a,b) ) { 
			font=(unsigned char *)Font00['*'];		// GB font 
			goto fxent;
		}
	}
	if ( ( ( ExtCharKanaFX )&&(a==0xFF)&&(0xA0<=b)&&(b<=0xEF) ) || ( ( ExtCharGaijiFX )&&(a==0xFF)&&(0x80<=b)&&(b<=0x9F) ) || ( ( extflag )&&( ExtCharAnkFX )&&(0x20<=a)&&(a<0x7F) ) ) {
		if ( ( 0x20 <= a )&&( a < 0x7F ) ) {
		    font = (unsigned char *)ExtAnkFontFX[a-0x20];
		} else {
			if ( b >= 0xF0 ) goto err ;
		    font = (unsigned char *)ExtKanaFontFX[a-0x80];
		}
	} else {				// fx font
	  mfont:
		if ( a < 0x7F ) font=(unsigned char *)Font00[a];
		else
		if ( ( 0x80 <= a ) && ( a <= 0xDF ) ) font=(unsigned char *)Font80[a-0x80];
		else 
		if ( a == 0x7F )  {
			if ( b == 0x50 ) font=(unsigned char *)Font7F50;
			else
			if ( b == 0x53 ) font=(unsigned char *)Font7F53;
			else
			if ( b == 0x54 ) font=(unsigned char *)Font7F54;
			else
			if ( b == 0xC7 ) font=(unsigned char *)Font7FC7;
			else goto err;
		} else
		if ( a == 0xE5 )  {
			font=(unsigned char *)FontE5[b];
		} else
		if ( a == 0xE6 )  {
			font=(unsigned char *)FontE6[b];
		} else
		if ( a == 0xE7 )  {
			if ( ( 0x40 <= b ) && ( b <= 0x7E ) ) font=(unsigned char *)FontE7[b-0x40];
			else goto err;
		} else
		if ( a == 0xFF ) {
			if ( ( 0x80 <= b )&&( b <= 0xEF ) ) font=(unsigned char *)KanaFont[b-0x80];
			else goto err;
		}
		else {
		  err:
			font=(unsigned char *)Font00['@'];
		}
	}
  fxent:
    mode &= 0xFF;
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
	if ( ( dy<=0 ) || ( dy>8 ) ) dy=8;
    kfont_info.width  = 6;
    kfont_info.height = dy;
    kfont_info.pBitmap = font;
    kfont.x = px;
    kfont.y = py-0;
    kfont.GraphData = kfont_info;
    if ( px+kfont_info.width > 384 ) kfont_info.width=384-px; 
    if ( kfont_info.width > 0 ) Bdisp_WriteGraph_VRAM_CG_1bit(&kfont);
  exit:
    return kfont_info.width;
}
//---------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------
void ClearExtFontflag(){
		ExtCharAnk=0;				// 0:Normal 	1:Ext Ank   font 
		ExtCharKana=0;				// 0:Normal 	1:Ext Kana  font 
		ExtCharGaiji=0;				// 0:Normal 	1:Ext Gaiji font 
		ExtCharAnkMini=0;			// 0:Normal 	1:Ext Ank   font 
		ExtCharKanaMini=0;			// 0:Normal 	1:Ext Kana  font 
		ExtCharGaijiMini=0;			// 0:Normal 	1:Ext Gaiji font 
		ExtCharAnkFX=0;				// 0:Normal 	1:Ext Ank  font FX
		ExtCharKanaFX=0;			// 0:Normal 	1:Ext Kana  font FX
		ExtCharGaijiFX=0;			// 0:Normal 	1:Ext Gaiji font FX
		ExtCharAnkMiniFX=0;			// 0:Normal 	1:Ext Ank  font FX
		ExtCharKanaMiniFX=0;		// 0:Normal 	1:Ext Kana  font FX
		ExtCharGaijiMiniFX=0;		// 0:Normal 	1:Ext Gaiji font FX
		memcpy( (char*)ExtAnkFontFX,     (char*)Font00  +32*8,  96*8 );		// Ank Font copy   FX font 
		memcpy( (char*)ExtAnkFontFXmini, (char*)Fontmini+32*8,  96*8 );		// Ank mini copy   FX font 
		memcpy( (char*)ExtKanaFontFX,    (char*)KanaFont    ,  112*8 );		// Ank Font copy   FX font 
		memcpy( (char*)ExtKanaFontFXmini,(char*)KanaFontmini,  112*8 );		// Ank mini copy   FX font 
}

void ReadExtFont(){
	LoadExtFontAnk(   3, "", -1 );			// LFONTA.bmp -> font 18x24
	LoadExtFontKana(  3, "", -1 );			// LFONTK.bmp -> font 18x24
	LoadExtFontGaiji( 3, "", -1 );			// LFONTG.bmp -> font 18x24
}

int CB_GetFontSub( char *SRC, char *cstr, int *orgflag, int getmode ) {
	int opcode;
	int c=SRC[ExecPtr];
	if ( c == ')' ) { ExecPtr++; return -1; }
	*orgflag=0; if ( c == '@' ) { ExecPtr++; *orgflag=1; }
	EnableDisableGB( SRC );	// GB mode enable ##    GB mode disable %%
	c=CB_IsStr( SRC, ExecPtr );
	if ( c ) {	// string
		CB_GetLocateStr( SRC, cstr, CB_StrBufferMax-1 );		// String -> buffer	return 
		GetOpcodeLen( cstr, 0, &opcode );
	} else {	// expression
		opcode = CB_EvalInt( SRC );
		if ( getmode == 0 ) {
			if ( opcode == 0 ) { EnableExtFont=0; ClearExtFontflag(); return -2; }
			if ( opcode == 1 ) { EnableExtFont=1; ReadExtFont(); return -2; } 
		}
		if ( opcode>0xFFFF ) { 
			cstr[0]=(opcode>>24) & 0xFF;
			cstr[1]=(opcode>>16) & 0xFF;
			cstr[2]=(opcode>> 8) & 0xFF;
			cstr[3]=(opcode) & 0xFF;
			cstr[4]='\0';
		} else 
		if ( opcode>0xFF ) { 
			cstr[0]=(opcode>>8) & 0xFF;
			cstr[1]=(opcode) & 0xFF;
			cstr[2]='\0';
		} else {
			cstr[0]=opcode;
			cstr[1]='\0';
		}
	}
	return opcode ;
}

int CB_GetFont( char *SRC ){	// GetFont(0xFFA0)->Mat C
	int c;
	int x,y,px,py;
	int reg,i;
	int mptr;
	int ElementSize=1;
	unsigned short CharNo;
	int width,height;
	char vbuf[2*384*24];
	char *vram=(char*)PictAry[0]+384*24*2;
	unsigned char cstr[256];
	int color=CB_ColorIndex;
	unsigned short backcolor=CB_BackColorIndex;
	int orgflag;
	int bk_GBcode=GBcode;

	c = CB_GetFontSub( SRC, (char*)cstr, &orgflag, 1 ) ;
	if ( c == -1 ) goto exit;
	if ( SRC[ExecPtr] == ')' ) ExecPtr++;
	if ( SRC[ExecPtr] == 0x0E ) {  // -> Mat C
			ExecPtr++;
			if ( CB_G1MorG3M==1 ) { 
				width=6;
				height=8;
			} else {
				width=18;
				height=24;
			}
			MatrixOprand( SRC, &reg, &x, &y );
			if ( ErrorNo == NoMatrixArrayERR ) { 	// No Matrix Array
				ErrorNo=0;	// error cancel
			}
			DimMatrixSub( reg, 1, width, height,  ElementSize ) ;	// 1bit
			if ( ErrorNo ) goto exit; // error

			memcpy( vbuf, vram, 384*24*2 );
			CB_ColorIndex=0x0000;	// current color index reset
			CB_BackColorIndex=0xFFFF;	// current color index reset
			if ( ( CB_G1MorG3M==1 ) && ( orgflag == 0 ) ) {
				if ( cstr==NULL )	CB_PrintC( 1, 1, (unsigned char *)" " );
				else 			CB_PrintC_ext( 1, 1, cstr, (orgflag==0)*0x100 );	// extflag
			} else {
				if ( cstr==NULL )	CB_PrintC( 1, 1, (unsigned char *)" " );
				else			CB_PrintC_ext( 1, 1, cstr, (orgflag==0)*0x100 | 0x20000000 );	// extflag
			}
			x=1;
			y=1;
			i=x;
			for ( py=0; py<height ; py++) {
				x=i;
				for ( px=0; px<width ; px++) WriteMatrixInt( reg, x++, y, BdispGetPointVRAM2(px,py) );
				y++;
			}
			memcpy( vram, vbuf, 384*24*2 );
	} else { CB_Error(SyntaxERR);}	// Syntax error
  exit:
	GBcode=bk_GBcode;
	CB_ColorIndex=color;	// current color index reset
	CB_BackColorIndex=backcolor;	// current color index reset
	if ( CB_G1MorG3M==1 )	return (ExtCharKanaFX!=0)*4 + (ExtCharGaijiFX!=0)*2 + (ExtCharAnkFX!=0);
	else 					return (ExtCharKana  !=0)*4 + (ExtCharGaiji  !=0)*2 + (ExtCharAnk  !=0);
}

int CB_GetFontMini( char *SRC ){	// GetFont(0xFFA0)->Mat C
	int c;
	int x,y,px,py;
	int reg,i;
	int mptr;
	int ElementSize=1;
	unsigned short CharNo;
	int width,height;
	char vbuf[2*384*24];
	char *vram=(char*)PictAry[0]+384*24*2;
	unsigned char cstr[256];
	int color=CB_ColorIndex;
	unsigned short backcolor=CB_BackColorIndex;
	int orgflag;
	int bk_GBcode=GBcode;
	int MiniMini=0;
	
	c = CB_GetFontSub( SRC, (char*)cstr, &orgflag, 1 ) ;
	if ( c == -1 ) goto exit;
	if ( SRC[ExecPtr] == ')' ) ExecPtr++;
	c=SRC[ExecPtr];
	if ( (c=='M')||(c=='m') ) { c=SRC[++ExecPtr];
		if ( (c=='B')||(c=='b') ) {
			ExecPtr++;
			MiniMini=2;	// MiniMini Bold
		} else {
			MiniMini=1;	// MiniMini
		}
	}
	if ( SRC[ExecPtr] == 0x0E ) {  // -> Mat C
			ExecPtr++;
			if ( CB_G1MorG3M==1 ) { 
				height=6; 
				width=CB_PrintMiniC_fx( 0, 0, cstr, 0x21 | (0x100*(orgflag==0)) )/3;
			} else {
				switch ( MiniMini ) {
					case 1:
						width=CB_PrintMiniMiniLength( cstr );
						height=10;
						break;
					case 2:
						width=CB_PrintMiniMiniBoldLength( cstr );
						height=10;
						break;
					default:
						height=18;
						width=CB_PrintMiniLength( cstr, (orgflag==0) );
				}
			}
			if ( width<2 ) width=2;
			MatrixOprand( SRC, &reg, &x, &y );
			if ( ErrorNo == NoMatrixArrayERR ) { 	// No Matrix Array
				ErrorNo=0;	// error cancel
			}
			DimMatrixSub( reg, 1, width, height, ElementSize ) ;	// 1bit
			if ( ErrorNo ) goto exit; // error
			
			memcpy( vbuf, vram, 384*24*2 );
			CB_ColorIndex=0x0000;	// current color index reset
			CB_BackColorIndex=0xFFFF;	// current color index reset
			if ( cstr==NULL ) {
									CB_PrintMiniC( 0, 0, (unsigned char *)" ", MINI_OVER );
			} else {
				if ( CB_G1MorG3M==1 ) {
					if ( orgflag )	CB_PrintMiniC_fx( 0, 0, cstr, MINI_OVER );
					else			CB_PrintMiniC_fx( 0, 0, cstr, MINI_OVER | 0x100 );
				} else {
					switch ( MiniMini ) {
						case 1:
							CB_PrintMiniMiniC( 0, 0, cstr, MINI_OVER );
							break;
						case 2:
							CB_PrintMiniMiniBoldC( 0, 0, cstr, MINI_OVER );
							break;
						default:
							if ( orgflag )	CB_PrintMiniC( 0, 0, cstr, MINI_OVER );
							else			CB_PrintMiniC( 0, 0, cstr, MINI_OVER | 0x100 );
					}
				}
			}
			x=1;
			y=1;
			i=x;
			for ( py=0; py<height ; py++) {
				x=i;
				for ( px=0; px<width ; px++) WriteMatrixInt( reg, x++, y, BdispGetPointVRAM2(px,py) );
				y++;
			}
			memcpy( vram, vbuf, 384*24*2 );
	} else { CB_Error(SyntaxERR); }	// Syntax error
  exit:
	GBcode=bk_GBcode;
	CB_ColorIndex=color;	// current color index reset
	CB_BackColorIndex=backcolor;	// current color index reset
	if ( CB_G1MorG3M==1 )	return (ExtCharKanaMiniFX!=0)*4 + (ExtCharGaijiMiniFX!=0)*2 + (ExtCharAnkMiniFX!=0);
	else 					return (ExtCharKanaMini  !=0)*4 + (ExtCharGaijiMini  !=0)*2 + (ExtCharAnkMini  !=0);
}



char* CB_SetFontSub( char *SRC, int *reg, int mini ) {
	int CharNo;
	int width,height;
	unsigned char cstr[256];
	int orgflag;
	char *fontptr;
	int bk_GBcode=GBcode;

	CharNo=CB_GetFontSub( SRC, (char*)cstr, &orgflag, 0 );
	GBcode=bk_GBcode;
	if ( CharNo == -1 ) { CB_Error(SyntaxERR); return 0; }	// Syntax error
	if ( CharNo == -2 ) { return 0; }	// SetFont 0   SetFont 1
	if ( CharNo < 0xFF ) {
		if ( ( CharNo < 0x20 ) || ( CharNo >= 0x7F ) ) {  CB_Error(OutOfDomainERR); return NULL; } // Out of Domain error
		CharNo&=0xFF;
		CharNo-=0x20;
		if ( CB_G1MorG3M==1 ) {
			if ( mini )	{ fontptr=(char*)ExtAnkFontFXmini+(7+1)*CharNo; ExtCharAnkMiniFX = 1; }
			else		{ fontptr=(char*)ExtAnkFontFX+(8)*CharNo;		ExtCharAnkFX = 1; }
		} else {
			if ( mini )	{ fontptr=(char*)ExtAnkFontmini+(18*2+1)*CharNo; ExtCharAnkMini = 1; }
			else		{ fontptr=(char*)ExtAnkFont+(24*3)*CharNo;	   ExtCharAnk = 1; }
		}
	} else {
		CharNo&=0xFFFF;
		if ( ( 0xFF80 > CharNo ) || ( 0xFFEF < CharNo ) ) {  CB_Error(OutOfDomainERR); return NULL; } // Out of Domain error
		CharNo&=0xFF;
		CharNo-=0x80;
		if ( CB_G1MorG3M==1 ) {
			if ( mini )	{
				fontptr=(char*)ExtKanaFontFXmini+(7+1)*CharNo;
				if ( CharNo< 0x20 ) ExtCharGaijiMiniFX = 1; else ExtCharKanaMiniFX = 1;
			} else {
				fontptr=(char*)ExtKanaFontFX+(8)*CharNo;
				if ( CharNo< 0x20 ) ExtCharGaijiFX = 1; else ExtCharKanaFX = 1;
			}
		} else {
			if ( mini )	{
				if ( CharNo< 0x20 ) ExtCharGaijiMini = 1; else ExtCharKanaMini = 1;
				fontptr=(char*)ExtKanaFontmini+(18*2+1)*CharNo;
			} else {
				if ( CharNo< 0x20 ) ExtCharGaiji = 1; else ExtCharKana = 1;
				 fontptr=(char*)ExtKanaFont+(24*3)*CharNo;
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

void CB_SetFont( char *SRC ){	// SetChar 0xFFA0,Mat C
	int x,y,px,py;
	int reg;
	char *matptr,*fontptr;

	fontptr=CB_SetFontSub( SRC, &reg, 0 );
	if ( ( fontptr==NULL ) || ( ErrorNo ) ) return ;
	if ( EnableExtFont==0 ) return ;
	matptr=(char*)MatAry[reg].Adrs;
	if ( CB_G1MorG3M==1 ) {
		for ( py=0; py<8 ; py++) {
			*(fontptr++)=*(matptr++);
		}
	} else {
		for ( py=0; py<24 ; py++) {
			*(fontptr++)=*(matptr++);
			*(fontptr++)=*(matptr++);
			*(fontptr++)=*(matptr++);
		}
	}
}

void CB_SetFontMini( char *SRC ){	// SetChar 0xFFA0,Mat C
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
	if ( CB_G1MorG3M==1 ) {
		*(fontptr++)=0;
		for ( py=0; py<6 ; py++) {
			*(fontptr++)=*(matptr++);
		}
	} else {
		for ( py=0; py<18 ; py++) {
			*(fontptr++)=*(matptr++);
			if ( width >  8 ) *(fontptr++)=*(matptr++); 
		}
	}
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
int ReadBmpHeader( unsigned char *filebase, int *bfOffBits, int *biWidth, int *biHeight );	// 1:1 bit mono bmp file   24:24bit   0:no bmp
int WriteBmpHeader( char *filebase, int biWidth, int biHeight, int bit );	// 1 bit mono bmp file
void DecodeBmp2mem( char *buffer2, char *buffer, int width, int height, int bit );	//	bmpformat(buffer) -> bmp(buffer2)
int EncodeMat2Bmp( int reg , char *buffer , int width, int height, int bit );	//	Mat -> bmpformat(buffer)

//------------------------------------------------------------------------------

void FontCopy( char *font,  int reg,  int mx, int my, int dx, int dy ) {	// buffer[x,y]  -> font  1bit data
	int i,j;
	int x,y;
	int xbyte;
	
	xbyte= (dx+7)/8;
	for ( y=0; y<dy; y++ ) {	//
		for ( x=0; x<dx; x++ ) {
			if ( ReadMatrixInt( reg, x+mx, y+my ) ) {
				font[y*xbyte+(x>>3)] |= 128>>(x&7);		// set 1
			} else {
				font[y*xbyte+(x>>3)] &= ~(128>>(x&7));	// set 0
			}
		}
	}
}
int FontWidth( int reg, int mx, int cx, int cy ) {	// buffer[x,y] 1bit data -> width
	int x,y,my,dx;
	if ( CB_G1MorG3M==1 ) {
		my=cy*8;
		for ( x=1; x<6; x++ ) if ( ReadMatrixInt( reg, x+mx, my ) ) break ; 
	} else {
		my=cy*19;
		for ( x=1; x<16; x++ ) if ( ReadMatrixInt( reg, x+mx, my ) ) break ; 
	}
	return x;
}

int LoadExtFontKana_sub( char* name, char *font, int line ){		// LFONTK.bmp -> font 18x24
	char * FilePtr;
	int offset,ptr=0,size=0,bit;
	int width,height;
	int i,x,y,dx,dy,k,kk;
	int reg=Mattmpreg;
	char* matptr;
	char fname[64];
//	char buf[32],buf2[32];

	if ( name[0]=='/' ) FilePtr = CB_LoadSub( name, 0, &size, "bmp" ) ;
	else	FilePtr = Load1st2nd( name, "@Font", "bmp" );
	if ( ErrorNo == CantFindFileERR ) ErrorNo=0; 	// Cancel CantFindFileERR
	if ( FilePtr == NULL ) return 0;
	if ( CB_G1MorG3M==1 ) { k=1; kk=8; } else { k=3; kk=24*3; }

	bit=ReadBmpHeader( (unsigned char*)FilePtr, &offset, &width, &height );
	if ( ( bit != 1 ) || ( width > 8*k*16 ) || ( height > 8*k*line ) ) { return 0; }
	DimMatrixSub( reg, 1, width, height, 0 ) ;	// ElementSize=1  base=0
	if ( ErrorNo ) { return 0; } // error
	matptr=(char*)MatAry[reg].Adrs;
	DecodeBmp2mem( matptr , FilePtr+offset, width, height, bit );	//	bmpformat -> bmp

//	memset( font, 0, line*16*(24*3) );
	i=0;
	for ( y=0; y<line; y++ ) {
		for ( x=0; x<16; x++ ) {
			FontCopy( font+i, reg, x*6*k,y*8*k,6*k,8*k);
			i+=(kk);
		}
	}
	DeleteMatrix( reg );
	return 1;
}

int LoadExtFontKanaMini_sub( char* name, char *font, int line ){			// MFONTK.bmp -> font ?x18
	char * FilePtr;
	int offset,ptr=0,size,bit;
	int width,height;
	int i,x,y,dx,dy,mx,k,kk;
	int reg=Mattmpreg;
	char* matptr;
	char fname[64];
//	char buf[32],buf2[32];

	if ( name[0]=='/' ) FilePtr = CB_LoadSub( name, 0, &size, "bmp" ) ;
	else	FilePtr = Load1st2nd( name, "@Font", "bmp" );
	if ( ErrorNo == CantFindFileERR ) ErrorNo=0; 	// Cancel CantFindFileERR
	if ( FilePtr == NULL ) return 0;
	if ( CB_G1MorG3M==1 ) { k=1; } else { k=3; }

	bit=ReadBmpHeader( (unsigned char*)FilePtr, &offset, &width, &height );
	if ( ( bit != 1 ) || ( width > 8*k*16 ) || ( height > 8*k*line ) ) { return 0; }
	DimMatrixSub( reg, 1, width, height, 0 ) ;	// ElementSize=1  base=0
	if ( ErrorNo ) { return 0; } // error
	matptr=(char*)MatAry[reg].Adrs;
	DecodeBmp2mem( matptr , FilePtr+offset, width, height, bit );	//	bmpformat -> bmp

	if ( CB_G1MorG3M==1 ) {
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
	} else {
		for ( y=0; y<line; y++ ) {
			mx=0;
			for ( x=0; x<16; x++ ) {
				*(font)=12;
				dx=FontWidth( reg, mx, x, y );
				if ( dx<=2 ) break;
				if ( dx>16 ) dx=16;
				*(font)=dx;
				FontCopy( font+1, reg, mx, y*19+1, dx, 18);
				font+=(1+18*2);
				mx+=dx;
			}
		}
	}
	DeleteMatrix( reg );
	return 1;
}
const char fontfolderCG[][8]={"","/@Font/"};

void LoadExtFontKanafolder( int flag, char* sname, int folder, int no ){		// LFONTK.bmp -> font 18x24
	char sname2[32];
		
	int mode=CB_G1MorG3M;
	strcpy( sname2, sname );
	
	memcpy( ExtKanaFontFX,     KanaFont,     (32+64+3)*8 );		// kana copy
	memcpy( ExtKanaFontFXmini, KanaFontmini, (32+64+3)*8 );		// kana mini copy
	
	if ( EnableExtFont==0 ) return ;
	CB_G1MorG3M=1;
	if ( sname[0]=='\0' ) {
			if ( ( no < 0 )||( 9 < no ) )	sprintf3(sname2 ,"%sFONTK8L",   fontfolderCG[folder] );
			else							sprintf3(sname2 ,"%sFONTK8L%d", fontfolderCG[folder], no );
	}
	if ( flag & 1 ) ExtCharKanaFX  = LoadExtFontKana_sub( sname2, (char*)ExtKanaFontFX+(8)*32, 5 );
	
	if ( sname[0]=='\0' ) {
			if ( ( no < 0 )||( 9 < no ) )	sprintf3(sname2 ,"%sFONTK6M",   fontfolderCG[folder] );
			else							sprintf3(sname2 ,"%sFONTK6M%d", fontfolderCG[folder], no);
	}
	if ( flag & 2 ) ExtCharKanaMiniFX  = LoadExtFontKanaMini_sub( sname2, (char*)ExtKanaFontFXmini+(1+7)*32, 5 );

	CB_G1MorG3M=3;
	if ( sname[0]=='\0' ) {
			if ( ( no < 0 )||( 9 < no ) )	sprintf3(sname2 ,"%s@LFONTK",   fontfolderCG[folder] );
			else							sprintf3(sname2 ,"%s@LFONTK%d", fontfolderCG[folder] , no );
	}
	if ( flag & 1 ) ExtCharKana  = LoadExtFontKana_sub( sname2, (char*)ExtKanaFont+(24*3)*32, 5 );
	
	if ( sname[0]=='\0' ) {
			if ( ( no < 0 )||( 9 < no ) )	sprintf3(sname2 ,"%s@MFONTK",   fontfolderCG[folder] );
			else							sprintf3(sname2 ,"%s@MFONTK%d", fontfolderCG[folder] , no );
	}
	if ( flag & 2 ) ExtCharKanaMini  = LoadExtFontKanaMini_sub( sname2, (char*)ExtKanaFontmini+(1+18*2)*32, 5 );
	CB_G1MorG3M=mode;
}

void LoadExtFontGaijifolder( int flag, char* sname, int folder, int no ){		// LFONTG.bmp -> font 18x24
	char sname2[32];
	int mode=CB_G1MorG3M;
	strcpy( sname2, sname );
	
	if ( EnableExtFont==0 ) return ;
	CB_G1MorG3M=1;
	if ( sname[0]=='\0' ) {
			if ( ( no < 0 )||( 9 < no ) )	sprintf3(sname2 ,"%sFONTG8L",   fontfolderCG[folder] );
			else							sprintf3(sname2 ,"%sFONTG8L%d", fontfolderCG[folder], no );
	}
	if ( flag & 1 ) ExtCharGaijiFX = LoadExtFontKana_sub( sname2, (char*)ExtKanaFontFX, 2 );

	if ( sname[0]=='\0' ) {
			if ( ( no < 0 )||( 9 < no ) )	sprintf3(sname2 ,"%sFONTG6M",   fontfolderCG[folder] );
			else							sprintf3(sname2 ,"%sFONTG6M%d", fontfolderCG[folder], no);
	}
	if ( flag & 2 ) ExtCharGaijiMiniFX = LoadExtFontKanaMini_sub( sname2, (char*)ExtKanaFontFXmini, 2 );

	CB_G1MorG3M=3;
	if ( sname[0]=='\0' ) {
			if ( ( no < 0 )||( 9 < no ) )	sprintf3(sname2 ,"%s@LFONTG",   fontfolderCG[folder] );
			else							sprintf3(sname2 ,"%s@LFONTG%d", fontfolderCG[folder] , no );
	}
	if ( flag & 1 ) ExtCharGaiji = LoadExtFontKana_sub( sname2, (char*)ExtKanaFont, 2 );
	
	if ( sname[0]=='\0' ) {
		if ( ( no < 0 )||( 9 < no ) )	sprintf3(sname2 ,"@MFONTG");
		else							sprintf3(sname2 ,"@MFONTG%d", no );
	}
	if ( flag & 2 ) ExtCharGaijiMini = LoadExtFontKanaMini_sub( sname2, (char*)ExtKanaFontmini, 2 );
	CB_G1MorG3M=mode;
}

void LoadExtFontAnkfolder( int flag, char* sname, int folder, int no ){		// LFONTK.bmp -> font 18x24
	char sname2[32];
	int mode=CB_G1MorG3M;
	strcpy( sname2, sname );
	
	if ( EnableExtFont==0 ) return ;
	ExtCharAnkFX = 1;
	ExtCharAnkMiniFX = 1;
	CB_G1MorG3M=1;
	if ( sname[0]=='\0' ) {
			if ( ( no < 0 )||( 9 < no ) )	sprintf3(sname2 ,"%sFONTA8L",   fontfolderCG[folder] );
			else							sprintf3(sname2 ,"%sFONTA8L%d", fontfolderCG[folder], no );
	}
	if ( flag & 1 ) ExtCharAnkFX = LoadExtFontKana_sub( sname2, (char*)ExtAnkFontFX, 6 );
	
	if ( sname[0]=='\0' ) {
			if ( ( no < 0 )||( 9 < no ) )	sprintf3(sname2 ,"%sFONTA6M",   fontfolderCG[folder] );
			else							sprintf3(sname2 ,"%sFONTA6M%d", fontfolderCG[folder], no);
	}
	if ( flag & 2 ) ExtCharAnkMiniFX = LoadExtFontKanaMini_sub( sname2, (char*)ExtAnkFontFXmini, 6 );

	CB_G1MorG3M=3;
	if ( sname[0]=='\0' ) {
			if ( ( no < 0 )||( 9 < no ) )	sprintf3(sname2 ,"%s@LFONTA",   fontfolderCG[folder] );
			else							sprintf3(sname2 ,"%s@LFONTA%d", fontfolderCG[folder] , no );
	}
	if ( flag & 1 ) ExtCharAnk = LoadExtFontKana_sub( sname2, (char*)ExtAnkFont, 6 );
	
	if ( sname[0]=='\0' ) {
			if ( ( no < 0 )||( 9 < no ) )	sprintf3(sname2 ,"%s@MFONTA",   fontfolderCG[folder] );
			else							sprintf3(sname2 ,"%s@MFONTA%d", fontfolderCG[folder] , no );
	}
	if ( flag & 2 ) ExtCharAnkMini = LoadExtFontKanaMini_sub( sname2, (char*)ExtAnkFontmini, 6 );
	CB_G1MorG3M=mode;
}

void LoadExtFontKana( int flag, char* sname, int no ){		// LFONTK.bmp -> font 18x24
	LoadExtFontKanafolder(flag, sname, 0, no );
}
void LoadExtFontGaiji( int flag, char* sname, int no ){		// LFONTK.bmp -> font 18x24
	LoadExtFontGaijifolder( flag, sname, 0, no );
}
void LoadExtFontAnk( int flag, char* sname, int no ){		// LFONTK.bmp -> font 18x24
	LoadExtFontAnkfolder( flag, sname, 0, no );
}

//------------------------------------------------------------------------------
void FontCopy2( char *font,  int reg,  int mx, int my, int dx, int dy ) {	// font  1bit data  -> Mat[x,y]
	int i,j;
	int x,y;
	int xbyte;
	
	xbyte= (dx+7)/8;
	for ( y=0; y<dy; y++ ) {	//
		for ( x=0; x<dx; x++ ) {
			WriteMatrixInt( reg, x+mx, y+my , ( font[y*xbyte+(x>>3)] & (128>>(x&7)) ) != 0 );
		}
	}
}

void SaveExtFontKana_sub( char* sname, char *font, int line, int check ){		// LFONTK.bmp -> font 18x24
	char * FilePtr;
	int ptr=0,size=0,bit;
	int width,height;
	int i,x,y,k,kk;
	int reg=Mattmpreg;
	int bfOffBits = 0x3E;
	int bfSize;
	char *bmpbuffer;
	int base=0;
	int ElementSize=1;
	
	if ( CB_G1MorG3M==1 ) { k=1; kk=8; } else { k=3; kk=24*3; }
	width =6*k*16;
	height=8*k*line;
	bit=1;
	DimMatrixSub( reg, ElementSize, width, height, base ) ;
	if ( ErrorNo ) return ; // error

	i=0;
	for ( y=0; y<line; y++ ) {
		for ( x=0; x<16; x++ ) {
			FontCopy2( font+i, reg, x*6*k,y*8*k,6*k,8*k);	// Fontdata -> Mat
			i+=(kk);
		}
	}
	
	bfSize = ((((width+7)/8)+3)/4)*4 *height +bfOffBits;	//  1bit
	bmpbuffer = ( char *)HiddenRAM_mallocTemp( bfSize+4 );
	if( bmpbuffer == NULL ) { CB_Error(NotEnoughMemoryERR); return ; }	// Not enough memory error
	memset( bmpbuffer, 0x00, bfSize+4 );
	WriteBmpHeader( bmpbuffer, width, height, bit );
	EncodeMat2Bmp( reg , bmpbuffer+bfOffBits , width, height, bit );	// Mat -> bmpbuffer
	CB_SaveSub( sname, bmpbuffer, bfSize, check, "bmp" );
	HiddenRAM_freeTemp( bmpbuffer );	// free
}

void SaveExtFontKanaMini_sub( char* sname, char *font, int line, int check ){			// MFONTK.bmp -> font ?x18
	char * FilePtr;
	int ptr=0,size,bit;
	int width,height;
	int i,x,y,dx,mx,k;
	int reg=Mattmpreg;
	int bfOffBits = 0x3E;
	int bfSize;
	char *bmpbuffer;
	int base=0;
	int ElementSize=1;

	if ( CB_G1MorG3M==1 ) { 
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
				FontCopy2( font+2, reg, mx, y*8+2, dx, 6);	// Fontdata -> Mat
				mx += dx;
				font += (2+6);
			}
		}
	} else {
		width= 16*17+2;
		height=19*line;
		bit=1;
		DimMatrixSub( reg, ElementSize, width, height, base ) ;
		if ( ErrorNo ) return ; // error

		for ( y=0; y<line; y++ ) {
			mx=0;
			WriteMatrixInt( reg, 0, y*19 , 1 );
			for ( x=0; x<16; x++ ) {
				WriteMatrixInt( reg, mx+1, y*19 , 0 );
				dx = *(font);
				if ( dx<4  ) dx=4;
				if ( dx>16 ) dx=16;
				WriteMatrixInt( reg, mx+dx,   y*19 , 1 );
				WriteMatrixInt( reg, mx+dx+1, y*19 , 1 );
				FontCopy2( font+1, reg, mx, y*19+1, dx, 18);	// Fontdata -> Mat
				font+=(1+18*2);
				mx+=dx;
			}
		}
	}
	bfSize = ((((width+7)/8)+3)/4)*4 *height +bfOffBits;	//  1bit
	bmpbuffer = ( char *)HiddenRAM_mallocTemp( bfSize+4 );
	if( bmpbuffer == NULL ) { CB_Error(NotEnoughMemoryERR); return ; }	// Not enough memory error
	memset( bmpbuffer, 0x00, bfSize+4 );
	WriteBmpHeader( bmpbuffer, width, height, bit );
	EncodeMat2Bmp( reg , bmpbuffer+bfOffBits , width, height, bit );	// Mat -> bmpbuffer
	CB_SaveSub( sname, bmpbuffer, bfSize, check, "bmp" );
	HiddenRAM_freeTemp( bmpbuffer );	// free
}

void SaveExtFontKana( int flag, char* sname, int folder, int no, int check ){		// font 18x24 -> LFONTK.bmp
	char sname2[32];
	if ( EnableExtFont==0 ) return ;
	strcpy( sname2, sname );
	if ( CB_G1MorG3M==1 ) {															// FX font
		if ( sname[0]=='\0' ) {
			if ( ( no < 0 )||( 9 < no ) )	sprintf3(sname2 ,"%sFONTK8L",   fontfolderCG[folder] );
			else							sprintf3(sname2 ,"%sFONTK8L%d", fontfolderCG[folder], no );
		}
		if ( ( flag & 1 ) && ( ExtCharKanaFX ) ) SaveExtFontKana_sub( sname2, (char*)ExtKanaFontFX+(8)*32, 5, check );
		
		if ( sname[0]=='\0' ) {
			if ( ( no < 0 )||( 9 < no ) )	sprintf3(sname2 ,"%sFONTK6M",   fontfolderCG[folder] );
			else							sprintf3(sname2 ,"%sFONTK6M%d", fontfolderCG[folder], no);
		}
		if ( ( flag & 2 ) && ( ExtCharKanaMiniFX ) ) SaveExtFontKanaMini_sub( sname2, (char*)ExtKanaFontFXmini+(1+7)*32, 5, check );
		
	} else {																		// CG font
		if ( sname[0]=='\0' ) {
			if ( ( no < 0 )||( 9 < no ) )	sprintf3(sname2 ,"%s@LFONTK",   fontfolderCG[folder] );
			else							sprintf3(sname2 ,"%s@LFONTK%d", fontfolderCG[folder] , no );
		}
		if ( ( flag & 1 ) && ( ExtCharKana ) ) SaveExtFontKana_sub( sname2, (char*)ExtKanaFont+(24*3)*32, 5, check );
		
		if ( sname[0]=='\0' ) {
			if ( ( no < 0 )||( 9 < no ) )	sprintf3(sname2 ,"%s@MFONTK",   fontfolderCG[folder] );
			else							sprintf3(sname2 ,"%s@MFONTK%d", fontfolderCG[folder] , no );
		}
		if ( ( flag & 2 ) && ( ExtCharKanaMini ) ) SaveExtFontKanaMini_sub( sname2, (char*)ExtKanaFontmini+(1+18*2)*32, 5, check );
		
	}
}

void SaveExtFontGaiji( int flag, char* sname, int folder, int no, int check ){		// font 18x24 -> LFONTG.bmp
	char sname2[32];
	if ( EnableExtFont==0 ) return ;
	strcpy( sname2, sname );
	if ( CB_G1MorG3M==1 ) {															// FX font
		if ( sname[0]=='\0' ) {
			if ( ( no < 0 )||( 9 < no ) )	sprintf3(sname2 ,"%sFONTG8L",   fontfolderCG[folder] );
			else							sprintf3(sname2 ,"%sFONTG8L%d", fontfolderCG[folder], no );
		}
		if ( ( flag & 1 ) && ( ExtCharGaijiFX ) ) SaveExtFontKana_sub( sname2, (char*)ExtKanaFontFX, 2, check );
		
		if ( sname[0]=='\0' ) {
			if ( ( no < 0 )||( 9 < no ) )	sprintf3(sname2 ,"%sFONTG6M",   fontfolderCG[folder] );
			else							sprintf3(sname2 ,"%sFONTG6M%d", fontfolderCG[folder], no);
		}
		if ( ( flag & 2 ) && ( ExtCharGaijiMiniFX ) ) SaveExtFontKanaMini_sub( sname2, (char*)ExtKanaFontFXmini, 2, check );
		
	} else {																		// CG font
		if ( sname[0]=='\0' ) {
			if ( ( no < 0 )||( 9 < no ) )	sprintf3(sname2 ,"%s@LFONTG",   fontfolderCG[folder] );
			else							sprintf3(sname2 ,"%s@LFONTG%d", fontfolderCG[folder] , no );
		}
		if ( ( flag & 1 ) && ( ExtCharGaiji ) ) SaveExtFontKana_sub( sname2, (char*)ExtKanaFont, 2, check );
		
		if ( sname[0]=='\0' ) {
			if ( ( no < 0 )||( 9 < no ) )	sprintf3(sname2 ,"%s@MFONTG",   fontfolderCG[folder] );
			else							sprintf3(sname2 ,"%s@MFONTG%d", fontfolderCG[folder] , no );
		}
		if ( ( flag & 2 ) && ( ExtCharGaijiMini ) ) SaveExtFontKanaMini_sub( sname2, (char*)ExtKanaFontmini, 2, check );
	}
}

void SaveExtFontAnk( int flag, char* sname, int folder, int no, int check ){		// font 18x24 -> LFONTA.bmp
	char sname2[32];
	if ( EnableExtFont==0 ) return ;
	strcpy( sname2, sname );
	if ( CB_G1MorG3M==1 ) {															// FX font
		if ( sname[0]=='\0' ) {
			if ( ( no < 0 )||( 9 < no ) )	sprintf3(sname2 ,"%sFONTA8L",   fontfolderCG[folder] );
			else							sprintf3(sname2 ,"%sFONTA8L%d", fontfolderCG[folder], no );
		}
		if ( ( flag & 1 ) && ( ExtCharAnkFX ) ) SaveExtFontKana_sub( sname2, (char*)ExtAnkFontFX, 6, check );
		
		if ( sname[0]=='\0' ) {
			if ( ( no < 0 )||( 9 < no ) )	sprintf3(sname2 ,"%sFONTA6M",   fontfolderCG[folder] );
			else							sprintf3(sname2 ,"%sFONTA6M%d", fontfolderCG[folder], no);
		}
		if ( ( flag & 2 ) && ( ExtCharAnkMiniFX ) ) SaveExtFontKanaMini_sub( sname2, (char*)ExtAnkFontFXmini, 6, check );
		
	} else {																		// CG font
		if ( sname[0]=='\0' ) {
			if ( ( no < 0 )||( 9 < no ) )	sprintf3(sname2 ,"%s@LFONTA",   fontfolderCG[folder] );
			else							sprintf3(sname2 ,"%s@LFONTA%d", fontfolderCG[folder] , no );
		}
		if ( ( flag & 1 ) && ( ExtCharAnk ) ) SaveExtFontKana_sub( sname2, (char*)ExtAnkFont, 6, check );
		
		if ( sname[0]=='\0' ) {
			if ( ( no < 0 )||( 9 < no ) )	sprintf3(sname2 ,"%s@MFONTA",   fontfolderCG[folder] );
			else							sprintf3(sname2 ,"%s@MFONTA%d", fontfolderCG[folder] , no );
		}
		if ( ( flag & 2 ) && ( ExtCharAnkMiniFX ) ) SaveExtFontKanaMini_sub( sname2, (char*)ExtAnkFontmini, 6, check );
	}
}


}
