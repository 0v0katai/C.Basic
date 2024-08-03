extern "C" {

#include "prizm.h"
#include "CBP.h"

//---------------------------------------------------------------------------------------------
char *HelpText;

extern const char root[][5];

//----------------------------------------------------------------------------------------------
char * LoadHelpfile( char *buffer ) {
	int r,i;
	char filename[32];
	char folder16[21];
	char basname[16];
	int progno = ProgNo;
	int storagemode = StorageMode;
	char ext[4];
	int progEntryN;
	int ProgEntryN_bk = ProgEntryN;

	for (i=0; i<=ProgMax; i++) {			// memory free
		if ( ProgfileAdrs[i] == NULL ) break;		// Prog
	}
	progEntryN = i;
	if ( progEntryN > ProgMax ) { CB_Error(TooManyProgERR); CB_ErrMsg(ErrorNo); return NULL; } // Too Many Prog error
	strcpy( basname, buffer);
	Setfoldername16( folder16, basname );
	Getfolder( buffer );
	strcpy( ext, extG1MorG3M[CB_G1MorG3M] );
	SetFullfilenameExt( filename, buffer, ext ) ;		// g1m/g3m 1st reading
	StorageMode &= 1;	// Storage or SD
	r=LoadProgfile( filename, progEntryN, 0, 0 ) ;
	if ( r ) {
		SaveDisp(SAVEDISP_PAGE1);
		ErrorNo=0;	// clear error
		SetFullfilenameExt( filename, buffer, "txt" ) ;	// retry 2nd text file
		r=LoadProgfile( filename, progEntryN, 256, 0 ) ;	// enable convert
		RestoreDisp(SAVEDISP_PAGE1);
	}
	ProgEntryN = ProgEntryN_bk;
	Restorefolder();
	StorageMode = storagemode;
	ProgNo = progno;
	if ( ErrorNo ) {	// Can't find
		ErrorNo=0;	// clear error
		return NULL;
	}
	return ProgfileAdrs[progEntryN];
}

char * LoadHelp(){
	char *fileptr;
	int storagemode = StorageMode;
	
	if ( CB_G1MorG3M == 1 ) fileptr = LoadHelpfile( "~~HELP1" );
	else					fileptr = LoadHelpfile( "~~HELP3" );
	if ( fileptr != NULL ) return fileptr;
	
	StorageMode = 0;	// Storage
	if ( CB_G1MorG3M == 1 ) fileptr = LoadHelpfile( "/~~HELP1" );
	else					fileptr = LoadHelpfile( "/~~HELP3" );
	StorageMode = storagemode;
	return fileptr;
}

//----------------------------------------------------------------------------------------------
void Help_Skip_cmmt( char *SRC, int *Ptr ); // skip "..."

void Help_EnableDisableGB( char *SRC, int *offset ) {	// GB mode enable:GB1/GB2  disable;GB0
	int ptr=(*offset);
	int c=SRC[(*offset)];
	if ( c=='#' ) (*offset)++;
	c=SRC[(*offset)++];
	if ( ( c=='G') || ( c=='g') ){
		c=SRC[(*offset)++];
		if ( ( c=='B') || ( c=='b') ){
			c=SRC[(*offset)++];
			if ( ( c=='0' ) ) { GBcode = 0; goto exit; }	// disable GB code
			else
			if ( ( c=='1' ) ) { GBcode = 1;	goto exit; }	// enable GB code
			else
			if ( ( c=='2' ) ) { GBcode = 2;	goto exit; }	// enable GB code full
		}
	}
	(*offset)=ptr;
	return;
  exit:
	Help_Skip_cmmt( SRC, &(*offset) );
}

void Help_Skip_cmmt( char *SRC, int *Ptr ){ // skip "..."
	int c;
	while (1){
		c=SRC[(*Ptr)++];
		switch ( c ) {
			case 0x00:	// <EOF>
				(*Ptr)--;
				return;
			case 0x0D:	// <CR>
				return ;
			case '#':
				Help_EnableDisableGB( SRC, &(*Ptr) );	// GB mode enable/disable
				break;	
			case 0x5C:		//  Backslash
			case 0x7F:	// 
			case 0xFFFFFFF7:	// 
			case 0xFFFFFFF9:	// 
			case 0xFFFFFFE5:	// 
			case 0xFFFFFFE6:	// 
			case 0xFFFFFFE7:	// 
			case 0xFFFFFFFF:	// 
				(*Ptr)++;
				break;
		}
	}
}

void Help_Skip_block( char *SRC, int *Ptr ){
	int c;
	while (1){
		c=SRC[(*Ptr)++];
		switch ( c ) {
			case 0x00:	// <EOF>
				(*Ptr)--;
				return ;
			case 0x0D:	// <CR>
				return ;
				break;
			case 0x27:	// '
				Help_Skip_cmmt( SRC, &(*Ptr) );
				break;
			case 0x5C:		//  Backslash
			case 0x7F:	// 
			case 0xFFFFFFF7:	// 
			case 0xFFFFFFF9:	// 
			case 0xFFFFFFE5:	// 
			case 0xFFFFFFE6:	// 
			case 0xFFFFFFE7:	// 
			case 0xFFFFFFFF:	// 
				(*Ptr)++;
				break;
		}
	}
}

int str2hex4( char *buffer ,int *ofst) ;

int PrintHelpLine( int *csry, int ymax, char *buffer, int ofst, int check ) {	
	char tmpbuf[18];
	unsigned char buff[16];
	unsigned char *buf;
	unsigned char *tmpb;
	int i,j,k,len,px=4,y=0;
	int opcode=0x20,preopcode=0x20,preopcode2=0x20;
	int  c=1,d;
	int yk,mini,ymin,ymaxpos;
	int colortmp=CB_ColorIndex;
	unsigned short backcolortmp=CB_BackColorIndex;
	int quotflag=0;	// ':1 ":2
	int g1mg3m=CB_G1MorG3M;
	int IsGB;
	int escape=0;
//	mini=SetEditScreenSize( &i, &j, &ymaxpos, &yk );
	ymaxpos = 10;
	yk      = 17;
	
	while ( 1 ) {
	  loop:
		preopcode2 = preopcode;
		preopcode  = opcode;
		if ( ( buffer[ofst]=='#' ) && ( ( buffer[ofst+1]=='G' ) || ( buffer[ofst+1]=='g' ) ) ){ 
			Help_EnableDisableGB( buffer, &ofst );	// GB mode enable/disable
		}
//		if ( ( buffer[ofst]=='#' ) && ( ( buffer[ofst+1]=='#' ) || ( buffer[ofst+2]=='#' ) ) ){ 
//			ofst+=3;
//			GBcode=2;	// GB mode enable
//		}
//		if ( ( buffer[ofst]=='%' ) && ( ( buffer[ofst+1]=='%' ) || ( buffer[ofst+2]=='%' ) ) ){ 
//			ofst+=3;
//			GBcode=0;	// GB mode disable
//		}
		opcode = GetOpcodeGB( buffer, ofst );
		IsGB = CheckOpcodeGB( opcode );
		if ( opcode=='\0' ) break;
		if ( ( opcode==0x27 ) && ( quotflag!=1 ) && ( quotflag!=2 ) && ( GetOpcodeGB( buffer, ofst+1 ) != '/' ) && ( GetOpcodeGB( buffer, ofst+1 ) != '#' ) ) quotflag=(quotflag==0); 
		if ( ( opcode==0x22 ) && ( quotflag==2 ) && ( preopcode2 == 0x5C ) && ( preopcode == 0x5C ) ) quotflag= 3;
		if ( ( opcode==0x22 ) && ( quotflag==2 ) && ( preopcode != 0x5C ) ) quotflag= 3;
		if ( ( opcode==0x22 ) && ( quotflag==0 ) ) quotflag= 2;
		ofst += OpcodeLenGB( opcode );
		CB_OpcodeToStr( opcode, tmpbuf ) ; // SYSCALL
		len = CB_MB_ElementCount( tmpbuf ) ;				// SYSCALL
		tmpb=(unsigned char*)&tmpbuf[0];
		i=0;
		while ( i < len ) {
			if ( opcode==0x5C ) { 	// Backslash
				escape=1;
				goto loop;
			}
			if ( escape == 0 ) {
				if ( ( opcode==0x0C ) || ( opcode==0x0D ) ) { (*csry)+=18; break; }
			}
 			if ( px > 366 ) { (*csry)+=18; px=4; }
			CB_BackColorIndex=CB_ColorIndexEditBack;	// back color
			if ( check==0 ) {
				CB_ColorIndex=CB_ColorIndexEditBase; // base color
				if ( ( len > 1 ) || ( opcode==0x0C ) || ( opcode==0x0E ) || ( opcode==0x13 ) ) CB_ColorIndex=CB_ColorIndexEditCMD;	// Blue
				if ( ( ( '0'<=opcode )&&( opcode<='9' ) ) || ( opcode=='.' ) || ( opcode==0x87 )|| ( opcode==0x0F ) ) CB_ColorIndex=CB_ColorIndexEditNum; // Numeric color
				if ( quotflag==1 ) CB_ColorIndex=CB_ColorIndexEditComt;	// Green
				if ( quotflag>=2 ) CB_ColorIndex=CB_ColorIndexEditQuot;	// Magenta
				c=px; d=(*csry);
				if ( ( CB_G1MorG3M==1 ) && EditExtFont ) {
					px+=CB_PrintMiniC_fx_ed( c, d, tmpb, MINI_OR   | (0x100*EditExtFont) | 0x00000000 ) ;
				} else {
					px+=CB_PrintMiniC_ed(    c, d, tmpb, MINI_OR   | (0x100*EditExtFont) | 0x00000000  ) ;
				}
//				Bdisp_PutDisp_DD();
			} else {
					// mini
				if ( ( CB_G1MorG3M==1 ) && EditExtFont ) {
					px+=CB_PrintMiniLength_fxed( tmpb, EditExtFont );	// minifont ext font mode
				} else {
					px+=CB_PrintMiniLength( tmpb, EditExtFont );		// minifont ext font mode
				}
			}
			c=(char)*tmpb & 0xFF;
			if ( ( IsGB )||(c==0x7F)||(c==0xF7)||(c==0xF9)||(c==0xE5)||(c==0xE6)||(c==0xE7)||(c==0xFF) ) { tmpb++; }
			tmpb++; i++;
		}
		if ( quotflag==3 ) quotflag=0; 
		if ( escape==0 ) {
			if ( ( opcode==0x0C ) || ( opcode==0x0D ) ) break ;
		}
		if ( (*csry) > ymax ) break ;
		escape=0;
	}
	CB_ColorIndex=colortmp;
	CB_BackColorIndex=backcolortmp;
	CB_G1MorG3M=g1mg3m;
	return ofst;
}

void CB_Help( int opcode, int yposflg ) {
	int y;
	char *vram=(char*)GetVRAMAddress();
	int i,c,Ptr,cont=1;
	int length,op;
	char htext[4][64];
	int sptr,eptr;
	int ofst;
	int csry,n;
	int tmp;
	int colortmp=CB_ColorIndex;				// current color index
	int GBcode_bk=GBcode;

	if ( ( CB_HelpOn==0 ) || ( opcode==0 ) ) return ;

	if ( HelpText == NULL ) HelpText = LoadHelp()+0x56;
	if ( HelpText == (char *)0x56 ) return ;	// NULL

	Ptr = 0;
	length = strlenOp( HelpText );

	while ( cont ) {
		c = HelpText[Ptr++];
		switch ( c ) {
			case 0x00:
				goto exit  ;
			case 0x27:	// '
				Help_Skip_cmmt( HelpText, &Ptr );
				break;
			case '$':
				op=str2hex4( HelpText ,&Ptr) ;
				if ( op != opcode ) break;
				do {
					Help_Skip_block( HelpText, &Ptr ) ;
					c = HelpText[Ptr];
				} while ( c=='$' );
				cont = 0;
				break;
			case '#':
				Help_EnableDisableGB( HelpText, &Ptr );	// GB mode enable/disable
				break;	
			case 0x5C:		//  Backslash
			case 0x7F:	// 
			case 0xFFFFFFF7:	// 
			case 0xFFFFFFF9:	// 
			case 0xFFFFFFE5:	// 
			case 0xFFFFFFE6:	// 
			case 0xFFFFFFE7:	// 
			case 0xFFFFFFFF:	// 
				Ptr++;
				break;
		}
		if ( Ptr > length ) goto exit ;
	}
	if ( op == 0 ) goto exit ;		// not found help

	ofst = Ptr;
	n = 0;
	y = 0;
	while ( ofst < length ) {		// count line
		if ( HelpText[ofst]==0x0D ) break;
		i = y;
		ofst = PrintHelpLine( &y, 192-18, HelpText, ofst, 1 );
		n++; if ( y-i > 18 ) n+=(y-i)/18-1;
	}
	if ( n==0 ) goto exit ;

	if ( yposflg ) {
		y=0;
	} else {
		y=53;
	}

	tmp=CB_G1MorG3M;
	CB_G1MorG3M=1;
	CB_ColorIndex=0x001F;	// Blue
	CB_ColorIndex=CB_HelpFrameColorIndex;
	
	i = 6*n+2;
	if ( y==53 ) {
		ML_rectangle( 0, y,     126, y-i,   1, 1, 0);
		ML_line(      1, y+1,   127, y+1,   1);
		ML_line(    127, y+1,   127, y-i+1, 1);
		y-=i;
	} else {
		ML_rectangle( 0, y,     126, y+i,   1, 1, 0);
		ML_line(      1, y+i+1, 127, y+i+1, 1);
		ML_line(    127, y+1,   127, y+i+1, 1);
	}

	CB_ColorIndex=colortmp;				// current color index
	CB_G1MorG3M=tmp;

	y = y*3+5;
	for ( i=0; i<n; i++ ) {
		if ( HelpText[Ptr]==0x0D ) break;
		Ptr = PrintHelpLine( &y, 192-18, HelpText, Ptr, 0 );
	}
	
//	Bdisp_PutDisp_DD();
  exit:
	GBcode=GBcode_bk;
}


//----------------------------------------------------------------------------------------------
//	Try~Except~TryEnd
//----------------------------------------------------------------------------------------------
char TryFlag;

int Search_ExceptTryEnd( char *SRC ){
	int c;
	while (1){
		c=SRC[ExecPtr++];
		switch ( c ) {
			case 0x00:	// <EOF>
				ExecPtr--;
				return  0;
			case 0x22:	// "
				Skip_quot(SRC);
				break;
			case 0x27:	// ' rem
				Skip_rem_no_op(SRC);
				break;
			case 0xFFFFFFF7:	// 
				c=SRC[ExecPtr++];
				if ( c == 0x00 ) { 			// If
					Search_IfEnd(SRC);
					break;
				} else
				if ( c == 0x38 ) return  c;	// Except
				else
				if ( c == 0x39 ) return  c;	// TryEnd
				break ;
			case 0x7F:	// 
			case 0xFFFFFFF9:	// 
			case 0xFFFFFFE5:	// 
			case 0xFFFFFFE6:	// 
			case 0xFFFFFFE7:	// 
//			case 0xFFFFFFFF:	// 
				ExecPtr++;
				break;
		}
	}
	return 0;
}


void CB_Try() {
	TryFlag = 1;	// enable Try~Except~TryEnd
}
void CB_TryEnd() {
	TryFlag = 0;	// disable Try~Except~TryEnd
}

void CB_Except( char*SRC ) {
	int c;
	int n,r;
	int ErrNo=ErrorNo;
	if ( TryFlag > 1 ) {	// exit
		TryFlag = 0;	// disable Try~Except~TryEnd
	  exitlp:
  		r = Search_ExceptTryEnd( SRC ) ;
		if ( r == 0x39 ) return;	// TryEnd
		if ( r == 0 ) return;	// end of program
		goto exitlp;
	}
	if ( TryFlag == 0 ) { CB_Error(ExcpetWithoutTryERR); return; } //  Excpet Without Try ERR

  loop:
  	r = Search_ExceptTryEnd( SRC ) ;
	if ( r == 0 ) { CB_Error(TryWithoutExceptERR); return; } //  Try Without Except ERR
	else
	if ( r == 0x38 ) {	// Except
		ErrorNo = 0;
		c=SRC[ExecPtr];
		if ( ( c == ':' ) || ( c == 0x0D ) ) n = 0; else  n = CB_EvalInt( SRC );
		if ( ErrorNo ) { TryFlag = 0; return; }
		if ( ( 0 < n ) && ( ErrNo != n ) ) goto loop;
		if ( TryFlag == 2 ) goto loop;
		TryFlag += ErrNo;	// complete Except  TryFlag=ErrorNo+1
		ErrNo = 0;
	}
	else
	if ( r == 0x39 ) {	// TryEnd
		TryFlag = 0;	// disable Try~Except~TryEnd
		ErrorNo = ErrNo;
		return;	// TryEnd
	}
	else goto loop;
}


}
