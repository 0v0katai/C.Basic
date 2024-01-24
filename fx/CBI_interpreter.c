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

#include "CBI_Eval.h"
#include "CBI_interpreter.h"
//#include "TIMER_FX2.H"

//-----------------------------------------------------------------------------
// Casio Basic inside
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Casio Basic Gloval variable
//-----------------------------------------------------------------------------
int  REGINT[59];

int CBint_CurrentValue=0;	// Ans

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
//-----------------------------------------------------------------------------
#define CBint_Eval  EvalIntsubTop
#define CBint_Eval1 EvalIntsub1

//-----------------------------------------------------------------------------
int MatOprandInt( char *SRC, int *reg){ 
	int dimA,dimB;
	int c=SRC[ExecPtr],d;
	if ( ( 'A'<=c )&&( c<='Z' ) ) {
		(*reg)=c-'A';
		ExecPtr++ ;
		
		if ( SRC[ExecPtr] != '[' ) { return -2; }	// [a,b]
		c=SRC[++ExecPtr];
		d=SRC[ExecPtr+1];
		if ( d == ',' ) {										// [a,
			ExecPtr++ ;
			if  ( ( '1'<= c ) && ( c<='9' ) ) dimA=c-'0';
			else if  ( ( 'A'<= c ) && ( c<='z' ) ) dimA=REGINT[c-'A'];
		} else
		if ( d == '+' ) { 										// [a+1,
			if ( SRC[ExecPtr+3] == ',' ) {
				ExecPtr+=2 ;
				if  ( ( '1'<= c ) && ( c<='9' ) ) dimA=c-'0';
				else if  ( ( 'A'<= c ) && ( c<='z' ) ) dimA=REGINT[c-'A'];
				c=SRC[ExecPtr++];
				if  ( ( '1'<= c ) && ( c<='9' ) ) dimA+=(c-'0');
				else if  ( ( 'A'<= c ) && ( c<='z' ) ) dimA+=REGINT[c-'A'];
			}
		} else
		if ( d == '-' ) { 										// [a-1,
			if ( SRC[ExecPtr+3] == ',' ) {
				ExecPtr+=2 ;
				if  ( ( '1'<= c ) && ( c<='9' ) ) dimA=c-'0';
				else if  ( ( 'A'<= c ) && ( c<='z' ) ) dimA=REGINT[c-'A'];
				c=SRC[ExecPtr++];
				if  ( ( '1'<= c ) && ( c<='9' ) ) dimA-=(c-'0');
				else if  ( ( 'A'<= c ) && ( c<='z' ) ) dimA-=REGINT[c-'A'];
			}
		} else {
			dimA=(CBint_Eval( SRC ));
			if ( SRC[ExecPtr] != ',' ) { CB_Error(SyntaxERR); return -1; }	// Syntax error
		}
		if ( ( dimA < 1 ) || ( MatArySizeA[(*reg)] < dimA ) ) { CB_Error(DimensionERR); return -1; }	// Dimension error
		c=SRC[++ExecPtr];
		d=SRC[ExecPtr+1];
		if ( d == ']' ) {										//    b]
			ExecPtr++ ;
			if  ( ( '1'<= c ) && ( c<='9' ) ) dimB=c-'0';
			else if  ( ( 'A'<= c ) && ( c<='z' ) ) dimB=REGINT[c-'A'];
		} else
		if ( d == '+' ) { 										//      b+1]
			if ( SRC[ExecPtr+3] == ']' ) {
				ExecPtr+=2 ;
				if  ( ( '1'<= c ) && ( c<='9' ) ) dimB=c-'0';
				else if  ( ( 'A'<= c ) && ( c<='z' ) ) dimB=REGINT[c-'A'];
				c=SRC[ExecPtr++];
				if  ( ( '1'<= c ) && ( c<='9' ) ) dimB+=(c-'0');
				else if  ( ( 'A'<= c ) && ( c<='z' ) ) dimB+=REGINT[c-'A'];
			}
		} else
		if ( d == '-' ) { 										//      b-1]
			if ( SRC[ExecPtr+3] == ']' ) {
				ExecPtr+=2 ;
				if  ( ( '1'<= c ) && ( c<='9' ) ) dimB=c-'0';
				else if  ( ( 'A'<= c ) && ( c<='z' ) ) dimB=REGINT[c-'A'];
				c=SRC[ExecPtr++];
				if  ( ( '1'<= c ) && ( c<='9' ) ) dimB-=(c-'0');
				else if  ( ( 'A'<= c ) && ( c<='z' ) ) dimB-=REGINT[c-'A'];
			}
		} else {
			dimB=(CBint_Eval( SRC ));
			if ( SRC[ExecPtr] != ']' ) { CB_Error(SyntaxERR); return -1; }	// Syntax error
		}
		if ( ( dimB < 1 ) || ( MatArySizeB[(*reg)] < dimB ) ) { CB_Error(DimensionERR); return -1; }	// Dimension error
		ExecPtr++ ;

		return (dimA-1)*MatArySizeB[(*reg)]+dimB-1;
	} else
	{ CB_Error(SyntaxERR); return -1; }	// Syntax error
	return -1; //
}
//----------------------------------------------------------------------------------------------

void CBint_Store( char *SRC ){	// ->
	int	st,en,i;
	int dimA,dimB,reg;
	int mptr;
	char*	MatAryC;
	short*	MatAryW;
	int*	MatAryI;
	
	int c=SRC[ExecPtr];
	if ( ( 'A' <= c ) && ( c <='z' ) ) {
		st=c-'A';
		ExecPtr++;
		if ( SRC[ExecPtr] == 0x7E ) {		// '~'
			ExecPtr++;
			c=SRC[ExecPtr];
			if ( ( 'A' <= c ) && ( c <='z' ) ) {
				en=c-'A';
				if ( en<st ) { CB_Error(SyntaxERR); return; }	// Syntax error
				c=SRC[++ExecPtr];
				if ( c=='#' ) { ExecPtr++;  for ( i=st; i<=en; i++) REG[ i ] = CBint_CurrentValue; }
				else
				if ( c=='%' ) ExecPtr++;
				for ( i=st; i<=en; i++) REGINT[ i ] = CBint_CurrentValue;
			}
		} else {
			c=SRC[ExecPtr];
			if ( c=='#' ) { ExecPtr++;  REG[st] = CBint_CurrentValue ; }
			else {
				if ( c=='%' ) ExecPtr++;
				REGINT[ st ] = CBint_CurrentValue;
			}
		}
	} else
	if ( c==0x7F ) {
		c = SRC[ExecPtr+1] ; 
		if ( c == 0x40 ) {	// Mat A[a,b]
			ExecPtr+=2;
			mptr=MatOprandInt( SRC, &reg);
			if ( ErrorNo ) return ; // error
			if ( mptr==-2 ) {	// Mat A
				dimA=MatArySizeA[reg];
				dimB=MatArySizeB[reg];
				switch ( MatAryElementSize[reg] ) {
					case 4:						// Matrix array int
						MatAryI=(int*)MatAry[reg];
						for (i=0; i<dimB*dimA; i++)
							MatAryI[i] = CBint_CurrentValue;
						break;
					case 1:						// Matrix array char
						MatAryC=(char*)MatAry[reg];
						for (i=0; i<dimB*dimA; i++)
							MatAryC[i] = CBint_CurrentValue;
						break;
					case 2:						// Matrix array word
						MatAryW=(short*)MatAry[reg];
						for (i=0; i<dimB*dimA; i++)
							MatAryW[i] = CBint_CurrentValue;
						break;
					case 8:						// Matrix array double
						for (i=0; i<dimB*dimA; i++)
							MatAry[reg][i] = CBint_CurrentValue;
						break;
					default:
						CB_Error(NoMatrixArrayERR); // No Matrix Array error
						break;
				}
			} else {	// Mat A[a,b]
				switch ( MatAryElementSize[reg] ) {
					case 4:						// Matrix array int
						MatAryI=(int*)MatAry[reg];
						MatAryI[mptr] = CBint_CurrentValue;
						break;
					case 1:						// Matrix array char
						MatAryC=(char*)MatAry[reg];
						MatAryC[mptr] = CBint_CurrentValue;
						break;
					case 2:						// Matrix array word
						MatAryW=(short*)MatAry[reg];
						MatAryW[mptr] = CBint_CurrentValue;
						break;
					case 8:						// Matrix array double
						MatAry[reg][mptr] = CBint_CurrentValue;
						break;
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
	if ( c==0xF9 ) {
		c = SRC[ExecPtr+1] ; 
		if ( c == 0x21 ) {	// Xdot
				if ( CBint_CurrentValue == 0 ) { ErrorNo=RangeERR; ErrorPtr=ExecPtr; return; }	// Range error
				ExecPtr+=2;
				Xdot = CBint_CurrentValue ;
				Xmax = Xmin + Xdot*126.;
		}
	} else
	if ( c=='%' ) {
		SetRtc( CBint_CurrentValue );
		CB_TicksAdjust=RTC_GetTicks()-CBint_CurrentValue;	// 
	} else { CB_Error(SyntaxERR); return; }	// Syntax error
}

//-----------------------------------------------------------------------------
void CBint_For( char *SRC ,StkFor *StackFor, CurrentStk *CurrentStruct ){
	int c;
	CBint_CurrentValue = CBint_Eval( SRC );
	c=SRC[ExecPtr];
	if ( c == 0x0E ) {	// ->
		ExecPtr++;
		c=SRC[ExecPtr];
		if ( ( 'A' <= c ) && ( c <= 'z' ) ) {
		StackFor->Var[StackFor->Ptr]=c-'A';
		CBint_Store(SRC);
		} else { CB_Error(SyntaxERR); return; }	// Syntax error
	}
	if ( StackFor->Ptr >= StackForMax-1 ) { CB_Error(NestingERR); return; } //  nesting error
	c=SRC[ExecPtr];
	if ( ( c != 0xFFFFFFF7 ) || ( SRC[ExecPtr+1] != 0x05 ) ) { CB_Error(SyntaxERR); return; }	// Syntax error
	ExecPtr+=2;
	StackFor->IntEnd[StackFor->Ptr] = CBint_Eval( SRC );
	c=SRC[ExecPtr];
	if ( ( c == 0xFFFFFFF7 ) && ( SRC[ExecPtr+1] == 0x06 ) ) {	// Step
		ExecPtr+=2;
		StackFor->IntStep[StackFor->Ptr] = CBint_Eval( SRC );
	} else {
		StackFor->IntStep[StackFor->Ptr] = 1;
	}
	StackFor->Adrs[StackFor->Ptr] = ExecPtr;
	StackFor->Ptr++;
	CurrentStruct->loop[CurrentStruct->CNT]=1;
	CurrentStruct->CNT++;
}
void CBint_Next( char *SRC ,StkFor *StackFor, CurrentStk *CurrentStruct ){
	int step,end;
	if ( StackFor->Ptr <= 0 ) { ErrorNo=NextWithoutForERR; ErrorPtr=ExecPtr; return; } // Next without for error
	StackFor->Ptr--;
	CurrentStruct->CNT--;
	step = StackFor->IntStep[StackFor->Ptr];
	REGINT[StackFor->Var[StackFor->Ptr]]+=step;
	if ( step > 0 ) { 	// step +
		if ( REGINT[StackFor->Var[StackFor->Ptr]] > StackFor->IntEnd[StackFor->Ptr] ) return ; // exit
		ExecPtr = StackFor->Adrs[StackFor->Ptr];
		(StackFor->Ptr)++;		// continue
	CurrentStruct->loop[CurrentStruct->CNT]=1;
	CurrentStruct->CNT++;
	}
	else {									// step -
		if ( REGINT[StackFor->Var[StackFor->Ptr]] < StackFor->IntEnd[StackFor->Ptr] ) return ; // exit
		ExecPtr = StackFor->Adrs[StackFor->Ptr];
		StackFor->Ptr++;		// continue
	CurrentStruct->loop[CurrentStruct->CNT]=1;
	CurrentStruct->CNT++;
	}
}
//-----------------------------------------------------------------------------

void CBint_Dsz( char *SRC ) { //	Dsz
	int c;
	int reg,mptr;
	char*	MatAryC;
	short*	MatAryW;
	int*	MatAryI;
	c=SRC[ExecPtr];
	if ( ( 'A' <= c ) && ( c <= 'z' ) ) {
		ExecPtr++;
		reg=c-'A';
		c=SRC[ExecPtr];
		if ( c=='#' ) {
			ExecPtr++;
			REG[reg] --;
			CBint_CurrentValue = REG[reg] ;
		} else {
			if ( c=='%' ) ExecPtr++;
			REGINT[reg] --;
			CBint_CurrentValue = REGINT[reg] ;
		}
	} else 
	if ( c==0x7F ) {
		c = SRC[ExecPtr+1] ; 
		if ( c == 0x40 ) {	// Mat A[a,b]
			ExecPtr+=2;
			mptr=MatOprandInt( SRC, &reg);
			if ( mptr==-2 ) { CB_Error(SyntaxERR); return; }	// Syntax error
			if ( ErrorNo ) return ; // error
			switch ( MatAryElementSize[reg] ) {
				case 4:
					MatAryI=(int*)MatAry[reg];
					MatAryI[mptr] --;
					CBint_CurrentValue = MatAryI[mptr]   ;			// Matrix array int
					break;
				case 1:
					MatAryC=(char*)MatAry[reg];
					MatAryC[mptr] --;
					CBint_CurrentValue = MatAryC[mptr]   ;			// Matrix array char
					break;
				case 2:
					MatAryW=(short*)MatAry[reg];
					MatAryW[mptr] --;
					CBint_CurrentValue = MatAryW[mptr]   ;			// Matrix array word
					break;
				case 8:
					MatAry[reg][mptr] --;
					CBint_CurrentValue = MatAry[reg][mptr];			// Matrix array double
					break;
			}
		}
	} else { CB_Error(SyntaxERR); return; }	// Syntax error

	c=SRC[ExecPtr++];
	if ( ( c==':' ) || ( c==0x0D ) ) {
		if ( CBint_CurrentValue ) return ;
		else { ExecPtr++;
			Skip_block(SRC);
		}
	} else if ( c==0x0C ) {  // dsps
		ExecPtr++;
		CB_Disps( SRC ,2);
		if ( CBint_CurrentValue ) return ;
		else {
			Skip_block(SRC);
		}
	} else { ErrorNo=SyntaxERR; ErrorPtr=ExecPtr; return; }	// Syntax error
}

void CBint_Isz( char *SRC ) { //	Isz
	int c;
	int reg,mptr;
	char*	MatAryC;
	short*	MatAryW;
	int*	MatAryI;
	c=SRC[ExecPtr];
	if ( ( 'A' <= c ) && ( c <= 'z' ) ) {
		ExecPtr++;
		reg=c-'A';
		c=SRC[ExecPtr];
		if ( c=='#' ) {
			ExecPtr++;
			REG[reg] ++;
			CBint_CurrentValue = REG[reg] ;
		} else {
			if ( c=='%' ) ExecPtr++;
			REGINT[reg] ++;
			CBint_CurrentValue = REGINT[reg] ;
		}
	} else 
	if ( c==0x7F ) {
		c = SRC[ExecPtr+1] ; 
		if ( c == 0x40 ) {	// Mat A[a,b]
			ExecPtr+=2;
			mptr=MatOprandInt( SRC, &reg);
			if ( mptr==-2 ) { CB_Error(SyntaxERR); return; }	// Syntax error
			if ( ErrorNo ) return ; // error
			switch ( MatAryElementSize[reg] ) {
				case 4:
					MatAryI=(int*)MatAry[reg];
					MatAryI[mptr] ++;
					CBint_CurrentValue = MatAryI[mptr]   ;			// Matrix array int
					break;
				case 1:
					MatAryC=(char*)MatAry[reg];
					MatAryC[mptr] ++;
					CBint_CurrentValue = MatAryC[mptr]   ;			// Matrix array char
					break;
				case 2:
					MatAryW=(short*)MatAry[reg];
					MatAryW[mptr] ++;
					CBint_CurrentValue = MatAryW[mptr]   ;			// Matrix array word
					break;
				case 8:
					MatAry[reg][mptr] ++;
					CBint_CurrentValue = MatAry[reg][mptr];			// Matrix array double
					break;
			}
		}
	} else { CB_Error(SyntaxERR); return; }	// Syntax error

	c=SRC[ExecPtr++];
	if ( ( c==':' ) || ( c==0x0D ) ) {
		if ( CBint_CurrentValue ) return ;
		else { ExecPtr++;
			Skip_block(SRC);
		}
	} else if ( c==0x0C ) {  // dsps
		ExecPtr++;
		CB_Disps( SRC ,2);
		if ( CBint_CurrentValue ) return ;
		else {
			Skip_block(SRC);
		}
	} else { ErrorNo=SyntaxERR; ErrorPtr=ExecPtr; return; }	// Syntax error
}

//----------------------------------------------------------------------------------------------

void CBint_PxlOprand( char *SRC, int *py, int *px) {
	int x,y;
	y = (CBint_Eval( SRC ));
	*py=y;
	if ( ( (*py)<1 ) || ( (*py)>63 ) ) { CB_Error(ArgumentERR); return; }  // Argument error
	if ( SRC[ExecPtr] != ',' ) { CB_Error(SyntaxERR); return; }  // Syntax error
	ExecPtr++;
	x = (CBint_Eval( SRC ));
	*px=x;
	if ( ( (*px)<1 ) || ( (*px)>127 ) ) { CB_Error(ArgumentERR); return; }  // Argument error}
	CB_SelectGraphVRAM();	// Select Graphic Screen
}
void CBint_PxlOn( char *SRC ) { //	PxlOn
	int x,y;
	if ( RangeErrorCK(SRC) ) return;
	CBint_PxlOprand( SRC, &y, &x);
	Bdisp_SetPoint_VRAM(x, y, 1);
//	regintX = (    px-1)*Xdot  + Xmin ;
//	regintY = ( 62-py+1)*Ydot  + Ymin ;
	Bdisp_PutDisp_DD_DrawBusy_skip_through(SRC);
}
void CBint_PxlOff( char *SRC ) { //	PxlOff
	int x,y;
	if ( RangeErrorCK(SRC) ) return;
	CBint_PxlOprand( SRC, &y, &x);
	Bdisp_SetPoint_VRAM(x, y, 0);
//	regintX = (    px-1)*Xdot  + Xmin ;
//	regintY = ( 62-py+1)*Ydot  + Ymin ;
	Bdisp_PutDisp_DD_DrawBusy_skip_through(SRC);
}
void CBint_PxlChg( char *SRC ) { //	PxlChg
	int x,y;
	if ( RangeErrorCK(SRC) ) return;
	CBint_PxlOprand( SRC, &y, &x);
	if (Bdisp_GetPoint_VRAM(x, y)) 
		Bdisp_SetPoint_VRAM(x, y, 0);
	else
		Bdisp_SetPoint_VRAM(x, y, 1);
//	regintX = (    px-1)*Xdot  + Xmin ;
//	regintY = ( 62-py+1)*Ydot  + Ymin ;
	Bdisp_PutDisp_DD_DrawBusy_skip_through(SRC);
}

//----------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------
int CBint_BinaryEval( char *SRC ) {	// eval 2
	int c,op;
	int reg,mptr,opPtr;
	int src,dst;
	char*	MatAryC;
	short*	MatAryW;
	int*	MatAryI;
	
	c=SRC[ExecPtr];
	if ( ( 'A' <= c ) && ( c <= 'z' ) ) {
		ExecPtr++;
		reg=c-'A';
		c=SRC[ExecPtr];
		if ( c=='#' ) {
			ExecPtr++;
			src = REG[reg] ;
		} else {
			if ( c=='%' ) ExecPtr++;
			src = REGINT[reg] ;
		}
	} else 
	if ( c==0x7F ) {
		c = SRC[ExecPtr+1] ; 
		if ( c == 0x40 ) {	// Mat A[a,b]
			ExecPtr+=2;
			mptr=MatOprandInt( SRC, &reg);
			if ( ErrorNo ) return ; // error
			switch ( MatAryElementSize[reg] ) {
				case 4:
					MatAryI=(int*)MatAry[reg];
					src = MatAryI[mptr] ;			// Matrix array int
					break;
				case 1:
					MatAryC=(char*)MatAry[reg];
					src = MatAryC[mptr] ;			// Matrix array char
					break;
				case 2:
					MatAryW=(short*)MatAry[reg];
					src = MatAryW[mptr] ;			// Matrix array word
					break;
				case 8:
					src = MatAry[reg][mptr] ;		// Matrix array doubl
					break;
			}
		}
	} else  src = CBint_Eval1(SRC);

	opPtr=ExecPtr;
	op=SRC[ExecPtr++];	
	c=SRC[ExecPtr];
	if ( ( 'A' <= c ) && ( c <= 'z' ) ) {
		ExecPtr++;
		reg=c-'A';
		c=SRC[ExecPtr];
		if ( c=='#' ) {
			ExecPtr++;
			dst = REG[reg] ;
		} else {
			if ( c=='%' ) ExecPtr++;
			dst = REGINT[reg] ;
		}
	} else 
	if ( c==0x7F ) {
		c = SRC[ExecPtr+1] ; 
		if ( c == 0x40 ) {	// Mat A[a,b]
			ExecPtr+=2;
			mptr=MatOprandInt( SRC, &reg);
			if ( ErrorNo ) return ; // error
			switch ( MatAryElementSize[reg] ) {
				case 4:
					MatAryI=(int*)MatAry[reg];
					dst = MatAryI[mptr] ;			// Matrix array int
					break;
				case 1:
					MatAryC=(char*)MatAry[reg];
					dst = MatAryC[mptr] ;			// Matrix array char
					break;
				case 2:
					MatAryW=(short*)MatAry[reg];
					dst = MatAryW[mptr] ;			// Matrix array word
					break;
				case 8:
					dst = MatAry[reg][mptr] ;		// Matrix array doubl
					break;
			}
		}
	} else  dst = CBint_Eval1(SRC);

//	ExecPtr++;
	switch ( op ) {
			case 0xFFFFFF89 :		// +
				return src + dst;
			case 0xFFFFFF99 :		// -
				return src - dst;
			case 0xFFFFFFA9 :		// ~
				return src * dst;
			case 0xFFFFFFB9 :		// €
				if ( dst == 0 ) CB_Error(DivisionByZeroERR); // Division by zero error 
				return src / dst;
			case '=' :	// =
				return src == dst;
			case '>' :	// >
				return src >  dst;
			case '<' :	// <
				return src <  dst;
			case 0x11 :	// !=
				return src != dst;
			case 0x12 :	// >=
				return src >= dst;
			case 0x10 :	// <=
				return src <= dst;
			case 0xFFFFFFB0 :	// And
				return (int)src & (int)dst;
			case 0xFFFFFFB1 :	// Or
				return (int)src | (int)dst;
			default:
				{ ErrorNo=SyntaxERR; ErrorPtr=opPtr; return; }  // Syntax error
				break;
	}
}

int CBint_UnaryEval( char *SRC ) {	// eval 1
	int c,op;
	int reg,mptr,opPtr;
	int src,dst;
	char*	MatAryC;
	short*	MatAryW;
	int*	MatAryI;
	
	c=SRC[ExecPtr];
	if ( ( 'A' <= c ) && ( c <= 'z' ) ) {
		ExecPtr++;
		reg=c-'A';
		c=SRC[ExecPtr];
		if ( c=='#' ) {
			ExecPtr++;
			return REG[reg] ;
		} else {
			if ( c=='%' ) ExecPtr++;
			return REGINT[reg] ;
		}
	} else 
	if ( c==0x7F ) {
		c = SRC[ExecPtr+1] ; 
		if ( c == 0x40 ) {	// Mat A[a,b]
			ExecPtr+=2;
			mptr=MatOprandInt( SRC, &reg);
			if ( ErrorNo ) return ; // error
			switch ( MatAryElementSize[reg] ) {
				case 4:
					MatAryI=(int*)MatAry[reg];
					return  MatAryI[mptr] ;			// Matrix array int
					break;
				case 1:
					MatAryC=(char*)MatAry[reg];
					return  MatAryC[mptr] ;			// Matrix array char
					break;
				case 2:
					MatAryW=(short*)MatAry[reg];
					return  MatAryW[mptr] ;			// Matrix array word
					break;
				case 8:
					return  MatAry[reg][mptr] ;		// Matrix array doubl
					break;
			}
		}
	} else  return CBint_Eval1(SRC);
}

//----------------------------------------------------------------------------------------------

int q_un_subint(void) {
     int i;
     int a[9];
     for (i=0;i<9;i++) a[i]=0;
     regintN=1;  
 l5: regintR=8;
     regintS=0;
     regintX=0;
 l0: if (regintX==regintR) goto l4;
     regintX++;
     a[(int)regintX]=regintR;
 l1: ++regintS;
     regintY=regintX;
 l2: if (!--regintY) goto l0;
     if (!(regintT=a[(int)regintX]-a[(int)regintY])) goto l3;
     if (regintX-regintY!=abs(regintT)) goto l2;
 l3: if (--a[(int)regintX]) goto l1;
     if (--regintX) goto l3;
 l4: if (--regintN) goto l5;

  return (regintS);
}

int asctt4int(){
	regintK=1;
	lbl0:
	regintK=regintK+1;
	regintA=regintK/3*regintK+4-regintK;
	if ( regintK<10000 ) goto lbl0;
	return regintA;;
}


void CBint_test() {
	int i, s, t, result;
	char	buffer[32];
	unsigned int key=0;

	Bdisp_AllClr_DDVRAM();
	locate(1,1); Print((unsigned char*)"8queen int");
	Bdisp_PutDisp_DD();
	
	start_timer();	// --------------------------------------

	regS=q_un_subint();

	t=get_timer();	// --------------------------------------

	locate(1,2);
	sprintG(buffer, regS, 21, RIGHT_ALIGN);
		Print((unsigned char*)buffer);
	locate(1,3);sprintf((char*)buffer,"time=%5.3fs",(float)t/1000); Print((unsigned char*)buffer);
	Bdisp_PutDisp_DD();

	
	locate(1,5); Print((unsigned char*)"Asciit4 int");
	Bdisp_PutDisp_DD();
	
	start_timer();	// --------------------------------------

	regS=asctt4int();

	t=get_timer();	// --------------------------------------

	locate(1,6);
	sprintG(buffer, regS, 21, RIGHT_ALIGN);
		Print((unsigned char*)buffer);
	locate(1,7);sprintf((char*)buffer,"time=%5.3fs",(float)t/1000); Print((unsigned char*)buffer);

	Bdisp_PutDisp_DD();
	GetKey(&key);
}


