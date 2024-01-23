/*
===============================================================================

 Casio Basic RUNTIME library for fx-9860G series  v0.40

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
#include "CB_lib.h"
#include "CB_Eval.h"
#include "CB_interpreter.h"
#include "CB_error.h"

//------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------
//		Expression evaluation    string -> double
//----------------------------------------------------------------------------------------------
#define ExpMax 255
unsigned char ExpBuffer[ExpMax+1];
//-----------------------------------------------------------------------------
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
double floor2( double x ) {
	unsigned char row[9];
	int tmp,exp;
	int i,j,bt;
	double result;
	unsigned char *ptr;

	ptr=(unsigned char *)(&x);
	for (i=0;i<8;i++) row[i]=ptr[i];

	tmp= ( (row[0]&0x7f)<<4 ) + ( (row[1]&0xf0)>>4 ) ;
	exp=tmp-1023; // 
	if ( ( 0 > exp ) || ( exp > 51 ) ) result = x;
	else {
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
		ptr=(unsigned char *)(&result);
		for (i=0;i<8;i++) ptr[i]=row[i];
	}
	return ( result ); 
}
*/
double frac( double x ) {
	double sign=1,tmp,d;
	if ( x <0 ) { sign=-1; x=-x; }
	if ( x<1e1 ) d= 1e15;
	if ( x>=1e1 ) d= 1e14;
	if ( x>=1e2 ) d= 1e13;
	if ( x>=1e3 ) d= 1e12;
	if ( x>=1e4 ) d= 1e11;
	if ( x>=1e5 ) d= 1e10;
	if ( x>=1e6 ) d= 1e9;
	if ( x>=1e7 ) d= 1e8;
	if ( x>=1e8 ) d= 1e7;
	if ( x>=1e9 ) d= 1e6;
	if ( x>=1e10 ) d= 1e5;
	if ( x>=1e11 ) d= 1e4;
	if ( x>=1e12 ) d= 1e3;
	if ( x>=1e13 ) d= 1e2;
	if ( x>=1e14 ) d= 1e1;
	if ( x>=1e15 ) d= 1;
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


//-----------------------------------------------------------------------------
void EvalError(int ErrNo) { // error
	if ( ErrorNo ) return ;
	ErrorNo=ErrNo;
	ErrorPtr=ExecPtr;
	if ( ErrNo==MaERR ) ErrorPtr--;
}

//-----------------------------------------------------------------------------
unsigned char Eval_atofNumDiv(unsigned char *SRC, double *num ){
	unsigned int c;
	double a=10;
	c = SRC[ExecPtr];
	while ( ('0'<=c)&&(c<='9') ) {
		(*num) = (*num) + (double)(c-'0')/a;
		a*=10;
		c=SRC[++ExecPtr];
	}
	return c;
}
unsigned char Eval_atofNumMult(unsigned char *SRC, double *num ){
	unsigned int c;
	c = SRC[ExecPtr];
	while ( ('0'<=c)&&(c<='9') ) {
		(*num) = (*num)*10 +(c-'0');
		c=SRC[++ExecPtr];
	}
	return c;
}
double Eval_atof(unsigned char *SRC) {
	char buffer[64];
	double temp,mantissa=0,exponent=0,result;
	double sign=1;
	unsigned int c;
	c = SRC[ExecPtr];
	if ( c == '.'  )   {
			ExecPtr++;
			c=Eval_atofNumDiv(SRC, &mantissa);	// .123456
	} else if ( c == 0x0F  ) {  // exp
			mantissa = 1.0;
			goto lblexp;
	} else { 
			c=Eval_atofNumMult(SRC, &mantissa);	// 123456
			if ( c == '.'  ) {
				ExecPtr++;
				c=Eval_atofNumDiv(SRC, &mantissa);	// 123456.789
			}
	}
	lblexp:
	if ( c == 0x0F  ) { // exp
			c=SRC[++ExecPtr];
			if ( ( c == 0x89 ) || ( c == '+'  ) ) c=SRC[++ExecPtr];
			if ( ( c == 0x87 ) || ( c == 0x99 ) ) { sign=-1; c=SRC[++ExecPtr]; }	// (-) & -
			if ( ( '0'<=c )&&( c<='9' ) ) c=Eval_atofNumMult(SRC, &exponent);
			else { ErrorNo=SyntaxERR; ErrorPtr=ExecPtr; } //  error 
		result = mantissa * pow(10,exponent*sign) ;
		return ( result ) ;
	} else
		return ( mantissa ) ;
}

//-----------------------------------------------------------------------------

double Evalsub1(unsigned char *SRC) {	// 1st Priority
	double result,tmp,tmp2;
	unsigned char c,d;
	unsigned char *pt;
	int dimA,dimB,reg,x,y;
	int i,ptr,mptr;
	c = SRC[ExecPtr];
	if ( c == '(') {
		ExecPtr++;
		result = EvalsubTop( SRC );
		if ( SRC[ExecPtr] == ')' ) ExecPtr++;
		return result;
	}
	if ( c == 0x89 ) while ( c == 0x89 ) c=SRC[++ExecPtr];	// +
	if ( ( c == 0x87 ) || ( c == 0x99 ) ) {	//  -
		ExecPtr++;
		return - Evalsub1( SRC );
	}
	if ( ( 'A'<=c )&&( c<='Z' ) || ( 'a'<=c )&&( c<='z' ) )  {
			ExecPtr++ ;
			return REG[c-'A'] ;
	}
	if ( ( c=='.' ) ||( c==0x0F ) || ( ( '0'<=c )&&( c<='9' ) ) ) {
			result = Eval_atof( SRC );
			return result ;
	}
	switch ( c ) { 			// ( type C function )  sin cos tan... 
		case 0x7F:	// 7F..
			c = SRC[ExecPtr+1];
			if ( c == 0x40 ) {	// Mat A[a,b]
				ExecPtr+=2;
				c = SRC[ExecPtr];
				if ( ( 'A'<=c )&&( c<='Z' ) ) {
					reg=c-'A';
					ExecPtr++ ;
					if ( SRC[ExecPtr] != '[' ) EvalError(SyntaxERR) ; // Syntax error 
					c = SRC[++ExecPtr];
					if ( SRC[ExecPtr+1] == ',' ) {
						ExecPtr++;
						if  ( ( '1'<= c ) && ( c<='9' ) ) dimA=c-'0';
							else if  ( ( 'A'<= c ) && ( c<='z' ) ) dimA=REG[c-'A'];
					} else {
						dimA=(EvalsubTop( SRC ));
						if ( SRC[ExecPtr] != ',' ) EvalError(SyntaxERR) ; // Syntax error 
					}
					if ( MatArySizeA[(reg)] < dimA ) EvalError(DimensionERR) ; // Dimension error 
					c = SRC[++ExecPtr];
					if ( SRC[ExecPtr+1] == ']' ) {
						ExecPtr++;
						if  ( ( '1'<= c ) && ( c<='9' ) ) dimB=c-'0';
						else if  ( ( 'A'<= c ) && ( c<='z' ) ) dimB=REG[c-'A'];
					} else {
						dimB=(EvalsubTop( SRC ));
						if ( SRC[ExecPtr] != ']' ) EvalError(SyntaxERR) ; // Syntax error 
					}
					if ( MatArySizeB[(reg)] < dimB ) EvalError(DimensionERR) ; // Dimension error 
					ExecPtr++ ;
					if ( ErrorNo ) return 0 ;
					result = MatAry[reg][(dimA-1)*MatArySizeB[reg]+dimB-1]   ;			// Matrix array ptr*
					return result ;
				}
			} else if ( c == 0x3A ) {	// MOD(a,b)
					ExecPtr+=2;
					tmp = floor(EvalsubTop( SRC ) +.5);
					if ( SRC[ExecPtr] != ',' ) EvalError(SyntaxERR) ; // Syntax error 
					ExecPtr++;
					tmp2 = floor( EvalsubTop( SRC ) +.5);
					if ( tmp2 == 0 ) { ErrorNo=MaERR; ErrorPtr=ExecPtr; } // Ma error 
					result= floor(fabs(fmod( tmp, tmp2 ))+.5);
					if ( result == tmp2  ) result--;
					if ( tmp < 0 ) {
						result=fabs(tmp2)-result;
						if ( result == tmp2  ) result=0;
					}
					if ( SRC[ExecPtr] == ')' ) ExecPtr++;
					return result ;
			} else if ( c == 0x8F ) {	// Getkey
					ExecPtr+=2;
					result = CB_Getkey() ;
					return result ;
			} else if ( c == 0x86 ) {	// RndFix(n,digit)
					ExecPtr+=2;
					tmp=(EvalsubTop( SRC ));
					if ( SRC[ExecPtr] != ',' ) EvalError(SyntaxERR) ; // Syntax error 
					ExecPtr++ ;	// ',' skip
					i = EvalsubTop( SRC ) +.5;
					if ( SRC[ExecPtr] == ')' ) ExecPtr++;
					result=Round( tmp, Fix, i) ;
					return result ;
			} else if ( c == 0x87 ) {	// RanInt#(st,en)
					ExecPtr+=2;
					x=(EvalsubTop( SRC ));
					if ( SRC[ExecPtr] != ',' ) EvalError(SyntaxERR) ; // Syntax error 
					ExecPtr++ ;	// ',' skip
					y=(EvalsubTop( SRC ));
					if ( SRC[ExecPtr] == ')' ) ExecPtr++;
					result=floor( ((double)rand()/(double)(RAND_MAX+1.0))*(x-y+1) ) +y ;
					return result ;
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
							EvalError(ArgumentERR);  // Argument error
							break;
					}
					ptr=ExecPtr;
					ExecPtr= 0;
					result= EvalsubTop( GraphY );
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
			return CB_CurrentValue ;
		case 0xD0 :	// PI
			ExecPtr++;
			return PI ;
		case 0xC1 :	// Ran#
			c = SRC[++ExecPtr];
			if ( ( '1' <= c ) && ( c <= '9' ) ) {
					ExecPtr++;
					srand( c-'0' );
			}
			result=(double)rand()/(double)(RAND_MAX+1.0);
			return result ;
		case 0x97 :	// abs
			ExecPtr++; result = fabs( Evalsub5( SRC ) );
			return result ;
		case 0xA6 :	// int
			ExecPtr++; result = floor( Evalsub5( SRC ) );
			return result ;
		case 0xB6 :	// frac
			ExecPtr++; result = frac( Evalsub5( SRC ) );
			return result ;
		case 0x86 :	// sqr
			ExecPtr++; result = sqrt( Evalsub5( SRC ) );
			pt=(unsigned char *)(&result); if ( pt[0]==0x7F ) EvalError(MaERR) ; // Math error
			return result ;
		case 0x95 :	// log10
			ExecPtr++; result = log10( Evalsub5( SRC ) );
			pt=(unsigned char *)(&result); if ( pt[0]==0x7F ) EvalError(MaERR) ; // Math error
			return result ;
		case 0xB5 :	// 10^
			ExecPtr++; result = pow(10, Evalsub5( SRC ) );
			pt=(unsigned char *)(&result); if ( pt[0]==0x7F ) EvalError(MaERR) ; // Math error
			return result ;
		case 0x85 :	// ln
			ExecPtr++; result = log( Evalsub5( SRC ) );
			pt=(unsigned char *)(&result); if ( pt[0]==0x7F ) EvalError(MaERR) ; // Math error
			return result ;
		case 0xA5 :	// expn
			ExecPtr++; result = exp( Evalsub5( SRC ) );
			pt=(unsigned char *)(&result); if ( pt[0]==0x7F ) EvalError(MaERR) ; // Math error
			return result ;
		case 0xA7 :	// Not
			ExecPtr++; result = ! (int) ( Evalsub5( SRC ) );
			return result ;
		case 0x96 :	// cuberoot
			ExecPtr++; result = pow( Evalsub5( SRC ), 1.0/3.0 );
			pt=(unsigned char *)(&result); if ( pt[0]==0x7F ) EvalError(MaERR) ; // Math error
			return result ;
		case 0x81 :	// sin
			ExecPtr++;
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
			pt=(unsigned char *)(&result); if ( pt[0]==0x7F ) EvalError(MaERR) ; // Math error
			return result ;
		case 0x82 :	// cos
			ExecPtr++;
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
			pt=(unsigned char *)(&result); if ( pt[0]==0x7F ) EvalError(MaERR) ; // Math error
			return result ;
		case 0x83 :	// tan
			ExecPtr++;
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
			pt=(unsigned char *)(&result); if ( pt[0]==0x7F ) EvalError(MaERR) ; // Math error
			return result ;
		case 0x91 :	// asin
			ExecPtr++;
			switch ( Angle ) { 
				case 0:	// Deg
					result = asin( Evalsub5( SRC ) )*180./PI ;
					break;
				case 1:	// Rad
					result = asin( Evalsub5( SRC ) ) ;
					break;
				case 2:	// Grad
					result = asin( Evalsub5( SRC ) )*299./PI ;
					break;
			}
			pt=(unsigned char *)(&result); if ( pt[0]==0x7F ) EvalError(MaERR) ; // Math error
			return result ;
		case 0x92 :	// acos
			ExecPtr++;
			switch ( Angle ) { 
				case 0:	// Deg
					result = acos( Evalsub5( SRC ) )*180./PI ;
					break;
				case 1:	// Rad
					result = acos( Evalsub5( SRC ) ) ;
					break;
				case 2:	// Grad
					result = acos( Evalsub5( SRC ) )*299./PI ;
					break;
			}
			pt=(unsigned char *)(&result); if ( pt[0]==0x7F ) EvalError(MaERR) ; // Math error
			return result ;
		case 0x93 :	// atan
			ExecPtr++;
			switch ( Angle ) { 
				case 0:	// Deg
					result = atan( Evalsub5( SRC ) )*180./PI ;
					break;
				case 1:	// Rad
					result = atan( Evalsub5( SRC ) ) ;
					break;
				case 2:	// Grad
					result = atan( Evalsub5( SRC ) )*299./PI ;
					break;
			}
			pt=(unsigned char *)(&result); if ( pt[0]==0x7F ) EvalError(MaERR) ; // Math error
			return result ;
		case 0xA1 :	// sinh
			ExecPtr++; result = sinh( Evalsub5( SRC ) );
			pt=(unsigned char *)(&result); if ( pt[0]==0x7F ) EvalError(MaERR) ; // Math error
			return result ;
		case 0xA2 :	// cosh
			ExecPtr++; result = cosh( Evalsub5( SRC ) );
			pt=(unsigned char *)(&result); if ( pt[0]==0x7F ) EvalError(MaERR) ; // Math error
			return result ;
		case 0xA3 :	// tanh
			ExecPtr++; result = tanh( Evalsub5( SRC ) );
			pt=(unsigned char *)(&result); if ( pt[0]==0x7F ) EvalError(MaERR) ; // Math error
			return result ;
		case 0xB1 :	// asinh
			ExecPtr++; result = asinh( Evalsub5( SRC ) );
			pt=(unsigned char *)(&result); if ( pt[0]==0x7F ) EvalError(MaERR) ; // Math error
			return result ;
		case 0xB2 :	// acosh
			ExecPtr++; result = acosh( Evalsub5( SRC ) );
			pt=(unsigned char *)(&result); if ( pt[0]==0x7F ) EvalError(MaERR) ; // Math error
			return result ;
		case 0xB3 :	// atanh
			ExecPtr++; result = atanh( Evalsub5( SRC ) );
			pt=(unsigned char *)(&result); if ( pt[0]==0x7F ) EvalError(MaERR) ; // Math error
			return result ;
		case 0xF7:	// F7..
			c = SRC[ExecPtr+1];
			if ( c == 0xAF ) {	// PxlTest(y,x)
					ExecPtr+=2;
					y=(EvalsubTop( SRC ));
					if ( SRC[ExecPtr] != ',' ) EvalError(SyntaxERR) ; // Syntax error 
					ExecPtr++ ;	// ',' skip
					x=(EvalsubTop( SRC ));
					if ( SRC[ExecPtr] != ')' ) EvalError(SyntaxERR) ; // Syntax error 
					ExecPtr++ ;
					result = PxlTest(y, x) ;			// 
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
		default:
			break;
	}
	EvalError(SyntaxERR) ; // Syntax error 
	return 0 ;
}

double Evalsub2(unsigned char *SRC) {	//  2nd Priority  ( type B function ) ...
	int cont=1;
	double result,tmp;
	unsigned int c;
	result = Evalsub1( SRC );
	while ( 1 ) {
		c = SRC[ExecPtr++];
		switch ( c ) {
			case  0x8B  :	// ^2
				result *= result ;
				break;
			case  0x9B  :	// ^(-1) RECIP
				if ( result == 0 ) EvalError(MaERR) ; // Math error
				result = 1 / result ;
				break;
			case  0xAB  :	//  !
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
				
			default:
				ExecPtr--;
				return result;
				break;
		}
	 }
	return result;
}
double Evalsub4(unsigned char *SRC) {	//  3rd Priority  ( ^ ...)
	double result,tmp;
	unsigned int c;
	unsigned char *pt;
	result = Evalsub2( SRC );
	while ( 1 ) {
		c = SRC[ExecPtr++];
		switch ( c ) {
			case  0xA8  :	// a ^ b
				result = pow( result, Evalsub2( SRC ) );
				pt=(unsigned char *)(&result); if ( pt[0]==0x7F ) EvalError(MaERR) ; // Math error
				break;
			case  0xB8  :	// powroot
				result = pow( Evalsub2( SRC ), 1/result );
				pt=(unsigned char *)(&result); if ( pt[0]==0x7F ) EvalError(MaERR) ; // Math error
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
double Evalsub4(unsigned char *SRC) {	//  4th Priority  (Fraction) a/b/c
	double result,frac1,frac2,frac3;
	unsigned int c;
	result = Evalsub3( SRC );
	c = SRC[ExecPtr];
	if ( c == 0xBB ) {
		ExecPtr++;
		frac1 = result ;
		frac2 = Evalsub3( SRC );
		c = SRC[ExecPtr];
		if ( c == 0xBB ) {
			ExecPtr++;
			frac3 = Evalsub3( SRC );
			if ( frac3 == 0 ) EvalError(MaERR) ; // Math error
			result = frac1 + ( frac2 / frac3 ) ;
		} else {
			if ( frac2 == 0 ) EvalError(MaERR) ; // Math error
			result = ( frac1 / frac2 ) ;
		}
	}
	return result;
}
*/
double Evalsub5(unsigned char *SRC) {	//  5th Priority
	double result,tmp;
	unsigned int c;
	int dimA,dimB,reg,x,y;
	result = Evalsub4( SRC );
	while ( 1 ) {
		c = SRC[ExecPtr];
		if ((( 'A' <= c ) && ( c <= 'Z' )) ||
			(( 'a' <= c ) && ( c <= 'z' )) ||
			 ( c == 0xD0 ) || // PI
			 ( c == 0xC0 ) || // Ans
			 ( c == 0xC1 )) { // Ran#
				result *= Evalsub4( SRC ) ;
		} else if ( c == 0x7F ) { // 7F..
			c = SRC[ExecPtr+1];
			switch ( c ) {
				case 0x40:	// Mat A[a,b]
				case 0x3A:	// MOD(a,b)
				case 0x8F:	// Getkey
				case 0x86:	// RndFix(n,digit)
				case 0x87:	// RanInt#(st,en)
				case 0xF0:	// GraphY
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
		} else if ( c == 0xF7 ) { // F7..
			c = SRC[ExecPtr+1];
			switch ( c ) {
				case 0xAF:	// PxlTest(y,x)
					result *= Evalsub4( SRC ) ;
					break;
				default:
					return result;
					break;
			}
		} else if ( c == 0xF9 ) { // F9..
			c = SRC[ExecPtr+1];
			switch ( c ) {
				case 0x21:	// Xdot
					result *= Evalsub4( SRC ) ;
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
double Evalsub6(unsigned char *SRC) {	//  6th Priority  ( type C function )  sin cos tan... 
	int cont=1;
	double result,tmp;
	unsigned int c;
	result = Evalsub5( SRC );
	return result;
}
*/
double Evalsub7(unsigned char *SRC) {	//  7th Priority
	double result,tmp;
	unsigned int c;
	result = Evalsub5( SRC );
	while ( 1 ) {
		c = SRC[ExecPtr];
		switch ( c ) {
			case '(' :
				ExecPtr++; result *= EvalsubTop( SRC );
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
			case 0x81 :	// sin
			case 0x82 :	// cos
			case 0x83 :	// tan
			case 0x91 :	// asin
			case 0x92 :	// acos
			case 0x93 :	// atan
			case 0xA1 :	// sinh
			case 0xA2 :	// cosh
			case 0xA3 :	// tanh
			case 0xB1 :	// asinh
			case 0xB2 :	// acosh
			case 0xB3 :	// atanh
				result *= Evalsub5( SRC );
				break;
			default:
				return result;
				break;
		}
	 }
	return result;
}
/*
double Evalsub8(unsigned char *SRC) {	//  8th Priority
	double result,tmp;
	unsigned int c;
	result = Evalsub7( SRC );
	return result;
}
double Evalsub9(unsigned char *SRC) {	//  9th Priority
	double result,tmp;
	unsigned int c;
	result = Evalsub8( SRC );
	return result;
}
*/
double Evalsub10(unsigned char *SRC) {	//  10th Priority  ( *,/, int.,Rmdr )
	double result,tmp;
	unsigned int c;
	result = Evalsub7( SRC );
	while ( 1 ) {
		c = SRC[ExecPtr++];
		switch ( c ) {
			case 0xA9 :		// ~
				result *= Evalsub7( SRC );
				break;
			case 0xB9 :		// €
				tmp = Evalsub7( SRC );
				if ( tmp == 0 ) EvalError(MaERR) ; // Math error
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
 
double Evalsub11(unsigned char *SRC) {	//  11th Priority  ( +,- )
	double result;
	unsigned int c;
	result = Evalsub10( SRC );
	while ( 1 ) {
		c = SRC[ExecPtr++];
		switch ( c ) {
			case 0x89 :		// +
				result += Evalsub10( SRC );
				break;
			case 0x99 :		// -
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

double Evalsub12(unsigned char *SRC) {	//  12th Priority ( =,!=,><,>=,<= )
	double result;
	unsigned int c;
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

double Evalsub13(unsigned char *SRC) {	//  13th Priority  ( And,and)
	double result;
	unsigned int c;
	result = Evalsub12( SRC );
	while ( 1 ) {
		c = SRC[ExecPtr];
		if ( c == 0x7F ) {
			c = SRC[ExecPtr+1];
			switch ( c ) {
				case 0xB0 :	// And
					ExecPtr+=2;
					result = ( (int)result & (int)Evalsub13( SRC ) );
					break;
				default:
					return result;
					break;
			}
		} else return result;
	}
}

double EvalsubTop(unsigned char *SRC) {	//  14th Priority  ( Or,Xor,or,xor,xnor )
	double result;
	unsigned int c;
	result = Evalsub13( SRC );
	while ( 1 ) {
		c = SRC[ExecPtr];
		if ( c == 0x7F ) {
			c = SRC[ExecPtr+1];
			switch ( c ) {
				case 0xB1 :	// Or
					ExecPtr+=2;
					result = ( (int)result | (int)Evalsub13( SRC ) );
					break;
				case 0xB4 :	// Xor
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

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
double Eval(unsigned char *SRC) {		// Eval temp
	double result;
	int execptr=ExecPtr;
    ExecPtr= 0;
    ErrorPtr= 0;
	ErrorNo = 0;
	if ( strlen((char*)SRC) == 0 ) { ExecPtr=execptr; return 0; }
    result = EvalsubTop( SRC );
    if ( ExecPtr < strlen((char*)SRC) ) EvalError(SyntaxERR) ; // Syntax error 
    if ( ErrorNo ) { CB_ErrNo( ErrorNo ); }
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
	if ( key==KEY_CTRL_AC  ) return 0;
	result = Eval( ExpBuffer );
	while ( ErrorNo || (strlen((char*)ExpBuffer)==0) ) {	// error loop
		key=InputStrSub( x, y, width, ErrorPtr, ExpBuffer, 64, ' ', REV_OFF, FLOAT_ON, EXP_ON, ALPHA_ON, HEX_OFF, PAL_OFF, EXIT_CANCEL_ON );
		if ( key==KEY_CTRL_AC  ) return 0;
		result = Eval( ExpBuffer );
	}
	return result; // value ok
}
double InputNumD_CB1(int x, int y, int width, double defaultNum) {		//  Basic Input 1
	unsigned int key;
	double result;
	ExpBuffer[0]='\0';
	key=InputStrSub( x, y, width, 0, ExpBuffer, 64, ' ', REV_OFF, FLOAT_ON, EXP_ON, ALPHA_ON, HEX_OFF, PAL_OFF, EXIT_CANCEL_ON );
	if ( key==KEY_CTRL_AC  ) return 0;
	if ( ExpBuffer[0]=='\0' ) if ( key==KEY_CTRL_EXE ) return (defaultNum);
	result = Eval( ExpBuffer );
	while ( ErrorNo || (strlen((char*)ExpBuffer)==0) ) {	// error loop
		key=InputStrSub( x, y, width, ErrorPtr, ExpBuffer, 64, ' ', REV_OFF, FLOAT_ON, EXP_ON, ALPHA_ON, HEX_OFF, PAL_OFF, EXIT_CANCEL_ON );
		if ( key==KEY_CTRL_AC  ) return 0;
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
	if ( key==KEY_CTRL_AC  ) return 0;
	result = Eval( ExpBuffer );
	while ( ErrorNo ) {	// error loop
		key=InputStrSub( x, y, width, ErrorPtr, ExpBuffer, 64, ' ', REV_OFF, FLOAT_ON, EXP_ON, ALPHA_ON, HEX_OFF, PAL_OFF, EXIT_CANCEL_ON );
		if ( key==KEY_CTRL_AC  ) return 0;
		result = Eval( ExpBuffer );
	}
	return result; // value ok
}

