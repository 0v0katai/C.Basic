/*
===============================================================================

 Casio Basic RUNTIME library for fx-9860G series  v1.8x

 copyright(c)2015/2016/2017/2018 by sentaro21
 e-mail sentaro21@pm.matrix.jp

===============================================================================
*/
#include "CB.h"
//----------------------------------------------------------------------------------------------
//		Expression evaluation    string -> int
//----------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//int EvalIntObjectAlignE4a( unsigned int n ){ return n; }	// align +4byte
//int EvalIntObjectAlignE4b( unsigned int n ){ return n+n; }	// align +6byte
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
int ReadMatrixInt( int reg, int dimA, int dimB){		// base:0  0-    base:1 1-
	char*	MatAryC;
	short*	MatAryW;
	int*	MatAryI;
	complex* MatAryCPLX;
	int		base=MatAry[reg].Base;
	int 	r;
	int result;
	dimA-=base;
	dimB-=base;
	switch ( MatAry[reg].ElementSize ) {
		case  2:		// Vram
//			dimA+=base;
//			dimB+=base;
		case  1:
			MatAryC=(char*)MatAry[reg].Adrs;			// Matrix array 1 bit
			return ( MatAryC[dimB*(((MatAry[reg].SizeA-1)>>3)+1)+(dimA>>3)] & ( 128>>(dimA&7) ) ) != 0 ;
		case  4:
			MatAryC=(char*)MatAry[reg].Adrs;			// Matrix array 4 bit
			r=MatAryC[dimB*(((MatAry[reg].SizeA-1)>>1)+1)+(dimA>>1)];
			if ( (dimA&1)==0 )  r = r>>4;
			else				r = r&0xF;
			if ( r>=8 ) r -= 16;	// minus
			return r;
		case  8:
			MatAryC=(char*)MatAry[reg].Adrs;
			return MatAryC[dimA*MatAry[reg].SizeB+dimB] ;			// Matrix array char
		case 16:
			MatAryW=(short*)MatAry[reg].Adrs;
			return MatAryW[dimA*MatAry[reg].SizeB+dimB] ;			// Matrix array word
		case 32:
			MatAryI=(int*)MatAry[reg].Adrs;
			return MatAryI[dimA*MatAry[reg].SizeB+dimB] ;			// Matrix array int
		case 64:
			return MatAry[reg].Adrs[dimA*MatAry[(reg)].SizeB+dimB] ;			// Matrix array doubl
		case 128:
			MatAryCPLX=(complex*)MatAry[reg].Adrs;			// Matrix array 1 bit
			return MatAryCPLX[dimA*MatAry[reg].SizeB+dimB].real ;			// Matrix array complex
			break;
	}
}
//-----------------------------------------------------------------------------
//int EvalIntObjectAlign4c( unsigned int n ){ return n; }	// align +4byte
//int EvalIntObjectAlign6d( unsigned int n ){ return n+n; }	// align +6byte
//-----------------------------------------------------------------------------
void WriteMatrixInt( int reg, int dimA, int dimB, int value){		// base:0  0-    base:1 1-
	char*	MatAryC;
	short*	MatAryW;
	int*	MatAryI;
	complex*	MatAryCPLX;
	int tmp,vtmp,mask;
	int mptr;
	int base=MatAry[reg].Base;
	dimA-=base;
	dimB-=base;
	switch ( MatAry[reg].ElementSize ) {
		case  2:		// Vram
//			dimA+=base;
//			dimB+=base;
		case  1:
			MatAryC=(char*)MatAry[reg].Adrs;					// Matrix array 1 bit
			tmp=( 128>>(dimA&7) );
			mptr=dimB*(((MatAry[reg].SizeA-1)>>3)+1)+(dimA>>3);
			if ( value ) 	MatAryC[mptr] |= tmp ;
			else	 		MatAryC[mptr] &= ~tmp ;
			break;
		case  4:
			MatAryC=(char*)MatAry[reg].Adrs;					// Matrix array 4 bit
			mptr=dimB*(((MatAry[reg].SizeA-1)>>1)+1)+((dimA)>>1);
			vtmp=(value)&0x0F; mask=0xF0;
			if ( (dimA&1)==0 ) { vtmp<<=4; mask>>=4; }
			MatAryC[mptr] &= mask ;
			MatAryC[mptr] |= vtmp ;
			break;
		case  8:
			MatAryC=(char*)MatAry[reg].Adrs;
			MatAryC[dimA*MatAry[reg].SizeB+dimB] = (char)value ;	// Matrix array char
			break;
		case 16:
			MatAryW=(short*)MatAry[reg].Adrs;
			MatAryW[dimA*MatAry[reg].SizeB+dimB] = (short)value ;	// Matrix array word
			break;
		case 32:
			MatAryI=(int*)MatAry[reg].Adrs;
			MatAryI[dimA*MatAry[reg].SizeB+dimB] = (int)value ;		// Matrix array int
			break;
		case 64:
			MatAry[reg].Adrs[dimA*MatAry[reg].SizeB+dimB]  = (double)value ;	// Matrix array double
			break;
		case 128:
			MatAryCPLX=(complex*)MatAry[reg].Adrs;			// Matrix array 1 bit
			MatAryCPLX[dimA*MatAry[reg].SizeB+dimB] = Int2Cplx( value );				// Matrix array complex
			break;
	}
}
//-----------------------------------------------------------------------------
int MatrixObjectAlign6a( unsigned int n ){ return n+n; }	// align +6byte
int MatrixObjectAlign4b( unsigned int n ){ return n; }	// align +4byte
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
int MatOperandIntSub( int c ) {
	if  ( ( '0'<=c )&&( c<='9' ) ) return c-'0';
	if ( ( ( 'A'<=c )&&( c<='Z' ) ) || ( ( 'a'<=c )&&( c<='z' ) ) ) return LocalInt[c-'A'][0] ;
	if ( c == 0xFFFFFFC0 ) return 28;		// Ans
	if ( ( c == 0xFFFFFFCD ) || ( c == 0xFFFFFFCE ) ) return LocalInt[c-0xFFFFFFCD+26][0] ;	// <r> or Theta
	CB_Error(SyntaxERR);
	return -1 ; 	// Syntax error
}
//-----------------------------------------------------------------------------
void MatOprandInt1sub( char *SRC, int reg, int *dimA ){ 	// base:0  0-    base:1 1-
	int c,d,e;
	c=SRC[ExecPtr];
	d=SRC[ExecPtr+1];
	if ( (d==',')||(d==']')||(d==0x0E)||(d==0x13)||(d==0x0D)||(d==0) )  {		// [a,
		ExecPtr++ ;
		(*dimA) = MatOperandIntSub( c );
	} else
	if ( d == 0xFFFFFF89 ) { 										// [a+1,
		e=SRC[ExecPtr+3];
		if ( (e==',')||(e==']')||(e==0x0E)||(e==0x13)||(e==0x0D)||(e==0) ) {
			ExecPtr+=2 ;
			(*dimA) = MatOperandIntSub( c );
			c=SRC[ExecPtr++];
			(*dimA) += MatOperandIntSub( c );
		} else goto L1;
	} else
	if ( d == 0xFFFFFF99 ) { 										// [a-1,
		e=SRC[ExecPtr+3];
		if ( (e==',')||(e==']')||(e==0x0E)||(e==0x13)||(e==0x0D)||(e==0) ) {
			ExecPtr+=2 ;
			(*dimA) = MatOperandIntSub( c );
			c=SRC[ExecPtr++];
			(*dimA) -= MatOperandIntSub( c );
		} else goto L1;
	} else {
  L1:	(*dimA) = (EvalIntsubTop( SRC ));
	}
}
void MatOprandInt2( char *SRC, int reg, int *dimA, int *dimB ){ 	// base:0  0-    base:1 1-
	int c,d;
	int base=MatAry[reg].Base;
	MatOprandInt1sub( SRC, reg, &(*dimA) );
	if ( ( (*dimA) < base ) || ( MatAry[reg].SizeA-1+base < (*dimA) ) ) { CB_Error(DimensionERR); return ; }	// Dimension error
	if ( SRC[ExecPtr] == ',' ) {
		ExecPtr++ ;
		MatOprandInt1sub( SRC, reg, &(*dimB) );
		if ( ( (*dimB) < base ) || ( MatAry[reg].SizeB-1+base < (*dimB) ) ) { CB_Error(DimensionERR); return ; }	// Dimension error
	} else {
		(*dimB)=base;
	}
	if ( SRC[ExecPtr] == ']' ) ExecPtr++ ;	// 
}
void MatOprandInt1( char *SRC, int reg, int *dimA, int *dimB ){ 	// base:0  0-    base:1 1-
	int c,d;
	int base;
	MatOprandInt1sub( SRC, reg, &(*dimA) );
	if ( MatAry[reg].SizeA == 0 ) { 
		DimMatrixSub( reg, DefaultElemetSize(), 10-MatBase, 1, MatBase );	// new matrix
		if ( ErrorNo ) return ; // error
	}
	base=MatAry[reg].Base;
	(*dimB)=base;
	if ( ( (*dimA) < base ) || ( MatAry[reg].SizeA-1+base < (*dimA) ) ) {
//		if ( MatAry[reg].SizeA < 10-base ) MatElementPlus( reg, 10-base, 1 );	// List element +
//		else 
		if ( (MatAry[reg].SizeA+1)==((*dimA)-1+base) ) MatElementPlus( reg, (*dimA)-1+base, 1 );	// List element +
		else
		{ CB_Error(DimensionERR); return ; }	// Dimension error
	}
	if ( SRC[ExecPtr] == ']' ) ExecPtr++ ;	// 
}
//-----------------------------------------------------------------------------
//int EvalIntObjectAlignE4d( unsigned int n ){ return n+n; }	// align +6byte
//int EvalIntObjectAlignE4e( unsigned int n ){ return n; }	// align +4byte
//int EvalIntObjectAlignE4f( unsigned int n ){ return n; }	// align +4byte
//int EvalIntObjectAlignE4g( unsigned int n ){ return n; }	// align +4byte
//int EvalIntObjectAlignE4h( unsigned int n ){ return n; }	// align +4byte
//int EvalIntObjectAlignE4i( unsigned int n ){ return n; }	// align +4byte
//-----------------------------------------------------------------------------

int fintint( int x ) {
	return x;
}
int fabsint( int x ) {
	return abs(x);
}
int fnotint( int x ) {
	return ! ( x );
}
int fsqrtint( int x ) {
	if ( x<0 ) CB_Error(MathERR) ; // Math error
	return sqrt( x );
}
int flog10int( int x ) {
	if ( x<=0 ) CB_Error(MathERR) ; // Math error
	return log10( x );
}
int fpow10int( int x ) {
	return pow( 10, x );
}
int flnint( int x ) {
	if ( x<=0 ) CB_Error(MathERR) ; // Math error
	return log( x );
}
int ffracint( int x ) {
	return 0;
}
int fexpint( int x ) {
	return exp( x );
}
int fcuberootint( int x ) {
	return pow( x, 1.0/3.0 );
}
int fsquint( int x ) {
	return x*x;
}
int ffactint( int x ) {
	int tmp;
	tmp = x ;
	x = 1;
	while ( tmp > 0 ) { x *= tmp; tmp--; }
	return x;
}
int frecipint( int x ) {	// ^(-1) RECIP
	if ( x == 0 ) CB_Error(DivisionByZeroERR); // Division by zero error 
	return 1 / x ;
}

int fsignint( int x ) {	// -x
	return -x;
}
int fADDint( int x, int y ) {	// x + y
	return x+y;
}
int fSUBint( int x, int y ) {	// x - y
	return x-y;
}
int fMULint( int x, int y ) {	// x * y
	return x*y;
}
int fDIVint( int x, int y ) {	// x / y
	if ( y == 0 ) CB_Error(DivisionByZeroERR); // Division by zero error 
	return x/y;
}
int fANDint( int x, int y ) {	// x and y
	return x & y;
}
int fORint( int x, int y ) {	// x or y
	return x | y;
}
int fXORint( int x, int y ) {	// x xor y
	return x ^ y;
}
int fNotint( int x ) {	// Not x
	return ~x;
}
int fANDint_logic( int x, int y ) {	// x && y
	return x && y;
}
int fORint_logic( int x, int y ) {	// x || y
	return x || y;
}
int fXORint_logic( int x, int y ) {	// x xor y
	return (x!=0) ^ (y!=0);
}
int fNotint_logic( int x ) {	// Not x
	return !x;
}
int fcmpEQint( int x, int y ) {	//  x = y
	return x == y;
}
int fcmpGTint( int x, int y ) {	//  x > y
	return x > y;
}
int fcmpLTint( int x, int y ) {	//  x < y
	return x < y;
}
int fcmpNEint( int x, int y ) {	//  x != y
	return x != y;
}
int fcmpGEint( int x, int y ) {	//  x >= y
	return x >= y;
}
int fcmpLEint( int x, int y ) {	//  x <= y
	return x <= y;
}

int fpowint( int x, int y ) {	// pow(x,y)
	return pow( x, y );
}
int fpowrootint( int x, int y ) {	// powroot(x,y)
	if ( y == 0 ) { CB_Error(MathERR) ; return 0; } // Math error
	return pow( x, 1/(double)y );
}
int flogabint( int x, int y ) {	// flogab(x,y)
	double base;
	int result;
	if ( x <= 0 ) { CB_Error(MathERR) ; return 0; } // Math error
	base  = log(x);
	result = log(y)/base;
	return result ;
}
int frandint() {
	return rand();
}
int frandIntint( int x, int y ) {
	int i;
	if ( x>y ) { i=x; x=y; y=i; }
	return rand()*(y-x+1)/(RAND_MAX+1) +x ;
}
int fMODint( int x, int y ) {	// fMODint(x,y)
	int result;
	if ( y == 0 )  CB_Error(DivisionByZeroERR); // Division by zero error 
	result= x-x/y*y;
	if ( result == y  ) result--;
	if ( x < 0 ) {
		result=abs(y)-result;
		if ( result == y  ) result=0;
	}
	return result ;
}

int fGCDint( int x, int y ) {	// GCD(x,y)
	int tmp;
	if ( x<y ) { tmp=x; x=y; y=tmp; }
	tmp=fMODint(x,y);
	while( tmp != 0 ) {
		x=y;
		y=tmp;
		tmp=fMODint(x,y);
	}
	return y;
}
int fLCMint( int x, int y ) {	// LCM(x,y)
	if ( ( x < 0 ) || ( x < 0 ) ) { CB_Error(ArgumentERR) ; return 0; } // Argumenterror
	return x/fGCDint(x,y)*y;
}

int CB_rand( char *SRC ) {
	int c = SRC[ExecPtr];
	if ( ( '0'<=c )&&( c<='9' ) ) {
		jp:
		srand(CB_EvalInt(SRC));
		if ( SRC[ExecPtr] == ')' ) ExecPtr++;
	} else
	if ( c=='#' ) { ExecPtr++; goto jp; }
	return rand() ;
}

//-----------------------------------------------------------------------------
//int EvalIntObjectAlignE4j( unsigned int n ){ return n; }	// align +4byte
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
int CB_EvalInt( char *SRC ) {
	int value;
	if (CB_INT==1) value=EvalIntsubTop( SRC ); else value=EvalsubTopReal( SRC ); 
	return value;
}
/*
int EvalIntEndCheck( int c ) {
	if ( c==':' ) return c;
	if ( c==0x0E ) return c;	// ->
	if ( c==0x13 ) return c;	// =>
	if ( c==',' ) return c;
	if ( c==')' ) return c;
	if ( c==']' ) return c;
	if ( c==0x0D ) return c;	// <CR>
	return ( c==0 );
}
*/
int EvalIntsubTop( char *SRC ) {	// eval 1
	int  result,dst;
	int c;
	int excptr=ExecPtr;
	int ansreg=CB_MatListAnsreg;

//	while ( SRC[ExecPtr]==0x20 ) ExecPtr++; // Skip Space
	result=EvalIntsub1(SRC);
	c=SRC[ExecPtr]; if ( (c==':')||(c==0x0E)||(c==0x13)||(c==',')||(c==')')||(c==']')||(c==0x0D)||(c==0) ) return result;
	else 
	if ( c==0xFFFFFF89 ) { // +
		ExecPtr++; dst=EvalIntsub1(SRC); c=SRC[ExecPtr]; if ( (c==':')||(c==0x0E)||(c==0x13)||(c==',')||(c==')')||(c==']')||(c==0x0D)||(c==0) ) return result+dst;
	} else
	if ( c==0xFFFFFF99 ) { // -
		ExecPtr++; dst=EvalIntsub1(SRC); c=SRC[ExecPtr]; if ( (c==':')||(c==0x0E)||(c==0x13)||(c==',')||(c==')')||(c==']')||(c==0x0D)||(c==0) ) return result-dst;
	} else
	if ( c=='=') { // ==
		ExecPtr++; dst=EvalIntsub1(SRC); c=SRC[ExecPtr]; if ( (c==':')||(c==0x0E)||(c==0x13)||(c==',')||(c==')')||(c==']')||(c==0x0D)||(c==0) ) return result == dst;
	} else
	if ( c=='>') { // >
		ExecPtr++; dst=EvalIntsub1(SRC); c=SRC[ExecPtr]; if ( (c==':')||(c==0x0E)||(c==0x13)||(c==',')||(c==')')||(c==']')||(c==0x0D)||(c==0) ) return result > dst;
	} else
	if ( c=='<') { // <
		ExecPtr++; dst=EvalIntsub1(SRC); c=SRC[ExecPtr]; if ( (c==':')||(c==0x0E)||(c==0x13)||(c==',')||(c==')')||(c==']')||(c==0x0D)||(c==0) ) return result < dst;
	} else
	if ( c==0x11) { // !=
		ExecPtr++; dst=EvalIntsub1(SRC); c=SRC[ExecPtr]; if ( (c==':')||(c==0x0E)||(c==0x13)||(c==',')||(c==')')||(c==']')||(c==0x0D)||(c==0) ) return result != dst;
	} else
	if ( c==0x12) { // >=
		ExecPtr++; dst=EvalIntsub1(SRC); c=SRC[ExecPtr]; if ( (c==':')||(c==0x0E)||(c==0x13)||(c==',')||(c==')')||(c==']')||(c==0x0D)||(c==0) ) return result >= dst;
	} else
	if ( c==0x10) { // <=
		ExecPtr++; dst=EvalIntsub1(SRC); c=SRC[ExecPtr]; if ( (c==':')||(c==0x0E)||(c==0x13)||(c==',')||(c==')')||(c==']')||(c==0x0D)||(c==0) ) return result <= dst;
	} else
	if ( c==0xFFFFFFA9 ) { // *
		ExecPtr++; dst=EvalIntsub1(SRC); c=SRC[ExecPtr]; if ( (c==':')||(c==0x0E)||(c==0x13)||(c==',')||(c==')')||(c==']')||(c==0x0D)||(c==0) ) return result*dst;
	} else
	if ( c==0xFFFFFFB9 ) { // /
	  divj:
		ExecPtr++; dst=EvalIntsub1(SRC); c=SRC[ExecPtr]; if ( (c==':')||(c==0x0E)||(c==0x13)||(c==',')||(c==')')||(c==']')||(c==0x0D)||(c==0) ) return fDIVint(result,dst);
	} else
	if ( c==0xFFFFFF9A ) { // xor
		ExecPtr++; dst=EvalIntsub1(SRC); c=SRC[ExecPtr]; if ( (c==':')||(c==0x0E)||(c==0x13)||(c==',')||(c==')')||(c==']')||(c==0x0D)||(c==0) ) return result ^ dst;
	} else
	if ( ( c=='|' ) || ( c==0xFFFFFFAA ) ) { // or
		ExecPtr++; dst=EvalIntsub1(SRC); c=SRC[ExecPtr]; if ( (c==':')||(c==0x0E)||(c==0x13)||(c==',')||(c==')')||(c==']')||(c==0x0D)||(c==0) ) return result | dst;
	} else
	if ( ( c=='&' ) || ( c==0xFFFFFFBA ) ) { // and
		ExecPtr++; dst=EvalIntsub1(SRC); c=SRC[ExecPtr]; if ( (c==':')||(c==0x0E)||(c==0x13)||(c==',')||(c==')')||(c==']')||(c==0x0D)||(c==0) ) return result & dst;
	} else
	if ( c==0xFFFFFF8B ) { // ^2
		ExecPtr++;
		c=SRC[ExecPtr]; if ( (c==':')||(c==0x0E)||(c==0x13)||(c==',')||(c==')')||(c==']')||(c==0x0D)||(c==0) ) return result*result;
	} else
	if ( c==0x7F ) { // 
		c=SRC[++ExecPtr];
		if ( c==0xFFFFFFB0 ) { // And
			ExecPtr++; dst=EvalIntsub1(SRC); c=SRC[ExecPtr]; if ( (c==':')||(c==0x0E)||(c==0x13)||(c==',')||(c==')')||(c==']')||(c==0x0D)||(c==0) ) return (int)result && (int)dst;
		} else
		if ( c==0xFFFFFFB1 ) { // Or
			ExecPtr++; dst=EvalIntsub1(SRC); c=SRC[ExecPtr]; if ( (c==':')||(c==0x0E)||(c==0x13)||(c==',')||(c==')')||(c==']')||(c==0x0D)||(c==0) ) return (int)result || (int)dst;
		} else
		if ( c==0xFFFFFFB4 ) { // Xor
			ExecPtr++; dst=EvalIntsub1(SRC); c=SRC[ExecPtr]; if ( (c==':')||(c==0x0E)||(c==0x13)||(c==',')||(c==')')||(c==']')||(c==0x0D)||(c==0) ) return ((int)result!=0) ^ ((int)dst!=0);
		} else
		if ( c==0xFFFFFFBC ) { // Int/
			goto divj;
		} else
		if ( c==0xFFFFFFBD ) { // Rmdr
			ExecPtr++; dst=EvalIntsub1(SRC); c=SRC[ExecPtr]; if ( (c==':')||(c==0x0E)||(c==0x13)||(c==',')||(c==')')||(c==']')||(c==0x0D)||(c==0) ) return fMODint(result,dst);
		}
	}

	ExecPtr=excptr;
	CB_MatListAnsreg=ansreg;
	return EvalIntsub14( SRC );
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//int EvalintObjectAlignE4c( unsigned int n ){ return n; }	// align +4byte
//int EvalintObjectAlignE4d( unsigned int n ){ return n+n; }	// align +6byte
//-----------------------------------------------------------------------------
int Eval_atoi(char *SRC, int c ) {
	int	result=0;
	if ( c == '0' ) {
		c = SRC[++ExecPtr];
		if ( (  c=='x' ) || ( c=='X' ) ) {
			c=SRC[++ExecPtr];
			while ( ( ( '0'<=c )&&( c<='9' ) ) || ( ('A'<=c)&&(c<='F') ) || ( ('a'<=c)&&(c<='f') ) ) {
				if ( ( '0'<=c )&&( c<='9' ) ) result = result*16 +(c-'0');
				else
				if ( ('A'<=c)&&(c<='F') ) result = result*16 +(c-'A'+10);
				else
				if ( ('a'<=c)&&(c<='f') ) result = result*16 +(c-'a'+10);
				c=SRC[++ExecPtr];
			}
			return result ;
		} else
		if ( (  c=='b' ) || ( c=='B' ) ) {
			c=SRC[++ExecPtr];
			while ( ('0'<=c)&&(c<='1') ) {
				result = result*2 +(c-'0');
				c=SRC[++ExecPtr];
			}
			return result ;
		}
	}
	while ( ( '0'<=c )&&( c<='9' ) ) {
		result = result*10 +(c-'0');
		c=SRC[++ExecPtr];
	}
	return result ;
}
//-----------------------------------------------------------------------------
int Get2EvalInt( char *SRC, int *tmp, int *tmp2){
	int c;
	(*tmp) = EvalIntsubTop( SRC );
	if ( SRC[ExecPtr] != ',' ) CB_Error(SyntaxERR) ; // Syntax error 
	ExecPtr++;
	(*tmp2) = EvalIntsubTop( SRC );
	c=SRC[ExecPtr];
	if ( c == ')' ) ExecPtr++;
	return c;
}

int EvalIntsub1(char *SRC) {	// 1st Priority
	int result,tmp,tmp2;
	char c,d;
	int dimA,dimB,reg,x,y;
	int i,ptr,mptr;
	char*	MatAryC;
	short*	MatAryW;
	int*	MatAryI;
	double*	MatAryF;

	c = SRC[ExecPtr++];
  topj:
	if ( c == '(') {
		result = EvalIntsubTop( SRC );
		if ( SRC[ExecPtr] == ')' ) ExecPtr++;
		return result;
	}
	while ( c == 0xFFFFFF89 ) c=SRC[ExecPtr++];	// +
	if ( ( c == 0xFFFFFF87 ) || ( c == 0xFFFFFF99 ) ) {	//  -
		return - EvalIntsub1( SRC );
	}
	if ( ( ( 'A'<=c )&&( c<='Z' ) ) || ( ( 'a'<=c )&&( c<='z' ) ) )  {
		reg=c-'A';
	  regj:
		c=SRC[ExecPtr];
		if ( c=='#' ) { ExecPtr++; return LocalDbl[reg][0].real ; }
		else
		if ( c=='[' ) { goto Matrix; }
		else
		if ( ( '0'<=c )&&( c<='9' ) ) {
				ExecPtr++;
				dimA=c-'0';
				MatOprand1num( SRC, reg, &dimA, &dimB );
				goto Matrix2;
		} else
		if ( c=='%' ) ExecPtr++;
		return LocalInt[reg][0] ;
	}
	if ( ( '0'<=c )&&( c<='9' ) ) {
		ExecPtr--; return  Eval_atoi( SRC, c );
	}
	
	switch ( c ) { 			// ( type C function )  sin cos tan... 
		case 0x7F:	// 7F..
			c = SRC[ExecPtr++];
			switch ( c ) {
				case 0x40 :		// Mat A[a,b]
				  Matjmp:
					c=SRC[ExecPtr];
					if ( ( ( 'A'<=c )&&( c<='Z' ) ) || ( ( 'a'<=c )&&( c<='z' ) ) ) { reg=c-'A'; ExecPtr++; } 
					else { reg=MatRegVar(SRC); if ( reg<0 ) CB_Error(SyntaxERR) ; } // Syntax error 
					Matrix1:
					if ( SRC[ExecPtr] == '[' ) {
					Matrix:
						ExecPtr++;
						MatOprandInt2( SRC, reg, &dimA, &dimB );
					Matrix2:
						if ( ErrorNo ) return 1 ; // error
					} else { dspflag=3;	// Mat A
							dimA=MatAry[reg].Base; dimB=dimA;
							CopyMatList2Ans( reg );
					}
					return ReadMatrixInt( reg, dimA, dimB);
					
				case 0x51 :		// List 1~26
					reg=ListRegVar( SRC );
				  Listj:
					if ( SRC[ExecPtr] == '[' ) {
						ExecPtr++;
						MatOprandInt1( SRC, reg, &dimA, &dimB );	// List 1[a]
						if ( ErrorNo ) return 1 ; // error
					} else { dspflag=4;	// List 1
							dimA=MatAry[reg].Base; dimB=dimA;
							CopyMatList2Ans( reg );
					}
					return ReadMatrixInt( reg, dimA, dimB);
					
				case 0x6A :		// List1
				case 0x6B :		// List2
				case 0x6C :		// List3
				case 0x6D :		// List4
				case 0x6E :		// List5
				case 0x6F :		// List6
					reg=c+(58-0x6A); goto Listj;
						
				case 0x3A :				// MOD(a,b)
					Get2EvalInt( SRC, &tmp, &tmp2);
					return fMODint(tmp,tmp2);
					
				case 0x3C :				// GCD(a,b)
					Get2EvalInt( SRC, &tmp, &tmp2);
					return fGCDint(tmp,tmp2);

				case 0x3D :				// LCM(a,b)
					Get2EvalInt( SRC, &tmp, &tmp2);
					return fLCMint(tmp,tmp2);
					
				case 0xFFFFFF85 :		// logab(a,b)
					Get2EvalInt( SRC, &tmp, &tmp2);
					return flogabint(tmp,tmp2);

				case 0xFFFFFFB3 :		// Not
					return ( ! EvalIntsub5(SRC) ) ;
						
				case 0xFFFFFF9F :		// KeyRow(
					return CB_KeyRow( SRC ) ; 
				case 0xFFFFFF8F :		// Getkey
					return CB_GetkeyEntry( SRC );
					
				case 0xFFFFFF87 :		// RanInt#(st,en)
					if ( Get2EvalInt( SRC, &x, &y) == ',' ) {
						ExecPtr++;
						CB_RanInt( SRC, x, y );
					}
					return frandIntint( x, y ) ;

				case 0xFFFFFFE9 :		// CellSum(Mat A[x,y])
					MatrixOprand( SRC, &reg, &x, &y );
					if ( ErrorNo ) return ; // error
					if ( SRC[ExecPtr] == ')' ) ExecPtr++;
					return Cellsum( reg, x, y );

				case 0x5F :				// 1/128 Ticks
					return CB_Ticks( SRC );	// 
						
				case 0xFFFFFFF0 :		// GraphY str
					return CBint_GraphYStr( SRC, 0 );
						
				case 0xFFFFFFF5 :		// IsExist(
					return  CB_IsExist( SRC, 0 );
				case 0xFFFFFFF6 :		// Peek(
					return  CB_PeekInt( SRC, EvalIntsubTop( SRC ) );
				case 0xFFFFFFF8 :		// VarPtr(
					return  CB_VarPtr( SRC );
				case 0xFFFFFFFA :		// ProgPtr(
					return  CB_ProgPtr( SRC );
				case 0x00 :				// Xmin
					return Xmin;
				case 0x01 :				// Xmax
					return Xmax;
				case 0x02 :				// Xscl
					return Xscl;
				case 0x04 :				// Ymin
					return Ymin;
				case 0x05 :				// Ymax
					return Ymax;
				case 0x06 :				// Yscl
					return Yscl;
				case 0x08 :				// Thetamin
					return TThetamin;
				case 0x09 :				// Thetamax
					return TThetamax;
				case 0x0A :				// Thetaptch
					return TThetaptch;
				case 0x0B :				// Xfct
					return Xfct;
				case 0x0C :				// Yfct
					return Yfct;
	
				case 0x29 :				// Sigma( X, X, 1, 1000)
					return CB_SigmaInt( SRC );
				case 0x20 :				// Max( List 1 )	Max( { 1,2,3,4,5 } )
					return CB_MinMaxInt( SRC, 1 );
				case 0x2D :				// Min( List 1 )	Min( { 1,2,3,4,5 } )
					return CB_MinMaxInt( SRC, 0 );
				case 0x2E :				// Mean( List 1 )	Mean( { 1,2,3,4,5 } )
					return CB_MeanInt( SRC );
				case 0x4C :				// Sum List 1
					return CB_SumInt( SRC );
				case 0x4D :				// Prod List 1)
					return CB_ProdInt( SRC );
				case 0x47:	// Fill(
					CB_MatFill(SRC);
					return 3;
				case 0x48:	// Identity 
					CB_Identity(SRC);
					return 3;
				case 0x49:	// Argument(
					CB_Argument(SRC);
					return 3;
				case 0x2C:	// Seq
					CB_SeqInt(SRC);
					return 4;
				case 0x41:	// Trn
					CB_MatTrn(SRC);
					return 3;
				
				case 0x46 :				// Dim
					result=CB_Dim( SRC );
					if ( result >= 0 ) return result;
					ExecPtr--;	// error
					break;
				case 0x58 :				// ElemSize( Mat A )
					return CB_ElemSize( SRC );
				case 0x59 :				// RowSize( Mat A )
					return CB_RowSize( SRC );
				case 0x5A :				// ColSize( Mat A )
					return CB_ColSize( SRC );
				case 0x5B :				// MatBase( Mat A )
					return CB_MatBase( SRC );

				case 0x5C :				// ListCmp( List 1, List 2)
					return CB_ListCmp( SRC );
					
				case 0x4A :				// List>Mat( List 1, List 2,..) -> List 5
					CB_List2Mat( SRC );
					return 0;
				case 0x4B :				// Mat>List( Mat A, m) -> List n
					CB_Mat2List( SRC );;
					return 0;
					
				case 0xFFFFFFCF :				// System(
					return CB_System( SRC );
				case 0xFFFFFFDF :				// Version
					return CB_Version();		//
				default:
					ExecPtr--;	// error
					break;
			}
			break;
			
//		case 0xFFFFFFC0 :	// Ans
//			return CBint_CurrentValue ;
		case '&' :	// & VarPtr
			return CB_VarPtr( SRC ) ;
		case 0xFFFFFFD0 :	// const_PI
			return const_PI ;
		case 0xFFFFFFC1 :	// Ran#
			return CB_rand( SRC ) ;
		case 0xFFFFFF97 :	// abs
			result = abs( EvalIntsub5( SRC ) );
			return result ;
		case 0xFFFFFFA6 :	// int
		case 0xFFFFFFDE :	// intg
			result = EvalIntsub5( SRC ) ;
			return result ;
		case 0xFFFFFFB6 :	// frac
			return 0 ;
		case 0xFFFFFFA7 :	// not
			result = ~ ( EvalIntsub5( SRC ) );
			return result ;
			
		case '%' :	// 1/128 Ticks
			return CB_Ticks( SRC );	// 
		case '*' :	// peek
			return CB_PeekInt( SRC, EvalIntsub1( SRC ) );	// 
		case '@' :	// Mat @A
			ExecPtr--;
			goto Matjmp;

		case '{':	// { 1,2,3,4,5... }->List Ans
			CB_List(SRC);
			return 0;
		case '[':	// [ [0,1,2][2,3,4] ]->Mat Ans
			CB_Matrix(SRC);
			return 0;
			
		case 0xFFFFFF86 :	// sqr
			return fsqrtint( EvalIntsub5( SRC ) );
		case 0xFFFFFF95 :	// log10
			return flog10int( EvalIntsub5( SRC ) );
		case 0xFFFFFFB5 :	// 10^
			return fpow10int( EvalIntsub5( SRC ) );
		case 0xFFFFFF85 :	// ln
			return flnint( EvalIntsub5( SRC ) );
		case 0xFFFFFFA5 :	// expn
			return fexpint( EvalIntsub5( SRC ) );
		case 0xFFFFFF96 :	// cuberoot
			return fcuberootint( EvalIntsub5( SRC ) );
			
		case 0xFFFFFFF7:	// F7..
			c = SRC[ExecPtr++];
			switch ( c ) {
				case 0xFFFFFFAF:	// PxlTest(y,x)
					y=(EvalIntsubTop( SRC ));
					if ( SRC[ExecPtr] != ',' ) CB_Error(SyntaxERR) ; // Syntax error 
					ExecPtr++ ;	// ',' skip
					x=(EvalIntsubTop( SRC ));
					if ( SRC[ExecPtr] == ')' ) ExecPtr++;
					result = PxlTest(y, x) ;			// 
					return result ;
				case 0xFFFFFFB0 :				// SortA( List 1)
					return CB_EvalSortAD( SRC, 1 );
				case 0xFFFFFFB1 :				// SortD( List 1)
					return CB_EvalSortAD( SRC, 0 );
				case 0xFFFFFFF4:	// SysCall(
					return  CB_SysCall( SRC );
				case 0xFFFFFFF5:	// Call(
					return  CB_Call( SRC );
				case 0xFFFFFFF8:	// RefreshCtrl
					return  RefreshCtrl;
				case 0xFFFFFFFA:	// RefreshTime
					return  Refreshtime+1;
				case 0xFFFFFFFB:	// Screen
					return  ScreenMode;
				case 0xFFFFFFFE:	// BackLight
					return	BackLight(-1);
				case 0xFFFFFFF2:	// PopUpWin(
					return CB_PopUpWin(SRC);
				default:
					ExecPtr--;	// error
					break;
			}
			break;
		case 0xFFFFFFF9:	// F9..
			c = SRC[ExecPtr++];
			switch ( c ) {
				case 0xFFFFFFC6:	// M_PixelTest(
					return CB_ML_PixelTest( SRC );
				case 0xFFFFFFD8:	// M_Test
					return CB_MLTest( SRC );
//				case 0x53:	// M_Contrast(
//					return CB_ML_GetContrast( SRC );
				case 0x31:	// StrLen(
					return CB_StrLen( SRC );
				case 0x32:	// StrCmp(
					return CB_StrCmp( SRC );
				case 0x33:	// StrSrc(
					return CB_StrSrc( SRC );
				case 0x38:	// Exp(
					return CBint_EvalStr(SRC, 0 );
				case 0x50:	// StrAsc(
					return CB_StrAsc( SRC );
				case 0x5F:	// IsError(
					return CB_IsError(SRC);
				case 0x60:	// GetFont(
					return CB_GetFont(SRC);
				case 0x62:	// GetFontMini(
					return CB_GetFontMini(SRC);
				case 0x21:	// Xdot
					return Xdot;
				case 0x1B :		// fn str
					return CBint_FnStr( SRC, 0 );
				default:
					ExecPtr--;	// error
					break;
			}
			break;
		case 0xFFFFFFDD :	// Eng
			return ENG ;
		default:
			break;
	}
	if ( c == '#') {
		result = EvalsubTopReal( SRC );
//		result = Evalsub1( SRC );
		return result;
	} else
	if ( c==' ' ) { while ( c==' ' )c=SRC[ExecPtr++]; goto topj; }	// Skip Space
	
	ExecPtr--;
	reg=RegVarAliasEx( SRC ); if ( reg>=0 ) goto regj;	// variable alias
	CB_Error(SyntaxERR) ; // Syntax error 
	return 0 ;
}
//-----------------------------------------------------------------------------
//int EvalIntObjectAlignE4g( unsigned int n ){ return n ; }	// align +4byte
//int EvalIntObjectAlignE4h( unsigned int n ){ return n+n; }	// align +6byte
//-----------------------------------------------------------------------------
int EvalIntsub2(char *SRC) {	//  2nd Priority  ( type B function ) ...
	int cont=1;
	int result,tmp;
	int c;
	result = EvalIntsub1( SRC );
	while ( 1 ) {
		c = SRC[ExecPtr++];
		switch ( c ) {
			case  0xFFFFFF8B  :	// ^2
				result *= result ;
				break;
			case  0xFFFFFF9B  :	// ^(-1) RECIP
				result = frecipint( result );
				break;
			case  0xFFFFFFAB  :	//  !
				result = ffactint( result );
				break;
			case ' ':	// Skip Space
				break;
			default:
				ExecPtr--;
				return result;
				break;
		}
	 }
	return result;
}
int EvalIntsub4(char *SRC) {	//  3rd Priority  ( ^ ...)
	int result,tmp;
	int c;
	result = EvalIntsub2( SRC );
	while ( 1 ) {
		c = SRC[ExecPtr++];
		switch ( c ) {
			case  0xFFFFFFA8  :	// a ^ b
				result = pow( result, EvalIntsub2( SRC ) );
				break;
			case  0xFFFFFFB8  :	// powroot
				result = pow( EvalIntsub2( SRC ), 1/result );
				break;
			case ' ':	// Skip Space
				break;
			default:
				ExecPtr--;
				return result;
				break;
		}
	 }
	return result;
}
int EvalIntsub5(char *SRC) {	//  5th Priority  abbreviated multiplication
	int result,tmp;
	int c,execptr;
	int dimA,dimB,reg,x,y;
	result = EvalIntsub4( SRC );
	while ( 1 ) {
		c = SRC[ExecPtr];
		if ((( 'A'<=c )&&( c<='Z' )) ||
			(( 'a'<=c )&&( c<='z' )) ||
			 ( c == 0xFFFFFFCD ) || // <r>
			 ( c == 0xFFFFFFCE ) || // Theta
			 ( c == 0xFFFFFFD0 ) || // const_PI
			 ( c == 0xFFFFFFC0 ) || // Ans
			 ( c == 0xFFFFFFC1 )) { // Ran#
				result *= EvalIntsub4( SRC ) ;
		} else if ( c == 0x7F ) { // 7F..
			c = SRC[ExecPtr+1];
			switch ( c ) {
				case 0x40:	// Mat A[a,b]
				case 0x51:	// List 1[a]
				case 0x3A:	// MOD(a,b)
				case 0x3C:	// GCD(a,b)
				case 0x3D:	// LCM(a,b)
				case 0xFFFFFF8F:	// Getkey
				case 0xFFFFFF85:	// logab(a,b)
				case 0xFFFFFF86:	// RndFix(n,digit)
				case 0xFFFFFF87:	// RanInt#(st,en)
				case 0xFFFFFFB3 :	// Not
				case 0xFFFFFFF0:	// GraphY
				case 0x00:	// Xmin
				case 0x01:	// Xmax
				case 0x02:	// Xscl
				case 0x04:	// Ymin
				case 0x05:	// Ymax
				case 0x06:	// Yscl
				case 0x08:	// Thetamin
				case 0x09:	// Thetamax
				case 0x0A:	// Thetaptch
				case 0x0B:	// Xfct
				case 0x0C:	// Yfct
				case 0x20 :			// Max( List 1 )	Max( { 1,2,3,4,5 } )
				case 0x29 :			// Sigma( X, X, 1, 1000)
				case 0x2D :			// Min( List 1 )	Min( { 1,2,3,4,5 } )
				case 0x2E :			// Mean( List 1 )	Mean( { 1,2,3,4,5 } )
				case 0x47 :			// Fill(
				case 0x4A :			// List>Mat( List 1, List 2,..) -> List 5
				case 0x4B :			// Mat>List( Mat A, m) -> List n
				case 0x4C :			// Sum List 1
				case 0x4D :			// Prod List 1
				case 0x58 :			// ElemSize( Mat A )
				case 0x59 :			// RowSize( Mat A )
				case 0x5A :			// ColSize( Mat A )
				case 0x5B :			// MatBase( Mat A )
					result *= EvalIntsub4( SRC ) ;
					break;
				default:
					goto exitj;
					break;
			}
		} else if ( c == 0xFFFFFFF7 ) { // F7..
			c = SRC[ExecPtr+1];
			switch ( c ) {
				case 0xFFFFFFAF:	// PxlTest(y,x)
					result *= EvalIntsub4( SRC ) ;
					break;
				default:
					goto exitj;
					break;
			}
		} else if ( c == 0xFFFFFFF9 ) { // F9..
			c = SRC[ExecPtr+1];
			switch ( c ) {
				case 0x21:	// Xdot
					result *= EvalIntsub4( SRC ) ;
					break;
				default:
					goto exitj;
					break;
			}
//		} else if ( c == 0xFFFFFFE7 ) { // E7..
//			c = SRC[ExecPtr+1];
//			switch ( c ) {
//				default:
//					return result;
//					break;
//			}
		} else {
		  exitj:
			execptr=ExecPtr;
			c=RegVarAliasEx(SRC);
			if (c>0) { ExecPtr=execptr; result *= EvalIntsub4( SRC ) ; }
			else return result;
		}
	 }
}
int EvalIntsub7(char *SRC) {	//  7th Priority abbreviated multiplication type A/C
	int result,tmp;
	int c;
	result = EvalIntsub5( SRC );
	while ( 1 ) {
		c = SRC[ExecPtr];
		switch ( c ) {
			case '(' :
			case 0xFFFFFF97 :	// abs
			case 0xFFFFFFA6 :	// int
			case 0xFFFFFFB6 :	// frac
			case 0xFFFFFFDE :	// intg
			case 0xFFFFFF85 :	// ln
			case 0xFFFFFF86 :	// sqr
			case 0xFFFFFF95 :	// log10
			case 0xFFFFFF96 :	// cuberoot
			case 0xFFFFFFA5 :	// expn
			case 0xFFFFFFA7 :	// Not
			case 0xFFFFFFB5 :	// 10^
			case 0xFFFFFFB7 :	// Neg
				result *= EvalIntsub5( SRC );
				break;
			default:
				return result;
				break;
		}
	 }
	return result;
}
int EvalIntsub10(char *SRC) {	//  10th Priority  ( *,/, int.,Rmdr )
	int result,tmp;
	int c;
	result = EvalIntsub7( SRC );
	while ( 1 ) {
		c = SRC[ExecPtr++];
		switch ( c ) {
			case 0xFFFFFFA9 :		// ~
				result *= EvalIntsub7( SRC );
				break;
			case 0xFFFFFFB9 :		// €
				tmp = EvalIntsub7( SRC );
				if ( tmp == 0 ) CB_Error(DivisionByZeroERR); // Division by zero error 
				result /= tmp ;
				break;
			case 0x7F:
				c = SRC[ExecPtr]; while ( c==0x20 )c=SRC[++ExecPtr]; ExecPtr++; // Skip Space
				switch ( c ) {
					case 0xFFFFFFBC:	// Int€
						tmp = EvalIntsub7( SRC );
						if ( tmp == 0 ) CB_Error(DivisionByZeroERR); // Division by zero error 
						result /= tmp ;
						break;
					case 0xFFFFFFBD:	// Rmdr
						tmp = EvalIntsub7( SRC );
						result = fMODint( result, tmp );
						break;
					default:
						ExecPtr-=2;
						return result;
						break;
				}
				break;
			case ' ':	// Skip Space
				break;
			default:
				ExecPtr--;
				return result;
				break;
		}
	 }
	return result;
}
int EvalIntsub11(char *SRC) {	//  11th Priority  ( +,- )
	int result;
	int c;
	result = EvalIntsub10( SRC );
	while ( 1 ) {
		c = SRC[ExecPtr++];
		switch ( c ) {
			case 0xFFFFFF89 :		// +
				result += EvalIntsub10( SRC );
				break;
			case 0xFFFFFF99 :		// -
				result -= EvalIntsub10( SRC );
				break;
			case ' ':	// Skip Space
				break;
			default:
				ExecPtr--;
				return result;
				break;
		}
	 }
	return result;
}
int EvalIntsub12(char *SRC) {	//  12th Priority ( =,!=,><,>=,<= )
	int result;
	int c;
	result = EvalIntsub11( SRC );
	while ( 1 ) {
		c = SRC[ExecPtr++];
		switch ( c ) {
			case '=' :	// =
				result = ( result == EvalIntsub11( SRC ) );
				break;
			case '>' :	// >
				result = ( result >  EvalIntsub11( SRC ) );
				break;
			case '<' :	// <
				result = ( result <  EvalIntsub11( SRC ) );
				break;
			case 0x11 :	// !=
				result = ( result != EvalIntsub11( SRC ) );
				break;
			case 0x12 :	// >=
				result = ( result >= EvalIntsub11( SRC ) );
				break;
			case 0x10 :	// <=
				result = ( result <= EvalIntsub11( SRC ) );
				break;
			case 0xFFFFFF9A :	// xor
				result = ( result ^  EvalIntsub11( SRC ) );
				break;
			case '|' :	// or
			case 0xFFFFFFAA :	// or
				result = ( result |  EvalIntsub11( SRC ) );
				break;
			case '&' :	// and
			case 0xFFFFFFBA :	// and
				result = ( result &  EvalIntsub11( SRC ) );
				break;
			case ' ':	// Skip Space
				break;
			default:
				ExecPtr--;
				return result;
				break;
		}
	 }
	return result;
}
int EvalIntsub13(char *SRC) {	//  13th Priority  ( And,and)
	int result;
	int c;
	result = EvalIntsub12( SRC );
	while ( 1 ) {
		c = SRC[ExecPtr];
		if ( c == 0x7F ) {
			c = SRC[ExecPtr+1];
			switch ( c ) {
				case 0xFFFFFFB0 :	// And
					ExecPtr+=2;
					result = ( ( result != 0 ) & ( EvalIntsub12( SRC ) != 0 ) );
					break;
				default:
					return result;
					break;
			}
		} else
		if ( c == ' ' ) ExecPtr++;	// Skip Space
		else return result;
	}
}
int EvalIntsub14(char *SRC) {	//  14th Priority  ( Or,Xor,or,xor,xnor )
	int result;
	int c;
	result = EvalIntsub13( SRC );
	while ( 1 ) {
		c = SRC[ExecPtr];
		if ( c == 0x7F ) {
			c = SRC[ExecPtr+1];
			switch ( c ) {
				case 0xFFFFFFB1 :	// Or
					ExecPtr+=2;
					result = ( ( result != 0 ) | ( EvalIntsub13( SRC ) != 0 ) );
					break;
				case 0xFFFFFFB4 :	// Xor
					ExecPtr+=2;
					result = ( ( result != 0 ) ^ ( EvalIntsub13( SRC ) != 0 ) );
					break;
				default:
					return result;
					break;
			}
		} else
		if ( c == ' ' ) ExecPtr++;	// Skip Space
		else return result;
	}
}

//----------------------------------------------------------------------------------------------
int EvalInt(char *SRC) {		// Eval temp
	int result;
	int execptr=ExecPtr;
	int oplen=strlenOp((char*)SRC);
	if ( oplen == 0 ) return 0;
	ExecPtr= 0;
	ErrorPtr= 0;
	ErrorNo = 0;
	result = EvalIntsubTop( SRC );
	if ( ExecPtr < oplen ) CB_Error(SyntaxERR) ; // Syntax error 
	if ( ErrorNo ) { CB_ErrMsg( ErrorNo ); }
	ExecPtr=execptr;
	return result;
}

//----------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------
int	tickstmp=0;
int	Hitickstmp=0;
#define P7305_EXTRA_TMU5_COUNT 0xA44D00D8

unsigned int GetTicks32768(){
	if ( IsSH3 ) return RTC_GetTicks()<<8;
	return *(unsigned int*)P7305_EXTRA_TMU5_COUNT;
}

void CB_StoreTicks( char *SRC, int value ) {
	int n;
	int high=0;
	int t;
	if ( SRC[ExecPtr] == '%' ) {	// hi-res timer
		ExecPtr++;
		if ( IsSH3 == 0 ) high=1;
	}
	if ( high ) {
		t=(int)GetTicks32768();
		CB_HiTicksAdjust=t-value;
	} else {
		t=RTC_GetTicks();
		CB_TicksAdjust=t-value;
	}
	skip_count=0;
}

int CB_RTC_GetTicks( int high ) {
	if ( high ) return CB_HiTicksAdjust-(int)GetTicks32768() ;
	return RTC_GetTicks()-CB_TicksAdjust ;
}
int CB_Ticks( char *SRC ) {
	int n;
	int high=0;
	int t;
	if ( SRC[ExecPtr] == '%' ) {	// hi-res timer
		ExecPtr++;
		if ( IsSH3 == 0 ) high=1;
	}
	t=CB_RTC_GetTicks(high);
	if ( SRC[ExecPtr]==0xFFFFFFF9 ) {
		if ( SRC[ExecPtr+1]==0x4F ) {	// Wait
			ExecPtr+=2;		// TicksWait n
			n = CB_EvalInt( SRC );
			if ( high ) {
				if ( n<0 ) n=-n;  else Hitickstmp=CB_RTC_GetTicks(high);
				do {
					if ( KeyScanDownAC() ) { KeyRecover(); if ( BreakCheck ) BreakPtr=ExecPtr; return t; }	// [AC] break?
					t=CB_RTC_GetTicks(high);
				} while ( abs( t-Hitickstmp ) <= n ) ;
				Hitickstmp=CB_RTC_GetTicks(high);
				return Hitickstmp;
			} else {
				if ( n<0 ) n=-n;  else tickstmp=CB_RTC_GetTicks(high);
				do {
					if ( KeyScanDownAC() ) { KeyRecover(); if ( BreakCheck ) BreakPtr=ExecPtr; return t; }	// [AC] break?
					t=CB_RTC_GetTicks(high);
				} while ( abs( t-tickstmp ) <= n ) ;
				tickstmp=CB_RTC_GetTicks(high);
				return tickstmp;
			}
		}
	} 
	return t;
}

int CB_Getkey3( char *SRC ) {
	int key,tmpkey=0;
	int result=0;
	int shift=0;
	int time1,time2;
	int t0=CB_RTC_GetTicks(0);
	if ( SRC[ExecPtr] != '(' ) { CB_Error(SyntaxERR); return 0; }	// Syntax error
	ExecPtr++;
	time1 = CB_EvalInt( SRC );
	if ( SRC[ExecPtr] == ',' )  {
			ExecPtr++;
			time2 = CB_EvalInt( SRC );
	} else	time2 = t0;
	if ( SRC[ExecPtr] == ')' ) ExecPtr++;

	do {
		key=CB_Getkey();
		if ( key == 34 ) return key;	// [AC]
		if ( key ) tmpkey=key;
	} while ( abs ( CB_RTC_GetTicks(0) - time2 ) < time1 ) ;
	key=tmpkey;
	return key;
}

int CB_KeyRowSub(int row){
	if ( IsSH3 ) {
		return ( CheckKeyRow(row) ) ;			//SH3
	}
	else {
		return ( CheckKeyRow7305(row) ) ;		//SH4A
	}
}

int CB_KeyRow( char *SRC ) {		// Row Keyscan
	int row;
	row = CB_EvalInt( SRC );
	if ( SRC[ExecPtr] == ')' ) ExecPtr++;

	return CB_KeyRowSub(row) ;
}

//----------------------------------------------------------------------------------------------
int CB_GetkeyM() {	//	Getkey multi  -> list { }
	//				   6  5  4  3  2  1
//	char keycode9[]={ 79,69,59,49,39,29};
//	char keycode8[]={ 78,68,58,48,38,28};
//	char keycode7[]={ 77,67,57,47,37,27};
//	char keycode6[]={ 76,66,56,46,36,26};
//	char keycode5[]={ 75,65,55,45,35,25};
//	char keycode4[]={ 74,64,54,44,34};
//	char keycode3[]={ 73,63,53,43,33};
//	char keycode2[]={ 72,62,52,42,32};
//	char keycode1[]={ 71,61,51,41,31};
	char rowdata[10];
	char result[55];
	int i,j,a,b,c=0;
	for(i=1; i<=9; i++) rowdata[i]=CB_KeyRowSub(i);
	b=128;
	for(j=6; j>=1; j--) {
		b>>=1;
		for(i=9; i>=1; i--) {
			if ( rowdata[i] & b ) result[c++] = i+(j+1)*10;
		}
	}

	dspflag=4;	// List ans
	if ( c==0 ) {
		c=1;
		result[0]=0;
	}
	
	NewMatListAns( c, 1, 1, 8 );		// List Ans[c].b
	for (i=0; i<c; i++) {
		WriteMatrix( CB_MatListAnsreg, i+1, 1, result[i] ) ;	//
	}
	return 1;
}

int CB_GetkeyEntry( char *SRC ) {	// CB_GetKey entry
	int c = SRC[ExecPtr],d;
	int result,sdkcode=0;
	if ( c=='@' ) { sdkcode=1; c = SRC[++ExecPtr]; }	// Getkey SDK code
	if ( ( ( '0'<=c )&&( c<='3' )) || ( c=='m') || ( c=='M') ) {
		ExecPtr++ ;
		switch ( c ) {
			case 'M':
			case 'm':
				result=CB_GetkeyM() ; 
				break;
			case '3':
				result=CB_Getkey3( SRC ) ; 
				break;
			default:
				result=CB_GetkeyN( c-'0', 1, sdkcode ) ;
				break;
		}
		if ( result==34 ) if (BreakCheck) { BreakPtr=ExecPtr; KeyRecover(); } 
	} else result=CB_Getkey();
	return result;
}

//----------------------------------------------------------------------------------------------

void CB_Wait( char *SRC ) {
	int n;
	n = CB_EvalInt( SRC );
	if ( n < 0 ) { CB_Error(ArgumentERR); return ; } // Argument error
	Waitcount = n ;
}

//----------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//int EvalIntObjectAlign4e( unsigned int n ){ return n; }	// align +4byte
//int EvalIntObjectAlign6f( unsigned int n ){ return n+n; }	// align +6byte
