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
#include "CB_error.h"

#include "CB_inp.h"
#include "CB_glib.h"
#include "CB_interpreter.h"
#include "CB_Eval.h"
#include "CB_Matrix.h"

#include "CBI_Eval.h"
#include "CBI_interpreter.h"
//----------------------------------------------------------------------------------------------
//		Expression evaluation    string -> int
//----------------------------------------------------------------------------------------------
int CB_EvalInt( char *SRC ) {
	int value;
	if (CB_INT) value=EvalIntsubTop( SRC ); else value=EvalsubTop( SRC ); 
	return value;
}

int EvalIntsubTop( char *SRC ) {	// eval 1
	int c;
	int excptr=ExecPtr;
	int  result,dst;

//	while ( SRC[ExecPtr]==0x20 ) ExecPtr++; // Skip Space
	result=EvalIntsub1(SRC);
	c=SRC[ExecPtr];
	if ( ( c==':' ) || ( c==0x0E ) || ( c==0x13 ) || ( c==0x0D ) || ( c==',' ) || ( c==')' ) || ( c==']' ) ) return result;
	else 
	if ( c==0xFFFFFF89 ) { // +
		ExecPtr++; dst=EvalIntsub1(SRC); c=SRC[ExecPtr];
		if ( ( c==':' ) || ( c==0x0E ) || ( c==0x13 ) || ( c==0x0D ) || ( c==',' ) || ( c==')' ) || ( c==']' ) ) return result+dst;
	} else
	if ( c==0xFFFFFF99 ) { // -
		ExecPtr++; dst=EvalIntsub1(SRC); c=SRC[ExecPtr];
		if ( ( c==':' ) || ( c==0x0E ) || ( c==0x13 ) || ( c==0x0D ) || ( c==',' ) || ( c==')' ) || ( c==']' ) ) return result-dst;
	} else
	if ( c=='=') { // ==
		ExecPtr++; dst=EvalIntsub1(SRC); c=SRC[ExecPtr];
		if ( ( c==':' ) || ( c==0x0E ) || ( c==0x13 ) || ( c==0x0D ) || ( c==',' ) || ( c==')' ) || ( c==']' ) ) return result == dst;
	} else
	if ( c=='>') { // >
		ExecPtr++; dst=EvalIntsub1(SRC); c=SRC[ExecPtr];
		if ( ( c==':' ) || ( c==0x0E ) || ( c==0x13 ) || ( c==0x0D ) || ( c==',' ) || ( c==')' ) || ( c==']' ) ) return result > dst;
	} else
	if ( c=='<') { // <
		ExecPtr++; dst=EvalIntsub1(SRC); c=SRC[ExecPtr];
		if ( ( c==':' ) || ( c==0x0E ) || ( c==0x13 ) || ( c==0x0D ) || ( c==',' ) || ( c==')' ) || ( c==']' ) ) return result < dst;
	} else
	if ( c==0x11) { // !=
		ExecPtr++; dst=EvalIntsub1(SRC); c=SRC[ExecPtr];
		if ( ( c==':' ) || ( c==0x0E ) || ( c==0x13 ) || ( c==0x0D ) || ( c==',' ) || ( c==')' ) || ( c==']' ) ) return result != dst;
	} else
	if ( c==0x12) { // >=
		ExecPtr++; dst=EvalIntsub1(SRC); c=SRC[ExecPtr];
		if ( ( c==':' ) || ( c==0x0E ) || ( c==0x13 ) || ( c==0x0D ) || ( c==',' ) || ( c==')' ) || ( c==']' ) ) return result >= dst;
	} else
	if ( c==0x10) { // <=
		ExecPtr++; dst=EvalIntsub1(SRC); c=SRC[ExecPtr];
		if ( ( c==':' ) || ( c==0x0E ) || ( c==0x13 ) || ( c==0x0D ) || ( c==',' ) || ( c==')' ) || ( c==']' ) ) return result <= dst;
	} else
	if ( c==0xFFFFFFA9 ) { // ~
		ExecPtr++; dst=EvalIntsub1(SRC); c=SRC[ExecPtr];
		if ( ( c==':' ) || ( c==0x0E ) || ( c==0x13 ) || ( c==0x0D ) || ( c==',' ) || ( c==')' ) || ( c==']' ) ) return result*dst;
	} else
	if ( c==0xFFFFFFB9 ) { // €
		ExecPtr++; dst=EvalIntsub1(SRC); c=SRC[ExecPtr];
		if ( dst == 0 ) CB_Error(DivisionByZeroERR); // Division by zero error
		if ( ( c==':' ) || ( c==0x0E ) || ( c==0x13 ) || ( c==0x0D ) || ( c==',' ) || ( c==')' ) || ( c==']' ) ) return result/dst;
	} else
	if ( c==0xFFFFFF8B ) { // ^2
		c=SRC[++ExecPtr];
		if ( ( c==':' ) || ( c==0x0E ) || ( c==0x13 ) || ( c==0x0D ) || ( c==',' ) || ( c==')' ) || ( c==']' ) ) return result*result;
	} else
	if ( c==0x7F ) { // 
		c=SRC[++ExecPtr];
		if ( c==0xFFFFFFB0 ) { // And
			ExecPtr++; dst=EvalIntsub1(SRC); c=SRC[ExecPtr];
			if ( ( c==':' ) || ( c==0x0E ) || ( c==0x13 ) || ( c==0x0D ) || ( c==',' ) || ( c==')' ) || ( c==']' ) ) return (int)result & (int)dst;
		} else
		if ( c==0xFFFFFFB1 ) { // Or
			ExecPtr++; dst=EvalIntsub1(SRC); c=SRC[ExecPtr];
			if ( ( c==':' ) || ( c==0x0E ) || ( c==0x13 ) || ( c==0x0D ) || ( c==',' ) || ( c==')' ) || ( c==']' ) ) return (int)result | (int)dst;
		} else
		if ( c==0xFFFFFFB4 ) { // Xor
			ExecPtr++; dst=EvalIntsub1(SRC); c=SRC[ExecPtr];
			if ( ( c==':' ) || ( c==0x0E ) || ( c==0x13 ) || ( c==0x0D ) || ( c==',' ) || ( c==')' ) || ( c==']' ) ) return (int)result ^ (int)dst;
		}
	}

	ExecPtr=excptr;
	result = EvalIntsub13( SRC );
	while ( 1 ) {
		c = SRC[ExecPtr];
		if ( c == 0x7F ) {
			c = SRC[ExecPtr+1];
			switch ( c ) {
				case 0xFFFFFFB1 :	// Or
					ExecPtr+=2;
					result = ( (int)result | (int)EvalIntsub13( SRC ) );
					break;
				case 0xFFFFFFB4 :	// Xor
					ExecPtr+=2;
					result = ( (int)result ^ (int)EvalIntsub13( SRC ) );
					break;
				default:
					return result;
					break;
			}
		} else return result;
	}
}
//-----------------------------------------------------------------------------
int Eval_atod(char *SRC, int c ) {
	int	result=0;
	if ( c == '0' ) {
		c = SRC[++ExecPtr];
		if ( (  c=='x' ) || ( c=='X' ) ) {
			c=SRC[++ExecPtr];
			while ( ( ('0'<=c)&&(c<='9') ) || ( ('A'<=c)&&(c<='F') ) || ( ('a'<=c)&&(c<='f') ) ) {
				if ( ('0'<=c)&&(c<='9') ) result = result*16 +(c-'0');
				else
				if ( ('A'<=c)&&(c<='F') ) result = result*16 +(c-'A'+10);
				else
				if ( ('a'<=c)&&(c<='f') ) result = result*16 +(c-'a'+10);
				c=SRC[++ExecPtr];
			}
			return result ;
		}
		else
		if ( (  c=='b' ) || ( c=='B' ) ) {
			c=SRC[++ExecPtr];
			while ( ('0'<=c)&&(c<='1') ) {
				result = result*2 +(c-'0');
				c=SRC[++ExecPtr];
			}
			return result ;
		}
	}
	while ( ('0'<=c)&&(c<='9') ) {
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

	c = SRC[ExecPtr];
	if ( c == '(') {
		ExecPtr++;
		result = EvalIntsubTop( SRC );
		if ( SRC[ExecPtr] == ')' ) ExecPtr++;
		return result;
	}
	if ( c == 0xFFFFFF89 ) while ( c == 0xFFFFFF89 ) c=SRC[++ExecPtr];	// +
	if ( ( c == 0xFFFFFF87 ) || ( c == 0xFFFFFF99 ) ) {	//  -
		ExecPtr++;
		return - EvalIntsub1( SRC );
	}
	if ( ( 'A' <= c )&&( c <= 'Z' ) )  {
			reg=c-'A';
			c=SRC[++ExecPtr];
			if ( c=='#' ) { ExecPtr++; return REG[reg] ; }
			else
			if ( c=='%' ) ExecPtr++;
			return REGINT[reg] ;
	}
	if ( ( 'a' <= c )&&( c <= 'z' ) )  {
			reg=c-'a';
			c=SRC[++ExecPtr];
			if ( c=='#' ) { ExecPtr++; return LocalDbl[reg][0] ; }
			else
			if ( c=='%' ) ExecPtr++;
			return LocalInt[reg][0] ;
	}
	if ( ( '0' <= c )&&( c <= '9' ) ) return  Eval_atod( SRC, c );
	
	switch ( c ) { 			// ( type C function )  sin cos tan... 
		case 0x7F:	// 7F..
			c = SRC[ExecPtr+1];
			if ( c == 0x40 ) {	// Mat A[a,b]
				ExecPtr+=2;
				c=SRC[ExecPtr]; if ( ( 'A'<=c )&&( c<='z' ) ) { reg=c-'A'; ExecPtr++; } else CB_Error(SyntaxERR) ; // Syntax error 
				if ( SRC[ExecPtr] != '[' ) { dspflag=3; return 1; }	// Mat A
				ExecPtr++;
				MatOprandInt2( SRC, reg, &dimA, &dimB );
				if ( ErrorNo ) return 1 ; // error
				return ReadMatrixInt( reg, dimA, dimB);

			} else if ( c == 0x3A ) {	// MOD(a,b)
					ExecPtr+=2;
					tmp = EvalIntsubTop( SRC );
					if ( SRC[ExecPtr] != ',' ) CB_Error(SyntaxERR) ; // Syntax error 
					ExecPtr++;
					tmp2 = EvalIntsubTop( SRC );
					if ( tmp2 == 0 )  CB_Error(DivisionByZeroERR); // Division by zero error 
					result= tmp-tmp/tmp2*tmp2;
					if ( result == tmp2  ) result--;
					if ( tmp < 0 ) {
						result=abs(tmp2)-result;
						if ( result == tmp2  ) result=0;
					}
					if ( SRC[ExecPtr] == ')' ) ExecPtr++;
					return result ;
			} else if ( c == 0xFFFFFF8F ) {	// Getkey
					ExecPtr+=2;
					c = SRC[ExecPtr];
					if ( ( '0'<=c )&&( c<='2' )) {	ExecPtr++ ; result = CB_GetkeyN(c-'0') ; if ( result==34 ) if (BreakCheck) { BreakPtr=ExecPtr; KeyRecover(); } }
					else		  	result = CB_Getkey() ;
					return result ;
			} else if ( c == 0xFFFFFF87 ) {	// RanInt#(st,en)
					ExecPtr+=2;
					x=(EvalIntsubTop( SRC ));
					if ( SRC[ExecPtr] != ',' ) CB_Error(SyntaxERR) ; // Syntax error 
					ExecPtr++ ;	// ',' skip
					y=(EvalIntsubTop( SRC ));
					if ( SRC[ExecPtr] == ')' ) ExecPtr++;
//					if ( x>=y ) CB_Error(ArgumentERR);  // Argument error
					if ( x>y ) { i=x; x=y; y=i; }
					result= rand()*(y-x+1)/(RAND_MAX+1) +x ;
					return result ;
			} else if ( c == 0xFFFFFFE9 ) {	// CellSum(Mat A[x,y])
					ExecPtr+=2;
					MatrixOprand( SRC, &reg, &x, &y );
					if ( ErrorNo ) return ; // error
					if ( SRC[ExecPtr] == ')' ) ExecPtr++;
					return Cellsum( reg, x, y );

			} else if ( c == 0xFFFFFFF0 ) {	// GraphY
					ExecPtr+=2;
					c=SRC[ExecPtr++];
					switch (c) {
						case '1':
							GraphY=GraphY1;
							break;
						case '2':
							GraphY=GraphY2;
							break;
						case '3':
							GraphY=GraphY3;
							break;
						default:
							CB_Error(ArgumentERR);  // Argument error
							break;
					}
					ptr=ExecPtr;
					ExecPtr= 0;
					result= EvalIntsubTop( GraphY );
					ExecPtr=ptr;
					return result ;
			} else if ( c == 0x00 ) {	// Xmin
					ExecPtr+=2;
					return Xmin;
			} else if ( c == 0x01 ) {	// Xmax
					ExecPtr+=2;
					return Xmax;
			} else if ( c == 0x02 ) {	// Xscl
					ExecPtr+=2;
					return Xscl;
			} else if ( c == 0x04 ) {	// Ymin
					ExecPtr+=2;
					return Ymin;
			} else if ( c == 0x05 ) {	// Ymax
					ExecPtr+=2;
					return Ymax;
			} else if ( c == 0x06) {	// Yscl
					ExecPtr+=2;
					return Yscl;
			} else if ( c == 0x0B ) {	// Xfct
					ExecPtr+=2;
					return Xfct;
			} else if ( c == 0x0C ) {	// Yfct
					ExecPtr+=2;
					return Yfct;
			}
			break;
			
		case 0xFFFFFFC0 :	// Ans
			ExecPtr++;
			return CBint_CurrentValue ;
		case 0xFFFFFFD0 :	// PI
			ExecPtr++;
			return PI ;
		case 0xFFFFFFC1 :	// Ran#
			c = SRC[++ExecPtr];
			if ( ( '0' <= c ) && ( c <= '9' ) ) srand( Eval_atod( SRC, c ) );
			result=rand();
			return result ;
		case 0xFFFFFF97 :	// abs
			ExecPtr++; result = abs( EvalIntsub5( SRC ) );
			return result ;
		case 0xFFFFFFA6 :	// int
			ExecPtr++; result = EvalIntsub5( SRC ) ;
			return result ;
		case 0xFFFFFFB6 :	// frac
			ExecPtr++; result = 0;
			return result ;
		case 0xFFFFFFA7 :	// Not
			ExecPtr++; result = ! (int) ( EvalIntsub5( SRC ) );
			return result ;
		case '%' :	// 1/128 Ticks
			ExecPtr++;
			return RTC_GetTicks()-CB_TicksAdjust;	// 
			
		case 0xFFFFFF86 :	// sqr
			ExecPtr++; tmp=EvalIntsub5( SRC ) ;
			if ( tmp<=0 ) CB_Error(MathERR) ; // Math error
			return sqrt( tmp );
		case 0xFFFFFF95 :	// log10
			ExecPtr++; tmp=EvalIntsub5( SRC ) ;
			if ( tmp<=0 ) CB_Error(MathERR) ; // Math error
			return log10( tmp );
		case 0xFFFFFFB5 :	// 10^
			ExecPtr++; result = pow(10, EvalIntsub5( SRC ) );
			return result ;
		case 0xFFFFFF85 :	// ln
			ExecPtr++; tmp=EvalIntsub5( SRC ) ;
			if ( tmp<=0 ) CB_Error(MathERR) ; // Math error
			return log( tmp );
		case 0xFFFFFFA5 :	// expn
			ExecPtr++; result = exp( EvalIntsub5( SRC ) );
			return result ;
		case 0xFFFFFF96 :	// cuberoot
			ExecPtr++; result = pow( EvalIntsub5( SRC ), 1.0/3.0 );
			return result ;
			
		case 0xFFFFFFF7:	// F7..
			c = SRC[ExecPtr+1];
			if ( c == 0xFFFFFFAF ) {	// PxlTest(y,x)
					ExecPtr+=2;
					y=(EvalIntsubTop( SRC ));
					if ( SRC[ExecPtr] != ',' ) CB_Error(SyntaxERR) ; // Syntax error 
					ExecPtr++ ;	// ',' skip
					x=(EvalIntsubTop( SRC ));
					if ( SRC[ExecPtr] != ')' ) CB_Error(SyntaxERR) ; // Syntax error 
					ExecPtr++ ;
					result = Bdisp_GetPoint_VRAM(x, y) ;			// 
					return result ;
			}
			break;
		case 0xFFFFFFF9:	// F9..
			c = SRC[ExecPtr+1];
			if ( c == 0x21 ) {	// Xdot
					ExecPtr+=2;
					return Xdot;
			}
			break;
		case 0xFFFFFFDD :	// Eng
			ExecPtr++;
			return ENG ;
		case 0xFFFFFFFD:	//  Eval(
			ExecPtr++; 
			return CBint_CurrentValue = CBint_EvalStr(SRC);
		default:
			break;
	}
	CB_Error(SyntaxERR) ; // Syntax error 
	return 0 ;
}
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
				if ( result == 0 ) CB_Error(DivisionByZeroERR); // Division by zero error 
				result = 1 / result ;
				break;
			case  0xFFFFFFAB  :	//  !
				tmp = floor( result );
				result = 1;
				while ( tmp > 0 ) { result *= tmp; tmp--; }
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
int EvalIntsub5(char *SRC) {	//  5th Priority
	int result,tmp;
	int c;
	int dimA,dimB,reg,x,y;
	result = EvalIntsub4( SRC );
	while ( 1 ) {
		c = SRC[ExecPtr];
		if ((( 'A' <= c ) && ( c <= 'Z' )) ||
			(( 'a' <= c ) && ( c <= 'z' )) ||
			 ( c == 0xFFFFFFD0 ) || // PI
			 ( c == 0xFFFFFFC0 ) || // Ans
			 ( c == 0xFFFFFFC1 )) { // Ran#
				result *= EvalIntsub4( SRC ) ;
		} else if ( c == 0x7F ) { // 7F..
			c = SRC[ExecPtr+1];
			switch ( c ) {
				case 0x40:	// Mat A[a,b]
				case 0x3A:	// MOD(a,b)
				case 0xFFFFFF8F:	// Getkey
				case 0xFFFFFF87:	// RanInt#(st,en)
				case 0xFFFFFFF0:	// GraphY
				case 0x00:	// Xmin
				case 0x01:	// Xmax
				case 0x02:	// Xscl
				case 0x04:	// Ymin
				case 0x05:	// Ymax
				case 0x06:	// Yscl
					result *= EvalIntsub4( SRC ) ;
					break;
				default:
					return result;
					break;
			}
		} else if ( c == 0xFFFFFFF7 ) { // F7..
			c = SRC[ExecPtr+1];
			switch ( c ) {
				case 0xFFFFFFAF:	// PxlTest(y,x)
					result *= EvalIntsub4( SRC ) ;
					break;
				default:
					return result;
					break;
			}
		} else if ( c == 0xFFFFFFF9 ) { // F9..
			c = SRC[ExecPtr+1];
			switch ( c ) {
				case 0x21:	// Xdot
					result *= EvalIntsub4( SRC ) ;
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
int EvalIntsub7(char *SRC) {	//  7th Priority
	int result,tmp;
	int c;
	result = EvalIntsub5( SRC );
	while ( 1 ) {
		c = SRC[ExecPtr];
		switch ( c ) {
			case '(' :
				ExecPtr++; result *= EvalIntsubTop( SRC );
				if ( SRC[ExecPtr] == ')' ) ExecPtr++;
				break;
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
					result = ( (int)result & (int)EvalIntsub12( SRC ) );
					break;
				default:
					return result;
					break;
			}
		} else return result;
	}
}

//----------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------

int CBint_BinaryEval( char *SRC ) {	// eval 2
	int c,op;
	int reg,mptr,opPtr;
	int src,dst;
	int dimA,dimB;
	char*	MatAryC;
	short*	MatAryW;
	int*	MatAryI;
	
	c=SRC[ExecPtr];
	if ( ( 'A' <= c ) && ( c <= 'Z' ) ) {
		ExecPtr++;
		reg=c-'A';
		c=SRC[ExecPtr];
		if ( c=='#' ) {
			ExecPtr++;
			src = REG[reg] ;
		} else {
			if ( c=='%' ) ExecPtr++;
			src = REGINT[reg] ;
		}
	} else 
	if ( c==0x7F ) {
			MatrixOprand( SRC, &reg, &dimA, &dimB );
			if ( ErrorNo ) return ; // error
			src = ReadMatrixInt( reg, dimA, dimB );
	} else  src = EvalIntsub1(SRC);

	opPtr=ExecPtr;
	op=SRC[ExecPtr++];	
	c=SRC[ExecPtr];
	if ( ( 'A' <= c ) && ( c <= 'Z' ) ) {
		ExecPtr++;
		reg=c-'A';
		c=SRC[ExecPtr];
		if ( c=='#' ) {
			ExecPtr++;
			dst = REG[reg] ;
		} else {
			if ( c=='%' ) ExecPtr++;
			dst = REGINT[reg] ;
		}
	} else 
	if ( c==0x7F ) {
			MatrixOprand( SRC, &reg, &dimA, &dimB );
			if ( ErrorNo ) return ; // error
			dst = ReadMatrixInt( reg, dimA, dimB );
	} else  dst = EvalIntsub1(SRC);

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

int CBint_UnaryEval( char *SRC ) {	// eval 1
	int c,op;
	int reg,mptr,opPtr;
	int src,dst;
	int dimA,dimB;
	char*	MatAryC;
	short*	MatAryW;
	int*	MatAryI;
	
	c=SRC[ExecPtr];
	if ( ( 'A' <= c ) && ( c <= 'Z' ) ) {
		ExecPtr++;
		reg=c-'A';
		c=SRC[ExecPtr];
		if ( c=='#' ) {
			ExecPtr++;
			return REG[reg] ;
		} else {
			if ( c=='%' ) ExecPtr++;
			return REGINT[reg] ;
		}
	} else 
	if ( c==0x7F ) {
			MatrixOprand( SRC, &reg, &dimA, &dimB );
			if ( ErrorNo ) return ; // error
			return ReadMatrixInt( reg, dimA, dimB );
	} else  return EvalIntsub1(SRC);
}

