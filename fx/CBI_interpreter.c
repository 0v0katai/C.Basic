/*
===============================================================================

 Casio Basic interpreter for fx-9860G series    v0.99

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
#include "CB_file.h"
#include "CB_edit.h"
#include "CB_glib.h"
#include "CB_glib2.h"
#include "CB_interpreter.h"
#include "CB_Eval.h"
#include "CB_Time.h"
#include "CB_Matrix.h"
#include "CB_Str.h"

#include "CBI_Eval.h"
#include "CBI_interpreter.h"

//----------------------------------------------------------------------------------------------
//		Interpreter
//----------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------
void CBint_Store( char *SRC ){	// ->
	int	en,i,j;
	int dimA,dimB,reg;
	int mptr;
	char*	MatAryC;
	short*	MatAryW;
	int*	MatAryI;
	
	int c=SRC[ExecPtr];
	if ( ( 'A'<=c )&&( c<='z' ) ) {
		reg=c-'A';
		ExecPtr++;
	  aliasj:
		c=SRC[ExecPtr];
		if ( c == 0x7E ) {		// '~'
			ExecPtr++;
			en=RegVarAliasEx(SRC);
			if ( en>=0 ) {
				if ( en<reg ) { CB_Error(SyntaxERR); return; }	// Syntax error
				c=SRC[ExecPtr];
				if ( c=='#' ) { ExecPtr++;  for ( i=reg; i<=en; i++) LocalDbl[ i ][0] = CBint_CurrentValue; }
				else
				if ( c=='%' ) ExecPtr++;
				for ( i=reg; i<=en; i++) LocalInt[ i ][0] = CBint_CurrentValue;
			}
		} else {
			if ( c=='#' ) { ExecPtr++;  LocalDbl[ reg ][0] = CBint_CurrentValue ; }
			else
			if ( c=='[' ) { goto Matrix; }
			else
			if ( ( '0'<=c )&&( c<='9' ) ) {
				ExecPtr++;
				dimA=c-'0';
				MatOprand1num( SRC, reg, &dimA, &dimB );
				goto Matrix2;
			} else {
				if ( c=='%' ) ExecPtr++;
				LocalInt[ reg ][0] = CBint_CurrentValue;
			}
		}
	} else
	if ( c==0x7F ) {
		c = SRC[ExecPtr+1] ; 
		if ( c == 0x40 ) {	// Mat A[a,b]
			ExecPtr+=2;
			c=SRC[ExecPtr];
			if ( ( 'A'<=c )&&( c<='z' ) ) { reg=c-'A'; ExecPtr++; } 
			else { reg=RegVarAliasEx(SRC); if ( reg<0 ) CB_Error(SyntaxERR) ; } // Syntax error 
			if ( SRC[ExecPtr] != '[' ) { 
				if ( dspflag ==3 ) { CopyAns2MatList( SRC, reg ) ; MatdspNo=reg; return ; }	// MatAns -> Mat A
				if ( MatAry[reg].SizeA == 0 ) { CB_Error(NoMatrixArrayERR); return; }	// No Matrix Array error
				InitMatSub( reg, CBint_CurrentValue);		// 10 -> Mat A
			} else {
			Matrix:
				ExecPtr++;
				MatOprandInt2( SRC, reg, &dimA, &dimB);
			Matrix2:
				if ( ErrorNo ) {  // error
					if ( MatAry[reg].SizeA == 0 ) ErrorNo=NoMatrixArrayERR;	// No Matrix Array error
					return ;
				}
				WriteMatrixInt( reg, dimA, dimB, CBint_CurrentValue);
			}
		} else if ( c == 0x51 ) {	// List
			ExecPtr+=2;
			reg=ListRegVar( SRC );
		  Listj:
			if ( SRC[ExecPtr] != '[' ) { 
				if ( dspflag ==4 ) { CopyAns2MatList( SRC, reg ) ; MatdspNo=reg; dspflag=0; return ; }	// ListAns -> List 1
				if ( MatAry[reg].SizeA == 0 ) { CB_Error(NoMatrixArrayERR); return; }	// No Matrix Array error
				InitMatSub( reg, CBint_CurrentValue);		// 10 -> List 1
			} else {
				ExecPtr++;
				MatOprandInt1( SRC, reg, &dimA, &dimB );	// List 1[a]
				goto Matrix2;
			}
		} else if ( (0x6A<=c) && (c<=0x6F) ) {	// List1~List6
			ExecPtr+=2;
			reg=c+(32-0x6A);
			goto Listj;
			
		} else if ( c == 0x46 ) {	// -> Dim
				ExecPtr+=2;
				if ( ( SRC[ExecPtr]==0x7F ) && ( SRC[ExecPtr+1]==0x51 ) ) {	// n -> Dim List
					ExecPtr+=2;
					if ( CBint_CurrentValue ) 
							CB_ListInitsub( SRC,  &reg, CBint_CurrentValue, 0 );
					else {
						reg=ListRegVar( SRC );
						if ( reg>=0 ) DeleteMatrix( reg );
					}
				} else
				if ( ( SRC[ExecPtr]==0x7F ) && ( SRC[ExecPtr+1]==0x40 ) ) {	// {10,5} -> Dim Mat A
					CB_MatrixInit( SRC );
					return ;
				} else {
					if ( CBint_CurrentValue ) 
							CB_MatrixInitsubNoMat( SRC, &reg, CBint_CurrentValue, 1, 0 );
					else {
						c = SRC[ExecPtr];
						reg=RegVarAliasEx(SRC);
						if ( reg>=0 ) DeleteMatrix( reg );
					}
				}
		} else if ( c == 0x5F ) {	// Ticks
				ExecPtr+=2;
				goto StoreTicks;
		} else if ( c == 0x00 ) {	// Xmin
				Xmin = CBint_CurrentValue ;
				Xdot = (Xmax-Xmin)/126.0;
				goto Graphj;
		} else if ( c == 0x01 ) {	// Xmax
				Xmax = CBint_CurrentValue ;
				Xdot = (Xmax-Xmin)/126.0;
				goto Graphj;
		} else if ( c == 0x02 ) {	// Xscl
				Xscl = CBint_CurrentValue ;
				goto Graphj;
		} else if ( c == 0x04 ) {	// Ymin
				Ymin = CBint_CurrentValue ;
				Ydot = (Ymax-Ymin)/62.0;
				goto Graphj;
		} else if ( c == 0x05 ) {	// Ymax
				Ymax = CBint_CurrentValue ;
				Ydot = (Ymax-Ymin)/62.0;
				goto Graphj;
		} else if ( c == 0x06) {	// Yscl
				Yscl = CBint_CurrentValue ;
				goto Graphj;
		} else if ( c == 0x08) {	// Thetamin
				TThetamin = CBint_CurrentValue ;
				goto Graphj;
		} else if ( c == 0x09) {	// Thetamax
				TThetamax = CBint_CurrentValue ;
				goto Graphj;
		} else if ( c == 0x0A) {	// Thetaptch
				TThetaptch = CBint_CurrentValue ;
				goto Graphj;
		} else if ( c == 0x0B ) {	// Xfct
				Xfct = CBint_CurrentValue ;
				goto Graphj;
		} else if ( c == 0x0C ) {	// Yfct
				Yfct = CBint_CurrentValue ;
			Graphj:
				ExecPtr+=2;
				CB_ChangeViewWindow() ;
		} else goto exitj;
	} else
	if ( c==0xFFFFFFF7 ) {
		c = SRC[ExecPtr+1] ; 
		if ( c == 0xFFFFFFF6 ) {	// Poke(A)
			ExecPtr+=2;
			CB_PokeSubInt( SRC, CBint_CurrentValue, EvalIntsubTop( SRC ) );
		} else goto exitj;
	} else
	if ( c=='*' ) { ExecPtr++;
			CB_PokeSubInt( SRC, CBint_CurrentValue, EvalIntsub1( SRC ) );
	} else
	if ( c=='%' ) { ExecPtr++;
		StoreTicks:
		CB_TicksStart=RTC_GetTicks()-CB_TicksStart;
		SetRtc( CBint_CurrentValue );
		i=RTC_GetTicks();
		CB_TicksAdjust=i-CBint_CurrentValue ;	// 
		CB_TicksStart+=(i-1);	// 
		skip_count=0;
	} else
	if ( c==0xFFFFFFF9 ) {
		c = SRC[ExecPtr+1] ; 
		if ( c == 0x21 ) {	// Xdot
				if ( CBint_CurrentValue == 0 ) { ErrorNo=RangeERR; ErrorPtr=ExecPtr; return; }	// Range error
				ExecPtr+=2;
				Xdot = CBint_CurrentValue ;
				Xmax = Xmin + Xdot*126.;
		} else goto exitj;
	} else {
	  exitj:
		reg=RegVarAliasEx( SRC ); if ( reg>=0 ) goto aliasj;	// variable alias
		CB_Error(SyntaxERR); return; }	// Syntax error
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

void CBint_Dsz( char *SRC ) { //	Dsz
	int c;
	int reg,mptr;
	int dimA,dimB;
	char*	MatAryC;
	short*	MatAryW;
	int*	MatAryI;
	c=SRC[ExecPtr];
	if ( ( 'A'<=c )&&( c<='z' ) ) {
		ExecPtr++;
		reg=c-'A';
	  regj:
		c=SRC[ExecPtr];
		if ( c=='#' ) {
			ExecPtr++;
			LocalDbl[reg][0] --;
			CBint_CurrentValue = LocalDbl[reg][0] ;
		} else
		if ( c=='[' ) { 
			ExecPtr++;
			MatOprandInt2( SRC, reg, &dimA, &dimB );
			goto Matrix;
		} else
		if ( ( '0'<=c )&&( c<='9' ) ) {
			ExecPtr++;
			dimA=c-'0';
			MatOprand1num( SRC, reg, &dimA, &dimB );
			goto Matrix;
		} else {
			if ( c=='%' ) ExecPtr++;
			LocalInt[reg][0] --;
			CBint_CurrentValue = LocalInt[reg][0] ;
		}
	} else 
	if ( ( ( c==0x7F ) && ( SRC[ExecPtr+1]==0x40 ) ) || ( ( c==0x7F ) && ( SRC[ExecPtr+1]==0x51 ) ) ) {	// Mat or List
			MatrixOprand( SRC, &reg, &dimA, &dimB );
		Matrix:
			if ( ErrorNo ) {  // error
				if ( MatAry[reg].SizeA == 0 ) ErrorNo=NoMatrixArrayERR;	// No Matrix Array error
				return ;
			}
			CBint_CurrentValue = ReadMatrixInt( reg, dimA,dimB ) ;
			CBint_CurrentValue --;
			WriteMatrixInt( reg, dimA,dimB, CBint_CurrentValue ) ;
	} else {
		reg=RegVarAliasEx(SRC);	if ( reg>=0 ) goto regj;
		{ CB_Error(SyntaxERR); return; }	// Syntax error
	}

	c=SRC[ExecPtr];
	if ( ( c==':' ) || ( c==0x0D ) ) {
		ExecPtr++;
		if ( CBint_CurrentValue ) return ;
		else {
			Skip_block(SRC);
		}
	} else if ( c==0x0C ) {  // dsps
		ExecPtr++;
		if ( CB_Disps(SRC, 2) ) { BreakPtr=ExecPtr ; return ; }  // [AC] break
		if ( CBint_CurrentValue ) return ;
		else {
			Skip_block(SRC);
		}
	} else { CB_Error(SyntaxERR); return; }	// Syntax error
}

void CBint_Isz( char *SRC ) { //	Isz
	int c;
	int reg,mptr;
	int dimA,dimB;
	char*	MatAryC;
	short*	MatAryW;
	int*	MatAryI;
	c=SRC[ExecPtr];
	if ( ( 'A'<=c )&&( c<='z' ) ) {
		ExecPtr++;
		reg=c-'A';
	  regj:
		c=SRC[ExecPtr];
		if ( c=='#' ) {
			ExecPtr++;
			LocalDbl[reg][0] ++;
			CBint_CurrentValue = LocalDbl[reg][0] ;
		} else
		if ( c=='[' ) { 
			ExecPtr++;
			MatOprandInt2( SRC, reg, &dimA, &dimB );
			goto Matrix;
		} else
		if ( ( '0'<=c )&&( c<='9' ) ) {
			ExecPtr++;
			dimA=c-'0';
			MatOprand1num( SRC, reg, &dimA, &dimB );
			goto Matrix;
		} else {
			if ( c=='%' ) ExecPtr++;
			LocalInt[reg][0] ++;
			CBint_CurrentValue = LocalInt[reg][0] ;
		}
	} else 
	if ( ( ( c==0x7F ) && ( SRC[ExecPtr+1]==0x40 ) ) || ( ( c==0x7F ) && ( SRC[ExecPtr+1]==0x51 ) ) ) {	// Mat or List
			MatrixOprand( SRC, &reg, &dimA, &dimB );
		Matrix:
			if ( ErrorNo ) {  // error
				if ( MatAry[reg].SizeA == 0 ) ErrorNo=NoMatrixArrayERR;	// No Matrix Array error
				return ;
			}
			CBint_CurrentValue = ReadMatrixInt( reg, dimA,dimB ) ;
			CBint_CurrentValue ++;
			WriteMatrixInt( reg, dimA,dimB, CBint_CurrentValue ) ;
	} else {
		reg=RegVarAliasEx(SRC);	if ( reg>=0 ) goto regj;
		{ CB_Error(SyntaxERR); return; }	// Syntax error
	}
	
	c=SRC[ExecPtr];
	if ( ( c==':' ) || ( c==0x0D ) ) {
		ExecPtr++;
		if ( CBint_CurrentValue ) return ;
		else {
			Skip_block(SRC);
		}
	} else if ( c==0x0C ) {  // dsps
		ExecPtr++;
		if ( CB_Disps(SRC, 2) ) { BreakPtr=ExecPtr ; return ; }  // [AC] break
		if ( CBint_CurrentValue ) return ;
		else {
			Skip_block(SRC);
		}
	} else { CB_Error(SyntaxERR); return; }	// Syntax error
}


//----------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------
/*
int RangeErrorCKint( char *SRC ) {
	if ( UseGraphic == 0 ) {
		if ( ScreenMode == 0 ) CB_SelectGraphVRAM();	// Select Graphic Screen
		Bdisp_AllClr_VRAM();
		GraphAxesGrid();
	}
	if ( ( Xdot == 0 ) || ( Ydot == 0 )  ) { ErrorNo=RangeERR; PrevOpcode( SRC, &ExecPtr ); ErrorPtr=ExecPtr; return ErrorNo; }	// Range error
	return 0;
}
*/
//----------------------------------------------------------------------------------------------

void CBint_PxlSub( char *SRC, int mode ) { //	mode  1:PxlOn   0:PxlOff   2:PxlChg
	int px,py;
	if ( RangeErrorCK(SRC) ) return;
	py = (EvalIntsubTop( SRC ));
	if ( ( py<MatBase ) || ( py>63 ) ) { CB_Error(ArgumentERR); return; }  // Argument error
	if ( SRC[ExecPtr] != ',' ) { CB_Error(SyntaxERR); return; }  // Syntax error
	ExecPtr++;
	px = (EvalIntsubTop( SRC ));
	if ( ( px<MatBase ) || ( px>127 ) ) { CB_Error(ArgumentERR); return; }  // Argument error}
	CB_SelectGraphVRAM();	// Select Graphic Screen
	BdispSetPointVRAM2(px, py, mode);
//	regintX = (    px-1)*Xdot  + Xmin ;
//	regintY = ( 62-py+1)*Ydot  + Ymin ;
	Bdisp_PutDisp_DD_DrawBusy_skip_through(SRC);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

int WordSizeSelect( char *SRC ) {
	int c,d;
	int WordSize=8;
	c =SRC[ExecPtr];
	if ( c=='.' ) {
		c =SRC[++ExecPtr];
		if ( ( c=='B' ) || ( c=='b' ) ) { ExecPtr++; }
		else
		if ( ( c=='W' ) || ( c=='w' ) ) { ExecPtr++; WordSize=16; }
		else
		if ( ( c=='L' ) || ( c=='l' ) ) { ExecPtr++; WordSize=32; }
		else
		if ( ( c=='F' ) || ( c=='f' ) ) { ExecPtr++; WordSize=64; }
	}
	return WordSize;
}

double CB_Peek( char *SRC, int adrs ) {	// Peek(123456).f
	int wsize;
	char *cptr;
	short *sptr;
	int *iptr;
	double *dptr;
	if ( SRC[ExecPtr]==')' ) ExecPtr++;
	wsize=WordSizeSelect( SRC );
	switch ( wsize ) {
		case 64:
			if ( adrs & 3 ) { CB_Error(ArgumentERR); return 0; } // Argument error
			dptr=(double *)adrs;
			return *dptr;
		case 32:
			if ( adrs & 3 ) { CB_Error(ArgumentERR); return 0; } // Argument error
			iptr=(int *)adrs;
			return *iptr;
		case 16:
			if ( adrs & 1 ) { CB_Error(ArgumentERR); return 0; } // Argument error
			sptr=(short *)adrs;
			return *sptr;
		default:
			cptr=(char *)adrs;
			return *cptr;
	}
}
int CB_PeekInt( char *SRC, int adrs ) {	// Peek(123456).w
	int wsize;
	char *cptr;
	short *sptr;
	int *iptr;
	double *dptr;
	if ( SRC[ExecPtr]==')' ) ExecPtr++;
	wsize=WordSizeSelect( SRC );
	switch ( wsize ) {
		case 64:
			if ( adrs & 3 ) { CB_Error(ArgumentERR); return 0; } // Argument error
			dptr=(double *)adrs;
			return *dptr;
		case 32:
			if ( adrs & 3 ) { CB_Error(ArgumentERR); return 0; } // Argument error
			iptr=(int *)adrs;
			return *iptr;
		case 16:
			if ( adrs & 1 ) { CB_Error(ArgumentERR); return 0; } // Argument error
			sptr=(short *)adrs;
			return *sptr;
		default:
			cptr=(char *)adrs;
			return *cptr;
	}
}

void CB_PokeSub( char *SRC, double data, int adrs ) {	// Poke(123456).f
	int wsize;
	char *cptr;
	short *sptr;
	int *iptr;
	double *dptr;
	if ( SRC[ExecPtr]==')' ) ExecPtr++;
	wsize=WordSizeSelect( SRC );
	switch ( wsize ) {
		case 64:
			if ( adrs & 3 ) { CB_Error(ArgumentERR); return; } // Argument error
			dptr=(double *)adrs;
			*dptr=data;
			break;
		case 32:
			if ( adrs & 3 ) { CB_Error(ArgumentERR); return; } // Argument error
			iptr=(int *)adrs;
			*iptr=data;
			break;
		case 16:
			if ( adrs & 1 ) { CB_Error(ArgumentERR); return; }  // Argument error
			sptr=(short *)adrs;
			*sptr=data;
			break;
		default:
			cptr=(char *)adrs;
			*cptr=data;
			break;
	}
}
void CB_PokeSubInt( char *SRC, int data, int adrs ) {	// Poke(123456).w
	int wsize;
	char *cptr;
	short *sptr;
	int *iptr;
	double *dptr;
	if ( SRC[ExecPtr]==')' ) ExecPtr++;
	wsize=WordSizeSelect( SRC );
	switch ( wsize ) {
		case 64:
			if ( adrs & 3 ) { CB_Error(ArgumentERR); return; } // Argument error
			dptr=(double *)adrs;
			*dptr=data;
			break;
		case 32:
			if ( adrs & 3 ) { CB_Error(ArgumentERR); return; } // Argument error
			iptr=(int *)adrs;
			*iptr=data;
			break;
		case 16:
			if ( adrs & 1 ) { CB_Error(ArgumentERR); return; }  // Argument error
			sptr=(short *)adrs;
			*sptr=data;
			break;
		default:
			cptr=(char *)adrs;
			*cptr=data;
			break;
	}
}

void CB_Poke( char *SRC ) {	// Poke(123456).w,123,...
	int adrs=CB_EvalInt( SRC );
	int wsize;
	char *cptr;
	short *sptr;
	int *iptr;
	double *dptr;
	if ( SRC[ExecPtr]==')' ) ExecPtr++;
	wsize=WordSizeSelect( SRC );
	if ( SRC[ExecPtr]!=',' ) { CB_Error(SyntaxERR); return; }	// Syntax error
	do {
		ExecPtr++;
		switch ( wsize ) {
			case 64:
				if ( adrs & 3 ) { CB_Error(ArgumentERR); return; } // Argument error
				dptr=(double *)adrs;
				*dptr=CB_EvalDbl( SRC );
				adrs+=8;
				break;
			case 32:
				if ( adrs & 3 ) { CB_Error(ArgumentERR); return; } // Argument error
				iptr=(int *)adrs;
				*iptr=CB_EvalInt( SRC );
				adrs+=4;
				break;
			case 16:
				if ( adrs & 1 ) { CB_Error(ArgumentERR); return; }  // Argument error
				sptr=(short *)adrs;
				*sptr=CB_EvalInt( SRC );
				adrs+=2;
				break;
			default:
				cptr=(char *)adrs;
				*cptr=CB_EvalInt( SRC );
				adrs++;
				break;
		}
	} while ( SRC[ExecPtr] == ',' );
}

int CB_EvalIntStrPtr( char *SRC ) {
	int c;
	int result;
	int maxoplen;
	char *buffer;
	c=CB_IsStr( SRC, ExecPtr );
	if ( c ) {	// string
		buffer = CB_GetOpStr( SRC, &maxoplen );	
		if ( ErrorNo ) return ;			// error
		return (int)buffer;	//  return buffer ptr
	} else {	// expression
		return CB_EvalInt( SRC );
	}
}

int CB_SysCall( char *SRC ) {	// SysCall( No,r4,r5,r6,r7 )
	int callNo=CB_EvalInt( SRC );
	int r4=0,r5=0,r6=0,r7=0;
	unsigned int ur4,ur5,ur6,ur7;
	if ( SRC[ExecPtr]==',' ) { 
		ExecPtr++;
		r4=CB_EvalIntStrPtr( SRC );
		if ( SRC[ExecPtr]==',' ) { 
			ExecPtr++;
			r5=CB_EvalIntStrPtr( SRC );
			if ( SRC[ExecPtr]==',' ) {
				ExecPtr++;
				r6=CB_EvalIntStrPtr( SRC );
				if ( SRC[ExecPtr]==',' ) {
					ExecPtr++;
					r7=CB_EvalIntStrPtr( SRC );
				}
			}
		}
	}
	if ( SRC[ExecPtr]==')' ) ExecPtr++;
	return SysCalljmp(r4,r5,r6,r7,callNo);	//		CallNo -> SysCallNo
}
int CB_Call( char *SRC ) {	// Call( adrs,r5,r6,r7 )
	int adrs=CB_EvalInt( SRC );
	int r4=0,r5=0,r6=0,r7=0;
	if ( adrs & 1 ) CB_Error(ArgumentERR);  // Argument error
	if ( SRC[ExecPtr]==',' ) { 
		ExecPtr++;
		r4=CB_EvalIntStrPtr( SRC );
		if ( SRC[ExecPtr]==',' ) { 
			ExecPtr++;
			r5=CB_EvalIntStrPtr( SRC );
			if ( SRC[ExecPtr]==',' ) {
				ExecPtr++;
				r6=CB_EvalIntStrPtr( SRC );
				if ( SRC[ExecPtr]==',' ) {
					ExecPtr++;
					r7=CB_EvalIntStrPtr( SRC );
				}
			}
		}
	}
	if ( SRC[ExecPtr]==')' ) ExecPtr++;
	return Calljmp(r4,r5,r6,r7,adrs);
}

int CB_VarPtr( char *SRC ) {
	int c;
	int reg,dimA,dimB;
	int result;
	int maxoplen;
	char *buffer;
	
	c=SRC[ExecPtr];
	if ( ( 'A'<=c )&&( c<='z' ) ) {	// variable
		ExecPtr++;
		reg=c-'A';
	  regj:
		c=SRC[ExecPtr];
		if ( c=='%' ) { ExecPtr++; result=(int)&LocalInt[reg][0]; }
		else
		if ( c=='[' ) { goto Matrix; }		// A[1]
		else
		if ( ( '0'<=c )&&( c<='9' ) ) {		// A1
				ExecPtr++;
				dimA=c-'0';
				MatOprand1num( SRC, reg, &dimA, &dimB );
				goto Matrix2;
		} else
		if ( c=='#' ) { ExecPtr++; result=(int)&LocalDbl[reg][0]; }
		else
		if (CB_INT)	result=(int)&LocalInt[reg][0]; else result=(int)&LocalDbl[reg][0];
	} else
	if ( ( c==0x7F ) && ( SRC[ExecPtr+1]==0x40 ) ) {	// Mat
		ExecPtr+=2;
		c=SRC[ExecPtr];
		if ( ( 'A'<=c )&&( c<='z' ) ) { reg=c-'A'; ExecPtr++; } 
		else { reg=RegVarAliasEx(SRC); if ( reg<0 ) CB_Error(SyntaxERR) ; } // Syntax error 
		if ( SRC[ExecPtr] == '[' ) {
		Matrix:	
			ExecPtr++;
			MatOprand2( SRC, reg, &dimA, &dimB );	// Mat A[a,b]
		Matrix2:	
			if ( ErrorNo ) return 1 ; // error
			result=(int)MatrixPtr( reg, dimA, dimB );
		} else {
			result=(int)MatAry[reg].Adrs;	// Mat A
		}
	} else
	if ( ( c==0x7F ) && ( SRC[ExecPtr+1]==0x51 ) ) {	// List
		ExecPtr+=2;
		c=SRC[ExecPtr];
		reg=ListRegVar( SRC );
		if ( reg<0 ) CB_Error(SyntaxERR) ;  // Syntax error 
		if ( SRC[ExecPtr] == '[' ) {
			ExecPtr++;
			MatOprand1( SRC, reg, &dimA, &dimB );	// List 1[a]
			if ( ErrorNo ) return 1 ; // error
			result=(int)MatrixPtr( reg, dimA, dimB );
		} else {
			result=(int)MatAry[reg].Adrs;	// List 1
		}
	} else {
		c=CB_IsStr( SRC, ExecPtr );
		if ( c ) {	// string
			buffer=CB_GetOpStr( SRC, &maxoplen ) ;		// String -> buffer	return 
			if ( ErrorNo ) return 1;			// error
			result=(int)buffer;	//  return buffer ptr
		} else {
			reg=RegVarAliasEx(SRC); if ( reg>=0 ) goto regj;
			CB_Error(SyntaxERR) ; return 0; // Syntax error 
		}
	}
	if ( SRC[ExecPtr]==')' ) ExecPtr++;
	return result;
}

int CB_ProgPtr( char *SRC ) { //	ProgPtr(
	int c;
	char buffer[32],folder16[21];
	int progno;

	c=SRC[ExecPtr];
	if ( c != 0x22 ) { 
		if ( c != ')' ) { ExecPtr++; return 0; }	// ProgPtr()
		ExecPtr++;
		return (int)ProgfileAdrs[ProgNo];
	}
	ExecPtr++;
	CB_GetQuotOpcode(SRC, buffer,32);	// Prog name
	
	Setfoldername16( folder16, buffer );
	progno = CB_SearchProg( folder16 );
	if ( progno < 0 ) { CB_Error(NotfoundProgERR); return 0 ; }  // Not found Prog

	if ( SRC[ExecPtr]==')' ) ExecPtr++;
	return  (int)ProgfileAdrs[progno];
}
