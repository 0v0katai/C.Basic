/*
===============================================================================

 Casio Basic RUNTIME library for fx-9860G series  v1.00

 copyright(c)2015/2016/2017 by sentaro21
 e-mail sentaro21@pm.matrix.jp

===============================================================================
*/
#include <ctype.h>
#include <fxlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <timer.h>
#include "KeyScan.h"
#include "CB_io.h"
#include "CB_error.h"

#include "CB_inp.h"
#include "CB_glib.h"
#include "CB_interpreter.h"
#include "CB_Eval.h"
#include "CB_Matrix.h"
#include "CB_Str.h"

#include "CBI_Eval.h"
#include "CBI_interpreter.h"
#include "CB_MonochromeLib.h"
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
	int		base=MatAry[reg].Base;
	dimA-=base;
	dimB-=base;
	switch ( MatAry[reg].ElementSize ) {
		case  2:		// Vram
//			dimA+=base;
//			dimB+=base;
		case  1:
			MatAryC=(char*)MatAry[reg].Adrs;			// Matrix array 1 bit
			return ( MatAryC[dimB*(((MatAry[reg].SizeA-1)>>3)+1)+(dimA>>3)] & ( 128>>(dimA&7) ) ) != 0 ;
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
	int tmp;
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
	}
}
//-----------------------------------------------------------------------------
//int MatrixObjectAlign6a( unsigned int n ){ return n+n; }	// align +6byte
//int MatrixObjectAlign4b( unsigned int n ){ return n; }	// align +4byte
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
		DimMatrixSub( reg, CB_INT? 32:64, 10-MatBase, 1, MatBase );	// new matrix
		if ( ErrorNo ) return ; // error
	}
	base=MatAry[reg].Base;
	(*dimB)=base;
	if ( ( (*dimA) < base+9 ) && ( MatAry[reg].SizeA-1+base < (*dimA) ) ) MatElementPlus( reg, (*dimA)-1+base, MatAry[reg].SizeB-1+base );	// List element+
	if ( ( (*dimA) < base ) || ( MatAry[reg].SizeA-1+base < (*dimA) ) ) { CB_Error(DimensionERR); return ; }	// Dimension error
	if ( SRC[ExecPtr] == ']' ) ExecPtr++ ;	// 
}
//-----------------------------------------------------------------------------
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
	return (int)x & (int)y;
}
int fORint( int x, int y ) {	// x or y
	return (int)x | (int)y;
}
int fXORint( int x, int y ) {	// x xor y
	return (int)x ^ (int)y;
}
int fNotint( int x ) {	// Not x
	return x==0;
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

int fMODint( int x, int y ) {	// fMODint(x,y)
	int tmp,tmp2,result;
	tmp  = x;
	tmp2 = y;
	if ( tmp2 == 0 )  CB_Error(DivisionByZeroERR); // Division by zero error 
	result= tmp-tmp/tmp2*tmp2;
	if ( result == tmp2  ) result--;
	if ( tmp < 0 ) {
		result=abs(tmp2)-result;
		if ( result == tmp2  ) result=0;
	}
	return result ;
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
int CB_EvalInt( char *SRC ) {
	int value;
	if (CB_INT) value=EvalIntsubTop( SRC ); else value=EvalsubTop( SRC ); 
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
		ExecPtr++; dst=EvalIntsub1(SRC); c=SRC[ExecPtr];
		if ( dst == 0 ) CB_Error(DivisionByZeroERR); // Division by zero error
		c=SRC[ExecPtr]; if ( (c==':')||(c==0x0E)||(c==0x13)||(c==',')||(c==')')||(c==']')||(c==0x0D)||(c==0) ) return result/dst;
	} else
	if ( c==0xFFFFFF8B ) { // ^2
		ExecPtr++;
		c=SRC[ExecPtr]; if ( (c==':')||(c==0x0E)||(c==0x13)||(c==',')||(c==')')||(c==']')||(c==0x0D)||(c==0) ) return result*result;
	} else
	if ( c==0x7F ) { // 
		c=SRC[++ExecPtr];
		if ( c==0xFFFFFFB0 ) { // And
			ExecPtr++; dst=EvalIntsub1(SRC); c=SRC[ExecPtr]; if ( (c==':')||(c==0x0E)||(c==0x13)||(c==',')||(c==')')||(c==']')||(c==0x0D)||(c==0) ) return (int)result & (int)dst;
		} else
		if ( c==0xFFFFFFB1 ) { // Or
			ExecPtr++; dst=EvalIntsub1(SRC); c=SRC[ExecPtr]; if ( (c==':')||(c==0x0E)||(c==0x13)||(c==',')||(c==')')||(c==']')||(c==0x0D)||(c==0) ) return (int)result | (int)dst;
		} else
		if ( c==0xFFFFFFB4 ) { // Xor
			ExecPtr++; dst=EvalIntsub1(SRC); c=SRC[ExecPtr]; if ( (c==':')||(c==0x0E)||(c==0x13)||(c==',')||(c==')')||(c==']')||(c==0x0D)||(c==0) ) return (int)result ^ (int)dst;
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
int Eval_atod(char *SRC, int c ) {
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
		if ( c=='#' ) { ExecPtr++; return LocalDbl[reg][0] ; }
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
		ExecPtr--; return  Eval_atod( SRC, c );
	}
	
	switch ( c ) { 			// ( type C function )  sin cos tan... 
		case 0x7F:	// 7F..
			c = SRC[ExecPtr++];
			switch ( c ) {
				case 0x40 :		// Mat A[a,b]
					c=SRC[ExecPtr];
					if ( ( 'A'<=c )&&( c<='z' ) ) { reg=c-'A'; ExecPtr++; } 
					else { reg=RegVarAliasEx(SRC); if ( reg<0 ) CB_Error(SyntaxERR) ; } // Syntax error 
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
					reg=c+(32-0x6A); goto Listj;
						
				case 0x3A :		// MOD(a,b)
					tmp = EvalIntsubTop( SRC );
					if ( SRC[ExecPtr] != ',' ) CB_Error(SyntaxERR) ; // Syntax error 
					ExecPtr++;
					tmp2 = EvalIntsubTop( SRC );
					result= fMODint(tmp,tmp2);
					if ( SRC[ExecPtr] == ')' ) ExecPtr++;
					return result ;
					
				case 0xFFFFFF85 :		// logab(a,b)
					tmp = EvalIntsubTop( SRC );
					if ( SRC[ExecPtr] != ',' ) CB_Error(SyntaxERR) ; // Syntax error 
					ExecPtr++;
					tmp2 = EvalIntsubTop( SRC );
					result = flogab(tmp,tmp2);
					if ( SRC[ExecPtr] == ')' ) ExecPtr++;
					return result ;

				case 0xFFFFFFB3 :		// Not
					return ( EvalIntsub5(SRC) == 0 ) ;
						
				case 0xFFFFFF9F :		// KeyRow(
					return CB_KeyRow( SRC ) ; 
				case 0xFFFFFF8F :		// Getkey
					c = SRC[ExecPtr];
					if ( ( '0'<=c )&&( c<='3' )) {	ExecPtr++ ;
						switch ( c ) {
							case '3':
								result = CB_Getkey3( SRC ) ; 
								break;
							default:
								result = CB_GetkeyN(c-'0') ;
								break;
						}
						if ( result==34 ) if (BreakCheck) { BreakPtr=ExecPtr; KeyRecover(); } 
					}
					else	result = CB_Getkey() ;
					return	result ;
					
				case 0xFFFFFF87 :		// RanInt#(st,en)
					x=(EvalIntsubTop( SRC ));
					if ( SRC[ExecPtr] != ',' ) CB_Error(SyntaxERR) ; // Syntax error 
					ExecPtr++ ;	// ',' skip
					y=(EvalIntsubTop( SRC ));
					if ( SRC[ExecPtr] == ')' ) ExecPtr++;
//					if ( x>=y ) CB_Error(ArgumentERR);  // Argument error
					if ( x>y ) { i=x; x=y; y=i; }
					return rand()*(y-x+1)/(RAND_MAX+1) +x ;

				case 0xFFFFFF88 :		// RanList(n) ->ListAns
					CB_RanList( SRC ) ;
					return 4 ;
					
				case 0xFFFFFFE9 :		// CellSum(Mat A[x,y])
					MatrixOprand( SRC, &reg, &x, &y );
					if ( ErrorNo ) return ; // error
					if ( SRC[ExecPtr] == ')' ) ExecPtr++;
					return Cellsum( reg, x, y );

				case 0x5F :				// 1/128 Ticks
					return RTC_GetTicks()-CB_TicksAdjust;	// 
						
				case 0xFFFFFFF0 :		// GraphY str
					return CBint_GraphYStr( SRC, 0 );
						
				case 0xFFFFFFF5 :		// IsExist(
					return  CB_IsExist( SRC );
				case 0xFFFFFFF6 :		// Peek(
					return  CB_Peek( SRC, EvalsubTop( SRC ) );
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
					break;
				case 0x58 :				// ElemSize( Mat A )
					MatrixOprandreg( SRC, &reg );
					if ( SRC[ExecPtr] == ')' ) ExecPtr++;
					i=MatAry[reg].ElementSize;
					if (i <= 4 ) i=1;
					return i;
				case 0x59 :				// ColSize( Mat A )
					MatrixOprandreg( SRC, &reg );
					if ( SRC[ExecPtr] == ')' ) ExecPtr++;
					return MatAry[reg].SizeA;
				case 0x5A :				// RowSize( Mat A )
					MatrixOprandreg( SRC, &reg );
					if ( SRC[ExecPtr] == ')' ) ExecPtr++;
					return MatAry[reg].SizeB;
				case 0x5B :				// MatBase( Mat A )
					MatrixOprandreg( SRC, &reg );
					if ( SRC[ExecPtr] == ')' ) ExecPtr++;
					return MatAry[reg].Base;

				case 0x4A :				// List>Mat( List 1, List 2,..) -> List 5
					CB_List2Mat( SRC );
					return 0;
				case 0x4B :				// Mat>List( Mat A, m) -> List n
					CB_Mat2List( SRC );;
					return 0;
				default:
					ExecPtr--;	// error
					break;
			}
			break;
			
//		case 0xFFFFFFC0 :	// Ans
//			return CBint_CurrentValue ;
		case '&' :	// & VarPtr
			return CB_VarPtr( SRC ) ;
		case 0xFFFFFFD0 :	// PI
			return PI ;
		case 0xFFFFFFC1 :	// Ran#
			c = SRC[ExecPtr];
			if ( ( '0'<=c )&&( c<='9' ) ) srand( Eval_atod( SRC, c ) );
			result=rand();
			return result ;
		case 0xFFFFFF97 :	// abs
			result = abs( EvalIntsub5( SRC ) );
			return result ;
		case 0xFFFFFFA6 :	// int
		case 0xFFFFFFDE :	// intg
			result = EvalIntsub5( SRC ) ;
			return result ;
		case 0xFFFFFFB6 :	// frac
			return 0 ;
//		case 0xFFFFFFA7 :	// Not
//			result = ! (int) ( EvalIntsub5( SRC ) );
//			return result ;
			
		case '%' :	// 1/128 Ticks
			return RTC_GetTicks()-CB_TicksAdjust;	// 
		case '*' :	// peek
			return CB_PeekInt( SRC, EvalIntsub1( SRC ) );	// 

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
				default:
					ExecPtr--;	// error
					break;
			}
			break;
		case 0xFFFFFFF9:	// F9..
			c = SRC[ExecPtr++];
			switch ( c ) {
				case 0x56:	// M_PixelTest(
					return CB_ML_PixelTest( SRC );
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
		result = EvalsubTop( SRC );
//		result = Evalsub1( SRC );
		return result;
	}
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
			 ( c == 0xFFFFFFD0 ) || // PI
			 ( c == 0xFFFFFFC0 ) || // Ans
			 ( c == 0xFFFFFFC1 )) { // Ran#
				result *= EvalIntsub4( SRC ) ;
		} else if ( c == 0x7F ) { // 7F..
			c = SRC[ExecPtr+1];
			switch ( c ) {
				case 0x40:	// Mat A[a,b]
				case 0x51:	// List 1[a]
				case 0x3A:	// MOD(a,b)
				case 0xFFFFFF8F:	// Getkey
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
			if (c>0) { ExecPtr=execptr; result *= Evalsub4( SRC ) ; }
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
				c = SRC[ExecPtr++];
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
					result = ( result & EvalIntsub12( SRC ) );
					break;
				default:
					return result;
					break;
			}
		} else return result;
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
					result = ( result | EvalIntsub13( SRC ) );
					break;
				case 0xFFFFFFB4 :	// Xor
					ExecPtr+=2;
					result = ( result ^ EvalIntsub13( SRC ) );
					break;
				default:
					return result;
					break;
			}
		} else return result;
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
int CB_Getkey3( char *SRC ) {
	int key,tmpkey=0;
	int result=0;
	int shift=0;
	int time1,time2;
	int t0=RTC_GetTicks()-CB_TicksAdjust;
	if ( SRC[ExecPtr] != '(' ) { CB_Error(SyntaxERR); return ; }	// Syntax error
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
	} while ( abs ( RTC_GetTicks()-CB_TicksAdjust - time2 ) < time1 ) ;
	key=tmpkey;
	return key;
}

int CB_KeyRow( char *SRC ) {		// Row Keyscan
	int row;
	row = CB_EvalInt( SRC );
	if ( SRC[ExecPtr] == ')' ) ExecPtr++;

	if ( IsSH3 ) {
		return ( CheckKeyRow(row) ) ;			//SH3
	}
	else {
		return ( CheckKeyRow7305(row) ) ;		//SH4A
	}
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
