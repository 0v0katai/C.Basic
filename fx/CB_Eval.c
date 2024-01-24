/*
===============================================================================

 Casio Basic RUNTIME library for fx-9860G series  v0.80

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
#include "CBI_interpreter.h"
#include "CB_error.h"

//------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------
//		Expression evaluation    string -> double
//----------------------------------------------------------------------------------------------
char ExpBuffer[ExpMax+1];
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
double Eval_atof(char *SRC) {
	double mantissa=0,exponent=0;
	double sign=1;
	int c = SRC[ExecPtr];
		if ( c == '.'  )   {	// .123456
				c = SRC[++ExecPtr];
				c=Eval_atofNumDiv(SRC, c, &mantissa);
		} else if ( c == 0x0F  ) {  // exp
				mantissa = 1.0;
				goto lblexp;
		} else { 	// 123456
				c=Eval_atofNumMult(SRC, c, &mantissa);	// 123456
				if ( c == '.'  ) {
				c = SRC[++ExecPtr];
					c=Eval_atofNumDiv(SRC, c, &mantissa);	// 123456.789
				}
		}
		lblexp:
		if ( c == 0x0F  ) { // exp
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
	double result=0;
	int tmp,tmp2,resultint;
	char c,d;
	char *pt;
	int dimA,dimB,reg,x,y;
	int i,ptr,mptr;
	char*	MatAryC;
	short*	MatAryW;
	int*	MatAryI;
	double*	MatAryF;
	double sign=1;

	c = SRC[ExecPtr];
	if ( c == '(') {
		ExecPtr++;
		result = EvalsubTop( SRC );
		if ( SRC[ExecPtr] == ')' ) ExecPtr++;
		return result;
	}
	if ( c == 0xFFFFFF89 ) while ( c == 0xFFFFFF89 ) c=SRC[++ExecPtr];	// +
	if ( ( c == 0xFFFFFF87 ) || ( c == 0xFFFFFF99 ) ) {	//  -
		ExecPtr++;
		result = - Evalsub1( SRC );
		return result;
	}
	if ( ( 'A' <= c )&&( c <= 'Z' ) || ( 'a' <= c )&&( c <= 'z' ) )  {
			reg=c-'A';
			c=SRC[++ExecPtr];
			if ( c=='%' ) { ExecPtr++; return REGINT[reg] ; }
			else
			if ( c=='#' ) ExecPtr++;
			return REG[reg] ;
	}
	if ( ( c=='.' ) ||( c==0x0F ) || ( ( '0'<=c )&&( c<='9' ) ) ) {
		return Eval_atof( SRC );
	}
	switch ( c ) { 			// ( type C function )  sin cos tan... 
		case 0x7F:	// 7F..
			c = SRC[ExecPtr+1];
			if ( c == 0x40 ) {	// Mat A[a,b]
				ExecPtr+=2;
				mptr=MatOprand( SRC, &reg);
				if ( ErrorNo ) return 1 ; // error
				if ( mptr==-2 ) { dspflag=3; return 1; }	// Mat A
				switch ( MatAryElementSize[reg] ) {
					case 8:
						return MatAry[reg][mptr] ;			// Matrix array doubl
					case 1:
						MatAryC=(char*)MatAry[reg];
						return MatAryC[mptr] ;				// Matrix array char
					case 2:
						MatAryW=(short*)MatAry[reg];
						return MatAryW[mptr] ;				// Matrix array word
					case 4:
						MatAryI=(int*)MatAry[reg];
						return MatAryI[mptr] ;				// Matrix array int
				}
			} else if ( c == 0x3A ) {	// MOD(a,b)
					ExecPtr+=2;
					tmp = EvalsubTop( SRC ) +.5;
					if ( SRC[ExecPtr] != ',' ) CB_Error(SyntaxERR) ; // Syntax error 
					ExecPtr++;
					tmp2 = EvalsubTop( SRC ) +.5;
					if ( tmp2 == 0 )  CB_Error(DivisionByZeroERR); // Division by zero error 
					resultint= tmp-tmp/tmp2*tmp2;
					if ( result == tmp2  ) resultint--;
					if ( tmp < 0 ) {
						resultint=abs(tmp2)-resultint;
						if ( resultint == tmp2  ) resultint=0;
					}
					if ( SRC[ExecPtr] == ')' ) ExecPtr++;
					return resultint ;
					
			} else if ( c == 0xFFFFFF8F ) {	// Getkey
					ExecPtr+=2;
					c = SRC[ExecPtr];
					if ( c=='0' ) {	ExecPtr++ ; result = CB_Getkey0() ; if ( result==34 ) if (BreakCheck) { BreakPtr=ExecPtr; KeyRecover(); } }
					if ( c=='1' ) {	ExecPtr++ ; result = CB_Getkey1() ; if ( result==34 ) if (BreakCheck) { BreakPtr=ExecPtr; KeyRecover(); } }
					if ( c=='2' ) {	ExecPtr++ ; result = CB_Getkey2() ; if ( result==34 ) if (BreakCheck) { BreakPtr=ExecPtr; KeyRecover(); } }
					else		  	result = CB_Getkey() ;
					return result ;
			} else if ( c == 0xFFFFFF86 ) {	// RndFix(n,digit)
					ExecPtr+=2;
					tmp=(EvalsubTop( SRC ));
					if ( SRC[ExecPtr] != ',' ) CB_Error(SyntaxERR) ; // Syntax error 
					ExecPtr++ ;	// ',' skip
					i = EvalsubTop( SRC ) +.5;
					if ( SRC[ExecPtr] == ')' ) ExecPtr++;
					result=Round( tmp, Fix, i) ;
					return result ;
			} else if ( c == 0xFFFFFF87 ) {	// RanInt#(st,en)
					ExecPtr+=2;
					x=EvalsubTop( SRC );
					if ( SRC[ExecPtr] != ',' ) CB_Error(SyntaxERR) ; // Syntax error 
					ExecPtr++ ;	// ',' skip
					y=EvalsubTop( SRC );
					if ( SRC[ExecPtr] == ')' ) ExecPtr++;
					if ( x>y ) { i=x; x=y; y=i; }
					return rand()*(y-x+1)/(RAND_MAX+1) +x ;
			} else if ( c == 0xFFFFFFE9 ) {	// CellSum(Mat A[x,y])
					ExecPtr+=2;
					c = SRC[ExecPtr];
					if ( ( c!=0x7F ) || ( SRC[ExecPtr+1]!=0x40 ) ) CB_Error(SyntaxERR) ; // Syntax error 
					ExecPtr+=2;
					mptr=MatOprand( SRC, &reg);
					if ( ErrorNo ) return 1 ; // error
					if ( mptr==-2 ) CB_Error(SyntaxERR) ; // Syntax error 
					if ( SRC[ExecPtr] == ')' ) ExecPtr++;
					i = MatArySizeB[reg] * MatAryElementSize[reg];
					switch ( MatAryElementSize[reg] ) {
						case 1:														// Matrix array char
							MatAryC=(char*)MatAry[reg];
							return  MatAryC[mptr-i-1] + MatAryC[mptr-i] + MatAryC[mptr-i+1] + MatAryC[mptr+i-1] + MatAryC[mptr+i] + MatAryC[mptr+i+1] + MatAryC[mptr-1] + MatAryC[mptr+1] ;
						case 2:														// Matrix array word
							MatAryW=(short*)MatAry[reg];
							return  MatAryW[mptr-i-1] + MatAryW[mptr-i] + MatAryW[mptr-i+1] + MatAryW[mptr+i-1] + MatAryW[mptr+i] + MatAryW[mptr+i+1] + MatAryW[mptr-1] + MatAryW[mptr+1] ;
						case 4:														// Matrix array int
							MatAryI=(int*)MatAry[reg];
							return  MatAryI[mptr-i-1] + MatAryI[mptr-i] + MatAryI[mptr-i+1] + MatAryI[mptr+i-1] + MatAryI[mptr+i] + MatAryI[mptr+i+1] + MatAryI[mptr-1] + MatAryI[mptr+1] ;
						case 8:														// Matrix array doubl
							MatAryF=(double*)MatAry[reg];
							return  MatAryF[mptr-i-1] + MatAryF[mptr-i] + MatAryF[mptr-i+1] + MatAryF[mptr+i-1] + MatAryF[mptr+i] + MatAryF[mptr+i+1] + MatAryF[mptr-1] + MatAryF[mptr+1] ;
					}
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
			
		case 0xFFFFFFC0 :	// Ans
			ExecPtr++;
			return CB_CurrentValue ;
		case 0xFFFFFFD0 :	// PI
			ExecPtr++;
			return PI ;
		case 0xFFFFFFC1 :	// Ran#
			c = SRC[++ExecPtr];
			if ( ( '1' <= c ) && ( c <= '9' ) ) {
					ExecPtr++;
					srand( c-'0' );
			}
			result=(double)rand()/(double)(RAND_MAX+1.0);
			return result ;
		case 0xFFFFFF97 :	// abs
			ExecPtr++; result = fabs( Evalsub5( SRC ) );
			return result ;
		case 0xFFFFFFA6 :	// int
			ExecPtr++; result = floor( Evalsub5( SRC ) );
			return result ;
		case 0xFFFFFFB6 :	// frac
			ExecPtr++; result = frac( Evalsub5( SRC ) );
			return result ;
		case 0xFFFFFF86 :	// sqr
			ExecPtr++; result = sqrt( Evalsub5( SRC ) );
			pt=(char *)(&result); if (pt[1]==0xFFFFFFF0) if ( (pt[0]==0x7F)||(pt[0]==0xFFFFFFFF) ) CB_Error(MathERR) ; // Math error
			return result ;
		case 0xFFFFFF95 :	// log10
			ExecPtr++; result = log10( Evalsub5( SRC ) );
			pt=(char *)(&result); if (pt[1]==0xFFFFFFF0) if ( (pt[0]==0x7F)||(pt[0]==0xFFFFFFFF) ) CB_Error(MathERR) ; // Math error
			return result ;
		case 0xFFFFFFB5 :	// 10^
			ExecPtr++; result = pow(10, Evalsub5( SRC ) );
			pt=(char *)(&result); if (pt[1]==0xFFFFFFF0) if ( (pt[0]==0x7F)||(pt[0]==0xFFFFFFFF) ) CB_Error(MathERR) ; // Math error
			return result ;
		case 0xFFFFFF85 :	// ln
			ExecPtr++; result = log( Evalsub5( SRC ) );
			pt=(char *)(&result); if (pt[1]==0xFFFFFFF0) if ( (pt[0]==0x7F)||(pt[0]==0xFFFFFFFF) ) CB_Error(MathERR) ; // Math error
			return result ;
		case 0xFFFFFFA5 :	// expn
			ExecPtr++; result = exp( Evalsub5( SRC ) );
			pt=(char *)(&result); if (pt[1]==0xFFFFFFF0) if ( (pt[0]==0x7F)||(pt[0]==0xFFFFFFFF) ) CB_Error(MathERR) ; // Math error
			return result ;
		case 0xFFFFFFA7 :	// Not
			ExecPtr++; result = ! (int) ( Evalsub5( SRC ) );
			return result ;
		case 0xFFFFFF96 :	// cuberoot
			ExecPtr++; result = pow( Evalsub5( SRC ), 1.0/3.0 );
			pt=(char *)(&result); if (pt[1]==0xFFFFFFF0) if ( (pt[0]==0x7F)||(pt[0]==0xFFFFFFFF) ) CB_Error(MathERR) ; // Math error
			return result ;
		case 0xFFFFFF81 :	// sin
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
			pt=(char *)(&result); if (pt[1]==0xFFFFFFF0) if ( (pt[0]==0x7F)||(pt[0]==0xFFFFFFFF) ) CB_Error(MathERR) ; // Math error
			return result ;
		case 0xFFFFFF82 :	// cos
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
			pt=(char *)(&result); if (pt[1]==0xFFFFFFF0) if ( (pt[0]==0x7F)||(pt[0]==0xFFFFFFFF) ) CB_Error(MathERR) ; // Math error
			return result ;
		case 0xFFFFFF83 :	// tan
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
			pt=(char *)(&result); if (pt[1]==0xFFFFFFF0) if ( (pt[0]==0x7F)||(pt[0]==0xFFFFFFFF) ) CB_Error(MathERR) ; // Math error
			return result ;
		case 0xFFFFFF91 :	// asin
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
			pt=(char *)(&result); if (pt[1]==0xFFFFFFF0) if ( (pt[0]==0x7F)||(pt[0]==0xFFFFFFFF) ) CB_Error(MathERR) ; // Math error
			return result ;
		case 0xFFFFFF92 :	// acos
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
			pt=(char *)(&result); if (pt[1]==0xFFFFFFF0) if ( (pt[0]==0x7F)||(pt[0]==0xFFFFFFFF) ) CB_Error(MathERR) ; // Math error
			return result ;
		case 0xFFFFFF93 :	// atan
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
			pt=(char *)(&result); if (pt[1]==0xFFFFFFF0) if ( (pt[0]==0x7F)||(pt[0]==0xFFFFFFFF) ) CB_Error(MathERR) ; // Math error
			return result ;
		case 0xFFFFFFA1 :	// sinh
			ExecPtr++; result = sinh( Evalsub5( SRC ) );
			pt=(char *)(&result); if (pt[1]==0xFFFFFFF0) if ( (pt[0]==0x7F)||(pt[0]==0xFFFFFFFF) ) CB_Error(MathERR) ; // Math error
			return result ;
		case 0xFFFFFFA2 :	// cosh
			ExecPtr++; result = cosh( Evalsub5( SRC ) );
			pt=(char *)(&result); if (pt[1]==0xFFFFFFF0) if ( (pt[0]==0x7F)||(pt[0]==0xFFFFFFFF) ) CB_Error(MathERR) ; // Math error
			return result ;
		case 0xFFFFFFA3 :	// tanh
			ExecPtr++; result = tanh( Evalsub5( SRC ) );
			pt=(char *)(&result); if (pt[1]==0xFFFFFFF0) if ( (pt[0]==0x7F)||(pt[0]==0xFFFFFFFF) ) CB_Error(MathERR) ; // Math error
			return result ;
		case 0xFFFFFFB1 :	// asinh
			ExecPtr++; result = asinh( Evalsub5( SRC ) );
			pt=(char *)(&result); if (pt[1]==0xFFFFFFF0) if ( (pt[0]==0x7F)||(pt[0]==0xFFFFFFFF) ) CB_Error(MathERR) ; // Math error
			return result ;
		case 0xFFFFFFB2 :	// acosh
			ExecPtr++; result = acosh( Evalsub5( SRC ) );
			pt=(char *)(&result); if (pt[1]==0xFFFFFFF0) if ( (pt[0]==0x7F)||(pt[0]==0xFFFFFFFF) ) CB_Error(MathERR) ; // Math error
			return result ;
		case 0xFFFFFFB3 :	// atanh
			ExecPtr++; result = atanh( Evalsub5( SRC ) );
			pt=(char *)(&result); if (pt[1]==0xFFFFFFF0) if ( (pt[0]==0x7F)||(pt[0]==0xFFFFFFFF) ) CB_Error(MathERR) ; // Math error
			return result ;
		case 0xFFFFFFF7:	// F7..
			c = SRC[ExecPtr+1];
			if ( c == 0xFFFFFFAF ) {	// PxlTest(y,x)
					ExecPtr+=2;
					y=(EvalsubTop( SRC ));
					if ( SRC[ExecPtr] != ',' ) CB_Error(SyntaxERR) ; // Syntax error 
					ExecPtr++ ;	// ',' skip
					x=(EvalsubTop( SRC ));
					if ( SRC[ExecPtr] != ')' ) CB_Error(SyntaxERR) ; // Syntax error 
					ExecPtr++ ;
					result = PxlTest(y, x) ;			// 
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
		case '%' :	// 1/128 Ticks
			ExecPtr++;
			return RTC_GetTicks() ;
		case 0xFFFFFFDD :	// Eng
			ExecPtr++;
			return ENG ;
		default:
			break;
	}
	CB_Error(SyntaxERR) ; // Syntax error 
	return 0 ;
}

double Evaldummy(char *SRC) {	// dummy
	double result,tmp;
	int c;
	result *= Evalsub1( SRC )*Evalsub1( SRC );
}


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
				pt=(char *)(&result); if (pt[1]==0xFFFFFFF0) if ( (pt[0]==0x7F)||(pt[0]==0xFFFFFFFF) ) CB_Error(MathERR) ; // Math error
				break;
			case  0xFFFFFFB8  :	// powroot
				result = pow( Evalsub2( SRC ), 1/result );
				pt=(char *)(&result); if (pt[1]==0xFFFFFFF0) if ( (pt[0]==0x7F)||(pt[0]==0xFFFFFFFF) ) CB_Error(MathERR) ; // Math error
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
				ExecPtr++; result *= EvalsubTop( SRC );
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

double EvalsubTop( char *SRC ) {	// eval 1
	int c;
	int excptr=ExecPtr;
	double  result,dst;

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


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
double Eval(char *SRC) {		// Eval temp
	double result;
	int execptr=ExecPtr;
    ExecPtr= 0;
    ErrorPtr= 0;
	ErrorNo = 0;
	if ( strlen((char*)SRC) == 0 ) { ExecPtr=execptr; return 0; }
    result = EvalsubTop( SRC );
    if ( ExecPtr < strlen((char*)SRC) ) CB_Error(SyntaxERR) ; // Syntax error 
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

