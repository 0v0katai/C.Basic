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
int RangeErrorCKint( unsigned char *SRC ) {
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
int MatOprandInt( unsigned char *SRC, int *reg){ 
	int dimA,dimB;
	unsigned int c=SRC[ExecPtr];
	if ( ( 'A'<=c )&&( c<='Z' ) ) {
		(*reg)=c-'A';
		ExecPtr++ ;
		if ( SRC[ExecPtr] != '[' ) { CB_Error(SyntaxERR); return -1; }	// Syntax error
		c=SRC[++ExecPtr];
		if ( SRC[ExecPtr+1] == ',' ) {
			ExecPtr++ ;
			if  ( ( '1'<= c ) && ( c<='9' ) ) dimA=c-'0';
			else if  ( ( 'A'<= c ) && ( c<='z' ) ) dimA=REGINT[c-'A'];
		} else {
			dimA=(CBint_Eval( SRC ));
			if ( SRC[ExecPtr] != ',' ) { CB_Error(SyntaxERR); return -1; }	// Syntax error
		}
		if ( ( dimA < 1 ) || ( MatArySizeA[(*reg)] < dimA ) ) { CB_Error(DimensionERR); return -1; }	// Dimension error
		c=SRC[++ExecPtr];
		if ( SRC[ExecPtr+1] == ']' ) {
			ExecPtr++ ;
			if  ( ( '1'<= c ) && ( c<='9' ) ) dimB=c-'0';
			else if  ( ( 'A'<= c ) && ( c<='z' ) ) dimB=REGINT[c-'A'];
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

void CBint_Store( unsigned char *SRC ){	// ->
	int	st,en,i;
	int dimA,dimB,reg;
	int mptr;
	char*	MatAryC;
	short*	MatAryW;
	int*	MatAryI;
	
	unsigned int c=SRC[ExecPtr];
	if ( ( 'A' <= c ) && ( c <='z' ) ) {
		st=c-'A';
		ExecPtr++;
		if ( SRC[ExecPtr] == 0x7E ) {		// '~'
			ExecPtr++;
			c=SRC[ExecPtr];
			if ( ( 'A' <= c ) && ( c <='z' ) ) {
				en=c-'A';
				if ( en<st ) { CB_Error(SyntaxERR); return; }	// Syntax error
				ExecPtr++;	
				for ( i=st; i<=en; i++) REGINT[ i ] = CBint_CurrentValue;
			}
		} else {
			REGINT[ st ] = CBint_CurrentValue;
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
					MatAryI[mptr] = CBint_CurrentValue;			// Matrix array int
					break;
				case 1:
					MatAryC=(char*)MatAry[reg];
					MatAryC[mptr] = CBint_CurrentValue;			// Matrix array char
					break;
				case 2:
					MatAryW=(short*)MatAry[reg];
					MatAryW[mptr] =  CBint_CurrentValue;			// Matrix array word
					break;
				case 8:
					MatAry[reg][mptr] = CBint_CurrentValue;		// Matrix array double
					break;
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
	} else { CB_Error(SyntaxERR); return; }	// Syntax error
}

int  CBint_Input( unsigned char *SRC ){
	unsigned int c;
	int DefaultValue=0;
	int flag=0;
	int reg,bptr,mptr;
	unsigned char buffer[32];
	char*	MatAryC;
	short*	MatAryW;
	int*	MatAryI;
	
	KeyRecover();
	CB_SelectTextDD();	// Select Text Screen
	if ( CursorX==22 ) CursorX=1;
	locate( CursorX, CursorY); Print((unsigned char*)"?");
	Scrl_Y();

	c=SRC[ExecPtr];
	bptr=ExecPtr;
	if ( c==0x0E ) {
		flag=0;
	} else
	if ( ( 'A' <= c ) && ( c <='z' ) ) {
		DefaultValue = REGINT[ c-'A' ];
		flag=1;
	} else
	if ( c==0x7F ) {
		c = SRC[ExecPtr+1] ; 
		if ( c == 0x40 ) {	// Mat A[a,b]
			ExecPtr+=2;
			mptr=MatOprandInt( SRC, &reg);
			if ( ErrorNo ) return ; // error
			switch ( MatAryElementSize[reg] ) {
				case 8:
					DefaultValue = MatAry[reg][mptr];			// Matrix array double
					break;
				case 1:
					MatAryC=(char*)MatAry[reg];
					DefaultValue = MatAryC[mptr]   ;			// Matrix array char
					break;
				case 2:
					MatAryW=(short*)MatAry[reg];
					DefaultValue = MatAryW[mptr]   ;			// Matrix array word
					break;
				case 4:
					MatAryI=(int*)MatAry[reg];
					DefaultValue = MatAryI[mptr]   ;			// Matrix array int
					break;
			}
			flag=1;
			ExecPtr=bptr;
		} else if ( c == 0x00 ) {	// Xmin
				DefaultValue = Xmin ;
				flag=1;
		} else if ( c == 0x01 ) {	// Xmax
				DefaultValue = Xmax ;
				flag=1;
		} else if ( c == 0x02 ) {	// Xscl
				DefaultValue = Xscl ;
				flag=1;
		} else if ( c == 0x04 ) {	// Ymin
				DefaultValue = Ymin ;
				flag=1;
		} else if ( c == 0x05 ) {	// Ymax
				DefaultValue = Ymax ;
				flag=1;
		} else if ( c == 0x06) {	// Yscl
				DefaultValue = Yscl ;
				flag=1;
		} else if ( c == 0x0B ) {	// Xfct
				DefaultValue = Xfct ;
				flag=1;
		} else if ( c == 0x0C ) {	// Yfct
				DefaultValue = Yfct ;
				flag=1;
		} else { CB_Error(SyntaxERR); return; }	// Syntax error	} else
	} else
	if ( c==0xF9 ) {
		c = SRC[ExecPtr+1] ; 
		if ( c == 0x21 ) {	// Xdot
				DefaultValue = Xdot ;
				flag=1;
		} else { CB_Error(SyntaxERR); return; }	// Syntax error
	} else { CB_Error(SyntaxERR); return; }	// Syntax error

//	if ( flag ) {
//			CBint_CurrentValue = InputNumD_CB2( 1, CursorY, 21, DefaultValue );
//			CBint_Store( SRC );
//	} else	CBint_CurrentValue = InputNumD_CB( 1, CursorY, 21, 0 );

	if ( flag ) {
			sprintGR(buffer, DefaultValue, 22-CursorX,RIGHT_ALIGN, CB_Round.MODE, CB_Round.DIGIT);
			locate( CursorX, CursorY); Print((unsigned char*)buffer);
			Scrl_Y();
			CBint_CurrentValue = InputNumD_CB1( 1, CursorY, 21, DefaultValue );
			CBint_Store( SRC );
	} else	CBint_CurrentValue = InputNumD_CB( 1, CursorY, 21, 0 );
	Scrl_Y();
	Bdisp_PutDisp_DD_DrawBusy();
	return 0 ;
}
//-----------------------------------------------------------------------------
void CBint_Locate( unsigned char *SRC ){
	unsigned char buffer[32];
	unsigned int c;
	int lx,ly;
	int value;

	CB_SelectTextVRAM();	// Select Text Screen
	lx = CBint_Eval( SRC );
	c=SRC[ExecPtr];
	if ( c != ',' ) { CB_Error(SyntaxERR); return; }	// Syntax error
	ExecPtr++;
	ly = CBint_Eval( SRC );
	c=SRC[ExecPtr];
	if ( c != ',' ) { CB_Error(SyntaxERR); return; }	// Syntax error
	ExecPtr++;
	c=SRC[ExecPtr];
	if ( c == 0x22 ) {	// String
		ExecPtr++;
		GetLocateStr(SRC, buffer, 32);
		CB_Print( lx,ly, (unsigned char*)buffer );
	} else {			// expression
		value = CBint_Eval( SRC );
		sprintGR(buffer, value, 22-lx,LEFT_ALIGN, CB_Round.MODE, CB_Round.DIGIT);
		CB_Print( lx,ly, (unsigned char*)buffer );
	}
	Bdisp_PutDisp_DD_DrawBusy_through(SRC);
}

void CBint_Text( unsigned char *SRC, int *dspflag ) { //	Text
	unsigned int key;
	unsigned char buffer[64];
	unsigned int c;
	int px,py,d;
	int value;

	if ( RangeErrorCK(SRC) ) return;
	CB_SelectGraphVRAM();	// Select Graphic Screen
	py=CBint_Eval( SRC );
	c=SRC[ExecPtr];
	if ( c != ',' ) { CB_Error(SyntaxERR); return; }	// Syntax error
	ExecPtr++;
	px=CBint_Eval( SRC );
	c=SRC[ExecPtr];
	if ( c != ',' ) { CB_Error(SyntaxERR); return; }	// Syntax error
	ExecPtr++;
	c=SRC[ExecPtr];
	if ( c == 0x22 ) {	// String
		ExecPtr++;
		GetLocateStr(SRC, buffer, 64);
		Text(py, px, buffer);
	} else {			// expression
		value = CBint_Eval( SRC );
		d=(128-px)/4;
		if (d>24) d=24;	// digit max
		sprintGR(buffer, value, d,LEFT_ALIGN, CB_Round.MODE, CB_Round.DIGIT);
		Text(py, px, buffer);
	}
	Bdisp_PutDisp_DD_DrawBusy_through(SRC);
}
//-----------------------------------------------------------------------------
void CBint_If( unsigned char *SRC ){
	unsigned int c,c2;
	int value;
	int ptr;
	value = CBint_Eval( SRC );
	c =SRC[ExecPtr];
	if ( ( c == ':'  ) || ( c == 0x0D ) )  { ExecPtr++;  c=SRC[ExecPtr]; }
	c2=SRC[ExecPtr+1];
	if ( ( c != 0xF7 ) || ( c2 != 0x01 ) ) { CB_Error(SyntaxERR); return; } // not Then error 
	ExecPtr+=2 ;
	if ( value == 0 ) {		// false
		if ( Search_ElseIfend(SRC) ) {	// Else or IfEnd
			return ;
		}
	}
}

//-----------------------------------------------------------------------------
void CBint_For( unsigned char *SRC ,int *StackForPtr, int *StackForAdrs, int *StackForVar, int *StackForEnd, int *StackForStep, int *CurrentStructCNT, int *CurrentStructloop){
	unsigned int c;
	CBint_CurrentValue = CBint_Eval( SRC );
	c=SRC[ExecPtr];
	if ( c == 0x0E ) {	// ->
		ExecPtr++;
		c=SRC[ExecPtr];
		if ( ( 'A' <= c ) && ( c <='z' ) ) {
		StackForVar[*StackForPtr]=c-'A';
		CBint_Store(SRC);
		} else { CB_Error(SyntaxERR); return; }	// Syntax error
	}
	if ( *StackForPtr >= StackForMax-1 ) { CB_Error(NestingERR); return; } //  nesting error
	c=SRC[ExecPtr];
	if ( ( c != 0xF7 ) || ( SRC[ExecPtr+1] != 0x05 ) ) { CB_Error(SyntaxERR); return; }	// Syntax error
	ExecPtr+=2;
	StackForEnd[*StackForPtr] = CBint_Eval( SRC );
	c=SRC[ExecPtr];
	if ( ( c == 0xF7 ) && ( SRC[ExecPtr+1] == 0x06 ) ) {	// Step
		ExecPtr+=2;
		StackForStep[*StackForPtr] = CBint_Eval( SRC );
	} else {
		StackForStep[*StackForPtr] = 1;
	}
	StackForAdrs[*StackForPtr] = ExecPtr;
	(*StackForPtr)++;
	CurrentStructloop[*CurrentStructCNT]=1;
	(*CurrentStructCNT)++;
}

void CBint_Next( unsigned char *SRC ,int *StackForPtr, int *StackForAdrs, int *StackForVar, int *StackForEnd, int *StackForStep, int *CurrentStructCNT, int *CurrentStructloop ){
	int step,end;
	if ( *StackForPtr <= 0 ) { ErrorNo=NextWithoutForERR; ErrorPtr=ExecPtr; return; } // Next without for error
	(*StackForPtr)--;
	(*CurrentStructCNT)--;
	step = StackForStep[*StackForPtr];
	REGINT[StackForVar[*StackForPtr]]+=step;
	if ( step > 0 ) { 	// step +
		if ( REGINT[StackForVar[*StackForPtr]] > StackForEnd[*StackForPtr] ) return ; // exit
		ExecPtr = StackForAdrs[*StackForPtr];
		(*StackForPtr)++;		// continue
	CurrentStructloop[*CurrentStructCNT]=1;
	(*CurrentStructCNT)++;
	}
	else {									// step -
		if ( REGINT[StackForVar[*StackForPtr]] < StackForEnd[*StackForPtr] ) return ; // exit
		ExecPtr = StackForAdrs[*StackForPtr];
		(*StackForPtr)++;		// continue
	CurrentStructloop[*CurrentStructCNT]=1;
	(*CurrentStructCNT)++;
	}
}
//-----------------------------------------------------------------------------
void CBint_While( unsigned char *SRC, int *StackWhilePtr, int *StackWhileAdrs, int *CurrentStructCNT, int *CurrentStructloop  ) {
	int wPtr=ExecPtr;
	if ( CBint_Eval( SRC ) == 0 ) {		// false
		if ( Search_WhileEnd(SRC) == 0 ) { CB_Error(WhileWithoutWhileEndERR); return; }  // While without WhileEnd error
		return ; // exit
	}
	if ( *StackWhilePtr >= StackWhileMax-1 ) { CB_Error(NestingERR); return; }  //  nesting error
	StackWhileAdrs[*StackWhilePtr] = wPtr;
	(*StackWhilePtr)++;
	CurrentStructloop[*CurrentStructCNT]=2;
	(*CurrentStructCNT)++;
}

void CBint_WhileEnd( unsigned char *SRC, int *StackWhilePtr, int *StackWhileAdrs, int *CurrentStructCNT, int *CurrentStructloop  ) {
	int exitPtr=ExecPtr;
	if ( *StackWhilePtr <= 0 ) { CB_Error(WhileEndWithoutWhileERR); return; }  // WhileEnd without While error
	(*StackWhilePtr)--;
	(*CurrentStructCNT)--;
	ExecPtr = StackWhileAdrs[*StackWhilePtr] ;
	if ( CBint_Eval( SRC ) == 0 ) {		// false
		ExecPtr=exitPtr;
		return ; // exit
	}
	(*StackWhilePtr)++;
	CurrentStructloop[*CurrentStructCNT]=2;
	(*CurrentStructCNT)++;
}

void CBint_Do( unsigned char *SRC, int *StackDoPtr, int *StackDoAdrs, int *CurrentStructCNT, int *CurrentStructloop ) {
	if ( *StackDoPtr >= StackDoMax-1 ) { CB_Error(NestingERR); return; }  //  nesting error
	StackDoAdrs[*StackDoPtr] = ExecPtr;
	(*StackDoPtr)++;
	CurrentStructloop[*CurrentStructCNT]=3;
	(*CurrentStructCNT)++;
}

void CBint_LpWhile( unsigned char *SRC, int *StackDoPtr, int *StackDoAdrs, int *CurrentStructCNT, int *CurrentStructloop ) {
	if ( *StackDoPtr <= 0 ) { CB_Error(LpWhileWithoutDoERR); return; }  // LpWhile without Do error
	(*StackDoPtr)--;
	(*CurrentStructCNT)--;
	if ( CBint_Eval( SRC ) == 0  ) return ; // exit
	ExecPtr = StackDoAdrs[*StackDoPtr] ;				// true
	(*StackDoPtr)++;
	CurrentStructloop[*CurrentStructCNT]=3;
	(*CurrentStructCNT)++;
}

void CBint_Break( unsigned char *SRC, int *StackForPtr, int *StackWhilePtr, int *StackDoPtr, int *CurrentStructCNT, int *CurrentStructloop ) {
	
	if ( *CurrentStructCNT <=0 ) { CB_Error(NotLoopERR); return; }  // Not Loop error
	(*CurrentStructCNT)--;
	switch ( CurrentStructloop[*CurrentStructCNT] ) {
		case 1:	// For Next
			if ( Search_Next(SRC) == 0 )     { CB_Error(ForWithoutNextERR); return; }  // For without Next error
			(*StackForPtr)--;
			return ;
			break;
		case 2:	// While WhileEnd
			if ( Search_WhileEnd(SRC) == 0 ) { CB_Error(WhileWithoutWhileEndERR); return; }  // While without WhileEnd error
			(*StackWhilePtr)--;
			return ;
			break;
		case 3:	// DO LpWhile
			if ( Search_LpWhile(SRC) == 0 )  { CB_Error(DoWithoutLpWhileERR); return; }  // Do without LpWhile error
			(*StackDoPtr)--;
			return ;
			break;
		default:
			break;
	}
}

//-----------------------------------------------------------------------------

void CBint_Cond( unsigned char *SRC ) { //	=> Conditional jump
	unsigned int c;
	if ( CBint_CurrentValue ) {		// true
		return ;
	} else {							// false
		Skip_block(SRC);
	}
}

void CBint_Dsz( unsigned char *SRC ) { //	Dsz
	unsigned int c;
	int reg,mptr;
	char*	MatAryC;
	short*	MatAryW;
	int*	MatAryI;
	c=SRC[ExecPtr];
	if ( ( 'A' <= c ) && ( c <='z' ) ) {
		ExecPtr++;
		reg=c-'A';
		REGINT[reg] --;
		CBint_CurrentValue = REGINT[reg] ;
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
		else {
			Skip_block(SRC);
		}
	} else if ( c==0x0C ) {  // dsps
		CB_Disps( SRC ,2);
		if ( CBint_CurrentValue ) return ;
		else {
			Skip_block(SRC);
		}
	} else { ErrorNo=SyntaxERR; ErrorPtr=ExecPtr-1; return; }	// Syntax error
}

void CBint_Isz( unsigned char *SRC ) { //	Isz
	unsigned int c;
	int reg,mptr;
	char*	MatAryC;
	short*	MatAryW;
	int*	MatAryI;
	c=SRC[ExecPtr];
	if ( ( 'A' <= c ) && ( c <='z' ) ) {
		ExecPtr++;
		reg=c-'A';
		REGINT[reg] ++ ;
		CBint_CurrentValue = REGINT[reg] ;
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
		else {
			Skip_block(SRC);
		}
	} else if ( c==0x0C ) {  // dsps
		CB_Disps( SRC ,2);
		if ( CBint_CurrentValue ) return ;
		else {
			Skip_block(SRC);
		}
	} else { ErrorNo=SyntaxERR; ErrorPtr=ExecPtr-1; return; }	// Syntax error
}

//----------------------------------------------------------------------------------------------

void CBint_ViewWindow( unsigned char *SRC ) { //	ViewWindow
	unsigned int c;
	int reg=0;
	while ( reg <= 10 ) {
		c=SRC[ExecPtr];
		if ( (c==':') || (c==0x0C) || (c==0x0D) || (c==0x00) ) break;
		REGv[reg]=CBint_Eval( SRC );
		c=SRC[ExecPtr];
		if ( (c==':') || (c==0x0C) || (c==0x0D) || (c==0x00) ) break;
		if ( c != ',' ) { CB_Error(SyntaxERR); return; }  // Syntax error
		ExecPtr++;
		reg++;
	}
	CB_SelectGraphVRAM();	// Select Graphic Screen
	ViewWindow( Xmin, Xmax, Xscl, Ymin, Ymax, Yscl);
	CB_SelectTextVRAM();	// Select Text Screen
//	Bdisp_PutDisp_DD_DrawBusy_skip();
}

void CBint_FLine( unsigned char *SRC) { //	F-Line
	int c;
	int x1,y1,x2,y2;
	int style=S_L_Style;

	if ( RangeErrorCK(SRC) ) return;
	x1=CBint_Eval( SRC );
	c=SRC[ExecPtr];
	if ( c != ',' ) { CB_Error(SyntaxERR); return; }  // Syntax error
	ExecPtr++;
	y1=CBint_Eval( SRC );
	c=SRC[ExecPtr];
	if ( c != ',' ) { CB_Error(SyntaxERR); return; }  // Syntax error
	ExecPtr++;
	x2=CBint_Eval( SRC );
	c=SRC[ExecPtr];
	if ( c != ',' ) { CB_Error(SyntaxERR); return; }  // Syntax error
	ExecPtr++;
	y2=CBint_Eval( SRC );

	if ( tmp_Style >= 0 ) style=tmp_Style;
	CB_SelectGraphVRAM();	// Select Graphic Screen
	F_Line(x1, y1, x2, y2, style);
	Bdisp_PutDisp_DD_DrawBusy_skip_through(SRC);
	tmp_Style = -1;
}

void CBint_Line( unsigned char *SRC ) { //	Line
	int style=S_L_Style;
	if ( tmp_Style >= 0 ) style=tmp_Style;
	if ( RangeErrorCK(SRC) ) return;
	CB_SelectGraphVRAM();	// Select Graphic Screen
	Line(style);
	Bdisp_PutDisp_DD_DrawBusy_skip_through(SRC);
	tmp_Style = -1;
}

void CBint_Vertical( unsigned char *SRC ) { //	Vertical
	int x;
	int style=S_L_Style;
	if ( tmp_Style >= 0 ) style=tmp_Style;
	if ( RangeErrorCK(SRC) ) return;
	x=CBint_Eval( SRC );
	CB_SelectGraphVRAM();	// Select Graphic Screen
	Vertical(x, style);
	Bdisp_PutDisp_DD_DrawBusy_skip_through(SRC);
	tmp_Style = -1;
}
void CBint_Horizontal( unsigned char *SRC ) { //	Horizontal
	int y;
	int style=S_L_Style;
	if ( tmp_Style >= 0 ) style=tmp_Style;
	if ( RangeErrorCK(SRC) ) return;
	y=CBint_Eval( SRC );
	CB_SelectGraphVRAM();	// Select Graphic Screen
	Horizontal(y, style);
	Bdisp_PutDisp_DD_DrawBusy_skip_through(SRC);
	tmp_Style = -1;
}

void CBint_Plot( unsigned char *SRC ) { //	Plot
	unsigned int c;
	int x,y;
	
	if ( RangeErrorCK(SRC) ) return;
	
	CB_SelectGraphVRAM();	// Select Graphic Screen
	c=SRC[ExecPtr];
	if ( ( c==':' ) || (c==0x0D) || ( c==0x0C ) || (c==0x00) ) {
		x=(Xmax+Xmin)/2;
		y=(Ymax+Ymin)/2;
	} else {
		x=CBint_Eval( SRC );
		c=SRC[ExecPtr];
		if ( c != ',' ) { CB_Error(SyntaxERR); return; }  // Syntax error
		ExecPtr++;
		y=CBint_Eval( SRC );
	}
	Plot_X = x;
	Plot_Y = y;
	regX = x;
	regY = y;
	regintX=x; regintY=y;
	PlotPreviousPXY();
	Bdisp_PutDisp_DD_DrawBusy_skip_through(SRC);
}

void CBint_PlotOprand( unsigned char *SRC, int  *x, int *y) {
	*x=CBint_Eval( SRC );
	if ( SRC[ExecPtr] != ',' ) { CB_Error(SyntaxERR); return; }  // Syntax error
	ExecPtr++;
	*y=CBint_Eval( SRC );
	CB_SelectGraphVRAM();	// Select Graphic Screen
}

void CBint_PlotOn( unsigned char *SRC ) { //	PlotOn
	int x,y;
	if ( RangeErrorCK(SRC) ) return;
	CBint_PlotOprand( SRC, &x, &y);
	PlotOn_VRAM(x,y);
	Bdisp_PutDisp_DD_DrawBusy_skip_through(SRC);
}
void CBint_PlotOff( unsigned char *SRC ) { //	PlotOff
	int x,y;
	if ( RangeErrorCK(SRC) ) return;
	CBint_PlotOprand( SRC, &x, &y);
	PlotOff_VRAM(x,y);
	Bdisp_PutDisp_DD_DrawBusy_skip_through(SRC);
}
void CBint_PlotChg( unsigned char *SRC ) { //	PlotChg
	int x,y;
	if ( RangeErrorCK(SRC) ) return;
	CBint_PlotOprand( SRC, &x, &y);
	PlotChg_VRAM(x,y);
	Bdisp_PutDisp_DD_DrawBusy_skip_through(SRC);
}

void CBint_PxlOprand( unsigned char *SRC, int *py, int *px) {
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
void CBint_PxlOn( unsigned char *SRC ) { //	PxlOn
	int x,y;
	if ( RangeErrorCK(SRC) ) return;
	CBint_PxlOprand( SRC, &y, &x);
	Bdisp_SetPoint_VRAM(x, y, 1);
//	regintX = (    px-1)*Xdot  + Xmin ;
//	regintY = ( 62-py+1)*Ydot  + Ymin ;
	Bdisp_PutDisp_DD_DrawBusy_skip_through(SRC);
}
void CBint_PxlOff( unsigned char *SRC ) { //	PxlOff
	int x,y;
	if ( RangeErrorCK(SRC) ) return;
	CBint_PxlOprand( SRC, &y, &x);
	Bdisp_SetPoint_VRAM(x, y, 0);
//	regintX = (    px-1)*Xdot  + Xmin ;
//	regintY = ( 62-py+1)*Ydot  + Ymin ;
	Bdisp_PutDisp_DD_DrawBusy_skip_through(SRC);
}
void CBint_PxlChg( unsigned char *SRC ) { //	PxlChg
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
void CBint_Circle( unsigned char *SRC ) { //	Circle
	unsigned int c;
	int x,y,r;
	int style=S_L_Style;
	if ( tmp_Style >= 0 ) style=tmp_Style;
	if ( RangeErrorCK(SRC) ) return;
	CB_SelectGraphVRAM();	// Select Graphic Screen
	x=CBint_Eval( SRC );
	c=SRC[ExecPtr];
	if ( c != ',' ) { CB_Error(SyntaxERR); return; }  // Syntax error
	ExecPtr++;
	y=CBint_Eval( SRC );
	c=SRC[ExecPtr];
	if ( c != ',' ) { CB_Error(SyntaxERR); return; }  // Syntax error
	ExecPtr++;
	r=CBint_Eval( SRC );
	c=SRC[ExecPtr];
	if ( c==':' ) 	Circle(x, y, r, style, 0);
		else 		Circle(x, y, r, style, 1);
	Bdisp_PutDisp_DD_DrawBusy_skip_through(SRC);
	tmp_Style = -1;
}

//----------------------------------------------------------------------------------------------
int CBint_BinaryEval( unsigned char *SRC ) {
	unsigned int c,op;
	int reg,mptr,opPtr;
	int src,dst;
	char*	MatAryC;
	short*	MatAryW;
	int*	MatAryI;
	
	c=SRC[ExecPtr];
	if ( ( 'A' <= c ) && ( c <='z' ) ) {
		ExecPtr++;
		reg=c-'A';
		src = REGINT[reg] ;
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
	} else  src = CBint_Eval1(SRC+ExecPtr);

	opPtr=ExecPtr;
	op=SRC[ExecPtr++];	
	c=SRC[ExecPtr];
	if ( ( 'A' <= c ) && ( c <='z' ) ) {
		ExecPtr++;
		reg=c-'A';
		dst = REGINT[reg] ;
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
	} else  dst = CBint_Eval1(SRC+ExecPtr);

//	ExecPtr++;
	switch ( op ) {
			case 0x89 :		// +
				return src + dst;
			case 0x99 :		// -
				return src - dst;
			case 0xA9 :		// ~
				return src * dst;
			case 0xB9 :		// €
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
			case 0xB0 :	// And
				return (int)src & (int)dst;
			case 0xB1 :	// Or
				return (int)src | (int)dst;
			default:
				{ ErrorNo=SyntaxERR; ErrorPtr=opPtr; return; }  // Syntax error
				break;
	}
}

int CBint_UnaryEval( unsigned char *SRC ) {
	unsigned int c,op;
	int reg,mptr,opPtr;
	int src,dst;
	char*	MatAryC;
	short*	MatAryW;
	int*	MatAryI;
	
	c=SRC[ExecPtr];
	if ( ( 'A' <= c ) && ( c <='z' ) ) {
		ExecPtr++;
		reg=c-'A';
		return REGINT[reg] ;
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
	} else  return CBint_Eval1(SRC+ExecPtr);
}

//----------------------------------------------------------------------------------------------

int CBint_interpreter_sub( unsigned char *SRC ) {
	int cont=1;
	int i;
	unsigned int c;
	
	int StackGotoAdrs[StackGotoMax];

	int	CurrentStructCNT=0;
	int	CurrentStructloop[20];

	int StackForPtr=0;
	int StackForAdrs[StackForMax];
	int StackForVar[StackForMax];
	int StackForEnd[StackForMax];
	int StackForStep[StackForMax];

	int StackWhilePtr=0;
	int StackWhileAdrs[StackWhileMax];

	int StackDoPtr=0;
	int StackDoAdrs[StackDoMax];


	for (i=0; i<StackGotoMax; i++) StackGotoAdrs[i]=0;
	ExecPtr=0;
	StackForPtr=0;
	StackWhilePtr=0;
	StackDoPtr=0;
	CurrentStructCNT=0;
	tmp_Style = -1;
	dspflag=0;
//	UseGraphic=0;

	while (cont) {
		if ( ErrorNo  ) return ErrorPtr;
		if ( BreakPtr ) { CB_BreakStop(); return BreakPtr; }
		c=SRC[ExecPtr++];
		if ( c==':'  ) { c=SRC[ExecPtr++]; if (BreakCheck) if ( KeyScanDown(KEYSC_AC) ) { BreakPtr=ExecPtr-1; KeyRecover(); } }	// [AC] break?
		if ( c==0x0D ) { while ( c==0x0D ) c=SRC[ExecPtr++];
				if (BreakCheck) if ( KeyScanDown(KEYSC_AC) ) { BreakPtr=ExecPtr-1; KeyRecover(); }	// [AC] break?
		}
		if ( c==0x00 ) { ExecPtr--; if ( ProgEntryN )  return -1;  else  break; }
		switch (c) {
			case 0xE2:	// Lbl
				CB_Lbl(SRC, StackGotoAdrs );
				break;
			case 0xEC:	// Goto
				CB_Goto(SRC, StackGotoAdrs );
				break;
			case 0xE8:	// Dsz
				CBint_Dsz(SRC) ;
				break;
			case 0xE9:	// Isz
				CBint_Isz(SRC) ;
				break;
//			case 0x0E:	// ->
//				CBint_Store(SRC);
//				break;
//			case 0x13:	// =>
//				CBint_Cond(SRC);
//				dspflag=0;
//				if ( CBint_CurrentValue == 0 ) Skip_block(SRC);		// false
//				break;
				
			case 0xF7:	// F7
				c=SRC[ExecPtr++];
				switch ( c ) {
					case 0x00:	// If
						CBint_If(SRC);
						break;
					case 0x02:	// Else
//						CBint_Else(SRC);
						Search_IfEnd(SRC);
						break;
					case 0x03:	// IfEnd
//						CBint_IfEnd(SRC);
						break;
					case 0x04:	// For
						CBint_For(SRC, &StackForPtr, StackForAdrs, StackForVar, StackForEnd, StackForStep, &CurrentStructCNT, CurrentStructloop );
						break;
					case 0x07:	// Next
						CBint_Next(SRC, &StackForPtr, StackForAdrs, StackForVar, StackForEnd, StackForStep, &CurrentStructCNT, CurrentStructloop );
						break;
					case 0x08:	// While
						CBint_While(SRC, &StackWhilePtr, StackWhileAdrs, &CurrentStructCNT, CurrentStructloop );
						break;
					case 0x09:	// WhileEnd
						CBint_WhileEnd(SRC, &StackWhilePtr, StackWhileAdrs, &CurrentStructCNT, CurrentStructloop );
						break;
					case 0x0A:	// Do
						CBint_Do(SRC, &StackDoPtr, StackDoAdrs, &CurrentStructCNT, CurrentStructloop );
						break;
					case 0x0B:	// LpWhile
						CBint_LpWhile(SRC, &StackDoPtr, StackDoAdrs, &CurrentStructCNT, CurrentStructloop );
						break;
					case 0x0C:	// Return
						if ( ProgEntryN ) return -1 ; //	in sub Prog
//						dspflag=0;
						cont=0;
						break;
					case 0x0D:	// Break
						CB_Break(SRC, &StackForPtr, &StackWhilePtr, &StackDoPtr, &CurrentStructCNT, CurrentStructloop );
						dspflag=0;
						break;
					case 0x0E:	// Stop
						BreakPtr=ExecPtr;
						cont=0; 
//						dspflag=0;
						break;
					case 0x10:	// Locate
						CBint_Locate(SRC);
						dspflag=3;
						break;
					case 0x18:	// ClrText
						CB_ClrText(SRC);
						dspflag=0;
						break;
					case 0x19:	// ClrGraph
						CB_ClrGraph(SRC);
						dspflag=0;
						UseGraphic=0;
						break;
					case 0x1C:	// S-L-Normal
						S_L_Style = S_L_Normal;
						dspflag=0;
						break;
					case 0x1D:	// S-L-Thick
						S_L_Style = S_L_Thick;
						dspflag=0;
						break;
					case 0x1E:	// S-L-Broken
						S_L_Style = S_L_Broken;
						dspflag=0;
						break;
					case 0x1F:	// S-L-Dot
						S_L_Style = S_L_Dot;
						dspflag=0;
						break;
					case 0x8C:	// SketchNormal
						tmp_Style = S_L_Normal;
						dspflag=0;
						break;
					case 0x8D:	// SketchThick
						tmp_Style = S_L_Thick;
						dspflag=0;
						break;
					case 0x8E:	// SketchBroken
						tmp_Style = S_L_Broken;
						dspflag=0;
						break;
					case 0x8F:	// SketchDot
						tmp_Style = S_L_Dot;
						dspflag=0;
						break;
					case 0x93:	// StoPict
						CB_StoPict(SRC);
						dspflag=0;
						UseGraphic=99;
						break;
					case 0x94:	// RclPict
						CB_RclPict(SRC);
						dspflag=0;
						UseGraphic=99;
						break;
					case 0xA3:	// Vertical
						CBint_Vertical(SRC);
						dspflag=0;
						UseGraphic=99;
						break;
					case 0xA4:	// Horizontal
						CBint_Horizontal(SRC);
						dspflag=0;
						UseGraphic=99;
						break;
					case 0xA5:	// Text
						dspflag=0;
						CBint_Text(SRC, &dspflag);
						UseGraphic=99;
						break;
					case 0xA6:	// Circle
						CB_Circle(SRC);
						dspflag=0;
						UseGraphic=99;
						break;
					case 0xA7:	// F-Line
						CBint_FLine(SRC);
						dspflag=0;
						UseGraphic=99;
						break;
					case 0xA8:	// PlotOn
						CBint_PlotOn(SRC);
						dspflag=0;
						UseGraphic=99;
						break;
					case 0xA9:	// PlotOff
						CBint_PlotOff(SRC);
						dspflag=0;
						UseGraphic=99;
						break;
					case 0xAA:	// PlotChg
						CBint_PlotChg(SRC);
						dspflag=0;
						UseGraphic=99;
						break;
					case 0xAB:	// PxlOn
						CBint_PxlOn(SRC);
						dspflag=0;
						UseGraphic=99;
						break;
					case 0xAC:	// PxlOff
						CBint_PxlOff(SRC);
						dspflag=0;
						UseGraphic=99;
						break;
					case 0xAD:	// PxlChg
						CBint_PxlChg(SRC);
						dspflag=0;
						UseGraphic=99;
						break;
					case 0x7A:	// GridOff
						Grid=0;
						dspflag=0;
						break;
					case 0x7D:	// GridOn
						Grid=1;
						dspflag=0;
						break;
					case 0xC2:	// AxesOn
						Axes=0;
						dspflag=0;
						break;
					case 0xC3:	// CoordOn
						Coord=1;
						dspflag=0;
						break;
					case 0xC4:	// LabelOn
						Label=1;
						dspflag=0;
						break;
					case 0xD2:	// AxesOff
						Axes=1;
						dspflag=0;
						break;
					case 0xD3:	// CoordOff
						Coord=0;
						dspflag=0;
						break;
					case 0xD4:	// LabelOff
						Label=0;
						dspflag=0;
						break;
					case 0x20:	// DrawGraph
						CB_DrawGraph(SRC);
						dspflag=0;
						UseGraphic=2;
						break;
					default:
						ExecPtr-=2;
						CBint_CurrentValue = CBint_Eval( SRC );
						dspflag=2;
				}
				break;
				
			case 0x7F:	// 7F
				c=SRC[ExecPtr++];
				switch ( c ) {
					case 0x40:	// Mat
						CB_MatCalc(SRC,&dspflag);
						break;
					default:
						ExecPtr-=2;
						CBint_CurrentValue = CBint_Eval( SRC );
						dspflag=2;
				}
				break;
				
			case 0xF9:	// F9
				c=SRC[ExecPtr++];
				switch ( c ) {
					case 0x0B:	// EngOn
						ENG=1;
						dspflag=0;
						break;
					case 0x0C:	// EngOff
						ENG=0;
						dspflag=0;
						break;
					case 0x1E:	// ClrMat
						CB_ClrMat(SRC);
						dspflag=0;
						break;
					case 0x4B:	// DotP(Z,x,y)
						CB_DotP(SRC);
						dspflag=0;
						UseGraphic=99;
						break;
					default:
						ExecPtr-=2;
						CBint_CurrentValue = CBint_Eval( SRC );
						dspflag=2;
				}
				break;
				
			case 0x27:	// ' rem
				Skip_rem(SRC);
				dspflag=0;
				break;
			case 0x22:	// " "
				CB_Quot(SRC) ;
				dspflag=1;
				break;
			case 0x3F:	// ?
				CBint_Input(SRC);
				CB_TicksStart=RTC_GetTicks();	// 
				dspflag=2;
				c=SRC[ExecPtr++];
				if ( c == 0x0E ) CBint_Store(SRC);		// ->
				else ExecPtr--;
				break;
			case 0x7B:	// {
				CB_MatrixInit(SRC);
				dspflag=0;
				break;
			case 0x0C:	// dsps
				if ( CB_Disps(SRC, dspflag) ) BreakPtr=ExecPtr ;  // [AC] break
				break;
			case 0xD1:	// Cls
				CB_Cls(SRC);
				dspflag=0;
				UseGraphic=0;
				break;
			case 0xDA:	// Deg
				Angle = 0;
				dspflag=0;
				break;
			case 0xDB:	// Rad
				Angle = 1;
				dspflag=0;
				break;
			case 0xDC:	// Grad
				Angle = 2;
				dspflag=0;
				break;
			case 0xE0:	// Plot
				CBint_Plot(SRC);
				dspflag=0;
				UseGraphic=1;
				break;
			case 0xE1:	// Line
				CBint_Line(SRC);
				dspflag=0;
				UseGraphic=99;
				break;
			case 0xED:	// Prog "..."
				CB_Prog(SRC);
				if ( BreakPtr ) return BreakPtr;
				dspflag=0;
				break;
				
			case 0xDD:	// Eng
				ENG=1-ENG;
				dspflag=0;
				break;
			case 0xD3:	// Rnd
				CB_Rnd();
				dspflag=2;
				break;
			case 0xD9:	// Norm
				CB_Norm(SRC);
				dspflag=0;
				break;
			case 0xE3:	// Fix
				CB_Fix(SRC);
				dspflag=0;
				break;
			case 0xE4:	// Sci
				CB_Sci(SRC);
				dspflag=0;
				break;
			case 0xEB:	// ViewWindow
				CB_ViewWindow(SRC);
				dspflag=0;
				UseGraphic=0;
				break;
			case 0xEE:	// Graph Y=
				CB_GraphY(SRC);
				dspflag=0;
				UseGraphic=2;
				break;
				
			case 0xFB:	//  P(  Unary Eval operation
				CBint_CurrentValue = CBint_UnaryEval(SRC);
				dspflag=2;
				break;
			case 0xFC:	//  Q(  binaly Eval operation
				CBint_CurrentValue = CBint_BinaryEval(SRC);
				dspflag=2;
				break;
		
			default:
				ExecPtr--;
				CBint_CurrentValue = CBint_Eval( SRC );
				dspflag=2;
				break;
		}
		c=SRC[ExecPtr];
		if ( c == 0x0E ) { 
			ExecPtr++;
			CBint_Store(SRC);		// ->
			c=SRC[ExecPtr];
		}
		if ( c == 0x13 ) {					// =>
			ExecPtr++;
			dspflag=0;
			if ( CBint_CurrentValue == 0 ) Skip_block(SRC);		// false
		}
	}
	CB_TicksEnd=RTC_GetTicks();	// 
	CB_end(SRC, dspflag);
	return -1;
}

//----------------------------------------------------------------------------------------------

int q_un_subint(void) {
     int i;
     int a[9];
     for (i=0;i<9;i++) a[i]=0;
     regN=1;  
 l5: regR=8;
     regS=0;
     regX=0;
 l0: if (regX==regR) goto l4;
     regX++;
     a[(int)regX]=regR;
 l1: ++regS;
     regY=regX;
 l2: if (!--regY) goto l0;
     if (!(regT=a[(int)regX]-a[(int)regY])) goto l3;
     if (regX-regY!=fabs(regT)) goto l2;
 l3: if (--a[(int)regX]) goto l1;
     if (--regX) goto l3;
 l4: if (--regN) goto l5;

  return (regS);
}

int asctt4int(){
	regK=1.0;
	lbl0:
	regK=regK+1.0;
	regA=regK/2.0*regK+4.0-regK;
	if ( regK<10000 ) goto lbl0;
	return regA;;
}


void CBint_test() {
	int i, s, t, result;
	unsigned char	buffer[32];
	unsigned int key=0;

	Bdisp_AllClr_DDVRAM();
	locate(1,1); Print((unsigned char*)"8queen int");
	Bdisp_PutDisp_DD();
	
	start_timer();	// --------------------------------------

	regS=q_un_subint();

	t=get_timer();	// --------------------------------------

	locate(1,2);
	sprintG(buffer, regS, 21, RIGHT_ALIGN);
		Print(buffer);
	locate(1,3);sprintf((char*)buffer,"time=%5.3fs",(float)t/1000); Print(buffer);
	Bdisp_PutDisp_DD();

	
	locate(1,5); Print((unsigned char*)"Asciit4 int");
	Bdisp_PutDisp_DD();
	
	start_timer();	// --------------------------------------

	regS=asctt4int();

	t=get_timer();	// --------------------------------------

	locate(1,6);
	sprintG(buffer, regS, 21, RIGHT_ALIGN);
		Print(buffer);
	locate(1,7);sprintf((char*)buffer,"time=%5.3fs",(float)t/1000); Print(buffer);

	Bdisp_PutDisp_DD();
	GetKey(&key);
}


