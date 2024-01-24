/*
===============================================================================

 Casio Basic interpreter for fx-9860G series    v0.63

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
#include "CB_glib2.h"
#include "CB_Eval.h"
#include "CB_file.h"
#include "CB_edit.h"
#include "CB_setup.h"

#include "CB_interpreter.h"
#include "CBI_interpreter.h"
#include "CB_error.h"
#include "TIMER_FX2.H"

//-----------------------------------------------------------------------------
// Casio Basic inside
//-----------------------------------------------------------------------------
int CB_INTDefault=0;	// default mode  0:normal  1: integer mode
int CB_INT=0;			// current mode  0:normal  1: integer mode

int	DrawType    = 0;	// 0:connect  1:Plot
int	Coord       = 1;	// 0:off 1:on
int	Grid        = 1;	// 0:off 1:on
int	Axes        = 1;	// 0:off 1:on
int	Label       = 1;	// 0:off 1:on
int	Derivative  = 1;	// 0:off 1:on
int S_L_Style   = S_L_Normal;
int tmp_Style   = S_L_Normal;
int Angle       = 1;	// 0:deg   1:rad  2:grad

double Previous_X=1e308 ;	// Plot Previous X
double Previous_Y=1e308 ;	// Plot Previous Y
int    Previous_PX=-1   ;	// Plot Previous PX
int    Previous_PY=-1   ;	// Plot Previous PY
double Plot_X    =1e308 ;	// Plot Current X
double Plot_Y    =1e308 ;	// Plot Current Y

int TimeDsp=0;		// Execution Time Display  0:off 1:on
int MatXYmode=0;		// 0: normal  1:reverse
//-----------------------------------------------------------------------------
// Casio Basic Gloval variable
//-----------------------------------------------------------------------------
double  REG[59];
double  REGv[11];

double	Xfct     =  2;					// 
double	Yfct     =  2;					// 

double	traceAry[130];		// Graph array X

unsigned char *GraphY;
unsigned char GraphY1[GraphStrMAX];
unsigned char GraphY2[GraphStrMAX];
unsigned char GraphY3[GraphStrMAX];

short	MatArySizeA[26];		// Matrix array status
short	MatArySizeB[26];		// Matrix array status
char	MatAryElementSize[26];		// Matrix array Element size
double *MatAry[26];		// Matrix array ptr*



//----------------------------------------------------------------------------------------------
//		Interpreter
//----------------------------------------------------------------------------------------------
int ScreenMode;	//  0:Text  1:Graphic
int UseGraphic=0;	// use Graph  ( no use :0    plot:1   graph:2   cls:3   other:99

int CursorX=1;	// text cursor X
int CursorY=1;	// text cursor X

int ExecPtr=0;		// Basic execute ptr
int BreakPtr=0;		// Basic break ptr

double CB_CurrentValue=0;	// Ans
char   CB_CurrentStr[128];	//

int ProgEntryN=0;		// Basic Program ptr (for subroutin)
int ProgNo=0;			// current Prog No
unsigned char *ProgfileAdrs[ProgMax+1];
int ProgfileMax[ProgMax+1] ;	// Max edit filesize 
int ProgfileEdit[ProgMax+1];	// no change : 0     edited : 1

int	CB_TicksStart;
int	CB_TicksEnd;

int BreakCheck=1;	// Break Stop on/off
//----------------------------------------------------------------------------------------------
unsigned char GVRAM[1024];

void SaveGVRAM(){
	Bdisp_GetDisp_VRAM( GVRAM );
}
void RestoreGVRAM(){
	DISPGRAPH Gpict;
	Gpict.x =   0; 
	Gpict.y =   0; 
	Gpict.GraphData.width   = 128;
	Gpict.GraphData.height  = 64;
	Gpict.GraphData.pBitmap = GVRAM;
	Gpict.WriteModify = IMB_WRITEMODIFY_NORMAL; 
	Gpict.WriteKind   = IMB_WRITEKIND_OVER;
	Bdisp_WriteGraph_VRAM(&Gpict);
}
//----------------------------------------------------------------------------------------------
void CB_SaveTextVRAM() {
	SaveDisp(SAVEDISP_PAGE2);		// ------ SaveDisp2 Text screen
}	
void CB_RestoreTextVRAM() {
	RestoreDisp(SAVEDISP_PAGE2);		// ------ RestoreDisp2 Text screen
}	
void CB_SelectTextVRAM() {
	if ( ScreenMode == 0 ) return;
	SaveGVRAM();		// ------ Save Graphic screen 
//	SaveDisp(SAVEDISP_PAGE3);		// ------ SaveDisp3 Graphic screen (NG: damage CATALOG key )
	RestoreDisp(SAVEDISP_PAGE2);		// ------ RestoreDisp2 Text screen
	ScreenMode=0;	// Text mode
}
void CB_SelectTextDD() {
	CB_SelectTextVRAM();
	Bdisp_PutDisp_DD();
}
void CB_SaveGraphVRAM() {
	SaveGVRAM();		// ------ Save Graphic screen 
}	
void CB_RestoreGraphVRAM() {
	RestoreGVRAM();		// ------ Restore Graphic screen
}	
void CB_SelectGraphVRAM() {
	if ( ScreenMode == 1 ) return;
	SaveDisp(SAVEDISP_PAGE2);		// ------ SaveDisp2 Text screen
	RestoreGVRAM();		// ------ Restore Graphic screen
//	RestoreDisp(SAVEDISP_PAGE2);		// ------ RestoreDisp3 Graphic screen (NG: damage CATALOG key )
	ScreenMode=1;	// Graphic mode
}
void CB_SelectGraphDD() {
	CB_SelectGraphVRAM();
	Bdisp_PutDisp_DD();
}


void Scrl_Y(){
	unsigned char pDATA[1024];
	DISPGRAPH Gscrl;
	DISPBOX sBOX;
	
	CursorY++;
	if ( CursorY > 7 ) {		// scroll
		sBOX.left  =0;
		sBOX.top   =8;
		sBOX.right =127;
		sBOX.bottom=63;
		Bdisp_ReadArea_VRAM(&sBOX,pDATA);

	    Gscrl.x =   0; 
		Gscrl.y =   0; 
		Gscrl.GraphData.width   = 128;
		Gscrl.GraphData.height  = 56;
		Gscrl.GraphData.pBitmap = pDATA;
		Gscrl.WriteModify = IMB_WRITEMODIFY_NORMAL; 
		Gscrl.WriteKind   = IMB_WRITEKIND_OVER;
		Bdisp_WriteGraph_VRAM(&Gscrl);
		
		sBOX.left  =0;
		sBOX.top   =56;
		sBOX.right =127;
		sBOX.bottom=63;
		Bdisp_AreaClr_VRAM(&sBOX);
		CursorY=7;
	}
	CursorX=1;
}

int CB_GotoEndPtr( unsigned char *SRC ) {		// goto Program End Ptr
	int size;
	unsigned char *src;
	src=SRC-0x56;
	size=(src[0x47]&0xFF)*256+(src[0x48]&0xFF)+0x4C;
	return ( size -0x56 - 1 ) ;
}

//----------------------------------------------------------------------------------------------

void CB_Cls( unsigned char *SRC ){
	CB_SelectGraphVRAM();	// Select Graphic Screen
	ViewWindow( Xmin, Xmax, Xscl, Ymin, Ymax, Yscl);
//	Bdisp_AllClr_VRAM();
//	GraphAxesGrid();
	Previous_PX=-1;   Previous_PY=-1; 		// ViewWindow Previous PXY init
	CB_SelectTextVRAM();	// Select Text Screen
//	Bdisp_PutDisp_DD_DrawBusy_through(SRC);
}
void CB_ClrText( unsigned char *SRC ){
	CB_SelectTextVRAM();	// Select Text Screen
	CursorX=1;
	CursorY=1;
	Bdisp_AllClr_VRAM();
	Bdisp_PutDisp_DD_DrawBusy_through(SRC);
}
void CB_ClrGraph( unsigned char *SRC ){
	CB_SelectGraphVRAM();	// Select Graphic Screen
	SetVeiwWindowInit();
	CB_Cls(SRC);
}

void CB_Deg(){
	Angle = 0;
}
void CB_Rad(){
	Angle = 1;
}
void CB_Grad(){
	Angle = 2;
}

int RangeErrorCK( unsigned char *SRC ) {
	if ( UseGraphic == 0 ) {
		if ( ScreenMode == 0 ) CB_SelectGraphVRAM();	// Select Graphic Screen
		Bdisp_AllClr_VRAM();
		GraphAxesGrid();
	}
	if ( ( Xdot == 0 ) || ( Ydot == 0 )  ) { ErrorNo=RangeERR; PrevOpcode( SRC, &ExecPtr ); ErrorPtr=ExecPtr; return ErrorNo; }	// Range error
	return 0;
}

//-----------------------------------------------------------------------------
#define CB_Eval  EvalsubTop
#define CBint_Eval  EvalIntsubTop
#define CB_Eval1 Evalsub1

//-----------------------------------------------------------------------------
int CB_Fix( unsigned char *SRC ){
	int tmp;
	if (CB_INT)	tmp=CBint_Eval( SRC );
	else		tmp=CB_Eval( SRC );
	if ( tmp < 0 ) if ( CB_Round.MODE == Fix ) return CB_Round.DIGIT; else return -1;
	if ( ( tmp < 0 ) || ( tmp > 15 ) ) { CB_Error(SyntaxERR); return; }	// Syntax error
	CB_Round.MODE = Fix ;
	CB_Round.DIGIT= tmp ;
	return tmp ;
}
int CB_Sci( unsigned char *SRC ){
	int tmp;
	if (CB_INT)	tmp=CBint_Eval( SRC );
	else		tmp=CB_Eval( SRC );
	if ( tmp < 0 ) if ( CB_Round.MODE == Sci ) return CB_Round.DIGIT; else return -1;
	if ( ( tmp < 0 ) || ( tmp > 15 ) ) { CB_Error(SyntaxERR); return; }	// Syntax error
	CB_Round.MODE = Sci ;
	CB_Round.DIGIT= tmp ;
	return tmp ;
}
int CB_Norm( unsigned char *SRC ){
	int tmp;
	if (CB_INT)	tmp=CBint_Eval( SRC );
	else		tmp=CB_Eval( SRC );
	if ( tmp < 0 ) if ( CB_Round.MODE == Norm ) return CB_Round.DIGIT; else return -1;
	if ( ( tmp < 0 ) || ( tmp > 15 ) ) { CB_Error(SyntaxERR); return; }	// Syntax error
	CB_Round.MODE = Norm ;
	CB_Round.DIGIT= tmp ;
	return tmp ;
}
void CB_Rnd(){
	CB_CurrentValue = Round( CB_CurrentValue, CB_Round.MODE, CB_Round.DIGIT );
}

//-----------------------------------------------------------------------------
int MatOprand( unsigned char *SRC, int *reg){ 
	int dimA,dimB;
	unsigned int c=SRC[ExecPtr];
	if ( ( 'A'<=c )&&( c<='Z' ) ) {
		(*reg)=c-'A';
		ExecPtr++ ;
		if ( SRC[ExecPtr] != '[' ) { return -2; }	// 
		c=SRC[++ExecPtr];
		if ( SRC[ExecPtr+1] == ',' ) {
			ExecPtr++ ;
			if  ( ( '1'<= c ) && ( c<='9' ) ) dimA=c-'0';
			else if  ( ( 'A'<= c ) && ( c<='z' ) ) dimA=REG[c-'A'];
		} else {
			if (CB_INT) dimA=(CBint_Eval( SRC )); else dimA=(CB_Eval( SRC ));
			if ( SRC[ExecPtr] != ',' ) { CB_Error(SyntaxERR); return -1; }	// Syntax error
		}
		if ( ( dimA < 1 ) || ( MatArySizeA[(*reg)] < dimA ) ) { CB_Error(DimensionERR); return -1; }	// Dimension error
		c=SRC[++ExecPtr];
		if ( SRC[ExecPtr+1] == ']' ) {
			ExecPtr++ ;
			if  ( ( '1'<= c ) && ( c<='9' ) ) dimB=c-'0';
			else if  ( ( 'A'<= c ) && ( c<='z' ) ) dimB=REG[c-'A'];
		} else {
			if (CB_INT) dimB=(CBint_Eval( SRC )); else dimB=(CB_Eval( SRC ));
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
void CB_MatrixInit( unsigned char *SRC ) { //	{n,m}->Dim Mat A[.B][.W][.L][.F]
	unsigned int c,d;
	int dimA,dimB,i;
	int reg;
	char	*cptr;
	short	*wptr;
	int		*iptr;
	double	*dptr;
	int ElementSize;
	
	if (CB_INT)	dimA=(CBint_Eval( SRC )); else dimA=(CB_Eval( SRC ));

	c=SRC[ExecPtr];
	if ( c != ',' ) { CB_Error(SyntaxERR); return; }  // Syntax error
	ExecPtr++;
	if (CB_INT)	dimB=(CBint_Eval( SRC )); else dimB=(CB_Eval( SRC ));

	c=SRC[ExecPtr];
	if ( c != '}' ) { CB_Error(SyntaxERR); return; }  // Syntax error
	ExecPtr++;
	c=SRC[ExecPtr];
	if ( c != 0x0E ) { CB_Error(SyntaxERR); return; }  // Syntax error
	ExecPtr++;
	c =SRC[ExecPtr];
	d =SRC[ExecPtr+1];
	if ( ( c != 0x7F ) || ( d !=0x46 ) ) { CB_Error(SyntaxERR); return; }  // no Dim  Syntax error
	ExecPtr+=2;
	c =SRC[ExecPtr];
	d =SRC[ExecPtr+1];
	if ( ( c != 0x7F ) || ( d !=0x40 ) ) { CB_Error(SyntaxERR); return; }  // no Mat  Syntax error
	ExecPtr+=2;
	c =SRC[ExecPtr];
	if ( ( 'A' <= c ) && ( c <= 'Z' ) ) {
		ExecPtr++;
		reg=c-'A';
		if ( SRC[ExecPtr]=='.' ) {
			c =SRC[++ExecPtr];
			if ( ( c=='B' ) || ( c=='b' ) ) { ElementSize=1; ExecPtr++; }
			if ( ( c=='W' ) || ( c=='w' ) ) { ElementSize=2; ExecPtr++; }
			if ( ( c=='L' ) || ( c=='l' ) ) { ElementSize=4; ExecPtr++; }
			if ( ( c=='F' ) || ( c=='f' ) ) { ElementSize=8; ExecPtr++; }
			DimMatrixSub( reg, ElementSize, dimA, dimB );
		}
		else DimMatrix( reg, dimA, dimB );
	} else { CB_Error(SyntaxERR); return; }  // Syntax error
}

void CB_ClrMat( unsigned char *SRC ) { //	ClrMat A
	unsigned int c;

	c =SRC[ExecPtr];
	if ( ( 'A' <= c ) && ( c <= 'Z' ) ) {
		ExecPtr++;
		DeleteMatrix( c-'A' );
	}
}

void CB_MatCalc( unsigned char *SRC ) { //	Mat A -> Mat B  etc
	unsigned int c,d;
	int dimA,dimB,i;
	int reg,reg2;
	double	*dptr, *dptr2;
	
	c =SRC[ExecPtr];
	if ( ( 'A' <= c ) && ( c <= 'Z' ) ) {
		ExecPtr++;
		reg=c-'A';
	} else { CB_Error(SyntaxERR); return; }	// Syntax error

	c =SRC[ExecPtr];
	if ( c == 0x0E ) {	// ->
		ExecPtr++;
		c =SRC[ExecPtr];
		if ( ( c != 0x7F ) || ( SRC[ExecPtr+1]!=0x40 ) ) { CB_Error(SyntaxERR); return; }	// Syntax error
		ExecPtr+=2;
		c =SRC[ExecPtr];
		if ( ( 'A' <= c ) && ( c <= 'Z' ) ) {
			ExecPtr++;
			reg2=c-'A';
		} else { CB_Error(SyntaxERR); return; }	// Syntax error

		if ( ( MatAryElementSize[reg] == MatAryElementSize[reg2] ) && 
			 ( MatArySizeA[reg] == MatArySizeA[reg2] ) &&
			 ( MatArySizeB[reg] == MatArySizeB[reg2] ) ) {
			memcpy( MatAry[reg2], MatAry[reg], MatAryElementSize[reg]*MatArySizeA[reg]*MatArySizeB[reg] ) ;
			dspflag=0;
		} else { CB_Error(DimensionERR); return; }	// Dimension error
	} else {
		ExecPtr-=3;
		dspflag=2;
		if (CB_INT)	CBint_CurrentValue = CBint_Eval( SRC );
		else		CB_CurrentValue    = CB_Eval( SRC );
	}
}

//-----------------------------------------------------------------------------

void CB_Store( unsigned char *SRC ){	// ->
	int	st,en,i;
	int dimA,dimB,reg;
	int mptr;
	char*	MatAryC;
	short*	MatAryW;
	int*	MatAryI;
	
	unsigned int c=SRC[ExecPtr];
	if ( ( 'A' <= c ) && ( c <= 'z' ) ) {
		st=c-'A';
		ExecPtr++;
		if ( SRC[ExecPtr] == 0x7E ) {		// '~'
			ExecPtr++;
			c=SRC[ExecPtr];
			if ( ( 'A' <= c ) && ( c <= 'z' ) ) {
				en=c-'A';
				if ( en<st ) { CB_Error(SyntaxERR); return; }	// Syntax error
				c=SRC[++ExecPtr];
				if ( c=='%' ) { ExecPtr++;  for ( i=st; i<=en; i++) REGINT[ i ] = CB_CurrentValue; }
				else
				if ( c=='#' ) ExecPtr++;
				for ( i=st; i<=en; i++) REG[ i ] = CB_CurrentValue;
			}
		} else {					// 
			c=SRC[ExecPtr];
			if ( c=='%' ) { ExecPtr++;  REGINT[st] = CB_CurrentValue ; }
			else {
				if ( c=='#' ) ExecPtr++;
				REG[st] = CB_CurrentValue;
			}
		}
	} else
	if ( c==0x7F ) {
		c = SRC[ExecPtr+1] ; 
		if ( c == 0x40 ) {	// Mat A[a,b]
			ExecPtr+=2;
			mptr=MatOprand( SRC, &reg);
			if ( ErrorNo ) return ; // error
			if ( mptr==-2 ) {	// Mat A
				dimA=MatArySizeA[reg];
				dimB=MatArySizeB[reg];
				switch ( MatAryElementSize[reg] ) {
					case 8:						// Matrix array double
						for (i=0; i<dimB*dimA; i++)
							MatAry[reg][i] = CB_CurrentValue;
						break;
					case 4:						// Matrix array int
						MatAryI=(int*)MatAry[reg];
						for (i=0; i<dimB*dimA; i++)
							MatAryI[i] = CB_CurrentValue;
						break;
					case 1:						// Matrix array char
						MatAryC=(char*)MatAry[reg];
						for (i=0; i<dimB*dimA; i++)
							MatAryC[i] = CB_CurrentValue;
						break;
					case 2:						// Matrix array word
						MatAryW=(short*)MatAry[reg];
						for (i=0; i<dimB*dimA; i++)
							MatAryW[i] = CB_CurrentValue;
						break;
					default:
						CB_Error(NoMatrixArrayERR); // No Matrix Array error
						break;
				}
			} else {	// Mat A[a,b]
				switch ( MatAryElementSize[reg] ) {
					case 8:						// Matrix array double
						MatAry[reg][mptr] = CB_CurrentValue;
						break;
					case 4:						// Matrix array int
						MatAryI=(int*)MatAry[reg];
						MatAryI[mptr] = CB_CurrentValue;
						break;
					case 1:						// Matrix array char
						MatAryC=(char*)MatAry[reg];
						MatAryC[mptr] = CB_CurrentValue;
						break;
					case 2:						// Matrix array word
						MatAryW=(short*)MatAry[reg];
						MatAryW[mptr] = CB_CurrentValue;
						break;
				}
			}
		} else if ( c == 0x00 ) {	// Xmin
				ExecPtr+=2;
				Xmin = CB_CurrentValue ;
				Xdot = (Xmax-Xmin)/126.0;
		} else if ( c == 0x01 ) {	// Xmax
				ExecPtr+=2;
				Xmax = CB_CurrentValue ;
				Xdot = (Xmax-Xmin)/126.0;
		} else if ( c == 0x02 ) {	// Xscl
				ExecPtr+=2;
				Xscl = CB_CurrentValue ;
		} else if ( c == 0x04 ) {	// Ymin
				ExecPtr+=2;
				Ymin = CB_CurrentValue ;
				Ydot = (Ymax-Ymin)/62.0;
		} else if ( c == 0x05 ) {	// Ymax
				ExecPtr+=2;
				Ymax = CB_CurrentValue ;
				Ydot = (Ymax-Ymin)/62.0;
		} else if ( c == 0x06) {	// Yscl
				ExecPtr+=2;
				Yscl = CB_CurrentValue ;
		} else if ( c == 0x0B ) {	// Xfct
				ExecPtr+=2;
				Xfct = CB_CurrentValue ;
		} else if ( c == 0x0C ) {	// Yfct
				ExecPtr+=2;
				Yfct = CB_CurrentValue ;
		}
	} else
	if ( c==0xF9 ) {
		c = SRC[ExecPtr+1] ; 
		if ( c == 0x21 ) {	// Xdot
				if ( CB_CurrentValue == 0 ) { ErrorNo=RangeERR; ErrorPtr=ExecPtr; return; }	// Range error
				ExecPtr+=2;
				Xdot = CB_CurrentValue ;
				Xmax = Xmin + Xdot*126.;
		}
	} else { CB_Error(SyntaxERR); return; }	// Syntax error
}

int  CB_Input( unsigned char *SRC ){
	unsigned int c;
	double DefaultValue=0;
	int flag=0,flagint=0;
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
	if ( c==0x0E ) {	// ->
		flag=0;
	} else
	if ( ( 'A' <= c ) && ( c <= 'z' ) ) {
		flag=1;
		reg=c-'A';
		c=SRC[ExecPtr+1];
		if ( CB_INT ) {
			if ( c=='#' ) {
				DefaultValue = REG[reg] ;
			} else { flagint=1; 
				if ( c=='%' ) 
				DefaultValue = REGINT[reg] ;
			}
		} else {
			if ( c=='%' ) {flagint=1; 
				DefaultValue = REGINT[reg] ;
			} else { 
				if ( c=='#' )
				DefaultValue = REG[reg] ;
			}
		}
	} else
	if ( c==0x7F ) {
		c = SRC[ExecPtr+1] ; 
		if ( c == 0x40 ) {	// Mat A[a,b]
			ExecPtr+=2;
			if ( CB_INT ) mptr=MatOprandInt( SRC, &reg); else mptr=MatOprand( SRC, &reg);
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
		} else { CB_Error(SyntaxERR); return; }	// Syntax error
	} else
	if ( c==0xF9 ) {
		c = SRC[ExecPtr+1] ; 
		if ( c == 0x21 ) {	// Xdot
				DefaultValue = Xdot ;
				flag=1;
		} else { CB_Error(SyntaxERR); return; }	// Syntax error
	} else { CB_Error(SyntaxERR); return; }	// Syntax error

	if ( flag ) {
			sprintGR(buffer, DefaultValue, 22-CursorX,RIGHT_ALIGN, CB_Round.MODE, CB_Round.DIGIT);
			locate( CursorX, CursorY); Print((unsigned char*)buffer);
			Scrl_Y();
			CB_CurrentValue = InputNumD_CB1( 1, CursorY, 21, DefaultValue );
			CBint_CurrentValue = CB_CurrentValue ;
			if ( flagint ) {
				CBint_Store( SRC );
			} else {
				CB_Store( SRC );
			}
	} else	{
			CB_CurrentValue = InputNumD_CB( 1, CursorY, 21, 0 );
			CBint_CurrentValue = CB_CurrentValue ;
	}
	Scrl_Y();
	Bdisp_PutDisp_DD_DrawBusy();
	return 0 ;
}


//-----------------------------------------------------------------------------
int GetQuotOpcode(unsigned char *SRC, unsigned char *buffer, int Maxlen) {
	unsigned int c;
	int ptr=0;
	while (1){
		c = SRC[ExecPtr++];
		buffer[ptr++]=c;
		switch ( c ) {
			case 0x00:	// <EOF>
			case 0x22:	// "
				buffer[--ptr]='\0' ;
				return ptr;
			case 0x5C:	//
				buffer[ptr-1]=SRC[ExecPtr++];
				break;
			case 0x7F:	// 
			case 0xF7:	// 
			case 0xF9:	// 
			case 0xE5:	// 
			case 0xE6:	// 
			case 0xE7:	// 
			case 0xFF:	// 
				buffer[ptr++]=SRC[ExecPtr++];
				break;
			default:
				break;
		}
		if ( ptr >= Maxlen-1 ) { CB_Error(StringTooLongERR); break; }	// String too Long error
	}
	return ptr;
}

void GetQuotStr(unsigned char *SRC, unsigned char *buffer, int Maxlen ) {
	unsigned char tmpbuf[18];
	int i,j=0,len,ptr=0;
	unsigned short c=1;
	while ( c != '\0' ) {
		c = SRC[ExecPtr++] ; 
		if ( c==0x22 ) break ; // "
		else
		if ( c==0x5C ) { // Backslash
			c = SRC[ExecPtr++] ;
		} else {
			if ( (c==0x7F)||(c==0xF7)||(c==0xF9)||(c==0xE5)||(c==0xE6)||(c==0xE7)||(c==0xFF) ) {
					c = ( c<<8 ) + SRC[ExecPtr++];
			}
		}
		CB_OpcodeToStr( c, tmpbuf ) ;	// SYSCALL
		len = strlen( (char*)tmpbuf ) ;
		i=0;
		while ( i < len ) buffer[ptr++]=tmpbuf[i++] ;
		if ( ptr >= Maxlen-1 ) { CB_Error(StringTooLongERR); break; }	// String too Long error
	}
	buffer[ptr]='\0' ;
}

void GetLocateStr(unsigned char *SRC, unsigned char *buffer, int Maxlen ) {
	unsigned char tmpbuf[18];
	int i,j=0,len,ptr=0;
	unsigned short c=1;
	while ( c != '\0' ) {
		c = SRC[ExecPtr++] ; 
		if ( c==0x22 ) break ; // "
		else
		if ( c==0x0D ) break ; // <CR>
		else
		if ( c==0x5C ) { // Backslash
			c = SRC[ExecPtr++] ;
		} else {
			if ( (c==0x7F)||(c==0xF7)||(c==0xF9)||(c==0xE5)||(c==0xE6)||(c==0xE7)||(c==0xFF) ) {
					c = ( c<<8 ) + SRC[ExecPtr++];
			}
		}
		CB_OpcodeToStr( c, tmpbuf ) ;	// SYSCALL
		len = strlen( (char*)tmpbuf ) ;
		i=0;
		while ( i < len ) buffer[ptr++]=tmpbuf[i++] ;
		if ( ptr >= Maxlen-1 ) { CB_Error(StringTooLongERR); break; }	// String too Long error
	}
	buffer[ptr]='\0' ;
}


void CB_Quot( unsigned char *SRC ){		// "" ""
	unsigned char buffer[256];
	unsigned int c,d;
	int ptr,len,i=0;
	ptr=ExecPtr;
	len=GetQuotOpcode( SRC, buffer, 255 );
	c = SRC[ExecPtr] ; 
	if ( c == 0x0E ) {	// -> Assign str
		ExecPtr++;
		c = SRC[ExecPtr] ; 
		d = SRC[ExecPtr+1] ; 
		if ( ( c == 0x7F ) && ( d == 0xF0 ) ) { // GraphY(n)
			ExecPtr+=2;
			c = SRC[ExecPtr] ; 
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
					{ CB_Error(ArgumentERR); return; }	// Argument error
					break;
			}
			if ( len > GraphStrMAX-1 ) len=GraphStrMAX-1;
			strncpy( (char *)GraphY, (const char *)buffer, len);
			GraphY[len]='\0';
		}
	} else {			// display str
		ExecPtr=ptr;
		GetQuotStr( SRC, buffer, 255 );
		CB_SelectTextVRAM();	// Select Text Screen
		if ( CursorX >1 ) Scrl_Y();
		while ( buffer[i] ) {
			CB_PrintC( CursorX,CursorY, (unsigned char*)buffer+i );
			CursorX++; if (CursorX > 21) Scrl_Y();
			c = buffer[i] ;
			if ( ( c==0x0C ) || ( c==0x0D ) ) Scrl_Y();
			if ( (c==0x7F)||(c==0xF7)||(c==0xF9)||(c==0xE5)||(c==0xE6)||(c==0xE7)||(c==0xFF) ) i++;
			i++;
		}
		if ( buffer[0]=='\0' ) CursorX=22;
		Bdisp_PutDisp_DD_DrawBusy();
	}
}


void PrintDone() {
	if ( CursorX >1 ) Scrl_Y();
	locate( CursorX, CursorY); Print((unsigned char*)"                 Done");
	CursorX=21;
}
void CB_Done(){
	if ( ScreenMode == 1 ) {	// Graphic mode
		CB_SelectTextVRAM();	// Select Text Screen
		PrintDone();
		CB_SelectGraphVRAM();	// Select Graphic Screen
	} else {
		PrintDone();
	}
}

void PlotXYtoPrevPXY() {
//		Previous_X=Plot_X;
//		Previous_Y=Plot_Y;
		VWtoPXY( Plot_X, Plot_Y, &Previous_PX, &Previous_PY );
}
void PlotPreviousPXY() {
	if ( Previous_PX > 0 ) LinesubSetPoint(Previous_PX, Previous_PY);
	PlotXYtoPrevPXY();
}
void PlotCurrentXY(){
	PlotXYtoPrevPXY();
	if ( ScreenMode == 0 ) {	//Text mode
		CB_SelectGraphVRAM();	// Select Graphic Screen
		PlotPreviousPXY();
		CB_SelectTextVRAM();	// Select Text Screen
	} else {
		PlotPreviousPXY();
	}
}

unsigned int GWait( int exit_cancel ) {
	unsigned int key=0,key2=0;
	int cont=1;
	if (ScreenMode) {
		if ( (UseGraphic==1)||(UseGraphic==2) ) { key=KEY_CTRL_SHIFT; key2=KEY_CTRL_F6; CB_SelectTextVRAM(); }
	}
	while (cont) {
		if (key==0) GetKey(&key);
		switch ( key ) {
			case KEY_CTRL_AC:
				if ( ScreenMode==0 ) cont =0 ;
				else { if (exit_cancel) cont=0; 
						else { key=KEY_CTRL_SHIFT; key2=KEY_CTRL_F6; cont=1; }
				}
				break;
			case KEY_CTRL_EXE:
				if (exit_cancel==0) { 
					if ( ScreenMode==0 ) cont=0;
					else { key=KEY_CTRL_SHIFT; key2=KEY_CTRL_F6; }	// G<>T
				} else { cont=0;
					if ( UseGraphic == 1 ) PlotXYtoPrevPXY();
				}
				break;
			case KEY_CTRL_EXIT:
				if (exit_cancel==0) { 
					if ( ScreenMode==0 ) cont =0 ;
					else { key=KEY_CTRL_SHIFT; key2=KEY_CTRL_F6; }	// G<>T
				} else { key=0; key2=0;
					if ( UseGraphic == 1 ) PlotXYtoPrevPXY();
				}
				break;
			case KEY_CTRL_SHIFT:
				SaveDisp(SAVEDISP_PAGE1);		// ------ SaveDisp1
				locate(1,8); PrintLine((unsigned char*)" ",21);
				Fkey_dispR( 0, "Var");
				Fkey_dispR( 1, "Mat");
				Fkey_dispR( 2, "V-W");
				Fkey_Clear( 3 );
				Fkey_Clear( 4 );
				Fkey_dispN( 5, "G<>T");
				if (key2==0) GetKey(&key2);
				switch (key2) {
					case KEY_CTRL_SETUP:
							selectSetup=SetupG(selectSetup);
							key=0; key2=0;
							break;
					case KEY_CTRL_F1:
							selectVar=SetVar(selectVar);		// A - 
							key=0; key2=0;
							break;
					case KEY_CTRL_F2:
							selectMatrix=SetMatrix(selectMatrix);		// 
							key=0; key2=0;
							break;
					case KEY_CTRL_F3:
							SetViewWindow();
							key=0; key2=0;
							break;
					case KEY_CTRL_F6:
							RestoreDisp(SAVEDISP_PAGE1);	// ------ RestoreDisp1
							key=0; key2=0;
							if (ScreenMode) CB_SelectTextVRAM();	// Select Text Screen
								else		CB_SelectGraphVRAM();	// Select Graphic Screen
							if (ScreenMode) {
								if (UseGraphic==1) {
									key=Plot();	// Plot
									if ( key==KEY_CTRL_EXE ) {
//										if (exit_cancel==0) { //  end program
											PlotCurrentXY(); // current plot
//										}
									}
								}
								if (UseGraphic==2) key=Graph_main();
								if ( key==KEY_CTRL_AC   ) { if (exit_cancel) cont=0; 
																else { key=KEY_CTRL_SHIFT; key2=KEY_CTRL_F6; } }
								if ( key==KEY_CTRL_EXIT ) { key=KEY_CTRL_SHIFT; key2=KEY_CTRL_F6; }
								if ( key==KEY_CTRL_F6   ) { key=KEY_CTRL_SHIFT; key2=KEY_CTRL_F6; }
							}
							SaveDisp(SAVEDISP_PAGE1);		// ------ SaveDisp1
							break;
					default:
						key=0; key2=0;
						break;
				}
				RestoreDisp(SAVEDISP_PAGE1);	// ------ RestoreDisp1
				break;
			case KEY_CTRL_OPTN:	//
				SaveDisp(SAVEDISP_PAGE1);		// ------ SaveDisp1
				key=Pict();
				switch (key) {
					case KEY_CTRL_AC:
					case KEY_CTRL_EXE:
						cont=0;
						break;
					case KEY_CTRL_SHIFT:
						break;
					default:
						break;
				}
				break;
			default:
				key=0; key2=0;
				break;
		}
	}
	return key;
}

int CB_Disps( unsigned char *SRC ,int dspflag){
	unsigned char buffer[32];
	unsigned int c;
	unsigned int key=0;
	int scrmode;
	
	KeyRecover();
	scrmode=ScreenMode;
	if ( dspflag == 2 ) { CB_SelectTextVRAM();	// Select Text Screen
		if ( CursorX >1 ) Scrl_Y();
		if ( CB_INT )
			sprintGR(buffer, CBint_CurrentValue, 22-CursorX,RIGHT_ALIGN, CB_Round.MODE, CB_Round.DIGIT);
		else
			sprintGR(buffer, CB_CurrentValue, 22-CursorX,RIGHT_ALIGN, CB_Round.MODE, CB_Round.DIGIT);
		locate( CursorX, CursorY); Print((unsigned char*)buffer);
		CursorX=21;
		scrmode=ScreenMode;
	} else
	if ( dspflag == 3 ) { 	// Matrix display
		c=SRC[ExecPtr-2];
		if ( ( 'A' <= c ) && ( c <= 'Z' ) ) {
			CB_SelectTextVRAM();	// Select Text Screen
			CB_SaveTextVRAM();
			EditMatrix( c -'A' );
			CB_RestoreTextVRAM();	// Resotre Graphic screen
			if ( scrmode  ) CB_SelectGraphVRAM();	// Select Graphic screen
			scrmode=ScreenMode;
		}
	}
	if (scrmode) {
		CB_SelectTextVRAM();	// Select Text Screen
		PrintDone();
	}
	if ( CursorX >1 ) Scrl_Y();
	locate( CursorX, CursorY); Print((unsigned char*)"             - Disp -");
	if ( scrmode ) CB_SelectGraphVRAM();	// Select Graphic Screen
	if (UseGraphic>0x100) UseGraphic=UseGraphic&0xFF; 
	Bdisp_PutDisp_DD();
	
	while ( 1 ) {
		key=GWait(EXIT_CANCEL_ON);
		if ( key == KEY_CTRL_EXE ) break ;
		if ( key == KEY_CTRL_AC  ) { ExecPtr--; return 1 ; }	// break;
	}

	CB_SelectTextVRAM();	// Select Text Screen
	locate( CursorX, CursorY); Print((unsigned char*)"                     ");
	CursorX=1;
	if ( scrmode ) CB_SelectGraphVRAM();	// Select Graphic Screen

	if ( UseGraphic == 1 ) PlotXYtoPrevPXY(); // Plot
	if ( UseGraphic ) UseGraphic=UseGraphic | 0x100;  // 
	Bdisp_PutDisp_DD_DrawBusy();
	return 0;
}

void CB_end( unsigned char *SRC, int dspflag ){
	unsigned char buffer[32];
	unsigned int c,t;
	unsigned int key=0;
	int scrmode=ScreenMode;

	if ( ProgEntryN ) return ; //	in sub Prog
	ExecPtr++;
	
	KeyRecover();
	CB_SelectTextVRAM();	// Select Text Screen
	if ( dspflag == 2 ) {
		if ( CursorX >1 ) Scrl_Y();
		if ( CB_INT )
			sprintGR(buffer, CBint_CurrentValue, 22-CursorX,RIGHT_ALIGN, CB_Round.MODE, CB_Round.DIGIT);
		else
			sprintGR(buffer, CB_CurrentValue, 22-CursorX,RIGHT_ALIGN, CB_Round.MODE, CB_Round.DIGIT);
		locate( CursorX, CursorY); Print((unsigned char*)buffer);
		CursorX=21;
	} else
	if ( dspflag == 3 ) { 	// Matrix display
		c=SRC[ExecPtr-2];
		if ( ( 'A' <= c ) && ( c <= 'Z' ) ) {
			CB_SaveTextVRAM();
			EditMatrix( c -'A' );
			CB_RestoreTextVRAM();	// Resotre Graphic screen
			CB_Done();
		}
	}
	if ( dspflag == 0 ) CB_Done();
	if ( scrmode ) CB_SelectGraphVRAM();	// Select Graphic Screen
	if ( UseGraphic ) if ( ( UseGraphic != 1 ) &&( UseGraphic != 2 ) && ( UseGraphic != 99 ) ) { UseGraphic=UseGraphic&0xFF; CB_SelectTextVRAM(); }
	Bdisp_PutDisp_DD();

	while ( 1 ) {
		key=GWait(EXIT_CANCEL_OFF);
		if ( key == KEY_CTRL_EXE ) break ;
		if ( key == KEY_CTRL_EXIT ) break ;
		if ( key == KEY_CTRL_AC  ) return ;
	}
	
	CB_SelectTextVRAM();	// Select Text Screen
	if ( TimeDsp ) {
		if ( CursorX >1 ) Scrl_Y();
		sprintGRS(buffer, (double)(CB_TicksEnd-CB_TicksStart)/128.0, 8,RIGHT_ALIGN, Fix, 2);  // Fix:2
		locate(  1, CursorY); Print((unsigned char*)"Execute Time=");
		locate( 14, CursorY); Print((unsigned char*)buffer);
		while ( 1 ) {
			key=GWait(EXIT_CANCEL_OFF);
			if ( key == KEY_CTRL_EXE ) break ;
			if ( key == KEY_CTRL_EXIT ) break ;
			if ( key == KEY_CTRL_AC  ) return ;
		}
	}
	if ( (UseGraphic&0xFF) == 1 ) {	// Plot 
		PlotXYtoPrevPXY();
	}
}


void CB_Locate( unsigned char *SRC ){
	unsigned char buffer[32];
	unsigned int c;
	int lx,ly;
	double value;
	int	valueint;

	CB_SelectTextVRAM();	// Select Text Screen
	if (CB_INT)	lx = CBint_Eval( SRC );
	else		lx = CB_Eval( SRC );
	c=SRC[ExecPtr];
	if ( c != ',' ) { CB_Error(SyntaxERR); return; }	// Syntax error
	ExecPtr++;
	if (CB_INT)	ly = CBint_Eval( SRC );
	else		ly = CB_Eval( SRC );
	c=SRC[ExecPtr];
	if ( c != ',' ) { CB_Error(SyntaxERR); return; }	// Syntax error
	ExecPtr++;
	c=SRC[ExecPtr];
	if ( c == 0x22 ) {	// String
		ExecPtr++;
		GetLocateStr(SRC, buffer, 32);
		CB_Print( lx,ly, (unsigned char*)buffer );
	} else {			// expression
		if (CB_INT) {
			valueint = CBint_Eval( SRC );
			sprintGR(buffer, valueint, 22-lx,LEFT_ALIGN, CB_Round.MODE, CB_Round.DIGIT);
		} else {
			value = CB_Eval( SRC );
			sprintGR(buffer, value, 22-lx,LEFT_ALIGN, CB_Round.MODE, CB_Round.DIGIT);
		}
		CB_Print( lx,ly, (unsigned char*)buffer );
	}
	Bdisp_PutDisp_DD_DrawBusy_through(SRC);
}

void CB_Text( unsigned char *SRC, int *dspflag ) { //	Text
	unsigned int key;
	unsigned char buffer[64];
	unsigned int c;
	int px,py,d;
	double value;
	int	valueint;

	if ( RangeErrorCK(SRC) ) return;
	CB_SelectGraphVRAM();	// Select Graphic Screen
	if (CB_INT)	py = CBint_Eval( SRC );
	else		py = CB_Eval( SRC );
	c=SRC[ExecPtr];
	if ( c != ',' ) { CB_Error(SyntaxERR); return; }	// Syntax error
	ExecPtr++;
	if (CB_INT)	px = CBint_Eval( SRC );
	else		px = CB_Eval( SRC );
	c=SRC[ExecPtr];
	if ( c != ',' ) { CB_Error(SyntaxERR); return; }	// Syntax error
	ExecPtr++;
	c=SRC[ExecPtr];
	if ( c == 0x22 ) {	// String
		ExecPtr++;
		GetLocateStr(SRC, buffer,64);
		Text(py, px, buffer);
	} else {			// expression
		d=(128-px)/4;
		if (d>24) d=24;	// digit max
		if (CB_INT) {
			valueint = CBint_Eval( SRC );
			sprintGR(buffer, valueint, d,LEFT_ALIGN, CB_Round.MODE, CB_Round.DIGIT);
		} else {
			value = CB_Eval( SRC );
			sprintGR(buffer, value, d,LEFT_ALIGN, CB_Round.MODE, CB_Round.DIGIT);
		}
		Text(py, px, buffer);
	}
	Bdisp_PutDisp_DD_DrawBusy_through(SRC);
}

//-----------------------------------------------------------------------------

void Skip_quot( unsigned char *SRC ){ // skip "..."
	unsigned int c;
	while (1){
		c=SRC[ExecPtr++];
		switch ( c ) {
			case 0x00:	// <EOF>
				ExecPtr--;
			case 0x22:	// "
//			case 0x3A:	// <:>
//			case 0x0C:	// dsps
//			case 0x0D:	// <CR>
				return ;
				break;
			case 0x7F:	// 
			case 0xF7:	// 
			case 0xF9:	// 
			case 0xE5:	// 
			case 0xE6:	// 
			case 0xE7:	// 
//			case 0xFF:	// 
				ExecPtr++;
				break;
		}
	}
}
void Skip_block( unsigned char *SRC ){
	unsigned int c;
	while (1){
		c=SRC[ExecPtr++];
		switch ( c ) {
			case 0x00:	// <EOF>
				ExecPtr--;
			case 0x3A:	// <:>
			case 0x0C:	// dsps
			case 0x0D:	// <CR>
				return ;
				break;
			case 0x22:	// "
				Skip_quot(SRC);
				break;
			case 0x7F:	// 
			case 0xF7:	// 
			case 0xF9:	// 
			case 0xE5:	// 
			case 0xE6:	// 
			case 0xE7:	// 
//			case 0xFF:	// 
				ExecPtr++;
				break;
		}
	}
}
void Skip_rem( unsigned char *SRC ){	// skip '...
	unsigned int c;
	while (1){
		c=SRC[ExecPtr++];
		if ( c=='#' ) {
			if ( strncmp( (char*)SRC+ExecPtr,"CBint",5)  == 0 ) { ExecPtr+=5; CB_INT=1; }
			if ( strncmp( (char*)SRC+ExecPtr,"CBINT",5)  == 0 ) { ExecPtr+=5; CB_INT=1; }
			if ( strncmp( (char*)SRC+ExecPtr,"CBdbl",5)  == 0 ) { ExecPtr+=5; CB_INT=0; }
			if ( strncmp( (char*)SRC+ExecPtr,"CBDBL",5)  == 0 ) { ExecPtr+=5; CB_INT=0; }
			if ( strncmp( (char*)SRC+ExecPtr,"CBasic",6) == 0 ) { ExecPtr+=6; CB_INT=0; }
			if ( strncmp( (char*)SRC+ExecPtr,"CBASIC",6) == 0 ) { ExecPtr+=6; CB_INT=0; }
		} else 
		switch ( c ) {
			case 0x00:	// <EOF>
				ExecPtr--;
//			case 0x3A:	// <:>
			case 0x0C:	// dsps
			case 0x0D:	// <CR>
				return ;
				break;
			case 0x22:	// "
				Skip_quot(SRC);
				break;
			case 0x7F:	// 
			case 0xF7:	// 
			case 0xF9:	// 
			case 0xE5:	// 
			case 0xE6:	// 
			case 0xE7:	// 
//			case 0xFF:	// 
				ExecPtr++;
				break;
		}
	}
}

//-----------------------------------------------------------------------------

void CB_Lbl( unsigned char *SRC, int *StackGotoAdrs ){
	unsigned int c;
	int label;
	c=SRC[ExecPtr];
	if ( ( '0' <= c ) && ( c <= '9' ) ) {
		ExecPtr++;
		label = c-'0';
	} else if ( ( 'A' <= c ) && ( c <= 'Z' ) ) {
		ExecPtr++;
		label = c-'A'+10;
	} else { CB_Error(SyntaxERR); return; }	// syntax error
	if ( StackGotoAdrs[label] == 0 ) StackGotoAdrs[label]=ExecPtr;
}

int Search_Lbl( unsigned char *SRC, unsigned int lc ){
	unsigned int c;
	int bptr=ExecPtr;
	ExecPtr=0;
	while (1){
		c=SRC[ExecPtr++];
		switch ( c ) {
			case 0x00:	// <EOF>
				ExecPtr=bptr;
				return 0 ;
				break;
			case 0x27:	// ' rem
				Skip_block(SRC);
				break;
			case 0xE2:	// Lbl
				c=SRC[ExecPtr++];
				if ( c == lc ) return 1 ;
				break;
			case 0xF7:	// 
			case 0x7F:	// 
			case 0xF9:	// 
			case 0xE5:	// 
			case 0xE6:	// 
			case 0xE7:	// 
//			case 0xFF:	// 
				ExecPtr++;
				break;
		}
	}
	ExecPtr=bptr;
	return 0;
}

void CB_Goto( unsigned char *SRC, int *StackGotoAdrs){
	unsigned int c;
	int label;
	int ptr;
	c=SRC[ExecPtr];
	if ( ( '0' <= c ) && ( c <= '9' ) ) {
		ExecPtr++;
		label = c-'0';
	} else if ( ( 'A' <= c ) && ( c <= 'z' ) ) {
		ExecPtr++;
		label = c-'A'+10;
	} else { CB_Error(SyntaxERR); return; }	// goto error
	
	ptr = StackGotoAdrs[label] ;
	if ( ptr == 0 ) {
		if ( Search_Lbl(SRC, c) == 0 ) { CB_Error(UndefinedLabelERR); return; }	// undefined label error
		ExecPtr++;
		StackGotoAdrs[label]=ExecPtr;
	} else  ExecPtr = ptr ;
}

//-----------------------------------------------------------------------------
int Search_IfEnd( unsigned char *SRC ){
	unsigned int c;
	while (1){
		c=SRC[ExecPtr++];
		switch ( c ) {
			case 0x00:	// <EOF>
				ExecPtr--;
				return 0 ;
				break;
			case 0x27:	// ' rem
				Skip_block(SRC);
				break;
			case 0xF7:	// 
				c=SRC[ExecPtr++];
				if ( c == 0x00 ) { 			// If
					Search_IfEnd(SRC);
					break;
				} else
				if ( c == 0x03 ) return 1;	// IfEnd
				break ;
			case 0x7F:	// 
			case 0xF9:	// 
			case 0xE5:	// 
			case 0xE6:	// 
			case 0xE7:	// 
//			case 0xFF:	// 
				ExecPtr++;
				break;
		}
	}
	return 0;
}

int Search_ElseIfend( unsigned char *SRC ){
	unsigned int c;
	while (1){
		c=SRC[ExecPtr++];
		switch ( c ) {
			case 0x00:	// <EOF>
				ExecPtr--;
				return 0 ;
				break;
			case 0x27:	// ' rem
				Skip_block(SRC);
				break;
			case 0xF7:	// 
				c=SRC[ExecPtr++];
				if ( c == 0x00 ) {			// If
					Search_IfEnd(SRC);
					break;
				} else
				if ( c == 0x02 ) return 1; 	// Else
				else
				if ( c == 0x03 ) return 1; 	// IfEnd
				break;
			case 0x7F:	// 
			case 0xF9:	// 
			case 0xE5:	// 
			case 0xE6:	// 
			case 0xE7:	// 
//			case 0xFF:	// 
				ExecPtr++;
				break;
		}
	}
	return 0;
}

//-----------------------------------------------------------------------------
void CB_If( unsigned char *SRC ){
	unsigned int c,c2;
	int value;
	int ptr;
	if (CB_INT) value = CBint_Eval( SRC );
	else		value = CB_Eval( SRC );
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

void CB_Else( unsigned char *SRC ){
	Search_IfEnd(SRC);
}
void CB_IfEnd( unsigned char *SRC ){
}

//-----------------------------------------------------------------------------
void CB_For( unsigned char *SRC ,int *StackForPtr, int *StackForAdrs, int *StackForVar, double *StackForEnd, double *StackForStep, int *CurrentStructCNT, int *CurrentStructloop){
	unsigned int c;
	CB_CurrentValue = CB_Eval( SRC );
	c=SRC[ExecPtr];
	if ( c == 0x0E ) {	// ->
		ExecPtr++;
		c=SRC[ExecPtr];
		if ( ( 'A' <= c ) && ( c <= 'z' ) ) {
		StackForVar[*StackForPtr]=c-'A';
		CB_Store(SRC);
		} else { CB_Error(SyntaxERR); return; }	// Syntax error
	}
	if ( *StackForPtr >= StackForMax-1 ) { CB_Error(NestingERR); return; } //  nesting error
	c=SRC[ExecPtr];
	if ( ( c != 0xF7 ) || ( SRC[ExecPtr+1] != 0x05 ) ) { CB_Error(SyntaxERR); return; }	// Syntax error
	ExecPtr+=2;
	StackForEnd[*StackForPtr] = CB_Eval( SRC );
	c=SRC[ExecPtr];
	if ( ( c == 0xF7 ) && ( SRC[ExecPtr+1] == 0x06 ) ) {	// Step
		ExecPtr+=2;
		StackForStep[*StackForPtr] = CB_Eval( SRC );
	} else {
		StackForStep[*StackForPtr] = 1;
	}
	StackForAdrs[*StackForPtr] = ExecPtr;
	(*StackForPtr)++;
	CurrentStructloop[*CurrentStructCNT]=1;
	(*CurrentStructCNT)++;
}
void CB_Next( unsigned char *SRC ,int *StackForPtr, int *StackForAdrs, int *StackForVar, double *StackForEnd, double *StackForStep, int *CurrentStructCNT, int *CurrentStructloop ){
	double step,end;
	if ( *StackForPtr <= 0 ) { ErrorNo=NextWithoutForERR; ErrorPtr=ExecPtr; return; } // Next without for error
	(*StackForPtr)--;
	(*CurrentStructCNT)--;
	step = StackForStep[*StackForPtr];
	REG[StackForVar[*StackForPtr]]+=step;
	if ( step > 0 ) { 	// step +
		if ( REG[StackForVar[*StackForPtr]] > StackForEnd[*StackForPtr] ) return ; // exit
		ExecPtr = StackForAdrs[*StackForPtr];
		(*StackForPtr)++;		// continue
	CurrentStructloop[*CurrentStructCNT]=1;
	(*CurrentStructCNT)++;
	}
	else {									// step -
		if ( REG[StackForVar[*StackForPtr]] < StackForEnd[*StackForPtr] ) return ; // exit
		ExecPtr = StackForAdrs[*StackForPtr];
		(*StackForPtr)++;		// continue
	CurrentStructloop[*CurrentStructCNT]=1;
	(*CurrentStructCNT)++;
	}
}

//-----------------------------------------------------------------------------
int Search_Next( unsigned char *SRC ){
	unsigned int c;
	while (1){
		c=SRC[ExecPtr++];
		switch ( c ) {
			case 0x00:	// <EOF>
				ExecPtr--;
				return 0 ;
				break;
			case 0x27:	// ' rem
				Skip_block(SRC);
				break;
			case 0xF7:	// 
				if ( SRC[ExecPtr] == 0x04 ) { ExecPtr++;				// For
					Search_Next(SRC);
					break;
				}
				if ( SRC[ExecPtr] == 0x07 ) { ExecPtr++; return 1; }	// Next
			case 0x7F:	// 
			case 0xF9:	// 
			case 0xE5:	// 
			case 0xE6:	// 
			case 0xE7:	// 
//			case 0xFF:	// 
				ExecPtr++;
				break;
		}
	}
	return 0;
}
int Search_WhileEnd( unsigned char *SRC ){
	unsigned int c;
	while (1){
		c=SRC[ExecPtr++];
		switch ( c ) {
			case 0x00:	// <EOF>
				ExecPtr--;
				return 0 ;
				break;
			case 0x27:	// ' rem
				Skip_block(SRC);
				break;
			case 0xF7:	// 
				if ( SRC[ExecPtr] == 0x08 ) { ExecPtr++;				// While
					Search_WhileEnd(SRC);
					break;
				}
				if ( SRC[ExecPtr] == 0x09 ) { ExecPtr++; return 1; }	// WhileEnd
			case 0x7F:	// 
			case 0xF9:	// 
			case 0xE5:	// 
			case 0xE6:	// 
			case 0xE7:	// 
//			case 0xFF:	// 
				ExecPtr++;
				break;
		}
	}
	return 0;
}
int Search_LpWhile( unsigned char *SRC ){
	unsigned int c;
	while (1){
		c=SRC[ExecPtr++];
		switch ( c ) {
			case 0x00:	// <EOF>
				ExecPtr--;
				return 0 ;
				break;
			case 0x27:	// ' rem
				Skip_block(SRC);
				break;
			case 0xF7:	// 
				if ( SRC[ExecPtr] == 0x0A ) { ExecPtr++;				// Do
					Search_LpWhile(SRC);
					break;
				}
				if ( SRC[ExecPtr] == 0x0B ) { ExecPtr++; return 1; }	// LpWhile
			case 0x7F:	// 
			case 0xF9:	// 
			case 0xE5:	// 
			case 0xE6:	// 
			case 0xE7:	// 
//			case 0xFF:	// 
				ExecPtr++;
				break;
		}
	}
	return 0;
}

void CB_While( unsigned char *SRC, int *StackWhilePtr, int *StackWhileAdrs, int *CurrentStructCNT, int *CurrentStructloop  ) {
	int wPtr=ExecPtr;
	int i;
	if (CB_INT) i=CBint_Eval( SRC ); else i=CB_Eval( SRC );
	if ( i == 0 ) {		// false
		if ( Search_WhileEnd(SRC) == 0 ) { CB_Error(WhileWithoutWhileEndERR); return; }  // While without WhileEnd error
		return ; // exit
	}
	if ( *StackWhilePtr >= StackWhileMax-1 ) { CB_Error(NestingERR); return; }  //  nesting error
	StackWhileAdrs[*StackWhilePtr] = wPtr;
	(*StackWhilePtr)++;
	CurrentStructloop[*CurrentStructCNT]=2;
	(*CurrentStructCNT)++;
}

void CB_WhileEnd( unsigned char *SRC, int *StackWhilePtr, int *StackWhileAdrs, int *CurrentStructCNT, int *CurrentStructloop  ) {
	int exitPtr=ExecPtr;
	int i;
	if ( *StackWhilePtr <= 0 ) { CB_Error(WhileEndWithoutWhileERR); return; }  // WhileEnd without While error
	(*StackWhilePtr)--;
	(*CurrentStructCNT)--;
	ExecPtr = StackWhileAdrs[*StackWhilePtr] ;
	if (CB_INT) i=CBint_Eval( SRC ); else i=CB_Eval( SRC );
	if ( i == 0 ) {		// false
		ExecPtr=exitPtr;
		return ; // exit
	}
	(*StackWhilePtr)++;
	CurrentStructloop[*CurrentStructCNT]=2;
	(*CurrentStructCNT)++;
}

void CB_Do( unsigned char *SRC, int *StackDoPtr, int *StackDoAdrs, int *CurrentStructCNT, int *CurrentStructloop ) {
	if ( *StackDoPtr >= StackDoMax-1 ) { CB_Error(NestingERR); return; }  //  nesting error
	StackDoAdrs[*StackDoPtr] = ExecPtr;
	(*StackDoPtr)++;
	CurrentStructloop[*CurrentStructCNT]=3;
	(*CurrentStructCNT)++;
}

void CB_LpWhile( unsigned char *SRC, int *StackDoPtr, int *StackDoAdrs, int *CurrentStructCNT, int *CurrentStructloop ) {
	int i;
	if ( *StackDoPtr <= 0 ) { CB_Error(LpWhileWithoutDoERR); return; }  // LpWhile without Do error
	(*StackDoPtr)--;
	(*CurrentStructCNT)--;
	if (CB_INT) i=CBint_Eval( SRC ); else i=CB_Eval( SRC );
	if ( i == 0  ) return ; // exit
	ExecPtr = StackDoAdrs[*StackDoPtr] ;				// true
	(*StackDoPtr)++;
	CurrentStructloop[*CurrentStructCNT]=3;
	(*CurrentStructCNT)++;
}

void CB_Break( unsigned char *SRC, int *StackForPtr, int *StackWhilePtr, int *StackDoPtr, int *CurrentStructCNT, int *CurrentStructloop ) {
	
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

void CB_Dsz( unsigned char *SRC ) { //	Dsz
	unsigned int c;
	int reg,mptr;
	char*	MatAryC;
	short*	MatAryW;
	int*	MatAryI;
	c=SRC[ExecPtr];
	if ( ( 'A' <= c ) && ( c <= 'z' ) ) {
		ExecPtr++;
		reg=c-'A';
		c=SRC[ExecPtr];
		if ( c=='%' ) {
			ExecPtr++;
			REGINT[reg] --;
			CB_CurrentValue = REGINT[reg] ;
		} else {
			if ( c=='#' ) ExecPtr++;
			REG[reg] --;
			CB_CurrentValue = REG[reg] ;
		}
	} else 
	if ( c==0x7F ) {
		c = SRC[ExecPtr+1] ; 
		if ( c == 0x40 ) {	// Mat A[a,b]
			ExecPtr+=2;
			mptr=MatOprand( SRC, &reg);
			if ( ErrorNo ) return ; // error
			switch ( MatAryElementSize[reg] ) {
				case 8:
					MatAry[reg][mptr] --;
					CB_CurrentValue = MatAry[reg][mptr];			// Matrix array double
					break;
				case 1:
					MatAryC=(char*)MatAry[reg];
					MatAryC[mptr] --;
					CB_CurrentValue = MatAryC[mptr]   ;			// Matrix array char
					break;
				case 2:
					MatAryW=(short*)MatAry[reg];
					MatAryW[mptr] --;
					CB_CurrentValue = MatAryW[mptr]   ;			// Matrix array word
					break;
				case 4:
					MatAryI=(int*)MatAry[reg];
					MatAryI[mptr] --;
					CB_CurrentValue = MatAryI[mptr]   ;			// Matrix array int
					break;
			}
		}
	} else { CB_Error(SyntaxERR); return; }	// Syntax error

	c=SRC[ExecPtr];
	if ( ( c==':' ) || ( c==0x0D ) ) {
		if ( CB_CurrentValue ) return ;
		else { ExecPtr++;
			Skip_block(SRC);
		}
	} else if ( c==0x0C ) {  // dsps
		ExecPtr++;
		CB_Disps( SRC ,2);
		if ( CB_CurrentValue ) return ;
		else {
			Skip_block(SRC);
		}
	} else { ErrorNo=SyntaxERR; ErrorPtr=ExecPtr; return; }	// Syntax error
}

void CB_Isz( unsigned char *SRC ) { //	Isz
	unsigned int c;
	int reg,mptr;
	char*	MatAryC;
	short*	MatAryW;
	int*	MatAryI;
	c=SRC[ExecPtr];
	if ( ( 'A' <= c ) && ( c <= 'z' ) ) {
		ExecPtr++;
		reg=c-'A';
		c=SRC[ExecPtr];
		if ( c=='%' ) {
			ExecPtr++;
			REGINT[reg] ++;
			CB_CurrentValue = REGINT[reg] ;
		} else {
			if ( c=='#' ) ExecPtr++;
			REG[reg] ++;
			CB_CurrentValue = REG[reg] ;
		}
	} else 
	if ( c==0x7F ) {
		c = SRC[ExecPtr+1] ; 
		if ( c == 0x40 ) {	// Mat A[a,b]
			ExecPtr+=2;
			mptr=MatOprand( SRC, &reg);
			if ( ErrorNo ) return ; // error
			switch ( MatAryElementSize[reg] ) {
				case 8:
					MatAry[reg][mptr] ++;
					CB_CurrentValue = MatAry[reg][mptr];			// Matrix array double
					break;
				case 1:
					MatAryC=(char*)MatAry[reg];
					MatAryC[mptr] ++;
					CB_CurrentValue = MatAryC[mptr]   ;			// Matrix array char
					break;
				case 2:
					MatAryW=(short*)MatAry[reg];
					MatAryW[mptr] ++;
					CB_CurrentValue = MatAryW[mptr]   ;			// Matrix array word
					break;
				case 4:
					MatAryI=(int*)MatAry[reg];
					MatAryI[mptr] ++;
					CB_CurrentValue = MatAryI[mptr]   ;			// Matrix array int
					break;
			}
		}
	} else { CB_Error(SyntaxERR); return; }	// Syntax error

	c=SRC[ExecPtr];
	if ( ( c==':' ) || ( c==0x0D ) ) {
		if ( CB_CurrentValue ) return ;
		else { ExecPtr++;
			Skip_block(SRC);
		}
	} else if ( c==0x0C ) {  // dsps
		ExecPtr++;
		CB_Disps( SRC ,2);
		if ( CB_CurrentValue ) return ;
		else {
			Skip_block(SRC);
		}
	} else { ErrorNo=SyntaxERR; ErrorPtr=ExecPtr; return; }	// Syntax error
}

//----------------------------------------------------------------------------------------------

void CB_ViewWindow( unsigned char *SRC ) { //	ViewWindow
	unsigned int c;
	int reg=0;
	while ( reg <= 10 ) {
		c=SRC[ExecPtr];
		if ( (c==':') || (c==0x0C) || (c==0x0D) || (c==0x00) ) break;
		if (CB_INT)	REGv[reg]=CBint_Eval( SRC );
		else		REGv[reg]=CB_Eval( SRC );
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


void CB_FLine( unsigned char *SRC) { //	F-Line
	int c;
	double x1,y1,x2,y2;
	int style=S_L_Style;

	if ( RangeErrorCK(SRC) ) return;
	if (CB_INT)	x1=CBint_Eval( SRC );
	else		x1=CB_Eval( SRC );
	c=SRC[ExecPtr];
	if ( c != ',' ) { CB_Error(SyntaxERR); return; }  // Syntax error
	ExecPtr++;
	if (CB_INT)	y1=CBint_Eval( SRC );
	else		y1=CB_Eval( SRC );
	c=SRC[ExecPtr];
	if ( c != ',' ) { CB_Error(SyntaxERR); return; }  // Syntax error
	ExecPtr++;
	if (CB_INT)	x2=CBint_Eval( SRC );
	else		x2=CB_Eval( SRC );
	c=SRC[ExecPtr];
	if ( c != ',' ) { CB_Error(SyntaxERR); return; }  // Syntax error
	ExecPtr++;
	if (CB_INT)	y2=CBint_Eval( SRC );
	else		y2=CB_Eval( SRC );

	if ( tmp_Style >= 0 ) style=tmp_Style;
	CB_SelectGraphVRAM();	// Select Graphic Screen
	F_Line(x1, y1, x2, y2, style);
	Bdisp_PutDisp_DD_DrawBusy_skip_through(SRC);
	tmp_Style = -1;
}

void CB_Line( unsigned char *SRC ) { //	Line
	int style=S_L_Style;
	if ( tmp_Style >= 0 ) style=tmp_Style;
	if ( RangeErrorCK(SRC) ) return;
	CB_SelectGraphVRAM();	// Select Graphic Screen
	Line(style);
	Bdisp_PutDisp_DD_DrawBusy_skip_through(SRC);
	tmp_Style = -1;
}

void CB_Vertical( unsigned char *SRC ) { //	Vertical
	double x;
	int style=S_L_Style;
	if ( tmp_Style >= 0 ) style=tmp_Style;
	if ( RangeErrorCK(SRC) ) return;
	if (CB_INT)	x=CBint_Eval( SRC );
	else		x=CB_Eval( SRC );
	CB_SelectGraphVRAM();	// Select Graphic Screen
	Vertical(x, style);
	Bdisp_PutDisp_DD_DrawBusy_skip_through(SRC);
	tmp_Style = -1;
}
void CB_Horizontal( unsigned char *SRC ) { //	Horizontal
	double y;
	int style=S_L_Style;
	if ( tmp_Style >= 0 ) style=tmp_Style;
	if ( RangeErrorCK(SRC) ) return;
	if (CB_INT)	y=CBint_Eval( SRC );
	else		y=CB_Eval( SRC );
	CB_SelectGraphVRAM();	// Select Graphic Screen
	Horizontal(y, style);
	Bdisp_PutDisp_DD_DrawBusy_skip_through(SRC);
	tmp_Style = -1;
}

void CB_Plot( unsigned char *SRC ) { //	Plot
	unsigned int c;
	double x,y;
	
	if ( RangeErrorCK(SRC) ) return;
	
	CB_SelectGraphVRAM();	// Select Graphic Screen
	c=SRC[ExecPtr];
	if ( ( c==':' ) || (c==0x0D) || ( c==0x0C ) || (c==0x00) ) {
		x=(Xmax+Xmin)/2;
		y=(Ymax+Ymin)/2;
	} else {
		if (CB_INT)	x=CBint_Eval( SRC );
		else		x=CB_Eval( SRC );
		c=SRC[ExecPtr];
		if ( c != ',' ) { CB_Error(SyntaxERR); return; }  // Syntax error
		ExecPtr++;
		if (CB_INT)	y=CBint_Eval( SRC );
		else		y=CB_Eval( SRC );
	}
	Plot_X = x;
	Plot_Y = y;
	regX = x;
	regY = y;
	regintX = x;
	regintY = y;
	PlotPreviousPXY();
	Bdisp_PutDisp_DD_DrawBusy_skip_through(SRC);
}

void CB_PlotOprand( unsigned char *SRC, double  *x, double *y) {
	if (CB_INT)	*x=CBint_Eval( SRC ); else *x=CB_Eval( SRC );
	if ( SRC[ExecPtr] != ',' ) { CB_Error(SyntaxERR); return; }  // Syntax error
	ExecPtr++;
	if (CB_INT)	*y=CBint_Eval( SRC ); else *y=CB_Eval( SRC );
	CB_SelectGraphVRAM();	// Select Graphic Screen
}

void CB_PlotOn( unsigned char *SRC ) { //	PlotOn
	double x,y;
	if ( RangeErrorCK(SRC) ) return;
	CB_PlotOprand( SRC, &x, &y);
	PlotOn_VRAM(x,y);
	Bdisp_PutDisp_DD_DrawBusy_skip_through(SRC);
}
void CB_PlotOff( unsigned char *SRC ) { //	PlotOff
	double x,y;
	if ( RangeErrorCK(SRC) ) return;
	CB_PlotOprand( SRC, &x, &y);
	PlotOff_VRAM(x,y);
	Bdisp_PutDisp_DD_DrawBusy_skip_through(SRC);
}
void CB_PlotChg( unsigned char *SRC ) { //	PlotChg
	double x,y;
	if ( RangeErrorCK(SRC) ) return;
	CB_PlotOprand( SRC, &x, &y);
	PlotChg_VRAM(x,y);
	Bdisp_PutDisp_DD_DrawBusy_skip_through(SRC);
}

void CB_PxlOprand( unsigned char *SRC, int *py, int *px) {
	double x,y;
	y = (CB_Eval( SRC ));
	*py=y;
	if ( ( y-floor(y) ) || ( (*py)<1 ) || ( (*py)>63 ) ) { CB_Error(ArgumentERR); return; }  // Argument error
	if ( SRC[ExecPtr] != ',' ) { CB_Error(SyntaxERR); return; }  // Syntax error
	ExecPtr++;
	x = (CB_Eval( SRC ));
	*px=x;
	if ( ( x-floor(x) ) || ( (*px)<1 ) || ( (*px)>127 ) ) { CB_Error(ArgumentERR); return; }  // Argument error}
	CB_SelectGraphVRAM();	// Select Graphic Screen
}
void CB_PxlOn( unsigned char *SRC ) { //	PxlOn
	int x,y;
	if ( RangeErrorCK(SRC) ) return;
	CB_PxlOprand( SRC, &y, &x);
	PxlOn_VRAM(y,x);
	Bdisp_PutDisp_DD_DrawBusy_skip_through(SRC);
}
void CB_PxlOff( unsigned char *SRC ) { //	PxlOff
	int x,y;
	if ( RangeErrorCK(SRC) ) return;
	CB_PxlOprand( SRC, &y, &x);
	PxlOff_VRAM(y,x);
	Bdisp_PutDisp_DD_DrawBusy_skip_through(SRC);
}
void CB_PxlChg( unsigned char *SRC ) { //	PxlChg
	int x,y;
	if ( RangeErrorCK(SRC) ) return;
	CB_PxlOprand( SRC, &y, &x);
	PxlChg_VRAM(y,x);
	Bdisp_PutDisp_DD_DrawBusy_skip_through(SRC);
}

//----------------------------------------------------------------------------------------------
void CB_DotOprand( unsigned char *SRC, int *px, int *py) {
	int x,y;
	if (CB_INT)	x=CBint_Eval( SRC ); else x=CB_Eval( SRC );
	*px=x;
	if ( ( (*px)<1 ) || ( (*px)>127 ) ) { CB_Error(ArgumentERR); return; }  // Argument error}
	if ( SRC[ExecPtr] != ',' ) { CB_Error(SyntaxERR); return; }  // Syntax error
	ExecPtr++;
	if (CB_INT)	y=CBint_Eval( SRC ); else y=CB_Eval( SRC );
	*py=y;
	if ( ( (*py)<1 ) || ( (*py)>63 ) ) { CB_Error(ArgumentERR); return; }  // Argument error
	CB_SelectGraphVRAM();	// Select Graphic Screen
}

void CB_DotP( unsigned char *SRC ){	// DotP(Mat B[x,y], px1,py1, px2,py2)
	unsigned int c;
	int x,y,px,py;
	int px1,py1,px2,py2;
	double	*Matary2;
	char	*MatAryC,*MatAryC2;
	short	*MatAryW,*MatAryW2;
	int		*MatAryI,*MatAryI2;
	int reg,reg2,i;
	int dimA,dimB;
	int mptr,mptr2;
	int value;
	int ElementSize;
	
	if ( RangeErrorCK(SRC) ) return;
	c =SRC[ExecPtr];
	if ( ( c != 0x7F ) || ( SRC[ExecPtr+1]!=0x40 ) ) { CB_Error(SyntaxERR); return; }	// Syntax error
	ExecPtr+=2;
	c=SRC[ExecPtr];
	if ( ( 'A' <= c ) && ( c <= 'Z' ) ) {
		reg=c-'A';
		if ( MatArySizeA[reg] == 0 ) { CB_Error(NoMatrixArrayERR); return; }	// No Matrix Array error
		ExecPtr++;
		c=SRC[ExecPtr];
		if ( c != '[' ) { CB_Error(SyntaxERR); return; }  // Syntax error
		ExecPtr++;
		if (CB_INT)	x=CBint_Eval( SRC ); else x=CB_Eval( SRC );
		if ( ( x < 1 ) || ( MatArySizeA[(reg)] < x ) ) CB_Error(RangeERR) ; // Range error 
		if ( SRC[ExecPtr] != ',' ) { CB_Error(SyntaxERR); return; }  // Syntax error
		ExecPtr++;
		if (CB_INT)	y=CBint_Eval( SRC ); else y=CB_Eval( SRC );
		if ( ( y < 1 ) || ( MatArySizeB[(reg)] < y ) ) CB_Error(RangeERR) ; // Range error 
		
		if ( SRC[ExecPtr] != ']' ) { CB_Error(SyntaxERR); return; }  // Syntax error
		ExecPtr++;
		if ( SRC[ExecPtr] != ',' ) { CB_Error(SyntaxERR); return; }  // Syntax error
		ExecPtr++;
		CB_DotOprand( SRC, &px1, &py1);

		if ( SRC[ExecPtr] != ',' ) { CB_Error(SyntaxERR); return; }  // Syntax error
		ExecPtr++;
		CB_DotOprand( SRC, &px2, &py2);
		
		if ( SRC[ExecPtr] != ')' ) { CB_Error(SyntaxERR); return; }  // Syntax error
		ExecPtr++;

		dimA=MatArySizeA[reg]-1; x--;
		dimB=MatArySizeB[reg]-1; y--;
		if (px1>px2) { i=px1; px1=px2; px2=i; }
		if (py1>py2) { i=py1; py1=py2; py2=i; }
		if ( dimA < px2-px1 ) px2=px1+dimA;
		if ( dimB < py2-py1 ) py2=py1+dimB;
			
		mptr=0;
		ElementSize=MatAryElementSize[reg];
		
		if ( SRC[ExecPtr] == 0x0E ) {  // -> Mat C
			ExecPtr++;
			c =SRC[ExecPtr];
			if ( ( c != 0x7F ) || ( SRC[ExecPtr+1]!=0x40 ) ) { CB_Error(SyntaxERR); return; }	// Syntax error
			ExecPtr+=2;
			c=SRC[ExecPtr];
			if ( ( 'A' <= c ) && ( c <= 'Z' ) ) {
				reg2=c-'A';
				if ( MatArySizeA[reg] == 0 ) { CB_Error(NoMatrixArrayERR); return; }	// No Matrix Array error
				ExecPtr++;
				if ( ElementSize != MatAryElementSize[reg2] ) { CB_Error(ArraySizeERR); return; }	// Illegal Array  size error
				
				dimA=MatArySizeA[reg2]-1;
				dimB=MatArySizeB[reg2]-1;
				if ( dimA+1 < px1 ) return;
				if ( dimB+1 < py1 ) return;
				if ( dimA+1 < px2 ) px2=dimA;
				if ( dimB+1 < py2 ) py2=dimB;
				if ( dimA < px2-px1 ) px2=px1+dimA;
				if ( dimB < py2-py1 ) py2=py1+dimB;

				switch ( ElementSize ) {
					case 1:
						MatAryC =(char*)MatAry[reg];
						MatAryC2=(char*)MatAry[reg2];
						for ( px=px1; px<=px2 ; px++) {
							mptr =MatArySizeB[reg]*x+y;		x++;
							mptr2=MatArySizeB[reg2]*px+py1;
							for ( py=py1; py<=py2 ; py++) {
								MatAryC2[mptr2++] = MatAryC[mptr++];
							}
						}
						break;
					case 2:
						MatAryW =(short*)MatAry[reg];
						MatAryW2=(short*)MatAry[reg2];
						for ( px=px1; px<=px2 ; px++) {
							mptr=MatArySizeB[reg]*x+y;		x++;
							mptr2=MatArySizeB[reg2]*px+py1;
							for ( py=py1; py<=py2 ; py++) {
								MatAryW2[mptr2++] = MatAryW[mptr++];
							}
						}
						break;
					case 4:
						MatAryI =(int*)MatAry[reg];
						MatAryI2=(int*)MatAry[reg2];
						for ( px=px1; px<=px2 ; px++) {
							mptr=MatArySizeB[reg]*x+y;		x++;
							mptr2=MatArySizeB[reg2]*px+py1;
							for ( py=py1; py<=py2 ; py++) {
								MatAryI2[mptr2++] = MatAryI[mptr++];
							}
						}
						break;
					case 8:
						for ( px=px1; px<=px2 ; px++) {
							mptr=MatArySizeB[reg]*x+y;		x++;
							mptr2=MatArySizeB[reg2]*px+py1;
							for ( py=py1; py<=py2 ; py++) {
								MatAry[mptr2++] = MatAry[mptr++];
							}
						}
						break;
				}
				dspflag=0;
			}	
		} else {	//			-> VRAM

			switch ( ElementSize ) {
				case 1:
					MatAryC=(char*)MatAry[reg];
					for ( px=px1; px<=px2 ; px++) {
						mptr=MatArySizeB[reg]*x+y; x++;
						for ( py=py1; py<=py2 ; py++) {
							if ( MatAryC[mptr++] ) 	Bdisp_SetPoint_VRAM(px, py, 1);
								else				Bdisp_SetPoint_VRAM(px, py, 0);
						}
					}
					break;
				case 2:
					MatAryW=(short*)MatAry[reg];
					for ( px=px1; px<=px2 ; px++) {
						mptr=MatArySizeB[reg]*x+y; x++;
						for ( py=py1; py<=py2 ; py++) {
							if ( MatAryW[mptr++] ) 	Bdisp_SetPoint_VRAM(px, py, 1);
								else				Bdisp_SetPoint_VRAM(px, py, 0);
						}
					}
					break;
				case 4:
					MatAryI=(int*)MatAry[reg];
					for ( px=px1; px<=px2 ; px++) {
						mptr=MatArySizeB[reg]*x+y; x++;
						for ( py=py1; py<=py2 ; py++) {
							if ( MatAryI[mptr++] ) 	Bdisp_SetPoint_VRAM(px, py, 1);
								else				Bdisp_SetPoint_VRAM(px, py, 0);
						}
					}
					break;
				case 8:
					for ( px=px1; px<=px2 ; px++) {
						mptr=MatArySizeB[reg]*x+y; x++;
						for ( py=py1; py<=py2 ; py++) {
							if ( MatAry[mptr++] ) 	Bdisp_SetPoint_VRAM(px, py, 1);
								else				Bdisp_SetPoint_VRAM(px, py, 0);
						}
					}
					break;
			}
//			ExecPtr++;
			Bdisp_PutDisp_DD_DrawBusy_skip_through(SRC);
			dspflag=0;
			UseGraphic=99;
		}
	}
}
void CB_DotG( unsigned char *SRC ){	// DotG(px1,py1, px2,py2)->Mat B [x,y]
	unsigned int c;
	int x,y,px,py;
	int px1,py1,px2,py2;
	char*	MatAryC;
	short*	MatAryW;
	int*	MatAryI;
	int reg,i;
	int dimA,dimB;
	int mptr;
	int value;
	int ElementSize;
	
	if ( RangeErrorCK(SRC) ) return;
	CB_DotOprand( SRC, &px1, &py1);
	if ( SRC[ExecPtr] != ',' ) { CB_Error(SyntaxERR); return; }  // Syntax error
	ExecPtr++;
	CB_DotOprand( SRC, &px2, &py2);
	if ( SRC[ExecPtr] != ')' ) { CB_Error(SyntaxERR); return; }  // Syntax error
	ExecPtr++;
	if ( SRC[ExecPtr] == 0x0E ) {  // -> Mat C
		ExecPtr++;
		c =SRC[ExecPtr];
		if ( ( c != 0x7F ) || ( SRC[ExecPtr+1]!=0x40 ) ) { CB_Error(SyntaxERR); return; }	// Syntax error
		ExecPtr+=2;
		c=SRC[ExecPtr];
		if ( ( 'A' <= c ) && ( c <= 'Z' ) ) {
			reg=c-'A';
			if ( MatArySizeA[reg] == 0 ) { CB_Error(NoMatrixArrayERR); return; }	// No Matrix Array error
			ExecPtr++;
			c=SRC[ExecPtr];
			if ( c != '[' ) { CB_Error(SyntaxERR); return; }  // Syntax error
			ExecPtr++;
			if (CB_INT)	x=CBint_Eval( SRC ); else x=CB_Eval( SRC );
			if ( ( x < 1 ) || ( MatArySizeA[(reg)] < x ) ) CB_Error(RangeERR) ; // Range error 
			if ( SRC[ExecPtr] != ',' ) { CB_Error(SyntaxERR); return; }  // Syntax error
			ExecPtr++;
			if (CB_INT)	y=CBint_Eval( SRC ); else y=CB_Eval( SRC );
			if ( ( y < 1 ) || ( MatArySizeB[(reg)] < y ) ) CB_Error(RangeERR) ; // Range error 
		
			if ( SRC[ExecPtr] != ']' ) { CB_Error(SyntaxERR); return; }  // Syntax error
			ExecPtr++;

			dimA=MatArySizeA[reg]-1; x--;
			dimB=MatArySizeB[reg]-1; y--;
			if ( dimA < x ) return;
			if ( dimB < y ) return;
			if (px1>px2) { i=px1; px1=px2; px2=i; }
			if (py1>py2) { i=py1; py1=py2; py2=i; }
			if ( dimA-x < px2-px1 ) px2=px1+dimA-x;
			if ( dimB-y < py2-py1 ) py2=py1+dimB-y;

			mptr=0;
			ElementSize=MatAryElementSize[reg];
			switch ( ElementSize ) {
				case 1:
					MatAryC=(char*)MatAry[reg];
					for ( px=px1; px<=px2 ; px++) {
						mptr=MatArySizeB[reg]*x+y; x++;
						for ( py=py1; py<=py2 ; py++) {
							MatAryC[mptr++]=Bdisp_GetPoint_VRAM(px,py);
						}
					}
					break;
				case 2:
					MatAryW=(short*)MatAry[reg];
					for ( px=px1; px<=px2 ; px++) {
						mptr=MatArySizeB[reg]*x+y; x++;
						for ( py=py1; py<=py2 ; py++) {
							MatAryW[mptr++]=Bdisp_GetPoint_VRAM(px,py);
						}
					}
					break;
				case 4:
					MatAryI=(int*)MatAry[reg];
					for ( px=px1; px<=px2 ; px++) {
						mptr=MatArySizeB[reg]*x+y; x++;
						for ( py=py1; py<=py2 ; py++) {
							MatAryI[mptr++]=Bdisp_GetPoint_VRAM(px,py);
						}
					}
					break;
				case 8:
					for ( px=px1; px<=px2 ; px++) {
						mptr=MatArySizeB[reg]*x+y; x++;
						for ( py=py1; py<=py2 ; py++) {
							MatAry[reg][mptr++]=Bdisp_GetPoint_VRAM(px,py);
						}
					}
					break;
			}
		}
	} else { CB_Error(SyntaxERR); return; }	// Syntax error
}

//----------------------------------------------------------------------------------------------
void CB_Circle( unsigned char *SRC ) { //	Circle
	unsigned int c;
	double x,y,r;
	int style=S_L_Style;
	if ( tmp_Style >= 0 ) style=tmp_Style;
	if ( RangeErrorCK(SRC) ) return;
	CB_SelectGraphVRAM();	// Select Graphic Screen
	if (CB_INT)	x=CBint_Eval( SRC );
	else		x=CB_Eval( SRC );
	c=SRC[ExecPtr];
	if ( c != ',' ) { CB_Error(SyntaxERR); return; }  // Syntax error
	ExecPtr++;
	if (CB_INT)	y=CBint_Eval( SRC );
	else		y=CB_Eval( SRC );
	c=SRC[ExecPtr];
	if ( c != ',' ) { CB_Error(SyntaxERR); return; }  // Syntax error
	ExecPtr++;
	if (CB_INT)	r=CBint_Eval( SRC );
	else		r=CB_Eval( SRC );
	c=SRC[ExecPtr];
	if ( c==':' ) 	Circle(x, y, r, style, 0);
		else 		Circle(x, y, r, style, 1);
	Bdisp_PutDisp_DD_DrawBusy_skip_through(SRC);
	tmp_Style = -1;
}

//----------------------------------------------------------------------------------------------
void GetGraphStr(  unsigned char *SRC ) {
	int ptr;
	int len,i;
	unsigned int c,d;
	ptr=ExecPtr;
	c=SRC[ExecPtr];
	d=SRC[ExecPtr+1];
	if ( ( c == 0x7F ) && ( d == 0xF0 ) ) { // GraphY
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
				{ CB_Error(ArgumentERR); return; }	// Argument error
				break;
		}
	} else {
		CB_Eval( SRC );
		len=(int)(ExecPtr-ptr);
		if ( len >= GraphStrMAX-1 ) { CB_Error(StringTooLongERR); return; }	// String too Long error
		GraphY=GraphY1;
		for (i=0; i<len; i++ ) GraphY[i]=SRC[ptr+i];
		GraphY[i]='\0';
	}
}

void CB_DrawGraph(  unsigned char *SRC ){
	if ( RangeErrorCK(SRC) ) return;
	CB_SelectGraphVRAM();	// Select Graphic Screen
	Graph_Draw();
}

void CB_GraphY( unsigned char *SRC ){
	if ( RangeErrorCK(SRC) ) return;
	GetGraphStr(SRC);
	CB_SelectGraphVRAM();	// Select Graphic Screen
	Graph_Draw();
}

//----------------------------------------------------------------------------------------------
void ReadVram( unsigned char *pDATA ){
	Bdisp_GetDisp_VRAM( pDATA ) ;
}
void WriteVram( unsigned char *pDATA ){
	DISPGRAPH Gpict;
	
	Gpict.x =   0; 
	Gpict.y =   0; 
	Gpict.GraphData.width   = 128;
	Gpict.GraphData.height  = 64;
	Gpict.GraphData.pBitmap = pDATA;
	Gpict.WriteModify = IMB_WRITEMODIFY_NORMAL; 
	Gpict.WriteKind   = IMB_WRITEKIND_OR;
	Bdisp_WriteGraph_VRAM(&Gpict);
}

int StoPict( int pictNo){
	unsigned char pict[2048+0x4C+4];
	int i,stat;
	ReadVram(pict+0x4C);
	for(i=1024+0x4c; i<2048+0x4c+2; i++) pict[i]=0;
	stat=SavePicture( pict, pictNo );
	if ( stat != 0 ) { CB_Error(MemoryERR); return; }	// Memory error
	return stat;
}

void RclPict( int pictNo){
	unsigned char *pict;
	int i;
	pict=LoadPicture( pictNo );
	if ( pict == NULL ) { CB_Error(MemoryERR); return; }	// Memory error
	WriteVram( pict+0x4C );
	free(pict);
}

void CB_StoPict( unsigned char *SRC ) { //	StoPict
	int n;
	CB_SelectGraphVRAM();	// Select Graphic Screen
	Bdisp_PutDisp_DD_DrawBusy();
	if (CB_INT)	n=CBint_Eval( SRC );
	else		n=CB_Eval( SRC );
	if ( (n<1) || (20<n) ){ CB_Error(ArgumentERR); return; }	// Argument error
	StoPict(n);
}
void CB_RclPict( unsigned char *SRC ) { //	RclPict
	int n;
	CB_SelectGraphVRAM();	// Select Graphic Screen
	if (CB_INT)	n=CBint_Eval( SRC );
	else		n=CB_Eval( SRC );
	if ( (n<1) || (20<n) ){ CB_Error(ArgumentERR); return; }	// Argument error
	RclPict(n);
	Bdisp_PutDisp_DD_DrawBusy();
}

//----------------------------------------------------------------------------------------------
int CB_SearchProg( unsigned char *name ) { //	Prog search
	int j,i=1;
	unsigned char *ptr;
	
	while ( ProgfileAdrs[i] ) {
		ptr=ProgfileAdrs[i]+0x3C;
		for (j=0;j<8;j++) if ( ptr[j] != name[j] ) break;
		if ( j==8 )	return i ; // ok
		i++;
	}
	return -1; // fault
}

void CB_Prog( unsigned char *SRC ) { //	Prog "..."
	unsigned int c;
	unsigned char buffer[32]="";
	unsigned char *src;
	unsigned char *StackProgSRC;
	int StackProgExecPtr;
	int stat;
	int ProgNo_bk;

	c=SRC[ExecPtr];
	if ( c == 0x22 ) {	// String
		ExecPtr++;
		GetQuotOpcode(SRC, buffer,16);	// Prog name
	}
	StackProgSRC     = SRC;
	StackProgExecPtr = ExecPtr;
	ProgNo_bk = ProgNo;
	
	ProgNo = CB_SearchProg( buffer );
	if ( ProgNo < 0 ) { ProgNo=ProgNo_bk; ErrorNo=GoERR; ErrorPtr=ExecPtr; return; }  // undefined Prog

	src = ProgfileAdrs[ProgNo];
	SRC = src + 0x56 ;
	ExecPtr=0;
	
	ProgEntryN++;
	
	stat=CB_interpreter_sub( SRC ) ;
	if ( stat ) {
		if ( ErrorNo ) return ;			// error
		else if ( BreakPtr > 0 ) return ;	// break
	}
	
	ProgEntryN--;
	SRC     = StackProgSRC ;
	ExecPtr = StackProgExecPtr ;
	ProgNo  = ProgNo_bk;
}

//----------------------------------------------------------------------------------------------
double CB_BinaryEval( unsigned char *SRC ) {	// eval 2
	unsigned int c,op;
	int reg,mptr,opPtr;
	double src,dst;
	char*	MatAryC;
	short*	MatAryW;
	int*	MatAryI;
	
	c=SRC[ExecPtr];
	if ( ( 'A' <= c ) && ( c <= 'z' ) ) {
		ExecPtr++;
		reg=c-'A';
		c=SRC[ExecPtr];
		if ( c=='%' ) {
			ExecPtr++;
			src = REGINT[reg] ;
		} else {
			if ( c=='#' ) ExecPtr++;
			src = REG[reg] ;
		}
	} else 
	if ( c==0x7F ) {
		c = SRC[ExecPtr+1] ; 
		if ( c == 0x40 ) {	// Mat A[a,b]
			ExecPtr+=2;
			mptr=MatOprand( SRC, &reg);
			if ( ErrorNo ) return ; // error
			switch ( MatAryElementSize[reg] ) {
				case 8:			// Matrix array doubl
					src    = MatAry[reg][mptr] ;
					break;
				case 1:			// Matrix array char
					MatAryC=(char*)MatAry[reg];
					src    = MatAryC[mptr] ;
					break;
				case 2:			// Matrix array word
					MatAryW=(short*)MatAry[reg];
					src    = MatAryW[mptr] ;
					break;
				case 4:			// Matrix array int
					MatAryI=(int*)MatAry[reg];
					src    = MatAryI[mptr] ;
					break;
			}
		}
	} else  src = CB_Eval1(SRC);

	opPtr=ExecPtr;
	op=SRC[ExecPtr++];	
	c=SRC[ExecPtr];
	if ( ( 'A' <= c ) && ( c <= 'z' ) ) {
		ExecPtr++;
		reg=c-'A';
		c=SRC[ExecPtr];
		if ( c=='%' ) {
			ExecPtr++;
			dst = REGINT[reg] ;
		} else {
			if ( c=='#' ) ExecPtr++;
			dst = REG[reg] ;
		}
	} else 
	if ( c==0x7F ) {
		c = SRC[ExecPtr+1] ; 
		if ( c == 0x40 ) {	// Mat A[a,b]
			ExecPtr+=2;
			mptr=MatOprand( SRC, &reg);
			if ( ErrorNo ) return ; // error
			switch ( MatAryElementSize[reg] ) {
				case 8:
					dst = MatAry[reg][mptr] ;		// Matrix array doubl
					break;
				case 1:
					MatAryC=(char*)MatAry[reg];
					dst = MatAryC[mptr] ;			// Matrix array char
					break;
				case 2:
					MatAryW=(short*)MatAry[reg];
					dst = MatAryW[mptr] ;			// Matrix array word
					break;
				case 4:
					MatAryI=(int*)MatAry[reg];
					dst = MatAryI[mptr] ;			// Matrix array int
					break;
			}
		}
	} else  dst = CB_Eval1(SRC);

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

double CB_UnaryEval( unsigned char *SRC ) {	// eval 1
	unsigned int c,op;
	int reg,mptr,opPtr;
	double src,dst;
	char*	MatAryC;
	short*	MatAryW;
	int*	MatAryI;
	
	c=SRC[ExecPtr];
	if ( ( 'A' <= c ) && ( c <= 'z' ) ) {
		ExecPtr++;
		reg=c-'A';
		c=SRC[ExecPtr];
		if ( c=='%' ) {
			ExecPtr++;
			return REGINT[reg] ;
		} else {
			if ( c=='#' ) ExecPtr++;
			return REG[reg] ;
		}
	} else 
	if ( c==0x7F ) {
		c = SRC[ExecPtr+1] ; 
		if ( c == 0x40 ) {	// Mat A[a,b]
			ExecPtr+=2;
			mptr=MatOprand( SRC, &reg);
			if ( ErrorNo ) return ; // error
			switch ( MatAryElementSize[reg] ) {
				case 8:
					return  MatAry[reg][mptr] ;		// Matrix array double
					break;
				case 1:
					MatAryC=(char*)MatAry[reg];
					return  MatAryC[mptr] ;			// Matrix array char
					break;
				case 2:
					MatAryW=(short*)MatAry[reg];
					return  MatAryW[mptr] ;			// Matrix array word
					break;
				case 4:
					MatAryI=(int*)MatAry[reg];
					return  MatAryI[mptr] ;			// Matrix array int
					break;
			}
		}
	} else  return CB_Eval1(SRC);
}
//----------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------
int dspflag=0;		// 0:nondsp  1:str  2:num

int CB_interpreter_sub( unsigned char *SRC ) {
	int cont=1;
	int i;
	unsigned int c;
	
	int StackGotoAdrs[StackGotoMax];

	int	CurrentStructCNT=0;
	int	CurrentStructloop[20];

	int StackForPtr=0;
	int StackForAdrs[StackForMax];
	int StackForVar[StackForMax];
	double StackForEnd[StackForMax];
	double StackForStep[StackForMax];
	int StackForEndint[StackForMax];
	int StackForStepint[StackForMax];

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
		if ( BreakPtr ) { if ( CB_BreakStop() ) return -1 ; }
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
				if (CB_INT) CBint_Dsz(SRC) ; else CB_Dsz(SRC) ;
				break;
			case 0xE9:	// Isz
				if (CB_INT) CBint_Isz(SRC) ; else CB_Isz(SRC) ;
				break;
				
			case 0xF7:	// F7
				c=SRC[ExecPtr++];
				switch ( c ) {
					case 0x00:	// If
						CB_If(SRC);
						break;
					case 0x02:	// Else
						Search_IfEnd(SRC);
						break;
					case 0x03:	// IfEnd
						break;
					case 0x04:	// For
						if (CB_INT)	CBint_For(SRC, &StackForPtr, StackForAdrs, StackForVar, StackForEndint, StackForStepint, &CurrentStructCNT, CurrentStructloop );
						else		CB_For(SRC, &StackForPtr, StackForAdrs, StackForVar, StackForEnd, StackForStep, &CurrentStructCNT, CurrentStructloop );
						break;
					case 0x07:	// Next
						if (CB_INT)	CBint_Next(SRC, &StackForPtr, StackForAdrs, StackForVar, StackForEndint, StackForStepint, &CurrentStructCNT, CurrentStructloop );
						else		CB_Next(SRC, &StackForPtr, StackForAdrs, StackForVar, StackForEnd, StackForStep, &CurrentStructCNT, CurrentStructloop );
						break;
					case 0x08:	// While
						CB_While(SRC, &StackWhilePtr, StackWhileAdrs, &CurrentStructCNT, CurrentStructloop );
						break;
					case 0x09:	// WhileEnd
						CB_WhileEnd(SRC, &StackWhilePtr, StackWhileAdrs, &CurrentStructCNT, CurrentStructloop );
						break;
					case 0x0A:	// Do
						CB_Do(SRC, &StackDoPtr, StackDoAdrs, &CurrentStructCNT, CurrentStructloop );
						break;
					case 0x0B:	// LpWhile
						CB_LpWhile(SRC, &StackDoPtr, StackDoAdrs, &CurrentStructCNT, CurrentStructloop );
						break;
					case 0x0C:	// Return
						if ( ProgEntryN ) return -1 ; //	in sub Prog
						cont=0;
						break;
					case 0x0D:	// Break
						CB_Break(SRC, &StackForPtr, &StackWhilePtr, &StackDoPtr, &CurrentStructCNT, CurrentStructloop );
						dspflag=0;
						break;
					case 0x0E:	// Stop
						BreakPtr=ExecPtr;
						cont=0; 
						break;
					case 0x10:	// Locate
						CB_Locate(SRC);
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
						CB_Vertical(SRC);
						dspflag=0;
						UseGraphic=99;
						break;
					case 0xA4:	// Horizontal
						CB_Horizontal(SRC);
						dspflag=0;
						UseGraphic=99;
						break;
					case 0xA5:	// Text
						dspflag=0;
						CB_Text(SRC, &dspflag);
						UseGraphic=99;
						break;
					case 0xA6:	// Circle
						CB_Circle(SRC);
						dspflag=0;
						UseGraphic=99;
						break;
					case 0xA7:	// F-Line
						CB_FLine(SRC);
						dspflag=0;
						UseGraphic=99;
						break;
					case 0xA8:	// PlotOn
						CB_PlotOn(SRC);
						dspflag=0;
						UseGraphic=99;
						break;
					case 0xA9:	// PlotOff
						CB_PlotOff(SRC);
						dspflag=0;
						UseGraphic=99;
						break;
					case 0xAA:	// PlotChg
						CB_PlotChg(SRC);
						dspflag=0;
						UseGraphic=99;
						break;
					case 0xAB:	// PxlOn
						if (CB_INT)	CBint_PxlOn(SRC); else CB_PxlOn(SRC);
						dspflag=0;
						UseGraphic=99;
						break;
					case 0xAC:	// PxlOff
						if (CB_INT)	CBint_PxlOff(SRC); else CB_PxlOff(SRC);
						dspflag=0;
						UseGraphic=99;
						break;
					case 0xAD:	// PxlChg
						if (CB_INT)	CBint_PxlChg(SRC); else CB_PxlChg(SRC);
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
						Axes=1;
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
						Axes=0;
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
					case 0x3F:	// DotG(
						CB_DotG(SRC);
						dspflag=0;
						break;
					default:
						ExecPtr-=2;
						dspflag=2;
						if (CB_INT)	CBint_CurrentValue = CBint_Eval( SRC );
						else		CB_CurrentValue    = CB_Eval( SRC );
				}
				break;
				
			case 0x7F:	// 7F
				c=SRC[ExecPtr++];
				switch ( c ) {
					case 0x40:	// Mat
						CB_MatCalc(SRC);
						break;
					default:
						ExecPtr-=2;
						dspflag=2;
						if (CB_INT)	CBint_CurrentValue = CBint_Eval( SRC );
						else		CB_CurrentValue    = CB_Eval( SRC );
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
						break;
					default:
						ExecPtr-=2;
						dspflag=2;
						if (CB_INT)	CBint_CurrentValue = CBint_Eval( SRC );
						else		CB_CurrentValue    = CB_Eval( SRC );
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
				CB_Input(SRC);
				CB_TicksStart=RTC_GetTicks();	// 
				dspflag=2;
				c=SRC[ExecPtr++];
				if ( c == 0x0E ) {		// ->
					if (CB_INT)	CBint_Store(SRC); else CB_Store(SRC);
				}
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
				CB_Plot(SRC);
				dspflag=0;
				UseGraphic=1;
				break;
			case 0xE1:	// Line
				CB_Line(SRC);
				dspflag=0;
				UseGraphic=99;
				break;
			case 0xED:	// Prog "..."
				CB_Prog(SRC);
				if ( BreakPtr ) return BreakPtr;
				dspflag=0;
				break;
				
//			case 0xDD:	// Eng
//				ENG=1-ENG;
//				dspflag=0;
//				break;
			case 0xD3:	// Rnd
				CB_Rnd();
				dspflag=2;
				break;
			case 0xD9:	// Norm
				CBint_CurrentValue = CB_Norm(SRC);
				CB_CurrentValue    = CBint_CurrentValue ;
				dspflag=2;
				break;
			case 0xE3:	// Fix
				CBint_CurrentValue = CB_Fix(SRC);
				CB_CurrentValue    = CBint_CurrentValue ;
				dspflag=2;
				break;
			case 0xE4:	// Sci
				CBint_CurrentValue = CB_Sci(SRC);
				CB_CurrentValue    = CBint_CurrentValue ;
				dspflag=2;
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
				dspflag=2;
				if (CB_INT)	CBint_CurrentValue = CBint_UnaryEval(SRC);
				else		CB_CurrentValue    = CB_UnaryEval(SRC);
				break;
			case 0xFC:	//  Q(  binaly Eval operation
				dspflag=2;
				if (CB_INT)	CBint_CurrentValue = CBint_BinaryEval(SRC);
				else		CB_CurrentValue    = CB_BinaryEval(SRC);
				break;
		
			default:
				dspflag=2;
				ExecPtr--;
				if (CB_INT)	CBint_CurrentValue = CBint_Eval( SRC );
				else		CB_CurrentValue    = CB_Eval( SRC );
				break;
		}
		c=SRC[ExecPtr];
		if ( c == 0x0E ) { 
			ExecPtr++;
			if (CB_INT)	CBint_Store(SRC);		// ->
			else		CB_Store(SRC);		// ->
			c=SRC[ExecPtr];
		}
		if ( c == 0x13 ) {					// =>
			ExecPtr++;
			dspflag=0;
			if (CB_INT)	{ if ( CBint_CurrentValue == 0 ) Skip_block(SRC); }		// false
			else 		{ if ( CB_CurrentValue    == 0 ) Skip_block(SRC); }		// false
		}
	}
	CB_TicksEnd=RTC_GetTicks();	// 
	CB_end(SRC, dspflag);
	return -1;
}
//----------------------------------------------------------------------------------------------
int CB_interpreter( unsigned char *SRC ) {
	int flag;
	unsigned int c;
	int stat;

	CB_INT=CB_INTDefault;
	CB_TicksStart=RTC_GetTicks();	// 
	srand( CB_TicksStart ) ;	// rand seed
	CB_ClrText(SRC);
	ProgEntryN=0;	// subroutin clear
	ErrorPtr=0;
	ErrorNo= 0;
	BreakPtr=0;
	Bdisp_PutDisp_DD_DrawBusy();
	KeyRecover(); 
	stat = CB_interpreter_sub( SRC );
	KeyRecover(); 
    if ( ErrorNo ) { CB_ErrMsg( ErrorNo ); }
	return stat;
}

//----------------------------------------------------------------------------------------------

double q_un_sub(void) {
     int i;
     double a[9];
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

double asctt4(){
	regK=1.0;
	lbl0:
	regK=regK+1.0;
	regA=regK/2.0*regK+4.0-regK;
	if ( regK<10000 ) goto lbl0;
	return regA;;
}


void CB_test() {
	int i, s, t, result;
	unsigned char	buffer[32];
	unsigned int key=0;

	Bdisp_AllClr_DDVRAM();
	locate(1,1); Print((unsigned char*)"8queen ");
	Bdisp_PutDisp_DD();
	
	start_timer();	// --------------------------------------

	regS=q_un_sub();

	t=get_timer();	// --------------------------------------

	locate(1,2);
	sprintG(buffer, regS, 21, RIGHT_ALIGN);
		Print(buffer);
	locate(1,3);sprintf((char*)buffer,"time=%5.3fs",(float)t/1000); Print(buffer);
	Bdisp_PutDisp_DD();

	
	locate(1,5); Print((unsigned char*)"Asciit4 ");
	Bdisp_PutDisp_DD();
	
	start_timer();	// --------------------------------------

	regS=asctt4();

	t=get_timer();	// --------------------------------------

	locate(1,6);
	sprintG(buffer, regS, 21, RIGHT_ALIGN);
		Print(buffer);
	locate(1,7);sprintf((char*)buffer,"time=%5.3fs",(float)t/1000); Print(buffer);

	Bdisp_PutDisp_DD();
	GetKey(&key);
}


