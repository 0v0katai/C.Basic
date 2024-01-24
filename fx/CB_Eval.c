/*
===============================================================================

 Casio Basic RUNTIME library for fx-9860G series  v0.90

 copyright(c)2015 by sentaro21
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
#include "CB_inp.h"
#include "CB_glib.h"
#include "CB_Eval.h"
#include "CB_interpreter.h"
#include "CBI_Eval.h"
#include "CBI_interpreter.h"
#include "CB_error.h"
#include "CB_Matrix.h"
#include "CB_Str.h"

//----------------------------------------------------------------------------------------------
//		Expression evaluation    string -> double
//----------------------------------------------------------------------------------------------
char ExpBuffer[ExpMax+1];

//----------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//int MatrixObjectAlignE4a( unsigned int n ){ return n; }	// align +4byte
//int MatrixObjectAlignE4b( unsigned int n ){ return n+n; }	// align +6byte
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

double ReadMatrix( int reg, int dimA, int dimB){		// base:0  0-    base:1 1-
	char*	MatAryC;
	short*	MatAryW;
	int*	MatAryI;
	int		base=MatAry[reg].Base;
	dimA-=base;
	dimB-=base;
	switch ( MatAry[reg].ElementSize ) {
		case 64:
			return MatAry[reg].Adrs[dimA*MatAry[reg].SizeB+dimB] ;			// Matrix array doubl
		case  2:		// Vram
			dimA+=base;
			dimB+=base;
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
	}
}

void WriteMatrix( int reg, int dimA, int dimB, double value){		// base:0  0-    base:1 1-
	char*	MatAryC;
	short*	MatAryW;
	int*	MatAryI;
	int tmp;
	int mptr;
	int		base=MatAry[reg].Base;
	dimA-=base;
	dimB-=base;
	switch ( MatAry[reg].ElementSize ) {
		case 64:
			MatAry[reg].Adrs[dimA*MatAry[reg].SizeB+dimB]  = (double)value ;	// Matrix array double
			break;
		case  2:	// Vram
			dimA+=base;
			dimB+=base;
		case  1:
			MatAryC=(char*)MatAry[reg].Adrs;					// Matrix array 1 bit
			tmp=( 128>>(dimA&7) );
			mptr=dimB*(((MatAry[reg].SizeA-1)>>3)+1)+((dimA)>>3);
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
	}
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
int MatOperandSub( int c ) {
	if  ( ( '1'<= c ) && ( c<='9' ) ) return c-'0';
	else if  ( ( 'A'<= c ) && ( c<='Z' ) ) return REG[c-'A'];
	else if  ( ( 'a'<= c ) && ( c<='z' ) ) return LocalDbl[c-'a'][0];
}
//-----------------------------------------------------------------------------
void MatOprand2( char *SRC, int reg, int *dimA, int *dimB ){	// base:0  0-    base:1 1-
	int dst;
	int c,d;
	int base=MatAry[reg].Base;
	(*dimB)=-1;
	c=SRC[ExecPtr];
	d=SRC[ExecPtr+1];
	if ( ( d == ',' ) || ( d == ']' ) ) {							// [a,
		ExecPtr++ ;
		(*dimA) = MatOperandSub( c );
	} else
	if ( d == 0xFFFFFF89 ) { 										// [a+1,
		d=SRC[ExecPtr+3];
		if ( ( d == ',' ) || ( d == ']' ) ) {
			ExecPtr+=2 ;
			(*dimA) = MatOperandSub( c );
			c=SRC[ExecPtr++];
			(*dimA) += MatOperandSub( c );
		} else goto L1;
	} else
	if ( d == 0xFFFFFF99 ) { 										// [a-1,
		c=SRC[ExecPtr+3];
		if ( ( d == ',' ) || ( d == ']' ) ) {
			ExecPtr+=2 ;
			(*dimA) = MatOperandSub( c );
			c=SRC[ExecPtr++];
			(*dimA) -= MatOperandSub( c );
		} else goto L1;
	} else {
  L1:	(*dimA)=(EvalsubTop( SRC ));
	}
	if ( ( (*dimA) < base ) || ( MatAry[reg].SizeA-1+base < (*dimA) ) ) { CB_Error(DimensionERR); return ; }	// Dimension error
	c=SRC[ExecPtr];
	if ( c == ']' ) { ExecPtr++ ; (*dimB)=base; return; }
	else 
	if ( c != ',' ) { CB_Error(SyntaxERR); return ; }	// Syntax error
	c=SRC[++ExecPtr];
	d=SRC[ExecPtr+1];
	if ( d == ']' ) {										//    b]
		ExecPtr++ ;
		(*dimB) = MatOperandSub( c );
	} else
	if ( d == 0xFFFFFF89 ) { 										//      b+1]
		if ( SRC[ExecPtr+3] == ']' ) {
			ExecPtr+=2 ;
			(*dimB) = MatOperandSub( c );
			c=SRC[ExecPtr++];
			(*dimB) += MatOperandSub( c );
		} else goto L2;
	} else
	if ( d == 0xFFFFFF99 ) { 										//      b-1]
		if ( SRC[ExecPtr+3] == ']' ) {
			ExecPtr+=2 ;
			(*dimB) = MatOperandSub( c );
			c=SRC[ExecPtr++];
			(*dimB) -= MatOperandSub( c );
		} else goto L2;
	} else {
  L2:	(*dimB)=(EvalsubTop( SRC ));
	}
	if ( ( (*dimB) < base ) || ( MatAry[reg].SizeB-1+base < (*dimB) ) ) { CB_Error(DimensionERR); return ; }	// Dimension error
	if ( SRC[ExecPtr] != ']' ) { CB_Error(SyntaxERR); return ; }	// Syntax error
	ExecPtr++ ;
}

int MatrixOprandreg( char *SRC, int *reg) {	// 0-
	int c;
//	if ( ( SRC[ExecPtr] != 0x7F ) || ( SRC[ExecPtr+1]!=0x40 ) ) { CB_Error(SyntaxERR); return 0 ; }	// Syntax error
//	ExecPtr+=2;
	if ( ( SRC[ExecPtr] == 0x7F ) && ( SRC[ExecPtr+1] == 0x40 ) ) ExecPtr+=2;	// skip 'Mat '
	c =SRC[ExecPtr];
	if ( ( 'A' <= c ) && ( c <= 'z' ) ) {
		ExecPtr++;
		*reg=c-'A';
		if ( MatAry[*reg].SizeA == 0 ) { CB_Error(NoMatrixArrayERR); return 0; }	// No Matrix Array error
	} else { CB_Error(SyntaxERR); return 0; }	// Syntax error
	return 1;
}

int MatrixOprand( char *SRC, int *reg, int *dimA, int *dimB  ) {	// base:0  0-    base:1 1-
	int c;
	if ( MatrixOprandreg( SRC, &(*reg)) == 0 ) return 0 ;
	c =SRC[ExecPtr];
	if ( SRC[ExecPtr] != '[' ) { (*dimA)=MatAry[(*reg)].Base; (*dimB)=(*dimA); return -1 ; }	// no element
	ExecPtr++;
	if ( CB_INT ) MatOprandInt2( SRC, *reg, &(*dimA), &(*dimB));else MatOprand2( SRC, *reg, &(*dimA), &(*dimB));
	return 1;
}

//----------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------
double CB_EvalDbl( char *SRC ) {
	double value;
	if (CB_INT) {
		if ( SRC[ExecPtr]=='#' ) { ExecPtr++;
			value=EvalsubTop( SRC ); 
		}
		else value=EvalIntsubTop( SRC ); 
	} else	 value=EvalsubTop( SRC ); 
	return value;
}

double EvalsubTop( char *SRC ) {	// eval 1
	double  result,dst;
	int c;
	int excptr=ExecPtr;

//	while ( SRC[ExecPtr]==0x20 ) ExecPtr++; // Skip Space
	result=Evalsub1(SRC);
	c=SRC[ExecPtr];
	if ( ( c==':' ) || ( c==0x0E ) || ( c==0x13 ) || ( c==0x0D ) || ( c==',' ) || ( c==')' ) || ( c==']' ) ) return result;
	else 
	if ( c==0xFFFFFF89 ) { // +
		ExecPtr++; dst=Evalsub1(SRC); c=SRC[ExecPtr];
		if ( ( c==':' ) || ( c==0x0E ) || ( c==0x13 ) || ( c==0x0D ) || ( c==',' ) || ( c==')' ) || ( c==']' ) ) return result+dst;
	} else
	if ( c==0xFFFFFF99 ) { // -
		ExecPtr++; dst=Evalsub1(SRC); c=SRC[ExecPtr];
		if ( ( c==':' ) || ( c==0x0E ) || ( c==0x13 ) || ( c==0x0D ) || ( c==',' ) || ( c==')' ) || ( c==']' ) ) return result-dst;
	} else
	if ( c=='=') { // ==
		ExecPtr++; dst=Evalsub1(SRC); c=SRC[ExecPtr];
		if ( ( c==':' ) || ( c==0x0E ) || ( c==0x13 ) || ( c==0x0D ) || ( c==',' ) || ( c==')' ) || ( c==']' ) ) return result == dst;
	} else
	if ( c=='>') { // >
		ExecPtr++; dst=Evalsub1(SRC); c=SRC[ExecPtr];
		if ( ( c==':' ) || ( c==0x0E ) || ( c==0x13 ) || ( c==0x0D ) || ( c==',' ) || ( c==')' ) || ( c==']' ) ) return result > dst;
	} else
	if ( c=='<') { // <
		ExecPtr++; dst=Evalsub1(SRC); c=SRC[ExecPtr];
		if ( ( c==':' ) || ( c==0x0E ) || ( c==0x13 ) || ( c==0x0D ) || ( c==',' ) || ( c==')' ) || ( c==']' ) ) return result < dst;
	} else
	if ( c==0x11) { // !=
		ExecPtr++; dst=Evalsub1(SRC); c=SRC[ExecPtr];
		if ( ( c==':' ) || ( c==0x0E ) || ( c==0x13 ) || ( c==0x0D ) || ( c==',' ) || ( c==')' ) || ( c==']' ) ) return result != dst;
	} else
	if ( c==0x12) { // >=
		ExecPtr++; dst=Evalsub1(SRC); c=SRC[ExecPtr];
		if ( ( c==':' ) || ( c==0x0E ) || ( c==0x13 ) || ( c==0x0D ) || ( c==',' ) || ( c==')' ) || ( c==']' ) ) return result >= dst;
	} else
	if ( c==0x10) { // <=
		ExecPtr++; dst=Evalsub1(SRC); c=SRC[ExecPtr];
		if ( ( c==':' ) || ( c==0x0E ) || ( c==0x13 ) || ( c==0x0D ) || ( c==',' ) || ( c==')' ) || ( c==']' ) ) return result <= dst;
	} else
	if ( c==0xFFFFFFA9 ) { // ~
		ExecPtr++; dst=Evalsub1(SRC); c=SRC[ExecPtr];
		if ( ( c==':' ) || ( c==0x0E ) || ( c==0x13 ) || ( c==0x0D ) || ( c==',' ) || ( c==')' ) || ( c==']' ) ) return result*dst;
	} else
	if ( c==0xFFFFFFB9 ) { // €
		ExecPtr++; dst=Evalsub1(SRC); c=SRC[ExecPtr];
		if ( dst == 0 ) CB_Error(DivisionByZeroERR); // Division by zero error
		if ( ( c==':' ) || ( c==0x0E ) || ( c==0x13 ) || ( c==0x0D ) || ( c==',' ) || ( c==')' ) || ( c==']' ) ) return result/dst;
	} else
	if ( c==0xFFFFFF8B ) { // ^2
		c=SRC[++ExecPtr];
		if ( ( c==':' ) || ( c==0x0E ) || ( c==0x13 ) || ( c==0x0D ) || ( c==',' ) || ( c==')' ) || ( c==']' ) ) return result*result;
	} else
	if ( c==0xFFFFFF9B ) { // ^(-1) RECIP
		if ( result == 0 ) CB_Error(DivisionByZeroERR); // Division by zero error
		c=SRC[++ExecPtr];
		if ( ( c==':' ) || ( c==0x0E ) || ( c==0x13 ) || ( c==0x0D ) || ( c==',' ) || ( c==')' ) || ( c==']' ) ) return 1/result;
	} else
	if ( c==0x7F ) { // 
		c=SRC[++ExecPtr];
		if ( c==0xFFFFFFB0 ) { // And
			ExecPtr++; dst=Evalsub1(SRC); c=SRC[ExecPtr];
			if ( ( c==':' ) || ( c==0x0E ) || ( c==0x13 ) || ( c==0x0D ) || ( c==',' ) || ( c==')' ) || ( c==']' ) ) return (int)result & (int)dst;
		} else
		if ( c==0xFFFFFFB1 ) { // Or
			ExecPtr++; dst=Evalsub1(SRC); c=SRC[ExecPtr];
			if ( ( c==':' ) || ( c==0x0E ) || ( c==0x13 ) || ( c==0x0D ) || ( c==',' ) || ( c==')' ) || ( c==']' ) ) return (int)result | (int)dst;
		} else
		if ( c==0xFFFFFFB4 ) { // Xor
			ExecPtr++; dst=Evalsub1(SRC); c=SRC[ExecPtr];
			if ( ( c==':' ) || ( c==0x0E ) || ( c==0x13 ) || ( c==0x0D ) || ( c==',' ) || ( c==')' ) || ( c==']' ) ) return (int)result ^ (int)dst;
		}
	}
	
	ExecPtr=excptr;
	result = Evalsub13( SRC );
	while ( 1 ) {
		c = SRC[ExecPtr];
		if ( c == 0x7F ) {
			c = SRC[ExecPtr+1];
			switch ( c ) {
				case 0xFFFFFFB1 :	// Or
					ExecPtr+=2;
					result = ( (int)result | (int)Evalsub13( SRC ) );
					break;
				case 0xFFFFFFB4 :	// Xor
					ExecPtr+=2;
					result = ( (int)result ^ (int)Evalsub13( SRC ) );
					break;
				default:
					return result;
					break;
			}
		} else return result;
	}
}
//----------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------
int MatrixObjectAlignE4c( unsigned int n ){ return n; }	// align +4byte
//int MatrixObjectAlignE4d( unsigned int n ){ return n+n; }	// align +6byte
//-----------------------------------------------------------------------------
/*
int lastrandom=0x12345678;
unsigned int random( int seed  ){
    if (seed) lastrandom=seed;
    lastrandom = ( 0x41C64E6D*lastrandom ) + 0x3039; // + RTC_GetTicks();
    return ( ( lastrandom >> 16 ) & 0xFFFF );
}
*/

/*
#define floor2 floor2
double floor2( double x ) {
	double result;
	unsigned char *row;
	int tmp,exp;
	int i,j,bt;
	unsigned char *ptr;

	ptr=(unsigned char *)(&x);
	row=(unsigned char *)(&result);
	result=x;

	tmp= ( (row[0]&0x7f)<<4 ) + ( (row[1]&0xf0)>>4 ) ;
	exp=tmp-1023; // 
	if ( ( 0 <= exp ) && ( exp <= 51 ) ) {
		tmp=52-exp;
		i=7; j=0; bt=0xFE;
		while ( tmp ) {
			if (tmp>7) {
				 tmp-=8; row[i]=0; i--;
			} else {
				row[i]&=bt;
				bt=bt<<1;
				j++;
				tmp--;
			}
		}
	}
	return ( result ); 
}
*/
double frac( double x ) {
	double sign=1,tmp,d;
	if ( x <0 ) { sign=-1; x=-x; }
	if ( x<1e1 ) d= 1e15;
	else if ( x>=1e15 ) d= 1;
	else if ( x>=1e14 ) d= 1e1;
	else if ( x>=1e13 ) d= 1e2;
	else if ( x>=1e12 ) d= 1e3;
	else if ( x>=1e11 ) d= 1e4;
	else if ( x>=1e10 ) d= 1e5;
	else if ( x>=1e9 ) d= 1e6;
	else if ( x>=1e8 ) d= 1e7;
	else if ( x>=1e7 ) d= 1e8;
	else if ( x>=1e6 ) d= 1e9;
	else if ( x>=1e5 ) d= 1e10;
	else if ( x>=1e4 ) d= 1e11;
	else if ( x>=1e3 ) d= 1e12;
	else if ( x>=1e2 ) d= 1e13;
	else if ( x>=1e1 ) d= 1e14;
	tmp=x-floor(x);
	return (floor(tmp*d+.5)/d*sign) ;
}

double asinh( double x ) {
	return ( (exp(x)+exp(-x))/2. );
}
double acosh( double x ) {
	return ( (exp(x)-exp(-x))/2. );
}
double atanh( double x ) {
	double ep=exp(x);
	double em=exp(-x);
	return ( (ep-em)/(ep+em) );
}

void CheckMathERR( double *result ) {
	char * pt;
	pt=(char *)(result); if (pt[1]==0xFFFFFFF0) if ( (pt[0]==0x7F)||(pt[0]==0xFFFFFFFF) ) CB_Error(MathERR) ; // Math error
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
int MatrixObjectAlignE4e( unsigned int n ){ return n ; }	// align +4byte
//int MatrixObjectAlignE4f( unsigned int n ){ return n+n; }	// align +6byte
//-----------------------------------------------------------------------------

unsigned int Eval_atofNumDiv(char *SRC, int c, double *num ){
	double a=.1;
	while ( ('0'<=c)&&(c<='9') ) {
		(*num) = (*num) + (double)(c-'0')*a;
		a*=.1;
		c=SRC[++ExecPtr];
	}
	return c;
}
unsigned int Eval_atofNumMult(char *SRC, int c, double *num ){
	while ( ('0'<=c)&&(c<='9') ) {
		(*num) = (*num)*10 +(c-'0');
		c=SRC[++ExecPtr];
	}
	return c;
}
double Eval_atof(char *SRC, int c) {
	double mantissa=0,exponent=0;
	double sign=1;
	int d;
		if ( c == '.'  )   {	// .123456
				c = SRC[++ExecPtr];
				c=Eval_atofNumDiv(SRC, c, &mantissa);
		} else if ( c == 0x0F  ) {  // exp
				mantissa = 1.0;
				goto lblexp;
		} else { 	// 123456
			if ( c == '0' ) {
				d = SRC[ExecPtr+1];
				if ( (  d=='x' ) || ( d=='X' ) || (  d=='b' ) || ( d=='B' ) ) return Eval_atod( SRC, c );
			}
			c=Eval_atofNumMult(SRC, c, &mantissa);	// 123456
			if ( c == '.'  ) {
				c = SRC[++ExecPtr];
				c=Eval_atofNumDiv(SRC, c, &mantissa);	// 123456.789
			}
		}
		lblexp:
		if ( ( c == 0x0F  ) || ( c == 'E'  ) || ( c == 'e'  ) ){ // exp
				c=SRC[++ExecPtr];
				if ( ( c == 0xFFFFFF89 ) || ( c == '+'  ) ) c=SRC[++ExecPtr];
				if ( ( c == 0xFFFFFF87 ) || ( c == 0xFFFFFF99 ) ) { sign=-1; c=SRC[++ExecPtr]; }	// (-) & -
				if ( ( '0'<=c )&&( c<='9' ) ) c=Eval_atofNumMult(SRC, c, &exponent);
				else { ErrorNo=SyntaxERR; ErrorPtr=ExecPtr; } //  error 
			return mantissa * pow(10,exponent*sign) ;
		} else
			return mantissa ;
}


//-----------------------------------------------------------------------------

double Evalsub1(char *SRC) {	// 1st Priority
	double result=0,tmp,tmp2;
	double sign=1;
	int c,d;
	char *pt;
	int dimA,dimB,reg,x,y;
	int i,ptr,mptr;
	char*	MatAryC;
	short*	MatAryW;
	int*	MatAryI;
	double*	MatAryF;

	c = SRC[ExecPtr++];
	if ( c == '(') {
		result = EvalsubTop( SRC );
		if ( SRC[ExecPtr] == ')' ) ExecPtr++;
		return result;
	}
	while ( c == 0xFFFFFF89 ) c=SRC[ExecPtr++];	// +
	if ( ( c == 0xFFFFFF87 ) || ( c == 0xFFFFFF99 ) ) {	//  -
		result = - Evalsub1( SRC );
		return result;
	}
	if ( ( 'A' <= c )&&( c <= 'Z' ) )  {
			reg=c-'A';
			c=SRC[ExecPtr];
			if ( c=='%' ) { ExecPtr++; return REGINT[reg] ; }
			else
			if ( c=='[' ) goto Matrix;
			else
			if ( c=='#' ) ExecPtr++;
			return REG[reg] ;
	}
	if ( ( 'a' <= c )&&( c <= 'z' ) )  {
			reg=c-'a';
			c=SRC[ExecPtr];
			if ( c=='%' ) { ExecPtr++; return LocalInt[reg][0]; }
			else
			if ( c=='[' ) goto Matrix;
			else
			if ( c=='#' ) ExecPtr++;
			return LocalDbl[reg][0];
	}
	if ( ( c=='.' ) ||( c==0x0F ) || ( ( '0'<=c )&&( c<='9' ) ) ) {
		ExecPtr--; return Eval_atof( SRC , c );
	}
	switch ( c ) { 			// ( type C function )  sin cos tan... 
		case 0x7F:	// 7F..
			c = SRC[ExecPtr++];
			if ( c == 0x40 ) {	// Mat A[a,b]
				c=SRC[ExecPtr]; if ( ( 'A'<=c )&&( c<='z' ) ) { reg=c-'A'; ExecPtr++; } else CB_Error(SyntaxERR) ; // Syntax error 
				if ( SRC[ExecPtr] == '[' ) {
				Matrix:	
					ExecPtr++;
					MatOprand2( SRC, reg, &dimA, &dimB );
					if ( ErrorNo ) return 1 ; // error
				} else { dspflag=3; dimA=MatAry[reg].Base; dimB=dimA; }	// Mat A
				return ReadMatrix( reg, dimA, dimB);
					
			} else if ( c == 0x3A ) {	// MOD(a,b)
					tmp = floor(EvalsubTop( SRC ) +.5);
					if ( SRC[ExecPtr] != ',' ) CB_Error(SyntaxERR) ; // Syntax error 
					ExecPtr++;
					tmp2 = floor( EvalsubTop( SRC ) +.5);
					if ( tmp2 == 0 )  CB_Error(DivisionByZeroERR); // Division by zero error 
					result= floor(fabs(fmod( tmp, tmp2 ))+.5);
					if ( result == tmp2  ) result--;
					if ( tmp < 0 ) {
						result=fabs(tmp2)-result;
						if ( result == tmp2  ) result=0;
					}
					if ( SRC[ExecPtr] == ')' ) ExecPtr++;
					return result ;
					
			} else if ( c == 0xFFFFFF8F ) {	// Getkey
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
					} else	result = CB_Getkey() ;
					return 	result ;
					
			} else if ( c == 0xFFFFFF87 ) {	// RanInt#(st,en)
					x=EvalsubTop( SRC );
					if ( SRC[ExecPtr] != ',' ) CB_Error(SyntaxERR) ; // Syntax error 
					ExecPtr++ ;	// ',' skip
					y=EvalsubTop( SRC );
					if ( SRC[ExecPtr] == ')' ) ExecPtr++;
//					if ( x>=y ) CB_Error(ArgumentERR);  // Argument error
					if ( x>y ) { i=x; x=y; y=i; }
					return rand()*(y-x+1)/(RAND_MAX+1) +x ;
					
			} else if ( c == 0xFFFFFFE9 ) {	// CellSum(Mat A[x,y])
					MatrixOprand( SRC, &reg, &x, &y );
					if ( ErrorNo ) return ; // error
					if ( SRC[ExecPtr] == ')' ) ExecPtr++;
					return Cellsum( reg, x, y );

			}else if ( c == 0x5F ) {	// 1/128 Ticks
					return RTC_GetTicks()-CB_TicksAdjust;	// 
					
			} else if ( c == 0xFFFFFF86 ) {	// RndFix(n,digit)
					tmp=(EvalsubTop( SRC ));
					if ( SRC[ExecPtr] != ',' ) CB_Error(SyntaxERR) ; // Syntax error 
					ExecPtr++ ;	// ',' skip
					i = EvalsubTop( SRC ) +.5;
					if ( SRC[ExecPtr] == ')' ) ExecPtr++;
					result=Round( tmp, Fix, i) ;
					return result ;
					
			} else if ( c == 0xFFFFFFF0 ) {	// GraphY
					reg='y'-'A';
					dimA=SRC[ExecPtr++]-'0';
					if ( ( dimA < 1 ) || ( dimA > MatAry[reg].SizeA ) ) { CB_Error(ArgumentERR); return 0; }  // Argument error
					ptr=ExecPtr;
					ExecPtr= 0;
					result= EvalsubTop( MatrixPtr( reg, dimA, 1 ) );
					ExecPtr=ptr;
					return result ;
					
			} else if ( c == 0xFFFFFFF5 ) {	// IsExist(
					return  CB_IsExist( SRC );
			} else if ( c == 0x00 ) {	// Xmin
					return Xmin;
			} else if ( c == 0x01 ) {	// Xmax
					return Xmax;
			} else if ( c == 0x02 ) {	// Xscl
					return Xscl;
			} else if ( c == 0x04 ) {	// Ymin
					return Ymin;
			} else if ( c == 0x05 ) {	// Ymax
					return Ymax;
			} else if ( c == 0x06) {	// Yscl
					return Yscl;
			} else if ( c == 0x0B ) {	// Xfct
					return Xfct;
			} else if ( c == 0x0C ) {	// Yfct
					return Yfct;
			} else if ( c == 0x58 ) {	// ElemSize( Mat A )
					MatrixOprandreg( SRC, &reg );
					if ( SRC[ExecPtr] == ')' ) ExecPtr++;
					return MatAry[reg].ElementSize;
			} else if ( c == 0x59 ) {	// ColSize( Mat A )
					MatrixOprandreg( SRC, &reg );
					if ( SRC[ExecPtr] == ')' ) ExecPtr++;
					return MatAry[reg].SizeA;
			} else if ( c == 0x5A ) {	// RowSize( Mat A )
					MatrixOprandreg( SRC, &reg );
					if ( SRC[ExecPtr] == ')' ) ExecPtr++;
					return MatAry[reg].SizeB;
			} else ExecPtr--;	// error
			break;
			
		case 0xFFFFFFC0 :	// Ans
			return CB_CurrentValue ;
		case 0xFFFFFFD0 :	// PI
			return PI ;
		case 0xFFFFFFC1 :	// Ran#
			c = SRC[ExecPtr];
			if ( ( '0' <= c ) && ( c <= '9' ) ) srand( Eval_atod( SRC, c ) );
			result=(double)rand()/(double)(RAND_MAX+1.0);
			return result ;
		case 0xFFFFFF97 :	// abs
			result = fabs( Evalsub5( SRC ) );
			return result ;
		case 0xFFFFFFA6 :	// int
			result = floor( Evalsub5( SRC ) );
			return result ;
		case 0xFFFFFFB6 :	// frac
			result = frac( Evalsub5( SRC ) );
			return result ;
		case 0xFFFFFFA7 :	// Not
			result = ! (int) ( Evalsub5( SRC ) );
			return result ;

		case '%' :	// 1/128 Ticks
			return RTC_GetTicks()-CB_TicksAdjust;	// 
		case 0xFFFFFFF7:	// F7..
			c = SRC[ExecPtr++];
			if ( c == 0xFFFFFFAF ) {	// PxlTest(y,x)
					y=(EvalsubTop( SRC ));
					if ( SRC[ExecPtr] != ',' ) CB_Error(SyntaxERR) ; // Syntax error 
					ExecPtr++ ;	// ',' skip
					x=(EvalsubTop( SRC ));
					if ( SRC[ExecPtr] == ')' ) ExecPtr++;
					result = PxlTest(y, x) ;			// 
					return result ;
			} else
			if ( c == 0xFFFFFFFE ) {	// BackLight
				return	BackLight(-1);
			} else ExecPtr--;	// error
			break;

		case 0xFFFFFF86 :	// sqr
			result = sqrt( Evalsub5( SRC ) );
			CheckMathERR(&result); // Math error ?
			return result ;
		case 0xFFFFFF95 :	// log10
			result = log10( Evalsub5( SRC ) );
			CheckMathERR(&result); // Math error ?
			return result ;
		case 0xFFFFFFB5 :	// 10^
			result = pow(10, Evalsub5( SRC ) );
			CheckMathERR(&result); // Math error ?
			return result ;
		case 0xFFFFFF85 :	// ln
			result = log( Evalsub5( SRC ) );
			CheckMathERR(&result); // Math error ?
			return result ;
		case 0xFFFFFFA5 :	// expn
			result = exp( Evalsub5( SRC ) );
			CheckMathERR(&result); // Math error ?
			return result ;
		case 0xFFFFFF96 :	// cuberoot
			result = pow( Evalsub5( SRC ), 1.0/3.0 );
			CheckMathERR(&result); // Math error ?
			return result ;

		case 0xFFFFFF81 :	// sin
			switch ( Angle ) { 
				case 0:	// Deg
					result = sin( Evalsub5( SRC )*PI/180.);
					break;
				case 1:	// Rad
					result = sin( Evalsub5( SRC ));
					break;
				case 2:	// Grad
					result = sin( Evalsub5( SRC )*PI/200. );
					break;
			}
			CheckMathERR(&result); // Math error ?
			return result ;
		case 0xFFFFFF82 :	// cos
			switch ( Angle ) { 
				case 0:	// Deg
					result = cos( Evalsub5( SRC )*PI/180.);
					break;
				case 1:	// Rad
					result = cos( Evalsub5( SRC ));
					break;
				case 2:	// Grad
					result = cos( Evalsub5( SRC )*PI/200. );
					break;
			}
			CheckMathERR(&result); // Math error ?
			return result ;
		case 0xFFFFFF83 :	// tan
			switch ( Angle ) { 
				case 0:	// Deg
					result = tan( Evalsub5( SRC )*PI/180.);
					break;
				case 1:	// Rad
					result = tan( Evalsub5( SRC ));
					break;
				case 2:	// Grad
					result = tan( Evalsub5( SRC )*PI/200. );
					break;
			}
			CheckMathERR(&result); // Math error ?
			return result ;
		case 0xFFFFFF91 :	// asin
			switch ( Angle ) { 
				case 0:	// Deg
					result = asin( Evalsub5( SRC ) )*180./PI ;
					break;
				case 1:	// Rad
					result = asin( Evalsub5( SRC ) ) ;
					break;
				case 2:	// Grad
					result = asin( Evalsub5( SRC ) )*200./PI ;
					break;
			}
			CheckMathERR(&result); // Math error ?
			return result ;
		case 0xFFFFFF92 :	// acos
			switch ( Angle ) { 
				case 0:	// Deg
					result = acos( Evalsub5( SRC ) )*180./PI ;
					break;
				case 1:	// Rad
					result = acos( Evalsub5( SRC ) ) ;
					break;
				case 2:	// Grad
					result = acos( Evalsub5( SRC ) )*200./PI ;
					break;
			}
			CheckMathERR(&result); // Math error ?
			return result ;
		case 0xFFFFFF93 :	// atan
			switch ( Angle ) { 
				case 0:	// Deg
					result = atan( Evalsub5( SRC ) )*180./PI ;
					break;
				case 1:	// Rad
					result = atan( Evalsub5( SRC ) ) ;
					break;
				case 2:	// Grad
					result = atan( Evalsub5( SRC ) )*200./PI ;
					break;
			}
			CheckMathERR(&result); // Math error ?
			return result ;
		case 0xFFFFFFA1 :	// sinh
			result = sinh( Evalsub5( SRC ) );
			CheckMathERR(&result); // Math error ?
			return result ;
		case 0xFFFFFFA2 :	// cosh
			result = cosh( Evalsub5( SRC ) );
			CheckMathERR(&result); // Math error ?
			return result ;
		case 0xFFFFFFA3 :	// tanh
			result = tanh( Evalsub5( SRC ) );
			CheckMathERR(&result); // Math error ?
			return result ;
		case 0xFFFFFFB1 :	// asinh
			result = asinh( Evalsub5( SRC ) );
			CheckMathERR(&result); // Math error ?
			return result ;
		case 0xFFFFFFB2 :	// acosh
			result = acosh( Evalsub5( SRC ) );
			CheckMathERR(&result); // Math error ?
			return result ;
		case 0xFFFFFFB3 :	// atanh
			result = atanh( Evalsub5( SRC ) );
			CheckMathERR(&result); // Math error ?
			return result ;
		case 0xFFFFFFF9:	// F9..
			c = SRC[ExecPtr++];
			if ( c == 0x31 ) {	// StrLen(
					return CB_StrLen( SRC );
			} else
			if ( c == 0x32 ) {	// StrCmp(
					return CB_StrCmp( SRC );
			} else
			if ( c == 0x33 ) {	// StrSrc(
					return CB_StrSrc( SRC );
			} else
			if ( c == 0x38 ) {	// Exp(
					return CB_EvalStr(SRC);
			} else
			if ( c == 0x21 ) {	// Xdot
					return Xdot;
			} else ExecPtr--;	// error
			break;
		case 0xFFFFFFDD :	// Eng
			return ENG ;
		case 0xFFFFFFFD:	//  Eval(
			return CB_EvalStr(SRC);
		default:
			break;
	}
	ExecPtr--;
	CB_Error(SyntaxERR) ; // Syntax error 
	return 0 ;
}

/*
double Evaldummy1(char *SRC, int c, int *num){
	double a=.1,result;
	while ( ('0'<=c)&&(c<='9') ) {
		(*num) = (*num) + (double)(c-'0')*a;
		a*=.1;
		c=SRC[++ExecPtr];
	}
	result = sin( Evalsub1( SRC )*PI/180.);
	return c+result;
}
*/
double DmsToDec( char *SRC, double h ) {
	double m,s,f=1;
	if ( h<0 ) { f=-1; h=-h; }
	m=Evalsub1(SRC);
	if ( SRC[ExecPtr] != 0xFFFFFF8C ) return (h + m/60)*f;
	ExecPtr++;
	s=Evalsub1(SRC);
	if ( SRC[ExecPtr] == 0xFFFFFF8C ) ExecPtr++;
	return (h + m/60 + s/3600)*f ;
}
//-----------------------------------------------------------------------------
int MatrixObjectAlignE4g( unsigned int n ){ return n ; }	// align +4byte
//int MatrixObjectAlignE4h( unsigned int n ){ return n+n; }	// align +6byte
//-----------------------------------------------------------------------------

double Evalsub2(char *SRC) {	//  2nd Priority  ( type B function ) ...
	int cont=1;
	double result,tmp;
	int c;
	result = Evalsub1( SRC );
	while ( 1 ) {
		c = SRC[ExecPtr++];
		switch ( c ) {
			case  0xFFFFFF8B  :	// ^2
				result *= result ;
				break;
			case  0xFFFFFF9B  :	// ^(-1) RECIP
				if ( result == 0 ) CB_Error(DivisionByZeroERR); // Division by zero error 
				result = 1 / result ;
				break;
			case  0xFFFFFFAB  :	//  !
				tmp = floor( result );
				result = 1;
				while ( tmp > 0 ) { result *= tmp; tmp--; }
				break;
				
			case  0x01  :	//  femto
				result *= 1e-15 ;
				break;
			case  0x02  :	//  pico
				result *= 1e-12 ;
				break;
			case  0x03  :	//  nano
				result *= 1e-9 ;
				break;
			case  0x04  :	//  micro
				result *= 1e-6 ;
				break;
			case  0x05  :	//  milli
				result *= 1e-3 ;
				break;
			case  0x06  :	//  Kiro
				result *= 1e3 ;
				break;
			case  0x07  :	//  Mega
				result *= 1e6 ;
				break;
			case  0x08  :	//  Giga
				result *= 1e9 ;
				break;
			case  0x09  :	//  Tera
				result *= 1e12 ;
				break;
			case  0x0A  :	//  Peta
				result *= 1e15 ;
				break;
			case  0x1B  :	//  Exa
				result *= 1e18 ;
				break;
				
			case  0xFFFFFF8C  :	//  dms
				result = DmsToDec( SRC, result );
				break;
				
			case  0xFFFFFF9C  :	//  Deg
				switch ( Angle ) { 
					case 0:	// Deg
						break;
					case 1:	// Rad
						result = result *(PI/180.);
						break;
					case 2:	// Grad
						result = result *(200./180.);
						break;
				}
				break;
			case  0xFFFFFFAC  :	//  Rad
				switch ( Angle ) { 
					case 0:	// Deg
						result = result *(180./PI);
						break;
					case 1:	// Rad
						break;
					case 2:	// Grad
						result = result *(200./PI);
						break;
				}
				break;
			case  0xFFFFFFBC  :	//  Grad
				switch ( Angle ) { 
					case 0:	// Deg
						result = result *(180./200.);
						break;
					case 1:	// Rad
						result = result *(PI/200.);
						break;
					case 2:	// Grad
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
double Evalsub4(char *SRC) {	//  3rd Priority  ( ^ ...)
	double result,tmp;
	int c;
	char *pt;
	result = Evalsub2( SRC );
	while ( 1 ) {
		c = SRC[ExecPtr++];
		switch ( c ) {
			case  0xFFFFFFA8  :	// a ^ b
				result = pow( result, Evalsub2( SRC ) );
				CheckMathERR(&result); // Math error ?
				break;
			case  0xFFFFFFB8  :	// powroot
				result = pow( Evalsub2( SRC ), 1/result );
				CheckMathERR(&result); // Math error ?
				break;
			default:
				ExecPtr--;
				return result;
				break;
		}
	 }
	return result;
}
double Evalsub5(char *SRC) {	//  5th Priority
	double result,tmp;
	int c;
	int dimA,dimB,reg,x,y;
	result = Evalsub4( SRC );
	while ( 1 ) {
		c = SRC[ExecPtr];
		if ((( 'A' <= c ) && ( c <= 'Z' )) ||
			(( 'a' <= c ) && ( c <= 'z' )) ||
			 ( c == 0xFFFFFFD0 ) || // PI
			 ( c == 0xFFFFFFC0 ) || // Ans
			 ( c == 0xFFFFFFC1 )) { // Ran#
				result *= Evalsub4( SRC ) ;
		} else if ( c == 0x7F ) { // 7F..
			c = SRC[ExecPtr+1];
			switch ( c ) {
				case 0x40:	// Mat A[a,b]
				case 0x3A:	// MOD(a,b)
				case 0xFFFFFF8F:	// Getkey
				case 0xFFFFFF86:	// RndFix(n,digit)
				case 0xFFFFFF87:	// RanInt#(st,en)
				case 0xFFFFFFF0:	// GraphY
				case 0x00:	// Xmin
				case 0x01:	// Xmax
				case 0x02:	// Xscl
				case 0x04:	// Ymin
				case 0x05:	// Ymax
				case 0x06:	// Yscl
					result *= Evalsub4( SRC ) ;
					break;
				default:
					return result;
					break;
			}
		} else if ( c == 0xFFFFFFF7 ) { // F7..
			c = SRC[ExecPtr+1];
			switch ( c ) {
				case 0xFFFFFFAF:	// PxlTest(y,x)
					result *= Evalsub4( SRC ) ;
					break;
				default:
					return result;
					break;
			}
		} else if ( c == 0xFFFFFFF9 ) { // F9..
			c = SRC[ExecPtr+1];
			switch ( c ) {
				case 0x21:	// Xdot
					result *= Evalsub4( SRC ) ;
					break;
				default:
					return result;
					break;
			}
		} else if ( c == 0xFFFFFFE7 ) { // E7..
			c = SRC[ExecPtr+1];
			switch ( c ) {
				default:
					return result;
					break;
			}
		} else return result;
	 }
}
double Evalsub7(char *SRC) {	//  7th Priority
	double result,tmp;
	int c;
	result = Evalsub5( SRC );
	while ( 1 ) {
		c = SRC[ExecPtr];
		switch ( c ) {
			case '(' :
			case 0xFFFFFF97 :	// abs
			case 0xFFFFFFA6 :	// int
			case 0xFFFFFFB6 :	// frac
			case 0xFFFFFF85 :	// ln
			case 0xFFFFFF86 :	// sqr
			case 0xFFFFFF95 :	// log10
			case 0xFFFFFF96 :	// cuberoot
			case 0xFFFFFFA5 :	// expn
			case 0xFFFFFFA7 :	// Not
			case 0xFFFFFFB5 :	// 10^
			case 0xFFFFFFB7 :	// Neg
			case 0xFFFFFF81 :	// sin
			case 0xFFFFFF82 :	// cos
			case 0xFFFFFF83 :	// tan
			case 0xFFFFFF91 :	// asin
			case 0xFFFFFF92 :	// acos
			case 0xFFFFFF93 :	// atan
			case 0xFFFFFFA1 :	// sinh
			case 0xFFFFFFA2 :	// cosh
			case 0xFFFFFFA3 :	// tanh
			case 0xFFFFFFB1 :	// asinh
			case 0xFFFFFFB2 :	// acosh
			case 0xFFFFFFB3 :	// atanh
				result *= Evalsub5( SRC );
				break;
			default:
				return result;
				break;
		}
	 }
	return result;
}
double Evalsub10(char *SRC) {	//  10th Priority  ( *,/, int.,Rmdr )
	double result,tmp;
	int c;
	result = Evalsub7( SRC );
	while ( 1 ) {
		c = SRC[ExecPtr++];
		switch ( c ) {
			case 0xFFFFFFA9 :		// ~
				result *= Evalsub7( SRC );
				break;
			case 0xFFFFFFB9 :		// €
				tmp = Evalsub7( SRC );
				if ( tmp == 0 ) CB_Error(DivisionByZeroERR); // Division by zero error 
				result /= tmp ;
				break;
			default:
				ExecPtr--;
				return result;
				break;
		}
	 }
	return result;
}
double Evalsub11(char *SRC) {	//  11th Priority  ( +,- )
	double result;
	int c;
	result = Evalsub10( SRC );
	while ( 1 ) {
		c = SRC[ExecPtr++];
		switch ( c ) {
			case 0xFFFFFF89 :		// +
				result += Evalsub10( SRC );
				break;
			case 0xFFFFFF99 :		// -
				result -= Evalsub10( SRC );
				break;
			default:
				ExecPtr--;
				return result;
				break;
		}
	 }
	return result;
}
double Evalsub12(char *SRC) {	//  12th Priority ( =,!=,><,>=,<= )
	double result;
	int c;
	result = Evalsub11( SRC );
	while ( 1 ) {
		c = SRC[ExecPtr++];
		switch ( c ) {
			case '=' :	// =
				result = ( result == Evalsub11( SRC ) );
				break;
			case '>' :	// >
				result = ( result >  Evalsub11( SRC ) );
				break;
			case '<' :	// <
				result = ( result <  Evalsub11( SRC ) );
				break;
			case 0x11 :	// !=
				result = ( result != Evalsub11( SRC ) );
				break;
			case 0x12 :	// >=
				result = ( result >= Evalsub11( SRC ) );
				break;
			case 0x10 :	// <=
				result = ( result <= Evalsub11( SRC ) );
				break;
			default:
				ExecPtr--;
				return result;
				break;
		}
	 }
	return result;
}
double Evalsub13(char *SRC) {	//  13th Priority  ( And,and)
	double result;
	int c;
	result = Evalsub12( SRC );
	while ( 1 ) {
		c = SRC[ExecPtr];
		if ( c == 0x7F ) {
			c = SRC[ExecPtr+1];
			switch ( c ) {
				case 0xFFFFFFB0 :	// And
					ExecPtr+=2;
					result = ( (int)result & (int)Evalsub12( SRC ) );
					break;
				default:
					return result;
					break;
			}
		} else return result;
	}
}
/*
double EvalsubTop14(char *SRC) {	//  14th Priority  ( Or,Xor,or,xor,xnor )
	double result;
	int c;
	result = Evalsub13( SRC );
	while ( 1 ) {
		c = SRC[ExecPtr];
		if ( c == 0x7F ) {
			c = SRC[ExecPtr+1];
			switch ( c ) {
				case 0xFFFFFFB1 :	// Or
					ExecPtr+=2;
					result = ( (int)result | (int)Evalsub13( SRC ) );
					break;
				case 0xFFFFFFB4 :	// Xor
					ExecPtr+=2;
					result = ( (int)result ^ (int)Evalsub13( SRC ) );
					break;
				default:
					return result;
					break;
			}
		} else return result;
	}
}
*/


//----------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------
/*
double CB_BinaryEval( char *SRC ) {	// eval 2
	int c,op;
	int reg,mptr,opPtr;
	int dimA,dimB;
	double src,dst;
	char*	MatAryC;
	short*	MatAryW;
	int*	MatAryI;
	
	c=SRC[ExecPtr];
	if ( ( 'A' <= c ) && ( c <= 'z' ) ) {
		ExecPtr++;
		reg=c-'A';
		c=SRC[ExecPtr];
		if ( c=='%' ) {
			ExecPtr++;
			src = REGINT[reg] ;
		} else {
			if ( c=='#' ) ExecPtr++;
			src = REG[reg] ;
		}
	} else 
	if ( c==0x7F ) {
			MatrixOprand( SRC, &reg, &dimA, &dimB );
			if ( ErrorNo ) return ; // error
			src = ReadMatrix( reg, dimA, dimB );
	} else  src = Evalsub1(SRC);

	opPtr=ExecPtr;
	op=SRC[ExecPtr++];	
	c=SRC[ExecPtr];
	if ( ( 'A' <= c ) && ( c <= 'z' ) ) {
		ExecPtr++;
		reg=c-'A';
		c=SRC[ExecPtr];
		if ( c=='%' ) {
			ExecPtr++;
			dst = REGINT[reg] ;
		} else {
			if ( c=='#' ) ExecPtr++;
			dst = REG[reg] ;
		}
	} else 
	if ( c==0x7F ) {
			MatrixOprand( SRC, &reg, &dimA, &dimB );
			if ( ErrorNo ) return ; // error
			dst = ReadMatrix( reg, dimA, dimB );
	} else  dst = Evalsub1(SRC);

//	ExecPtr++;
	switch ( op ) {
			case 0xFFFFFF89 :		// +
				return src + dst;
			case 0xFFFFFF99 :		// -
				return src - dst;
			case 0xFFFFFFA9 :		// ~
				return src * dst;
			case 0xFFFFFFB9 :		// €
				if ( dst == 0 ) CB_Error(DivisionByZeroERR); // Division by zero error 
				return src / dst;
			case '=' :	// =
				return src == dst;
			case '>' :	// >
				return src >  dst;
			case '<' :	// <
				return src <  dst;
			case 0x11 :	// !=
				return src != dst;
			case 0x12 :	// >=
				return src >= dst;
			case 0x10 :	// <=
				return src <= dst;
			case 0xFFFFFFB0 :	// And
				return (int)src & (int)dst;
			case 0xFFFFFFB1 :	// Or
				return (int)src | (int)dst;
			default:
				{ ErrorNo=SyntaxERR; ErrorPtr=opPtr; return; }  // Syntax error
				break;
	}
}

double CB_UnaryEval( char *SRC ) {	// eval 1
	int c,op;
	int reg,mptr,opPtr;
	int dimA,dimB;
	double src,dst;
	char*	MatAryC;
	short*	MatAryW;
	int*	MatAryI;
	
	c=SRC[ExecPtr];
	if ( ( 'A' <= c ) && ( c <= 'z' ) ) {
		ExecPtr++;
		reg=c-'A';
		c=SRC[ExecPtr];
		if ( c=='%' ) {
			ExecPtr++;
			return REGINT[reg] ;
		} else {
			if ( c=='#' ) ExecPtr++;
			return REG[reg] ;
		}
	} else 
	if ( c==0x7F ) {
			MatrixOprand( SRC, &reg, &dimA, &dimB );
			if ( ErrorNo ) return ; // error
			return ReadMatrix( reg, dimA, dimB );
	} else  return Evalsub1(SRC);
}
*/

//----------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------
double Eval(char *SRC) {		// Eval temp
	double result;
	int execptr=ExecPtr;
	int oplen=strlenOp((char*)SRC);
	if ( oplen == 0 ) return 0;
	ExecPtr= 0;
	ErrorPtr= 0;
	ErrorNo = 0;
	CB_StrBufferCNT=0;			// Quot String buffer clear
	result = EvalsubTop( SRC );
	if ( ExecPtr < oplen ) CB_Error(SyntaxERR) ; // Syntax error 
	if ( ErrorNo ) { CB_ErrMsg( ErrorNo ); }
	ExecPtr=execptr;
	return result;
}

double InputNumD_full(int x, int y, int width, double defaultNum) {		// full number display
	unsigned int key;
	double result;
	sprintG(ExpBuffer, defaultNum, 30, LEFT_ALIGN);
	key= InputStrSub( x, y, width, 0, ExpBuffer, 64, ' ', REV_OFF, FLOAT_ON, EXP_ON, ALPHA_ON, HEX_OFF, PAL_ON, EXIT_CANCEL_OFF) ;
	if ( ( key == KEY_CTRL_EXIT ) || ( key != KEY_CTRL_EXE ) ) return (defaultNum);
	result = Eval( ExpBuffer );
	while ( ErrorNo ) {	// error loop
		key= InputStrSub( x, y, width, ErrorPtr, ExpBuffer, 64, ' ', REV_OFF, FLOAT_ON, EXP_ON, ALPHA_ON, HEX_OFF, PAL_ON, EXIT_CANCEL_OFF) ;
		if ( ( key == KEY_CTRL_EXIT ) || ( key != KEY_CTRL_EXE ) ) return (defaultNum);
		result = Eval( ExpBuffer );
	}
	CB_CurrentValue = result ;
	return result; // value ok
}

double InputNumD_Char(int x, int y, int width, double defaultNum, char code) {		//  1st char key in
	unsigned int key;
	double result;
	ExpBuffer[0]=code;
	ExpBuffer[1]='\0';
	key= InputStrSub( x, y, width, 1, ExpBuffer, 64, ' ', REV_OFF, FLOAT_ON, EXP_ON, ALPHA_ON, HEX_OFF, PAL_ON, EXIT_CANCEL_OFF) ;
	if ( ( key == KEY_CTRL_EXIT ) || ( key != KEY_CTRL_EXE ) ) return (defaultNum);
	result = Eval( ExpBuffer );
	while ( ErrorNo ) {	// error loop
		key= InputStrSub( x, y, width, ErrorPtr, ExpBuffer, 64, ' ', REV_OFF, FLOAT_ON, EXP_ON, ALPHA_ON, HEX_OFF, PAL_ON, EXIT_CANCEL_OFF) ;
		if ( ( key == KEY_CTRL_EXIT ) || ( key != KEY_CTRL_EXE ) ) return (defaultNum);
		result = Eval( ExpBuffer );
	}
	CB_CurrentValue = result ;
	return result; // value ok
}

double InputNumD_replay(int x, int y, int width, double defaultNum) {		//  replay expression
	unsigned int key;
	double result;
	key= InputStrSub( x, y, width, strlen((char*)ExpBuffer), ExpBuffer, 64, ' ', REV_OFF, FLOAT_ON, EXP_ON, ALPHA_ON, HEX_OFF, PAL_ON, EXIT_CANCEL_OFF) ;
	if ( ( key == KEY_CTRL_EXIT ) || ( key != KEY_CTRL_EXE ) ) return (defaultNum);
	result = Eval( ExpBuffer );
	while ( ErrorNo ) {	// error loop
		key= InputStrSub( x, y, width, ErrorPtr, ExpBuffer, 64, ' ', REV_OFF, FLOAT_ON, EXP_ON, ALPHA_ON, HEX_OFF, PAL_ON, EXIT_CANCEL_OFF) ;
		if ( ( key == KEY_CTRL_EXIT ) || ( key != KEY_CTRL_EXE ) ) return (defaultNum);
		result = Eval( ExpBuffer );
	}
	CB_CurrentValue = result ;
	return result; // value ok
}

//-----------------------------------------------------------------------------
double InputNumD_CB(int x, int y, int width, double defaultNum) {		//  Basic Input
	unsigned int key;
	double result;
	ExpBuffer[0]='\0';
	key=InputStrSub( x, y, width, 0, ExpBuffer, 64, ' ', REV_OFF, FLOAT_ON, EXP_ON, ALPHA_ON, HEX_OFF, PAL_OFF, EXIT_CANCEL_ON );
	if ( key==KEY_CTRL_AC  ) { BreakPtr=ExecPtr; return 0; }
	result = Eval( ExpBuffer );
	while ( ErrorNo || (strlen((char*)ExpBuffer)==0) ) {	// error loop
		key=InputStrSub( x, y, width, ErrorPtr, ExpBuffer, 64, ' ', REV_OFF, FLOAT_ON, EXP_ON, ALPHA_ON, HEX_OFF, PAL_OFF, EXIT_CANCEL_ON );
		if ( key==KEY_CTRL_AC  ) { BreakPtr=ExecPtr; return 0; }
		result = Eval( ExpBuffer );
	}
	return result; // value ok
}
double InputNumD_CB1(int x, int y, int width, double defaultNum) {		//  Basic Input 1
	unsigned int key;
	double result;
	ExpBuffer[0]='\0';
	key=InputStrSub( x, y, width, 0, ExpBuffer, 64, ' ', REV_OFF, FLOAT_ON, EXP_ON, ALPHA_ON, HEX_OFF, PAL_OFF, EXIT_CANCEL_ON );
	if ( key==KEY_CTRL_AC  ) { BreakPtr=ExecPtr; return 0; }
	if ( ExpBuffer[0]=='\0' ) if ( key==KEY_CTRL_EXE ) return (defaultNum);
	result = Eval( ExpBuffer );
	while ( ErrorNo || (strlen((char*)ExpBuffer)==0) ) {	// error loop
		key=InputStrSub( x, y, width, ErrorPtr, ExpBuffer, 64, ' ', REV_OFF, FLOAT_ON, EXP_ON, ALPHA_ON, HEX_OFF, PAL_OFF, EXIT_CANCEL_ON );
		if ( key==KEY_CTRL_AC  ) { BreakPtr=ExecPtr; return 0; }
		if ( strlen((char*)ExpBuffer)==0 ) if ( key==KEY_CTRL_EXE  ) return (defaultNum);
		result = Eval( ExpBuffer );
	}
	return result; // value ok
}
double InputNumD_CB2(int x, int y, int width, double defaultNum) {		//  Basic Input 2
	unsigned int key;
	double result;
	sprintGR(ExpBuffer, defaultNum, 30, LEFT_ALIGN, CB_Round.MODE, CB_Round.DIGIT );
	key=InputStrSub( x, y, width, 0, ExpBuffer, 64, ' ', REV_OFF, FLOAT_ON, EXP_ON, ALPHA_ON, HEX_OFF, PAL_OFF, EXIT_CANCEL_ON );
	if ( key==KEY_CTRL_AC  ) { BreakPtr=ExecPtr; return 0; }
	result = Eval( ExpBuffer );
	while ( ErrorNo ) {	// error loop
		key=InputStrSub( x, y, width, ErrorPtr, ExpBuffer, 64, ' ', REV_OFF, FLOAT_ON, EXP_ON, ALPHA_ON, HEX_OFF, PAL_OFF, EXIT_CANCEL_ON );
		if ( key==KEY_CTRL_AC  ) { BreakPtr=ExecPtr; return 0; }
		result = Eval( ExpBuffer );
	}
	return result; // value ok
}

