#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <fxlib.h>
#include "fx_syscall.h"
#include "CB_io.h"
#include "CB_error.h"

#include "CB_inp.h"
#include "CB_edit.h"
#include "CB_file.h"
#include "CB_interpreter.h"
#include "CB_Matrix.h"

//----------------------------------------------------------------------------------------------
unsigned char ClipBuffer[ClipMax+1];
int selectSetup=0;
int selectVar=0;
int selectMatrix=0;
//----------------------------------------------------------------------------------------------

int SrcSize( unsigned char *src ) {
	int size ;
	if ( src[0]=='\0' ) return 0 ;
	size = (src[0x47]&0xFF)*256+(src[0x48]&0xFF)+0x4C;
	return size;
}
void SetSrcSize( unsigned char *src, int size ) {
	int sizeH,sizeL ;
	size=size-0x4C;
	sizeH = (size&0xFF00) >> 8 ;
	sizeL = (size&0x00FF) ;
	src[0x47]=sizeH;
	src[0x48]=sizeL;
}
int SrcEndPtr( unsigned char *src ) {
	return ( SrcSize( src ) - 0x56 - 1 ) ;
}	

//---------------------------------------------------------------------------------------------
void CB_PreProcess(unsigned char *SRC) {
	unsigned int c;
	int ofst=0;

	return ;
	
	while (1) {
		c=SRC[ofst++];
//		if	( (c=0x0C) || (c=0x0D) || (c=':') ) c=SRC[ofst++];
		switch ( c ) {
			case 0x00:	// <EOF>
				return;
			case 0x3A:	// <:>
			case 0x0C:	// dsps
			case 0x0D:	// <CR>
				break;
			case 0x0F:	// exp
				ofst--;
				SRC[ofst++]='e';
				if (SRC[ofst] == 0x87 ) SRC[ofst++]='-';
				break;
			case 0x7F:	// 
			case 0xF7:	// 
			case 0xF9:	// 
			case 0xE5:	// 
			case 0xE6:	// 
			case 0xE7:	// 
			case 0xFF:	// 
				ofst++;
				break;
			default:
				break;
		}
	}
}

//----------------------------------------------------------------------------------------------

void InsertOpcode( unsigned char *filebase, int ptr, unsigned short opcode ){
	int len,i,j;
	int opH,opL;
	unsigned char *srcbase;
	len=OpcodeLen( opcode );
	if ( ( len + SrcSize(filebase) ) > ProgfileMax[ProgNo] ) {
		ErrorPtr=ptr; ErrorNo=NotEnoughMemoryERR;		// Memory error
		CB_ErrMsg(ErrorNo);
		return ;
	}
	j=ptr+len+0x56;
	for ( i=ProgfileMax[ProgNo]; i>=j; i-- )
		 filebase[i]=filebase[i-len];
		 
	opH = (opcode&0xFF00) >> 8 ;
	opL = (opcode&0x00FF) ;
	srcbase=filebase+0x56;
	switch (len) {
		case 1:
			srcbase[ptr  ]=opL;
			break;
		case 2:
			srcbase[ptr ]=opH;
			srcbase[ptr+1]=opL;
			break;
	}
	SetSrcSize( filebase, SrcSize(filebase)+len ) ; 	// set new file size
	ProgfileEdit[ProgNo]=1;	// edit program
}

void DeleteOpcode( unsigned char *filebase, int *ptr){
	int len,i;
	int opH,opL;
	unsigned short opcode;
	opcode=GetOpcode( filebase+0x56, *ptr );
	if ( opcode == 0 ) {
		PrevOpcode( filebase+0x56, &(*ptr) );
		opcode=GetOpcode( filebase+0x56, *ptr );
	}
	len=OpcodeLen( opcode );
	if ( len == 0 ) return ;
	for ( i=(*ptr)+0x56; i<=ProgfileMax[ProgNo]; i++ ) filebase[i]=filebase[i+len];

	SetSrcSize( filebase, SrcSize(filebase)-len ) ; 	// set new file size
	ProgfileEdit[ProgNo]=1;	// edit program
}

int EndOfSrc( unsigned char *SRC, int ptr ) {
	unsigned char *filebase,*srcbase;
	int endPtr;
	filebase=ProgfileAdrs[ProgNo];
	srcbase=filebase+0x56;
	endPtr=SrcEndPtr( filebase );
	if (SRC < srcbase ) SRC=srcbase;
	while ( SRC[ptr] ) {
		if ( ptr >= endPtr ) return 0 ;
		NextLine( SRC, &ptr );
	}
	return ptr;
}

//---------------------------------------------------------------------------------------------
void EditPaste( unsigned char *filebase, int *ptr ){
	int len,i,j;
	unsigned char *srcbase;

	if ( ClipBuffer[0]=='\0' ) return ;	// no clip data
		
	len=strlenOp(ClipBuffer);
	if ( ( len + SrcSize(filebase) ) > ProgfileMax[ProgNo] ) {
		ErrorPtr=(*ptr); ErrorNo=NotEnoughMemoryERR;		// Memory error
		CB_ErrMsg(ErrorNo);
		return ;
	}
	j=(*ptr)+len+0x56;
	for ( i=ProgfileMax[ProgNo]; i>=j; i-- ) filebase[i]=filebase[i-len];
		 
	srcbase=filebase+0x56+(*ptr);
	for ( i=0; i<len; i++ ) srcbase[i]=ClipBuffer[i];
			
	SetSrcSize( filebase, SrcSize(filebase)+len ) ; 	// set new file size
	ProgfileEdit[ProgNo]=1;	// edit program
	
	(*ptr)=(*ptr)+len;
}

void EditCopy( unsigned char *filebase, int ptr, int startp, int endp ){
	int len,i,j;
	unsigned char *srcbase;

	i=OpcodeLen( GetOpcode(filebase+0x56, ptr) );
	len=(endp)-(startp)+i;
	if ( len > ClipMax ) {
		ErrorPtr=ptr; ErrorNo=NotEnoughMemoryERR;		// Memory error
		CB_ErrMsg(ErrorNo);
		return ;
	}
	
	srcbase=filebase+0x56+(startp);
	for ( i=0; i<len; i++ ) ClipBuffer[i]=srcbase[i];
	ClipBuffer[i]='\0';
}

void EditCut( unsigned char *filebase, int *ptr, int startp, int endp ){
	int len,i;
	unsigned char *srcbase;

	i=OpcodeLen( GetOpcode(filebase+0x56, *ptr) );
	len=(endp)-(startp)+i;
	if ( len > ClipMax ) {
		ErrorPtr=(*ptr); ErrorNo=NotEnoughMemoryERR;		// Memory error
		CB_ErrMsg(ErrorNo);
		return ;
	}
	
	srcbase=filebase+0x56+(startp);
	for ( i=0; i<len; i++ ) ClipBuffer[i]=srcbase[i];
	ClipBuffer[i]='\0';

	for ( i=(startp)+0x56; i<=ProgfileMax[ProgNo]; i++ ) filebase[i]=filebase[i+len];

	SetSrcSize( filebase, SrcSize(filebase)-len ) ; 	// set new file size
	ProgfileEdit[ProgNo]=1;	// edit program

	(*ptr)=(startp);
}

//---------------------------------------------------------------------------------------------

void OpcodeLineN(unsigned char *buffer, int *ofst, int *x, int *y) {
	unsigned char tmpbuf[18];
	int i,len,xmax=21,cont=1;
	unsigned short opcode;
	(*x)=1;
	while ( cont ) {
		opcode = GetOpcode( buffer, *ofst );
		if ( opcode=='\0' ) break;
		if ( ( opcode==0x0C ) || ( opcode==0x0D ) ) break ;
		(*ofst) += OpcodeLen( opcode );
		CB_OpcodeToStr( opcode, tmpbuf ) ; // SYSCALL
		len = CB_MB_ElementCount( tmpbuf ) ;				// SYSCALL
		i=0;
		while ( i < len ) {
			(*x)++;
			if ( (*x) > xmax ) { (*y)++; (*x)=1; }
			i++;
		}
	}
	return ;
}

int OpcodeLineYptr(int n, unsigned char *buffer, int ofst, int *x) {
	unsigned char tmpbuf[18];
	int i,len,y=0,xmax=21,cont=1;
	unsigned short opcode;
	(*x)=1;
	if ( y==n ) return ofst;
	while ( cont ) {
		opcode = GetOpcode( buffer, ofst );
		if ( opcode=='\0' ) break;
		if ( ( opcode==0x0C ) || ( opcode==0x0D ) ) break ;
		(ofst) += OpcodeLen( opcode );
		CB_OpcodeToStr( opcode, tmpbuf ) ; // SYSCALL
		len = CB_MB_ElementCount( tmpbuf ) ;				// SYSCALL
		i=0;
		while ( i < len ) {
			(*x)++;
			if ( (*x) > xmax ) { (y)++; (*x)=1; }
			i++;
		}
		if ( y>n ) break;
	}
	return ofst;
}

int OpcodeLineSrcXendpos(int n, unsigned char *buffer, int ofst) {
	unsigned char tmpbuf[18];
	int i,len,x0,x=1,y=0,xmax=21,cont=1;
	unsigned short opcode;
	while ( cont ) {
		opcode = GetOpcode( buffer, ofst );
		if ( opcode=='\0' ) break;
		if ( ( opcode==0x0C ) || ( opcode==0x0D ) ) break ;
		(ofst) += OpcodeLen( opcode );
		CB_OpcodeToStr( opcode, tmpbuf ) ; // SYSCALL
		len = CB_MB_ElementCount( tmpbuf ) ;				// SYSCALL
		i=0; x0=x;
		while ( i < len ) {
			(x)++;
			if ( (x) > xmax ) { (y)++; (x)=1; }
			i++;
		}
		if ( y>n ) break;
	}
	return x0;
}
int OpcodeLineSrcYpos(unsigned char *buffer, int ofst, int csrptr ) {
	unsigned char tmpbuf[18];
	int i,len=0,x=1,y=0,xmax=21,cont=1;
	unsigned short opcode;
	if ( ofst==csrptr ) return y;
	while ( cont ) {
		opcode = GetOpcode( buffer, ofst );
		if ( opcode=='\0' ) break;
		if ( ( opcode==0x0C ) || ( opcode==0x0D ) ) break ;
		(ofst) += OpcodeLen( opcode );
		CB_OpcodeToStr( opcode, tmpbuf ) ; // SYSCALL
		len = CB_MB_ElementCount( tmpbuf ) ;				// SYSCALL
		i=0;
		while ( i < len ) {
			(x)++;
			if ( (x) > xmax ) { (y)++; (x)=1; }
			i++;
		}
		if ( ofst==csrptr ) break;
	}
	return y;
}

void NextLinePhy( unsigned char *buffer, int *ofst, int *ofst_y ) {
	int ofst2,x,y;
	if ( *ofst_y == 0 ) {
		ofst2= *ofst;
		x=1; y=0;
		OpcodeLineN( buffer, &ofst2 ,&x ,&y);
		if ( y==0 ) {
			NextLine( buffer, &(*ofst));
			*ofst_y = 0;
		} else {
			*ofst_y = 1;
		}
	} else {
		ofst2= *ofst;
		x=1; y=0;
		OpcodeLineN( buffer, &ofst2 ,&x ,&y);
		if ( y > *ofst_y ) {
			(*ofst_y)++;
		} else {
			NextLine( buffer, &(*ofst));
			*ofst_y = 0;
		}
	}
}

void PrevLinePhy( unsigned char *buffer, int *ofst, int *ofst_y ) {
	int ofst2,x,y;
	if ( *ofst_y == 0 ) {
		PrevLine( buffer, &(*ofst));
		ofst2= *ofst;
		x=1; y=0;
		OpcodeLineN( buffer, &ofst2 ,&x ,&y);
		if ( y==0 ) {
			*ofst_y = 0;
		} else {
			*ofst_y = y;
		}
	} else {
		(*ofst_y)--;
	}
}

int PrintOpcodeLine1(int csry, int n, unsigned char *buffer, int ofst, int csrPtr, int *cx, int *cy, int ClipStartPtr, int ClipEndPtr) {
	unsigned char tmpbuf[18],*tmpb;
	int i,len,x=1,y=0,xmax=21,cont=1,rev;
	unsigned short opcode;
	unsigned char  c=1;
	if ( ClipEndPtr < ClipStartPtr ) { i=ClipStartPtr; ClipStartPtr=ClipEndPtr; ClipEndPtr=i; }
	while ( cont ) {
		rev=0; if ( ( ClipStartPtr >= 0 ) && ( ClipStartPtr <= ofst ) && ( ofst <= ClipEndPtr ) ) rev=1;
		if ( y == n ) if (ofst==csrPtr) { *cx=x; *cy=csry; }
		opcode = GetOpcode( buffer, ofst );
		if ( opcode=='\0' ) break;
		ofst += OpcodeLen( opcode );
		CB_OpcodeToStr( opcode, tmpbuf ) ; // SYSCALL
		len = CB_MB_ElementCount( tmpbuf ) ;				// SYSCALL
		i=0;
		tmpb=tmpbuf;
		while ( i < len ) {
			if ( y == n ) {
				if ( rev )
						CB_PrintRevC(x,csry, (unsigned char*)(tmpb+i) ) ;
				else	CB_PrintC(   x,csry, (unsigned char*)(tmpb+i) ) ;
//				Bdisp_PutDisp_DD();
			}
			x++;
			if ( ( x > xmax ) || ( opcode==0x0C ) || ( opcode==0x0D ) ) { (y)++; x=1; }
			c=tmpb[i];
			if ( (c==0x7F)||(c==0xF7)||(c==0xF9)||(c==0xE5)||(c==0xE6)||(c==0xE7)||(c==0xFF) ) tmpb++;
			i++;
		}
		if ( ( opcode==0x0C ) || ( opcode==0x0D ) ) break ; 
//		if ( y > n ) break ;
	}
	if ( y == n+1 ) if (ofst==csrPtr) { *cx=x; *cy=csry+1; }
	return ofst;
}

//---------------------------------------------------------------------------------------------

void DumpOpcode(unsigned char *SrcBase, int *offset, int *offset_y, int csrPtr, int *cx, int *cy, int ClipStartPtr, int ClipEndPtr){
	int i,x,y,ynum;
	int ofst,ofst2,ofstYptr;

	*cx=0; *cy=0;
//	if ( *offset > csrPtr ) { 
//		*offset=csrPtr;
//	}
	
	while ( 1 ) {
		locate(1,2); PrintLine((unsigned char*)" ",21);
		locate(1,3); PrintLine((unsigned char*)" ",21);
		locate(1,4); PrintLine((unsigned char*)" ",21);
		locate(1,5); PrintLine((unsigned char*)" ",21);
		locate(1,6); PrintLine((unsigned char*)" ",21);
		locate(1,7); PrintLine((unsigned char*)" ",21);
		y=2; ofst=(*offset);
				ofst2=ofst;
				x=0; ynum=0;
				OpcodeLineN( SrcBase, &ofst2 ,&x ,&ynum);
				i=*offset_y;
				while ( i < ynum ) {
					ofst2=ofst;
					PrintOpcodeLine1( y, i, SrcBase, ofst2, csrPtr, &(*cx), &(*cy), ClipStartPtr, ClipEndPtr);
					i++;
					y++;
					if ( y>7 ) break;
				}
				if ( y>7 ) break;
				ofst = PrintOpcodeLine1( y, i, SrcBase, ofst, csrPtr, &(*cx), &(*cy), ClipStartPtr, ClipEndPtr);
				y++;
		while ( y<8 ) {
				if ( SrcBase[ofst]==0x00 ) break ;
				ofst2=ofst;
				x=0; ynum=0;
				OpcodeLineN( SrcBase, &ofst2 ,&x ,&ynum);
				i=0;
				while ( i < ynum ) {
					ofst2=ofst;
					PrintOpcodeLine1( y, i, SrcBase, ofst2, csrPtr, &(*cx), &(*cy), ClipStartPtr, ClipEndPtr);
					i++;
					y++;
					if ( y>7 ) break;
				}
				if ( y>7 ) break;
				ofst = PrintOpcodeLine1( y, i, SrcBase, ofst, csrPtr, &(*cx), &(*cy), ClipStartPtr, ClipEndPtr);
				y++;
		}
		if ( ( (*cx)!=0 ) && ( (*cy)>1 ) && ( (*cy)<8 ) ) break ;
		ofstYptr=OpcodeLineYptr( *offset_y, SrcBase, *offset, &x);
		if ( csrPtr < ofstYptr ) {
					PrevLinePhy( SrcBase, &(*offset), &(*offset_y) );
		} else {	NextLinePhy( SrcBase, &(*offset), &(*offset_y) );
		}
//		if ( SrcBase[ofst]==0x00 ) break ;
	}
}


//---------------------------------------------------------------------------------------------

void DumpMix(unsigned char *SrcBase, int offset){
	int 	i,j;
	for (j=0; j<6 ; j++){
		Hex4PrintXY( 1, j+2, "", ((int)offset+j*4)&0xFFFF);
		locate( 5,j+2); Print((unsigned char*)":           /    ");
		for (i=0; i<4 ; i++){
				Hex2PrintXY( i*3+6, j+2, "", SrcBase[offset+i+j*4]&0xFF);
		}
		for (i=0; i<4 ; i++){
				CB_PrintC(18+i,j+2,(unsigned char*)SrcBase+offset+i+j*4);
		}
	}
}

void DumpAsc(unsigned char *SrcBase, int offset){
	int 	i,j;
	for (j=0; j<6 ; j++){
		Hex4PrintXY( 1, j+2, "", ((int)offset+j*16)&0xFFFF);
		locate( 5,j+2); Print((unsigned char*)"/                ");
		for (i=0; i<16 ; i++){
				CB_PrintC(6+i,j+2,(unsigned char*)SrcBase+offset+i+j*16);
		}
	}
}

//---------------------------------------------------------------------------------------------

unsigned short oplistOPTN[]={
		0x23,	// #
//		0x24,	// $
		0x25,	// %
//		0x26,	// &
//		0x27,	// '
		0x97,	// Abs
		0xA6,	// Int
		0xB6,	// frac
		0xAB,	// !
//		0x81,	// sin
//		0x82,	// cos
//		0x83,	// tan
//		0x91,	// arcsin
//		0x92,	// arccos
//		0x93,	// arctan
//		0x85,	// ln
//		0x86,	// sqrt
//		0x95,	// log
		0xDA,	// Deg
		0xDB,	// Rad
		0xDC,	// Grad
		0xA1,	// sinh
		0xA2,	// cosh
		0xA3,	// tanh
		0xB1,	// arcsinh
		0xB2,	// arccosh
		0xB3,	// arctanh

		0x7F3A,	// MOD(
		0x7F46,	// Dim
		0x7F40,	// Mat	

		0x7FB0,	// And
		0x7FB1,	// Or
		0x7FB3,	// Not
		0x7FB4,	// Xor

		0xD3,	// Rnd
		0x7F86,	// RndFix(
		0xC1,	// Ran#
		0x7F87,	// RanInt#(

		0xD9,	// Norm
		0xE3,	// Fix
		0xE4,	// Sci
		0xDD,	// Eng
		0xF90B,	// EngOn
		0xF90C,	// EngOff

		0x01,	// femto
		0x02,	// pico
		0x03,	// nano
		0x04,	// micro
		0x05,	// milli
		0x06,	// Kiro
		0x07,	// Mega
		0x08,	// Giga
		0x09,	// Tera
		0x0A,	// Peta
		0x0B,	// Exa
		
		
		0xF793,	// StoPict
		0xF794,	// RclPict
//		0xF797,	// StoV-Win
//		0xF798,	// RclV-Win
//		0xF79F,	// RclCapt
//		0xF74C,	// Sum
		0x0FB,	// ProbP(
		0x0FC,	// ProbQ(
//		0x0FD,	// ProbR(
//		0x0FE,	// Probt(
		0x7FE9,	// CellSum(
		0};
							
unsigned short oplistPRGM[]={	
		0x0C,	// dsps
		0x3A,	// :
		0x3F,	// ?
		0x27,	// '
		0x7E,	// ~
		
		0x3C,	// <
		0x3E,	// >
		0x10,	// <=
		0x11,	// !=
		0x12,	// >=
		
		0xE2,	// Lbl
		0xEC,	// Goto
		0x13,	// =>
		0xE9,	// Isz
		0xE8,	// Dsz
		
		0xF700,	// If
		0xF701,	// Then
		0xF702,	// Else
		0xF703,	// IfEnd
		0xF704,	// For
		0xF705,	// To
		0xF706,	// Step
		0XF707,	// Next
		0xF708,	// While
		0xF709,	// WhileEnd
		0xF70A,	// Do
		0xF70B,	// LpWhile
		
		0xED,	// Prog
		0xF70C,	// Return
		0xF70D,	// Break
		0xF70E,	// Stop

		0xF718,	// ClrText
		0xF719,	// ClrGraph
		0xF71A,	// ClrList
		0xF91E,	// ClrMat
		
		0xF710,	// Locate
		0x7F8F,	// Getkey

		0xF720,	// DrawGraph
		0};
		
unsigned short oplistVARS[]={
		0xD1,	// Cls
		0xF719,	// ClrGraph
		0xE0,	// Plot
		0xE1,	// Line
		0xEB,	// ViewWindow

		0xF7A3,	// Vertical
		0xF7A4,	// Horizontal
		0xF7A5,	// Text
		0xF7A6,	// Circle
		0xF7A7,	// F-Line
		0xF7A8,	// PlotOn
		0xF7A9,	// PlotOff
		0xF7AA,	// PlotChg
		0xF7AB,	// PxlOn
		0xF7AC,	// PxlOff
		0xF7AD,	// PxlChg
		0xF7AF,	// PxlTest(

//		0xF797,	// StoV-Win
//		0xF798,	// RclV-Win
		0xF793,	// StoPict
		0xF794,	// RclPict
//		0xF79F,	// RclCapt
		

		0xDA,	// Deg
		0xDB,	// Rad
		0xDC,	// Grad
		0xF7C3,	// CoordOn
		0xF7D3,	// CoordOff
		0xF77D,	// GridOn
		0xF77A,	// GridOff
		0xF7C2,	// AxesOn
		0xF7D2,	// AxesOff
		0xF7C4,	// LabelOn
		0xF7D4,	// LabelOff
		0xF770,	// G-Connect
		0xF771,	// G-Plot
		0xF71C,	// S-L-Normal
		0xF71D,	// S-L-Thick
		0xF71E,	// S-L-Broken
		0xF71F,	// S-L-Dot
		0xF78C,	// SketchNormal
		0xF78D,	// SketchThick
		0xF78E,	// SketchBroken
		0xF78F,	// SketchDot
		
		0x7F00,	// Xmin
		0x7F01,	// Xmax
		0x7F02,	// Xscl
		0XF921,	// Xdot
		0x7F04,	// Ymin
		0x7F05,	// Ymax
		0x7F06,	// Yscl
		0x7F0B,	// Xfct
		0x7F0C,	// Yfct
		0x7FF0,	// GraphY
		0xF720,	// DrawGraph
		0xEE,	// Graph Y=
		0xF73F,	// DotG(
		0xF94B,	// DotP(
		0};

//---------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------
int SelectOpcode5800P(unsigned short *oplist, int *select) {
	int opNum=0 ;
	char buffer[22];
	char tmpbuf[18];
	unsigned int key;
	int	cont=1;
	int i,j,y,n;
	int seltop=*select;

	while ( oplist[opNum++] ) ;
	opNum-=2;

	SaveDisp(SAVEDISP_PAGE1);
	Bdisp_AllClr_DDVRAM();
	
//	PopUpWin(6);

	while (cont) {
		Bdisp_AllClr_VRAM();
		locate(1,1); Print((unsigned char *)"== Command Select ==");
		(*select)=(*select)/12*12;
		for ( i=0; i<12; i++ ) {
			n=oplist[(*select)+i];
			tmpbuf[0]='\0'; 
			if ( n == 0xFFFF ) n=' ';
			else CB_OpcodeToStr( n, (unsigned char*)tmpbuf ) ; // SYSCALL
			tmpbuf[8]='\0'; 
			n=i+1; if (n>9) n=0;
			j=0; if ( tmpbuf[0]==' ' ) j++;
			if ( i< 10 ) sprintf(buffer,"%d:%-9s",n,tmpbuf+j ) ;
			if ( i==10 ) sprintf(buffer,".:%-9s",tmpbuf+j ) ;
			if ( i==11 ) sprintf(buffer,"\x0F:%-9s",tmpbuf+j ) ;
			locate(0+(i%2)*12,2+i/2); 
			Print((unsigned char *)buffer);
			if ( i==11 ) { locate(0+(i%2)*12,2+i/2); Print((unsigned char *)"\x0F"); }
		}
		Fkey_DISPR( 0, " ? ");
		Fkey_DISPR( 1, " \x0C ");
		Fkey_DISPR( 2, " : ");
		Fkey_DISPR( 3, " ' ");
		Fkey_DISPR( 4, " = ");
		Fkey_DISPR( 5, " \x11 ");

		Bdisp_PutDisp_DD();	
		
		y = ((*select)-seltop) + 1 ;
//		Bdisp_AreaReverseVRAM(12, y*8, 113, y*8+7);	// reverse select line 
		Bdisp_PutDisp_DD();

		GetKey( &key );
		switch (key) {
			case KEY_CTRL_EXIT:
				return 0;
			case KEY_CTRL_EXE:
				cont=0;
				break;
		
			case KEY_CTRL_F1:	// ?
				RestoreDisp(SAVEDISP_PAGE1);
				return '?';
				break;
			case KEY_CTRL_F2:	// dsps
				RestoreDisp(SAVEDISP_PAGE1);
				return 0x0C;
				break;
			case KEY_CTRL_F3:	// :
				RestoreDisp(SAVEDISP_PAGE1);
				return ':';
				break;
			case KEY_CTRL_F4:	// '
				RestoreDisp(SAVEDISP_PAGE1);
				return 0x27;
				break;
			case KEY_CTRL_F5:	// =
				RestoreDisp(SAVEDISP_PAGE1);
				return '=';
				break;
			case KEY_CTRL_F6:	// !=
				RestoreDisp(SAVEDISP_PAGE1);
				return 0x11;
				break;

			case KEY_CTRL_LEFT:
				*select = 0;
				break;
			case KEY_CTRL_RIGHT:
				*select = opNum;
				break;
			case KEY_CTRL_UP:
				(*select)-=12;
				if ( *select < 0 ) *select = opNum;
				break;
			case KEY_CTRL_DOWN:
				(*select)+=12;
				if ( *select >= opNum ) *select =0;
				break;
			case KEY_CHAR_1:
			case KEY_CHAR_U:
				n=0;
				cont=0;
				break;
			case KEY_CHAR_2:
			case KEY_CHAR_V:
				n=1;
				cont=0;
				break;
			case KEY_CHAR_3:
			case KEY_CHAR_W:
				n=2;
				cont=0;
				break;
			case KEY_CHAR_4:
			case KEY_CHAR_P:
				n=3;
				cont=0;
				break;
			case KEY_CHAR_5:
			case KEY_CHAR_Q:
				n=4;
				cont=0;
				break;
			case KEY_CHAR_6:
			case KEY_CHAR_R:
				n=5;
				cont=0;
				break;
			case KEY_CHAR_7:
			case KEY_CHAR_M:
				n=6;
				cont=0;
				break;
			case KEY_CHAR_8:
			case KEY_CHAR_N:
				n=7;
				cont=0;
				break;
			case KEY_CHAR_9:
			case KEY_CHAR_O:
				n=8;
				cont=0;
				break;
			case KEY_CHAR_0:
			case KEY_CHAR_Z:
				n=9;
				cont=0;
				break;
			case KEY_CHAR_DP:
			case KEY_CHAR_SPACE:
				n=10;
				cont=0;
				break;
			case KEY_CHAR_EXP:
			case KEY_CHAR_DQUATE:
				n=11;
				cont=0;
				break;
			default:
				break;
		}
	}

	RestoreDisp(SAVEDISP_PAGE1);
//	Bdisp_PutDisp_DD();

	i=oplist[(*select)+n]; if (i==0xFFFF ) i=0;
	return i;
}

							
unsigned short oplistCMD[]={	
		0x3F,	// ?			1
		0x0E,	// ->			2
		0xF700,	// If			3
		0xF701,	// Then			4
		0xF702,	// Else			5
		0xF703,	// IfEnd		6
		0xE2,	// Lbl			7
		0xEC,	// Goto			8
		0x27,	// '
		0x7E,	// ~
		0x23,	// #
		0x25,	// %
//		0x0C,	// dsps	
//		0x3A,	// :	

		0x3D,	// =			1
		0x11,	// !=			2
		0x3E,	// >			3
		0x3C,	// <			4
		0x12,	// >=			5
		0x10,	// <=			6
		0xFFFF,	// 				-
		0xFFFF,	// 				-
		0xF718,	// ClrText	
		0xF719,	// ClrGraph	
		0xF71A,	// ClrList	
		0xF91E,	// ClrMat	
		
		0xE8,	// Dsz			1
		0xE9,	// Isz			2
		0x13,	// =>			3
		0xF710,	// Locate		4
		0xD1,	// Cls			5
		0x7FB0,	// And			6
		0x7FB1,	// Or			7
		0x7FB3,	// Not			8
		0x7FB4,	// Xor			9
		0xFFFF,	// 				-
		0xFFFF,	// 				-
		0x25,	// 	%			-
		
		0xF704,	// For			1
		0xF705,	// To			2
		0xF706,	// Step			3
		0XF707,	// Next			4
		0xF708,	// While		5
		0xF709,	// WhileEnd		6
		0xF70A,	// Do			7
		0xF70B,	// LpWhile		8
		0xFFFF,	// 				-
		0x7FE9,	// CellSum(
		0x7F46,	// Dim	
		0x7F40,	// Mat	

		
		0xF70D,	// Break		1
		0xF70C,	// Return		2
		0xF70E,	// Stop			3
		0x7F8F,	// Getkey		4
		0xED,	// Prog			5
		0xFFFF,	// 				-
		0xFFFF,	// 				-
		0xFFFF,	// 				-
		0xDA,	// Deg
		0xDB,	// Rad
		0xDC,	// Grad
		0xFFFF,	// 				-

		0xD1,	// Cls
		0xF719,	// ClrGraph
		0xEB,	// ViewWindow
		0xE0,	// Plot
		0xF7A8,	// PlotOn
		0xF7A9,	// PlotOff
		0xF7AA,	// PlotChg
		0xF7A6,	// Circle
		0xE1,	// Line
		0xF7A7,	// F-Line
		0xF7A3,	// Vertical
		0xF7A4,	// Horizontal
		
		0xF7AB,	// PxlOn
		0xF7AC,	// PxlOff
		0xF7AD,	// PxlChg
		0xF7AF,	// PxlTest(
		0xF7A5,	// Text
		0xFFFF,	// 				-
		0xF78C,	// SketchNormal
		0xF78D,	// SketchThick
		0xF78E,	// SketchBroken
		0xF78F,	// SketchDot
		0xF770,	// G-Connect
		0xF771,	// G-Plot
		
		0xF7C3,	// CoordOn
		0xF7D3,	// CoordOff
		0xF77D,	// GridOn
		0xF77A,	// GridOff
		0xF7C2,	// AxesOn
		0xF7D2,	// AxesOff
		0xF7C4,	// LabelOn
		0xF7D4,	// LabelOff
		0xF71C,	// S-L-Normal
		0xF71D,	// S-L-Thick
		0xF71E,	// S-L-Broken
		0xF71F,	// S-L-Dot
		
		0x7F00,	// Xmin
		0x7F01,	// Xmax
		0x7F02,	// Xscl
		0x7F0B,	// Xfct
		0XF921,	// Xdot
		0xFFFF,	// 				-
		0x7F04,	// Ymin
		0x7F05,	// Ymax
		0x7F06,	// Yscl
		0x7F0C,	// Yfct
		0xFFFF,	// 				-
		0xFFFF,	// 				-

		0x97,	// Abs
		0xA6,	// Int
		0xB6,	// frac
		0xAB,	// !
		0x7F3A,	// MOD(
		0xFFFF,	// 				-
		0xA1,	// sinh
		0xA2,	// cosh
		0xA3,	// tanh
		0xB1,	// arcsinh
		0xB2,	// arccosh
		0xB3,	// arctanh

		0xD3,	// Rnd
		0x7F86,	// RndFix(
		0xC1,	// Ran#
		0x7F87,	// RanInt#(
		0xFFFF,	// 				-
		0xFFFF,	// 				-
		0xD9,	// Norm
		0xE3,	// Fix
		0xE4,	// Sci
		0xDD,	// Eng
		0xF90B,	// EngOn
		0xF90C,	// EngOff

		0x01,	// femto
		0x02,	// pico
		0x03,	// nano
		0x04,	// micro
		0x05,	// milli
		0x06,	// Kiro
		0x07,	// Mega
		0x08,	// Giga
		0x09,	// Tera
		0x0A,	// Peta
		0x0B,	// Exa
		0xFFFF,	// 				-
		
		0xF720,	// DrawGraph
		0xEE,	// Graph Y=
		0x7FF0,	// GraphY
		0xFFFF,	// 				-
		0xF73F,	// DotG(
		0xF94B,	// DotP(
		0xFFFF,	// 				-
		0xFFFF,	// 				-
//		0xF797,	// StoV-Win
//		0xF798,	// RclV-Win
		0xF793,	// StoPict
		0xF794,	// RclPict
//		0xF79F,	// RclCapt
		0xFFFF,	// 				-

		0};

//---------------------------------------------------------------------------------------------
unsigned int SearchForText( char *buffer ){
	unsigned int key;
	int stat;
	
	Bdisp_AllClr_DDVRAM();
	
	locate(1,1); Print((unsigned char *)"Search For Text");
	locate(1,3); Print((unsigned char*)"---------------------");
	locate(1,5); Print((unsigned char*)"---------------------");
	Bdisp_PutDisp_DD();
	
	KeyRecover(); 
	buffer[0]='\0';
	key= InputStrSub( 1, 4, 21, 0, (unsigned char*)buffer, 63, ' ', REV_OFF, FLOAT_ON, EXP_ON, ALPHA_ON, HEX_OFF, PAL_ON, EXIT_CANCEL_OFF);

	return key;
}

int SearchOpcode( unsigned char *SrcBase, char *searchstr, int *csrptr){
	int csrbkup=(*csrptr);
	int sptr=0,cptr;
	unsigned short opcode=1,srccode;

	opcode =GetOpcode( SrcBase, *csrptr ) ;
	srccode=GetOpcode( (unsigned char*)searchstr, sptr ) ;
	if ( opcode == srccode ) NextOpcode( SrcBase, &(*csrptr) );
	while ( 1 ) {
		sptr=0;
		opcode =GetOpcode( SrcBase, *csrptr ) ;
		srccode=GetOpcode( (unsigned char*)searchstr, sptr ) ;
		if ( opcode == 0x00 ) { *csrptr=csrbkup; return 0; }	// No search
		if ( opcode != srccode ) {
			NextOpcode( SrcBase, &(*csrptr) );
		} else {
			cptr = *csrptr;
			while ( 1 ) {
				NextOpcode( (unsigned char*)searchstr, &sptr );
				srccode=GetOpcode( (unsigned char*)searchstr, sptr ) ;
				if ( srccode == 0x00 ) { *csrptr=cptr; return 1; }	// Search Ok
				NextOpcode( SrcBase, &(*csrptr) );
				opcode =GetOpcode( SrcBase, *csrptr ) ;
				if ( opcode == 0x00 ) { *csrptr=csrbkup; return 0; }	// No search
				if ( opcode != srccode ) break ;
			}
		}
	}
	{ *csrptr=csrbkup; return 0; }	// No search
}

//---------------------------------------------------------------------------------------------
int Contflag=0;
int selectCMD=0;
int selectOPTN=0;
int selectVARS=0;
int selectPRGM=0;

unsigned int EditRun(int run){		// run:1 exec      run:2 edit
	unsigned char *FileBase,*SrcBase;
	unsigned int key=0,keyH,keyL;
	int cont=1,stat;
	char buffer[32];
	char buffer2[32];
	char searchbuf[64];
	unsigned char c;
	int i,j,n,d,x,y,cx,cy,ptr,ptr2,tmpkey=0;
	int 	offset=0;
	int 	offset_y=0;
	int 	csrPtr=0;
	int 	csrPtr_y=0;
	int 	dumpflg=2;
	unsigned short opcode;
	int lowercase=0, CursorStyle;
	int ClipStartPtr = -1 ;
	int ClipEndPtr   = -1 ;
	int alphalock = 0 ;
	int SearchMode=0;

	long FirstCount;		// pointer to repeat time of first repeat
	long NextCount; 		// pointer to repeat time of second repeat

	Bkey_Get_RepeatTime(&FirstCount,&NextCount);	// repeat time
	Bkey_Set_RepeatTime(FirstCount,3);				// set cursor rep

//	PreProcess(SrcBase);			// Basic source adjust

	Bdisp_AllClr_DDVRAM();

//	i=0;
//	while ( ProgfileAdrs[i] ) {
//			locate (1,i+1);
//			Print( ProgfileAdrs[i]+0x3C );
//			i++;
//	}
//	GetKey(&key);

	FileBase = ProgfileAdrs[ProgNo];
	SrcBase  = FileBase+0x56;
	offset = ExecPtr;
	csrPtr = offset;
	for ( i=0; i<7; i++ ) PrevLinePhy( SrcBase, &offset, &offset_y );
	
	if ( run==1 ) { ProgNo=0; ExecPtr=0; key=KEY_CTRL_F6; }	// direct run
	
//	Cursor_SetFlashMode(1);			// cursor flashing on
	if (Cursor_GetFlashStyle()<0x6)	Cursor_SetFlashOn(0x0);		// insert mode cursor 
		else 						Cursor_SetFlashOn(0x6);		// overwrite mode cursor 
		
	while ( cont ) {
		ErrorNo=0;
		FileBase = ProgfileAdrs[ProgNo];
		SrcBase  = FileBase+0x56;
		if ( run!=1 ) {
			Bdisp_AllClr_VRAM();
			strncpy(buffer2,(const char*)ProgfileAdrs[ProgNo]+0x3C,8);
			buffer2[8]='\0';
			if (dumpflg==2)	sprintf(buffer,"==%-8s==%s",buffer2, CB_INTDefault?" [ INT ]":" [Double]");
			else 			sprintf(buffer,"==%-8s==%08X",buffer2, ProgfileAdrs[ProgNo]);
			locate (1,1); Print( (unsigned char*)buffer );
			if (SearchMode ) {
					Fkey_dispN( 0, "SRC ");
			} else {
			if ( ClipStartPtr>=0 ) {
					Fkey_dispN( 0, "COPY ");
					Fkey_dispN( 1, "CUT ");
				} else {
					ClipEndPtr   = -1 ;		// ClipMode cancel
					Fkey_dispN( 0, "TOP ");
					Fkey_dispN( 1, "BTM");
					Fkey_dispR( 2, "CMD");
					if ( lowercase  ) Fkey_dispN_aA(3,"A<>a"); else Fkey_dispN_Aa(3,"A<>a");
					Fkey_dispR( 4, "CHAR");
					if ( Contflag ) Fkey_dispN( 5, "Cont"); else Fkey_dispN( 5, "EXE");
				}
			}
			switch (dumpflg) {
				case 2: 		// Opcode
						DumpOpcode( SrcBase, &offset, &offset_y, csrPtr, &cx, &cy, ClipStartPtr, ClipEndPtr);
						break;
				case 4: 		// hex dump
						DumpMix( SrcBase, offset );
						break;
				case 16:		// Ascii dump
						DumpAsc( SrcBase, offset );
						break;
				default:
						break;
			}
			Bdisp_PutDisp_DD();


			locate(cx,cy);
			if ( (dumpflg==2) || (dumpflg==4) ) {
				Cursor_SetFlashMode(1);			// cursor flashing on
				if (Cursor_GetFlashStyle()<0x6) {
					if ( ClipStartPtr>=0 ) 	Cursor_SetFlashOn(0x05);	// ClipMode cursor
				} else { 
					if ( ClipStartPtr>=0 )	Cursor_SetFlashOn(0x0B);	// ClipMode cursor
				}
				CursorStyle=Cursor_GetFlashStyle();
				if ( ( CursorStyle==0x3 ) && lowercase != 0 ) Cursor_SetFlashOn(0x4);		// lowercase  cursor
				if ( ( CursorStyle==0x4 ) && lowercase == 0 ) Cursor_SetFlashOn(0x3);		// upperrcase cursor
				if ( ( CursorStyle==0x9 ) && lowercase != 0 ) Cursor_SetFlashOn(0xA);		// lowercase  cursor
				if ( ( CursorStyle==0xA ) && lowercase == 0 ) Cursor_SetFlashOn(0x9);		// upperrcase cursor
				if (key < 0x7F00)	GetKey(&key);
			} else
									GetKey(&key);
		}						
		switch (dumpflg) {
			case 4: 		// hex dump
				if ( ( ( KEY_CHAR_0 <= key ) && ( key <= KEY_CHAR_9 ) ) ||
					 ( ( KEY_CHAR_A <= key ) && ( key <= KEY_CHAR_Z ) ) ||
					   ( KEY_CTRL_XTT == key ) ||
					   ( KEY_CHAR_LOG == key ) ||
					   ( KEY_CHAR_LN  == key ) ||
					   ( KEY_CHAR_SIN == key ) ||
					   ( KEY_CHAR_COS == key ) ||
					   ( KEY_CHAR_TAN == key ) ) {
						if ( key == KEY_CTRL_XTT )	key='A';
						if ( key == KEY_CHAR_LOG )	key='B';
						if ( key == KEY_CHAR_LN  )	key='C';
						if ( key == KEY_CHAR_SIN )	key='D';
						if ( key == KEY_CHAR_COS )	key='E';
						if ( key == KEY_CHAR_TAN )	key='F';
						if ( ( '0' <= key ) && ( key <= '9' ) ) n= key-'0' ;
						if ( ( 'A' <= key ) && ( key <= 'F' ) ) n= key-'A'+10 ;
						if ( ( cx==6 ) || ( cx==9 ) || (cx==12) || (cx==15) ) { //
								ptr = (offset)+((cy-2)*4+(cx-6)/3) ;
								d = SrcBase[ptr];
								d = (d&0x0F) + n*16;
								SrcBase[ptr] = d;
						} else {				// lower nibbe
								ptr = (offset)+((cy-2)*4+(cx-6)/3) ;
								d = SrcBase[ptr];
								d = (d&0xF0) + n;
								SrcBase[ptr] = d;
						}
						key=KEY_CTRL_RIGHT;
				}
				break;
			case 2: 		// Opcode
			case 16:		// Ascii dump
				break;
			default:
				break;
		}

		switch (key) {
			case KEY_CTRL_NOP:
					ClipStartPtr = -1 ;		// ClipMode cancel
					if ( SearchMode ) break;;
					alphalock = 0 ;
					break;
//			case KEY_CTRL_ALPHA:
//					if ( alphalock ) alphalock = 0 ;
//					ClipStartPtr = -1 ;		// ClipMode cancel
//					break;
			case KEY_CTRL_EXIT:
					cont=0;
					break;
			case KEY_CTRL_F1:
			case 0x7553:		// ClipMode F1
				if ( SearchMode ) {		//	Next Search
					i=SearchOpcode( SrcBase, searchbuf, &csrPtr);
					if ( i==0 ) SearchMode=0;
				} else {
					if ( ClipEndPtr >= 0 ) {
						if ( ClipEndPtr < ClipStartPtr ) { i=ClipStartPtr; ClipStartPtr=ClipEndPtr; ClipEndPtr=i; }
						EditCopy( FileBase, csrPtr, ClipStartPtr, ClipEndPtr );
					} else {
						offset=0;
						csrPtr=0;
						switch (dumpflg) {
							case 2: 		// Opcode
								break;
							case 4: 		// hex dump
							case 16:		// Ascii dump
								cx=6; cy=2;
								break;
							default:
								break;
						}
					}
				}
					key=0;
					alphalock = 0 ;
					ClipStartPtr = -1 ;		// ClipMode cancel
					break;
			case KEY_CTRL_F2:
			case 0x7563:		// ClipMode F2
					if ( SearchMode ) break;;
					if ( ClipEndPtr >= 0 ) {
						if ( ClipEndPtr < ClipStartPtr ) { i=ClipStartPtr; ClipStartPtr=ClipEndPtr; ClipEndPtr=i; }
						EditCut( FileBase, &csrPtr, ClipStartPtr, ClipEndPtr );
					} else {
						offset=EndOfSrc( SrcBase, offset );
						csrPtr=offset;
						switch (dumpflg) {
							case 2: 		// Opcode
								for ( i=0; i<6; i++ ) PrevLinePhy( SrcBase, &offset, &offset_y );
								break;
							case 4: 		// hex dump
							case 16:		// Ascii dump
								cx=6; cy=2;
								break;
							default:
								break;
						}
					}
					key=0;
					alphalock = 0 ;
					ClipStartPtr = -1 ;		// ClipMode cancel
					break;
			case KEY_CTRL_F4:
					if ( SearchMode ) break;;
					lowercase=1-lowercase;
					key=0;
					if ( alphalock == 0 ) if ( ( CursorStyle==0x4 ) || ( CursorStyle==0x3 ) || ( CursorStyle==0xA ) || ( CursorStyle==0x9 ) ) PutKey( KEY_CTRL_ALPHA, 1 );
					ClipStartPtr = -1 ;		// ClipMode cancel
					break;
			case KEY_CTRL_F5:
					if ( SearchMode ) break;;
					Cursor_SetFlashMode(0); 		// cursor flashing off
					key=SelectChar();
					if ( alphalock == 0 ) if ( ( CursorStyle==0x4 ) || ( CursorStyle==0x3 ) || ( CursorStyle==0xA ) || ( CursorStyle==0x9 ) ) PutKey( KEY_CTRL_ALPHA, 1 );
					ClipStartPtr = -1 ;		// ClipMode cancel
					break;
			case KEY_CTRL_F6:
					if ( SearchMode ) break;;
					Cursor_SetFlashMode(0); 		// cursor flashing off
					if ( Contflag ) {						// ====== continue mode ======
						cont=0;
						ExecPtr=csrPtr;
						BreakPtr=0;
					} else {
						stat = CB_interpreter( SrcBase ) ;	// ====== run interpreter ======
						SaveConfig();
						FileBase = ProgfileAdrs[ProgNo];
						SrcBase  = FileBase+0x56;
						if ( stat ) {
							if ( ErrorNo ) offset = ErrorPtr ;			// error
							else if ( BreakPtr ) offset = ExecPtr ;	// break
						} else offset = 0;
						if ( stat == -1 ) offset = ExecPtr-1;
						csrPtr = offset;
						run=2; // edit mode
						if ( dumpflg == 2 ) {
						for ( i=0; i<7; i++ ) PrevLinePhy( SrcBase, &offset, &offset_y );
						if ( stat == -1 ) cont=0;	// 
						}
					}
					key=0;
					alphalock = 0 ;
					ClipStartPtr = -1 ;		// ClipMode cancel
					break;
					
			case 0x755A:		// ClipMode LEFT
			case KEY_CTRL_LEFT:
				switch ( dumpflg ) {
					case 2: 		// Opcode
							PrevOpcode( SrcBase, &csrPtr );
							if ( (cx==1)&&(cy==2) ) PrevLinePhy( SrcBase, &offset, &offset_y );
							break;
					case 4: 		// hex dump
							cx--;
							if ( cx<6 ) {
									cy--; cx=16;
									if ( cy<2 ) { (offset)-=4; cy=2; }
							}
							break;
					case 16:		// Ascii dump
							offset-=1;
							break;
					default:
							break;
				}
				key=0;
				if ( ClipStartPtr>=0 ) ClipEndPtr=csrPtr;
				SearchMode=0;
				break;
				
			case 0x755B:		// ClipMode RIGHT
			case KEY_CTRL_RIGHT:
				switch ( dumpflg ) {
					case 2: 		// Opcode
							if ( SrcBase[csrPtr] != 0x00 ) NextOpcode( SrcBase, &csrPtr );
							break;
					case 4: 		// hex dump
							cx++;
							if ( cx>16 ) {
									cy++; cx=6;
									if ( cy>7 ) { (offset)+=4; cx=6; cy=7; }
							}
							break;
					case 16:		// Ascii dump
							offset+=1;
							break;
					default:
							break;
				}
				key=0;
				if ( ClipStartPtr>=0 ) ClipEndPtr=csrPtr;
				SearchMode=0;
				break;
				
			case 0x7559:		// ClipMode UP
			case KEY_CTRL_UP:
				switch ( dumpflg ) {
					case 2: 		// Opcode
//							if ( SrcBase[csrPtr]==0x00 ) PrevOpcode( SrcBase, &csrPtr );
							if ( csrPtr==0 ) break; 
							ptr=csrPtr;
							PrevOpcode( SrcBase, &ptr );
							c = GetOpcode( SrcBase, ptr );
							ptr=csrPtr;
							if ( ( c!=0x0C ) && ( c!=0x0D ) ) PrevLine( SrcBase, &ptr );	// current line top
							y=0; ptr2=ptr;
							OpcodeLineN( SrcBase, &ptr2 , &x, &y);
							csrPtr_y = OpcodeLineSrcYpos( SrcBase, ptr, csrPtr ) ;	//
							if ( ( csrPtr_y == 0 ) ) {
								PrevLine( SrcBase, &ptr );	// prev line top
								y=0;
								OpcodeLineN( SrcBase, &ptr , &x, &y);
								i=x; x=cx;
							} else {
								x = OpcodeLineSrcXendpos( csrPtr_y-1, SrcBase, ptr );
								i=x; x=cx;
							}
							while ( 1 ) {
								PrevOpcode( SrcBase, &csrPtr );
								x -= OpcodeStrLenBuf( SrcBase, csrPtr);
								if ( x < 1 ) { x=i; break ; }
							}
							while ( x > cx ) {
								PrevOpcode( SrcBase, &csrPtr );
								x -= OpcodeStrLenBuf( SrcBase, csrPtr);
								if ( x < 1 ) { NextOpcode( SrcBase, &csrPtr ); break ; }
							}
							break;
					case 4: 		// hex dump
							cy--;
							if ( cy<2 ) { (offset)-=4; cy=2; }
							if ( (cx==7) || (cx==10) || (cx==13) ) cx--;
							break;
					case 16:		// Ascii dump
							offset-=dumpflg;
							break;
					default:
							break;
				}
				key=0;
				if ( ClipStartPtr>=0 ) ClipEndPtr=csrPtr;
				SearchMode=0;
				break;
				
			case 0x755C:		// ClipMode DOWN
			case KEY_CTRL_DOWN:
				switch ( dumpflg ) {
					case 2: 		// Opcode
							c=SrcBase[csrPtr];
							if ( c==0x00 ) break ;
							if ( ( c==0x0C ) || ( c==0x0D ) ) {
								NextOpcode( SrcBase, &csrPtr );
								x=1;
							} else {
								x=cx;
								while ( 1 ) {
									x += OpcodeStrLenBuf( SrcBase, csrPtr);
									if ( x > 21 ) { x-=21; NextOpcode( SrcBase, &csrPtr ); break ; }
									NextOpcode( SrcBase, &csrPtr );
									c=SrcBase[csrPtr];
									if ( ( c==0x0C ) || ( c==0x0D ) || ( c==0x00 ) ) { x=1; break; }
								}
							}
							c=SrcBase[csrPtr];
							if ( c==0x00 )  break;
							if ( ( c==0x0C ) || ( c==0x0D ) ) csrPtr++;
							while ( x < cx ) {
								x += OpcodeStrLenBuf( SrcBase, csrPtr);
								NextOpcode( SrcBase, &csrPtr );
								c=SrcBase[csrPtr];
								if ( ( c==0x0C ) || ( c==0x0D ) || ( c==0x00 ) ) break;
							}
							break;
					case 4: 		// hex dump
							cy++;
							if ( cy>7 ) { (offset)+=4; cy=7; }
							if ( (cx==7) || (cx==10) || (cx==13) ) cx--;
							break;
					case 16:		// Ascii dump
							offset+=dumpflg;
							break;
					default:
							break;
				}
				key=0;
				if ( ClipStartPtr>=0 ) ClipEndPtr=csrPtr;
				SearchMode=0;
				break;
				
			case KEY_CTRL_SHIFT:
				if ( SearchMode ) break;;
				alphalock = 0 ;
				ClipStartPtr = -1 ;		// ClipMode cancel
				Fkey_dispR( 0, "Var");
				Fkey_dispR( 1, "Mat");
				Fkey_dispR( 2, "V-W");
				Fkey_Clear( 3 );
				if ( dumpflg==2 ) Fkey_dispR( 4, "Dump"); else Fkey_dispR( 4, "List");
				Fkey_dispR( 5, "SRC" );
				GetKey(&key);
				switch (key) {
					case KEY_CTRL_EXIT:
							key=0;
							break;
					case KEY_CTRL_PAGEUP:
							offset=0;
							csrPtr=0;
							switch (dumpflg) {
								case 2: 		// Opcode
									break;
								case 4: 		// hex dump
								case 16:		// Ascii dump
									cx=6; cy=2;
									break;
								default:
									break;
							}
							key=0;
							break;
					case KEY_CTRL_PAGEDOWN:
							offset=EndOfSrc( SrcBase, offset );
							csrPtr=offset;
							switch (dumpflg) {
								case 2: 		// Opcode
									PrevLinePhy( SrcBase, &offset, &offset_y );
									PrevLinePhy( SrcBase, &offset, &offset_y );
									PrevLinePhy( SrcBase, &offset, &offset_y );
									PrevLinePhy( SrcBase, &offset, &offset_y );
									PrevLinePhy( SrcBase, &offset, &offset_y );
									PrevLinePhy( SrcBase, &offset, &offset_y );
									break;
								case 4: 		// hex dump
								case 16:		// Ascii dump
									cx=6; cy=2;
									break;
								default:
									break;
							}
							key=0;
							break;
					case KEY_CTRL_SETUP:
							Cursor_SetFlashMode(0); 		// cursor flashing off
							selectSetup=SetupG(selectSetup);
							key=0;
							ClipStartPtr = -1 ;			// ClipMode cancel
							break;
					case KEY_CTRL_F1:
						if ( SearchMode ) break;;
							Cursor_SetFlashMode(0); 		// cursor flashing off
							selectVar=SetVar(selectVar);		// A - 
							key=0;
							ClipStartPtr = -1 ;			// ClipMode cancel
							break;
					case KEY_CTRL_F2:
						if ( SearchMode ) break;;
							Cursor_SetFlashMode(0); 		// cursor flashing off
							selectMatrix=SetMatrix(selectMatrix);		//
							key=0;
							ClipStartPtr = -1 ;			// ClipMode cancel
							break;
					case KEY_CTRL_F3:
						if ( SearchMode ) break;;
							Cursor_SetFlashMode(0); 		// cursor flashing off
							SetViewWindow();
							key=0;
							ClipStartPtr = -1 ;			// ClipMode cancel
							break;
					case KEY_CTRL_F5:
						if ( SearchMode ) break;;
							switch (dumpflg) {
								case 2: 		// Opcode
									dumpflg=4;
									break;
								case 4: 		// hex dump
									dumpflg=2;
//								case 16:		// Ascii dump
//									dumpflg=16;
//									break;
								default:
									break;
							}
							offset=csrPtr;
							cx=6; cy=2;
							key=0;
							break;
					case KEY_CTRL_F6:	// Search for text
							SearchForText( searchbuf ) ;
							if ( key == KEY_CTRL_EXIT ) break;
							i=SearchOpcode( SrcBase, searchbuf, &csrPtr);
							if ( i==0 ) ErrorMSGstr1(" Not Found ");
							if ( i ) SearchMode=1; else SearchMode=0;
							ClipStartPtr = -1 ;			// ClipMode cancel
							break;
							
					case KEY_CHAR_POWROOT:
							CB_test();
							key=0;
							ClipStartPtr = -1 ;			// ClipMode cancel
							break;
							
					case KEY_CTRL_CLIP:
							ClipStartPtr=csrPtr;
							key=0;
							break;
					case KEY_CTRL_PASTE:
							EditPaste( FileBase, &csrPtr);
							key=0;
							break;
							
//					case KEY_CTRL_CATALOG:
//							key=CatalogDialog();
//							tmpkey=key;
//							break;
					case KEY_CTRL_ALPHA:
							alphalock = 1 ;
							break;
					default:
						break;
				}
				break;
				
			case KEY_CTRL_DEL:
				if ( CursorStyle < 0x6 ) {		// insert mode
					PrevOpcode( SrcBase, &csrPtr );
				}
				DeleteOpcode( FileBase, &csrPtr);
				key=0;
				ClipStartPtr = -1 ;		// ClipMode cancel
				SearchMode=0;
				break;
			default:
				break;
		}


		if ( dumpflg==2 ) {
			if ( key == KEY_CTRL_F3 ) {
					Cursor_SetFlashMode(0); 		// cursor flashing off
					key=SelectOpcode5800P(oplistCMD,&selectCMD);
				if ( alphalock == 0 ) if ( ( CursorStyle==0x4 ) || ( CursorStyle==0x3 ) || ( CursorStyle==0xA ) || ( CursorStyle==0x9 ) ) PutKey( KEY_CTRL_ALPHA, 1 );
			}
			if ( key == KEY_CTRL_OPTN ) {
					Cursor_SetFlashMode(0); 		// cursor flashing off
					key=SelectOpcode(oplistOPTN,&selectOPTN);
				if ( alphalock == 0 ) if ( ( CursorStyle==0x4 ) || ( CursorStyle==0x3 ) || ( CursorStyle==0xA ) || ( CursorStyle==0x9 ) ) PutKey( KEY_CTRL_ALPHA, 1 );
			}
			if ( key == KEY_CTRL_VARS ) {
					Cursor_SetFlashMode(0); 		// cursor flashing off
					key=SelectOpcode(oplistVARS,&selectVARS);
				if ( alphalock == 0 ) if ( ( CursorStyle==0x4 ) || ( CursorStyle==0x3 ) || ( CursorStyle==0xA ) || ( CursorStyle==0x9 ) ) PutKey( KEY_CTRL_ALPHA, 1 );
			}
			if ( key == KEY_CTRL_PRGM ) {
					Cursor_SetFlashMode(0); 		// cursor flashing off
					key=SelectOpcode(oplistPRGM,&selectPRGM);
				if ( alphalock == 0 ) if ( ( CursorStyle==0x4 ) || ( CursorStyle==0x3 ) || ( CursorStyle==0xA ) || ( CursorStyle==0x9 ) ) PutKey( KEY_CTRL_ALPHA, 1 );
			}
			keyH=(key&0xFF00) >>8 ;
			keyL=(key&0x00FF) ;
			switch ( keyH ) {		// ----- 2byte code -----
				case 0x7F:		// 
				case 0xF7:		// 
				case 0xF9:		// 
				case 0xE5:		// 
				case 0xE6:		// 
				case 0xE7:		// 
				case 0xFF:	// 
					if (ClipStartPtr>=0) { 
						ClipStartPtr = -1 ;		// ClipMode cancel			
						SearchMode=0;
						break;
					}
					if ( CursorStyle < 0x6 ) {		// insert mode
						InsertOpcode( FileBase, csrPtr, key );
					} else {					// overwrite mode
						if ( SrcBase[csrPtr] !=0 ) DeleteOpcode( FileBase, &csrPtr);
						InsertOpcode( FileBase, csrPtr, key );
					}
					if ( ErrorNo==0 ) NextOpcode( SrcBase, &csrPtr );
					key=0;
					alphalock = 0 ;
					break;
				default:
					break;
			}
			if ( key == KEY_CTRL_EXE )   key=0x0D; // <CR>
			if ( ( 0x00 < key ) && ( key < 0xFF ) || ( key == KEY_CTRL_XTT ) ) {		// ----- 1 byte code -----
				if ( ClipStartPtr >= 0 ) { 
						ClipStartPtr = -1 ;		// ClipMode cancel			
						SearchMode=0;
				} else {
					if ( lowercase  && ( 'A' <= key  ) && ( key <= 'Z' ) ) key+=('a'-'A');
//					if ( key == KEY_CHAR_POW )   key='^';
					if ( key == KEY_CTRL_XTT  )   key='X'; // 
					if ( CursorStyle < 0x6 ) {		// insert mode
							InsertOpcode( FileBase, csrPtr, key );
					} else {					// overwrite mode
							if ( SrcBase[csrPtr] !=0 ) DeleteOpcode( FileBase, &csrPtr);
							InsertOpcode( FileBase, csrPtr, key );
					}
					if ( ErrorNo==0 ) NextOpcode( SrcBase, &csrPtr );
					key=0;
				}
			}
		}
	}
	Cursor_SetFlashMode(0); 		// cursor flashing off
	Bkey_Set_RepeatTime(FirstCount,NextCount);		// restore repeat time
	return key;
}

//----------------------------------------------------------------------------------------------

int CB_BreakStop() {
	unsigned int key=1;
	char buf[22];
	int stat;
	int scrmode=ScreenMode;
	CB_SelectTextVRAM();	// Select Text Screen
	CB_SelectGraphVRAM();	// Select Graphic screen
	CB_SelectTextDD();	// Select Text Screen
//	SaveDisp(SAVEDISP_PAGE1);
	PopUpWin(4);
	locate(9,3); Print((unsigned char *)"Break");
	locate(6,5); Print((unsigned char *) "Press:[EXIT]");
	Bdisp_PutDisp_DD();
	
	KeyRecover(); 
	while ( KeyCheckAC() ) ;
	while ( 1 ) {
		GetKey(&key);
		if ( key == KEY_CTRL_EXIT  ) break ;
		if ( key == KEY_CTRL_AC    ) break ;
		if ( key == KEY_CTRL_RIGHT ) break ;
		if ( key == KEY_CTRL_LEFT  ) break ;
	}
//	RestoreDisp(SAVEDISP_PAGE1);
//	Bdisp_PutDisp_DD();
	Contflag=1;	// enable Continue
	key=EditRun(2);	// Program listing & edit
	Contflag=0;	// disable Continue
	
	CB_RestoreTextVRAM();	// Resotre Graphic screen
	if ( scrmode  ) CB_SelectGraphVRAM();	// Select Graphic screen

	if ( key == KEY_CTRL_EXIT  ) { BreakPtr=-999; return BreakPtr; }
	Bdisp_PutDisp_DD();
	return 0;
}

