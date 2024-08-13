extern "C" {

#include "prizm.h"
#include "CBP.h"


struct st_round CB_Round = { Norm , 1, 2} ; // Round
char ENG=0;	// ENG flag

//---------------------------------------------------------------------------------------------
int CB_PrintC2( int px, int py, unsigned char *str, int extflag ){	// extflag 0x1000:fx6*8fontmode  0x100:ExtAnkChar
	int color=CB_ColorIndex,i;
	if ( color < 0 ) CB_ColorIndex=0x0000;
	i=KPrintCharSub( px, py, str, MINI_OVER | extflag ); 
	CB_ColorIndex=color;
	return i;
}
int CB_PrintRevC2( int px, int py, unsigned char *str, int extflag ){	// extflag 0x1000:fx6*8fontmode  0x100:ExtAnkChar
	int color=CB_ColorIndex,i;
	if ( color < 0 ) CB_ColorIndex=0x0000;
	i=KPrintCharSub( px, py, str, MINI_REV | extflag ); 
	CB_ColorIndex=color;
	return i;
}

void CB_Prints2( int px, int py, unsigned char *str, int extflag ){
	int i;
	while ( *str ) {
		i=CB_PrintC2( px, py, str, extflag );
		str += OpCodeLenGB2( (char*)str );
		px  += i;
		if ( px>378 ) break;
	}
}
void CB_PrintRevs2( int px, int py, unsigned char *str, int extflag ){
	int i;
	while ( *str ) {
		i=CB_PrintRevC2( px, py, str, extflag );
		str += OpCodeLenGB2( (char*)str );
		px += i;
		if ( px>378 ) break;
	}
}
void CB_PrintLocateXY( int px, int py, unsigned char *str, int mode, int extflag){	// extflag 0x1000:fx6*8fontmode  0x100:ExtAnkChar
	switch ( mode ) {
		case 0:
			CB_Prints2( px, py ,str, extflag);
			break;
		case 1:
			CB_PrintRevs2( px, py ,str, extflag);
			break;
	}
}
//-----------------------------------------------------------------------------
int CB_PrintC( int x, int y, unsigned char *cstr ){
	return CB_PrintC2( (x-1)*3*6, (y-1)*3*8 , cstr, 0 );
}
int CB_PrintC_ext( int x, int y, unsigned char *cstr, int extflag ){
	return CB_PrintC2( (x-1)*3*6, (y-1)*3*8 , cstr, extflag );
}
void CB_Prints( int x, int y, unsigned char *str ){
	unsigned int c=*str;
	int px,py;
	int tmp=CB_G1MorG3M,i;
	int bk_GBcode=GBcode;
	GBcode=0;
	CB_G1MorG3M=3;
	px=(x-1)*18; py=(y-1)*24;
	CB_Prints2( px, py, str, 0 );
	CB_G1MorG3M=tmp;
	GBcode=bk_GBcode;
}
void CB_Prints_ext( int x, int y, unsigned char *str, int extflag ){
	unsigned int c=*str;
	int i;
	int px,py;
	px=(x-1)*18; py=(y-1)*24;
	CB_Prints2( px, py, str, extflag );
}

void CB_PrintXYCx3( int px, int py, unsigned char *c , int mode ){	// {128,64}
	unsigned short color,backcolor=CB_BackColorIndex;
	if ( CB_ColorIndex >= 0 ) color=CB_ColorIndex; else color=TEXT_COLOR_BLACK;
	PrintCXY1( px*3, py*3, c, 0, -1, color, backcolor, 1, 0 );
}
void CB_PrintXYx3( int px, int py, unsigned char *str, int mode ){	// {128,64}
	while ( *str ) {
		CB_PrintXYCx3( px, py, str , mode);
		str += OpCodeLenGB2( (char*)str );
		px  += 6;
		if ( px>127 ) break;
	}
}
void CB_PrintXYC( int px, int py,unsigned char *c , int mode ){
	unsigned short color,backcolor=CB_BackColorIndex;
	if ( CB_ColorIndex >= 0 ) color=CB_ColorIndex; else color=TEXT_COLOR_BLACK;
	PrintCXY1( px, py, c, 0, -1, color, backcolor, 1, 0 );
}
void CB_PrintXY( int px, int py, unsigned char *str, int mode){
	while ( *str ) {
		CB_PrintXYC( px, py, str , mode);
		str += OpCodeLenGB2( (char*)str );
		px  += 18;
		if ( px>383 ) break;
	}
}

//---------------------------------------------------------------------------------------------
int CB_PrintMiniBackColor(  int mode, int *modef, int *dy, int ddy ) ;

//  mode : xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx
//           1:75%
//            1:GBfont
//                  11111111:height
//  mode : xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx
//                           0001:fxfontmode
//                               0001:extflag
//                                    11111111:mode

int CB_PrintModeC( int px, int py, unsigned char *str, int mode){	// {384,192} mode char
	int k,i,j;
    int modef,dy;
	int color=CB_PrintMiniBackColor( mode, &modef, &dy, 24 );
	i=KPrintCharSub( px, py, str, mode );
	if ( (dy>24) && ( (modef==MINI_OVER) || (modef==MINI_REV) ) ) {
		ML_rectangle_CG( px, py+24, px+i-1, py+dy-1, 0, 0, 0, color, color );
	}
	return i;
}
void CB_PrintMode( int px, int py, unsigned char *str, int mode, int xlimit ){	// {384,192} mode string
	unsigned char mod,kind;
	int i;
	while ( *str ) {
		i=CB_PrintModeC( px, py, str, mode );
		str += OpCodeLenGB2( (char*)str );
		px  += i;
		if ( px > xlimit ) break;
	}
}

int CB_PrintLength( unsigned char *str, int extflag ){
	int X=0,Y=0;
	X=KPrintCharSub(  0, 0, str, 0x21+ 0x100*(extflag!=0) ); 
	return X;
}
int CB_PrintLengthStr( unsigned char *str, int extflag ){
	int i,length=0;
	while ( *str ) {
		i=CB_PrintLength( str, extflag );
		str += OpCodeLenGB2( (char*)str );
		length += i;
	}
	return length;
}
//---------------------------------------------------------------------------------------------
void PrintMiniXY(int X, int Y, char*buf, int COLOR,int BackCOLOR, int extflag, int xlength ) {	// xlength :57 fkeyicon 
	int k,i,j;
	int color=CB_ColorIndex;
	unsigned short backcolor=CB_BackColorIndex;
	if ( COLOR < 0 ) COLOR=0;
	CB_ColorIndex=COLOR;
	CB_BackColorIndex=BackCOLOR;
	j=CB_PrintMiniLengthStr( (unsigned char *)buf, 0x100*(extflag!=0) );
	i=(xlength +j)/2-j; if ( i<0 ) i=0;
	CB_PrintMini( X+i, Y, (unsigned char *)buf, MINI_OR | 0x100*(extflag!=0) );	// extflag on
	CB_ColorIndex=color;
	CB_BackColorIndex=backcolor;
}

int CB_PrintMiniBackColor(  int mode, int *modef, int *dy, int ddy ) {
	int color=CB_BackColorIndex;
	*modef=mode&0xFF;
	*dy=(mode>>16)&0xFF;
	if( *modef==MINI_REV ) {
		if ( CB_ColorIndex >= 0 ) color=CB_ColorIndex; else color=CB_ColorIndexPlot;
	}
	if ( *dy<=0 ) *dy=ddy;
	return color;
}

int CB_PrintMiniC( int px, int py, unsigned char *str, int mode){	// {384,192} mode char
	int k,i,j;
    int modef,dy;
	int color=CB_PrintMiniBackColor( mode, &modef, &dy, 18 );
	i=KPrintCharMiniCG( px, py, str , mode );
	if ( (dy>18) && ( (modef==MINI_OVER) || (modef==MINI_REV) ) ) {
		ML_rectangle_CG( px, py+18, px+i-1, py+dy-1, 0, 0, 0, color, color );
	}
	return i;
}
void CB_PrintMini( int px, int py, unsigned char *str, int mode){	// {384,192} mode string
	unsigned char mod,kind;
	int i;
	while ( *str ) {
		i=CB_PrintMiniC( px, py, str, mode );
		str += OpCodeLenGB2( (char*)str );
		px  += i;
		if ( px>383 ) break;
	}
}
void CB_PrintMini_wx( int px, int py, unsigned char *str, int mode, int xlim ){	// {384,192} mode string
	unsigned char mod,kind;
	int i;
	while ( *str ) {
		i=CB_PrintMiniLength( str, mode & 0xFF00 );
		if ( px+i > xlim ) break;
		CB_PrintMiniC( px, py, str, mode );
		str += OpCodeLenGB2( (char*)str );
		px  += i;
		if ( px>383 ) break;
	}
}

int CB_PrintMiniC_Fix( int px, int py, unsigned char *str, int mode){	// {384,192} mode char Fixed pitch 12dot
	int k,i,j;
    int modef,dy;
	int color=CB_PrintMiniBackColor( mode, &modef, &dy, 18 );
	j=CB_PrintMiniLength( str, mode & 0xFF00 );
	i=(12+j)/2-j; if ( i<0 ) i=0;
	if ( (modef==MINI_OVER) || (modef==MINI_REV) ) ML_rectangle_CG( px, py, px+11, py+dy-1, 0, 0, 0, color, color );
	px+=i;
	if ( j>12 ) px--;
	KPrintCharMiniCG( px, py, str, mode | 0x20000000 );
	return j;
}
void CB_PrintMini_Fix( int px, int py, unsigned char *str, int mode){	// {384,192} mode string Fixed pitch 12dot
	unsigned char mod,kind;
	int i;
	while ( *str ) {
		i=CB_PrintMiniC_Fix( px, py, str, mode );
		str += OpCodeLenGB2( (char*)str );
		px  += 12;
		if ( px>383 ) break;
	}
}
int CB_PrintMiniC_Fix10( int px, int py, unsigned char *str, int mode){	// {384,192} mode char Fixed pitch 10dot
	int k,i,j;
	int c=*str;
    int modef,dy;
	int color=CB_PrintMiniBackColor( mode, &modef, &dy, 18 );
	if ( (modef==MINI_OVER) || (modef==MINI_REV) ) ML_rectangle_CG( px, py, px+9, py+dy-1, 0, 0, 0, color, color );
	KPrintCharMiniCG( px, py, str, mode );
	k=10; 
	if ( c=='1' ) k=8;
	if ( c=='/' ) k=6;
	if ( c==' ' ) k=6;
	return  k;
}
void CB_PrintMini_Fix10( int px, int py, unsigned char *str, int mode) {	// {384,192} mode string Fixed pitch 10dot
	unsigned char mod,kind;
	int i;
	while ( *str ) {
		i=CB_PrintMiniC_Fix10( px, py, str, mode );
		str += OpCodeLenGB2( (char*)str );
		px  += i;
		if ( px>383 ) break;
	}
}

int CB_PrintMiniC_fx( int px, int py, unsigned char *str, int mode){	// {384,192} mode string fx font
	int k,i,j;
    int modef,dy;
	int color=CB_PrintMiniBackColor( mode, &modef, &dy, 8 );
	i=KPrintCharMiniFX( px, py, str , mode );
	if ( (dy>8) && ( (modef==MINI_OVER) || (modef==MINI_REV) ) ) {
		ML_rectangle_CG( px, py+8, px+5, py+dy-1, 0, 0, 0, color, color );
	}
	return i;
}
void CB_PrintMini_fx( int px, int py, unsigned char *str, int mode){	// {384,192} mode string fx font
	unsigned char mod,kind;
	int i;
	while ( *str ) {
		i=KPrintCharMini( px, py, str , mode );
		str += OpCodeLenGB2( (char*)str );
		px  += i;
		if ( px>383 ) break;
	}
}

int CB_PrintMiniCx3( int px, int py, unsigned char *str, int mode){	// {128,64} mode	fx mode char
	int k,i=0;
	i=KPrintCharMini( px*3, py*3, str, mode )/3;
	return i;
}
void CB_PrintMinix3( int px, int py, unsigned char *str, int mode){	// {128,64} mode	fx mode string
	unsigned char mod,kind;
	int i;
	while ( *str ) {
		i=CB_PrintMiniCx3( px, py, (unsigned char *)str, mode );
		str += OpCodeLenGB2( (char*)str );
		px  += i;
		if ( px>127 ) break;
	}
}

int CB_PrintMiniLength( unsigned char *str, int extflag ){
	int X=0,Y=0;
	int k,i=0;
	X=KPrintCharMiniCG( 0, 0, str , 0x21+ 0x100*(extflag!=0) );
	return X;
}
int CB_PrintMiniLengthStr( unsigned char *str, int extflag ){
	int i,length=0;
	while ( *str ) {
		i=CB_PrintMiniLength( str, extflag );
		str += OpCodeLenGB2( (char*)str );
		length += i;
	}
	return length;
}

int CB_PrintMiniLength_fxed( unsigned char *str, int extflag ){
	int X=0,Y=0;
	int k,i=0;
	X=KPrintCharMiniFX( 0, 0, str , 0x21+ 0x100*(extflag!=0) );
	return X;
}
int CB_PrintMiniLengthStr_fxed( unsigned char *str, int extflag ){
	int i,length=0;
	while ( *str ) {
		i=CB_PrintMiniLength_fxed( str, extflag );
		str += OpCodeLenGB2( (char*)str );
		length += i;
	}
	return length;
}
//------------------------------------------------------------------	// minimini font
int CB_PrintMiniMiniC( int px, int py, unsigned char *str, int mode){	// {384,192} mode char   MiniMini font
	int k,i,j;
    int modef,dy;
	int color=CB_PrintMiniBackColor( mode, &modef, &dy, 10 );
	i=KPrintCharMiniMini( px, py, str , mode );
	if ( (dy>10) && ( (modef==MINI_OVER) || (modef==MINI_REV) ) ) {
		ML_rectangle_CG( px, py+10, px+i-1, py+dy-1, 0, 0, 0, color, color );
	}
	return i;
}
void CB_PrintMiniMini( int px, int py, unsigned char *str, int mode){	// {384,192} mode string MiniMini font
	unsigned char mod,kind;
	int i;
	while ( *str ) {
		i=CB_PrintMiniMiniC( px, py, str, mode );
		str += OpCodeLenGB2( (char*)str );
		px  += i;
		if ( px>383 ) break;
	}
}
void CB_PrintMiniMiniorWB( int X, int Y, unsigned char*buf ) {
	int color=CB_ColorIndex;
	unsigned short backcolor=CB_BackColorIndex;
	CB_ColorIndex    =0xFFFF;	// white
	CB_BackColorIndex=0x0000;	// black
	CB_PrintMiniMini( X+1, Y+1, buf, MINI_OR ) ;
	CB_ColorIndex    =0x0000;	// black
	CB_BackColorIndex=0xFFFF;	// white
	CB_PrintMiniMini( X, Y, buf, MINI_OR ) ;
	CB_ColorIndex=color;
	CB_BackColorIndex=backcolor;
}

int CB_PrintMiniMiniBoldC( int px, int py, unsigned char *str, int mode){	// {384,192} mode char   MiniMini font Bold
	int k,i,j;
    int modef,dy;
	int color=CB_PrintMiniBackColor( mode, &modef, &dy, 10 );
	i=KPrintCharMiniMiniBold( px, py, str , mode );
	if ( (dy>10) && ( (modef==MINI_OVER) || (modef==MINI_REV) ) ) {
		ML_rectangle_CG( px, py+10, px+i-1, py+dy-1, 0, 0, 0, color, color );
	}
	return i;
}
void CB_PrintMiniMiniBold( int px, int py, unsigned char *str, int mode){	// {384,192} mode string MiniMini font Bold
	unsigned char mod,kind;
	int i;
	while ( *str ) {
		i=CB_PrintMiniMiniBoldC( px, py, str, mode );
		str += OpCodeLenGB2( (char*)str );
		px  += i;
		if ( px>383 ) break;
	}
}

int CB_PrintMiniMiniC_Fix( int px, int py, unsigned char *str, int mode){	// {384,192} mode char Fixed pitch 8dot
	int k,i=0,j;
    int modef,dy;
	int color=CB_PrintMiniBackColor( mode, &modef, &dy, 10 );
	j=CB_PrintMiniMiniLength( str );
	i=(8+j)/2-j; if ( i<0 ) i=0;
	if ( (modef==MINI_OVER) || (modef==MINI_REV) ) ML_rectangle_CG( px, py, px+7, py+dy-1, 0, 0, 0, color, color );
	px+=i;
	if ( j>8 ) px--;
	CB_PrintMiniMiniC( px, py, str, mode );
	return 8;
}
void CB_PrintMiniMini_Fix( int px, int py, unsigned char *str, int mode){	// {384,192} mode string Fixed pitch 8dot
	unsigned char mod,kind;
	int i;
	while ( *str ) {
		i=CB_PrintMiniMiniC_Fix( px, py, str, mode );
		str += OpCodeLenGB2( (char*)str );
		px  += i;
		if ( px>383 ) break;
	}
}
int CB_PrintMiniMiniC_Fix7( int px, int py, unsigned char *str, int mode){	// {384,192} mode char Fixed pitch 7dot
	int k,i=0,j;
    int modef,dy;
	int color=CB_PrintMiniBackColor( mode, &modef, &dy, 10 );
	j=CB_PrintMiniMiniLength( str );
	i=(7+j)/2-j; if ( i<0 ) i=0;
	if ( (modef==MINI_OVER) || (modef==MINI_REV) ) ML_rectangle_CG( px, py, px+6, py+dy-1, 0, 0, 0, color, color );
	px+=i;
	if ( j>7 ) px--;
	CB_PrintMiniMiniC( px, py, str, mode );
	return 7;
}
void CB_PrintMiniMini_Fix7( int px, int py, unsigned char *str, int mode){	// {384,192} mode string Fixed pitch 8dot
	unsigned char mod,kind;
	int i;
	while ( *str ) {
		i=CB_PrintMiniMiniC_Fix7( px, py, str, mode );
		str += OpCodeLenGB2( (char*)str );
		px  += i;
		if ( px>383 ) break;
	}
}
int CB_PrintMiniMiniBoldC_Fix( int px, int py, unsigned char *str, int mode){	// {384,192} mode char Fixed pitch 9dot
	int k,i=0,j;
    int modef,dy;
	int color=CB_PrintMiniBackColor( mode, &modef, &dy, 10 );
	j=CB_PrintMiniMiniBoldLength( str );
	i=(9+j)/2-j; if ( i<0 ) i=0;
	if ( (modef==MINI_OVER) || (modef==MINI_REV) ) ML_rectangle_CG( px, py, px+8, py+dy-1, 0, 0, 0, color, color );
	px+=i;
	if ( j>9 ) px--;
	CB_PrintMiniMiniBoldC( px, py, str, mode );
	return 9;
}
void CB_PrintMiniMiniBold_Fix( int px, int py, unsigned char *str, int mode){	// {384,192} mode string Fixed pitch 9dot
	unsigned char mod,kind;
	int i;
	while ( *str ) {
		i=CB_PrintMiniMiniBoldC_Fix( px, py, str, mode );
		str += OpCodeLenGB2( (char*)str );
		px  += i;
		if ( px>383 ) break;
	}
}

int CB_PrintMiniMiniLength( unsigned char *str ){
	int X=0,Y=0;
	int k,i=0;
	X=KPrintCharMiniMini( 0, 0, str , 0x21 );
	return X;
}
int CB_PrintMiniMiniLengthStr( unsigned char *str ){
	int i,length=0;
	while ( *str ) {
		i=CB_PrintMiniMiniLength( str );
		str += OpCodeLenGB2( (char*)str );
		length+=i;
	}
	return length;
}
int CB_PrintMiniMiniBoldLength( unsigned char *str ){
	int X=0,Y=0;
	int k,i=0;
	X=KPrintCharMiniMiniBold( 0, 0, str , 0x21 );
	return X;
}
int CB_PrintMiniMiniBoldLengthStr( unsigned char *str ){
	int i,length=0;
	while ( *str ) {
		i=CB_PrintMiniMiniBoldLength( str );
		str += OpCodeLenGB2( (char*)str );
		length+=i;
	}
	return length;
}

//------------------------------------------------------------------	// minimini 6x8 fx font
int CB_PrintMiniMiniFXC( int px, int py, unsigned char *str, int mode){	// {384,192} mode char   6x8dot FX font
	int k,i=0;
    int modef,dy;
	int color=CB_PrintMiniBackColor( mode, &modef, &dy, 8 );
	if ( (dy>8) && ( (modef==MINI_OVER) || (modef==MINI_REV) ) ) ML_rectangle_CG( px, py+8, px+5, py+dy-1, 0, 0, 0, color, color );
	i=KPrintCharMiniMiniFX( px, py, str , mode );
	return i;
}
void CB_PrintMiniMiniFX( int px, int py, unsigned char *str, int mode){	// {384,192} mode string 6x8dot FX font
	unsigned char mod,kind;
	int i;
	while ( *str ) {
		i=CB_PrintMiniMiniFXC( px, py, str, mode );
		str += OpCodeLenGB2( (char*)str );
		px+=i;
		if ( px>383 ) break;
	}
}
//------------------------------------------------------------------	// mini/minimini font editor only

int CB_PrintMiniC_ed( int px, int py, unsigned char *str, int mode ){	// {384,192} mode char
	int i,j;
	int color,modef=mode&0xFF;
    int dy = (mode>>16)&0xFF;
    int escgb = (mode>>28)<<28;
	if ( ( modef==MINI_OVER ) ) {
		j=CB_PrintMiniLength( str, mode & 0xFF00 );
		ML_rectangle_CG( px, py, px+j, py+dy-1, 0, 0, 0, CB_BackColorIndex, CB_BackColorIndex );
		mode=MINI_OR | escgb;
	}
	i=KPrintCharMiniCG( px, py, str , mode );
	return i;
}
int CB_PrintMiniC_Fix_ed( int px, int py, unsigned char *str, int mode ){	// {384,192} mode char Fixed pitch 12dot
	int k,i,j;
	int color,modef=mode&0xFF;
    int dy = (mode>>16)&0xFF;
    int escgb = (mode>>28)<<28;
	j=CB_PrintMiniLength( str, mode & 0xFF00 );
	i=(12+j)/2-j; if ( i<0 ) i=0;
	if ( ( modef==MINI_OVER ) ) {
		ML_rectangle_CG( px, py, px+11, py+dy-1, 0, 0, 0, CB_BackColorIndex, CB_BackColorIndex );
		mode=MINI_OR | escgb;
	}
	px+=i;
	if ( j>12 ) px--;
	KPrintCharMiniCG( px, py, str, mode | 0x20000000 );
	return 12;
}
int CB_PrintMiniC_fx_ed( int px, int py, unsigned char *str, int mode ){	// {384,192} mode string fx font
	int k,i=0;
	i=KPrintCharMiniFX( px, py, str , mode );
	return i;
}

int CB_PrintMiniMiniC_ed( int px, int py, unsigned char *str, int mode ){	// {384,192} mode char   MiniMini font
	int k,i=0,j;
	int color,modef=mode&0xFF;
    int dy = (mode>>16)&0xFF;
    int escgb = (mode>>28)<<28;
	if ( ( modef==MINI_OVER ) ) {
		j=CB_PrintMiniMiniLength( str );
		ML_rectangle_CG( px, py, px+j, py+dy-1, 0, 0, 0, CB_BackColorIndex, CB_BackColorIndex );
		mode=MINI_OR | escgb;
	}
	i=KPrintCharMiniMini( px, py, str , mode );
	return i;
}

int CB_PrintMiniMiniBoldC_ed( int px, int py, unsigned char *str, int mode ){	// {384,192} mode char   MiniMini font Bold
	int k,i=0,j;
	int modef=mode&0xFF;
    int dy = (mode>>16)&0xFF;
    int escgb = (mode>>28)<<28;
	if ( ( modef==MINI_OVER ) ) {
		j=CB_PrintMiniMiniBoldLength( str );
		ML_rectangle_CG( px, py, px+j, py+dy-1, 0, 0, 0, CB_BackColorIndex, CB_BackColorIndex );
		mode=MINI_OR | escgb;
	}
	i=KPrintCharMiniMiniBold( px, py, str , mode );
	return i;
}

int CB_PrintMiniMiniC_Fix_ed( int px, int py, unsigned char *str, int mode ){	// {384,192} mode char Fixed pitch 8dot
	int k,i=0,j;
	int modef=mode&0xFF;
    int dy = (mode>>16)&0xFF;
    int escgb = (mode>>28)<<28;
	j=CB_PrintMiniMiniLength( str );
	i=(8+j)/2-j; if ( i<0 ) i=0;
	if ( ( modef==MINI_OVER ) ) {
		ML_rectangle_CG( px, py, px+7, py+dy-1, 0, 0, 0, CB_BackColorIndex, CB_BackColorIndex );
		mode=MINI_OR | escgb;
	}
	px+=i;
	if ( j>8 ) px--;
	CB_PrintMiniMiniC_ed( px, py, str, mode );
	return 8;
}

int CB_PrintMiniMiniBoldC_Fix_ed( int px, int py, unsigned char *str, int mode ){	// {384,192} mode char Fixed pitch 9dot
	int k,i=0,j;
	int modef=mode&0xFF;
    int dy = (mode>>16)&0xFF;
    int escgb = (mode>>28)<<28;
	j=CB_PrintMiniMiniBoldLength( str );
	i=(9+j)/2-j; if ( i<0 ) i=0;
	if ( ( modef==MINI_OVER ) ) {
		ML_rectangle_CG( px, py, px+8, py+dy-1, 0, 0, 0, CB_BackColorIndex, CB_BackColorIndex );
		mode=MINI_OR | escgb;
	}
	px+=i;
	if ( j>9 ) px--;
	CB_PrintMiniMiniBoldC_ed( px, py, str, mode );
	return 9;
}

int CB_PrintMiniMiniFXC_ed( int px, int py, unsigned char *str, int mode ) {	// {384,192} mode char   6x8dot FX font
	int k,i=0;
	int modef=mode&0xFF;
    int dy = (mode>>16)&0xFF;
    int escgb = (mode>>28)<<28;
	if ( ( modef==MINI_OVER ) ) {
		ML_rectangle_CG( px, py, px+5, py+dy-1, 0, 0, 0, CB_BackColorIndex, CB_BackColorIndex );
		mode=MINI_OR | escgb;
	}
	i=KPrintCharMiniMiniFX( px, py, str , mode );
	return i;
}

//---------------------------------------------------------------------------------------------
//void PrintXYR(int x,int y,char *buffer,int rev){
// 	if ( rev ) PrintXY(x,y,(unsigned char *)buffer,1);
//		else   PrintXY(x,y,(unsigned char *)buffer,0);
//}

void Hex8PrintXY(int x, int y, char *str, unsigned int hex){
	char buffer[22];
	sprintf3(buffer,"%08X",hex);
	locate(x,y); Prints((unsigned char *)str); Prints((unsigned char *)buffer);
}
void Hex4PrintXY(int x, int y, char *str, unsigned int hex){
	char buffer[22];
	sprintf3(buffer,"%04X",hex);
	locate(x,y); Prints((unsigned char *)str); Prints((unsigned char *)buffer);
}
void Hex2PrintXY(int x, int y, char *str, unsigned int hex){
	char buffer[22];
	sprintf3(buffer,"%02X",hex);
	locate(x,y); Prints((unsigned char *)str); Prints((unsigned char *)buffer);
}

void ACCheckBreak(){
	if ( BreakCheck ) BreakPtr=ExecPtr;
}

int YesNo2sub( char*buffer, char*buffer2){
	int key;
	int x,y,i,p;
//	short buf[ 330*174 ];
//	short *vram=(short *)PictAry[0];
	i=0;
//	for( y=30; y<203; y++) {
//		p=26+384*y; for ( x=26; x<355; x++) buf[i++]=vram[p++];
//	}
	SaveDisp(SAVEDISP_PAGE2);
	if ( buffer2[0]=='\0' ) {
		PopUpWin(4); 
		y=4;
	} else { 
		PopUpWin(5);
		CB_Prints( 3, 3, (unsigned char *)buffer2);
		y=5;
	}
	CB_Prints( 3, 2,  (unsigned char *)buffer);
	CB_Prints( 3, y,  (unsigned char *) "   Yes:[F1]");
	CB_Prints( 3, y+1,(unsigned char *) "   No :[F6]");

	KeyRecover(); 
	while ( 1 ) {
		GetKey_DisableCatalog( &key );	
		if ( key == KEY_CTRL_F1  ) break ;
		if ( key == KEY_CTRL_F6  ) break ;
		if ( key == KEY_CTRL_EXIT) { WaitKeyEXIT(); break ; }
		if ( key == KEY_CTRL_AC  ) { ACCheckBreak(); break ; }
		if ( key == KEY_CTRL_EXE ) break ;
	}
	RestoreDisp(SAVEDISP_PAGE2);
//	i=0;
//	for( y=30; y<203; y++) {
//		p=26+384*y; for ( x=26; x<355; x++) vram[p++]=buf[i++];
//	}
	KeyRecover(); 
	if ( key == KEY_CTRL_F1  ) return 1 ; // Yes
	if ( key == KEY_CTRL_EXE ) return 1 ; // Yes
//	Bdisp_PutDisp_DD();
	return 0 ; // No
}

int YesNo( char*buffer){
	int r;
	int coloetmp=CB_ColorIndex;
	unsigned short backcoloetmp=CB_BackColorIndex;
	CB_ColorIndex=-1;
	CB_BackColorIndex=0xFFFF;	// white
	EnableDisplayStatusArea();
	r=YesNo2sub( buffer, "" );
	CB_ColorIndex=coloetmp;
	CB_BackColorIndex=backcoloetmp;
	return r;
}
int YesNo2( char*buffer, char*buffer2 ){
	int r;
	int coloetmp=CB_ColorIndex;
	unsigned short backcoloetmp=CB_BackColorIndex;
	CB_ColorIndex=-1;
	CB_BackColorIndex=0xFFFF;	// white
	EnableDisplayStatusArea();
	r=YesNo2sub( buffer, buffer2 );
	CB_ColorIndex=coloetmp;
	CB_BackColorIndex=backcoloetmp;
	return r;
}

int YesNo_NoSave( char*buffer){
	int key;

	PopUpWin(4);
	locate(3,2); Prints((unsigned char *)buffer);
	locate(3,4); Prints((unsigned char *) "   Yes:[F1]");
	locate(3,5); Prints((unsigned char *) "   No :[F6]");
	while ( 1 ) {
		GetKey(&key);
		if ( key == KEY_CTRL_F1  ) break ;
		if ( key == KEY_CTRL_F6  ) break ;
		if ( key == KEY_CTRL_EXIT) break ;
		if ( key == KEY_CTRL_AC  ) { ACCheckBreak(); break ; }
		if ( key == KEY_CTRL_EXE ) break ;
	}
	if ( key == KEY_CTRL_F1  ) return 1 ; // Yes
	if ( key == KEY_CTRL_EXE ) return 1 ; // Yes
	return 0 ; // No
}

void ExitKey(){
	int key;
	KeyRecover(); 
	while ( 1 ) {
		GetKey_DisableCatalog( &key );	
		if ( key == KEY_CTRL_EXIT) { WaitKeyEXIT(); break ; }
		if ( key == KEY_CTRL_AC  ) { ACCheckBreak(); break ; }
	}
}

void OkMSGstr2(char*buffer,char*buffer2){
	char buf[20];
	int y;

	SaveDisp(SAVEDISP_PAGE1);
	if ( buffer2[0]=='\0' ) {
		PopUpWin(4); 
		y=5;
	} else { 
		PopUpWin(5);
		CB_Prints( 3, 4, (unsigned char *)buffer2);
		y=6;
	}
	CB_Prints( 3, 8-y, (unsigned char *)buffer);
	CB_Prints( 3, y, (unsigned char *) "   Press:[EXIT]");

	ExitKey();
	RestoreDisp(SAVEDISP_PAGE1);
}

void ErrorMSGstr1( char*buffer){
	int coloetmp=CB_ColorIndex;
	unsigned short backcoloetmp=CB_BackColorIndex;
	CB_ColorIndex=-1;
	CB_BackColorIndex=0xFFFF;	// white
	OkMSGstr2( buffer, "" );
	CB_ColorIndex=coloetmp;
	CB_BackColorIndex=backcoloetmp;
}
void ErrorMSGstr( char*buffer, char*buffer2 ){
	int coloetmp=CB_ColorIndex;
	unsigned short backcoloetmp=CB_BackColorIndex;
	CB_ColorIndex=-1;
	CB_BackColorIndex=0xFFFF;	// white
	OkMSGstr2( buffer, buffer2 );
	CB_ColorIndex=coloetmp;
	CB_BackColorIndex=backcoloetmp;
}

void ErrorMSG(char*buffer,int err){
	char buf[20];

	SaveDisp(SAVEDISP_PAGE1);
	PopUpWin(5);
	sprintf3(buf,"code:%d",err);
	locate(3,2); Prints((unsigned char *)buffer);
	locate(3,4); Prints((unsigned char *)buf);
	locate(3,6); Prints((unsigned char *) "   Press:[EXIT]");

	ExitKey();
	RestoreDisp(SAVEDISP_PAGE1);
}

void ErrorADRS(char*buffer,int err){
	char buf[20];

	SaveDisp(SAVEDISP_PAGE1);
	PopUpWin(5);
	locate(3,2); Prints((unsigned char *)buffer);
	Hex8PrintXY(3,4,"addres:",err);
	locate(3,6); Prints((unsigned char *) "   Press:[EXIT]");

	ExitKey();
	RestoreDisp(SAVEDISP_PAGE1);
}

void MSG1(char*buffer1){
	SaveDisp(SAVEDISP_PAGE1);
	PopUpWin(1);
	locate(3,4); Prints((unsigned char *)buffer1);
	Bdisp_PutDisp_DD_DrawBusy();
}
void MSG2(char*buffer1,char*buffer2){
	SaveDisp(SAVEDISP_PAGE1);
	PopUpWin(2);
	locate(3,3); Prints((unsigned char *)buffer1);
	locate(3,4); Prints((unsigned char *)buffer2);
	Bdisp_PutDisp_DD_DrawBusy();
}
void MSGpop(void){
//	MsgBoxPop();
	RestoreDisp(SAVEDISP_PAGE1);
	Bdisp_PutDisp_DD();
}

void PopUpWin( int n ){
//	MsgBoxPush( n );
	int y1,y2;
	int	tmp=CB_G1MorG3M;
	int tmp1=CB_BackColorIndex;
	int tmp2=CB_ColorIndex;
	CB_BackColorIndex=0xFFFF;		// Back color index (default White)
	switch ( n ) {
		case 1:
			y1=2;
			y2=4;
			break;
		case 2:
			y1=1;
			y2=4;
			break;
		case 3:
			y1=1;
			y2=5;
			break;
		case 4:
			y1=0;
			y2=5;
			break;
		case 5:
			y1=0;
			y2=6;
			break;
		case 6:
			y1=0;
			y2=7;
			break;
		default:
			return;
	}
	CB_G1MorG3M=1;	// fx style
	CB_ColorIndex=0x001F;	// Blue
	MLV_width=1;	// line width
	ML_rectangle( 9, 8*y1+3, 116, 8*y2+3, 1, 1, 0);
	ML_line(  10, 8*y2+4, 117, 8*y2+4, 1);
	ML_line( 117, 8*y1+4, 117, 8*y2+4, 1);
	CB_G1MorG3M=tmp;
	CB_BackColorIndex=tmp1;
	CB_ColorIndex=tmp2;
}

//---------------------------------------------------------------------------------------------

void ProgressBarPopUp( char *buffer, char *buffer2 ) {
//		MsgBoxPush( 5 );
		ProgressBar2( (unsigned char *)buffer, 0,10);
//		locate(3,2); Prints((unsigned char *)buffer);
//		locate(3,3); Prints((unsigned char *)buffer2);
//		ML_rectangle( 17, 32, 106, 40, 1, 1, 0);
}

void ProgressBar(int current, int max) {
	int i;
	unsigned int t=RTC_GetTicks();
	if ( (current>=max ) || ( abs(t-skip_count)>4 ) ) { skip_count=t;		// default 128/4=32  1/32s
		ProgressBar1( current, max);
		Bdisp_PutDisp_DD_stripe( 5*24+24, 6*24+24 );
	}
}

//---------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------
// refer to https://www.cemetech.net/forum/viewtopic.php?t=11908

#define LCDC *(unsigned short*)(0xB4000000)
#define PORTN *(unsigned char*)(0xA4050138)
int GetBacklightSubLevel(){
  Bdisp_DDRegisterSelect(0x5a1);
  return (LCDC & 0xFF) - 0;
}
//void SetBacklightSubLevel(int level){
//  Bdisp_DDRegisterSelect(0x5a1);
//  LCDC = (level & 0xFF) + 0;
//}

int BackLight( int n ){		// 
	if ( IsEmu ) return 210;	// default value
	if ( n>=0 ) {
		if ( n > 255 ) PORTN |= 0x10; else PORTN &= 0xEF;
		if ( ErrorNo == 0 ) Bdisp_WriteDDRegister5A1( n & 0xFF ); 
	}
	return GetBacklightSubLevel() + ( PORTN & 0x10)*16;
}

//---------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------

char UseHiddenRAM=1;	//	0x11 :HiddenRAMInit off
char IsHiddenRAM=0;

char * HiddenRAM_Top;				// Hidden RAM TOP
char * HiddenRAM_End;				// Hidden RAM END

char * HiddenRAM_ProgNextPtr;		// Hidden RAM Prog next ptr
char * HiddenRAM_MatTopPtr;			// Hidden RAM Mat top ptr

char IsCG20;	//	1:CG10/20   0:CG50

/*
----------------------------------------
HiddenRAM_Top(0x8C200000)
		>>>
		History buffer(512byte)
		(prog area) HiddenRAM_ProgNextPtr
			... 
			(free area)
				...
				(Mat area)	HiddenRAM_MatTopPtr
				<<<
					Matrix & List data area
					<<<
					(*MatAry)
					MatAry index area
						<<<
						HiddenRAM_End(0x8C800000)
----------------------------------------
*/
//---------------------------------------------------------------------------------------------
void * HiddenRAM(void){	// Check HiddenRAM 
//	unsigned int *VRAM=PictAry[0];
	volatile unsigned int *NorRAM=(volatile unsigned int*)(NORMALRAM_TOP-(IsEmu));	// Nomarl RAM TOP (no cache area)
	volatile unsigned int *HidRAM=(volatile unsigned int*)(HIDDENRAM_TOP-(IsEmu));	// Hidden RAM TOP (cashe area)
	int a,b;
	int K55=0x55555555;
	int KAA=0xAAAAAAAA;
	char * HidAddress=NULL;

	IsHiddenRAM=0;
	a= *NorRAM;
	b= *HidRAM;
	*NorRAM=K55;
	*HidRAM=KAA;
	if ( *NorRAM != *HidRAM ) {
			HidAddress=(char*)HidRAM;	// Hidden RAM Exist
			IsHiddenRAM=1;
	}
	*NorRAM=a;
	*HidRAM=b;
	return HidAddress;
}

void * HiddenRAM_mallocTemp( size_t size ){
	if ( ( UseHiddenRAM ) && ( IsHiddenRAM ) ) {
		return (void *)(HIDDENRAM_TOPTOP -(IsEmu));
	} else {
		if ( size > 0x40000 ) return NULL;
		return (void *)WorkBuf;
	}
}
void * HiddenRAM_mallocProg( size_t size ){
	char * ptr;
	ptr = HiddenRAM_ProgNextPtr;
	HiddenRAM_ProgNextPtr += ( (size+15) & 0xFFFFFFF0 );	// 16byte align
	if ( HiddenRAM_ProgNextPtr < HiddenRAM_MatTopPtr ) return ptr;
	HiddenRAM_ProgNextPtr = ptr;
	return 0;
}
void * HiddenRAM_mallocMat( size_t size ){
	char * ptr;
	ptr = HiddenRAM_MatTopPtr;
	ptr -= ( (size+15) & 0xFFFFFFF0 );	// 16byte align
	if ( ptr < HiddenRAM_ProgNextPtr ) return 0;
	HiddenRAM_MatTopPtr = ptr;
	return ptr;
}

unsigned char *  HiddenRAM_mallocPict( int pictNo ){
	unsigned char *  ptr;
	if ( IsCG20 ) {
		return (unsigned char*)GVRAM+0x28800 + 1024*(pictNo-1);	// 1 bit pixel
	} 
	ptr = (unsigned char*)HiddenRAM_End-VRAMSIZE*(pictNo);		//	16bit pixel
	return ptr;
}

void HiddenRAM_freeTemp( void *ptr ){
}
void HiddenRAM_freeProg( void *ptr ){
	if ( ( (int)HiddenRAM_Top <= (int)ptr ) && ( (int)ptr < (int)HiddenRAM_ProgNextPtr ) ) HiddenRAM_ProgNextPtr= (char*)ptr;	// Hidden RAM Prog next ptr
}

//		HiddenRAM_MatTopPtr
//		...
//		ptr
//		ptr+size
//		...
//		
//
void HiddenRAM_freeMat( int reg ){
	int ptr = (int)MatAry[reg].Adrs;
	int	adrs, size = MatAry[reg].Maxbyte; 
	if ( size <= 0 ) return ;
	if ( (int)HiddenRAM_MatTopPtr == ptr ) goto plus;
	else
	if ( ( (int)HiddenRAM_MatTopPtr < ptr ) && ( ptr < (int)HiddenRAM_End ) ) {
		memcpy2( HiddenRAM_MatTopPtr+size, HiddenRAM_MatTopPtr, ptr-(int)HiddenRAM_MatTopPtr );
		for ( reg=0; reg<MatAryMax; reg++){
			adrs = (int)MatAry[reg].Adrs;
			if ( ( MatAry[reg].SizeA ) && ( adrs > (int)HiddenRAM_ProgNextPtr ) && ( adrs < ptr ) ) MatAry[reg].Adrs =(double*)(adrs+size);
		}
	  plus:
		HiddenRAM_MatTopPtr += size;
	} 
}

const char MatAryCheckStr[]="#CBasicCG1#";

void HiddenRAM_MatAryStore(){	// MatAry ptr -> HiddenRAM
	char buffer[256];
	int *iptr1=(int*)(HIDDENRAM_TOP+12 -(IsEmu) );
	int *iptr2=(int*)(HiddenRAM_End+12 );
	if ( ( UseHiddenRAM ) && ( IsHiddenRAM ) ) {
		memcpy( (char *)(HIDDENRAM_TOP-(IsEmu)), MatAryCheckStr, sizeof(MatAryCheckStr) );
		memcpy( HiddenRAM_End,         MatAryCheckStr, sizeof(MatAryCheckStr) );
		iptr1[0]=(int)MatAry;
		iptr2[0]=(int)HiddenRAM_MatTopPtr;
	} else {
		memcpy( buffer, MatAryCheckStr, sizeof(MatAryCheckStr) );	// dummy
		memcpy( buffer, MatAryCheckStr, sizeof(MatAryCheckStr) );	// dummy
	}
}
int HiddenRAM_MatAryRestore(){	//  HiddenRAM -> MatAry ptr
	char buffer[10];
	int *iptr1=(int*)(HIDDENRAM_TOP+12 -(IsEmu));
	int *iptr2=(int*)(HiddenRAM_End+12 );
	if ( !(UseHiddenRAM&0xF0) )return 0;	// hidden init
	if ( ( UseHiddenRAM ) && ( IsHiddenRAM ) ){
		if ( ( strcmp((char *)(HIDDENRAM_TOP-(IsEmu)), MatAryCheckStr) == 0 ) && ( strcmp(HiddenRAM_End, MatAryCheckStr) == 0 ) ) {
			HiddenRAM_MatTopPtr=(char*)iptr2[0];
			return 1;	// ok
		}
	}
	return 0;
}
void HiddenRAM_MatAryInit(){	// HiddenRAM Initialize
	int i;
	MatAryMax=MATARY_MAX +ExtendList*52;
	Mattmpreg=MatAryMax-1;
//	Mattmpreg=56;
	ExtListMax=(ExtendList+1)*52;
	if ( ( UseHiddenRAM ) && ( IsHiddenRAM ) ) {		// hidden RAM init
		IsCG20=0;	// CG50
		EditMaxfree = EDITMAXFREE2;
		EditMaxProg = EDITMAXPROG2;
		NewMax      = NEWMAX2;
		ClipMax     = CLIPMAX2;
		HiddenRAM_Top  =(char*)(HIDDENRAM_TOP+16+1024 -(IsEmu));			// Hidden RAM TOP
		HiddenRAM_End  =(char*)(HIDDENRAM_END -(IsEmu));			;		// Hidden RAM END
		HiddenRAM_ProgNextPtr = HiddenRAM_Top;					// Hidden RAM Prog next ptr
		OplistRecentFreq=(toplistrecentfreq *)((char*)(HIDDENRAM_TOP+16 -(IsEmu)));
		OplistRecent    =(int *)OplistRecentMem;
		InitOpcodeRecent();
		MatAry = (matary *) (HiddenRAM_End - VRAMSIZE*(21+ExtendPict) - sizeof(matary)*MatAryMax );
		if ( HiddenRAM_MatAryRestore() ) return ;				// hidden RAM ready
		HiddenRAM_MatTopPtr = (char*)MatAry;
		HiddenRAM_MatAryStore();
	} else {		// use heap RAM
		IsCG20=1;	// CG10/20
		EditMaxfree = EDITMAXFREE;
		EditMaxProg = EDITMAXPROG;
		NewMax      = NEWMAX;
		ClipMax     = CLIPMAX;
		HiddenRAM_Top         = HeapRAM;					// Heap RAM TOP
		HiddenRAM_End         = HeapRAM+MaxHeapRam*1024;	// Heap RAM END
		HiddenRAM_ProgNextPtr = HiddenRAM_Top;					// Hidden RAM Prog next ptr
		HiddenRAM_MatTopPtr   = HiddenRAM_End;				// Heap RAM Mat top ptr
		MatAry = MatAryR;
//		HiddenRAM_MatTopPtr   = HiddenRAM_End - sizeof(matary)*MatAryMax ;
//		MatAry = (matary *)HiddenRAM_MatTopPtr;
		OplistRecentFreq=(toplistrecentfreq *)OplistRecentFreqMem;
		OplistRecent    =(int *)OplistRecentMem;
	}
	InitOpcodeRecentAll();
	memset( MatAry, 0, sizeof(matary)*MatAryMax );
	DeleteStrBuffer();
	DeletePictPtr();
	FileListUpdate=1;
}

void HeapRAM_malloc( int sizeK ){
	if ( HeapRAM )free(HeapRAM);
	while ( 1 ) {
		HeapRAM = (char *)malloc( sizeK*1024 );		// ???KB C.Basic area (program & Mat)
		if ( HeapRAM != NULL )  break; 
		sizeK--; if (sizeK<MINHEAP) Abort(); 
	}
	MaxHeapRam = sizeK;
	HiddenRAM_MatAryInit();
}
//---------------------------------------------------------------------------------------------- align dummy
int ioObjectAlign4a( unsigned int n ){ return n; }	// align +4byte
int ioObjectAlign4b( unsigned int n ){ return n; }	// align +4byte
int ioObjectAlign4c( unsigned int n ){ return n; }	// align +4byte
int ioObjectAlign4d( unsigned int n ){ return n; }	// align +4byte
int ioObjectAlign4e( unsigned int n ){ return n; }	// align +4byte
int ioObjectAlign4f( unsigned int n ){ return n; }	// align +4byte
int ioObjectAlign4g( unsigned int n ){ return n; }	// align +4byte
int ioObjectAlign4h( unsigned int n ){ return n; }	// align +4byte
int ioObjectAlign4i( unsigned int n ){ return n; }	// align +4byte
int ioObjectAlign4j( unsigned int n ){ return n; }	// align +4byte
int ioObjectAlign4k( unsigned int n ){ return n; }	// align +4byte
int ioObjectAlign4l( unsigned int n ){ return n; }	// align +4byte
//int ioObjectAlign4m( unsigned int n ){ return n; }	// align +4byte
//int ioObjectAlign4n( unsigned int n ){ return n; }	// align +4byte
//int ioObjectAlign4o( unsigned int n ){ return n; }	// align +4byte
//int ioObjectAlign4p( unsigned int n ){ return n; }	// align +4byte
//int ioObjectAlign4q( unsigned int n ){ return n; }	// align +4byte
//int ioObjectAlign4r( unsigned int n ){ return n; }	// align +4byte
//int ioObjectAlign4s( unsigned int n ){ return n; }	// align +4byte
//int ioObjectAlign4t( unsigned int n ){ return n; }	// align +4byte
//int ioObjectAlign4u( unsigned int n ){ return n; }	// align +4byte
//int ioObjectAlign4v( unsigned int n ){ return n; }	// align +4byte
//int ioObjectAlign4w( unsigned int n ){ return n; }	// align +4byte
//int ioObjectAlign4x( unsigned int n ){ return n; }	// align +4byte
//int ioObjectAlign4y( unsigned int n ){ return n; }	// align +4byte
//int ioObjectAlign4z( unsigned int n ){ return n; }	// align +4byte
}
