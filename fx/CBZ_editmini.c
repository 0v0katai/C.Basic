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

//---------------------------------------------------------------------------------------------
int Mini_OpcodeLineSub( char *buffer, int *ofst , int *len) {
	char tmpbuf[18];
	int opcode = GetOpcode( buffer, *ofst );
	if ( opcode=='\0' ) return 0;
	if ( ( opcode==0x0C ) || ( opcode==0x0D ) ) return 0 ;
	(*ofst) += OpcodeLen( opcode );
	CB_OpcodeToStr( opcode, tmpbuf ) ;		// SYSCALL
//	(*len) = CB_MB_ElementCount( tmpbuf ) ;	// SYSCALL
	(*len) = CB_PrintMiniLength(( tmpbuf ) ;	// 
	
	return 1;
}
void Mini_OpcodeLineN( char *buffer, int *ofst, int *x, int *y ) {
	int i,len,xmax=20*6,cont=1;
	(*x)=1;
	while ( cont ) {
		if ( OpcodeLineSub( buffer, &(*ofst) , &len) == 0 ) break;
		i=0;
		while ( i < len ) {
			(*x)+=(*len) CB_PrintMiniLength(( buffer+ptr );
			if ( (*x) > xmax ) { (*y)++; (*x)=1; }
			ptr += oplen;;
			i++;
		}
	}
	return ;
}
int Mini_OpcodeLineYptr(int n, char *buffer, int ofst, int *x ) {
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
int Mini_OpcodeLineSrcXendpos(int n, char *buffer, int ofst) {
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
int Mini_OpcodeLineSrcYpos( char *buffer, int ofst, int csrptr ) {
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

void Mini_NextLinePhy( char *buffer, int *ofst, int *ofst_y ) {
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

void Mini_PrevLinePhy( char *buffer, int *ofst, int *ofst_y ) {
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

void Mini_NextLinePhyN( int n, char *SrcBase, int *offset, int *offset_y ) {
	int i;
	for ( i=0; i<n; i++ ) {
		if ( SrcBase[(*offset)] == 0 ) break;
		NextLinePhy( SrcBase, &(*offset), &(*offset_y) );
	}
	return ;
}
void Mini_PrevLinePhyN( int n, char *SrcBase, int *offset, int *offset_y ) {
	int i;
	for ( i=0; i<n; i++ ) {
		if ( ( (*offset) == 0 ) && ( (*offset_y) == 0 ) ) break;
		PrevLinePhy( SrcBase, &(*offset), &(*offset_y) );
	}
}

int Mini_PrintOpcodeLine1( int csry, int n, char *buffer, int ofst, int csrPtr, int *cx, int *cy, int ClipStartPtr, int ClipEndPtr) {
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

int Mini_DumpOpcode( char *SrcBase, int *offset, int *offset_y, int csrPtr, int *cx, int *cy, int ClipStartPtr, int ClipEndPtr){
	int i,n,x,y,ynum;
	int ofst,ofst2,ofstYptr;
	int count=100;

	*cx=0; *cy=0;

	while ( 1 ) {
		for ( y=(2-EditTopLine); y<8; y++ ) { locate(1,y); PrintLine((unsigned char*)" ",21); }
		y=(2-EditTopLine); ofst=(*offset);
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
		if ( ( (*cx)!=0 ) && ( (*cy)>(1-EditTopLine) ) && ( (*cy)<8 ) ) break ;
		ofstYptr=OpcodeLineYptr( *offset_y, SrcBase, *offset, &x);
		if ( csrPtr < ofstYptr ) PrevLinePhy( SrcBase, &(*offset), &(*offset_y) );
		else 					 NextLinePhy( SrcBase, &(*offset), &(*offset_y) );

//		if ( SrcBase[ofst]==0x00 ) break ;
		count--;
		if ( count<50 ) if ( csrPtr > 0 ) csrPtr--;
		if ( count==0 ) {  // error reset
			(*offset)=0; (*offset_y)=0; (*cx)=(1-EditTopLine); (*cy)=2; return -1; 
		}
	}
	
	return 0; // ok
}


