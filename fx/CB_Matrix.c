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
#include "CB_edit.h"
#include "CB_error.h"

//-----------------------------------------------------------------------------
// Matrix edit
//-----------------------------------------------------------------------------
double MatDefaultValue=0;
//-----------------------------------------------------------------------------
void DeleteMatrix( int reg ) {
	double *ptr;
	if ( ( 0 <= reg ) && ( reg <= 25 ) ) {
			ptr = MatAry[reg] ;							// Matrix array ptr*
			if (ptr != NULL ) free(ptr);		
			MatAryElementSize[reg]=0;
			MatArySizeA[reg]=0;							// Matrix array size
			MatArySizeB[reg]=0;							// Matrix array size
			MatAry[reg]=NULL ;							// Matrix array ptr*
	} else {
		for ( reg=0; reg<26; reg++){
			ptr = MatAry[reg] ;							// Matrix array ptr*
			if (ptr != NULL ) free(ptr);		
			MatAryElementSize[reg]=0;
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
	int mptr=(dimA)*MatArySizeB[reg]+dimB;
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
	int mptr=(dimA)*MatArySizeB[reg]+dimB;
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
int ReadMatrixInt( int reg, int dimA, int dimB){
	char*	MatAryC;
	short*	MatAryW;
	int*	MatAryI;
	int value;
	int mptr=(dimA)*MatArySizeB[reg]+dimB;
	switch ( MatAryElementSize[reg] ) {
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
		case 8:
			value = MatAry[reg][mptr];			// Matrix array double
			break;
	}
	return value;
}
void WriteMatrixInt( int reg, int dimA, int dimB, int value){
	char*	MatAryC;
	short*	MatAryW;
	int*	MatAryI;
	int mptr=(dimA)*MatArySizeB[reg]+dimB;
	switch ( MatAryElementSize[reg] ) {
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
		case 8:
			MatAry[reg][mptr] = (double)value ;			// Matrix array double
			break;
	}
}
//-----------------------------------------------------------------------------
void MatAryElementSizePrint( int ElementSize ) {
		switch ( ElementSize ) {
			case 1:
				Print((unsigned char*)"(byte)");
				break;
			case 2:
				Print((unsigned char*)"(word)");
				break;
			case 4:
				Print((unsigned char*)"(long)");
				break;
			case 8:
				Print((unsigned char*)"(Dbl)");
				break;
		}
}

unsigned int SetDimension(int reg, int *dimA, int *dimB, int *Elsize){
	char buffer[22];
	unsigned int key;
	int	cont=1;
	int select=0;
	int y;

	PopUpWin(4);
	
	*dimA=MatArySizeA[reg];	//
	*dimB=MatArySizeB[reg];	//
	if ( *dimA==0 ) *dimA=1;
	if ( *dimB==0 ) *dimB=1;

	while (cont) {
		locate( 3,2); Print((unsigned char *)"Dimension m\xA9n");
		locate( 3,3); 
		if ( MatXYmode ) Print((unsigned char *) "  X  :           "); else Print((unsigned char *) "  m  :           ");
		sprintG(buffer,*dimA,  10,LEFT_ALIGN); locate( 9, 3); Print((unsigned char*)buffer);
		locate( 3,4); 
		if ( MatXYmode ) Print((unsigned char *) "  Y  :           "); else Print((unsigned char *) "  n  :           ");
		sprintG(buffer,*dimB,  10,LEFT_ALIGN); locate( 9, 4); Print((unsigned char*)buffer);
		locate( 3,5); Print((unsigned char *) " byte:           ");
		sprintG(buffer,*Elsize,10,LEFT_ALIGN); locate( 9, 5); Print((unsigned char*)buffer);
		locate(11,5); MatAryElementSizePrint( *Elsize ) ;

		y = select + 2 ;
		Bdisp_AreaReverseVRAM(12, y*8, 113, y*8+7);	// reverse select line 
		Bdisp_PutDisp_DD();

		GetKey( &key );
		switch (key) {
			case KEY_CTRL_EXIT:
			case KEY_CTRL_EXE:
				cont=0;
				return key;
				break;
		
			case KEY_CTRL_UP:
				select-=1;
				if ( select < 0 ) select=2;
				break;
			case KEY_CTRL_DOWN:
				select+=1;
				if ( select > 2 ) select=0;
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
					case 2: // size
						do	{
							*Elsize =InputNumD_full( 9, y, 10, (*Elsize));	// 
						} while ( (*Elsize!=1)&&(*Elsize!=2)&&(*Elsize!=4)&&(*Elsize!=8) ) ;
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
					case 2: // size
						do	{
							*Elsize =InputNumD_Char( 9, y, 10, (*Elsize), key);	// 
						} while ( (*Elsize!=1)&&(*Elsize!=2)&&(*Elsize!=4)&&(*Elsize!=8) ) ;
						break;
					default:
						break;
				}
		}

	}
	return key;
}
//-----------------------------------------------------------------------------

unsigned int GotoMatrixElement(int reg, int *m, int *n ){
	char buffer[22];
	unsigned int key;
	int	cont=1;
	int select=0;
	int y;
	int dimA,dimB;
	
	dimA=MatArySizeA[reg]-1;	//
	dimB=MatArySizeB[reg]-1;	//

	PopUpWin(3);

	while (cont) {
		locate( 3,3); Print((unsigned char *)"Goto Element");
		if ( MatXYmode ) sprintf( (char*)buffer," X(1~%3d)  ", dimA+1); else sprintf( (char*)buffer," m(1~%3d)  ", dimA+1);
		locate( 3,4); Print((unsigned char*)buffer);
		locate(13,4); Print((unsigned char *)":     ");
		sprintG(buffer,*m+1,  5,LEFT_ALIGN); locate(14, 4); Print((unsigned char*)buffer);
		if ( MatXYmode ) sprintf( (char*)buffer," Y(1~%3d)  ", dimB+1); else sprintf( (char*)buffer," n(1~%3d)  ", dimB+1);
		locate( 3,5); Print((unsigned char*)buffer);
		
		locate(13,5); Print((unsigned char *)":     ");
		sprintG(buffer,*n+1,  5,LEFT_ALIGN); locate(14, 5); Print((unsigned char*)buffer);

		y = select + 3 ;
		Bdisp_AreaReverseVRAM(12, y*8, 113, y*8+7);	// reverse select line 
		Bdisp_PutDisp_DD();

		GetKey( &key );
		switch (key) {
			case KEY_CTRL_EXIT:
			case KEY_CTRL_EXE:
				cont=0;
				return key;
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
						do {
							*m =InputNumD_full( 14, y, 5, *m+1)-1;	// 
						} while ( ( *m < 0 ) || ( *m > dimA ) ) ;
						select+=1;
						break;
					case 1: // dim n
						do {
							*n =InputNumD_full( 14, y, 5, *n+1)-1;	// 
						} while ( ( *n < 0 ) || ( *n > dimB ) ) ;
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
						do {
							*m =InputNumD_Char( 14, y, 5, *m+1, key)-1;	// 
						} while ( ( *m < 0 ) || ( *m > dimA ) ) ;
						select+=1;
						break;
					case 1: // dim n
						do {
							*n =InputNumD_Char( 14, y, 5, *n+1, key)-1;	// 
						} while ( ( *n < 0 ) || ( *n > dimB ) ) ;
						break;
					default:
						break;
				}
		}

	}
	return key;
}
//-----------------------------------------------------------------------------
double InitMatrix( int reg, double value ,int ElementSize ) {
	char buffer[32];
	unsigned int key;
	int	cont=1;
	int i,j;
	int dimA,dimB;
	PopUpWin(3);

	if (MatArySizeA[reg]==0) return;

	while (cont) {
		locate( 3,3); Print((unsigned char *)"Init Matrix array");
		locate( 3,5); Print((unsigned char *)"value:           ");
		sprintG(buffer,value,  10,LEFT_ALIGN); locate( 9, 5); Print((unsigned char*)buffer);
		locate(1,8); PrintLine((unsigned char *)" ",21);
		locate(1,8); MatAryElementSizePrint( MatAryElementSize[reg] ) ;
//		Bdisp_PutDisp_DD();

		GetKey( &key );
		switch (key) {
			case KEY_CTRL_EXIT:
				return 0 ;
				break;
			case KEY_CTRL_EXE:
				cont=0;
				break;
			case KEY_CTRL_RIGHT:
				value  =InputNumD_full( 9, 5, 10, value);	// 
				break;
			default:
				break;
		}
		key=MathKey( key );
		if ( key ) {
			value  =InputNumD_Char( 9, 5, 10, value, key);	// 
		}
	}

	if ( YesNo("Initialize Ok?") ) {
			dimA=MatArySizeA[reg]-1;
			dimB=MatArySizeB[reg]-1;
			MatAryElementSize[reg];
			for (j=0; j<=dimB; j++ ) {
				for (i=0; i<=dimA; i++ ) {
					WriteMatrix( reg, i, j, value);
				}
			}
	}
	return value;
}


//-----------------------------------------------------------------------------
int DimMatrixSub( int reg, int ElementSize, int dimA, int dimB ) {
	char	*cptr;
	short	*wptr;
	int		*iptr;
	double	*dptr;
	int i;

	if ( ( ElementSize==MatAryElementSize[reg] ) && ( MatAry[reg] != NULL ) && ( MatArySizeA[reg] >= dimA ) && ( MatArySizeB[reg] >= dimB ) ) { // already exist
		dptr = MatAry[reg] ;						// Matrix array ptr*
		MatArySizeA[reg]=dimA;						// Matrix array size
		MatArySizeB[reg]=dimB;						// Matrix array size
	} else {
		if ( MatAry[reg] != NULL ) 	free(MatAry[reg]);	// free
		dptr = malloc( dimA*dimB*ElementSize );
		if( dptr == NULL ) { ErrorNo=NotEnoughMemoryERR; ErrorPtr=ExecPtr; return ErrorNo; }	// Not enough memory error
		MatArySizeA[reg]=dimA;						// Matrix array size
		MatArySizeB[reg]=dimB;						// Matrix array size
		MatAryElementSize[reg]=ElementSize;			// Matrix array Elementsize
		MatAry[reg] = dptr ;						// Matrix array ptr*
	}
	memset( dptr, 0, dimA*dimB*ElementSize );	// initialize

	return 0;	// ok
}

int DimMatrixDefaultElementSize( int reg ) {
	switch ( reg+'A' ) {
		case 'B':
		case 'C':
			return 1;
			break;
		case 'D':
		case 'W':
			return 2;
			break;
		case 'L':
		case 'I':
			return 4;
			break;
		case 'E':
		case 'F':
			return 8;
			break;
		default:
			return CB_INT?4:8 ;
			break;
	}
}

int DimMatrix( int reg, int dimA, int dimB ) {
	char	*cptr;
	short	*wptr;
	int		*iptr;
	double	*dptr;
	int i;
	int ElementSize;
	
	ElementSize = DimMatrixDefaultElementSize( reg ) ;
	return	DimMatrixSub( reg, ElementSize, dimA, dimB );
}

//-----------------------------------------------------------------------------

void EditMatrix(int reg){		// ----------- Edit Matrix
	char buffer[22];
	unsigned int key;
	int	cont=1;
	int seltopY=0,seltopX=0;
	int i,j,dimA,dimB,x,y;
	int selectX=0, selectY=0;
	double value;
	int ElementSize=MatAryElementSize[ reg ];
	int dx,MaxX,MaxDX,MaxDY;
	
	if (MatArySizeA[reg]==0) return;

	while (cont) {
		if ( MatXYmode ) {
			dimA=MatArySizeA[reg]-1;	//
			dimB=MatArySizeB[reg]-1;	//
		} else {
			dimB=MatArySizeA[reg]-1;	//
			dimA=MatArySizeB[reg]-1;	//
		}

		MaxDY=5; if (MaxDY>dimB) MaxDY=dimB;
		
		if ( ElementSize == 1 ) {
			MaxDX=6; if (MaxDX>dimA) MaxDX=dimA;
			dx=18;
			MaxX=5;
		} else {
			MaxDX=4; if (MaxDX>dimA) MaxDX=dimA;
			dx=27;
			MaxX=3;
		}
		
		Bdisp_AllClr_VRAM();
		
		if (  selectY<seltopY ) seltopY = selectY;
		if ( (selectY-seltopY) > 4 ) seltopY = selectY-4;
		if ( (dimB -seltopY) < 4 ) seltopY = dimB -4; 
		if ( seltopY < 0 ) seltopY=0;

		if (  selectX<seltopX ) seltopX = selectX;
		if ( (selectX-seltopX) > MaxX ) seltopX = selectX-MaxX;
		if ( (dimA -seltopX) < MaxX ) seltopX = dimA -MaxX; 
		if ( seltopX < 0 ) seltopX=0;
		
		buffer[0]='A'+reg;
		buffer[1]='\0';
		locate( 1, 1); Print((unsigned char*)buffer);

		
		for ( x=0; x<=MaxDX; x++ ) { 
			Bdisp_DrawLineVRAM( x*dx+20,7,x*dx+20+dx-5,7);
			sprintf((char*)buffer,"%3d",seltopX+x+1);
			PrintMini(x*dx+27-MaxX,1,(unsigned char*)buffer,MINI_OVER);
		}

		Bdisp_DrawLineVRAM( 16,8,16,14+MaxDY*8);
		x=(dimA+1)*dx+20-MaxX/2 ;
		if ( dimA == seltopX+MaxX ) x=(MaxX+1)*dx+20-MaxX/2 ;
		if ( x < 128 ) Bdisp_DrawLineVRAM( x, 8, x, 14+MaxDY*8);

		for ( y=0; y<=MaxDY; y++ ) {
			sprintf((char*)buffer,"%4d",seltopY+y+1);
			PrintMini(0,y*8+10,(unsigned char*)buffer,MINI_OVER);
			x=(dimA+1)*dx+20-MaxX/2 ;
			if ( dimA == seltopX+MaxX ) x=(MaxX+1)*dx+20-MaxX/2 ;
			if ( seltopY   == 0    ) {	Bdisp_ClearLineVRAM( 16, 8,    16,10    ); Bdisp_DrawLineVRAM( 16,10,    18,10    ); 
					if ( x < 128 )	 {	Bdisp_ClearLineVRAM(  x, 8,     x,10    ); Bdisp_DrawLineVRAM(  x,10,   x-2,10    ); }
			}
			if ( seltopY+y == dimB ) {	Bdisp_ClearLineVRAM( 16,14+y*8,16,16+y*8); Bdisp_DrawLineVRAM( 16,14+y*8,18, 14+y*8);
					if ( x < 128 )	 {	Bdisp_ClearLineVRAM(  x,14+y*8, x,16+y*8); Bdisp_DrawLineVRAM(  x,14+y*8,x-2,14+y*8); }
			}
			for ( x=0; x<=MaxDX; x++ ) {
				if ( ((seltopY+y)<=dimB) && ((seltopX+x)<=dimA) ) {
					if ( MatXYmode ) value=ReadMatrix( reg, seltopX+x, seltopY+y);
					else			 value=ReadMatrix( reg, seltopY+y, seltopX+x);
					if ( MaxX==3 )	sprintG(buffer, value, 6,RIGHT_ALIGN);
					else 			sprintG(buffer, value, 4,RIGHT_ALIGN);
					PrintMini(x*dx+23-MaxX,y*8+10,(unsigned char*)buffer,MINI_OVER);
//					Bdisp_PutDisp_DD();
				}
			}
		}
		
		if ( ( seltopX ) )               	 PrintMini( 16,1,(unsigned char*)"\xE6\x90",MINI_OVER);	// <-
		if ( ( seltopX==0 )&&( dimA>MaxX ) ) PrintMini(122,1,(unsigned char*)"\xE6\x91",MINI_OR);	// ->

		if ( MatXYmode ) value=ReadMatrix( reg, selectX, selectY);
		else			 value=ReadMatrix( reg, selectY, selectX);
		sprintG(buffer, value,21,RIGHT_ALIGN);
		locate(1,7); Print((unsigned char*)buffer);
		
		y = (selectY-seltopY) ;
		x = (selectX-seltopX) ;
		Bdisp_AreaReverseVRAM(x*dx+20, y*8+9, x*dx+20+dx-5, y*8+15);	// reverse selectY line 

		Fkey_dispN(0,"Edit");
		Fkey_dispN(1,"Goto");
		Fkey_dispR(2,"Init");
		if ( MatXYmode ) Fkey_dispN(3,"X,Y"); else Fkey_dispN(3,"m,n"); 
		locate(16, 8); MatAryElementSizePrint( MatAryElementSize[reg] ) ;

		Bdisp_PutDisp_DD();

		GetKey( &key );
		switch (key) {
			case KEY_CTRL_EXIT:
				cont=0;
				break;
		
			case KEY_CTRL_EXE:
				break;
				
			case KEY_CTRL_F1:
				locate(1,8); PrintLine((unsigned char *)" ",21);
				locate(1,8); MatAryElementSizePrint( MatAryElementSize[reg] ) ;
				if ( MatXYmode ) {
					value=ReadMatrix( reg, selectX, selectY);
					WriteMatrix( reg, selectX, selectY, InputNumD_full( 1, 7, 21, value));
				} else {
					value=ReadMatrix( reg, selectY, selectX);
					WriteMatrix( reg, selectY, selectX, InputNumD_full( 1, 7, 21, value));
				}
				selectX++;
				if ( selectX > dimA ) { selectX = dimA;
					if ( selectY < dimB ) { selectY++; selectX =0; }
				}
				break;
			case KEY_CTRL_F2:
				if ( MatXYmode ) GotoMatrixElement( reg, &selectX, &selectY );
				else			 GotoMatrixElement( reg, &selectY, &selectX );
				break;
			case KEY_CTRL_F3:
				MatDefaultValue = InitMatrix( reg, MatDefaultValue ,MatAryElementSize[reg] ) ;
				break;
			case KEY_CTRL_F4:
				MatXYmode=1-MatXYmode;
				i=selectX; selectX=selectY; selectY=i;
				i=seltopX; seltopX=seltopY; seltopY=i;
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
				if ( selectX > dimA ) { 
					selectX = 0;
					selectY++;
					if ( selectY > dimB ) selectY =0;
				}
				break;
			case KEY_CTRL_LEFT:
				selectX--;
				if ( selectX < 0 ) {
					selectX = dimA;
					selectY--;
					if ( selectY < 0 ) selectY = dimB;
				}
				break;
				
			default:
				break;
		}
		key=MathKey( key );
		if ( key ) {
				locate(1,8); PrintLine((unsigned char *)" ",21);
				locate(1,8); MatAryElementSizePrint( MatAryElementSize[reg] ) ;
				if ( MatXYmode ) {
					value=ReadMatrix( reg, selectX, selectY);
					WriteMatrix( reg, selectX, selectY, InputNumD_Char( 1, 7, 21, value, key));
				} else {
					value=ReadMatrix( reg, selectY, selectX);
					WriteMatrix( reg, selectY, selectX, InputNumD_Char( 1, 7, 21, value, key));
				}
				selectX++;
				if ( selectX > dimA ) { selectX = dimA;
					if ( selectY < dimB ) { selectY++; selectX =0; }
				}
		}
	}
}

//-----------------------------------------------------------------------------

int SetMatrix(int select){		// ----------- Set Matrix
	char buffer[22];
	unsigned int key;
	int	cont=1;
	int seltop=select;
	int i,j,y,len;
	int opNum=25;
	int dimA,dimB,ElementSize;

	Cursor_SetFlashMode(0); 		// cursor flashing off

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
			locate( 5, 1+i); Print((unsigned char*)buffer);
			locate(13, 1+i); Print((unsigned char*)":");
			locate( 6, 1+i); MatAryElementSizePrint( MatAryElementSize[j] ) ;
			if ( MatArySizeA[j] ) {
				sprintf((char*)buffer,"%3d",MatArySizeA[j]);
				locate(14,1+i); Print((unsigned char*)buffer);
				len=strlen((char*)buffer)-3;
				locate(17+len,1+i); Print((unsigned char*)"\xA9");
				if (len) sprintf((char*)buffer,"%2d",MatArySizeB[j]);
				else	 sprintf((char*)buffer,"%3d",MatArySizeB[j]);
				locate(18+len,1+i); Print((unsigned char*)buffer);
			} else {
				locate(14,1+i); Print((unsigned char*)"None");
			}
		}

		y = (select-seltop) ;
		Bdisp_AreaReverseVRAM(0, y*8, 127, y*8+7);	// reverse select line 

		Fkey_dispR(0,"DEL");
		Fkey_dispR(1,"DelA");
		Fkey_dispR(2,"DIM");
		Fkey_dispR(3,"Init");
		if ( MatXYmode ) Fkey_dispN(4,"X,Y"); else Fkey_dispN(4,"m,n"); 

		Bdisp_PutDisp_DD();
		
		ElementSize=MatAryElementSize[select];

		GetKey( &key );
		switch (key) {
			case KEY_CTRL_EXIT:
				cont=0;
				break;
		
			case KEY_CTRL_EXE:
				if ( ElementSize==0 ) {	ElementSize = DimMatrixDefaultElementSize(select) ;
					key=SetDimension(select,&dimA,&dimB,&ElementSize);
					if ( key==KEY_CTRL_EXIT ) break;
					if ( DimMatrixSub(select,ElementSize,dimA,dimB) ) CB_ErrMsg(NotEnoughMemoryERR);
				}
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
				if ( ElementSize==0 ) 	ElementSize = DimMatrixDefaultElementSize(select) ;
				key=SetDimension(select,&dimA,&dimB,&ElementSize);
				if ( key==KEY_CTRL_EXIT ) break;
				if ( DimMatrixSub(select,ElementSize,dimA,dimB) ) CB_ErrMsg(NotEnoughMemoryERR);
				EditMatrix(select);
				break;
			case KEY_CTRL_F4:
				MatDefaultValue = InitMatrix( select, MatDefaultValue ,ElementSize ) ;
				break;
			case KEY_CTRL_F5:
				MatXYmode=1-MatXYmode;
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
	return select;
}

