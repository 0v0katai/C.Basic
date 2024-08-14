extern "C" {

#include "prizm.h"
#include "CBP.h"

//----------------------------------------------------------------------------------------------
//char ClipBuffer[ClipMax];
char *ClipBuffer;
char DisableDebugMode=0;	// 0:enable debug mode
char ExitDebugModeCheck=0;  // ExitDebugModeCheck
char ForceDebugMode;
char DebugMode=0;	// 0:disable   1:cont   2:trace   3:step over   4:step out   9:debug mode start
char ScreenModeEdit=0;
char DebugScreen=0;		// 0:no   1:Screen debug mode with fkey    2: Screen debug mode
char InpMethod=1;		// 0:C.Basic  1:Casio Basic
char InpCommandList=0;	// 0:none  1:OPTN 
short EditLineNum=0;
short CurrentLineNum;
char  EDITpxNum=0;		//  EditLineNumber pixel
int   EDITpxMAX;		//  max x pixel
char  UpdateLineNum;

int EditMaxfree;
int EditMaxProg;
int NewMax;
int ClipMax;

//----------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------

int SrcSize( char *src ) {
	int size ;
	if ( src[0]=='\0' ) return 0 ;
//	size = (src[0x47]&0xFF)*256+(src[0x48]&0xFF)+0x4C;
	size = ((src[0x45]&0xFF)<<24)+((src[0x46]&0xFF)<<16)+((src[0x47]&0xFF)<<8)+(src[0x48]&0xFF)+0x4C;
	return size ;
}
void SetSrcSize( char *src, int size ) {
	int sizeH1,sizeL1 ;
	int sizeH,sizeL ;
	size=size-0x4C;
	sizeH1 = (size&0xFF000000) >> 24 ;
	sizeL1 = (size&0xFF0000) >> 16 ;
	sizeH  = (size&0xFF00) >> 8 ;
	sizeL  = (size&0x00FF) ;
	src[0x45]=sizeH1;
	src[0x46]=sizeL1;
	src[0x47]=sizeH;
	src[0x48]=sizeL;
}
int SrcEndPtr( char *src ) {
	return ( SrcSize( src ) - 0x56 - 1 ) ;
}


int SizeOfSrc( char *SRC, int ptr, int endPtr ){
	while ( SRC[ptr] ) {
		if ( ptr >= endPtr ) return endPtr ;
		NextLineGB( SRC, &ptr );
	}
	return ptr;
}
int FixSrcSize( char *filebase ) {
	int size;
	char *srcptr=filebase+0x56;
	int endPtr=SrcEndPtr( filebase );
	size = SizeOfSrc( srcptr, 0, endPtr ) +1;
	srcptr[size  ]=0x00;
	srcptr[size+1]=0x00;
	srcptr[size+2]=0x00;
	return size;
}

//----------------------------------------------------------------------------------------------
int ProgMoveMem( char *src, int sptr, int eptr, int len ) {
	int i,c;
	for ( i=sptr; i<=eptr; i++ ) {
		c=src[i]; src[i]=src[i+len]; src[i+len]=c; }
	return i;
}

// ******---ABCDEF0
// ******123ABCDEF0  ->
// ******A231BCDEF0  ->
// ******AB312CDEF0  ->
// ******ABC123DEF0  ->
// ******ABCD231EF0  ->
// ******ABCDE312F0  ->
// ******ABCDEF1230  ->
// ******ABCDEF0231
void ProgDeleteSub( char *filebase, int ptr, int len, cUndo *Undo ) {
	int i,c;
	i = ProgMoveMem( filebase, ptr+0x56, SrcSize(filebase), len );
	Undo->enable = 1;	// delete flag
	Undo->sPtr   = ptr+0x56;	//
	Undo->ePtr   = i;	//
	Undo->Len    = len;	//
}

// ******++++0...  ->  ******...++++0
// ******ABCDEF0231  ->
// ******ABCDEF1230  ->
// ******ABCDE312F0  ->
// ******ABCD231EF0  ->
// ******ABC123DEF0  ->
// ******AB312CDEF0  ->
// ******A231BCDEF0  ->
// ******123ABCDEF0
void ProgUndo( char *filebase, int *ptr, cUndo *Undo ) {
	int i,c;
	int sptr = Undo->sPtr;
	int eptr = Undo->ePtr;
	int len = Undo->Len;
	if ( Undo->enable == 0 ) return ;
	if ( Undo->enable == 3 ) {	// paste
		ProgDeleteSub( filebase, sptr, len ,Undo );
		*ptr = sptr;
		len = -len;
	} else {
		for ( i=eptr-1; i>=sptr; i-- ) {
			c=filebase[i+len]; filebase[i+len]=filebase[i]; filebase[i]=c; }
		*ptr = sptr-0x56;
		NextOpcodeGB( filebase+0x56, ptr );
	}
	Undo->enable = 0;	// clear Undo
	SetSrcSize( filebase, SrcSize(filebase)+len ) ; 	// set new file size
}


void InsertOpcode( char *filebase, int ptr, int opcode ){
	int len,i,j;
	char *srcbase;
	len  = OpcodeLenGB( opcode );
	if ( ( len + SrcSize(filebase) ) > ProgfileMax[ProgNo] ) {
		ErrorPtr=ptr; ErrorNo=NotEnoughMemoryERR;		// Memory error
		CB_ErrMsg(ErrorNo);
		return ;
	}
	j=ptr+len+0x56;
	for ( i=ProgfileMax[ProgNo]; i>=j; i-- )
		 filebase[i]=filebase[i-len];

	srcbase=filebase+0x56;
	switch (len) {
		case 1:
			srcbase[ptr  ]= (opcode    )&0xFF ;
			break;
		case 2:
			srcbase[ptr ] = (opcode>> 8)&0xFF ;
			srcbase[ptr+1]= (opcode    )&0xFF ;
			break;
		case 4:
			srcbase[ptr ] = (opcode>>24)&0xFF ;
			srcbase[ptr+1]= (opcode>>16)&0xFF ;
			srcbase[ptr+2]= (opcode>> 8)&0xFF ;
			srcbase[ptr+3]= (opcode    )&0xFF ;
			break;
		
	}
	SetSrcSize( filebase, SrcSize(filebase)+len ) ; 	// set new file size
	ProgfileEdit[ProgNo]=1;	// edit program
	AddOpcodeRecent( opcode ) ;
}

void DeleteOpcode( char *filebase, int *ptr){
	int len,i;
	int opcode;
	opcode=GetOpcodeGB( filebase+0x56, *ptr );
	if ( opcode == 0 ) {
		if ( *ptr == 0 ) return ;
		PrevOpcodeGB( filebase+0x56, &(*ptr) );
		opcode=GetOpcodeGB( filebase+0x56, *ptr );
	}
	len=OpcodeLenGB( opcode );
	if ( len == 0 ) return ;
	for ( i=(*ptr)+0x56; i<=ProgfileMax[ProgNo]; i++ ) filebase[i]=filebase[i+len];

	SetSrcSize( filebase, SrcSize(filebase)-len ) ; 	// set new file size
	ProgfileEdit[ProgNo]=1;	// edit program
}

void DeleteOpcodeUndo( char *filebase, int *ptr, cUndo *Undo ){
	int len,i;
	int opcode;
	opcode=GetOpcodeGB( filebase+0x56, *ptr );
	if ( opcode == 0 ) {
		if ( *ptr == 0 ) return ;
		PrevOpcodeGB( filebase+0x56, &(*ptr) );
		opcode=GetOpcodeGB( filebase+0x56, *ptr );
	}
	len=OpcodeLenGB( opcode );
	if ( len == 0 ) return ;
//	for ( i=(*ptr)+0x56; i<=ProgfileMax[ProgNo]; i++ ) filebase[i]=filebase[i+len];
	ProgDeleteSub( filebase, (*ptr), len, Undo  );

	SetSrcSize( filebase, SrcSize(filebase)-len ) ; 	// set new file size
	ProgfileEdit[ProgNo]=1;	// edit program
}

int EndOfSrc( char *SRC, int ptr ) {
	char *filebase,*srcbase;
	int endPtr;
	filebase=ProgfileAdrs[ProgNo];
	srcbase=filebase+0x56;
	endPtr=SrcEndPtr( filebase );
	if (SRC < srcbase ) SRC=srcbase;
	return SizeOfSrc( SRC, ptr, endPtr );
}

//---------------------------------------------------------------------------------------------
void EditPaste( char *filebase, char *Buffer, int *ptr, cUndo *Undo ){
	int len,i,j;
	char *srcbase;

	if ( Buffer[0]=='\0' ) return ;	// no clip data
		
	len=strlenOpGB(Buffer);
	if ( ( len <=0 ) || ( ClipMax<=len ) ) return;
	if ( ( len + SrcSize(filebase) ) > ProgfileMax[ProgNo] ) {
		ErrorPtr=(*ptr); ErrorNo=NotEnoughMemoryERR;		// Memory error
		CB_ErrMsg(ErrorNo);
		return ;
	}
	j=(*ptr)+len+0x56;
	for ( i=ProgfileMax[ProgNo]; i>=j; i-- ) filebase[i]=filebase[i-len];
		 
	srcbase=filebase+0x56+(*ptr);
	for ( i=0; i<len; i++ ) srcbase[i]=Buffer[i];	// copy from Buffer
			
	Undo->enable = 3;	// pasetelete flag
	Undo->sPtr   = (*ptr);	//
	Undo->ePtr   = (*ptr)+len;	//
	Undo->Len    = len;	//

	SetSrcSize( filebase, SrcSize(filebase)+len ) ; 	// set new file size
	ProgfileEdit[ProgNo]=1;	// edit program
	
	(*ptr)=(*ptr)+len;
}

void EditCopy( char *filebase, char *Buffer, int ptr, int startp, int endp ){
	int len,i,j;
	char *srcbase;
	
	PrevOpcodeGB( filebase+0x56, &endp ); if ( startp>endp ) return;
	i=OpcodeLenGB( GetOpcodeGB(filebase+0x56, endp ) );
	len=(endp)-(startp)+i;
	if ( len <=0 ) return;
	if ( len > ClipMax ) {
		ErrorPtr=ptr; ErrorNo=NotEnoughMemoryERR;		// Memory error
		CB_ErrMsg(ErrorNo);
		return ;
	}
	
	srcbase=filebase+0x56+(startp);
	for ( i=0; i<len; i++ ) Buffer[i]=srcbase[i];	// copy to Buffer
	Buffer[i]='\0';
}

void EditCutDel( char *filebase, char *Buffer, int *ptr, int startp, int endp, int del, cUndo *Undo ){	// del:1 delete
	int len,i,flag=0;
	char *srcbase;

	if ( startp>endp ) { i=startp; startp=endp; endp=i; flag=1; }
	PrevOpcodeGB( filebase+0x56, &endp ); if ( startp>endp ) return;
	i=OpcodeLenGB( GetOpcodeGB(filebase+0x56, endp )  );
	len=(endp)-(startp)+i;
	if ( len <=0 ) return;
	if ( del == 0 ) {
		if ( len > ClipMax ) {
			ErrorPtr=(*ptr); ErrorNo=NotEnoughMemoryERR;		// Memory error
			CB_ErrMsg(ErrorNo);
			return ;
		}
		srcbase=filebase+0x56+(startp);
		for ( i=0; i<len; i++ ) Buffer[i]=srcbase[i];	// copy to Buffer
		Buffer[i]='\0';
	}
//	for ( i=(startp)+0x56; i<=ProgfileMax[ProgNo]; i++ ) filebase[i]=filebase[i+len];
	ProgDeleteSub( filebase, (startp), len, Undo );

	SetSrcSize( filebase, SrcSize(filebase)-len ) ; 	// set new file size
	ProgfileEdit[ProgNo]=1;	// edit program

	(*ptr)=(startp);
}

//------------------------------------------------------------------------------

int AddComment( char *filebase, int ptr, int startp, int endp ){
	int len,i,j,plus=0;
	char *srcbase=filebase+0x56;
	int c; 
	int flag=0;
	if ( startp<0 ) return ptr;
	if ( startp==0 ) flag=1;
	PrevLineGB( srcbase, &startp );
	ptr=startp;
	if ( (flag) && ( ptr==0 ) ) { 
		InsertOpcode( filebase, ptr, 0x27 );
		plus++;
	}
	c=srcbase[ptr++];
	while ( ptr<endp+plus ) {
		switch ( c ) {
			case 0x00:	// <EOF>
				return endp+plus;
			case 0x0C:	// dsps
			case 0x0D:	// <CR>
				InsertOpcode( filebase, ptr, 0x27 );
				plus++;
				break;
			case 0x7F:	// 
			case 0xFFFFFFF7:	// 
			case 0xFFFFFFF9:	// 
			case 0xFFFFFFE5:	// 
			case 0xFFFFFFE6:	// 
			case 0xFFFFFFE7:	// 
			case 0xFFFFFFFF:	// 
				ptr++;
				break;
			default:
				if ( GBcode ) if ( IsGBCode2(c, srcbase[ptr]) ) ptr++;	// GB code
				break;
		}
		c=srcbase[ptr++];
	}
	return endp+plus;
}

int DelComment( char *filebase, int ptr, int startp, int endp ){
	int len,i,j,plus=0;
	char *srcbase=filebase+0x56;
	int c; 
	if ( startp<0 ) return ptr;
	ptr=startp;
	if ( srcbase[ptr]==0x27 ) {
		DeleteOpcode( filebase, &ptr );
		plus--;
	} else PrevLineGB( srcbase, &startp );
	c=srcbase[ptr++];
	while ( ptr<endp+plus ) {
		switch ( c ) {
			case 0x00:	// <EOF>
				return endp+plus;
			case 0x0C:	// dsps
			case 0x0D:	// <CR>
				if ( srcbase[ptr]==0x27 ) {
					DeleteOpcode( filebase, &ptr );
					plus--;
				}
				break;
			case 0x7F:	// 
			case 0xFFFFFFF7:	// 
			case 0xFFFFFFF9:	// 
			case 0xFFFFFFE5:	// 
			case 0xFFFFFFE6:	// 
			case 0xFFFFFFE7:	// 
			case 0xFFFFFFFF:	// 
				ptr++;
				break;
			default:
				if ( GBcode ) if ( IsGBCode2(c, srcbase[ptr]) ) ptr++;	// GB code
				break;
		}
		c=srcbase[ptr++];
	}
	return endp+plus;
}


//---------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------
//int MiniCursorDummy1;
char  MiniCursorClipMode=0;
char  MiniCursorflag = 0;	// Cursor Pixel ON:1 OFF:0
short MiniCursorX;
short MiniCursorY;
//int MiniCursorDummy2;

void MiniCursorFlashing() {		// timer IRQ handler
	unsigned short *VRAM=(unsigned short *)PictAry[0];
	DISPBOX area;
	unsigned char CsrDATA[]={ 0xFF,0xFF,0xFF,0xFF,0xFF };
	DISPGRAPH minicsr;
	int x,y,c;
	int mini=EditFontSize & 0x0F;
	int MiniCursorDX;
	int MiniCursorDY;
	int r,g,b;
	int overwrite = Setup_GetEntry(0x15) == 01 ;	// 01:OverWrite   02:Insert
	int minicsry;
	int f;
	if ( MiniCursorClipMode ) overwrite = 0;

	if ( ( 0<=MiniCursorX ) && ( MiniCursorX<384 ) && ( -24<=MiniCursorY ) && ( MiniCursorY<192 ) ) {
		minicsr.x = MiniCursorX;
		minicsr.y = MiniCursorY;
		minicsr.x++;
		switch ( mini ) {
			case 0:	// Standard font fixed 18
			case 8:	// Standard font GB24
				MiniCursorDY = 22;
				MiniCursorDX = 16; 
				if ( ( CB_G1MorG3M==1 ) && EditExtFont ) {
					minicsr.x++;
					if ( overwrite ) { minicsr.x++; }
					else
					if ( MiniCursorClipMode ) { minicsr.x--; MiniCursorDY +=2; }
				} else {
					if ( overwrite ) { minicsr.x++; minicsr.y++; }
					if ( MiniCursorClipMode ) { minicsr.x--; MiniCursorDY +=2; }
				}
				break;
				
			case 3:	// minimini
			case 5:	// minimini Bold
				MiniCursorDY = 10;
				MiniCursorDX = 6;
				break;
			case 4:	// minimini fix
				MiniCursorDY = 10;
				MiniCursorDX = 8;
				break;
			case 6: // minimini Bold fix
				MiniCursorDY = 10;
				MiniCursorDX = 9;
				break;
			case 11:	// minimini			+Gap
			case 13:	// minimini Bold	+Gap
				MiniCursorDY = 11;
				MiniCursorDX = 6;
				break;
			case 12:	// minimini fix		+Gap
				MiniCursorDY = 11;
				MiniCursorDX = 8;
				break;
			case 14:	// minimini Bold fix+Gap
				MiniCursorDY = 11;
				MiniCursorDX = 9;
				break;

			case 7: // minimini fx 6x8
				MiniCursorDY = 8;
				MiniCursorDX = 6;
				break;
			case 15:	// minimini fx 6x8	+Gap
				MiniCursorDY = 9;
				MiniCursorDX = 6;
				break;

			case 1:	// mini
				MiniCursorDY = 16; 
				MiniCursorDX = 8;
				if ( ( CB_G1MorG3M==1 ) && EditExtFont ) minicsr.x--;
				else
				if ( MiniCursorClipMode ) { minicsr.x--; MiniCursorDY +=1; }
				break;
			case 2:	// mini fix
				MiniCursorDY = 16; 
				MiniCursorDX = 11;
				if ( ( CB_G1MorG3M==1 ) && EditExtFont ) minicsr.x--;
				if ( MiniCursorClipMode ) { minicsr.x--; MiniCursorDY +=1; }
				break;
			case 9:		// mini				+Gap
				MiniCursorDY = 17; 
				MiniCursorDX = 12;
				if ( ( CB_G1MorG3M==1 ) && EditExtFont ) minicsr.x--;
				else
				if ( MiniCursorClipMode ) { minicsr.x--; MiniCursorDY +=2; }
				break;
			case 10:	// mini fix			+Gap
				MiniCursorDY = 17; 
				MiniCursorDX = 12;
				if ( ( CB_G1MorG3M==1 ) && EditExtFont ) minicsr.x--;
				else
				if ( MiniCursorClipMode ) { minicsr.x--; MiniCursorDY +=2; }
				break;
		}
		f = MiniCursorflag & 31 ;
		if ( f<16 ) {
				minicsr.GraphData.width = 2; if ( MiniCursorDX==16 ) minicsr.GraphData.width = 3;
				if  ( overwrite  ) minicsr.GraphData.width = MiniCursorDX;	// 
				if ( minicsr.x<0 ) minicsr.x=0;
				if ( minicsr.x>383 ) minicsr.x=383;
				if ( minicsr.y<-24 ) minicsr.y=-24;
				if ( minicsr.y>192-MiniCursorDY ) minicsr.y=192-MiniCursorDY;
				if ( minicsr.x + minicsr.GraphData.width > 383 ) minicsr.GraphData.width =1;
				minicsr.GraphData.height  = MiniCursorDY;
				minicsr.GraphData.pBitmap = CsrDATA; 	// mini cursor pattern
				minicsr.WriteModify       = IMB_WRITEMODIFY_NORMAL;
				minicsr.WriteKind         = IMB_WRITEKIND_OR;
				minicsr.y+=24;
				minicsry=minicsr.y; 
				c = VRAM[minicsr.x + (minicsry)*384]; if ( overwrite ) c=CB_BackColorIndex ;
				b = ((c&0x001F) << 3);
				g = ((c&0x07E0) >> 3);
				r = ((c&0xF800) >> 8);
				if ( b*1+g*5+3*r > 5*256 ) c=0x0000; else c=0xFFFF;
				if ( minicsr.GraphData.height == 1 ) {
					y=minicsr.y+1;
						for ( x=minicsr.x; x<minicsr.x+5; x++ ) {
							Bdisp_SetPoint_DD( x+3, y  , c );
						}
				} else {
					for ( y=minicsr.y; y<minicsr.y+minicsr.GraphData.height; y++ ) {
							for ( x=minicsr.x; x<minicsr.x+minicsr.GraphData.width; x++ ) 
							if ( x<383 ) Bdisp_SetPoint_DD( x, y , c );
					}
				}
		} else {
				y=(MiniCursorY+24);
				if ( IsEmu ) Bdisp_PutDisp_DD();
				else Bdisp_PutDisp_DD_stripe( y, y+MiniCursorDY+1);
		}
		MiniCursorflag++;
	}
}

void MiniCursorSetFlashMode(int set) {	// 1:on  0:off
	switch (set) {
		case 0:
			if ( timer_id ) KillTimer(timer_id);
			timer_id = 0;
			break;
		case 1:
			if ( timer_id ) KillTimer(timer_id);
			timer_id = SetTimer(timer_id0, 25, &MiniCursorFlashing );
		  init:
			MiniCursorflag=0;		// mini cursor initialize
			break;
		default:
			break;
	}
}

//---------------------------------------------------------------------------------------------
int OpcodeStrLenBufpx(char *buffer, int ofst) {
	char tmpbuf[18];
	int slen;
	int mini=EditFontSize & 0x0F;
	int opcode = GetOpcodeGB( buffer, ofst );
	if ( opcode=='\0' ) return 0;
	if ( ( opcode==0x0C ) || ( opcode==0x0D ) ) {
		switch ( mini&7 ) {
			case 1:	// mini
			case 2:	// mini fix
				return 12;
			case 3:	// minimini
			case 4:	// minimini fix
			case 5:	// minimini Bold
			case 6: // minimini Bold fix
				return 8;
			case 7: // minimini fx 6x8
				return 6;
			case 0:	// normal
				return 18;
		}
	}
	CB_OpcodeToStr( opcode, tmpbuf ) ;		// SYSCALL
	slen = CB_MB_ElementCount( tmpbuf ) ;	// SYSCALL
	switch ( mini&7 ) {
		case 0:	// normal
			if ( ( CB_G1MorG3M==1 ) && EditExtFont )
				return slen*18;	// normal
			else {
				if ( ( slen==1 ) && ( CheckOpcodeGB( opcode ) ) ) { if ( mini&8 ) return 24; else return 18; }	// GB code
				return slen*18;	// normal
			}
		case 1:	// mini
			if ( ( CB_G1MorG3M==1 ) && EditExtFont )
				return CB_PrintMiniLengthStr_fxed( (unsigned char*)tmpbuf, EditExtFont);	// minifont ext font mode
			else
				return CB_PrintMiniLengthStr( (unsigned char*)tmpbuf, EditExtFont);			// minifont ext font mode
		case 2:	// mini fix
			return slen*12;
		case 3:	// minimini
			return CB_PrintMiniMiniLengthStr( (unsigned char*)tmpbuf);	// miniminifont
		case 4:	// minimini fix
			return slen*8;
		case 5:	// minimini Bold
			return CB_PrintMiniMiniBoldLengthStr( (unsigned char*)tmpbuf);	// miniminifont bold
		case 6: // minimini Bold fix
			return slen*9;
		case 7: // minimini fx 6x8
			return slen*6;
	}
	return slen*18;	// normal
}

int OpcodeLineSub( char *buffer, int *ofst , int *len, char *opstr ) {
	int opcode = GetOpcodeGB( buffer, *ofst );
	if ( opcode=='\0' ) return 0;
	if ( ( opcode==0x0C ) || ( opcode==0x0D ) ) return 0 ;
	(*ofst) += OpcodeLenGB( opcode );
	CB_OpcodeToStr( opcode, opstr ) ;		// SYSCALL
	(*len) = CB_MB_ElementCount( opstr ) ;	// SYSCALL
	return 1;
}

int OpcodeMiniwidth( int mini, char *str, int *oplen ){
	int c=(char)*str & 0xFF;
	int width;
	int opcode;
	int IsGB;
	switch ( mini&7 ) {
		case 0:	// normal
			if ( ( CB_G1MorG3M==1 ) && EditExtFont )
				width=18;	// normal
			else {
				opcode   = GetOpcodeGB( str, 0 );
				if ( CheckOpcodeGB( opcode ) ) { (*oplen)=2; if ( mini&8 ) return 24; else return 18; } // GB code
				width=18;	// normal
			}
			break;
		case 1:	// mini
			if ( ( CB_G1MorG3M==1 ) && EditExtFont ) {
				width=CB_PrintMiniLength_fxed( (unsigned char *)str, EditExtFont);	// minifont ext font mode
			} else {
				width=CB_PrintMiniLength( (unsigned char *)str, EditExtFont);		// minifont ext font mode
			}
			break;
		case 3:	// minimini
			width=CB_PrintMiniMiniLength( (unsigned char *)str++ );	// miniminifont 
			break;
		case 5:	// minimini Bold
			width=CB_PrintMiniMiniBoldLength( (unsigned char *)str++ );	// miniminifont bold
			break;
	}
	opcode   = GetOpcodeGB( str, 0 );
	(*oplen) = OpcodeLenGB( opcode );
	return width;
}
void OpcodeLineXYsub( char *buffer, int len, int *px, int *y ) {
	int i,oplen;
	char *opstr2;
	int mini=EditFontSize & 0x0F;
	opstr2=(char *)buffer;
	i=0;
	while ( i < len ) {
		switch ( mini&7 ) {
			case 0:	// normal
			case 1:	// mini
			case 3:	// minimini
			case 5:	// minimini Bold
				(*px)+=OpcodeMiniwidth( mini, opstr2, &oplen ); opstr2+=oplen;	// 
				break;
			case 2:	// mini fix
				(*px)+=12;
				break;
			case 4:	// minimini fix
				(*px)+=8;
				break;
			case 6: // minimini Bold fix
				(*px)+=9;
				break;
			case 7: // minimini fx 6x8
				(*px)+=6;
				break;
		}
		if ( (*px) > EDITpxMAX ) { (*y)++; (*px)=1; }
		i++;
	}
}

void OpcodeLineN( char *buffer, int *ofst, int *px, int *y ) {
	int i,len,cont=1;
	char opstr[18];
	char *opstr2;
	int oplen;
	(*px)=1;
	while ( cont ) {
		if ( OpcodeLineSub( buffer, &(*ofst), &len, opstr ) == 0 ) break;
		OpcodeLineXYsub( opstr, len, &(*px), &(*y) );
	}
	return ;
}
int OpcodeLineYptr(int n, char *buffer, int ofst, int *px ) {
	int i,len,y=0,cont=1;
	char opstr[18];
	char *opstr2;
	int oplen;
	(*px)=1;
	if ( y == n ) return ofst;
	while ( cont ) {
		if ( OpcodeLineSub( buffer, &ofst, &len, opstr ) == 0 ) break;
		OpcodeLineXYsub( opstr, len, &(*px), &y );
		if ( y > n ) break;
	}
	return ofst;
}
int OpcodeLinePtr(int n, char *buffer, int ofst ) {
	int i,len,y=0,cont=1;
	char opstr[18];
	char *opstr2;
	int oplen;
	int px=1;
	if ( y == n ) return ofst;
	while ( cont ) {
		if ( OpcodeLineSub( buffer, &ofst, &len, opstr ) == 0 ) break;
		OpcodeLineXYsub( opstr, len, &px, &y );
		if ( y == n ) break;
	}
	return ofst;
}
int OpcodeLineSrcXendpos(int n, char *buffer, int ofst) {
	int i,len,px0,px=1,y=0,cont=1;
	char opstr[18];
	char *opstr2;
	int oplen;
	while ( cont ) {
		if ( OpcodeLineSub( buffer, &ofst, &len, opstr ) == 0 ) break;
		px0=px;
		OpcodeLineXYsub( opstr, len, &px, &y );
		if ( y > n ) break;
	}
	return px0;
}
int OpcodeLineSrcYpos( char *buffer, int ofst, int csrptr ) {
	int i,len=0,px=1,y=0,cont=1;
	char opstr[18];
	char *opstr2;
	int oplen;
	if ( ofst==csrptr ) return y;
	while ( cont ) {
		if ( OpcodeLineSub( buffer, &ofst, &len, opstr ) == 0 ) break;
		OpcodeLineXYsub( opstr, len, &px, &y );
		if ( ofst==csrptr ) break;
	}
	return y;
}

void NextLinePhy( char *buffer, int *ofst, int *ofst_y ) {
	int ofst2,px,y;
	if ( *ofst_y == 0 ) {
		ofst2= *ofst;
		px=1; y=0;
		OpcodeLineN( buffer, &ofst2 ,&px ,&y);
		if ( y==0 ) {
			NextLineGB( buffer, &(*ofst));
			*ofst_y = 0;
		} else {
			*ofst_y = 1;
		}
//		EditLineNum++;		// Physical line number
	} else {
		ofst2= *ofst;
		px=1; y=0;
		OpcodeLineN( buffer, &ofst2 ,&px ,&y);
		if ( y > *ofst_y ) {
			(*ofst_y)++;
		} else {
			NextLineGB( buffer, &(*ofst));
			*ofst_y = 0;
		}
	}
}

void PrevLinePhy( char *buffer, int *ofst, int *ofst_y ) {
	int ofst2,px,y;
	if ( *ofst_y == 0 ) {
		if ( *ofst != 0 ) { 
			PrevLineGB( buffer, &(*ofst));
			ofst2= *ofst;
			px=1; y=0;
			OpcodeLineN( buffer, &ofst2 ,&px ,&y);
			if ( y==0 ) {
				*ofst_y = 0;
			} else {
				*ofst_y = y;
			}
		}
//		if ( EditLineNum > 1 ) EditLineNum--;		// Physical line number
	} else {
		(*ofst_y)--;
	}
}

int NextLinePhyN( int n, char *SrcBase, int *offset, int *offset_y ) {
	int i;
	for ( i=0; i<n; i++ ) {
		if ( SrcBase[(*offset)] == 0 ) break;
		NextLinePhy( SrcBase, &(*offset), &(*offset_y) );
	}
	return i;
}
int PrevLinePhyN( int n, char *SrcBase, int *offset, int *offset_y ) {
	int i;
	for ( i=0; i<n; i++ ) {
		if ( ( (*offset) == 0 ) && ( (*offset_y) == 0 ) ) break;
		PrevLinePhy( SrcBase, &(*offset), &(*offset_y) );
	}
	return i;
}

int HomeLineNum( char * SrcBase, int offset ) {	// Logical line numbrer
	int ofst;
	int c,d,no;
	ofst=0;
	no=1;
	while ( ofst < offset ){
		c = SrcBase[ofst++];
		switch ( c ) {
			case 0x00:	// <EOF>
				return no;
//			case 0x0C:	// dsps
			case 0x0D:	// <CR>
				no++;
				break;
			case 0x7F:	// 
			case 0xFFFFFFF7:	// 
			case 0xFFFFFFF9:	// 
			case 0xFFFFFFE5:	// 
			case 0xFFFFFFE6:	// 
			case 0xFFFFFFE7:	// 
			case 0xFFFFFFFF:	// 
				ofst++;
				break;
			default:
				if ( GBcode ) {
					if ( IsGBCode2(c,SrcBase[ofst]) ) ofst++;	// GB code
				}
				break;
		}
	}
	return no ; // 
}

int JumpLineNum( char * SrcBase, int lineNum ) {	// Logical line numbrer
	int ofst;
	int c,d,no;
	ofst=0; 
	no=1;
	while ( no < lineNum ){
		c = SrcBase[ofst++];
		switch ( c ) {
			case 0x00:	// <EOF>
				return ofst;
//			case 0x0C:	// dsps
			case 0x0D:	// <CR>
				no++;
				break;
			case 0x7F:	// 
			case 0xFFFFFFF7:	// 
			case 0xFFFFFFF9:	// 
			case 0xFFFFFFE5:	// 
			case 0xFFFFFFE6:	// 
			case 0xFFFFFFE7:	// 
			case 0xFFFFFFFF:	// 
				ofst++;
				break;
			default:
				if ( GBcode ) {
					if ( IsGBCode2(c,SrcBase[ofst]) ) ofst++;	// GB code
				}
				break;
		}
	}
	return ofst;
}

int AllLineNum( char * SrcBase ) {	// Logical all line numbrer
	int ofst;
	int c,d,no;
	ofst=0;
	no=1;
	while ( 1 ){
		c = SrcBase[ofst++];
		switch ( c ) {
			case 0x00:	// <EOF>
				return no;
//			case 0x0C:	// dsps
			case 0x0D:	// <CR>
				no++;
				break;
			case 0x7F:	// 
			case 0xFFFFFFF7:	// 
			case 0xFFFFFFF9:	// 
			case 0xFFFFFFE5:	// 
			case 0xFFFFFFE6:	// 
			case 0xFFFFFFE7:	// 
			case 0xFFFFFFFF:	// 
				ofst++;
				break;
			default:
				if ( GBcode ) {
					if ( IsGBCode2(c,SrcBase[ofst]) ) ofst++;	// GB code
				}
				break;
		}
	}
	return no ; // 
}
/*
int AllLineNum( char * SrcBase ) {	// Logical line numbrer
	int ptr=0;
	ptr=EndOfSrc( SrcBase, ptr );
	return HomeLineNum( SrcBase, ptr );
}
*/

int SetEditScreenSize( int *ymin, int *ymax, int *ymaxpos, int *yk ) {	// return mini
	int mini=EditFontSize & 0x0F ;
	EDITpxNum=6*4+2;
	GBcode = EditGBFont;
	switch ( mini ) {
		case 0:	// normal fixed  18
		case 8:	// normal GBfont 24
			*ymax=7; *yk=24;	// 168dot
			EDITpxNum=12*3-2;
//			GBcode = EditGBFont;
			break;
		case 1:	// mini
		case 2:	// mini fix
			*ymax=10; *yk=17;	// 170 dot
			EDITpxNum=6*4+5;
//			GBcode = EditGBFont;
			break;
		case 3:	// minimini
		case 4:	// minimini fix
		case 5:	// minimini Bold
		case 6: // minimini Bold fix
			*ymax=15; *yk=11;	// 165 dot
			break;
		case 7: // minimini fx 6x8
			*ymax=21; *yk=8;	// 168 dot
			break;
		case 9:		// mini				+Gap
		case 10:	// mini fix			+Gap
			*ymax=9; *yk=19;	// 171 dot
			EDITpxNum=6*4+5;
//			GBcode = EditGBFont;
			break;
		case 11:	// minimini			+Gap
		case 12:	// minimini fix		+Gap
		case 13:	// minimini Bold	+Gap
		case 14:	// minimini Bold fix+Gap
			*ymax=13; *yk=13;	// 169 dot
			break;
		case 15:	// minimini fx 6x8	+Gap
			*ymax=17; *yk=10;	// 170 dot
			break;
	}
	*ymaxpos=*ymax; 
	if ( EditTopLine  ) { *ymin=1; } else { *ymin=2; *ymax--;}
	if ( ( EditFontSize & 0xF0 ) == 0 ) EDITpxNum=0;
	EDITpxMAX=123*3-6-EDITpxNum;
	return mini ;
}
int PrintOpcodeLineN( int *csry, int ynum, int ymax, int *n, char *buffer, int ofst, int csrPtr, int *pcx, int *cy, int ClipStartPtr, int ClipEndPtr, int *NumOfset, int *quotflag, int *quotflag2 ) {
	char tmpbuf[18];
	unsigned char buff[16];
	unsigned char *buf;
	unsigned char *tmpb;
	int i,j,k,len,px=1,y=0,rev;
	int opcode=0x20,preopcode=0x20,preopcode2=0x20;
	int  c=1,d;
	int Numflag=0,CurrentLine;
	int yk,mini,ymin,ymaxpos;
	int colortmp=CB_ColorIndex;
	unsigned short backcolortmp=CB_BackColorIndex;
	int g1mg3m=CB_G1MorG3M;
	int ExpTYPE=CB_Round.ExpTYPE;
	int IsGB;
	mini=SetEditScreenSize( &i, &j, &ymaxpos, &yk );
	if ( ClipEndPtr < ClipStartPtr ) { i=ClipStartPtr; ClipStartPtr=ClipEndPtr; ClipEndPtr=i; }
	while ( 1 ) {
		CurrentLine=EditLineNum+(*NumOfset);
		rev=0; if ( ( ClipStartPtr >= 0 ) && ( ClipStartPtr <= ofst ) && ( ofst < ClipEndPtr ) ) rev=1;
		if ( y == (*n) )if (ofst==csrPtr) { *pcx=px; *cy=(*csry); CurrentLineNum=CurrentLine; }
		preopcode2 = preopcode;
		preopcode  = opcode;
		opcode = GetOpcodeGB( buffer, ofst );
		IsGB = CheckOpcodeGB( opcode );
		if ( opcode=='\0' ) break;
		if ( ( opcode==0x27 ) && ( (*quotflag)!=1 ) && ( (*quotflag)!=2 ) && ( GetOpcodeGB( buffer, ofst+1 ) != '/' ) && ( GetOpcodeGB( buffer, ofst+1 ) != '#' ) ) (*quotflag)=((*quotflag)==0); 
		if ( ( opcode==0x22 ) && ( (*quotflag)==2 ) && ( preopcode2 == 0x5C ) && ( preopcode == 0x5C ) ) (*quotflag)= 3;
		if ( ( opcode==0x22 ) && ( (*quotflag)==2 ) && ( preopcode != 0x5C ) ) (*quotflag)= 3;
		ofst += OpcodeLenGB( opcode );
		if ( ( opcode==0x22 ) && ( (*quotflag)==0 ) ) { (*quotflag)= 2;
			if ( CheckQuotCR( buffer, ofst ) ) (*quotflag2)=1;
		}
		if ( ( CB_Round.ExpTYPE == 1 ) && ( mini ) ) CB_Round.ExpTYPE=0;
		CB_OpcodeToStr( opcode, tmpbuf ) ; // SYSCALL
		CB_Round.ExpTYPE=ExpTYPE;
		len = CB_MB_ElementCount( tmpbuf ) ;				// SYSCALL
		tmpb=(unsigned char*)&tmpbuf[0];
		i=0;
		while ( i < len ) {
			CB_BackColorIndex=CB_ColorIndexEditBack;	// back color
			if ( y == (*n) ) {
				if ( ( Numflag==0 ) && ( EDITpxNum ) ) { Numflag=1;
					sprintf3( (char*)buff, "%4d", CurrentLine );
					CB_ColorIndex=CB_ColorIndexEditLine;	// Blue
					k=((*csry)-1)*yk;
					switch ( mini&7 ) {
						case 0:	// normal
							CB_PrintMini_Fix10( -3, k+4, buff, MINI_OR );
							break;
						case 1:	// mini
						case 2:	// mini fix
							CB_PrintMiniMini_Fix7( 0, k+4, buff, MINI_OR );
							break;
						case 3:	// minimini
						case 4:	// minimini fix
						case 5:	// minimini Bold
						case 6: // minimini Bold fix
							CB_PrintMiniMiniFX( -1, k+1, buff, MINI_OR ) ;
							break;
						case 7: // minimini fx 6x8
							CB_PrintMiniMiniFX( -1, k, buff, MINI_OR ) ;
							break;
					}
				}
				CB_ColorIndex=CB_ColorIndexEditBase; // base color
				if ( ( len > 1 ) || ( opcode==0x0C ) || ( opcode==0x0E ) || ( opcode==0x13 ) ) CB_ColorIndex=CB_ColorIndexEditCMD;	// Blue
				if ( ( ( '0'<=opcode )&&( opcode<='9' ) ) || ( opcode=='.' ) || ( opcode==0x87 )|| ( opcode==0x0F ) ) CB_ColorIndex=CB_ColorIndexEditNum; // Numeric color
				if ( (*quotflag)==1 ) CB_ColorIndex=CB_ColorIndexEditComt;	// Green
				if ( (*quotflag)>=2 ) CB_ColorIndex=CB_ColorIndexEditQuot;	// Magenta
				if ( rev ){	CB_ColorIndex^=0xFFFF; CB_BackColorIndex^=0xFFFF; }	// reverse set
				c=px+EDITpxNum; d=((*csry)-1)*yk;
				if ( ( mini==0 ) || ( mini==8 ) ) { 
					if ( ( CB_G1MorG3M==1 ) && EditExtFont ) {
						CB_G1MorG3M=1;
					} else {
						CB_G1MorG3M=3;
					}
					if ( mini==8 ) CB_PrintModeC(  c, d, (unsigned char*)tmpb, MINI_OVER | (0x100*EditExtFont) | 0x10000000 );
					else		   CB_PrintModeC(  c, d, (unsigned char*)tmpb, MINI_OVER | (0x100*EditExtFont) | 0x30000000 );	// fixed 18dot
					CB_G1MorG3M=g1mg3m;
					px+=18; if ( (mini==8)&&(IsGB) ) px+=6;
				} else { 	
					switch ( mini&7 ) {
						case 1:	// mini
							if ( ( CB_G1MorG3M==1 ) && EditExtFont ) {
								if ( rev )	px+=CB_PrintMiniC_fx_ed( c, d, tmpb, MINI_OVER | (0x100*EditExtFont) | 0x10000000 ) ;
								else		px+=CB_PrintMiniC_fx_ed( c, d, tmpb, MINI_OR   | (0x100*EditExtFont) | 0x10000000 ) ;
							} else {
								if ( rev )	px+=CB_PrintMiniC_ed( c, d, tmpb, MINI_OVER | (0x100*EditExtFont) | (yk*rev)<<16 | 0x10000000 ) ;
								else		px+=CB_PrintMiniC_ed( c, d, tmpb, MINI_OR   | (0x100*EditExtFont) | (yk*rev)<<16 | 0x10000000  ) ;
							}
							break;
						case 2:	// mini fix
							if ( rev )	px+=CB_PrintMiniC_Fix_ed( c, d, tmpb, MINI_OVER | (0x100*EditExtFont) | (yk*rev)<<16 | 0x10000000  ) ;
							else		px+=CB_PrintMiniC_Fix_ed( c, d, tmpb, MINI_OR   | (0x100*EditExtFont) | (yk*rev)<<16 | 0x10000000  ) ;
							break;
						case 3:	// minimini
							if ( rev )	px+=CB_PrintMiniMiniC_ed( c, d, tmpb, MINI_OVER | (yk*rev)<<16 | 0x10000000  ) ;
							else		px+=CB_PrintMiniMiniC_ed( c, d, tmpb, MINI_OR   | (yk*rev)<<16 | 0x10000000 ) ;
							break;
						case 4:	// minimini fix
							if ( rev )	px+=CB_PrintMiniMiniC_Fix_ed( c, d, tmpb, MINI_OVER | (yk*rev)<<16 | 0x10000000  ) ;
							else		px+=CB_PrintMiniMiniC_Fix_ed( c, d, tmpb, MINI_OR   | (yk*rev)<<16 | 0x10000000  ) ;
							break;
						case 5:	// minimini Bold
							if ( rev )	px+=CB_PrintMiniMiniBoldC_ed( c, d, tmpb, MINI_OVER | (yk*rev)<<16 | 0x10000000  ) ;
							else		px+=CB_PrintMiniMiniBoldC_ed( c, d, tmpb, MINI_OR   | (yk*rev)<<16 | 0x10000000 ) ;
							break;
						case 6: // minimini Bold fix
							if ( rev )	px+=CB_PrintMiniMiniBoldC_Fix_ed( c, d, tmpb, MINI_OVER | (yk*rev)<<16 | 0x10000000  ) ;
							else		px+=CB_PrintMiniMiniBoldC_Fix_ed( c, d, tmpb, MINI_OR   | (yk*rev)<<16 | 0x10000000  ) ;
							break;
						case 7: // minimini fx 6x8
							if ( rev ) 	px+=CB_PrintMiniMiniFXC_ed( c, d, tmpb, MINI_OVER | (0x100*EditExtFont) | (yk*rev)<<16 | 0x10000000 ) ;
							else		px+=CB_PrintMiniMiniFXC_ed( c, d, tmpb, MINI_OR   | (0x100*EditExtFont) | (yk*rev)<<16 | 0x10000000  ) ;
							break;
					}
				}
				if ( ( px > EDITpxMAX ) || ( opcode==0x0C ) || ( opcode==0x0D ) ) {  (y)++; px=1; (*n)++; (*csry)++; }
//				Bdisp_PutDisp_DD();
			} else {
					switch ( mini&7 ) {
						case 1:	// mini
							if ( ( CB_G1MorG3M==1 ) && EditExtFont ) {
								px+=CB_PrintMiniLength_fxed( tmpb, EditExtFont );	// minifont ext font mode
							} else {
								px+=CB_PrintMiniLength( tmpb, EditExtFont );		// minifont ext font mode
							}
							break;
						case 2:	// mini fix
							px+=12;
							break;
						case 3:	// minimini
							px+=CB_PrintMiniMiniLengthStr( tmpb );	// miniminifont
							break;
						case 4:	// minimini fix
							px+=8;
							break;
						case 5:	// minimini Bold
							px+=CB_PrintMiniMiniBoldLengthStr( tmpb );	// miniminifont bold
							break;
						case 6: // minimini Bold fix
							px+=9;
							break;
						case 7: // minimini fx 6x8
							px+=6;
							break;
						case 0:	// normal
							px+=18; if ( (mini==8)&&(IsGB) ) px+=6;
							break;
					}
				if ( ( px > EDITpxMAX ) ) { (y)++; px=1; }
			}
			if ( ( opcode==0x0D ) ) { Numflag=0; (*NumOfset)++; 
				if ( ( (*quotflag2)==0 ) || ( (*quotflag)==1 )  ) (*quotflag)=0;
			}
			c=(char)*tmpb & 0xFF;
			if ( ( IsGB )||(c==0x7F)||(c==0xF7)||(c==0xF9)||(c==0xE5)||(c==0xE6)||(c==0xE7)||(c==0xFF) ) { tmpb++; }
			tmpb++; i++;
		}
		if ( (*quotflag)==3 ) (*quotflag)=0; 
		if ( ( opcode==0x0C ) || ( opcode==0x0D ) ) break ;
		if ( (*csry) > ymax ) break ;
		if ( (*n) > ynum ) break;
	}
	if ( ( y == (*n) )   && (ofst==csrPtr) ) { *pcx=px; *cy=(*csry);   CurrentLineNum=CurrentLine; }
	if ( ( y == (*n)+1 ) && (ofst==csrPtr) ) { *pcx=px; *cy=(*csry)+1; CurrentLineNum=CurrentLine; }
	CB_ColorIndex=colortmp;
	CB_BackColorIndex=backcolortmp;
	CB_G1MorG3M=g1mg3m;
	return ofst;
}

//---------------------------------------------------------------------------------------------
int DumpOpcode( char *SrcBase, int *offset, int *offset_y, int csrPtr, int *pcx, int *cy, int ClipStartPtr, int ClipEndPtr){
	int i,j,n,px,y,ynum;
	int ofst,ofst2,ofstYptr;
	int count=60;
	int ymin,ymax,ymaxpos,yk;
	int NumOfset;
	int mini;
	int quotflag;	// ':1 ":2
	int quotflag2;
	mini=SetEditScreenSize( &ymin, &ymax, &ymaxpos, &yk );
	if ( ( UpdateLineNum==0 ) ) {
		EditLineNum=HomeLineNum(SrcBase, *offset );	// line number re-adjust
	}
  jp:
	*pcx=0; *cy=0;
	while ( 1 ) {
		quotflag=0;
		quotflag2=( (*offset)==0 );
		NumOfset=0;	// line number offset
		CB_BackColorIndex=CB_ColorIndexEditBack;		// Back color index (default White)
		Bdisp_AllClr_VRAM3(0,169);
		y=ymin; ofst=(*offset);
		ofst2=ofst;
		px=1; ynum=0;
		OpcodeLineN( SrcBase, &ofst2 ,&px ,&ynum);
		n=(*offset_y);
		ofst=PrintOpcodeLineN( &y, ynum, ymax, &n, SrcBase, ofst, csrPtr, &(*pcx), &(*cy), ClipStartPtr, ClipEndPtr, &NumOfset, &quotflag, &quotflag2 );
		while ( y <= ymaxpos ) {
				if ( SrcBase[ofst]==0x00 ) break ;
				ofst2=ofst;
				px=1; ynum=0;
				OpcodeLineN( SrcBase, &ofst2 ,&px ,&ynum);
				n=0;
				ofst = PrintOpcodeLineN( &y, ynum, ymax, &n, SrcBase, ofst, csrPtr, &(*pcx), &(*cy), ClipStartPtr, ClipEndPtr, &NumOfset, &quotflag, &quotflag2 );
		}
	  exit:
		if ( ( (*pcx)!=0 ) && ( (*cy)>(ymin-1) ) && ( (*cy)<=ymax ) ) break ;
		ofstYptr=OpcodeLineYptr( *offset_y, SrcBase, *offset, &px);
		if ( csrPtr < ofstYptr ) { PrevLinePhy( SrcBase, &(*offset), &(*offset_y) ); }
		else 					 { NextLinePhy( SrcBase, &(*offset), &(*offset_y) ); } 
//		if ( SrcBase[ofst]==0x00 ) break ;
		count--;
		if ( count<50 ) if ( csrPtr > 0 ) csrPtr--;
		if ( count==0 ) {  // error reset
			(*offset)=0; (*offset_y)=0; (*pcx)=0; (*cy)=(ymin); EditLineNum=1; return -1; 	// line number reset
		}
		UpdateLineNum=1;
	}
	if ( ( UpdateLineNum ) ) {
		EditLineNum=HomeLineNum(SrcBase, *offset );	// line number re-adjust
		UpdateLineNum=0;
		goto jp;
	}
	return 0; // ok
}


//---------------------------------------------------------------------------------------------

void DumpMix( char *SrcBase, int offset){
	int 	i,j;
	for (j=0; j<(6) ; j++){
		Hex4PrintXY( 1, j+(2), "", ((int)offset+j*4)&0xFFFF);
		locate( 5,j+2); Prints((unsigned char*)":           /    ");
		for (i=0; i<4 ; i++){
				Hex2PrintXY( i*3+6, j+(2), "", SrcBase[offset+i+j*4]&0xFF);
		}
		for (i=0; i<4 ; i++){
				CB_PrintC(18+i,j+(2),(unsigned char*)SrcBase+offset+i+j*4);
		}
	}
}
//---------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------
void editmenu(){
	Fkey_Icon( FKeyNo1, 508 );	//	Fkey_dispR( FKeyNo1, "JUMP");
	Fkey_Icon( FKeyNo2, 391 );	//	Fkey_dispR( FKeyNo2, "SRC");
	if ( CommandInputMethod ) {
			Fkey_Icon( FKeyNo3, 396 );	//	Fkey_dispR( FKeyNo3, "MENU"); 
		} else {
			Fkey_Icon( FKeyNo3, 506 );	//	Fkey_dispN( FKeyNo3, "CMD");
		}
	if ( lowercase  ) Fkey_dispN_aA( FKeyNo4, "A <> a"); else Fkey_dispN_Aa( FKeyNo4, "A <> a");
	Fkey_Icon( FKeyNo5, 673 );	//	Fkey_dispR( FKeyNo5, "CHAR");
}

int SearchOpcodeEdit( char *SrcBase, char *searchstr, int *csrptr, int next){
	int csrbkup=(*csrptr);
	int sptr=0,cptr;
	int opcode=1,srccode;
	int size=SrcEndPtr(SrcBase-0x56);

	Bdisp_PutDisp_DD_DrawBusy();
	opcode =GetOpcodeGB( SrcBase, *csrptr ) ;
	srccode=GetOpcodeGB( searchstr, sptr ) ;
	if ( next ) if ( opcode == srccode ) NextOpcodeGB( SrcBase, &(*csrptr) );
	while ( (*csrptr)<size ) {
		sptr=0;
		opcode =GetOpcodeGB( SrcBase, *csrptr ) ;
		if ( opcode == 0 ) break;
		srccode=GetOpcodeGB( searchstr, sptr ) ;
		if ( opcode != srccode ) {
			NextOpcodeGB( SrcBase, &(*csrptr) );
		} else {
			cptr = *csrptr;
			while ( (*csrptr)<size ) {
				NextOpcodeGB( searchstr, &sptr );
				srccode=GetOpcodeGB( searchstr, sptr ) ;
				if ( srccode == 0x00 ) { *csrptr=cptr; return 1; }	// Search Ok
				NextOpcodeGB( SrcBase, &(*csrptr) );
				opcode =GetOpcodeGB( SrcBase, *csrptr ) ;
//				if ( (*csrptr) >= size ) { *csrptr=csrbkup; return 0; }	// No search
				if ( opcode != srccode ) break ;
			}
		}
	}
	{ *csrptr=csrbkup; return 0; }	// No search
}

void srcmenu(){
	editmenu();
	Fkey_Icon( FKeyNo1, 165 );	//	Fkey_dispN( FKeyNo1, "SRC ");
	Fkey_dispN( FKeyNo2, "REPL");
}

int SearchForText( char *SrcBase, char *searchstr, int *csrptr, char *repstr ){	// SRC repstr:NULL 
	int key;
	int i=0,sptr;
	int srcmode=1;	// 1:SRC  2:Replace
	int bk_G1MorG3M=CB_G1MorG3M;
	
	repstr[0]='\0';
  loop:
	Bdisp_AllClr_VRAM1();
	CB_ColorIndex=0x001F;	// Blue
	CB_Prints( 1, 1, (unsigned char*)"Search For Text");
	locate(1,2); Prints((unsigned char*)"---------------------");
	locate(1,4); Prints((unsigned char*)"---------------------");
	Bdisp_PutDisp_DD_DrawBusy();
	KeyRecover(); 
	CB_G1MorG3M=3;
	do {
		srcmenu();
		key= InputStrSubFn( 1, 3, 21, strlenOp(searchstr), searchstr, 63, " ", REV_OFF, FLOAT_ON, EXP_ON, ALPHA_ON, HEX_OFF, PAL_ON, EXIT_CANCEL_OFF, AC_CANCEL_ON );
	} while ( key == KEY_CTRL_AC ) ;	// AC
	CB_G1MorG3M = bk_G1MorG3M;
	if ( key == KEY_CTRL_EXIT ) { srcmode=0; goto exit; }	// exit
	if ( strlenOp(searchstr) == 0 ) goto loop;
	if ( key == KEY_CTRL_F2 ) {
		CB_ColorIndex=0x001F;	// Blue
		CB_Prints( 1, 1, (unsigned char*)"Replacement Text");
		locate(1,6); Prints((unsigned char*)"---------------------");
		Bdisp_PutDisp_DD_DrawBusy();
		KeyRecover(); 
		CB_G1MorG3M=3;
		do {
			srcmenu();
			key= InputStrSubFn( 1, 5, 21, strlenOp(repstr), repstr, 63, " ", REV_OFF, FLOAT_ON, EXP_ON, ALPHA_ON, HEX_OFF, PAL_ON, EXIT_CANCEL_OFF, AC_CANCEL_ON );
		} while ( key == KEY_CTRL_AC ) ;	// AC
		CB_G1MorG3M = bk_G1MorG3M;
		if ( key == KEY_CTRL_EXIT ) { srcmode=0; goto exit; }	// exit
		srcmode=2;	// replace
	}

	i = SearchOpcodeEdit( SrcBase, searchstr, &(*csrptr), 0 );
	CB_G1MorG3M = bk_G1MorG3M;
	if ( i==0 ) {
		EnableDisplayStatusArea();
		ErrorMSGstr1(" Not Found ");
		goto loop;	//  not found loop
	}
  exit:
	Setup_SetEntry(0x14, 0x00 ); alphastatus = 0; alphalock = 0;
	return srcmode;	//

}

//---------------------------------------------------------------------------------------------
void jumpmenu(){
//	Fkey_Icon( FKeyNo1, 393 );	//	Fkey_dispN( FKeyNo1, "TOP ");
//	Fkey_Icon( FKeyNo2, 394 );	//	Fkey_dispN( FKeyNo2, "BTM");
	Fkey_Icon( FKeyNo1, 965 );	//	Fkey_dispN( FKeyNo1, "TOP ");
	Fkey_Icon( FKeyNo2, 967 );	//	Fkey_dispN( FKeyNo2, "BTM");
	Fkey_Icon( FKeyNo3, 964 );	//	Fkey_dispN( FKeyNo3, "GOTO");
	FkeyClear( FKeyNo4 );
	Fkey_dispN_aA( FKeyNo5, "Skip\xE6\x92");
	Fkey_dispN_aA( FKeyNo6, "Skip\xE6\x93");
}

int JumpGoto( char * SrcBase, int *offset, int *offset_y, int cy) {
	int key;
	char buffer[32];
	int lineAll,curline;
	int ofst,ofsty;
	int i,n;
	PopUpWin(3);
	locate( 3,3); Prints((unsigned char *)"Goto Line Number");
	Bdisp_PutDisp_DD_DrawBusy();

	sprintf3(buffer,"current:%d",CurrentLineNum);
	locate( 3,4); Prints((unsigned char *)buffer);
	Bdisp_PutDisp_DD_DrawBusy();
	
	lineAll=AllLineNum( SrcBase );
	
	sprintf3(buffer,"[1~%d]:",lineAll);
	locate( 3,5); Prints((unsigned char *)buffer);
	i=strlen(buffer)+3;
	n=0;
	while (1) {
		n=InputNumD( i, 5, 5, n, " ", REV_OFF, FLOAT_OFF, EXP_OFF, &key);		// 0123456789
 		if ( (1<=n)&&(n<=lineAll) ) break;
 		if ( n==0 ) break;
 		if ( n > lineAll ) n=lineAll;
 	}
	return n ; // ok
}


int CheckIndentCommand( char *SrcBase, int ptr ){
	int c=SrcBase[ptr++];
	int indent=0;
	if ( c==0xFFFFFFF7 ) {
		c=SrcBase[ptr++];
		switch (c) {
			case 0x00:	// If
			case 0x02:	// Else
			case 0x04:	// For
			case 0x08:	// While
			case 0x0A:	// Do
			case 0x0F:	// ElseIf
			case 0x0FFFFFFEA:	// Switch
//			case 0x0FFFFFFEB:	// Case
//			case 0x0FFFFFFEC:	// Default
				indent++;
				break;
		}
	}
	return indent*(CB_EditIndent&0x07);
}

void RestoreScreenModeEdit(){
	if ( ScreenModeEdit )	CB_RestoreGraphVRAM();	// Resotre Graphic screen
	else					CB_RestoreTextVRAM();	// Resotre Text screen
}


int EditRun(int run){		// run:1 exec      run:2 edit
	char *filebase,*SrcBase;
	int key=0;
	char keyH,keyL;
	char cont=1,stat;
	char buffer[32];
	char buffer2[32];
	char buffer3[32];
	char searchbuf[64]="";
	char replacebuf[64]="";
	unsigned char c;
	int i,j,n,d,x,y,pcx,cx,cy,ptr,ptr2,tmpkey=0;
	int 	offset=0;
	int 	offset_y=0;
	int 	ofstYptr;
	int 	csrPtr=0;
	int 	csrPtr_y=0;
	char 	dumpflg=2;
	int ClipStartPtr = -1 ;
	int ClipEndPtr   = -1 ;
	char alphastatus_bk ;
	char alphalock_bk ;
	char SearchMode=0;
	int ContinuousSelect=0;
	char DebugMenuSw=0;
	char JumpMenuSw=0;
	int ymin,ymax,ymaxpos,mini,yk ;
	int ShiftAlphaStatus=0;
	int InsertMode;
 	unsigned short color=0x0000;
 	unsigned short *vram;
 	unsigned short linebuf[384];
	int alllinenum=AllLineNum( SrcBase );
	int Is1st=1;
	int H,S,V,L;
	int help_code=0;
	int indent;
	int execptr;
	int selectStr=0;
	char *string;

	cUndo Undo;
 
	long FirstCount;		// pointer to repeat time of first repeat
	long NextCount; 		// pointer to repeat time of second repeat

 	filebase = ProgfileAdrs[ProgNo];
	SrcBase  = filebase+0x56;
	i = EndOfSrc( SrcBase, 0 );
	if ( i < ExecPtr ) ExecPtr = i;
	offset = ExecPtr;
	csrPtr = offset;
	EditLineNum=0;
	UpdateLineNum=1;
	
	Undo.enable = 0;	// clear Undo

	Bkey_Get_RepeatTime(&FirstCount,&NextCount);	// repeat time
	Bkey_Set_RepeatTime(KeyRepeatFirstCount,KeyRepeatNextCount);		// set cursor rep

	mini=SetEditScreenSize( &ymin, &ymax, &ymaxpos, &yk );
	PrevLinePhyN( ymax, SrcBase, &offset, &offset_y );	// csrY adjust
	
	if ( run == 1 ) { key=KEY_CTRL_F6; }	// direct run

	if ( DebugMode ) DebugMenuSw=1; 
	
//	if (Cursor_GetFlashStyle()<0x6)	Cursor_SetFlashOn(0x0);		// insert mode cursor 
//		else 						Cursor_SetFlashOn(0x6);		// overwrite mode cursor 
	reentrant_SetRGBColor = 0;	//	reentrant ok
		
	KeyRecover();
	Setup_SetEntry(0x14, 0x00 ); alphastatus = 0; alphalock = 0;

	while ( cont ) {
		if ( ( alphastatus ) || ( alphalock ) ) SetAlphaStatus( alphalock, lowercase );
		if ( DebugMode==0 ) DebugMenuSw=0; 
		MiniCursorClipMode = 0;
		GBcode = EditGBFont;
//		EnableColor( 1 );	// full color mode 
		CB_BackPict=0;				// back image
		CB_ColorIndex=-1;								// current color index reset
		CB_BackColorIndex=0xFFFF;						// Back color index (default White)
		mini=SetEditScreenSize( &ymin, &ymax, &ymaxpos, &yk );

		ErrorNo=0;
		filebase = ProgfileAdrs[ProgNo];
		SrcBase  = filebase+0x56;

		if ( ( ClipStartPtr >=0 ) || ( SearchMode ) ) { CommandType=0; CommandPage=0; }
		
		if ( ( run != 1 ) ) { // exec mode is Invalid
			if ( filebase[0x55]==2 ) goto editpass;	// 
			if ( filebase[0x55]==0 ) {	// pass no match (C.basic)
				if ( CheckPassWord( filebase ) ) {
					filebase[0x55]=2;	// 
					editpass:
					key=KEY_CTRL_EXIT;	// password error
					goto edit_exit;
				}
			}
		}
		
		if ( ( DebugScreen == 0 ) && ( run != 1 ) ) {

			if ( UpdateLineNum ) alllinenum=AllLineNum( SrcBase );
   			
			if (dumpflg!=2) Bdisp_AllClr_VRAM3(192-24,191);
//			Bdisp_AllClr_VRAM2();
//			ML_rectangle_FX( 0, 0, 125, 55, 0, 0, 1, CB_ColorIndexEditBack,CB_ColorIndexEditBack);

			if ( dumpflg == 2 ) CB_BackColorIndex=CB_ColorIndexEditBack;	// back color
			MiniCursorflag=0;		// cursor initialize
//			if ( dumpflg == 2 ) if ( Is1st==0 ) MiniCursorFlashing(); else Is1st=0;
			strncpy(buffer2,(const char*)ProgfileAdrs[ProgNo]+0x3C,8);
			buffer2[8]='\0';
			if (dumpflg==2) {
				if ( DebugMode >=1 ) { i=CB_INT;        j=MatBase; }
					else             { i=CB_INTDefault; j=MatBaseDefault; }
				sprintf3(buffer, "==%-8s==%s%d%s", buffer2, i ? " [INT%" : " [DBL#", j, "]");
			} else {
				Bdisp_AllClr_VRAM2();
				sprintf3(buffer, "==%-8s==%08X", buffer2, SrcBase);
				locate (1,1); Prints(    (unsigned char*)buffer );
				if ( DebugMode >=1 ) // debug mode
					Bdisp_AreaReverseVRAMx3(0, 0, 125,7);	// reverse top line 
			}
			
			switch (dumpflg) {
				case 2: 		// Opcode listing
						if ( SearchMode ) {
							ofstYptr=OpcodeLineYptr( offset_y, SrcBase, offset, &x);
							if ( csrPtr > ofstYptr ) {
								offset=csrPtr; offset_y=0;
								PrevLineGB( SrcBase, &offset);
								PrevLinePhyN( ymax, SrcBase, &offset, &offset_y );
							}
						}
						if ( DumpOpcode( SrcBase, &offset, &offset_y, csrPtr, &pcx, &cy, ClipStartPtr, ClipEndPtr) ) csrPtr=0; //return KEY_CTRL_EXIT;
						MiniCursorflag=0;		// cursor initialize
						if ( Is1st==0 ) MiniCursorFlashing(); else Is1st=0;
						CB_ScrollBar( alllinenum, EditLineNum-1, ymax, 384-6, 0, 168, 6 ) ;

						CB_Help( help_code, cy>(ymax/2+1) );	// help display
						KeyRecover();
						help_code = 0;

						break;
				case 4: 		// hex dump
						i = csrPtr-offset;
						if ( ( i <  0 ) || ( i > 23 ) ) {
							offset=csrPtr; cx=6; cy=(2);
						} else {
							if ( (cx!=7) && (cx!=10) && (cx!=13) && (cx!=16) ) { 
								cx = (i&3)*3+6;
								cy = (i/4)+(2);
							}
						}
						DumpMix( SrcBase, offset );
						break;
				default:
						break;
			}
//			StatusArea_Edit(buffer, ProgfileMax[ProgNo]-SrcSize(filebase) );
			if ( DebugScreen == 0 )	StatusArea_Run();
//			strncpy(buffer3,(const char*)ProgfileAdrs[ProgNo]+0x3C,8);
//			buffer3[8]='\0';
//			sprintf3(buffer2, "%-8s (%d)", buffer3, CurrentLineNum);
//			StatusArea_Run_sub( buffer2, CB_INT, CB_G1MorG3M );

			EnableDisplayStatusArea();
			sprintf3(buffer3, "%4d/%d", CurrentLineNum, alllinenum);
			CB_ColorIndex=0xFFFE;	// 
			CB_PrintMini_Fix10( 385-10*8, -20, (unsigned char *)buffer3, MINI_OR); 	// display current line number 
			CB_ColorIndex=CB_ColorIndexEditLine;	// 
			CB_PrintMini_Fix10( 384-10*8, -21, (unsigned char *)buffer3, MINI_OR); 	// display current line number 
		}
		vram=(unsigned short *)PictAry[0]+384*192; for ( i=0; i<384; i++ ) linebuf[i]=vram[i];
		
		if ( DebugScreen ) {	// screen debug mode
				Cursor_SetFlashOff(); 		// cursor flashing off
				RestoreScreenModeEdit();
		}
		
		if ( ( DebugScreen != 1 ) && ( run != 1 ) ) {
			if ( SearchMode ) {
					Fkey_Icon( FKeyNo1, 165 );	//	Fkey_dispR( FKeyNo1, "SRC ");
					FkeyClear( FKeyNo3 );
					FkeyClear( FKeyNo5 );
					Fkey_Icon( FKeyNo6,  276 );	//	Fkey_dispN( FKeyNo6, "RETRY");
					switch ( SearchMode ) {
						case 1:
							FkeyClear( FKeyNo2 );
							FkeyClear( FKeyNo4 );
							break;
						case 2:
							Fkey_dispN( FKeyNo2, "REPL");
							Fkey_dispN( FKeyNo4, "ALL");
							Fkey_dispN( FKeyNo5, "ALL+");
							break;
						case 3:
						case 4:
							FkeyClearAll();
							locate (11,8); Prints((unsigned char*)"[AC]:Stop" );	//  replace all mode
							break;
					}
			} else {
				if ( ClipStartPtr>=0 ) {
					MiniCursorClipMode = 1;
					Fkey_Icon( FKeyNo1,  52 );	//	Fkey_dispN( FKeyNo1, "COPY ");
					Fkey_Icon( FKeyNo2, 105 );	//	Fkey_dispN( FKeyNo2, "CUT ");
					Fkey_Icon( FKeyNo3,   9 );	//	Fkey_dispN( FKeyNo2, "DEL ");
					Fkey_Icon( FKeyNo4, 165 );	//	Fkey_dispN( FKeyNo1, "SRC ");
					Fkey_dispN( FKeyNo5, "add '");
					Fkey_dispN( FKeyNo6, "del '");
				} else {
					ClipEndPtr   = -1 ;		// ClipMode cancel
					if ( DebugMode >=1 ) {  // debug mode
						switch ( DebugMenuSw ) {
							case 1:
								Fkey_Icon( FKeyNo1, 386 );	//	Fkey_dispN( FKeyNo1, "CONT");
								Fkey_Icon( FKeyNo2, 558 );	//	Fkey_dispN( FKeyNo2, "TRCE");
								Fkey_Icon( FKeyNo3,1232 );	//	Fkey_dispN( FKeyNo3, "STEPS");
								Fkey_dispN( FKeyNo4, "S.Out");
								Fkey_dispN( FKeyNo5, "L<>S");
								Fkey_Icon(FKeyNo6, 6 ); //Fkey_DISPN( FKeyNo6," \xE6\x9E ");
								break;
							case 0:
								if ( JumpMenuSw ) {		// Jump Mode
									jumpmenu();
								} else {
									editmenu();
									Fkey_Icon(FKeyNo6, 6 ); //Fkey_DISPN( FKeyNo6," \xE6\x9E ");
								}
								break;
						}
					} else {	// normal mode
						if ( JumpMenuSw ) {		// Jump Mode
							jumpmenu();
						} else {
							editmenu();
							Fkey_Icon( FKeyNo6, 388 );	//	Fkey_dispN( FKeyNo6, "EXE");
						}
					}
				}
			}
		}
		
		if ( CommandInputMethod ) { DispGenuineCmdMenu();
			if ( (CommandType==CMD_SHIFT_VWIN ) && (CommandPage==0) ) {
				Fkey_dispN( FKeyNo4, "VWIN");
				i=ProgfileMax[ProgNo]-SrcSize(filebase);
				if ( i>9999 ) 	sprintf3(buffer, "%dk",  i/1024 );
				else 			sprintf3(buffer, "%d",  i );
				Fkey_dispN( FKeyNo5, buffer);
				Fkey_Icon( FKeyNo6, 563 );	//	Fkey_dispN( FKeyNo6, "G<>T");
			}
		}

		if ( DebugScreen==0 ) { vram=(unsigned short  *)PictAry[0]+384*192; for ( i=0; i<384; i++ ) if ( linebuf[i] != CB_ColorIndexEditBack ) vram[i]=linebuf[i]; }

		if ( run != 1 ) {
			if ( SearchMode == 3 ) {	//  replace all mode
				if ( KeyScanDownAC() == 0 ) {
					PutKey( KEY_CTRL_F2, 1 );
				} else { 
					while ( KeyScanDownAC() );
					KeyRecover();
					PutKey( KEY_CTRL_NOP, 1 );
					SearchMode = 2;
				}
			}

			if ( dumpflg == 2 ) {
					locate((pcx-1+EDITpxNum)/18+1,cy); 
			} else	locate(cx,cy);
			
			if  ( (dumpflg==2) || (dumpflg==4) ) {
				if (dumpflg==4) Cursor_SetFlashMode(1);			// cursor flashing on
//				if (Cursor_GetFlashStyle()<0x6) {
//					if ( ClipStartPtr>=0 ) 	Cursor_SetFlashOn(0x05);	// ClipMode cursor
//				} else { 
//					if ( ClipStartPtr>=0 )	Cursor_SetFlashOn(0x0B);	// ClipMode cursor
//				}
				if ( (dumpflg==2) ) {
					Cursor_SetFlashOff(); 			// cursor flashing off
					MiniCursorX=pcx-1+EDITpxNum;
					MiniCursorY=(cy-1)*yk;
					MiniCursorSetFlashMode( 1 );		// mini cursor flashing on
					if ( DebugScreen ) MiniCursorSetFlashMode( 0 );		// mini cursor flashing off
				} else {
					if ( DebugScreen ) Cursor_SetFlashOff();			// cursor flashing off
				}

				InsertMode = Setup_GetEntry(0x15) == 02 ;	// 01:OverWrite   02:Insert
				if ( ClipStartPtr >= 0 ) Setup_SetEntry(0x14, 02 );	// Clip mode
				if (key < 0x7F00) {
					if ( ContinuousSelect & 0xFF ) {
						MiniCursorSetFlashMode( 0 );		// mini cursor flashing off
						goto F5Continue;
					} else { GetKey_DisableMenuCatalog(&key); }
				}
			} else { GetKey_DisableMenuCatalog(&key); }
		}						
		
		if ( lowercase  && ( 'A' <= key  ) && ( key <= 'Z' ) ) key+=('a'-'A');
//		if ( key==0 ) if ( KeyCheckEXIT() ) key=KEY_CTRL_EXIT;
		CB_ColorIndex=-1;					// current color index reset
		CB_BackColorIndex=0xFFFF;			// Back color index (default White)
		MiniCursorSetFlashMode( 0 );		// mini cursor flashing off
		Cursor_SetFlashOff(); 			// cursor flashing off
		switch (dumpflg) {
			case 4: 		// hex edit
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
								cx++;
						} else {				// lower nibbe
								ptr = (offset)+((cy-2)*4+(cx-6)/3) ;
								d = SrcBase[ptr];
								d = (d&0xF0) + n;
								SrcBase[ptr] = d;
								key=KEY_CTRL_RIGHT;
						}
						ProgfileEdit[ProgNo]=1;	// edit program
						DebugScreen = 0;
				}
				break;
//			case 2: 		// Opcode
//				break;
			default:
				break;
		}
		
		UpdateLineNum=0;
		
		if ( alphastatus == 1 ) {
			if ( KeyCheckPMINUS() ) {
//			if ( CheckKeyRow7305(1) & 0x08 ) {
				key = '%';
			}
		}
		switch (key) {
			case KEY_CTRL_NOP:
//					if ( SearchMode ) break;;
					ClipStartPtr = -1 ;		// ClipMode cancel+
					alphalock = 0 ;
					break;
			case KEY_CTRL_ALPHA:
			  alphaj:
					if ( alphastatus ) { 
						alphastatus = 0;
						alphalock = 0 ; 
						Setup_SetEntry(0x14, 0x00); 	// clear
					} else { 
						SetAlphaStatus( alphalock, lowercase ); 
						alphastatus = 1; 
					}
					ClipStartPtr = -1 ;		// ClipMode cancel+
					key=0;
					break;
			case KEY_CTRL_EXIT:
//					if ( KeyCheckEXIT() == 0 ) break;
					WaitKeyEXIT();
				  exitjp:
					if ( SearchMode ) {
						SearchMode=0;		//	Search mode cancel
					} else {
						if ( CommandType ) {
							CommandType>>=4;
							CommandPage=CommandType & 0xF;
							CommandType>>=4;
						} else { 
							if ( JumpMenuSw  ) { JumpMenuSw  = 0; break; }
							if ( DebugScreen ) { DebugScreen = 0; break; }
							if ( ClipEndPtr < 0 ) {
								if ( DebugMode >=1 ) {	// debug mode
									if ( (ExitDebugModeCheck&2) ) {
										if ( YesNo( "Exit Debug mode ?" )==0 ) break;
									}
//									ExitDebugModeCheck&=1; 
									BreakPtr=-7;	// return to main program
									if ( ProgEntryN == 0 ) ExecPtr=csrPtr; 
								} else ExecPtr=csrPtr;
								cont=0;
							}
						}
					}
					ClipStartPtr = -1 ;		// ClipMode cancel
					break;
			case KEY_CTRL_F1:
			case 0x7553:		// ClipMode F1
					if ( SearchMode ) {		//	Next Search
						i=SearchOpcodeEdit( SrcBase, searchbuf, &csrPtr, 1 );
						if ( i==0 ) SearchMode=0;
					} else {
						if ( ClipStartPtr >= 0 ) {
							if ( ClipEndPtr < 0 ) goto F1j;
							if ( ClipEndPtr < ClipStartPtr ) { i=ClipStartPtr; ClipStartPtr=ClipEndPtr; ClipEndPtr=i; }
							EditCopy( filebase, ClipBuffer, csrPtr, ClipStartPtr, ClipEndPtr );
							UpdateLineNum=1;
						} else {
							if ( CommandType ) { GetGenuineCmdF1( &key ); goto F1j; }
							else {
								if ( DebugMenuSw ) {		// ====== Debug Mode ======
									cont=0;
									ExecPtr=csrPtr;
									BreakPtr=0;
									DebugMode=1;		// cont mode
									WaitKeyF1() ;
								} else {
									if ( JumpMenuSw ) {		// ====== Jump Mode
										csrPtr=0;
										offset=0; offset_y=0; EditLineNum=1;	// line number reset
										switch (dumpflg) {
											case 2: 		// Opcode
												break;
											case 4: 		// hex dump
												cx=6; cy=2;
												break;
											default:
												break;
										}
									} else {
										JumpMenuSw = 1;
									}
								}
							}
						}
					}
					key=0;
			F1j:	alphalock = 0 ;
					ClipStartPtr = -1 ;		// ClipMode cancel
					break;
			case KEY_CTRL_F2:
			case 0x7563:		// ClipMode F2
					if ( SearchMode >= 2 ) {	// replace one
							i = strlenOp(searchbuf);
							csrPtr+=i;
							EditCutDel( filebase, searchbuf, &csrPtr, csrPtr-i, csrPtr, 1, &Undo );	// delete
							Undo.enable = 0;
							EditPaste( filebase, replacebuf, &csrPtr, &Undo );	// insert
							UpdateLineNum=1;
							i = SearchOpcodeEdit( SrcBase, searchbuf, &csrPtr, 0 );
							if ( i==0 ) SearchMode=0; 
							KeyRecover(); 
					} else {
						if ( SearchMode ) break;;
						if ( ClipStartPtr >= 0 ) {
							if ( ClipEndPtr < 0 ) goto F2j;
							if ( ClipEndPtr < ClipStartPtr ) { i=ClipStartPtr; ClipStartPtr=ClipEndPtr; ClipEndPtr=i; }
							EditCutDel( filebase, ClipBuffer, &csrPtr, ClipStartPtr, ClipEndPtr, 0, &Undo );	// cut
							UpdateLineNum=1;
						} else {
							if ( CommandType ) { GetGenuineCmdF2( &key ); goto F2j; }
							else {
								if ( DebugMenuSw ) {		// ====== Debug Mode ======
										cont=0;
										ExecPtr=csrPtr;
										BreakPtr=-1;
										DebugMode=2;		// trace into mode
								} else {
									if ( JumpMenuSw ) {		// ====== Jump Mode
										offset=EndOfSrc( SrcBase, offset );
										csrPtr=offset;
										offset_y=0;
										switch (dumpflg) {
											case 2: 		// Opcode
												PrevLinePhyN( ymax, SrcBase, &offset, &offset_y ) ;
												UpdateLineNum=1;
												break;
											case 4: 		// hex dump
												cx=6; cy=2;
												break;
											default:
												break;
										}
									} else {	// Search for text
									 searchStart:
//										searchbuf[0]='\0';
										SearchMode = SearchForText(  SrcBase, searchbuf, &csrPtr, replacebuf ) ;
									}
								}
							}
						}
					}
					key=0;
			F2j:	alphalock = 0 ;
					ClipStartPtr = -1 ;		// ClipMode cancel
					break;
			case KEY_CTRL_F3:
					if ( SearchMode == 2 ) {	// replace all
						break;
					} else {
						if ( SearchMode ) break;;
						if ( ClipStartPtr >= 0 ) {
						  F3del:					// clip delete
							if ( ClipEndPtr < 0 ) goto F3j;
							if ( ClipEndPtr < ClipStartPtr ) { i=ClipStartPtr; ClipStartPtr=ClipEndPtr; ClipEndPtr=i; }
							EditCutDel( filebase, ClipBuffer, &csrPtr, ClipStartPtr, ClipEndPtr, 1, &Undo );	// delete
							UpdateLineNum=1;
						} else {
							if ( CommandType ) GetGenuineCmdF3( &key );
							else {
								if ( DebugMenuSw ) {		// ====== Debug Mode ======
											cont=0;
											ExecPtr=csrPtr;
											BreakPtr=-1;
											DebugMode=3;		// step over mode
									key=0;
								} else {
									if ( JumpMenuSw ) {		// ====== Jump Mode
										i=JumpGoto( SrcBase, &offset, &offset_y, cy );					// Goto
										if ( i>0 ) {
											EditLineNum=i;
											offset=JumpLineNum( SrcBase, EditLineNum);
											offset_y=0;
											csrPtr=offset;
										}
									} else {	// command select
										if ( CommandInputMethod ) {
											CommandType=CMD_MENU; CommandPage=0;
										} else {
											key=SelectOpcode5800P( 0 );
											if ( ( alphastatus ) || ( alphalock ) ) SetAlphaStatus( alphalock, lowercase );
											Is1st=1;	// delay cursor flashing 
										}
									}
								}
							}
						}
					}
				  F3j:
					ClipStartPtr = -1 ;		// ClipMode cancel
					break;
			case KEY_CTRL_F4:
					if ( SearchMode == 2 ) {
						FkeyClearAll();
						if ( YesNo( "Replace All Ok?" ) == 0 ) break;
						SearchMode = 3; //	Replace All
						key=0;
					} else {
						if ( SearchMode ) break;;
						if ( ClipStartPtr >= 0 ) {	// Clip -> Search for text
							if ( ClipEndPtr < 0 ) goto F4j;
							if ( ClipEndPtr < ClipStartPtr ) { i=ClipStartPtr; ClipStartPtr=ClipEndPtr; ClipEndPtr=i; }
							EditCopy( filebase, ClipBuffer, csrPtr, ClipStartPtr, ClipEndPtr );
							ClipBuffer[64]='\0';
							StrMid( searchbuf, ClipBuffer, 1, 64 ); 
							SearchMode = SearchForText(  SrcBase, searchbuf, &csrPtr, replacebuf ) ;
						} else {
							if ( CommandType ) {
								if ( (CommandType==CMD_SHIFT_VWIN ) && (CommandPage==0) ) {
									SetViewWindow();
									SaveFavorites();
								} else GetGenuineCmdF4( &key );
							} else {
								if ( DebugMenuSw ) {		// ====== Debug Mode ======
											cont=0;
											ExecPtr=csrPtr;
											BreakPtr=0;
											DebugMode=4;		// step out mode
								} else {
									if ( JumpMenuSw ) {		// ====== Jump Mode
									} else {
										lowercase=1-lowercase;
										if ( alphastatus ) SetAlphaStatus( alphalock, lowercase );
									}
								}
								key=0;
							}
						}
					}
				  F4j:
					ClipStartPtr = -1 ;		// ClipMode cancel
					break;
			case KEY_CTRL_F5:
					if ( SearchMode == 2 ) {
						FkeyClearAll();
						if ( YesNo2( "Not Display mode", "Replace All Ok?") == 0 ) break;
						locate (11,8); Prints((unsigned char*)"[AC]:Stop" );	//  replace all mode not display
						Bdisp_PutDisp_DD_stripe( 192, 192+23);
						while ( KeyScanDownAC() == 0 ) {
							i = strlenOp(searchbuf);
							csrPtr+=i;
							EditCutDel( filebase, searchbuf, &csrPtr, csrPtr-i, csrPtr, 1, &Undo );	// delete
							Undo.enable = 0;
							EditPaste( filebase, replacebuf, &csrPtr, &Undo );	// insert
							i = SearchOpcodeEdit( SrcBase, searchbuf, &csrPtr, 0 );
							if ( i==0 ) { SearchMode=0; break; }
						}
						while ( KeyScanDownAC() );
						KeyRecover(); 
						offset   = csrPtr;
						offset_y = 0;
						PrevLinePhyN( ymax, SrcBase, &offset, &offset_y );	// csrY adjust
						UpdateLineNum=1;
						key=0;
					} else {
						if ( SearchMode ) break;;
						if ( ClipStartPtr >= 0 ) {	// Clip -> add '
							if ( ClipEndPtr < ClipStartPtr ) { i=ClipStartPtr; ClipStartPtr=ClipEndPtr; ClipEndPtr=i; }
							csrPtr = AddComment( filebase, csrPtr, ClipStartPtr, ClipEndPtr );
							UpdateLineNum=1;
						} else {
							if ( CommandType ) {
								if ( (CommandType==CMD_SHIFT_VWIN ) && (CommandPage==0) ) goto listdump;
								else { 
									GetGenuineCmdF5( &key );
									if ( key == KEY_CTRL_F5 ) { selectSetup=SetupG(selectSetup, 1); key=0; CommandType=0; }
								}
							} else {
								if ( DebugMenuSw ) {		// ====== Debug Mode ======
									if ( DebugScreen )  DebugScreen=0; else DebugScreen=1;	// swap list <---> screen
									key=0;
								} else {
									if ( JumpMenuSw ) {		// ====== Jump Mode
											PrevLinePhyN( PageUpDownNum*ymax, SrcBase, &offset, &offset_y );	// Skip Up
											csrPtr=OpcodeLinePtr( offset_y, SrcBase, offset);
									} else {
									  F5Continue:
										key=SelectChar( &ContinuousSelect);
										if ( ( alphastatus ) || ( alphalock ) ) SetAlphaStatus( alphalock, lowercase );
										Is1st=1;	// delay cursor flashing 
									}
								}
							}
						}
					}
					ClipStartPtr = -1 ;		// ClipMode cancel
					break;
			case KEY_CTRL_F6:
					execptr=csrPtr;
					if ( SearchMode ) goto searchStart;	// Retry search
					if ( ClipStartPtr >= 0 ) {	// Clip -> del '
						if ( ClipEndPtr < ClipStartPtr ) { i=ClipStartPtr; ClipStartPtr=ClipEndPtr; ClipEndPtr=i; }
						csrPtr = DelComment( filebase, csrPtr, ClipStartPtr, ClipEndPtr );
//						offset=csrPtr;
//						offset_y=0;
//						PrevLinePhyN( cy-1, SrcBase, &offset, &offset_y );	// csrY adjust
						UpdateLineNum=1;
					} else {
						Cursor_SetFlashOff(); 			// cursor flashing off
						MiniCursorSetFlashMode( 0 );		// mini cursor flashing off
						if ( ErrorNo ) {
								offset = ErrorPtr;
								csrPtr = offset;
								offset_y=0;
								run=2; // edit mode
								if ( dumpflg == 2 ) {
									PrevLinePhyN( ymax, SrcBase, &offset, &offset_y );
								}
						} else {
							if ( CommandType ) {
								if ( (CommandType==CMD_SHIFT_VWIN ) && (CommandPage==0) ) {
									ScreenModeEdit=1-ScreenModeEdit;
									RestoreScreenModeEdit();
									GetKey_DisableCatalog(&key);
								} else { GetGenuineCmdF6( &key ); goto F6j; }
							} else {
								if ( JumpMenuSw ) {		// ====== Jump Mode
									NextLinePhyN( PageUpDownNum*ymax, SrcBase, &offset, &offset_y );	// Skip Down
									csrPtr=OpcodeLinePtr( offset_y, SrcBase, offset);
									if ( SrcBase[offset] == 0 ) PrevLinePhyN( ymax, SrcBase, &offset, &offset_y );
								} else {
									if ( ( 1 <= DebugMode ) && ( DebugMode <=3 ) ) {		// ====== Debug Mode ======
										if ( DebugScreen == 1 ) { DebugMenuSw = 1; DebugScreen = 2; }
										else
										DebugMenuSw = 1-DebugMenuSw;
									} else {
										if ( DebugMode == 9 ) { DebugMode=2; BreakPtr=-1; } else BreakPtr=0;
										ProgEntryN=0;
//										MSG1("Prog Loading.....");
										CB_AliasVarClr();
										StoreRoot2();
										CB_GetAliasLocalProg( SrcBase ) ;		// sub program search
										if ( ErrorNo ) { 
											ProgNo=ErrorProg; 
											stat=1;
										} else {
											ProgNo=0;
											ExecPtr=0;
											stat=CB_interpreter( SrcBase ) ;	// ====== run 1st interpreter ======
											if ( ( stat==-7 ) && ( ProgEntryN == 0 ) ) DebugMode=0;
										}
//										CB_ColorIndex=-1;								// current color index reset
//										CB_BackColorIndex=CB_ColorIndexEditBack;		// Back color index (default White)
										RestoreRoot2();
										SaveConfig();
										filebase = ProgfileAdrs[ProgNo];
										SrcBase  = filebase+0x56;
										if ( ForceReturn ) { goto finish; }	// force program end
										if ( stat ) {
											if ( ErrorNo ) offset = ErrorPtr ;			// error
											else if ( BreakPtr ) offset = ExecPtr ;	// break
										} else offset = 0;
										if ( stat == -1 ) offset = ExecPtr-1;	// program  no error return
										csrPtr = offset;
										offset_y=0;
										run=2; // edit mode
										if ( dumpflg == 2 ) {
											PrevLinePhyN( ymax, SrcBase, &offset, &offset_y );	// csrY adjust
											if ( stat == -1 ) {
											  finish:
												cont=0;	// program finish
												ExecPtr=execptr; 
											}
										}
										Is1st=1;	// delay cursor flashing 
										UpdateLineNum=1;
									}
								}
							}
						}
					}
					key=0;
			F6j:	alphalock = 0 ;
					ClipStartPtr = -1 ;		// ClipMode cancel
					break;
					
			case 0x755A:		// ClipMode LEFT
			case KEY_CTRL_LEFT:
				key=0;
				switch ( dumpflg ) {
					case 2: 		// Opcode
							PrevOpcodeGB( SrcBase, &csrPtr );
							if ( (pcx==1)&&(cy==ymin) ) PrevLinePhy( SrcBase, &offset, &offset_y );
							break;
					case 4: 		// hex dump
							if ( (cx!=7) && (cx!=10) && (cx!=13) && (cx!=16) ) { cx-=2; csrPtr--; }
							cx--;
							if ( cx<6 ) {
									cy--;
									if ( cy<(2) ) { (offset)-=4; }
							}
							break;
					default:
							break;
				}
				if ( ClipStartPtr>=0 ) ClipEndPtr=csrPtr;
				SearchMode=0;
				if ( ( alphastatus ) && ( alphalock==0 ) )  goto alphaj;
				break;
				
			case 0x755B:		// ClipMode RIGHT
			case KEY_CTRL_RIGHT:
				key=0;
				switch ( dumpflg ) {
					case 2: 		// Opcode
							if ( SrcBase[csrPtr] != 0x00 ) NextOpcodeGB( SrcBase, &csrPtr );
							break;
					case 4: 		// hex dump
							csrPtr++;
							if ( (cx==6) || (cx== 9) || (cx==12) || (cx==15) )  cx+=3; else cx+=2;
							if ( cx>16 ) {
									cy++;
									if ( cy>7 ) { (offset)+=4; }
							}
							break;
					default:
							break;
				}
				if ( ClipStartPtr>=0 ) ClipEndPtr=csrPtr;
				SearchMode=0;
				if ( ( alphastatus ) && ( alphalock==0 ) )  goto alphaj;
				break;
				
			case 0x7559:		// ClipMode UP
			case KEY_CTRL_UP:
				key=0;
				switch ( dumpflg ) {
					case 2: 		// Opcode
//							if ( SrcBase[csrPtr]==0x00 ) PrevOpcodeGB( SrcBase, &csrPtr );
							if ( csrPtr==0 ) break; 
							ptr=csrPtr;
							PrevOpcodeGB( SrcBase, &ptr );
							d = GetOpcodeGB( SrcBase, ptr );
							ptr=csrPtr;
							if ( ( d!=0x0C ) && ( d!=0x0D ) ) PrevLineGB( SrcBase, &ptr );	// current line top
							y=0; ptr2=ptr;
							OpcodeLineN( SrcBase, &ptr2 , &x, &y);
							csrPtr_y = OpcodeLineSrcYpos( SrcBase, ptr, csrPtr ) ;	//
							if ( ( csrPtr_y == 0 ) ) {
								PrevLineGB( SrcBase, &ptr );	// prev line top
								y=0;
								OpcodeLineN( SrcBase, &ptr , &x, &y);
								i=x; x=pcx;
							} else {
								x = OpcodeLineSrcXendpos( csrPtr_y-1, SrcBase, ptr );
								i=x; x=pcx;
							}
							while ( 1 ) {
								PrevOpcodeGB( SrcBase, &csrPtr );
								x -= OpcodeStrLenBufpx( SrcBase, csrPtr);
								if ( x < 1 ) {
									 x=i; break ; }
							}
							while ( x > pcx ) {
								PrevOpcodeGB( SrcBase, &csrPtr );
								x -= OpcodeStrLenBufpx( SrcBase, csrPtr);
								if ( x < 1 ) {
									 NextOpcodeGB( SrcBase, &csrPtr ); break ; }
							}
							if ( cy==ymin ) { PrevLinePhy( SrcBase, &offset, &offset_y ); }

							break;
					case 4: 		// hex dump
							cy--;
							csrPtr-=4;
							if ( cy<(2) ) { (offset)-=4; cy=(2); }
//							if ( (cx==7) || (cx==10) || (cx==13) ) cx--;
							break;
					default:
							break;
				}
				if ( ClipStartPtr>=0 ) ClipEndPtr=csrPtr;
				SearchMode=0;
				if ( ( alphastatus ) && ( alphalock==0 ) )  goto alphaj;
				break;
				
			case 0x755C:		// ClipMode DOWN
			case KEY_CTRL_DOWN:
				key=0;
				switch ( dumpflg ) {
					case 2: 		// Opcode
							c=SrcBase[csrPtr];
							if ( c==0x00 ) break ;
							if ( ( c==0x0C ) || ( c==0x0D ) ) {
								x=1;
								d=SrcBase[csrPtr+1];
								if ( ( d==0x0C ) || ( d==0x0D ) ) { csrPtr++; goto DOWNexit; }
							} else {
								x=pcx;
								while ( 1 ) {
									x += OpcodeStrLenBufpx( SrcBase, csrPtr);
									if ( x > EDITpxMAX ) { x=1; NextOpcodeGB( SrcBase, &csrPtr ); break ; }
									NextOpcodeGB( SrcBase, &csrPtr );
									c=SrcBase[csrPtr];
									if ( ( c==0x0C ) || ( c==0x0D ) || ( c==0x00 ) ) {
										x=1; 
										d=SrcBase[csrPtr+1];
										if ( ( d==0x0C ) || ( d==0x0D ) ) { csrPtr++; goto DOWNexit; }
										break;
									}
								}
							}
							c=SrcBase[csrPtr]; 
							if ( c==0x00 )  break;
							if ( ( c==0x0C ) || ( c==0x0D ) ) csrPtr++;
							while ( x < pcx ) {
								x += OpcodeStrLenBufpx( SrcBase, csrPtr);
								NextOpcodeGB( SrcBase, &csrPtr );
								c=SrcBase[csrPtr];
								if ( ( c==0x0C ) || ( c==0x0D ) || ( c==0x00 ) ) break;
							}
							if ( cy==(ymaxpos) ) { NextLinePhy( SrcBase, &offset, &offset_y ); }
							break;
					case 4: 		// hex dump
							cy++;
							csrPtr+=4;
							if ( cy>7 ) { (offset)+=4; cy=7; }
//							if ( (cx==7) || (cx==10) || (cx==13) ) cx--;
							break;
					default:
							break;
				}
			  DOWNexit:
				if ( ClipStartPtr>=0 ) ClipEndPtr=csrPtr;
				SearchMode=0;
				if ( ( alphastatus ) && ( alphalock==0 ) )  goto alphaj;
				break;
				
			case KEY_CTRL_SHIFT:
				MiniCursorSetFlashMode( 0 );		// mini cursor flashing off
				Cursor_SetFlashOff(); 			// cursor flashing off
				if ( ClipStartPtr >= 0 ) {  ClipStartPtr = -1 ; 	// ClipMode cancel
					DumpOpcode( SrcBase, &offset, &offset_y, csrPtr, &pcx, &cy, ClipStartPtr, ClipEndPtr);
					CB_ScrollBar( alllinenum, EditLineNum-1, ymax, 384-6, 0, 168, 6 ) ;
				 }
				if ( SearchMode ) break;;
				ShiftF6loop:
				DebugScreen = 0;
				alphastatus_bk = alphastatus ;
				alphastatus    = 0; 
				alphalock_bk   = alphalock ;
				alphalock      = 0;
				if ( CommandInputMethod ) { 
					Menu_SHIFT_MENU();
				} else {
//					FkeyClear( FKeyNo4 );
					i=ProgfileMax[ProgNo]-SrcSize(filebase);
					if ( i>9999 ) 	sprintf3(buffer, "%dk",  i/1024 );
					else 			sprintf3(buffer, "%d",  i );
					Fkey_dispN( FKeyNo4, buffer);
				}
				Fkey_Icon( FKeyNo1, 877 );	//	Fkey_dispN( FKeyNo1, "Var");
				Fkey_Icon( FKeyNo2, 286 );	//	Fkey_dispN( FKeyNo2, "Mat");
				Fkey_Icon( FKeyNo3, 560 );	//	Fkey_dispR( FKeyNo3, "VWIN");
				if ( lowercase  ) Fkey_dispN_aA( FKeyNo5, "A <> a"); else Fkey_dispN_Aa( FKeyNo5, "A <> a");
				if ( ( ( 1 <= DebugMode ) && ( DebugMode <=3 ) ) || ( CommandInputMethod==0 ) ) {		// ====== Debug Mode ======
					Fkey_Icon( FKeyNo6, 563 );	//	Fkey_dispN( FKeyNo6, "G<>T");
				} else {
					Fkey_Icon( FKeyNo6, 673 );	//	Fkey_dispR( FKeyNo5, "CHAR");
				}
				if ( DebugScreen==0 ) { vram=(unsigned short  *)PictAry[0]+384*192; for ( i=0; i<384; i++ ) if ( linebuf[i] != CB_ColorIndexEditBack ) vram[i]=linebuf[i]; }
//				Setup_SetEntry(0x14,01);	// SHIFT

				EnableDisplayStatusArea();
				CB_ColorIndex=0xFFFE;	// 

				i=ProgfileMax[ProgNo]-SrcSize(filebase);
//				if ( i>9999 ) 	sprintf3(buffer3, "%dk",  i/1024 );
//				else 			sprintf3(buffer3, "%d",  i );
				sprintf3(buffer3, "%5d free",  i );

				CB_PrintMini_Fix10( 385-10*8-15, -20, (unsigned char *)buffer3, MINI_OR); 	// display current line number 
				CB_ColorIndex=CB_ColorIndexEditLine;	// 
				CB_PrintMini_Fix10( 384-10*8-15, -21, (unsigned char *)buffer3, MINI_OR); 	// display current line number 
				CB_ColorIndex=-1;					// current color index reset
//				if ( (dumpflg==2) ) MiniCursorSetFlashMode( 1 );		// mini cursor flashing on
				GetKey_DisableMenuCatalog(&key);
				if ( key==0 ) if ( KeyCheckCHAR6() ) key=KEY_CHAR_6;
				if ( key==0 ) if ( KeyCheckCHAR3() ) key=KEY_CHAR_3;
				KeyRecover();
//				MiniCursorSetFlashMode( 0 );		// mini cursor flashing off
				switch (key) {
					case KEY_CTRL_QUIT:
							key = KEY_CTRL_EXIT;
//							ClipStartPtr = -1 ;		// ClipMode cancel
							CommandType=0; CommandPage=0;
							JumpMenuSw  = 0;
							DebugScreen = 0;
							DebugMenuSw = 0;
							goto exitjp;
							
					case KEY_CTRL_ALPHA:
						alphalockjp:
							alphalock = 1 ;
							alphastatus = 1;
							SetAlphaStatus( alphalock, lowercase );
							key=0;
							break;
//					case KEY_CTRL_SHIFT:
//							key=0;
//							ClipStartPtr = -1 ;		// ClipMode cancel
//							break;
							
					case KEY_CTRL_PAGEUP:
							switch (dumpflg) {
								case 2: 		// Opcode
									PrevLinePhyN( PageUpDownNum*ymax, SrcBase, &offset, &offset_y );	// Page Up
									csrPtr=OpcodeLinePtr( offset_y, SrcBase, offset);
									break;
								case 4: 		// hex dump
									csrPtr-=0x100;
									goto hexpage;
									break;
								default:
									break;
							}
							key=0;
							ClipStartPtr = -1 ;		// ClipMode cancel
							break;

					case KEY_CTRL_PAGEDOWN:
							switch (dumpflg) {
								case 2: 		// Opcode
									NextLinePhyN( PageUpDownNum*ymax, SrcBase, &offset, &offset_y );	// Page Down
									csrPtr=OpcodeLinePtr( offset_y, SrcBase, offset);
									if ( SrcBase[offset] == 0 ) PrevLinePhyN( ymax, SrcBase, &offset, &offset_y );
									break;
								case 4: 		// hex dump
									csrPtr+=0x100;
								  hexpage:
									offset=csrPtr;
									offset_y=0;
									cx=6; cy=2;
									break;
								default:
									break;
							}
							key=0;
							ClipStartPtr = -1 ;		// ClipMode cancel
							break;

					case KEY_CTRL_SETUP:
							if ( CommandInputMethod ) { 
								CommandType=CMD_SETUP; CommandPage=0;
							} else {
								selectSetup=SetupG(selectSetup, 1);
								Is1st=1;	// delay cursor flashing 
							}
							key=0;
							ClipStartPtr = -1 ;			// ClipMode cancel
							break;
							
					case KEY_CTRL_F1:
							selectVar=SetVar(selectVar);		// A - 
							key=0;
							ClipStartPtr = -1 ;			// ClipMode cancel
							break;
					case KEY_CTRL_F2:
							selectMatrix=SetMatrix(selectMatrix);		//
							key=0;
							ClipStartPtr = -1 ;			// ClipMode cancel
							break;
					case KEY_CTRL_F3:
							if ( CommandInputMethod ) { 
								CommandType=CMD_SHIFT_VWIN; CommandPage=0;
							} else {
								SetViewWindow();
								Is1st=1;	// delay cursor flashing 
							}
							key=0;
							ClipStartPtr = -1 ;			// ClipMode cancel
							break;
					case KEY_CTRL_F4:
							if ( CommandInputMethod ) { 
								CommandType=CMD_SHIFT_SKTCH; CommandPage=0;
								key=0;
								ClipStartPtr = -1 ;			// ClipMode cancel
							} else {
							  listdump:
								switch (dumpflg) {
									case 2: 		// Opcode
										dumpflg=4;
										break;
									case 4: 		// hex dump
										dumpflg=2;
									default:
										break;
								}
								offset=csrPtr;
								offset_y=0;
								cx=6; cy=2;
							}
							break;
					case KEY_CTRL_F5:	// A<>a
							lowercase=1-lowercase;
							if ( alphalock_bk ) goto alphalockjp;
							else
							if ( alphastatus_bk ) SetAlphaStatus( alphalock_bk, lowercase );
							key=0;
							ClipStartPtr = -1 ;		// ClipMode cancel
							break;
					case KEY_CTRL_F6:	// G<>T / CHAR
							if ( ( ( 1 <= DebugMode ) && ( DebugMode <=3 ) ) || ( CommandInputMethod==0 ) ) {		// ====== Debug Mode ======
								ScreenModeEdit=1-ScreenModeEdit;
								RestoreScreenModeEdit();
								GetKey_DisableCatalog(&key);
								if ( key == KEY_CTRL_SHIFT) goto ShiftF6loop;
							} else {
								key=SelectChar( &ContinuousSelect);
								if ( ( alphastatus ) || ( alphalock ) ) SetAlphaStatus( alphalock, lowercase );
								Is1st=1;	// delay cursor flashing 
							}
							break;
							
					case KEY_CTRL_CLIP:
							ClipStartPtr=csrPtr;
							Setup_SetEntry(0x14, 02 );	// Clip mode
							key=0;
							break;
					case KEY_CTRL_PASTE:
							if ( ClipBuffer != NULL ) {
								EditPaste( filebase, ClipBuffer, &csrPtr, &Undo );
								x=1; y=0; ptr=0;
								OpcodeLineN( ClipBuffer, &ptr, &x, &y );
								if ( ( y >= (ymax-cy) ) || ( y >= ymax ) ) {
									offset=csrPtr;
									offset_y=0;
									PrevLinePhyN( cy, SrcBase, &offset, &offset_y );		// csrY adjust
								}
								UpdateLineNum=1;
							}
							key=0;
							break;
							
					case KEY_CTRL_PRGM:
							if ( dumpflg==2 ) {
								if ( CommandInputMethod ) { 
									CommandType=CMD_PRGM; CommandPage=0;
								} else {
									key=SelectOpcode( CMDLIST_PRGM, 0 );
									if ( ( alphastatus ) || ( alphalock ) ) SetAlphaStatus( alphalock, lowercase );
								Is1st=1;	// delay cursor flashing 
								}
							}
							ClipStartPtr = -1 ;		// ClipMode cancel
							break;
//					case KEY_CTRL_INS:
//							if (InsertMode )	Setup_SetEntry(0x15, 1);		// insert mode cursor 
//							else 						Setup_SetEntry(0x15, 2);		// overwrite mode cursor 
//							key=0;
//							UpdateLineNum=1;
//							break;
							
					case KEY_CHAR_3:
							MsgBoxPush( 1 );
							locate(3,4); Prints((unsigned char*)"Hit GetKey Code");
							KeyRecover();
							GetKey_DisableMenuCatalog(&key);
							MsgBoxPop();
							sprintf3(buffer,"%d",CB_KeyCodeCnvt( key ) );
							EditPaste( filebase, buffer, &csrPtr, &Undo );
							key=0;
							UpdateLineNum=1;
							break;
					case KEY_CHAR_6:
						RGBselect:
							key=SetRGBColor( &color );
							if ( key != KEY_CTRL_EXE ) break;
							switch ( SetRGBColorMode ) {
								case 0:	// RGB
									sprintf3(buffer,"\x7F\x5E%d,%d,%d)",((color&0xF800) >> 8),((color&0x07E0) >> 3),((color&0x001F) << 3) );
									break;
								case 1:	// HSV -> HSL
									rgb2hsv( ((color&0xF800) >> 8), ((color&0x07E0) >> 3), ((color&0x001F) << 3), &H,&S,&V);
									sprintf3(buffer,"\x7F\x71%d,%d,%d)",H,S,V );
									break;
								case 2:	// HSL -> RGB
									rgb2hsl( ((color&0xF800) >> 8), ((color&0x07E0) >> 3), ((color&0x001F) << 3), &H,&S,&L);
									sprintf3(buffer,"\x7F\x73%d,%d,%d)",H,S,L );
									break;
							}
							EditPaste( filebase, buffer, &csrPtr, &Undo );
							key=0;
							UpdateLineNum=1;
							break;
					case KEY_CTRL_FORMAT:
							i=ColorIndexDialog0( 0 );
							switch ( i ) {
								case 0:	// black
									key=0xF99B;	
									break;
								case 1:	// blue
									key=0x7F35;	
									break;
								case 2:	// green
									key=0x7F36;	
									break;
								case 3:	// cyan
									key=0xF99E;	
									break;
								case 4:	// red
									key=0x7F34;	
									break;
								case 5:	// magenta
									key=0xF99D;	
									break;
								case 6:	// yellow
									key=0xF99F;	
									break;
								case 7:	// white
									key=0xF99C;	
									break;
								default:
									key=0;
									break;
							}
							break;
					case KEY_CTRL_OPTN:
					case KEY_SHIFT_OPTN:
							goto key_optn;
							
//					case 0x756E:	// SHIFT + <-
					case KEY_SHIFT_LEFT:
							i=EditFontSize & 0xF0;
							switch ( EditFontSize & 0x0F ) {
								case 8:		// normal +24
									EditFontSize=0;	// -> mini fix +Gap
									break;
								case 0:		// normal
									EditFontSize=10;	// -> mini fix +Gap
									break;
								case 10:	// mini fix			+Gap
									EditFontSize=9;		// -> mini +Gap
									break;
								case 9:		// mini				+Gap
									EditFontSize=2;		// -> mini fix
									break;
								case 2:		// mini fix
									EditFontSize=1;		// -> mini
									break;
								case 1:		// mini
									EditFontSize=14;	// -> minimini bold fix +Gap
									break;
									
								case 14: 	// minimini Bold fix	+Gap
									EditFontSize=13;	// -> minimini bold +Gap
									break;
								case 13:	// minimini Bold		+Gap
									EditFontSize=12;	// -> minimini fix +Gap
									break;
								case 12:	// minimini fix		+Gap
									EditFontSize=11;	// -> minimini +Gap
									break;
								case 11:	// minimini			+Gap
									EditFontSize=6;		// -> minimini bold fix
									break;
									
								case 6:	// minimini Bold fix
									EditFontSize=5;		// -> minimini bold
									break;
								case 5:		// minimini Bold
									EditFontSize=4;		// -> minimini fix
									break;
								case 4:		// minimini fix
									EditFontSize=3;		// -> minimini
									break;
								case 3:		// minimini
									EditFontSize=15;	// -> minimini fx 6x8 +Gap
									break;
									
								case 15: 	// minimini fx 6x8		+Gap
									EditFontSize=7	;	// -> minimini fx 6x8
									break;
								case 7: 	// minimini fx 6x8
									break;
							}
							EditFontSize|=i;
							UpdateLineNum=1;
							key=0;
							break;
//					case 0x756F:	// SHIFT + ->
					case KEY_SHIFT_RIGHT:
							i=EditFontSize & 0xF0;
							switch ( EditFontSize & 0x0F ) {
								case 8:		// normal +24
								case 0:		// normal
									EditFontSize=8;		// -> normal
									break;
								case 10:	// mini fix			+Gap
									EditFontSize=0;		// -> normal
									break;
								case 9:		// mini				+Gap
									EditFontSize=10;	// -> mini fix +Gap
									break;
								case 2:		// mini fix
									EditFontSize=9;		// -> mini +Gap
									break;
								case 1:		// mini
									EditFontSize=2;		// -> mini fix
									break;
									
								case 14: 	// minimini Bold fix	+Gap
									EditFontSize=1;		// -> mini
									break;
								case 13:	// minimini Bold		+Gap
									EditFontSize=14;	// -> minimini bold fix +Gap
									break;
								case 12:	// minimini fix		+Gap
									EditFontSize=13	;	// -> minimini bold +Gap
									break;
								case 11:	// minimini			+Gap
									EditFontSize=12	;	// -> minimini fix +Gap
									break;
									
								case 6:	// minimini Bold fix
									EditFontSize=11	;	// -> minimini bold +Gap
									break;
								case 5:		// minimini Bold
									EditFontSize=6	;	// -> minimini bold fix
									break;
								case 4:		// minimini fix
									EditFontSize=5	;	// -> minimini +Gap
									break;
								case 3:		// minimini
									EditFontSize=4	;	// -> minimini fix
									break;
									
								case 15: 	// minimini fx 6x8		+Gap
									EditFontSize=3	;	// -> minimini
									break;
								case 7: 	// minimini fx 6x8
									EditFontSize=15;	// -> minimini fx 6x8 +Gap
									break;
							}
							EditFontSize|=i;
							UpdateLineNum=1;
							key=0;
							break;
							
					case KEY_CTRL_CAPTURE:
							SysCalljmp( 4,5,6,7,0x17E6);	// CAPTURE
							break;
					case KEY_CTRL_CATALOG:
							key=CB_Catalog();
							break;
					default:
							break;
				}
				break;
				
			case KEY_CTRL_DEL:
				if ( ClipStartPtr >= 0 ) {
					goto F3del;
				} else {
					if ( InsertMode ) {		// insert mode
						PrevOpcodeGB( SrcBase, &csrPtr);
					}
					DeleteOpcodeUndo( filebase, &csrPtr, &Undo);
					key=0;
				}
				ClipStartPtr = -1 ;		// ClipMode cancel
				SearchMode=0;
				UpdateLineNum=1;
				break;
			case KEY_CTRL_OPTN:
			  key_optn:
				if ( SearchMode ) break;;
				if ( dumpflg==2 ) {
					if ( CommandInputMethod ) { 
						CommandType=CMD_OPTN; CommandPage=0;
					} else {
						key=SelectOpcode( CMDLIST_OPTN, 0 );
						if ( ( alphastatus ) || ( alphalock ) ) SetAlphaStatus( alphalock, lowercase );
						Is1st=1;	// delay cursor flashing 
					}
				}
				ClipStartPtr = -1 ;		// ClipMode cancel
				break;
			case KEY_CTRL_VARS:
				if ( SearchMode ) break;;
				if ( dumpflg==2 ) {
					if ( CommandInputMethod ) { 
						CommandType=CMD_VARS; CommandPage=0;
					} else {
						key=SelectOpcode( CMDLIST_VARS, 0 );
						if ( ( alphastatus ) || ( alphalock ) ) SetAlphaStatus( alphalock, lowercase );
						Is1st=1;	// delay cursor flashing 
					}
				}
				ClipStartPtr = -1 ;		// ClipMode cancel
				break;
			case KEY_CTRL_MENU:
				if ( SearchMode ) break;;
				if ( dumpflg==2 ) {
					key=SelectOpcodeRecent( CMDLIST_RECENT );
					Is1st=1;	// delay cursor flashing 
				}
				ClipStartPtr = -1 ;		// ClipMode cancel
				break;
				
			case 30045:		// KEY_CTRL_UNDO:
				ProgUndo( filebase, &csrPtr, &Undo );
				key=0;
				alphastatus = 0;
				alphalock = 0 ; 
				Setup_SetEntry(0x14, 0x00); 	// clear
				UpdateLineNum=1;
				ClipStartPtr = -1 ;		// ClipMode cancel
				break;

			default:
				break;
		}
		if ( key == 0xFFFF7F5D ) goto RGBselect;
		if ( key == 0xFFFF0945 ) {	// OPEN g3p
			i = BG_OpenFileDialog( buffer+1 );
			if ( i ) {
				buffer[0]='"';
				i=strlen(buffer);
				buffer[i++]='"';
				buffer[i]='\0';
				EditPaste( filebase, buffer, &csrPtr, &Undo );
				key=0;
				UpdateLineNum=1;
			}
		}
		
		if ( key == 0x1F91B ) { 	// Graph Y Store
			i=CommandType;
			CB_StoreString( 1, ClipBuffer );
			CommandType=i;
			key=0;
		}
		if ( key == 0x2F91B ) { 	// Graph Y Recall
			i=CommandType;
			string = CB_RecallString( 1 );
			goto pastestring;
		}
		if ( key == 0x4F91B ) { 	// Graph Y See
			i=CommandType;
			string = CB_SeeString( 1, &selectStr, ClipBuffer );
		  pastestring:
			CommandType=i;
			if ( string != NULL ) {
				EditPaste( filebase, string, &csrPtr, &Undo );
				UpdateLineNum=1;
			}
			key=0;
		}
		if ( dumpflg==2 ) {
			if ( (key&0xFF00FF00) == 0x5C005C00 ) goto gbnext;	// escape GB code 4byte
			keyH=(key&0xFF00) >>8 ;
			keyL=(key&0x00FF) ;
			switch ( keyH ) {		// ----- 2byte code -----
				case 0xFFFFFFF7:		// 
					if ( ( keyL==0x02 ) || ( keyL==0x0F ) || ( keyL==0x03 ) ||	// Else/ElseIf
						 ( keyL==0x07 ) || ( keyL==0x09 ) || ( keyL==0x0B ) || ( keyL==0xFFFFFFED )  ) {	// Next/WhileEnd/LpWhile/SwitchEnd
						i=0;
						while ( i < (CB_EditIndent&0x07) ) {
							PrevOpcodeGB( SrcBase, &csrPtr );
							c=GetOpcodeGB( SrcBase, csrPtr );
							if ( c==' ' ) DeleteOpcode( filebase, &csrPtr);
							else NextOpcodeGB( SrcBase, &csrPtr );
							i++;
						}
					}
				case 0x5C:		//
				case 0x7F:		// 
				case 0xFFFFFFF9:		// 
				case 0xFFFFFFE5:		// 
				case 0xFFFFFFE6:		// 
				case 0xFFFFFFE7:		// 
				case 0xFFFFFFFF:	// 
				  gbnext:
					if (ClipStartPtr>=0) { 
						ClipStartPtr = -1 ;		// ClipMode cancel			
						break;
					}
					if ( InsertMode ) {		// insert mode
						InsertOpcode( filebase, csrPtr, key );
					} else {					// overwrite mode
						if ( SrcBase[csrPtr] !=0 ) DeleteOpcode( filebase, &csrPtr);
						InsertOpcode( filebase, csrPtr, key );
					}
					if ( ErrorNo==0 ) NextOpcodeGB( SrcBase, &csrPtr );
					alphalock = 0 ;
					alphastatus = 0;
					help_code=key;
					SearchMode=0;
					key=0;
					DebugScreen = 0;
					UpdateLineNum=1;
					Undo.enable = 0;
					break;
				default:
					if ( IsGBCode1(keyH,keyL) ) goto gbnext;
					break;
			}
			indent=0;
			if ( key == 0x0C ) goto indent_CR;
			if ( key == KEY_CTRL_EXE ) {
					if ( SearchMode ) {		//	Next Search
						i=SearchOpcodeEdit( SrcBase, searchbuf, &csrPtr, 1 );
						if ( i==0 ) SearchMode=0;
					} else  {
						key=0x0D; // <CR>
					 indent_CR:
						ptr=csrPtr;
						PrevOpcodeGB( SrcBase, &ptr );
						d = GetOpcodeGB( SrcBase, ptr );
						ptr=csrPtr;
						if ( ( d!=0x0C ) && ( d!=0x0D ) ) PrevLineGB( SrcBase, &ptr );	// current line top
						x=1;
						while ( SrcBase[ptr]==' ' ) {
							if ( x >= pcx ) break;
							x += OpcodeStrLenBufpx( SrcBase, ptr);
							ptr++;
							indent++;
						}
					  	if ( csrPtr != ptr ) indent += CheckIndentCommand( SrcBase, ptr );
					}
			}
			if ( ( 0x00 < key ) && ( key < 0xFF ) || ( key == KEY_CTRL_XTT ) ) {		// ----- 1 byte code -----
				if ( ClipStartPtr >= 0 ) { 
						ClipStartPtr = -1 ;		// ClipMode cancel			
				} else {
//					if ( key == KEY_CHAR_POW )   key='^';
					if ( key == KEY_CTRL_XTT  )  key=XTTKey( key ); 	// 'X' or 0x90 or <r> or <Theta> or T

					help_code=key;
					do {
						if ( ( InsertMode ) || ( indent ) ) {		// insert mode
								InsertOpcode( filebase, csrPtr, key );
						} else {					// overwrite mode
								if ( SrcBase[csrPtr] !=0 ) DeleteOpcode( filebase, &csrPtr);
								InsertOpcode( filebase, csrPtr, key );
						}
						if ( ErrorNo==0 ) NextOpcodeGB( SrcBase, &csrPtr );
						indent--;
						key=' ';
					} while ( indent >= 0 ) ;

					if ( alphalock == 0 ) alphastatus = 0;
					SearchMode=0;
					key=0;
					DebugScreen = 0;
					UpdateLineNum=1;
					Undo.enable = 0;
					if ( help_code == 0x0D ) {
						if ( alphalock ) goto alphalockjp;
					}	
				}
			} else {
				if ( key == KEY_CTRL_AC ) SearchMode=0;
				if ( key != 0 ) {
					if ( alphalock == 0 ) alphastatus = 0;
//					alphalock = 0 ;
//					alphastatus = 0;
				}
			}
		} else
		if ( dumpflg==4 ) {
				key=0;
		}

	}
	edit_exit:
	Cursor_SetFlashOff(); 			// cursor flashing off
	MiniCursorSetFlashMode( 0 );		// mini cursor flashing off
	EnableStatusArea(3);	// disable StatusArea
//	Bkey_Set_RepeatTime(FirstCount,NextCount);		// restore repeat time
//	RestoreScreenModeEdit();
	return key;
}

//----------------------------------------------------------------------------------------------

int CB_BreakStop(char * SRC) {
	int key=1;
	int r=0;
	int picttmp =CB_BackPict;
	int colortmp=CB_ColorIndex;
	int backcolortmp=CB_BackColorIndex;
	int scrmode=ScreenMode;
	int dbgmode= ( ( DisableDebugMode == 0 ) || ( ForceDebugMode ) ) ;
	int bk_GBcode=GBcode;
	char buf[22];
	
	if ( BreakPtr == -7 ) return BreakPtr;	// return to main program
	if ( ErrorNo == StackERR ) { BreakPtr=-999; TryFlag=0; return BreakPtr; }	// stack error
	if ( ( BreakPtr == 0 ) && ( TryFlag ) ) return 0;
	
	CB_ColorIndex=-1;				// current color index reset
	CB_BackColorIndex=0xFFFF;		// Back color index (default White)
	if ( ScreenMode ) {
		CB_SaveGraphVRAM();	// save Graphic screen
	} else {
		CB_SaveTextVRAM();	// save Graphic screen
	}
	CMT_Delay_micros( 300 );
	HiddenRAM_MatAryStore();	// MatAry ptr -> HiddenRAM
//	CB_SelectTextVRAM();	// Select Text Screen
//	CB_SelectGraphVRAM();	// Select Graphic screen
//	CB_SelectTextVRAM();	// Select Text Screen

	if ( ErrorNo ) { 
		CB_ErrMsg( ErrorNo );
		BreakPtr=-999;
		ExecPtr=ErrorPtr;
//		if ( ErrorNo == StackERR ) { 	// stack error
//			BreakPtr=-9999; 
//			CB_BackPict=picttmp; 
//			CB_ColorIndex=colortmp; 
//			CB_BackColorIndex=backcolortmp; 
//			return BreakPtr;
//		}
		DebugScreen = 0;
		ErrorNo=0;
		if ( dbgmode ) DebugMode=2;	// enable debug mode
	}
	
	if ( ( DebugMode < 2 ) && ( BreakPtr > -999) ) {
		PopUpWin(6);
		locate(9,3); Prints((unsigned char *)"Break");
		locate(6,5); Prints((unsigned char *) "Press:[EXIT]");
		PrintMinix3(26,46,(unsigned char *)"Continue : [EXE]/[F1]",MINI_OVER);
		if ( ProgfileAdrs[ProgNo] == 0 ) {
			sprintf3( buf, "%08X  ProgNo=%d ",ProgfileAdrs[ProgNo], ProgNo );
			PrintMinix3(26,46,(unsigned char *)buf,MINI_OVER);
		}
		
//		Bdisp_PutDisp_DD();
		
		KeyRecover(); 
		WaitKeyAC();
		while ( 1 ) {
			EnableDisplayStatusArea();
			GetKey_DisableCatalog(&key);
			EnableStatusArea(3);	// disable StatusArea
			if ( key == KEY_CTRL_EXIT  ) break ;
			if ( key == KEY_CTRL_AC    ) break ;
			if ( key == KEY_CTRL_RIGHT ) break ;
			if ( key == KEY_CTRL_LEFT  ) break ;
			if ( key == KEY_CTRL_F1  ) { 
				WaitKeyF1() ;
				goto cont0;
			}
			if ( key == KEY_CTRL_EXE ) { 
				WaitKeyEXE() ;
			  cont0:
				DebugMode=0; BreakPtr=0;
				CB_BackPict=picttmp;
				CB_ColorIndex=colortmp;
				CB_BackColorIndex=backcolortmp;
				goto cont;
			}
		}
		if ( dbgmode  ) DebugMode=2;	// enable debug mode
		DebugScreen = 0;
	} else {	// Step mode
		if( BreakPtr > -999) BreakPtr=ExecPtr;	// set breakptr
	}
	
	if ( ProgfileAdrs[ProgNo] == 0 ) ProgfileAdrs[ProgNo] = SRC-0x56;

	ScreenModeEdit=scrmode;
	if ( ForceReturn ) return 1;	// force program end
	if ( dbgmode  ) key=EditRun(2);	// Program listing & edit
	
	CB_BackPict=picttmp;
	CB_ColorIndex=colortmp;
	CB_BackColorIndex=backcolortmp;
	GBcode=bk_GBcode;
	if ( ( dbgmode == 0 ) ||  ( key == KEY_CTRL_EXIT ) ) { 
		if ( ProgEntryN == 0 ) DebugMode=0;
		if ( BreakPtr != -7 ) BreakPtr=-999;
		r = BreakPtr;
	}
	cont:
	KeyRecover(); 
	if ( DebugMode == 1 ) DebugMode=0;
	ScreenMode=scrmode;
	if ( ScreenMode ) {
		CB_RestoreGraphVRAM();	// Resotre Graphic screen
	} else {
		CB_RestoreTextVRAM();	// Resotre Text screen
	}
	if ( r ) return r;
//	Bdisp_PutDisp_DD_DrawBusy();
	CB_StatusDisp_Run();
	Bdisp_PutDisp_DD_stripe_BG( 0, 215);
	HourGlass();
	return r;
}

//----------------------------------------------------------------------------------------------
//int eObjectAlign4a( unsigned int n ){ return n; }	// align +4byte
//int eObjectAlign4b( unsigned int n ){ return n; }	// align +4byte
//int eObjectAlign4c( unsigned int n ){ return n; }	// align +4byte
//int eObjectAlign4d( unsigned int n ){ return n; }	// align +4byte
//int eObjectAlign4e( unsigned int n ){ return n; }	// align +4byte
//int eObjectAlign4f( unsigned int n ){ return n; }	// align +4byte
//----------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------
//int edeitdummy(int x, int y){
//	return x*y;
//	locate(x,y  ); Prints((unsigned char *) "1234");
//	locate(x,y+1); Prints((unsigned char *) "5678");
//	locate(x,y+2); Prints((unsigned char *) "ABCD");
//}

}
