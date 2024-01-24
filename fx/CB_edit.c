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
//char ClipBuffer[ClipMax];
char *ClipBuffer;
char DebugMode=0;	// 0:disable   1:cont   2:trace   3:step over   4:step out   9:debug mode start
char ScreenModeEdit=0;
char DebugScreen=0;		// 0:no   1:Screen debug mode with fkey    2: Screen debug mode
//----------------------------------------------------------------------------------------------
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
int OpcodeLineSub( char *buffer, int *ofst , int *len) {
	char tmpbuf[18];
	int opcode = GetOpcode( buffer, *ofst );
	if ( opcode=='\0' ) return 0;
	if ( ( opcode==0x0C ) || ( opcode==0x0D ) ) return 0 ;
	(*ofst) += OpcodeLen( opcode );
	CB_OpcodeToStr( opcode, tmpbuf ) ;		// SYSCALL
	(*len) = CB_MB_ElementCount( tmpbuf ) ;	// SYSCALL
	return 1;
}
void OpcodeLineN( char *buffer, int *ofst, int *x, int *y ) {
	int i,len,xmax=21,cont=1;
	(*x)=1;
	while ( cont ) {
		if ( OpcodeLineSub( buffer, &(*ofst) , &len) == 0 ) break;
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
	int i,len,y=0,xmax=21,cont=1;
	(*x)=1;
	if ( y==n ) return ofst;
	while ( cont ) {
		if ( OpcodeLineSub( buffer, &ofst , &len) == 0 ) break;
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
	int i,len,x0,x=1,y=0,xmax=21,cont=1;
	while ( cont ) {
		if ( OpcodeLineSub( buffer, &ofst , &len) == 0 ) break;
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
	int i,len=0,x=1,y=0,xmax=21,cont=1;
	if ( ofst==csrptr ) return y;
	while ( cont ) {
		if ( OpcodeLineSub( buffer, &ofst , &len) == 0 ) break;
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
		if ( *ofst != 0 ) { 
			PrevLine( buffer, &(*ofst));
			ofst2= *ofst;
			x=1; y=0;
			OpcodeLineN( buffer, &ofst2 ,&x ,&y);
			if ( y==0 ) {
				*ofst_y = 0;
			} else {
				*ofst_y = y;
			}
		}
	} else {
		(*ofst_y)--;
	}
}

void NextLinePhyN( int n, char *SrcBase, int *offset, int *offset_y ) {
	int i;
	for ( i=0; i<n; i++ ) {
		if ( SrcBase[(*offset)] == 0 ) break;
		NextLinePhy( SrcBase, &(*offset), &(*offset_y) );
	}
	return ;
}
void PrevLinePhyN( int n, char *SrcBase, int *offset, int *offset_y ) {
	int i;
	for ( i=0; i<n; i++ ) {
		if ( ( (*offset) == 0 ) && ( (*offset_y) == 0 ) ) break;
		PrevLinePhy( SrcBase, &(*offset), &(*offset_y) );
	}
}

int PrintOpcodeLine1( int csry, int n, char *buffer, int ofst, int csrPtr, int *cx, int *cy, int ClipStartPtr, int ClipEndPtr) {
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

int DumpOpcode( char *SrcBase, int *offset, int *offset_y, int csrPtr, int *cx, int *cy, int ClipStartPtr, int ClipEndPtr){
	int i,n,x,y,ynum;
	int ofst,ofst2,ofstYptr;
	int count=100;

	*cx=0; *cy=0;

	while ( 1 ) {
		for ( y=2; y<8; y++ ) { locate(1,y); PrintLine((unsigned char*)" ",21); }
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
		count--;
		if ( count<50 ) if ( csrPtr > 0 ) csrPtr--;
		if ( count==0 ) {  // error reset
			(*offset)=0; (*offset_y)=0; (*cx)=1; (*cy)=2; return -1; 
		}
	}
	
	return 0; // ok
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
//---------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------
int SearchOpcodeEdit( char *SrcBase, char *searchstr, int *csrptr){
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

int SearchForText( char *SrcBase, char *searchstr, int *csrptr){
	unsigned int key;
	int i=0,sptr;
	int csrp;
	
	Bdisp_AllClr_DDVRAM();
	
	locate(1,1); Print((unsigned char *)"Search For Text");
	locate(1,3); Print((unsigned char*)"---------------------");
	locate(1,5); Print((unsigned char*)"---------------------");

	Bdisp_PutDisp_DD_DrawBusy();
		
	KeyRecover(); 
	do {
		key= InputStrSub( 1, 4, 21, strlenOp(searchstr), searchstr, 63, ' ', REV_OFF, FLOAT_ON, EXP_ON, ALPHA_ON, HEX_OFF, PAL_ON, EXIT_CANCEL_OFF);
	} while ( key == KEY_CTRL_AC ) ;	// AC
	if ( key == KEY_CTRL_EXIT ) return 0;	// exit

	while ( i==0 ) {	//  not found loop
		csrp=*csrptr;
		i=SearchOpcodeEdit( SrcBase, searchstr, &csrp);
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
void editmenu( char lowercase ){
	Fkey_dispR( 0, "JUMP");
	Fkey_dispR( 1, "SRC");
	Fkey_dispR( 2, "CMD");
	if ( lowercase  ) Fkey_dispN_aA(3,"A<>a"); else Fkey_dispN_Aa(3,"A<>a");
	Fkey_dispR( 4, "CHAR");
}
void jumpmenu(){
	Fkey_dispN( 0, "TOP ");
	Fkey_dispN( 1, "BTM");
	Fkey_dispR( 2, "Goto");
	Fkey_dispN_aA( 4, "Skp\xE6\x92");
	Fkey_dispN_aA( 5, "Skp\xE6\x93");
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
	
	ofst=0; ofsty=0;
	curline=0;
	while ( ( (*offset) != ofst ) || ( (*offset_y) != ofsty ) ){
		NextLinePhy( SrcBase, &ofst, &ofsty );
		curline++;
	}
	curline+=(cy-1);
	sprintf(buffer,"current:%d",curline);
	locate( 3,4); Print((unsigned char *)buffer);
	Bdisp_PutDisp_DD_DrawBusy();
	
	ofst=0; ofsty=0;
	lineAll=0;
	while ( SrcBase[ofst] ) {
		NextLinePhy( SrcBase, &ofst, &ofsty );
		lineAll++;
	}
	lineAll++;
	
	sprintf(buffer,"[1~%d]:",lineAll);
	locate( 3,5); Print((unsigned char *)buffer);
	i=strlen(buffer)+3;
	n=0;
	while (1) {
		n=InputNumD( i, 5, 5, n, ' ', REV_OFF, FLOAT_OFF, EXP_OFF);		// 0123456789
 		if ( (1<=n)&&(n<=lineAll) ) break;
 		if ( n==0 ) break;
 		if ( n < 0 ) n=1;
 		if ( n > lineAll ) n=lineAll;
 	}
	return n ; // ok
}


unsigned int EditRun(int run){		// run:1 exec      run:2 edit
	char *FileBase,*SrcBase;
	unsigned int key=0;
	char keyH,keyL;
	char cont=1,stat;
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
	char 	dumpflg=2;
	unsigned short opcode;
	char lowercase=0, CursorStyle;
	int ClipStartPtr = -1 ;
	int ClipEndPtr   = -1 ;
	char alphalock = 0 ;
	char SearchMode=0;
	int ContinuousSelect=0;
	char DebugMenuSw=0;
	char JumpMenuSw=0;
	

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

	PrevLinePhyN( 6, SrcBase, &offset, &offset_y );
	
	if ( run == 1 ) { ProgNo=0; ExecPtr=0; key=KEY_CTRL_F6; }	// direct run

	if ( DebugMode ) DebugMenuSw=1; 

//	Cursor_SetFlashMode(1);			// cursor flashing on
	if (Cursor_GetFlashStyle()<0x6)	Cursor_SetFlashOn(0x0);		// insert mode cursor 
		else 						Cursor_SetFlashOn(0x6);		// overwrite mode cursor 
		
	KeyRecover();
	while ( cont ) {
		ErrorNo=0;
		FileBase = ProgfileAdrs[ProgNo];
		SrcBase  = FileBase+0x56;
		
		if ( ( DebugScreen == 0 ) && ( run != 1 ) ) {
			Bdisp_AllClr_VRAM();
			strncpy(buffer2,(const char*)ProgfileAdrs[ProgNo]+0x3C,8);
			buffer2[8]='\0';
			if (dumpflg==2) {
				if ( DebugMode >=1 ) { i=CB_INT;        j=MatBase; }
					else             { i=CB_INTDefault; j=MatBaseDefault; }
							sprintf(buffer, "==%-8s==%s%d%s", buffer2, i ? " [INT%" : " [Dbl#", j, "]");
			} else 			sprintf(buffer, "==%-8s==%08X", buffer2, ProgfileAdrs[ProgNo]);
			
			locate (1,1); Print(    (unsigned char*)buffer );
			if ( DebugMode >=1 ) // debug mode
				Bdisp_AreaReverseVRAM(0, 0, 127,7);	// reverse top line 
				
			switch (dumpflg) {
				case 2: 		// Opcode listing
						if ( SearchMode ) {
							ofstYptr=OpcodeLineYptr( offset_y, SrcBase, offset, &x);
							if ( csrPtr > ofstYptr ) {
								offset=csrPtr; offset_y=0;
								PrevLine( SrcBase, &offset);
								PrevLinePhyN( 6, SrcBase, &offset, &offset_y );
							}
						}
						if ( DumpOpcode( SrcBase, &offset, &offset_y, csrPtr, &cx, &cy, ClipStartPtr, ClipEndPtr) ) csrPtr=0; //return KEY_CTRL_EXIT;

//						Bdisp_AreaReverseVRAM(127, 0, 127,55);		// reverse thumb line 
						d = SrcEndPtr( FileBase );					// Csr thumb point display
						if ( d ) {									//
							y = csrPtr*46/d+8;						//
							BdispSetPointVRAM2( 127, y  , 2);		//
							BdispSetPointVRAM2( 127, y+1, 2);		//
						}											//
						
						break;
				case 4: 		// hex dump
						DumpMix( SrcBase, offset );
						break;
//				case 16:		// Ascii dump
//						DumpAsc( SrcBase, offset );
//						break;
				default:
						break;
			}
		}
		
		
		if ( DebugScreen ) {	// screen debug mode
//				Cursor_SetFlashMode(0); 		// cursor flashing off
				if ( ScreenModeEdit )	CB_RestoreGraphVRAM();	// Resotre Graphic screen
				else					CB_RestoreTextVRAM();	// Resotre Text screen
		}
		
		if ( ( DebugScreen != 1 ) && ( run != 1 ) ) {
			locate(1,8); PrintLine((unsigned char*)" ",21);
			if (SearchMode ) {
					Fkey_dispN( 0, "SRC ");
			} else {
			if ( ClipStartPtr>=0 ) {
					Fkey_dispN( 0, "COPY ");
					Fkey_dispN( 1, "CUT ");
				} else {
					ClipEndPtr   = -1 ;		// ClipMode cancel
					if ( DebugMode >=1 ) {  // debug mode
						switch ( DebugMenuSw ) {
							case 1:
								Fkey_dispN( 0, "CONT");
								Fkey_dispN( 1, "Trce");
								Fkey_dispN( 2, "Step");
								Fkey_dispN( 3, "S.ot");
								Fkey_dispN( 4, "L<>S");
								Fkey_DISPN( 5," \xE6\x9E ");
								break;
							case 0:
								if ( JumpMenuSw ) {		// Jump Mode
									jumpmenu(lowercase );
								} else {
									editmenu(lowercase );
									Fkey_DISPN( 5," \xE6\x9E ");
								}
								break;
						}
					} else {	// normal mode
						if ( JumpMenuSw ) {		// Jump Mode
							jumpmenu(lowercase );
						} else {
							editmenu(lowercase );
							Fkey_dispN( 5, "EXE");
						}
					}
				}
			}
		}
		
//		Bdisp_PutDisp_DD();

		if ( run != 1 ) {
			locate(cx,cy);
			
			if  ( (dumpflg==2) || (dumpflg==4) ) {
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
				if ( DebugScreen ) Cursor_SetFlashMode(0);			// cursor flashing off
				if (key < 0x7F00) {
					if ( ContinuousSelect ) key=KEY_CTRL_F5; 
					else GetKey(&key);
				}
			} else GetKey(&key);
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
//			case 2: 		// Opcode
//			case 16:		// Ascii dump
//				break;
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
					if ( JumpMenuSw  ) { JumpMenuSw  = 0; break; }
					if ( DebugScreen ) { DebugScreen = 0; break; }
//			case KEY_CTRL_QUIT:
					cont=0;
					break;
			case KEY_CTRL_F1:
			case 0x7553:		// ClipMode F1
					if ( SearchMode ) {		//	Next Search
						i=SearchOpcodeEdit( SrcBase, searchbuf, &csrPtr);
						if ( i==0 ) SearchMode=0;
					} else {
						if ( ClipEndPtr >= 0 ) {
							if ( ClipEndPtr < ClipStartPtr ) { i=ClipStartPtr; ClipStartPtr=ClipEndPtr; ClipEndPtr=i; }
							EditCopy( FileBase, csrPtr, ClipStartPtr, ClipEndPtr );
						} else {
							if ( DebugMenuSw ) {		// ====== Debug Mode ======
								cont=0;
								ExecPtr=csrPtr;
								BreakPtr=0;
								DebugMode=1;		// cont mode
							} else {
								if ( JumpMenuSw ) {		// ====== Jump Mode
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
								} else {
									JumpMenuSw = 1;
								}
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
										PrevLinePhyN( 6, SrcBase, &offset, &offset_y ) ;
										break;
									case 4: 		// hex dump
									case 16:		// Ascii dump
										cx=6; cy=2;
										break;
									default:
										break;
								}
							} else {	// Search for text
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
							}
						}
					}
					key=0;
					alphalock = 0 ;
					ClipStartPtr = -1 ;		// ClipMode cancel
					break;
			case KEY_CTRL_F3:
					if ( SearchMode ) break;;
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
								offset=0; offset_y=0;
								NextLinePhyN( i-1, SrcBase, &offset, &offset_y );
								csrPtr=offset;
								if ( SrcBase[csrPtr]==0 ) PrevLinePhyN( 6, SrcBase, &offset, &offset_y );
							}
						} else {	// command select
							key=SelectOpcode5800P(oplistCMD,&selectCMD);
							if ( alphalock == 0 ) PutAlphamode1(CursorStyle);
						}
					}
					ClipStartPtr = -1 ;		// ClipMode cancel
					break;
			case KEY_CTRL_F4:
					if ( SearchMode ) break;;
					if ( DebugMenuSw ) {		// ====== Debug Mode ======
								cont=0;
								ExecPtr=csrPtr;
								BreakPtr=0;
								DebugMode=4;		// step out mode
					} else {
						if ( JumpMenuSw ) {		// ====== Jump Mode
						} else {
							lowercase=1-lowercase;
							if ( alphalock == 0 ) PutAlphamode1(CursorStyle);
						}
					}
					key=0;
					ClipStartPtr = -1 ;		// ClipMode cancel
					break;
			case KEY_CTRL_F5:
					if ( SearchMode ) break;;
					if ( DebugMenuSw ) {		// ====== Debug Mode ======
						if ( DebugScreen )  DebugScreen=0; else DebugScreen=1;	// swap list <---> screen
						key=0;
					} else {
						if ( JumpMenuSw ) {		// ====== Jump Mode
								PrevLinePhyN( PageUpDownNum*6, SrcBase, &offset, &offset_y );	// Skip Up
								csrPtr=offset;
						} else {
							key=SelectChar( &ContinuousSelect);
							if ( alphalock == 0 ) PutAlphamode1(CursorStyle);
						}
					}
					ClipStartPtr = -1 ;		// ClipMode cancel
					break;
			case KEY_CTRL_F6:
					if ( SearchMode ) break;;
					Cursor_SetFlashMode(0); 		// cursor flashing off
					if ( ErrorNo ) {
							offset = ErrorPtr;
							csrPtr = offset;
							offset_y=0;
							run=2; // edit mode
							if ( dumpflg == 2 ) {
								PrevLinePhyN( 6, SrcBase, &offset, &offset_y );
							}
					} else {
						if ( ( 1 <= DebugMode ) && ( DebugMode <=3 ) ) {		// ====== Debug Mode ======
							if ( DebugScreen == 1 ) { DebugMenuSw = 1; DebugScreen = 2; }
							else
							DebugMenuSw = 1-DebugMenuSw;
						} else {
							if ( JumpMenuSw ) {		// ====== Jump Mode
								NextLinePhyN( PageUpDownNum*6, SrcBase, &offset, &offset_y );	// Skip Down
								csrPtr=offset;
								if ( SrcBase[offset] == 0 ) PrevLinePhyN( 6, SrcBase, &offset, &offset_y );
							} else {
								if ( DebugMode == 9 ) { DebugMode=2; BreakPtr=-1; } else BreakPtr=0;
								ProgEntryN=1;
								MSG1("Prog Loading.....");
								CB_ProgEntry( SrcBase ) ;		// sub program search
								if ( ErrorNo ) { 
									ProgNo=ErrorProg; 
									stat=1;
								} else {
									ProgNo=0;
									ExecPtr=0;
									stat=CB_interpreter( SrcBase ) ;	// ====== run 1st interpreter ======
								}
								SaveConfig();
								FileBase = ProgfileAdrs[ProgNo];
								SrcBase  = FileBase+0x56;
								if ( stat ) {
									if ( ErrorNo ) offset = ErrorPtr ;			// error
									else if ( BreakPtr ) offset = ExecPtr ;	// break
								} else offset = 0;
								if ( stat == -1 ) offset = ExecPtr-1;	// program  no error return
								csrPtr = offset;
								offset_y=0;
								run=2; // edit mode
								if ( dumpflg == 2 ) {
									PrevLinePhyN( 6, SrcBase, &offset, &offset_y );
									if ( stat == -1 ) cont=0;	// program finish
								}
							}
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
				ShiftF6loop:
				DebugScreen = 0;
				alphalock = 0 ;
				Fkey_dispR( 0, "Var");
				Fkey_dispR( 1, "Mat");
				Fkey_dispR( 2, "V-W");
				if ( dumpflg==2 ) Fkey_dispN( 3, "Dump"); else Fkey_dispN( 3, "List");
//				Fkey_Clear( 4 );
				Fkey_dispR( 4, "SRC" );
				Fkey_dispN( 5, "G<>T");
				GetKey(&key);
				switch (key) {
//					case KEY_CTRL_EXIT:
					case KEY_CTRL_QUIT:
							key = KEY_CTRL_EXIT;
//							ClipStartPtr = -1 ;		// ClipMode cancel
							cont=0;
							break;
							
					case KEY_CTRL_PAGEUP:
							switch (dumpflg) {
								case 2: 		// Opcode
									PrevLinePhyN( 6, SrcBase, &offset, &offset_y );	// Page Up
									csrPtr=offset;
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
									NextLinePhyN( 6, SrcBase, &offset, &offset_y );	// Page Down
									csrPtr=offset;
									if ( SrcBase[offset] == 0 ) PrevLinePhyN( 6, SrcBase, &offset, &offset_y );
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
							selectSetup=SetupG(selectSetup);
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
							SetViewWindow();
							key=0;
							ClipStartPtr = -1 ;			// ClipMode cancel
							break;
					case KEY_CTRL_F4:
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
							ScreenModeEdit=1-ScreenModeEdit;
							if ( ScreenModeEdit  )  CB_RestoreGraphVRAM();	// Resotre Graphic screen
							else					CB_RestoreTextVRAM();	// Resotre Text screen
							GetKey(&key);
							if ( key == KEY_CTRL_SHIFT) goto ShiftF6loop;
							break;
					case KEY_CTRL_CLIP:
							ClipStartPtr=csrPtr;
							key=0;
							break;
					case KEY_CTRL_PASTE:
							EditPaste( FileBase, &csrPtr);
							key=0;
							break;
							
					case KEY_CTRL_ALPHA:
							alphalock = 1 ;
							break;
					case KEY_CTRL_PRGM:
							if ( dumpflg==2 ) {
								key=SelectOpcode(oplistPRGM,&selectPRGM);
								if ( alphalock == 0 ) PutAlphamode1(CursorStyle);
							}
							ClipStartPtr = -1 ;		// ClipMode cancel
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
			case KEY_CTRL_OPTN:
				if ( SearchMode ) break;;
				if ( dumpflg==2 ) {
					key=SelectOpcode(oplistOPTN,&selectOPTN);
					if ( alphalock == 0 ) PutAlphamode1(CursorStyle);
				}
				ClipStartPtr = -1 ;		// ClipMode cancel
				break;
			case KEY_CTRL_VARS:
				if ( SearchMode ) break;;
				if ( dumpflg==2 ) {
					key=SelectOpcode(oplistVARS,&selectVARS);
					if ( alphalock == 0 ) PutAlphamode1(CursorStyle);
				}
				ClipStartPtr = -1 ;		// ClipMode cancel
				break;
			default:
				break;
		}

		if ( dumpflg==2 ) {
			keyH=(key&0xFF00) >>8 ;
			keyL=(key&0x00FF) ;
			switch ( keyH ) {		// ----- 2byte code -----
				case 0x7F:		// 
				case 0xFFFFFFF7:		// 
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

	if ( BreakPtr == -9999) return BreakPtr;

	Bdisp_PutDisp_DD();
	CB_SelectTextVRAM();	// Select Text Screen
	CB_SelectGraphVRAM();	// Select Graphic screen
	CB_SelectTextVRAM();	// Select Text Screen

	if ( ErrorNo ) { 
		CB_ErrMsg( ErrorNo );
		BreakPtr=-999;
		ExecPtr=ErrorPtr;
		if ( ErrorNo == StackERR ) { BreakPtr=-9999; return BreakPtr; }	// stack error
		DebugScreen = 0;
		ErrorNo=0;
		DebugMode=2;	// enable debug mode
	}
	
	if ( ( DebugMode < 2 ) && ( BreakPtr > -999) ) {
		PopUpWin(6);
		locate(9,3); Print((unsigned char *)"Break");
		locate(6,5); Print((unsigned char *) "Press:[EXIT]");
//		locate(6,6); Print((unsigned char *) " Cont:[EXE]");
		PrintMini(26,46,"Continue : [EXE]/[F1]",MINI_OVER);
		Bdisp_PutDisp_DD();
		
		KeyRecover(); 
		while ( KeyCheckAC() ) ;
		while ( 1 ) {
			GetKey(&key);
			if ( key == KEY_CTRL_EXIT  ) break ;
			if ( key == KEY_CTRL_AC    ) break ;
			if ( key == KEY_CTRL_RIGHT ) break ;
			if ( key == KEY_CTRL_LEFT  ) break ;
			if ( ( key == KEY_CTRL_F1 ) || ( key == KEY_CTRL_EXE ) ) { DebugMode=0; BreakPtr=0; goto cont; }
		}
		DebugMode=2;	// enable debug mode
		DebugScreen = 0;
	} else {	// Step mode
		if( BreakPtr > -999) BreakPtr=ExecPtr;	// set breakptr
	}
	
	ScreenModeEdit=scrmode;
	key=EditRun(2);	// Program listing & edit

//	if ( ( key == KEY_CTRL_QUIT ) || ( key == KEY_CTRL_EXIT ) ) { 
	if ( key == KEY_CTRL_EXIT ) { 
		if ( ProgEntryN == 0 ) DebugMode=0;
		BreakPtr=-999;
		return BreakPtr;
	}
	cont:
	if ( DebugMode == 1 ) DebugMode=0;
	CB_RestoreTextVRAM();	// Resotre Text screen
	if ( scrmode  ) CB_SelectGraphVRAM();	// Select Graphic screen
	Bdisp_PutDisp_DD_DrawBusy();
	return 0;
}

//----------------------------------------------------------------------------------------------
//int eObjectAlign4( unsigned int n ){ return n; }	// align +4byte
//int eObjectAlign6a( unsigned int n ){ return n+n; }	// align +6byte
//int eObjectAlign4b( unsigned int n ){ return n; }	// align +4byte
//int eObjectAlign4c( unsigned int n ){ return n; }	// align +4byte
//----------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------
//int edeitdummy(int x, int y){
//	return x*y;
//	locate(x,y  ); Print((unsigned char *) "1234");
//	locate(x,y+1); Print((unsigned char *) "5678");
//	locate(x,y+2); Print((unsigned char *) "ABCD");
//}

