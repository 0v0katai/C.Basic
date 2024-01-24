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
#include "CB_setup.h"
#include "MonochromeLib.h"

//-----------------------------------------------------------------------------
// Matrix 
//-----------------------------------------------------------------------------
//matary MatAry[MATARY_MAX];
int MatAryMax=MATARY_MAX;
matary *MatAry;
short ExtListMax=0;		// Extend List Max number
short Mattmpreg;		//
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
int DimMatrixSubNoinit( int reg, int ElementSize, int m, int n , int base ) {	// 1-
	char	*cptr;
	short	*wptr;
	int		*iptr;
	double	*dptr;
	int i;
	int matsize;
	if ( ( ElementSize!= 1 ) && ( ElementSize!= 2 ) && ( ElementSize!= 8 ) && ( ElementSize!= 16 ) && ( ElementSize!= 32 ) && ( ElementSize!= 64 ) ) { CB_Error(ElementSizeERR); return; }	// Illegal Element size
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
	return 0;	// ok
}

int DimMatrixSub( int reg, int ElementSize, int m, int n , int base ) {	// 1-
	int r=DimMatrixSubNoinit( reg, ElementSize, m, n , base );
	if ( r==0 ) memset( (char*)MatAry[reg].Adrs, 0, MatAry[reg].Maxbyte  );	// initialize
	return r;
}

int MatElementPlus( int reg, int m, int n ) {	// 1-
	char	*cptr;
	short	*wptr;
	int		*iptr;
	double	*dptr;
	int i,tmpreg=Mattmpreg;
	int matsize;
	int base=MatAry[reg].Base;
	int sizeA=MatAry[reg].SizeA;
	int sizeB=MatAry[reg].SizeB;
	int ElementSize=MatAry[reg].ElementSize;
	int maxbyte=MatAry[reg].Maxbyte;

	if ( ( ElementSize!= 1 ) && ( ElementSize!= 8 ) && ( ElementSize!= 16 ) && ( ElementSize!= 32 ) && ( ElementSize!= 64 ) ) { CB_Error(ElementSizeERR); return; }	// Illegal Element size
	if ( ElementSize>1 ) {
			matsize=m*n*(ElementSize>>3);
	} else {	// 1 bit matrix
			matsize=( ((m-1)>>3)+1 )*n;
	}
	matsize = (matsize+7) & 0xFFFFFFF8;	// 8byte align
	if ( ( MatAry[reg].Adrs != NULL ) && ( MatAry[reg].Maxbyte >= matsize ) && ( MatAry[reg].ElementSize!=2 ) ) { // already exist
		MatAry[reg].SizeA       = m;						// new Matrix array size
		MatAry[reg].SizeB       = n;						// new Matrix array size
	} else {
		DimMatrixSub( tmpreg, ElementSize, m, n, base );	// new Matrix tmp reg
		CopyMatrix( tmpreg, reg );
		if ( ( MatAry[reg].Adrs != NULL ) && ( MatAry[reg].ElementSize != 2 ) ) HiddenRAM_freeMat( reg );	// free
		MatAry[reg].SizeA       = m;						// Matrix array size
		MatAry[reg].SizeB       = n;						// new Matrix array size
		MatAry[reg].Adrs        = MatAry[tmpreg].Adrs ;		// Matrix array ptr*
		MatAry[reg].Maxbyte     = MatAry[tmpreg].Maxbyte ;	// Matrix array max byte
		MatAry[tmpreg].SizeA    = 0;						// tmp Matrix array delete
		MatAry[tmpreg].Adrs     = NULL; 					// tmp Matrix array delete
	}
	return 0;	// ok
}

int DimMatrix( int reg, int dimA, int dimB, int base ) {
	char	*cptr;
	short	*wptr;
	int		*iptr;
	double	*dptr;
	int i;
	int ElementSize;
	
	ElementSize = CB_INT? 32:64 ;
	return	DimMatrixSub( reg, ElementSize, dimA, dimB, base );
}

//-----------------------------------------------------------------------------
void DeleteMatrix( int reg ) {
	double *ptr;
	if ( ( 0<=reg ) && ( reg<MatAryMax ) ) {
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
		HiddenRAM_MatTopPtr = (char *)MatAry;
	}
	HiddenRAM_MatAryStore();	// MatAry ptr -> HiddenRAM
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
		if ( MatXYmode ) sprintf( (char*)buffer," X(%d~%3d)  ", base, dimA); else sprintf( (char*)buffer," m(%d~%3d)  ", base, dimA);
		locate( 3,4); Print((unsigned char*)buffer);
		locate(13,4); Print((unsigned char *)":     ");
		sprintG(buffer,*m,  5,LEFT_ALIGN); locate(14, 4); Print((unsigned char*)buffer);
		if ( MatXYmode ) sprintf( (char*)buffer," Y(%d~%3d)  ", base, dimB); else sprintf( (char*)buffer," n(%d~%3d)  ", base, dimB);
		locate( 3,5); Print((unsigned char*)buffer);
		
		locate(13,5); Print((unsigned char *)":     ");
		sprintG(buffer,*n,  5,LEFT_ALIGN); locate(14, 5); Print((unsigned char*)buffer);

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
							*m =InputNumD_full( 14, y, 5, *m);	// 
						} while ( ( *m < base ) || ( *m > dimA ) ) ;
						select+=1;
						break;
					case 1: // dim n
						do {
							*n =InputNumD_full( 14, y, 5, *n);	// 
						} while ( ( *n < base ) || ( *n > dimB ) ) ;
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
							*m =InputNumD_Char( 14, y, 5, *m, key);	// 
						} while ( ( *m < base ) || ( *m > dimA ) ) ;
						select+=1;
						break;
					case 1: // dim n
						do {
							*n =InputNumD_Char( 14, y, 5, *n, key);	// 
						} while ( ( *n < base ) || ( *n > dimB ) ) ;
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
//----------------------------------------------------------------------------
char   MiniDotCursorSize = 3;
char   MiniDotCursorflag = 0;	// GCursor Pixel ON:1 OFF:0
char  MiniDotCursorX;
char  MiniDotCursorY;

void MatDotEditCursorFlashing() {		// timer IRQ handler
	DISPBOX area;
//	unsigned char CsrDATA[]={ 0xFF,0xFF,0xFF,0x00 };
	unsigned char CsrDATA[]={ 0xA0,0x00,0xA0,0x00 };
	DISPGRAPH minicsr;

	if ( ( 0<=MiniDotCursorX ) && ( MiniDotCursorX<126 ) && ( 0<=MiniDotCursorY ) && ( MiniDotCursorY<60 ) ) {
		switch (MiniDotCursorflag) {
			case 0:
				minicsr.x = MiniDotCursorX;
				minicsr.y = MiniDotCursorY;
				minicsr.GraphData.height = MiniDotCursorSize;
				minicsr.GraphData.width  = MiniDotCursorSize;
				minicsr.GraphData.pBitmap = CsrDATA; 	// mini cursor pattern
				minicsr.WriteModify = IMB_WRITEMODIFY_NORMAL;
				minicsr.WriteKind = IMB_WRITEKIND_XOR;	// reverse mode
			    Bdisp_WriteGraph_DD(&minicsr); 		// drawing only display driver
				MiniDotCursorflag=1;
				break;
			case 1:
				Bdisp_PutDisp_DD();
				MiniDotCursorflag=0;
				break;
			default:
				break;
		}
	}
}

void MatDotEditCursorSetFlashMode(int set) {	// 1:on  0:off
	switch (set) {
		case 0:
			KillTimer(ID_USER_TIMER1);
			break;
		case 1:
			SetTimer(ID_USER_TIMER1, 250, (void*)&MatDotEditCursorFlashing);
			MiniDotCursorflag=0;		// mini cursor initialize
			break;
		default:
			break;
	}
}

//-----------------------------------------------------------------------------
void Mat2Clip( int reg, char *buffer , int max ) {	// 
	int i,j,dimA,dimB,x,y;
	int base=MatAry[reg].Base;
	int ElementSize=MatAry[reg].ElementSize;
	int ptr=0;
	char buffer2[32];
	dimA=MatAry[reg].SizeA-1+base;
	dimB=MatAry[reg].SizeB-1+base;

	buffer[ptr++]='[';
//	buffer[ptr++]=0x0D;
	for (x=base; x<=dimA; x++) {
		buffer[ptr++]='[';
		for (y=base; y<=dimB; y++) {
			sprintf(buffer2,"%g",ReadMatrix( reg, x, y));
			i=strlen(buffer2);
			if ( ptr+i > max ) { 
				buffer[0]='\0';
				ErrorMSGstr1("CLip Buffer Over");
				return;
			}
			strcpy(buffer+ptr,buffer2);
			ptr+=i;
			buffer[ptr++]=',';
		}
		ptr--;
		buffer[ptr++]=']';
		buffer[ptr++]=0x0D;
	}
	ptr--;
	buffer[ptr++]=']';
	buffer[ptr]='\0';
	ErrorMSGstr1("Mat to Clip Ok!");
}
/*
void Mat2ClipEntry( int reg  ) {	// 
	unsigned int key;

	if ( MatAry[reg].ElementSize == 1 ) {
		while ( 1 ) {
			Fkey_dispN( FKeyNo1, "Norm");
			Fkey_dispN( FKeyNo2, "Byte");
			Fkey_dispN( FKeyNo3, "Word");
			Fkey_dispN( FKeyNo4, "Long");
			FkeyClear( FKeyNo5 );
			FkeyClear( FKeyNo6 );
			GetKey(&key);
			switch ( key )
				case KEY_CTRL_AC:
				case KEY_CTRL_EXIT:
					cont=0;
				break;
			case KEY_CTRL_F1:
				
			case KEY_CTRL_F2:
			case KEY_CTRL_F3:
			case KEY_CTRL_F4:
			

	}

}
*/
int SkipSpcCRsub( char *buf, int *ptr ) {
	if ( buf[(*ptr)] == 0x0D ) (*ptr)++; // Skip [CR]
	while ( buf[(*ptr)]==0x20 ) (*ptr)++; // Skip Space
	return buf[(*ptr)];
}

void Clip2Mat( char *buffer, int reg ) { //	[[1.2,3][4,5,6]]->Mat
	int c,d;
	int dimA,dimB,i;
	int exptr,exptr2;
	double data;
	int m,n;
	int base;
	int ElementSize;
	int ptr=0;
	int dimA2,dimB2;
	
	c=SkipSpcCRsub(buffer,&ptr);
	if ( c != '[' ) return ;
	ptr++;
	SkipSpcCRsub(buffer,&ptr);
	if ( c != '[' ) return ;
	ptr++;
	SkipSpcCRsub(buffer,&ptr);
	exptr=ptr;
	n=1;
	while ( 1 ) {
		data=Eval2( buffer, &ptr ); if ( ErrorNo ) return;
		c=SkipSpcCRsub(buffer,&ptr);
		if ( c != ',' ) break;  // Syntax error
		ptr++;
		SkipSpcCRsub(buffer,&ptr);
		n++;
	}
	dimB=n;
	if ( c == ']' ) ptr++;
	c=SkipSpcCRsub(buffer,&ptr);
	m=1;
	if ( c == '[' ) { 
		while ( 1 ) {
			if ( c != '[' ) return ;
			ptr++;
			SkipSpcCRsub(buffer,&ptr);
			n=1;
			while ( 1 ) {
				data=Eval2( buffer, &ptr ); if ( ErrorNo ) return;
				c=SkipSpcCRsub(buffer,&ptr);
				if ( c != ',' ) break;
				ptr++;
				SkipSpcCRsub(buffer,&ptr);
				n++;
			}
			if ( n != dimB ) return ;
			m++;
			if ( c != ']' ) break;
			ptr++;
			c=SkipSpcCRsub(buffer,&ptr);
			if ( EvalEndCheck( c ) ) break;
		}
	}
	if ( c == ']' ) ptr++;
	dimA=m;

	exptr2=ptr;
	ptr=exptr;
	base=MatAry[reg].Base;
	m=base; n=base;
	dimA2=MatAry[reg].SizeA-1+base;
	dimB2=MatAry[reg].SizeB-1+base;

	while ( m < dimA+base ) {
		n=base;
		while ( n < dimB+base ) {
			data=Eval2( buffer, &ptr );
			if ( ( m<=dimA2 ) && ( n<=dimB2 ) ) WriteMatrix( reg, m, n, data);
			SkipSpcCRsub(buffer,&ptr);
			ptr++;	// , skip
			SkipSpcCRsub(buffer,&ptr);
			n++;
		}
		ptr++;
		SkipSpcCRsub(buffer,&ptr);
		m++;
	}
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

void EditMatrix(int reg, int ans ){		// ----------- Edit Matrix
	char buffer[32];
	unsigned int key;
	int	cont=1;
	int seltopY=0,seltopX=0;
	int i,j,dimA,dimB,x,y;
	int selectX=0, selectY=0;
	double value;
	int ElementSize;
	int dx,dy,MaxX,MaxY,MaxDX,MaxDY,adjX=3;
	int bit=0;	// 0:normal  1:bin  16~64:hex
	int base;
	int strdisp=0;
	int dotedit=0;
	int list=0;
	if ( reg >= 0x1000 ) { list=1; reg&=0x0FFF; }
	base=MatAry[reg].Base;
	ElementSize=MatAry[reg].ElementSize;
	if (MatAry[reg].SizeA==0) return;
//	if ( ( ( 28 <= reg ) && ( reg <= 31 ) ) || ( ( 28+32 <= reg ) && ( reg <= 31+32 ) ) ) ans=1; // Ans

	while (cont) {
		if ( MatXYmode ) {
			dimA=MatAry[reg].SizeA-1;	//	X,Y
			dimB=MatAry[reg].SizeB-1;
		} else {
			dimB=MatAry[reg].SizeA-1;	//	m,n
			dimA=MatAry[reg].SizeB-1;
		}

		MaxDY=5; if (MaxDY>dimB) MaxDY=dimB;
		MaxY=4;
		
		if ( dotedit ) {
			MaxDX=31; if (MaxDX>dimA) MaxDX=dimA;
			MaxX =31;
			MaxDY=11; if (MaxDY>dimB) MaxDY=dimB;
			MaxY =11;
			dx=3;
			dy=3;
			adjX=0;
		} else
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
		if ( (selectY-seltopY) > MaxY ) seltopY = selectY-MaxY;
		if ( (dimB -seltopY) < MaxY ) seltopY = dimB -MaxY; 
		if ( seltopY < 0 ) seltopY=0;

		if (  selectX<seltopX ) seltopX = selectX;
		if ( (selectX-seltopX) > MaxX ) seltopX = selectX-MaxX;
		if ( (dimA -seltopX) < MaxX ) seltopX = dimA -MaxX; 
		if ( seltopX < 0 ) seltopX=0;

		j=0;
		if ( reg<84 ) {
			j=SetVarCharMat( buffer, reg); buffer[j]='\0';
			if ( ( 32<=reg ) && ( list ) ) {
				if ( reg>=58 ) i=reg-57;
				else
				if ( reg<=57 ) i=reg-5;
				sprintf(buffer,"%d",i);
			}
		} else {
			if ( list ) i=reg-31; else i=reg-31;
			sprintf(buffer,"%d",i);
		}
		locate( 1, 1); Print((unsigned char*)buffer);

		if ( dotedit ) {
			Bdisp_DrawLineVRAM( 20,7,MaxDX*dx+22,7);
			sprintf((char*)buffer,"%d",seltopX+base);
			PrintMini(     0*dx+22,1,(unsigned char*)buffer,MINI_OVER);
			sprintf((char*)buffer,"%3d",seltopX+MaxDX+base);
			PrintMini( MaxDX*dx+14,1,(unsigned char*)buffer,MINI_OVER);
			Bdisp_DrawLineVRAM( 16,8,16,12+MaxDY*dy);
			x=(dimA+1)*dx+23 ;
			if ( dimA == seltopX+MaxX ) x=(MaxX+1)*dx+23 ;
			if ( x < 128 ) Bdisp_DrawLineVRAM( x, 8, x, 12+MaxDY*dy);
		} else {
			for ( x=0; x<=MaxDX; x++ ) { 
				if ( ( x >= 8 ) && ( MaxX==8 ) ) break;
				Bdisp_DrawLineVRAM( x*dx+20,7,x*dx+20+dx-5,7);
				if ( MaxX==7 ) 	sprintf((char*)buffer,"%2d",seltopX+x+base);
				else 			sprintf((char*)buffer,"%3d",seltopX+x+base);
				PrintMini(x*dx+16+(dx-5)/2,1,(unsigned char*)buffer,MINI_OVER);
			}
			Bdisp_DrawLineVRAM( 16,8,16,14+MaxDY*8);
			x=(dimA+1)*dx+20-adjX/2 ;
			if ( dimA == seltopX+MaxX ) x=(MaxX+1)*dx+20-adjX/2 ;
			if ( x < 128 ) Bdisp_DrawLineVRAM( x, 8, x, 14+MaxDY*8);
		}

		if ( dotedit ) {
			for ( y=0; y<=MaxDY; y++) {
				if ( ( y==0 ) || ( y==MaxDY ) ) {
					sprintf((char*)buffer,"%4d",seltopY+y+base);
					PrintMini(0,8+dy*y,(unsigned char*)buffer,MINI_OVER);
				}
				x=(dimA+1)*dx+23;
				if ( dimA == seltopX+MaxX ) x=(MaxX+1)*dx+23;
				if ( seltopY   == 0    ) {	Bdisp_ClearLineVRAM( 16, 8,     16,10     ); Bdisp_DrawLineVRAM( 16,10,      18,10    ); 
						if ( x < 128 )	 {	Bdisp_ClearLineVRAM(  x, 8,      x,10     ); Bdisp_DrawLineVRAM(  x,10,     x-2,10    ); }
				}
				if ( seltopY+y == dimB ) {	Bdisp_ClearLineVRAM( 16,12+y*dx,16,12+y*dx); Bdisp_DrawLineVRAM( 16,12+y*dx, 18,12+y*dx);
						if ( x < 128 )	 {	Bdisp_ClearLineVRAM(  x,12+y*dx, x,12+y*dx); Bdisp_DrawLineVRAM(  x,12+y*dx,x-2,12+y*dx); }
				}
				for ( x=0; x<=MaxDX; x++) {
					if ( MatXYmode ) value=ReadMatrix( reg, seltopX+x+base, seltopY+y+base);
					else			 value=ReadMatrix( reg, seltopY+y+base, seltopX+x+base);
					if ( value )	ML_point( x*dx+21, y*dy+10, 3, ML_BLACK );
					else			ML_point( x*dx+21, y*dy+10, 1, ML_BLACK );
				}
			}
			MiniDotCursorflag=0;		// mini cursor initialize
			MatDotEditCursorFlashing();
		} else {
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
				if ( ( strdisp ) && (ElementSize > 2 ) )  {
					if ( MatXYmode==0 ) OpcodeStringToAsciiString( buffer, MatrixPtr(reg, seltopY+y+base, seltopX  +base), 32-1 );
					else				OpcodeStringToAsciiString( buffer, MatrixPtr(reg, seltopX  +base, seltopY+y+base), 32-1 );
					PrintMini( 20,y*8+10, (unsigned char*)buffer,MINI_OVER );	// string disp
				} else {
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
//							Bdisp_PutDisp_DD();
						}
					}
				}
			}
		}

		if ( ( seltopX ) )               	 PrintMini( 16,1,(unsigned char*)"\xE6\x90",MINI_OVER);	// <-
		if ( ( seltopX==0 )&&( dimA>MaxX ) ) PrintMini(122,1,(unsigned char*)"\xE6\x91",MINI_OR);	// ->

		if ( ( strdisp ) && ( ElementSize > 2 ) )  {
			if ( MatXYmode==0 ) OpcodeStringToAsciiString( buffer, MatrixPtr(reg, selectY+base, selectX+base), 32-1 );
			else				OpcodeStringToAsciiString( buffer, MatrixPtr(reg, selectX+base, selectY+base), 32-1 );
			locate(1,7); PrintLine((unsigned char *)" ",21);
			locate(1,7); Print((unsigned char *)buffer);
		} else {
			if ( MatXYmode ) value=ReadMatrix( reg, selectX+base, selectY+base);
			else			 value=ReadMatrix( reg, selectY+base, selectX+base);
			sprintG(buffer, value,21,RIGHT_ALIGN);
			locate(1,7); Print((unsigned char*)buffer);
		}
		y = (selectY-seltopY) ;
		x = (selectX-seltopX) ;
		if ( dotedit == 0 ) Bdisp_AreaReverseVRAM(x*dx+20, y*8+9, x*dx+20+dx-5, y*8+15);	// reverse select element

		if ( ans==0 ) {
			if ( dotedit ) 	Fkey_dspRB( FKeyNo1, "0<>1");
			else			Fkey_Icon( FKeyNo1,  42 );	//	Fkey_dispN( FKeyNo1, "Edit");
		}
		Fkey_dispN( FKeyNo2, "GOTO");
		if ( ans==0 ) Fkey_Icon( FKeyNo3,  95 );	//	Fkey_dispR( FKeyNo3, "Init");
		if ( MatXYmode ) Fkey_dispN( FKeyNo4, "X,Y"); else Fkey_dispN( FKeyNo4, "m,n"); 
		locate(16, 8); MatAryElementSizePrint( ElementSize ) ;

		
		if ( dotedit  ) {
			MiniDotCursorX = x*dx+20;
			MiniDotCursorY = y*dy+9;
			MatDotEditCursorSetFlashMode( 1 );
		}
		Bdisp_PutDisp_DD();
		if ( dotedit ) GetKey_DisableMenu(&key); else GetKey(&key);
		MatDotEditCursorSetFlashMode( 0 );
		switch (key) {
			case KEY_CTRL_AC:
				if ( ans==0 ) break;
			case KEY_CTRL_EXIT:
				cont=0;
				break;
		
			case KEY_CTRL_EXE:
				if ( ans ) cont=0;
				break;
				
			case KEY_CTRL_F1:
				if ( ans ) break;
				if ( dotedit ) {
						if ( MatXYmode ) {
							if ( ( selectX <= dimA ) && ( selectY <= dimB ) ) {
								value=ReadMatrix( reg, selectX+base, selectY+base);
								if ( value ) value=0; else value=1;
								WriteMatrix( reg, selectX+base, selectY+base, value );
							}
						} else {
							if ( ( selectX <= dimA ) && ( selectY <= dimB ) ) {
								value=ReadMatrix( reg, selectY+base, selectX+base);
								if ( value ) value=0; else value=1;
								WriteMatrix( reg, selectY+base, selectX+base, value );
							}
						}
				} else {
					locate(1,8); PrintLine((unsigned char *)" ",21);
					locate(1,8); MatAryElementSizePrint( ElementSize ) ;
					if ( ( strdisp ) && ( ElementSize > 2 ) )  {
						if ( MatXYmode==0 )	key=InputStr( 1, 7, (MatAry[reg].SizeB-selectX)*ElementSize/8-1,  MatrixPtr(reg, selectY+base, selectX+base), (MatAry[reg].SizeB-selectX)*ElementSize/8-1, " ", REV_OFF)	;
						else				key=InputStr( 1, 7, (MatAry[reg].SizeA-selectY)*ElementSize/8-1,  MatrixPtr(reg, selectX+base, selectY+base), (MatAry[reg].SizeA-selectY)*ElementSize/8-1, " ", REV_OFF)	;
					} else {
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
					}
				}
				break;
			case KEY_CTRL_F2:
				if ( MatXYmode ) GotoMatrixElement( reg, &selectX, &selectY );
				else			 GotoMatrixElement( reg, &selectY, &selectX );
				break;
			case KEY_CTRL_F3:
				if ( ans ) break;
				MatDefaultValue = InitMatrix( reg, MatDefaultValue ,ElementSize ) ;
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
			case KEY_CTRL_VARS:
				if ( strdisp ) strdisp=0; else strdisp=1;
				break;
			case KEY_CTRL_OPTN:
				if ( dotedit ) dotedit=0; else dotedit=1;
				break;

			case KEY_CTRL_SHIFT:
				if ( dotedit  ) MatDotEditCursorSetFlashMode( 1 );
				if ( dotedit ) GetKey_DisableMenu(&key); else GetKey(&key);
				MatDotEditCursorSetFlashMode( 0 );
				switch ( key ) {			
					case KEY_CTRL_CLIP:
						Mat2Clip( reg, ClipBuffer, ClipMax-16 );
						break;
					case KEY_CTRL_PASTE:
						Clip2Mat( ClipBuffer, reg );
						break;
					default:
						break;
				}
				break;
			default:
				break;
		}
		key=MathKey( key );
		if ( ( ans==0 ) && ( key ) ) {
				locate(1,8); PrintLine((unsigned char *)" ",21);
				locate(1,8); MatAryElementSizePrint( ElementSize ) ;
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
char listdsp=0;	// Mat:0  list:1
int SetMatrix(int select){		// ----------- Set Matrix
	char buffer[22];
	unsigned int key;
	int	cont=1;
	int seltop=select;
	int i,j,k,x,y,len,oplen;
	int opNum=58-1;
	int reg,dimA,dimB,ElementSize;
	int base;
	int small=0;
	int listselect=1;
	char MatRegData[]={ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,
                       32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,26,27,28};
                       
	char LstRegData[]={58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,26,27,28,
                       32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,26,27,28};

	if ( dspflag == 3 ) listdsp=0;	// Mat
	if ( dspflag == 4 ) listdsp=1;	// List

	Cursor_SetFlashMode(0); 		// cursor flashing off

	while (cont) {
		Bdisp_AllClr_VRAM();
		
		if (  select<seltop ) seltop = select;
		if ( (select-seltop) > 6 ) seltop = select-6;
		if ( (opNum -seltop) < 6 ) seltop = opNum -6; 
		for ( i=0; i<7; i++ ) {
			j=0;
			k=seltop+i; if ( small ) { if ( k>=29 ) k-=29; else k+=29; }
			if ( listdsp )  {
				reg=LstRegData[k];
				if ( ( 26<=reg ) && ( reg<=28 ) ) { 
					j=SetVarCharMat( buffer, reg); 
				} else  { 
					k=k+1; if ( reg<58 ) k-=3;
					sprintf( buffer,"%d", k ); j=strlen(buffer);
				}
			} else {
				reg=MatRegData[k]; j=SetVarCharMat( buffer, reg);
			}
			buffer[j]='\0';
			locate( 1, 1+i); if ( listdsp )  Print((unsigned char*)"Lst"); else Print((unsigned char*)"Mat");
			locate( 5, 1+i); Print((unsigned char*)buffer);
			locate(13, 1+i); Print((unsigned char*)":");
			x=5+CB_MB_ElementCount( buffer );
			locate( x, 1+i); MatAryElementSizePrint( MatAry[reg].ElementSize ) ;
			if ( MatAry[reg].SizeA ) {
				sprintf((char*)buffer,"%3d",MatAry[reg].SizeA);
				locate(14,1+i); Print((unsigned char*)buffer);
				len=strlen((char*)buffer)-3;
				locate(17+len,1+i); Print((unsigned char*)"\xA9");
				if (len) sprintf((char*)buffer,"%2d",MatAry[reg].SizeB);
				else	 sprintf((char*)buffer,"%3d",MatAry[reg].SizeB);
				locate(18+len,1+i); Print((unsigned char*)buffer);
			} else {
				locate(14,1+i); Print((unsigned char*)"None");
			}
		}

		y = (select-seltop) ;
		Bdisp_AreaReverseVRAM(0, y*8, 127, y*8+7);	// reverse select line 

		Fkey_Icon( FKeyNo1,  56 );	//	Fkey_dispR( FKeyNo1, "DEL");
		Fkey_Icon( FKeyNo2, 260 );	//	Fkey_dispR( FKeyNo2, "DelA");
		Fkey_Icon( FKeyNo3, 306 );	//	Fkey_dispR( FKeyNo3, "DIM");
		Fkey_Icon( FKeyNo4,  95 );	//	Fkey_dispR( FKeyNo4, "Init");
		if ( listdsp )  Fkey_dispN( FKeyNo5, "Lst:"); else Fkey_dispN( FKeyNo5, "Mat:");
//		Fkey_Icon( FKeyNo6, 775 );	//	Fkey_dispN( FKeyNo6,  "A<>a");
		if ( small  ) Fkey_dispN_aA( FKeyNo6, "A<>a"); else Fkey_dispN_Aa( FKeyNo6, "A<>a");

		Bdisp_PutDisp_DD();

		k=select; if ( small ) { if ( k>=29 ) k-=29; else k+=29; }
		if ( listdsp )  { reg=LstRegData[k]; }
		else            { reg=MatRegData[k]; }
		ElementSize=MatAry[reg].ElementSize;

		GetKey( &key );
		switch (key) {
			case KEY_CTRL_EXIT:
				cont=0;
				break;
		
			case KEY_CTRL_EXE:
				if ( ElementSize==0 ) {	ElementSize = CB_INT? 32:64 ;
					key=SetDimension(reg, &dimA, &dimB, &ElementSize, &base);
					if ( key==KEY_CTRL_EXIT ) break;
					if ( DimMatrixSub(reg, ElementSize, dimA, dimB, base ) ) CB_ErrMsg(NotEnoughMemoryERR);
				}
				EditMatrix( reg + 0x1000*(listdsp!=0), 0 );
				break;
				
			case KEY_CTRL_F1:
				if ( YesNo( "Delete Matrix?") ==0 ) break;
				DeleteMatrix(reg);
				break;
			case KEY_CTRL_F2:
				if ( YesNo2( "Delete","    All Matrices?") ==0 ) break;
				DeleteMatrix(-1);
				break;
			case KEY_CTRL_F3:
			case KEY_CTRL_LEFT:
				if ( ElementSize==0 ) 	ElementSize = CB_INT? 32:64 ;
				key=SetDimension(reg, &dimA, &dimB, &ElementSize, &base);
				if ( key==KEY_CTRL_EXIT ) break;
				if ( DimMatrixSub(reg, ElementSize, dimA, dimB, base ) ) CB_ErrMsg(NotEnoughMemoryERR);
				HiddenRAM_MatAryStore();	// MatAry ptr -> HiddenRAM
				EditMatrix( reg, 0 );
				break;
			case KEY_CTRL_RIGHT:
				select+=7;
				if ( select >= opNum  ) select=opNum;
				break;
			case KEY_CTRL_F4:
				MatDefaultValue = InitMatrix( reg, MatDefaultValue ,ElementSize ) ;
				break;
			case KEY_CTRL_F5:
				if ( listdsp ) {
					listselect=SelectNum2( "List", listselect ,1, ExtListMax);
					reg=listselect;
				 	if ( reg<=52 ) { select=reg-1;
						if ( 27<=reg ) { reg+=5; select+=3; }
						else reg+=57;
					} else reg+=31;
				} else {
					listselect=SelectNum2( "Mat ", listselect ,1, ExtListMax);
					reg=listselect;
				 	if ( reg<=52 ) { select=reg-1;
						if ( 27<=reg ) { reg+=5; select+=3; }
						else reg--;
					} else reg+=31;
				}
				ElementSize=MatAry[reg].ElementSize;
				if ( ElementSize == 0) {	ElementSize = CB_INT? 32:64 ;
					key=SetDimension(reg, &dimA, &dimB, &j, &base);
					if ( key==KEY_CTRL_EXIT ) break;
					if ( DimMatrixSub(reg, ElementSize, dimA, dimB, base ) ) CB_ErrMsg(NotEnoughMemoryERR);
				}
				EditMatrix( reg + 0x1000*(listdsp!=0), 0 );
				break;
			case KEY_CTRL_F6:
				small=1-small;
				break;
				
			case KEY_CTRL_UP:
				select--;
				if ( select < 0 )  { select = opNum; }
				break;
			case KEY_CTRL_DOWN:
				select++;
				if ( select > opNum ) select =0;
				break;

			case KEY_CTRL_OPTN:
				if ( listdsp ) { listdsp=0; } else { listdsp=1; } 
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
int ElementSizeSelect( char *SRC, int *base, int ElementSize ) {
	int c,d;
	*base=MatBase;
	c =SRC[ExecPtr];
	if ( c=='.' ) {
		c =SRC[++ExecPtr];
		if (CB_INT)	ElementSize=32; else ElementSize=64;
		if ( ( c=='0' ) || ( c=='1' ) ) { *base = c-'0'; c=SRC[++ExecPtr]; }
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
		if ( ElementSize == 0 ) ElementSize = ( CB_INT? 32:64 ) +0x100 ;
	}
	return ElementSize;
}

void CB_MatrixInitsubNoMat( char *SRC, int *reg, int dimA, int dimB , int ElementSize, int dimdim ) { 	// 1-
	int c;
	int base=MatBase;
	*reg=MatRegVar(SRC);
	if ( *reg>=0 ) {
		ElementSize=ElementSizeSelect( SRC, &base, ElementSize) & 0xFF;
		if ( dimdim )	DimMatrixSubNoinit( *reg, ElementSize, dimA, dimB, base);
		else			DimMatrixSub( *reg, ElementSize, dimA, dimB, base);
	} else { CB_Error(SyntaxERR); return; }  // Syntax error
}
void CB_MatrixInitsub( char *SRC, int *reg, int dimA, int dimB , int ElementSize, int dimdim ) { 	// 1-
	int c,d;
	int base=MatBase;
	c =SRC[ExecPtr];
	d =SRC[ExecPtr+1];
	if ( ( c != 0x7F ) || ( d !=0x40 ) ) { CB_Error(SyntaxERR); return; }  // no Mat  Syntax error
	ExecPtr+=2;
	CB_MatrixInitsubNoMat( SRC, &(*reg), dimA, dimB, ElementSize, dimdim );
}

void CB_MatrixInit( char *SRC, int dimdim ) { //	{n,m}->Dim Mat A[.B][.W][.L][.F]
	int c,d;
	int dimA,dimB,i;
	int reg;
	int base;
	
	if ( MatAry[CB_MatListAnsreg].SizeA != 2 ) { CB_Error(ArgumentERR); return ; } // Argument error

	base = MatAry[CB_MatListAnsreg].Base;
	dimA = ReadMatrix( CB_MatListAnsreg, base  , base) ;
	dimB = ReadMatrix( CB_MatListAnsreg, base+1, base) ;

	CB_MatrixInitsub( SRC, &reg, dimA, dimB, 0, dimdim );
}

//-----------------------------------------------------------------------------

int SkipSpcCR( char *SRC ) {
	if ( SRC[ExecPtr] == 0x0D ) ExecPtr++; // Skip [CR]
	while ( SRC[ExecPtr]==0x20 ) ExecPtr++; // Skip Space
	return SRC[ExecPtr];
}

void CB_Matrix( char *SRC ) { //	[[1.2,3][4,5,6]]->Mat Ans
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
	int ElementSize;
	
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
//		if ( c == ']' ) break;
		if ( c != ',' ) break;  // Syntax error
		ExecPtr++;
		SkipSpcCR(SRC);
		n++;
	}
	dimB=n;
	if ( c == ']' ) ExecPtr++;
	c=SkipSpcCR(SRC);
	m=1;
	if ( c == '[' ) { 
		while ( 1 ) {
			if ( c != '[' ) { CB_Error(SyntaxERR); return; }  // Syntax error
			ExecPtr++;
			SkipSpcCR(SRC);
			n=1;
			while ( 1 ) {
				data=CB_EvalDbl( SRC );
				c=SkipSpcCR(SRC);
				if ( c != ',' ) break;
				ExecPtr++;
				SkipSpcCR(SRC);
				n++;
			}
			if ( n != dimB ) { CB_Error(DimensionERR); return; }  // Dimension error
			m++;
			if ( c != ']' ) break;
			ExecPtr++;
			c=SkipSpcCR(SRC);
			if ( EvalEndCheck( c ) ) break;
		}
	}
	if ( c == ']' ) ExecPtr++;
	dimA=m;

	ElementSize=ElementSizeSelect( SRC, &base, 0) & 0xFF;
	NewMatListAns( dimA, dimB, base, ElementSize );
	if ( ErrorNo ) return ;
	reg=CB_MatListAnsreg;

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
		ExecPtr++;
		SkipSpcCR(SRC);
		m++;
	}
	ExecPtr=exptr2;
	dspflag=3;	// Matrix data
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

int MatEndCheck( int c ) {
	if ( c==':' ) return c;
	if ( c==0x0E ) return c;	// ->
	if ( c==0x13 ) return c;	// =>
	if ( c==')' ) return c;
	if ( c==']' ) return c;
	if ( c=='}' ) return c;
	if ( c==0x0D ) return c;	// <CR>
	if ( c==0x0C ) return c;	// <Dsps>
	return ( c==0 );
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
	char buffer[256];
	int len;
	int maxlen=0;
	int base=MatBase;
	int ElementSize;
	
//	c=SkipSpcCR(SRC);
//	if ( c != '"' ) { CB_Error(SyntaxERR); return; }  // Syntax error
	ExecPtr++;
	exptr=ExecPtr;
	m=1;
//	SkipSpcCR(SRC);
	while ( 1 ) {
		len=MatGetOpcode(SRC, buffer, 255);
		if ( len > maxlen ) maxlen=len;
		c=SkipSpcCR(SRC);
		if ( MatEndCheck( c ) ) break;
		if ( c != ',' ) { CB_Error(SyntaxERR); return; }  // Syntax error
		ExecPtr++;
		c=SkipSpcCR(SRC);
		if ( c != 0x22 ) { CB_Error(SyntaxERR); return; }  // Syntax error
		ExecPtr++;
		SkipSpcCR(SRC);
		m++;
	}
	if ( c == ']' ) ExecPtr++;
	dimA=m;
	dimB=maxlen+1;

	ElementSize=8;
	NewMatListAns( dimA, dimB, base, ElementSize );
	if ( ErrorNo ) return ;
	reg=CB_MatListAnsreg;

	exptr2=ExecPtr;
	ExecPtr=exptr;
	base=MatAry[reg].Base;
	m=base; n=base;
	while ( m < dimA+base ) {
		cptr=MatrixPtr( reg, m, n );
		len=MatGetOpcode(SRC, cptr, 255);
		c=SkipSpcCR(SRC);
		if ( MatEndCheck( c ) ) break;
		if ( c != ',' ) { CB_Error(SyntaxERR); return; }  // Syntax error
		ExecPtr++;
		c=SkipSpcCR(SRC);
		if ( c != 0x22 ) { CB_Error(SyntaxERR); return; }  // Syntax error
		ExecPtr++;
		SkipSpcCR(SRC);
		m++;
	}
	ExecPtr=exptr2;
	dspflag=3;	// Matrix data
}

//-----------------------------------------------------------------------------

void CB_ClrMat( char *SRC ) { //	ClrMat A
	int reg=MatRegVar(SRC);
	if ( reg>=0 ) {
		DeleteMatrix( reg );
	} else { ErrorNo=0;	//
		DeleteMatrix(-1);	// ClrMat
	}
}
void CB_ClrList( char *SRC ) { //	ClrList 1
	int i;
	int reg=ListRegVar( SRC );
	if ( reg>=0 ) {
		DeleteMatrix( reg );
	} else { ErrorNo=0;	//
		for ( i=32; i<32+26; i++ ) DeleteMatrix( i );	// ClrList 1-26
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

void MatCalcDimCopySub( char *SRC, int reg, int reg2, int dim ) {
	int base,base2;
	int dimA,dimB,dimA2,dimB2;
	int ElementSize,ElementSize2;
	ElementSize =MatAry[reg].ElementSize;
	ElementSize2=ElementSizeSelect( SRC, &base2, 0 ) & 0xFF;
	if ( dim ) { 		// Mat A -> Dim Mat B[.w]
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
		if ( reg == reg2 ) return;
		DimMatrixSub( reg2, ElementSize, MatAry[reg].SizeA, MatAry[reg].SizeB, MatAry[reg].Base );
		CopyMatrix( reg2, reg );
	}
}

int CB_MatCalc( char *SRC ) { //	Mat A -> Mat B  etc
	char dspflagtmp;
	int c,d,i,m,n;
	int dim=0,dimA,dimB,dimA2,dimB2;
	int ElementSize,ElementSize2;
	int reg,reg2;
	int base,base2;
	int excptr=ExecPtr;
	double	*dptr, *dptr2;
	
	reg=MatRegVar(SRC);
	c =SRC[ExecPtr];
	if ( c == 0x0E ) {	// ->
		if ( reg>=0 ) {
			if ( MatAry[reg].SizeA == 0 ) { CB_Error(NoMatrixArrayERR); return; }	// No Matrix Array error
		} else { CB_Error(SyntaxERR); return; }	// Syntax error
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
		ExecPtr++;
		reg2=MatRegVar(SRC);
		if ( reg2<0 ) { CB_Error(SyntaxERR); return; }	// Syntax error
		
		MatCalcDimCopySub( SRC, reg, reg2, dim ); if ( ErrorNo ) return ;

		dspflagtmp=0;
	} else {
		ExecPtr=excptr;
		dspflagtmp=2;
	}
	return dspflagtmp;
}

int CB_ListCalc( char *SRC ) { //	List 1 -> List 2  etc
	char dspflagtmp;
	int c,d,i,m,n;
	int dim=0,dimA,dimB,dimA2,dimB2;
	int ElementSize,ElementSize2;
	int reg,reg2;
	int base,base2;
	int excptr=ExecPtr;
	double	*dptr, *dptr2;
	
	reg=ListRegVar( SRC );
	c =SRC[ExecPtr];
	if ( c == 0x0E ) {	// ->
		if ( reg>=0 ) {
			if ( MatAry[reg].SizeA == 0 ) { CB_Error(NoMatrixArrayERR); return; }	// No Matrix Array error
		} else { CB_Error(SyntaxERR); return; }	// Syntax error
		ExecPtr++;
		c =SRC[ExecPtr];
		if ( c != 0x7F ) { CB_Error(SyntaxERR); return; }	// Syntax error
		c =SRC[++ExecPtr];
		if ( c == 0x46 ) { 		// List 1 -> Dim List 2[.w]
			dim=1;
			c =SRC[++ExecPtr];
			if ( c != 0x7F ) { CB_Error(SyntaxERR); return; }	// Syntax error
			c =SRC[++ExecPtr];
		}
		if ( ( c !=0x51 ) ) { CB_Error(SyntaxERR); return; }	// Syntax error
		ExecPtr++;
		reg2=ListRegVar( SRC );
		if ( reg2<0 ) { CB_Error(SyntaxERR); return; }	// Syntax error

		MatCalcDimCopySub( SRC, reg, reg2, dim ); if ( ErrorNo ) return ;
		
		dspflagtmp=0;
	} else {
		ExecPtr=excptr;
		dspflagtmp=2;
	}
	return dspflagtmp;
}


//-----------------------------------------------------------------------------
void CB_MatFill( char *SRC ) { //	Fill(value, Mat A)		Fill(value, List 1)
	int c,d;
	int dimA,dimB,i;
	int reg;
	double	*dptr, *dptr2;
	double value;
	
	value=CB_EvalDbl( SRC );
	c =SRC[ExecPtr];
	if ( c != ',' ) { CB_Error(SyntaxERR); return; }  // Syntax error
	ExecPtr++;
	MatrixOprandreg( SRC, &reg);
	if ( reg>=0 ) {
		if ( MatAry[reg].SizeA == 0 ) { CB_Error(NoMatrixArrayERR); return; }	// No Matrix Array error
	} else { CB_Error(SyntaxERR); return; }	// Syntax error
	if ( SRC[ExecPtr] == ')' ) ExecPtr++;
	
	InitMatSub( reg,value);
}

//-----------------------------------------------------------------------------
void CB_MatSwap( char *SRC ) {	// Swap Mat A,2,3
	int c,d;
	int dimA,dimB,m,n;
	int reg;
	int base;
	int ElementSize;
	double	dtmp,dtmp2;
	int itmp,itmp2;
	int a,b;
	
	MatrixOprandreg( SRC, &reg);
	if ( reg>=0 ) {
		if ( MatAry[reg].SizeA == 0 ) { CB_Error(NoMatrixArrayERR); return; }	// No Matrix Array error
	} else { CB_Error(SyntaxERR); return; }	// Syntax error
	if ( SRC[ExecPtr] != ',' ) { CB_Error(SyntaxERR); return; }  // Syntax error
	ExecPtr++;

	ElementSize=MatAry[reg].ElementSize;
	base       =MatAry[reg].Base;
	dimA       =MatAry[reg].SizeA+base;
	dimB       =MatAry[reg].SizeB+base;
	
	a = CB_EvalInt( SRC );
	if ( ( a < base ) || ( dimA < a ) ) { CB_Error(ArgumentERR); return ; } // Argument error

	if ( SRC[ExecPtr] != ',' ) { CB_Error(SyntaxERR); return; }  // Syntax error
	ExecPtr++;
	b = CB_EvalInt( SRC );
	if ( ( b < base ) || ( dimA < b ) ) { CB_Error(ArgumentERR); return ; } // Argument error

	if ( SRC[ExecPtr] == ')' ) ExecPtr++;

	if ( a == b ) return;

	switch ( ElementSize ) {
		case 64:
			for ( n=base; n<dimB; n++ ) {
				dtmp  = ReadMatrix( reg, a, n );
				dtmp2 = ReadMatrix( reg, b, n );
				WriteMatrix( reg, b, n, dtmp  );
				WriteMatrix( reg, a, n, dtmp2 );
			}
			break;
		default:
			for ( n=base; n<dimB; n++ ) {
				itmp  = ReadMatrixInt( reg, a, n );
				itmp2 = ReadMatrixInt( reg, b, n );
				WriteMatrixInt( reg, b, n, itmp  );
				WriteMatrixInt( reg, a, n, itmp2 );
			}
			break;
	}
}

void CB_MatxRow( char *SRC ) {	// *Row 5,A,2
	int c,d;
	int dimA,dimB,m,n;
	int reg;
	int base;
	int ElementSize;
	int a,b;
	double dbl_k;
	int  int_k;

	if ( CB_INT) int_k = CB_EvalInt( SRC ); else dbl_k = CB_EvalDbl( SRC ); 
	if ( SRC[ExecPtr] != ',' ) { CB_Error(SyntaxERR); return; }  // Syntax error
	ExecPtr++;

	MatrixOprandreg( SRC, &reg);
	if ( reg>=0 ) {
		if ( MatAry[reg].SizeA == 0 ) { CB_Error(NoMatrixArrayERR); return; }	// No Matrix Array error
	} else { CB_Error(SyntaxERR); return; }	// Syntax error
	if ( SRC[ExecPtr] != ',' ) { CB_Error(SyntaxERR); return; }  // Syntax error
	ExecPtr++;

	ElementSize=MatAry[reg].ElementSize;
	base       =MatAry[reg].Base;
	dimA       =MatAry[reg].SizeA+base;
	dimB       =MatAry[reg].SizeB+base;
	
	a = CB_EvalInt( SRC );
	if ( ( a < base ) || ( dimA < a ) ) { CB_Error(ArgumentERR); return ; } // Argument error

	if ( SRC[ExecPtr] == ')' ) ExecPtr++;

	switch ( ElementSize ) {
		case 64:
			for ( n=base; n<dimB; n++ ) {
				WriteMatrix( reg, a, n, ReadMatrix( reg, a, n )*dbl_k );
			}
			break;
		default:
			for ( n=base; n<dimB; n++ ) {
				WriteMatrixInt( reg, a, n, ReadMatrixInt( reg, a, n )*int_k );
			}
			break;
	}
}

void CB_MatxRowPlus( char *SRC ) {	// *Row+ 5,A,2,3
	int c,d;
	int dimA,dimB,m,n;
	int reg;
	int base;
	int ElementSize;
	int a,b;
	double dbl_k;
	int  int_k;

	if ( CB_INT) int_k = CB_EvalInt( SRC ); else dbl_k = CB_EvalDbl( SRC ); 
	if ( SRC[ExecPtr] != ',' ) { CB_Error(SyntaxERR); return; }  // Syntax error
	ExecPtr++;

	MatrixOprandreg( SRC, &reg);
	if ( reg>=0 ) {
		if ( MatAry[reg].SizeA == 0 ) { CB_Error(NoMatrixArrayERR); return; }	// No Matrix Array error
	} else { CB_Error(SyntaxERR); return; }	// Syntax error
	if ( SRC[ExecPtr] != ',' ) { CB_Error(SyntaxERR); return; }  // Syntax error
	ExecPtr++;

	ElementSize=MatAry[reg].ElementSize;
	base       =MatAry[reg].Base;
	dimA       =MatAry[reg].SizeA+base;
	dimB       =MatAry[reg].SizeB+base;
	
	a = CB_EvalInt( SRC );
	if ( ( a < base ) || ( dimA < a ) ) { CB_Error(ArgumentERR); return ; } // Argument error

	if ( SRC[ExecPtr] != ',' ) { CB_Error(SyntaxERR); return; }  // Syntax error
	ExecPtr++;
	b = CB_EvalInt( SRC );
	if ( ( b < base ) || ( dimA < b ) ) { CB_Error(ArgumentERR); return ; } // Argument error

	if ( SRC[ExecPtr] == ')' ) ExecPtr++;

	if ( a == b ) return;

	switch ( ElementSize ) {
		case 64:
			for ( n=base; n<dimB; n++ ) {
				WriteMatrix( reg, b, n, ReadMatrix( reg, a, n )*dbl_k + ReadMatrix( reg, b, n )  );
			}
			break;
		default:
			for ( n=base; n<dimB; n++ ) {
				WriteMatrixInt( reg, b, n, ReadMatrixInt( reg, a, n )*int_k + ReadMatrixInt( reg, b, n )  );
			}
			break;
	}
}

void CB_MatRowPlus( char *SRC ) {	// Row+ A,2,3
	int c,d;
	int dimA,dimB,m,n;
	int reg;
	int base;
	int ElementSize;
	int a,b;
	
	MatrixOprandreg( SRC, &reg);
	if ( reg>=0 ) {
		if ( MatAry[reg].SizeA == 0 ) { CB_Error(NoMatrixArrayERR); return; }	// No Matrix Array error
	} else { CB_Error(SyntaxERR); return; }	// Syntax error
	if ( SRC[ExecPtr] != ',' ) { CB_Error(SyntaxERR); return; }  // Syntax error
	ExecPtr++;

	ElementSize=MatAry[reg].ElementSize;
	base       =MatAry[reg].Base;
	dimA       =MatAry[reg].SizeA+base;
	dimB       =MatAry[reg].SizeB+base;
	
	a = CB_EvalInt( SRC );
	if ( ( a < base ) || ( dimA < a ) ) { CB_Error(ArgumentERR); return ; } // Argument error

	if ( SRC[ExecPtr] != ',' ) { CB_Error(SyntaxERR); return; }  // Syntax error
	ExecPtr++;
	b = CB_EvalInt( SRC );
	if ( ( b < base ) || ( dimA < b ) ) { CB_Error(ArgumentERR); return ; } // Argument error

	if ( SRC[ExecPtr] == ')' ) ExecPtr++;

	if ( a == b ) return;

	switch ( ElementSize ) {
		case 64:
			for ( n=base; n<dimB; n++ ) {
				WriteMatrix( reg, b, n, ReadMatrix( reg, a, n ) + ReadMatrix( reg, b, n )  );
			}
			break;
		default:
			for ( n=base; n<dimB; n++ ) {
				WriteMatrixInt( reg, b, n, ReadMatrixInt( reg, a, n ) + ReadMatrixInt( reg, b, n )  );
			}
			break;
	}
}
//-----------------------------------------------------------------------------
void CB_MatTrn( char *SRC ) { //	Trn Mat A -> Mat Ans
	int c,d;
	int m,n;
	int dimA,dimB,i;
	int reg,reg2;
	double	*dptr, *dptr2;
	double value;
	int base;
	int ElementSize;
	
	ListEvalsubTop(SRC);
	if ( dspflag != 3 ) { CB_Error(ArgumentERR); return ; } // Argument error
	reg = CB_MatListAnsreg;

	ElementSize=MatAry[reg].ElementSize;
	if ( ElementSize == 2 ) { CB_Error(ArgumentERR); return ; } // Argument error
	base       =MatAry[reg].Base;
	dimA       =MatAry[reg].SizeA;
	dimB       =MatAry[reg].SizeB;
	NewMatListAns( dimB, dimA, base, ElementSize );
	if ( ErrorNo ) return ;
	reg2=CB_MatListAnsreg;
	
	switch ( ElementSize ) {
		case 64:
			for ( m=base; m<dimA+base; m++ ) {
				for ( n=base; n<dimB+base; n++ ) WriteMatrix( reg2, n, m,  ReadMatrix( reg, m, n ) );
			}
			break;
		default:
			for ( m=base; m<dimA+base; m++ ) {
				for ( n=base; n<dimB+base; n++ ) WriteMatrixInt( reg2, n, m,  ReadMatrixInt( reg, m, n ) );
			}
			break;
	}
	MatAry[reg].SizeA = dimB;
	MatAry[reg].SizeB = dimA;
	CopyMatrix( reg, reg2 );	// reg2 -> reg
	DeleteMatListAns();			// delete reg2
}

//-----------------------------------------------------------------------------	List
//-----------------------------------------------------------------------------
void CB_ListInitsub( char *SRC, int *reg, int dimA , int ElementSize, int dimdim ) { 	// 1-
	int c;
	int base=MatBase;
	int dimB=1;
	*reg=ListRegVar( SRC );
	if ( *reg>=0 ) {
		ElementSize=ElementSizeSelect( SRC, &base, ElementSize) & 0xFF;
		if ( dimdim )	DimMatrixSubNoinit( *reg, ElementSize, dimA, dimB, base);
		else			DimMatrixSub( *reg, ElementSize, dimA, dimB, base);
	} else { CB_Error(SyntaxERR); return; }  // Syntax error
}

void CB_List( char *SRC ) { //	{1.2,3,4,5,6} -> List Ans
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
	int base=MatBase;
	int ElementSize;
	
	exptr=ExecPtr;
	c=SkipSpcCR(SRC);
	n=1;
	while ( 1 ) {
		data=CB_EvalDbl( SRC );
		c=SkipSpcCR(SRC);
		if ( c != ',' ) break;
		ExecPtr++;
		SkipSpcCR(SRC);
		n++;
	}
	if ( c == '}' ) ExecPtr++;
	dimA=n;
	dimB=1;

	ElementSize=ElementSizeSelect( SRC, &base, 0) & 0xFF;
	NewMatListAns( dimA, dimB, base, ElementSize );
	if ( ErrorNo ) return ;
	reg=CB_MatListAnsreg;

	exptr2=ExecPtr;
	ExecPtr=exptr;
	m=base; n=base;
	while ( m < dimA+base ) {
		if (CB_INT)	WriteMatrixInt( reg, m, n, EvalIntsubTop( SRC ));
		else 		   WriteMatrix( reg, m, n, EvalsubTop( SRC ));
		SkipSpcCR(SRC);
		ExecPtr++;	// "," skip
		SkipSpcCR(SRC);
		m++;
	}
	ExecPtr=exptr2;
	dspflag =4 ;	// List data
}

//-----------------------------------------------------------------------------
void CB_Mat2List( char *SRC ) {	// Mat>List( Mat A, m) -> List Ans
	int c;
	int reg,reg2;
	int m,n;
	int sizeA,sizeB;
	int ElementSize;
	int base;

	reg=MatRegVar(SRC);
	if ( reg<0 ) {
		ListEvalsubTop(SRC);
		if ( dspflag != 3 ) { CB_Error(ArgumentERR); return ; } // Argument error
		reg = CB_MatListAnsreg;
	}
	if ( SRC[ExecPtr] != ',' ) { CB_Error(SyntaxERR); return; }  // Syntax error
	ExecPtr++;
	n=CB_EvalInt( SRC );
	
	sizeA        = MatAry[reg ].SizeA;
	sizeB        = MatAry[reg ].SizeB;
	base         = MatAry[reg ].Base;
	ElementSize  = MatAry[reg ].ElementSize;
	if ( ElementSize  == 2 ) ElementSize  == 1;

	if ( ( n<base ) || ( n>sizeB+base ) ) { CB_Error(DimensionERR); return ; }	// Dimension error
	
	NewMatListAns( sizeA, 1, base, ElementSize );
	if ( ErrorNo ) return ;
	reg2=CB_MatListAnsreg;
	for ( m=base ; m<sizeA +base ; m++ ) WriteMatrix( reg2, m, base, ReadMatrix( reg , m, n ) );

	if ( SRC[ExecPtr] == ')' ) ExecPtr++;
	dspflag = 4;
}

void CB_List2Mat( char *SRC ) {	// List>Mat( List 1, List 2,..) -> Mat Ans
	int c;
	int reg,reg2=-1,reg3=-1;
	int tmpreg=Mattmpreg;
	int m,n;
	int sizeA,sizeB;
	int ElementSize;
	int base;
	int ansreg=CB_MatListAnsreg;

	c=1;
	reg = ListRegVar( SRC );
	if ( dspflag >= 3 ) reg = CB_MatListAnsreg;

	if ( SRC[ExecPtr] == ',' ) { 
		ExecPtr++;
		reg2 = ListRegVar( SRC );
		if ( dspflag >= 3 ) reg2 = CB_MatListAnsreg;
		c++;
		if ( SRC[ExecPtr] == ',' ) { 
			ExecPtr++;
			reg3 = ListRegVar( SRC );
			if ( dspflag >= 3 ) reg3 = CB_MatListAnsreg;
			c++;
		}
	}
	if ( ErrorNo ) return ;
	
	sizeA        = MatAry[reg ].SizeA;
	sizeB        = c;
	base         = MatAry[reg ].Base;
	ElementSize  = MatAry[reg ].ElementSize;
	if ( ElementSize  == 2 ) ElementSize  == 1;
	
	DimMatrixSub( tmpreg, ElementSize, sizeA, sizeB, base);	//
	if ( ErrorNo ) return ;

	n=base;
//  	if ( base != MatAry[reg ].Base )  { CB_Error(ArgumentERR); return ; } // Argument error
		for ( m=base ; m<sizeA +base ; m++ ) WriteMatrix( tmpreg, m, n, ReadMatrix( reg , m, base ) );
		n++;
	if ( reg2>0 ) {
		if ( base != MatAry[reg2].Base )  { CB_Error(ArgumentERR); return ; } // Argument error
		for ( m=base ; m<sizeA +base ; m++ ) WriteMatrix( tmpreg, m, n, ReadMatrix( reg2, m, base ) );
		n++;
	}
	if ( reg3>0 ) {
		if ( base != MatAry[reg3].Base )  { CB_Error(ArgumentERR); return ; } // Argument error
		for ( m=base ; m<sizeA +base ; m++ ) WriteMatrix( tmpreg, m, n, ReadMatrix( reg3, m, base ) );
		n++;
	}
	if ( SRC[ExecPtr] == ')' ) ExecPtr++;
	CB_MatListAnsreg=ansreg;
	CopyMatList2Ans( tmpreg );		// tmpreg -> CB_MatListAnsreg
	dspflag = 3;
}


int CB_RanListsub( char *SRC ) {
	int m;
	int ElementSize=64;
	
	m=CB_EvalInt( SRC ) ;
	if ( m<1 ) { CB_Error(ArgumentERR); return -1; } // Argument error
	NewMatListAns( m, 1, MatBase, ElementSize );
	if ( ErrorNo ) return -1;
	return CB_MatListAnsreg;
}

void CB_RanList( char *SRC ) {	// RanList#( 50 ) -> List Ans
	int reg;
	int m;
	int sizeA;
	int base=MatBase;

	reg = CB_RanListsub( SRC );
	if ( ErrorNo ) return ;
  	for ( m=base ; m<MatAry[reg ].SizeA +base ; m++ ) WriteMatrix( reg, m, base, frand() );

	if ( SRC[ExecPtr] == ')' ) ExecPtr++;
	dspflag = 4;
}

void CB_RanInt( char *SRC, int x, int y ) {	// RanIntNorm#( st, en [,n] ) -> List Ans
	int reg;
	int m;
	int sizeA;
	int base=MatBase;

	reg = CB_RanListsub( SRC );
	if ( ErrorNo ) return ;
  	for ( m=base ; m<MatAry[reg ].SizeA +base ; m++ ) WriteMatrix( reg, m, base, frandIntint( x, y ) );
	if ( SRC[ExecPtr] == ')' ) ExecPtr++;
	dspflag = 4;
}

double CB_RanNorm( char *SRC ) {	// RanNorm#( sd, mean [,n] ) -> List Ans
	double sd,mean;
	int reg;
	int m;
	int sizeA;
	int ElementSize=64;
	int base=MatBase;
	
	if ( Get2Eval( SRC, &sd, &mean ) == ',' ) {
		ExecPtr++;
		reg = CB_RanListsub( SRC );
		if ( ErrorNo ) return 0;
  		for ( m=base ; m<MatAry[reg ].SizeA +base ; m++ ) WriteMatrix( reg, m, base, fRanNorm( sd, mean ) );
		if ( SRC[ExecPtr] == ')' ) ExecPtr++;
		dspflag = 4;
	}
	return fRanNorm( sd, mean ) ;
}

double CB_RanBin( char *SRC ) {		// RanBin#( n, p [,m] ) -> List Ans
	double n,p;
	int reg;
	int m;
	int sizeA;
	int ElementSize=64;
	int base=MatBase;
	
	if ( Get2Eval( SRC, &n, &p ) == ',' ) {
		ExecPtr++;
		reg = CB_RanListsub( SRC );
		if ( ErrorNo ) return 0;
  		for ( m=base ; m<MatAry[reg ].SizeA +base ; m++ ) WriteMatrix( reg, m, base, fRanBin( n, p ) );
		if ( SRC[ExecPtr] == ')' ) ExecPtr++;
		dspflag = 4;
	}
	return fRanBin( n, p ) ;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CB_ArgumentMat( char *SRC, int reg ) {	// Argument( Mat A, Mat B )	
	int reg2,reg3;
	int m,n,i;
	int sizeA,sizeB,sizeA2,sizeB2,sizeB3;
	int ElementSize,ElementSize2;
	int base,base2;

	if ( SRC[ExecPtr] != ',' ) { CB_Error(SyntaxERR); return; }  // Syntax error
	ExecPtr++;
	
	ListEvalsubTop(SRC);
	if ( dspflag != 3 ) { CB_Error(ArgumentERR); return ; } // Argument error
	reg2 = CB_MatListAnsreg;

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

	if ( ( sizeA != sizeA2 ) || ( base != base2 ) || ( ElementSize != ElementSize2 ) ) { CB_Error(DimensionERR); return ; }	// Dimension error

	sizeB3 = sizeB + sizeB2 ;
	NewMatListAns( sizeA, sizeB3, base, ElementSize );
	if ( ErrorNo ) return ;
	reg3=CB_MatListAnsreg;
	i=base;
	for ( m=base ; m<sizeA +base ; m++ ) 
		for ( n=base ; n<sizeB +base ; n++ ) WriteMatrix( reg3, m, n, ReadMatrix( reg , m, n ) );
	for ( m=base ; m<sizeA+base ; m++ ) 
		for ( n=base ; n<sizeB2 +base ; n++ ) WriteMatrix( reg3, m, n+sizeB, ReadMatrix( reg2, m, n) );
	
	if ( SRC[ExecPtr] == ')' ) ExecPtr++;
}
void CB_ArgumentList( char *SRC, int reg ) {	// Argument( List1, List2 )	
	int reg2,reg3;
	int m,n,i;
	int sizeA,sizeB,sizeA2,sizeB2,sizeA3;
	int ElementSize,ElementSize2;
	int base,base2;

	if ( SRC[ExecPtr] != ',' ) { CB_Error(SyntaxERR); return; }  // Syntax error
	ExecPtr++;
	
	ListEvalsubTop(SRC);
	if ( dspflag != 4 ) { CB_Error(ArgumentERR); return ; } // Argument error
	reg2 = CB_MatListAnsreg;

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

	if ( ( sizeB != sizeB2 ) || ( base != base2 ) || ( ElementSize != ElementSize2 ) ) { CB_Error(DimensionERR); return ; }	// Dimension error

	sizeA3 = sizeA + sizeA2 ;
	NewMatListAns( sizeA3, sizeB, base, ElementSize );
	if ( ErrorNo ) return ;
	reg3=CB_MatListAnsreg;
	i=base;
	for ( m=base ; m<sizeA +base ; m++ ) WriteMatrix( reg3, i++, base, ReadMatrix( reg , m, base ) );
	for ( m=base ; m<sizeA2+base ; m++ ) WriteMatrix( reg3, i++, base, ReadMatrix( reg2, m, base ) );
	
	if ( SRC[ExecPtr] == ')' ) ExecPtr++;
}

void CB_Argument( char *SRC ) {	// Argument( List1, List2 )		Argument( Mat A, Mat B)
	int reg,reg2;
	int m,n;
	int sizeA,sizeB,sizeA2,sizeB2;
	int ElementSize,ElementSize2;
	int base,base2;
	int c;
	ListEvalsubTop(SRC);
	reg = CB_MatListAnsreg;
	if ( dspflag == 3 )  { CB_ArgumentMat(SRC, reg); return; }
	if ( dspflag == 4 )  { CB_ArgumentList(SRC, reg); return; }
	 { CB_Error(ArgumentERR); return ; } // Argument error
}

int qsortA_dbl( const void *p1, const void *p2 ){
	return *(double*)p1-*(double*)p2;
}
int qsortB_dbl( const void *p1, const void *p2 ){
	return *(double*)p2-*(double*)p1;
}
int qsortA_long( const void *p1, const void *p2 ){
	return *(int*)p1-*(int*)p2;
}
int qsortB_long( const void *p1, const void *p2 ){
	return *(int*)p2-*(int*)p1;
}
int qsortA_word( const void *p1, const void *p2 ){
	return *(short*)p1-*(short*)p2;
}
int qsortB_word( const void *p1, const void *p2 ){
	return *(short*)p2-*(short*)p1;
}
int qsortA_byte( const void *p1, const void *p2 ){
	return *(char*)p1-*(char*)p2;
}
int qsortB_byte( const void *p1, const void *p2 ){
	return *(char*)p2-*(char*)p1;
}
void qsortSub( double *dptr, int  ElementSize, int sizeA, int sizeB, int flagAD ) {	//
	if ( flagAD ) {	// sortA
		switch ( ElementSize ) {
			case 8:
				qsort( (char*)dptr,   sizeA*sizeB, sizeof(char),   qsortA_byte );
				break;
			case 16:
				qsort( (short*)dptr,  sizeA*sizeB, sizeof(short),  qsortA_word );
				break;
			case 32:
				qsort( (int*)dptr,    sizeA*sizeB, sizeof(int),    qsortA_long );
				break;
			case 64:
				qsort( (double*)dptr, sizeA*sizeB, sizeof(double), qsortA_dbl );
				break;
			default:
				{ CB_Error(DimensionERR); return ; }	// Dimension error
		}
	} else {	// sortD
		switch ( ElementSize ) {
			case 8:
				qsort( (char*)dptr,   sizeA*sizeB, sizeof(char),   qsortB_byte );
				break;
			case 16:
				qsort( (short*)dptr,  sizeA*sizeB, sizeof(short),  qsortB_word );
				break;
			case 32:
				qsort( (int*)dptr,    sizeA*sizeB, sizeof(int),    qsortB_long );
				break;
			case 64:
				qsort( (double*)dptr, sizeA*sizeB, sizeof(double), qsortB_dbl );
				break;
			default:
				{ CB_Error(DimensionERR); return ; }	// Dimension error
		}
	}
}

void CB_SortAD( char *SRC, int flagAD) {	// SortA( List 1 ) or 	// SortD( List 1 )
	int c;
	int reg;
	int m,n;
	int sizeA,sizeB;
	int ElementSize;
	int base;
	double	*dptr;

  loop:
	c=SRC[ExecPtr];;
	if ( ( c != 0x7F ) || ( SRC[ExecPtr+1]!=0x51 ) ) { CB_Error(SyntaxERR); return; }	// Syntax error
	ExecPtr+=2;
	reg=ListRegVar( SRC );
	if ( reg>=0 ) {
		if ( MatAry[reg].SizeA == 0 ) { CB_Error(DimensionERR); return ; }	// Dimension error
	} else { CB_Error(SyntaxERR); return; }	// Syntax error

	sizeA        = MatAry[reg ].SizeA;
	sizeB        = MatAry[reg ].SizeB;
	base         = MatAry[reg ].Base;
	ElementSize  = MatAry[reg ].ElementSize;
	if ( ElementSize  == 2 ) ElementSize  == 1;
	dptr         = MatAry[reg ].Adrs;

	qsortSub( dptr, ElementSize, sizeA, sizeB, flagAD );
	
	if ( SRC[ExecPtr] == ',' ) { ExecPtr++; goto loop; }
	if ( SRC[ExecPtr] == ')' ) ExecPtr++;
}

int CB_EvalSortAD( char *SRC, int flagAD) {	// SortA( List 1 ) or 	// SortD( List 1 )  for Eval
	int c;
	int reg;
	int m,n;
	int sizeA,sizeB;
	int ElementSize;
	int base;
	double	*dptr;

	ListEvalsub1(SRC);
	if ( dspflag < 3 ) { CB_Error(ArgumentERR); return 0; } // Argument error
	reg=CB_MatListAnsreg;
	if ( reg>=0 ) {
		if ( MatAry[reg].SizeA == 0 ) { CB_Error(DimensionERR); return 0; }	// Dimension error
	} else { CB_Error(SyntaxERR); return; }	// Syntax error

	sizeA        = MatAry[reg ].SizeA;
	sizeB        = MatAry[reg ].SizeB;
	base         = MatAry[reg ].Base;
	ElementSize  = MatAry[reg ].ElementSize;
	if ( ElementSize  == 2 ) ElementSize  == 1;
	dptr         = MatAry[reg ].Adrs;

	qsortSub( dptr, ElementSize, sizeA, sizeB, flagAD );
	
	if ( SRC[ExecPtr] == ')' ) ExecPtr++;
	return 1;
}

double CB_MinMax( char *SRC, int flag) {	// Min( List 1 )	flag  0:min  1:max
	int reg,reg2,reg3;
	int m,n;
	int sizeA,sizeB;
	int ElementSize;
	int base;
	double min,tmp;
	int dspflagtmp=dspflag;
	
	ListEvalsub1(SRC);
	if ( dspflag < 3 ) { CB_Error(ArgumentERR); return ; } // Argument error
	reg=CB_MatListAnsreg;
	sizeA        = MatAry[reg ].SizeA;
	sizeB        = MatAry[reg ].SizeB;
	base         = MatAry[reg ].Base;
	
	if ( SRC[ExecPtr] == ',' ) { 
		ExecPtr++;
		ListEvalsub1(SRC);
		if ( dspflag < 3 ) { CB_Error(ArgumentERR); return ; } // Argument error
		reg2=CB_MatListAnsreg;
		if ( sizeA != MatAry[reg2].SizeA ) { CB_Error(DimensionERR); return 0 ; }	// Dimension error
		for ( m=base; m<sizeA+base; m++) {
			min=ReadMatrix( reg,  m, base ) ;
			tmp=ReadMatrix( reg2, m, base ) ;
			if ( flag )	{ 
				if ( min < tmp ) WriteMatrix( reg,m, base , tmp ) ;	// max
			} else {
				if ( min > tmp ) WriteMatrix( reg,m, base , tmp ) ;	// min
			}
		}
	} else {
		min = ReadMatrix( reg, base, base ) ;
		for ( m=base; m<sizeA+base; m++) {
			tmp=ReadMatrix( reg, m, base ) ;
			if ( flag )	{
				if ( min < tmp ) min=tmp;	// max
			} else {
				if ( min > tmp ) min=tmp;	// min
			}
		}
		dspflag=dspflagtmp; 
	}
	DeleteMatListAns();
	if ( SRC[ExecPtr] == ')' ) ExecPtr++;
	return min;
}
double CB_Mean( char *SRC ) {	// Mean( List 1 )
	int reg,reg2,reg3;
	int m,n;
	int sizeA,sizeB;
	int ElementSize;
	int base;
	double result;
	int dspflagtmp=dspflag;
	
	ListEvalsub1(SRC);
	if ( dspflag < 3 ) { CB_Error(ArgumentERR); return ; } // Argument error
	reg=CB_MatListAnsreg;
	sizeA        = MatAry[reg ].SizeA;
	sizeB        = MatAry[reg ].SizeB;
	base         = MatAry[reg ].Base;
	
	result=0;
	for ( m=base; m<sizeA+base; m++) {
		result+=ReadMatrix( reg, m, base ) ;
	}
	
	DeleteMatListAns();
	dspflag=dspflagtmp; 
	if ( SRC[ExecPtr] == ')' ) ExecPtr++;
	return result/sizeA;
}
double CB_Sum( char *SRC ) {	// Sum List 1 
	int reg;
	int m,n;
	int sizeA,sizeB;
	int ElementSize;
	int base;
	double result;
	int dspflagtmp=dspflag;
	
	ListEvalsub1(SRC);
	if ( dspflag < 3 ) { CB_Error(ArgumentERR); return ; } // Argument error
	reg=CB_MatListAnsreg;

	sizeA        = MatAry[reg ].SizeA;
	sizeB        = MatAry[reg ].SizeB;
	base         = MatAry[reg ].Base;

	result=0;
	for ( m=base; m<sizeA+base; m++) {
		result+=ReadMatrix( reg, m, base ) ;
	}
	
	DeleteMatListAns();
	dspflag=dspflagtmp; 
	return result;
}
double CB_Prod( char *SRC ) {	// Prod List 1 
	int reg;
	int m,n;
	int sizeA,sizeB;
	int ElementSize;
	int base;
	double result;
	int dspflagtmp=dspflag;
	
	ListEvalsub1(SRC);
	if ( dspflag < 3 ) { CB_Error(ArgumentERR); return ; } // Argument error
	reg=CB_MatListAnsreg;

	sizeA        = MatAry[reg ].SizeA;
	sizeB        = MatAry[reg ].SizeB;
	base         = MatAry[reg ].Base;

	result=1;
	for ( m=base; m<sizeA+base; m++) {
		result*=ReadMatrix( reg, m, base ) ;
	}
	
	DeleteMatListAns();
	dspflag=dspflagtmp; 
	return result;
}

//-----------------------------------------------------------------------------
int CB_MinMaxInt( char *SRC, int flag) {	// Min( List 1 )	flag  0:min  1:max
	int reg,reg2,reg3;
	int m,n;
	int sizeA,sizeB;
	int ElementSize;
	int base;
	int min,tmp;
	int dspflagtmp=dspflag;
	
	ListEvalIntsub1(SRC);
	if ( dspflag < 3 ) { CB_Error(ArgumentERR); return ; } // Argument error
	reg=CB_MatListAnsreg;
	sizeA        = MatAry[reg ].SizeA;
	sizeB        = MatAry[reg ].SizeB;
	base         = MatAry[reg ].Base;
	
	if ( SRC[ExecPtr] == ',' ) { 
		ExecPtr++;
		ListEvalIntsub1(SRC);
		if ( dspflag < 3 ) { CB_Error(ArgumentERR); return ; } // Argument error
		reg2=CB_MatListAnsreg;
		if ( sizeA != MatAry[reg2].SizeA ) { CB_Error(DimensionERR); return 0 ; }	// Dimension error
		for ( m=base; m<sizeA+base; m++) {
			min=ReadMatrixInt( reg,  m, base ) ;
			tmp=ReadMatrixInt( reg2, m, base ) ;
			if ( flag )	{ 
				if ( min < tmp ) WriteMatrixInt( reg,m, base , tmp ) ;	// max
			} else {
				if ( min > tmp ) WriteMatrixInt( reg,m, base , tmp ) ;	// min
			}
		}
	} else {
		min = ReadMatrixInt( reg, base, base ) ;
		for ( m=base; m<sizeA+base; m++) {
			tmp=ReadMatrixInt( reg, m, base ) ;
			if ( flag )	{
				if ( min < tmp ) min=tmp;	// max
			} else {
				if ( min > tmp ) min=tmp;	// min
			}
		}
		dspflag=dspflagtmp; 
	}
	DeleteMatListAns();
	if ( SRC[ExecPtr] == ')' ) ExecPtr++;
	return min;
}
int CB_MeanInt( char *SRC ) {	// Mean( List 1 )
	int reg,reg2,reg3;
	int m,n;
	int sizeA,sizeB;
	int ElementSize;
	int base;
	int result;
	int dspflagtmp=dspflag;
	
	ListEvalIntsub1(SRC);
	if ( dspflag < 3 ) { CB_Error(ArgumentERR); return ; } // Argument error
	reg=CB_MatListAnsreg;
	sizeA        = MatAry[reg ].SizeA;
	sizeB        = MatAry[reg ].SizeB;
	base         = MatAry[reg ].Base;
	
	result=0;
	for ( m=base; m<sizeA+base; m++) {
		result+=ReadMatrixInt( reg, m, base ) ;
	}
	
	DeleteMatListAns();
	dspflag=dspflagtmp; 
	if ( SRC[ExecPtr] == ')' ) ExecPtr++;
	return result/sizeA;
}
int CB_SumInt( char *SRC ) {	// Sum List 1 
	int reg;
	int m,n;
	int sizeA,sizeB;
	int ElementSize;
	int base;
	int result;
	int dspflagtmp=dspflag;
	
	ListEvalIntsub1(SRC);
	if ( dspflag < 3 ) { CB_Error(ArgumentERR); return ; } // Argument error
	reg=CB_MatListAnsreg;

	sizeA        = MatAry[reg ].SizeA;
	sizeB        = MatAry[reg ].SizeB;
	base         = MatAry[reg ].Base;

	result=0;
	for ( m=base; m<sizeA+base; m++) {
		result+=ReadMatrixInt( reg, m, base ) ;
	}
	
	DeleteMatListAns();
	dspflag=dspflagtmp; 
	return result;
}
int CB_ProdInt( char *SRC ) {	// Prod List 1 
	int reg;
	int m,n;
	int sizeA,sizeB;
	int ElementSize;
	int base;
	int result;
	int dspflagtmp=dspflag;
	
	ListEvalIntsub1(SRC);
	if ( dspflag < 3 ) { CB_Error(ArgumentERR); return ; } // Argument error
	reg=CB_MatListAnsreg;

	sizeA        = MatAry[reg ].SizeA;
	sizeB        = MatAry[reg ].SizeB;
	base         = MatAry[reg ].Base;

	result=1;
	for ( m=base; m<sizeA+base; m++) {
		result*=ReadMatrixInt( reg, m, base ) ;
	}
	
	DeleteMatListAns();
	dspflag=dspflagtmp; 
	return result;
}

//-----------------------------------------------------------------------------
void SeqOprand( char *SRC, int *fxreg, double *start, double *end, double *step ){	// Seq(X^2,X,1,10[,1])
	int exptr=ExecPtr;
	int errflag;
	double data;
  restart:
	data=CB_EvalDbl( SRC );	// dummy read
	if ( ErrorNo == 0 ) errflag=0;
	if ( dspflag >= 3 ) { CB_Error(ArgumentERR); return ; } // Argument error
	if ( errflag ) if ( ErrorNo ) return ;	// fatal error
	errflag=ErrorNo;	// error?
	if ( SRC[ExecPtr] != ',' ) { CB_Error(SyntaxERR); return ; }  // Syntax error
	ExecPtr++;
	*fxreg=RegVarAliasEx(SRC); if ( (*fxreg)<0 ) CB_Error(SyntaxERR) ; // Syntax error 
	if ( SRC[ExecPtr] != ',' ) { CB_Error(SyntaxERR); return ; }  // Syntax error
	ExecPtr++;
	*start=CB_EvalDbl( SRC );	// start
	if ( SRC[ExecPtr] != ',' ) { CB_Error(SyntaxERR); return ; }  // Syntax error
	ExecPtr++;
	*end=CB_EvalDbl( SRC );	// end
	if ( SRC[ExecPtr] == ',' ) {
		ExecPtr++;
		*step=CB_EvalDbl( SRC );	// step
	} else *step=1;
	
	if ( (*start)>(*end) ) { data=*start; *start=*end; *end=data; }
	if ( SRC[ExecPtr] == ')' ) ExecPtr++;
	if ( errflag ) {
		ExecPtr=exptr;
		LocalDbl[*fxreg][0]=*start;
		ErrorPtr= 0;
		ErrorNo = 0;	// error cancel
		goto restart;
	}
	
}

void CB_Seq( char *SRC ) { //	Seq(X^2,X,1,10,1)->List 1[.B][.W][.L][.F]
	int c,d;
	int dimA,dimB,i;
	int fxreg,reg;
	int exptr,exptr2;
	double data,databack;
	double start,end,step;
	int dataint,databackint;
	int startint,endint,stepint;
	int m,n;
	int base;
	int ElementSize;
	
	exptr=ExecPtr;
	SeqOprand( SRC, &fxreg, &start, &end, &step );

	dimA = (end-start)/step +1;
	dimB = 1;

	ElementSize=ElementSizeSelect( SRC, &base, 0) & 0xFF;
	NewMatListAns( dimA, dimB, base, ElementSize );
	if ( ErrorNo ) return ;
	reg=CB_MatListAnsreg;

	exptr2=ExecPtr;
	base=MatAry[reg].Base;
	m=base; n=base;
	databack=LocalDbl[fxreg][0];
	LocalDbl[fxreg][0]=start;
	while ( m < dimA+base ) {
		ExecPtr=exptr;
		data=CB_EvalDbl( SRC );	//
		WriteMatrix( reg, m, n, data);
		LocalDbl[fxreg][0]+=step;
		m++;
	}
	LocalDbl[fxreg][0]=databack;
	ExecPtr=exptr2;
	dspflag =4 ;	// List data
}
void SeqOprandInt( char *SRC, int *fxreg, int *start, int *end, int *step ){	// Seq(X^2,X,1,10[,1])
	int exptr=ExecPtr;
	int errflag;
	int data;
  restart:
	data=CB_EvalInt( SRC );	// dummy read
	if ( ErrorNo == 0 ) errflag=0;
	if ( dspflag >= 3 ) { CB_Error(ArgumentERR); return ; } // Argument error
	if ( errflag ) if ( ErrorNo ) return ;	// fatal error
	errflag=ErrorNo;	// error?
	if ( SRC[ExecPtr] != ',' ) { CB_Error(SyntaxERR); return; }  // Syntax error
	ExecPtr++;
	*fxreg=RegVarAliasEx(SRC); if ( (*fxreg)<0 ) CB_Error(SyntaxERR) ; // Syntax error 
	if ( SRC[ExecPtr] != ',' ) { CB_Error(SyntaxERR); return; }  // Syntax error
	ExecPtr++;
	*start=CB_EvalInt( SRC );	// start
	if ( SRC[ExecPtr] != ',' ) { CB_Error(SyntaxERR); return; }  // Syntax error
	ExecPtr++;
	*end=CB_EvalInt( SRC );	// end
	if ( SRC[ExecPtr] == ',' ) {
		ExecPtr++;
		*step=CB_EvalInt( SRC );	// step
	} else *step=1;
	
	if ( (*start)>(*end) ) { data=*start; *start=*end; *end=data; }
	if ( SRC[ExecPtr] == ')' ) ExecPtr++;
	if ( errflag ) {
		ExecPtr=exptr;
		LocalInt[*fxreg][0]=*start;
		ErrorPtr= 0;
		ErrorNo = 0;	// error cancel
		goto restart;
	}
}

void CB_SeqInt( char *SRC ) { //	Seq(X^2,X,1,10,1)->List 1[.B][.W][.L][.F]
	int c,d;
	int dimA,dimB,i;
	int fxreg,reg;
	int exptr,exptr2;
	int data,databack;
	int start,end,step;
	int dataint,databackint;
	int startint,endint,stepint;
	int m,n;
	int base;
	int ElementSize;
	
	exptr=ExecPtr;
	SeqOprandInt( SRC, &fxreg, &start, &end, &step );

	dimA = (end-start)/step +1;
	dimB = 1;

	ElementSize=ElementSizeSelect( SRC, &base, 0) & 0xFF;
	NewMatListAns( dimA, dimB, base, ElementSize );
	if ( ErrorNo ) return ;
	reg=CB_MatListAnsreg;

	exptr2=ExecPtr;
	base=MatAry[reg].Base;
	m=base; n=base;
	databack=LocalInt[fxreg][0];
	LocalInt[fxreg][0]=start;
	while ( m < dimA+base ) {
		ExecPtr=exptr;
		data=CB_EvalInt( SRC );	//
		WriteMatrixInt( reg, m, n, data);
		LocalInt[fxreg][0]+=step;
		m++;
	}
	LocalInt[fxreg][0]=databack;
	ExecPtr=exptr2;
	dspflag =4 ;	// List data
}


double CB_Sigma( char *SRC ) { //	Sigma(X^2,X,1,10[,1])
	int c,d;
	int i;
	int fxreg,reg;
	int exptr,exptr2;
	double data;
	double start,end,step;
	double result;
	int errflag=0;
	
	exptr=ExecPtr;
  restart:
	SeqOprand( SRC, &fxreg, &start, &end, &step );

	exptr2=ExecPtr;
	LocalDbl[fxreg][0]=start;
	result=0;
	while ( LocalDbl[fxreg][0] <= end ) {
		ExecPtr=exptr;
		result+=CB_EvalDbl( SRC );	//
		LocalDbl[fxreg][0]+=step;
	}
	LocalDbl[fxreg][0]=end;
	ExecPtr=exptr2;
	return result;
}

int CB_SigmaInt( char *SRC ) { //	Sigma(X^2,X,1,10[,1])
	int c,d;
	int i;
	int fxreg,reg;
	int exptr,exptr2;
	int data;
	int start,end,step;
	int result;
	int errflag=0;
	
	exptr=ExecPtr;
  restart:
	SeqOprandInt( SRC, &fxreg, &start, &end, &step );

	if ( errflag ) {
		ExecPtr=exptr;
		LocalInt[fxreg][0]=start;
		ErrorPtr= 0;
		ErrorNo = 0;	// error cancel
		goto restart;
	}

	exptr2=ExecPtr;
	LocalInt[fxreg][0]=start;
	result=0;
	while ( LocalInt[fxreg][0] <= end ) {
		ExecPtr=exptr;
		result+=CB_EvalInt( SRC );	//
		LocalInt[fxreg][0]+=step;
	}
	LocalInt[fxreg][0]=end;
	ExecPtr=exptr2;
	return result;
}


//-----------------------------------------------------------------------------
int CB_ListCmp( char *SRC ) { //	ListCmp( List1, n) or ListCmp( List,List2)
	double value;
	int valueint;
	int reg,reg2;
	int m,n,i;
	int sizeA,sizeB,sizeA2,sizeB2,sizeA3;
	int ElementSize,ElementSize2;
	int base,base2;
	int listflag=0;
	int dspflagtmp=dspflag;

	if ( CB_MatListAnsreg >=28 ) CB_MatListAnsreg=28;
	ListEvalsubTop(SRC);
	if ( dspflag != 4 ) { CB_Error(ArgumentERR); return 0; } // Argument error
	reg  = CB_MatListAnsreg;
	sizeA        = MatAry[reg ].SizeA;
	sizeB        = MatAry[reg ].SizeB;
	base         = MatAry[reg ].Base;
	ElementSize  = MatAry[reg ].ElementSize;
	if ( ElementSize  == 2 ) ElementSize  == 1;

	if ( SRC[ExecPtr] != ',' ) { CB_Error(SyntaxERR); return 0; }  // Syntax error
	ExecPtr++;
	value=ListEvalsubTop(SRC);
	valueint=value;
	reg2 = CB_MatListAnsreg;
	if ( dspflag == 3 ) { CB_Error(ArgumentERR); return 0; } // Argument error
	if ( dspflag == 4 ) { 
		listflag=1;
		sizeA2       = MatAry[reg2].SizeA;
		sizeB2       = MatAry[reg2].SizeB;
		base2        = MatAry[reg2].Base;
		ElementSize2 = MatAry[reg2].ElementSize;
		if ( ElementSize2 == 2 ) ElementSize2 == 1;
	}
	if ( SRC[ExecPtr] == ')' ) ExecPtr++;
	dspflag=dspflagtmp; 

	if ( listflag==0 ) {
		if ( ElementSize == 64 ) {
			for ( m=base; m<sizeA+base; m++ ) {
				if ( ReadMatrix( reg, base, m ) == value ) return 1;
			}
			return 0;
		} else {
			for ( m=base; m<sizeA+base; m++ ) {
				if ( ReadMatrixInt( reg, base, m ) == valueint ) return 1;
			}
			return 0;
		}
	} else {
		if ( ( sizeA != sizeA2 ) ) { return 0 ; }	// 
		if ( ( ElementSize != 64 ) && ( ElementSize != 64 ) ) {
			for ( m=0; m<sizeA; m++ ) {
				if ( ReadMatrixInt( reg,  base, m+base ) != ReadMatrixInt( reg2,  base2, m+base2 ) ) return 0;
			}
			return 1;
		} else {
			for ( m=0; m<sizeA; m++ ) {
				if ( ReadMatrix( reg,  base, m+base ) != ReadMatrix( reg2,  base2, m+base2 ) ) return 0;
			}
			return 1;
		}
	}
	return 0;
}

//-----------------------------------------------------------------------------
int CB_Dim( char *SRC ){	// Dim Mat or Dim List
	int reg;
	if ( SRC[ExecPtr]==0x7F ) {
		if ( SRC[ExecPtr+1]==0x40 ) {	// Dim Mat
			MatrixOprandreg( SRC, &reg );
			WriteListAns2( MatAry[reg].SizeA, MatAry[reg].SizeB );
			return MatAry[reg].SizeA;
		} else
		if ( SRC[ExecPtr+1]==0x51 ) {	// Dim List
			MatrixOprandreg( SRC, &reg );
			return MatAry[reg].SizeA;
		}
	} 
	ExecPtr--;	// error
	return -1;
}
int CB_ElemSize( char *SRC ){	// ElemSize( Mat A )
	int reg;
	int i;
	MatrixOprandreg( SRC, &reg );
	if ( SRC[ExecPtr] == ')' ) ExecPtr++;
	i=MatAry[reg].ElementSize;
	if (i <= 4 ) i=1;
	return i;
}
int CB_RowSize( char *SRC ){	// RowSize( Mat A )
	int reg;
	MatrixOprandreg( SRC, &reg );
	if ( SRC[ExecPtr] == ')' ) ExecPtr++;
	return MatAry[reg].SizeA;
}
int CB_ColSize( char *SRC ){	// ColSize( Mat A )
	int reg;
	MatrixOprandreg( SRC, &reg );
	if ( SRC[ExecPtr] == ')' ) ExecPtr++;
	return MatAry[reg].SizeB;
}
int CB_MatBase( char *SRC ){	// MatBase( Mat A )
	int reg;
	MatrixOprandreg( SRC, &reg );
	if ( SRC[ExecPtr] == ')' ) ExecPtr++;
	return MatAry[reg].Base;
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
int MatrixObjectAlign4M1( unsigned int n ){ return n; }	// align +4byte
int MatrixObjectAlign4M2( unsigned int n ){ return n; }	// align +4byte
int MatrixObjectAlign4M3( unsigned int n ){ return n; }	// align +4byte
//int MatrixObjectAlign4M4( unsigned int n ){ return n; }	// align +4byte
//int MatrixObjectAlign4M5( unsigned int n ){ return n; }	// align +4byte
//int MatrixObjectAlign4M6( unsigned int n ){ return n; }	// align +4byte
//int MatrixObjectAlign4M7( unsigned int n ){ return n; }	// align +4byte
//int MatrixObjectAlign4M8( unsigned int n ){ return n; }	// align +4byte
//int MatrixObjectAlign4M9( unsigned int n ){ return n; }	// align +4byte
//-----------------------------------------------------------------------------

