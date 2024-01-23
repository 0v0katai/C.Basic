/*****************************************************************/
/*                                                               */
/*   inp Library  ver 0.95                                       */
/*                                                               */
/*   written by sentaro21                                        */
/*                                                               */
/*****************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <fxlib.h>
#include "fx_syscall.h"
#include "CB_inp.h"
#include "CB_io.h"

//----------------------------------------------------------------------------------------------
unsigned short CharMATH[]= {
	0x002B,0x002D,0x00A9,0x00B9,0x005E,0x002A,0x002F,0x003D,0x0011,0x003C,0x003E,0x0010,0x0012,0x0087,0xE5BE,0xE5BF,0xE6B0,0xE6B1,0xE5A3,
	0xE5A4,0x7F50,0xE5B0,0x000F,0xE64F,0x7F53,0xE542,0x0086,0xE551,0xE54F,0xE6BB,0xE6B7,0xE6B8,0xE6B9,0xE5C0,0xE5C1,0xE5C2,0xE5C3,0xE5C4,
	0xE5C5,0xE5C6,0xE5C7,0xE5C8,0xE5C9,0xE5CA,0xE5CB,0xE5CC,0xE5CD,0xE5CE,0xE5CF,0xE5D0,0xE5D1,0xE5D2,0xE5D3,0xE5D4,0xE5D5,0xE5D6,0xE5D7,
	0xE5D8,0xE5D9,0xE5DA,0xE5DB,0xE5DC,0xE5DD,0xE5DE,0xE5DF,0x00C2,0x00C3,0x00CB,0x00CC,0x7FC7,0x7F54,0x008C,0x009C,0x00AC,0x00BC,0xE6BD,
	0xE6BE,0xE6BF,0xE6C0,0xE6C1,0xE6C2,0xE6C3,0xE6C4,0xE6C5,0xE6C6,0xE6C7,0xE6C8,0xE6C9,0xE6CA,0xE6CB,0xE6D6,0xE6CC,0xE6CD,0xE6CE,0xE6CF,
	0xE6D0,0xE6D1,0xE6D2,0xE6D3,0xE6D4,0xE6D5,0xE6B2,0xE6B3,0xE6B4,0xE6B5,0xE6BC,0xE6B6,0xE6D7,0xE6D8,0xE6D9,0xE6DA,0xE6DB,0xE6DC,0xE6DD,
	0xE6DE,0x0000 };
	
unsigned short CharSYBL[]= {
	0x0021,0x0022,0x0023,0x0024,0x0025,0x0026,0x0027,0x0028,0x0029,0x002C,0x002E,0x003A,0x003B,0x003F,0x0040,0x005B,0x005C,0x005D,0x005F,
	0x0060,0x007B,0x007C,0x007D,0x007E,0x0013,0x00B5,0x00BB,0xE594,0xE595,0xE596,0xE597,0xE598,0xE590,0xE591,0xE592,0xE593,0xE599,0xE59A,
	0xE59B,0xE59C,0xE59D,0xE59E,0xE5A1,0xE59F,0xE5A2,0xE5A0,0xE5A5,0xE5A6,0xE5A7,0xE5B5,0xE5B6,0xE5B8,0xE5B9,0xE5BA,0xE5BB,0xE5BC,0xE690,
	0xE691,0xE692,0xE693,0xE694,0xE695,0xE696,0xE697,0xE698,0xE699,0xE69A,0xE69B,0xE69C,0xE69D,0xE69E,0xE69F,0xE6A0,0xE6A1,0xE6A2,0xE6A3,
	0xE6A4,0xE6A5,0xE6A6,0xE6A7,0xE6A8,0xE6A9,0xE6AA,0x0000 };

unsigned short CharABT[]= {
	0xE540,0xE541,0xE542,0xE543,0xE544,0xE545,0xE546,0xE547,0xE548,0xE549,0xE54A,0xE54B,0xE54C,0xE54D,0xE54E,0xE54F,0xE550,0xE551,0xE553,
	0xE554,0xE555,0xE556,0xE557,0xE558,0xE501,0xE502,0xE503,0xE504,0xE505,0xE506,0xE507,0xE508,0xE509,0xE50A,0xE50B,0xE50C,0xE50D,0xE50E,
	0xE50F,0xE510,0xE511,0xE512,0xE513,0xE514,0xE515,0xE516,0xE517,0xE518,0xE519,0xE51A,0xE51B,0xE51C,0xE51D,0xE51E,0xE520,0xE521,0xE522,
	0xE523,0xE524,0xE525,0xE526,0xE527,0xE528,0xE529,0xE52A,0xE52B,0xE52C,0xE52D,0xE52E,0xE52F,0xE530,0xE531,0xE532,0xE533,0xE534,0xE535,
	0xE560,0xE561,0xE562,0xE563,0xE564,0xE565,0xE566,0xE567,0xE568,0xE569,0xE56A,0xE56B,0xE56C,0xE56D,0xE56E,0xE56F,0xE570,0xE571,0xE572,
	0xE573,0xE574,0xE575,0xE576,0xE577,0xE578,0xE579,0xE57A,0xE57B,0xE57C,0xE57D,0xE57E,0xE580,0xE581,0xE582,0x0000 };

unsigned short Charabr[]= {
	0xE640,0xE641,0xE642,0xE643,0xE644,0xE645,0xE646,0xE647,0xE648,0xE649,0xE64A,0xE64B,0xE64C,0xE64D,0xE64E,0xE64F,0xE650,0xE651,0xE652,
	0xE653,0xE654,0xE655,0xE656,0xE657,0xE658,0xE601,0xE602,0xE603,0xE604,0xE605,0xE606,0xE607,0xE608,0xE609,0xE60A,0xE60B,0xE60C,0xE60D,
	0xE60E,0xE60F,0xE610,0xE611,0xE612,0xE613,0xE614,0xE615,0xE616,0xE617,0xE618,0xE619,0xE61A,0xE61B,0xE61C,0xE61D,0xE61E,0xE61F,0xE620,
	0xE621,0xE622,0xE623,0xE624,0xE625,0xE626,0xE627,0xE628,0xE629,0xE62A,0xE62B,0xE62C,0xE62D,0xE62E,0xE62F,0xE630,0xE631,0xE632,0xE633,
	0xE634,0xE635,0xE660,0xE661,0xE662,0xE663,0xE664,0xE665,0xE666,0xE667,0xE668,0xE669,0xE66A,0xE66B,0xE66C,0xE66D,0xE66E,0xE66F,0xE670,
	0xE671,0xE672,0xE673,0xE674,0xE675,0xE676,0xE677,0xE678,0xE679,0xE67A,0xE67B,0xE67C,0xE67D,0xE67E,0xE680,0xE681,0xE682,0x0000 };

unsigned short CharABC[]= {
	0xE741,0xE742,0xE743,0xE744,0xE745,0xE746,0xE747,0xE748,0xE749,0xE74A,0xE74B,0xE74C,0xE74D,0xE74E,0xE74F,0xE750,0xE751,0xE752,0xE753,
	0xE754,0xE755,0xE756,0xE757,0xE758,0xE759,0xE75A,0xE761,0xE762,0xE763,0xE764,0xE765,0xE766,0xE767,0xE768,0xE769,0xE76A,0xE76B,0xE76C,
	0xE76D,0xE76E,0xE76F,0xE770,0xE771,0xE772,0xE773,0xE774,0xE775,0xE776,0xE777,0xE778,0xE779,0xE77A,0x0000 };
	
unsigned short *oplist=CharMATH;
int CharPtr=0;

unsigned int SelectChar() {

	int cont=1, x,y,cx,cy,ptr,scrl=0;
	unsigned int key;
	char tmpbuf[18];
	unsigned short opcode;
	int opNum=0 ;

	SaveDisp(SAVEDISP_PAGE1);		// --------

	while (cont) {
		opNum=0;
		while ( oplist[opNum++] ) ;
		opNum-=2;
		if ( scrl == 0 ) if ( CharPtr>=114 ) scrl=1;
		if ( scrl == 1 ) if ( CharPtr<  19 ) scrl=0;
		Bdisp_AllClr_VRAM();
		locate(1,1); Print((unsigned char*)"==Character Select===");
		x=2; y=2;
		opcode=1;
		while ( opcode ) {
			ptr=(y-2+scrl)*19+x-2 ;
			opcode=oplist[ ptr ];
			OpcodeToStr( opcode, (unsigned char*)tmpbuf ) ; // SYSCALL
			locate(x,y);
			if ( CharPtr == ptr ) PrintRevC((unsigned char*)tmpbuf ); else PrintC((unsigned char*)tmpbuf );
			x++;
			if (x>20) { x=2; y++; if ( y>7 ) break ; }
//			Bdisp_PutDisp_DD();
		}
		if ( opNum>=114 ) {
			if ( scrl ) {
				locate(21,2); Print((unsigned char*)"\xE6\x92"); // 
			} else {
				locate(21,7); Print((unsigned char*)"\xE6\x93"); // 
			}
		}
		Fkey_dispN( 0, "MATH");
		Fkey_dispN( 1, "SYBL");
		Fkey_dispN( 2, "AB\xCD");
		Fkey_dispN( 3, "abr");
		Fkey_dispN( 4, "ABC");
		
		cx=(CharPtr%19)+2;
		cy=(CharPtr/19)+2-scrl;
		locate(cx,cy);

		GetKey(&key);
		switch (key) {
			case KEY_CTRL_AC:
				RestoreDisp(SAVEDISP_PAGE1);	// --------
				return 0;	// input cancel
				break;
			case KEY_CTRL_EXIT:
				RestoreDisp(SAVEDISP_PAGE1);	// --------
				return 0;	// input cancel
				break;
			case KEY_CTRL_EXE:
				cont=0;
				break;
			case KEY_CTRL_F1:	// CharMATH
				oplist=CharMATH;
				CharPtr=0;
				break;
			case KEY_CTRL_F2:	// CharSYBL
				oplist=CharSYBL;
				CharPtr=0;
				break;
			case KEY_CTRL_F3:	// CharABT
				oplist=CharABT;
				CharPtr=0;
				break;
			case KEY_CTRL_F4:	// Charabr
				oplist=Charabr;
				CharPtr=0;
				break;
			case KEY_CTRL_F5:	// CharABC
				oplist=CharABC;
				CharPtr=0;
				break;
			case KEY_CTRL_LEFT:
				CharPtr--; if ( CharPtr < 0 ) CharPtr=opNum;
				break;
			case KEY_CTRL_RIGHT:
				CharPtr++; if ( CharPtr > opNum ) CharPtr=0;
				break;
			case KEY_CTRL_UP:
				CharPtr-=19; if ( CharPtr < 0 ) CharPtr=opNum;
				break;
			case KEY_CTRL_DOWN:
				if ( CharPtr == opNum ) { CharPtr= 0; break; }
				CharPtr+=19; if ( CharPtr > opNum ) CharPtr=opNum;
				break;
			default:
				break;
		}
	}
	
	RestoreDisp(SAVEDISP_PAGE1);	// --------
	return ( oplist[ CharPtr ] );
}

//----------------------------------------------------------------------------------------------
int charX=0,charY=0;

int InputChar() {
	unsigned char
	CHAR[128]={' ', '!', '"', '#', '$', '%', '&',0x27, '(', ')', '*', '+', ',', '-', '.', '/',
	           '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', ':', ';', '<', '=', '>', '?',
	           '@', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O',
	           'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '[',0x5C, ']', '^', '_',
	           0x60,'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o',
	           'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '{', '|', '}', '~', 0xD8,
	           0x01,0x03,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0F,0x10,0x11,0x12,0x13,0xC2,0xC3,
	           0x1A,0x1B,0x1C,0x1D,0x1E,0x1F,0x86,0x87,0x8C,0x9C,0xAC,0xBC,0xB5,0xBB,0xCB,0xCC};

	int cont=1, cx,cy, C=0;
	unsigned int key;

	SaveDisp(SAVEDISP_PAGE1);		// --------
	Bdisp_AllClr_DDVRAM();

	for (cy=0; cy<8; cy++) {
		for (cx=0; cx<16; cx++) {
			locate(cx+3,cy+1); PrintC( CHAR+ cx + cy*16 );
		}
	}
	
	while (cont) {
		Cursor_SetPosition(charX+2,charY+0);
	
		GetKey(&key);
		switch (key) {
			case KEY_CTRL_AC:
				RestoreDisp(SAVEDISP_PAGE1);	// --------
				return -1;	// input cancel
				break;
			case KEY_CTRL_EXIT:
				RestoreDisp(SAVEDISP_PAGE1);	// --------
				return -1;	// input cancel
				break;
			case KEY_CTRL_EXE:
				cont=0;
				break;
			case KEY_CTRL_LEFT:
				charX-=1; if ( charX <  0 ) charX=15;
				break;
			case KEY_CTRL_RIGHT:
				charX+=1; if ( charX > 15 ) charX= 0;
				break;
			case KEY_CTRL_UP:
				charY-=1; if ( charY <  0 ) charY= 7;
				break;
			case KEY_CTRL_DOWN:
				charY+=1; if ( charY >  7 ) charY= 0;
				break;
			default:
				break;
		}
	}
	
	RestoreDisp(SAVEDISP_PAGE1);	// --------
	return ( CHAR[ charX + charY*16 ] );
}

//----------------------------------------------------------------------------------------------

int OpcodeLen( unsigned short opcode ){
	switch ( (opcode&0xFF00) >> 8 ) {
		case 0x7F:		// 
		case 0xF7:		// 
		case 0xF9:		// 
		case 0xE5:		// 
		case 0xE6:		// 
		case 0xE7:		// 
			return 2 ;
			break;
	}
	return 1 ;
}

int OpcodeStrlen(int c) {
	char tmpbuf[18];
	int len;
	unsigned short opcode;
		opcode = c & 0xFFFF ;
		OpcodeToStr( opcode, (unsigned char*)tmpbuf ) ;	// SYSCALL
		len = MB_ElementCount( tmpbuf ) ;				// SYSCALL
	return len;
}
int OpcodeStrlen1(int c) {
	char tmpbuf[18];
	int len;
	unsigned short opcode;
		opcode = c & 0x00FF ;
		OpcodeToStr( opcode, (unsigned char*)tmpbuf ) ;	// SYSCALL
		len = MB_ElementCount( tmpbuf ) ;				// SYSCALL
	return len;
}

int OpcodeStrLenBuf(unsigned char *SRC, int offset) {
		unsigned short opcode;
		unsigned int c,d;
		c=SRC[offset];
		d=SRC[offset+1];
		switch ( c ) {
				case 0x7F:		// 
				case 0xF7:		// 
				case 0xF9:		// 
				case 0xE5:		// 
				case 0xE6:		// 
				case 0xE7:		// 
						opcode= (c&0xFF)*0x100+(d&0xFF);
						break;
				default:
						opcode= (c&0xFF);
						break;
		}
		return OpcodeStrlen( opcode) ;
}

unsigned short GetOpcode( unsigned char *SRC, int ptr ){
	unsigned short c=SRC[ptr]&0xFF;
	switch ( c ) {
		case 0x00:		// <EOF>
			return 0 ;
			break;
		case 0x7F:		// 
		case 0xF7:		// 
		case 0xF9:		// 
		case 0xE5:		// 
		case 0xE6:		// 
		case 0xE7:		// 
			return (c<<8)+(SRC[ptr+1]&0xFF);
			break;
	}
	return c ;
}


int NextOpcode( unsigned char *SRC, int *offset ){
	switch ( SRC[(*offset)++] ) {
		case 0x00:		// <EOF>
			(*offset)--;
			return 0 ;
			break;
		case 0x7F:		// 
		case 0xF7:		// 
		case 0xF9:		// 
		case 0xE5:		// 
		case 0xE6:		// 
		case 0xE7:		// 
			(*offset)++;
			return 2 ;
			break;
	}
	return 1 ;
}

int NextLine( unsigned char *SRC, int *offset ){
	unsigned int c;
	int ofst=*offset,ofst2;
	int n;
	while (1) {
		 if ( NextOpcode( SRC, &(*offset) ) ==0 ) return (*offset)-ofst ;
		ofst2=*offset;
		PrevOpcode( SRC, &ofst2 );
		c=SRC[ofst2];
		switch ( c ) {
			case 0x0C:		// dsps
			case 0x0D:		// <CR>
				return (*offset)-ofst ;
				break;
		}
	}
	return (*offset)-ofst ;
}

int PrevOpcode( unsigned char *SRC, int *offset ){
	unsigned int c;
	if ( *offset <= 0 ) return 0;
		(*offset)-=2;
		c=SRC[*offset];
		switch ( c ) {
			case 0x7F:		// 
			case 0xF7:		// 
			case 0xF9:		// 
			case 0xE5:		// 
			case 0xE6:		// 
			case 0xE7:		// 
				return 2 ;
				break;
		}
	(*offset)++;
	return 1 ;
}

int PrevLine( unsigned char *SRC, int *offset ){
	unsigned int c,d;
	int ofst=*offset,ofst2;
	while (1) {
		PrevOpcode( SRC, &(*offset) );
		if ( *offset <= 0 ) { *offset=0; return ofst-(*offset) ; }
		c=SRC[*offset];
		ofst2=*offset;
		PrevOpcode( SRC, &ofst2 );
		d=SRC[ofst2];
		switch ( c ) {
			case 0x0C:		// dsps
			case 0x0D:		// <CR>
				switch ( d ) {
					case 0x0C:		// dsps
					case 0x0D:		// <CR>
						return ofst-(*offset) ;
						break;
					default:
						break;
				}
				break;
			default:
				break;
		}
		switch ( d ) {
			case 0x0C:		// dsps
			case 0x0D:		// <CR>
				return ofst-(*offset) ;
				break;
			default:
				break;
		}
	}
	return ofst-(*offset) ;
}



/*
int PrintlineOpcode(int *y, unsigned char *buffer, int ofst, int csrPtr, int *cx, int *cy, int ClipStartPtr, int ClipEndPtr) {
	char tmpbuf[18];
	int i,len,x=1,xmax=21,cont=1,rev;
	unsigned short opcode;
	unsigned char  c=1;
	if ( ClipEndPtr < ClipStartPtr ) { i=ClipStartPtr; ClipStartPtr=ClipEndPtr; ClipEndPtr=i; }
	if ( ofst > csrPtr ) { 
		ofst=csrPtr;
	}
	while ( cont ) {
		rev=0; if ( ( ClipStartPtr >= 0 ) && ( ClipStartPtr <= ofst ) && ( ofst <= ClipEndPtr ) ) rev=1;
		if (ofst==csrPtr) { *cx=x; *cy=*y; }
		opcode = GetOpcode( buffer, ofst );
		if ( opcode=='\0' ) break;
		ofst += OpcodeLen( opcode );
		OpcodeToStr( opcode, (unsigned char*)tmpbuf ) ; // SYSCALL
		len = MB_ElementCount( tmpbuf ) ;				// SYSCALL
		i=0;
		while ( i < len ) {
			locate(x,*y);
			if ( rev )
					PrintRevC( (unsigned char*)(tmpbuf+i) ) ;
			else	PrintC(    (unsigned char*)(tmpbuf+i) ) ;
//			Bdisp_PutDisp_DD();
			x++;
			if ( ( x > xmax ) || ( opcode==0x0C ) || ( opcode==0x0D ) ) {
					if ( (*y) >= 7 ) { cont=0; break; }
					(*y)++; x=1;
			}
			i++;
		}
	}
	return ofst;
}
*/
//----------------------------------------------------------------------------------------------
int PrintOpcode(int x, int y, char *buffer, int width, int rev_mode, char SPC) {
	char tmpbuf[18];
	char spcbuf[2];
	int i,j=0,len,xmax=x+width;
	unsigned short opcode;
	unsigned char  c=1;
	while ( c != '\0' ) {
		c = buffer[j++] ;
		opcode = c & 0x00FF ;
		OpcodeToStr( opcode, (unsigned char*)tmpbuf ) ;	// SYSCALL
		len = MB_ElementCount( tmpbuf ) ;				// SYSCALL
		i=0;
		while ( i < len ) {
			if ( x < xmax ) {
				locate(x,y); x++ ;
				if ( rev_mode ) PrintRevC( (unsigned char*)(tmpbuf+i) ) ;
					else        PrintC(    (unsigned char*)(tmpbuf+i) ) ;
			}
			i++ ;
		}
	}
	spcbuf[0]=SPC;
	while ( x < xmax ) {
				locate(x,y); x++;
				if ( rev_mode ) PrintRevC( (unsigned char*)spcbuf ) ;
					else        PrintC(    (unsigned char*)spcbuf ) ;
	}
	return x;
}

void OpStrToStr(char *buffer, char *buffer2) {
	char tmpbuf[18];
	int i,j,len;
	unsigned short opcode;
	unsigned char  c;
	i=0; j=0;
	c = buffer[j] ;
	while ( c != '\0' ) {
		c = buffer[j++] ;
		opcode = c & 0x00FF ;
		OpcodeToStr( opcode, (unsigned char*)tmpbuf ) ;	// SYSCALL
		strcat(buffer2,tmpbuf);
	}
}

int OpStrLastoffset(char *buffer, int ptrX, int csrwidth, int *csrX) {
	int i,j,len,csX,ofstX;
	unsigned char  c;
	csX=0;
	ofstX=ptrX;
	while ( ptrX > 0 ) {
		c = buffer[--ptrX] ;
		len = OpcodeStrlen1( c );
		csX += len ;
		ofstX--;
		if ( csX > csrwidth ) { ofstX++; csX-=len; break; }
	}
	*csrX = csX;
	return  ofstX;
}

int csrX_to_dspX(int csrX, char *buffer, int width) {
	int i,j,len;
	unsigned char  c=1;
	i=0; j=0;
	while ( j < csrX ) {
		c = buffer[j++] ;
		len = OpcodeStrlen1( c );
		if ( i>=width ) { i=width-1;  break ; }
		i += len ;
	}
	return i;
}

int OpcodeOffsetCount(char *buffer, int length, int *csrX) {
	int i,j,len;
	unsigned char  c;
	i=0; j=0;
	while ( i < length ) {
		c = buffer[j++] ;
		len = OpcodeStrlen1( c );
		i += len ;
		*csrX = *csrX - len;
	}
	return j;
}

//----------------------------------------------------------------------------------------------

int InputStrSub(int x, int y, int width, int ptrX, char* buffer, int MaxStrlen, char SPC, int rev_mode, int float_mode, int exp_mode, int alpha_mode, int hex_mode, int pallet_mode, int exit_cancel) {
	char buffer2[64];
	char buf[22];
	unsigned int key=0;
	int cont=1;
	int i,j,k;
	int csrX=ptrX;	// ptrX:  current buffer ptr      csrX: ccursor X
	int offsetX=0;	// offsetX:  buffer offset for display
	int length;		// buffer length (<MaxStrlen)
	int csrwidth;	// 
	int lowercase=0, CursorStyle;
	int key2,multibyte=0;	// 0:mono   non 0:multi
	int dspX;
	int oplen,oplenL,oplenR;

	if ( x + width > 22 ) width=22-x;
	csrwidth=width; if ( x + csrwidth > 20 ) csrwidth=21-x;
	
	length=strlen(buffer);
	for(i=0; i<=MaxStrlen; i++) buffer2[i]=buffer[i]; // backup

	offsetX=OpStrLastoffset(buffer, ptrX, csrwidth ,&csrX);

//	Cursor_SetFlashMode(1);			// cursor flashing on
	CursorStyle=Cursor_GetFlashStyle();
	if (CursorStyle<0x6)	Cursor_SetFlashOn(0x0);		// insert mode cursor 
		else 				Cursor_SetFlashOn(0x6);		// overwrite mode cursor 
//	if ( ( alpha_mode ) && ( float_mode == 0) ) {
//		if ( CursorStyle=0x0 ) Cursor_SetFlashOn(0x3);		// upperrcase cursor
//		if ( CursorStyle=0x6 ) Cursor_SetFlashOn(0x9);		// upperrcase cursor
//	} else {
//		if (CursorStyle<0x6)	Cursor_SetFlashOn(0x0);		// insert mode cursor 
//			else 				Cursor_SetFlashOn(0x6);		// overwrite mode cursor 
//	}
	Cursor_SetFlashMode(1);			// cursor flashing on
	
	while (cont) {
		multibyte=0;
		buffer[length]='\0';
		PrintOpcode(x, y, buffer+offsetX, width, rev_mode , SPC);
		if ( ( pallet_mode ) && ( alpha_mode ) ) if ( lowercase ) Fkey_dispN(4,"a<>A"); else Fkey_dispN(4,"A<>a");
		if ( ( pallet_mode ) && ( alpha_mode ) ) Fkey_dispR(5,"CHAR");

		CursorStyle=Cursor_GetFlashStyle();
		if ( ( CursorStyle==0x3 ) && lowercase != 0 ) Cursor_SetFlashOn(0x4);		// lowercase  cursor
		if ( ( CursorStyle==0x4 ) && lowercase == 0 ) Cursor_SetFlashOn(0x3);		// upperrcase cursor
		if ( ( CursorStyle==0x9 ) && lowercase != 0 ) Cursor_SetFlashOn(0xA);		// lowercase  cursor
		if ( ( CursorStyle==0xA ) && lowercase == 0 ) Cursor_SetFlashOn(0x9);		// upperrcase cursor
//		dspX = csrX_to_dspX(csrX, buffer+offsetX, csrwidth);
		Cursor_SetPosition(x+csrX-1,y-1);

//		sprintf(buf,"len=%2d ptr=%2d off=%2d   csr=%2d  ",length,ptrX,offsetX,csrX); PrintMini( 0,7*8+2,(unsigned char *)buf, MINI_OVER);

		GetKey(&key);
		switch (key) {
			case KEY_CTRL_AC:
				if ( length==0 ) cont=0;
				csrX=0; ptrX=0; offsetX=0; length=0;
				break;
			case KEY_CTRL_EXIT:
			 	if ( exit_cancel == 0 ) {
					cont=0;
					for(i=0; i<=MaxStrlen; i++) buffer[i]=buffer2[i]; // restore
				}
				break;
			case KEY_CTRL_EXE:
				cont=0;
				break;
			case KEY_CTRL_DEL:
				if ( offsetX == 0 ) {
					if ( csrX == 0 ) {	// left edge
						if ( length > 0 ) {
							for(i=ptrX; i<length; i++) buffer[i]=buffer[i+1];
							length--;
						}
					} else { // csrX>0
						if ( CursorStyle < 0x6 ) {	// insert mode
								oplen =OpcodeStrlen1( buffer[ptrX-1] );
								for(i=ptrX-1; i<length; i++) buffer[i]=buffer[i+1];
								ptrX--;
								length--;
								csrX -= oplen;
						}
						else {					// overwrite mode
							if ( ptrX == length ) { // rigth edge
								oplen =OpcodeStrlen1( buffer[ptrX-1] );
								for(i=ptrX-1; i<length; i++) buffer[i]=buffer[i+1];
								ptrX--;
								length--;
								csrX -= oplen;
							}
							else {
								for(i=ptrX  ; i<length; i++) buffer[i]=buffer[i+1];
								length--;
							}
						}
					}
				} else {	// offsetX > 0
					if ( csrX == OpcodeStrlen1( buffer[offsetX] ) ) {
						if (CursorStyle<0x6) {	// insert mode
								for(i=ptrX-1; i<length; i++) buffer[i]=buffer[i+1];
								ptrX--;
								length--;
								offsetX--;
								csrX = OpcodeStrlen1( buffer[offsetX] );
						}
						else {					// overwrite mode
							if ( ptrX == length )  {
								for(i=ptrX-1; i<length; i++) buffer[i]=buffer[i+1];
								ptrX--;
								length--;
								offsetX--;
								csrX = OpcodeStrlen1( buffer[offsetX] );
							}
							else {
								for(i=ptrX  ; i<length; i++) buffer[i]=buffer[i+1];
								length--;
							}
						}
					} else
					if ( csrX > OpcodeStrlen1( buffer[offsetX] ) ) {
						if ( CursorStyle < 0x6 ) {	// insert mode
								oplen =OpcodeStrlen1( buffer[--ptrX] );
								for(i=ptrX  ; i<length; i++) buffer[i]=buffer[i+1];
								length--;
								csrX -= oplen;
						}
						else {					// overwrite mode
							if ( ptrX == length )  {
								oplen =OpcodeStrlen1( buffer[--ptrX] );
								for(i=ptrX  ; i<length; i++) buffer[i]=buffer[i+1];
								length--;
								csrX -= oplen;
							}
							else {
								for(i=ptrX  ; i<length; i++) buffer[i]=buffer[i+1];
								length--;
							}
						}
					}
				}
				break;
			case KEY_CTRL_LEFT:
				if ( offsetX == 0 ) {
						if ( csrX > 0 ) {
							ptrX--;
							csrX-=OpcodeStrlen1(buffer[ptrX]);
						}
				}
				if ( offsetX > 0 ) {
						if ( csrX > OpcodeStrlen1(buffer[offsetX]) ) {
							ptrX--;
							csrX-=OpcodeStrlen1(buffer[ptrX]);
						} else  {
							offsetX--;
							ptrX--;
							csrX = OpcodeStrlen1(buffer[offsetX]);
						}
				}
				break;
			case KEY_CTRL_RIGHT:
					if ( ptrX >= length ) break ;
					while ( ( csrX + OpcodeStrlen1(buffer[ptrX]) ) > csrwidth ) {
						csrX -= OpcodeStrlen1(buffer[offsetX++]);
					}
					csrX += OpcodeStrlen1(buffer[ptrX]);
					ptrX++;
				break;
			case KEY_CTRL_UP:
					csrX=0; 
					ptrX=0;
					offsetX=0;
				break;
			case KEY_CTRL_DOWN:
					ptrX=length;
					offsetX=OpStrLastoffset(buffer, ptrX, csrwidth ,&csrX);
				break;
			case KEY_CTRL_F5:
				lowercase=1-lowercase;
				break;
			case KEY_CTRL_F6:
				key=InputChar();
				break;
			default:
				break;
		}
		if (
		   ( KEY_CHAR_0 <= key  ) && ( key <= KEY_CHAR_9     ) ||
		   ( hex_mode && ( KEY_CHAR_A <= key  ) && ( key <= KEY_CHAR_F ) ) ||
		   ( hex_mode && ( KEY_CTRL_XTT == key ) ) ||
		   ( hex_mode && ( KEY_CHAR_LOG == key ) ) ||
		   ( hex_mode && ( KEY_CHAR_LN  == key ) ) ||
		   ( hex_mode && ( KEY_CHAR_SIN == key ) ) ||
		   ( hex_mode && ( KEY_CHAR_COS == key ) ) ||
		   ( hex_mode && ( KEY_CHAR_TAN == key ) ) ||
		   ( alpha_mode && ( 0x1A <= key  ) && ( key <= 0x7E ) ) ||
		   ( alpha_mode && ( 0xD8 == key  ) ) ||
		   ( float_mode && ( key == KEY_CHAR_DP ) ) || ( key == KEY_CHAR_MINUS ) || ( key == KEY_CHAR_PMINUS ) || ( key == KEY_CHAR_PLUS ) ||
		   ( float_mode && exp_mode && ( key == KEY_CHAR_EXP )  ) ||
		   ( float_mode && ( 0x07 <= key  ) && ( key <= 0x0D ) ) ||
		   ( float_mode && ( 0x0F <= key  ) && ( key <= 0x13 ) ) ||
		   ( float_mode && ( 0x01 == key  ) ) ||
		   ( float_mode && ( 0x03 == key  ) ) ||
		   ( float_mode && exp_mode && ( 'A' <= key  ) && ( key <= 'Z' ) ) ||
		   ( float_mode && exp_mode && ( KEY_CHAR_FRAC == key  ) ) ||
		   ( float_mode && exp_mode && ( KEY_CHAR_LOG == key  ) ) ||
		   ( float_mode && exp_mode && ( KEY_CHAR_LN == key  ) ) ||
		   ( float_mode && exp_mode && ( KEY_CHAR_SIN == key  ) ) ||
		   ( float_mode && exp_mode && ( KEY_CHAR_COS == key  ) ) ||
		   ( float_mode && exp_mode && ( KEY_CHAR_TAN == key  ) ) ||
		   ( float_mode && exp_mode && ( KEY_CHAR_SQUARE == key  ) ) ||
		   ( float_mode && exp_mode && ( KEY_CHAR_POW == key  ) ) ||
		   ( float_mode && exp_mode && ( KEY_CHAR_PI == key  ) ) ||
		   ( float_mode && exp_mode && ( KEY_CHAR_CUBEROOT == key  ) ) ||
		   ( float_mode && exp_mode && ( KEY_CHAR_RECIP == key  ) ) ||
		   ( float_mode && exp_mode && ( KEY_CHAR_EXPN10 == key  ) ) ||
		   ( float_mode && exp_mode && ( KEY_CHAR_EXPN == key  ) ) ||
		   ( float_mode && exp_mode && ( KEY_CHAR_ASIN == key  ) ) ||
		   ( float_mode && exp_mode && ( KEY_CHAR_ACOS == key  ) ) ||
		   ( float_mode && exp_mode && ( KEY_CHAR_ATAN == key  ) ) ||
		   ( float_mode && exp_mode && ( KEY_CHAR_ROOT == key  ) ) ||
		   ( float_mode && exp_mode && ( KEY_CHAR_POWROOT == key  ) ) ||
		   ( float_mode && exp_mode && ( KEY_CHAR_ASIN == key  ) ) ||
		   ( float_mode && exp_mode && ( 0x80 <= key  ) && ( key <= 0xFF ) ) ||
		   (  ( KEY_CHAR_MULT == key  ) ) ||
		   (  ( KEY_CHAR_DIV  == key  ) ) ||
		   (  ( KEY_CHAR_POW  == key  ) ) ||
		   (  ( KEY_CTRL_XTT  == key  ) )
		   ) {
			if ( lowercase  && ( 'A' <= key  ) && ( key <= 'Z' ) ) key+=('a'-'A');
			if ( hex_mode && ( KEY_CTRL_XTT == key ) ) key=KEY_CHAR_A;
			if ( hex_mode && ( KEY_CHAR_LOG == key ) ) key=KEY_CHAR_B;
			if ( hex_mode && ( KEY_CHAR_LN  == key ) ) key=KEY_CHAR_C;
			if ( hex_mode && ( KEY_CHAR_SIN == key ) ) key=KEY_CHAR_D;
			if ( hex_mode && ( KEY_CHAR_COS == key ) ) key=KEY_CHAR_E;
			if ( hex_mode && ( KEY_CHAR_TAN == key ) ) key=KEY_CHAR_F;
			if ( float_mode && ( key == KEY_CHAR_POW ) )    key='^';
			if ( ( key == KEY_CTRL_XTT ) )   key='X'; // ^
//			if ( key == KEY_CHAR_PLUS  )  key='+';
//			if ( key == KEY_CHAR_MINUS )  key='-';
//			if ( key == KEY_CHAR_PMINUS ) key='-';
//			if ( key == KEY_CHAR_EXP )    key='e';
			
			if ( CursorStyle < 0x6 ) {		// insert mode
				if ( csrX == csrwidth ) {	// rigth edge
					if ( ptrX == length ) {			// rigth edge new
						if ( length < MaxStrlen ) {
							buffer[ptrX]=key;
							ptrX++;
							length++;
							oplen=OpcodeStrlen1(key); j=0;
							while ( j<oplen ) {
								i = OpcodeStrlen1(buffer[offsetX++]);
								j += i;
								csrX -= i;
							}
							csrX += oplen;
						}
					} else
					if ( ptrX < length ) {	// rigth edge insert
						if ( length < MaxStrlen ) {
							for(i=length; i>ptrX; i--) buffer[i]=buffer[i-1];
							buffer[ptrX]=key;
							ptrX++;
							length++;
							oplen=OpcodeStrlen1(key); j=0;
							while ( j<oplen ) {
								i = OpcodeStrlen1(buffer[offsetX++]);
								j += i;
								csrX -= i;
							}
							csrX += oplen;
						}
					}
				} else
				if ( csrX < csrwidth ) {
					if ( ptrX == length ) {	 // new 1st input at left edge
						if ( length < MaxStrlen ) {
							buffer[ptrX]=key;
							ptrX++;
							length++;
							oplen=OpcodeStrlen1(key);
							csrX+=oplen;
							if ( csrX > csrwidth ) offsetX+=OpcodeOffsetCount(buffer+offsetX, csrX-csrwidth, &csrX);
						}
					} else 
					if ( ptrX < length ) {	// insert
						if ( length < MaxStrlen ) {
							for(i=length; i>ptrX; i--) buffer[i]=buffer[i-1];
							buffer[ptrX]=key;
							ptrX++;
							length++;
							oplen=OpcodeStrlen1(key);
							csrX+=oplen;
							if ( csrX > csrwidth ) offsetX+=OpcodeOffsetCount(buffer+offsetX, csrX-csrwidth, &csrX);
						}
					}
				}
			} else {					// overwrite mode
				if ( csrX == csrwidth ) {	// rigth edge
					if ( ptrX == length ) {			// rigth edge new
						if ( length < MaxStrlen ) {
							buffer[ptrX]=key;
							ptrX++;
							length++;
							oplen=OpcodeStrlen1(key); j=0;
							while ( j<oplen ) {
								i = OpcodeStrlen1(buffer[offsetX++]);
								j += i;
								csrX -= i;
							}
							csrX += oplen;
						}
					} else
					if ( ptrX < length ) {	// overwrite
							buffer[ptrX]=key;
							ptrX++;
							oplen=OpcodeStrlen1(key); j=0;
							while ( j<oplen ) {
								i = OpcodeStrlen1(buffer[offsetX++]);
								j += i;
								csrX -= i;
							}
							csrX += oplen;
					}
				} else
				if ( csrX < csrwidth ) {  // overwrite
					if ( ptrX == length ) {			// new 1st input at left edge
						if ( length < MaxStrlen ) {
							buffer[ptrX]=key;
							ptrX++;
							length++;
							oplen=OpcodeStrlen1(key);
							csrX+=oplen;
							if ( csrX > csrwidth ) offsetX+=OpcodeOffsetCount(buffer+offsetX, csrX-csrwidth, &csrX);
						}
					} else
					if ( ptrX < length ) {	// overwrite
							buffer[ptrX]=key;
							ptrX++;
							oplen=OpcodeStrlen1(key);
							csrX+=oplen;
							if ( csrX > csrwidth ) offsetX+=OpcodeOffsetCount(buffer+offsetX, csrX-csrwidth, &csrX);
					}
				}
			}
		}
	}

	Cursor_SetFlashMode(0);		// cursor flashing off
	if ( alpha_mode ) { Fkey_Clear(4); Fkey_Clear(5); }
	
	buffer[length]='\0';
	return key ;	// EXE:0
}

//----------------------------------------------------------------------------------------------
/*
double round( double x, int n ) {
	unsigned char row[9];
	int tmp,exp;
	int i,j,bt;
	double result;
	unsigned char *ptr;

	ptr=(unsigned char *)(&x);
	for (i=0;i<8;i++) row[i]=ptr[i];

	tmp= ( (row[0]&0x7f)<<4 ) + ( (row[1]&0xf0)>>4 ) ;
	exp=tmp-1023; // 
	if ( ( 0 > exp ) || ( exp > 51 ) ) result = x;
	else {
		tmp=52-exp;
		i=7; j=0; bt=0xFE;
		while ( tmp ) {
			if (tmp>7) {
				 tmp-=8; row[i]=0; i--;
			} else {
				row[i]&=bt;
				bt=bt<<1;
				j++;
				tmp--;
			}
		}
		ptr=(unsigned char *)(&result);
		for (i=0;i<8;i++) ptr[i]=row[i];
	}
	return ( result ); 
}

*/

double Round( double num, int round_mode, int digit){
	int minus=0,exp,ex2;
	double fabsnum=fabs(num),tmp,tmp2,tmp3,exf;

	if ( num < 0 ) minus=-1;

	switch ( round_mode ) {
		case Fix: // fix
			if (fabsnum<1e15) {
				tmp=pow(10,digit);
				num=floor(fabsnum*tmp+.5)/tmp;
				fabsnum=num;
				if ( minus ) num=-num;
			}
			break;
		case Norm:	// normal
			if ( digit==1 ) digit=10;
			if ( digit==2 ) digit=10;
		case Sci: // sci
			if ( digit==0 ) break;
			if ( digit>=16 ) break;
			exf=log10(fabsnum);
			exp=exf;
			if ( exf > 0 ) exp++;
			exf=pow(10,exp);
			tmp=fabsnum/exf;
			tmp2=pow(10,digit);
			tmp3=floor(tmp*tmp2+.5)/tmp2;
			num=tmp3*exf;
			if ( minus ) num=-num;
			break;
	}
	return num;
}

void sprintGRS(char* buffer, double num, int width, int align_mode, int round_mode, int round_digit) { // + round
	int i,j,w,adj,minus=0,p,digit=round_digit;
	char buffer2[32],fstr[16],tmp[16];
	double fabsnum,pw;
	unsigned char c;
	char *dptr,*eptr,*nptr;
	double dpoint=0.01;

	switch ( round_mode ) {
		case Norm:
			if ( round_digit==1 ) { dpoint=0.01;        digit=10; }
			if ( round_digit==2 ) { dpoint=0.000000001; digit=10; }
			if ( round_digit==0 ) digit=16;
//			if ( round_digit==2 ) { dpoint=0.000000001; digit=10; i=11; c='f'; break; }
//			num = Round( num, round_mode, digit);
			if ( num < 0 ) minus=-1;
			fabsnum=fabs(num);
		 	w=15; if ( w > width )  w=width;
		 	pw=pow(10,w+minus);
			if ( ( fabsnum==0 ) || ( ( dpoint <= fabsnum ) && ( fabsnum < pw ) ) ) {
				w = floor((log10(fabsnum))) - minus + (15-digit);
				if ( digit >= width ) w= w+(digit-width);
				i=14-w;
				if ( i >= 18 ) i=18;
				c='f';
				if ( i <  0  ) { i=digit-1; c='e'; }
				if ( i <  0  ) i=15;
				if ( round_digit==2 ) if ( i>11 ) i=11;
			} else {
				adj = 1 - minus+ floor(log10(fabs(log10(fabsnum))))-1;
				if ( ( 1e-10 <= num ) && ( num < 0.001 )) adj++;
				i=width-adj-5;
				if ( i > digit-1 ) i=digit-1;
				if ( i >= 18 ) i=18;
				if ( i <  1  ) i=1;
				if ( round_digit==2 ) if ( i>11 ) i=11;
				c='e';
			}
			break;
		case Fix:
				i=digit;
				c='f';
			break;
		case Sci:
				num = Round( num, round_mode, digit);
				i=digit-1; if ( i < 0 ) i=15;
				c='e';
			break;
	}
	
	p=0;
	fstr[p++]='%';
	fstr[p++]='.';
	if ( i >= 10 ) fstr[p++]='0'+i/10;
	fstr[p++]='0'+i%10;
	fstr[p++]=c;
	fstr[p++]='\0';
	sprintf(buffer, fstr, num);

/*
	if ( round_mode != Fix ) {
		if ( ( strchr(buffer,'.') != '\0' ) && ( strchr(buffer,'e') == '\0' ) ) {
			i=strlen(buffer)-1;
			while ( buffer[i]=='0' ) buffer[i--]='\0';		// 1.234500000  zero cut
			if ( buffer[i]=='.' ) buffer[i]='\0';
		}
	}
*/	
	if ( round_mode == Norm ) {
		dptr=strchr(buffer,'.');
		if ( dptr ) {
			eptr=strchr(buffer,'e');
			i=strlen(buffer); 
			nptr=buffer+i;
			if (  eptr != '\0' ) {	// 1.234500000e123  zero cut
				eptr--; i=0;
				while ( eptr[i] == '0' ) i-- ;
				if ( i ) {
					j=0;
					while ( eptr[j] != '\0' ) eptr[++i]=eptr[++j];
				}
			} else {				// 1.234500000  zero cut
				i=-1;
				while ( nptr[i] == '0' ) nptr[i--]='\0';
				if ( nptr[i]=='.' ) nptr[i]='\0';
			}
		}
	}

	if ( align_mode == RIGHT_ALIGN ) {
		for(i=0; i<22; i++) buffer2[i]=buffer[i]; // 
		w=strlen(buffer2);
		if ( w < width ) {
			for ( i=1; i<=w; i++)
			 buffer2[width-i]=buffer2[w-i];
			for ( i=0; i<width-w; i++) buffer2[i]=' ';
		}
		for(i=0; i<22; i++) buffer[i]=buffer2[i]; // 
	}
	
	buffer[width]='\0';

	i=-1;
	while (i<width) {
		c=buffer[++i];
		switch ( c ) {
			case '-':
				buffer[i]=0x87;	// (-)
				break;
//			case '+':
//				buffer[i]=0x89;	// (+)
//				break;
			case 'e':	// exp
				buffer[i]=0x0F;	// (exp)
				break;
		}
	}

	return ;
}

void sprintGR(char* buffer, double num, int width, int align_mode, int round_mode, int round_digit) { // + round  ENG
	unsigned char c,d=0;
	if (ENG) { 
		if ( ( 1e-15 <= num  ) && ( num < 1e21 ) ) {
			width-- ; 
			if      ( num >= 1e18 ) { num/=1e18;  c=0x0B; }	//  Exa
			else if ( num >= 1e15 ) { num/=1e15;  c=0x0A; }	//  Peta
			else if ( num >= 1e12 ) { num/=1e12;  c=0x09; }	//  Tera
			else if ( num >= 1e09 ) { num/=1e09;  c=0x08; }	//  Giga
			else if ( num >= 1e06 ) { num/=1e06;  c=0x07; }	//  Mega
			else if ( num >= 1e03 ) { num/=1e03;  c=0x6B; }	//  Kiro
			else if ( num >= 1    ) {             c=' ';  }
			else if ( num >= 1e-3 ) { num/=1e-3;  c=0x6d; }	//  milli
			else if ( num >= 1e-6 ) { num/=1e-6;  c=0xE6; d=0x4B; }	//  micro
			else if ( num >= 1e-9 ) { num/=1e-9;  c=0x03; }	//  nano
			else if ( num >= 1e-12) { num/=1e-12; c=0x70; }	//  pico
			else if ( num >= 1e-15) { num/=1e-15; c=0x66; }	//  femto
			sprintGRS( buffer, num, width, align_mode, round_mode, round_digit);
			width=strlen(buffer);
			buffer[width++]=c;
			buffer[width++]=d;
			buffer[width]='\0';
			return ;
		}
	}
	sprintGRS( buffer, num, width, align_mode, round_mode, round_digit);
}

void sprintG(char* buffer, double num, int width, int align_mode) {
	sprintGRS(buffer, num, width, align_mode, Norm, 15); // + round
}

//----------------------------------------------------------------------------------------------
double InputNumD(int x, int y, int width, double defaultNum, char SPC, int rev_mode, int float_mode, int exp_mode) {		// 0123456789.(-)exp
	char buffer[32];
	int csrX=0;
	unsigned int key;

	buffer[csrX]='\0';
	if ( defaultNum != 0 ) {
		sprintG(buffer, defaultNum, width, LEFT_ALIGN);
		csrX=strlen(buffer);
	}
	key= InputStrSub( x, y, width, csrX, buffer, width, SPC, rev_mode, float_mode, exp_mode, ALPHA_OFF, HEX_OFF, PAL_ON, EXIT_CANCEL_OFF) ;
	if ( ( key == KEY_CTRL_EXIT ) || ( key != KEY_CTRL_EXE ) ) return (defaultNum);  // exit
	return atof( buffer );
}

unsigned int InputStr(int x, int y, int width, char* buffer, char SPC, int rev_mode) {		// ABCDEF0123456789.(-)exp
	int csrX=0;
	unsigned int key;

	buffer[width]='\0';
	csrX=strlen(buffer);
	key= InputStrSub( x, y, width, csrX, buffer, width, SPC, rev_mode, FLOAT_ON, EXP_ON, ALPHA_ON, HEX_OFF, PAL_ON, EXIT_CANCEL_OFF);
	return ( key );
}

