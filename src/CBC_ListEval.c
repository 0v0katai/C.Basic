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
				if ( ErrorNo ) return Int2Cplx(0); // error
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
				if ( ErrorNo ) return Int2Cplx(0); // error
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

	c = (unsigned char)SRC[ExecPtr++];
  topj:
	if ( c == '(') {
		result = Cplx_ListEvalsubTop( SRC );
		if ( (unsigned char)SRC[ExecPtr] == ')' ) ExecPtr++;
		return result;
	}
	while ( c == 0x00000089 ) c = (unsigned char)SRC[ExecPtr++];	// +
	if ( ( c == 0x00000087 ) || ( c == 0x00000099 ) ) {	//  -
		return Cplx_EvalFxDbl( &Cplx_fsign, Cplx_ListEvalsub5( SRC ) ) ; 
	}
	if ( ( ( 'A'<=c )&&( c<='Z' ) ) || ( ( 'a'<=c )&&( c<='z' ) ) )  {
		reg=c-'A';
	  regj:
		c = (unsigned char)SRC[ExecPtr];
		if ( c=='%' ) { ExecPtr++; return Int2Cplx( LocalInt[reg][0] ); }
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
		if (CB_INT==1) return Int2Cplx( LocalInt[reg][0] ); else return LocalDbl[reg][0] ;
	}
	if ( ( c=='.' ) ||( c==0x0F ) || ( ( '0'<=c )&&( c<='9' ) ) ) {
		ExecPtr--;
		result.real = Eval_atof( SRC , c );
		return result;
	}
	
	switch ( c ) { 			// ( type C function )  sin cos tan... 
		case 0x7F:	// 7F..
			c = (unsigned char)SRC[ExecPtr++];
			switch ( c ) {
				case 0x40 :		// Mat A[a,b]
				  Matjmp:
					reg=MatRegVar(SRC); if ( reg<0 ) CB_Error(SyntaxERR) ; // Syntax error 
					Matrix1:	
					if ( (unsigned char)SRC[ExecPtr] == '[' ) {
					Matrix:	
						ExecPtr++;
						MatOprand2( SRC, reg, &dimA, &dimB );
					Matrix2:
						if ( ErrorNo ) return Int2Cplx( 1 ) ; // error
					} else { dspflag=3;	// Mat A
							dimA=MatAry[reg].Base; dimB=dimA;
							CopyMatList2Ans( reg );
					}
					return Cplx_ReadMatrix( reg, dimA, dimB);
						
				case 0x00000084 :	// Vct A[a,b]
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
					if ( (unsigned char)SRC[ExecPtr] == '[' ) {
						ExecPtr++;
						MatOprand1( SRC, reg, &dimA, &dimB );	// List 1[a]
						if ( ErrorNo ) return Int2Cplx( 1 ) ; // error
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
					if ( (unsigned char)SRC[ExecPtr] != ',' ) CB_Error(SyntaxERR) ; // Syntax error 
					ExecPtr++;
					result = Cplx_EvalFxDbl2( &Cplx_fMOD, &resultflag, &resultreg, result, Cplx_ListEvalsubTop( SRC ) ) ;
					if ( (unsigned char)SRC[ExecPtr] == ')' ) ExecPtr++;
					return result ;
						
				case 0x3C :		// GCD(a,b)
					result = Cplx_ListEvalsubTop( SRC );
					resultflag=dspflag;		// 2:result	3:Listresult
					resultreg=CB_MatListAnsreg;
					if ( (unsigned char)SRC[ExecPtr] != ',' ) CB_Error(SyntaxERR) ; // Syntax error 
					ExecPtr++;
					result = Cplx_EvalFxDbl2( &Cplx_fGCD, &resultflag, &resultreg, result, Cplx_ListEvalsubTop( SRC ) ) ;
					if ( (unsigned char)SRC[ExecPtr] == ')' ) ExecPtr++;
					return result ;
						
				case 0x3D :		// LCM(a,b)
					result = Cplx_ListEvalsubTop( SRC );
					resultflag=dspflag;		// 2:result	3:Listresult
					resultreg=CB_MatListAnsreg;
					if ( (unsigned char)SRC[ExecPtr] != ',' ) CB_Error(SyntaxERR) ; // Syntax error 
					ExecPtr++;
					result = Cplx_EvalFxDbl2( &Cplx_fLCM, &resultflag, &resultreg, result, Cplx_ListEvalsubTop( SRC ) ) ;
					if ( (unsigned char)SRC[ExecPtr] == ')' ) ExecPtr++;
					return result ;
						
				case 0x00000085 :		// logab(a,b)
					result = Cplx_ListEvalsubTop( SRC );
					resultflag=dspflag;		// 2:result	3:Listresult
					resultreg=CB_MatListAnsreg;
					if ( (unsigned char)SRC[ExecPtr] != ',' ) CB_Error(SyntaxERR) ; // Syntax error 
					ExecPtr++;
					result = Cplx_EvalFxDbl2( &Cplx_flogab, &resultflag, &resultreg, result, Cplx_ListEvalsubTop( SRC ) ) ;
					if ( (unsigned char)SRC[ExecPtr] == ')' ) ExecPtr++;
					return result ;

				case 0x000000B3 :		// Not
					return Cplx_EvalFxDbl( &Cplx_fNot_logic, Cplx_ListEvalsub5( SRC ) ) ; 
						
				case 0x0000009F :		// KeyRow(
					return Int2Cplx( CB_KeyRow( SRC ) ) ; 
				case 0x0000008F :		// Getkey
					return Int2Cplx( CB_GetkeyEntry( SRC ) );
					
				case 0x00000087 :		// RanInt#(st,en[,n])
					x=Cplx_NoListEvalsubTop( SRC ).real;
					if ( (unsigned char)SRC[ExecPtr] != ',' ) CB_Error(SyntaxERR) ; // Syntax error 
					ExecPtr++ ;	// ',' skip
					y=Cplx_NoListEvalsubTop( SRC ).real;
					if ( (unsigned char)SRC[ExecPtr] == ',' ) {
						ExecPtr++;
						CB_RanInt( SRC, x, y );
					}
					if ( (unsigned char)SRC[ExecPtr] == ')' ) ExecPtr++;
					return Dbl2Cplx( frandIntint( x, y ) );
					
				case 0x00000088 :		// RanList#(n) ->ListAns
					CB_RanList( SRC ) ;
					return Dbl2Cplx( 4 );
				case 0x00000089 :		// RanBin#(n,p[,m]) ->ListAns
					return Dbl2Cplx( CB_RanBin( SRC ) );
				case 0x0000008A :		// RanNorm#(sd,mean[,n]) ->ListAns
					return Dbl2Cplx( CB_RanNorm( SRC ) );
						
				case 0x000000E9 :		// CellSum(Mat A[x,y])
					MatrixOprand( SRC, &reg, &x, &y );
					if ( ErrorNo ) return Int2Cplx( 1 ); // error
					if ( (unsigned char)SRC[ExecPtr] == ')' ) ExecPtr++;
					return Int2Cplx( Cellsum( reg, x, y ) );
	
				case 0x5F :				// 1/128 Ticks
					return Int2Cplx( CB_Ticks( SRC ) );	// 
						
				case 0x00000086 :		// RndFix(n,digit)
					result = Cplx_ListEvalsubTop( SRC );
					resultflag=dspflag;		// 2:result	3:Listresult
					resultreg=CB_MatListAnsreg;
					if ( (unsigned char)SRC[ExecPtr] != ',' ) CB_Error(SyntaxERR) ; // Syntax error 
					if ( (unsigned char)SRC[++ExecPtr] == 0x000000E4 ) { ExecPtr++; i=(int)&Cplx_RoundSci; } else i=(int)&Cplx_RoundFix;
					result = Cplx_EvalFxDbl2( (CFXPTR2)i, &resultflag, &resultreg, result, Cplx_ListEvalsubTop( SRC ) ) ;
					if ( (unsigned char)SRC[ExecPtr] == ')' ) ExecPtr++;
					return result ;
					
				case 0x000000F0 :		// GraphY str
				case 0x000000F1:		// Graphr
				case 0x000000F2:		// GraphXt
				case 0x000000F3:		// GraphYt
				case 0x000000F4:		// GraphX
					return CB_Cplx_GraphYStr( SRC, 1 );
					
				case 0x000000F5 :		// IsExist(
					return  Int2Cplx( CB_IsExist( SRC, 1 ) );
				case 0x000000F6 :		// Peek(
					return  CB_Peek( SRC, Cplx_NoListEvalsubTop( SRC ).real );
				case 0x000000F8 :		// VarPtr(
					return  Int2Cplx( CB_VarPtr( SRC ) );
				case 0x000000FA :		// ProgPtr(
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
					ExecPtr--;	// error
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
					
				case 0x000000CF :				// System(
					return Int2Cplx( CB_System( SRC ) );
				case 0x000000DF :				// Version
					return Int2Cplx( CB_Version() );		//

				case 0x00000090 :				// F Result
					CB_F_Result( SRC );
				case 0x00000091 :				// F Start
				case 0x00000092 :				// F End
				case 0x00000093 :				// F pitch
					return Dbl2Cplx( REGf[c-0x00000090] );

				default:
					ExecPtr--;	// error
					break;
			}
			break;
		case '&' :	// & VarPtr
			return Int2Cplx( CB_VarPtr( SRC ) );
		case 0x000000D0 :	// const_PI
			return Dbl2Cplx( const_PI );
		case 0x000000C1 :	// Ran#
			return Dbl2Cplx( CB_frand( SRC ) );
		case 0x00000097 :	// abs
			return Cplx_EvalFxDbl( &Cplx_fabs, Cplx_ListEvalsub5( SRC ) ) ; 
		case 0x000000A6 :	// int
			return Cplx_EvalFxDbl( &Cplx_fint, Cplx_ListEvalsub5( SRC ) ) ; 
		case 0x000000DE :	// intg
			return Cplx_EvalFxDbl( &Cplx_intg, Cplx_ListEvalsub5( SRC ) ) ; 
		case 0x000000B6 :	// frac
			return Cplx_EvalFxDbl( &Cplx_frac, Cplx_ListEvalsub5( SRC ) ) ; 
		case 0x000000A7 :	// not
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

		case 0x000000F7:	// F7..
			c = (unsigned char)SRC[ExecPtr++];
			switch ( c ) {
				case 0x000000AF:	// PxlTest(y,x)
					y=Cplx_NoListEvalsubTop( SRC ).real;
					if ( (unsigned char)SRC[ExecPtr] != ',' ) CB_Error(SyntaxERR) ; // Syntax error 
					ExecPtr++ ;	// ',' skip
					x=Cplx_NoListEvalsubTop( SRC ).real;
					if ( (unsigned char)SRC[ExecPtr] == ')' ) ExecPtr++;
					return Int2Cplx( PxlTest(y, x) );			// 
				case 0x000000B0 :				// SortA( List 1)
					return Int2Cplx( CB_EvalSortAD( SRC, 1 ) );
				case 0x000000B1 :				// SortD( List 1)
					return Int2Cplx( CB_EvalSortAD( SRC, 0 ) );
				case 0x000000F4:	// SysCall(
					return  Int2Cplx( CB_SysCall( SRC ) );
				case 0x000000F5:	// Call(
					return  Int2Cplx( CB_Call( SRC ) );
				case 0x000000F8:	// RefreshCtrl
					return  Int2Cplx( RefreshCtrl );
				case 0x000000FA:	// RefreshTime
					return  Int2Cplx( Refreshtime+1 );
				case 0x000000FB:	// Screen
					return  Int2Cplx( ScreenMode );
				case 0x000000FE:	// BackLight
					return	Int2Cplx( BackLight(-1) );
				case 0x000000F2:	// PopUpWin(
					return Int2Cplx( CB_PopUpWin(SRC) );
				case 0x000000DE:	// BatteryStatus
					return Int2Cplx( CB_BatteryStatus(SRC) );
				default:
					ExecPtr--;	// error
					break;
			}
			break;

		case 0x00000086 :	// sqr
			return Cplx_EvalFxDbl( &Cplx_fsqrt, Cplx_ListEvalsub5( SRC ) ) ; 
		case 0x00000095 :	// log10
			return Cplx_EvalFxDbl( &Cplx_flog10, Cplx_ListEvalsub5( SRC ) ) ; 
		case 0x000000B5 :	// 10^
			return Cplx_EvalFxDbl( &Cplx_fpow10, Cplx_ListEvalsub5( SRC ) ) ; 
		case 0x00000085 :	// ln
			return Cplx_EvalFxDbl( &Cplx_fln, Cplx_ListEvalsub5( SRC ) ) ; 
		case 0x000000A5 :	// expn
			return Cplx_EvalFxDbl( &Cplx_fexp, Cplx_ListEvalsub5( SRC ) ) ; 
		case 0x00000096 :	// cuberoot
			return Cplx_EvalFxDbl( &Cplx_fcuberoot, Cplx_ListEvalsub5( SRC ) ) ; 

		case 0x00000081 :	// sin
			return Cplx_EvalFxDbl( &Cplx_fsin, Cplx_ListEvalsub5( SRC ) ) ; 
		case 0x00000082 :	// cos
			return Cplx_EvalFxDbl( &Cplx_fcos, Cplx_ListEvalsub5( SRC ) ) ; 
		case 0x00000083 :	// tan
			return Cplx_EvalFxDbl( &Cplx_ftan, Cplx_ListEvalsub5( SRC ) ) ; 
		case 0x00000091 :	// asin
			return Cplx_EvalFxDbl( &Cplx_fasin, Cplx_ListEvalsub5( SRC ) ) ; 
		case 0x00000092 :	// acos
			return Cplx_EvalFxDbl( &Cplx_facos, Cplx_ListEvalsub5( SRC ) ) ; 
		case 0x00000093 :	// atan
			return Cplx_EvalFxDbl( &Cplx_fatan, Cplx_ListEvalsub5( SRC ) ) ; 
		case 0x000000A1 :	// sinh
			return Cplx_EvalFxDbl( &Cplx_fsinh, Cplx_ListEvalsub5( SRC ) ) ; 
		case 0x000000A2 :	// cosh
			return Cplx_EvalFxDbl( &Cplx_fcosh, Cplx_ListEvalsub5( SRC ) ) ; 
		case 0x000000A3 :	// tanh
			return Cplx_EvalFxDbl( &Cplx_ftanh, Cplx_ListEvalsub5( SRC ) ) ; 
		case 0x000000B1 :	// asinh
			return Cplx_EvalFxDbl( &Cplx_fasinh, Cplx_ListEvalsub5( SRC ) ) ; 
		case 0x000000B2 :	// acosh
			return Cplx_EvalFxDbl( &Cplx_facosh, Cplx_ListEvalsub5( SRC ) ) ; 
		case 0x000000B3 :	// atanh
			return Cplx_EvalFxDbl( &Cplx_fatanh, Cplx_ListEvalsub5( SRC ) ) ; 
			
		case 0x0000008D :	// integral
			return  Dbl2Cplx( CB_Integral( SRC ) );
			
		case 0x00000080 :	// Pol( x, y ) -> r=List Ans[1] , Theta=List Ans[2]
			tmp=Cplx_NoListEvalsubTop( SRC );
			if ( (unsigned char)SRC[ExecPtr] != ',' ) { CB_Error(SyntaxERR) ; return Int2Cplx( 0 ); }	// Syntax error
			ExecPtr++;
			tmp2=Cplx_NoListEvalsubTop( SRC );
			if ( (unsigned char)SRC[ExecPtr] == ')' ) ExecPtr++;
			Cplx_WriteListAns2( Cplx_fpolr(tmp,tmp2) , Cplx_fpolt(tmp,tmp2) ) ;
			return Int2Cplx( 0 );
		case 0x000000A0 :	// Rec( r, Theta ) -> X,Y
			tmp=Cplx_NoListEvalsubTop( SRC );
			if ( (unsigned char)SRC[ExecPtr] != ',' ) { CB_Error(SyntaxERR) ; return Int2Cplx( 0 ); }	// Syntax error
			ExecPtr++;
			tmp2=Cplx_NoListEvalsubTop( SRC );
			if ( (unsigned char)SRC[ExecPtr] == ')' ) ExecPtr++;
			Cplx_WriteListAns2( Cplx_frecx(tmp,tmp2) , Cplx_frecy(tmp,tmp2) ) ;
			return Int2Cplx( 0 );

		case 0x000000F9:	// F9..
			c = (unsigned char)SRC[ExecPtr++];
			switch ( c ) {
				case 0x000000C6:	// M_PixelTest(
					return Int2Cplx( CB_ML_PixelTest( SRC ) );
				case 0x000000D8:	// M_Test
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
					ExecPtr--;	// error
					break;
			}
			break;
		case 0x000000DD :	// Eng
			return Int2Cplx( ENG );
		default:
			break;
	}
	if ( c == '#') {
		result = Cplx_EvalsubTop( SRC );
		return result;
	} else
	if ( c==' ' ) { while ( c==' ' )c = (unsigned char)SRC[ExecPtr++]; goto topj; }	// Skip Space
	
	ExecPtr--;
	reg=RegVarAliasEx( SRC ); if ( reg>=0 ) goto regj;	// variable alias
	CB_Error(SyntaxERR) ; // Syntax error 
	return Int2Cplx( 0 );
}

//-----------------------------------------------------------------------------
//int Cplx_ListEvalObjectAlignE4g( unsigned int n ){ return n ; }	// align +4byte
//int Cplx_ListEvalObjectAlignE4h( unsigned int n ){ return n+n; }	// align +6byte
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
		c = (unsigned char)SRC[ExecPtr++];
		switch ( c ) {
			case  0x0000008B  :	// ^2
				if ( resultflag==3 ) {
						CopyMatList2Ans( resultreg );
						result = Cplx_EvalFxDbl2( &Cplx_fMUL, &resultflag, &resultreg, result, result ) ;
				} else	result = Cplx_EvalFxDbl( &Cplx_fsqu, result) ; 
				break;
			case  0x0000009B  :	// ^(-1) RECIP
				if ( resultflag==3 ) Cplx_Mat_inverse( resultreg );
				else				 result = Cplx_EvalFxDbl( &Cplx_frecip, result) ; 
				break;
			case  0x000000AB  :	//  !
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
				
			case  0x0000008C  :	//  dms
				result.real = DmsToDec( SRC, result.real );
				result.imag = 0;
				break;
				
			case  0x0000009C  :	//  Deg
				result = Cplx_EvalFxDbl( &Cplx_finvdegree, result) ; 
				break;
			case  0x000000AC  :	//  Rad
				result = Cplx_EvalFxDbl( &Cplx_finvradian, result) ; 
				break;
			case  0x000000BC  :	//  Grad
				result = Cplx_EvalFxDbl( &Cplx_finvgrad, result) ; 
				break;
				
			case ' ':	// Skip Space
				break;
			default:
				ExecPtr--;
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
		c = (unsigned char)SRC[ExecPtr++];
		switch ( c ) {
			case  0x000000A8  :	// a ^ b
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
			case  0x000000B8  :	// powroot
				result = Cplx_EvalFxDbl2( &Cplx_fpowroot, &resultflag, &resultreg, result, Cplx_ListEvalsub2( SRC ) ) ;
				break;
			case ' ':	// Skip Space
				break;
			default:
				ExecPtr--;
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
	c = (unsigned char)SRC[ExecPtr];
	if ( c == 0x000000BB ) {
		ExecPtr++;
		frac1 = result ;
		frac2 = Cplx_ListEvalsub3( SRC );
		c = (unsigned char)SRC[ExecPtr];
		if ( c == 0x000000BB ) {
			ExecPtr++;
			frac3 = Cplx_ListEvalsub3( SRC );
			if ( frac3.real == 0 ) CB_Error(DivisionByZeroERR); // Division by zero error 
			result = Cplx_fADD( frac1, Cplx_fDIV( frac2, frac3 ) );
		} else {
			if ( frac2.real == 0 ) CB_Error(DivisionByZeroERR); // Division by zero error 
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
		c = (unsigned char)SRC[ExecPtr];
		if ((( 'A'<=c )&&( c<='Z' )) ||
			(( 'a'<=c )&&( c<='z' )) ||
			 ( c == 0x000000CD ) || // <r>
			 ( c == 0x000000CE ) || // Theta
			 ( c == 0x000000D0 ) || // const_PI
			 ( c == 0x000000C0 ) || // Ans
			 ( c == 0x000000C1 ) || // Ran#
			 ( c == 0x0000008D )) { // integral
				result = Cplx_EvalFxDbl2( &Cplx_fMUL, &resultflag, &resultreg, result, Cplx_ListEvalsub4( SRC ) ) ;
		} else if ( c == 0x7F ) { // 7F..
				if ( ErrorNo ) goto exitj;
				c = (unsigned char)SRC[ExecPtr+1];
				if ( ( 0x000000B0 <= c ) && ( c <= 0x000000BD ) && ( c != 0x000000B3 ) ) goto exitj;	// And Or xor
				result = Cplx_EvalFxDbl2( &Cplx_fMUL, &resultflag, &resultreg, result, Cplx_ListEvalsub4( SRC ) ) ;
		} else if ( c == 0x000000F7 ) { // F7..
			c = (unsigned char)SRC[ExecPtr+1];
			switch ( c ) {
				case 0x000000AF:	// PxlTest(y,x)
				result = Cplx_EvalFxDbl2( &Cplx_fMUL, &resultflag, &resultreg, result, Cplx_ListEvalsub4( SRC ) ) ;
					break;
				default:
					goto exitj;
					break;
			}
		} else if ( c == 0x000000F9 ) { // F9..
			c = (unsigned char)SRC[ExecPtr+1];
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
//		} else if ( c == 0x000000E7 ) { // E7..
//			c = (unsigned char)SRC[ExecPtr+1];
//			switch ( c ) {
//				default:
//					return result;
//					break;
//			}
		} else {
		  exitj:
			execptr=ExecPtr;
			c=RegVarAliasEx(SRC);
			if (c>0) { ExecPtr=execptr; result = Cplx_EvalFxDbl2( &Cplx_fMUL, &resultflag, &resultreg, result, Cplx_ListEvalsub4( SRC ) ) ; }
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
		c = (unsigned char)SRC[ExecPtr];
		switch ( c ) {
			case '(' :
			case '{' :
			case 0x00000097 :	// abs
			case 0x000000A6 :	// int
			case 0x000000DE :	// intg
			case 0x000000B6 :	// frac
			case 0x00000085 :	// ln
			case 0x00000086 :	// sqr
			case 0x00000095 :	// log10
			case 0x00000096 :	// cuberoot
			case 0x000000A5 :	// expn
			case 0x000000A7 :	// Not
			case 0x000000B5 :	// 10^
			case 0x000000B7 :	// Neg
			case 0x00000081 :	// sin
			case 0x00000082 :	// cos
			case 0x00000083 :	// tan
			case 0x00000091 :	// asin
			case 0x00000092 :	// acos
			case 0x00000093 :	// atan
			case 0x000000A1 :	// sinh
			case 0x000000A2 :	// cosh
			case 0x000000A3 :	// tanh
			case 0x000000B1 :	// asinh
			case 0x000000B2 :	// acosh
			case 0x000000B3 :	// atanh
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
		c = (unsigned char)SRC[ExecPtr++];
		switch ( c ) {
			case 0x00000088 :		// nPr
				result = Cplx_EvalFxDbl2( &Cplx_f_nPr, &resultflag, &resultreg, result, Cplx_ListEvalsub7( SRC ) ) ;
				break;
			case 0x00000098 :		// nCr
				result = Cplx_EvalFxDbl2( &Cplx_f_nCr, &resultflag, &resultreg, result, Cplx_ListEvalsub7( SRC ) ) ;
				break;
			case 0x7F:
				c = (unsigned char)SRC[ExecPtr++];
				switch ( c ) {
					case 0x54:	// /_ Angle
						result = Cplx_EvalFxDbl2( &Cplx_fAngle, &resultflag, &resultreg, result, Cplx_ListEvalsub7( SRC ) ) ;
						break;
					default:
						ExecPtr-=2;
						return result;
						break;
				}
				break;
			case ' ':	// Skip Space
				break;
			default:
				ExecPtr--;
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
		c = (unsigned char)SRC[ExecPtr++];
		switch ( c ) {
			case 0x000000A9 :		// �~
				result = Cplx_EvalFxDbl2( &Cplx_fMUL, &resultflag, &resultreg, result, Cplx_ListEvalsub8( SRC ) ) ;
				break;
			case 0x000000B9 :		// ��
				result = Cplx_EvalFxDbl2( &Cplx_fDIV, &resultflag, &resultreg, result, Cplx_ListEvalsub8( SRC ) ) ;
				break;
			case 0x7F:
				c = (unsigned char)SRC[ExecPtr++];
				switch ( c ) {
					case 0x000000BC:	// Int��
						result = Cplx_EvalFxDbl2( &Cplx_fIDIV, &resultflag, &resultreg, result, Cplx_ListEvalsub8( SRC ) ) ;
						break;
					case 0x000000BD:	// Rmdr
						result = Cplx_EvalFxDbl2( &Cplx_fMOD, &resultflag, &resultreg, result, Cplx_ListEvalsub8( SRC ) ) ;
						break;
					default:
						ExecPtr-=2;
						return result;
						break;
				}
				break;
			case ' ':	// Skip Space
				break;
			default:
				ExecPtr--;
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
		c = (unsigned char)SRC[ExecPtr++];
		switch ( c ) {
			case 0x00000089 :		// +
				result = Cplx_EvalFxDbl2( &Cplx_fADD, &resultflag, &resultreg, result, Cplx_ListEvalsub10( SRC ) ) ;
				break;
			case 0x00000099 :		// -
				result = Cplx_EvalFxDbl2( &Cplx_fSUB, &resultflag, &resultreg, result, Cplx_ListEvalsub10( SRC ) ) ;
				break;
			case ' ':	// Skip Space
				break;
			default:
				ExecPtr--;
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
		c = (unsigned char)SRC[ExecPtr++];
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
			case 0x0000009A :	// xor
				result = Cplx_EvalFxDbl2( &Cplx_fXOR, &resultflag, &resultreg, result, Cplx_ListEvalsub11( SRC ) ) ;
				break;
			case '|' :	// or
			case 0x000000AA :	// or
				result = Cplx_EvalFxDbl2( &Cplx_fOR, &resultflag, &resultreg, result, Cplx_ListEvalsub11( SRC ) ) ;
				break;
			case '&' :	// and
			case 0x000000BA :	// and
				result = Cplx_EvalFxDbl2( &Cplx_fAND, &resultflag, &resultreg, result, Cplx_ListEvalsub11( SRC ) ) ;
				break;
			case ' ':	// Skip Space
				break;
			default:
				ExecPtr--;
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
		c = (unsigned char)SRC[ExecPtr];
		if ( c == 0x7F ) {
			c = (unsigned char)SRC[ExecPtr+1];
			switch ( c ) {
				case 0x000000B0 :	// And
					ExecPtr+=2;
					result = Cplx_EvalFxDbl2( &Cplx_fAND_logic, &resultflag, &resultreg, result, Cplx_ListEvalsub12( SRC ) ) ;
					break;
				default:
					return result;
					break;
			}
		} else
		if ( c == ' ' ) ExecPtr++;	// Skip Space
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
		c = (unsigned char)SRC[ExecPtr];
		if ( c == 0x7F ) {
			c = (unsigned char)SRC[ExecPtr+1];
			switch ( c ) {
				case 0x000000B1 :	// Or
					ExecPtr+=2;
					result = Cplx_EvalFxDbl2( &Cplx_fOR_logic, &resultflag, &resultreg, result, Cplx_ListEvalsub13( SRC ) ) ;
					break;
				case 0x000000B4 :	// Xor
					ExecPtr+=2;
					result = Cplx_EvalFxDbl2( &Cplx_fXOR_logic, &resultflag, &resultreg, result, Cplx_ListEvalsub13( SRC ) ) ;
					break;
				default:
					return result;
					break;
			}
		} else
		if ( c == ' ' ) ExecPtr++;	// Skip Space
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

	if ( (unsigned char)SRC[ExecPtr] != 0x0E ) CopyMatList2AnsTop( resultreg );	// ListResult -> List Ans top
	return 0;
}


int Cplx_ListEvalsubTopAns(char *SRC) {	//  
	int result,tmp;
	int c,i;
	int base;
	int resultreg,tmpreg;

	if ( CB_MatListAnsreg >=28 ) CB_MatListAnsreg=28;
	ErrorNo = 0;
	Cplx_ListEvalsubTop( SRC );
	resultreg=CB_MatListAnsreg;
	if ( dspflag < 3 ) { CB_Error(ArgumentERR); return -1; } // Argument error

	if ( (unsigned char)SRC[ExecPtr] != 0x0E ) CopyMatList2AnsTop( resultreg );	// ListResult -> List Ans top
	MatdspNo=CB_MatListAnsreg;
	return dspflag;
}

