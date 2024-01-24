/*
===============================================================================

 Casio Basic RUNTIME library for fx-9860G series  v0.50

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

#include "CBI_Eval.h"
#include "CBI_interpreter.h"
//------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------
//		Expression evaluation    string -> double
//----------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------

int EvalIntsub1(unsigned char *SRC) {	// 1st Priority
	int result,tmp,tmp2;
	unsigned char c,d;
	unsigned char *pt;
	int dimA,dimB,reg,x,y;
	int i,ptr,mptr;
	char*	MatAryC;
	short*	MatAryW;
	int*	MatAryI;

	c = SRC[ExecPtr];
	if ( c == '(') {
		ExecPtr++;
		result = EvalIntsubTop( SRC );
		if ( SRC[ExecPtr] == ')' ) ExecPtr++;
		return result;
	}
	if ( c == 0x89 ) while ( c == 0x89 ) c=SRC[++ExecPtr];	// +
	if ( ( c == 0x87 ) || ( c == 0x99 ) ) {	//  -
		ExecPtr++;
		return - EvalIntsub1( SRC );
	}
	if ( ( 'A' <= c )&&( c <= 'Z' ) || ( 'a' <= c )&&( c <= 'z' ) )  {
			reg=c-'A';
			c=SRC[++ExecPtr];
			if ( c=='#' ) { ExecPtr++; return REG[reg] ; }
			else
			if ( c=='%' ) ExecPtr++;
			return REGINT[reg] ;
	}
	if ( ( '0' <= c )&&( c <= '9' ) ) {
//		return  Eval_atof( SRC );
		result=0;
		while ( ('0'<=c)&&(c<='9') ) {
			result = result*10 +(c-'0');
			c=SRC[++ExecPtr];
		}
		return result ;
	}
	switch ( c ) { 			// ( type C function )  sin cos tan... 
		case 0x7F:	// 7F..
			c = SRC[ExecPtr+1];
			if ( c == 0x40 ) {	// Mat A[a,b]
				ExecPtr+=2;
				c = SRC[ExecPtr];
				if ( ( 'A' <= c )&&( c <= 'Z' ) ) {
					reg=c-'A';
					ExecPtr++ ;
					if ( SRC[ExecPtr] != '[' ) { dspflag=3; return 1; }	// Mat A
					c = SRC[++ExecPtr];
					if ( SRC[ExecPtr+1] == ',' ) {
						ExecPtr++;
						if  ( ( '1'<= c ) && ( c<='9' ) ) dimA=c-'0';
							else if  ( ( 'A' <= c ) && ( c <= 'z' ) ) dimA=REGINT[c-'A'];
					} else {
						dimA=(EvalIntsubTop( SRC ));
						if ( SRC[ExecPtr] != ',' ) CB_Error(SyntaxERR) ; // Syntax error 
					}
					if ( MatArySizeA[(reg)] < dimA ) CB_Error(DimensionERR) ; // Dimension error 
					c = SRC[++ExecPtr];
					if ( SRC[ExecPtr+1] == ']' ) {
						ExecPtr++;
						if  ( ( '1'<= c ) && ( c<='9' ) ) dimB=c-'0';
						else if  ( ( 'A' <= c ) && ( c <= 'z' ) ) dimB=REGINT[c-'A'];
					} else {
						dimB=(EvalIntsubTop( SRC ));
						if ( SRC[ExecPtr] != ']' ) CB_Error(SyntaxERR) ; // Syntax error 
					}
					if ( MatArySizeB[(reg)] < dimB ) CB_Error(DimensionERR) ; // Dimension error 
					c = SRC[++ExecPtr];
					if ( ErrorNo ) return 0 ;
					switch ( MatAryElementSize[reg] ) {
						case 4:
							MatAryI=(int*)MatAry[reg];
							return MatAryI[(dimA-1)*MatArySizeB[reg]+dimB-1] ;			// Matrix array int
							break;
						case 1:
							MatAryC=(char*)MatAry[reg];
							return MatAryC[(dimA-1)*MatArySizeB[reg]+dimB-1] ;			// Matrix array char
							break;
						case 2:
							MatAryW=(short*)MatAry[reg];
							return MatAryW[(dimA-1)*MatArySizeB[reg]+dimB-1] ;			// Matrix array word
							break;
						case 8:
							return MatAry[reg][(dimA-1)*MatArySizeB[reg]+dimB-1] ;			// Matrix array doubl
							break;
					}
				}
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
			} else if ( c == 0x8F ) {	// Getkey
					ExecPtr+=2;
					c = SRC[ExecPtr];
					if ( c=='0' ) {	ExecPtr++ ; result = CB_Getkey0() ; if ( result==34 ) if (BreakCheck) { BreakPtr=ExecPtr; KeyRecover(); } }
					if ( c=='1' ) {	ExecPtr++ ; result = CB_Getkey1() ; if ( result==34 ) if (BreakCheck) { BreakPtr=ExecPtr; KeyRecover(); } }
					if ( c=='2' ) {	ExecPtr++ ; result = CB_Getkey2() ; if ( result==34 ) if (BreakCheck) { BreakPtr=ExecPtr; KeyRecover(); } }
					else		  	result = CB_Getkey() ;
					return result ;
			} else if ( c == 0x87 ) {	// RanInt#(st,en)
					ExecPtr+=2;
					x=(EvalIntsubTop( SRC ));
					if ( SRC[ExecPtr] != ',' ) CB_Error(SyntaxERR) ; // Syntax error 
					ExecPtr++ ;	// ',' skip
					y=(EvalIntsubTop( SRC ));
					if ( SRC[ExecPtr] == ')' ) ExecPtr++;
					result= rand()*(x-y+1)/(RAND_MAX+1) +y ;
					return result ;
			} else if ( c == 0xE9 ) {	// CellSum(Mat A[x,y])
					ExecPtr+=2;
					c = SRC[ExecPtr];
					if ( ( c!=0x7F ) || ( SRC[ExecPtr+1]!=0x40 ) ) CB_Error(SyntaxERR) ; // Syntax error 
					ExecPtr+=2;
					c = SRC[ExecPtr];
					if ( ( 'A' <= c )&&( c <= 'Z' ) ) {
						reg=c-'A';
						ExecPtr++ ;
						if ( SRC[ExecPtr] != '[' ) CB_Error(SyntaxERR) ; // Syntax error 
						c = SRC[++ExecPtr];
						if ( SRC[ExecPtr+1] == ',' ) {
							ExecPtr++;
							if  ( ( '1'<= c ) && ( c<='9' ) ) dimA=c-'0';
								else if  ( ( 'A' <= c ) && ( c <= 'z' ) ) dimA=REGINT[c-'A'];
						} else {
							dimA=(EvalIntsubTop( SRC ));
							if ( SRC[ExecPtr] != ',' ) CB_Error(SyntaxERR) ; // Syntax error 
						}
						if ( ( dimA < 2 ) || ( MatArySizeA[(reg)] < dimA-1 ) ) CB_Error(DimensionERR) ; // Dimension error 
						c = SRC[++ExecPtr];
						if ( SRC[ExecPtr+1] == ']' ) {
							ExecPtr++;
							if  ( ( '1'<= c ) && ( c<='9' ) ) dimB=c-'0';
							else if  ( ( 'A' <= c ) && ( c <= 'z' ) ) dimB=REGINT[c-'A'];
						} else {
							dimB=(EvalIntsubTop( SRC ));
							if ( SRC[ExecPtr] != ']' ) CB_Error(SyntaxERR) ; // Syntax error 
						}
						if ( ( dimB < 2 ) || ( MatArySizeB[(reg)] < dimB-1 ) ) CB_Error(DimensionERR) ; // Dimension error 
						c = SRC[++ExecPtr];
						if ( ErrorNo ) return 0 ;
						if ( c == ')' ) ExecPtr++ ;
						dimA--; dimB--;
						switch ( MatAryElementSize[reg] ) {
							case 4:														// Matrix array int
								MatAryI=(int*)MatAry[reg];
								return  MatAryI[(dimA-1)*MatArySizeB[reg]+dimB-1] + MatAryI[(dimA-1)*MatArySizeB[reg]+dimB] + MatAryI[(dimA-1)*MatArySizeB[reg]+dimB+1] + MatAryI[(dimA+1)*MatArySizeB[reg]+dimB-1] + MatAryI[(dimA+1)*MatArySizeB[reg]+dimB] + MatAryI[(dimA+1)*MatArySizeB[reg]+dimB+1] + MatAryI[(dimA)*MatArySizeB[reg]+dimB-1] + MatAryI[(dimA)*MatArySizeB[reg]+dimB+1] ;
								break;
							case 1:														// Matrix array char
								MatAryC=(char*)MatAry[reg];
								return MatAryC[(dimA-1)*MatArySizeB[reg]+dimB-1] + MatAryC[(dimA-1)*MatArySizeB[reg]+dimB] + MatAryC[(dimA-1)*MatArySizeB[reg]+dimB+1] + MatAryC[(dimA+1)*MatArySizeB[reg]+dimB-1] + MatAryC[(dimA+1)*MatArySizeB[reg]+dimB] + MatAryC[(dimA+1)*MatArySizeB[reg]+dimB+1] + MatAryC[(dimA)*MatArySizeB[reg]+dimB-1] + MatAryC[(dimA)*MatArySizeB[reg]+dimB+1] ;
								break;
							case 2:														// Matrix array word
								MatAryW=(short*)MatAry[reg];
								return  MatAryW[(dimA-1)*MatArySizeB[reg]+dimB-1] + MatAryW[(dimA-1)*MatArySizeB[reg]+dimB] + MatAryW[(dimA-1)*MatArySizeB[reg]+dimB+1] + MatAryW[(dimA+1)*MatArySizeB[reg]+dimB-1] + MatAryW[(dimA+1)*MatArySizeB[reg]+dimB] + MatAryW[(dimA+1)*MatArySizeB[reg]+dimB+1] + MatAryW[(dimA)*MatArySizeB[reg]+dimB-1] + MatAryW[(dimA)*MatArySizeB[reg]+dimB+1] ;
								break;
							case 8:														// Matrix array doubl
								return MatAry[reg][(dimA-1)*MatArySizeB[reg]+dimB-1] + MatAry[reg][(dimA-1)*MatArySizeB[reg]+dimB] + MatAry[reg][(dimA-1)*MatArySizeB[reg]+dimB+1] + MatAry[reg][(dimA+1)*MatArySizeB[reg]+dimB-1] + MatAry[reg][(dimA+1)*MatArySizeB[reg]+dimB] + MatAry[reg][(dimA+1)*MatArySizeB[reg]+dimB+1] + MatAry[reg][(dimA)*MatArySizeB[reg]+dimB-1] + MatAry[reg][(dimA)*MatArySizeB[reg]+dimB+1];
								break;
						}
					}
			} else if ( c == 0xF0 ) {	// GraphY
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
			
		case 0xC0 :	// Ans
			ExecPtr++;
			return CBint_CurrentValue ;
		case 0xD0 :	// PI
			ExecPtr++;
			return PI ;
		case 0xC1 :	// Ran#
			c = SRC[++ExecPtr];
			if ( ( '1' <= c ) && ( c <= '9' ) ) {
					ExecPtr++;
					srand( c-'0' );
			}
			result=rand();
			return result ;
		case 0x97 :	// abs
			ExecPtr++; result = abs( EvalIntsub5( SRC ) );
			return result ;
		case 0xA6 :	// int
			ExecPtr++; result = EvalIntsub5( SRC ) ;
			return result ;
		case 0xB6 :	// frac
			ExecPtr++; result = 0;
			return result ;
		case 0x86 :	// sqr
			ExecPtr++; tmp=EvalIntsub5( SRC ) ;
			if ( tmp<=0 ) CB_Error(MathERR) ; // Math error
			return sqrt( tmp );
		case 0x95 :	// log10
			ExecPtr++; tmp=EvalIntsub5( SRC ) ;
			if ( tmp<=0 ) CB_Error(MathERR) ; // Math error
			return log10( tmp );
		case 0xB5 :	// 10^
			ExecPtr++; result = pow(10, EvalIntsub5( SRC ) );
			return result ;
		case 0x85 :	// ln
			ExecPtr++; tmp=EvalIntsub5( SRC ) ;
			if ( tmp<=0 ) CB_Error(MathERR) ; // Math error
			return log( tmp );
		case 0xA5 :	// expn
			ExecPtr++; result = exp( EvalIntsub5( SRC ) );
			return result ;
		case 0xA7 :	// Not
			ExecPtr++; result = ! (int) ( EvalIntsub5( SRC ) );
			return result ;
		case 0x96 :	// cuberoot
			ExecPtr++; result = pow( EvalIntsub5( SRC ), 1.0/3.0 );
			return result ;
		case 0xF7:	// F7..
			c = SRC[ExecPtr+1];
			if ( c == 0xAF ) {	// PxlTest(y,x)
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
		case 0xF9:	// F9..
			c = SRC[ExecPtr+1];
			if ( c == 0x21 ) {	// Xdot
					ExecPtr+=2;
					return Xdot;
			}
			break;
		case '%' :	// 1/128 Ticks
			ExecPtr++;
			return RTC_GetTicks() ;
		case 0xDD :	// Eng
			ExecPtr++;
			return ENG ;
		default:
			break;
	}
	CB_Error(SyntaxERR) ; // Syntax error 
	return 0 ;
}

int EvalIntsub2(unsigned char *SRC) {	//  2nd Priority  ( type B function ) ...
	int cont=1;
	int result,tmp;
	unsigned int c;
	result = EvalIntsub1( SRC );
	while ( 1 ) {
		c = SRC[ExecPtr++];
		switch ( c ) {
			case  0x8B  :	// ^2
				result *= result ;
				break;
			case  0x9B  :	// ^(-1) RECIP
				if ( result == 0 ) CB_Error(DivisionByZeroERR); // Division by zero error 
				result = 1 / result ;
				break;
			case  0xAB  :	//  !
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
int EvalIntsub4(unsigned char *SRC) {	//  3rd Priority  ( ^ ...)
	int result,tmp;
	unsigned int c;
	unsigned char *pt;
	result = EvalIntsub2( SRC );
	while ( 1 ) {
		c = SRC[ExecPtr++];
		switch ( c ) {
			case  0xA8  :	// a ^ b
				result = pow( result, EvalIntsub2( SRC ) );
				break;
			case  0xB8  :	// powroot
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
/*
double EvalIntsub4(unsigned char *SRC) {	//  4th Priority  (Fraction) a/b/c
	double result,frac1,frac2,frac3;
	unsigned int c;
	result = EvalIntsub3( SRC );
	c = SRC[ExecPtr];
	if ( c == 0xBB ) {
		ExecPtr++;
		frac1 = result ;
		frac2 = EvalIntsub3( SRC );
		c = SRC[ExecPtr];
		if ( c == 0xBB ) {
			ExecPtr++;
			frac3 = EvalIntsub3( SRC );
			if ( frac3 == 0 ) CB_Error(MathERR) ; // Math error
			result = frac1 + ( frac2 / frac3 ) ;
		} else {
			if ( frac2 == 0 ) CB_Error(MathERR) ; // Math error
			result = ( frac1 / frac2 ) ;
		}
	}
	return result;
}
*/
int EvalIntsub5(unsigned char *SRC) {	//  5th Priority
	int result,tmp;
	unsigned int c;
	int dimA,dimB,reg,x,y;
	result = EvalIntsub4( SRC );
	while ( 1 ) {
		c = SRC[ExecPtr];
		if ((( 'A' <= c ) && ( c <= 'Z' )) ||
			(( 'a' <= c ) && ( c <= 'z' )) ||
			 ( c == 0xD0 ) || // PI
			 ( c == 0xC0 ) || // Ans
			 ( c == 0xC1 )) { // Ran#
				result *= EvalIntsub4( SRC ) ;
		} else if ( c == 0x7F ) { // 7F..
			c = SRC[ExecPtr+1];
			switch ( c ) {
				case 0x40:	// Mat A[a,b]
				case 0x3A:	// MOD(a,b)
				case 0x8F:	// Getkey
				case 0x87:	// RanInt#(st,en)
				case 0xF0:	// GraphY
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
		} else if ( c == 0xF7 ) { // F7..
			c = SRC[ExecPtr+1];
			switch ( c ) {
				case 0xAF:	// PxlTest(y,x)
					result *= EvalIntsub4( SRC ) ;
					break;
				default:
					return result;
					break;
			}
		} else if ( c == 0xF9 ) { // F9..
			c = SRC[ExecPtr+1];
			switch ( c ) {
				case 0x21:	// Xdot
					result *= EvalIntsub4( SRC ) ;
					break;
				default:
					return result;
					break;
			}
		} else if ( c == 0xE7 ) { // E7..
			c = SRC[ExecPtr+1];
			switch ( c ) {
				default:
					return result;
					break;
			}
		} else return result;
	 }
}
/*
int EvalIntsub6(unsigned char *SRC) {	//  6th Priority  ( type C function )  sin cos tan... 
	int cont=1;
	int result,tmp;
	unsigned int c;
	result = EvalIntsub5( SRC );
	return result;
}
*/
int EvalIntsub7(unsigned char *SRC) {	//  7th Priority
	int result,tmp;
	unsigned int c;
	result = EvalIntsub5( SRC );
	while ( 1 ) {
		c = SRC[ExecPtr];
		switch ( c ) {
			case '(' :
				ExecPtr++; result *= EvalIntsubTop( SRC );
				if ( SRC[ExecPtr] == ')' ) ExecPtr++;
				break;
			case 0x97 :	// abs
			case 0xA6 :	// int
			case 0xB6 :	// frac
			case 0x85 :	// ln
			case 0x86 :	// sqr
			case 0x95 :	// log10
			case 0x96 :	// cuberoot
			case 0xA5 :	// expn
			case 0xA7 :	// Not
			case 0xB5 :	// 10^
			case 0xB7 :	// Neg
				result *= EvalIntsub5( SRC );
				break;
			default:
				return result;
				break;
		}
	 }
	return result;
}
/*
int EvalIntsub8(unsigned char *SRC) {	//  8th Priority
	int result,tmp;
	unsigned int c;
	result = EvalIntsub7( SRC );
	return result;
}
int EvalIntsub9(unsigned char *SRC) {	//  9th Priority
	int result,tmp;
	unsigned int c;
	result = EvalIntsub8( SRC );
	return result;
}
*/
int EvalIntsub10(unsigned char *SRC) {	//  10th Priority  ( *,/, int.,Rmdr )
	int result,tmp;
	unsigned int c;
	result = EvalIntsub7( SRC );
	while ( 1 ) {
		c = SRC[ExecPtr++];
		switch ( c ) {
			case 0xA9 :		// ~
				result *= EvalIntsub7( SRC );
				break;
			case 0xB9 :		// €
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
 
int EvalIntsub11(unsigned char *SRC) {	//  11th Priority  ( +,- )
	int result;
	unsigned int c;
	result = EvalIntsub10( SRC );
	while ( 1 ) {
		c = SRC[ExecPtr++];
		switch ( c ) {
			case 0x89 :		// +
				result += EvalIntsub10( SRC );
				break;
			case 0x99 :		// -
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

int EvalIntsub12(unsigned char *SRC) {	//  12th Priority ( =,!=,><,>=,<= )
	int result;
	unsigned int c;
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

int EvalIntsub13(unsigned char *SRC) {	//  13th Priority  ( And,and)
	int result;
	unsigned int c;
	result = EvalIntsub12( SRC );
	while ( 1 ) {
		c = SRC[ExecPtr];
		if ( c == 0x7F ) {
			c = SRC[ExecPtr+1];
			switch ( c ) {
				case 0xB0 :	// And
					ExecPtr+=2;
					result = ( (int)result & (int)EvalIntsub13( SRC ) );
					break;
				default:
					return result;
					break;
			}
		} else return result;
	}
}

int EvalIntsubTop(unsigned char *SRC) {	//  14th Priority  ( Or,Xor,or,xor,xnor )
	int result;
	unsigned int c;
	result = EvalIntsub13( SRC );
	while ( 1 ) {
		c = SRC[ExecPtr];
		if ( c == 0x7F ) {
			c = SRC[ExecPtr+1];
			switch ( c ) {
				case 0xB1 :	// Or
					ExecPtr+=2;
					result = ( (int)result | (int)EvalIntsub13( SRC ) );
					break;
				case 0xB4 :	// Xor
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
//-----------------------------------------------------------------------------
/*
int EvalInt(unsigned char *SRC) {		// EvalInt temp
	int result;
	int execptr=ExecPtr;
    ExecPtr= 0;
    ErrorPtr= 0;
	ErrorNo = 0;
	if ( strlen((char*)SRC) == 0 ) { ExecPtr=execptr; return 0; }
    result = EvalIntsubTop( SRC );
    if ( ExecPtr < strlen((char*)SRC) ) CB_Error(SyntaxERR) ; // Syntax error 
    if ( ErrorNo ) { CB_ErrMsg( ErrorNo ); }
    ExecPtr=execptr;
	return result;
}

int InputNumI_full(int x, int y, int width, int defaultNum) {		// full number display
	unsigned int key;
	int result;
	sprintG(ExpBuffer, defaultNum, 30, LEFT_ALIGN);
	key= InputStrSub( x, y, width, 0, ExpBuffer, 64, ' ', REV_OFF, FLOAT_ON, EXP_ON, ALPHA_ON, HEX_OFF, PAL_ON, EXIT_CANCEL_OFF) ;
	if ( ( key == KEY_CTRL_EXIT ) || ( key != KEY_CTRL_EXE ) ) return (defaultNum);
	result = EvalInt( ExpBuffer );
	while ( ErrorNo ) {	// error loop
		key= InputStrSub( x, y, width, ErrorPtr, ExpBuffer, 64, ' ', REV_OFF, FLOAT_ON, EXP_ON, ALPHA_ON, HEX_OFF, PAL_ON, EXIT_CANCEL_OFF) ;
		if ( ( key == KEY_CTRL_EXIT ) || ( key != KEY_CTRL_EXE ) ) return (defaultNum);
		result = EvalInt( ExpBuffer );
	}
	CB_CurrentValue = result ;
	return result; // value ok
}

int InputNumI_Char(int x, int y, int width, int defaultNum, char code) {		//  1st char key in
	unsigned int key;
	int result;
	ExpBuffer[0]=code;
	ExpBuffer[1]='\0';
	key= InputStrSub( x, y, width, 1, ExpBuffer, 64, ' ', REV_OFF, FLOAT_ON, EXP_ON, ALPHA_ON, HEX_OFF, PAL_ON, EXIT_CANCEL_OFF) ;
	if ( ( key == KEY_CTRL_EXIT ) || ( key != KEY_CTRL_EXE ) ) return (defaultNum);
	result = EvalInt( ExpBuffer );
	while ( ErrorNo ) {	// error loop
		key= InputStrSub( x, y, width, ErrorPtr, ExpBuffer, 64, ' ', REV_OFF, FLOAT_ON, EXP_ON, ALPHA_ON, HEX_OFF, PAL_ON, EXIT_CANCEL_OFF) ;
		if ( ( key == KEY_CTRL_EXIT ) || ( key != KEY_CTRL_EXE ) ) return (defaultNum);
		result = EvalInt( ExpBuffer );
	}
	CB_CurrentValue = result ;
	return result; // value ok
}

int InputNumI_replay(int x, int y, int width, int defaultNum) {		//  replay expression
	unsigned int key;
	int result;
	key= InputStrSub( x, y, width, strlen((char*)ExpBuffer), ExpBuffer, 64, ' ', REV_ON, FLOAT_ON, EXP_OFF, ALPHA_ON, HEX_OFF, PAL_ON, EXIT_CANCEL_OFF) ;
	if ( ( key == KEY_CTRL_EXIT ) || ( key != KEY_CTRL_EXE ) ) return (defaultNum);
	result = EvalInt( ExpBuffer );
	while ( ErrorNo ) {	// error loop
		key= InputStrSub( x, y, width, ErrorPtr, ExpBuffer, 64, ' ', REV_OFF, FLOAT_ON, EXP_ON, ALPHA_ON, HEX_OFF, PAL_ON, EXIT_CANCEL_OFF) ;
		if ( ( key == KEY_CTRL_EXIT ) || ( key != KEY_CTRL_EXE ) ) return (defaultNum);
		result = EvalInt( ExpBuffer );
	}
	CB_CurrentValue = result ;
	return result; // value ok
}

//-----------------------------------------------------------------------------
int InputNumI_CB(int x, int y, int width, int defaultNum) {		//  Basic Input
	unsigned int key;
	int result;
	ExpBuffer[0]='\0';
	key=InputStrSub( x, y, width, 0, ExpBuffer, 64, ' ', REV_OFF, FLOAT_ON, EXP_ON, ALPHA_ON, HEX_OFF, PAL_OFF, EXIT_CANCEL_ON );
	if ( key==KEY_CTRL_AC  ) return 0;
	result = EvalInt( ExpBuffer );
	while ( ErrorNo || (strlen((char*)ExpBuffer)==0) ) {	// error loop
		key=InputStrSub( x, y, width, ErrorPtr, ExpBuffer, 64, ' ', REV_OFF, FLOAT_ON, EXP_ON, ALPHA_ON, HEX_OFF, PAL_OFF, EXIT_CANCEL_ON );
		if ( key==KEY_CTRL_AC  ) return 0;
		result = EvalInt( ExpBuffer );
	}
	return result; // value ok
}
int InputNumI_CB1(int x, int y, int width, int defaultNum) {		//  Basic Input 1
	unsigned int key;
	int result;
	ExpBuffer[0]='\0';
	key=InputStrSub( x, y, width, 0, ExpBuffer, 64, ' ', REV_OFF, FLOAT_ON, EXP_ON, ALPHA_ON, HEX_OFF, PAL_OFF, EXIT_CANCEL_ON );
	if ( key==KEY_CTRL_AC  ) return 0;
	if ( ExpBuffer[0]=='\0' ) if ( key==KEY_CTRL_EXE ) return (defaultNum);
	result = EvalInt( ExpBuffer );
	while ( ErrorNo || (strlen((char*)ExpBuffer)==0) ) {	// error loop
		key=InputStrSub( x, y, width, ErrorPtr, ExpBuffer, 64, ' ', REV_OFF, FLOAT_ON, EXP_ON, ALPHA_ON, HEX_OFF, PAL_OFF, EXIT_CANCEL_ON );
		if ( key==KEY_CTRL_AC  ) return 0;
		if ( strlen((char*)ExpBuffer)==0 ) if ( key==KEY_CTRL_EXE  ) return (defaultNum);
		result = EvalInt( ExpBuffer );
	}
	return result; // value ok
}
int InputNumI_CB2(int x, int y, int width, int defaultNum) {		//  Basic Input 2
	unsigned int key;
	int result;
	sprintGR(ExpBuffer, defaultNum, 30, LEFT_ALIGN, CB_Round.MODE, CB_Round.DIGIT );
	key=InputStrSub( x, y, width, 0, ExpBuffer, 64, ' ', REV_OFF, FLOAT_ON, EXP_ON, ALPHA_ON, HEX_OFF, PAL_OFF, EXIT_CANCEL_ON );
	if ( key==KEY_CTRL_AC  ) return 0;
	result = EvalInt( ExpBuffer );
	while ( ErrorNo ) {	// error loop
		key=InputStrSub( x, y, width, ErrorPtr, ExpBuffer, 64, ' ', REV_OFF, FLOAT_ON, EXP_ON, ALPHA_ON, HEX_OFF, PAL_OFF, EXIT_CANCEL_ON );
		if ( key==KEY_CTRL_AC  ) return 0;
		result = EvalInt( ExpBuffer );
	}
	return result; // value ok
}
*/
