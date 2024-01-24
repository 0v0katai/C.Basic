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
#include "CB_setup.h"

//----------------------------------------------------------------------------------------------
char ClipBuffer[ClipMax+1];
int Contflag=0;	// Continue mode    0:disable  1:enable
//----------------------------------------------------------------------------------------------

int SrcSize( char *src ) {
	int size ;
	if ( src[0]=='\0' ) return 0 ;
	size = (src[0x47]&0xFF)*256+(src[0x48]&0xFF)+0x4C;
	return size;
}
void SetSrcSize( char *src, int size ) {
	int sizeH,sizeL ;
	size=size-0x4C;
	sizeH = (size&0xFF00) >> 8 ;
	sizeL = (size&0x00FF) ;
	src[0x47]=sizeH;
	src[0x48]=sizeL;
}
int SrcEndPtr( char *src ) {
	return ( SrcSize( src ) - 0x56 - 1 ) ;
}	

//----------------------------------------------------------------------------------------------

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
}

void DeleteOpcode( char *filebase, int *ptr){
	int len,i;
	int opH,opL;
	int opcode;
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

int EndOfSrc( char *SRC, int ptr ) {
	char *filebase,*srcbase;
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
void EditPaste( char *filebase, int *ptr ){
	int len,i,j;
	char *srcbase;

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

void EditCopy( char *filebase, int ptr, int startp, int endp ){
	int len,i,j;
	char *srcbase;

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

void EditCut( char *filebase, int *ptr, int startp, int endp ){
	int len,i;
	char *srcbase;

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

void OpcodeLineN( char *buffer, int *ofst, int *x, int *y ) {
	char tmpbuf[18];
	int i,len,xmax=21,cont=1;
	int opcode;
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

int OpcodeLineYptr(int n, char *buffer, int ofst, int *x ) {
	char tmpbuf[18];
	int i,len,y=0,xmax=21,cont=1;
	int opcode;
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

int OpcodeLineSrcXendpos(int n, char *buffer, int ofst) {
	char tmpbuf[18];
	int i,len,x0,x=1,y=0,xmax=21,cont=1;
	int opcode;
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
int OpcodeLineSrcYpos( char *buffer, int ofst, int csrptr ) {
	char tmpbuf[18];
	int i,len=0,x=1,y=0,xmax=21,cont=1;
	int opcode;
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

void NextLinePhy( char *buffer, int *ofst, int *ofst_y ) {
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

void PrevLinePhy( char *buffer, int *ofst, int *ofst_y ) {
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

int PrintOpcodeLine1(int csry, int n, char *buffer, int ofst, int csrPtr, int *cx, int *cy, int ClipStartPtr, int ClipEndPtr) {
	char tmpbuf[18],*tmpb;
	int i,len,x=1,y=0,xmax=21,cont=1,rev;
	int opcode;
	int  c=1;
	if ( ClipEndPtr < ClipStartPtr ) { i=ClipStartPtr; ClipStartPtr=ClipEndPtr; ClipEndPtr=i; }
	if ( csry>7 ) return ofst;
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
			c=tmpb[i]&0xFF;
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

void DumpOpcode( char *SrcBase, int *offset, int *offset_y, int csrPtr, int *cx, int *cy, int ClipStartPtr, int ClipEndPtr){
	int i,n,x,y,ynum;
	int ofst,ofst2,ofstYptr;

	*cx=0; *cy=0;

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
		n=(*offset_y);
		while ( n < ynum ) {
				ofst2=ofst;
				PrintOpcodeLine1( y, n, SrcBase, ofst2, csrPtr, &(*cx), &(*cy), ClipStartPtr, ClipEndPtr);
				n++;
				y++;
				if ( y>6 ) break ;
		}
		ofst = PrintOpcodeLine1( y, n, SrcBase, ofst, csrPtr, &(*cx), &(*cy), ClipStartPtr, ClipEndPtr);
		y++;
		while ( y<8 ) {
				if ( SrcBase[ofst]==0x00 ) break ;
				ofst2=ofst;
				x=0; ynum=0;
				OpcodeLineN( SrcBase, &ofst2 ,&x ,&ynum);
				n=0;
				while ( n < ynum ) {
					ofst2=ofst;
					PrintOpcodeLine1( y, n, SrcBase, ofst2, csrPtr, &(*cx), &(*cy), ClipStartPtr, ClipEndPtr);
					n++;
					y++;
					if ( y>6 ) break ;
				}
				ofst = PrintOpcodeLine1( y, n, SrcBase, ofst, csrPtr, &(*cx), &(*cy), ClipStartPtr, ClipEndPtr);
				y++;
		}
		if ( ( (*cx)!=0 ) && ( (*cy)>1 ) && ( (*cy)<8 ) ) break ;
		ofstYptr=OpcodeLineYptr( *offset_y, SrcBase, *offset, &x);
		if ( csrPtr < ofstYptr ) PrevLinePhy( SrcBase, &(*offset), &(*offset_y) );
		else 					 NextLinePhy( SrcBase, &(*offset), &(*offset_y) );

//		if ( SrcBase[ofst]==0x00 ) break ;
	}
}


//---------------------------------------------------------------------------------------------

void DumpMix( char *SrcBase, int offset){
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

void DumpAsc( char *SrcBase, int offset){
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
//---------------------------------------------------------------------------------------------
int SearchOpcode( char *SrcBase, char *searchstr, int *csrptr){
	int csrbkup=(*csrptr);
	int sptr=0,cptr;
	int opcode=1,srccode;
	int size=SrcEndPtr(SrcBase-0x56);

	Bdisp_PutDisp_DD_DrawBusy();
	opcode =GetOpcode( SrcBase, *csrptr ) ;
	srccode=GetOpcode( searchstr, sptr ) ;
	if ( opcode == srccode ) NextOpcode( SrcBase, &(*csrptr) );
	while ( (*csrptr)<size ) {
		sptr=0;
		opcode =GetOpcode( SrcBase, *csrptr ) ;
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
				if ( (*csrptr) >= size ) { *csrptr=csrbkup; return 0; }	// No search
				if ( opcode != srccode ) break ;
			}
		}
	}
	{ *csrptr=csrbkup; return 0; }	// No search
}

int SearchForText( char *SrcBase, char *searchstr, int *csrptr){
	unsigned int key;
	int i=0,sptr;
	int csrp;
	
	Bdisp_AllClr_DDVRAM();
	
	locate(1,1); Print((unsigned char *)"Search For Text");
	locate(1,3); Print((unsigned char*)"---------------------");
	locate(1,5); Print((unsigned char*)"---------------------");

	Bdisp_PutDisp_DD();
		
	KeyRecover(); 
	do {
		key= InputStrSub( 1, 4, 21, strlenOp(searchstr), searchstr, 63, ' ', REV_OFF, FLOAT_ON, EXP_ON, ALPHA_ON, HEX_OFF, PAL_ON, EXIT_CANCEL_OFF);
	} while ( key == KEY_CTRL_AC ) ;	// AC
	if ( key == KEY_CTRL_EXIT ) return 0;	// exit

	while ( i==0 ) {	//  not found loop
		csrp=*csrptr;
		i=SearchOpcode( SrcBase, searchstr, &csrp);
		if ( i    ) break;
		if ( i==0 ) ErrorMSGstr1(" Not Found ");
		sptr=strlenOp(searchstr);
		do {
			key= InputStrSub( 1, 4, 21, sptr, searchstr, 63, ' ', REV_OFF, FLOAT_ON, EXP_ON, ALPHA_ON, HEX_OFF, PAL_ON, EXIT_CANCEL_OFF) ;
		} while ( key == KEY_CTRL_AC ) ;	// AC
		if ( key == KEY_CTRL_EXIT ) return 0;	// exit
	}
	*csrptr=csrp;
	return 1;	// ok
}

//---------------------------------------------------------------------------------------------
unsigned int EditRun(int run){		// run:1 exec      run:2 edit
	char *FileBase,*SrcBase;
	unsigned int key=0;
	int keyH,keyL;
	int cont=1,stat;
	char buffer[32];
	char buffer2[32];
	char searchbuf[64];
	unsigned char c;
	int i,j,n,d,x,y,cx,cy,ptr,ptr2,tmpkey=0;
	int 	offset=0;
	int 	offset_y=0;
	int 	ofstYptr;
	int 	csrPtr=0;
	int 	csrPtr_y=0;
	int 	dumpflg=2;
	unsigned short opcode;
	int lowercase=0, CursorStyle;
	int ClipStartPtr = -1 ;
	int ClipEndPtr   = -1 ;
	int alphalock = 0 ;
	int SearchMode=0;
	int ContinuousSelect=0;
	int scrmode=ScreenMode;

	long FirstCount;		// pointer to repeat time of first repeat
	long NextCount; 		// pointer to repeat time of second repeat

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
						if (SearchMode ) {
							ofstYptr=OpcodeLineYptr( offset_y, SrcBase, offset, &x);
							if ( csrPtr > ofstYptr ) {
								offset=csrPtr; offset_y=0;
								PrevLine( SrcBase, &offset);
								for ( i=0; i<7; i++ ) PrevLinePhy( SrcBase, &offset, &offset_y );
							}
						}
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
//			Bdisp_PutDisp_DD();


			locate(cx,cy);
			if ( (dumpflg==2) || (dumpflg==4) ) {
				Cursor_SetFlashMode(1);			// cursor flashing on
				if (Cursor_GetFlashStyle()<0x6) {
					if ( ClipStartPtr>=0 ) 	Cursor_SetFlashOn(0x05);	// ClipMode cursor
				} else { 
					if ( ClipStartPtr>=0 )	Cursor_SetFlashOn(0x0B);	// ClipMode cursor
				}
				Cursor_SetFlashMode(1);			// cursor flashing on
				CursorStyle=Cursor_GetFlashStyle();
				if ( ( CursorStyle==0x3 ) && lowercase != 0 ) Cursor_SetFlashOn(0x4);		// lowercase  cursor
				if ( ( CursorStyle==0x4 ) && lowercase == 0 ) Cursor_SetFlashOn(0x3);		// upperrcase cursor
				if ( ( CursorStyle==0x9 ) && lowercase != 0 ) Cursor_SetFlashOn(0xA);		// lowercase  cursor
				if ( ( CursorStyle==0xA ) && lowercase == 0 ) Cursor_SetFlashOn(0x9);		// upperrcase cursor
				if (key < 0x7F00)	if ( ContinuousSelect ) key=KEY_CTRL_F5; else GetKey(&key);
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
			case KEY_CTRL_QUIT:
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
							offset_y=0;
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
						offset_y=0;
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
					key=SelectChar( &ContinuousSelect);
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
						offset_y=0;
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
							if ( cy==2 ) PrevLinePhy( SrcBase, &offset, &offset_y );
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
							if ( cy==7 ) NextLinePhy( SrcBase, &offset, &offset_y );
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
				F6loop:
				alphalock = 0 ;
				Fkey_dispR( 0, "Var");
				Fkey_dispR( 1, "Mat");
				Fkey_dispR( 2, "V-W");
				Fkey_Clear( 3 );
				if ( dumpflg==2 ) Fkey_dispN( 3, "Dump"); else Fkey_dispN( 3, "List");
				Fkey_dispR( 4, "SRC" );
				Fkey_dispN( 5, "G<>T");
				GetKey(&key);
				switch (key) {
					case KEY_CTRL_EXIT:
					case KEY_CTRL_QUIT:
							key=0;
							ClipStartPtr = -1 ;		// ClipMode cancel
							break;
							
					case KEY_CTRL_PAGEUP:
							offset=0;
							offset_y=0;
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
							ClipStartPtr = -1 ;		// ClipMode cancel
							break;
					case KEY_CTRL_PAGEDOWN:
							offset=EndOfSrc( SrcBase, offset );
							csrPtr=offset;
							offset_y=0;
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
							key=0;
							ClipStartPtr = -1 ;		// ClipMode cancel
							break;

					case KEY_CTRL_SETUP:
							selectSetup=SetupG(selectSetup);
							key=0;
							ClipStartPtr = -1 ;			// ClipMode cancel
							break;
					case KEY_CTRL_F1:
						if ( SearchMode ) break;;
							selectVar=SetVar(selectVar);		// A - 
							key=0;
							ClipStartPtr = -1 ;			// ClipMode cancel
							break;
					case KEY_CTRL_F2:
						if ( SearchMode ) break;;
							selectMatrix=SetMatrix(selectMatrix);		//
							key=0;
							ClipStartPtr = -1 ;			// ClipMode cancel
							break;
					case KEY_CTRL_F3:
						if ( SearchMode ) break;;
							SetViewWindow();
							key=0;
							ClipStartPtr = -1 ;			// ClipMode cancel
							break;
					case KEY_CTRL_F4:
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
							offset_y=0;
							cx=6; cy=2;
							key=0;
							ClipStartPtr = -1 ;		// ClipMode cancel
							break;
					case KEY_CTRL_F5:	// Search for text
							if ( ClipEndPtr >= 0 ) {
								if ( ClipEndPtr < ClipStartPtr ) { i=ClipStartPtr; ClipStartPtr=ClipEndPtr; ClipEndPtr=i; }
								EditCopy( FileBase, csrPtr, ClipStartPtr, ClipEndPtr );
								ClipBuffer[63]='\0';
								i=SearchForText(  SrcBase, ClipBuffer, &csrPtr) ;
							} else {
								searchbuf[0]='\0';
								i=SearchForText(  SrcBase, searchbuf, &csrPtr) ;
							}
							if ( i ) SearchMode=1; 
							else	 SearchMode=0;
							ClipStartPtr = -1 ;			// ClipMode cancel
							break;
							
					case KEY_CTRL_F6:	// G<>T
							Cursor_SetFlashMode(0); 		// cursor flashing off
							if ( scrmode  ) CB_RestoreGraphVRAM();	// Resotre Graphic screen
							else			CB_RestoreTextVRAM();	// Resotre Text screen
							scrmode=1-scrmode;
							GetKey(&key);
							if ( key == KEY_CTRL_SHIFT) goto F6loop;
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
			if ( key == KEY_CTRL_F3 )  {
				key=SelectOpcode5800P(oplistCMD,&selectCMD);
				if ( key ) SearchMode=0;
				if ( alphalock == 0 ) PutAlphamode1(CursorStyle);
			}
			if ( key == KEY_CTRL_OPTN ) {
				key=SelectOpcode(oplistOPTN,&selectOPTN);
				if ( key ) SearchMode=0;
				if ( alphalock == 0 ) PutAlphamode1(CursorStyle);
			}
			if ( key == KEY_CTRL_VARS ) {
				key=SelectOpcode(oplistVARS,&selectVARS);
				if ( key ) SearchMode=0;
				if ( alphalock == 0 ) PutAlphamode1(CursorStyle);
			}
			if ( key == KEY_CTRL_PRGM ) {
				key=SelectOpcode(oplistPRGM,&selectPRGM);
				if ( key ) SearchMode=0;
				if ( alphalock == 0 ) PutAlphamode1(CursorStyle);
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
					SearchMode=0;
					break;
				default:
					break;
			}
			if ( key == KEY_CTRL_EXE )   key=0x0D; // <CR>
			if ( ( 0x00 < key ) && ( key < 0xFF ) || ( key == KEY_CTRL_XTT ) ) {		// ----- 1 byte code -----
				if ( ClipStartPtr >= 0 ) { 
						ClipStartPtr = -1 ;		// ClipMode cancel			
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
					SearchMode=0;
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

	if ( key == KEY_CTRL_EXIT  ) { BreakPtr=-999; return BreakPtr; }

	CB_RestoreTextVRAM();	// Resotre Text screen
	if ( scrmode  ) CB_SelectGraphVRAM();	// Select Graphic screen
//	Bdisp_PutDisp_DD();
	Bdisp_PutDisp_DD_DrawBusy();
	return 0;
}

