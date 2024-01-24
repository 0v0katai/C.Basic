/*
===============================================================================

 Casio Basic interpreter for fx-9860G series    v0.50

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

#include "CBI_Eval.h"
#include "CBI_interpreter.h"

//----------------------------------------------------------------------------------------------
//		Interpreter
//----------------------------------------------------------------------------------------------
int RangeErrorCKint( char *SRC ) {
	if ( UseGraphic == 0 ) {
		if ( ScreenMode == 0 ) CB_SelectGraphVRAM();	// Select Graphic Screen
		Bdisp_AllClr_VRAM();
		GraphAxesGrid();
	}
	if ( ( Xdot == 0 ) || ( Ydot == 0 )  ) { ErrorNo=RangeERR; PrevOpcode( SRC, &ExecPtr ); ErrorPtr=ExecPtr; return ErrorNo; }	// Range error
	return 0;
}
//----------------------------------------------------------------------------------------------
void CBint_Store( char *SRC ){	// ->
	int	en,i,j;
	int dimA,dimB,reg;
	int mptr;
	char*	MatAryC;
	short*	MatAryW;
	int*	MatAryI;
	
	int c=SRC[ExecPtr];
	if ( ( 'A' <= c ) && ( c <='Z' ) ) {
		reg=c-'A';
		ExecPtr++;
		if ( SRC[ExecPtr] == 0x7E ) {		// '~'
			ExecPtr++;
			c=SRC[ExecPtr];
			if ( ( 'A' <= c ) && ( c <='Z' ) ) {
				en=c-'A';
				if ( en<reg ) { CB_Error(SyntaxERR); return; }	// Syntax error
				c=SRC[++ExecPtr];
				if ( c=='#' ) { ExecPtr++;  for ( i=reg; i<=en; i++) REG[ i ] = CBint_CurrentValue; }
				else
				if ( c=='%' ) ExecPtr++;
				for ( i=reg; i<=en; i++) REGINT[ i ] = CBint_CurrentValue;
			}
		} else {
			c=SRC[ExecPtr];
			if ( c=='#' ) { ExecPtr++;  REG[reg] = CBint_CurrentValue ; }
			else
			if ( c=='[' ) goto Matrix;
			else {
				if ( c=='%' ) ExecPtr++;
				REGINT[ reg ] = CBint_CurrentValue;
			}
		}
	} else
	if ( ( 'a' <= c ) && ( c <='z' ) ) {
		reg=c-'a';
		ExecPtr++;
		if ( SRC[ExecPtr] == 0x7E ) {		// '~'
			ExecPtr++;
			c=SRC[ExecPtr];
			if ( ( 'a' <= c ) && ( c <='z' ) ) {
				en=c-'a';
				if ( en<reg ) { CB_Error(SyntaxERR); return; }	// Syntax error
				c=SRC[++ExecPtr];
				if ( c=='#' ) { ExecPtr++;  for ( i=reg; i<=en; i++) LocalDbl[ i ][0] = CBint_CurrentValue; }
				else
				if ( c=='%' ) ExecPtr++;
				for ( i=reg; i<=en; i++) LocalInt[ i ][0] = CBint_CurrentValue;
			}
		} else {
			c=SRC[ExecPtr];
			if ( c=='#' ) { ExecPtr++;  LocalDbl[ reg ][0] = CBint_CurrentValue ; }
			else
			if ( c=='[' ) goto Matrix;
			else {
				if ( c=='%' ) ExecPtr++;
				LocalInt[ reg ][0] = CBint_CurrentValue;
			}
		}
	} else
	if ( c==0x7F ) {
		c = SRC[ExecPtr+1] ; 
		if ( c == 0x40 ) {	// Mat A[a,b]
			ExecPtr+=2;
			c=SRC[ExecPtr]; if ( ( 'A'<=c )&&( c<='Z' ) ) { reg=c-'A'; ExecPtr++; } else CB_Error(SyntaxERR) ; // Syntax error 
			if ( SRC[ExecPtr] != '[' ) { 
				if ( MatAry[reg].SizeA == 0 ) { CB_Error(NoMatrixArrayERR); return; }	// No Matrix Array error
				InitMatIntSub( reg, CBint_CurrentValue);
			} else {
			Matrix:
				if ( MatAry[reg].SizeA == 0 ) { CB_Error(NoMatrixArrayERR); return; }	// No Matrix Array error
				ExecPtr++;
				MatOprandInt2( SRC, reg, &dimA, &dimB);
				if ( ErrorNo ) return ; // error
				WriteMatrixInt( reg, dimA, dimB, CBint_CurrentValue);
			}
		} else if ( c == 0x5F ) {	// Ticks
				ExecPtr+=2;
				goto StoreTicks;
		} else if ( c == 0x46 ) {	// ->DimZ
				ExecPtr+=2;
				if ( CBint_CurrentValue ) 
						CB_MatrixInitsubNoMat( SRC, &reg, CBint_CurrentValue, 1, 0 );
				else {
					c = SRC[ExecPtr];
					if ( ( 'A' <= c ) && ( c <= 'z' ) ) {
						ExecPtr++;
						DeleteMatrix( c-'A' );
					}
				}
		} else if ( c == 0x00 ) {	// Xmin
				ExecPtr+=2;
				Xmin = CBint_CurrentValue ;
				Xdot = (Xmax-Xmin)/126.0;
		} else if ( c == 0x01 ) {	// Xmax
				ExecPtr+=2;
				Xmax = CBint_CurrentValue ;
				Xdot = (Xmax-Xmin)/126.0;
		} else if ( c == 0x02 ) {	// Xscl
				ExecPtr+=2;
				Xscl = CBint_CurrentValue ;
		} else if ( c == 0x04 ) {	// Ymin
				ExecPtr+=2;
				Ymin = CBint_CurrentValue ;
				Ydot = (Ymax-Ymin)/62.0;
		} else if ( c == 0x05 ) {	// Ymax
				ExecPtr+=2;
				Ymax = CBint_CurrentValue ;
				Ydot = (Ymax-Ymin)/62.0;
		} else if ( c == 0x06) {	// Yscl
				ExecPtr+=2;
				Yscl = CBint_CurrentValue ;
		} else if ( c == 0x0B ) {	// Xfct
				ExecPtr+=2;
				Xfct = CBint_CurrentValue ;
		} else if ( c == 0x0C ) {	// Yfct
				ExecPtr+=2;
				Yfct = CBint_CurrentValue ;
		}
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
		} else { CB_Error(SyntaxERR); return; }	// Syntax error
	} else { CB_Error(SyntaxERR); return; }	// Syntax error
}

//-----------------------------------------------------------------------------
void CBint_For( char *SRC ,StkFor *StackFor, CurrentStk *CurrentStruct ){
	int c;
	CBint_CurrentValue = EvalIntsubTop( SRC );
	c=SRC[ExecPtr];
	if ( c == 0x0E ) {	// ->
		ExecPtr++;
		c=SRC[ExecPtr];
		if ( ( 'A' <= c ) && ( c <= 'Z' ) ) {
			StackFor->Var[StackFor->Ptr]=&REGINT[c-'A'];
			CBint_Store(SRC);
		} else
		if ( ( 'a' <= c ) && ( c <= 'z' ) ) {
			StackFor->Var[StackFor->Ptr]=LocalInt[c-'a'];
			CBint_Store(SRC);
		} else { CB_Error(SyntaxERR); return; }	// Syntax error
	}
	if ( StackFor->Ptr >= StackForMax ) { CB_Error(NestingERR); return; } //  nesting error
	c=SRC[ExecPtr];
	if ( ( c != 0xFFFFFFF7 ) || ( SRC[ExecPtr+1] != 0x05 ) ) { CB_Error(SyntaxERR); return; }	// Syntax error
	ExecPtr+=2;
	StackFor->IntEnd[StackFor->Ptr] = EvalIntsubTop( SRC );
	c=SRC[ExecPtr];
	if ( ( c == 0xFFFFFFF7 ) && ( SRC[ExecPtr+1] == 0x06 ) ) {	// Step
		ExecPtr+=2;
		StackFor->IntStep[StackFor->Ptr] = EvalIntsubTop( SRC );
	} else {
		StackFor->IntStep[StackFor->Ptr] = 1;
	}
	StackFor->Adrs[StackFor->Ptr] = ExecPtr;
	StackFor->Ptr++;
	CurrentStruct->TYPE[CurrentStruct->CNT]=1;
	CurrentStruct->CNT++;
}
void CBint_Next( char *SRC ,StkFor *StackFor, CurrentStk *CurrentStruct ){
	int step,end;
	int i;
	int *iptr;
	if ( StackFor->Ptr <= 0 ) { ErrorNo=NextWithoutForERR; ErrorPtr=ExecPtr; return; } // Next without for error
	StackFor->Ptr--;
	CurrentStruct->CNT--;
	step = StackFor->IntStep[StackFor->Ptr];
	iptr=StackFor->Var[StackFor->Ptr];
	*iptr += step;
	if ( step > 0 ) { 	// step +
		if ( *iptr > StackFor->IntEnd[StackFor->Ptr] ) return ; // exit
		ExecPtr = StackFor->Adrs[StackFor->Ptr];
		(StackFor->Ptr)++;		// continue
	CurrentStruct->TYPE[CurrentStruct->CNT]=1;
	CurrentStruct->CNT++;
	}
	else {									// step -
		if ( *iptr < StackFor->IntEnd[StackFor->Ptr] ) return ; // exit
		ExecPtr = StackFor->Adrs[StackFor->Ptr];
		StackFor->Ptr++;		// continue
	CurrentStruct->TYPE[CurrentStruct->CNT]=1;
	CurrentStruct->CNT++;
	}
}
//-----------------------------------------------------------------------------

void CBint_Dsz( char *SRC ) { //	Dsz
	int c;
	int reg,mptr;
	int dimA,dimB;
	char*	MatAryC;
	short*	MatAryW;
	int*	MatAryI;
	c=SRC[ExecPtr];
	if ( ( 'A' <= c ) && ( c <= 'Z' ) ) {
		ExecPtr++;
		reg=c-'A';
		c=SRC[ExecPtr];
		if ( c=='#' ) {
			ExecPtr++;
			REG[reg] --;
			CBint_CurrentValue = REG[reg] ;
		} else
		if ( c=='[' ) { 
			ExecPtr++;
			MatOprandInt2( SRC, reg, &dimA, &dimB );
			goto Matrix;
		} else {
			if ( c=='%' ) ExecPtr++;
			REGINT[reg] --;
			CBint_CurrentValue = REGINT[reg] ;
		}
	} else 
	if ( ( 'a' <= c ) && ( c <= 'z' ) ) {
		ExecPtr++;
		reg=c-'a';
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
		} else {
			if ( c=='%' ) ExecPtr++;
			LocalInt[reg][0] --;
			CBint_CurrentValue = LocalInt[reg][0] ;
		}
	} else 
	if ( c==0x7F ) {
			MatrixOprand( SRC, &reg, &dimA, &dimB );
		Matrix:
			if ( ErrorNo ) return ; // error
			CBint_CurrentValue = ReadMatrixInt( reg, dimA,dimB ) ;
			CBint_CurrentValue --;
			WriteMatrixInt( reg, dimA,dimB, CBint_CurrentValue ) ;
	} else { CB_Error(SyntaxERR); return; }	// Syntax error

	c=SRC[ExecPtr];
	if ( ( c==':' ) || ( c==0x0D ) ) {
		ExecPtr++;
		if ( CBint_CurrentValue ) return ;
		else {
			Skip_block(SRC);
		}
	} else if ( c==0x0C ) {  // dsps
		ExecPtr++;
		CB_Disps( SRC ,2);
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
	if ( ( 'A' <= c ) && ( c <= 'Z' ) ) {
		ExecPtr++;
		reg=c-'A';
		c=SRC[ExecPtr];
		if ( c=='#' ) {
			ExecPtr++;
			REG[reg] ++;
			CBint_CurrentValue = REG[reg] ;
		} else
		if ( c=='[' ) { 
			ExecPtr++;
			MatOprandInt2( SRC, reg, &dimA, &dimB );
			goto Matrix;
		} else {
			if ( c=='%' ) ExecPtr++;
			REGINT[reg] ++;
			CBint_CurrentValue = REGINT[reg] ;
		}
	} else 
	if ( ( 'a' <= c ) && ( c <= 'z' ) ) {
		ExecPtr++;
		reg=c-'a';
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
		} else {
			if ( c=='%' ) ExecPtr++;
			LocalInt[reg][0] ++;
			CBint_CurrentValue = LocalInt[reg][0] ;
		}
	} else 
	if ( c==0x7F ) {
			MatrixOprand( SRC, &reg, &dimA, &dimB );
		Matrix:
			if ( ErrorNo ) return ; // error
			CBint_CurrentValue = ReadMatrixInt( reg, dimA,dimB ) ;
			CBint_CurrentValue ++;
			WriteMatrixInt( reg, dimA,dimB, CBint_CurrentValue ) ;
	} else { CB_Error(SyntaxERR); return; }	// Syntax error

	c=SRC[ExecPtr];
	if ( ( c==':' ) || ( c==0x0D ) ) {
		ExecPtr++;
		if ( CBint_CurrentValue ) return ;
		else {
			Skip_block(SRC);
		}
	} else if ( c==0x0C ) {  // dsps
		ExecPtr++;
		CB_Disps( SRC ,2);
		if ( CBint_CurrentValue ) return ;
		else {
			Skip_block(SRC);
		}
	} else { CB_Error(SyntaxERR); return; }	// Syntax error
}

//----------------------------------------------------------------------------------------------

void CBint_PxlOprand( char *SRC, int *py, int *px) {
	int x,y;
	y = (EvalIntsubTop( SRC ));
	*py=y;
	if ( ( (*py)<MatBase ) || ( (*py)>63 ) ) { CB_Error(ArgumentERR); return; }  // Argument error
	if ( SRC[ExecPtr] != ',' ) { CB_Error(SyntaxERR); return; }  // Syntax error
	ExecPtr++;
	x = (EvalIntsubTop( SRC ));
	*px=x;
	if ( ( (*px)<MatBase ) || ( (*px)>127 ) ) { CB_Error(ArgumentERR); return; }  // Argument error}
	CB_SelectGraphVRAM();	// Select Graphic Screen
}
void CBint_PxlOn( char *SRC ) { //	PxlOn
	int x,y;
	if ( RangeErrorCK(SRC) ) return;
	CBint_PxlOprand( SRC, &y, &x);
	BdispSetPointVRAM2(x, y, 1);
//	regintX = (    px-1)*Xdot  + Xmin ;
//	regintY = ( 62-py+1)*Ydot  + Ymin ;
	Bdisp_PutDisp_DD_DrawBusy_skip_through(SRC);
}
void CBint_PxlOff( char *SRC ) { //	PxlOff
	int x,y;
	if ( RangeErrorCK(SRC) ) return;
	CBint_PxlOprand( SRC, &y, &x);
	BdispSetPointVRAM2(x, y, 0);
//	regintX = (    px-1)*Xdot  + Xmin ;
//	regintY = ( 62-py+1)*Ydot  + Ymin ;
	Bdisp_PutDisp_DD_DrawBusy_skip_through(SRC);
}
void CBint_PxlChg( char *SRC ) { //	PxlChg
	int x,y;
	if ( RangeErrorCK(SRC) ) return;
	CBint_PxlOprand( SRC, &y, &x);
	BdispSetPointVRAM2(x, y, 2);
//	if (Bdisp_GetPoint_VRAM(x, y)) 
//		Bdisp_SetPoint_VRAM(x, y, 0);
//	else
//		Bdisp_SetPoint_VRAM(x, y, 1);
//	regintX = (    px-1)*Xdot  + Xmin ;
//	regintY = ( 62-py+1)*Ydot  + Ymin ;
	Bdisp_PutDisp_DD_DrawBusy_skip_through(SRC);
}

