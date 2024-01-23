/*
===============================================================================

 Casio Basic RUNTIME library for fx-9860G series  v0.10

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
char ExpBuffer[ExpMax+1];
int ExpPtr;	// Expression buffer ptr
//-----------------------------------------------------------------------------

double InputNumD_full(int x, int y, int width, double defaultNum) {		// full number display
	double result;
	sprintG(ExpBuffer, defaultNum, 30, LEFT_ALIGN);
	if ( InputStrSub( x, y, width, 0, ExpBuffer, 64, ' ', REV_OFF, FLOAT_ON, EXP_ON, ALPHA_ON, HEX_OFF, PAL_ON, EXIT_CANCEL_OFF) ) return (defaultNum);
	result = Eval( ExpBuffer );
	while ( ErrorNo ) {	// error loop
		if ( InputStrSub( x, y, width, ErrorPtr, ExpBuffer, 64, ' ', REV_OFF, FLOAT_ON, EXP_ON, ALPHA_ON, HEX_OFF, PAL_ON, EXIT_CANCEL_OFF) ) return (defaultNum);
		result = Eval( ExpBuffer );
	}
	return result; // value ok
}

double InputNumD_Char(int x, int y, int width, double defaultNum, char code) {		//  1st char key in
	double result;
	ExpBuffer[0]=code;
	ExpBuffer[1]='\0';
	if ( InputStrSub( x, y, width, 1, ExpBuffer, 64, ' ', REV_OFF, FLOAT_ON, EXP_ON, ALPHA_ON, HEX_OFF, PAL_ON, EXIT_CANCEL_OFF) ) return (defaultNum);
	result = Eval( ExpBuffer );
	while ( ErrorNo ) {	// error loop
		if ( InputStrSub( x, y, width, ErrorPtr, ExpBuffer, 64, ' ', REV_OFF, FLOAT_ON, EXP_ON, ALPHA_ON, HEX_OFF, PAL_ON, EXIT_CANCEL_OFF) ) return (defaultNum);
		result = Eval( ExpBuffer );
	}
	return result; // value ok
}

double InputNumD_replay(int x, int y, int width, double defaultNum) {		//  replay expression
	double result;
	if ( InputStrSub( x, y, width, strlen(ExpBuffer), ExpBuffer, 64, ' ', REV_OFF, FLOAT_ON, EXP_ON, ALPHA_ON, HEX_OFF, PAL_ON, EXIT_CANCEL_OFF) ) return (defaultNum);
	result = Eval( ExpBuffer );
	while ( ErrorNo ) {	// error loop
		if ( InputStrSub( x, y, width, ErrorPtr, ExpBuffer, 64, ' ', REV_OFF, FLOAT_ON, EXP_ON, ALPHA_ON, HEX_OFF, PAL_ON, EXIT_CANCEL_OFF) ) return (defaultNum);
		result = Eval( ExpBuffer );
	}
	return result; // value ok
}

double InputNumD_CB(int x, int y, int width, double defaultNum) {		//  Basic Input
	unsigned int key;
	double result;
	ExpBuffer[0]='\0';
	key=InputStrSub( x, y, width, 0, ExpBuffer, 64, ' ', REV_OFF, FLOAT_ON, EXP_ON, ALPHA_ON, HEX_OFF, PAL_OFF, EXIT_CANCEL_ON );
	if ( key==KEY_CTRL_AC  ) return 0;
	result = Eval( ExpBuffer );
	while ( ErrorNo || (strlen(ExpBuffer)==0) ) {	// error loop
		key=InputStrSub( x, y, width, ErrorPtr, ExpBuffer, 64, ' ', REV_OFF, FLOAT_ON, EXP_ON, ALPHA_ON, HEX_OFF, PAL_OFF, EXIT_CANCEL_ON );
		if ( key==KEY_CTRL_AC  ) return 0;
		result = Eval( ExpBuffer );
	}
	return result; // value ok
}
double InputNumD_CB2(int x, int y, int width, double defaultNum) {		//  Basic Input 2
	unsigned int key;
	double result;
	sprintG(ExpBuffer, defaultNum, 30, LEFT_ALIGN);
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
//-----------------------------------------------------------------------------
int lastrandom=0x12345678;
unsigned int random( int seed  ){
    if (seed) lastrandom=seed;
    lastrandom = ( 0x41C64E6D*lastrandom ) + 0x3039; // + RTC_GetTicks();
    return ( ( lastrandom >> 16 ) & 0xFFFF );
}

int intlog10( double n ) {
}


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
unsigned char Eval_atofNumDiv(char *expbuf, double *num ){
	unsigned char c;
	double a=10;
	c = expbuf[ExpPtr];
	while ( ('0'<=c)&&(c<='9') ) {
		(*num) = (*num) + (double)(c-'0')/a;
		a*=10;
		c=expbuf[++ExpPtr];
	}
	return c;
}
unsigned char Eval_atofNumMult(char *expbuf, double *num ){
	unsigned char c;
	c = expbuf[ExpPtr];
	while ( ('0'<=c)&&(c<='9') ) {
		(*num) = (*num)*10 +(c-'0');
		c=expbuf[++ExpPtr];
	}
	return c;
}
double Eval_atof(char *expbuf) {
	char buffer[64];
	double temp,mantissa=0,exponent=0,result;
	double sign=1;
	unsigned char c;
	c = expbuf[ExpPtr];
	if ( c == '.'  )   {
			ExpPtr++;
			c=Eval_atofNumDiv(expbuf, &mantissa);	// .123456
	} else { 
			c=Eval_atofNumMult(expbuf, &mantissa);	// 123456
			if ( c == '.'  ) {
				ExpPtr++;
				c=Eval_atofNumDiv(expbuf, &mantissa);	// 123456.789
			}
	}
	if ( c == 0x0F  ) { // exp
			c=expbuf[++ExpPtr];
			if ( c == 0x89 ) c=expbuf[++ExpPtr];
			if ( ( c == 0x87 )|| ( c == 0x99 ) ) { sign=-1; c=expbuf[++ExpPtr]; }	// (-)
			if ( ( '0'<=c )&&( c<='9' ) ) c=Eval_atofNumMult(expbuf, &exponent);
			else { ErrorNo=SyntaxERR; ErrorPtr=ExpPtr; } //  error 
		result = mantissa * pow(10,exponent*sign) ;
		return ( result ) ;
	} else
		return ( mantissa ) ;
}

//-----------------------------------------------------------------------------

double EvalGetNum(char *expbuf){
	double result,tmp,tmp2;
	unsigned char c,d;
	int dimA,dimB,reg,x,y;
	int ptr,mptr;
	c = expbuf[ExpPtr];
	if ( ( c == '+' ) || ( c == 0x89 ) ) while ( ( c == '+' ) || ( c == 0x89 ) ) c=expbuf[++ExpPtr];
	if ( ( c == '-' ) || ( c == 0x87 ) || ( c == 0x99 ) ) {	//  -
		ExpPtr++;
		return - EvalGetNum( expbuf );
	}
	if ( ( 'A'<=c )&&( c<='Z' ) ) {
			ExpPtr++ ;
			return REG[c-'A'] ;
	}
	if ( c==0xD0 ) { // PI
			ExpPtr++;
			return PI ;
	}
	if ( c==0xC0 ) { // Ans
			ExpPtr++;
			return CB_CurrentValue ;
	}
	if ( ( c=='.' ) || ( ( '0'<=c )&&( c<='9' ) ) ) {
			result = Eval_atof( expbuf );
			return result ;
	}
	switch ( c ) { 			// ( type C function )  sin cos tan... 
		case 0x97 :	// abs
			ExpPtr++; result = fabs( Evalsub5( expbuf ) );
			return result ;
		case 0xA6 :	// int
			ExpPtr++; result = floor( Evalsub5( expbuf ) );
			return result ;
		case 0xB6 :	// frac
			ExpPtr++; result = frac( Evalsub5( expbuf ) );
			return result ;
		case 0x86 :	// sqr
			ExpPtr++; result = sqrt( Evalsub5( expbuf ) );
			return result ;
		case 0x81 :	// sin
			ExpPtr++;
			switch ( Angle ) { 
				case 0:	// Deg
					result = sin( Evalsub5( expbuf )*PI/180.);
					break;
				case 1:	// Rad
					result = sin( Evalsub5( expbuf ));
					break;
				case 2:	// Grad
					result = sin( Evalsub5( expbuf )*PI/200. );
					break;
			}
			return result ;
		case 0x82 :	// cos
			ExpPtr++;
			switch ( Angle ) { 
				case 0:	// Deg
					result = cos( Evalsub5( expbuf )*PI/180.);
					break;
				case 1:	// Rad
					result = cos( Evalsub5( expbuf ));
					break;
				case 2:	// Grad
					result = cos( Evalsub5( expbuf )*PI/200. );
					break;
			}
			return result ;
		case 0x83 :	// tan
			ExpPtr++;
			switch ( Angle ) { 
				case 0:	// Deg
					result = tan( Evalsub5( expbuf )*PI/180.);
					break;
				case 1:	// Rad
					result = tan( Evalsub5( expbuf ));
					break;
				case 2:	// Grad
					result = tan( Evalsub5( expbuf )*PI/200. );
					break;
			}
			return result ;
		case 0x91 :	// asin
			ExpPtr++;
			switch ( Angle ) { 
				case 0:	// Deg
					result = asin( Evalsub5( expbuf ) )*180./PI ;
					break;
				case 1:	// Rad
					result = asin( Evalsub5( expbuf ) ) ;
					break;
				case 2:	// Grad
					result = asin( Evalsub5( expbuf ) )*299./PI ;
					break;
			}
			return result ;
		case 0x92 :	// acos
			ExpPtr++;
			switch ( Angle ) { 
				case 0:	// Deg
					result = acos( Evalsub5( expbuf ) )*180./PI ;
					break;
				case 1:	// Rad
					result = acos( Evalsub5( expbuf ) ) ;
					break;
				case 2:	// Grad
					result = acos( Evalsub5( expbuf ) )*299./PI ;
					break;
			}
			return result ;
		case 0x93 :	// atan
			ExpPtr++;
			switch ( Angle ) { 
				case 0:	// Deg
					result = atan( Evalsub5( expbuf ) )*180./PI ;
					break;
				case 1:	// Rad
					result = atan( Evalsub5( expbuf ) ) ;
					break;
				case 2:	// Grad
					result = atan( Evalsub5( expbuf ) )*299./PI ;
					break;
			}
			return result ;
		case 0xA1 :	// sinh
			ExpPtr++; result = sinh( Evalsub5( expbuf ) );
			return result ;
		case 0xA2 :	// cosh
			ExpPtr++; result = cosh( Evalsub5( expbuf ) );
			return result ;
		case 0xA3 :	// tanh
			ExpPtr++; result = tanh( Evalsub5( expbuf ) );
			return result ;
		case 0xB1 :	// asinh
			ExpPtr++; result = asinh( Evalsub5( expbuf ) );
			return result ;
		case 0xB2 :	// acosh
			ExpPtr++; result = acosh( Evalsub5( expbuf ) );
			return result ;
		case 0xB3 :	// atanh
			ExpPtr++; result = atanh( Evalsub5( expbuf ) );
			return result ;
		case 0x85 :	// ln
			ExpPtr++; result = log( Evalsub5( expbuf ) );
			return result ;
		case 0x95 :	// log10
			ExpPtr++; result = log10( Evalsub5( expbuf ) );
			return result ;
		case 0x96 :	// cuberoot
			ExpPtr++; result = pow( Evalsub5( expbuf ), 1.0/3.0 );
			return result ;
		case 0xA5 :	// expn
			ExpPtr++; result = exp( Evalsub5( expbuf ) );
			return result ;
		case 0xA7 :	// Not
			ExpPtr++; result = ! (int) ( Evalsub5( expbuf ) );
			return result ;
		case 0xB5 :	// 10^
			ExpPtr++; result = pow(10, Evalsub5( expbuf ) );
			return result ;
		case 0xC1 :	// Ran#
			ExpPtr++;
			result=(double)random(0)/65536.;
			return result ;
		case 0x7F:	// 7F..
			c = expbuf[ExpPtr+1];
			if ( c == 0x40 ) {	// Mat A[a,b]
				ExpPtr+=2;
				c = expbuf[ExpPtr];
				if ( ( 'A'<=c )&&( c<='Z' ) ) {
					reg=c-'A';
					ExpPtr++ ;
					if ( expbuf[ExpPtr] != '[' ) { ErrorNo=SyntaxERR; ErrorPtr=ExpPtr; } // error 
					ExpPtr++ ;
					dimA=floor(EvalsubTop( expbuf ));
					if ( MatArySizeA[reg] < dimA ) { ErrorNo=DimensionERR; ErrorPtr=ExpPtr; } // Dimension error 
					if ( expbuf[ExpPtr] != ',' ) { ErrorNo=SyntaxERR; ErrorPtr=ExpPtr; } // error 
					ExpPtr++ ;
					dimB=floor(EvalsubTop( expbuf ));
					if ( MatArySizeB[reg] < dimB ) { ErrorNo=DimensionERR; ErrorPtr=ExpPtr; } // Dimension error 
					if ( expbuf[ExpPtr] != ']' ) { ErrorNo=SyntaxERR; ErrorPtr=ExpPtr; } // error 
					ExpPtr++ ;
					result = MatAry[reg][(dimA-1)*MatArySizeB[reg]+dimB-1]   ;			// Matrix array ptr*
					return result ;
				}
			} else if ( c == 0x3A ) {	// MOD(a,b)
				ExpPtr+=2;
				tmp = EvalsubTop( expbuf );
				if ( expbuf[ExpPtr] != ',' ) { ErrorNo=SyntaxERR; ErrorPtr=ExpPtr; } // error 
				ExpPtr++;
				tmp2 = EvalsubTop( expbuf );
				if ( tmp2 == 0 ) { ErrorNo=MaERR; ErrorPtr=ExpPtr; } // Ma error 
				result= fmod( tmp, tmp2 ) ;
				if ( expbuf[ExpPtr] == ')' ) ExpPtr++;
				return result ;
			} else if ( c == 0x8F ) {	// Getkey
					ExpPtr+=2;
					result = CB_Getkey() ;
					return result ;
			} else if ( c == 0x87 ) {	// RanInt#(st,en)
					ExpPtr+=2;
					x=floor(EvalsubTop( expbuf ));
					if ( expbuf[ExpPtr] != ',' ) { ErrorNo=SyntaxERR; ErrorPtr=ExpPtr; } // error 
					ExpPtr++ ;	// ',' skip
					y=floor(EvalsubTop( expbuf ));
					if ( expbuf[ExpPtr] != ')' ) { ErrorNo=SyntaxERR; ErrorPtr=ExpPtr; } // error 
					ExpPtr++ ;
					result=floor(((double)random(0)/65536.)*(x-y+1))+y;
					return result ;
			} else if ( c == 0xF0 ) {	// GraphY
					ExpPtr+=2;
					c=expbuf[ExpPtr++];
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
							{ ErrorNo=ArgumentERR; ErrorPtr=ExpPtr; } // Argument error
							break;
					}
					ptr=ExpPtr;
					ExpPtr= 0;
					result= EvalsubTop( GraphY );
					ExpPtr=ptr;
					return result ;
			} else if ( c == 0x00 ) {	// Xmin
					ExpPtr+=2;
					return Xmin;
			} else if ( c == 0x01 ) {	// Xmax
					ExpPtr+=2;
					return Xmax;
			} else if ( c == 0x02 ) {	// Xscl
					ExpPtr+=2;
					return Xscl;
			} else if ( c == 0x04 ) {	// Ymin
					ExpPtr+=2;
					return Ymin;
			} else if ( c == 0x05 ) {	// Ymax
					ExpPtr+=2;
					return Ymax;
			} else if ( c == 0x06) {	// Yscl
					ExpPtr+=2;
					return Yscl;
			} else if ( c == 0x0B ) {	// Xfct
					ExpPtr+=2;
					return Xfct;
			} else if ( c == 0x0C ) {	// Yfct
					ExpPtr+=2;
					return Yfct;
			}
			break;
		case 0xF7:	// F7..
			c = expbuf[ExpPtr+1];
			if ( c == 0xAF ) {	// PxlTest(y,x)
					ExpPtr+=2;
					y=floor(EvalsubTop( expbuf ));
					if ( expbuf[ExpPtr] != ',' ) { ErrorNo=SyntaxERR; ErrorPtr=ExpPtr; } // error 
					ExpPtr++ ;	// ',' skip
					x=floor(EvalsubTop( expbuf ));
					if ( expbuf[ExpPtr] != ')' ) { ErrorNo=SyntaxERR; ErrorPtr=ExpPtr; } // error 
					ExpPtr++ ;
					result = PxlTest(y, x) ;			// 
					return result ;
			}
			break;
		case 0xF9:	// F9..
			c = expbuf[ExpPtr+1];
			if ( c == 0x21 ) {	// Xdot
					ExpPtr+=2;
					return Xdot;
			}
			break;
		default:
			break;
	}
	{ ErrorNo=SyntaxERR; ErrorPtr=ExpPtr; } // error 
	return 0 ;
}


double Evalsub1(char *expbuf) {	// 1th Priority
	double result;
	unsigned char c;
	c = expbuf[ExpPtr];
	if ( c == '(') {
		ExpPtr++;
		result = EvalsubTop( expbuf );
		if ( expbuf[ExpPtr] == ')' ) ExpPtr++;
		return result;
	 } else 
	 	return EvalGetNum( expbuf );
}

double Evalsub2(char *expbuf) {	//  2nd Priority  ( type B function ) ...
	int cont=1;
	double result,tmp;
	unsigned char c;
	result = Evalsub1( expbuf );
	while ( cont ) {
		c = expbuf[ExpPtr];
		switch ( c ) {
			case  0xAB  :	//  !
			case  '!'  :	//  !
				ExpPtr++;
				tmp = floor( result );
				result = 1;
				while ( tmp > 0 ) { result *= tmp; tmp--; }
				break;
			case  0x8B  :	// ^2
				ExpPtr++;
				result *= result ;
				break;
			case  0x9B  :	// ^(-1) RECIP
				ExpPtr++;
				result = 1 / result ;
				break;
			default:
				cont=0;
				break;
		}
	 }
	return result;
}
double Evalsub3(char *expbuf) {	//  3rd Priority  ( ^ ...)
	int cont=1;
	double result,tmp;
	unsigned char c;
	result = Evalsub2( expbuf );
	while ( cont ) {
		c = expbuf[ExpPtr];
		switch ( c ) {
			case  0xA8  :	// a ^ b
			case  '^'  :
				ExpPtr++;
				result = pow( result, Evalsub2( expbuf ) );
				break;
			case  0xB8  :	// powroot
				ExpPtr++;
				if ( result == 0 ) { ErrorNo=MaERR; ErrorPtr=ExpPtr; } // error 
				result = pow( Evalsub2( expbuf ), 1/result );
				break;
			default:
				cont=0;
				break;
		}
	 }
	return result;
}
double Evalsub4(char *expbuf) {	//  4th Priority  (Fraction) a/b/c
	double result,frac1,frac2,frac3;
	unsigned char c;
	result = Evalsub3( expbuf );
	c = expbuf[ExpPtr];
	if ( c == 0xBB ) {
		ExpPtr++;
		frac1 = result ;
		frac2 = Evalsub3( expbuf );
		c = expbuf[ExpPtr];
		if ( c == 0xBB ) {
			ExpPtr++;
			frac3 = Evalsub3( expbuf );
			if ( frac3 == 0 ) { ErrorNo=MaERR; ErrorPtr=ExpPtr; } // error 
			result = frac1 + ( frac2 / frac3 ) ;
		} else {
			if ( frac2 == 0 ) { ErrorNo=MaERR; ErrorPtr=ExpPtr; } // error 
			result = ( frac1 / frac2 ) ;
		}
	}
	return result;
}
double Evalsub5(char *expbuf) {	//  5th Priority
	int cont=1;
	double result,tmp;
	unsigned char c;
	int dimA,dimB,reg,x,y;
	result = Evalsub4( expbuf );
	while ( cont ) {
		c = expbuf[ExpPtr];
		if ((( 'A' <= c ) && ( c <= 'Z' )) ||
			 ( c == 0xD0 ) || // PI
			 ( c == 0xC0 ) || // Ans
			 ( c == 0xC1 )) { // Ran#
				result *= Evalsub4( expbuf ) ;
		} else if ( c == 0x7F ) { // 7F..
			c = expbuf[ExpPtr+1];
			switch ( c ) {
				case 0x40:	// Mat A[a,b]
				case 0x3A:	// MOD(a,b)
				case 0x8F:	// Getkey
				case 0x87:	// RanInt#(st,en)
				case 0x00:	// Xmin
				case 0x01:	// Xmax
				case 0x02:	// Xscl
				case 0x04:	// Ymin
				case 0x05:	// Ymax
				case 0x06:	// Yscl
					result *= Evalsub4( expbuf ) ;
					break;
				default:
					cont=0;
					break;
			}
		} else if ( c == 0xF7 ) { // F7..
			c = expbuf[ExpPtr+1];
			switch ( c ) {
				case 0xAF:	// PxlTest(y,x)
					result *= Evalsub4( expbuf ) ;
					break;
				default:
					cont=0;
					break;
			}
		} else if ( c == 0xF9 ) { // F9..
			c = expbuf[ExpPtr+1];
			switch ( c ) {
				case 0x21:	// Xdot
					result *= Evalsub4( expbuf ) ;
					break;
				default:
					cont=0;
					break;
			}
		} else if ( c == 0xE7 ) { // E7..
			c = expbuf[ExpPtr+1];
			switch ( c ) {
				default:
					cont=0;
					break;
			}
		} else cont=0;
	 }
	return result;
}
double Evalsub6(char *expbuf) {	//  6th Priority  ( type C function )  sin cos tan... 
	int cont=1;
	double result,tmp;
	unsigned char c;
	result = Evalsub5( expbuf );
	return result;
}
double Evalsub7(char *expbuf) {	//  7th Priority
	int cont=1;
	double result,tmp;
	unsigned char c;
	result = Evalsub6( expbuf );
	while ( cont ) {
		c = expbuf[ExpPtr];
		switch ( c ) {
			case '(' :
				ExpPtr++; result *= EvalsubTop( expbuf );
				if ( expbuf[ExpPtr] == ')' ) ExpPtr++;
				break;
			case 0x97 :	// abs
			case 0xA6 :	// int
			case 0xB6 :	// frac
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
			case 0x85 :	// ln
			case 0x86 :	// sqr
			case 0x95 :	// log10
			case 0x96 :	// cuberoot
			case 0xA5 :	// expn
			case 0xA7 :	// Not
			case 0xB5 :	// 10^
			case 0xB7 :	// Neg
				result *= Evalsub6( expbuf );
				break;
			default:
				cont=0;
				break;
		}
	 }
	return result;
}
double Evalsub8(char *expbuf) {	//  8th Priority
	double result,tmp;
	unsigned char c;
	result = Evalsub7( expbuf );
	return result;
}
double Evalsub9(char *expbuf) {	//  9th Priority
	double result,tmp;
	unsigned char c;
	result = Evalsub8( expbuf );
	return result;
}
double Evalsub10(char *expbuf) {	//  10th Priority  ( *,/, int.,Rmdr )
	int cont=1;
	double result,tmp;
	unsigned char c;
	result = Evalsub9( expbuf );
	while ( cont ) {
		c = expbuf[ExpPtr];
		switch ( c ) {
			case  0xA9 :	// ~
				ExpPtr++;
				result *= Evalsub9( expbuf );
				break;
			case 0xB9 :		// €
				ExpPtr++;
				tmp = Evalsub9( expbuf );
				if ( tmp == 0 ) { ErrorNo=MaERR; ErrorPtr=ExpPtr; } // Ma error 
				result /= tmp ;
				break;
			default:
				cont=0;
				break;
		}
	 }
	return result;
}
 
double Evalsub11(char *expbuf) {	//  11th Priority  ( +,- )
	int cont=1;
	double result;
	unsigned char c;
	result = Evalsub10( expbuf );
	while ( cont ) {
		c = expbuf[ExpPtr];
		switch ( c ) {
			case 0x89:
			case '+' :
				ExpPtr++;
				result += Evalsub10( expbuf );
				break;
			case 0x99 :
			case '-' :
				ExpPtr++;
				result -= Evalsub10( expbuf );
				break;
			default:
				cont=0;
				break;
		}
	 }
	return result;
}

double Evalsub12(char *expbuf) {	//  12th Priority ( =,!=,><,>=,<= )
	int cont=1;
	double result;
	unsigned char c;
	result = Evalsub11( expbuf );
	while ( cont ) {
		c = expbuf[ExpPtr];
		switch ( c ) {
			case '=' :	// =
				ExpPtr++;
				result = ( result == Evalsub11( expbuf ) );
				break;
			case '>' :	// >
				ExpPtr++;
				result = ( result >  Evalsub11( expbuf ) );
				break;
			case '<' :	// <
				ExpPtr++;
				result = ( result <  Evalsub11( expbuf ) );
				break;
			case 0x11 :	// !=
				ExpPtr++;
				result = ( result != Evalsub11( expbuf ) );
				break;
			case 0x12 :	// >=
				ExpPtr++;
				result = ( result >= Evalsub11( expbuf ) );
				break;
			case 0x10 :	// <=
				ExpPtr++;
				result = ( result <= Evalsub11( expbuf ) );
				break;
			default:
				cont=0;
				break;
		}
	 }
	return result;
}

double Evalsub13(char *expbuf) {	//  13th Priority  ( And,and)
	int cont=1;
	double result;
	unsigned char c;
	result = Evalsub12( expbuf );
	while ( cont ) {
		c = expbuf[ExpPtr];
		if ( c == 0x7F ) {
			c = expbuf[ExpPtr+1];
			switch ( c ) {
				case 0xB0 :	// And
					ExpPtr+=2;
					result = ( (int)result & (int)Evalsub13( expbuf ) );
					break;
				default:
					cont=0;
					break;
			}
		} else break;
	}
	return result;
}

double EvalsubTop(char *expbuf) {	//  14th Priority  ( Or,Xor,or,xor,xnor )
	int cont=1;
	double result;
	unsigned char c;
	result = Evalsub13( expbuf );
	while ( cont ) {
		c = expbuf[ExpPtr];
		if ( c == 0x7F ) {
			c = expbuf[ExpPtr+1];
			switch ( c ) {
				case 0xB1 :	// Or
					ExpPtr+=2;
					result = ( (int)result | (int)Evalsub13( expbuf ) );
					break;
				case 0xB4 :	// Xor
					ExpPtr+=2;
					result = ( (int)result ^ (int)Evalsub13( expbuf ) );
					break;
				default:
					cont=0;
					break;
			}
		} else break;
	}
	return result;
}

double Eval(char *expbuf) {		// Eval
	unsigned int key;
	double result;
    ExpPtr= 0;
    ErrorPtr= 0;
	ErrorNo = 0;
	if ( strlen(expbuf) == 0 ) return 0;
    result = EvalsubTop( expbuf );
    if ( ExpPtr < strlen(expbuf) ) { ErrorNo=SyntaxERR; ErrorPtr=ExpPtr; } // error 
    if ( ErrorNo ) { CB_ErrNo( ErrorNo ); }
	return result;
}


