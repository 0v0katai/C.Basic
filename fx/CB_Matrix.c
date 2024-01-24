#include <ctype.h>
#include <fxlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <timer.h>
#include "fx_syscall.h"
#include "KeyScan.h"
#include "CB_io.h"
#include "CB_inp.h"
#include "CB_glib.h"
#include "CB_glib2.h"
#include "CB_Eval.h"
#include "CB_interpreter.h"
#include "CB_file.h"
#include "CB_error.h"

//-----------------------------------------------------------------------------
// Matrix edit
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

unsigned int SetDimension(int reg, int *dimA, int *dimB){
	unsigned char buffer[22];
	unsigned int key;
	int	cont=1;
	int select=0;
	int y;

	PopUpWin(3);
	
	*dimA=MatArySizeA[reg];	//
	*dimB=MatArySizeB[reg];	//
	if ( *dimA==0 ) *dimA=1;
	if ( *dimB==0 ) *dimB=1;

	while (cont) {
		locate(3,3); Print((unsigned char *)"Dimension m\xA9n");
		locate(3,4); Print((unsigned char *) "  m  :           ");
		sprintG(buffer,*dimA,  10,LEFT_ALIGN); locate( 9, 4); Print(buffer);
		locate(3,5); Print((unsigned char *) "  n  :           ");
		sprintG(buffer,*dimB,  10,LEFT_ALIGN); locate( 9, 5); Print(buffer);

		y = select + 3 ;
		Bdisp_AreaReverseVRAM(12, y*8, 113, y*8+7);	// reverse select line 
		Bdisp_PutDisp_DD();

		GetKey( &key );
		switch (key) {
			case KEY_CTRL_EXIT:
			case KEY_CTRL_EXE:
				cont=0;
				break;
		
			case KEY_CTRL_UP:
				select-=1;
				if ( select < 0 ) select=1;
				break;
			case KEY_CTRL_DOWN:
				select+=1;
				if ( select > 1 ) select=0;
				break;

			case KEY_CTRL_RIGHT:
				Bdisp_AreaReverseVRAM(12, y*8, 113, y*8+7);	// reverse select line 
				Bdisp_PutDisp_DD();
				y++;
				switch (select) {
					case 0: // dim m
						*dimA  =InputNumD_full( 9, y, 10, *dimA);	// 
						select+=1;
						break;
					case 1: // dim n
						*dimB  =InputNumD_full( 9, y, 10, *dimB);	// 
						break;
					default:
						break;
				}
				break;
			default:
				break;
		}
		key=MathKey( key );
		if ( key ) {
				Bdisp_AreaReverseVRAM(12, y*8, 113, y*8+7);	// reverse select line 
				Bdisp_PutDisp_DD();
				y++;
				switch (select) {
					case 0: // dim m
						*dimA  =InputNumD_Char( 9, y, 10, *dimA, key);	// 
						select+=1;
						break;
					case 1: // dim n
						*dimB  =InputNumD_Char( 9, y, 10, *dimB, key);	// 
						break;
					default:
						break;
				}
		}

	}
	return key;
}
//-----------------------------------------------------------------------------
int DimMatrix( int reg, int dimA, int dimB ) {
	char	*cptr;
	short	*wptr;
	int		*iptr;
	double	*dptr;
	int i;
	unsigned int c=reg+'A';

	if ( ( MatAry[reg] != NULL ) && ( MatArySizeA[reg] >= dimA ) && ( MatArySizeB[reg] >= dimB ) ) { // already exist
		dptr = MatAry[reg] ;							// Matrix array ptr*
	} else {
		if ( MatAry[reg] != NULL ) 	free(MatAry[reg]);	// free
		MatArySizeA[reg]=dimA;						// Matrix array size
		MatArySizeB[reg]=dimB;						// Matrix array size
		if ( ( c=='B' ) || ( c=='C' ) ) {
			MatAryElementSize[reg]=1;
			dptr = malloc( dimA*dimB*sizeof(char) );
		} else 
		if ( ( c=='D' ) || ( c=='W' ) ) {
			MatAryElementSize[reg]=2;
			dptr = malloc( dimA*dimB*sizeof(short) );
		} else
		if ( ( c=='L' ) || ( c=='I' ) ) {
			MatAryElementSize[reg]=4;
			dptr = malloc( dimA*dimB*sizeof(int) );
		} else {
			MatAryElementSize[reg]=8;
			dptr = malloc( dimA*dimB*sizeof(double) );
		}
   		if( dptr == NULL ) { ErrorNo=NotEnoughMemoryERR; ErrorPtr=ExecPtr; return; }	// Not enough memory error
		MatAry[reg] = dptr ;							// Matrix array ptr*
	}
	
	switch ( MatAryElementSize[reg] ) {
		case 1:
			cptr=(char*)dptr;
			for (i=0; i< dimA*dimB; i++) cptr[i]=0;	// initialize double
			break;
		case 2:
			wptr=(short*)dptr;
			for (i=0; i< dimA*dimB; i++) wptr[i]=0;	// initialize char
			break;
		case 4:
			iptr=(int*)dptr;
			for (i=0; i< dimA*dimB; i++) iptr[i]=0;	// initialize word
			break;
		case 8:
			for (i=0; i< dimA*dimB; i++) dptr[i]=0;	// initialize int
			break;
	}
	return 0;	// ok
}

//-----------------------------------------------------------------------------
void DeleteMatrix( int reg ) {
	double *ptr;
	if ( ( 0 <= reg ) && ( reg <= 25 ) ) {
			ptr = MatAry[reg] ;							// Matrix array ptr*
			if (ptr != NULL ) free(ptr);		
			MatArySizeA[reg]=0;							// Matrix array size
			MatArySizeB[reg]=0;							// Matrix array size
			MatAry[reg]=NULL ;							// Matrix array ptr*
	} else {
		for ( reg=0; reg<26; reg++){
			ptr = MatAry[reg] ;							// Matrix array ptr*
			if (ptr != NULL ) free(ptr);		
			MatArySizeA[reg]=0;							// Matrix array size
			MatArySizeB[reg]=0;							// Matrix array size
			MatAry[reg]=NULL ;							// Matrix array ptr*		
		}
	}
}

//-----------------------------------------------------------------------------
double ReadMatrix( int reg, int dimA, int dimB){
	char*	MatAryC;
	short*	MatAryW;
	int*	MatAryI;
	double value;
	int mptr=(dimA-1)*MatArySizeB[reg]+dimB-1;
	switch ( MatAryElementSize[reg] ) {
		case 8:
			value = MatAry[reg][mptr];			// Matrix array double
			break;
		case 1:
			MatAryC=(char*)MatAry[reg];
			value = MatAryC[mptr]   ;			// Matrix array char
			break;
		case 2:
			MatAryW=(short*)MatAry[reg];
			value = MatAryW[mptr]   ;			// Matrix array word
			break;
		case 4:
			MatAryI=(int*)MatAry[reg];
			value = MatAryI[mptr]   ;			// Matrix array int
			break;
	}
	return value;
}
void WriteMatrix( int reg, int dimA, int dimB, double value){
	char*	MatAryC;
	short*	MatAryW;
	int*	MatAryI;
	int mptr=(dimA-1)*MatArySizeB[reg]+dimB-1;
	switch ( MatAryElementSize[reg] ) {
		case 8:
			MatAry[reg][mptr] = value ;			// Matrix array double
			break;
		case 1:
			MatAryC=(char*)MatAry[reg];
			MatAryC[mptr] = (char)value ;			// Matrix array char
			break;
		case 2:
			MatAryW=(short*)MatAry[reg];
			MatAryW[mptr] = (short)value ;			// Matrix array word
			break;
		case 4:
			MatAryI=(int*)MatAry[reg];
			MatAryI[mptr] = (int)value ;			// Matrix array int
			break;
	}
}
//-----------------------------------------------------------------------------

void EditMatrix(int reg){		// ----------- Edit Matrix
	unsigned char buffer[22];
	unsigned int key;
	int	cont=1;
	int seltopY=0,seltopX=0;
	int i,j,dimA,dimB,x,y;
	int selectX=0, selectY=0;
	double value;
	
	if (MatArySizeA[reg]==0) return;
	dimB=MatArySizeA[reg]-1;	//
	dimA=MatArySizeB[reg]-1;	//

	while (cont) {
		Bdisp_AllClr_VRAM();
		
		if (  selectY<seltopY ) seltopY = selectY;
		if ( (selectY-seltopY) > 4 ) seltopY = selectY-4;
		if ( (dimB -seltopY) < 4 ) seltopY = dimB -4; 
		if ( seltopY < 0 ) seltopY=0;

		if (  selectX<seltopX ) seltopX = selectX;
		if ( (selectX-seltopX) > 3 ) seltopX = selectX-3;
		if ( (dimA -seltopX) < 3 ) seltopX = dimA -3; 
		if ( seltopX < 0 ) seltopX=0;
		
		buffer[0]='A'+reg;
		buffer[1]='\0';
		locate( 1, 1); Print(buffer);

		j=5; if (j>dimB) j=dimB;
		i=4; if (i>dimA) i=dimA;
		for ( x=0; x<=i; x++ ) { 
			Bdisp_DrawLineVRAM( x*28+20,7,x*28+43,7);
			sprintf((char*)buffer,"%3d",seltopX+x+1);
			PrintMini(x*28+20,1,buffer,MINI_OVER);
		}
		for ( y=0; y<=j; y++ ) {
				sprintf((char*)buffer,"%4d",seltopY+y+1);
				PrintMini(1,y*8+10,buffer,MINI_OVER);
			for ( x=0; x<=i; x++ ) {
				if ( ((seltopY+y)<=dimB) && ((seltopX+x)<=dimA) ) {
					value=ReadMatrix( reg, seltopY+y, seltopX+x);
					sprintG(buffer, value, 6,RIGHT_ALIGN);
//					sprintGRS(buffer, value, 5,RIGHT_ALIGN, Norm, 6);
					PrintMini(x*28+20,y*8+10,buffer,MINI_OVER);
//					Bdisp_PutDisp_DD();
				}
			}
		}
		value=ReadMatrix( reg, selectY, selectX);
		sprintG(buffer, value,21,RIGHT_ALIGN);
		locate(1,7); Print( buffer );
		
		y = (selectY-seltopY) ;
		x = (selectX-seltopX) ;
		Bdisp_AreaReverseVRAM(x*28+20, y*8+9, x*28+43, y*8+15);	// reverse selectY line 

		Fkey_dispN(0,"Edit");

		Bdisp_PutDisp_DD();

		GetKey( &key );
		switch (key) {
			case KEY_CTRL_EXIT:
				cont=0;
				break;
		
			case KEY_CTRL_EXE:
				break;
				
			case KEY_CTRL_F1:
				value=ReadMatrix( reg, selectY, selectX);
				WriteMatrix( reg, selectY, selectX, InputNumD_full( 1, 7, 21, value));
				selectX++;
				if ( selectX > dimA ) { 
					selectX =0;
					if ( selectY < dimB ) selectY++;
				}
				break;
			case KEY_CTRL_UP:
				selectY--;
				if ( selectY < 0 ) selectY = dimB;
				break;
			case KEY_CTRL_DOWN:
				selectY++;
				if ( selectY > dimB ) selectY =0;
				break;
				
			case KEY_CTRL_RIGHT:
				selectX++;
				if ( selectX > dimA ) selectX =0;
				break;
			case KEY_CTRL_LEFT:
				selectX--;
				if ( selectX < 0 ) selectX = dimA;
				break;
				
			default:
				break;
		}
		key=MathKey( key );
		if ( key ) {
				value=ReadMatrix( reg, selectY, selectX);
				WriteMatrix( reg, selectY, selectX, InputNumD_Char( 1, 7, 21, value, key));
				selectX++;
				if ( selectX > dimA ) { 
					selectX =0;
					if ( selectY < dimB ) selectY++;
				}
		}
	}
}

//-----------------------------------------------------------------------------

void SetMatrix(int select){		// ----------- Set Matrix
	unsigned char buffer[22];
	unsigned int key;
	int	cont=1;
	int seltop=select;
	int i,j,y;
	int opNum=25;
	int dimA,dimB;
	
	while (cont) {
		Bdisp_AllClr_VRAM();
		
		if (  select<seltop ) seltop = select;
		if ( (select-seltop) > 6 ) seltop = select-6;
		if ( (opNum -seltop) < 6 ) seltop = opNum -6; 
		for ( i=0; i<7; i++ ) {
			j=seltop+i;
			buffer[0]='A'+j;
			buffer[1]='\0';
			locate( 1, 1+i); Print((unsigned char*)"Mat");
			locate( 5, 1+i); Print(buffer);
			locate(13, 1+i); Print((unsigned char*)":");
			if ( MatArySizeA[j] ) {
				sprintf((char*)buffer,"%3d",MatArySizeA[j]);
				locate(14,1+i); Print(buffer);
				locate(17,1+i); Print((unsigned char*)"\xA9");
				sprintf((char*)buffer,"%3d",MatArySizeB[j]);
				locate(18,1+i); Print(buffer);
			} else {
				locate(14,1+i); Print((unsigned char*)"None");
			}
		}

		y = (select-seltop) ;
		Bdisp_AreaReverseVRAM(0, y*8, 127, y*8+7);	// reverse select line 

		Fkey_dispR(0,"DEL");
		Fkey_dispR(1,"DelA");
		Fkey_dispR(2,"DIM");

		Bdisp_PutDisp_DD();

		GetKey( &key );
		switch (key) {
			case KEY_CTRL_EXIT:
				cont=0;
				break;
		
			case KEY_CTRL_EXE:
				EditMatrix(select);
				break;
				
			case KEY_CTRL_F1:
				if ( YesNo( "Delete Matrix?") ==0 ) break;
				DeleteMatrix(select);
				break;
			case KEY_CTRL_F2:
				if ( YesNo2( "Delete","    All Matrices?") ==0 ) break;
				DeleteMatrix(-1);
				break;
			case KEY_CTRL_F3:
			case KEY_CTRL_LEFT:
				key=SetDimension(select,&dimA,&dimB);
				if ( key==KEY_CTRL_EXIT ) break;
				if ( DimMatrix(select,dimA,dimB) ) CB_ErrMsg(NotEnoughMemoryERR);
				break;
				
			case KEY_CTRL_UP:
				select--;
				if ( select < 0 ) select = opNum;
				break;
			case KEY_CTRL_DOWN:
				select++;
				if ( select > opNum ) select =0;
				break;

			case KEY_CTRL_RIGHT:
				break;
			default:
				break;
		}
	}
}

