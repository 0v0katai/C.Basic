#include <ctype.h>
#include <fxlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <timer.h>
#include "KeyScan.h"
#include "CB_io.h"
#include "CB_inp.h"
#include "CB_glib.h"
#include "CB_glib2.h"
#include "CB_Eval.h"
#include "CB_file.h"
#include "CB_edit.h"
#include "CB_setup.h"
#include "CB_Time.h"

#include "CB_interpreter.h"
#include "CBI_interpreter.h"
#include "CB_error.h"
#include "fx_syscall.h"
#include "CB_Matrix.h"


//-----------------------------------------------------------------------------
void TextOut( int opcode, char *text, char *str, int *textptr ) {
		switch ( opcode ) {
			case 0x01:
				text[(*textptr)++]='f';
				text[(*textptr)++]='e';
				text[(*textptr)++]='m';
				text[(*textptr)++]='t';
				text[(*textptr)++]='o';
				return;
			case 0x02:
				text[(*textptr)++]='p';
				text[(*textptr)++]='i';
				text[(*textptr)++]='c';
				text[(*textptr)++]='o';
				return;
			case 0x03:
				text[(*textptr)++]='n';
				text[(*textptr)++]='a';
				text[(*textptr)++]='n';
				text[(*textptr)++]='o';
				return;
			case 0x04:
				text[(*textptr)++]='m';
				text[(*textptr)++]='i';
				text[(*textptr)++]='c';
				text[(*textptr)++]='r';
				text[(*textptr)++]='o';
				return;
			case 0x05:
				text[(*textptr)++]='m';
				text[(*textptr)++]='i';
				text[(*textptr)++]='l';
				text[(*textptr)++]='l';
				text[(*textptr)++]='i';
				return;
			case 0x06: 
				text[(*textptr)++]='k';
				text[(*textptr)++]='i';
				text[(*textptr)++]='l';
				text[(*textptr)++]='o';
				return;
			case 0x07: 
				text[(*textptr)++]='M';
				text[(*textptr)++]='e';
				text[(*textptr)++]='g';
				text[(*textptr)++]='a';
				return;
			case 0x08: 
				text[(*textptr)++]='G';
				text[(*textptr)++]='i';
				text[(*textptr)++]='g';
				text[(*textptr)++]='a';
				return;
			case 0x09: 
				text[(*textptr)++]='T';
				text[(*textptr)++]='e';
				text[(*textptr)++]='r';
				text[(*textptr)++]='a';
				return;
			case 0x0A: 
				text[(*textptr)++]='P';
				text[(*textptr)++]='e';
				text[(*textptr)++]='t';
				text[(*textptr)++]='a';
				return;
			case 0x0B: 
				text[(*textptr)++]='E';
				text[(*textptr)++]='x';
				text[(*textptr)++]='a';
				return;
			case 0x0C: 
				text[(*textptr)++]='D';
				text[(*textptr)++]='i';
				text[(*textptr)++]='s';
				text[(*textptr)++]='p';
				text[(*textptr)++]='s';
				text[(*textptr)++]=0x0D;
				text[(*textptr)++]=0x0A;
				return;
			case 0x0D: 
				text[(*textptr)++]=0x0D;
				text[(*textptr)++]=0x0A;
				return;
			case 0x0E:
				text[(*textptr)++]='-';
				text[(*textptr)++]='>';
				return;
			case 0x0F: 
				text[(*textptr)++]='E';
				text[(*textptr)++]='x';
				text[(*textptr)++]='p';
				return;
			case 0x10: 
				text[(*textptr)++]='<';
				text[(*textptr)++]='=';
				return;
			case 0x11: 
				text[(*textptr)++]='<';
				text[(*textptr)++]='>';
				return;
			case 0x12: 
				text[(*textptr)++]='>';
				text[(*textptr)++]='=';
				return;
			case 0x13: 
				text[(*textptr)++]='=';
				text[(*textptr)++]='>';
				return;
			case 0x14: 
				text[(*textptr)++]='f';
				text[(*textptr)++]='1';
				return;
			case 0x15: 
				text[(*textptr)++]='f';
				text[(*textptr)++]='2';
				return;
			case 0x16: 
				text[(*textptr)++]='f';
				text[(*textptr)++]='3';
				return;
			case 0x17: 
				text[(*textptr)++]='f';
				text[(*textptr)++]='4';
				return;
			case 0x18: 
				text[(*textptr)++]='f';
				text[(*textptr)++]='5';
				return;
			case 0x19: 
				text[(*textptr)++]='f';
				text[(*textptr)++]='6';
				return;
			case 0x1A: 
				text[(*textptr)++]='&';
				text[(*textptr)++]='H';
				text[(*textptr)++]='A';
				return;
			case 0x1B: 
				text[(*textptr)++]='&';
				text[(*textptr)++]='H';
				text[(*textptr)++]='B';
				return;
			case 0x1C: 
				text[(*textptr)++]='&';
				text[(*textptr)++]='H';
				text[(*textptr)++]='C';
				return;
			case 0x1D: 
				text[(*textptr)++]='&';
				text[(*textptr)++]='H';
				text[(*textptr)++]='D';
				return;
			case 0x1E: 
				text[(*textptr)++]='&';
				text[(*textptr)++]='H';
				text[(*textptr)++]='E';
				return;
			case 0x1F: 
				text[(*textptr)++]='&';
				text[(*textptr)++]='H';
				text[(*textptr)++]='F';
				return;
			case 0x87: 
				text[(*textptr)++]='(';
				text[(*textptr)++]='-';
				text[(*textptr)++]=')';
				return;
			case 0x89: 
				text[(*textptr)++]='+';
				return;
			case 0x99: 
				text[(*textptr)++]='-';
				return;
			case 0x9B: 
				text[(*textptr)++]='^';
				text[(*textptr)++]='-';
				text[(*textptr)++]='1';
				return;
			case 0xA9: 
				text[(*textptr)++]='*';
				return;
			case 0xB9: 
				text[(*textptr)++]='/';
				return;
			case 0xB5:
				text[(*textptr)++]='(';
				text[(*textptr)++]='1';
				text[(*textptr)++]='0';
				text[(*textptr)++]=')';
				return;
			case 0xD0:
				text[(*textptr)++]='p';
				text[(*textptr)++]='i';
				return;
		}

		switch ( str[0] ) {		
			case 0xFFFFFFE5:
				switch ( str[1] ) {
					case 0xFFFFFFCA:
						text[(*textptr)++]='^';
						text[(*textptr)++]='-';
						text[(*textptr)++]='1';
						return;
					default:
						text[(*textptr)++]='#';		
						NumToHex( text+(*textptr), str[0]*256+str[1], 4);
						(*textptr)+=4;
				}
				return;
			case 0xFFFFFFE6:
				switch ( str[1] ) {
					case 0x4F:
						text[(*textptr)++]='p';
						text[(*textptr)++]='i';
						return;
					default:
						text[(*textptr)++]='#';		
						NumToHex( text+(*textptr), str[0]*256+str[1], 4);
						(*textptr)+=4;
				}
				return;
			case 0xFFFFFFE7:
				text[(*textptr)++]='#';		
				NumToHex( text+(*textptr), str[0]*256+str[1], 4);
				(*textptr)+=4;
				return;
			case 0xFFFFFFFF:	// kana
				switch ( str[1] ) {
					case 0xFFFFFFA0:
						text[(*textptr)++]=' ';
						return;
					default:
					text[(*textptr)++]=str[1];
				}
				return;
			default:
				text[(*textptr)++]=str[0];
				return;
		}
}

int OpcodeToText( char *buffer, char *text, int maxsize ) {
	char tmpbuf[18],*tmpb;
	int i,cont=1;
	int opcode;
	int c=1;
	int ofst=0,len;
	int textptr=18;
//	strncpy( text, "'ProgramMode:RUN\r\n",18);
	strncpy( text, "'C.Basic Program\r\n",18);
	while ( cont ) {
		opcode = GetOpcode( buffer, ofst );
		if ( opcode=='\0' ) break;
		ofst += OpcodeLen( opcode );
		CB_OpcodeToStr( opcode, tmpbuf ) ; // SYSCALL
		len = CB_MB_ElementCount( tmpbuf ) ;				// SYSCALL
		i=0;
		tmpb=tmpbuf;
		while ( i < len ) {
			TextOut( opcode, text, tmpb+i, &textptr ) ;
			c=tmpb[i]&0xFF;
			if ( (c==0x7F)||(c==0xF7)||(c==0xF9)||(c==0xE5)||(c==0xE6)||(c==0xE7)||(c==0xFF) ) tmpb++;
			i++;
		}
		if ( textptr > maxsize-16 ) return -1; // text buffer overflow
	}
	return textptr;	//ok
}

