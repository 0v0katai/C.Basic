#include "CB.h"

//-----------------------------------------------------------------------------
// Matrix 
//-----------------------------------------------------------------------------
//matary MatAryR[MATARY_MAX];
int MatAryMax=MATARY_MAX;
matary *MatAry;
short ExtListMax=0;		// Extend List Max number
short Mattmpreg;		//
complex MatDefaultValue={0,0};
char	MatBaseDefault=1;
char	MatBase=1;
char	ListFileNo=0;	// 0:normal	 1~:file mode
short	ListFilePtr=0;

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
	complex*	MatAryCPLX;
	int		dimB=MatAry[reg].SizeB;
	int		base=MatAry[reg].Base;
	m-=base;
	n-=base;
	switch ( MatAry[reg].ElementSize ) {
		case  2:	// Vram
		case  1:
			MatAryC=(char*)MatAry[reg].Adrs;
			return  (char *)(MatAryC+((MatAry[reg].SizeA-1)>>3+1)*n+(m>>3));
		case  4:
			MatAryC=(char*)MatAry[reg].Adrs;
			return  (char *)(MatAryC+((MatAry[reg].SizeA-1)>>1+1)*n+(m>>1));
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
		case 128:
			MatAryCPLX=(complex*)MatAry[reg].Adrs;			// Matrix array 128 bit
			return  (char *)(MatAryCPLX+dimB*m+n);
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
		case  4:
			sizeA=((sizeA-1)>>1)+1;
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
int DimMatrixSubNoinit( int reg, int ElementSize, int m, int n, int base, int adrs ) {	// 1-
	char	*cptr;
	short	*wptr;
	int		*iptr;
	double	*dptr;
	int i;
	int matsize,mats;
	if ( ( ElementSize!= 1 )&&( ElementSize!= 2 )&&( ElementSize!= 3 )&&( ElementSize!= 4 )&&( ElementSize!= 5 )&&( ElementSize!= 8 )&&( ElementSize!= 16 )&&( ElementSize!= 32 )&&( ElementSize!= 64 )&&( ElementSize!= 128 ) ) { CB_Error(ElementSizeERR); return 0; }	// Illegal Element size
	if ( ( 2==ElementSize )||( ElementSize==3 )||( ElementSize==5 ) ) { 		// 1 bit Vram array
		if ( ( m != 128 ) || ( n != 64 ) ) { CB_Error(ArraySizeERR); return 0; }	// Illegal Ary size
		switch ( ElementSize ) {
			case 2:
				MatAry[reg].Adrs        = (double*)(PictAry[0]);	// Matrix array ptr*  VRAM
				break;
			case 3:
				MatAry[reg].Adrs        = (double*)(TVRAM);			// Matrix array ptr*  Text VRAM
				break;
			case 5:
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
	if ( ( m<1 ) || ( n<1 ) ) { CB_Error(ArgumentERR); return 0; }	// Argument error
	if ( ElementSize==4 ) {	// 4 bit matrix
			matsize=( ((m-1)>>1)+1 )*n;
	} else 
	if ( ElementSize>1 ) {
			matsize=m*n*(ElementSize>>3);
	} else {	// 1 bit matrix
			matsize=( ((m-1)>>3)+1 )*n;
	}
	mats = matsize;
	matsize = (matsize+7) & 0xFFFFFFF8;	// 8byte align
	if ( ( MatAry[reg].Adrs != NULL ) && ( abs(MatAry[reg].Maxbyte) >= matsize ) && ( MatAry[reg].ElementSize!=2 ) ) { // already exist
		if ( adrs )	goto newmat;
		MatAry[reg].SizeA       = m;						// Matrix array size
		MatAry[reg].SizeB       = n;						// Matrix array size
		MatAry[reg].ElementSize = ElementSize;				// Matrix array Elementsize
		MatAry[reg].Base        = base;						// Matrix array base
	} else {
	  newmat:
		if ( ( MatAry[reg].Adrs != NULL ) && ( MatAry[reg].ElementSize != 2 ) ) HiddenRAM_freeMat( reg );	// free
		if ( adrs ) {
			dptr = (double *)adrs ;
			matsize = -matsize;	// Adrs redefinition flag
			if ( CheckAdrsAlignError( ElementSize, (int)adrs ) ) { CB_Error(AlignmentERR); return 0; } // Address Alignment error
		} else {
			dptr = (double*)HiddenRAM_mallocMat( matsize );
			mats = matsize;
			if( dptr == NULL ) { CB_Error(NotEnoughMemoryERR); return 0; }	// Not enough memory error
		}
		MatAry[reg].SizeA       = m;						// Matrix array size
		MatAry[reg].SizeB       = n;						// Matrix array size
		MatAry[reg].ElementSize = ElementSize;				// Matrix array Elementsize
		MatAry[reg].Adrs        = dptr ;					// Matrix array ptr*
		MatAry[reg].Maxbyte     = matsize;					// Matrix array max byte
		MatAry[reg].Base        = base;						// Matrix array base
	}
	return mats;	// ok
}

int DimMatrixSub( int reg, int ElementSize, int m, int n , int base ) {	// 1-
	int mats = DimMatrixSubNoinit( reg, ElementSize, m, n , base, 0 );
	if ( mats ) memset( (char*)MatAry[reg].Adrs, 0, mats  );	// initialize
	return mats;
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

	if ( ( ElementSize!= 1 )&&( ElementSize!= 4 )&&( ElementSize!= 8 )&&( ElementSize!= 16 )&&( ElementSize!= 32 )&&( ElementSize!= 64 )&&( ElementSize!=128 ) ) { CB_Error(ElementSizeERR); return; }	// Illegal Element size
	if ( ElementSize==4 ) {	// 4 bit matrix
			matsize=( ((m-1)>>1)+1 )*n;
	} else 
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

int DefaultElemetSize(){
	switch (CB_INT){
		case 1:
			return 32;
		case 0:
			return 64;
	}
	return 128;
}
int DimMatrix( int reg, int dimA, int dimB, int base ) {
	char	*cptr;
	short	*wptr;
	int		*iptr;
	double	*dptr;
	int i;
	int ElementSize;
	
	ElementSize = DefaultElemetSize() ;
	return	DimMatrixSub( reg, ElementSize, dimA, dimB, base );
}

//-----------------------------------------------------------------------------
void DeleteMatrix1( int reg ){
	int i;
	MatAry[reg].SizeA       = 0;				// Matrix array size
	MatAry[reg].SizeB       = 0;				// Matrix array size
	MatAry[reg].ElementSize = 0;				// Matrix array Elementsize
	MatAry[reg].Adrs        = NULL ;			// Matrix array ptr*
	MatAry[reg].Maxbyte     = 0;				// Matrix array max byte
	memset( MatAry[reg].name, 0, 9 );
}

void DeleteMatrix( int reg ) {
	double *ptr;
	if ( ( 0<=reg ) && ( reg<MatAryMax ) ) {
			ptr = MatAry[reg].Adrs ;					// Matrix array ptr*
			if ( (ptr != NULL ) && ( MatAry[reg].ElementSize != 2 ) ) HiddenRAM_freeMat( reg );
			DeleteMatrix1( reg );
	} else {
		for ( reg=0; reg<MatAryMax; reg++){
			ptr = MatAry[reg].Adrs ;					// Matrix array ptr*
			if ( (ptr != NULL ) && ( MatAry[reg].ElementSize != 2 ) ) HiddenRAM_freeMat( reg );
			DeleteMatrix1( reg );
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
			case 4:
				Print((unsigned char*)"[nibl]");
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
			case 128:
				Print((unsigned char*)"[CPLX]");
				break;
		}
}

unsigned int SetDimension(int reg, int *dimA, int *dimB, int *Elsize, int *base, int list ){	// 1-
	char buffer[22];
	unsigned int key;
	int	cont=1;
	int select=0;
	int y,e;

	*base=MatBase;

	PopUpWin(5);
	FkeyClearAll();
	
	*dimA=MatAry[reg].SizeA;	//
	*dimB=MatAry[reg].SizeB;	//
	if ( *dimA==0 ) *dimA=1;
	if ( *dimB==0 ) *dimB=1;

	while (cont) {
		locate( 3,2);
		if ( MatXYmode ) Print((unsigned char *)"Dimension X\xA9Y"); else Print((unsigned char *)"Dimension m\xA9n");
		locate( 3,3); 
		if ( MatXYmode ) Print((unsigned char *) "  X  :           "); else Print((unsigned char *) "  m  :           ");
		sprintG(buffer,*dimA,  10,LEFT_ALIGN); locate( 9, 3); Print((unsigned char*)buffer);
		if ( list!=1 ) {
			locate( 3,4); 
			if ( MatXYmode ) Print((unsigned char *) "  Y  :           "); else Print((unsigned char *) "  n  :           ");
			sprintG(buffer,*dimB,  10,LEFT_ALIGN); locate( 9, 4); Print((unsigned char*)buffer);
		}
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
				if ( ( list==1 )&&( select==1 ) ) select=0;
				break;
			case KEY_CTRL_DOWN:
				select+=1;
				if ( select > 3 ) select=0;
				if ( ( list==1 )&&( select==1 ) ) select=2;
				break;

			case KEY_CTRL_RIGHT:
				Bdisp_AreaReverseVRAM(12, y*8, 113, y*8+7);	// reverse select line 
				Bdisp_PutDisp_DD();
				y++;
				switch (select) {
					case 0: // dim m
					  INPdimA:
						e = *dimA;
						do {
							*dimA = e;
							*dimA  =InputNumD_full( 9, y, 10, *dimA);	// 
						} while ( *dimA<1 ) ;
						select++;	if ( list==1 ) select++;
						if ( list==2 ) *dimB=1;	// vct
						break;
					case 1: // dim n
					  INPdimB:
						e = *dimB;
						do {
							*dimB = e;
							*dimB  =InputNumD_full( 9, y, 10, *dimB);	// 
						} while ( *dimB<1 ) ;
						if ( list==2 ) *dimA=1;	// vct
						break;
					case 2: // size
						if ( (*Elsize)==  1 ) { (*Elsize)=  2; break; }
						if ( (*Elsize)==  2 ) { (*Elsize)=  4; break; }
						if ( (*Elsize)==  4 ) { (*Elsize)=  8; break; }
						if ( (*Elsize)==  8 ) { (*Elsize)= 16; break; }
						if ( (*Elsize)== 16 ) { (*Elsize)= 32; break; }
						if ( (*Elsize)== 32 ) { (*Elsize)= 64; break; }
						if ( (*Elsize)== 64 ) { (*Elsize)=128; break; }
						if ( (*Elsize)==128 ) { (*Elsize)=  1; break; }
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
						goto INPdimA;
						e = *dimA;
						do {
							*dimA = e;
							*dimA  =InputNumD_full( 9, y, 10, *dimA);	// 
						} while ( *dimA<1 ) ;
						select++;	if ( list==1 ) select++;
						if ( list==2 ) *dimB=1;	// vct
						break;
					case 1: // dim n
//						goto INPdimB;
						e = *dimB;
						do {
							*dimB = e;
							*dimB  =InputNumD_full( 9, y, 10, *dimB);	// 
						} while ( *dimB<1 ) ;
						if ( list==2 ) *dimA=1;	// vct
						break;
					case 2: // size
						if ( (*Elsize)==  1 ) { (*Elsize)=128; break; }
						if ( (*Elsize)==  2 ) { (*Elsize)=  1; break; }
						if ( (*Elsize)==  4 ) { (*Elsize)=  2; break; }
						if ( (*Elsize)==  8 ) { (*Elsize)=  4; break; }
						if ( (*Elsize)== 16 ) { (*Elsize)=  8; break; }
						if ( (*Elsize)== 32 ) { (*Elsize)= 16; break; }
						if ( (*Elsize)== 64 ) { (*Elsize)= 32; break; }
						if ( (*Elsize)==128 ) { (*Elsize)= 64; break; }
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
						e = *dimA;
						do {
							*dimA = e;
							*dimA  =InputNumD_Char( 9, y, 10, *dimA, key);	// 
							key=0;
						} while ( *dimA<1 ) ;
						select++;	if ( list==1 ) select++;
						if ( list==2 ) *dimB=1;	// vct
						break;
					case 1: // dim n
						e = *dimB;
						do {
							*dimB = e;
							*dimB  =InputNumD_Char( 9, y, 10, *dimB, key);	// 
							key=0;
						} while ( *dimB<1 ) ;
						if ( list==2 ) *dimA=1;	// vct
						break;
					case 2: // size
						e = *Elsize;
						do	{
							*Elsize = e;
							*Elsize =InputNumD_Char( 9, y, 10, (*Elsize), key);	// 
						} while ( (*Elsize!=1)&&(*Elsize!=2)&&(*Elsize!=4)&&(*Elsize!=8)&&(*Elsize!=16)&&(*Elsize!=32)&&(*Elsize!=64) ) ;
						break;
					case 3: // base
						e = *base;
						do	{
							*base = e;
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
	
	if ( base ) { (*m)++; (*n)++; }

	PopUpWin(3);
	FkeyClear( FKeyNo1 );
	FkeyClear( FKeyNo2 );
	FkeyClear( FKeyNo3 );
	FkeyClear( FKeyNo4 );
	FkeyClear( FKeyNo5 );
	FkeyClear( FKeyNo6 );

	while (cont) {
		locate( 3,3); Print((unsigned char *)"Goto Element");
		if ( MatXYmode ) sprintf( (char*)buffer," X(%d~%3d)  ", base, dimA); else sprintf( (char*)buffer," m(%d~%3d)  ", base, dimA);
		locate( 3,4); Print((unsigned char*)buffer);
		locate(13,4); Print((unsigned char *)":      ");
		sprintG(buffer,*m,  5,LEFT_ALIGN); locate(14, 4); Print((unsigned char*)buffer);
		if ( MatXYmode ) sprintf( (char*)buffer," Y(%d~%3d)  ", base, dimB); else sprintf( (char*)buffer," n(%d~%3d)  ", base, dimB);
		locate( 3,5); Print((unsigned char*)buffer);
		
		locate(13,5); Print((unsigned char *)":      ");
		sprintG(buffer,*n,  5,LEFT_ALIGN); locate(14, 5); Print((unsigned char*)buffer);

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
						*m =InputNumD_full( 14, y, 5, *m);	// 
						if ( *m < base ) *m=base;
						if ( *m > dimA ) *m=dimA;
						select+=1;
						break;
					case 1: // dim n
						*n =InputNumD_full( 14, y, 5, *n);	// 
						if ( *n < base ) *n=base;
						if ( *n > dimB ) *n=dimB;
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
						*m =InputNumD_Char( 14, y, 5, *m, key);	// 
						if ( *m < base ) *m=base;
						if ( *m > dimA ) *m=dimA;
						select+=1;
						break;
					case 1: // dim n
						*n =InputNumD_Char( 14, y, 5, *n, key);	// 
						if ( *n < base ) *n=base;
						if ( *n > dimB ) *n=dimB;
						break;
					default:
						break;
				}
		}

	}
	if ( base ) { (*m)--; (*n)--; }
	return key;
}
//-----------------------------------------------------------------------------

void InitMatSub( int reg, complex value ){
	int i,j;
	int dimA,dimB;
	int base=MatAry[reg].Base;
	dimA=MatAry[reg].SizeA+base;
	dimB=MatAry[reg].SizeB+base;
	for (j=base; j<dimB; j++ ) {
		for (i=base; i<dimA; i++ ) {
			Cplx_WriteMatrix( reg, i, j, value);
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

complex InitMatrix( int reg, complex value ,int ElementSize ) {
	char buffer[64];
	unsigned int key;
	int	cont=1;
	int i,j;
	int dimA,dimB;

	if (MatAry[reg].SizeA==0) return Int2Cplx(0);

	PopUpWin(3);
	FkeyClear( FKeyNo6 );
	while (cont) {
		locate( 3,3); Print((unsigned char *)"Init Matrix array");
		locate( 3,5); Print((unsigned char *)"value:           ");
		Cplx_sprintGR1cutlim( buffer, value, 11,LEFT_ALIGN, Norm, 10 );
		locate( 9, 5); Print((unsigned char*)buffer);
		locate(1,8); PrintLine((unsigned char *)" ",21);
		locate(1,8); MatAryElementSizePrint( MatAry[reg].ElementSize ) ;
//		Bdisp_PutDisp_DD();

		GetKey( &key );
		switch (key) {
			case KEY_CTRL_EXIT:
				return Int2Cplx(0) ;
				break;
			case KEY_CTRL_EXE:
				cont=0;
				break;
			case KEY_CTRL_LEFT:
				PutKey( KEY_CTRL_DOWN, 1 );
			case KEY_CTRL_RIGHT:
				value  =InputNumC_full( 9, 5, 11, value);	// 
				break;
			default:
				break;
		}
		key=MathKey( key );
		if ( key ) {
			value  =InputNumC_Char( 9, 5, 11, value, key);	// 
		}
	}

	if ( YesNo("Initialize Ok?") ) if ( ElementSize >= 64 ) InitMatSub( reg, value ); else InitMatIntSub( reg, (int)value.real ); 

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

void MatNumToExpBuf( complex value, int bit ){	// value -> ExpBuffer
	int eng=ENG;
	ExpBuffer[0]='0';
	ExpBuffer[1]='x';
	if ( bit== 1 ) {	NumToBin(ExpBuffer+2, value.real, 8); 	ExpBuffer[1]='b';
	} else 
	if ( bit== 2 ) {	NumToBin(ExpBuffer+2, value.real, 16);	ExpBuffer[1]='b';
	} else 
	if ( bit== 8 ) {	NumToHex(ExpBuffer+2, value.real, 2);
	} else 
	if ( bit==16 ) {	NumToHex(ExpBuffer+2, value.real, 4);
	} else 
	if ( bit==32 ) {	NumToHex(ExpBuffer+2, value.real, 8);
	} else 
	if ( bit==64 ) {	DNumToHex(ExpBuffer+2, value.real, 16);
	} else { 
		if (ENG==3) ENG=0;
		Cplx_sprintGR1(ExpBuffer, value, 63, LEFT_ALIGN, CB_Round.MODE, CB_Round.DIGIT );
		ENG=eng;
	}
}

char* NewclipBuffer( int *size );
void AdjclipBuffer( int size );

void List2Clip( int reg, int bit ) {	// 
	int i,j,dimA,dimB,x,y;
	int base=MatAry[reg].Base;
	int ElementSize=MatAry[reg].ElementSize;
	int ptr=0;
	int max;
	char *buffer;
	if ( MatXYmode ) {
		dimB=MatAry[reg].SizeA-1+base;	//	X,Y
		dimA=MatAry[reg].SizeB-1+base;
	} else {
		dimA=MatAry[reg].SizeA-1+base;	//	m,n
		dimB=MatAry[reg].SizeB-1+base;
	}
	max = -1;
	buffer = NewclipBuffer( &max );	// max size

	buffer[ptr++]='{';
	y=base;
	for (x=base; x<=dimA; x++) {
		if ( MatXYmode ) MatNumToExpBuf( Cplx_ReadMatrix( reg, y, x), bit ) ;
		else			 MatNumToExpBuf( Cplx_ReadMatrix( reg, x, y), bit ) ;
		i=strlen(ExpBuffer);
		if ( ptr+i > max ) { 
			buffer[0]='\0';
			ErrorMSGstr1("CLip Buffer Over");
			return;
		}
		strcpy(buffer+ptr, ExpBuffer);
		ptr+=i;
		buffer[ptr++]=',';
	}
	ptr--;
	buffer[ptr++]='}';
	buffer[ptr++]='\0';
	AdjclipBuffer( ptr );	// adjust size
	ErrorMSGstr1("List to Clip Ok!");
}

void Mat2Clip( int reg, int bit ) {	// 
	int i,j,dimA,dimB,x,y;
	int base=MatAry[reg].Base;
	int ElementSize=MatAry[reg].ElementSize;
	int ptr=0;
	int max;
	char *buffer;
	if ( MatXYmode ) {
		dimB=MatAry[reg].SizeA-1+base;	//	X,Y
		dimA=MatAry[reg].SizeB-1+base;
	} else {
		dimA=MatAry[reg].SizeA-1+base;	//	m,n
		dimB=MatAry[reg].SizeB-1+base;
	}
	max = -1;
	buffer = NewclipBuffer( &max );	// max size

	buffer[ptr++]='[';
//	buffer[ptr++]=0x0D;
	for (x=base; x<=dimA; x++) {
		buffer[ptr++]='[';
		for (y=base; y<=dimB; y++) {
			if ( MatXYmode ) MatNumToExpBuf( Cplx_ReadMatrix( reg, y, x), bit ) ;
			else			 MatNumToExpBuf( Cplx_ReadMatrix( reg, x, y), bit ) ;
			i=strlen(ExpBuffer);
			if ( ptr+i > max ) { 
				buffer[0]='\0';
				ErrorMSGstr1("CLip Buffer Over");
				return;
			}
			strcpy(buffer+ptr, ExpBuffer);
			ptr+=i;
			buffer[ptr++]=',';
		}
		ptr--;
		buffer[ptr++]=']';
		buffer[ptr++]=0x0D;
	}
	ptr--;
	buffer[ptr++]=']';
	buffer[ptr++]='\0';
	AdjclipBuffer( ptr );	// adjust size
	ErrorMSGstr1("Mat to Clip Ok!");
}

int SkipSpcsub( char *buf, int *ptr ) {
	int c=buf[(*ptr)];
	while ( ( c==0x20 ) ) c=buf[++(*ptr)];	//  Skip SPACE
	return buf[(*ptr)];
}
int SkipSpcCRsub( char *buf, int *ptr ) {
	int c=buf[(*ptr)];
	while ( ( c==0x20 ) || ( c==0x0D ) ) c=buf[++(*ptr)];	//  Skip SPACE or [CR]
	return buf[(*ptr)];
}

void Clip2List( int reg ) { //	{1.2,3,4,5,6}->List
	int c,d;
	int dimA,dimB,i;
	int exptr,exptr2;
	complex data;
	int m,n;
	int base;
	int ElementSize;
	int ptr=0;
	int dimA2,dimB2;
	char *buffer=ClipBuffer;
	
	c=SkipSpcCRsub(buffer,&ptr);
	if ( c != '{' ) return ;
	ptr++;
	SkipSpcCRsub(buffer,&ptr);
	exptr=ptr;
	m=1;
	while ( 1 ) {
		data=Cplx_Eval2( buffer, &ptr ); if ( ErrorNo ) return;
		c=SkipSpcCRsub(buffer,&ptr);
		if ( c != ',' ) break;  // Syntax error
		ptr++;
		SkipSpcCRsub(buffer,&ptr);
		m++;
	}
	if ( c == '}' ) ExecPtr++;
	dimA=m;
	dimB=1;
	exptr2=ptr;
	ptr=exptr;
	base=MatAry[reg].Base;
	m=base; n=base;
	if ( MatXYmode ) {
		dimB2=MatAry[reg].SizeA-1+base;	//	X,Y
		dimA2=MatAry[reg].SizeB-1+base;
	} else {
		dimA2=MatAry[reg].SizeA-1+base;	//	m,n
		dimB2=MatAry[reg].SizeB-1+base;
	}

	n=base;
	while ( m < dimA+base ) {
		data=Cplx_Eval2( buffer, &ptr );
			if ( ( m<=dimA2 ) && ( n<=dimB2 ) ) {
				if ( MatXYmode ) {
					Cplx_WriteMatrix( reg, n, m, data);
				} else {
					Cplx_WriteMatrix( reg, m, n, data);
				}
			}
		SkipSpcCRsub(buffer,&ptr);
		ptr++;	// , skip
		SkipSpcCRsub(buffer,&ptr);
		m++;
	}
}

void Clip2Mat( int reg ) { //	[[1.2,3][4,5,6]]->Mat
	int c,d;
	int dimA,dimB,i;
	int exptr,exptr2;
	complex data;
	int m,n;
	int base;
	int ElementSize;
	int ptr=0;
	int dimA2,dimB2;
	char *buffer=ClipBuffer;
	
	c=SkipSpcCRsub(buffer,&ptr);
	if ( c == '{' ) { Clip2List( reg ); return ; }
	if ( c != '[' ) return ;
	ptr++;
	SkipSpcCRsub(buffer,&ptr);
	if ( c != '[' ) return ;
	ptr++;
	SkipSpcCRsub(buffer,&ptr);
	exptr=ptr;
	n=1;
	while ( 1 ) {
		data=Cplx_Eval2( buffer, &ptr ); if ( ErrorNo ) return;
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
				data=Cplx_Eval2( buffer, &ptr ); if ( ErrorNo ) return;
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
	if ( MatXYmode ) {
		dimB2=MatAry[reg].SizeA-1+base;	//	X,Y
		dimA2=MatAry[reg].SizeB-1+base;
	} else {
		dimA2=MatAry[reg].SizeA-1+base;	//	m,n
		dimB2=MatAry[reg].SizeB-1+base;
	}
	while ( m < dimA+base ) {
		n=base;
		while ( n < dimB+base ) {
			data=Cplx_Eval2( buffer, &ptr );
			if ( ( m<=dimA2 ) && ( n<=dimB2 ) ) {
				if ( MatXYmode ) {
					Cplx_WriteMatrix( reg, n, m, data);
				} else {
					Cplx_WriteMatrix( reg, m, n, data);
				}
			}
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

void EditMatrix(int reg, int ans ){		// ----------- Edit Matrix
	char buffer[128];
	char buffer2[64];
	char buffer3[64];
	unsigned int key;
	int	cont=1;
	int seltopY=0,seltopX=0;
	int i,j,dimA,dimB,x,y;
	int selectX=0, selectY=0;
	complex value;
	int ElementSize;
	int dx,dy,MaxX,MaxY,MaxDX,MaxDY,adjX=3;
	int bit=0;	// 0:normal  1:bin  16~64:hex
	int base;
	int strdisp=0;
	int dotedit=0;
	int list=0;
	if ( reg >= 0x1000 ) { 
		if ( reg >=0x2000 ) {
			list=2;
		} else {
			list=1;
		}
		reg&=0x0FFF;
	}
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
		if ( ElementSize == 4 ) {
			MaxDX=8; if (MaxDX>dimA) MaxDX=dimA;
			dx=13;
			MaxX=7;
			adjX=3;
			if ( bit==4 ) {	// 4bit
				MaxDX=5; if (MaxDX>dimA) MaxDX=dimA;
				dx=22;
				MaxX=4;
				adjX=4;
			}
			if ( bit==8 ) {	// 2hex
				MaxDX=8; if (MaxDX>dimA) MaxDX=dimA;
				dx=13;
				MaxX=7;
				adjX=3;
			}
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
		} else
		if ( ElementSize == 128 ) {
			MaxDX=3; if (MaxDX>dimA) MaxDX=dimA;
			dx=36;
			MaxX=2;
			adjX=3;
			if ( bit==64 ) {	// 16hex
				MaxDX=0;
				dx=69;
				MaxX=0;
				adjX=0;
			}
		}

		if (  selectY<seltopY ) seltopY = selectY;
		if ( (selectY-seltopY) > MaxY ) seltopY = selectY-MaxY;
		if ( (dimB -seltopY) < MaxY ) seltopY = dimB -MaxY; 
		if ( seltopY < 0 ) seltopY=0;
	  dspjp:
		if (  selectX<seltopX ) seltopX = selectX;
		if ( (selectX-seltopX) > MaxX ) seltopX = selectX-MaxX;
		if ( (dimA -seltopX) < MaxX ) seltopX = dimA -MaxX; 
		if ( seltopX < 0 ) seltopX=0;

		Bdisp_AllClr_VRAM();
		j=0;
		if ( reg<110 ) {
			switch( list ) {
				case 0:	// Mat
					j=SetVarCharMat( buffer, reg); buffer[j]='\0';
					break;
				case 2:	// Vct
					j=SetVarCharVct( buffer, reg); buffer[j]='\0';
					break;
				case 1:	// List
					if ( ( 26<=reg ) && ( reg<=28 ) ) {
						j=SetVarChar( buffer, reg) ; buffer[j]='\0';
					} else
					if ( 32<=reg ) {
						if ( reg>=58 ) i=reg-57;
						else
						if ( reg<=57 ) i=reg-5;
						sprintf(buffer,"%d",i);
					}
					break;
			}
		} else {
			i=reg-57;
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
				if ( ( x >= 8 ) ) break;
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
					if ( MatXYmode ) value=Cplx_ReadMatrix( reg, seltopX+x+base, seltopY+y+base);
					else			 value=Cplx_ReadMatrix( reg, seltopY+y+base, seltopX+x+base);
					if ( value.real )	ML_point( x*dx+21, y*dy+10, 3, ML_BLACK );
					else				ML_point( x*dx+21, y*dy+10, 1, ML_BLACK );
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
				if ( ( strdisp ) && (ElementSize > 4 ) )  {
					if ( MatXYmode==0 ) OpcodeStringToAsciiString( buffer, MatrixPtr(reg, seltopY+y+base, seltopX  +base), 64-1 );
					else				OpcodeStringToAsciiString( buffer, MatrixPtr(reg, seltopX  +base, seltopY+y+base), 64-1 );
//					PrintMini( 20,y*8+10, (unsigned char*)buffer,MINI_OVER );	// string disp
					CB_PrintMini( 20,y*8+10, (unsigned char*)buffer , MINI_OVER | 0x100 );
				} else {
					for ( x=0; x<=MaxDX; x++ ) {
						if ( ( x >= 8 ) ) break;
						if ( ((seltopY+y)<=dimB) && ((seltopX+x)<=dimA) ) {
							if ( MatXYmode ) value=Cplx_ReadMatrix( reg, seltopX+x+base, seltopY+y+base);
							else			 value=Cplx_ReadMatrix( reg, seltopY+y+base, seltopX+x+base);
							if ( ( bit==0 ) && ( MaxX==7 ) ) {	sprintG(buffer, value.real, 2,RIGHT_ALIGN);
										PrintMini(x*dx+20,y*8+10,(unsigned char*)buffer,MINI_OVER);
							} else 
							if ( ( bit==0 ) && ( MaxX==3 ) ) {	sprintG(buffer, value.real, 6,RIGHT_ALIGN);
										PrintMini(x*dx+20,y*8+10,(unsigned char*)buffer,MINI_OVER);
							} else 
							if ( ( bit==0 ) && ( MaxX==2 ) ) {
										Cplx_sprintGR1cutlim( buffer, value, 8, RIGHT_ALIGN, Norm, 8 );
//										sprintG(buffer, value.real, 6,RIGHT_ALIGN);
//										OpcodeStringToAsciiString( buffer2, buffer, 64-1 );
//										if ( value.imag ) sprintGi(buffer, value.imag, 7,RIGHT_ALIGN); else buffer[0]='\0';
//										OpcodeStringToAsciiString( buffer3, buffer, 64-1 );
//										strcat(buffer2,buffer3);
										PrintMini(x*dx+20,y*8+10,(unsigned char*)buffer,MINI_OVER);
							} else 
							if ( bit== 1 ) {	NumToBin(buffer, value.real, 8);
										PrintMini(x*dx+21,y*8+10,(unsigned char*)buffer,MINI_OVER);
							} else 
							if ( bit== 2 ) {	NumToBin(buffer, value.real, 16);
										PrintMini(x*dx+21,y*8+10,(unsigned char*)buffer,MINI_OVER);
							} else 
							if ( bit== 4 ) {	NumToBin(buffer, value.real, 4);
										PrintMini(x*dx+21,y*8+10,(unsigned char*)buffer,MINI_OVER);
							} else 
							if ( bit== 8 ) {	
								if ( ElementSize == 4 ) NumToHex(buffer, value.real, 1);
								else					NumToHex(buffer, value.real, 2);
										PrintMini(x*dx+21,y*8+10,(unsigned char*)buffer,MINI_OVER);
							} else 
							if ( bit==16 ) {	NumToHex(buffer, value.real, 4);
										PrintMini(x*dx+22,y*8+10,(unsigned char*)buffer,MINI_OVER);
							} else 
							if ( bit==32 ) {	NumToHex(buffer, value.real, 8);
										PrintMini(x*dx+21,y*8+10,(unsigned char*)buffer,MINI_OVER);
							} else 
							if ( bit==64 ) {	DNumToHex(buffer, value.real, 16);
										PrintMini(x*dx+21,y*8+10,(unsigned char*)buffer,MINI_OVER);
							} else { 	sprintG(buffer, value.real, 4,RIGHT_ALIGN);
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

		if ( ( strdisp ) && ( ElementSize > 4 ) )  {
			if ( MatXYmode==0 ) OpcodeStringToAsciiString( buffer, MatrixPtr(reg, selectY+base, selectX+base), 64-1 );
			else				OpcodeStringToAsciiString( buffer, MatrixPtr(reg, selectX+base, selectY+base), 64-1 );
			locate(1,7); PrintLine((unsigned char *)" ",21);
			locate(1,7); Print((unsigned char *)buffer);
		} else {
			if ( MatXYmode ) value=Cplx_ReadMatrix( reg, selectX+base, selectY+base);
			else			 value=Cplx_ReadMatrix( reg, selectY+base, selectX+base);
			Cplx_sprintGR2(buffer, buffer2, value, 21, RIGHT_ALIGN, CB_Round.MODE, CB_Round.DIGIT );
			OpcodeStringToAsciiString( buffer3, buffer, 64-1 );
			if ( buffer2[0] != '\0' ) {
				locate(1,6); Print((unsigned char*)buffer3);
				OpcodeStringToAsciiString( buffer3, buffer2, 64-1 );
				if ( (selectY-seltopY)==4 ) { seltopY++; goto dspjp; }
			}
			if ( list==1 ) {
				if ( MatXYmode ) locate(1,6); else locate(13,1);
				Print((unsigned char*)MatAry[reg].name);
			}
			locate(1,7); Print((unsigned char*)buffer3);
		}
		y = (selectY-seltopY) ;
		x = (selectX-seltopX) ;
		if ( dotedit == 0 ) Bdisp_AreaReverseVRAM(x*dx+20, y*8+9, x*dx+20+dx-5, y*8+15);	// reverse select element

		if ( ans==0 ) {
			if ( dotedit ) 	Fkey_dspRB( FKeyNo1, "0<>1");
			else			Fkey_Icon( FKeyNo1,  42 );	//	Fkey_dispN( FKeyNo1, "Edit");
		}
		Fkey_Icon( FKeyNo2, 964 );	//	Fkey_dispN( FKeyNo2, "GOTO");
		if ( ans==0 ) Fkey_Icon( FKeyNo3,  95 );	//	Fkey_dispR( FKeyNo3, "Init");
		if ( MatXYmode ) Fkey_dispN( FKeyNo4, "X,Y"); else Fkey_dispN( FKeyNo4, "m,n"); 
		locate(16, 8); MatAryElementSizePrint( ElementSize ) ;

		
		if ( dotedit  ) {
			MiniDotCursorX = x*dx+20;
			MiniDotCursorY = y*dy+9;
			MatDotEditCursorSetFlashMode( 1 );
		}
//		Bdisp_PutDisp_DD();
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
								value=Cplx_ReadMatrix( reg, selectX+base, selectY+base);
								if ( value.real ) value.real=0; else value.real=1;
								Cplx_WriteMatrix( reg, selectX+base, selectY+base, value );
							}
						} else {
							if ( ( selectX <= dimA ) && ( selectY <= dimB ) ) {
								value=Cplx_ReadMatrix( reg, selectY+base, selectX+base);
								if ( value.real ) value.real=0; else value.real=1;
								Cplx_WriteMatrix( reg, selectY+base, selectX+base, value );
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
								value=Cplx_ReadMatrix( reg, selectX+base, selectY+base);
								MatNumToExpBuf( value, bit );	// value -> ExpBuffer
								Cplx_WriteMatrix( reg, selectX+base, selectY+base, InputNumC_fullsub( 1, 7, 21, value));
							}
						} else {
							if ( ( selectX <= dimA ) && ( selectY <= dimB ) ) {
								value=Cplx_ReadMatrix( reg, selectY+base, selectX+base);
								MatNumToExpBuf( value, bit );	// value -> ExpBuffer
								Cplx_WriteMatrix( reg, selectY+base, selectX+base, InputNumC_fullsub( 1, 7, 21, value));
							}
						}
						if ( KeyCheckEXIT() == 0 ) selectX++;
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
				if ( ElementSize ==  4 ) if ( ( bit==0 ) || ( bit== 8 ) ) bit= 4; else bit=0;
				if ( ElementSize ==  8 ) if ( ( bit==0 ) || ( bit== 8 ) ) bit= 1; else bit=0;
				if ( ElementSize == 16 ) if ( ( bit==0 ) || ( bit==16 ) ) bit= 2; else bit=0;
				break;
			case KEY_CTRL_F6:	// hex
				if ( ElementSize ==  4 ) if ( ( bit==0 ) || ( bit== 4 ) ) bit= 8; else bit=0;
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
//				if ( dotedit  ) MatDotEditCursorSetFlashMode( 1 );
				if ( dotedit ) GetKey_DisableMenu(&key); else GetKey(&key);
				MatDotEditCursorSetFlashMode( 0 );
				switch ( key ) {			
					case KEY_CTRL_CLIP:
						if ( list==1 ) List2Clip( reg, bit );
						else           Mat2Clip(  reg, bit );
						break;
					case KEY_CTRL_PASTE:
						if ( ClipBuffer ) {
							if ( list==1 ) Clip2List( reg );
							else           Clip2Mat(  reg );
						}
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
					value=Cplx_ReadMatrix( reg, selectX+base, selectY+base);
					Cplx_WriteMatrix( reg, selectX+base, selectY+base, InputNumC_Char( 1, 7, 21, value, key));
				} else {
					value=Cplx_ReadMatrix( reg, selectY+base, selectX+base);
					Cplx_WriteMatrix( reg, selectY+base, selectX+base, InputNumC_Char( 1, 7, 21, value, key));
				}
				if ( KeyCheckEXIT() == 0 ) selectX++;
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

	char VctRegData[]={84,85,86,87,88,89,90,91,92,93,94,95,96,97,98,99,100,101,102,103,104,105,106,107,108,109,26,27,28,
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
			locate( 1, 1+i);
			switch ( listdsp ) {
				case 0: // Mat
					reg=MatRegData[k]; j=SetVarCharMat( buffer, reg);
					Print((unsigned char*)"Mat");
					break;
				case 1: // List
					reg=LstRegData[k]; 
					if ( ( 26<=reg ) && ( reg<=28 ) ) { 
						j=SetVarCharMat( buffer, reg); 
						Print((unsigned char*)"Lst ");
					} else  { 
						if ( reg<58 ) k-=3;
						if ( ListFilePtr ) reg = k+ListFilePtr;
						sprintf( buffer,"L%d",k+1);
						locate( 1, 1+i); Print((unsigned char*)buffer);
						if ( MatAry[reg].name[0] !='\0' ) {
							sprintf( buffer,"%s",MatAry[reg].name);
							j=strlen(buffer);
						} else j=0;
					}
					break;
				case 2: // Vct
					reg=VctRegData[k]; j=SetVarCharVct( buffer, reg);
					Print((unsigned char*)"Vct");
					break;
			}
			buffer[j]='\0';
			locate( 5, 1+i); Print((unsigned char*)buffer);
			locate(13, 1+i); Print((unsigned char*)":");
			x=5+CB_MB_ElementCount( buffer ); if ( x>12 ) x=12;
			locate( x, 1+i); MatAryElementSizePrint( MatAry[reg].ElementSize ) ;
			if ( MatAry[reg].SizeA ) {
				locate( 4, 1+i); if ( MatAry[reg].Maxbyte <= 0 ) Print((unsigned char*)"*");	// Adrs redefinition
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
//		Fkey_Icon( FKeyNo6, 775 );	//	Fkey_dispN( FKeyNo6,  "A<>a");
		if ( small  ) Fkey_dispN_aA( FKeyNo6, "A<>a"); else Fkey_dispN_Aa( FKeyNo6, "A<>a");

		Bdisp_PutDisp_DD();

		k=select; if ( small ) { if ( k>=29 ) k-=29; else k+=29; }
		switch ( listdsp ) {
			case 0: // Mat
				Fkey_dispN( FKeyNo5, "Mat:");
				reg=MatRegData[k]; 
				break;
			case 1: // List
				if ( ListFileNo ) {
					sprintf( buffer, "L:%d", ListFileNo );
					Fkey_dispN( FKeyNo5, buffer);
				} else  {
					Fkey_dispN( FKeyNo5, "Lst:");
				}
				reg=LstRegData[k]; if ( ListFilePtr ) if ( ( reg<26 ) || ( 28<reg ) ) reg = k -(reg<58)*3 +ListFilePtr;
				break;
			case 2: // Vct
				Fkey_dispN( FKeyNo5, "Vct:");
				reg=VctRegData[k];
				break;
		}
		ElementSize=MatAry[reg].ElementSize;

		GetKey( &key );
		if ( KEY_CTRL_XTT   == key ) select=23;	// X
		if ( KEY_CHAR_ANS   == key ) select=28;	// Ans
		if ( KEY_CHAR_THETA == key ) select=27;	// Theta
		if ( KEY_CHAR_VALR  == key ) select=26;	// <r>
		if ( ( 'A' <= key ) && ( key <= 'z' ) ) {
			select=key-'A';
		}
		switch (key) {
			case KEY_CTRL_EXIT:
				cont=0;
				break;
		
			case KEY_CTRL_EXE:
				if ( ElementSize==0 ) goto newmat; else goto edmat;
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
			  newmat:
				if ( ElementSize==0 ) 	ElementSize = DefaultElemetSize() ;
				key=SetDimension(reg, &dimA, &dimB, &ElementSize, &base, listdsp );
				if ( key==KEY_CTRL_EXIT ) break;
				if ( MatAry[reg].SizeA == 0 ) {
					if ( DimMatrixSub(reg, ElementSize, dimA, dimB, base )==0 ) CB_ErrMsg(ErrorNo);
				} else {
					if ( DimMatrixSubNoinit(reg, ElementSize, dimA, dimB, base, 0 )==0 ) CB_ErrMsg(ErrorNo);
				}
				HiddenRAM_MatAryStore();	// MatAry ptr -> HiddenRAM
			  edmat:
				EditMatrix( reg + 0x1000*(listdsp), 0 );
				break;
			case KEY_CTRL_RIGHT:
				select+=7;
				if ( select >= opNum  ) select=opNum;
				break;
			case KEY_CTRL_F4:
				MatDefaultValue = InitMatrix( reg, MatDefaultValue ,ElementSize ) ;
				break;
			case KEY_CTRL_F5:
				switch ( listdsp ) {
					case 1:	// List
						if ( ListFileNo ) {
							i=SelectNum1( "ListFile", ListFileNo ,1, ExtendList+1, &key);
							if ( key == KEY_CTRL_EXIT ) break;
							ListFileNo  = i;
							CB_ListNo2Ptr( ListFileNo );
							reg=LstRegData[k]; if ( ListFilePtr ) if ( ( reg<26 ) || ( 28<reg ) ) reg = k -(reg<58)*3 +ListFilePtr;
						} else {
							i=SelectNum1( "List", listselect ,1, ExtListMax, &key);
							if ( key == KEY_CTRL_EXIT ) break;
							listselect=i;
							reg=i-1;
						 	if ( reg<52 ) { select=reg;
								if ( 26<=reg ) { reg+=6; select+=3; } else reg+=58;
							} else reg+=32+26;
						}
						break;
					case 0:	// Mat
						i=SelectNum1( "Mat ", listselect ,1, ExtListMax, &key);
						if ( key == KEY_CTRL_EXIT ) break;
						listselect=i;
						reg=i-1;
					 	if ( reg<52 ) { select=reg;
							if ( 26<=reg ) { reg+=6; select+=3; }
						} else reg+=32+26;
						break;
					case 2:	// Vct
						i=SelectNum1( "Vct ", listselect ,1, ExtListMax, &key);
						if ( key == KEY_CTRL_EXIT ) break;
						listselect=i;
						reg=i-1;
					 	if ( reg<52 ) { select=reg;
							if ( 26<=reg ) { reg+=6; select+=3; } else reg+=84;
						} else reg+=32+26;
						break;
				}
				ElementSize=MatAry[reg].ElementSize;
				if ( (  listdsp != 1 ) || ( ListFileNo == 0 ) ) goto newmat;
				break;
			case KEY_CTRL_F6:
				small=1-small;
				break;
				
			case KEY_CTRL_UP:
				select--;
				if ( select < 0 )  select = opNum;
				break;
			case KEY_CTRL_DOWN:
				select++;
				if ( select > opNum ) select =0;
				break;
			case KEY_CTRL_PAGEUP:
				select-=7;
				if ( select < 0 )  select = 0;
				break;
			case KEY_CTRL_PAGEDOWN:
				select+=7;
				if ( select > opNum ) select = opNum;
				break;

			case KEY_CTRL_OPTN:		// Mat/Vct <-> List
				switch ( listdsp ) {
					case 0:	// Mat
						listdsp=1; // ->list
						break;
					case 1:	// List
						listdsp=0; // ->Mat
						break;
					case 2:	// Vct
						listdsp=1; // ->list
						break;
				}
				break;
			case KEY_CTRL_VARS:		// Mat/List <-> Vct
				switch ( listdsp ) {
					case 0:	// Mat
						listdsp=2; // ->Vct
						break;
					case 1:	// List
						listdsp=2; // ->Vct
						break;
					case 2:	// Vct
						listdsp=0; // ->Mat
						break;
				}
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
int ElementSizeSelectAdrs( char *SRC, int *base, int *adrs, int ElementSize ) {	// Mat A.L0(0x88000000)
	int c,d;
	*base=MatBase;
	*adrs=0;
	c =SRC[ExecPtr];
	if ( c=='.' ) {
		c =SRC[++ExecPtr];
		ElementSize = DefaultElemetSize();
		if ( ( c=='0' ) || ( c=='1' ) ) { *base = c-'0'; c=SRC[++ExecPtr]; }
		if ( ( c=='P' ) || ( c=='p' ) ) { ExecPtr++; ElementSize= 1; }
		else
		if ( ( c=='V' ) || ( c=='v' ) ) { ExecPtr++; ElementSize= 2;
			c =SRC[ExecPtr];
			if ( ( c=='T' ) || ( c=='t' ) ) { ExecPtr++; ElementSize= 3; } // text VRAM
			if ( ( c=='G' ) || ( c=='g' ) ) { ExecPtr++; ElementSize= 5; } // GraphicVRAM
		} else
		if ( ( c=='N' ) || ( c=='n' ) ) { ExecPtr++; ElementSize= 4; }
		else
		if ( ( c=='B' ) || ( c=='b' ) ) { ExecPtr++; ElementSize= 8; }
		else
		if ( ( c=='W' ) || ( c=='w' ) ) { ExecPtr++; ElementSize=16; }
		else
		if ( ( c=='L' ) || ( c=='l' ) ) { ExecPtr++; ElementSize=32; }
		else
		if ( ( c=='F' ) || ( c=='f' ) ) { ExecPtr++; ElementSize=64; }
		else
		if ( ( c=='C' ) || ( c=='c' ) ) { ExecPtr++; ElementSize=128; }
		c =SRC[ExecPtr];
		if ( c=='.' ) c =SRC[++ExecPtr];
		if ( ( c=='0' ) || ( c=='1' ) ) { 
			*base = c-'0' ;
			c = SRC[++ExecPtr];
		}
		if ( c=='(' ) { 
			ExecPtr++;
			*adrs = CB_EvalInt( SRC );
			if ( SRC[ExecPtr] == ')' ) ExecPtr++;
		}
	}
	else {
		if ( ElementSize == 0 ) ElementSize = ( DefaultElemetSize() ) +0x100 ;
	}
	return ElementSize;
}
int ElementSizeSelect( char *SRC, int *base, int ElementSize ) {
	int adrs;
	return ElementSizeSelectAdrs( SRC, &(*base), &adrs, ElementSize ) ;
}

void CB_MatrixInitsubNoMat( char *SRC, int reg, int dimA, int dimB , int ElementSize, int dimdim ) { 	// 1-
	int c;
	int base=MatBase;
	int adrs;
	if ( reg>=0 ) {
		ElementSize=ElementSizeSelectAdrs( SRC, &base, &adrs, ElementSize) & 0xFF;
		if ( dimdim )	DimMatrixSubNoinit( reg, ElementSize, dimA, dimB, base, adrs );
		else			DimMatrixSub( reg, ElementSize, dimA, dimB, base);
	} else { CB_Error(SyntaxERR); return; }  // Syntax error
}
void CB_MatrixInitsub( char *SRC, int *reg, int dimA, int dimB , int ElementSize, int dimdim ) { 	// 1-
	int c,d;
	int base=MatBase;
	c =SRC[ExecPtr];
	if ( c == 0x7F ) {
		d =SRC[ExecPtr+1];
		if ( d ==0x40 ) {		// Mat
			ExecPtr+=2;
			*reg=MatRegVar(SRC);
		  jp:
			CB_MatrixInitsubNoMat( SRC, *reg, dimA, dimB, ElementSize, dimdim );
		} else 
		if ( d ==0xFFFFFF84 ) {	// Vct
			ExecPtr+=2;
			*reg=VctRegVar(SRC);
			goto jp;
		}
	} else { CB_Error(SyntaxERR); return; }  // no Mat  Syntax error
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

int SkipSpc( char *SRC ) {
	return SkipSpcsub( SRC, &ExecPtr );
}
int SkipSpcCR( char *SRC ) {
	return SkipSpcCRsub( SRC, &ExecPtr );
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
	complex data;
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
		data=CB_Cplx_EvalDbl( SRC );
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
				data=CB_Cplx_EvalDbl( SRC );
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
			if (CB_INT==1)	WriteMatrixInt( reg, m, n, EvalIntsubTop( SRC ));
			else 		    Cplx_WriteMatrix( reg, m, n, Cplx_EvalsubTop( SRC ));
			SkipSpcCR(SRC);
			ExecPtr++;	// , skip
			SkipSpcCR(SRC);
			n++;
		}
		m++;
		ExecPtr++;
		SkipSpcCR(SRC);
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
void CB_ClrVct( char *SRC ) { //	ClrVct A
	int i;
	int reg=VctRegVar(SRC);
	if ( reg>=0 ) {
		DeleteMatrix( reg );
	} else { ErrorNo=0;	//
		for ( i=84; i<84+26; i++ ) DeleteMatrix( i );	// ClrVct A-Z
	}
}
void CB_ClrList( char *SRC ) { //	ClrList 1
	int i;
	int reg=ListRegVar( SRC );
	if ( reg>=0 ) {
		DeleteMatrix( reg );
	} else { ErrorNo=0;	//
		if ( ListFilePtr ) {
			for ( i=32; i<32+26; i++ ) DeleteMatrix( i +ListFilePtr );	// ClrList 1-26 (File 2~)
		} else {
			for ( i=58; i<58+26; i++ ) DeleteMatrix( i );	// ClrList 1-26
		}
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
				if ( m<sizeA2+base2 ) for ( n=base; n<sizeB+base; n++ ) if ( n<sizeB2+base2 ) Cplx_WriteMatrix( reg2, m, n,  Cplx_ReadMatrix( reg, m, n ) );
			}
		}
	}
}

void MatCalcDimCopySub( char *SRC, int reg, int reg2, int dim ) {
	int base,base2;
	int dimA,dimB,dimA2,dimB2;
	int ElementSize,ElementSize2;
	ElementSize =MatAry[reg].ElementSize;
	ElementSize2=ElementSizeSelect( SRC, &base2, ElementSize ) & 0xFF;
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
					case  4:		//  4 -> 1
						dimB2 *= 4;	break;
					case  8:		//  8 -> 1
						dimB2 *= 8;	break;
					case 16:		// 16 -> 1
						dimB2 *=16;	break;
					case 32:		// 32 -> 1
						dimB2 *=32;	break;
					case 64:		// 64 -> 1
						dimB2 *=64;	break;
					case 128:		// 64 -> 1
						dimB2 *=128;break;
				}
				break;
			case  4:
				switch ( ElementSize ) {
					case  1:		//  1 -> 4
						dimB2=((dimB2-1)/4)+1;	break;
					case  8:		//  8 -> 4
						dimB2 *= 2;	break;
					case 16:		// 16 -> 4
						dimB2 *= 4;	break;
					case 32:		// 32 -> 4
						dimB2 *= 8;	break;
					case 64:		// 64 -> 4
						dimB2 *=16;	break;
					case 128:		// 128 -> 4
						dimB2 *=32;	break;
				}
				break;
			case  8:
				switch ( ElementSize ) {
					case  4:		// 4 -> 8
						dimB2=((dimB2-1)/2)+1;	break;
					case  1:		// 1 -> 8
						dimB2=((dimB2-1)/8)+1;	break;
					case 16:		// 16 -> 8
						dimB2 *= 2;	break;
					case 32:		// 32 -> 8
						dimB2 *= 4;	break;
					case 64:		// 64 -> 8
						dimB2 *= 8;	break;
					case 128:		// 128 -> 8
						dimB2 *= 16;	break;
				}
				break;
			case 16:
				switch ( ElementSize ) {
					case  4:		// 4 -> 16
						dimB2=((dimB2-1)/2)+1;
						goto j16;
					case  1:		// 1 -> 16
						dimB2=((dimB2-1)/8)+1;
					case  8:		// 8 -> 16
					  j16:
						if ( dimB2 & 1 ) { dimB2=-1; break; }
						dimB2 /= 2;	break;
					case 32:		// 32 -> 16
						dimB2 *= 2;	break;
					case 64:		// 64 -> 16
						dimB2 *= 4;	break;
					case 128:		// 128 -> 16
						dimB2 *= 8;	break;
				}
				break;
			case 32:
				switch ( ElementSize ) {
					case  4:		// 4 -> 32
						dimB2=((dimB2-1)/2)+1;
						goto j32;
					case  1:		// 1 -> 32
						dimB2=((dimB2-1)/8)+1;
					case  8:		// 8 -> 32
					  j32:
						if ( dimB2 & 3 ) { dimB2=-1; break; }
						dimB2 /= 4;	break;
					case 16:		// 16 -> 32
						if ( dimB2 & 1 ) { dimB2=-1; break; }
						dimB2 /= 2;	break;
					case 64:		// 64 -> 32
						dimB2 *= 2;	break;
					case 128:		// 128 -> 32
						dimB2 *= 4;	break;
				}
				break;
			case 64:
				switch ( ElementSize ) {
					case  4:		// 4 -> 64
						dimB2=((dimB2-1)/2)+1;
						goto j64;
					case  1:		// 1 -> 64
						dimB2=((dimB2-1)/8)+1;
					  j64:
						if ( dimB2 & 7 ) { dimB2=-1; break; }
					case  8:		// 8 -> 64
						dimB2 /= 8;	break;
					case 16:		// 16 -> 64
						if ( dimB2 & 3 ) { dimB2=-1; break; }
						dimB2 /= 4;	break;
					case 32:		// 32 -> 64
						if ( dimB2 & 1 ) { dimB2=-1; break; }
						dimB2 /= 2;	break;
					case 128:		// 128 -> 64
						dimB2 *= 2;	break;
				}
				break;
			case 128:
				switch ( ElementSize ) {
					case  4:		// 4 -> 128
						dimB2=((dimB2-1)/2)+1;
						goto j128;
					case  1:		// 1 -> 128
						dimB2=((dimB2-1)/8)+1;
					  j128:
					case  8:		// 8 -> 128
						if ( dimB2 & 15 ) { dimB2=-1; break; }
						dimB2 /=16;	break;
					case 16:		// 16 -> 128
						if ( dimB2 & 7 ) { dimB2=-1; break; }
						dimB2 /= 8;	break;
					case 32:		// 32 -> 128
						if ( dimB2 & 3 ) { dimB2=-1; break; }
						dimB2 /= 4;	break;
					case 64:		// 64 -> 128
						if ( dimB2 & 1 ) { dimB2=-1; break; }
						dimB2 /= 2;	break;
				}
				break;
		}
		if ( ( dimA2 < 0 ) || ( dimB2 < 0 ) ) { CB_Error(DimensionERR); return ; }	// Dimension error
		if ( reg != reg2 ) { 
			DimMatrixSub( reg2, ElementSize2, MatAry[reg].SizeA, MatAry[reg].SizeB, MatAry[reg].Base );
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
		DimMatrixSub( reg2, ElementSize2, MatAry[reg].SizeA, MatAry[reg].SizeB, MatAry[reg].Base );
		CopyMatrix( reg2, reg );
	}
}

int CB_MatCalc( char *SRC, int Matflag ) { //	Mat A -> Mat B  etc
	char dspflagtmp;
	int c,d,i,m,n;
	int dim=0,dimA,dimB,dimA2,dimB2;
	int ElementSize,ElementSize2;
	int reg,reg2;
	int base,base2;
	int excptr=ExecPtr;
	double	*dptr, *dptr2;
	
	if ( Matflag==0x40 ) reg=MatRegVar(SRC); else reg=VctRegVar(SRC); 
	c =SRC[ExecPtr];
	if ( c == 0x0E ) {	// ->
		if ( reg>=0 ) {
			if ( MatAry[reg].SizeA == 0 ) { CB_Error(NoMatrixArrayERR); return 0; }	// No Matrix Array error
		} else { CB_Error(SyntaxERR); return 0; }	// Syntax error
		ExecPtr++;
		c =SRC[ExecPtr];
		if ( c != 0x7F ) { CB_Error(SyntaxERR); return 0; }	// Syntax error
		c =SRC[++ExecPtr];
		if ( c == 0x46 ) { 		// Mat A -> Dim Mat B[.w]
			dim=1;
			c =SRC[++ExecPtr];
			if ( c != 0x7F ) { CB_Error(SyntaxERR); return 0; }	// Syntax error
			c =SRC[++ExecPtr];
		}
		if ( c ==0x40 ) {	// Mat
			ExecPtr++;
			reg2=MatRegVar(SRC); 
		} else 
		if ( c == 0xFFFFFF84 ) {	// Vct
			ExecPtr++;
			reg2=VctRegVar(SRC); 
		} else { CB_Error(SyntaxERR); return 0; }	// Syntax error
		if ( reg2<0 ) { CB_Error(SyntaxERR); return 0; }	// Syntax error
		
		MatCalcDimCopySub( SRC, reg, reg2, dim ); if ( ErrorNo ) return 0;

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
	if ( reg<0 ) { CB_Error(SyntaxERR); return 0; }	// Syntax error
	c =SRC[ExecPtr];
	if ( c == 0x0E ) {	// ->
		if ( MatAry[reg].SizeA == 0 ) { CB_Error(NoMatrixArrayERR); return 0; }	// No Matrix Array error
		ExecPtr++;
		c =SRC[ExecPtr];
		if ( c != 0x7F ) { CB_Error(SyntaxERR); return 0; }	// Syntax error
		c =SRC[++ExecPtr];
		if ( c == 0x46 ) { 		// List 1 -> Dim List 2[.w]
			dim=1;
			c =SRC[++ExecPtr];
			if ( c != 0x7F ) { CB_Error(SyntaxERR); return 0; }	// Syntax error
			c =SRC[++ExecPtr];
		}
//		if ( ( c !=0x51 ) ) { CB_Error(SyntaxERR); return 0; }	// Syntax error
		ExecPtr++;
		reg2=ListRegVar( SRC );
		if ( reg2<0 ) { CB_Error(SyntaxERR); return 0; }	// Syntax error

		MatCalcDimCopySub( SRC, reg, reg2, dim ); if ( ErrorNo ) return 0;
		
		dspflagtmp=0;
	} else {
		if ( c == '[' ) {	// [
			ExecPtr++;
			dimA = CB_EvalInt( SRC );
			if ( MatAry[reg].SizeA == 0 ) base=MatBase; else base=MatAry[reg].Base;
			if ( SRC[ExecPtr] == ']' ) ExecPtr++ ;	// 
			if ( dimA < MatAry[reg].Base ) {		// List 1[0]->"ABCDEF"
				CB_CurrentStr=NewStrBuffer(); if ( ErrorNo ) return 0;
				memcpy( CB_CurrentStr, MatAry[reg].name, 9);
				CB_StrPrint( SRC , 0 ) ;
				return 0;
			}
		}
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
	complex value;
	
	value=CB_Cplx_EvalDbl( SRC );
	c =SRC[ExecPtr];
	if ( c != ',' ) { CB_Error(SyntaxERR); return; }  // Syntax error
	c =SRC[++ExecPtr];
	if ( ( '1'<=c ) && ( c<='9' ) ) reg=ListRegVar(SRC);	// Fill(value, 1) = Fill(value, List 1)
	else MatrixOprandreg( SRC, &reg);	// Fill(value, Mat A)
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
	complex	dtmp,dtmp2;
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
	if ( ( a < base ) || ( dimA <= a ) ) { CB_Error(ArgumentERR); return ; } // Argument error

	if ( SRC[ExecPtr] != ',' ) { CB_Error(SyntaxERR); return; }  // Syntax error
	ExecPtr++;
	b = CB_EvalInt( SRC );
	if ( ( b < base ) || ( dimA <= b ) ) { CB_Error(ArgumentERR); return ; } // Argument error

	if ( SRC[ExecPtr] == ')' ) ExecPtr++;

	if ( a == b ) return;

	switch ( ElementSize ) {
		case 64:
			for ( n=base; n<dimB; n++ ) {
				dtmp  = Cplx_ReadMatrix( reg, a, n );
				dtmp2 = Cplx_ReadMatrix( reg, b, n );
				Cplx_WriteMatrix( reg, b, n, dtmp  );
				Cplx_WriteMatrix( reg, a, n, dtmp2 );
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
	complex dbl_k;
	int  int_k;

	if (CB_INT==1) int_k = CB_EvalInt( SRC ); else dbl_k = CB_Cplx_EvalDbl( SRC ); 
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
	if ( ( a < base ) || ( dimA <= a ) ) { CB_Error(ArgumentERR); return ; } // Argument error

	if ( SRC[ExecPtr] == ')' ) ExecPtr++;

	switch ( ElementSize ) {
		case 128:
		case 64:
			for ( n=base; n<dimB; n++ ) {
				Cplx_WriteMatrix( reg, a, n, Cplx_fMUL( Cplx_ReadMatrix( reg, a, n ), dbl_k ) );
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
	complex dbl_k;
	int  int_k;

	if (CB_INT==1) int_k = CB_EvalInt( SRC ); else dbl_k = CB_Cplx_EvalDbl( SRC ); 
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
	if ( ( a < base ) || ( dimA <= a ) ) { CB_Error(ArgumentERR); return ; } // Argument error

	if ( SRC[ExecPtr] != ',' ) { CB_Error(SyntaxERR); return; }  // Syntax error
	ExecPtr++;
	b = CB_EvalInt( SRC );
	if ( ( b < base ) || ( dimA <= b ) ) { CB_Error(ArgumentERR); return ; } // Argument error

	if ( SRC[ExecPtr] == ')' ) ExecPtr++;

	if ( a == b ) return;

	switch ( ElementSize ) {
		case 128:
		case 64:
			for ( n=base; n<dimB; n++ ) {
				Cplx_WriteMatrix( reg, b, n, Cplx_fADD( Cplx_fMUL(Cplx_ReadMatrix( reg, a, n ),dbl_k), Cplx_ReadMatrix( reg, b, n ) ) );
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
	if ( ( a < base ) || ( dimA <= a ) ) { CB_Error(ArgumentERR); return ; } // Argument error

	if ( SRC[ExecPtr] != ',' ) { CB_Error(SyntaxERR); return; }  // Syntax error
	ExecPtr++;
	b = CB_EvalInt( SRC );
	if ( ( b < base ) || ( dimA <= b ) ) { CB_Error(ArgumentERR); return ; } // Argument error

	if ( SRC[ExecPtr] == ')' ) ExecPtr++;

	if ( a == b ) return;

	switch ( ElementSize ) {
		case 128:
		case 64:
			for ( n=base; n<dimB; n++ ) {
				Cplx_WriteMatrix( reg, b, n, Cplx_fADD( Cplx_ReadMatrix( reg, a, n ), Cplx_ReadMatrix( reg, b, n ) ) );
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
	int tmpreg=Mattmpreg;
	
//	ListEvalsubTop(SRC);
	ListEvalsub1(SRC);
	if ( dspflag != 3 ) { CB_Error(ArgumentERR); return ; } // Argument error
	reg = CB_MatListAnsreg;

	ElementSize=MatAry[reg].ElementSize;
	if ( ElementSize == 2 ) { CB_Error(ArgumentERR); return ; } // Argument error
	base       =MatAry[reg].Base;
	dimA       =MatAry[reg].SizeA;
	dimB       =MatAry[reg].SizeB;
	DimMatrixSub( tmpreg, ElementSize, dimB, dimA, base);	//
	if ( ErrorNo ) return ;
	reg2=tmpreg;
	
	switch ( ElementSize ) {
		case 128:
		case 64:
			for ( m=base; m<dimA+base; m++ ) {
				for ( n=base; n<dimB+base; n++ ) Cplx_WriteMatrix( reg2, n, m,  Cplx_ReadMatrix( reg, m, n ) );
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
}

void CB_Identity( char *SRC ) { //	Identity 3 -> Mat Ans
	int c;
	int reg,reg2;
	int m,n;
	int sizeA,sizeB;
	int ElementSize;
	int base;
	complex cone ={1,0};
	complex czero={0,0};

	n=CB_EvalInt( SRC );
	if ( n<1 ) { CB_Error(ArgumentERR); return ; } // Argument error
	NewMatListAns( n, n, MatBase, DefaultElemetSize() );
	if ( ErrorNo ) return ;
	reg  = CB_MatListAnsreg;
	base =MatAry[reg].Base;
	sizeA=n;
	sizeB=n;
	for ( n=base ; n<sizeA +base ; n++ ) {
		for ( m=base ; m<sizeB +base ; m++ ) {
			if ( m==n )	Cplx_WriteMatrix( reg, m, n, cone );
			else		Cplx_WriteMatrix( reg, m, n, czero);
		}
	}

	if ( SRC[ExecPtr] == ')' ) ExecPtr++;
	dspflag = 3;	// Mat ans
}

//-----------------------------------------------------------------------------	List
//-----------------------------------------------------------------------------
void CB_ListInitsub( char *SRC, int *reg, int dimA , int ElementSize, int dimdim ) { 	// 1-
	int c;
	int base=MatBase;
	int dimB=1;
	int adrs;
	*reg=ListRegVar( SRC );
	if ( *reg>=0 ) {
		ElementSize=ElementSizeSelectAdrs( SRC, &base, &adrs, ElementSize) & 0xFF;
		if ( dimdim )	DimMatrixSubNoinit( *reg, ElementSize, dimA, dimB, base, adrs );
		else			DimMatrixSub( *reg, ElementSize, dimA, dimB, base);
	} else { CB_Error(SyntaxERR); return; }  // Syntax error
}

void CB_List( char *SRC ) { //	{1.2,3,4,5,6} -> List Ans
	int c,d;
	int dimA,dimB,i;
	int reg;
	int exptr,exptr2;
	complex data;
	int m,n;
	int base=MatBase;
	int ElementSize;

	exptr=ExecPtr;
	c=SkipSpcCR(SRC);
	n=1;
	while ( 0 ) {
		data=CB_Cplx_EvalDbl( SRC );
		c=SkipSpc(SRC);
		if ( c != ',' ) break;
		ExecPtr++;
		SkipSpcCR(SRC);
		n++;
	}
	ExecPtr=exptr;


	dimA=64;
	dimB=1;

	ElementSize=ElementSizeSelect( SRC, &base, 0) & 0xFF;
	NewMatListAns( dimA, dimB, base, ElementSize );
	if ( ErrorNo ) return ;
	reg=CB_MatListAnsreg;

	m=base; n=base;
	dimA -= base;
	while ( 1 ) {
		if (CB_INT==1)	WriteMatrixInt( reg, m, n, EvalIntsubTop( SRC ));
		else 		    Cplx_WriteMatrix( reg, m, n, CB_Cplx_EvalDbl( SRC ));
		c=SkipSpc(SRC);
//		c = SRC[ExecPtr];
		if ( c != ',' ) break;
		ExecPtr++;	// "," skip
		SkipSpcCR(SRC);
		m++;
		if ( m >= dimA ) MatElementPlus( reg, m, 1 );	// List element +
	}
	if ( c == '}' ) ExecPtr++;
	MatAry[reg].SizeA = m+1-base;
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
	if ( reg<0 ) {	ErrorNo=0;	// error cancel
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

	if ( ( n<base ) || ( n>=sizeB+base ) ) { CB_Error(DimensionERR); return ; }	// Dimension error
	
	NewMatListAns( sizeA, 1, base, ElementSize );
	if ( ErrorNo ) return ;
	reg2=CB_MatListAnsreg;
	for ( m=base ; m<sizeA +base ; m++ ) Cplx_WriteMatrix( reg2, m, base, Cplx_ReadMatrix( reg , m, n ) );

	if ( SRC[ExecPtr] == ')' ) ExecPtr++;
	dspflag = 4;
}

void List2Mat( int *reg, int tmpreg, int sizeB ) {	// List>Mat( List 1[,List 2][,List 3]...) -> tmpreg
	int m,n;
	int sizeA;
	int ElementSize;
	int base;
	int i;

	sizeA        = MatAry[ reg[0] ].SizeA;
	base         = MatAry[ reg[0] ].Base;
	ElementSize  = MatAry[ reg[0] ].ElementSize;
	if ( ElementSize  == 2 ) ElementSize  == 1;
	
	DimMatrixSub( tmpreg, ElementSize, sizeA, sizeB, base);	//
	if ( ErrorNo ) return ;

	n=base; i=0;
	do {
		for ( m=base ; m<sizeA +base ; m++ ) Cplx_WriteMatrix( tmpreg, m, n, Cplx_ReadMatrix( reg[i] , m, base ) );
		n++; i++;
	} while ( i < sizeB );
}

void CB_List2Mat( char *SRC ) {	// List>Mat( List 1[,List 2][,List 3]...) -> Mat Ans
	int c,i;
	int reg,areg[8];
	int tmpreg=Mattmpreg;
	int m,n;
	int sizeA,sizeB;
	int ElementSize;
	int base;
	int ansreg=CB_MatListAnsreg;

	i=0;
	reg=ListRegVar(SRC);
	if ( reg<0 ) {	ErrorNo=0;	// error cancel
		ListEvalsubTop(SRC);
		if ( dspflag != 4 ) { CB_Error(ArgumentERR); return ; } // Argument error
		areg[i++] = CB_MatListAnsreg;
	} else  areg[i++] = reg;

	while ( SRC[ExecPtr] == ',' ) { 
		ExecPtr++;
		reg=ListRegVar(SRC);
		if ( reg<0 ) {	ErrorNo=0;	// error cancel
			ListEvalsubTop(SRC);
			if ( dspflag != 4 ) { CB_Error(ArgumentERR); return ; } // Argument error
			areg[i++] = CB_MatListAnsreg;
		} else  areg[i++] = reg;
		if ( MatAry[reg].SizeA == 0 ) { CB_Error(ArgumentERR); } // Argument error
		if ( i>8 ) { CB_Error(TooMuchData); }
		if ( ErrorNo ) return ;
	}
	
	List2Mat( &areg[0], tmpreg, i );
	if ( ErrorNo ) return ;

	if ( SRC[ExecPtr] == ')' ) ExecPtr++;
	CB_MatListAnsreg=ansreg;
	CopyMatList2Ans( tmpreg );		// tmpreg -> CB_MatListAnsreg
	dspflag = 3;
}


int CB_RanListsub( char *SRC ) {
	int m;
	int ElementSize = DefaultElemetSize() ;
	
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
  	for ( m=base ; m<MatAry[reg ].SizeA +base ; m++ ) WriteMatrixInt( reg, m, base, frandIntint( x, y ) );
	if ( SRC[ExecPtr] == ')' ) ExecPtr++;
	dspflag = 4;
}

double CB_RanNorm( char *SRC ) {	// RanNorm#( sd, mean [,n] ) -> List Ans
	double sd,mean;
	int reg;
	int m;
	int sizeA;
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
void CB_AugmentMat( char *SRC, int reg ) {	// Augment( Mat A, Mat B )	
	int reg2;
	int m,n,i;
	int sizeA,sizeB,sizeA2,sizeB2,sizeB3;
	int ElementSize,ElementSize2;
	int base,base2;
	int tmpreg=Mattmpreg;
	int ansreg=CB_MatListAnsreg;

	if ( SRC[ExecPtr] != ',' ) { CB_Error(SyntaxERR); return; }  // Syntax error
	ExecPtr++;
	
	Cplx_ListEvalsubTop(SRC);
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
	DimMatrixSub( tmpreg, ElementSize, sizeA, sizeB3, base);	//
	if ( ErrorNo ) return ;
	i=base;
	for ( m=base ; m<sizeA +base ; m++ ) 
		for ( n=base ; n<sizeB +base ; n++ ) Cplx_WriteMatrix( tmpreg, m, n, Cplx_ReadMatrix( reg , m, n ) );
	for ( m=base ; m<sizeA+base ; m++ ) 
		for ( n=base ; n<sizeB2 +base ; n++ ) Cplx_WriteMatrix( tmpreg, m, n+sizeB, Cplx_ReadMatrix( reg2, m, n) );
	
	if ( SRC[ExecPtr] == ')' ) ExecPtr++;
	CB_MatListAnsreg=ansreg;
	CopyMatList2Ans( tmpreg );		// tmpreg -> CB_MatListAnsreg
	dspflag = 3;
}
void CB_AugmentList( char *SRC, int reg ) {	// Augment( List1, List2 )	
	int reg2;
	int m,n,i;
	int sizeA,sizeB,sizeA2,sizeB2,sizeA3;
	int ElementSize,ElementSize2;
	int base,base2;
	int tmpreg=Mattmpreg;
	int ansreg=CB_MatListAnsreg;

	if ( SRC[ExecPtr] != ',' ) { CB_Error(SyntaxERR); return; }  // Syntax error
	ExecPtr++;
	
	Cplx_ListEvalsubTop(SRC);
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
	DimMatrixSub( tmpreg, ElementSize, sizeA3, sizeB, base);	//
	if ( ErrorNo ) return ;
	i=base;
	for ( m=base ; m<sizeA +base ; m++ ) Cplx_WriteMatrix( tmpreg, i++, base, Cplx_ReadMatrix( reg , m, base ) );
	for ( m=base ; m<sizeA2+base ; m++ ) Cplx_WriteMatrix( tmpreg, i++, base, Cplx_ReadMatrix( reg2, m, base ) );
	
	if ( SRC[ExecPtr] == ')' ) ExecPtr++;
	CB_MatListAnsreg=ansreg;
	CopyMatList2Ans( tmpreg );		// tmpreg -> CB_MatListAnsreg
	dspflag = 4;
}

void CB_Augment( char *SRC ) {	// Augment( List1, List2 )		Augment( Mat A, Mat B)
	int reg,reg2;
	int m,n;
	int sizeA,sizeB,sizeA2,sizeB2;
	int ElementSize,ElementSize2;
	int base,base2;
	int c;
	Cplx_ListEvalsubTop(SRC);
	reg = CB_MatListAnsreg;
	if ( dspflag == 3 )  { CB_AugmentMat(SRC, reg); return; }
	if ( dspflag == 4 )  { CB_AugmentList(SRC, reg); return; }
	 { CB_Error(ArgumentERR); return ; } // Argument error
}

int qsortA_cplx( const void *p1, const void *p2 ){
	complex a,b;
	double  c,d;
	a = *(complex*)p1;
	b = *(complex*)p2;
	if ( ( a.imag != 0 ) || ( b.imag != 0 ) ) {
		c = fabsz(a);
		d = fabsz(b);
		if ( c == d ) return 0;
		if ( c >  d ) return 1;
	} else {
		if ( a.real == b.real ) return 0;
		if ( a.real >  b.real ) return 1;
	}
	return -1;
}
int qsortB_cplx( const void *p1, const void *p2 ){
	complex a,b;
	double  c,d;
	a = *(complex*)p1;
	b = *(complex*)p2;
	if ( ( a.imag != 0 ) || ( b.imag != 0 ) ) {
		c = fabsz(a);
		d = fabsz(b);
		if ( d == c ) return 0;
		if ( d >  c ) return 1;
	} else {
		if ( b.real == a.real ) return 0;
		if ( b.real >  a.real ) return 1;
	}
	return -1;
}
int qsortA_dbl( const void *p1, const void *p2 ){
	double a,b;
	a = *(double*)p1;
	b = *(double*)p2;
	if ( a == b ) return 0;
	if ( a >  b ) return 1;
	return -1;
}
int qsortB_dbl( const void *p1, const void *p2 ){
	double a,b;
	a = *(double*)p1;
	b = *(double*)p2;
	if ( b == a ) return 0;
	if ( b >  a ) return 1;
	return -1;
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
				qsort( (char*)dptr,   sizeA, sizeof(char)*sizeB,   qsortA_byte );
				break;
			case 16:
				qsort( (short*)dptr,  sizeA, sizeof(short)*sizeB,  qsortA_word );
				break;
			case 32:
				qsort( (int*)dptr,    sizeA, sizeof(int)*sizeB,    qsortA_long );
				break;
			case 64:
				qsort( (double*)dptr, sizeA, sizeof(double)*sizeB, qsortA_dbl );
				break;
			case 128:
				qsort( (complex*)dptr, sizeA, sizeof(complex)*sizeB, qsortA_cplx );
				break;
			default:
				{ CB_Error(DimensionERR); return ; }	// Dimension error
		}
	} else {	// sortD
		switch ( ElementSize ) {
			case 8:
				qsort( (char*)dptr,   sizeA, sizeof(char)*sizeB,   qsortB_byte );
				break;
			case 16:
				qsort( (short*)dptr,  sizeA, sizeof(short)*sizeB,  qsortB_word );
				break;
			case 32:
				qsort( (int*)dptr,    sizeA, sizeof(int)*sizeB,    qsortB_long );
				break;
			case 64:
				qsort( (double*)dptr, sizeA, sizeof(double)*sizeB, qsortB_dbl );
				break;
			case 128:
				qsort( (complex*)dptr, sizeA, sizeof(complex)*sizeB, qsortB_cplx );
				break;
			default:
				{ CB_Error(DimensionERR); return ; }	// Dimension error
		}
	}
}

void CB_SortAD( char *SRC, int flagAD) {	// SortA( List 1 [,List 2][,List 3]...) or 	// SortD( List 1 [,List 2][,List 3]...)
	int c,d,i;
	int reg,areg[8];
	int tmpreg=Mattmpreg;
	int m,n;
	int sizeA,sizeB;
	int ElementSize;
	int base;
	double	*dptr;

	c=SRC[ExecPtr];
	d=SRC[ExecPtr+1];
	if ( !( ( c == 0x7F ) && ( ( d == 0x51 ) || ( (0x6A<=d)&&(d<=0x6F) ) ) ) ) { CB_Error(SyntaxERR); return; }	// Syntax error
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

	if ( SRC[ExecPtr] != ',' ) { //	SortA( List 1 ) 
		qsortSub( dptr, ElementSize, sizeA, sizeB, flagAD );
	} else {					//	SortA( List 1 ,List 2 ) 
		i=0;
		areg[i++]=reg;
		while ( SRC[ExecPtr] == ',' ) {
			ExecPtr++;
			d=SRC[ExecPtr+1];
			if ( !( ( c == 0x7F ) && ( ( d == 0x51 ) || ( (0x6A<=d)&&(d<=0x6F) ) ) ) ) { CB_Error(SyntaxERR); return; }	// Syntax error
			ExecPtr+=2;
			reg=ListRegVar( SRC );
			if ( reg>=0 ) {
				if ( MatAry[reg].SizeA == 0 ) { CB_Error(DimensionERR); return ; }	// Dimension error
				if ( ( MatAry[reg].ElementSize != ElementSize ) || ( MatAry[reg].SizeA != sizeA ) || ( MatAry[reg].SizeB != sizeB ) || ( MatAry[reg].Base != base ) ) { CB_Error(ArgumentERR); return ; }	// Argument error
			} else { CB_Error(SyntaxERR); return; }	// Syntax error
			areg[i++]=reg;
			if ( i>8 ) { CB_Error(TooMuchData); }
			if ( ErrorNo ) return ;
		}
		List2Mat( &areg[0], tmpreg, i );
		if ( ErrorNo ) return ;
		sizeA        = MatAry[tmpreg ].SizeA;
		sizeB        = MatAry[tmpreg ].SizeB;
		base         = MatAry[tmpreg ].Base;
		ElementSize  = MatAry[tmpreg ].ElementSize;
		dptr         = MatAry[tmpreg ].Adrs;
		qsortSub( dptr, ElementSize, sizeA, sizeB, flagAD );

		n=base; i=0;
		do {
			for ( m=base ; m<sizeA +base ; m++ ) Cplx_WriteMatrix( areg[i] , m, base, Cplx_ReadMatrix( tmpreg , m, n ) );
			n++; i++;
		} while ( i < sizeB );
	}
	
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

complex CB_MinMax( char *SRC, int flag) {	// Min( List 1 )	flag  0:min  1:max
	int reg,reg2,reg3;
	int m,n;
	int sizeA,sizeB;
	int ElementSize;
	int base;
	complex min,tmp;
	int dspflagtmp=dspflag;
	
	ListEvalsub1(SRC);
	if ( dspflag < 3 ) { CB_Error(ArgumentERR); return Int2Cplx(0); } // Argument error
	reg=CB_MatListAnsreg;
	sizeA        = MatAry[reg ].SizeA;
	sizeB        = MatAry[reg ].SizeB;
	base         = MatAry[reg ].Base;
	
	if ( SRC[ExecPtr] == ',' ) { 
		ExecPtr++;
		ListEvalsub1(SRC);
		if ( dspflag < 3 ) { CB_Error(ArgumentERR); return Int2Cplx(0); } // Argument error
		reg2=CB_MatListAnsreg;
		if ( sizeA != MatAry[reg2].SizeA ) { CB_Error(DimensionERR); return Int2Cplx(0); }	// Dimension error
		for ( m=base; m<sizeA+base; m++) {
			min=Cplx_ReadMatrix( reg,  m, base ) ;
			tmp=Cplx_ReadMatrix( reg2, m, base ) ;
			if ( ( min.imag != 0 ) || ( tmp.imag != 0 ) ) {
				if ( flag )	{ 
					if ( fabsz(min) < fabsz(tmp) ) Cplx_WriteMatrix( reg,m, base , tmp ) ;	// max cplx
				} else {
					if ( fabsz(min) > fabsz(tmp) ) Cplx_WriteMatrix( reg,m, base , tmp ) ;	// min cplx
				}
			} else {
				if ( flag )	{ 
					if ( min.real < tmp.real )     Cplx_WriteMatrix( reg,m, base , tmp ) ;	// max
				} else {
					if ( min.real > tmp.real )     Cplx_WriteMatrix( reg,m, base , tmp ) ;	// max
				}
			}
		}
	} else {
		min = Cplx_ReadMatrix( reg, base, base ) ;
		for ( m=base; m<sizeA+base; m++) {
			tmp=Cplx_ReadMatrix( reg, m, base ) ;
			if ( ( min.imag != 0 ) || ( tmp.imag != 0 ) ) {
				if ( flag )	{
					if ( fabsz(min) < fabsz(tmp) ) min=tmp;	// max
				} else {
					if ( fabsz(min) > fabsz(tmp) ) min=tmp;	// min
				}
			} else {
				if ( flag )	{
					if ( min.real < tmp.real ) min=tmp;	// max
				} else {
					if ( min.real > tmp.real ) min=tmp;	// min
				}
			}
		}
		dspflag=dspflagtmp; 
	}
	DeleteMatListAns();
	if ( SRC[ExecPtr] == ')' ) ExecPtr++;
	return min;
}

complex CB_Mean( char *SRC ) {	// Mean( List 1 )
	int reg,reg2,reg3;
	int m,n;
	int sizeA,sizeB;
	int ElementSize;
	int base;
	complex result={0,0};
	int dspflagtmp=dspflag;
	
	ListEvalsub1(SRC);
	if ( dspflag < 3 ) { CB_Error(ArgumentERR); return Int2Cplx(0); } // Argument error
	reg=CB_MatListAnsreg;
	sizeA        = MatAry[reg ].SizeA;
	sizeB        = MatAry[reg ].SizeB;
	base         = MatAry[reg ].Base;
	
	for ( m=base; m<sizeA+base; m++) {
		result = Cplx_fADD( result, Cplx_ReadMatrix( reg, m, base ) );
	}
	
	DeleteMatListAns();
	dspflag=dspflagtmp; 
	if ( SRC[ExecPtr] == ')' ) ExecPtr++;
	return Cplx_fDIV( result, Int2Cplx(sizeA) ) ;
}

complex CB_Sum( char *SRC ) {	// Sum List 1 
	int reg;
	int m,n;
	int sizeA,sizeB;
	int ElementSize;
	int base;
	complex result;
	int dspflagtmp=dspflag;
	
	ListEvalsub1(SRC);
	if ( dspflag < 3 ) { CB_Error(ArgumentERR); return Int2Cplx(0); } // Argument error
	reg=CB_MatListAnsreg;

	sizeA        = MatAry[reg ].SizeA;
	sizeB        = MatAry[reg ].SizeB;
	base         = MatAry[reg ].Base;

	result.real=0;
	result.imag=0;
	for ( m=base; m<sizeA+base; m++) {
		result = Cplx_fADD( result, Cplx_ReadMatrix( reg, m, base ) );
	}
	
	DeleteMatListAns();
	dspflag=dspflagtmp; 
	return result;
}
complex CB_Prod( char *SRC ) {	// Prod List 1 
	int reg;
	int m,n;
	int sizeA,sizeB;
	int ElementSize;
	int base;
	complex result;
	int dspflagtmp=dspflag;
	
	ListEvalsub1(SRC);
	if ( dspflag < 3 ) { CB_Error(ArgumentERR); return Int2Cplx(0); } // Argument error
	reg=CB_MatListAnsreg;

	sizeA        = MatAry[reg ].SizeA;
	sizeB        = MatAry[reg ].SizeB;
	base         = MatAry[reg ].Base;

	result.real=1;
	result.imag=0;
	for ( m=base; m<sizeA+base; m++) {
		result = Cplx_fMUL( result, Cplx_ReadMatrix( reg, m, base ) );
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
	int errflag=0;
	double data;
  restart:
	data=CB_EvalDbl( SRC );	// dummy read
	if ( ErrorNo == 0 ) errflag=0;
	if ( dspflag >= 3 ) { CB_Error(ArgumentERR); return ; } // Argument error
	if ( errflag ) if ( ErrorNo ) return ;	// fatal error
	errflag=ErrorNo;	// error?
	if ( SRC[ExecPtr] != ',' ) { CB_Error(SyntaxERR); return ; }  // Syntax error
	ExecPtr++;
	*fxreg=RegVarAliasEx(SRC);
	if ( ( SRC[ExecPtr] != ',' ) || ( (*fxreg)<0 ) ) { CB_Error(SyntaxERR); return; }  // Syntax error
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
		LocalDbl[*fxreg][0]=Dbl2Cplx(*start);
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
	complex data,databack;
	double start,end,step;
	int dataint,databackint;
	int startint,endint,stepint;
	int m,n;
	int base;
	int ElementSize;
	
	exptr=ExecPtr;
	SeqOprand( SRC, &fxreg, &start, &end, &step );
	if ( ErrorNo ) return ;

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
	LocalDbl[fxreg][0].real = start;
	while ( m < dimA+base ) {
		ExecPtr=exptr;
		data=CB_Cplx_EvalDbl( SRC );	//
		Cplx_WriteMatrix( reg, m, n, data);
		LocalDbl[fxreg][0].real += step;
		m++;
		if ( BreakCheck )if ( KeyScanDownAC() ) { KeyRecover(); BreakPtr=ExecPtr; return ; }	// [AC] break?
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
	*fxreg=RegVarAliasEx(SRC); 
	if ( ( SRC[ExecPtr] != ',' ) || ( (*fxreg)<0 ) ) { CB_Error(SyntaxERR); return; }  // Syntax error
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
	if ( ErrorNo ) return ;

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
		if ( BreakCheck )if ( KeyScanDownAC() ) { KeyRecover(); BreakPtr=ExecPtr; return ; }	// [AC] break?
	}
	LocalInt[fxreg][0]=databack;
	ExecPtr=exptr2;
	dspflag =4 ;	// List data
}


complex CB_Sigma( char *SRC ) { //	Sigma(X^2,X,1,10[,1])
	int c,d;
	int i;
	int fxreg,reg;
	int exptr,exptr2;
	complex data;
	double start,end,step;
	complex result;
	int errflag=0;
	int breakcount=BREAKCOUNT;
	
	exptr=ExecPtr;
  restart:
	SeqOprand( SRC, &fxreg, &start, &end, &step );
	if ( ErrorNo ) return Int2Cplx(0);

	exptr2=ExecPtr;
	LocalDbl[fxreg][0].real = start;
	result = Dbl2Cplx(0);
	if (CB_INT==0) {	// double
		while ( LocalDbl[fxreg][0].real <= end ) {
			ExecPtr=exptr;
			result.real += CB_EvalDbl( SRC );	//
			LocalDbl[fxreg][0].real += step;
			if ( breakcount == 0 ) {
				if ( BreakCheck )if ( KeyScanDownAC() ) { KeyRecover(); BreakPtr=ExecPtr; return Int2Cplx(0); }	// [AC] break?
				breakcount = 10;
			} else breakcount--;
		}
	} else {			// complex
		while ( LocalDbl[fxreg][0].real <= end ) {
			ExecPtr=exptr;
			result = Cplx_fADD( result, CB_Cplx_EvalDbl( SRC ) );	//
			LocalDbl[fxreg][0].real += step;
			if ( breakcount == 0 ) {
				if ( BreakCheck )if ( KeyScanDownAC() ) { KeyRecover(); BreakPtr=ExecPtr; return Int2Cplx(0); }	// [AC] break?
				breakcount = BREAKCOUNT;
			} else breakcount--;
		}
	}
	LocalDbl[fxreg][0].real = end;
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
	int breakcount=BREAKCOUNT;
	
	exptr=ExecPtr;
  restart:
	SeqOprandInt( SRC, &fxreg, &start, &end, &step );
	if ( ErrorNo ) return 0;

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
		result += CB_EvalInt( SRC );	//
		LocalInt[fxreg][0]+=step;
		if ( breakcount == 0 ) {
			if ( BreakCheck )if ( KeyScanDownAC() ) { KeyRecover(); BreakPtr=ExecPtr; return 0; }	// [AC] break?
			breakcount = BREAKCOUNT;
		} else breakcount--;
	}
	LocalInt[fxreg][0]=end;
	ExecPtr=exptr2;
	return result;
}


//-----------------------------------------------------------------------------
int CB_ListCmp( char *SRC ) { //	ListCmp( List1, n) or ListCmp( List,List2)
	complex value;
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
	value=Cplx_ListEvalsubTop(SRC);
	valueint=value.real;
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
		if ( ElementSize >= 64 ) {
			for ( m=base; m<sizeA+base; m++ ) {
				if ( Cplx_fcmpEQ( Cplx_ReadMatrix( reg, base, m ), value ).real ) return 1;
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
		if ( ElementSize < 64 ) {
			for ( m=0; m<sizeA; m++ ) {
				if ( ReadMatrixInt( reg,  base, m+base ) != ReadMatrixInt( reg2,  base2, m+base2 ) ) return 0;
			}
			return 1;
		} else {
			for ( m=0; m<sizeA; m++ ) {
				if ( Cplx_fcmpEQ( Cplx_ReadMatrix( reg,  base, m+base ), Cplx_ReadMatrix( reg2,  base2, m+base2 ) ).real==0 ) return 0;
			}
			return 1;
		}
	}
	return 0;
}

//-----------------------------------------------------------------------------
//	refer to https://hikalium.com/page/lectures/c/0004.md
/*
double determinant( double *m, int N)
{
    int x, y, i;
    double det = 1, r;
    double tmp;
 
    // 上三角行列に変換しつつ、対角成分の積を計算する。
    for(y = 0; y < N - 1; y++){
        if(m[y * N + y] == 0){
            // 対角成分が0だった場合は、その列の値が0でない行と交換する
            for(i = y + 1; i < N; i++){
                if(m[i * N + y] != 0){
                    break;
                }
            }
            if(i < N){
                for(x = 0; x < N; x++){
                    SWAP(m[i * N + x], m[y * N + x]);
                }
                // 列を交換したので行列式の値の符号は反転する。
                det = -det;
            }
        }
        for(i = y + 1; i < N; i++){
            r = m[i * N + y] / m[y * N + y];
            for(x = y; x < N; x++){
                m[i * N + x] -= r * m[y * N + x];
            }
        }
        det *= m[y * N + y];
    }
    det *= m[y * N + y];
 
    return det;
}
*/
double Mat_determinant( int reg ) {
    int x, y, i, N;
    double det = 1, r;
    double tmp;
 	int sizeA,sizeB;
	int ElementSize;
	int base;

	sizeA        = MatAry[reg ].SizeA;
	sizeB        = MatAry[reg ].SizeB;
	base         = MatAry[reg ].Base;
	ElementSize  = MatAry[reg ].ElementSize;

	N = sizeA +base ;
	
    for(y = base; y < N - 1; y++){
        if( ReadMatrix( reg, y, y ) == 0){
            for(i = y + 1; i < N; i++){
                if( ReadMatrix( reg, i, y ) != 0){
                    break;
                }
            }
            if(i < N){
                for(x = base ; x < N; x++){
					tmp = ReadMatrix( reg, i, x); 
					WriteMatrix( reg, i, x, ReadMatrix( reg, y, x) ) ;
					WriteMatrix( reg, y, x, tmp);
                }
                det = -det;
            }
        }
        for(i = y + 1; i < N; i++){
            r = ReadMatrix( reg, i, y ) / ReadMatrix( reg, y, y );
            for(x = y; x < N; x++){
                WriteMatrix( reg, i, x,  ReadMatrix( reg, i, x)- r * ReadMatrix( reg, y, x) );
            }
        }
        det *= ReadMatrix( reg, y, y );
    }
    det *= ReadMatrix( reg, y, y );
 
    return det;
}
complex Cplx_Mat_determinant( int reg ) {
	int x, y, i, N;
	complex det = {1,0}, r, z;
	complex tmp;
 	int sizeA,sizeB;
	int ElementSize;
	int base;

	sizeA		 = MatAry[reg ].SizeA;
	sizeB		 = MatAry[reg ].SizeB;
	base		 = MatAry[reg ].Base;
	ElementSize  = MatAry[reg ].ElementSize;

	N = sizeA +base ;
	
	for(y = base; y < N - 1; y++){
		z = Cplx_ReadMatrix( reg, y, y );
		if ( ( z.real == 0 ) && ( z.imag == 0 ) ) {
			for(i = y + 1; i < N; i++){
				z = Cplx_ReadMatrix( reg, i, y );
				if( ( z.real != 0 ) || ( z.imag != 0 ) ) {
					break;
				}
			}
			if (i < N){
				for(x = base ; x < N; x++){
					tmp = Cplx_ReadMatrix( reg, i, x); 
					Cplx_WriteMatrix( reg, i, x, Cplx_ReadMatrix( reg, y, x) ) ;
					Cplx_WriteMatrix( reg, y, x, tmp);
				}
				Cplx_fsign( det );
			} else { CB_Error(MathERR); return det; }
		}
		for(i = y + 1; i < N; i++){
			z = Cplx_ReadMatrix( reg, y, y );
			if ( ( z.real != 0 ) || ( z.imag != 0 ) ) r = Cplx_fDIV( Cplx_ReadMatrix( reg, i, y ), z ); else r=Int2Cplx(0);
			for(x = y; x < N; x++){
				Cplx_WriteMatrix( reg, i, x,  Cplx_fSUB( Cplx_ReadMatrix( reg, i, x), Cplx_fMUL( r,  Cplx_ReadMatrix( reg, y, x))) );
			}
		}
		det = Cplx_fMUL( det, Cplx_ReadMatrix( reg, y, y ) );
	}
	det = Cplx_fMUL( det, Cplx_ReadMatrix( reg, y, y ) );
 
	return det;
}
complex Cplx_CB_MatDet( char *SRC ) {	// Det Mat A	
	int reg,reg2;
	int sizeA,sizeB;
	int ElementSize;
	int base;
	int dspflagtmp=dspflag;
	complex result={0,0};

	MatrixOprandreg( SRC, &reg);
	if ( reg>=0 ) {
		if ( MatAry[reg].SizeA == 0 ) { CB_Error(NoMatrixArrayERR); return Int2Cplx(0); }	// No Matrix Array error
		NewMatListAns( MatAry[reg].SizeA, MatAry[reg].SizeB, MatAry[reg].Base, MatAry[reg].ElementSize );
		if ( ErrorNo ) return Int2Cplx(0);
		reg2=reg;
		reg=CB_MatListAnsreg;
		CopyMatrix( reg, reg2 );	// reg2->reg
	} else {	ErrorNo=0;	// error cancel
		Cplx_ListEvalsub1(SRC);
		if ( dspflag != 3 ) { CB_Error(ArgumentERR); return Int2Cplx(0); } // Argument error
		reg = CB_MatListAnsreg;
	}

	sizeA        = MatAry[reg ].SizeA;
	sizeB        = MatAry[reg ].SizeB;
	base         = MatAry[reg ].Base;
	ElementSize  = MatAry[reg ].ElementSize;
	if ( ElementSize  == 2 ) ElementSize  == 1;

	if ( sizeA != sizeB ) { CB_Error(DimensionERR); return Int2Cplx(0); }	// Dimension error
	if ( CB_INT==2 )  result      = Cplx_Mat_determinant( reg );
	else			  result.real = Mat_determinant( reg );

	DeleteMatListAns();
	dspflag=dspflagtmp; 
	return result;
}


//
// refer to http://www.yamamo10.jp/yamamoto/lecture/2004/5E/linear_equations/how_to_make_GJ/html/node2.html

void Mat_inverse( int ansreg ) {
	int x,y,i,k,N,flag=0,x1,x2;
	int sizeA,sizeB;
	int ElementSize;
	int base;
	int reg,tmpreg=Mattmpreg;
	int pv[256],pvrow;
	double a,b,c,d;
	double z,tmp,tmp2,tmp3;
	base    = MatAry[ansreg ].Base;
	sizeA   = MatAry[ansreg ].SizeA;
	N = sizeA +base ;

	ErrorNo=0;
	if ( ( sizeA > 255 ) || ( sizeA == 0 ) || ( sizeA != MatAry[ansreg].SizeB ) ) { CB_Error(DimensionERR); return ; }	// Dimension error
/*
	if ( sizeA==1 ) {
		tmp = frecip( ReadMatrix( ansreg, base+0, base+0 ) );
		if ( ErrorNo ) return ;
		WriteMatrix( ansreg, base+0, base+0, tmp );
		return ;
	}
	if ( sizeA==2 ) {
		a =  ReadMatrix( ansreg, base+0, base+0 );
		b =  ReadMatrix( ansreg, base+0, base+1 );
		c =  ReadMatrix( ansreg, base+1, base+0 );
		d =  ReadMatrix( ansreg, base+1, base+1 );
		tmp = frecip( a*d + b*c );
		if ( ErrorNo ) return ;
		WriteMatrix( ansreg, base+0, base+0, d*tmp );
		WriteMatrix( ansreg, base+0, base+1, fsign(b)*tmp );
		WriteMatrix( ansreg, base+1, base+0, fsign(c)*tmp );
		WriteMatrix( ansreg, base+1, base+1, a*tmp );
		return ;
	}
*/
	for ( y=base; y<N; y++ ){
		b=0.0; pvrow = y;
		for(i = y ; i < N; i++){
			z = fabs(ReadMatrix( ansreg, i, y ));
			if ( z > b ) {
				b = z;
				pvrow = i;
			}
		}
		if ( b==0.0 ) { CB_Error(MathERR); return ; }
		pv[y] = pvrow;
		
		if ( y != pvrow ) {
			for(x = base ; x < N; x++){
				tmp = ReadMatrix( ansreg, y, x); 
				WriteMatrix( ansreg, y, x, ReadMatrix( ansreg, pvrow, x) ) ;
				WriteMatrix( ansreg, pvrow, x, tmp);
			}
		}

		tmp = frecip( ReadMatrix( ansreg, y, y ) );
		WriteMatrix( ansreg, y, y, 1.0 );
		for ( x=base; x<N; x++ ) {
			tmp2 = ReadMatrix( ansreg, y, x );	WriteMatrix( ansreg, y, x, tmp*tmp2 );
		}
		
		for ( x=base; x<N; x++ ){
			if ( y != x ) {
				tmp = ReadMatrix( ansreg, x, y );
				WriteMatrix( ansreg, x, y, 0.0 );
				for ( k=base; k<N; k++ ) {
					tmp2 = ReadMatrix( ansreg, y, k );	tmp3 = ReadMatrix( ansreg, x, k );	WriteMatrix( ansreg, x, k, tmp3-tmp*tmp2 );
				}
			}
		}
	}
	for ( i = N-1; i>=base;  i--){
		if ( i != pv[i] ) {
			for(y = base ; y < N; y++){
				tmp = ReadMatrix( ansreg, y, i); 
				WriteMatrix( ansreg, y, i, ReadMatrix( ansreg, y, pv[i]) ) ;
				WriteMatrix( ansreg, y, pv[i], tmp);
			}
		}
	}
}

void Cplx_Mat_inverse( int ansreg ) {
	int x,y,i,k,N,flag=0,x1,x2;
	int sizeA,sizeB;
	int ElementSize;
	int base;
	int reg,tmpreg=Mattmpreg;
	int pv[256],pvrow;
	complex a,b,c,d;
	complex k0={0,0},k1={1,0};
	complex z,tmp,tmp2,tmp3;
	base    = MatAry[ansreg ].Base;
	sizeA   = MatAry[ansreg ].SizeA;
	N = sizeA +base ;

	ErrorNo=0;
	if ( ( sizeA > 255 ) || ( sizeA == 0 ) || ( sizeA != MatAry[ansreg].SizeB ) ) { CB_Error(DimensionERR); return ; }	// Dimension error
/*
	if ( sizeA==1 ) {
		tmp = Cplx_frecip( Cplx_ReadMatrix( ansreg, base+0, base+0 ) );
		if ( ErrorNo ) return ;
		Cplx_WriteMatrix( ansreg, base+0, base+0, tmp );
		return ;
	}
	if ( sizeA==2 ) {
		a =  Cplx_ReadMatrix( ansreg, base+0, base+0 );
		b =  Cplx_ReadMatrix( ansreg, base+0, base+1 );
		c =  Cplx_ReadMatrix( ansreg, base+1, base+0 );
		d =  Cplx_ReadMatrix( ansreg, base+1, base+1 );
		tmp = Cplx_frecip( Cplx_fSUB( Cplx_fMUL(a,d), Cplx_fMUL(b,c) ) );
		if ( ErrorNo ) return ;
		Cplx_WriteMatrix( ansreg, base+0, base+0, Cplx_fMUL( d, tmp ) );
		Cplx_WriteMatrix( ansreg, base+0, base+1, Cplx_fMUL( Cplx_fsign(b), tmp ) );
		Cplx_WriteMatrix( ansreg, base+1, base+0, Cplx_fMUL( Cplx_fsign(c), tmp ) );
		Cplx_WriteMatrix( ansreg, base+1, base+1, Cplx_fMUL( a, tmp ) );
		return ;
	}
*/
	for ( y=base; y<N; y++ ){
		b.real=0.0; pvrow = y;
		for(i = y ; i < N; i++){
			z.real = Cplx_fabs( Cplx_ReadMatrix( ansreg, i, y ) ).real;
			if ( z.real > b.real ) {
				b.real = z.real;
				pvrow = i;
			}
		}
		if ( b.real==0.0 ) { CB_Error(MathERR); return ; }
		pv[y] = pvrow;
		
		if ( y != pvrow ) {
			for(x = base ; x < N; x++){
				tmp = Cplx_ReadMatrix( ansreg, pvrow, x); 
				Cplx_WriteMatrix( ansreg, pvrow, x, Cplx_ReadMatrix( ansreg, y, x) ) ;
				Cplx_WriteMatrix( ansreg, y,     x, tmp);
			}
		}
		
		tmp = Cplx_frecip( Cplx_ReadMatrix( ansreg, y, y ) );
		Cplx_WriteMatrix( ansreg, y, y, k1 );
		for ( x=base; x<N; x++ ) {
			tmp2 = Cplx_ReadMatrix( ansreg, y, x );	Cplx_WriteMatrix( ansreg, y, x, Cplx_fMUL( tmp, tmp2 ) );
		}
		for ( x=base; x<N; x++ ){
			if ( y != x ) {
				tmp = Cplx_ReadMatrix( ansreg, x, y );
				Cplx_WriteMatrix( ansreg, x, y, k0 );
				for ( k=base; k<N; k++ ) {
					tmp2 = Cplx_ReadMatrix( ansreg, y, k );	tmp3 = Cplx_ReadMatrix( ansreg, x, k );	Cplx_WriteMatrix( ansreg, x, k, Cplx_fSUB( tmp3, Cplx_fMUL( tmp, tmp2 ) ) );
				}
			}
		}
	}
	for ( i = N-1; i>=base;  i--){
		if ( i != pv[i] ) {
			for(y = base ; y < N; y++){
				tmp = Cplx_ReadMatrix( ansreg, y, i); 
				Cplx_WriteMatrix( ansreg, y, i, Cplx_ReadMatrix( ansreg, y, pv[i]) ) ;
				Cplx_WriteMatrix( ansreg, y, pv[i], tmp);
			}
		}
	}
}
/*
void Cplx_CB_MatInv( char *SRC ) {	// Inverse Mat A	
	int reg,reg2;
	int sizeA,sizeB;
	int ElementSize;
	int base;
	int dspflagtmp=dspflag;
	complex result;

	MatrixOprandreg( SRC, &reg);
	if ( reg>=0 ) {
		if ( MatAry[reg].SizeA == 0 ) { CB_Error(NoMatrixArrayERR); return ; }	// No Matrix Array error
	} else {	ErrorNo=0;	// error cancel
		ListEvalsubTop(SRC);
		if ( dspflag != 3 ) { CB_Error(ArgumentERR); return ; } // Argument error
		reg = CB_MatListAnsreg;
	}
	if ( MatAry[reg].SizeA != MatAry[reg].SizeB ) { CB_Error(DimensionERR); return ; }	// Dimension error

	Cplx_Mat_inverse( reg );
	dspflag=3; 
}
*/

//-----------------------------------------------------------------------------
//	Vector
//-----------------------------------------------------------------------------
int VectorNormInt( int reg ) {
	int m,n,i;
	int sizeA,sizeB;
	int ElementSize;
	int base;
	int result=0,tmp;
	
	sizeA        = MatAry[reg ].SizeA;
	sizeB        = MatAry[reg ].SizeB;
	base         = MatAry[reg ].Base;

	for ( m=base; m<sizeA+base; m++ ) {
		for ( n=base; n<sizeB+base; n++ ) {
			tmp = ReadMatrixInt( reg, m, n);
			result += tmp*tmp ;
		}
	}
	return fsqrtint(result);
}
double VectorNorm( int reg ) {
	int m,n,i;
	int sizeA,sizeB;
	int ElementSize;
	int base;
	double result=0,tmp;
	
	sizeA        = MatAry[reg ].SizeA;
	sizeB        = MatAry[reg ].SizeB;
	base         = MatAry[reg ].Base;

	for ( m=base; m<sizeA+base; m++ ) {
		for ( n=base; n<sizeB+base; n++ ) {
			tmp = ReadMatrix( reg, m, n);
			result += tmp*tmp ;
		}
	}
	return fsqrt(result);
}
double Cplx_VectorNorm( int reg ) {
	int m,n,i;
	int sizeA,sizeB;
	int ElementSize;
	int base;
	double result=0;
	complex tmp;
	
	sizeA        = MatAry[reg ].SizeA;
	sizeB        = MatAry[reg ].SizeB;
	base         = MatAry[reg ].Base;

	for ( m=base; m<sizeA+base; m++ ) {
		for ( n=base; n<sizeB+base; n++ ) {
			tmp = Cplx_ReadMatrix( reg, m, n);
			result += tmp.real*tmp.real + tmp.imag*tmp.imag ;
		}
	}
	return fsqrt(result);
}

int VectorDotPInt( int reg, int reg2 ) {
	int m,n,i;
	int m2,n2;
	int size,sizeA,sizeB,sizeA2,sizeB2,sizeB3;
	int ElementSize,ElementSize2;
	int base,base2;
	int result=0;
	
	sizeA        = MatAry[reg ].SizeA;
	sizeA2       = MatAry[reg2].SizeA;
	sizeB        = MatAry[reg ].SizeB;
	sizeB2       = MatAry[reg2].SizeB;
	base         = MatAry[reg ].Base;
	base2        = MatAry[reg2].Base;

	if ( sizeA == 1 ) {
		if ( sizeA2 == 1 ) {
			if ( sizeB != sizeB2 ) { CB_Error(DimensionERR); return 0; }	// Dimension error
		} else
		if ( sizeB2 == 1 ) {
			if ( sizeB != sizeA2 ) { CB_Error(DimensionERR); return 0; }	// Dimension error
		} else { CB_Error(DimensionERR); return 0; }	// Dimension error
		size=sizeB;
	} else 
	if ( sizeB == 1 ) {
		if ( sizeA2 == 1 ) {
			if ( sizeA != sizeB2 ) { CB_Error(DimensionERR); return 0; }	// Dimension error
		} else
		if ( sizeB2 == 1 ) {
			if ( sizeA != sizeA2 ) { CB_Error(DimensionERR); return 0; }	// Dimension error
		} else { CB_Error(DimensionERR); return 0; }	// Dimension error
		size=sizeA;
	} else { CB_Error(DimensionERR); return 0; }	// Dimension error

	n=base; n2=base2;
	for ( i=0; i<size; i++ ) result += ReadMatrixInt( reg, base, n++) * ReadMatrixInt( reg2, base2, n2++) ;

	return result;
}
double VectorDotP( int reg, int reg2 ) {
	int m,n,i;
	int m2,n2;
	int size,sizeA,sizeB,sizeA2,sizeB2,sizeB3;
	int ElementSize,ElementSize2;
	int base,base2;
	double result=0;
	
	sizeA        = MatAry[reg ].SizeA;
	sizeA2       = MatAry[reg2].SizeA;
	sizeB        = MatAry[reg ].SizeB;
	sizeB2       = MatAry[reg2].SizeB;
	base         = MatAry[reg ].Base;
	base2        = MatAry[reg2].Base;

	if ( sizeA == 1 ) {
		if ( sizeA2 == 1 ) {
			if ( sizeB != sizeB2 ) { CB_Error(DimensionERR); return 0; }	// Dimension error
		} else
		if ( sizeB2 == 1 ) {
			if ( sizeB != sizeA2 ) { CB_Error(DimensionERR); return 0; }	// Dimension error
		} else { CB_Error(DimensionERR); return 0; }	// Dimension error
		size=sizeB;
	} else 
	if ( sizeB == 1 ) {
		if ( sizeA2 == 1 ) {
			if ( sizeA != sizeB2 ) { CB_Error(DimensionERR); return 0; }	// Dimension error
		} else
		if ( sizeB2 == 1 ) {
			if ( sizeA != sizeA2 ) { CB_Error(DimensionERR); return 0; }	// Dimension error
		} else { CB_Error(DimensionERR); return 0; }	// Dimension error
		size=sizeA;
	} else { CB_Error(DimensionERR); return 0; }	// Dimension error

	n=base; n2=base2;
	for ( i=0; i<size; i++ ) result += ReadMatrix( reg, base, n++) * ReadMatrix( reg2, base2, n2++) ;

	return result;
}
complex Cplx_VectorDotP( int reg, int reg2 ) {
	int m,n,i;
	int m2,n2;
	int size,sizeA,sizeB,sizeA2,sizeB2,sizeB3;
	int ElementSize,ElementSize2;
	int base,base2;
	complex result={0,0};

	sizeA        = MatAry[reg ].SizeA;
	sizeA2       = MatAry[reg2].SizeA;
	sizeB        = MatAry[reg ].SizeB;
	sizeB2       = MatAry[reg2].SizeB;
	base         = MatAry[reg ].Base;
	base2        = MatAry[reg2].Base;

	if ( sizeA == 1 ) {
		if ( sizeA2 == 1 ) {
			if ( sizeB != sizeB2 ) { CB_Error(DimensionERR); return Int2Cplx(0); }	// Dimension error
		} else
		if ( sizeB2 == 1 ) {
			if ( sizeB != sizeA2 ) { CB_Error(DimensionERR); return Int2Cplx(0); }	// Dimension error
		} else { CB_Error(DimensionERR); return Int2Cplx(0); }	// Dimension error
		size=sizeB;
	} else 
	if ( sizeB == 1 ) {
		if ( sizeA2 == 1 ) {
			if ( sizeA != sizeB2 ) { CB_Error(DimensionERR); return Int2Cplx(0); }	// Dimension error
		} else
		if ( sizeB2 == 1 ) {
			if ( sizeA != sizeA2 ) { CB_Error(DimensionERR); return Int2Cplx(0); }	// Dimension error
		} else { CB_Error(DimensionERR); return Int2Cplx(0); }	// Dimension error
		size=sizeA;
	} else { CB_Error(DimensionERR); return Int2Cplx(0); }	// Dimension error

	n=base; n2=base2;
	for ( i=0; i<size; i++ ) result = Cplx_fADD( result, Cplx_fMUL( Cplx_ReadMatrix( reg, base, n++), Cplx_fconjg( Cplx_ReadMatrix( reg2, base2, n2++) ) ) );
	

	return result;
}

int GetVctArg1_Int( char *SRC, int *reg ){
	ListEvalIntsubTop(SRC);
	*reg = CB_MatListAnsreg;
	if ( dspflag < 3 )  { CB_Error(ArgumentERR); return 1; } // Argument error
	if ( MatAry[*reg].SizeA == 0 ) { CB_Error(DimensionERR); return 1; }	// Dimension error
	return 0;
}
int GetVctArg2_Int( char *SRC, int *reg, int *reg2 ){
	if ( GetVctArg1_Int( SRC, &(*reg) ) ) return 1;
	if ( SRC[ExecPtr] != ',' ) { CB_Error(SyntaxERR); return 1; }  // Syntax error
	ExecPtr++;
	if ( GetVctArg1_Int( SRC, &(*reg2) ) ) return 1;
	return 0;
}
int GetVctArg1_Dbl( char *SRC, int *reg ){
	ListEvalsubTop(SRC);
	*reg = CB_MatListAnsreg;
	if ( dspflag < 3 )  { CB_Error(ArgumentERR); return 1; } // Argument error
	if ( MatAry[*reg].SizeA == 0 ) { CB_Error(DimensionERR); return 1; }	// Dimension error
	return 0;
}
int GetVctArg2_Dbl( char *SRC, int *reg, int *reg2 ){
	if ( GetVctArg1_Dbl( SRC, &(*reg) ) ) return 1;
	if ( SRC[ExecPtr] != ',' ) { CB_Error(SyntaxERR); return 1; }  // Syntax error
	ExecPtr++;
	if ( GetVctArg1_Dbl( SRC, &(*reg2) ) ) return 1;
	return 0;
}
int GetVctArg1_Cplx( char *SRC, int *reg ){
	Cplx_ListEvalsubTop(SRC);
	*reg = CB_MatListAnsreg;
	if ( dspflag < 3 )  { CB_Error(ArgumentERR); return 1; } // Argument error
	if ( MatAry[*reg].SizeA == 0 ) { CB_Error(DimensionERR); return 1; }	// Dimension error
	return 0;
}
int GetVctArg2_Cplx( char *SRC, int *reg, int *reg2 ){
	if ( GetVctArg1_Cplx( SRC, &(*reg) ) ) return 1;
	if ( SRC[ExecPtr] != ',' ) { CB_Error(SyntaxERR); return 1; }  // Syntax error
	ExecPtr++;
	if ( GetVctArg1_Cplx( SRC, &(*reg2) ) ) return 1;
	return 0;
}


int CB_DotPInt( char *SRC ){	// DotP(Vct A,Vct B)
	int reg,reg2;
	int ansreg=CB_MatListAnsreg;
	int result=0;

	if ( GetVctArg2_Int( SRC, &reg, &reg2 ) ) return 0; // Argument error
	
	result = VectorDotPInt( reg, reg2 );

	if ( SRC[ExecPtr] == ')' ) ExecPtr++;
	CB_MatListAnsreg=ansreg;
	dspflag = 2;
	return result;
}

double CB_DotP( char *SRC ){	// DotP(Vct A,Vct B)
	int reg,reg2;
	int ansreg=CB_MatListAnsreg;
	double result=0;

	if ( GetVctArg2_Dbl( SRC, &reg, &reg2 ) ) return 0; // Argument error
	
	result = VectorDotP( reg, reg2 );

	if ( SRC[ExecPtr] == ')' ) ExecPtr++;
	CB_MatListAnsreg=ansreg;
	dspflag = 2;
	return result;
}

complex Cplx_CB_DotP( char *SRC ){	// DotP(Vct A,Vct B)
	int reg,reg2;
	int ansreg=CB_MatListAnsreg;
	complex result={0,0};

	if ( GetVctArg2_Cplx( SRC, &reg, &reg2 ) ) return Int2Cplx(0); // Argument error

	result = Cplx_VectorDotP( reg, reg2 );

	if ( SRC[ExecPtr] == ')' ) ExecPtr++;
	CB_MatListAnsreg=ansreg;
	dspflag = 2;
	return result;
}

void CB_CrossP( char *SRC ){	// CrossP(Vct A,Vct B)
	int reg,reg2;
	int m,n,i;
	int m2,n2;
	int sizeA,sizeB,sizeA2,sizeB2,sizeB3;
	int ElementSize,ElementSize2;
	int base,base2;
	int tmpreg=Mattmpreg;
	int ansreg=CB_MatListAnsreg;
	double x[4];
	double y[4];

	if ( CB_INT==1 ) {
		if ( GetVctArg2_Int( SRC, &reg, &reg2 ) ) return ; // Argument error
	} else {
		if ( GetVctArg2_Dbl( SRC, &reg, &reg2 ) ) return ; // Argument error
	}

	sizeA        = MatAry[reg ].SizeA;
	sizeA2       = MatAry[reg2].SizeA;
	sizeB        = MatAry[reg ].SizeB; if ( sizeB >sizeA  ) { i=sizeA ; sizeA =sizeB ; sizeB =i; }
	sizeB2       = MatAry[reg2].SizeB; if ( sizeB2>sizeA2 ) { i=sizeA2; sizeA2=sizeB2; sizeB2=i; }
	base         = MatAry[reg ].Base;
	base2        = MatAry[reg2].Base;

	if ( ( sizeA < 2 ) || ( sizeA2 > 3 ) || ( sizeA != sizeA2 ) ) { CB_Error(DimensionERR); return ; }	// Dimension error
	
	sizeB3 = 3 ;
	ElementSize = 64;
	DimMatrixSub( tmpreg, ElementSize, 1, sizeB3, 1);	//
	if ( ErrorNo ) return ;

	x[3] = 0;
	y[3] = 0;
	for ( i=1; i<=sizeA; i++){
		x[i] = ReadMatrix( reg , base , base +i-1 );
		y[i] = ReadMatrix( reg2, base2, base2+i-1 );
	}

	WriteMatrix( tmpreg, 1, 1, x[2]*y[3]-x[3]*y[2] );
	WriteMatrix( tmpreg, 1, 2, x[3]*y[1]-x[1]*y[3] );
	WriteMatrix( tmpreg, 1, 3, x[1]*y[2]-x[2]*y[1] );

	if ( SRC[ExecPtr] == ')' ) ExecPtr++;
	CB_MatListAnsreg=ansreg;
	CopyMatList2Ans( tmpreg );		// tmpreg -> CB_MatListAnsreg
	dspflag = 3;
}

void Cplx_CB_CrossP( char *SRC ){	// CrossP(Vct A,Vct B)
	int reg,reg2;
	int m,n,i;
	int m2,n2;
	int sizeA,sizeB,sizeA2,sizeB2,sizeB3;
	int ElementSize,ElementSize2;
	int base,base2;
	int tmpreg=Mattmpreg;
	int ansreg=CB_MatListAnsreg;
	complex x[4];
	complex y[4];

	if ( GetVctArg2_Cplx( SRC, &reg, &reg2 ) ) return ; // Argument error

	sizeA        = MatAry[reg ].SizeA;
	sizeA2       = MatAry[reg2].SizeA;
	sizeB        = MatAry[reg ].SizeB; if ( sizeB >sizeA  ) { i=sizeA ; sizeA =sizeB ; sizeB =i; }
	sizeB2       = MatAry[reg2].SizeB; if ( sizeB2>sizeA2 ) { i=sizeA2; sizeA2=sizeB2; sizeB2=i; }
	base         = MatAry[reg ].Base;
	base2        = MatAry[reg2].Base;

	if ( ( sizeA < 2 ) || ( sizeA2 > 3 ) || ( sizeA != sizeA2 ) ) { CB_Error(DimensionERR); return ; }	// Dimension error
	
	sizeB3 = 3 ;
	ElementSize = 128;
	DimMatrixSub( tmpreg, ElementSize, 1, sizeB3, 1);	//
	if ( ErrorNo ) return ;

	x[3] = Int2Cplx(0);
	y[3] = Int2Cplx(0);
	for ( i=1; i<=sizeA; i++){
		x[i] = Cplx_ReadMatrix( reg , base , base +i-1 );
		y[i] = Cplx_ReadMatrix( reg2, base2, base2+i-1 );
	}

	Cplx_WriteMatrix( tmpreg, 1, 1, Cplx_fSUB( Cplx_fMUL(x[2],y[3]), Cplx_fMUL(x[3],y[2]) ) );
	Cplx_WriteMatrix( tmpreg, 1, 2, Cplx_fSUB( Cplx_fMUL(x[3],y[1]), Cplx_fMUL(x[1],y[3]) ) );
	Cplx_WriteMatrix( tmpreg, 1, 3, Cplx_fSUB( Cplx_fMUL(x[1],y[2]), Cplx_fMUL(x[2],y[1]) ) );
	
	if ( SRC[ExecPtr] == ')' ) ExecPtr++;
	CB_MatListAnsreg=ansreg;
	CopyMatList2Ans( tmpreg );		// tmpreg -> CB_MatListAnsreg
	dspflag = 3;
}

double CB_AngleV( char *SRC ){	// Angle(Vct A,Vct B)
	int reg,reg2;
	int tmpreg=Mattmpreg;
	int ansreg=CB_MatListAnsreg;
	double result=0;

	if ( CB_INT==1 ) {
		if ( GetVctArg2_Int( SRC, &reg, &reg2 ) ) return 0; // Argument error
	} else {
		if ( GetVctArg2_Dbl( SRC, &reg, &reg2 ) ) return 0; // Argument error
	}

	result = facos( VectorDotP( reg, reg2 ) / ( VectorNorm( reg ) * VectorNorm( reg2 ) ) );

	if ( SRC[ExecPtr] == ')' ) ExecPtr++;
	CB_MatListAnsreg=ansreg;
	dspflag = 2;
	return result;
}

complex Cplx_CB_AngleV( char *SRC ){	// Angle(Vct A,Vct B)
	int reg,reg2;
	int tmpreg=Mattmpreg;
	int ansreg=CB_MatListAnsreg;
	complex result={0,0};

	if ( GetVctArg2_Cplx( SRC, &reg, &reg2 ) ) return Int2Cplx(0); // Argument error

	result = Cplx_facos( Cplx_fDIV( Cplx_VectorDotP( reg, reg2 ), Dbl2Cplx( Cplx_VectorNorm( reg )*Cplx_VectorNorm( reg2 ) ) ) );

	if ( SRC[ExecPtr] == ')' ) ExecPtr++;
	CB_MatListAnsreg=ansreg;
	dspflag = 2;
	return result;
}

void CB_UnitV( char *SRC ){	// UnitV(Vct A)
	int reg;
	int m,n,i;
	int m2,n2;
	int sizeA,sizeB;
	int ElementSize;
	int base;
	int tmpreg=Mattmpreg;
	int ansreg=CB_MatListAnsreg;
	double result=0,length;
	double tmp;

	if ( CB_INT==1 ) {
		if ( GetVctArg1_Int( SRC, &reg ) ) return ; // Argument error
	} else {
		if ( GetVctArg1_Dbl( SRC, &reg ) ) return ; // Argument error
	}

	if ( CB_INT==1 ) length = VectorNormInt( reg ); else length = VectorNorm( reg );

	sizeA        = MatAry[reg ].SizeA;
	sizeB        = MatAry[reg ].SizeB;
	base         = MatAry[reg ].Base;
	
	ElementSize = MatAry[reg ].ElementSize;
	DimMatrixSub( tmpreg, ElementSize, sizeA, sizeB, base );	//
	if ( ErrorNo ) return ;

	if ( sizeA == 1 ) {
			n=base;;
			for ( i=0; i<sizeB; i++ ) {
				tmp = ReadMatrix( reg, base, n)/length;
				WriteMatrix( tmpreg, base, n++, tmp);
			}
	} else 
	if ( sizeB == 1 ) {
			m=base;;
			for ( i=0; i<sizeA; i++ ) {
				tmp = ReadMatrix( reg, m, base)/length;
				WriteMatrix( tmpreg, m++, base, tmp);
			}
	} else { CB_Error(DimensionERR); return ; }	// Dimension error
	

	if ( SRC[ExecPtr] == ')' ) ExecPtr++;
	CB_MatListAnsreg=ansreg;
	CopyMatList2Ans( tmpreg );		// tmpreg -> CB_MatListAnsreg
	dspflag = 3;
}

void Cplx_CB_UnitV( char *SRC ){	// UnitV(Vct A)
	int reg;
	int m,n,i;
	int m2,n2;
	int sizeA,sizeB;
	int ElementSize;
	int base;
	int tmpreg=Mattmpreg;
	int ansreg=CB_MatListAnsreg;
	complex length={0,0};
	complex tmp;

	if ( GetVctArg1_Cplx( SRC, &reg ) ) return ; // Argument error

	length.real = Cplx_VectorNorm( reg );
	if ( length.real==0 )  { CB_Error(MathERR); return ; } // Math error

	sizeA        = MatAry[reg ].SizeA;
	sizeB        = MatAry[reg ].SizeB;
	base         = MatAry[reg ].Base;
	
	ElementSize = MatAry[reg ].ElementSize;
	DimMatrixSub( tmpreg, ElementSize, sizeA, sizeB, base );	//
	if ( ErrorNo ) return ;

	if ( sizeA == 1 ) {
			n=base;;
			for ( i=0; i<sizeB; i++ ) {
				tmp = Cplx_fDIV( Cplx_ReadMatrix( reg, base, n), length );
				Cplx_WriteMatrix( tmpreg, base, n++, tmp);
			}
	} else 
	if ( sizeB == 1 ) {
			m=base;;
			for ( i=0; i<sizeA; i++ ) {
				tmp = Cplx_fDIV( Cplx_ReadMatrix( reg, m, base), length );
				Cplx_WriteMatrix( tmpreg, m++, base, tmp);
			}
	} else { CB_Error(DimensionERR); return ; }	// Dimension error
	

	if ( SRC[ExecPtr] == ')' ) ExecPtr++;
	CB_MatListAnsreg=ansreg;
	CopyMatList2Ans( tmpreg );		// tmpreg -> CB_MatListAnsreg
	dspflag = 3;
}

int CB_NormVInt( char *SRC ){	// Norm(Vct A)
	int reg;
	int ansreg=CB_MatListAnsreg;
	int result;

	if ( GetVctArg1_Int( SRC, &reg ) ) return 0; // Argument error

	result = VectorNormInt( reg );

	if ( SRC[ExecPtr] == ')' ) ExecPtr++;
	CB_MatListAnsreg=ansreg;
	dspflag = 2;
	return result;
}
double CB_NormV( char *SRC ){	// Norm(Vct A)
	int reg;
	int ansreg=CB_MatListAnsreg;
	double result;

	if ( GetVctArg1_Dbl( SRC, &reg ) ) return 0; // Argument error

	result = VectorNorm( reg );

	if ( SRC[ExecPtr] == ')' ) ExecPtr++;
	CB_MatListAnsreg=ansreg;
	dspflag = 2;
	return result;
}
complex Cplx_CB_NormV( char *SRC ){	// Norm(Vct A)
	int reg;
	int ansreg=CB_MatListAnsreg;
	complex result={0,0};

	if ( GetVctArg1_Cplx( SRC, &reg ) ) return Int2Cplx(0); // Argument error

	result.real = Cplx_VectorNorm( reg );

	if ( SRC[ExecPtr] == ')' ) ExecPtr++;
	CB_MatListAnsreg=ansreg;
	dspflag = 2;
	return result;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
int CB_Dim( char *SRC ){	// Dim Mat or Dim List
	int reg,c;
	if ( SRC[ExecPtr]==0x7F ) {
		if ( ( SRC[ExecPtr+1]==0x40 ) || ( SRC[ExecPtr+1]==0xFFFFFF84 ) ) {	// Dim Mat or Vct
			MatrixOprandreg( SRC, &reg );
			WriteListAns2( MatAry[reg].SizeA, MatAry[reg].SizeB );
			return MatAry[reg].SizeA;
		} else { c=SRC[ExecPtr+1];
			if ( ( c == 0x51 ) || ( (0x6A<=c)&&(c<=0x6F) ) ) {	// Dim List or List1~List6
				MatrixOprandreg( SRC, &reg );
				return MatAry[reg].SizeA;
			}
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
	if (i < 4 ) i=1;
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


void CB_ListNo2Ptr( int n ){
	if ( n > 1 ) { 
		ListFilePtr = (110)+(n-2)*52;
	} else {
		ListFilePtr = 0;
	}
}
void CB_ListFile( char *SRC ){
	int n, c = SRC[ExecPtr-1];
	if ( ( 0xFFFFFFB8 <= c )&& ( c <= 0xFFFFFFBD ) ) { n = c-0xFFFFFFB7; }	// File1~File6
	else n = CB_EvalInt( SRC );
	if ( ( n<0 ) || ( ExtendList+1<n ) ) { CB_Error(ArgumentERR); return ; } // Argument error
	ListFileNo = n;
	n--;
	if ( ExtendList < n) { CB_Error(MemoryERR); return ; } // memory error
	CB_ListNo2Ptr( ListFileNo );
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
int MatrixObjectAlign4M1( unsigned int n ){ return n; }	// align +4byte
int MatrixObjectAlign4M2( unsigned int n ){ return n; }	// align +4byte
int MatrixObjectAlign4M3( unsigned int n ){ return n; }	// align +4byte
int MatrixObjectAlign4M4( unsigned int n ){ return n; }	// align +4byte
int MatrixObjectAlign4M5( unsigned int n ){ return n; }	// align +4byte
int MatrixObjectAlign4M6( unsigned int n ){ return n; }	// align +4byte
int MatrixObjectAlign4M7( unsigned int n ){ return n; }	// align +4byte
int MatrixObjectAlign4M8( unsigned int n ){ return n; }	// align +4byte
//int MatrixObjectAlign4M9( unsigned int n ){ return n; }	// align +4byte
//int MatrixObjectAlign4M0( unsigned int n ){ return n; }	// align +4byte
//int MatrixObjectAlign4MA( unsigned int n ){ return n; }	// align +4byte
//int MatrixObjectAlign4MB( unsigned int n ){ return n; }	// align +4byte
//int MatrixObjectAlign4MC( unsigned int n ){ return n; }	// align +4byte
//int MatrixObjectAlign4MD( unsigned int n ){ return n; }	// align +4byte
//int MatrixObjectAlign4ME( unsigned int n ){ return n; }	// align +4byte
//int MatrixObjectAlign4MF( unsigned int n ){ return n; }	// align +4byte
//int MatrixObjectAlign4MG( unsigned int n ){ return n; }	// align +4byte
//int MatrixObjectAlign4MH( unsigned int n ){ return n; }	// align +4byte
//int MatrixObjectAlign4MI( unsigned int n ){ return n; }	// align +4byte
//int MatrixObjectAlign4MJ( unsigned int n ){ return n; }	// align +4byte
//int MatrixObjectAlign4MK( unsigned int n ){ return n; }	// align +4byte
//int MatrixObjectAlign4ML( unsigned int n ){ return n; }	// align +4byte
//int MatrixObjectAlign4MM( unsigned int n ){ return n; }	// align +4byte
//int MatrixObjectAlign4MN( unsigned int n ){ return n; }	// align +4byte
//int MatrixObjectAlign4MO( unsigned int n ){ return n; }	// align +4byte
//int MatrixObjectAlign4MP( unsigned int n ){ return n; }	// align +4byte
//int MatrixObjectAlign4MQ( unsigned int n ){ return n; }	// align +4byte
//int MatrixObjectAlign4MR( unsigned int n ){ return n; }	// align +4byte
//int MatrixObjectAlign4MS( unsigned int n ){ return n; }	// align +4byte
//int MatrixObjectAlign4MT( unsigned int n ){ return n; }	// align +4byte
//int MatrixObjectAlign4MU( unsigned int n ){ return n; }	// align +4byte
//int MatrixObjectAlign4MV( unsigned int n ){ return n; }	// align +4byte
//int MatrixObjectAlign4MW( unsigned int n ){ return n; }	// align +4byte
//-----------------------------------------------------------------------------

