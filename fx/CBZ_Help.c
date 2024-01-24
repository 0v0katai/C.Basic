#include "CB.h"

//---------------------------------------------------------------------------------------------
char *HelpText;

extern const char root[][5];

//----------------------------------------------------------------------------------------------
char * LoadHelpfile( char *buffer ) {
	int r;
	char filename[32];
	char folder16[21];
	char basname[16];
	int progno = ProgNo;
	int storagemode = StorageMode;

	ProgEntryN++;
	if ( ProgEntryN > ProgMax ) { CB_Error(TooManyProgERR); CB_ErrMsg(ErrorNo); return NULL; } // Too Many Prog error
	strcpy( basname, buffer);
	Setfoldername16( folder16, basname );
	Getfolder( buffer );
	SetFullfilenameExt( filename, buffer, "g1m" ) ;		// g1m 1st reading
	StorageMode &= 1;	// Storage or SD
	r=LoadProgfile( filename, ProgEntryN, 0, 0 ) ;
	if ( r ) {
		SaveDisp(SAVEDISP_PAGE1);
		ErrorNo=0;	// clear error
		SetFullfilenameExt( filename, buffer, "txt" ) ;	// retry 2nd text file
		r=LoadProgfile( filename, ProgEntryN, 256, 0 ) ;	// enable convert
		RestoreDisp(SAVEDISP_PAGE1);
	}
	Restorefolder();
	StorageMode = storagemode;
	ProgNo = progno;
	if ( ErrorNo ) {	// Can't find
		ErrorNo=0;	// clear error
		return NULL;
	}
	return ProgfileAdrs[ProgEntryN];
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
			else px+=CB_PrintMiniC( c+1, d, tmpb, MINI_OVER ) ;
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
	if ( HelpText == 0x56 ) return ;	// NULL

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

