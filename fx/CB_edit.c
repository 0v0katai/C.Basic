#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <fxlib.h>
#include "fx_syscall.h"
#include "CB_io.h"
#include "CB_inp.h"

#include "CB_file.h"
#include "CB_edit.h"

#include "CB_lib.h"
#include "CB_interpreter.h"
#include "CB_error.h"

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
		ErrorPtr=ptr; ErrorNo=6;		// Memory error
		CB_ErrNo(ErrorNo);
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

void DumpOpcode(unsigned char *SrcBase, int *offset, int csrPtr, int *cx, int *cy){
	int y;
	int ofst;

	*cx=0; *cy=0;
	while ( 1 ) {
		locate(1,2); PrintLine((unsigned char*)" ",21);
		locate(1,3); PrintLine((unsigned char*)" ",21);
		locate(1,4); PrintLine((unsigned char*)" ",21);
		locate(1,5); PrintLine((unsigned char*)" ",21);
		locate(1,6); PrintLine((unsigned char*)" ",21);
		locate(1,7); PrintLine((unsigned char*)" ",21);
		y=2; ofst=(*offset);
		while ( y<8 ) {
				ofst = PrintlineOpcode( &y, SrcBase, ofst, csrPtr, &(*cx), &(*cy));
				if ( SrcBase[ofst]==0x00 ) break ;
				y++;
		}
		if ( ( (*cx)!=0 ) && ( (*cy)!=0 ) ) break ;
		if ( csrPtr <= *offset ) {
					PrevLine( SrcBase, &(*offset) );
		} else {	NextLine( SrcBase, &(*offset) );
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
				locate(18+i,j+2); PrintC((unsigned char*)SrcBase+offset+i+j*4);
		}
	}
}

void DumpAsc(unsigned char *SrcBase, int offset){
	int 	i,j;
	for (j=0; j<6 ; j++){
		Hex4PrintXY( 1, j+2, "", ((int)offset+j*16)&0xFFFF);
		locate( 5,j+2); Print((unsigned char*)"/                ");
		for (i=0; i<16 ; i++){
				locate(6+i,j+2); PrintC((unsigned char*)SrcBase+offset+i+j*16);
		}
	}
}


		
//---------------------------------------------------------------------------------------------
int SelectOpcode(unsigned short *oplist, int *select) {
	int opNum=0 ;
	char buffer[22];
	char tmpbuf[18];
	unsigned int key;
	int	cont=1;
	int i,y;
	int seltop=*select;

	while ( oplist[opNum++] ) ;
	opNum-=2;

	SaveDisp(SAVEDISP_PAGE1);
	PopUpWin(6);

	while (cont) {
		if (  (*select)<seltop ) seltop=(*select);
		if ( ((*select)-seltop) > 5 ) seltop=(*select)-5;
		if ( (opNum-seltop) < 5 ) seltop = opNum-5; 
		for ( i=0; i<6; i++ ) {
			locate(8,2+i); Print((unsigned char *)"            ");
			OpcodeToStr( oplist[seltop+i], (unsigned char*)tmpbuf ) ; // SYSCALL
			tmpbuf[12]='\0'; 
			sprintf(buffer,"%04X:%-12s",oplist[seltop+i],tmpbuf ) ;
			locate(3,2+i); Print((unsigned char *)buffer);
			Bdisp_PutDisp_DD();	
		}
		
		y = ((*select)-seltop) + 1 ;
		Bdisp_AreaReverseVRAM(12, y*8, 113, y*8+7);	// reverse select line 
		Bdisp_PutDisp_DD();

		GetKey( &key );
		switch (key) {
			case KEY_CTRL_EXIT:
				return 0;
			case KEY_CTRL_EXE:
				cont=0;
				break;
		
			case KEY_CTRL_LEFT:
				*select = 0;
				break;
			case KEY_CTRL_RIGHT:
				*select = opNum;
				break;
			case KEY_CTRL_UP:
				(*select)--;
				if ( *select < 0 ) *select = opNum;
				break;
			case KEY_CTRL_DOWN:
				(*select)++;
				if ( *select > opNum ) *select =0;
				break;
			default:
				break;
		}
	}

	RestoreDisp(SAVEDISP_PAGE1);
	Bdisp_PutDisp_DD();
	
	return oplist[(*select)];
}
/*
unsigned short oplistALL[]={
		0x0C,	// dsps
		0x3A,	// :
		0x3F,	// ?
		0x13,	// =>
		0x3C,	// <
		0x3E,	// >
		0x10,	// <=
		0x11,	// !=
		0x12,	// >=
		0x23,	// #
		0x24,	// $
		0x25,	// %
		0x26,	// &
		0x27,	// '
		0xAB,	// !
		0x97,	// Abs
		0xA6,	// Int
		0xB6,	// frac
		0x81,	// sin
		0x82,	// cos
		0x83,	// tan
		0x91,	// arcsin
		0x92,	// arccos
		0x93,	// arctan
		0x85,	// ln
		0x86,	// sqrt
		0x95,	// log
		0xA1,	// sinh
		0xA2,	// cosh
		0xA3,	// tanh
		0xB1,	// arcsinh
		0xB2,	// arccosh
		0xB3,	// arctanh
		0xC1,	// Ran#
		0xD1,	// Cls
		0xD3,	// Rnd
		0xDA,	// Deg
		0xDB,	// Rad
		0xDC,	// Grad
		0xE0,	// Plot
		0xE1,	// Line
		0xE2,	// Lbl
		0xE3,	// Fix
		0xE4,	// Sci
		0xE8,	// Dsz
		0xE9,	// Isz
		0xEB,	// ViewWindow
		0xEC,	// Goto
		0xED,	// Prog
		0xEE,	// Graph Y=

		0x7F00,	// Xmin
		0x7F01,	// Xmax
		0x7F02,	// Xscl
		0x7F04,	// Ymin
		0x7F05,	// Ymax
		0x7F06,	// Yscl
		0x7F0B,	// Xfct
		0x7F0C,	// Yfct

		0x7F3A,	// MOD(
		0x7F46,	// Dim

		0x7F86,	// RanFix(
		0x7F87,	// RanInt#(
		0x7F8F,	// Getkey

		0x7FB0,	// And
		0x7FB1,	// Or
		0x7FB3,	// Not
		0x7FB4,	// Xor

		0x7FF0,	// GraphY

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
		0xF70C,	// Return
		0xF70D,	// Break
		0xF70E,	// Stop
		0xF710,	// Locate
		0xF718,	// ClrText
		0xF719,	// ClrGraph
		0xF71A,	// ClrList
		0xF71C,	// S-L-Normal
		0xF71D,	// S-L-Thick
		0xF71E,	// S-L-Broken
		0xF71F,	// S-L-Dot
		0xF720,	// DrawGraph
		0xF770,	// G-Connect
		0xF771,	// G-Plot
		0xF77A,	// GridOff
		0xF77D,	// GridOn
		0xF78C,	// SketchNormal
		0xF78D,	// SketchThick
		0xF78E,	// SketchBroken
		0xF78F,	// SketchDot
		0xF793,	// StoPict
		0xF794,	// RclPict
		0xF797,	// StoV-Win
		0xF798,	// RclV-Win
		0xF79F,	// RclCapt
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
		0xF7C2,	// AxesOn
		0xF7C3,	// CoordOn
		0xF7C4,	// LabelOn
		0xF7D2,	// AxesOff
		0xF7D3,	// CoordOff
		0xF7D4,	// LabelOff
		
		0xF90B,	// EngOn
		0xF90C,	// EngOff
		0xF91E,	// ClrMat
		0XF921,	// Xdot
		0};
*/
unsigned short oplistOPTN[]={
//		0x23,	// #
//		0x24,	// $
//		0x25,	// %
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

		0xC1,	// Ran#
		0x7F86,	// RanFix(
		0x7F87,	// RanInt#(

		0x7FB0,	// And
		0x7FB1,	// Or
		0x7FB3,	// Not
		0x7FB4,	// Xor

		0xF793,	// StoPict
		0xF794,	// RclPict
//		0xF797,	// StoV-Win
//		0xF798,	// RclV-Win
//		0xF79F,	// RclCapt
		0};
							
unsigned short oplistPRGM[]={	
		0x0C,	// dsps
		0x3A,	// :
		0x3F,	// ?
		0x27,	// '
		
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

		0xF797,	// StoV-Win
		0xF798,	// RclV-Win
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
		0xD3,	// Rnd
		0xE3,	// Fix
		0xE4,	// Sci
		
//		0xF90B,	// EngOn
//		0xF90C,	// EngOff
		0};

//---------------------------------------------------------------------------------------------

void EditRun(int run){		// run:1 exec      run:2 edit
	unsigned char *FileBase,*SrcBase;
	unsigned int key=0,keyH,keyL;
	int cont=1,stat;
	char buffer[32];
	char buffer2[32];
	unsigned char c;
	int i,j,n,d,x,y,cx,cy,ptr;
	int 	offset=0;
	int 	csrPtr=0;
	int 	dumpflg=2;
	unsigned short opcode;
	int selectOPTN=0;
	int selectVARS=0;
	int selectPRGM=0;
	int lowercase=0, CursorStyle;

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

	ProgNo=0;
	if ( run==1 ) key=KEY_CTRL_F6;	// direct run
	
	while ( cont ) {
		ErrorNo=0;
		FileBase = ProgfileAdrs[ProgNo];
		SrcBase  = FileBase+0x56;
		Bdisp_AllClr_VRAM();
		strncpy(buffer2,(const char*)ProgfileAdrs[ProgNo]+0x3C,8);
		buffer2[8]='\0';
		sprintf(buffer,"==%-8s==%08X",buffer2, ProgfileAdrs[ProgNo]);
		locate (1,1); Print( (unsigned char*)buffer );
			Fkey_dispN( 0, "TOP ");
			Fkey_dispN( 1, "BTM");
			if ( lowercase ) Fkey_dispN(3,"a<>A"); else Fkey_dispN(3,"A<>a");
			Fkey_dispR( 4, "CHAR");
			Fkey_dispN( 5, "EXE");
		switch (dumpflg) {
			case 2: 		// Opcode
					Fkey_dispR( 2, "Dump");
					DumpOpcode( SrcBase, &offset, csrPtr, &cx, &cy);
					break;
			case 4: 		// hex dump
					Fkey_dispR( 2, "List");
					DumpMix( SrcBase, offset );
					break;
			case 16:		// Ascii dump
					DumpAsc( SrcBase, offset );
					break;
			default:
					break;
		}
		Bdisp_PutDisp_DD();

		CursorStyle=Cursor_GetFlashStyle();
		if ( ( CursorStyle==0x3 ) && lowercase != 0 ) Cursor_SetFlashOn(0x4);		// lowercase  cursor
		if ( ( CursorStyle==0x4 ) && lowercase == 0 ) Cursor_SetFlashOn(0x3);		// upperrcase cursor
		if ( ( CursorStyle==0x9 ) && lowercase != 0 ) Cursor_SetFlashOn(0xA);		// lowercase  cursor
		if ( ( CursorStyle==0xA ) && lowercase == 0 ) Cursor_SetFlashOn(0x9);		// upperrcase cursor

		locate(cx,cy);
		if ( (dumpflg==2) || (dumpflg==4) ) {
			Cursor_SetFlashMode(1);			// cursor flashing on
			if (Cursor_GetFlashStyle()<0x6)	Cursor_SetFlashOn(0x0);		// insert mode cursor 
				else 						Cursor_SetFlashOn(0x6);		// overwrite mode cursor 
			if (key < 0x7F00)	if (run!=1) GetKey(&key);
		} else
								if (run!=1) GetKey(&key);

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
			case KEY_CTRL_EXIT:
					cont=0;
					break;
			case KEY_CTRL_F1:
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
			case KEY_CTRL_F2:
					offset=EndOfSrc( SrcBase, offset );
					csrPtr=offset;
					switch (dumpflg) {
						case 2: 		// Opcode
							PrevLine( SrcBase, &offset );
							PrevLine( SrcBase, &offset );
							PrevLine( SrcBase, &offset );
							PrevLine( SrcBase, &offset );
							PrevLine( SrcBase, &offset );
							PrevLine( SrcBase, &offset );
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
			case KEY_CTRL_F3:
					switch (dumpflg) {
						case 2: 		// Opcode
							dumpflg=4;
							break;
						case 4: 		// hex dump
							dumpflg=2;
						case 16:		// Ascii dump
							dumpflg=2;
							break;
						default:
							break;
					}
					offset=csrPtr;
					cx=6; cy=2;
//					n  =(csrPtr)-(offset);
//					if ( n > 24) n=0;
//					cx = (n%4)*3+6;
//					cy = (n/4)+2;
					key=0;
					break;
			case KEY_CTRL_F4:
					lowercase=1-lowercase;
					break;
			case KEY_CTRL_F5:
					key=SelectChar();
					break;
			case KEY_CTRL_F6:
					Cursor_SetFlashMode(0); 		// cursor flashing off
					stat = CB_interpreter( SrcBase ) ;	// ====== run interpreter ======
					ProgNo=ProgEntryPtr;
					FileBase = ProgfileAdrs[ProgNo];
					SrcBase  = FileBase+0x56;
					offset = stat;
					if ( stat == -1 ) offset = ExecPtr-1;
					csrPtr = offset;
					key=0;
					run=2; // edit mode
					if ( dumpflg == 2 ) {
						PrevLine( SrcBase, &offset );
						PrevLine( SrcBase, &offset );
						PrevLine( SrcBase, &offset );
						PrevLine( SrcBase, &offset );
						PrevLine( SrcBase, &offset );
						PrevLine( SrcBase, &offset );
						PrevLine( SrcBase, &offset );
					if ( stat == -1 ) cont=0;	// 
					}
					break;
			case KEY_CTRL_LEFT:
				switch ( dumpflg ) {
					case 2: 		// Opcode
							PrevOpcode( SrcBase, &csrPtr );
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
				break;
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
				break;
			case KEY_CTRL_UP:
				switch ( dumpflg ) {
					case 2: 		// Opcode
							if ( cx>1 ) PrevLine( SrcBase, &csrPtr );
							PrevLine( SrcBase, &csrPtr );
							c=SrcBase[csrPtr];
							if ( ( c==0x0C ) || ( c==0x0D ) ) break;
							x=1;
							while ( x < cx ) {
								x += OpcodeStrLenBuf( SrcBase, csrPtr);
								NextOpcode( SrcBase, &csrPtr );
								c=SrcBase[csrPtr];
								if ( ( c==0x0C ) || ( c==0x0D ) ) break;
							}
							if ( x > cx ) PrevOpcode( SrcBase, &csrPtr );
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
				break;
			case KEY_CTRL_DOWN:
				switch ( dumpflg ) {
					case 2: 		// Opcode
							NextLine( SrcBase, &csrPtr );
							c=SrcBase[csrPtr];
							if ( ( c==0x0C ) || ( c==0x0D ) || ( c==0x00 ) ) break;
							x=1;
							while ( x < cx ) {
								x += OpcodeStrLenBuf( SrcBase, csrPtr);
								NextOpcode( SrcBase, &csrPtr );
								c=SrcBase[csrPtr];
								if ( ( c==0x0C ) || ( c==0x0D ) || ( c==0x00 ) ) break;
							}
							if ( x > cx ) PrevOpcode( SrcBase, &csrPtr );
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
				break;
				
			case KEY_CTRL_SHIFT:
				Fkey_dispR( 0, "Var");
				Fkey_Clear( 1 );
				Fkey_dispR( 2, "V-W");
				Fkey_Clear( 3 );
				Fkey_Clear( 4 );
				Fkey_Clear( 5 );
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
									PrevLine( SrcBase, &offset );
									PrevLine( SrcBase, &offset );
									PrevLine( SrcBase, &offset );
									PrevLine( SrcBase, &offset );
									PrevLine( SrcBase, &offset );
									PrevLine( SrcBase, &offset );
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
							SetupG();
							key=0;
							break;
					case KEY_CTRL_F1:
							Cursor_SetFlashMode(0); 		// cursor flashing off
							SetVar(0);		// A - 
							key=0;
							break;
					case KEY_CTRL_F3:
							Cursor_SetFlashMode(0); 		// cursor flashing off
							SetViewWindow();
							key=0;
							break;
					case KEY_CTRL_F6:
							CB_test();
							key=0;
							break;
//					case KEY_CTRL_CATALOG:
//							key=CatalogDialog();
//							break;
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
				break;
			default:
				break;
		}
		
		if ( dumpflg==2 ) {
			if ( key == KEY_CTRL_XTT  )   key='X'; // 
			if ( key == KEY_CTRL_OPTN ) {
					Cursor_SetFlashMode(0); 		// cursor flashing off
					key=SelectOpcode(oplistOPTN,&selectOPTN);
			}
			if ( key == KEY_CTRL_VARS ) {
					Cursor_SetFlashMode(0); 		// cursor flashing off
					key=SelectOpcode(oplistVARS,&selectVARS);
			}
			if ( key == KEY_CTRL_PRGM ) {
					Cursor_SetFlashMode(0); 		// cursor flashing off
					key=SelectOpcode(oplistPRGM,&selectPRGM);
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
					if ( CursorStyle < 0x6 ) {		// insert mode
						InsertOpcode( FileBase, csrPtr, key );
					} else {					// overwrite mode
						DeleteOpcode( FileBase, &csrPtr);
						InsertOpcode( FileBase, csrPtr, key );
					}
					if ( ErrorNo==0 ) NextOpcode( SrcBase, &csrPtr );
					key=0;
					break;
				default:
					break;
			}
			if ( key == KEY_CTRL_EXE )   key=0x0D; // <CR>
			if ( ( 0x00 < key ) && ( key < 0xFF ) ) {		// ----- 1 byte code -----
			if ( lowercase  && ( 'A' <= key  ) && ( key <= 'Z' ) ) key+=('a'-'A');
				if ( key == KEY_CHAR_POW )   key='^';
				if ( CursorStyle < 0x6 ) {		// insert mode
						InsertOpcode( FileBase, csrPtr, key );
				} else {					// overwrite mode
						DeleteOpcode( FileBase, &csrPtr);
						InsertOpcode( FileBase, csrPtr, key );
				}
				if ( ErrorNo==0 ) NextOpcode( SrcBase, &csrPtr );
				key=0;
			}
		}
	}
	Cursor_SetFlashMode(0); 		// cursor flashing off
	Bkey_Set_RepeatTime(FirstCount,NextCount);		// restore repeat time
}

