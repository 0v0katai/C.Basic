#include "CB.h"

//----------------------------------------------------------------------------------------------
//char ClipBuffer[ClipMax];
//char *ClipBuffer;
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
char  EDITpxMAX=123;	//  max x pixel
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
		NextLine( SRC, &ptr );
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
		NextOpcode( filebase+0x56, ptr );
	}
	Undo->enable = 0;	// clear Undo
	SetSrcSize( filebase, SrcSize(filebase)+len ) ; 	// set new file size
}


void InsertOpcode( char *filebase, int ptr, int opcode ){
	int len,i,j;
	int opH,opL;
	char *srcbase;
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
	AddOpcodeRecent( opcode ) ;
}

void DeleteOpcode( char *filebase, int *ptr ){
	int len,i;
	int opH,opL;
	int opcode;
	opcode=GetOpcode( filebase+0x56, *ptr );
	if ( opcode == 0 ) {
		if ( *ptr == 0 ) return ;
		PrevOpcode( filebase+0x56, &(*ptr) );
		opcode=GetOpcode( filebase+0x56, *ptr );
	}
	len=OpcodeLen( opcode );
//	if ( len == 0 ) return ;
//	for ( i=(*ptr)+0x56; i<=ProgfileMax[ProgNo]; i++ ) filebase[i]=filebase[i+len];
	ProgMoveMem( filebase+0x56, (*ptr), SrcSize(filebase), len );

	SetSrcSize( filebase, SrcSize(filebase)-len ) ; 	// set new file size
	ProgfileEdit[ProgNo]=1;	// edit program
}

void DeleteOpcodeUndo( char *filebase, int *ptr, cUndo *Undo ){
	int len,i;
	int opH,opL;
	int opcode;
	opcode=GetOpcode( filebase+0x56, *ptr );
	if ( opcode == 0 ) {
		if ( *ptr == 0 ) return ;
		PrevOpcode( filebase+0x56, &(*ptr) );
		opcode=GetOpcode( filebase+0x56, *ptr );
	}
	len=OpcodeLen( opcode );
//	if ( len == 0 ) return ;
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
char* CLIP_Buffer(){
	return (char*)MatAry[Mattmp_clipBuffer].Adrs;
}

char * Recent_HiddenRAM_MatTopPtr;

char* NewclipBuffer( int *size ){	// size:-1  max
	int free;
	int reg = Mattmp_clipBuffer;	//	ClipBuffer
	char *buffer;
	free = HiddenRAM_MatTopPtr - HiddenRAM_ProgNextPtr;
	Recent_HiddenRAM_MatTopPtr = HiddenRAM_MatTopPtr;
	if ( *size<0 ) *size=free-4;
	if ( free < *size ) {
		ErrorNo=NotEnoughMemoryERR;		// Memory error
		CB_ErrMsg(ErrorNo);
		return NULL;
	}
	if ( MatAry[reg].SizeA == 0 ) {
		DimMatrixSub( reg, 8, *size, 1, 0 );	// byte matrix
	} else {
		DimMatrixSubNoinit( reg, 8, *size, 1, 0, 0 );
	}
	buffer = (char *)MatAry[reg].Adrs;
	buffer[0]='\0';
	return buffer;
}
void AdjclipBuffer( int size ){	// size
	size = (size+7) & 0xFFFFFFF8;	// 8byte align
	memcpy2( Recent_HiddenRAM_MatTopPtr-size, HiddenRAM_MatTopPtr, size );
	HiddenRAM_MatTopPtr = Recent_HiddenRAM_MatTopPtr - size;
	MatAry[Mattmp_clipBuffer].Adrs = (double*)HiddenRAM_MatTopPtr;
}

void EditPaste( char *filebase, char *Buffer, int *ptr, cUndo *Undo ){
	int len,i,j;
	char *srcbase;
	if ( ( Buffer== 0 ) || ( Buffer[0] =='\0' ) ) return ;	// no clip data
		
	len=strlenOp(Buffer);
	if ( len <=0 ) return;
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

void EditCopy( char *filebase, int ptr, int startp, int endp ){
	int len,i,j;
	char *srcbase;
	char *Buffer;

	PrevOpcode( filebase+0x56, &endp ); if ( startp>endp ) return;
	i=OpcodeLen( GetOpcode(filebase+0x56, endp) );
	len=(endp)-(startp)+i;
	if ( len <=0 ) return;
	Buffer=NewclipBuffer(&len);
	if ( Buffer== 0 ) return ;	// no clip data
	srcbase=filebase+0x56+(startp);
	for ( i=0; i<len; i++ ) Buffer[i]=srcbase[i];	// copy to Buffer
	Buffer[i]='\0';
}

void EditCutDel( char *filebase, int *ptr, int startp, int endp, int del, cUndo *Undo ){	// del:1 delete
	int len,i;
	char *srcbase;
	char *Buffer;

	PrevOpcode( filebase+0x56, &endp ); if ( startp>endp ) return;
	i=OpcodeLen( GetOpcode(filebase+0x56, endp) );
	len=(endp)-(startp)+i;
	if ( len <=0 ) return;
	if ( del == 0 ) {
		Buffer=NewclipBuffer(&len);
		if ( Buffer== 0 ) return ;	// no clip data
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
	PrevLine( srcbase, &startp );
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
	} else PrevLine( srcbase, &startp );
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
		}
		c=srcbase[ptr++];
	}
	return endp+plus;
}

//---------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------
char	CursorStyle=0;
char   MiniCursorflag = 0;	// GCursor Pixel ON:1 OFF:0
short  MiniCursorX;
short  MiniCursorY;

void MiniCursorFlashing() {		// timer IRQ handler
	DISPBOX area;
	unsigned char CsrDATA[]={ 0xFF,0xFF,0xFF,0xFF,0xFF };
	DISPGRAPH minicsr;
	int mini=(EditFontSize & 0x0F),f;

	if ( ( 0<=MiniCursorX ) && ( MiniCursorX<126 ) && ( 0<=MiniCursorY ) && ( MiniCursorY<60 ) ) {
		f = MiniCursorflag & 31;
		if ( f<16 ) {
				minicsr.x = MiniCursorX;
				minicsr.y = MiniCursorY;
				minicsr.GraphData.width =  2; if  ( CursorStyle >= 0x06 ) minicsr.GraphData.width = 4;
				if ( MiniCursorX + minicsr.GraphData.width > 127 ) minicsr.GraphData.width =2;
				minicsr.GraphData.height = 5;
				minicsr.GraphData.pBitmap = CsrDATA; 	// mini cursor pattern
				minicsr.WriteModify = IMB_WRITEMODIFY_NORMAL;
				minicsr.WriteKind = IMB_WRITEKIND_OR;
				if (   ( mini>=2 ) )	minicsr.WriteKind = IMB_WRITEKIND_XOR;	// reverse mode
				if ( ( ( mini==3 ) || ( mini==4 ) ) && ( CursorStyle < 0x06 ) ) {			// under cursor
					minicsr.y = MiniCursorY+5;
					minicsr.GraphData.height = 1;
					minicsr.GraphData.width = 4;
				}
			    Bdisp_WriteGraph_DD(&minicsr); 		// drawing only display driver
		} else {
				Bdisp_PutDisp_DD();
		}
		MiniCursorflag++;
	}
}

void MiniCursorSetFlashMode(int set) {	// 1:on  0:off
	switch (set) {
		case 0:
			KillTimer(ID_USER_TIMER1);
			break;
		case 1:
			SetTimer(ID_USER_TIMER1, 25, (void*)&MiniCursorFlashing);
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
	int opcode = GetOpcode( buffer, ofst );
	int mini=(EditFontSize & 0x0F);
	if ( opcode=='\0' ) return 0;
	if ( ( opcode==0x0C ) || ( opcode==0x0D ) ) {
		if ( mini == 0 ) return 6;
		else return 5;	// minifont
	}
	CB_OpcodeToStr( opcode, tmpbuf ) ;		// SYSCALL
	slen = CB_MB_ElementCount( tmpbuf ) ;	// SYSCALL
	if ( mini == 0 ) return slen*6;
//	MiniCursorflag=0;		// mini cursor initialize
	else return CB_PrintMiniLengthStr( (unsigned char*)tmpbuf, EditExtFont);	// minifont ext font mode
}

int OpcodeLineSub( char *buffer, int *ofst , int *len, char *opstr ) {
	int opcode = GetOpcode( buffer, *ofst );
	if ( opcode=='\0' ) return 0;
	if ( ( opcode==0x0C ) || ( opcode==0x0D ) ) return 0 ;
	(*ofst) += OpcodeLen( opcode );
	CB_OpcodeToStr( opcode, opstr ) ;		// SYSCALL
	(*len) = CB_MB_ElementCount( opstr ) ;	// SYSCALL
	return 1;
}
int OpcodeMinilength( char *str, int *oplen ){
	int c=(char)*str;
	int length=CB_PrintMiniLength( (unsigned char *)str++, EditExtFont);	// minifont ext font mode
	(*oplen)=1;
	if ( (c==0x7F)||(c==0xFFFFFFF9)||(c==0xFFFFFFE5)||(c==0xFFFFFFE6)||(c==0xFFFFFFE7)||(c==0xFFFFFFFF) )  (*oplen)++;
	return length;
}

void OpcodeLineXYsub( char *buffer, int len, int *px, int *y ) {
	int i,oplen;
	char *opstr2;
	opstr2=(char *)buffer;
	i=0;
	while ( i < len ) {
		if ( (EditFontSize & 0x0F) == 0 ) {
			(*px)+=6;
		} else {
			(*px)+=OpcodeMinilength( opstr2,&oplen ); opstr2+=oplen;
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
			NextLine( buffer, &(*ofst));
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
			NextLine( buffer, &(*ofst));
			*ofst_y = 0;
		}
	}
}

void PrevLinePhy( char *buffer, int *ofst, int *ofst_y ) {
	int ofst2,px,y;
	if ( *ofst_y == 0 ) {
		if ( *ofst != 0 ) { 
			PrevLine( buffer, &(*ofst));
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
	int c,no;
	ofst=0;
	no=1;
	while ( ofst < offset ){
		c=SrcBase[ofst++];
		switch ( c ) {
			case 0x00:	// <EOF>
				return no;
			case 0x0C:	// dsps
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
		}
	}
	return no ; // 
}

int JumpLineNum( char * SrcBase, int lineNum ) {	// Logical line numbrer
	int ofst;
	int c,no;
	ofst=0; 
	no=1;
	while ( no < lineNum ){
		c=SrcBase[ofst++];
		switch ( c ) {
			case 0x00:	// <EOF>
				return ofst;
			case 0x0C:	// dsps
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
		}
	}
	return ofst;
}

int AllLineNum( char * SrcBase ) {	// Logical line numbrer
	int ptr=0;
	ptr=EndOfSrc( SrcBase, ptr );
	return HomeLineNum( SrcBase, ptr );
}


int PrintOpcodeLineN( int *csry, int ynum, int ymax, int *n, char *buffer, int ofst, int csrPtr, int *pcx, int *cy, int ClipStartPtr, int ClipEndPtr, int *NumOfset) {
	char tmpbuf[18];
	unsigned char buff[16];
	unsigned char *buf;
	unsigned char *tmpb;
	int i,len,px=1,y=0,rev;
	int opcode;
	int  c=1,d;
	int Numflag=0,CurrentLine;
	int yk,mini= EditFontSize & 0x0F ;
	if ( mini ) yk=6; else yk=8;
	if ( ClipEndPtr < ClipStartPtr ) { i=ClipStartPtr; ClipStartPtr=ClipEndPtr; ClipEndPtr=i; }
	while ( 1 ) {
		CurrentLine=EditLineNum+(*NumOfset);
		rev=0; if ( ( ClipStartPtr >= 0 ) && ( ClipStartPtr <= ofst ) && ( ofst < ClipEndPtr ) ) rev=1;
		if ( y == (*n) )if (ofst==csrPtr) { *pcx=px; *cy=(*csry); CurrentLineNum=CurrentLine; }
		opcode = GetOpcode( buffer, ofst );
		if ( opcode=='\0' ) break;
		ofst += OpcodeLen( opcode );
		CB_OpcodeToStr( opcode, tmpbuf ) ; // SYSCALL
		len = CB_MB_ElementCount( tmpbuf ) ;				// SYSCALL
		tmpb=(unsigned char*)&tmpbuf[0];
		i=0;
		while ( i < len ) {
			if ( y == (*n) ) {
				if ( ( Numflag==0 ) && ( EDITpxNum ) ) { Numflag=1;
					sprintf( (char*)buff, "%3d", CurrentLine );
					buf=buff; if ( strlen((char*)buff)>3 ) buf++;
					CB_PrintMini( 0, ((*csry)-1)*yk+2, buf, MINI_OVER );
				}
				if ( mini == 0 ) { c=px-1+EDITpxNum;
					if ( rev )	CB_PrintRevC_ext( c/6+1 ,(*csry), (unsigned char*)(tmpb), EditExtFont  ) ;
					else		CB_PrintC_ext(    c/6+1 ,(*csry), (unsigned char*)(tmpb), EditExtFont  ) ;
					px+=6;
				} else { c=px+EDITpxNum; d=((*csry)-1)*6+2;
					if ( rev )	px+=CB_PrintMiniC( c, d, tmpb, MINI_REV  | (0x100*EditExtFont) ) ;
					else		px+=CB_PrintMiniC( c, d, tmpb, MINI_OVER | (0x100*EditExtFont) ) ;
				}
				if ( ( px > EDITpxMAX ) || ( opcode==0x0C ) || ( opcode==0x0D ) ) {  (y)++; px=1; (*n)++; (*csry)++; }
				if ( ( opcode==0x0D ) && ( buffer[ofst-2]!=0x5C ) ) { Numflag=0; (*NumOfset)++; }
//				Bdisp_PutDisp_DD();
			} else {
				if ( mini == 0 ) {
						px+=6;
				} else {
						px+=CB_PrintMiniLength( tmpb, EditExtFont );	// minifont ext font mode
				}
				if ( ( px > EDITpxMAX ) ) { (y)++; px=1; }
			}
			c=(char)*tmpb & 0xFF;
			if ( (c==0x7F)||(c==0xF7)||(c==0xF9)||(c==0xE5)||(c==0xE6)||(c==0xE7)||(c==0xFF) ) { tmpb++; }
			tmpb++; i++;
		}
		if ( ( opcode==0x0C ) || ( opcode==0x0D ) ) break ;
		if ( (*csry) > ymax ) break ;
		if ( (*n) > ynum ) break;
	}
	if ( y == (*n) )  if (ofst==csrPtr) { *pcx=px; *cy=(*csry); CurrentLineNum=CurrentLine; }
	if ( y == (*n)+1 )if (ofst==csrPtr) { *pcx=px; *cy=(*csry)+1; CurrentLineNum=CurrentLine; }
	return ofst;
}

//---------------------------------------------------------------------------------------------

int DumpOpcode( char *SrcBase, int *offset, int *offset_y, int csrPtr, int *pcx, int *cy, int ClipStartPtr, int ClipEndPtr){
	int i,j,n,px,y,ynum;
	int ofst,ofst2,ofstYptr;
	int count=60;
	int ymin,ymax,ymaxpos;
	int NumOfset;
	if ( (EditFontSize & 0x0F) ) { ymax=8; ymaxpos=9; } else { ymax=6; ymaxpos=7; }
	if ( EditTopLine  ) { ymin=1; ymax++; } else { ymin=2; ymax++; }
	if ( ( EDITpxNum ) && ( UpdateLineNum==0 ) ) {
		EditLineNum=HomeLineNum(SrcBase, *offset );	// line number re-adjust
	}
  jp:
	*pcx=0; *cy=0;
	while ( 1 ) {
		NumOfset=0;	// line number offset
//		for ( y=ymin; y<8; y++ ) { locate(1,y); PrintLine((unsigned char*)" ",21); }
		ML_rectangle( 0, 8*(ymin-1), 127, 55, 0, 0, 0);
//		ML_clear_vram(); 
		y=ymin; ofst=(*offset);
		ofst2=ofst;
		px=1; ynum=0;
		OpcodeLineN( SrcBase, &ofst2 ,&px ,&ynum);
		n=(*offset_y);
		ofst=PrintOpcodeLineN( &y, ynum, ymax, &n, SrcBase, ofst, csrPtr, &(*pcx), &(*cy), ClipStartPtr, ClipEndPtr, &NumOfset);
		while ( y <= ymaxpos ) {
				if ( SrcBase[ofst]==0x00 ) break ;
				ofst2=ofst;
				px=1; ynum=0;
				OpcodeLineN( SrcBase, &ofst2 ,&px ,&ynum);
				n=0;
				ofst = PrintOpcodeLineN( &y, ynum, ymax, &n, SrcBase, ofst, csrPtr, &(*pcx), &(*cy), ClipStartPtr, ClipEndPtr, &NumOfset);
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
	if ( ( EDITpxNum ) && ( UpdateLineNum ) ) {
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
		locate( 5,j+2); Print((unsigned char*)":           /    ");
		for (i=0; i<4 ; i++){
				Hex2PrintXY( i*3+6, j+(2), "", SrcBase[offset+i+j*4]&0xFF);
		}
		for (i=0; i<4 ; i++){
//				CB_PrintC(18+i,j+(2),(unsigned char*)SrcBase+offset+i+j*4);
				locate (18+i,j+(2));
				PrintC( (unsigned char*)SrcBase+offset+i+j*4 );
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
			Fkey_dispN( FKeyNo3, "CMD");
		}
	if ( lowercase  ) Fkey_dispN_aA( FKeyNo4, "A<>a"); else Fkey_dispN_Aa( FKeyNo4, "A<>a");
	Fkey_Icon( FKeyNo5, 673 );	//	Fkey_dispR( FKeyNo5, "CHAR");
}

int SearchOpcodeEdit( char *SrcBase, char *searchstr, int *csrptr, int next){
	int csrbkup=(*csrptr);
	int sptr=0,cptr;
	int opcode=1,srccode;
	int size=SrcEndPtr(SrcBase-0x56);

	Bdisp_PutDisp_DD_DrawBusy();
	opcode =GetOpcode( SrcBase, *csrptr ) ;
	srccode=GetOpcode( searchstr, sptr ) ;
	if ( next ) if ( opcode == srccode ) NextOpcode( SrcBase, &(*csrptr) );
	while ( (*csrptr)<size ) {
		sptr=0;
		opcode =GetOpcode( SrcBase, *csrptr ) ;
		if ( opcode == 0 ) break;
		srccode=GetOpcode( searchstr, sptr ) ;
		if ( opcode != srccode ) {
			NextOpcode( SrcBase, &(*csrptr) );
		} else {
			cptr = *csrptr;
			while ( (*csrptr)<size ) {
				NextOpcode( searchstr, &sptr );
				srccode=GetOpcode( searchstr, sptr ) ;
				if ( srccode == 0x00 ) { *csrptr=cptr; return 1; }	// Search Ok
				NextOpcode( SrcBase, &(*csrptr) );
				opcode =GetOpcode( SrcBase, *csrptr ) ;
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
	unsigned int key;
	int i=0,sptr;
	int srcmode=1;	// 1:SRC  2:Replace
	
	repstr[0]='\0';
  loop:
	Bdisp_AllClr_DDVRAM();
	locate(1,1); Print((unsigned char*)"Search For Text");
	locate(1,2); Print((unsigned char*)"---------------------");
	locate(1,4); Print((unsigned char*)"---------------------");
	Bdisp_PutDisp_DD_DrawBusy();
	KeyRecover(); 
	do {
		srcmenu();
		key= InputStrSubFn( 1, 3, 21, strlenOp(searchstr), searchstr, 63, " ", REV_OFF, FLOAT_ON, EXP_ON, ALPHA_ON, HEX_OFF, PAL_ON, EXIT_CANCEL_OFF, AC_CANCEL_ON);
	} while ( key == KEY_CTRL_AC ) ;	// AC
	if ( key == KEY_CTRL_EXIT ) return 0;	// exit
	if ( strlenOp(searchstr) == 0 ) goto loop;
	if ( key == KEY_CTRL_F2 ) {
		locate(1,1); Print((unsigned char*)"Replacement Text");
		locate(1,6); Print((unsigned char*)"---------------------");
		Bdisp_PutDisp_DD_DrawBusy();
		KeyRecover(); 
		do {
			srcmenu();
			key= InputStrSubFn( 1, 5, 21, strlenOp(repstr), repstr, 63, " ", REV_OFF, FLOAT_ON, EXP_ON, ALPHA_ON, HEX_OFF, PAL_ON, EXIT_CANCEL_OFF, AC_CANCEL_ON);
		} while ( key == KEY_CTRL_AC ) ;	// AC
		if ( key == KEY_CTRL_EXIT ) return 0;	// exit
		srcmode=2;	// replace
	}

	i = SearchOpcodeEdit( SrcBase, searchstr, &(*csrptr), 0 );
	if ( i ) return srcmode;	// ok
	ErrorMSGstr1(" Not Found ");
	goto loop;	//  not found loop
}

//---------------------------------------------------------------------------------------------
void jumpmenu(){
//	Fkey_Icon( FKeyNo1, 393 );	//	Fkey_dispN( FKeyNo1, "TOP ");
//	Fkey_Icon( FKeyNo2, 394 );	//	Fkey_dispN( FKeyNo2, "BTM");
	Fkey_Icon( FKeyNo1, 965 );	//	Fkey_dispN( FKeyNo1, "TOP ");
	Fkey_Icon( FKeyNo2, 967 );	//	Fkey_dispN( FKeyNo2, "BTM");
	Fkey_Icon( FKeyNo3, 964 );	//	Fkey_dispN( FKeyNo3, "GOTO");
	Fkey_dispN_aA( FKeyNo5, "Skp\xE6\x92");
	Fkey_dispN_aA( FKeyNo6, "Skp\xE6\x93");
}

int JumpGoto( char * SrcBase, int *offset, int *offset_y, int cy) {
	unsigned int key;
	char buffer[32];
	int lineAll,curline;
	int ofst,ofsty;
	int i,n;
	PopUpWin(3);
	locate( 3,3); Print((unsigned char *)"Goto Line Number");
	Bdisp_PutDisp_DD_DrawBusy();

	sprintf(buffer,"current:%d",CurrentLineNum);
	locate( 3,4); Print((unsigned char *)buffer);
	Bdisp_PutDisp_DD_DrawBusy();
	
	lineAll=AllLineNum( SrcBase );
	
	sprintf(buffer,"[1~%d]:",lineAll);
	locate( 3,5); Print((unsigned char *)buffer);
	i=strlen(buffer)+3;
	n=0;
	while (1) {
		n=InputNumD( i, 5, 5, n, " ", REV_OFF, FLOAT_OFF, EXP_OFF, &key);		// 0123456789
 		if ( (1<=n)&&(n<=lineAll) ) break;
 		if ( n==0 ) break;
 		if ( n < 0 ) n=1;
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

unsigned int EditRun(int run){		// run:1 exec      run:2 edit
	char *filebase,*SrcBase;
	unsigned int key=0;
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
	unsigned short opcode;
	int ClipStartPtr = -1 ;
	int ClipEndPtr   = -1 ;
	char alphalock = 0 ;
	char alphalock_bk ;
	char alphastatus = 0;
	char alphastatus_bk ;
	char SearchMode=0;
	int ContinuousSelect=0;
	char DebugMenuSw=0;
	char JumpMenuSw=0;
	int ymin,ymax,ymaxpos;
	int mini;
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
	CursorStyle=0;	// insert mode
	EditLineNum=0;
	UpdateLineNum=0;

	Undo.enable = 0;	// clear Undo

	Bkey_Get_RepeatTime(&FirstCount,&NextCount);	// repeat time
	Bkey_Set_RepeatTime(KeyRepeatFirstCount,KeyRepeatNextCount);		// set cursor rep

//	PreProcess(SrcBase);			// Basic source adjust

//	Bdisp_AllClr_DDVRAM();

//	i=0;
//	while ( ProgfileAdrs[i] ) {
//			locate (1,i+1);
//			Print( ProgfileAdrs[i]+0x3C );
//			i++;
//	}
//	GetKey(&key);

	mini= EditFontSize & 0x03 ;
	if ( mini ) { ymax=8; ymaxpos=9; } else { ymax=6; ymaxpos=7; }
	if ( EditTopLine  ) { ymin=1; ymax++; } else { ymin=2; }
	PrevLinePhyN( ymax, SrcBase, &offset, &offset_y );	// csrY adjust
	UpdateLineNum=1;

	if ( run == 1 ) { key=KEY_CTRL_F6; goto directrun; }	// direct run

	if ( DebugMode ) DebugMenuSw=1; 
	PutKey( KEY_CTRL_SHIFT, 1 ); GetKey(&key);
	PutKey( KEY_CTRL_SHIFT, 1 ); GetKey(&key);

//	Cursor_SetFlashMode(1);			// cursor flashing on
	if (Cursor_GetFlashStyle()<0x6)	Cursor_SetFlashOn(0x0);		// insert mode cursor 
		else 						Cursor_SetFlashOn(0x6);		// overwrite mode cursor 
		
	KeyRecover();
	while ( cont ) {
		if ( DebugMode==0 ) DebugMenuSw=0; 
		mini=(EditFontSize & 0x0F);
		if ( mini ) { ymax=8; ymaxpos=9; } else { ymax=6; ymaxpos=7; }
		if ( EditTopLine  ) { ymin=1; ymax++; } else { ymin=2; }
		if ( EditFontSize & 0xF0 ) EDITpxNum=12; else EDITpxNum=0;
		EDITpxMAX=123-EDITpxNum;

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
   			
//			Bdisp_AllClr_VRAM();
//			ML_clear_vram(); 
			if ( ( dumpflg!=4 ) && ( mini ) ) {
				MiniCursorflag=0;		// mini cursor initialize
				MiniCursorFlashing();
			}

			switch (dumpflg) {
				case 2: 		// Opcode listing
						if ( SearchMode ) {
							ofstYptr=OpcodeLineYptr( offset_y, SrcBase, offset, &x);
							if ( csrPtr > ofstYptr ) {
								offset=csrPtr; offset_y=0;
								PrevLine( SrcBase, &offset);
								PrevLinePhyN( ymax, SrcBase, &offset, &offset_y );
							}
						}
						if ( DumpOpcode( SrcBase, &offset, &offset_y, csrPtr, &pcx, &cy, ClipStartPtr, ClipEndPtr) ) csrPtr=0; //return KEY_CTRL_EXIT;
//						Bdisp_AreaReverseVRAM(127, 8-8*EditTopLine, 127,55);		// reverse thumb line 
						d = SrcEndPtr( filebase );					// Csr thumb point display
						if ( d ) {									//
							y = csrPtr*(46+8*EditTopLine)/d+8-8*EditTopLine;						//
							BdispSetPointVRAM2( 127, y  , 2);		//
							BdispSetPointVRAM2( 127, y+1, 2);		//
						}											//

						CB_Help( help_code, cy>(ymax/2+1) );	// help display
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
			strncpy(buffer2,(const char*)ProgfileAdrs[ProgNo]+0x3C,8);
			buffer2[8]='\0'; 
			if (dumpflg==2) {
				if ( DebugMode >=1 ) { i=CB_INT;        j=MatBase; }
					else             { i=CB_INTDefault; j=MatBaseDefault; }
					sprintf(buffer, "==%-8s==[%s%d]  ", buffer2, CBmode[i], j);
			} else 	{
					sprintf(buffer, "==%-8s==%08X ", buffer2, SrcBase);
			}
			if ( (dumpflg!=2) || (ymin==2) ) {
					locate (1,1); Print(    (unsigned char*)buffer );
				if ( DebugMode >=1 ) // debug mode
					Bdisp_AreaReverseVRAM(0, 0, 127,7);	// reverse top line 
			}
		}
		
		if ( DebugScreen ) {	// screen debug mode
//				Cursor_SetFlashMode(0); 		// cursor flashing off
				RestoreScreenModeEdit();
		}
		
		if ( ( DebugScreen != 1 ) && ( run != 1 ) ) {
			locate(1,8); PrintLine((unsigned char*)" ",21);
			if ( SearchMode ) {
					FkeyClearAll();
					Fkey_Icon( FKeyNo1, 165 );	//	Fkey_dispR( FKeyNo1, "SRC ");
					Fkey_Icon( FKeyNo6,  276 );	//	Fkey_dispN( FKeyNo6, "RETRY");
					switch ( SearchMode ) {
						case 1:
							break;
						case 2:
							Fkey_dispN( FKeyNo2, "REPL");
							Fkey_dispN( FKeyNo4, "ALL");
							Fkey_dispN( FKeyNo5, "ALL+");
							break;
						case 3:
						case 4:
							FkeyClearAll();
							locate (11,8); Print((unsigned char*)"[AC]:Stop" );	//  replace all mode
							break;
					}
			} else {
				if ( ClipStartPtr>=0 ) {
					Fkey_Icon( FKeyNo1,  52 );	//	Fkey_dispN( FKeyNo1, "COPY ");
					Fkey_Icon( FKeyNo2, 105 );	//	Fkey_dispN( FKeyNo2, "CUT ");
					Fkey_Icon( FKeyNo3,   9 );	//	Fkey_dispN( FKeyNo2, "DEL ");
					Fkey_Icon( FKeyNo4, 165 );	//	Fkey_dispN( FKeyNo1, "SRC ");
					Fkey_dispN( FKeyNo5, "add'");
					Fkey_dispN( FKeyNo6, "del'");
				} else {
					ClipEndPtr   = -1 ;		// ClipMode cancel
					if ( DebugMode >=1 ) {  // debug mode
						switch ( DebugMenuSw ) {
							case 1:
								Fkey_dispN( FKeyNo1, "CONT");
								Fkey_Icon( FKeyNo2, 558 );	//	Fkey_dispN( FKeyNo2, "TRCE");
								Fkey_dispN( FKeyNo3, "STEP");
								Fkey_dispN( FKeyNo4, "S.OT");
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
				Fkey_dispN( FKeyNo4, "VWin");
				sprintf(buffer, "%d",  ProgfileMax[ProgNo]-SrcSize(filebase) );
				Fkey_dispN( FKeyNo5, buffer);
				Fkey_Icon( FKeyNo6, 563 );	//	Fkey_dispN( FKeyNo6, "G<>T");
			}
		}
		if ( ( mini == 2 ) || ( mini == 4 ) )  ML_rectangle( 0, 0, 127, 55, 0, 0, 2);	// reverse
//		Bdisp_PutDisp_DD();

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
					locate((pcx-1+EDITpxNum)/6+1,cy); 
			} else	locate(cx,cy);
			
			if  ( (dumpflg==2) || (dumpflg==4) ) {
				Cursor_SetFlashMode(1);			// cursor flashing on
				if (Cursor_GetFlashStyle()<0x6) {
					if ( ClipStartPtr>=0 ) 	Cursor_SetFlashOn(0x05);	// ClipMode cursor
				} else { 
					if ( ClipStartPtr>=0 )	Cursor_SetFlashOn(0x0B);	// ClipMode cursor
				}
				if ( mini && (dumpflg!=4) ) {
					Cursor_SetFlashMode(0); 			// cursor flashing off
					MiniCursorX=pcx-1+EDITpxNum;
					MiniCursorY=(cy-1)*6+2;
					MiniCursorSetFlashMode( 1 );		// mini cursor flashing on
					if ( DebugScreen ) MiniCursorSetFlashMode( 0 );		// mini cursor flashing off
				} else {
					CursorStyle=Cursor_GetFlashStyle();
					if ( ( CursorStyle==0x3 ) && lowercase != 0 ) Cursor_SetFlashOn(0x4);		// lowercase  cursor
					if ( ( CursorStyle==0x4 ) && lowercase == 0 ) Cursor_SetFlashOn(0x3);		// upperrcase cursor
					if ( ( CursorStyle==0x9 ) && lowercase != 0 ) Cursor_SetFlashOn(0xA);		// lowercase  cursor
					if ( ( CursorStyle==0xA ) && lowercase == 0 ) Cursor_SetFlashOn(0x9);		// upperrcase cursor
					Cursor_SetFlashMode(1);			// cursor flashing on
					if ( DebugScreen ) Cursor_SetFlashMode(0);			// cursor flashing off
				}
				if (key < 0x7F00) {
					if ( ContinuousSelect ) goto F5Continue;
					else goto gkeyj;
				}
			} else { 
			  gkeyj:
				MiniCursorflag=0;
				GetKey_DisableMenu(&key); 
			}
		}						
		
		if ( lowercase  && ( 'A' <= key  ) && ( key <= 'Z' ) ) key+=('a'-'A');
		MiniCursorSetFlashMode( 0 );		// mini cursor flashing off
//		Cursor_SetFlashMode(0); 			// cursor flashing off
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
			if ( KeyCheckDEL() ) {
				key=30045;		// KEY_CTRL_UNDO:
				alphastatus = 0;
				alphalock = 0 ; 
			} else 
			if ( KeyCheckPMINUS() ) {
				key = '%';
			}
		}
		
		switch (key) {
			case KEY_CTRL_NOP:
					ClipStartPtr = -1 ;		// ClipMode cancel
					if ( SearchMode ) break;;
					alphalock = 0 ;
					break;
			case KEY_CTRL_ALPHA:
//					if ( alphalock ) alphalock = 0 ;
//					ClipStartPtr = -1 ;		// ClipMode cancel
//					break;
			  alphaj:
					if ( alphastatus ) { 
						alphastatus = 0;
						alphalock = 0 ; 
//						Setup_SetEntry(0x14, 0x00); 	// clear
					} else { 
//						SetAlphaStatus( alphalock, lowercase ); 
						alphastatus = 1; 
					}
					key=0;
					ClipStartPtr = -1 ;		// ClipMode cancel+
					break;
			case KEY_CTRL_EXIT:
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
							EditCopy( filebase, csrPtr, ClipStartPtr, ClipEndPtr );
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
			F1j:	alphalock   = 0;
					alphastatus = 0;
					ClipStartPtr = -1 ;		// ClipMode cancel
					break;
			case KEY_CTRL_F2:
			case 0x7563:		// ClipMode F2
					if ( SearchMode >= 2 ) {	// replace one
							i = strlenOp(searchbuf);
							csrPtr+=i;
							EditCutDel( filebase, &csrPtr, csrPtr-i, csrPtr, 1, &Undo );	// delete
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
							EditCutDel( filebase, &csrPtr, ClipStartPtr, ClipEndPtr, 0, &Undo );	// cut
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
			F2j:	goto F1j;
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
							EditCutDel( filebase, &csrPtr, ClipStartPtr, ClipEndPtr, 1, &Undo );	// delete
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
											goto PUTALPHA;
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
					} else {
						if ( SearchMode ) break;;
						if ( ClipStartPtr >= 0 ) {	// Clip -> Search for text
							if ( ClipEndPtr < 0 ) goto F4j;
							if ( ClipEndPtr < ClipStartPtr ) { i=ClipStartPtr; ClipStartPtr=ClipEndPtr; ClipEndPtr=i; }
							EditCopy( filebase, csrPtr, ClipStartPtr, ClipEndPtr );
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
										key=0;
										goto PUTALPHA;
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
						locate (11,8); Print((unsigned char*)"[AC]:Stop" );	//  replace all mode not display
						Bdisp_PutDisp_DD;
						while ( KeyScanDownAC() == 0 ) {
							i = strlenOp(searchbuf);
							csrPtr+=i;
							EditCutDel( filebase, &csrPtr, csrPtr-i, csrPtr, 1, &Undo );	// delete
							EditPaste( filebase, replacebuf, &csrPtr, &Undo );	// insert
							Undo.enable = 0;
							i = SearchOpcodeEdit( SrcBase, searchbuf, &csrPtr, 0 );
							if ( i==0 ) { SearchMode=0; break; }
						}
						while ( KeyScanDownAC() );
						KeyRecover(); 
						offset   = csrPtr;
						offset_y = 0;
						PrevLinePhyN( ymax, SrcBase, &offset, &offset_y );	// csrY adjust
						UpdateLineNum=1;
					} else {
						if ( SearchMode ) break;;
						if ( ClipStartPtr >= 0 ) {	// Clip -> add '
							if ( ClipEndPtr < ClipStartPtr ) { i=ClipStartPtr; ClipStartPtr=ClipEndPtr; ClipEndPtr=i; }
							csrPtr = AddComment( filebase, csrPtr, ClipStartPtr, ClipEndPtr );
//							offset=csrPtr;
//							offset_y=0;
//							PrevLinePhyN( cy, SrcBase, &offset, &offset_y );		// csrY adjust
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
										goto PUTALPHA;
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
//						PrevLinePhyN( cy, SrcBase, &offset, &offset_y );		// csrY adjust
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
									if ( mini ) GetKey_DisableMenu(&key); else GetKey(&key);
								} else { GetGenuineCmdF6( &key ); goto F6j; }
							} else {
								if ( JumpMenuSw ) {		// ====== Jump Mode
									NextLinePhyN( PageUpDownNum*ymax, SrcBase, &offset, &offset_y );	// Skip Down
									csrPtr=OpcodeLinePtr( offset_y, SrcBase, offset);
									if ( SrcBase[offset] == 0 ) PrevLinePhyN( ymax, SrcBase, &offset, &offset_y );
								} else {
								  directrun:
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
										CB_GetAliasLocalProg( SrcBase );	//	Preprocess Alias/Local
										if ( ErrorNo ) { 
											ProgNo=ErrorProg; 
											stat=1;
										} else {
											ProgNo=0;
											ExecPtr=0;
											stat=CB_interpreter( SrcBase ) ;	// ====== run 1st interpreter ======
											if ( ( stat==-7 ) && ( ProgEntryN == 0 ) ) DebugMode=0;
										}
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
											PrevLinePhyN( ymax, SrcBase, &offset, &offset_y );
											if ( stat == -1 ) {
											  finish:
												cont=0;	// program finish
												ExecPtr=execptr; 
											}
										}
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
				switch ( dumpflg ) {
					case 2: 		// Opcode
							PrevOpcode( SrcBase, &csrPtr );
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
							d = GetOpcode( SrcBase, ptr );
							ptr=csrPtr;
							if ( ( d!=0x0C ) && ( d!=0x0D ) ) PrevLine( SrcBase, &ptr );	// current line top
							y=0; ptr2=ptr;
							OpcodeLineN( SrcBase, &ptr2 , &x, &y);
							csrPtr_y = OpcodeLineSrcYpos( SrcBase, ptr, csrPtr ) ;	//
							if ( ( csrPtr_y == 0 ) ) {
								PrevLine( SrcBase, &ptr );	// prev line top
								y=0;
								OpcodeLineN( SrcBase, &ptr , &x, &y);
								i=x; x=pcx;
							} else {
								x = OpcodeLineSrcXendpos( csrPtr_y-1, SrcBase, ptr );
								i=x; x=pcx;
							}
							while ( 1 ) {
								PrevOpcode( SrcBase, &csrPtr );
								x -= OpcodeStrLenBufpx( SrcBase, csrPtr);
								if ( x < 1 ) {
									 x=i; break ; }
							}
							while ( x > pcx ) {
								PrevOpcode( SrcBase, &csrPtr );
								x -= OpcodeStrLenBufpx( SrcBase, csrPtr);
								if ( x < 1 ) {
									 NextOpcode( SrcBase, &csrPtr ); break ; }
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
								x=1;
								d=SrcBase[csrPtr+1];
								if ( ( d==0x0C ) || ( d==0x0D ) ) { csrPtr++; goto DOWNexit; }
							} else {
								x=pcx;
								while ( 1 ) {
									x += OpcodeStrLenBufpx( SrcBase, csrPtr);
									if ( x > EDITpxMAX ) { x=1; NextOpcode( SrcBase, &csrPtr ); break ; }
									NextOpcode( SrcBase, &csrPtr );
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
								NextOpcode( SrcBase, &csrPtr );
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
				key=0;
				if ( ClipStartPtr>=0 ) ClipEndPtr=csrPtr;
				SearchMode=0;
				break;
				
			case KEY_CTRL_SHIFT:
				if ( ClipStartPtr > 0 ) {  ClipStartPtr = -1 ; 	// ClipMode cancel
					if ( mini == 0 ) break;
					DumpOpcode( SrcBase, &offset, &offset_y, csrPtr, &pcx, &cy, ClipStartPtr, ClipEndPtr);
//					locate((pcx-1+EDITpxNum)/6+1,cy); 
				 }
				if ( SearchMode ) break;;
				ShiftF6loop:
				DebugScreen = 0;
				alphastatus_bk = alphastatus;
				alphastatus = 0; 
				alphalock_bk   = alphalock;
				alphalock   = 0;
				if ( CommandInputMethod ) { 
					Menu_SHIFT_MENU();
				} else {
//					FkeyClear( FKeyNo4 );
					sprintf(buffer, "%d",  ProgfileMax[ProgNo]-SrcSize(filebase) );
					Fkey_dispN( FKeyNo4, buffer);
				}
				Fkey_Icon( FKeyNo1, 877 );	//	Fkey_dispN( FKeyNo1, "Var");
				Fkey_Icon( FKeyNo2, 286 );	//	Fkey_dispN( FKeyNo2, "Mat");
				Fkey_Icon( FKeyNo3, 560 );	//	Fkey_dispR( FKeyNo3, "VWIN");
				if ( lowercase  ) Fkey_dispN_aA( FKeyNo5, "A<>a"); else Fkey_dispN_Aa( FKeyNo5, "A<>a");
				if ( ( ( 1 <= DebugMode ) && ( DebugMode <=3 ) ) || ( CommandInputMethod==0 ) ) {		// ====== Debug Mode ======
					Fkey_Icon( FKeyNo6, 563 );	//	Fkey_dispN( FKeyNo6, "G<>T");
				} else {
					Fkey_Icon( FKeyNo6, 673 );	//	Fkey_dispR( FKeyNo5, "CHAR");
				}

				if (dumpflg==2) {
						j=ProgfileMax[ProgNo]-SrcSize(filebase) ;
						sprintf(buffer, "==%-8s==[%dfree]    ", buffer2, j);
				} else 	{
						sprintf(buffer, "==%-8s==%08X ", buffer2, SrcBase);
				}
				if ( (dumpflg!=2) || (ymin==2) ) {
						locate (1,1); Print(    (unsigned char*)buffer );
					if ( DebugMode >=1 ) // debug mode
						Bdisp_AreaReverseVRAM(0, 0, 127,7);	// reverse top line 
				}
				if ( dumpflg == 2 ) {
						locate((pcx-1+EDITpxNum)/6+1,cy); 
				} else	locate(cx,cy);

				GetKey_DisableMenu(&key);
				if ( key==0 ) if ( KeyCheckCHAR6() ) key=KEY_CHAR_6;
				if ( key==0 ) if ( KeyCheckCHAR3() ) key=KEY_CHAR_3;
				KeyRecover();
				MiniCursorSetFlashMode( 0 );		// mini cursor flashing off
				switch (key) {
//					case KEY_CTRL_EXIT:
					case KEY_CTRL_QUIT:
//							key = KEY_CTRL_EXIT;
//							ClipStartPtr = -1 ;		// ClipMode cancel
							CommandType=0; CommandPage=0;
							goto exitjp;
							break;
							
					case KEY_CTRL_ALPHA:
							alphalock = 1 ;
							alphastatus = 1;
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
									cx=6; cy=2;
									offset=csrPtr;
									offset_y=0;
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
					case KEY_CTRL_F5:	//
							lowercase=1-lowercase;
							if  ( alphalock_bk ) { 
								alphalock = 1; PutKey( KEY_CTRL_SHIFT, 1 ); GetKey(&key); PutKey( KEY_CTRL_ALPHA, 1 ); GetKey(&key);
							} else {
								if ( ( mini ) && ( alphastatus_bk ) ) {
									PutKey( KEY_CTRL_ALPHA, 1 ); GetKey(&key);
								}
							}
							key=0;
							goto PUTALPHA;
							break;
					case KEY_CTRL_F6:	// G<>T
							if ( ( ( 1 <= DebugMode ) && ( DebugMode <=3 ) ) || ( CommandInputMethod==0 ) ) {		// ====== Debug Mode ======
							  chgGT:
								Cursor_SetFlashMode(0); 			// cursor flashing off
								ScreenModeEdit=1-ScreenModeEdit;
								RestoreScreenModeEdit();
								if ( mini ) GetKey_DisableMenu(&key); else GetKey(&key);
								if ( key == KEY_CTRL_SHIFT) goto ShiftF6loop;
							} else {
								key=SelectChar( &ContinuousSelect);
								goto PUTALPHA;
							}
							break;
							
					case KEY_CTRL_CLIP:
							ClipStartPtr=csrPtr;
							key=0;
							break;
					case KEY_CTRL_PASTE:
							if ( ClipBuffer != NULL ) {
								EditPaste( filebase, ClipBuffer, &csrPtr, &Undo );
								x=1; y=0; ptr=0;
								OpcodeLineN( ClipBuffer, &ptr, &x, &y );
								if ( ( y >= (ymaxpos-cy) ) || ( y >= ymax ) ) {
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
									goto PUTALPHA;
								}
							}
							ClipStartPtr = -1 ;		// ClipMode cancel
							break;
					case KEY_CTRL_INS:
							if ( mini ) {
//								CursorStyle=Cursor_GetFlashStyle();
								if ( CursorStyle >= 0x6 )	CursorStyle-=0x6;		//  -> insert mode
								else 						CursorStyle+=0x6;		//  -> overwrite mode
							}
							break;
					case KEY_CHAR_3:
							PopUpWin(1);
							locate(3,4); Print((unsigned char*)"Hit Getkey Code");
							KeyRecover();
							GetKey_DisableMenu(&key);
							MSGpop();
							sprintf(buffer,"%d",CB_KeyCodeCnvt( key ) );
							EditPaste( filebase, buffer, &csrPtr, &Undo );
							key=0;
							break;
					default:
						break;
				}
				break;
				
			case KEY_CTRL_DEL:
				if ( ClipStartPtr >= 0 ) {
					goto F3del;
				} else {
					if ( CursorStyle < 0x6 ) {		// insert mode
						PrevOpcode( SrcBase, &csrPtr );
					}
					DeleteOpcodeUndo( filebase, &csrPtr, &Undo);
					key=0;
				}
				SearchMode=0;
				ClipStartPtr = -1 ;		// ClipMode cancel
				break;
			case KEY_CTRL_OPTN:
				if ( SearchMode ) break;;
				if ( dumpflg==2 ) {
					if ( CommandInputMethod ) { 
						CommandType=CMD_OPTN; CommandPage=0;
					} else {
						key=SelectOpcode( CMDLIST_OPTN, 0 );
						goto PUTALPHA;
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
					  PUTALPHA:
						if ( alphalock == 0 ) {
							if ( mini ) {
								if ( alphastatus ) PutKey( KEY_CTRL_ALPHA, 1 );
							} else PutAlphamode1(CursorStyle);
						}
					}
				}
				ClipStartPtr = -1 ;		// ClipMode cancel
				break;
			case KEY_CTRL_MENU:
				if ( SearchMode ) break;;
				if ( dumpflg==2 ) {
					key=SelectOpcodeRecent( CMDLIST_RECENT );
				}
				ClipStartPtr = -1 ;		// ClipMode cancel
				break;

//		case KEY_CHAR_POW:
			case 30045:		// KEY_CTRL_UNDO:
				ProgUndo( filebase, &csrPtr, &Undo );
				key=0;
				ClipStartPtr = -1 ;		// ClipMode cancel
				break;

			default:
				break;
		}


		if ( key == 0x1F91B ) { 
			CB_StoreString( 1, ClipBuffer );
			key=0;
		}
		if ( key == 0x2F91B ) { 
			string = CB_RecallString( 1 );
			goto pastestring;
		}
		if ( key == 0x4F91B ) { 
			string = CB_SeeString( 1, &selectStr, ClipBuffer );
		  pastestring:
			if ( string != NULL ) {
				EditPaste( filebase, string, &csrPtr, &Undo );
				UpdateLineNum=1;
			}
			key=0;
		}
		if ( dumpflg==2 ) {
			keyH=(key&0xFF00) >>8 ;
			keyL=(key&0x00FF) ;
			switch ( keyH ) {		// ----- 2byte code -----
				case 0xFFFFFFF7:		// 
					if ( ( keyL==0x02 ) || ( keyL==0x0F ) || ( keyL==0x03 ) ||	// Else/ElseIf
						 ( keyL==0x07 ) || ( keyL==0x09 ) || ( keyL==0x0B ) || ( keyL==0xFFFFFFED )  ) {	// Next/WhileEnd/LpWhile/SwitchEnd
						i=0;
						while ( i < (CB_EditIndent&0x07) ) {
							PrevOpcode( SrcBase, &csrPtr );
							c=GetOpcode( SrcBase, csrPtr );
							if ( c==' ' ) DeleteOpcode( filebase, &csrPtr);
							else NextOpcode( SrcBase, &csrPtr );
							i++;
						}
					}
				case 0x7F:		// 
				case 0xFFFFFFF9:		// 
				case 0xFFFFFFE5:		// 
				case 0xFFFFFFE6:		// 
				case 0xFFFFFFE7:		// 
				case 0xFFFFFFFF:	// 
					if (ClipStartPtr>=0) { 
						ClipStartPtr = -1 ;		// ClipMode cancel			
						break;
					}
					if ( CursorStyle < 0x6 ) {		// insert mode
						InsertOpcode( filebase, csrPtr, key );
					} else {					// overwrite mode
						if ( SrcBase[csrPtr] !=0 ) DeleteOpcode( filebase, &csrPtr);
						InsertOpcode( filebase, csrPtr, key );
					}
					if ( ErrorNo==0 ) NextOpcode( SrcBase, &csrPtr );
					alphalock = 0 ;
					alphastatus = 0;
					help_code=key;
					key=0;
					SearchMode=0;
					DebugScreen = 0;
					UpdateLineNum=1;
					Undo.enable = 0;
					break;
				default:
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
						PrevOpcode( SrcBase, &ptr );
						d = GetOpcode( SrcBase, ptr );
						ptr=csrPtr;
						if ( ( d!=0x0C ) && ( d!=0x0D ) ) PrevLine( SrcBase, &ptr );	// current line top
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
					if ( key == KEY_CTRL_XTT  )   key='X'; // 
					
					help_code=key;
					do {
						if ( ( CursorStyle < 0x6 ) || ( indent ) ) {	// insert mode
								InsertOpcode( filebase, csrPtr, key );
						} else {										// overwrite mode
								if ( SrcBase[csrPtr] !=0 ) DeleteOpcode( filebase, &csrPtr);
								InsertOpcode( filebase, csrPtr, key );
						}
						if ( ErrorNo==0 ) NextOpcode( SrcBase, &csrPtr );
						indent--;
						key=' ';
					} while ( indent >= 0 ) ;
					
					if ( alphalock == 0 ) alphastatus = 0;
					key=0;
					SearchMode=0;
					DebugScreen = 0;
					UpdateLineNum=1;
					Undo.enable = 0;
					if ( help_code == 0x0D ) {
						if ( alphalock ) {
							PutKey( KEY_CTRL_SHIFT, 1 );
							PutKey( KEY_CTRL_ALPHA, 1 );
							alphalock   = 0;
							alphastatus = 0;
						}
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
	Cursor_SetFlashMode(0); 			// cursor flashing off
	MiniCursorSetFlashMode( 0 );		// mini cursor flashing off
	Bkey_Set_RepeatTime(FirstCount,NextCount);		// restore repeat time
//	RestoreScreenModeEdit();
	return key;
}

//----------------------------------------------------------------------------------------------

int CB_BreakStop() {
	unsigned int key=1;
	char buf[22];
	int r=0;
	int scrmode=ScreenMode;
	int dbgmode= ( ( DisableDebugMode == 0 ) || ( ForceDebugMode ) ) ;

	if ( BreakPtr == -7 ) return BreakPtr;	// return to main program
	if ( ErrorNo == StackERR ) { BreakPtr=-999; TryFlag=0; return BreakPtr; }	// stack error
	if ( TryFlag ) return 0;

	HiddenRAM_MatAryStore();	// MatAry ptr -> HiddenRAM
	Bdisp_PutDisp_DD();
	if ( ScreenMode ) {
		CB_SaveGraphVRAM();	// save Graphic screen
	} else {
		CB_SaveTextVRAM();	// save Graphic screen
	}
//	CB_SelectTextVRAM();	// Select Text Screen
//	CB_SelectGraphVRAM();	// Select Graphic screen
//	CB_SelectTextVRAM();	// Select Text Screen

	if ( ErrorNo ) { 
		CB_ErrMsg( ErrorNo );
		BreakPtr=-999;
		ExecPtr=ErrorPtr;
		DebugScreen = 0;
		ErrorNo=0;
		if ( dbgmode ) DebugMode=2;	// enable debug mode
	}
	
	if ( ( DebugMode < 2 ) && ( BreakPtr > -999) ) {
		PopUpWin(6);
		locate(9,3); Print((unsigned char *)"Break");
		locate(6,5); Print((unsigned char *) "Press:[EXIT]");
//		locate(6,6); Print((unsigned char *) " Cont:[EXE]");
		PrintMini(26,46,(unsigned char *)"Continue : [EXE]/[F1]",MINI_OVER);
//		Bdisp_PutDisp_DD();
		
		KeyRecover(); 
		WaitKeyAC();
		while ( 1 ) {
			GetKey(&key);
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
				DebugMode=0; BreakPtr=0; goto cont;
			}
		}
		if ( dbgmode  ) DebugMode=2;	// enable debug mode
		DebugScreen = 0;
	} else {	// Step mode
		if( BreakPtr > -999) BreakPtr=ExecPtr;	// set breakptr
	}
	
	ScreenModeEdit=scrmode;
	if ( ForceReturn ) return 1;	// force program end
	if ( dbgmode  ) key=EditRun(2);	// Program listing & edit

	if ( ( dbgmode == 0 ) ||  ( key == KEY_CTRL_EXIT ) ) { 
		if ( ProgEntryN == 0 ) DebugMode=0;
		if ( BreakPtr != -7 ) BreakPtr=-999;
		r = BreakPtr;
	}
	cont:
	KeyRecover(); 
	if ( DebugMode == 1 ) DebugMode=0;
//	CB_RestoreTextVRAM();	// Resotre Text screen
//	if ( scrmode  ) CB_SelectGraphVRAM();	// Select Graphic screen
	ScreenMode=scrmode;
	if ( ScreenMode ) {
		CB_RestoreGraphVRAM();	// Resotre Graphic screen
	} else {
		CB_RestoreTextVRAM();	// Resotre Text screen
	}
	Bdisp_PutDisp_DD_DrawBusy();
	return r;
}

//----------------------------------------------------------------------------------------------
int eObjectAlign4a( unsigned int n ){ return n; }	// align +4byte
//int eObjectAlign4b( unsigned int n ){ return n; }	// align +4byte
//int eObjectAlign4c( unsigned int n ){ return n; }	// align +4byte
//int eObjectAlign4d( unsigned int n ){ return n; }	// align +4byte
//int eObjectAlign4e( unsigned int n ){ return n; }	// align +4byte
//int eObjectAlign4f( unsigned int n ){ return n; }	// align +4byte
//----------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------
//int edeitdummy(int x, int y){
//	return x*y;
//	locate(x,y  ); Print((unsigned char *) "1234");
//	locate(x,y+1); Print((unsigned char *) "5678");
//	locate(x,y+2); Print((unsigned char *) "ABCD");
//}

