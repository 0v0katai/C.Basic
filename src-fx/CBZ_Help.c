#include "CB.h"

//---------------------------------------------------------------------------------------------
char *HelpText;

//----------------------------------------------------------------------------------------------
char * LoadHelpfile( char *buffer ) {
	int r,i;
	char filename[64];
	char folder16[21];
	char basname[16];
	int progno = g_current_prog;
	int storagemode = StorageMode;
	int progEntryN;
	int ProgEntryN_bk = ProgEntryN;

	for (i=0; i<=ProgMax; i++) {			// memory free
		if ( ProgfileAdrs[i] == NULL ) break;		// Prog
	}
	progEntryN = i;
	if ( progEntryN > ProgMax ) { CB_Error(TooManyProg); CB_ErrMsg(g_error_type); return NULL; } // Too Many Prog error
	strcpy( basname, buffer);
	Setfoldername16( folder16, basname );
	Getfolder( buffer );
	SetFullfilenameExt( filename, buffer, "g1m" ) ;		// g1m 1st reading
	StorageMode &= 1;	// Storage or SD
	r=LoadProgfile( filename, progEntryN, 0, 0 ) ;
	if ( r ) {
		SaveDisp(SAVEDISP_PAGE1);
		g_error_type=0;	// clear error
		SetFullfilenameExt( filename, buffer, "txt" ) ;	// retry 2nd text file
		r=LoadProgfile( filename, progEntryN, 256, 0 ) ;	// enable convert
		RestoreDisp(SAVEDISP_PAGE1);
	}
	ProgEntryN = ProgEntryN_bk;
	Restorefolder();
	StorageMode = storagemode;
	g_current_prog = progno;
	if ( g_error_type ) {	// Can't find
		g_error_type=0;	// clear error
		return NULL;
	}
	return ProgfileAdrs[progEntryN];
}

char * LoadHelp(){
	char *fileptr;
	int storagemode = StorageMode;
	
	fileptr = LoadHelpfile( "~~HELP1" );
	if ( fileptr != NULL ) return fileptr;
	
	StorageMode = 0;	// Storage
	fileptr = LoadHelpfile( "/~~HELP1" );
	StorageMode = storagemode;
	return fileptr;
}

//----------------------------------------------------------------------------------------------


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
	int i,len,px=1;
	int opcode;
	int  c=1,d;
	int yk=6;
	int escape=0;
	while ( 1 ) {
	  loop:
		opcode = GetOpcode( buffer, ofst );
		if ( opcode=='\0' ) break;
		ofst += OpcodeLen( opcode );
		CB_OpcodeToStr( opcode, tmpbuf ) ; // SYSCALL
		len = CB_MB_ElementCount( tmpbuf ) ;				// SYSCALL
		tmpb=(unsigned char*)&tmpbuf[0];
		i=0;
		while ( i < len ) {
			c=px; d=(*csry);
			if ( opcode==0x5C ) { 	// Backslash
				escape=1;
				goto loop;
			}
			if ( escape == 0 ) {
				if ( ( opcode==0x0C ) || ( opcode==0x0D ) ) { (*csry)+=6; break; }
			}
 			if ( px > 122 ) { (*csry)+=6; px=1; }
			if ( check ) px+=CB_PrintMiniLength( tmpb, 0 );
			else px+=CB_PrintMiniC( c+1, d, tmpb, MINI_OVER, false);
 //			Bdisp_PutDisp_DD();
			c=(char)*tmpb & 0xFF;
			if ( (c==0x7F)||(c==0xF7)||(c==0xF9)||(c==0xE5)||(c==0xE6)||(c==0xE7)||(c==0xFF) ) { tmpb++; }
			tmpb++; i++;
		}
		if ( escape==0 ) {
			if ( ( opcode==0x0C ) || ( opcode==0x0D ) ) break ;
		}
		if ( (*csry) > ymax ) break ;
		escape=0;
	}
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

	if ( ( CB_HelpOn==0 ) || ( opcode==0 ) ) return ;

//	HelpText = help0000;
	if ( HelpText == NULL ) HelpText = LoadHelp()+0x56;
	if ( HelpText == (char*)0x56 ) return ;	// NULL

	Ptr = 0;
	length = strlenOp( HelpText );

	while ( cont ) {
		c = HelpText[Ptr++];
		switch ( c ) {
			case 0x00:
				return ;
			case 0x27:	// '
				Help_Skip_cmmt( HelpText, &Ptr );
				break;
			case '$':
				op=str2hex4( HelpText ,&Ptr) ;
				if ( op != opcode ) break;
				cont = 0;
				do {
					Help_Skip_block( HelpText, &Ptr ) ;
					c = HelpText[Ptr];
				} while ( c=='$' );
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
		if ( Ptr > length ) return;
	}
	if ( op == 0 ) return ;		// not found help

	ofst = Ptr;
	n = 0;
	y = 0;
	while ( ofst < length ) {		// count line
		if ( HelpText[ofst]==0x0D ) break;
		i = y;
		ofst = PrintHelpLine( &y, 63-12, HelpText, ofst, 1 );
		n++; if ( y-i > 6 ) n+=(y-i)/6-1;
	}
	if ( n==0 ) return ;

	if ( yposflg ) {
		if ( EditTopLine  ) y=0; else y=8;
	} else {
		y=53;
	}

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

	y+=2;
	for ( i=0; i<n; i++ ) {
		if ( HelpText[Ptr]==0x0D ) break;
		Ptr = PrintHelpLine( &y, 63-8, HelpText, Ptr, 0 );
	}
	
	Bdisp_PutDisp_DD();
}


//----------------------------------------------------------------------------------------------
//	Try~Except~TryEnd
//----------------------------------------------------------------------------------------------
char TryFlag;

int Search_ExceptTryEnd( char *SRC ){
	int c;
	while (1){
		c=SRC[g_exec_ptr++];
		switch ( c ) {
			case 0x00:	// <EOF>
				g_exec_ptr--;
				return  0;
			case 0x22:	// "
				Skip_quot(SRC);
				break;
			case 0x27:	// ' rem
				Skip_rem_no_op(SRC);
				break;
			case 0xFFFFFFF7:	// 
				c=SRC[g_exec_ptr++];
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
				g_exec_ptr++;
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
	int ErrNo=g_error_type;
	if ( TryFlag > 1 ) {	// exit
		TryFlag = 0;	// disable Try~Except~TryEnd
	  exitlp:
  		r = Search_ExceptTryEnd( SRC ) ;
		if ( r == 0x39 ) return;	// TryEnd
		if ( r == 0 ) return;	// end of program
		goto exitlp;
	}
	if ( TryFlag == 0 ) { CB_Error(MissingTry); return; } //  Excpet Without Try ERR

  loop:
  	r = Search_ExceptTryEnd( SRC ) ;
	if ( r == 0 ) { CB_Error(MissingExcept); TryFlag = 0; return; } //  Try Without Except ERR
	else
	if ( r == 0x38 ) {	// Except
		g_error_type = 0;
		c=SRC[g_exec_ptr];
		if ( ( c == ':' ) || ( c == 0x0D ) ) n = 0; else  n = CB_EvalInt( SRC );
		if ( g_error_type ) { TryFlag = 0; return; }
		if ( ( 0 < n ) && ( ErrNo != n ) ) goto loop;
		if ( TryFlag == 2 ) goto loop;
		TryFlag += ErrNo;	// complete Except  TryFlag=ErrorNo+1
		ErrNo = 0;
	}
	else
	if ( r == 0x39 ) {	// TryEnd
		TryFlag = 0;	// disable Try~Except~TryEnd
		g_error_type = ErrNo;
		return;	// TryEnd
	}
	else goto loop;
}


//----------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------
int CB_GetRGB( char *SRC, int mode ){	// GetRGB/HSV/HSL() -> ListAns
	int c,d,r,g,b,m;
	int h,s,v;
	int base=MatBase;
	int errorCheck=1;
	int pipe=mode & 0xF0;
	mode &= 0x0F;
	d = CB_EvalInt( SRC );
	if ( SRC[g_exec_ptr] == ',' ) {
		c=SRC[++g_exec_ptr];
		if ( ( c == 'N' ) || ( c == 'N' ) ) { g_exec_ptr++; errorCheck=0; }
	}
	if ( SRC[g_exec_ptr] == ')' ) g_exec_ptr++;

	b = ((d&0x001F) << 3);
	g = ((d&0x07E0) >> 3);
	r = ((d&0xF800) >> 8);

	h=r; s=g; v=b;
//	if ( mode ) {
//		if ( ( errorCheck ) && ( pipe==0 ) ) {
//			if ( (d==0x003F) || (d==0x041F) || (d==0xF820) || (d==0xFC00) ) { if ( CannotSupportERROR( d ) ) return 0; } // Can not support error
//		}
//		if ( mode==1 ) rgb2hsv(r,g,b,&h,&s,&v);	// rgb->HSV
//		if ( mode==2 ) rgb2hsl(r,g,b,&h,&s,&v);	// rgb->HSL
//	}
	dspflag=4;	// List ans
	NewMatListAns( 3, 1, base, 32 );		// List Ans[3]
	m=base;
	WriteMatrix( CB_MatListAnsreg, m++, base, h ) ;	//
	WriteMatrix( CB_MatListAnsreg, m++, base, s ) ;	// 
	WriteMatrix( CB_MatListAnsreg, m  , base, v ) ;	// 
	return d;
}

int CB_RGBlistsub( char *SRC, int*r, int*g, int*b ){
	int reg;
	int m,n;
	int sizeA,sizeB;
	int ElementSize;
	int base;
	int dspflagtmp=dspflag;
	int execptr=g_exec_ptr;
	ListEvalsubTop(SRC);
	if ( dspflag <  3 )  { g_exec_ptr=execptr; return 1; } // List 1[0] etc
	if ( dspflag != 4 )  { CB_Error(ArgumentERR); return 0; } // Argument error
	reg = CB_MatListAnsreg;
	base=MatAry[reg].Base;
	m=base;
	*r = ReadMatrix( reg, m++, base ) ;
	*g = ReadMatrix( reg, m++, base ) ;
	*b = ReadMatrix( reg, m  , base ) ;
	DeleteMatListAns();
	dspflag=dspflagtmp; 
	return 4;	// List
}

unsigned short CB_RGB( char *SRC, int mode ) {	// n or (r,g,b)    return : color code	// mode 0:RGB  1:HSV 2:HSL
	int reg;
	int m,n;
	int sizeA,sizeB;
	int ElementSize;
	int base;
	int dspflagtmp=dspflag;
	int r,g,b;
	int h,s,v;
	int	c=SRC[g_exec_ptr];
	if ( c=='#' ) { 	// direct 16bit color #12345
		g_exec_ptr++;
		r = CB_EvalInt( SRC );
		if ( SRC[g_exec_ptr] == ')' ) g_exec_ptr++;
		return r;
	}
	c=CB_RGBlistsub( SRC, &r, &g, &b );
	if ( c==0 ) return 0;	// error
	if ( mode == 0 ) {	// RGB
		if ( c==4 ) goto exit;	// List
		r=CB_EvalInt( SRC );
//		if ( r<  0 ) r=  0;
//		if ( r>255 ) r=255;
		if ( SRC[g_exec_ptr] != ',' ) { CB_Error(SyntaxERR); return 0; }  // Syntax error
		g_exec_ptr++;
		g=CB_EvalInt( SRC );
//		if ( g<  0 ) g=  0;
//		if ( g>255 ) g=255;
		if ( SRC[g_exec_ptr] != ',' ) { CB_Error(SyntaxERR); return 0; }  // Syntax error
		g_exec_ptr++;
		b=CB_EvalInt( SRC );
//		if ( b<  0 ) b=  0;
//		if ( b>255 ) b=255;
	} else {		// HSV/HSL
		if ( c==4 ) { h=r; s=g; v=b; goto exithsv; }	// List
		h=CB_EvalInt( SRC );
		if ( ( h<0 ) || ( h>359 ) ) h = h%360;
		if ( SRC[g_exec_ptr] != ',' ) { CB_Error(SyntaxERR); return 0; }  // Syntax error
		g_exec_ptr++;
		s=CB_EvalInt( SRC );
		if ( s<  0 ) s=  0;
		if ( s>255 ) s=255;
		if ( SRC[g_exec_ptr] != ',' ) { CB_Error(SyntaxERR); return 0; }  // Syntax error
		g_exec_ptr++;
		v=CB_EvalInt( SRC );
		if ( v<  0 ) v=  0;
		if ( v>255 ) v=255;
//		if ( mode==1 ) 	hsv2rgb( h, s, v, &r, &g, &b);
//		else			hsl2rgb( h, s, v, &r, &g, &b);
	}
	exithsv:
  exit:
	if ( SRC[g_exec_ptr] == ')' ) g_exec_ptr++;
	return 0;

}

int CB_GetColor( char *SRC ){
	int	c=SRC[g_exec_ptr++];
	switch ( c ) {
			case 0x7F:	// 7F
				c=SRC[g_exec_ptr++];
				switch ( c ) {
					case 0x34 :				// Red
//						return 0xF800;	// Red
						break;
					case 0x35 :				// Blue
//						return 0x001F;	// Blue
						break;
					case 0x36 :				// Green
//						return 0x07E0;	// Green
						break;
					case 0x5E :				// RGB(
						return CB_RGB( SRC, 0 );
						break;
					case 0x71 :				// HSV(
						return CB_RGB( SRC, 1 );
						break;
					case 0x73 :				// HSL(
						return CB_RGB( SRC, 2 );
						break;
					default:
						g_exec_ptr-=2;
						{ CB_Error(SyntaxERR); return -1; }	// syntax error
						break;
				}
				break;
			case 0xFFFFFFF9:	// F9
				c=SRC[g_exec_ptr++];
				switch ( c ) {
					case 0xFFFFFF9B :			// Black
//						return 0x0000;	// Black
						break;
					case 0xFFFFFF9C :			// White
//						return 0xFFFF;	// White
						break;
					case 0xFFFFFF9D :			// Magenta
//						return 0xF81F;	// Magenta
						break;
					case 0xFFFFFF9E :			// Cyan
//						return 0x07FF;	// Cyan
						break;
					case 0xFFFFFF9F :			// Yellow
//						return 0xFFE0;	// Yellow
						break;
					default:
						g_exec_ptr-=2;
						{ CB_Error(SyntaxERR); return -1; }	// syntax error
						break;
				}
				break;
			case '#':
				return CB_EvalInt( SRC );
				break;
			default:
				g_exec_ptr--;
				{ CB_Error(SyntaxERR); return -1; }	// syntax error
				break;
		
	}
	return -1;
}

void CB_PlotLineColor( char *SRC ){
	CB_GetColor( SRC );
}
void CB_BackColor( char *SRC ){
	int c=SRC[g_exec_ptr];
	if ( c=='@' ) {
		g_exec_ptr++;
		CB_GetColor( SRC );
	} else {
		CB_GetColor( SRC );
	} 
}
void CB_TransparentColor( char *SRC ){
	int color;
	int c=SRC[g_exec_ptr];
	if ( ( c==':' ) || ( c==0x0D ) || ( c==0x0C ) || ( c==0 ) ) {
	} else {
		CB_GetColor( SRC );
	}
}

