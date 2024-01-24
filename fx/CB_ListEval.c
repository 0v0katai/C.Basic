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
	CopyMatrix( CB_MatListAnsreg, reg );		// reg -> CB_MatListAnsreg
	MatdspNo=CB_MatListAnsreg;
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
	ElementSize  = MatAry[CB_MatListAnsreg].ElementSize;
	if ( sizeA == 0 ) { CB_Error(ArgumentERR); return ; } // Argument error
	
	ElementSize=ElementSizeSelect( SRC, &base, ElementSize) & 0xFF;
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

void WriteListAns2( double x, double y ) {
	dspflag=4;	// List ans
	NewMatListAns( 2, 1, 1, 64 );		// List Ans[2]
	WriteMatrix( CB_MatListAnsreg, 1,1, x ) ;	//
	WriteMatrix( CB_MatListAnsreg, 2,1, y ) ;	// 
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
typedef double (*FXPTR)( double x );
double EvalFxDbl( FXPTR fxptr, double result ) { 
	int i,j;
	int base;
	int resultreg=CB_MatListAnsreg;
	if ( dspflag >= 3 ) {	// Listresult
		base=MatAry[resultreg].Base;
		for (i=base; i<MatAry[resultreg].SizeA+base; i++ ) {
			for (j=base; j<MatAry[resultreg].SizeB+base; j++ ) {
				result = ReadMatrix( resultreg, i, j );
				result = (fxptr)( result );
				WriteMatrix( resultreg, i, j, result ) ;	// Fx(Listresult) -> Listresult
			}
		}
	} else {	// result
		result = (fxptr)( result ) ;
	}
	return result;
}

typedef double (*FXPTR2)( double x, double y );
double EvalFxDbl2( FXPTR2 fxptr2, int *resultflag, int *resultreg, double result, double tmp ) { 
	double sum;
	int i,j,k;
	int colA,rowB;
	int base;
	int tmpreg58=58;	// Mattmpreg
	int tmpreg=CB_MatListAnsreg;
	if ( dspflag >= 3 ) {	// Listtmp
		base=MatAry[tmpreg].Base;
		if ( *resultflag >= 3 ) {
			if ( ( *resultflag == 4 ) || ( fxptr2 != fMUL ) ) {
				if ( CheckAnsMatList(*resultreg) ) return 0;	// Not same List error
				for (i=base; i<MatAry[*resultreg].SizeA+base; i++ ) {
					for (j=base; j<MatAry[*resultreg].SizeB+base; j++ ) {
						result = ReadMatrix( *resultreg, i, j);
						tmp    = ReadMatrix( tmpreg, i, j);
						WriteMatrix( *resultreg, i, j, (fxptr2)(result,tmp) ) ;	// Listresult (op) Listtmp -> Listresult
					}
				}
			} else {	// Mat A * Mat B -> Mat C		Matresult * Mattmp -> Matresult
				if ( MatAry[*resultreg].SizeB != MatAry[tmpreg].SizeA ) { CB_Error(DimensionERR); return 0 ; }	// Dimension error
				colA=MatAry[*resultreg].SizeA;
				rowB=MatAry[tmpreg].SizeB;
				DimMatrixSub( tmpreg58, MatAry[*resultreg].ElementSize, colA, rowB, base);	// Mattmpreg
				if ( ErrorNo ) return 0; // error
				for ( i=base; i<colA+base; i++ ){
					for ( j=base; j<rowB+base; j++ ){
						sum=0;
						for ( k=base; k<MatAry[*resultreg].SizeB+base; k++ ){
							result = ReadMatrix( *resultreg, i, k);
							tmp    = ReadMatrix( tmpreg, k, j);
							sum+=(result*tmp);
						}
						WriteMatrix( tmpreg58, i, j, sum ) ;
					}
				}
				DimMatrixSub( *resultreg, MatAry[*resultreg].ElementSize, colA, rowB, base);	// Mattmpreg
				if ( ErrorNo ) return 0; // error
				CopyMatrix( *resultreg, tmpreg58 );		// tmpreg58 -> Matresult
			}
			DeleteMatListAns();	// delete tmp
		} else {
			for (i=base; i<MatAry[tmpreg].SizeA+base; i++ ) {
				for (j=base; j<MatAry[tmpreg].SizeB+base; j++ ) {
					tmp    = ReadMatrix( tmpreg, i, j);
					WriteMatrix( tmpreg, i, j, (fxptr2)(result,tmp) ) ;	// result * Listtmp -> Listresult
				}
			}
			*resultflag=dspflag;
			*resultreg=tmpreg;
		}
	} else {	// tmp
		if ( *resultflag >= 3 ) { // 4:Listresult
			base=MatAry[*resultreg].Base;
			for (i=base; i<MatAry[*resultreg].SizeA+base; i++ ) {
				for (j=base; j<MatAry[*resultreg].SizeB+base; j++ ) {
					result = ReadMatrix( *resultreg, i, j);
					WriteMatrix( *resultreg, i, j, (fxptr2)(result,tmp) ) ;	// Listresult * tmp -> Listresult
				}
			}
			dspflag = *resultflag ;
		} else	{
			result = (fxptr2)(result,tmp) ;
		}
	}
	return result;
}
//-----------------------------------------------------------------------------
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
	int resultreg;
	int resultflag;

	dspflag=2;		// 2:value		3:list    4:mat

	c = SRC[ExecPtr++];
	if ( c == '(') {
		result = ListEvalsubTop( SRC );
		if ( SRC[ExecPtr] == ')' ) ExecPtr++;
		return result;
	}
	while ( c == 0xFFFFFF89 ) c=SRC[ExecPtr++];	// +
	if ( ( c == 0xFFFFFF87 ) || ( c == 0xFFFFFF99 ) ) {	//  -
		return EvalFxDbl( &fsign, ListEvalsub1( SRC ) ) ; 
	}
	if ( ( ( 'A'<=c )&&( c<='Z' ) ) || ( ( 'a'<=c )&&( c<='z' ) ) )  {
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
					reg=RegVarAliasEx(SRC); if ( reg<0 ) CB_Error(SyntaxERR) ; // Syntax error 
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
				  Listj:
					if ( SRC[ExecPtr] == '[' ) {
						ExecPtr++;
						MatOprand1( SRC, reg, &dimA, &dimB );	// List 1[a]
						if ( ErrorNo ) return 1 ; // error
					} else { dspflag=4;	// List 1
							dimA=MatAry[reg].Base; dimB=dimA;
							CopyMatList2Ans( reg );
					}
					return ReadMatrix( reg, dimA, dimB);
					
				case 0x6A :		// List1
				case 0x6B :		// List2
				case 0x6C :		// List3
				case 0x6D :		// List4
				case 0x6E :		// List5
				case 0x6F :		// List6
					reg=c+(32-0x6A); goto Listj;
						
				case 0x3A :		// MOD(a,b)
					result = ListEvalsubTop( SRC );
					resultflag=dspflag;		// 2:result	3:Listresult
					resultreg=CB_MatListAnsreg;
					if ( SRC[ExecPtr] != ',' ) CB_Error(SyntaxERR) ; // Syntax error 
					ExecPtr++;
					result = EvalFxDbl2( &fMOD, &resultflag, &resultreg, result, ListEvalsubTop( SRC ) ) ;
					if ( SRC[ExecPtr] == ')' ) ExecPtr++;
					return result ;
						
				case 0x3C :		// GCD(a,b)
					result = ListEvalsubTop( SRC );
					resultflag=dspflag;		// 2:result	3:Listresult
					resultreg=CB_MatListAnsreg;
					if ( SRC[ExecPtr] != ',' ) CB_Error(SyntaxERR) ; // Syntax error 
					ExecPtr++;
					result = EvalFxDbl2( &fGCD, &resultflag, &resultreg, result, ListEvalsubTop( SRC ) ) ;
					if ( SRC[ExecPtr] == ')' ) ExecPtr++;
					return result ;
						
				case 0x3D :		// LCM(a,b)
					result = ListEvalsubTop( SRC );
					resultflag=dspflag;		// 2:result	3:Listresult
					resultreg=CB_MatListAnsreg;
					if ( SRC[ExecPtr] != ',' ) CB_Error(SyntaxERR) ; // Syntax error 
					ExecPtr++;
					result = EvalFxDbl2( &fLCM, &resultflag, &resultreg, result, ListEvalsubTop( SRC ) ) ;
					if ( SRC[ExecPtr] == ')' ) ExecPtr++;
					return result ;
						
				case 0xFFFFFF85 :		// logab(a,b)
					result = ListEvalsubTop( SRC );
					resultflag=dspflag;		// 2:result	3:Listresult
					resultreg=CB_MatListAnsreg;
					if ( SRC[ExecPtr] != ',' ) CB_Error(SyntaxERR) ; // Syntax error 
					ExecPtr++;
					result = EvalFxDbl2( &flogab, &resultflag, &resultreg, result, ListEvalsubTop( SRC ) ) ;
					if ( SRC[ExecPtr] == ')' ) ExecPtr++;
					return result ;

				case 0xFFFFFFB3 :		// Not
					return EvalFxDbl( &fNot_logic, ListEvalsub5( SRC ) ) ; 
						
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
					
				case 0xFFFFFF87 :		// RanInt#(st,en[,n])
					x=NoListEvalsubTop( SRC );
					if ( SRC[ExecPtr] != ',' ) CB_Error(SyntaxERR) ; // Syntax error 
					ExecPtr++ ;	// ',' skip
					y=NoListEvalsubTop( SRC );
					if ( SRC[ExecPtr] == ',' ) {
						ExecPtr++;
						CB_RanInt( SRC, x, y );
					}
					if ( SRC[ExecPtr] == ')' ) ExecPtr++;
					return frandIntint( x, y ) ;
					
				case 0xFFFFFF88 :		// RanList#(n) ->ListAns
					CB_RanList( SRC ) ;
					return 4 ;
				case 0xFFFFFF89 :		// RanBin#(n,p[,m]) ->ListAns
					return CB_RanBin( SRC ) ;
				case 0xFFFFFF8A :		// RanNorm#(sd,mean[,n]) ->ListAns
					return CB_RanNorm( SRC ) ;
						
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
					
				case 0xFFFFFFF0 :		// GraphY str
					return CB_GraphYStr( SRC, 1 );
					
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
					return CB_MinMax( SRC, 1 );
				case 0x2D :				// Min( List 1 )	Min( { 1,2,3,4,5 } )
					return CB_MinMax( SRC, 0 );
				case 0x2E :				// Mean( List 1 )	Mean( { 1,2,3,4,5 } )
					return CB_Mean( SRC );
				case 0x4C :				// Sum List 1
					return CB_Sum( SRC );
				case 0x4D :				// Prod List 1
					return CB_Prod( SRC );
				case 0x47:	// Fill(
					CB_MatFill(SRC);
					return 3;
				case 0x49:	// Argument(
					CB_Argument(SRC);
					return 3;
				case 0x2C:	// Seq
					CB_Seq(SRC);
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
							goto ColSizej;
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
					
				case 0x4A :				// List>Mat( List 1, List 2,..) -> List 5
					CB_List2Mat( SRC );
					return 0;
				case 0x4B :				// Mat>List( Mat A, m) -> List n
					CB_Mat2List( SRC );
					return 0;
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
			return EvalFxDbl( &fabs, ListEvalsub5( SRC ) ) ; 
		case 0xFFFFFFA6 :	// int
			return EvalFxDbl( &fint, ListEvalsub5( SRC ) ) ; 
		case 0xFFFFFFDE :	// intg
			return EvalFxDbl( &floor, ListEvalsub5( SRC ) ) ; 
		case 0xFFFFFFB6 :	// frac
			return EvalFxDbl( &frac, ListEvalsub5( SRC ) ) ; 
		case 0xFFFFFFA7 :	// not
			return EvalFxDbl( &fNot, ListEvalsub5( SRC ) ) ; 

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
			switch ( c ) {
				case 0xFFFFFFAF:	// PxlTest(y,x)
					y=(NoListEvalsubTop( SRC ));
					if ( SRC[ExecPtr] != ',' ) CB_Error(SyntaxERR) ; // Syntax error 
					ExecPtr++ ;	// ',' skip
					x=(NoListEvalsubTop( SRC ));
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

		case 0xFFFFFF86 :	// sqr
			return EvalFxDbl( &fsqrt, ListEvalsub5( SRC ) ) ; 
		case 0xFFFFFF95 :	// log10
			return EvalFxDbl( &flog10, ListEvalsub5( SRC ) ) ; 
		case 0xFFFFFFB5 :	// 10^
			return EvalFxDbl( &fpow10, ListEvalsub5( SRC ) ) ; 
		case 0xFFFFFF85 :	// ln
			return EvalFxDbl( &fln, ListEvalsub5( SRC ) ) ; 
		case 0xFFFFFFA5 :	// expn
			return EvalFxDbl( &fexp, ListEvalsub5( SRC ) ) ; 
		case 0xFFFFFF96 :	// cuberoot
			return EvalFxDbl( &fcuberoot, ListEvalsub5( SRC ) ) ; 

		case 0xFFFFFF81 :	// sin
			return EvalFxDbl( &fsin, ListEvalsub5( SRC ) ) ; 
		case 0xFFFFFF82 :	// cos
			return EvalFxDbl( &fcos, ListEvalsub5( SRC ) ) ; 
		case 0xFFFFFF83 :	// tan
			return EvalFxDbl( &ftan, ListEvalsub5( SRC ) ) ; 
		case 0xFFFFFF91 :	// asin
			return EvalFxDbl( &fasin, ListEvalsub5( SRC ) ) ; 
		case 0xFFFFFF92 :	// acos
			return EvalFxDbl( &facos, ListEvalsub5( SRC ) ) ; 
		case 0xFFFFFF93 :	// atan
			return EvalFxDbl( &fatan, ListEvalsub5( SRC ) ) ; 
		case 0xFFFFFFA1 :	// sinh
			return EvalFxDbl( &sinh, ListEvalsub5( SRC ) ) ; 
		case 0xFFFFFFA2 :	// cosh
			return EvalFxDbl( &cosh, ListEvalsub5( SRC ) ) ; 
		case 0xFFFFFFA3 :	// tanh
			return EvalFxDbl( &tanh, ListEvalsub5( SRC ) ) ; 
		case 0xFFFFFFB1 :	// asinh
			return EvalFxDbl( &asinh, ListEvalsub5( SRC ) ) ; 
		case 0xFFFFFFB2 :	// acosh
			return EvalFxDbl( &acosh, ListEvalsub5( SRC ) ) ; 
		case 0xFFFFFFB3 :	// atanh
			return EvalFxDbl( &atanh, ListEvalsub5( SRC ) ) ; 
			
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
					return CB_EvalStr(SRC, 1 );
				case 0x21:	// Xdot
					return Xdot;
				case 0x1B :		// fn str
					return CB_FnStr( SRC, 1 );
				default:
					ExecPtr--;	// error
					break;
			}
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
				result = EvalFxDbl( &fsqu, result) ; 
				break;
			case  0xFFFFFF9B  :	// ^(-1) RECIP
				result = EvalFxDbl( &frecip, result) ; 
				break;
			case  0xFFFFFFAB  :	//  !
				result = EvalFxDbl( &ffact, result) ; 
				break;
				
			case  0x01  :	//  femto
				result = EvalFxDbl( &ffemto, result) ; 
				break;
			case  0x02  :	//  pico
				result = EvalFxDbl( &fpico, result) ; 
				break;
			case  0x03  :	//  nano
				result = EvalFxDbl( &fnano, result) ; 
				break;
			case  0x04  :	//  micro
				result = EvalFxDbl( &fmicro, result) ; 
				break;
			case  0x05  :	//  milli
				result = EvalFxDbl( &fmilli, result) ; 
				break;
			case  0x06  :	//  Kiro
				result = EvalFxDbl( &fKiro, result) ; 
				break;
			case  0x07  :	//  Mega
				result = EvalFxDbl( &fMega, result) ; 
				break;
			case  0x08  :	//  Giga
				result = EvalFxDbl( &fGiga, result) ; 
				break;
			case  0x09  :	//  Tera
				result = EvalFxDbl( &fTera, result) ; 
				break;
			case  0x0A  :	//  Peta
				result = EvalFxDbl( &fPeta, result) ; 
				break;
			case  0x1B  :	//  Exa
				result = EvalFxDbl( &fExa, result) ; 
				break;
				
			case  0xFFFFFF8C  :	//  dms
				result = DmsToDec( SRC, result );
				break;
				
			case  0xFFFFFF9C  :	//  Deg
				result = EvalFxDbl( &finvdegree, result) ; 
				break;
			case  0xFFFFFFAC  :	//  Rad
				result = EvalFxDbl( &finvradian, result) ; 
				break;
			case  0xFFFFFFBC  :	//  Grad
				result = EvalFxDbl( &finvgrad, result) ; 
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
	double result;
	int c;
	int resultreg;
	int resultflag;
	int execptr;
	
	result = ListEvalsub2( SRC );
	resultflag=dspflag;		// 2:result	3:Listresult
	resultreg=CB_MatListAnsreg;
	while ( 1 ) {
		c = SRC[ExecPtr++];
		switch ( c ) {
			case  0xFFFFFFA8  :	// a ^ b
				result = EvalFxDbl2( &fpow, &resultflag, &resultreg, result, ListEvalsub2( SRC ) ) ;
				break;
			case  0xFFFFFFB8  :	// powroot
				result = EvalFxDbl2( &fpowroot, &resultflag, &resultreg, result, ListEvalsub2( SRC ) ) ;
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
	double result;
	int c;
	int resultreg;
	int resultflag;
	int execptr;
	
	result = ListEvalsub4( SRC );
	resultflag=dspflag;		// 2:result	3:Listresult
	resultreg=CB_MatListAnsreg;
	while ( 1 ) {
		c = SRC[ExecPtr];
		if ((( 'A'<=c )&&( c<='Z' )) ||
			(( 'a'<=c )&&( c<='z' )) ||
			 ( c == 0xFFFFFFCD ) || // <r>
			 ( c == 0xFFFFFFCE ) || // Theta
			 ( c == 0xFFFFFFD0 ) || // PI
			 ( c == 0xFFFFFFC0 ) || // Ans
			 ( c == 0xFFFFFFC1 )) { // Ran#
				result = EvalFxDbl2( &fMUL, &resultflag, &resultreg, result, ListEvalsub4( SRC ) ) ;
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
				case 0x59 :			// ColSize( Mat A )
				case 0x5A :			// RowSize( Mat A )
				case 0x5B :			// MatBase( Mat A )
				result = EvalFxDbl2( &fMUL, &resultflag, &resultreg, result, ListEvalsub4( SRC ) ) ;
					break;
				default:
					goto exitj;
					break;
			}
		} else if ( c == 0xFFFFFFF7 ) { // F7..
			c = SRC[ExecPtr+1];
			switch ( c ) {
				case 0xFFFFFFAF:	// PxlTest(y,x)
				result = EvalFxDbl2( &fMUL, &resultflag, &resultreg, result, ListEvalsub4( SRC ) ) ;
					break;
				default:
					goto exitj;
					break;
			}
		} else if ( c == 0xFFFFFFF9 ) { // F9..
			c = SRC[ExecPtr+1];
			switch ( c ) {
				case 0x21:	// Xdot
				result = EvalFxDbl2( &fMUL, &resultflag, &resultreg, result, ListEvalsub4( SRC ) ) ;
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
			if (c>0) { ExecPtr=execptr; result = EvalFxDbl2( &fMUL, &resultflag, &resultreg, result, ListEvalsub4( SRC ) ) ; }
			else return result;
		}
	 }
}
double ListEvalsub7(char *SRC) {	//  7th Priority abbreviated multiplication type A/C
	double result;
	int c;
	int resultreg;
	int resultflag;
	
	result = ListEvalsub5( SRC );
	resultflag=dspflag;		// 2:result	3:Listresult
	resultreg=CB_MatListAnsreg;
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
				result = EvalFxDbl2( &fMUL, &resultflag, &resultreg, result, ListEvalsub5( SRC ) ) ;
				break;
			default:
				return result;
				break;
		}
	 }
	return result;
}

double ListEvalsub10(char *SRC) {	//  10th Priority  ( *,/, int.,Rmdr )
	double result;
	int c;
	int resultreg;
	int resultflag;
	
	result = ListEvalsub7( SRC );
	resultflag=dspflag;		// 2:result	3:Listresult
	resultreg=CB_MatListAnsreg;
	while ( 1 ) {
		c = SRC[ExecPtr++];
		switch ( c ) {
			case 0xFFFFFFA9 :		// ~
				result = EvalFxDbl2( &fMUL, &resultflag, &resultreg, result, ListEvalsub7( SRC ) ) ;
				break;
			case 0xFFFFFFB9 :		// €
				result = EvalFxDbl2( &fDIV, &resultflag, &resultreg, result, ListEvalsub7( SRC ) ) ;
				break;
			case 0x7F:
				c = SRC[ExecPtr++];
				switch ( c ) {
					case 0xFFFFFFBC:	// Int€
						result = EvalFxDbl2( &fIDIV, &resultflag, &resultreg, result, ListEvalsub7( SRC ) ) ;
						break;
					case 0xFFFFFFBD:	// Rmdr
						result = EvalFxDbl2( &fMOD, &resultflag, &resultreg, result, ListEvalsub7( SRC ) ) ;
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
double ListEvalsub11(char *SRC) {	//  11th Priority  ( +,- )
	double result;
	int c;
	int resultreg;
	int resultflag;

	result = ListEvalsub10( SRC );
	resultflag=dspflag;		// 2:result	3:Listresult
	resultreg=CB_MatListAnsreg;
	while ( 1 ) {
		c = SRC[ExecPtr++];
		switch ( c ) {
			case 0xFFFFFF89 :		// +
				result = EvalFxDbl2( &fADD, &resultflag, &resultreg, result, ListEvalsub10( SRC ) ) ;
				break;
			case 0xFFFFFF99 :		// -
				result = EvalFxDbl2( &fSUB, &resultflag, &resultreg, result, ListEvalsub10( SRC ) ) ;
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
	int resultreg;
	int resultflag;
	
	result = ListEvalsub11( SRC );
	resultflag=dspflag;		// 2:result	3:Listresult
	resultreg=CB_MatListAnsreg;
	while ( 1 ) {
		c = SRC[ExecPtr++];
		switch ( c ) {
			case '=' :	// =
				result = EvalFxDbl2( &fcmpEQ, &resultflag, &resultreg, result, ListEvalsub11( SRC ) ) ;
				break;
			case '>' :	// >
				result = EvalFxDbl2( &fcmpGT, &resultflag, &resultreg, result, ListEvalsub11( SRC ) ) ;
				break;
			case '<' :	// <
				result = EvalFxDbl2( &fcmpLT, &resultflag, &resultreg, result, ListEvalsub11( SRC ) ) ;
				break;
			case 0x11 :	// !=
				result = EvalFxDbl2( &fcmpNE, &resultflag, &resultreg, result, ListEvalsub11( SRC ) ) ;
				break;
			case 0x12 :	// >=
				result = EvalFxDbl2( &fcmpGE, &resultflag, &resultreg, result, ListEvalsub11( SRC ) ) ;
				break;
			case 0x10 :	// <=
				result = EvalFxDbl2( &fcmpLE, &resultflag, &resultreg, result, ListEvalsub11( SRC ) ) ;
				break;
			case 0xFFFFFF9A :	// xor
				result = EvalFxDbl2( &fXOR, &resultflag, &resultreg, result, ListEvalsub11( SRC ) ) ;
				break;
			case '|' :	// or
			case 0xFFFFFFAA :	// or
				result = EvalFxDbl2( &fOR, &resultflag, &resultreg, result, ListEvalsub11( SRC ) ) ;
				break;
			case '&' :	// and
			case 0xFFFFFFBA :	// and
				result = EvalFxDbl2( &fAND, &resultflag, &resultreg, result, ListEvalsub11( SRC ) ) ;
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
	int resultreg;
	int resultflag;
	
	result = ListEvalsub12( SRC );
	resultflag=dspflag;		// 2:result	3:Listresult
	resultreg=CB_MatListAnsreg;
	while ( 1 ) {
		c = SRC[ExecPtr];
		if ( c == 0x7F ) {
			c = SRC[ExecPtr+1];
			switch ( c ) {
				case 0xFFFFFFB0 :	// And
					ExecPtr+=2;
					result = EvalFxDbl2( &fAND_logic, &resultflag, &resultreg, result, ListEvalsub12( SRC ) ) ;
					break;
				default:
					return result;
					break;
			}
		} else return result;
	}
}


double ListEvalsubTop(char *SRC) {	//  
	double result,dst;
	int c,i;
	int resultreg;
	int resultflag;
	
	result = ListEvalsub13( SRC );
	resultflag=dspflag;		// 2:result	3:Listresult
	resultreg=CB_MatListAnsreg;
	while ( 1 ) {
		c = SRC[ExecPtr];
		if ( c == 0x7F ) {
			c = SRC[ExecPtr+1];
			switch ( c ) {
				case 0xFFFFFFB1 :	// Or
					ExecPtr+=2;
					result = EvalFxDbl2( &fOR_logic, &resultflag, &resultreg, result, ListEvalsub13( SRC ) ) ;
					break;
				case 0xFFFFFFB4 :	// Xor
					ExecPtr+=2;
					result = EvalFxDbl2( &fXOR_logic, &resultflag, &resultreg, result, ListEvalsub13( SRC ) ) ;
					break;
				default:
					return result;
					break;
			}
		} else return result;
	}
}

double NoListEvalsubTop(char *SRC) {	//  
	double result,tmp;
	int c,i;
	int base;

	result = EvalsubTop( SRC );
	if ( dspflag >= 3 ) { CB_Error(ArgumentERR); return result; } // Argument error
	return result;
}
int ListEvalsub1Ans(char *SRC) {	//  
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


int ListEvalsubTopAns(char *SRC) {	//  
	double result,tmp;
	int c,i;
	int base;
	int resultreg,tmpreg;

	if ( CB_MatListAnsreg >=28 ) CB_MatListAnsreg=28;
	result = ListEvalsubTop( SRC );
	resultreg=CB_MatListAnsreg;
	if ( dspflag < 3 ) { CB_Error(ArgumentERR); return result; } // Argument error

	if ( SRC[ExecPtr] != 0x0E ) CopyMatList2AnsTop( resultreg );	// ListResult -> List Ans top
	MatdspNo=CB_MatListAnsreg;
	return dspflag;
}

