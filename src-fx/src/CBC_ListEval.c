#include "CB.h"
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void Cplx_WriteListAns2( complex x, complex y ) {
	int base=MatBase;
	dspflag=4;	// List ans
	NewMatListAns( 2, 1, base, 128 );		// List Ans[2]
	Cplx_WriteMatrix( CB_MatListAnsreg, base,   base, x ) ;	//
	Cplx_WriteMatrix( CB_MatListAnsreg, base+1, base, y ) ;	// 
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
typedef complex (*CFXPTR)( complex x );
complex Cplx_EvalFxDbl( CFXPTR fxptr, complex result ) { 
	int i,j;
	int base;
	int resultreg=CB_MatListAnsreg;
	if ( dspflag >= 3 ) {	// Listresult
		base=MatAry[resultreg].Base;
		for (i=base; i<MatAry[resultreg].SizeA+base; i++ ) {
			for (j=base; j<MatAry[resultreg].SizeB+base; j++ ) {
				result = Cplx_ReadMatrix( resultreg, i, j );
				result = (fxptr)( result );
				Cplx_WriteMatrix( resultreg, i, j, result ) ;	// Fx(Listresult) -> Listresult
			}
		}
	} else {	// result
		result = (fxptr)( result ) ;
	}
	return result;
}

typedef complex (*CFXPTR2)( complex x, complex y );
complex Cplx_EvalFxDbl2( CFXPTR2 fxptr2, int *resultflag, int *resultreg, complex result, complex tmp ) { 
	complex sum;
	int i,j,k;
	int colA,rowB;
	int base;
	int tmpreg58=Mattmpreg;
	int tmpreg=CB_MatListAnsreg;
	if ( dspflag >= 3 ) {	// Listtmp
		base=MatAry[tmpreg].Base;
		if ( *resultflag >= 3 ) {
			if ( ( *resultflag == 4 ) || ( fxptr2 != Cplx_fMUL ) ) {
				if ( CheckAnsMatList(*resultreg) ) return Int2Cplx(0);	// Not same List error
				for (i=base; i<MatAry[*resultreg].SizeA+base; i++ ) {
					for (j=base; j<MatAry[*resultreg].SizeB+base; j++ ) {
						result = Cplx_ReadMatrix( *resultreg, i, j);
						tmp    = Cplx_ReadMatrix( tmpreg, i, j);
						Cplx_WriteMatrix( *resultreg, i, j, (fxptr2)(result,tmp) ) ;	// Listresult (op) Listtmp -> Listresult
					}
				}
			} else {	// Mat A * Mat B -> Mat C		Matresult * Mattmp -> Matresult
				if ( MatAry[*resultreg].SizeB != MatAry[tmpreg].SizeA ) { CB_Error(DimensionERR); return Int2Cplx(0) ; }	// Dimension error
				colA=MatAry[*resultreg].SizeA;
				rowB=MatAry[tmpreg].SizeB;
				DimMatrixSub( tmpreg58, MatAry[*resultreg].ElementSize, colA, rowB, base);	// Mattmpreg
				if ( g_error_type ) return Int2Cplx(0); // error
				for ( i=base; i<colA+base; i++ ){
					for ( j=base; j<rowB+base; j++ ){
						sum=Int2Cplx(0);
						for ( k=base; k<MatAry[*resultreg].SizeB+base; k++ ){
							result = Cplx_ReadMatrix( *resultreg, i, k);
							tmp    = Cplx_ReadMatrix( tmpreg, k, j);
							sum = Cplx_fADD( sum, Cplx_fMUL(result,tmp) );
						}
						Cplx_WriteMatrix( tmpreg58, i, j, sum ) ;
					}
				}
				DimMatrixSub( *resultreg, MatAry[*resultreg].ElementSize, colA, rowB, base);	// Mattmpreg
				if ( g_error_type ) return Int2Cplx(0); // error
				CopyMatrix( *resultreg, tmpreg58 );		// tmpreg58 -> Matresult
			}
			DeleteMatListAns();	// delete tmp
		} else {
			for (i=base; i<MatAry[tmpreg].SizeA+base; i++ ) {
				for (j=base; j<MatAry[tmpreg].SizeB+base; j++ ) {
					tmp    = Cplx_ReadMatrix( tmpreg, i, j);
					Cplx_WriteMatrix( tmpreg, i, j, (fxptr2)(result,tmp) ) ;	// result * Listtmp -> Listresult
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
					result = Cplx_ReadMatrix( *resultreg, i, j);
					Cplx_WriteMatrix( *resultreg, i, j, (fxptr2)(result,tmp) ) ;	// Listresult * tmp -> Listresult
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

complex Cplx_ListEvalsub1(char *SRC) {	// 1st Priority
	complex result={0,0},tmp,tmp2;
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

	dspflag=2;		// 2:value		3:Mat    4:List

	c = SRC[g_exec_ptr++];
  topj:
	if ( c == '(') {
		result = Cplx_ListEvalsubTop( SRC );
		if ( SRC[g_exec_ptr] == ')' ) g_exec_ptr++;
		return result;
	}
	while ( c == 0xFFFFFF89 ) c=SRC[g_exec_ptr++];	// +
	if ( ( c == 0xFFFFFF87 ) || ( c == 0xFFFFFF99 ) ) {	//  -
		return Cplx_EvalFxDbl( &Cplx_fsign, Cplx_ListEvalsub5( SRC ) ) ; 
	}
	if ( ( ( 'A'<=c )&&( c<='Z' ) ) || ( ( 'a'<=c )&&( c<='z' ) ) )  {
		reg=c-'A';
	  regj:
		c=SRC[g_exec_ptr];
		if ( c=='%' ) { g_exec_ptr++; return Int2Cplx( LocalInt[reg][0] ); }
		else
		if ( c=='[' ) { goto Matrix; }
		else
		if ( ( '0'<=c )&&( c<='9' ) ) {
				g_exec_ptr++;
				dimA=c-'0';
				MatOprand1num( SRC, reg, &dimA, &dimB );
				goto Matrix2;
		} else
		if ( c=='#' ) { g_exec_ptr++; return LocalDbl[reg][0] ; }
		if (CB_INT==1) return Int2Cplx( LocalInt[reg][0] ); else return LocalDbl[reg][0] ;
	}
	if ( ( c=='.' ) ||( c==0x0F ) || ( ( '0'<=c )&&( c<='9' ) ) ) {
		g_exec_ptr--;
		result.real = Eval_atof( SRC , c );
		return result;
	}
	
	switch ( c ) { 			// ( type C function )  sin cos tan... 
		case 0x7F:	// 7F..
			c = SRC[g_exec_ptr++];
			switch ( c ) {
				case 0x40 :		// Mat A[a,b]
				  Matjmp:
					reg=MatRegVar(SRC); if ( reg<0 ) CB_Error(SyntaxERR) ; // Syntax error 
					Matrix1:	
					if ( SRC[g_exec_ptr] == '[' ) {
					Matrix:	
						g_exec_ptr++;
						MatOprand2( SRC, reg, &dimA, &dimB );
					Matrix2:
						if ( g_error_type ) return Int2Cplx( 1 ) ; // error
					} else { dspflag=3;	// Mat A
							dimA=MatAry[reg].Base; dimB=dimA;
							CopyMatList2Ans( reg );
					}
					return Cplx_ReadMatrix( reg, dimA, dimB);
						
				case 0xFFFFFF84 :	// Vct A[a,b]
					reg=VctRegVar(SRC); if ( reg<0 ) CB_Error(SyntaxERR) ; // Syntax error 
					goto Matrix1;
					
				case 0x51 :		// List 1~26
				case 0x6A :		// List1
				case 0x6B :		// List2
				case 0x6C :		// List3
				case 0x6D :		// List4
				case 0x6E :		// List5
				case 0x6F :		// List6
					reg=ListRegVar( SRC );
				  Listj:
					if ( SRC[g_exec_ptr] == '[' ) {
						g_exec_ptr++;
						MatOprand1( SRC, reg, &dimA, &dimB );	// List 1[a]
						if ( g_error_type ) return Int2Cplx( 1 ) ; // error
					} else { dspflag=4;	// List 1
							dimA=MatAry[reg].Base; dimB=dimA;
							CopyMatList2Ans( reg );
					}
					return Cplx_ReadMatrix( reg, dimA, dimB);
					
				case 0x50 :		// i
					result.imag = 1;
					return result;
					
				case 0x3A :		// MOD(a,b)
					result = Cplx_ListEvalsubTop( SRC );
					resultflag=dspflag;		// 2:result	3:Listresult
					resultreg=CB_MatListAnsreg;
					if ( SRC[g_exec_ptr] != ',' ) CB_Error(SyntaxERR) ; // Syntax error 
					g_exec_ptr++;
					result = Cplx_EvalFxDbl2( &Cplx_fMOD, &resultflag, &resultreg, result, Cplx_ListEvalsubTop( SRC ) ) ;
					if ( SRC[g_exec_ptr] == ')' ) g_exec_ptr++;
					return result ;
						
				case 0x3C :		// GCD(a,b)
					result = Cplx_ListEvalsubTop( SRC );
					resultflag=dspflag;		// 2:result	3:Listresult
					resultreg=CB_MatListAnsreg;
					if ( SRC[g_exec_ptr] != ',' ) CB_Error(SyntaxERR) ; // Syntax error 
					g_exec_ptr++;
					result = Cplx_EvalFxDbl2( &Cplx_fGCD, &resultflag, &resultreg, result, Cplx_ListEvalsubTop( SRC ) ) ;
					if ( SRC[g_exec_ptr] == ')' ) g_exec_ptr++;
					return result ;
						
				case 0x3D :		// LCM(a,b)
					result = Cplx_ListEvalsubTop( SRC );
					resultflag=dspflag;		// 2:result	3:Listresult
					resultreg=CB_MatListAnsreg;
					if ( SRC[g_exec_ptr] != ',' ) CB_Error(SyntaxERR) ; // Syntax error 
					g_exec_ptr++;
					result = Cplx_EvalFxDbl2( &Cplx_fLCM, &resultflag, &resultreg, result, Cplx_ListEvalsubTop( SRC ) ) ;
					if ( SRC[g_exec_ptr] == ')' ) g_exec_ptr++;
					return result ;
						
				case 0xFFFFFF85 :		// logab(a,b)
					result = Cplx_ListEvalsubTop( SRC );
					resultflag=dspflag;		// 2:result	3:Listresult
					resultreg=CB_MatListAnsreg;
					if ( SRC[g_exec_ptr] != ',' ) CB_Error(SyntaxERR) ; // Syntax error 
					g_exec_ptr++;
					result = Cplx_EvalFxDbl2( &Cplx_flogab, &resultflag, &resultreg, result, Cplx_ListEvalsubTop( SRC ) ) ;
					if ( SRC[g_exec_ptr] == ')' ) g_exec_ptr++;
					return result ;

				case 0xFFFFFFB3 :		// Not
					return Cplx_EvalFxDbl( &Cplx_fNot_logic, Cplx_ListEvalsub5( SRC ) ) ; 
						
				case 0xFFFFFF9F :		// KeyRow(
					return Int2Cplx( CB_KeyRow( SRC ) ) ; 
				case 0xFFFFFF8F :		// Getkey
					return Int2Cplx( CB_GetkeyEntry( SRC ) );
					
				case 0xFFFFFF87 :		// RanInt#(st,en[,n])
					x=Cplx_NoListEvalsubTop( SRC ).real;
					if ( SRC[g_exec_ptr] != ',' ) CB_Error(SyntaxERR) ; // Syntax error 
					g_exec_ptr++ ;	// ',' skip
					y=Cplx_NoListEvalsubTop( SRC ).real;
					if ( SRC[g_exec_ptr] == ',' ) {
						g_exec_ptr++;
						CB_RanInt( SRC, x, y );
					}
					if ( SRC[g_exec_ptr] == ')' ) g_exec_ptr++;
					return Dbl2Cplx( frandIntint( x, y ) );
					
				case 0xFFFFFF88 :		// RanList#(n) ->ListAns
					CB_RanList( SRC ) ;
					return Dbl2Cplx( 4 );
				case 0xFFFFFF89 :		// RanBin#(n,p[,m]) ->ListAns
					return Dbl2Cplx( CB_RanBin( SRC ) );
				case 0xFFFFFF8A :		// RanNorm#(sd,mean[,n]) ->ListAns
					return Dbl2Cplx( CB_RanNorm( SRC ) );
						
				case 0xFFFFFFE9 :		// CellSum(Mat A[x,y])
					MatrixOprand( SRC, &reg, &x, &y );
					if ( g_error_type ) return Int2Cplx( 1 ); // error
					if ( SRC[g_exec_ptr] == ')' ) g_exec_ptr++;
					return Int2Cplx( Cellsum( reg, x, y ) );
	
				case 0x5F :				// 1/128 Ticks
					return Int2Cplx( CB_Ticks( SRC ) );	// 
						
				case 0xFFFFFF86 :		// RndFix(n,digit)
					result = Cplx_ListEvalsubTop( SRC );
					resultflag=dspflag;		// 2:result	3:Listresult
					resultreg=CB_MatListAnsreg;
					if ( SRC[g_exec_ptr] != ',' ) CB_Error(SyntaxERR) ; // Syntax error 
					if ( SRC[++g_exec_ptr] == 0xFFFFFFE4 ) { g_exec_ptr++; i=(int)&Cplx_RoundSci; } else i=(int)&Cplx_RoundFix;
					result = Cplx_EvalFxDbl2( (CFXPTR2)i, &resultflag, &resultreg, result, Cplx_ListEvalsubTop( SRC ) ) ;
					if ( SRC[g_exec_ptr] == ')' ) g_exec_ptr++;
					return result ;
					
				case 0xFFFFFFF0 :		// GraphY str
				case 0xFFFFFFF1:		// Graphr
				case 0xFFFFFFF2:		// GraphXt
				case 0xFFFFFFF3:		// GraphYt
				case 0xFFFFFFF4:		// GraphX
					return CB_Cplx_GraphYStr( SRC, 1 );
					
				case 0xFFFFFFF5 :		// IsExist(
					return  Int2Cplx( CB_IsExist( SRC, 1 ) );
				case 0xFFFFFFF6 :		// Peek(
					return  CB_Peek( SRC, Cplx_NoListEvalsubTop( SRC ).real );
				case 0xFFFFFFF8 :		// VarPtr(
					return  Int2Cplx( CB_VarPtr( SRC ) );
				case 0xFFFFFFFA :		// ProgPtr(
					return  Int2Cplx( CB_ProgPtr( SRC ) );
				case 0x00 :				// Xmin
					return Dbl2Cplx( Xmin );
				case 0x01 :				// Xmax
					return Dbl2Cplx( Xmax );
				case 0x02 :				// Xscl
					return Dbl2Cplx( Xscl );
				case 0x04 :				// Ymin
					return Dbl2Cplx( Ymin );
				case 0x05 :				// Ymax
					return Dbl2Cplx( Ymax );
				case 0x06 :				// Yscl
					return Dbl2Cplx( Yscl );
				case 0x08 :				// Thetamin
					return Dbl2Cplx( TThetamin );
				case 0x09 :				// Thetamax
					return Dbl2Cplx( TThetamax );
				case 0x0A :				// Thetaptch
					return Dbl2Cplx( TThetaptch );
				case 0x0B :				// Xfct
					return Dbl2Cplx( Xfct );
				case 0x0C :				// Yfct
					return Dbl2Cplx( Yfct );
	
				case 0x22 :				// Arg
					return Cplx_EvalFxDbl( &Cplx_farg, Cplx_ListEvalsub5( SRC ) ) ;
				case 0x23 :				// Conjg
					return Cplx_EvalFxDbl( &Cplx_fconjg, Cplx_ListEvalsub5( SRC ) ) ;
				case 0x24 :				// ReP
					return Cplx_EvalFxDbl( &Cplx_fReP, Cplx_ListEvalsub5( SRC ) ) ;
				case 0x25 :				// ImP
					return Cplx_EvalFxDbl( &Cplx_fImP, Cplx_ListEvalsub5( SRC ) ) ;
					
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
					return Int2Cplx( 3 );
				case 0x48:	// Identity 
					CB_Identity(SRC);
					return Int2Cplx( 3 );
				case 0x49:	// Augment(
					CB_Augment(SRC);
					return Int2Cplx( 3 );
				case 0x2C:	// Seq
					CB_Seq(SRC);
					return Int2Cplx( 3 );
				case 0x41:	// Trn
					CB_MatTrn(SRC);
					return Int2Cplx( 3 );
				case 0x21:	// Det
					return Cplx_CB_MatDet(SRC);
				
				case 0x55 :				// Ref Mat A
					return Cplx_CB_MatRefRref( SRC, 0 );
				case 0x56 :				// Rref Mat A
					return Cplx_CB_MatRefRref( SRC, 1 );

				case 0x46 :				// Dim
					result.real = CB_Dim( SRC );
					result.imag = 0;
					if ( (result.real) >= 0 ) return result;
					g_exec_ptr--;	// error
					break;
				case 0x58 :				// ElemSize( Mat A )
					return Int2Cplx( CB_ElemSize( SRC ) );
				case 0x59 :				// RowSize( Mat A )
					return Int2Cplx( CB_RowSize( SRC ) );
				case 0x5A :				// ColSize( Mat A )
					return Int2Cplx( CB_ColSize( SRC ) );
				case 0x5B :				// MatBase( Mat A )
					return Int2Cplx( CB_MatBase( SRC ) );
					
				case 0x5D :				// GetRGB() ->ListAns
					return Int2Cplx(CB_GetRGB( SRC , 0 ) );
				case 0x5E :				// RGB(
					return Int2Cplx(CB_RGB( SRC , 0 ) );
				case 0x70 :				// GetHSV() ->ListAns
					return Int2Cplx(CB_GetRGB( SRC , 0x11 ) );
				case 0x71 :				// HSV(
					return Int2Cplx(CB_RGB( SRC , 1) );
				case 0x72 :				// GetHSL() ->ListAns
					return Int2Cplx(CB_GetRGB( SRC , 0x12 ) );
				case 0x73 :				// HSL(
					return Int2Cplx(CB_RGB( SRC , 2 ) );

				case 0x5C :				// ListCmp( List 1, List 2)
					return Int2Cplx( CB_ListCmp( SRC ) );
					
				case 0x4A :				// List>Mat( List 1, List 2,..) -> List 5
					CB_List2Mat( SRC );
					return Int2Cplx( 0 );
				case 0x4B :				// Mat>List( Mat A, m) -> List n
					CB_Mat2List( SRC );
					return Int2Cplx( 0 );
					
				case 0x26 :				// dx/dy
					return Dbl2Cplx( CB_Differ( SRC ) );
					
				case 0xFFFFFFCF :				// System(
					return Int2Cplx( CB_System( SRC ) );
				case 0xFFFFFFDF :				// Version
					return Int2Cplx( CB_Version() );		//

				case 0xFFFFFF90 :				// F Result
					CB_F_Result( SRC );
				case 0xFFFFFF91 :				// F Start
				case 0xFFFFFF92 :				// F End
				case 0xFFFFFF93 :				// F pitch
					return Dbl2Cplx( REGf[c-0xFFFFFF90] );

				default:
					g_exec_ptr--;	// error
					break;
			}
			break;
		case '&' :	// & VarPtr
			return Int2Cplx( CB_VarPtr( SRC ) );
		case 0xFFFFFFD0 :	// const_PI
			return Dbl2Cplx( const_PI );
		case 0xFFFFFFC1 :	// Ran#
			return Dbl2Cplx( CB_frand( SRC ) );
		case 0xFFFFFF97 :	// abs
			return Cplx_EvalFxDbl( &Cplx_fabs, Cplx_ListEvalsub5( SRC ) ) ; 
		case 0xFFFFFFA6 :	// int
			return Cplx_EvalFxDbl( &Cplx_fint, Cplx_ListEvalsub5( SRC ) ) ; 
		case 0xFFFFFFDE :	// intg
			return Cplx_EvalFxDbl( &Cplx_intg, Cplx_ListEvalsub5( SRC ) ) ; 
		case 0xFFFFFFB6 :	// frac
			return Cplx_EvalFxDbl( &Cplx_frac, Cplx_ListEvalsub5( SRC ) ) ; 
		case 0xFFFFFFA7 :	// not
			return Cplx_EvalFxDbl( &Cplx_fNot, Cplx_ListEvalsub5( SRC ) ) ; 

		case '%' :	// 1/128 Ticks
			return Int2Cplx( CB_Ticks( SRC ) );	// 
		case '*' :	// peek
			return CB_Peek( SRC, Cplx_ListEvalsub1( SRC ).real );	// 
//		case '@' :	// Mat @A
//			ExecPtr--;
//			goto Matjmp;

		case '{':	// { 1,2,3,4,5... }->List Ans
			CB_List(SRC);
			return Int2Cplx( 0 );
		case '[':	// [ [0,1,2][2,3,4] ]->Mat Ans
			CB_Matrix(SRC);
			return Int2Cplx( 0 );

		case 0xFFFFFFF7:	// F7..
			c = SRC[g_exec_ptr++];
			switch ( c ) {
				case 0xFFFFFFAF:	// PxlTest(y,x)
					y=Cplx_NoListEvalsubTop( SRC ).real;
					if ( SRC[g_exec_ptr] != ',' ) CB_Error(SyntaxERR) ; // Syntax error 
					g_exec_ptr++ ;	// ',' skip
					x=Cplx_NoListEvalsubTop( SRC ).real;
					if ( SRC[g_exec_ptr] == ')' ) g_exec_ptr++;
					return Int2Cplx( PxlTest(y, x) );			// 
				case 0xFFFFFFB0 :				// SortA( List 1)
					return Int2Cplx( CB_EvalSortAD( SRC, 1 ) );
				case 0xFFFFFFB1 :				// SortD( List 1)
					return Int2Cplx( CB_EvalSortAD( SRC, 0 ) );
				case 0xFFFFFFF4:	// SysCall(
					return  Int2Cplx( CB_SysCall( SRC ) );
				case 0xFFFFFFF5:	// Call(
					return  Int2Cplx( CB_Call( SRC ) );
				case 0xFFFFFFF8:	// RefreshCtrl
					return  Int2Cplx( RefreshCtrl );
				case 0xFFFFFFFA:	// RefreshTime
					return  Int2Cplx( Refreshtime+1 );
				case 0xFFFFFFFB:	// Screen
					return  Int2Cplx( ScreenMode );
				case 0xFFFFFFFE:	// BackLight
					return	Int2Cplx( BackLight(-1) );
				case 0xFFFFFFF2:	// PopUpWin(
					return Int2Cplx( CB_PopUpWin(SRC) );
				case 0xFFFFFFDE:	// BatteryStatus
					return Int2Cplx( CB_BatteryStatus(SRC) );
				default:
					g_exec_ptr--;	// error
					break;
			}
			break;

		case 0xFFFFFF86 :	// sqr
			return Cplx_EvalFxDbl( &Cplx_fsqrt, Cplx_ListEvalsub5( SRC ) ) ; 
		case 0xFFFFFF95 :	// log10
			return Cplx_EvalFxDbl( &Cplx_flog10, Cplx_ListEvalsub5( SRC ) ) ; 
		case 0xFFFFFFB5 :	// 10^
			return Cplx_EvalFxDbl( &Cplx_fpow10, Cplx_ListEvalsub5( SRC ) ) ; 
		case 0xFFFFFF85 :	// ln
			return Cplx_EvalFxDbl( &Cplx_fln, Cplx_ListEvalsub5( SRC ) ) ; 
		case 0xFFFFFFA5 :	// expn
			return Cplx_EvalFxDbl( &Cplx_fexp, Cplx_ListEvalsub5( SRC ) ) ; 
		case 0xFFFFFF96 :	// cuberoot
			return Cplx_EvalFxDbl( &Cplx_fcuberoot, Cplx_ListEvalsub5( SRC ) ) ; 

		case 0xFFFFFF81 :	// sin
			return Cplx_EvalFxDbl( &Cplx_fsin, Cplx_ListEvalsub5( SRC ) ) ; 
		case 0xFFFFFF82 :	// cos
			return Cplx_EvalFxDbl( &Cplx_fcos, Cplx_ListEvalsub5( SRC ) ) ; 
		case 0xFFFFFF83 :	// tan
			return Cplx_EvalFxDbl( &Cplx_ftan, Cplx_ListEvalsub5( SRC ) ) ; 
		case 0xFFFFFF91 :	// asin
			return Cplx_EvalFxDbl( &Cplx_fasin, Cplx_ListEvalsub5( SRC ) ) ; 
		case 0xFFFFFF92 :	// acos
			return Cplx_EvalFxDbl( &Cplx_facos, Cplx_ListEvalsub5( SRC ) ) ; 
		case 0xFFFFFF93 :	// atan
			return Cplx_EvalFxDbl( &Cplx_fatan, Cplx_ListEvalsub5( SRC ) ) ; 
		case 0xFFFFFFA1 :	// sinh
			return Cplx_EvalFxDbl( &Cplx_fsinh, Cplx_ListEvalsub5( SRC ) ) ; 
		case 0xFFFFFFA2 :	// cosh
			return Cplx_EvalFxDbl( &Cplx_fcosh, Cplx_ListEvalsub5( SRC ) ) ; 
		case 0xFFFFFFA3 :	// tanh
			return Cplx_EvalFxDbl( &Cplx_ftanh, Cplx_ListEvalsub5( SRC ) ) ; 
		case 0xFFFFFFB1 :	// asinh
			return Cplx_EvalFxDbl( &Cplx_fasinh, Cplx_ListEvalsub5( SRC ) ) ; 
		case 0xFFFFFFB2 :	// acosh
			return Cplx_EvalFxDbl( &Cplx_facosh, Cplx_ListEvalsub5( SRC ) ) ; 
		case 0xFFFFFFB3 :	// atanh
			return Cplx_EvalFxDbl( &Cplx_fatanh, Cplx_ListEvalsub5( SRC ) ) ; 
			
		case 0xFFFFFF8D :	// integral
			return  Dbl2Cplx( CB_Integral( SRC ) );
			
		case 0xFFFFFF80 :	// Pol( x, y ) -> r=List Ans[1] , Theta=List Ans[2]
			tmp=Cplx_NoListEvalsubTop( SRC );
			if ( SRC[g_exec_ptr] != ',' ) { CB_Error(SyntaxERR) ; return Int2Cplx( 0 ); }	// Syntax error
			g_exec_ptr++;
			tmp2=Cplx_NoListEvalsubTop( SRC );
			if ( SRC[g_exec_ptr] == ')' ) g_exec_ptr++;
			Cplx_WriteListAns2( Cplx_fpolr(tmp,tmp2) , Cplx_fpolt(tmp,tmp2) ) ;
			return Int2Cplx( 0 );
		case 0xFFFFFFA0 :	// Rec( r, Theta ) -> X,Y
			tmp=Cplx_NoListEvalsubTop( SRC );
			if ( SRC[g_exec_ptr] != ',' ) { CB_Error(SyntaxERR) ; return Int2Cplx( 0 ); }	// Syntax error
			g_exec_ptr++;
			tmp2=Cplx_NoListEvalsubTop( SRC );
			if ( SRC[g_exec_ptr] == ')' ) g_exec_ptr++;
			Cplx_WriteListAns2( Cplx_frecx(tmp,tmp2) , Cplx_frecy(tmp,tmp2) ) ;
			return Int2Cplx( 0 );

		case 0xFFFFFFF9:	// F9..
			c = SRC[g_exec_ptr++];
			switch ( c ) {
				case 0xFFFFFFC6:	// M_PixelTest(
					return Int2Cplx( CB_ML_PixelTest( SRC ) );
				case 0xFFFFFFD8:	// M_Test
					return Int2Cplx( CB_MLTest( SRC ) );
//				case 0x53:	// M_Contrast(
//					return Int2Cplx( CB_ML_GetContrast( SRC ) );
				case 0x31:	// StrLen(
					return Int2Cplx( CB_StrLen( SRC ) );
				case 0x32:	// StrCmp(
					return Int2Cplx( CB_StrCmp( SRC ) );
				case 0x33:	// StrSrc(
					return Int2Cplx( CB_StrSrc( SRC ) );
				case 0x38:	// Exp(
					return CB_Cplx_EvalStr(SRC, 0 );
				case 0x50:	// StrAsc(
					return Int2Cplx( CB_StrAsc( SRC ) );
//				case 0x5F:	// IsError(
//					return Int2Cplx( CB_IsError(SRC) );
				case 0x60:	// GetFont(
					return Int2Cplx( CB_GetFont(SRC) );
				case 0x62:	// GetFontMini(
					return Int2Cplx( CB_GetFontMini(SRC) );
				case 0x21:	// Xdot
					return Dbl2Cplx( Xdot );
				case 0x1B :		// fn str
					return CB_Cplx_FnStr( SRC, 1 );
					
				case 0x4B:	// DotP(
					return Cplx_CB_DotP( SRC );
				case 0x4A:	// CrossP(
					Cplx_CB_CrossP( SRC );
					return Int2Cplx( 0 );
				case 0x6D:	// Angle(
					return Cplx_CB_AngleV( SRC );
				case 0x5E:	// UnitV(
					Cplx_CB_UnitV( SRC );
					return Int2Cplx( 0 );
				case 0x5B:	// Norm(
					return Cplx_CB_NormV( SRC );
					
				default:
					g_exec_ptr--;	// error
					break;
			}
			break;
		case 0xFFFFFFDD :	// Eng
			return Int2Cplx( ENG );
		default:
			break;
	}
	if ( c == '#') {
		result = Cplx_EvalsubTop( SRC );
		return result;
	} else
	if ( c==' ' ) { while ( c==' ' )c=SRC[g_exec_ptr++]; goto topj; }	// Skip Space
	
	g_exec_ptr--;
	reg=RegVarAliasEx( SRC ); if ( reg>=0 ) goto regj;	// variable alias
	CB_Error(SyntaxERR) ; // Syntax error 
	return Int2Cplx( 0 );
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

complex Cplx_ListEvalsub2(char *SRC) {	//  2nd Priority  ( type B function ) ...
	int cont=1;
	complex result,tmp;
	int c,i;
	int base;
	int resultreg,tmpreg;
	int resultflag=dspflag;		// 2:result	3:Listresult
	result = Cplx_ListEvalsub1( SRC );
	resultreg=CB_MatListAnsreg;
	while ( 1 ) {
		c = SRC[g_exec_ptr++];
		switch ( c ) {
			case  0xFFFFFF8B  :	// ^2
				if ( resultflag==3 ) {
						CopyMatList2Ans( resultreg );
						result = Cplx_EvalFxDbl2( &Cplx_fMUL, &resultflag, &resultreg, result, result ) ;
				} else	result = Cplx_EvalFxDbl( &Cplx_fsqu, result) ; 
				break;
			case  0xFFFFFF9B  :	// ^(-1) RECIP
				if ( resultflag==3 ) Cplx_Mat_inverse( resultreg );
				else				 result = Cplx_EvalFxDbl( &Cplx_frecip, result) ; 
				break;
			case  0xFFFFFFAB  :	//  !
				result = Cplx_EvalFxDbl( &Cplx_ffact, result) ; 
				break;
				
			case  0x01  :	//  femto
				result = Cplx_EvalFxDbl( &Cplx_ffemto, result) ; 
				break;
			case  0x02  :	//  pico
				result = Cplx_EvalFxDbl( &Cplx_fpico, result) ; 
				break;
			case  0x03  :	//  nano
				result = Cplx_EvalFxDbl( &Cplx_fnano, result) ; 
				break;
			case  0x04  :	//  micro
				result = Cplx_EvalFxDbl( &Cplx_fmicro, result) ; 
				break;
			case  0x05  :	//  milli
				result = Cplx_EvalFxDbl( &Cplx_fmilli, result) ; 
				break;
			case  0x06  :	//  Kiro
				result = Cplx_EvalFxDbl( &Cplx_fKiro, result) ; 
				break;
			case  0x07  :	//  Mega
				result = Cplx_EvalFxDbl( &Cplx_fMega, result) ; 
				break;
			case  0x08  :	//  Giga
				result = Cplx_EvalFxDbl( &Cplx_fGiga, result) ; 
				break;
			case  0x09  :	//  Tera
				result = Cplx_EvalFxDbl( &Cplx_fTera, result) ; 
				break;
			case  0x0A  :	//  Peta
				result = Cplx_EvalFxDbl( &Cplx_fPeta, result) ; 
				break;
			case  0x1B  :	//  Exa
				result = Cplx_EvalFxDbl( &Cplx_fExa, result) ; 
				break;
				
			case  0xFFFFFF8C  :	//  dms
				result.real = DmsToDec( SRC, result.real );
				result.imag = 0;
				break;
				
			case  0xFFFFFF9C  :	//  Deg
				result = Cplx_EvalFxDbl( &Cplx_finvdegree, result) ; 
				break;
			case  0xFFFFFFAC  :	//  Rad
				result = Cplx_EvalFxDbl( &Cplx_finvradian, result) ; 
				break;
			case  0xFFFFFFBC  :	//  Grad
				result = Cplx_EvalFxDbl( &Cplx_finvgrad, result) ; 
				break;
				
			case ' ':	// Skip Space
				break;
			default:
				g_exec_ptr--;
				return result;
				break;
		}
	 }
	return result;
}
complex Cplx_ListEvalsub3(char *SRC) {	//  3rd Priority  ( ^ ...)
	complex result;
	int c,i;
	int resultreg,resultreg2;
	int resultflag;
	int execptr;
	
	result = Cplx_ListEvalsub2( SRC );
	resultflag=dspflag;		// 2:result	3:Listresult
	resultreg=CB_MatListAnsreg;
	while ( 1 ) {
		c = SRC[g_exec_ptr++];
		switch ( c ) {
			case  0xFFFFFFA8  :	// a ^ b
				if ( resultflag==3 ) {	// Mat
					c = CB_EvalInt( SRC );
					if ( c== 1 ) break;
					else
					if ( c==-1 ) { Cplx_Mat_inverse( resultreg ); break; }
					else
					if ( c>= 1 ) {
						resultreg2=resultreg;
						CopyMatList2Ans( resultreg );	//	result -> new result2
						resultreg=CB_MatListAnsreg;
						for ( i=1; i<c; i++ ) {
							CopyMatList2Ans( resultreg2 );
							result = Cplx_EvalFxDbl2( &Cplx_fMUL, &resultflag, &resultreg, result, result ) ;
						}
						CopyMatrix( resultreg2, resultreg );		// resultreg -> resultreg2
						DeleteMatListAns();	// delete result2
					} else { CB_Error(MathERR); break ; }
				} else	result = Cplx_EvalFxDbl2( &Cplx_fpow, &resultflag, &resultreg, result, Cplx_ListEvalsub2( SRC ) ) ;
				break;
			case  0xFFFFFFB8  :	// powroot
				result = Cplx_EvalFxDbl2( &Cplx_fpowroot, &resultflag, &resultreg, result, Cplx_ListEvalsub2( SRC ) ) ;
				break;
			case ' ':	// Skip Space
				break;
			default:
				g_exec_ptr--;
				return result;
				break;
		}
	 }
	return result;
}

complex Cplx_ListEvalsub4(char *SRC) {	//  4th Priority  (Fraction) a/b/c
	complex result,frac1,frac2,frac3;
	unsigned int c;
	result = Cplx_ListEvalsub3( SRC );
	c = SRC[g_exec_ptr];
	if ( c == 0xFFFFFFBB ) {
		g_exec_ptr++;
		frac1 = result ;
		frac2 = Cplx_ListEvalsub3( SRC );
		c = SRC[g_exec_ptr];
		if ( c == 0xFFFFFFBB ) {
			g_exec_ptr++;
			frac3 = Cplx_ListEvalsub3( SRC );
			// _div_check(frac3.real);
			result = Cplx_fADD( frac1, Cplx_fDIV( frac2, frac3 ) );
		} else {
			// _div_check(frac2.real);
			result = Cplx_fDIV( frac1, frac2 );
		}
	}
	return result;
}
complex Cplx_ListEvalsub5(char *SRC) {	//  5th Priority abbreviated multiplication
	complex result;
	int c;
	int resultreg;
	int resultflag;
	int execptr;
	
	result = Cplx_ListEvalsub4( SRC );
	resultflag=dspflag;		// 2:result	3:Listresult
	resultreg=CB_MatListAnsreg;
	while ( 1 ) {
		c = SRC[g_exec_ptr];
		if ((( 'A'<=c )&&( c<='Z' )) ||
			(( 'a'<=c )&&( c<='z' )) ||
			 ( c == 0xFFFFFFCD ) || // <r>
			 ( c == 0xFFFFFFCE ) || // Theta
			 ( c == 0xFFFFFFD0 ) || // const_PI
			 ( c == 0xFFFFFFC0 ) || // Ans
			 ( c == 0xFFFFFFC1 ) || // Ran#
			 ( c == 0xFFFFFF8D )) { // integral
				result = Cplx_EvalFxDbl2( &Cplx_fMUL, &resultflag, &resultreg, result, Cplx_ListEvalsub4( SRC ) ) ;
		} else if ( c == 0x7F ) { // 7F..
				if ( g_error_type ) goto exitj;
				c = SRC[g_exec_ptr+1];
				if ( ( 0xFFFFFFB0 <= c ) && ( c <= 0xFFFFFFBD ) && ( c != 0xFFFFFFB3 ) ) goto exitj;	// And Or xor
				result = Cplx_EvalFxDbl2( &Cplx_fMUL, &resultflag, &resultreg, result, Cplx_ListEvalsub4( SRC ) ) ;
		} else if ( c == 0xFFFFFFF7 ) { // F7..
			c = SRC[g_exec_ptr+1];
			switch ( c ) {
				case 0xFFFFFFAF:	// PxlTest(y,x)
				result = Cplx_EvalFxDbl2( &Cplx_fMUL, &resultflag, &resultreg, result, Cplx_ListEvalsub4( SRC ) ) ;
					break;
				default:
					goto exitj;
					break;
			}
		} else if ( c == 0xFFFFFFF9 ) { // F9..
			c = SRC[g_exec_ptr+1];
			switch ( c ) {
				case 0x1B:	// fn
				case 0x21:	// Xdot
				case 0x31:	// StrLen(
				case 0x32:	// StrCmp(
				case 0x33:	// StrSrc(
				case 0x38:	// Exp(
				case 0x4B:	// DotP(
				result = Cplx_EvalFxDbl2( &Cplx_fMUL, &resultflag, &resultreg, result, Cplx_ListEvalsub4( SRC ) ) ;
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
			execptr=g_exec_ptr;
			c=RegVarAliasEx(SRC);
			if (c>0) { g_exec_ptr=execptr; result = Cplx_EvalFxDbl2( &Cplx_fMUL, &resultflag, &resultreg, result, Cplx_ListEvalsub4( SRC ) ) ; }
			else return result;
		}
	 }
}
complex Cplx_ListEvalsub7(char *SRC) {	//  7th Priority abbreviated multiplication type A/C
	complex result;
	int c;
	int resultreg;
	int resultflag;
	
	result = Cplx_ListEvalsub5( SRC );
	resultflag=dspflag;		// 2:result	3:Listresult
	resultreg=CB_MatListAnsreg;
	while ( 1 ) {
		c = SRC[g_exec_ptr];
		switch ( c ) {
			case '(' :
			case '{' :
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
				result = Cplx_EvalFxDbl2( &Cplx_fMUL, &resultflag, &resultreg, result, Cplx_ListEvalsub5( SRC ) ) ;
				break;
			default:
				return result;
				break;
		}
	 }
	return result;
}
complex Cplx_ListEvalsub8(char *SRC) {	//  8th Priority  ( nPr,nCr,/_ )
	complex result;
	int c;
	int resultreg;
	int resultflag;
	
	result = Cplx_ListEvalsub7( SRC );
	resultflag=dspflag;		// 2:result	3:Listresult
	resultreg=CB_MatListAnsreg;
	while ( 1 ) {
		c = SRC[g_exec_ptr++];
		switch ( c ) {
			case 0xFFFFFF88 :		// nPr
				result = Cplx_EvalFxDbl2( &Cplx_f_nPr, &resultflag, &resultreg, result, Cplx_ListEvalsub7( SRC ) ) ;
				break;
			case 0xFFFFFF98 :		// nCr
				result = Cplx_EvalFxDbl2( &Cplx_f_nCr, &resultflag, &resultreg, result, Cplx_ListEvalsub7( SRC ) ) ;
				break;
			case 0x7F:
				c = SRC[g_exec_ptr++];
				switch ( c ) {
					case 0x54:	// /_ Angle
						result = Cplx_EvalFxDbl2( &Cplx_fAngle, &resultflag, &resultreg, result, Cplx_ListEvalsub7( SRC ) ) ;
						break;
					default:
						g_exec_ptr-=2;
						return result;
						break;
				}
				break;
			case ' ':	// Skip Space
				break;
			default:
				g_exec_ptr--;
				return result;
				break;
		}
	 }
	return result;
}
complex Cplx_ListEvalsub10(char *SRC) {	//  10th Priority  ( *,/, int.,Rmdr )
	complex result;
	int c;
	int resultreg;
	int resultflag;
	
	result = Cplx_ListEvalsub8( SRC );
	resultflag=dspflag;		// 2:result	3:Listresult
	resultreg=CB_MatListAnsreg;
	while ( 1 ) {
		c = SRC[g_exec_ptr++];
		switch ( c ) {
			case 0xFFFFFFA9 :		// ×
				result = Cplx_EvalFxDbl2( &Cplx_fMUL, &resultflag, &resultreg, result, Cplx_ListEvalsub8( SRC ) ) ;
				break;
			case 0xFFFFFFB9 :		// ÷
				result = Cplx_EvalFxDbl2( &Cplx_fDIV, &resultflag, &resultreg, result, Cplx_ListEvalsub8( SRC ) ) ;
				break;
			case 0x7F:
				c = SRC[g_exec_ptr++];
				switch ( c ) {
					case 0xFFFFFFBC:	// Int÷
						result = Cplx_EvalFxDbl2( &Cplx_fIDIV, &resultflag, &resultreg, result, Cplx_ListEvalsub8( SRC ) ) ;
						break;
					case 0xFFFFFFBD:	// Rmdr
						result = Cplx_EvalFxDbl2( &Cplx_fMOD, &resultflag, &resultreg, result, Cplx_ListEvalsub8( SRC ) ) ;
						break;
					default:
						g_exec_ptr-=2;
						return result;
						break;
				}
				break;
			case ' ':	// Skip Space
				break;
			default:
				g_exec_ptr--;
				return result;
				break;
		}
	 }
	return result;
}
complex Cplx_ListEvalsub11(char *SRC) {	//  11th Priority  ( +,- )
	complex result;
	int c;
	int resultreg;
	int resultflag;

	result = Cplx_ListEvalsub10( SRC );
	resultflag=dspflag;		// 2:result	3:Listresult
	resultreg=CB_MatListAnsreg;
	while ( 1 ) {
		c = SRC[g_exec_ptr++];
		switch ( c ) {
			case 0xFFFFFF89 :		// +
				result = Cplx_EvalFxDbl2( &Cplx_fADD, &resultflag, &resultreg, result, Cplx_ListEvalsub10( SRC ) ) ;
				break;
			case 0xFFFFFF99 :		// -
				result = Cplx_EvalFxDbl2( &Cplx_fSUB, &resultflag, &resultreg, result, Cplx_ListEvalsub10( SRC ) ) ;
				break;
			case ' ':	// Skip Space
				break;
			default:
				g_exec_ptr--;
				return result;
				break;
		}
	 }
	return result;
}
complex Cplx_ListEvalsub12(char *SRC) {	//  12th Priority ( =,!=,><,>=,<= )
	complex result;
	int c;
	int resultreg;
	int resultflag;
	
	result = Cplx_ListEvalsub11( SRC );
	resultflag=dspflag;		// 2:result	3:Listresult
	resultreg=CB_MatListAnsreg;
	while ( 1 ) {
		c = SRC[g_exec_ptr++];
		switch ( c ) {
			case '=' :	// =
				result = Cplx_EvalFxDbl2( &Cplx_fcmpEQ, &resultflag, &resultreg, result, Cplx_ListEvalsub11( SRC ) ) ;
				break;
			case '>' :	// >
				result = Cplx_EvalFxDbl2( &Cplx_fcmpGT, &resultflag, &resultreg, result, Cplx_ListEvalsub11( SRC ) ) ;
				break;
			case '<' :	// <
				result = Cplx_EvalFxDbl2( &Cplx_fcmpLT, &resultflag, &resultreg, result, Cplx_ListEvalsub11( SRC ) ) ;
				break;
			case 0x11 :	// !=
				result = Cplx_EvalFxDbl2( &Cplx_fcmpNE, &resultflag, &resultreg, result, Cplx_ListEvalsub11( SRC ) ) ;
				break;
			case 0x12 :	// >=
				result = Cplx_EvalFxDbl2( &Cplx_fcmpGE, &resultflag, &resultreg, result, Cplx_ListEvalsub11( SRC ) ) ;
				break;
			case 0x10 :	// <=
				result = Cplx_EvalFxDbl2( &Cplx_fcmpLE, &resultflag, &resultreg, result, Cplx_ListEvalsub11( SRC ) ) ;
				break;
			case 0xFFFFFF9A :	// xor
				result = Cplx_EvalFxDbl2( &Cplx_fXOR, &resultflag, &resultreg, result, Cplx_ListEvalsub11( SRC ) ) ;
				break;
			case '|' :	// or
			case 0xFFFFFFAA :	// or
				result = Cplx_EvalFxDbl2( &Cplx_fOR, &resultflag, &resultreg, result, Cplx_ListEvalsub11( SRC ) ) ;
				break;
			case '&' :	// and
			case 0xFFFFFFBA :	// and
				result = Cplx_EvalFxDbl2( &Cplx_fAND, &resultflag, &resultreg, result, Cplx_ListEvalsub11( SRC ) ) ;
				break;
			case ' ':	// Skip Space
				break;
			default:
				g_exec_ptr--;
				return result;
				break;
		}
	 }
	return result;
}
complex Cplx_ListEvalsub13(char *SRC) {	//  13th Priority  ( And,and)
	complex result;
	int c;
	int resultreg;
	int resultflag;
	
	result = Cplx_ListEvalsub12( SRC );
	resultflag=dspflag;		// 2:result	3:Listresult
	resultreg=CB_MatListAnsreg;
	while ( 1 ) {
		c = SRC[g_exec_ptr];
		if ( c == 0x7F ) {
			c = SRC[g_exec_ptr+1];
			switch ( c ) {
				case 0xFFFFFFB0 :	// And
					g_exec_ptr+=2;
					result = Cplx_EvalFxDbl2( &Cplx_fAND_logic, &resultflag, &resultreg, result, Cplx_ListEvalsub12( SRC ) ) ;
					break;
				default:
					return result;
					break;
			}
		} else
		if ( c == ' ' ) g_exec_ptr++;	// Skip Space
		else return result;
	}
}


complex Cplx_ListEvalsubTop(char *SRC) {	//  
	complex result,dst;
	int c,i;
	int resultreg;
	int resultflag;
	
	result = Cplx_ListEvalsub13( SRC );
	resultflag=dspflag;		// 2:result	3:Listresult
	resultreg=CB_MatListAnsreg;
	while ( 1 ) {
		c = SRC[g_exec_ptr];
		if ( c == 0x7F ) {
			c = SRC[g_exec_ptr+1];
			switch ( c ) {
				case 0xFFFFFFB1 :	// Or
					g_exec_ptr+=2;
					result = Cplx_EvalFxDbl2( &Cplx_fOR_logic, &resultflag, &resultreg, result, Cplx_ListEvalsub13( SRC ) ) ;
					break;
				case 0xFFFFFFB4 :	// Xor
					g_exec_ptr+=2;
					result = Cplx_EvalFxDbl2( &Cplx_fXOR_logic, &resultflag, &resultreg, result, Cplx_ListEvalsub13( SRC ) ) ;
					break;
				default:
					return result;
					break;
			}
		} else
		if ( c == ' ' ) g_exec_ptr++;	// Skip Space
		else return result;
	}
}

complex Cplx_NoListEvalsubTop(char *SRC) {	//  
	complex result,tmp;
	int c,i;
	int base;

	result = Cplx_EvalsubTop( SRC );
	if ( dspflag >= 3 ) { CB_Error(ArgumentERR); return result; } // Argument error
	return result;
}
int Cplx_ListEvalsub1Ans(char *SRC) {	//  
	complex result,tmp;
	int c,i;
	int base;
	int resultreg,tmpreg;

	result = Cplx_ListEvalsub1( SRC );
	resultreg=CB_MatListAnsreg;
	if ( dspflag < 3 ) { CB_Error(ArgumentERR); return -1; } // Argument error

	if ( SRC[g_exec_ptr] != 0x0E ) CopyMatList2AnsTop( resultreg );	// ListResult -> List Ans top
	return 0;
}


int Cplx_ListEvalsubTopAns(char *SRC) {	//  
	int result,tmp;
	int c,i;
	int base;
	int resultreg,tmpreg;

	if ( CB_MatListAnsreg >=28 ) CB_MatListAnsreg=28;
	g_error_type = 0;
	Cplx_ListEvalsubTop( SRC );
	resultreg=CB_MatListAnsreg;
	if ( dspflag < 3 ) { CB_Error(ArgumentERR); return -1; } // Argument error

	if ( SRC[g_exec_ptr] != 0x0E ) CopyMatList2AnsTop( resultreg );	// ListResult -> List Ans top
	MatdspNo=CB_MatListAnsreg;
	return dspflag;
}

