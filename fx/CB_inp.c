/*****************************************************************/
/*                                                               */
/*   inp Library  ver 0.94                                       */
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
	int ofst=*offset;
	while (1) {
		 if ( NextOpcode( SRC, &(*offset) ) ==0 ) return (*offset)-ofst ;
		c=SRC[(*offset)-1];
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
	int ofst=*offset;
	while (1) {
		PrevOpcode( SRC, &(*offset) );
		if ( *offset <= 0 ) { *offset=0; return ofst-(*offset) ; }
		c=SRC[*offset];
		d=SRC[*offset-1];
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

int PrintlineOpcode(int *y, unsigned char *buffer, int ofst, int csrPtr, int *cx, int *cy) {
	char tmpbuf[18];
	int i,len,x=1,xmax=21,cont=1;
	unsigned short opcode;
	unsigned char  c=1;
	while ( cont ) {
		if (ofst==csrPtr) { *cx=x; *cy=*y; }
		c = buffer[ofst] ; 
		opcode = c & 0x00FF ;
		if ( opcode=='\0' ) break;
		if ( ( c==0x7F ) || ( c==0xF7 ) ||( c==0xF9 ) ||( c==0xE5 ) ||( c==0xE6 ) ||( c==0xE7 ) ) {
				ofst++;
				opcode = ( ( c & 0x00FF )<<8 )	+ ( buffer[ofst] & 0x00FF );
		}
		ofst++;
		OpcodeToStr( opcode, (unsigned char*)tmpbuf ) ; // SYSCALL
		len = MB_ElementCount( tmpbuf ) ;				// SYSCALL
		i=0;
		while ( i < len ) {
			locate(x,*y);
			PrintC(    (unsigned char*)(tmpbuf+i) ) ;
//			Bdisp_PutDisp_DD();
			x++;
			if ( ( x > xmax ) || ( opcode==0x0C ) || ( opcode==0x0D ) ) {
					if ( (*y) >= 7 ) { cont=0;break;}
					(*y)++; x=1;
			}
			i++;
		}
	}
	return ofst;
}


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

	Cursor_SetFlashMode(1);			// cursor flashing on
	if (Cursor_GetFlashStyle()<0x6)	Cursor_SetFlashOn(0x0);		// insert mode cursor 
		else 						Cursor_SetFlashOn(0x6);		// overwrite mode cursor 

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
				key=0;
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
			if ( float_mode && ( key == KEY_CTRL_XTT ) )   key='X'; // ^
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
void sprintG(char* buffer, double num, int width, int align_mode) {
	int i,w,adj,minus=0;
	double round_adj=0;
	char buffer2[32];
    
	if ( num < 0 ) minus=-1;
 	w=15; if ( w > width )  w=width;
	if ( ( 0.0001 <= fabs(num) ) && ( fabs(num) < pow(10,w+minus) ) ) {
		w = floor((log10(fabs(num)))) - minus;
		if ( 16 >= width ) w= w+(16-width);
		if ( w <= -5 ) sprintf(buffer,"%.19f",num);
		if ( w == -4 ) sprintf(buffer,"%.18f",num);
		if ( w == -3 ) sprintf(buffer,"%.17f",num);
		if ( w == -2 ) sprintf(buffer,"%.16f",num);
		if ( w == -1 ) sprintf(buffer,"%.15f",num);
		if ( w ==  0 ) sprintf(buffer,"%.14f",num);
		if ( w ==  1 ) sprintf(buffer,"%.13f",num);
		if ( w ==  2 ) sprintf(buffer,"%.12f",num);
		if ( w ==  3 ) sprintf(buffer,"%.11f",num);
		if ( w ==  4 ) sprintf(buffer,"%.10f",num);
		if ( w ==  5 ) sprintf(buffer,"%.9f",num);
		if ( w ==  6 ) sprintf(buffer,"%.8f",num);
		if ( w ==  7 ) sprintf(buffer,"%.7f",num);
		if ( w ==  8 ) sprintf(buffer,"%.6f",num);
		if ( w ==  9 ) sprintf(buffer,"%.5f",num);
		if ( w == 10 ) sprintf(buffer,"%.4f",num);
		if ( w == 11 ) sprintf(buffer,"%.3f",num);
		if ( w == 12 ) sprintf(buffer,"%.2f",num);
		if ( w == 13 ) sprintf(buffer,"%.1f",num);
		if ( w >= 14 ) sprintf(buffer,"%.0f",num);
	}
	else {
		adj = 1 - minus+ floor(log10(fabs(log10(fabs(num)))))-1;
		if ( ( 1.e-10 < num ) && ( num < 0.001 )) adj++;
		if ( width >= 23+adj ) sprintf(buffer,"%.18g",num);
		if ( width >= 22+adj ) sprintf(buffer,"%.17g",num);
		if ( width >= 21+adj ) sprintf(buffer,"%.16g",num);
		if ( width >= 20+adj ) sprintf(buffer,"%.15g",num);
		if ( width == 19+adj ) sprintf(buffer,"%.14g",num);
		if ( width == 18+adj ) sprintf(buffer,"%.13g",num);
		if ( width == 17+adj ) sprintf(buffer,"%.12g",num);
		if ( width == 16+adj ) sprintf(buffer,"%.11g",num);
		if ( width == 15+adj ) sprintf(buffer,"%.10g",num);
		if ( width == 14+adj ) sprintf(buffer,"%.9g",num);
		if ( width == 13+adj ) sprintf(buffer,"%.8g",num);
		if ( width == 12+adj ) sprintf(buffer,"%.7g",num);
		if ( width == 11+adj ) sprintf(buffer,"%.6g",num);
		if ( width == 10+adj ) sprintf(buffer,"%.5g",num);
		if ( width ==  9+adj ) sprintf(buffer,"%.4g",num);
		if ( width ==  8+adj ) sprintf(buffer,"%.3g",num);
		if ( width ==  7+adj ) sprintf(buffer,"%.2g",num);
		if ( width <   7+adj ) sprintf(buffer,"%.1g",num);
	}

	if ( ( strchr(buffer,'.') != '\0' ) && ( strchr(buffer,'e') == '\0' ) ) {
		i=strlen(buffer)-1;
		while ( buffer[i]=='0' ) buffer[i--]='\0';		// 1.234500000  zero cut
		if ( buffer[i]=='.' ) buffer[i]='\0';
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
	return ;
}

//----------------------------------------------------------------------------------------------
double InputNumD(int x, int y, int width, double defaultNum, char SPC, int rev_mode, int float_mode, int exp_mode) {		// 0123456789.(-)exp
	char buffer[32];
	int csrX=0;

	buffer[csrX]='\0';
	if ( defaultNum != 0 ) {
		sprintG(buffer, defaultNum, width, LEFT_ALIGN);
		csrX=strlen(buffer);
	}
	if ( InputStrSub( x, y, width, csrX, buffer, width, SPC, rev_mode, float_mode, exp_mode, ALPHA_OFF, HEX_OFF, PAL_ON, EXIT_CANCEL_OFF) ) return (defaultNum);  // exit
	return atof( buffer );
}

unsigned int InputStr(int x, int y, int width, char* buffer, char SPC, int rev_mode) {		// ABCDEF0123456789.(-)exp
	int csrX=0;
	unsigned int key;

	buffer[width]='\0';
	csrX=strlen(buffer);
	key=InputStrSub( x, y, width, csrX, buffer, width, SPC, rev_mode, FLOAT_ON, EXP_ON, ALPHA_ON, HEX_OFF, PAL_ON, EXIT_CANCEL_OFF);
	return ( key );
}

