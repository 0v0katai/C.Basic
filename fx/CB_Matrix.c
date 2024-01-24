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
#include "CBI_interpreter.h"
#include "CB_file.h"
#include "CB_edit.h"
#include "CB_error.h"
#include "CB_Matrix.h"

//-----------------------------------------------------------------------------
// Matrix 
//-----------------------------------------------------------------------------
int		MatAryMaxbyte[MatAryMax];			// Matrix array max memory size
short	MatArySizeA[MatAryMax];				// Matrix array size
short	MatArySizeB[MatAryMax];				// Matrix array size
char	MatAryElementSize[MatAryMax];		// Matrix array Element size
double *MatAry[MatAryMax];					// Matrix array ptr*

double MatDefaultValue=0;

//-----------------------------------------------------------------------------
int ReadMatrixInt( int reg, int dimA, int dimB){
	char*	MatAryC;
	short*	MatAryW;
	int*	MatAryI;
	int value;
	switch ( MatAryElementSize[reg] ) {
		case  2:		// Vram
			dimA++; dimB++;
		case  1:
			MatAryC=(char*)MatAry[reg];			// Matrix array 1 bit
			return ( MatAryC[dimB*(((MatArySizeA[reg]-1)>>3)+1)+((dimA)>>3)] & ( 128>>(dimA&7) ) ) != 0 ;
		case  8:
			MatAryC=(char*)MatAry[reg];
			return MatAryC[dimB*MatArySizeA[(reg)]+dimA] ;			// Matrix array char
		case 16:
			MatAryW=(short*)MatAry[reg];
			return MatAryW[dimB*MatArySizeA[(reg)]+dimA] ;			// Matrix array word
		case 32:
			MatAryI=(int*)MatAry[reg];
			return MatAryI[dimB*MatArySizeA[(reg)]+dimA] ;			// Matrix array int
		case 64:
			return MatAry[reg][dimB*MatArySizeA[(reg)]+dimA] ;			// Matrix array doubl
	}
}

double ReadMatrix( int reg, int dimA, int dimB){
	char*	MatAryC;
	short*	MatAryW;
	int*	MatAryI;
	double value;
	switch ( MatAryElementSize[reg] ) {
		case 64:
			return MatAry[reg][dimB*MatArySizeA[(reg)]+dimA] ;			// Matrix array doubl
		case  2:		// Vram
			dimA++; dimB++;
		case  1:
			MatAryC=(char*)MatAry[reg];			// Matrix array 1 bit
			return ( MatAryC[dimB*(((MatArySizeA[reg]-1)>>3)+1)+((dimA)>>3)] & ( 128>>(dimA&7) ) ) != 0 ;
		case  8:
			MatAryC=(char*)MatAry[reg];
			return MatAryC[dimB*MatArySizeA[(reg)]+dimA] ;			// Matrix array char
		case 16:
			MatAryW=(short*)MatAry[reg];
			return MatAryW[dimB*MatArySizeA[(reg)]+dimA] ;			// Matrix array word
		case 32:
			MatAryI=(int*)MatAry[reg];
			return MatAryI[dimB*MatArySizeA[(reg)]+dimA] ;			// Matrix array int
	}
}

void WriteMatrixInt( int reg, int dimA, int dimB, int value){
	char*	MatAryC;
	short*	MatAryW;
	int*	MatAryI;
	int tmp;
	int mptr;
//	if ( ( (dimA) < 0 ) || ( MatArySizeA[(reg)] <= (dimA) ) ) { CB_Error(DimensionERR); return ; }	// Dimension error
//	if ( ( (dimB) < 0 ) || ( MatArySizeB[(reg)] <= (dimB) ) ) { CB_Error(DimensionERR); return ; }	// Dimension error
	switch ( MatAryElementSize[reg] ) {
		case  2:	// Vram
			dimA++; dimB++;
		case  1:
			MatAryC=(char*)MatAry[reg];					// Matrix array 1 bit
			tmp=( 128>>(dimA&7) );
			mptr=dimB*(((MatArySizeA[reg]-1)>>3)+1)+((dimA)>>3);
			if ( value ) 	MatAryC[mptr] |= tmp ;
			else	 		MatAryC[mptr] &= ~tmp ;
			break;
		case  8:
			MatAryC=(char*)MatAry[reg];
			MatAryC[dimB*MatArySizeA[reg]+dimA] = (char)value ;	// Matrix array char
			break;
		case 16:
			MatAryW=(short*)MatAry[reg];
			MatAryW[dimB*MatArySizeA[reg]+dimA] = (short)value ;	// Matrix array word
			break;
		case 32:
			MatAryI=(int*)MatAry[reg];
			MatAryI[dimB*MatArySizeA[reg]+dimA] = (int)value ;		// Matrix array int
			break;
		case 64:
			MatAry[reg][dimB*MatArySizeA[reg]+dimA]  = (double)value ;	// Matrix array double
			break;
	}
}
void WriteMatrix( int reg, int dimA, int dimB, double value){
	char*	MatAryC;
	short*	MatAryW;
	int*	MatAryI;
	int tmp;
	int mptr;
//	if ( ( (dimA) < 0 ) || ( MatArySizeA[(reg)] <= (dimA) ) ) { CB_Error(DimensionERR); return ; }	// Dimension error
//	if ( ( (dimB) < 0 ) || ( MatArySizeB[(reg)] <= (dimB) ) ) { CB_Error(DimensionERR); return ; }	// Dimension error
	switch ( MatAryElementSize[reg] ) {
		case 64:
			MatAry[reg][dimB*MatArySizeA[reg]+dimA]  = (double)value ;	// Matrix array double
			break;
		case  2:	// Vram
			dimA++; dimB++;
		case  1:
			MatAryC=(char*)MatAry[reg];					// Matrix array 1 bit
			tmp=( 128>>(dimA&7) );
			mptr=dimB*(((MatArySizeA[reg]-1)>>3)+1)+((dimA)>>3);
			if ( value ) 	MatAryC[mptr] |= tmp ;
			else	 		MatAryC[mptr] &= ~tmp ;
			break;
		case  8:
			MatAryC=(char*)MatAry[reg];
			MatAryC[dimB*MatArySizeA[reg]+dimA] = (char)value ;	// Matrix array char
			break;
		case 16:
			MatAryW=(short*)MatAry[reg];
			MatAryW[dimB*MatArySizeA[reg]+dimA] = (short)value ;	// Matrix array word
			break;
		case 32:
			MatAryI=(int*)MatAry[reg];
			MatAryI[dimB*MatArySizeA[reg]+dimA] = (int)value ;		// Matrix array int
			break;
	}
}


//-----------------------------------------------------------------------------
int Cellsum( int reg, int x, int y ){
	return ReadMatrixInt(reg,x-1,y-1)+ReadMatrixInt(reg,x-1,y)+ReadMatrixInt(reg,x-1,y+1)+ReadMatrixInt(reg,x,y-1)+ReadMatrixInt(reg,x,y+1)+ReadMatrixInt(reg,x+1,y-1)+ReadMatrixInt(reg,x+1,y)+ReadMatrixInt(reg,x+1,y+1);
}

//-----------------------------------------------------------------------------
void TransposeMatirx( int reg ) {
	int m,n;
	int dimA=MatArySizeA[reg],dimB=MatArySizeB[reg];
	int ElementSize=MatAryElementSize[reg];
	int reg2=26;	// temp mat

	if ( MatAryElementSize[reg] == 2 ) { CB_Error(ArgumentERR); return ; }	// Argument error
	
	if ( ( dimA == 1 ) || ( dimB == 1 ) ) {
		MatArySizeA[reg]=dimB;						// Matrix array size
		MatArySizeB[reg]=dimA;						// Matrix array size
		return;
	}
	
	DimMatrixSub( reg2, ElementSize, dimB, dimA );		// temp mat
	
	switch ( ElementSize ) {
		case 64:
			for ( m=0; m<dimA; m++ ) {
				for ( n=0; n<dimB; n++ ) WriteMatrix( reg2, n, m,  ReadMatrix( reg, m, n ) );
			}
			break;
		default:
			for ( m=0; m<dimA; m++ ) {
				for ( n=0; n<dimB; n++ ) WriteMatrixInt( reg2, n, m,  ReadMatrixInt( reg, m, n ) );
			}
			break;
	}
	
	DeleteMatrix(reg);							// temp -> new mat
	MatAryElementSize[reg]=ElementSize;			// Matrix array Elementsize
	MatAry[reg] = MatAry[reg2] ;				// Matrix array ptr*
	MatAryMaxbyte[reg]=MatAryMaxbyte[reg2];		// Matrix array max byte
	MatArySizeA[reg]=dimB;							// Matrix array size
	MatArySizeB[reg]=dimA;							// Matrix array size
	MatAry[reg2]=NULL;
	DeleteMatrix(reg2);
}

//-----------------------------------------------------------------------------
int DimMatrixSub( int reg, int ElementSize, int m, int n ) {
	char	*cptr;
	short	*wptr;
	int		*iptr;
	double	*dptr;
	int i;
	int dimA=m,dimB=n;
	int matsize;
	if ( ElementSize==2 ) { 						// 1 bit Vram array
		MatArySizeA[reg]=127;						// Matrix array size
		MatArySizeB[reg]= 63;						// Matrix array size
		MatAryElementSize[reg]=ElementSize;			// Matrix array Elementsize
		MatAry[reg] = (double*)(PictAry[0]);		// Matrix array ptr*
		MatAryMaxbyte[reg]=1024;					// Matrix array max byte
		return 0;
	}
	if ( ( m<1 ) || ( n<1 ) ) { CB_Error(ArgumentERR); return ErrorNo; }	// Argument error
	if ( ElementSize>1 ) {
			matsize=dimA*dimB*(ElementSize>>3);
	} else {	// 1 bit matrix
			dimA=((m-1)>>3)+1;
			matsize=dimA*dimB;
	}
	if ( ( MatAry[reg] != NULL ) && ( MatAryMaxbyte[reg] >= matsize ) && ( MatAryElementSize[reg]!=2 ) ) { // already exist
		dptr = MatAry[reg] ;						// Matrix array ptr*
		MatArySizeA[reg]=m;							// Matrix array size
		MatArySizeB[reg]=n;							// Matrix array size
		MatAryElementSize[reg]=ElementSize;			// Matrix array Elementsize
	} else {
		if ( ( MatAry[reg] != NULL ) && ( MatAryElementSize[reg]!=2 ) ) free(MatAry[reg]);	// free
		dptr = malloc( matsize );
		if( dptr == NULL ) { CB_Error(NotEnoughMemoryERR); return ErrorNo; }	// Not enough memory error
		MatArySizeA[reg]=m;							// Matrix array size
		MatArySizeB[reg]=n;							// Matrix array size
		MatAryElementSize[reg]=ElementSize;			// Matrix array Elementsize
		MatAry[reg] = dptr ;						// Matrix array ptr*
		MatAryMaxbyte[reg]=matsize;					// Matrix array max byte
	}
	memset( dptr, 0, matsize );	// initialize

	return 0;	// ok
}

int DimMatrixDefaultElementSize( int reg ) {
	switch ( reg+'A' ) {
		case 'B':
		case 'C':
			return 8;
			break;
		case 'D':
		case 'W':
			return 16;
			break;
		case 'L':
		case 'I':
			return 32;
			break;
		case 'E':
		case 'F':
			return 64;
			break;
		default:
			return CB_INT? 32:64 ;
			break;
	}
}

int DimMatrix( int reg, int m, int n) {
	char	*cptr;
	short	*wptr;
	int		*iptr;
	double	*dptr;
	int i;
	int ElementSize;
	
	ElementSize = DimMatrixDefaultElementSize( reg ) ;
	return	DimMatrixSub( reg, ElementSize, m, n );
}

//-----------------------------------------------------------------------------
void DeleteMatrix( int reg ) {
	double *ptr;
	if ( ( 0 <= reg ) && ( reg < MatAryMax ) ) {
			ptr = MatAry[reg] ;							// Matrix array ptr*
			if ( (ptr != NULL ) && ( MatAryElementSize[reg] != 2 ) ) free(ptr);
			MatAryElementSize[reg]=0;
			MatArySizeA[reg]=0;							// Matrix array size
			MatArySizeB[reg]=0;							// Matrix array size
			MatAry[reg]=NULL ;							// Matrix array ptr*
			MatAryMaxbyte[reg]=0;						// Matrix array max byte
	} else {
		for ( reg=0; reg<MatAryMax; reg++){
			ptr = MatAry[reg] ;							// Matrix array ptr*
			if ( (ptr != NULL ) && ( MatAryElementSize[reg] != 2 ) ) free(ptr);
			MatAryElementSize[reg]=0;
			MatArySizeA[reg]=0;							// Matrix array size
			MatArySizeB[reg]=0;							// Matrix array size
			MatAry[reg]=NULL ;							// Matrix array ptr*		
			MatAryMaxbyte[reg]=0;						// Matrix array max byte
		}
	}
}

//-----------------------------------------------------------------------------
void MatAryElementSizePrint( int ElementSize ) {
		switch ( ElementSize ) {
			case 1:
				Print((unsigned char*)"[1bit]");
				break;
			case 2:
				Print((unsigned char*)"[VRAM]");
				break;
			case 8:
				Print((unsigned char*)"[byte]");
				break;
			case 16:
				Print((unsigned char*)"[word]");
				break;
			case 32:
				Print((unsigned char*)"[long]");
				break;
			case 64:
				Print((unsigned char*)"[Dbl]");
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
		locate( 3,5);	 Print((unsigned char *) " bit :           ");
		sprintG(buffer,*Elsize,10,LEFT_ALIGN); locate( 9, 5); Print((unsigned char*)buffer);
		locate(12,5); MatAryElementSizePrint( *Elsize );

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
						if ( (*Elsize)== 1 ) { (*Elsize)= 2; break; }
						if ( (*Elsize)== 2 ) { (*Elsize)= 8; break; }
						if ( (*Elsize)== 8 ) { (*Elsize)=16; break; }
						if ( (*Elsize)==16 ) { (*Elsize)=32; break; }
						if ( (*Elsize)==32 ) { (*Elsize)=64; break; }
						if ( (*Elsize)==64 ) { (*Elsize)= 1; break; }
						break;
					default:
						break;
				}
				break;
			case KEY_CTRL_LEFT:
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
						if ( (*Elsize)== 1 ) { (*Elsize)=64; break; }
						if ( (*Elsize)== 2 ) { (*Elsize)= 1; break; }
						if ( (*Elsize)== 8 ) { (*Elsize)= 2; break; }
						if ( (*Elsize)==16 ) { (*Elsize)= 8; break; }
						if ( (*Elsize)==32 ) { (*Elsize)=16; break; }
						if ( (*Elsize)==64 ) { (*Elsize)=32; break; }
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
						} while ( (*Elsize!=1)&&(*Elsize!=2)&&(*Elsize!=8)&&(*Elsize!=16)&&(*Elsize!=32)&&(*Elsize!=64) ) ;
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

void InitMatSub( int reg, double value ){
	int i,j;
	int dimA,dimB;
	dimA=MatArySizeA[reg]-1;
	dimB=MatArySizeB[reg]-1;
	for (j=0; j<=dimB; j++ ) {
		for (i=0; i<=dimA; i++ ) {
			WriteMatrix( reg, i, j, value);
		}
	}
}

void InitMatIntSub( int reg, int value ){
	int i,j;
	int dimA,dimB;
	dimA=MatArySizeA[reg]-1;
	dimB=MatArySizeB[reg]-1;
	for (j=0; j<=dimB; j++ ) {
		for (i=0; i<=dimA; i++ ) {
			WriteMatrixInt( reg, i, j, value);
		}
	}
}

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

	if ( YesNo("Initialize Ok?") ) if ( ElementSize == 64 ) InitMatSub( reg,value); else InitMatIntSub( reg,(int)value); 

	return value;
}

//-----------------------------------------------------------------------------

int MatrixObjectAlign( char *buffer, unsigned int n, int digit) {		// align
	unsigned int i,j,k;
	n &= (k*2-1);
	buffer[digit]='\0';
	return n*2;
}

void NumToBin( char *buffer, unsigned int n, int digit) {
	unsigned int i,j,k=pow(2,(digit-1));
	char bins[]="01";
	n &= (k*2-1);
	for (i=0;i<digit;i++){
		j=n/k;
		buffer[i]=bins[j];
		n=n-k*j;
		k/=2;
	}
	buffer[digit]='\0';
}

void NumToHex( char *buffer, unsigned int n, int digit) {
	unsigned int i,j,k=pow(16,(digit-1));
	char hexs[]="0123456789ABCDEF";
	n &= (k*16-1);
	for (i=0;i<digit;i++){
		j=n/k;
		buffer[i]=hexs[j];
		n=n-k*j;
		k/=16;
	}
	buffer[digit]='\0';
}
void DNumToHex( char *buffer, double x, int digit) {
	char buffer2[20];
	unsigned int i;
	unsigned int n[2];
	unsigned char *dptr;
	unsigned char *iptr;
	dptr=(unsigned char *)(&x);
	iptr=(unsigned char *)(&n);
	for (i=0; i<8; i++ ) iptr[i]=dptr[i];
	NumToHex( buffer2, n[0], 8);
	for (i=0; i<8; i++ )  buffer[i]=buffer2[i];
	NumToHex( buffer2, n[1], 8);
	for (i=0; i<8; i++ ) buffer[i+8]=buffer2[i];
	buffer[digit]='\0';
}


void EditMatrix(int reg){		// ----------- Edit Matrix
	char buffer[22];
	unsigned int key;
	int	cont=1;
	int seltopY=0,seltopX=0;
	int i,j,dimA,dimB,x,y;
	int selectX=0, selectY=0;
	double value;
	int ElementSize=MatAryElementSize[ reg ];
	int dx,MaxX,MaxDX,MaxDY,adjX=3;
	int bit=0;	// 0:normal  1:bin  16~64:hex
	
	if (MatArySizeA[reg]==0) return;

	while (cont) {
		if ( MatXYmode ) {
			dimA=MatArySizeA[reg]-1;	//	X,Y
			dimB=MatArySizeB[reg]-1;
		} else {
			dimB=MatArySizeA[reg]-1;	//	m,n
			dimA=MatArySizeB[reg]-1;
		}

		MaxDY=5; if (MaxDY>dimB) MaxDY=dimB;
		
		if ( ( ElementSize == 1 ) || ( ElementSize == 2 ) ) {
			MaxDX=8; if (MaxDX>dimA) MaxDX=dimA;
			dx=13;
			MaxX=7;
			adjX=3;
		} else
		if ( ElementSize == 8 ) {
			MaxDX=6; if (MaxDX>dimA) MaxDX=dimA;
			dx=18;
			MaxX=5;
			adjX=5;
			if ( bit==1 ) {	// 8bit
				MaxDX=3; if (MaxDX>dimA) MaxDX=dimA;
				dx=36;
				MaxX=2;
				adjX=2;
			}
			if ( bit==8 ) {	// 2hex
				MaxDX=8; if (MaxDX>dimA) MaxDX=dimA;
				dx=13;
				MaxX=7;
				adjX=3;
			}
		} else
		if ( ElementSize == 16 ) {
			MaxDX=4; if (MaxDX>dimA) MaxDX=dimA;
			dx=27;
			MaxX=3;
			adjX=3;
			if ( bit==2 ) {	// 16bit
				MaxDX=0;
				dx=69;
				MaxX=0;
				adjX=0;
			}
			if ( bit==16 ) {	// 4hex
				MaxDX=5; if (MaxDX>dimA) MaxDX=dimA;
				dx=22;
				MaxX=4;
				adjX=4;
			}
		} else
		if ( ElementSize == 32 ) {
			MaxDX=4; if (MaxDX>dimA) MaxDX=dimA;
			dx=27;
			MaxX=3;
			adjX=3;
			if ( bit==32 ) {	// 8hex
				MaxDX=3; if (MaxDX>dimA) MaxDX=dimA;
				dx=36;
				MaxX=2;
				adjX=2;
			}
		} else
		if ( ElementSize == 64 ) {
			MaxDX=4; if (MaxDX>dimA) MaxDX=dimA;
			dx=27;
			MaxX=3;
			adjX=3;
			if ( bit==64 ) {	// 16hex
				MaxDX=0;
				dx=69;
				MaxX=0;
				adjX=0;
			}
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
			if ( x<8 ) {
				Bdisp_DrawLineVRAM( x*dx+20,7,x*dx+20+dx-5,7);
				if ( MaxX==7 ) 	sprintf((char*)buffer,"%2d",seltopX+x+1);
				else 			sprintf((char*)buffer,"%3d",seltopX+x+1);
				PrintMini(x*dx+16+(dx-5)/2,1,(unsigned char*)buffer,MINI_OVER);
			}
		}

		Bdisp_DrawLineVRAM( 16,8,16,14+MaxDY*8);
		x=(dimA+1)*dx+20-adjX/2 ;
		if ( dimA == seltopX+MaxX ) x=(MaxX+1)*dx+20-adjX/2 ;
		if ( x < 128 ) Bdisp_DrawLineVRAM( x, 8, x, 14+MaxDY*8);

		for ( y=0; y<=MaxDY; y++ ) {
			sprintf((char*)buffer,"%4d",seltopY+y+1);
			PrintMini(0,y*8+10,(unsigned char*)buffer,MINI_OVER);
			x=(dimA+1)*dx+20-adjX/2 ;
			if ( dimA == seltopX+MaxX ) x=(MaxX+1)*dx+20-adjX/2 ;
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
					
					if ( ( bit==0 ) && ( MaxX==7 ) ) {	sprintG(buffer, value, 2,RIGHT_ALIGN);
								PrintMini(x*dx+20,y*8+10,(unsigned char*)buffer,MINI_OVER);
					} else 
					if ( ( bit==0 ) && ( MaxX==3 ) ) {	sprintG(buffer, value, 6,RIGHT_ALIGN);
								PrintMini(x*dx+20,y*8+10,(unsigned char*)buffer,MINI_OVER);
					} else 
					if ( bit== 1 ) {	NumToBin(buffer, value, 8);
								PrintMini(x*dx+21,y*8+10,(unsigned char*)buffer,MINI_OVER);
					} else 
					if ( bit== 2 ) {	NumToBin(buffer, value, 16);
								PrintMini(x*dx+21,y*8+10,(unsigned char*)buffer,MINI_OVER);
					} else 
					if ( bit== 8 ) {	NumToHex(buffer, value, 2);
								PrintMini(x*dx+21,y*8+10,(unsigned char*)buffer,MINI_OVER);
					} else 
					if ( bit==16 ) {	NumToHex(buffer, value, 4);
								PrintMini(x*dx+22,y*8+10,(unsigned char*)buffer,MINI_OVER);
					} else 
					if ( bit==32 ) {	NumToHex(buffer, value, 8);
								PrintMini(x*dx+21,y*8+10,(unsigned char*)buffer,MINI_OVER);
					} else 
					if ( bit==64 ) {	DNumToHex(buffer, value, 16);
								PrintMini(x*dx+21,y*8+10,(unsigned char*)buffer,MINI_OVER);
					} else { 	sprintG(buffer, value, 4,RIGHT_ALIGN);
								PrintMini(x*dx+23-adjX,y*8+10,(unsigned char*)buffer,MINI_OVER);
					}
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
		Bdisp_AreaReverseVRAM(x*dx+20, y*8+9, x*dx+20+dx-5, y*8+15);	// reverse select element

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
					if ( ( selectX <= dimA ) && ( selectY <= dimB ) ) {
						value=ReadMatrix( reg, selectX, selectY);
						WriteMatrix( reg, selectX, selectY, InputNumD_full( 1, 7, 21, value));
					}
				} else {
					if ( ( selectX <= dimA ) && ( selectY <= dimB ) ) {
						value=ReadMatrix( reg, selectY, selectX);
						WriteMatrix( reg, selectY, selectX, InputNumD_full( 1, 7, 21, value));
					}
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
			case KEY_CTRL_F5:	// bin
				if ( ElementSize ==  8 ) if ( ( bit==0 ) || ( bit== 8 ) ) bit= 1; else bit=0;
				if ( ElementSize == 16 ) if ( ( bit==0 ) || ( bit==16 ) ) bit= 2; else bit=0;
				break;
			case KEY_CTRL_F6:	// hex
				if ( ElementSize ==  8 ) if ( ( bit==0 ) || ( bit== 1 ) ) bit= 8; else bit=0;
				if ( ElementSize == 16 ) if ( ( bit==0 ) || ( bit== 2 ) ) bit=16; else bit=0;
				if ( ElementSize == 32 ) if ( ( bit==0 ) || ( bit== 1 ) ) bit=32; else bit=0;
				if ( ElementSize == 64 ) if ( ( bit==0 ) || ( bit== 1 ) ) bit=64; else bit=0;
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

//-----------------------------------------------------------------------------
// CB entry Matrix 
//-----------------------------------------------------------------------------
int ElementSizeSelect( char *SRC, int reg ) {
	int c,d;
	int ElementSize;
	c =SRC[ExecPtr];
	if ( c=='.' ) {
		c =SRC[++ExecPtr];
		if (CB_INT)	ElementSize=32; else ElementSize=64;
		if ( ( c=='G' ) || ( c=='g' ) ) { ExecPtr++; ElementSize= 1; }
		if ( ( c=='P' ) || ( c=='p' ) ) { ExecPtr++; ElementSize= 1; }
		if ( ( c=='V' ) || ( c=='v' ) ) { ExecPtr++; ElementSize= 2; }
		if ( ( c=='B' ) || ( c=='b' ) ) { ExecPtr++; ElementSize= 8; }
		if ( ( c=='W' ) || ( c=='w' ) ) { ExecPtr++; ElementSize=16; }
		if ( ( c=='L' ) || ( c=='l' ) ) { ExecPtr++; ElementSize=32; }
		if ( ( c=='F' ) || ( c=='f' ) ) { ExecPtr++; ElementSize=64; }
	}
	else {
		ElementSize = DimMatrixDefaultElementSize( reg ) ;
	}
	return ElementSize;
}

void CB_MatrixInitsub( char *SRC, int *reg, int dimA, int dimB ) { 
	int c,d;
	int ElementSize;
	c =SRC[ExecPtr];
	d =SRC[ExecPtr+1];
	if ( ( c != 0x7F ) || ( d !=0x40 ) ) { CB_Error(SyntaxERR); return; }  // no Mat  Syntax error
	ExecPtr+=2;
	c =SRC[ExecPtr];
	if ( ( 'A' <= c ) && ( c <= 'Z' ) ) {
		ExecPtr++;
		*reg=c-'A';
		ElementSize=ElementSizeSelect( SRC, *reg );
		DimMatrixSub( *reg, ElementSize, dimA, dimB );
	} else { CB_Error(SyntaxERR); return; }  // Syntax error
}

void CB_MatrixInit( char *SRC ) { //	{n,m}->Dim Mat A[.B][.W][.L][.F]
	int c,d;
	int dimA,dimB,i;
	int reg;
	char	*cptr;
	short	*wptr;
	int		*iptr;
	double	*dptr;
	
	if (CB_INT)	dimA=(EvalIntsubTop( SRC )); else dimA=(EvalsubTop( SRC ));

	c=SRC[ExecPtr];
	if ( c != ',' ) { CB_Error(SyntaxERR); return; }  // Syntax error
	ExecPtr++;
	if (CB_INT)	dimB=(EvalIntsubTop( SRC )); else dimB=(EvalsubTop( SRC ));

	c=SRC[ExecPtr];
	if ( c != '}' ) { CB_Error(SyntaxERR); return; }  // Syntax error
	ExecPtr++;
	c=SRC[ExecPtr];
	if ( c != 0x0E ) { CB_Error(SyntaxERR); return; }  // Syntax error
	ExecPtr++;
	c =SRC[ExecPtr];
	d =SRC[ExecPtr+1];
	if ( ( c != 0x7F ) || ( d !=0x46 ) ) { CB_Error(SyntaxERR); return; }  // no Dim  Syntax error
	ExecPtr+=2;
	
	CB_MatrixInitsub( SRC, &reg, dimA, dimB );
}

//-----------------------------------------------------------------------------

int SkipSpcCR( char *SRC ) {
	if ( SRC[ExecPtr] == 0x0D ) ExecPtr++; // Skip [CR]
	while ( SRC[ExecPtr]==0x20 ) ExecPtr++; // Skip Space
	return SRC[ExecPtr];
}

void CB_MatrixInit2( char *SRC ) { //	[[1.2,3][4,5,6]]->Mat A[.B][.W][.L][.F]
	int c,d;
	int dimA,dimB,i;
	int reg;
	int exptr,exptr2;
	char	*cptr;
	short	*wptr;
	int		*iptr;
	double	*dptr;
	double data;
	int m,n;
	
	c=SkipSpcCR(SRC);
	if ( c != '[' ) { CB_Error(SyntaxERR); return; }  // Syntax error
	ExecPtr++;
	SkipSpcCR(SRC);
	exptr=ExecPtr;
	n=1;
	while ( 1 ) {
		if (CB_INT)	data=(EvalIntsubTop( SRC )); else data=(EvalsubTop( SRC ));
		c=SkipSpcCR(SRC);
		if ( c == ']' ) break;
		if ( c != ',' ) { CB_Error(SyntaxERR); return; }  // Syntax error
		ExecPtr++;
		SkipSpcCR(SRC);
		n++;
	}
	dimB=n;
	ExecPtr++;
	c=SkipSpcCR(SRC);
	m=1;
	if ( c != ']' ) { 
		while ( 1 ) {
			c=SkipSpcCR(SRC);
			if ( c != '[' ) { CB_Error(SyntaxERR); return; }  // Syntax error
			ExecPtr++;
			SkipSpcCR(SRC);
			n=1;
			while ( 1 ) {
				if (CB_INT)	data=(EvalIntsubTop( SRC )); else data=(EvalsubTop( SRC ));
				c=SkipSpcCR(SRC);
				if ( c == ']' ) break;
				if ( c != ',' ) { CB_Error(SyntaxERR); return; }  // Syntax error
				ExecPtr++;
				SkipSpcCR(SRC);
				n++;
			}
			if ( n != dimB ) { CB_Error(DimensionERR); return; }  // Dimension error
			m++;
			ExecPtr++;
			c=SkipSpcCR(SRC);
			if ( c == ']' ) break;
		}
	}
	dimA=m;
	ExecPtr++;

	c=SRC[ExecPtr];
	if ( c != 0x0E ) { CB_Error(SyntaxERR); return; }  // Syntax error
	ExecPtr++;

	CB_MatrixInitsub( SRC, &reg, dimA, dimB );
	if ( ErrorNo ) return ;

	exptr2=ExecPtr;
	ExecPtr=exptr;
	m=0; n=0;
	while ( m < dimA ) {
		n=0;
		while ( n < dimB ) {
			if (CB_INT)	WriteMatrixInt( reg, m, n, EvalIntsubTop( SRC ));
			else 		   WriteMatrix( reg, m, n, EvalsubTop( SRC ));
			SkipSpcCR(SRC);
			ExecPtr++;	// , skip
			SkipSpcCR(SRC);
			n++;
		}
		ExecPtr++;		// ] skip
		SkipSpcCR(SRC);
		m++;
	}
	ExecPtr=exptr2;
}

//-----------------------------------------------------------------------------

void CB_ClrMat( char *SRC ) { //	ClrMat A
	int c =SRC[ExecPtr];
	if ( ( 'A' <= c ) && ( c <= 'Z' ) ) {
		ExecPtr++;
		DeleteMatrix( c-'A' );
	}
}

void CopyMatrix( int reg, int reg2 ) {
	int m,n;
	int dimA,dimB,dimA2,dimB2;
	int ElementSize,ElementSize2;
	
	ElementSize =MatAryElementSize[reg ];
	ElementSize2=MatAryElementSize[reg2];
	dimA =MatArySizeA[reg ];
	dimA2=MatArySizeA[reg2];
	dimB =MatArySizeB[reg ];
	dimB2=MatArySizeB[reg2];

	if ( ( ElementSize == ElementSize2 ) && ( dimA == dimA2 ) && ( dimB == dimB2 ) ) {
		if ( ElementSize < 8 ) { dimA=((dimA-1)>>3)+1; ElementSize=1; } else ElementSize >>=3;
		memcpy( MatAry[reg2], MatAry[reg], ElementSize*dimA*dimB ) ;
	} else { 
		if ( ( ElementSize < 64 ) && ( ElementSize2 < 64 ) ) {
			for ( m=0; m<dimA; m++ ) {
				if ( m<dimA2 ) for ( n=0; n<dimB; n++ ) if ( n<dimB2 ) WriteMatrixInt( reg2, m, n,  ReadMatrixInt( reg, m, n ) );
			}
		} else {
			for ( m=0; m<dimA; m++ ) {
				if ( m<dimA2 ) for ( n=0; n<dimB; n++ ) if ( n<dimB2 ) WriteMatrix( reg2, m, n,  ReadMatrix( reg, m, n ) );
			}
		}
	}
}

void CB_MatCalc( char *SRC ) { //	Mat A -> Mat B  etc
	int c,d,i,m,n;
	int dim=0,dimA,dimB,dimA2,dimB2;
	int ElementSize,ElementSize2;
	int reg,reg2;
	double	*dptr, *dptr2;
	
	c =SRC[ExecPtr];
	if ( ( 'A' <= c ) && ( c <= 'Z' ) ) {
		ExecPtr++;
		reg=c-'A';
		if ( MatArySizeA[reg] == 0 ) { CB_Error(NoMatrixArrayERR); return; }	// No Matrix Array error
	} else { CB_Error(SyntaxERR); return; }	// Syntax error

	c =SRC[ExecPtr];
	if ( c == 0x0E ) {	// ->
		ExecPtr++;
		c =SRC[ExecPtr];
		if ( c != 0x7F ) { CB_Error(SyntaxERR); return; }	// Syntax error
		c =SRC[++ExecPtr];
		if ( c == 0x46 ) { 		// Mat A -> Dim Mat B[.w]
			dim=1;
			c =SRC[++ExecPtr];
			if ( c != 0x7F ) { CB_Error(SyntaxERR); return; }	// Syntax error
			c =SRC[++ExecPtr];
		}
		if ( ( c !=0x40 ) ) { CB_Error(SyntaxERR); return; }	// Syntax error
		c =SRC[++ExecPtr];
		if ( ( 'A' <= c ) && ( c <= 'Z' ) ) {
			ExecPtr++;
			reg2=c-'A';
		} else { CB_Error(SyntaxERR); return; }	// Syntax error
		if ( dim ) { 		// Mat A -> Dim Mat B[.w]
			ElementSize =MatAryElementSize[reg];
			ElementSize2=ElementSizeSelect( SRC, reg2 );
			if ( ( ElementSize == 2 ) || ( ElementSize2 == 2 ) ) { CB_Error(DimensionERR); return ; }	// Dimension error
			if ( ElementSize == ElementSize2 ) return;
			dimA =MatArySizeA[reg];
			dimB =MatArySizeB[reg];
			dimB2=dimB;
			dimA2=dimA;
			switch ( ElementSize2 ) {
				case  1:
					switch ( ElementSize ) {
						case  8:
							dimA2 *= 8;	break;
						case 16:
							dimA2 *=16;	break;
						case 32:
							dimA2 *=32;	break;
						case 64:
							dimA2 *=64;	break;
					}
					break;
				case  8:
					switch ( ElementSize ) {
						case  1:
							dimA2=((dimA2-1)/8)+1;	break;
						case 16:
							dimA2 *= 2;	break;
						case 32:
							dimA2 *= 4;	break;
						case 64:
							dimA2 *= 8;	break;
					}
					break;
				case 16:
					switch ( ElementSize ) {
						case  1:
							dimA2=((dimA2-1)/8)+1;
						case  8:
							if ( dimA2 & 1 ) { dimA2=-1; break; }
							dimA2 /= 2;	break;
						case 32:
							dimA2 *= 2;	break;
						case 64:
							dimA2 *= 4;	break;
					}
					break;
				case 32:
					switch ( ElementSize ) {
						case  1:
							dimA2=((dimA2-1)/8)+1;
						case  8:
							if ( dimA2 & 3 ) { dimA2=-1; break; }
							dimA2 /= 4;	break;
						case 16:
							if ( dimA2 & 1 ) { dimA2=-1; break; }
							dimA2 /= 2;	break;
						case 64:
							dimA2 *= 2;	break;
					}
					break;
				case 64:
					switch ( ElementSize ) {
						case  1:
							dimA2=((dimA2-1)/8)+1;
							if ( dimA2 & 7 ) { dimA2=-1; break; }
						case  8:
							dimA2 /= 8;	break;
						case 16:
							if ( dimA2 & 3 ) { dimA2=-1; break; }
							dimA2 /= 4;	break;
						case 32:
							if ( dimA2 & 1 ) { dimA2=-1; break; }
							dimA2 /= 2;	break;
					}
					break;
			}
			if ( dimA2 < 0 ) { CB_Error(DimensionERR); return ; }	// Dimension error
			if ( reg != reg2 ) { DimMatrixSub( reg2, ElementSize, dimA, dimB ); CopyMatrix( reg, reg2 ); }
			MatArySizeA[reg2]=dimA2;
			MatAryElementSize[reg2]=ElementSize2;
			
		} else {								// Mat A -> Mat B
			if ( MatArySizeA[reg2] == 0 ) { CB_Error(NoMatrixArrayERR); return; }	// No Matrix Array error
			if ( reg == reg2 ) return;
			CopyMatrix( reg, reg2 );
		}
		dspflag=0;
	} else {
		ExecPtr-=3;
		dspflag=2;
		if (CB_INT)	CBint_CurrentValue = EvalIntsubTop( SRC );
		else		CB_CurrentValue    = EvalsubTop( SRC );
	}
}

//-----------------------------------------------------------------------------
void CB_MatFill( char *SRC ) { //	Fill(value, Mat A)
	int c,d;
	int dimA,dimB,i;
	int reg;
	double	*dptr, *dptr2;
	double value;
	
	if (CB_INT)	value=(EvalIntsubTop( SRC )); else value=(EvalsubTop( SRC ));
	c =SRC[ExecPtr];
	if ( c != ',' ) { CB_Error(SyntaxERR); return; }  // Syntax error
	c =SRC[++ExecPtr];
	if ( ( c != 0x7F ) || ( SRC[ExecPtr+1]!=0x40 ) ) { CB_Error(SyntaxERR); return; }	// Syntax error
	ExecPtr+=2;
	c =SRC[ExecPtr];
	if ( ( 'A' <= c ) && ( c <= 'Z' ) ) {
		ExecPtr++;
		reg=c-'A';
		if ( MatArySizeA[reg] == 0 ) { CB_Error(NoMatrixArrayERR); return; }	// No Matrix Array error
	} else { CB_Error(SyntaxERR); return; }	// Syntax error
	c =SRC[ExecPtr];
	if ( c == ')' ) ExecPtr++;
	
	InitMatSub( reg,value);
}

//-----------------------------------------------------------------------------
void CB_MatTrn( char *SRC ) { //	Trn Mat A
	int c,d;
	int dimA,dimB,i;
	int reg;
	double	*dptr, *dptr2;
	double value;
	
	c =SRC[ExecPtr];
	if ( ( c != 0x7F ) || ( SRC[ExecPtr+1]!=0x40 ) ) { CB_Error(SyntaxERR); return; }	// Syntax error
	ExecPtr+=2;
	c =SRC[ExecPtr];
	if ( ( 'A' <= c ) && ( c <= 'Z' ) ) {
		ExecPtr++;
		reg=c-'A';
		if ( MatArySizeA[reg] == 0 ) { CB_Error(NoMatrixArrayERR); return; }	// No Matrix Array error
	} else { CB_Error(SyntaxERR); return; }	// Syntax error

	TransposeMatirx( reg );
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void MatOprandInt2( char *SRC, int reg, int *dimA, int *dimB ){ 
	int c,d;
	c=SRC[ExecPtr];
	d=SRC[ExecPtr+1];
	if ( d == ',' ) {										// [a,
		ExecPtr++ ;
		if  ( ( '1'<= c ) && ( c<='9' ) ) (*dimA)=c-'0';
		else if  ( ( 'A'<= c ) && ( c<='z' ) ) (*dimA)=REGINT[c-'A'];
	} else
	if ( d == '+' ) { 										// [a+1,
		if ( SRC[ExecPtr+3] == ',' ) {
			ExecPtr+=2 ;
			if  ( ( '1'<= c ) && ( c<='9' ) ) (*dimA)=c-'0';
			else if  ( ( 'A'<= c ) && ( c<='z' ) ) (*dimA)=REGINT[c-'A'];
			c=SRC[ExecPtr++];
			if  ( ( '1'<= c ) && ( c<='9' ) ) (*dimA)+=(c-'0');
			else if  ( ( 'A'<= c ) && ( c<='z' ) ) (*dimA)+=REGINT[c-'A'];
		}
	} else
	if ( d == '-' ) { 										// [a-1,
		if ( SRC[ExecPtr+3] == ',' ) {
			ExecPtr+=2 ;
			if  ( ( '1'<= c ) && ( c<='9' ) ) (*dimA)=c-'0';
			else if  ( ( 'A'<= c ) && ( c<='z' ) ) (*dimA)=REGINT[c-'A'];
			c=SRC[ExecPtr++];
			if  ( ( '1'<= c ) && ( c<='9' ) ) (*dimA)-=(c-'0');
			else if  ( ( 'A'<= c ) && ( c<='z' ) ) (*dimA)-=REGINT[c-'A'];
		}
	} else {
		(*dimA)=(EvalIntsubTop( SRC ));
		if ( SRC[ExecPtr] != ',' ) { CB_Error(SyntaxERR); return ; }	// Syntax error
	}
	if ( ( (*dimA) < 1 ) || ( MatArySizeA[(reg)] < (*dimA) ) ) { CB_Error(DimensionERR); return ; }	// Dimension error
	(*dimA)--;
	c=SRC[++ExecPtr];
	d=SRC[ExecPtr+1];
	if ( d == ']' ) {										//    b]
		ExecPtr++ ;
		if  ( ( '1'<= c ) && ( c<='9' ) ) (*dimB)=c-'0';
		else if  ( ( 'A'<= c ) && ( c<='z' ) ) (*dimB)=REGINT[c-'A'];
	} else
	if ( d == '+' ) { 										//      b+1]
		if ( SRC[ExecPtr+3] == ']' ) {
			ExecPtr+=2 ;
			if  ( ( '1'<= c ) && ( c<='9' ) ) (*dimB)=c-'0';
			else if  ( ( 'A'<= c ) && ( c<='z' ) ) (*dimB)=REGINT[c-'A'];
			c=SRC[ExecPtr++];
			if  ( ( '1'<= c ) && ( c<='9' ) ) (*dimB)+=(c-'0');
			else if  ( ( 'A'<= c ) && ( c<='z' ) ) (*dimB)+=REGINT[c-'A'];
		}
	} else
	if ( d == '-' ) { 										//      b-1]
		if ( SRC[ExecPtr+3] == ']' ) {
			ExecPtr+=2 ;
			if  ( ( '1'<= c ) && ( c<='9' ) ) (*dimB)=c-'0';
			else if  ( ( 'A'<= c ) && ( c<='z' ) ) (*dimB)=REGINT[c-'A'];
			c=SRC[ExecPtr++];
			if  ( ( '1'<= c ) && ( c<='9' ) ) (*dimB)-=(c-'0');
			else if  ( ( 'A'<= c ) && ( c<='z' ) ) (*dimB)-=REGINT[c-'A'];
		}
	} else {
		(*dimB)=(EvalIntsubTop( SRC ));
		if ( SRC[ExecPtr] != ']' ) { CB_Error(SyntaxERR); return ; }	// Syntax error
	}
	if ( ( (*dimB) < 1 ) || ( MatArySizeB[(reg)] < (*dimB) ) ) { CB_Error(DimensionERR); return ; }	// Dimension error
	(*dimB)--;
	ExecPtr++ ;
}
//-----------------------------------------------------------------------------
void MatOprand2( char *SRC, int reg, int *dimA, int *dimB ){ 
	int dst;
	int c,d;
	c=SRC[ExecPtr];
	d=SRC[ExecPtr+1];
	if ( d == ',' ) {										// [a,
		ExecPtr++ ;
		if  ( ( '1'<= c ) && ( c<='9' ) ) (*dimA)=c-'0';
		else if  ( ( 'A'<= c ) && ( c<='z' ) ) (*dimA)=REG[c-'A'];
	} else
	if ( d == '+' ) { 										// [a+1,
		if ( SRC[ExecPtr+3] == ',' ) {
			ExecPtr+=2 ;
			if  ( ( '1'<= c ) && ( c<='9' ) ) (*dimA)=c-'0';
			else if  ( ( 'A'<= c ) && ( c<='z' ) ) (*dimA)=REG[c-'A'];
			c=SRC[ExecPtr++];
			if  ( ( '1'<= c ) && ( c<='9' ) ) (*dimA)+=(c-'0');
			else if  ( ( 'A'<= c ) && ( c<='z' ) ) (*dimA)+=REG[c-'A'];
		}
	} else
	if ( d == '-' ) { 										// [a-1,
		if ( SRC[ExecPtr+3] == ',' ) {
			ExecPtr+=2 ;
			if  ( ( '1'<= c ) && ( c<='9' ) ) (*dimA)=c-'0';
			else if  ( ( 'A'<= c ) && ( c<='z' ) ) (*dimA)=REG[c-'A'];
			c=SRC[ExecPtr++];
			if  ( ( '1'<= c ) && ( c<='9' ) ) (*dimA)-=(c-'0');
			else if  ( ( 'A'<= c ) && ( c<='z' ) ) (*dimA)-=REG[c-'A'];
		}
	} else {
		(*dimA)=(EvalsubTop( SRC ));
		if ( SRC[ExecPtr] != ',' ) { CB_Error(SyntaxERR); return ; }	// Syntax error
	}
	if ( ( (*dimA) < 1 ) || ( MatArySizeA[(reg)] < (*dimA) ) ) { CB_Error(DimensionERR); return ; }	// Dimension error
	(*dimA)--;
	c=SRC[++ExecPtr];
	d=SRC[ExecPtr+1];
	if ( d == ']' ) {										//    b]
		ExecPtr++ ;
		if  ( ( '1'<= c ) && ( c<='9' ) ) (*dimB)=c-'0';
		else if  ( ( 'A'<= c ) && ( c<='z' ) ) (*dimB)=REG[c-'A'];
	} else
	if ( d == '+' ) { 										//      b+1]
		if ( SRC[ExecPtr+3] == ']' ) {
			ExecPtr+=2 ;
			if  ( ( '1'<= c ) && ( c<='9' ) ) (*dimB)=c-'0';
			else if  ( ( 'A'<= c ) && ( c<='z' ) ) (*dimB)=REG[c-'A'];
			c=SRC[ExecPtr++];
			if  ( ( '1'<= c ) && ( c<='9' ) ) (*dimB)+=(c-'0');
			else if  ( ( 'A'<= c ) && ( c<='z' ) ) (*dimB)+=REG[c-'A'];
		}
	} else
	if ( d == '-' ) { 										//      b-1]
		if ( SRC[ExecPtr+3] == ']' ) {
			ExecPtr+=2 ;
			if  ( ( '1'<= c ) && ( c<='9' ) ) (*dimB)=c-'0';
			else if  ( ( 'A'<= c ) && ( c<='z' ) ) (*dimB)=REG[c-'A'];
			c=SRC[ExecPtr++];
			if  ( ( '1'<= c ) && ( c<='9' ) ) (*dimB)-=(c-'0');
			else if  ( ( 'A'<= c ) && ( c<='z' ) ) (*dimB)-=REG[c-'A'];
		}
	} else {
		(*dimB)=(EvalsubTop( SRC ));
		if ( SRC[ExecPtr] != ']' ) { CB_Error(SyntaxERR); return ; }	// Syntax error
	}
	if ( ( (*dimB) < 1 ) || ( MatArySizeB[(reg)] < (*dimB) ) ) { CB_Error(DimensionERR); return ; }	// Dimension error
	(*dimB)--;
	ExecPtr++ ;
}
