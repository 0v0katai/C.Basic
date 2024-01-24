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
#include "CB_MonochromeLib.h"
//-----------------------------------------------------------------------------

#define CB_MatListAnsregMax 4+28-1
char   CB_MatListAnsreg=27;

//-----------------------------------------------------------------------------

void NewMatListAns( int dimA, int dimB, int base, int element ){
	if ( CB_MatListAnsreg > CB_MatListAnsregMax-1 ) { CB_Error(MemoryERR); return ; }	// Memory error
	CB_MatListAnsreg++;
	DimMatrixSub( CB_MatListAnsreg, element, dimA, dimB, base);	//
//	if ( ErrorNo ) return ; // error
}
void DeleteMatListAns() {
	if ( CB_MatListAnsreg >= 28 ) {
		DeleteMatrix( CB_MatListAnsreg );
		CB_MatListAnsreg--;
	}
}
void DeleteMatListAnsAll() {
	CB_MatListAnsreg=CB_MatListAnsregMax;	// Ans init	
	DeleteMatListAns();
	DeleteMatListAns();
	DeleteMatListAns();
	DeleteMatListAns();
}
void CopyMatList2Ans( int reg ) {	// List 1 -> ListAns
	int sizeA,sizeB;
	int ElementSize;
	int base;
	sizeA        = MatAry[reg].SizeA;
	sizeB        = MatAry[reg].SizeB;
	base         = MatAry[reg].Base;
	ElementSize  = MatAry[reg].ElementSize;
	
	NewMatListAns( sizeA, sizeB, base, ElementSize );
	CopyMatrix( CB_MatListAnsreg, reg );
}
void CopyMatList2AnsTop( int reg ) {	// List 1 -> ListAns top
	if ( reg == 28 ) return ;
	CB_MatListAnsreg=27;
	CopyMatList2Ans( reg );
}
void CopyAns2MatList( char* SRC, int reg ) {	// ListAns -> List 1
	int sizeA,sizeB;
	int ElementSize;
	int base;
	
	sizeA        = MatAry[CB_MatListAnsreg].SizeA;
	sizeB        = MatAry[CB_MatListAnsreg].SizeB;
	base         = MatAry[CB_MatListAnsreg].Base;
	if ( sizeA == 0 ) { CB_Error(ArgumentERR); return ; } // Argument error
	
	ElementSize=ElementSizeSelect( SRC, &base) & 0xFF;
	DimMatrixSub( reg, ElementSize, sizeA, sizeB, base);	//
	if ( ErrorNo ) return ; // error
	CopyMatrix( reg, CB_MatListAnsreg );
	if ( CB_MatListAnsreg >=28 ) CB_MatListAnsreg=28;
}
int CheckAnsMatList( int reg ) {	// ListAns <-> List 1
	
	if ( MatAry[reg].SizeA != MatAry[CB_MatListAnsreg].SizeA ) { CB_Error(DimensionERR); return 1 ; }	// Dimension error
	else
		return 0;	// ok	
}

//-----------------------------------------------------------------------------

double ListEvalsub1(char *SRC) {	// 1st Priority
	double result=0,tmp,tmp2;
	double sign=1;
	int c,d;
	char *pt;
	int dimA,dimB,reg,x,y,base;
	int i,ptr,mptr;
	char*	MatAryC;
	short*	MatAryW;
	int*	MatAryI;
	double*	MatAryF;

	dspflag=2;		// 2:value		3:list    4:mat

	c = SRC[ExecPtr++];
	if ( c == '(') {
		result = ListEvalsubTop( SRC );
		if ( SRC[ExecPtr] == ')' ) ExecPtr++;
		return result;
	}
	while ( c == 0xFFFFFF89 ) c=SRC[ExecPtr++];	// +
	if ( ( c == 0xFFFFFF87 ) || ( c == 0xFFFFFF99 ) ) {	//  -
		result = - ListEvalsub1( SRC );
		if ( dspflag == 4 ) {	// Listresult
			reg=CB_MatListAnsreg;
			base=MatAry[reg].Base;
			for (i=base; i<MatAry[reg].SizeA+base; i++ ) {
				result= ReadMatrix( reg, i, base );
				WriteMatrix( reg, i, base, -result ) ;	// -Listresult -> Listresult
			}
		} else {	// result
			return result ;
		}
	}
	if ( ( ( 'A'<=c )&&( c<='z' ) ) && ( c != '[' ) )  {
		reg=c-'A';
	  regj:
		c=SRC[ExecPtr];
		if ( c=='%' ) { ExecPtr++; return LocalInt[reg][0] ; }
		else
		if ( c=='[' ) { goto Matrix; }
		else
		if ( ( '0'<=c )&&( c<='9' ) ) {
				ExecPtr++;
				dimA=c-'0';
				MatOprand1num( SRC, reg, &dimA, &dimB );
				goto Matrix2;
		} else
		if ( c=='#' ) { ExecPtr++; return LocalDbl[reg][0] ; }
		if ( CB_INT) return LocalInt[reg][0] ; else return LocalDbl[reg][0] ;
	}
	if ( ( c=='.' ) ||( c==0x0F ) || ( ( '0'<=c )&&( c<='9' ) ) ) {
		ExecPtr--; return Eval_atof( SRC , c );
	}
	
	switch ( c ) { 			// ( type C function )  sin cos tan... 
		case 0x7F:	// 7F..
			c = SRC[ExecPtr++];
			switch ( c ) {
				case 0x40 :		// Mat A[a,b]
					if ( ( 'A'<=c )&&( c<='z' ) ) { reg=c-'A'; ExecPtr++; } 
					else { reg=RegVarAliasEx(SRC); if ( reg<0 ) CB_Error(SyntaxERR) ; } // Syntax error 
					Matrix1:	
					if ( SRC[ExecPtr] == '[' ) {
					Matrix:	
						ExecPtr++;
						MatOprand2( SRC, reg, &dimA, &dimB );
					Matrix2:
						if ( ErrorNo ) return 1 ; // error
					} else { dspflag=3;	// Mat A
							dimA=MatAry[reg].Base; dimB=dimA;
							CopyMatList2Ans( reg );
					}
					return ReadMatrix( reg, dimA, dimB);
						
				case 0x51 :		// List 1~26
					reg=ListRegVar( SRC );
					if ( SRC[ExecPtr] == '[' ) {
						ExecPtr++;
						MatOprand1( SRC, reg, &dimA, &dimB );	// List 1[a]
						if ( ErrorNo ) return 1 ; // error
					} else { dspflag=4;	// List 1
							dimA=MatAry[reg].Base; dimB=dimA;
							CopyMatList2Ans( reg );
					}
					return ReadMatrix( reg, dimA, dimB);
						
				case 0x3A :		// MOD(a,b)
						tmp = floor(ListEvalsubTop( SRC ) +.5);
						if ( SRC[ExecPtr] != ',' ) CB_Error(SyntaxERR) ; // Syntax error 
						ExecPtr++;
						tmp2 = floor( NoListEvalsubTop( SRC ) +.5);
						if ( tmp2 == 0 )  CB_Error(DivisionByZeroERR); // Division by zero error 
						result= floor(fabs(fmod( tmp, tmp2 ))+.5);
						if ( result == tmp2  ) result--;
						if ( tmp < 0 ) {
							result=fabs(tmp2)-result;
							if ( result == tmp2  ) result=0;
						}
						if ( SRC[ExecPtr] == ')' ) ExecPtr++;
						return result ;
						
				case 0xFFFFFFB3 :		// Not
						return ( ListEvalsubTop(SRC) == 0 ) ;
						
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
						} else	result = CB_Getkey() ;
						return 	result ;
						
				case 0xFFFFFF87 :		// RanInt#(st,en)
						x=NoListEvalsubTop( SRC );
						if ( SRC[ExecPtr] != ',' ) CB_Error(SyntaxERR) ; // Syntax error 
						ExecPtr++ ;	// ',' skip
						y=NoListEvalsubTop( SRC );
						if ( SRC[ExecPtr] == ')' ) ExecPtr++;
	//					if ( x>=y ) CB_Error(ArgumentERR);  // Argument error
						if ( x>y ) { i=x; x=y; y=i; }
						return rand()*(y-x+1)/(RAND_MAX+1) +x ;
						
				case 0xFFFFFFE9 :		// CellSum(Mat A[x,y])
						MatrixOprand( SRC, &reg, &x, &y );
						if ( ErrorNo ) return ; // error
					if ( SRC[ExecPtr] == ')' ) ExecPtr++;
						return Cellsum( reg, x, y );
	
				case 0x5F :				// 1/128 Ticks
						return RTC_GetTicks()-CB_TicksAdjust;	// 
						
				case 0xFFFFFF86 :		// RndFix(n,digit)
						tmp=(NoListEvalsubTop( SRC ));
						if ( SRC[ExecPtr] != ',' ) CB_Error(SyntaxERR) ; // Syntax error 
						ExecPtr++ ;	// ',' skip
						i = NoListEvalsubTop( SRC ) +.5;
						if ( SRC[ExecPtr] == ')' ) ExecPtr++;
						result=Round( tmp, Fix, i) ;
						return result ;
						
				case 0xFFFFFFF0 :		// GraphY
						reg=defaultGraphAry;
						dimA=CB_EvalInt( SRC );
						if ( ( dimA < MatAry[reg].Base ) || ( dimA > MatAry[reg].SizeA ) ) { CB_Error(ArgumentERR); return 0; }  // Argument error
						result=CB_EvalStrDBL( MatrixPtr( reg, dimA, 1 ) );
						return result ;
						
				case 0xFFFFFFF5 :		// IsExist(
						return  CB_IsExist( SRC );
				case 0xFFFFFFF6 :		// Peek(
						return  CB_Peek( SRC, NoListEvalsubTop( SRC ) );
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
					return CB_Sigma( SRC );
				case 0x20 :				// Max( List 1 )	Max( { 1,2,3,4,5 } )
					return CB_Max( SRC );
				case 0x2D :				// Min( List 1 )	Min( { 1,2,3,4,5 } )
					return CB_Min( SRC );
				case 0x4C :				// Sum( List 1)
					return CB_Sum( SRC );
				case 0x4D :				// Prod( List 1)
					return CB_Prod( SRC );
				
				case 0x46 :				// Dim
						if ( ( SRC[ExecPtr]==0x7F ) && ( SRC[ExecPtr+1]==0x51 ) ) {	// Dim List
							ExecPtr+=2;
							goto ColSizej;
						}
				case 0x58 :				// ElemSize( Mat A )
						MatrixOprandreg( SRC, &reg );
						if ( SRC[ExecPtr] == ')' ) ExecPtr++;
						i=MatAry[reg].ElementSize;
						if (i <= 4 ) i=1;
						return i;
				case 0x59 :				// ColSize( Mat A )
					ColSizej:
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
				default:
						ExecPtr--;	// error
						break;
			}
			break;
		case 0xFFFFFFD0 :	// PI
			return PI ;
		case 0xFFFFFFC1 :	// Ran#
			c = SRC[ExecPtr];
			if ( ( '0'<=c )&&( c<='9' ) ) srand( Eval_atod( SRC, c ) );
			result=(double)rand()/(double)(RAND_MAX+1.0);
			return result ;
		case 0xFFFFFF97 :	// abs
			result = fabs( ListEvalsub5( SRC ) );
			return result ;
		case 0xFFFFFFA6 :	// int
			result = ListEvalsub5( SRC );
			if ( result >= 0 ) goto intg;
			return -floor(-result);
		case 0xFFFFFFDE :	// intg
			result = ListEvalsub5( SRC );
			intg:
			return floor(result) ;
		case 0xFFFFFFB6 :	// frac
			result = frac( ListEvalsub5( SRC ) );
			return result ;
		case 0xFFFFFFA7 :	// Not
			result = ! (int) ( ListEvalsub5( SRC ) );
			return result ;

		case '%' :	// 1/128 Ticks
			return RTC_GetTicks()-CB_TicksAdjust;	// 
		case '*' :	// peek
			return CB_Peek( SRC, ListEvalsub1( SRC ) );	// 

		case '{':	// { 1,2,3,4,5... }->List Ans
			CB_List(SRC);
			return 0;
		case '[':	// [ [0,1,2][2,3,4] ]->Mat Ans
			CB_Matrix(SRC);
			return 0;

		case 0xFFFFFFF7:	// F7..
			c = SRC[ExecPtr++];
			if ( c == 0xFFFFFFAF ) {	// PxlTest(y,x)
					y=(NoListEvalsubTop( SRC ));
					if ( SRC[ExecPtr] != ',' ) CB_Error(SyntaxERR) ; // Syntax error 
					ExecPtr++ ;	// ',' skip
					x=(NoListEvalsubTop( SRC ));
					if ( SRC[ExecPtr] == ')' ) ExecPtr++;
					result = PxlTest(y, x) ;			// 
					return result ;
			} else
			if ( c == 0xFFFFFFF4 ) {	// SysCall(
					return  CB_SysCall( SRC );
			} else
			if ( c == 0xFFFFFFF5 ) {	// Call(
					return  CB_Call( SRC );
			} else
			if ( c == 0xFFFFFFF8 ) {	// RefreshCtrl
					return  RefreshCtrl;
			} else
			if ( c == 0xFFFFFFFA ) {	// RefreshTime
					return  Refreshtime+1;
			} else
			if ( c == 0xFFFFFFFB ) {	// Screen
					return  ScreenMode;
			} else
			if ( c == 0xFFFFFFFE ) {	// BackLight
				return	BackLight(-1);
			} else ExecPtr--;	// error
			break;

		case 0xFFFFFF86 :	// sqr
			result = sqrt( ListEvalsub5( SRC ) );
			CheckMathERR(&result); // Math error ?
			return result ;
		case 0xFFFFFF95 :	// log10
			result = log10( ListEvalsub5( SRC ) );
			CheckMathERR(&result); // Math error ?
			return result ;
		case 0xFFFFFFB5 :	// 10^
			result = pow(10, ListEvalsub5( SRC ) );
			CheckMathERR(&result); // Math error ?
			return result ;
		case 0xFFFFFF85 :	// ln
			result = log( ListEvalsub5( SRC ) );
			CheckMathERR(&result); // Math error ?
			return result ;
		case 0xFFFFFFA5 :	// expn
			result = exp( ListEvalsub5( SRC ) );
			CheckMathERR(&result); // Math error ?
			return result ;
		case 0xFFFFFF96 :	// cuberoot
			result = pow( ListEvalsub5( SRC ), 1.0/3.0 );
			CheckMathERR(&result); // Math error ?
			return result ;

		case 0xFFFFFF81 :	// sin
			return fsin( ListEvalsub5( SRC ) );
		case 0xFFFFFF82 :	// cos
			return fcos( ListEvalsub5( SRC ) );
		case 0xFFFFFF83 :	// tan
			return ftan( ListEvalsub5( SRC ) );
		case 0xFFFFFF91 :	// asin
			return fasin( ListEvalsub5( SRC ) );
		case 0xFFFFFF92 :	// acos
			return facos( ListEvalsub5( SRC ) );
		case 0xFFFFFF93 :	// atan
			return fatan( ListEvalsub5( SRC ) );
		case 0xFFFFFFA1 :	// sinh
			return  sinh( ListEvalsub5( SRC ) );
		case 0xFFFFFFA2 :	// cosh
			return  cosh( ListEvalsub5( SRC ) );
		case 0xFFFFFFA3 :	// tanh
			return  tanh( ListEvalsub5( SRC ) );
		case 0xFFFFFFB1 :	// asinh
			return  asinh( ListEvalsub5( SRC ) );
		case 0xFFFFFFB2 :	// acosh
			return  acosh( ListEvalsub5( SRC ) );
		case 0xFFFFFFB3 :	// atanh
			return  atanh( ListEvalsub5( SRC ) );
			
		case 0xFFFFFF80 :	// Pol( x, y ) -> r=List Ans[1] , Theta=List Ans[2]
			tmp=NoListEvalsubTop( SRC );
			if ( SRC[ExecPtr] != ',' ) { CB_Error(SyntaxERR) ; return 0; }	// Syntax error
			ExecPtr++;
			tmp2=NoListEvalsubTop( SRC );
			if ( SRC[ExecPtr] == ')' ) ExecPtr++;
			dspflag=4;	// List ans
			NewMatListAns( 2, 1, 1, 64 );		// List Ans[2]
			WriteMatrix( CB_MatListAnsreg, 1,1, fpolr(tmp,tmp2) ) ;	// r
			WriteMatrix( CB_MatListAnsreg, 2,1, fpolt(tmp,tmp2) ) ;	// Theta
			return 0;
		case 0xFFFFFFA0 :	// Rec( r, Theta ) -> X,Y
			tmp=NoListEvalsubTop( SRC );
			if ( SRC[ExecPtr] != ',' ) { CB_Error(SyntaxERR) ; return 0; }	// Syntax error
			ExecPtr++;
			tmp2=NoListEvalsubTop( SRC );
			if ( SRC[ExecPtr] == ')' ) ExecPtr++;
			dspflag=4;	// List ans
			NewMatListAns( 2, 1, 1, 64 );		// List Ans[2]
			WriteMatrix( CB_MatListAnsreg, 1,1, frecx(tmp,tmp2) ) ;	// x
			WriteMatrix( CB_MatListAnsreg, 2,1, frecy(tmp,tmp2) ) ;	// y
			return 0;

		case 0xFFFFFFF9:	// F9..
			c = SRC[ExecPtr++];
			if ( c == 0x56 ) {	// M_PixelTest(
					return CB_ML_PixelTest( SRC );
			} else
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
		case '&' :	// & VarPtr
			return CB_VarPtr( SRC ) ;
		default:
			break;
	}
	if ( c == '#') {
		result = EvalsubTop( SRC );
		return result;
	}
	ExecPtr--;
	reg=RegVarAliasEx( SRC ); if ( reg>=0 ) goto regj;	// variable alias
	CB_Error(SyntaxERR) ; // Syntax error 
	return 0 ;
}

//-----------------------------------------------------------------------------
//int ListEvalObjectAlignE4g( unsigned int n ){ return n ; }	// align +4byte
//int ListEvalObjectAlignE4h( unsigned int n ){ return n+n; }	// align +6byte
//-----------------------------------------------------------------------------

double ListEvalsub2(char *SRC) {	//  2nd Priority  ( type B function ) ...
	int cont=1;
	double result,tmp;
	int c,i;
	int base;
	int resultreg,tmpreg;
	result = ListEvalsub1( SRC );
	resultreg=CB_MatListAnsreg;
	while ( 1 ) {
		c = SRC[ExecPtr++];
		switch ( c ) {
			case  0xFFFFFF8B  :	// ^2
				if ( dspflag == 4 ) {	// Listresult
					base=MatAry[resultreg].Base;
					for (i=base; i<MatAry[resultreg].SizeA+base; i++ ) {
						result= ReadMatrix( resultreg, i, base );
						WriteMatrix( resultreg, i, base, result * result ) ;	// Listresult ^2 -> Listresult
					}
				} else {	// result
					result *= result ;
				}
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
				
			case  0xFFFFFF8C  :	//  dms
				result = DmsToDec( SRC, result );
				break;
				
			case  0xFFFFFF9C  :	//  Deg
				result=fdegree( result );
				break;
			case  0xFFFFFFAC  :	//  Rad
				result=fradian( result );
				break;
			case  0xFFFFFFBC  :	//  Grad
				result=fgrad( result );
				break;
				
			default:
				ExecPtr--;
				return result;
				break;
		}
	 }
	return result;
}
double ListEvalsub3(char *SRC) {	//  3rd Priority  ( ^ ...)
	double result,tmp;
	char *pt;
	int c,i;
	int base;
	int reg;
	int flag;
	result = ListEvalsub2( SRC );
	flag=dspflag;		// 2:result	3:Listresult
	reg=CB_MatListAnsreg;
	base==MatAry[reg].Base;
	while ( 1 ) {
		c = SRC[ExecPtr++];
		switch ( c ) {
			case  0xFFFFFFA8  :	// a ^ b
				result = pow( result, ListEvalsub2( SRC ) );
				CheckMathERR(&result); // Math error ?
				break;
			case  0xFFFFFFB8  :	// powroot
				result = pow( ListEvalsub2( SRC ), 1/result );
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

double ListEvalsub4(char *SRC) {	//  4th Priority  (Fraction) a/b/c
	double result,frac1,frac2,frac3;
	unsigned int c;
	result = ListEvalsub3( SRC );
	c = SRC[ExecPtr];
	if ( c == 0xFFFFFFBB ) {
		ExecPtr++;
		frac1 = result ;
		frac2 = ListEvalsub3( SRC );
		c = SRC[ExecPtr];
		if ( c == 0xFFFFFFBB ) {
			ExecPtr++;
			frac3 = ListEvalsub3( SRC );
			if ( frac3 == 0 ) CB_Error(DivisionByZeroERR); // Division by zero error 
			result = frac1 + ( frac2 / frac3 ) ;
		} else {
			if ( frac2 == 0 ) CB_Error(DivisionByZeroERR); // Division by zero error 
			result = ( frac1 / frac2 ) ;
		}
	}
	return result;
}
double ListEvalsub5(char *SRC) {	//  5th Priority abbreviated multiplication
	double result,tmp;
	int dimA,dimB,reg,x,y;
	int c,execptr;
	int base;
	int flag;
	result = ListEvalsub4( SRC );
	flag=dspflag;		// 2:result	3:Listresult
	reg=CB_MatListAnsreg;
	base==MatAry[reg].Base;
	while ( 1 ) {
		c = SRC[ExecPtr];
		if ((( 'A'<=c )&&( c<='z' )) ||
			 ( c == 0xFFFFFFCD ) || // <r>
			 ( c == 0xFFFFFFCE ) || // Theta
			 ( c == 0xFFFFFFD0 ) || // PI
			 ( c == 0xFFFFFFC0 ) || // Ans
			 ( c == 0xFFFFFFC1 )) { // Ran#
				result *= ListEvalsub4( SRC ) ;
		} else if ( c == 0x7F ) { // 7F..
			c = SRC[ExecPtr+1];
			switch ( c ) {
				case 0x40:	// Mat A[a,b]
				case 0x51:	// List 1[a]
				case 0x3A:	// MOD(a,b)
				case 0xFFFFFF8F:	// Getkey
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
					result *= ListEvalsub4( SRC ) ;
					break;
				default:
					goto exitj;
					break;
			}
		} else if ( c == 0xFFFFFFF7 ) { // F7..
			c = SRC[ExecPtr+1];
			switch ( c ) {
				case 0xFFFFFFAF:	// PxlTest(y,x)
					result *= ListEvalsub4( SRC ) ;
					break;
				default:
					goto exitj;
					break;
			}
		} else if ( c == 0xFFFFFFF9 ) { // F9..
			c = SRC[ExecPtr+1];
			switch ( c ) {
				case 0x21:	// Xdot
					result *= ListEvalsub4( SRC ) ;
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
double ListEvalsub7(char *SRC) {	//  7th Priority abbreviated multiplication type A/C
	double result,tmp;
	int c,i;
	int base;
	int reg;
	int flag;
	result = ListEvalsub5( SRC );
	flag=dspflag;		// 2:result	3:Listresult
	reg=CB_MatListAnsreg;
	base==MatAry[reg].Base;
	while ( 1 ) {
		c = SRC[ExecPtr];
		switch ( c ) {
			case '(' :
			case 0xFFFFFF97 :	// abs
			case 0xFFFFFFA6 :	// int
			case 0xFFFFFFDE :	// intg
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
				result *= ListEvalsub5( SRC );
				break;
			default:
				return result;
				break;
		}
	 }
	return result;
}

double ListEvalsub10(char *SRC) {	//  10th Priority  ( *,/, int.,Rmdr )
	double result,tmp,tmp2;
	int c,i;
	int base;
	int resultreg,tmpreg;
	int resultflag;
	result = ListEvalsub7( SRC );
	resultflag=dspflag;		// 2:result	3:Listresult
	resultreg=CB_MatListAnsreg;
	while ( 1 ) {
		c = SRC[ExecPtr++];
		switch ( c ) {
			case 0xFFFFFFA9 :		// Å~
				tmp = ListEvalsub7( SRC );
				tmpreg=CB_MatListAnsreg;
				if ( dspflag == 4 ) {	// Listtmp
					base=MatAry[tmpreg].Base;
					if ( resultflag == 4 ) {
						if ( CheckAnsMatList(resultreg) ) return 0;	// Not same List error
						for (i=base; i<MatAry[resultreg].SizeA+base; i++ ) {
							WriteMatrix( resultreg, i, base, ReadMatrix(resultreg, i, base) * ReadMatrix(tmpreg, i, base) ) ;	// Listresult * Listtmp -> Listresult
						}
						DeleteMatListAns();
					} else {
						for (i=base; i<MatAry[tmpreg].SizeA+base; i++ ) {
							WriteMatrix( tmpreg, i, base, result * ReadMatrix(tmpreg, i, base) ) ;	// result * Listtmp -> Listresult
						}
						resultflag=dspflag;
						resultreg=tmpreg;
					}
				} else {	// tmp
					if ( resultflag == 4 ) { // 4:Listresult
						base=MatAry[resultreg].Base;
						for (i=base; i<MatAry[resultreg].SizeA+base; i++ ) WriteMatrix( resultreg, i, base, ReadMatrix(resultreg, i, base) * tmp ) ;	// Listresult * tmp -> Listresult
						dspflag = resultflag ;
					} else	{
						result = result * tmp ;
					}
				}
				break;
			case 0xFFFFFFB9 :		// ÅÄ
				tmp = ListEvalsub7( SRC );
				tmpreg=CB_MatListAnsreg;
				if ( dspflag == 4 ) {	// Listtmp
					tmpreg=CB_MatListAnsreg;
					if ( resultflag == 4 ) {
						if ( CheckAnsMatList(resultreg) ) return 0;	// Not same List error
						base=MatAry[resultreg].Base;
						for (i=base; i<MatAry[resultreg].SizeA+base; i++ ) {
							tmp2=ReadMatrix( tmpreg, i, base );
							if ( tmp2 == 0 ) CB_Error(DivisionByZeroERR); // Division by zero error 
							WriteMatrix( resultreg, i, base, ReadMatrix( resultreg, i, base ) / tmp2 ) ;	// Listresult / Listtmp -> Listresult
						}
						DeleteMatListAns();
					} else {
						tmp2=ReadMatrix( tmpreg, i, base ) ;
						if ( tmp2 == 0 ) CB_Error(DivisionByZeroERR); // Division by zero error 
						base=MatAry[tmpreg].Base;
						for (i=base; i<MatAry[tmpreg].SizeA+base; i++ ) {
							WriteMatrix( tmpreg, i, base, result / tmp2 ) ;	// result / Listtmp -> Listresult
						}
						resultflag=dspflag;
						resultreg=tmpreg;
					}
				} else {	// tmp
					if ( tmp == 0 ) CB_Error(DivisionByZeroERR); // Division by zero error 
					if ( resultflag == 4 ) { // 4:Listresult
						base=MatAry[resultreg].Base;
						for (i=base; i<MatAry[resultreg].SizeA+base; i++ ) {
							WriteMatrix( resultreg, i, base, ReadMatrix( resultreg, i, base ) / tmp ) ;	// Listresult / tmp -> Listresult
						}
						dspflag = resultflag ;
					} else	{
						result = result / tmp ;
					}
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
double ListEvalsub11(char *SRC) {	//  11th Priority  ( +,- )
	double result,tmp;
	int c,i;
	int base;
	int resultreg,tmpreg;
	int resultflag;

	result = ListEvalsub10( SRC );
	resultflag=dspflag;		// 2:result	3:Listresult
	resultreg=CB_MatListAnsreg;
	while ( 1 ) {
		c = SRC[ExecPtr++];
		switch ( c ) {
			case 0xFFFFFF89 :		// +
				tmp = ListEvalsub10( SRC );
				tmpreg=CB_MatListAnsreg;
				if ( dspflag == 4 ) {	// Listtmp
					base=MatAry[tmpreg].Base;
					if ( resultflag == 4 ) {
						if ( CheckAnsMatList(resultreg) ) return 0;	// Not same List error
						for (i=base; i<MatAry[resultreg].SizeA+base; i++ ) {
							WriteMatrix( resultreg, i, base, ReadMatrix(resultreg, i, base) + ReadMatrix(tmpreg, i, base) ) ;	// Listresult + Listtmp -> Listresult
						}
						DeleteMatListAns();
					} else {
						for (i=base; i<MatAry[tmpreg].SizeA+base; i++ ) {
							WriteMatrix( tmpreg, i, base, result + ReadMatrix(tmpreg, i, base) ) ;	// result + Listtmp -> Listresult
						}
						resultflag=dspflag;
						resultreg=tmpreg;
					}
				} else {	// tmp
					if ( resultflag == 4 ) { // 4:Listresult
						base=MatAry[resultreg].Base;
						for (i=base; i<MatAry[resultreg].SizeA+base; i++ ) WriteMatrix( resultreg, i, base, ReadMatrix(resultreg, i, base) + tmp ) ;	// Listresult + tmp -> Listresult
						dspflag = resultflag ;
					} else	{
						result = result + tmp ;
					}
				}
				break;
			case 0xFFFFFF99 :		// -
				tmp = ListEvalsub10( SRC );
				tmpreg=CB_MatListAnsreg;
				if ( dspflag == 4 ) {	// Listtmp
					tmpreg=CB_MatListAnsreg;
					base=MatAry[tmpreg].Base;
					if ( resultflag == 4 ) {
						if ( CheckAnsMatList(resultreg) ) return 0;	// Not same List error
						for (i=base; i<MatAry[resultreg].SizeA+base; i++ ) {
							WriteMatrix( resultreg, i, base, ReadMatrix(resultreg, i, base) - ReadMatrix(tmpreg, i, base) ) ;	// Listresult - Listtmp -> Listresult
						}
						DeleteMatListAns();
					} else {
						for (i=base; i<MatAry[tmpreg].SizeA+base; i++ ) {
							WriteMatrix( tmpreg, i, base, result - ReadMatrix(tmpreg, i, base) ) ;	// result - Listtmp -> Listresult
						}
						resultflag=dspflag;
						resultreg=tmpreg;
					}
				} else {	// tmp
					if ( resultflag == 4 ) { // 4:Listresult
						base=MatAry[resultreg].Base;
						for (i=base; i<MatAry[resultreg].SizeA+base; i++ ) WriteMatrix( resultreg, i, base, ReadMatrix(resultreg, i, base) - tmp ) ;	// Listresult - tmp -> Listresult
						dspflag = resultflag ;
					} else	{
						result = result - tmp ;
					}
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
double ListEvalsub12(char *SRC) {	//  12th Priority ( =,!=,><,>=,<= )
	double result;
	int c;
	result = ListEvalsub11( SRC );
	while ( 1 ) {
		c = SRC[ExecPtr++];
		switch ( c ) {
			case '=' :	// =
				result = ( result == ListEvalsub11( SRC ) );
				break;
			case '>' :	// >
				result = ( result >  ListEvalsub11( SRC ) );
				break;
			case '<' :	// <
				result = ( result <  ListEvalsub11( SRC ) );
				break;
			case 0x11 :	// !=
				result = ( result != ListEvalsub11( SRC ) );
				break;
			case 0x12 :	// >=
				result = ( result >= ListEvalsub11( SRC ) );
				break;
			case 0x10 :	// <=
				result = ( result <= ListEvalsub11( SRC ) );
				break;
			default:
				ExecPtr--;
				return result;
				break;
		}
	 }
	return result;
}
double ListEvalsub13(char *SRC) {	//  13th Priority  ( And,and)
	double result;
	int c;
	result = ListEvalsub12( SRC );
	while ( 1 ) {
		c = SRC[ExecPtr];
		if ( c == 0x7F ) {
			c = SRC[ExecPtr+1];
			switch ( c ) {
				case 0xFFFFFFB0 :	// And
					ExecPtr+=2;
					result = ( (int)result & (int)ListEvalsub12( SRC ) );
					break;
				default:
					return result;
					break;
			}
		} else return result;
	}
}
double ListEvalsub14(char *SRC) {	//  14th Priority  ( Or,Xor,or,xor,xnor )
	double result;
	int c;
	result = ListEvalsub13( SRC );
	while ( 1 ) {
		c = SRC[ExecPtr];
		if ( c == 0x7F ) {
			c = SRC[ExecPtr+1];
			switch ( c ) {
				case 0xFFFFFFB1 :	// Or
					ExecPtr+=2;
					result = ( (int)result | (int)ListEvalsub13( SRC ) );
					break;
				case 0xFFFFFFB4 :	// Xor
					ExecPtr+=2;
					result = ( (int)result ^ (int)ListEvalsub13( SRC ) );
					break;
				default:
					return result;
					break;
			}
		} else return result;
	}
}

double ListEvalsubTop(char *SRC) {	//  
	double result,tmp;
	int c,i;
	int base;

	result = ListEvalsub14( SRC );
//	if ( dspflag < 3 ) { CB_Error(ArgumentERR); return result; } // Argument error
	return result;
}
/*
double ListEvalsub1(char *SRC) {	//  
	double result,tmp;
	int c,i;
	int base;

	result = ListEvalsub1( SRC );
	if ( dspflag < 3 ) { CB_Error(ArgumentERR); return ; } // Argument error
	return result;
}
*/
double ListEvalsub1Ans(char *SRC) {	//  
	double result,tmp;
	int c,i;
	int base;
	int resultreg,tmpreg;

	result = ListEvalsub1( SRC );
	resultreg=CB_MatListAnsreg;
	if ( dspflag < 3 ) { CB_Error(ArgumentERR); return result; } // Argument error

	if ( SRC[ExecPtr] != 0x0E ) CopyMatList2AnsTop( resultreg );	// ListResult -> List Ans top
	return result;
}


double ListEvalsubTopAns(char *SRC) {	//  
	double result,tmp;
	int c,i;
	int base;
	int resultreg,tmpreg;

	result = ListEvalsub14( SRC );
	resultreg=CB_MatListAnsreg;
	if ( dspflag < 3 ) { CB_Error(ArgumentERR); return result; } // Argument error

	if ( SRC[ExecPtr] != 0x0E ) CopyMatList2AnsTop( resultreg );	// ListResult -> List Ans top
	return result;
}

