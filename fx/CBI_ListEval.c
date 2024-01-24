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
//-----------------------------------------------------------------------------
typedef int (*FXPTR)( int x );
int EvalFxInt( FXPTR fxptr, int result ) { 
	int i,j;
	int base;
	int resultreg=CB_MatListAnsreg;
	if ( dspflag >= 3 ) {	// Listresult
		base=MatAry[resultreg].Base;
		for (i=base; i<MatAry[resultreg].SizeA+base; i++ ) {
			for (j=base; j<MatAry[resultreg].SizeB+base; j++ ) {
				result = ReadMatrixInt( resultreg, i, j );
				result = (fxptr)( result );
				WriteMatrixInt( resultreg, i, j, result ) ;	// Fx(Listresult) -> Listresult
			}
		}
	} else {	// result
		result = (fxptr)( result ) ;
	}
	return result;
}

typedef int (*FXPTR2)( int x, int y );
int EvalFxInt2( FXPTR2 fxptr2, int *resultflag, int *resultreg, int result, int tmp ) { 
	int i,j,k,sum;
	int colA,rowB;
	int base;
	int tmpreg58=58;	// Mattmpreg
	int tmpreg=CB_MatListAnsreg;
	if ( dspflag >= 3 ) {	// Listtmp
		base=MatAry[tmpreg].Base;
		if ( *resultflag >= 3 ) {
			if ( ( *resultflag == 4 ) || ( fxptr2 != fMULint ) ) {
				if ( CheckAnsMatList(*resultreg) ) return 0;	// Not same List error
				for (i=base; i<MatAry[*resultreg].SizeA+base; i++ ) {
					for (j=base; j<MatAry[*resultreg].SizeB+base; j++ ) {
						result = ReadMatrixInt( *resultreg, i, j);
						tmp    = ReadMatrixInt( tmpreg, i, j);
						WriteMatrixInt( *resultreg, i, j, (fxptr2)(result,tmp) ) ;	// Listresult (op) Listtmp -> Listresult
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
							result = ReadMatrixInt( *resultreg, i, k);
							tmp    = ReadMatrixInt( tmpreg, k, j);
							sum+=(result*tmp);
						}
						WriteMatrixInt( tmpreg58, i, j, sum ) ;
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
					tmp    = ReadMatrixInt( tmpreg, i, j);
					WriteMatrixInt( tmpreg, i, j, (fxptr2)(result,tmp) ) ;	// result * Listtmp -> Listresult
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
					result = ReadMatrixInt( *resultreg, i, j);
					WriteMatrixInt( *resultreg, i, j, (fxptr2)(result,tmp) ) ;	// Listresult * tmp -> Listresult
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

int ListEvalIntsub1(char *SRC) {	// 1st Priority
	int result=0,tmp,tmp2;
	int sign=1;
	int c,d;
	char *pt;
	int dimA,dimB,reg,x,y,base;
	int i,ptr,mptr;
	char*	MatAryC;
	short*	MatAryW;
	int*	MatAryI;
	int*	MatAryF;
	int resultreg;
	int resultflag;

	dspflag=2;		// 2:value		3:list    4:mat

	c = SRC[ExecPtr++];
	if ( c == '(') {
		result = ListEvalIntsubTop( SRC );
		if ( SRC[ExecPtr] == ')' ) ExecPtr++;
		return result;
	}
	while ( c == 0xFFFFFF89 ) c=SRC[ExecPtr++];	// +
	if ( ( c == 0xFFFFFF87 ) || ( c == 0xFFFFFF99 ) ) {	//  -
		return EvalFxInt( &fsignint, ListEvalIntsub1( SRC ) ) ; 
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
					reg=RegVarAliasEx(SRC); if ( reg<0 ) CB_Error(SyntaxERR) ;  // Syntax error 
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
					result = ListEvalIntsubTop( SRC );
					resultflag=dspflag;		// 2:result	3:Listresult
					resultreg=CB_MatListAnsreg;
					if ( SRC[ExecPtr] != ',' ) CB_Error(SyntaxERR) ; // Syntax error 
					ExecPtr++;
					result = EvalFxInt2( &fMODint, &resultflag, &resultreg, result, ListEvalIntsubTop( SRC ) ) ;
					if ( SRC[ExecPtr] == ')' ) ExecPtr++;
					return result ;
						
				case 0x3C :		// GCD(a,b)
					result = ListEvalIntsubTop( SRC );
					resultflag=dspflag;		// 2:result	3:Listresult
					resultreg=CB_MatListAnsreg;
					if ( SRC[ExecPtr] != ',' ) CB_Error(SyntaxERR) ; // Syntax error 
					ExecPtr++;
					result = EvalFxInt2( &fGCDint, &resultflag, &resultreg, result, ListEvalIntsubTop( SRC ) ) ;
					if ( SRC[ExecPtr] == ')' ) ExecPtr++;
					return result ;
						
				case 0x3D :		// LCM(a,b)
					result = ListEvalIntsubTop( SRC );
					resultflag=dspflag;		// 2:result	3:Listresult
					resultreg=CB_MatListAnsreg;
					if ( SRC[ExecPtr] != ',' ) CB_Error(SyntaxERR) ; // Syntax error 
					ExecPtr++;
					result = EvalFxInt2( &fLCMint, &resultflag, &resultreg, result, ListEvalIntsubTop( SRC ) ) ;
					if ( SRC[ExecPtr] == ')' ) ExecPtr++;
					return result ;
						
				case 0xFFFFFF85 :		// logab(a,b)
					result = ListEvalIntsubTop( SRC );
					resultflag=dspflag;		// 2:result	3:Listresult
					resultreg=CB_MatListAnsreg;
					if ( SRC[ExecPtr] != ',' ) CB_Error(SyntaxERR) ; // Syntax error 
					ExecPtr++;
					result = EvalFxInt2( &flogabint, &resultflag, &resultreg, result, ListEvalIntsubTop( SRC ) ) ;
					if ( SRC[ExecPtr] == ')' ) ExecPtr++;
					return result ;

				case 0xFFFFFFB3 :		// Not
					return EvalFxInt( &fNotint_logic, ListEvalIntsub5( SRC ) ) ; 
						
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
					x=NoListEvalIntsubTop( SRC );
					if ( SRC[ExecPtr] != ',' ) CB_Error(SyntaxERR) ; // Syntax error 
					ExecPtr++ ;	// ',' skip
					y=NoListEvalIntsubTop( SRC );
					if ( SRC[ExecPtr] == ',' ) {
						ExecPtr++;
						CB_RanInt( SRC, x, y );
					}
					if ( SRC[ExecPtr] == ')' ) ExecPtr++;
					return frandIntint( x, y ) ;
					
				case 0xFFFFFFE9 :		// CellSum(Mat A[x,y])
					MatrixOprand( SRC, &reg, &x, &y );
					if ( ErrorNo ) return ; // error
					if ( SRC[ExecPtr] == ')' ) ExecPtr++;
					return Cellsum( reg, x, y );
	
				case 0x5F :				// 1/128 Ticks
					return CB_Ticks( SRC );	// 
						
				case 0xFFFFFFF0 :		// GraphY str
					return CBint_GraphYStr( SRC, 1 );
					
				case 0xFFFFFFF5 :		// IsExist(
					return  CB_IsExist( SRC );
				case 0xFFFFFFF6 :		// Peek(
					return  CB_Peek( SRC, NoListEvalIntsubTop( SRC ) );
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
				case 0x4D :				// Prod List 1
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
			return rand();
		case 0xFFFFFF97 :	// abs
			return EvalFxInt( &fabsint, ListEvalIntsub5( SRC ) ) ; 
		case 0xFFFFFFA6 :	// int
		case 0xFFFFFFDE :	// intg
			return EvalFxInt( &fintint, ListEvalIntsub5( SRC ) ) ; 
		case 0xFFFFFFB6 :	// frac
			return EvalFxInt( &ffracint, ListEvalIntsub5( SRC ) ) ; 
		case 0xFFFFFFA7 :	// not
			return EvalFxInt( &fNotint, ListEvalIntsub5( SRC ) ) ; 

		case '%' :	// 1/128 Ticks
			return CB_Ticks( SRC );	// 
		case '*' :	// peek
			return CB_Peek( SRC, ListEvalIntsub1( SRC ) );	// 

		case '{':	// { 1,2,3,4,5... }->List Ans
			CB_List(SRC);
			return 0;
		case '[':	// [ [0,1,2][2,3,4] ]->Mat Ans
			CB_Matrix(SRC);
			return 0;

		case 0xFFFFFF86 :	// sqr
			return EvalFxInt( &fsqrtint, ListEvalIntsub5( SRC ) ) ; 
		case 0xFFFFFF95 :	// log10
			return EvalFxInt( &flog10int, ListEvalIntsub5( SRC ) ) ; 
		case 0xFFFFFFB5 :	// 10^
			return EvalFxInt( &fpow10int, ListEvalIntsub5( SRC ) ) ; 
		case 0xFFFFFF85 :	// ln
			return EvalFxInt( &flnint, ListEvalIntsub5( SRC ) ) ; 
		case 0xFFFFFFA5 :	// expn
			return EvalFxInt( &fexpint, ListEvalIntsub5( SRC ) ) ; 
		case 0xFFFFFF96 :	// cuberoot
			return EvalFxInt( &fcuberootint, ListEvalIntsub5( SRC ) ) ; 
			
		case 0xFFFFFFF7:	// F7..
			c = SRC[ExecPtr++];
			switch ( c ) {
				case 0xFFFFFFAF:	// PxlTest(y,x)
					y=(NoListEvalIntsubTop( SRC ));
					if ( SRC[ExecPtr] != ',' ) CB_Error(SyntaxERR) ; // Syntax error 
					ExecPtr++ ;	// ',' skip
					x=(NoListEvalIntsubTop( SRC ));
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
				case 0xFFFFFFC6:	// M_PixelTest(
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
					return CBint_FnStr( SRC, 1 );
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
//int ListEvalIntObjectAlignE4g( unsigned int n ){ return n ; }	// align +4byte
//int ListEvalIntObjectAlignE4h( unsigned int n ){ return n+n; }	// align +6byte
//-----------------------------------------------------------------------------

int ListEvalIntsub2(char *SRC) {	//  2nd Priority  ( type B function ) ...
	int cont=1;
	int result,tmp;
	int c,i;
	int base;
	int resultreg,tmpreg;
	result = ListEvalIntsub1( SRC );
	resultreg=CB_MatListAnsreg;
	while ( 1 ) {
		c = SRC[ExecPtr++];
		switch ( c ) {
			case  0xFFFFFF8B  :	// ^2
				result = EvalFxInt( &fsquint, result) ; 
				break;
			case  0xFFFFFF9B  :	// ^(-1) RECIP
				result = EvalFxInt( &frecipint, result) ; 
				break;
			case  0xFFFFFFAB  :	//  !
				result = EvalFxInt( &ffactint, result) ; 
				break;
			default:
				ExecPtr--;
				return result;
				break;
		}
	 }
	return result;
}
int ListEvalIntsub4(char *SRC) {	//  3rd Priority  ( ^ ...)
	int result;
	int c;
	int resultreg;
	int resultflag;
	int execptr;
	
	result = ListEvalIntsub2( SRC );
	resultflag=dspflag;		// 2:result	3:Listresult
	resultreg=CB_MatListAnsreg;
	while ( 1 ) {
		c = SRC[ExecPtr++];
		switch ( c ) {
			case  0xFFFFFFA8  :	// a ^ b
				result = EvalFxInt2( &fpowint, &resultflag, &resultreg, result, ListEvalIntsub2( SRC ) ) ;
				break;
			case  0xFFFFFFB8  :	// powroot
				result = EvalFxInt2( &fpowrootint, &resultflag, &resultreg, result, ListEvalIntsub2( SRC ) ) ;
				break;
			default:
				ExecPtr--;
				return result;
				break;
		}
	 }
	return result;
}

int ListEvalIntsub5(char *SRC) {	//  5th Priority abbreviated multiplication
	int result;
	int c;
	int resultreg;
	int resultflag;
	int execptr;
	
	result = ListEvalIntsub4( SRC );
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
				result = EvalFxInt2( &fMULint, &resultflag, &resultreg, result, ListEvalIntsub4( SRC ) ) ;
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
				result = EvalFxInt2( &fMULint, &resultflag, &resultreg, result, ListEvalIntsub4( SRC ) ) ;
					break;
				default:
					goto exitj;
					break;
			}
		} else if ( c == 0xFFFFFFF7 ) { // F7..
			c = SRC[ExecPtr+1];
			switch ( c ) {
				case 0xFFFFFFAF:	// PxlTest(y,x)
				result = EvalFxInt2( &fMULint, &resultflag, &resultreg, result, ListEvalIntsub4( SRC ) ) ;
					break;
				default:
					goto exitj;
					break;
			}
		} else if ( c == 0xFFFFFFF9 ) { // F9..
			c = SRC[ExecPtr+1];
			switch ( c ) {
				case 0x21:	// Xdot
				result = EvalFxInt2( &fMULint, &resultflag, &resultreg, result, ListEvalIntsub4( SRC ) ) ;
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
			if (c>0) { ExecPtr=execptr; result = EvalFxInt2( &fMULint, &resultflag, &resultreg, result, ListEvalIntsub4( SRC ) ) ; }
			else return result;
		}
	 }
}
int ListEvalIntsub7(char *SRC) {	//  7th Priority abbreviated multiplication type A/C
	int result;
	int c;
	int resultreg;
	int resultflag;
	
	result = ListEvalIntsub5( SRC );
	resultflag=dspflag;		// 2:result	3:Listresult
	resultreg=CB_MatListAnsreg;
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
				result = EvalFxInt2( &fMULint, &resultflag, &resultreg, result, ListEvalIntsub5( SRC ) ) ;
				break;
			default:
				return result;
				break;
		}
	 }
	return result;
}

int ListEvalIntsub10(char *SRC) {	//  10th Priority  ( *,/, int.,Rmdr )
	int result;
	int c;
	int resultreg;
	int resultflag;
	
	result = ListEvalIntsub7( SRC );
	resultflag=dspflag;		// 2:result	3:Listresult
	resultreg=CB_MatListAnsreg;
	while ( 1 ) {
		c = SRC[ExecPtr++];
		switch ( c ) {
			case 0xFFFFFFA9 :		// ~
				result = EvalFxInt2( &fMULint, &resultflag, &resultreg, result, ListEvalIntsub7( SRC ) ) ;
				break;
			case 0xFFFFFFB9 :		// €
				result = EvalFxInt2( &fDIVint, &resultflag, &resultreg, result, ListEvalIntsub7( SRC ) ) ;
				break;
			case 0x7F:
				c = SRC[ExecPtr++];
				switch ( c ) {
					case 0xFFFFFFBC:	// Int€
						result = EvalFxInt2( &fDIVint, &resultflag, &resultreg, result, ListEvalIntsub7( SRC ) ) ;
						break;
					case 0xFFFFFFBD:	// Rmdr
						result = EvalFxInt2( &fMODint, &resultflag, &resultreg, result, ListEvalIntsub7( SRC ) ) ;
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
int ListEvalIntsub11(char *SRC) {	//  11th Priority  ( +,- )
	int result;
	int c;
	int resultreg;
	int resultflag;

	result = ListEvalIntsub10( SRC );
	resultflag=dspflag;		// 2:result	3:Listresult
	resultreg=CB_MatListAnsreg;
	while ( 1 ) {
		c = SRC[ExecPtr++];
		switch ( c ) {
			case 0xFFFFFF89 :		// +
				result = EvalFxInt2( &fADDint, &resultflag, &resultreg, result, ListEvalIntsub10( SRC ) ) ;
				break;
			case 0xFFFFFF99 :		// -
				result = EvalFxInt2( &fSUBint, &resultflag, &resultreg, result, ListEvalIntsub10( SRC ) ) ;
				break;
			default:
				ExecPtr--;
				return result;
				break;
		}
	 }
	return result;
}
int ListEvalIntsub12(char *SRC) {	//  12th Priority ( =,!=,><,>=,<= )
	int result;
	int c;
	int resultreg;
	int resultflag;
	
	result = ListEvalIntsub11( SRC );
	resultflag=dspflag;		// 2:result	3:Listresult
	resultreg=CB_MatListAnsreg;
	while ( 1 ) {
		c = SRC[ExecPtr++];
		switch ( c ) {
			case '=' :	// =
				result = EvalFxInt2( &fcmpEQint, &resultflag, &resultreg, result, ListEvalIntsub11( SRC ) ) ;
				break;
			case '>' :	// >
				result = EvalFxInt2( &fcmpGTint, &resultflag, &resultreg, result, ListEvalIntsub11( SRC ) ) ;
				break;
			case '<' :	// <
				result = EvalFxInt2( &fcmpLTint, &resultflag, &resultreg, result, ListEvalIntsub11( SRC ) ) ;
				break;
			case 0x11 :	// !=
				result = EvalFxInt2( &fcmpNEint, &resultflag, &resultreg, result, ListEvalIntsub11( SRC ) ) ;
				break;
			case 0x12 :	// >=
				result = EvalFxInt2( &fcmpGEint, &resultflag, &resultreg, result, ListEvalIntsub11( SRC ) ) ;
				break;
			case 0x10 :	// <=
				result = EvalFxInt2( &fcmpLEint, &resultflag, &resultreg, result, ListEvalIntsub11( SRC ) ) ;
				break;
			case 0xFFFFFF9A :	// xor
				result = EvalFxInt2( &fXORint, &resultflag, &resultreg, result, ListEvalIntsub11( SRC ) ) ;
				break;
			case '|' :	// or
			case 0xFFFFFFAA :	// or
				result = EvalFxInt2( &fORint,  &resultflag, &resultreg, result, ListEvalIntsub11( SRC ) ) ;
				break;
			case '&' :	// and
			case 0xFFFFFFBA :	// and
				result = EvalFxInt2( &fANDint, &resultflag, &resultreg, result, ListEvalIntsub11( SRC ) ) ;
				break;
			default:
				ExecPtr--;
				return result;
				break;
		}
	 }
	return result;
}
int ListEvalIntsub13(char *SRC) {	//  13th Priority  ( And,and)
	int result;
	int c;
	int resultreg;
	int resultflag;
	
	result = ListEvalIntsub12( SRC );
	resultflag=dspflag;		// 2:result	3:Listresult
	resultreg=CB_MatListAnsreg;
	while ( 1 ) {
		c = SRC[ExecPtr];
		if ( c == 0x7F ) {
			c = SRC[ExecPtr+1];
			switch ( c ) {
				case 0xFFFFFFB0 :	// And
					ExecPtr+=2;
					result = EvalFxInt2( &fANDint_logic, &resultflag, &resultreg, result, ListEvalIntsub12( SRC ) ) ;
					break;
				default:
					return result;
					break;
			}
		} else return result;
	}
}

int ListEvalIntsubTop(char *SRC) {	//  
	int result,dst;
	int c,i;
	int resultreg;
	int resultflag;
	
	result = ListEvalIntsub13( SRC );
	resultflag=dspflag;		// 2:result	3:Listresult
	resultreg=CB_MatListAnsreg;
	while ( 1 ) {
		c = SRC[ExecPtr];
		if ( c == 0x7F ) {
			c = SRC[ExecPtr+1];
			switch ( c ) {
				case 0xFFFFFFB1 :	// Or
					ExecPtr+=2;
					result = EvalFxInt2( &fORint_logic, &resultflag, &resultreg, result, ListEvalIntsub13( SRC ) ) ;
					break;
				case 0xFFFFFFB4 :	// Xor
					ExecPtr+=2;
					result = EvalFxInt2( &fXORint_logic, &resultflag, &resultreg, result, ListEvalIntsub13( SRC ) ) ;
					break;
				default:
					return result;
					break;
			}
		} else return result;
	}
}

int NoListEvalIntsubTop(char *SRC) {	//  
	double result,tmp;
	int c,i;
	int base;

	result = EvalIntsubTop( SRC );
	if ( dspflag >= 3 ) { CB_Error(ArgumentERR); return result; } // Argument error
	return result;
}

int ListEvalIntsub1Ans(char *SRC) {	//  
	int result,tmp;
	int c,i;
	int base;
	int resultreg,tmpreg;

	result = ListEvalIntsub1( SRC );
	resultreg=CB_MatListAnsreg;
	if ( dspflag < 3 ) { CB_Error(ArgumentERR); return result; } // Argument error

	if ( SRC[ExecPtr] != 0x0E ) CopyMatList2AnsTop( resultreg );	// ListResult -> List Ans top
	return result;
}


int ListEvalIntsubTopAns(char *SRC) {	//  
	int result,tmp;
	int c,i;
	int base;
	int resultreg,tmpreg;

	if ( CB_MatListAnsreg >=28 ) CB_MatListAnsreg=28;
	result = ListEvalIntsubTop( SRC );
	resultreg=CB_MatListAnsreg;
	if ( dspflag < 3 ) { CB_Error(ArgumentERR); return result; } // Argument error

	if ( SRC[ExecPtr] != 0x0E ) CopyMatList2AnsTop( resultreg );	// ListResult -> List Ans top
	MatdspNo=CB_MatListAnsreg;
	return dspflag;
}

