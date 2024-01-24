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
#include "CBI_Eval.h"
#include "CB_interpreter.h"
#include "CBI_interpreter.h"
#include "CB_file.h"
#include "CB_edit.h"
#include "CB_error.h"
#include "CB_Matrix.h"
#include "CB_Str.h"

//-----------------------------------------------------------------------------
// Matrix 
//-----------------------------------------------------------------------------
//int	MatAryMaxbyte[MatAryMax];			// Matrix array max memory size
//char	MatAryBase[MatAryMax]				// Matrix array base 0 or 1
//char	MatAryElementSize[MatAryMax];		// Matrix array Element size
//short	MatArySizeA[MatAryMax];				// Matrix array size
//short	MatAry[reg].SizeB[MatAryMax];				// Matrix array size
//double *MatAry[MatAryMax];				// Matrix array ptr*

matary MatAry[MatAryMax];

double MatDefaultValue=0;
char	MatBaseDefault=1;
char	MatBase=1;
//----------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//int MatrixObjectAlign4e( unsigned int n ){ return n; }	// align +4byte
//int MatrixObjectAlign4f( unsigned int n ){ return n; }	// align +4byte
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
char * MatrixPtr( int reg, int m, int n ){		// base:0  0-    base:1 1-
	char*	MatAryC;
	short*	MatAryW;
	int*	MatAryI;
	double*	MatAryD;
	int		dimB=MatAry[reg].SizeB;
	int		base=MatAry[reg].Base;
	m-=base;
	n-=base;
	switch ( MatAry[reg].ElementSize ) {
		case  2:	// Vram
//			m+=base;
//			n+=base;
		case  1:
			MatAryC=(char*)MatAry[reg].Adrs;
			return  (char *)(MatAryC+((MatAry[reg].SizeA-1)>>3)*n+(m>>3));
		case  8:
			MatAryC=(char*)MatAry[reg].Adrs;
			return  (char *)(MatAryC+dimB*m+n);
		case 16:
			MatAryW=(short*)MatAry[reg].Adrs;
			return  (char *)(MatAryW+dimB*m+n);
		case 32:
			MatAryI=(int*)MatAry[reg].Adrs;
			return  (char *)(MatAryI+dimB*m+n);
		case 64:
			MatAryD=(double*)MatAry[reg].Adrs;
			return  (char *)(MatAryD+dimB*m+n);
	}
}

int	MatrixSize( int reg, int sizeA, int sizeB ) {	// size 1-
	int ElementSize =MatAry[reg].ElementSize;
	switch ( ElementSize ) {
		case  2:	// Vram
		case  1:
			sizeA=((sizeA-1)>>3)+1;
			ElementSize=1;
			break;
		default:
			ElementSize >>=3;
	}
	return ElementSize*sizeA*sizeB  ;
}

//-----------------------------------------------------------------------------
int Cellsum( int reg, int x, int y ){	// 0-
	return ReadMatrixInt(reg,x-1,y-1)+ReadMatrixInt(reg,x-1,y)+ReadMatrixInt(reg,x-1,y+1)+ReadMatrixInt(reg,x,y-1)+ReadMatrixInt(reg,x,y+1)+ReadMatrixInt(reg,x+1,y-1)+ReadMatrixInt(reg,x+1,y)+ReadMatrixInt(reg,x+1,y+1);
}

//-----------------------------------------------------------------------------
void TransposeMatirx( int reg ) {
	int m,n;
	int dimA=MatAry[reg].SizeA, dimB=MatAry[reg].SizeB;
	int ElementSize=MatAry[reg].ElementSize;
	int reg2=26;	// temp mat
	int base=MatAry[reg].Base;

	if ( 2 == ElementSize ) { CB_Error(ArgumentERR); return ; }	// Argument error
	
	if ( ( dimA == 1 ) || ( dimB == 1 ) ) {
		MatAry[reg].SizeA=dimB;						// Matrix array size
		MatAry[reg].SizeB=dimA;						// Matrix array size
		return;
	}
	
	DimMatrixSub( reg2, ElementSize, dimB, dimA, base );		// temp mat
	
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
	MatAry[reg].ElementSize = ElementSize;				// Matrix array Elementsize
	MatAry[reg].Adrs        = MatAry[reg2].Adrs ;		// Matrix array ptr*
	MatAry[reg].Maxbyte     = MatAry[reg2].Maxbyte;		// Matrix array max byte
	MatAry[reg].SizeA       = dimB;						// Matrix array size
	MatAry[reg].SizeB       = dimA;						// Matrix array size
	MatAry[reg].Base        = base;						// Matrix array base
	MatAry[reg2].Adrs       = NULL;
	DeleteMatrix(reg2);
}

//-----------------------------------------------------------------------------
int DimMatrixSub( int reg, int ElementSize, int m, int n , int base ) {	// 1-
	char	*cptr;
	short	*wptr;
	int		*iptr;
	double	*dptr;
	int i;
	int matsize;
	if ( ( 2<=ElementSize )&&( ElementSize<=4 ) ) { 						// 1 bit Vram array
		if ( ( m != 128 ) || ( n != 64 ) ) { CB_Error(ArraySizeERR); return; }	// Illegal Ary size
		switch ( ElementSize ) {
			case 2:
				MatAry[reg].Adrs        = (double*)(PictAry[0]);	// Matrix array ptr*  VRAM
				break;
			case 3:
				MatAry[reg].Adrs        = (double*)(TVRAM);			// Matrix array ptr*  Text VRAM
				break;
			case 4:
				MatAry[reg].Adrs        = (double*)(GVRAM);			// Matrix array ptr*  Graphic VRAM
				break;
		}
		MatAry[reg].SizeA       =128;						// Matrix array size
		MatAry[reg].SizeB       = 64;						// Matrix array size
		MatAry[reg].ElementSize = 2;						// Matrix array Elementsize  2:VRAM
		MatAry[reg].Maxbyte     =1024;						// Matrix array max byte
		MatAry[reg].Base        = 0;						// Matrix array base
		return 0;
	}
	if ( ( m<1 ) || ( n<1 ) ) { CB_Error(ArgumentERR); return ErrorNo; }	// Argument error
	if ( ElementSize>1 ) {
			matsize=m*n*(ElementSize>>3);
	} else {	// 1 bit matrix
			matsize=( ((m-1)>>3)+1 )*n;
	}
	matsize = (matsize+7) & 0xFFFFFFF8;	// 8byte align
	if ( ( MatAry[reg].Adrs != NULL ) && ( MatAry[reg].Maxbyte >= matsize ) && ( MatAry[reg].ElementSize!=2 ) ) { // already exist
		dptr = MatAry[reg].Adrs ;							// Matrix array ptr*
		MatAry[reg].SizeA       = m;						// Matrix array size
		MatAry[reg].SizeB       = n;						// Matrix array size
		MatAry[reg].ElementSize = ElementSize;				// Matrix array Elementsize
		MatAry[reg].Base        = base;						// Matrix array base
	} else {
		if ( ( MatAry[reg].Adrs != NULL ) && ( MatAry[reg].ElementSize != 2 ) ) HiddenRAM_freeMat( reg );	// free
		dptr = HiddenRAM_mallocMat( matsize );
		if( dptr == NULL ) { CB_Error(NotEnoughMemoryERR); return ErrorNo; }	// Not enough memory error
		MatAry[reg].SizeA       = m;						// Matrix array size
		MatAry[reg].SizeB       = n;						// Matrix array size
		MatAry[reg].ElementSize = ElementSize;				// Matrix array Elementsize
		MatAry[reg].Adrs        = dptr ;					// Matrix array ptr*
		MatAry[reg].Maxbyte     = matsize;					// Matrix array max byte
		MatAry[reg].Base        = base;						// Matrix array base
	}
	memset( dptr, 0, matsize );	// initialize

	return 0;	// ok
}

int DimMatrixDefaultElementSize( int reg ) {
//	switch ( reg+'A' ) {
//		case 'B':
//		case 'C':
//		case 's':
//			return 8;
//			break;
//		case 'D':
//		case 'W':
//			return 16;
//			break;
//		case 'L':
//		case 'I':
//			return 32;
//			break;
//		case 'E':
//		case 'F':
//			return 64;
//			break;
//		default:
			return CB_INT? 32:64 ;
//			break;
//	}
}

int DimMatrix( int reg, int dimA, int dimB, int base ) {
	char	*cptr;
	short	*wptr;
	int		*iptr;
	double	*dptr;
	int i;
	int ElementSize;
	
	ElementSize = DimMatrixDefaultElementSize( reg ) ;
	return	DimMatrixSub( reg, ElementSize, dimA, dimB, base );
}

//-----------------------------------------------------------------------------
void DeleteMatrix( int reg ) {
	double *ptr;
	if ( ( 0 <= reg ) && ( reg < MatAryMax ) ) {
			ptr = MatAry[reg].Adrs ;					// Matrix array ptr*
			if ( (ptr != NULL ) && ( MatAry[reg].ElementSize != 2 ) ) HiddenRAM_freeMat( reg );
			MatAry[reg].SizeA       = 0;				// Matrix array size
			MatAry[reg].SizeB       = 0;				// Matrix array size
			MatAry[reg].ElementSize = 0;				// Matrix array Elementsize
			MatAry[reg].Adrs        = NULL ;			// Matrix array ptr*
			MatAry[reg].Maxbyte     = 0;				// Matrix array max byte
	} else {
		for ( reg=0; reg<MatAryMax; reg++){
			ptr = MatAry[reg].Adrs ;					// Matrix array ptr*
			if ( (ptr != NULL ) && ( MatAry[reg].ElementSize != 2 ) ) HiddenRAM_freeMat( reg );
			MatAry[reg].SizeA       = 0;				// Matrix array size
			MatAry[reg].SizeB       = 0;				// Matrix array size
			MatAry[reg].ElementSize = 0;				// Matrix array Elementsize
			MatAry[reg].Adrs        = NULL ;			// Matrix array ptr*
			MatAry[reg].Maxbyte     = 0;				// Matrix array max byte
		}
		HiddenRAM_MatTopPtr = HiddenRAM_End;
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

unsigned int SetDimension(int reg, int *dimA, int *dimB, int *Elsize, int *base){	// 1-
	char buffer[22];
	unsigned int key;
	int	cont=1;
	int select=0;
	int y;

	*base=MatBase;

	PopUpWin(5);
	
	*dimA=MatAry[reg].SizeA;	//
	*dimB=MatAry[reg].SizeB;	//
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
		locate( 3,6);	 Print((unsigned char *) "base :           ");
		sprintG(buffer,*base,  10,LEFT_ALIGN); locate( 9, 6); Print((unsigned char*)buffer);

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
				if ( select < 0 ) select=3;
				break;
			case KEY_CTRL_DOWN:
				select+=1;
				if ( select > 3 ) select=0;
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
					case 3: // base
						*base = 1;
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
					case 3: // base
						*base = 0;
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
					case 3: // base
						do	{
							*base =InputNumD_Char( 9, y, 10, *base, key);	// 
						} while ( (*base!=0)&&(*base!=1) ) ;
						break;
					default:
						break;
				}
		}

	}
	return key;
}
//-----------------------------------------------------------------------------

unsigned int GotoMatrixElement(int reg, int *m, int *n ){	// base:0  0-    base:1 1-
	char buffer[22];
	unsigned int key;
	int	cont=1;
	int select=0;
	int y;
	int dimA,dimB;
	int base=MatAry[reg].Base;
	
	dimA=MatAry[reg].SizeA-1+base;	//
	dimB=MatAry[reg].SizeB-1+base;	//

	PopUpWin(3);

	while (cont) {
		locate( 3,3); Print((unsigned char *)"Goto Element");
		if ( MatXYmode ) sprintf( (char*)buffer," X(%d~%3d)  ", base, dimA+base); else sprintf( (char*)buffer," m(%d~%3d)  ", base, dimA+base);
		locate( 3,4); Print((unsigned char*)buffer);
		locate(13,4); Print((unsigned char *)":     ");
		sprintG(buffer,*m+1,  5,LEFT_ALIGN); locate(14, 4); Print((unsigned char*)buffer);
		if ( MatXYmode ) sprintf( (char*)buffer," Y(%d~%3d)  ", base, dimB+base); else sprintf( (char*)buffer," n(%d~%3d)  ", base, dimB+base);
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
						} while ( ( *m < base ) || ( *m > dimA+base ) ) ;
						select+=1;
						break;
					case 1: // dim n
						do {
							*n =InputNumD_full( 14, y, 5, *n+1)-1;	// 
						} while ( ( *n < base ) || ( *n > dimB+base ) ) ;
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
						} while ( ( *m < base ) || ( *m > dimA+base ) ) ;
						select+=1;
						break;
					case 1: // dim n
						do {
							*n =InputNumD_Char( 14, y, 5, *n+1, key)-1;	// 
						} while ( ( *n < base ) || ( *n > dimB+base ) ) ;
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
	int base=MatAry[reg].Base;
	dimA=MatAry[reg].SizeA+base;
	dimB=MatAry[reg].SizeB+base;
	for (j=base; j<dimB; j++ ) {
		for (i=base; i<dimA; i++ ) {
			WriteMatrix( reg, i, j, value);
		}
	}
}

void InitMatIntSub( int reg, int value ){
	int i,j;
	int dimA,dimB;
	int base=MatAry[reg].Base;
	dimA=MatAry[reg].SizeA+base;
	dimB=MatAry[reg].SizeB+base;
	for (j=base; j<dimB; j++ ) {
		for (i=base; i<dimA; i++ ) {
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

	if (MatAry[reg].SizeA==0) return;

	while (cont) {
		locate( 3,3); Print((unsigned char *)"Init Matrix array");
		locate( 3,5); Print((unsigned char *)"value:           ");
		sprintG(buffer,value,  10,LEFT_ALIGN); locate( 9, 5); Print((unsigned char*)buffer);
		locate(1,8); PrintLine((unsigned char *)" ",21);
		locate(1,8); MatAryElementSizePrint( MatAry[reg].ElementSize ) ;
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

void MatNumToExpBuf( double value, int bit ){	// value -> ExpBuffer
	int eng=ENG;
	ExpBuffer[0]='0';
	ExpBuffer[1]='x';
	if ( bit== 1 ) {	NumToBin(ExpBuffer+2, value, 8); 	ExpBuffer[1]='b';
	} else 
	if ( bit== 2 ) {	NumToBin(ExpBuffer+2, value, 16);	ExpBuffer[1]='b';
	} else 
	if ( bit== 8 ) {	NumToHex(ExpBuffer+2, value, 2);
	} else 
	if ( bit==16 ) {	NumToHex(ExpBuffer+2, value, 4);
	} else 
	if ( bit==32 ) {	NumToHex(ExpBuffer+2, value, 8);
	} else 
	if ( bit==64 ) {	DNumToHex(ExpBuffer+2, value, 16);
	} else { 
		if (ENG==3) ENG=0;
		sprintG(ExpBuffer, value, 21,LEFT_ALIGN);
		ENG=eng;
	}
}

void EditMatrix(int reg){		// ----------- Edit Matrix
	char buffer[22];
	unsigned int key;
	int	cont=1;
	int seltopY=0,seltopX=0;
	int i,j,dimA,dimB,x,y;
	int selectX=0, selectY=0;
	double value;
	int ElementSize=MatAry[reg].ElementSize;
	int dx,MaxX,MaxDX,MaxDY,adjX=3;
	int bit=0;	// 0:normal  1:bin  16~64:hex
	int base=MatAry[reg].Base;
	
	if (MatAry[reg].SizeA==0) return;

	while (cont) {
		if ( MatXYmode ) {
			dimA=MatAry[reg].SizeA-1;	//	X,Y
			dimB=MatAry[reg].SizeB-1;
		} else {
			dimB=MatAry[reg].SizeA-1;	//	m,n
			dimA=MatAry[reg].SizeB-1;
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
				if ( MaxX==7 ) 	sprintf((char*)buffer,"%2d",seltopX+x+base);
				else 			sprintf((char*)buffer,"%3d",seltopX+x+base);
				PrintMini(x*dx+16+(dx-5)/2,1,(unsigned char*)buffer,MINI_OVER);
			}
		}

		Bdisp_DrawLineVRAM( 16,8,16,14+MaxDY*8);
		x=(dimA+1)*dx+20-adjX/2 ;
		if ( dimA == seltopX+MaxX ) x=(MaxX+1)*dx+20-adjX/2 ;
		if ( x < 128 ) Bdisp_DrawLineVRAM( x, 8, x, 14+MaxDY*8);

		for ( y=0; y<=MaxDY; y++ ) {
			sprintf((char*)buffer,"%4d",seltopY+y+base);
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
					if ( MatXYmode ) value=ReadMatrix( reg, seltopX+x+base, seltopY+y+base);
					else			 value=ReadMatrix( reg, seltopY+y+base, seltopX+x+base);
					
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

		if ( MatXYmode ) value=ReadMatrix( reg, selectX+base, selectY+base);
		else			 value=ReadMatrix( reg, selectY+base, selectX+base);
		sprintG(buffer, value,21,RIGHT_ALIGN);
		locate(1,7); Print((unsigned char*)buffer);
		
		y = (selectY-seltopY) ;
		x = (selectX-seltopX) ;
		Bdisp_AreaReverseVRAM(x*dx+20, y*8+9, x*dx+20+dx-5, y*8+15);	// reverse select element

		Fkey_dispN(0,"Edit");
		Fkey_dispN(1,"Goto");
		Fkey_dispR(2,"Init");
		if ( MatXYmode ) Fkey_dispN(3,"X,Y"); else Fkey_dispN(3,"m,n"); 
		locate(16, 8); MatAryElementSizePrint( MatAry[reg].ElementSize ) ;

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
				locate(1,8); MatAryElementSizePrint( MatAry[reg].ElementSize ) ;
				if ( MatXYmode ) {
					if ( ( selectX <= dimA ) && ( selectY <= dimB ) ) {
						value=ReadMatrix( reg, selectX+base, selectY+base);
						MatNumToExpBuf( value, bit );	// value -> ExpBuffer
						WriteMatrix( reg, selectX+base, selectY+base, InputNumD_fullsub( 1, 7, 21, value));
					}
				} else {
					if ( ( selectX <= dimA ) && ( selectY <= dimB ) ) {
						value=ReadMatrix( reg, selectY+base, selectX+base);
						MatNumToExpBuf( value, bit );	// value -> ExpBuffer
						WriteMatrix( reg, selectY+base, selectX+base, InputNumD_fullsub( 1, 7, 21, value));
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
				MatDefaultValue = InitMatrix( reg, MatDefaultValue ,MatAry[reg].ElementSize ) ;
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
				locate(1,8); MatAryElementSizePrint( MatAry[reg].ElementSize ) ;
				if ( MatXYmode ) {
					value=ReadMatrix( reg, selectX+base, selectY+base);
					WriteMatrix( reg, selectX+base, selectY+base, InputNumD_Char( 1, 7, 21, value, key));
				} else {
					value=ReadMatrix( reg, selectY+base, selectX+base);
					WriteMatrix( reg, selectY+base, selectX+base, InputNumD_Char( 1, 7, 21, value, key));
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
	int small=0;
	int base;
	
	if (select>=32) { small=32; select-=32; }

	Cursor_SetFlashMode(0); 		// cursor flashing off

	while (cont) {
		Bdisp_AllClr_VRAM();
		
		if (  select<seltop ) seltop = select;
		if ( (select-seltop) > 6 ) seltop = select-6;
		if ( (opNum -seltop) < 6 ) seltop = opNum -6; 
		for ( i=0; i<7; i++ ) {
			j=seltop+i+small;
			buffer[0]='A'+j;
			buffer[1]='\0';
			locate( 1, 1+i); Print((unsigned char*)"Mat");
			locate( 5, 1+i); Print((unsigned char*)buffer);
			locate(13, 1+i); Print((unsigned char*)":");
			locate( 6, 1+i); MatAryElementSizePrint( MatAry[j].ElementSize ) ;
			if ( MatAry[j].SizeA ) {
				sprintf((char*)buffer,"%3d",MatAry[j].SizeA);
				locate(14,1+i); Print((unsigned char*)buffer);
				len=strlen((char*)buffer)-3;
				locate(17+len,1+i); Print((unsigned char*)"\xA9");
				if (len) sprintf((char*)buffer,"%2d",MatAry[j].SizeB);
				else	 sprintf((char*)buffer,"%3d",MatAry[j].SizeB);
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
		Fkey_dispN(5, "A<>a");

		Bdisp_PutDisp_DD();
		
		ElementSize=MatAry[select+small].ElementSize;

		GetKey( &key );
		switch (key) {
			case KEY_CTRL_EXIT:
				cont=0;
				break;
		
			case KEY_CTRL_EXE:
				if ( ElementSize==0 ) {	ElementSize = DimMatrixDefaultElementSize(select+small) ;
					key=SetDimension(select+small, &dimA, &dimB, &ElementSize, &base);
					if ( key==KEY_CTRL_EXIT ) break;
					if ( DimMatrixSub(select+small, ElementSize, dimA, dimB, base ) ) CB_ErrMsg(NotEnoughMemoryERR);
				}
				EditMatrix(select+small);
				break;
				
			case KEY_CTRL_F1:
				if ( YesNo( "Delete Matrix?") ==0 ) break;
				DeleteMatrix(select+small);
				break;
			case KEY_CTRL_F2:
				if ( YesNo2( "Delete","    All Matrices?") ==0 ) break;
				DeleteMatrix(-1);
				break;
			case KEY_CTRL_F3:
			case KEY_CTRL_LEFT:
				if ( ElementSize==0 ) 	ElementSize = DimMatrixDefaultElementSize(select) ;
				key=SetDimension(select+small, &dimA, &dimB, &ElementSize, &base);
				if ( key==KEY_CTRL_EXIT ) break;
				if ( DimMatrixSub(select+small, ElementSize, dimA, dimB, base ) ) CB_ErrMsg(NotEnoughMemoryERR);
				EditMatrix(select+small);
				break;
			case KEY_CTRL_F4:
				MatDefaultValue = InitMatrix( select+small, MatDefaultValue ,ElementSize ) ;
				break;
			case KEY_CTRL_F5:
				MatXYmode=1-MatXYmode;
				break;
			case KEY_CTRL_F6:
				small=32-small;
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
	if ( small ) select+=32;
	return select;
}

//-----------------------------------------------------------------------------
// CB entry Matrix 
//-----------------------------------------------------------------------------
int ElementSizeSelect( char *SRC, int reg, int *base ) {
	int c,d;
	int ElementSize;
	*base=MatBase;
	c =SRC[ExecPtr];
	if ( c=='.' ) {
		c =SRC[++ExecPtr];
		if (CB_INT)	ElementSize=32; else ElementSize=64;
		if ( ( c=='0' ) || ( c=='1' ) ) { c=SRC[++ExecPtr]; *base = c-'0' ; }
		if ( ( c=='P' ) || ( c=='p' ) ) { ExecPtr++; ElementSize= 1; }
		if ( ( c=='V' ) || ( c=='v' ) ) { ExecPtr++; ElementSize= 2;
			c =SRC[ExecPtr];
			if ( ( c=='T' ) || ( c=='t' ) ) { ExecPtr++; ElementSize= 3; } // text VRAM
			if ( ( c=='G' ) || ( c=='g' ) ) { ExecPtr++; ElementSize= 4; } // GraphicVRAM
		}
		if ( ( c=='B' ) || ( c=='b' ) ) { ExecPtr++; ElementSize= 8; }
		if ( ( c=='W' ) || ( c=='w' ) ) { ExecPtr++; ElementSize=16; }
		if ( ( c=='L' ) || ( c=='l' ) ) { ExecPtr++; ElementSize=32; }
		if ( ( c=='F' ) || ( c=='f' ) ) { ExecPtr++; ElementSize=64; }
		c =SRC[ExecPtr];
		if ( ( c=='0' ) || ( c=='1' ) ) { 
			*base = c-'0' ;
			ExecPtr++;
		}
	}
	else {
		ElementSize = DimMatrixDefaultElementSize( reg ) +0x100 ;
	}
	return ElementSize;
}

void CB_MatrixInitsubNoMat( char *SRC, int *reg, int dimA, int dimB , int ElementSize ) { 	// 1-
	int c;
	int base=MatBase;
	c =SRC[ExecPtr];
	if ( ( 'A'<=c )&&( c<='z' ) ) {
		ExecPtr++;
		*reg=c-'A';
		if ( ElementSize<1 ) ElementSize=ElementSizeSelect( SRC, *reg , &base) & 0xFF;
		DimMatrixSub( *reg, ElementSize, dimA, dimB, base);
	} else { CB_Error(SyntaxERR); return; }  // Syntax error
}
void CB_MatrixInitsub( char *SRC, int *reg, int dimA, int dimB , int ElementSize ) { 	// 1-
	int c,d;
	int base=MatBase;
	c =SRC[ExecPtr];
	d =SRC[ExecPtr+1];
	if ( ( c != 0x7F ) || ( d !=0x40 ) ) { CB_Error(SyntaxERR); return; }  // no Mat  Syntax error
	ExecPtr+=2;
	CB_MatrixInitsubNoMat( SRC, &(*reg), dimA, dimB, ElementSize );
}

void CB_MatrixInit( char *SRC ) { //	{n,m}->Dim Mat A[.B][.W][.L][.F]
	int c,d;
	int dimA,dimB,i;
	int reg;
	char	*cptr;
	short	*wptr;
	int		*iptr;
	double	*dptr;
	
	dimA=CB_EvalInt( SRC );
	c=SRC[ExecPtr];
	if ( c != ',' ) { CB_Error(SyntaxERR); return; }  // Syntax error
	ExecPtr++;
	dimB=CB_EvalInt( SRC );
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
	
	CB_MatrixInitsub( SRC, &reg, dimA, dimB, 0 );
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
	int base;
	
	c=SkipSpcCR(SRC);
	if ( c == '"' ) { CB_MatrixInit2Str( SRC );  return; }  // String
	if ( c != '[' ) { CB_Error(SyntaxERR); return; }  // Syntax error
	ExecPtr++;
	SkipSpcCR(SRC);
	exptr=ExecPtr;
	n=1;
	while ( 1 ) {
		data=CB_EvalDbl( SRC );
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
				data=CB_EvalDbl( SRC );
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

	CB_MatrixInitsub( SRC, &reg, dimA, dimB, 0 );
	if ( ErrorNo ) return ;

	exptr2=ExecPtr;
	ExecPtr=exptr;
	base=MatAry[reg].Base;
	m=base; n=base;
	while ( m < dimA+base ) {
		n=base;
		while ( n < dimB+base ) {
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

int MatGetOpcode(char *SRC, char *buffer, int Maxlen ) {
	char tmpbuf[18];
	int i,j=0,len,ptr=0;
	int c=1,d;
	while ( c != '\0' ) {
		c = SRC[ExecPtr++] ;
		if ( c==0x22 ) break ; // <CR>
		else
		if ( c==0x5C ) // Backslash
			buffer[ptr++] = SRC[ExecPtr++]&0xFF ;
		else
		if ( (c==0x7F)||(c==0xFFFFFFF7)||(c==0xFFFFFFF9)||(c==0xFFFFFFE5)||(c==0xFFFFFFE6)||(c==0xFFFFFFE7)||(c==0xFFFFFFFF) ) {
			buffer[ptr++] = c & 0xFF;
			buffer[ptr++] = SRC[ExecPtr++]&0xFF ;
		} else buffer[ptr++] = c & 0xFF;

		if ( ptr >= Maxlen-1 ) { CB_Error(StringTooLongERR); break; }	// String too Long error
	}
	buffer[ptr]='\0' ;
	return ptr;
}


void CB_MatrixInit2Str( char *SRC ) { //	["ABCD","12345","XYZ"]->Mat A[.B]
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
	char buffer[129];
	int len;
	int maxlen=0;
	int base;
	
//	c=SkipSpcCR(SRC);
//	if ( c != '"' ) { CB_Error(SyntaxERR); return; }  // Syntax error
	ExecPtr++;
	exptr=ExecPtr;
	m=1;
	SkipSpcCR(SRC);
	while ( 1 ) {
		len=MatGetOpcode(SRC, buffer, 128);
		if ( len > maxlen ) maxlen=len;
		c=SkipSpcCR(SRC);
		if ( c == ']' ) break;
		if ( c != ',' ) { CB_Error(SyntaxERR); return; }  // Syntax error
		ExecPtr++;
		c=SkipSpcCR(SRC);
		if ( c != 0x22 ) { CB_Error(SyntaxERR); return; }  // Syntax error
		ExecPtr++;
		SkipSpcCR(SRC);
		m++;
	}
	dimA=m;
	dimB=maxlen+1;

	c=SRC[++ExecPtr];
	if ( c != 0x0E ) { CB_Error(SyntaxERR); return; }  // Syntax error
	ExecPtr++;

	CB_MatrixInitsub( SRC, &reg, dimA, dimB, 8 );	// byte 
	if ( ErrorNo ) return ;

	exptr2=ExecPtr;
	ExecPtr=exptr;
	base=MatAry[reg].Base;
	m=base; n=base;
	while ( m < dimA+base ) {
		len=MatGetOpcode(SRC, buffer, 128);
		cptr=MatrixPtr( reg, m, 1 );
		strncpy(cptr, buffer, len);
		c=SkipSpcCR(SRC);
		if ( c == ']' ) break;
		if ( c != ',' ) { CB_Error(SyntaxERR); return; }  // Syntax error
		ExecPtr++;
		c=SkipSpcCR(SRC);
		if ( c != 0x22 ) { CB_Error(SyntaxERR); return; }  // Syntax error
		ExecPtr++;
		SkipSpcCR(SRC);
		m++;
	}
	ExecPtr=exptr2;
}

//-----------------------------------------------------------------------------

void CB_ClrMat( char *SRC ) { //	ClrMat A
	int c =SRC[ExecPtr];
	if ( ( 'A'<=c )&&( c<='z' ) ) {
		ExecPtr++;
		DeleteMatrix( c-'A' );
	} else {
		DeleteMatrix(-1);	// ClrMat
	}
}

void CopyMatrix( int reg2, int reg ) {	// reg -> reg2
	int m,n;
	int sizeA,sizeB,sizeA2,sizeB2;
	int ElementSize,ElementSize2;
	int base,base2;
	
	sizeA        = MatAry[reg ].SizeA;
	sizeA2       = MatAry[reg2].SizeA;
	sizeB        = MatAry[reg ].SizeB;
	sizeB2       = MatAry[reg2].SizeB;
	base         = MatAry[reg ].Base;
	base2        = MatAry[reg2].Base;
	ElementSize  = MatAry[reg ].ElementSize;
	if ( ElementSize  == 2 ) ElementSize  == 1;
	ElementSize2 = MatAry[reg2].ElementSize;
	if ( ElementSize2 == 2 ) ElementSize2 == 1;

//	if ( base < base2 ) { CB_Error(ArraySizeERR); return; }	// Illegal Ary size
	
	if ( ( ElementSize == ElementSize2 ) && ( sizeA == sizeA2 ) && ( sizeB == sizeB2 ) && ( base == base2 ) ) {
		memcpy( MatAry[reg2].Adrs, MatAry[reg].Adrs, MatrixSize(reg, sizeA, sizeB) ) ;
	} else { 
		if ( ( ElementSize < 64 ) && ( ElementSize2 < 64 ) ) {
			for ( m=base; m<sizeA+base; m++ ) {
				if ( m<sizeA2+base2 ) for ( n=base; n<sizeB+base; n++ ) if ( n<sizeB2+base2 ) WriteMatrixInt( reg2, m, n,  ReadMatrixInt( reg, m, n ) );
			}
		} else {
			for ( m=base; m<sizeA+base; m++ ) {
				if ( m<sizeA2+base2 ) for ( n=base; n<sizeB+base; n++ ) if ( n<sizeB2+base2 ) WriteMatrix( reg2, m, n,  ReadMatrix( reg, m, n ) );
			}
		}
	}
}

int CB_MatCalc( char *SRC ) { //	Mat A -> Mat B  etc
	char dspflagtmp;
	int c,d,i,m,n;
	int dim=0,dimA,dimB,dimA2,dimB2;
	int ElementSize,ElementSize2;
	int reg,reg2;
	int base,base2;
	double	*dptr, *dptr2;
	
	c =SRC[ExecPtr];
	if ( ( 'A'<=c )&&( c<='z' ) ) {
		ExecPtr++;
		reg=c-'A';
		if ( MatAry[reg].SizeA == 0 ) { CB_Error(NoMatrixArrayERR); return; }	// No Matrix Array error
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
		if ( ( 'A'<=c )&&( c<='z' ) ) {
			ExecPtr++;
			reg2=c-'A';
		} else { CB_Error(SyntaxERR); return; }	// Syntax error
		if ( dim ) { 		// Mat A -> Dim Mat B[.w]
			ElementSize =MatAry[reg].ElementSize;
			ElementSize2=ElementSizeSelect( SRC, reg2, &base2 ) & 0xFF;
			if ( ( ElementSize == 2 ) || ( ElementSize2 == 2 ) ) { CB_Error(DimensionERR); return ; }	// Dimension error
			if ( ElementSize == ElementSize2 ) return;
			if ( ElementSize == 1 ) {
				dimA =MatAry[reg].SizeB;
				dimB =MatAry[reg].SizeA;
			} else {
				dimA =MatAry[reg].SizeA;
				dimB =MatAry[reg].SizeB;
			}
			dimA2=dimA;
			dimB2=dimB;
			switch ( ElementSize2 ) {
				case  1:
					switch ( ElementSize ) {
						case  8:		//  8 -> 1
							dimB2 *= 8;	break;
						case 16:		// 16 -> 1
							dimB2 *=16;	break;
						case 32:		// 32 -> 1
							dimB2 *=32;	break;
						case 64:		// 64 -> 1
							dimB2 *=64;	break;
					}
					break;
				case  8:
					switch ( ElementSize ) {
						case  1:		// 1 -> 8
							dimB2=((dimB2-1)/8)+1;	break;
						case 16:		// 16 -> 8
							dimB2 *= 2;	break;
						case 32:		// 32 -> 8
							dimB2 *= 4;	break;
						case 64:		// 64 -> 8
							dimB2 *= 8;	break;
					}
					break;
				case 16:
					switch ( ElementSize ) {
						case  1:		// 1 -> 16
							dimB2=((dimB2-1)/8)+1;
						case  8:		// 8 -> 16
							if ( dimB2 & 1 ) { dimB2=-1; break; }
							dimB2 /= 2;	break;
						case 32:		// 32 -> 16
							dimB2 *= 2;	break;
						case 64:		// 64 -> 16
							dimB2 *= 4;	break;
					}
					break;
				case 32:
					switch ( ElementSize ) {
						case  1:		// 1 -> 32
							dimB2=((dimB2-1)/8)+1;
						case  8:		// 8 -> 32
							if ( dimB2 & 3 ) { dimB2=-1; break; }
							dimB2 /= 4;	break;
						case 16:		// 16 -> 32
							if ( dimB2 & 1 ) { dimB2=-1; break; }
							dimB2 /= 2;	break;
						case 64:		// 64 -> 32
							dimB2 *= 2;	break;
					}
					break;
				case 64:
					switch ( ElementSize ) {
						case  1:		// 1 -> 64
							dimB2=((dimB2-1)/8)+1;
							if ( dimB2 & 7 ) { dimB2=-1; break; }
						case  8:		// 8 -> 64
							dimB2 /= 8;	break;
						case 16:		// 16 -> 64
							if ( dimB2 & 3 ) { dimB2=-1; break; }
							dimB2 /= 4;	break;
						case 32:		// 32 -> 64
							if ( dimB2 & 1 ) { dimB2=-1; break; }
							dimB2 /= 2;	break;
					}
					break;
			}
			if ( ( dimA2 < 0 ) || ( dimB2 < 0 ) ) { CB_Error(DimensionERR); return ; }	// Dimension error
			if ( reg != reg2 ) { 
				DimMatrixSub( reg2, ElementSize, MatAry[reg].SizeA, MatAry[reg].SizeB, MatAry[reg].Base );
				CopyMatrix( reg2, reg );
			}
			if ( ElementSize2 == 1 ) {
				MatAry[reg2].SizeA=dimB2;
				MatAry[reg2].SizeB=dimA2;
			} else {
				MatAry[reg2].SizeA=dimA2;
				MatAry[reg2].SizeB=dimB2;
			}
			MatAry[reg2].ElementSize=ElementSize2;
		} else {								// Mat A -> Mat B
			if ( MatAry[reg2].SizeA == 0 ) { CB_Error(NoMatrixArrayERR); return; }	// No Matrix Array error
			if ( reg == reg2 ) return;
			CopyMatrix( reg2, reg );
		}
		dspflagtmp=0;
	} else {
		ExecPtr-=3;
		dspflagtmp=2;
		if (CB_INT)	CBint_CurrentValue = EvalIntsubTop( SRC );
		else		CB_CurrentValue    = EvalsubTop( SRC );
	}
	return dspflagtmp;
}

//-----------------------------------------------------------------------------
void CB_MatFill( char *SRC ) { //	Fill(value, Mat A)
	int c,d;
	int dimA,dimB,i;
	int reg;
	double	*dptr, *dptr2;
	double value;
	
	value=CB_EvalDbl( SRC );
	c =SRC[ExecPtr];
	if ( c != ',' ) { CB_Error(SyntaxERR); return; }  // Syntax error
	c =SRC[++ExecPtr];
	if ( ( c != 0x7F ) || ( SRC[ExecPtr+1]!=0x40 ) ) { CB_Error(SyntaxERR); return; }	// Syntax error
	ExecPtr+=2;
	c =SRC[ExecPtr];
	if ( ( 'A'<=c )&&( c<='z' ) ) {
		ExecPtr++;
		reg=c-'A';
		if ( MatAry[reg].SizeA == 0 ) { CB_Error(NoMatrixArrayERR); return; }	// No Matrix Array error
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
	if ( ( 'A'<=c )&&( c<='z' ) ) {
		ExecPtr++;
		reg=c-'A';
		if ( MatAry[reg].SizeA == 0 ) { CB_Error(NoMatrixArrayERR); return; }	// No Matrix Array error
	} else { CB_Error(SyntaxERR); return; }	// Syntax error

	TransposeMatirx( reg );
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

int WordSizeSelect( char *SRC ) {
	int c,d;
	int WordSize=8;
	c =SRC[ExecPtr];
	if ( c=='.' ) {
		c =SRC[++ExecPtr];
		if ( ( c=='B' ) || ( c=='b' ) ) { ExecPtr++; }
		else
		if ( ( c=='W' ) || ( c=='w' ) ) { ExecPtr++; WordSize=16; }
		else
		if ( ( c=='L' ) || ( c=='l' ) ) { ExecPtr++; WordSize=32; }
		else
		if ( ( c=='F' ) || ( c=='f' ) ) { ExecPtr++; WordSize=64; }
	}
	return WordSize;
}

double CB_Peek( char *SRC, int adrs ) {	// Peek(123456).f
	int wsize;
	char *cptr;
	short *sptr;
	int *iptr;
	double *dptr;
	if ( SRC[ExecPtr]==')' ) ExecPtr++;
	wsize=WordSizeSelect( SRC );
	switch ( wsize ) {
		case 64:
			if ( adrs & 3 ) { CB_Error(ArgumentERR); return 0; } // Argument error
			dptr=(double *)adrs;
			return *dptr;
		case 32:
			if ( adrs & 3 ) { CB_Error(ArgumentERR); return 0; } // Argument error
			iptr=(int *)adrs;
			return *iptr;
		case 16:
			if ( adrs & 1 ) { CB_Error(ArgumentERR); return 0; } // Argument error
			sptr=(short *)adrs;
			return *sptr;
		default:
			cptr=(char *)adrs;
			return *cptr;
	}
}
int CB_PeekInt( char *SRC, int adrs ) {	// Peek(123456).w
	int wsize;
	char *cptr;
	short *sptr;
	int *iptr;
	double *dptr;
	if ( SRC[ExecPtr]==')' ) ExecPtr++;
	wsize=WordSizeSelect( SRC );
	switch ( wsize ) {
		case 64:
			if ( adrs & 3 ) { CB_Error(ArgumentERR); return 0; } // Argument error
			dptr=(double *)adrs;
			return *dptr;
		case 32:
			if ( adrs & 3 ) { CB_Error(ArgumentERR); return 0; } // Argument error
			iptr=(int *)adrs;
			return *iptr;
		case 16:
			if ( adrs & 1 ) { CB_Error(ArgumentERR); return 0; } // Argument error
			sptr=(short *)adrs;
			return *sptr;
		default:
			cptr=(char *)adrs;
			return *cptr;
	}
}

void CB_PokeSub( char *SRC, double data, int adrs ) {	// Poke(123456).f
	int wsize;
	char *cptr;
	short *sptr;
	int *iptr;
	double *dptr;
	if ( SRC[ExecPtr]==')' ) ExecPtr++;
	wsize=WordSizeSelect( SRC );
	switch ( wsize ) {
		case 64:
			if ( adrs & 3 ) { CB_Error(ArgumentERR); return; } // Argument error
			dptr=(double *)adrs;
			*dptr=data;
			break;
		case 32:
			if ( adrs & 3 ) { CB_Error(ArgumentERR); return; } // Argument error
			iptr=(int *)adrs;
			*iptr=data;
			break;
		case 16:
			if ( adrs & 1 ) { CB_Error(ArgumentERR); return; }  // Argument error
			sptr=(short *)adrs;
			*sptr=data;
			break;
		default:
			cptr=(char *)adrs;
			*cptr=data;
			break;
	}
}
void CB_PokeSubInt( char *SRC, int data, int adrs ) {	// Poke(123456).w
	int wsize;
	char *cptr;
	short *sptr;
	int *iptr;
	double *dptr;
	if ( SRC[ExecPtr]==')' ) ExecPtr++;
	wsize=WordSizeSelect( SRC );
	switch ( wsize ) {
		case 64:
			if ( adrs & 3 ) { CB_Error(ArgumentERR); return; } // Argument error
			dptr=(double *)adrs;
			*dptr=data;
			break;
		case 32:
			if ( adrs & 3 ) { CB_Error(ArgumentERR); return; } // Argument error
			iptr=(int *)adrs;
			*iptr=data;
			break;
		case 16:
			if ( adrs & 1 ) { CB_Error(ArgumentERR); return; }  // Argument error
			sptr=(short *)adrs;
			*sptr=data;
			break;
		default:
			cptr=(char *)adrs;
			*cptr=data;
			break;
	}
}

void CB_Poke( char *SRC ) {	// Poke(123456).w,123,...
	int adrs=CB_EvalInt( SRC );
	int wsize;
	char *cptr;
	short *sptr;
	int *iptr;
	double *dptr;
	if ( SRC[ExecPtr]==')' ) ExecPtr++;
	wsize=WordSizeSelect( SRC );
	if ( SRC[ExecPtr]!=',' ) { CB_Error(SyntaxERR); return; }	// Syntax error
	do {
		ExecPtr++;
		switch ( wsize ) {
			case 64:
				if ( adrs & 3 ) { CB_Error(ArgumentERR); return; } // Argument error
				dptr=(double *)adrs;
				*dptr=CB_EvalDbl( SRC );
				adrs+=8;
				break;
			case 32:
				if ( adrs & 3 ) { CB_Error(ArgumentERR); return; } // Argument error
				iptr=(int *)adrs;
				*iptr=CB_EvalInt( SRC );
				adrs+=4;
				break;
			case 16:
				if ( adrs & 1 ) { CB_Error(ArgumentERR); return; }  // Argument error
				sptr=(short *)adrs;
				*sptr=CB_EvalInt( SRC );
				adrs+=2;
				break;
			default:
				cptr=(char *)adrs;
				*cptr=CB_EvalInt( SRC );
				adrs++;
				break;
		}
	} while ( SRC[ExecPtr] == ',' );
}

int CB_EvalIntStrPtr( char *SRC ) {
	int c;
	int result;
	int maxoplen;
	char *buffer;
	c=CB_IsStr( SRC, ExecPtr );
	if ( c ) {	// string
		buffer = CB_GetOpStr( SRC, &maxoplen );	
		if ( ErrorNo ) return ;			// error
		return (int)buffer;	//  return buffer ptr
	} else {	// expression
		return CB_EvalInt( SRC );
	}
}

int CB_SysCall( char *SRC ) {	// SysCall( No,r4,r5,r6,r7 )
	int callNo=CB_EvalInt( SRC );
	int r4=0,r5=0,r6=0,r7=0;
	unsigned int ur4,ur5,ur6,ur7;
	if ( SRC[ExecPtr]==',' ) { 
		ExecPtr++;
		r4=CB_EvalIntStrPtr( SRC );
		if ( SRC[ExecPtr]==',' ) { 
			ExecPtr++;
			r5=CB_EvalIntStrPtr( SRC );
			if ( SRC[ExecPtr]==',' ) {
				ExecPtr++;
				r6=CB_EvalIntStrPtr( SRC );
				if ( SRC[ExecPtr]==',' ) {
					ExecPtr++;
					r7=CB_EvalIntStrPtr( SRC );
				}
			}
		}
	}
	if ( SRC[ExecPtr]==')' ) ExecPtr++;
	return SysCalljmp(r4,r5,r6,r7,callNo);	//		CallNo -> SysCallNo
}
int CB_Call( char *SRC ) {	// Call( adrs,r5,r6,r7 )
	int adrs=CB_EvalInt( SRC );
	int r4=0,r5=0,r6=0,r7=0;
	if ( adrs & 1 ) CB_Error(ArgumentERR);  // Argument error
	if ( SRC[ExecPtr]==',' ) { 
		ExecPtr++;
		r4=CB_EvalIntStrPtr( SRC );
		if ( SRC[ExecPtr]==',' ) { 
			ExecPtr++;
			r5=CB_EvalIntStrPtr( SRC );
			if ( SRC[ExecPtr]==',' ) {
				ExecPtr++;
				r6=CB_EvalIntStrPtr( SRC );
				if ( SRC[ExecPtr]==',' ) {
					ExecPtr++;
					r7=CB_EvalIntStrPtr( SRC );
				}
			}
		}
	}
	if ( SRC[ExecPtr]==')' ) ExecPtr++;
	return Calljmp(r4,r5,r6,r7,adrs);
}

int CB_VarPtr( char *SRC ) {
	int c;
	int reg,dimA,dimB;
	int result;
	int maxoplen;
	char *buffer;
	
	c=SRC[ExecPtr];
	if ( ( c == 0x22 ) || ( (c==0xFFFFFFF9)&&(SRC[ExecPtr+1]==0x3F) ) || ( (c==0x7F)&&(SRC[ExecPtr+1]==0xFFFFFFF0) ) ) {	// string "" str graphy
		buffer = CB_GetOpStr( SRC, &maxoplen );	
		if ( ErrorNo ) return ;			// error
		result=(int)buffer;	//  return buffer ptr
		
	} else {	// variable
		c=SRC[ExecPtr++];
		if ( ( 'A'<=c )&&( c<='Z' ) )  {
				reg=c-'A';
				c=SRC[ExecPtr];
				if ( c=='%' ) { ExecPtr++; result=(int)&REGINT[reg]; }
				else
				if ( c=='[' ) goto Matrix;
				else
				if ( ( '0'<=c )&&( c<='9' ) ) {
						goto Matrix1;
				} else
				if ( c=='#' ) { ExecPtr++; result=(int)&REG[reg]; }
				else
				if (CB_INT)	result=(int)&REGINT[reg]; else result=(int)&REG[reg];
		} else
		if ( ( 'a'<=c )&&( c<='z' ) )  {
				reg=c-'a';
				c=SRC[ExecPtr];
				if ( c=='%' ) { ExecPtr++; result=(int)&LocalInt[reg][0]; }
				else
				if ( c=='[' ) goto Matrix;
				else
				if ( ( '0'<=c )&&( c<='9' ) ) {
					Matrix1:
						ExecPtr++;
						dimA=c-'0';
						MatOprand1( SRC, reg, &dimA, &dimB );
						goto Matrix2;
				} else
				if ( c=='#' ) { ExecPtr++; result=(int)&LocalDbl[reg][0]; }
				else
				if (CB_INT)	result=(int)&LocalInt[reg][0]; else result=(int)&LocalDbl[reg][0];
		} else
		if ( c==0x7F ) { 			// 
			if ( SRC[ExecPtr++] == 0x40 ) {	// Mat A[a,b]
				c=SRC[ExecPtr]; 
				if ( ( 'A'<=c )&&( c<='z' ) ) { 
					reg=c-'A';
					if ( SRC[++ExecPtr] == '[' ) {
						result=(int)MatAry[reg].Adrs;
					Matrix:	
						ExecPtr++;
						MatOprand2( SRC, reg, &dimA, &dimB );
					Matrix2:	
						if ( ErrorNo ) return 1 ; // error
						result=(int)MatrixPtr( reg, dimA, dimB );
					} else {
						result=(int)MatAry[reg].Adrs;	// Mat A
					}
				} else { CB_Error(SyntaxERR) ; return 0; }// Syntax error 
			} else { CB_Error(SyntaxERR) ; return 0; }// Syntax error 
		} else { CB_Error(SyntaxERR) ; return 0; }// Syntax error 
	}
	if ( SRC[ExecPtr]==')' ) ExecPtr++;
	return result;
}

int CB_ProgPtr( char *SRC ) { //	ProgPtr(
	int c;
	char buffer[32],folder16[21];
	int progno;

	c=SRC[ExecPtr];
	if ( c != 0x22 ) { 
		if ( c != ')' ) { ExecPtr++; return 0; }	// ProgPtr()
		ExecPtr++;
		return (int)ProgfileAdrs[ProgNo];
	}
	ExecPtr++;
	CB_GetQuotOpcode(SRC, buffer,32);	// Prog name
	
	Setfoldername16( folder16, buffer );
	progno = CB_SearchProg( folder16 );
	if ( progno < 0 ) { CB_Error(NotfoundProgERR); return 0 ; }  // Not found Prog

	if ( SRC[ExecPtr]==')' ) ExecPtr++;
	return  (int)ProgfileAdrs[progno];
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//int MatrixObjectAlign4M1( unsigned int n ){ return n; }	// align +4byte
//int MatrixObjectAlign4M2( unsigned int n ){ return n; }	// align +4byte
//int MatrixObjectAlign4M3( unsigned int n ){ return n; }	// align +4byte
//int MatrixObjectAlign4M4( unsigned int n ){ return n; }	// align +4byte
//int MatrixObjectAlign4M5( unsigned int n ){ return n; }	// align +4byte
//int MatrixObjectAlign4M6( unsigned int n ){ return n; }	// align +4byte
//int MatrixObjectAlign4M7( unsigned int n ){ return n; }	// align +4byte
//int MatrixObjectAlign4M8( unsigned int n ){ return n; }	// align +4byte
//int MatrixObjectAlign4M9( unsigned int n ){ return n; }	// align +4byte
//-----------------------------------------------------------------------------
/*
int Dummy1_ElementSizeSelect( char *SRC, int reg, int *base ) {
	int c,d;
	int ElementSize;
	*base=MatBase;
	c =SRC[ExecPtr];
	if ( c=='.' ) {
		c =SRC[++ExecPtr];
		if (CB_INT)	ElementSize=32; else ElementSize=64;
		if ( ( c=='0' ) || ( c=='1' ) ) { c=SRC[++ExecPtr]; *base = c-'0' ; }
		if ( ( c=='P' ) || ( c=='p' ) ) { ExecPtr++; ElementSize= 1; }
		if ( ( c=='V' ) || ( c=='v' ) ) { ExecPtr++; ElementSize= 2;
			c =SRC[ExecPtr];
			if ( ( c=='T' ) || ( c=='t' ) ) { ExecPtr++; ElementSize= 3; } // text VRAM
			if ( ( c=='G' ) || ( c=='g' ) ) { ExecPtr++; ElementSize= 4; } // GraphicVRAM
		}
		if ( ( c=='B' ) || ( c=='b' ) ) { ExecPtr++; ElementSize= 8; }
		if ( ( c=='W' ) || ( c=='w' ) ) { ExecPtr++; ElementSize=16; }
		if ( ( c=='L' ) || ( c=='l' ) ) { ExecPtr++; ElementSize=32; }
		if ( ( c=='F' ) || ( c=='f' ) ) { ExecPtr++; ElementSize=64; }
		c =SRC[ExecPtr];
		if ( ( c=='0' ) || ( c=='1' ) ) { 
			*base = c-'0' ;
			ExecPtr++;
		}
	}
	else {
		ElementSize = DimMatrixDefaultElementSize( reg ) +0x100 ;
	}
	return ElementSize;
}
int Dummy2_ElementSizeSelect( char *SRC, int reg, int *base ) {
	int c,d;
	int ElementSize;
	*base=MatBase;
	c =SRC[ExecPtr];
	if ( c=='.' ) {
		c =SRC[++ExecPtr];
		if (CB_INT)	ElementSize=32; else ElementSize=64;
		if ( ( c=='0' ) || ( c=='1' ) ) { c=SRC[++ExecPtr]; *base = c-'0' ; }
		if ( ( c=='P' ) || ( c=='p' ) ) { ExecPtr++; ElementSize= 1; }
		if ( ( c=='V' ) || ( c=='v' ) ) { ExecPtr++; ElementSize= 2;
			c =SRC[ExecPtr];
			if ( ( c=='T' ) || ( c=='t' ) ) { ExecPtr++; ElementSize= 3; } // text VRAM
			if ( ( c=='G' ) || ( c=='g' ) ) { ExecPtr++; ElementSize= 4; } // GraphicVRAM
		}
		if ( ( c=='B' ) || ( c=='b' ) ) { ExecPtr++; ElementSize= 8; }
		if ( ( c=='W' ) || ( c=='w' ) ) { ExecPtr++; ElementSize=16; }
		if ( ( c=='L' ) || ( c=='l' ) ) { ExecPtr++; ElementSize=32; }
		if ( ( c=='F' ) || ( c=='f' ) ) { ExecPtr++; ElementSize=64; }
		c =SRC[ExecPtr];
		if ( ( c=='0' ) || ( c=='1' ) ) { 
			*base = c-'0' ;
			ExecPtr++;
		}
	}
	else {
		ElementSize = DimMatrixDefaultElementSize( reg ) +0x100 ;
	}
	return ElementSize;
}
*/
//-----------------------------------------------------------------------------

