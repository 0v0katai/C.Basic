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
#include "CB_inp.h"
#include "CB_glib.h"
#include "CB_glib2.h"
#include "CB_Eval.h"
#include "CB_file.h"
#include "CB_edit.h"
#include "CB_setup.h"
#include "CB_Time.h"
#include "CB_Matrix.h"
#include "CB_Str.h"

#include "CB_interpreter.h"
#include "CBI_interpreter.h"
#include "CB_error.h"
#include "fx_syscall.h"

//-----------------------------------------------------------------------------
// Casio Basic inside
//-----------------------------------------------------------------------------
char CB_INTDefault=0;	// default mode  0:normal  1: integer mode

char	DrawType    = 0;	// 0:connect  1:Plot
char	Coord       = 1;	// 0:off 1:on
char	Grid        = 1;	// 0:off 1:on
char	Axes        = 1;	// 0:off 1:on
char	Label       = 1;	// 0:off 1:on
char	Derivative  = 1;	// 0:off 1:on
char	S_L_Style   = S_L_Normal;
char	tmp_Style   = S_L_Normal;
char	Angle       = 1;	// 0:deg   1:rad  2:grad

double Previous_X=1e308 ;	// Plot Previous X
double Previous_Y=1e308 ;	// Plot Previous Y
int    Previous_PX=-1   ;	// Plot Previous PX
int    Previous_PY=-1   ;	// Plot Previous PY
double Plot_X    =1e308 ;	// Plot Current X
double Plot_Y    =1e308 ;	// Plot Current Y

char PxlMode=1;		// Pxl  1:set  0:clear

char BreakCheck=1;	// Break Stop on/off
char ACBreak=1;		// AC Break on/off

char TimeDsp=0;		// Execution Time Display  0:off 1:on
char MatXYmode=0;		// 0: normal  1:reverse
char PictMode=0;	// StoPict/RclPict  StrageMem:0  heap:1
char CheckIfEnd=1;	// If...IfEnd check  1:off  0:on
//-----------------------------------------------------------------------------
// Casio Basic Gloval variable
//-----------------------------------------------------------------------------
double  REG[26];
double  REGsmall[26];
double  REGv[11];
int  	REGINT[26];
int  	REGINTsmall[26];


double	Xfct     =  2;					// 
double	Yfct     =  2;					// 

short 	Argc=0;
double 	LocalDbltmp[ArgcMAX];
int		LocalInttmp[ArgcMAX];

double 	*LocalDbl[26];
int		*LocalInt[26];

double	traceAry[130];		// Graph array X

char *GraphY;
//char GraphY1[GraphStrMAX];
//char GraphY2[GraphStrMAX];
//char GraphY3[GraphStrMAX];

unsigned char *PictAry[PictMax+1];		// Pict array ptr
//----------------------------------------------------------------------------------------------
//		Interpreter inside
//----------------------------------------------------------------------------------------------
int	CB_TicksStart;
int	CB_TicksEnd;
int	CB_TicksAdjust;

int CB_INT=0;		// current mode  0:normal  1: integer mode
int ExecPtr=0;		// Basic execute ptr
int BreakPtr=0;		// Basic break ptr

double CB_CurrentValue=0;	// Ans
int CBint_CurrentValue=0;	// Ans

int ScreenMode;	//  0:Text  1:Graphic
int UseGraphic=0;	// use Graph  ( no use :0    plot:1   graph:2   cls:3   other:99
int dspflag=0;		// 0:nondsp  1:str  2:num

int CursorX=1;	// text cursor X
int CursorY=1;	// text cursor X

char GosubNestN=0;		// Gosub Nesting lebel (for subroutin)
char ProgEntryN=0;		// Basic Program ptr (for subroutin)
char ProgNo=0;			// current Prog No
char *ProgfileAdrs[ProgMax+1];
int   ProgfileMax[ProgMax+1] ;	// Max edit filesize 
char  ProgfileEdit[ProgMax+1];	// no change : 0     edited : 1
char  ProgLocalN[ProgMax+1];
char  ProgLocalVar[ProgMax+1][26];

//----------------------------------------------------------------------------------------------
short StackGotoAdrs[StackGotoMax];

CchIf	CacheIf;
CchIf	CacheElse;
CchIf	CacheElseIf;
CchIf	CacheSwitch;
CchRem	CacheRem;

void ClrCahche();
//----------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------
//int ObjectAlign4( unsigned int n ){ return n; }	// align +4byte
//int ObjectAlign6a( unsigned int n ){ return n+n; }	// align +6byte
//int ObjectAlign4b( unsigned int n ){ return n; }	// align +4byte
//int ObjectAlign4c( unsigned int n ){ return n; }	// align +4byte
//----------------------------------------------------------------------------------------------

int CB_interpreter_sub( char *SRC ) {
	char cont=1;
	char dspflagtmp=0;
	int c,j;

	short StackGosubAdrs[StackGosubMax];
	
	CurrentStk	CurrentStruct;
	StkFor		StackFor;
	StkWhileDo	StackWhileDo;
	StkSwitch	StackSwitch;
	
	double	localvarDbl[ArgcMAX];	//	local var
	int		localvarInt[ArgcMAX];	//	local var

	if ( 0x88020200 > (int)&cont ) { CB_Error(StackERR); return -1; } //  stack error
	
	for (c=0; c<StackGotoMax; c++) StackGotoAdrs[c]=0;
	
	ClrCahche();
	
	StackFor.Ptr=0;
	StackWhileDo.WhilePtr=0;
	StackWhileDo.DoPtr=0;
	StackSwitch.Ptr=0;
	CurrentStruct.CNT=0;
	
	tmp_Style = -1;
	dspflag=0;

	if ( ProgNo ) {			// set local variable
		for ( c=0; c<26; c++ ) {
			LocalDbl[c]=&REGsmall[c];
			LocalInt[c]=&REGINTsmall[c];
		}
		for ( c=0; c<ProgLocalN[ProgNo]; c++ ) {
			j=ProgLocalVar[ProgNo][c];
			if ( j>=0 ) { 
				LocalDbl[j]=&localvarDbl[c]; LocalDbl[j][0]=LocalDbltmp[c];
				LocalInt[j]=&localvarInt[c]; LocalInt[j][0]=LocalInttmp[c];
			}
		}
	}

	
	while (cont) {
		dspflagtmp=0;
		CB_StrBufferCNT=0;			// Quot String buffer clear
		if ( ErrorNo || BreakPtr ) { 
			if ( CB_BreakStop() ) return -7 ;
			if ( SRC[ExecPtr] == 0x0C ) ExecPtr++; // disps
			ClrCahche();
		}
		c=SRC[ExecPtr++];
		if ( c==':'  ) { c=SRC[ExecPtr++]; }
		if ( c==0x0D ) {
				while ( c==0x0D ) c=SRC[ExecPtr++];
				if (BreakCheck) if ( KeyScanDown(KEYSC_AC) ) { KeyRecover(); if ( BreakPtr == 0 ) BreakPtr=ExecPtr-1; }	// [AC] break?
		}
		if ( c==0x00 ) { ExecPtr--;
			if ( ProgEntryN ) return -1;
			else  break;
		}
		
		while ( c==0x20 ) c=SRC[ExecPtr++];
		
		switch (c) {
			case 0xFFFFFFEC:	// Goto
				CB_Goto(SRC, StackGotoAdrs );
				break;
			case 0xFFFFFFE8:	// Dsz
				if (CB_INT) CBint_Dsz(SRC) ; else CB_Dsz(SRC) ;
				break;
			case 0xFFFFFFE9:	// Isz
				if (CB_INT) CBint_Isz(SRC) ; else CB_Isz(SRC) ;
				break;
			case 0xFFFFFFE2:	// Lbl
				CB_Lbl(SRC, StackGotoAdrs );
				break;
				
			case 0xFFFFFFF7:	// F7
				c=SRC[ExecPtr++];
				switch ( c ) {
					case 0x00:	// If
						CB_If(SRC, &CacheIf );
						break;
					case 0x02:	// Else
						CB_Else(SRC, &CacheElse );
						break;
					case 0x0F:	// ElseIf
						CB_Else(SRC, &CacheElseIf );
						break;
					case 0x03:	// IfEnd
						break;
					case 0x04:	// For
						if (CB_INT)	CBint_For(SRC, &StackFor, &CurrentStruct );
						else		CB_For(SRC, &StackFor, &CurrentStruct );
//						ClrCahche();
						break;
					case 0x07:	// Next
						if (CB_INT)	CBint_Next(SRC, &StackFor, &CurrentStruct );
						else		CB_Next(SRC, &StackFor, &CurrentStruct );
						break;
					case 0x08:	// While
						CB_While(SRC, &StackWhileDo, &CurrentStruct );
//						ClrCahche();
						break;
					case 0x09:	// WhileEnd
						CB_WhileEnd(SRC, &StackWhileDo, &CurrentStruct );
						break;
					case 0x0A:	// Do
						CB_Do(SRC, &StackWhileDo, &CurrentStruct );
//						ClrCahche();
						break;
					case 0x0B:	// LpWhile
						CB_LpWhile(SRC, &StackWhileDo, &CurrentStruct );
						break;
					case 0xFFFFFFEA:	// Switch
						CB_Switch(SRC, &StackSwitch, &CurrentStruct, &CacheSwitch );
//						CB_Switch(SRC, &StackSwitch, &CurrentStruct );
						break;
					case 0xFFFFFFEB:	// Case
						CB_Case(SRC, &StackSwitch, &CurrentStruct );
						break;
					case 0xFFFFFFEC:	// Default
						CB_Default(SRC, &StackSwitch, &CurrentStruct );
						break;
					case 0xFFFFFFED:	// SwitchEnd
						CB_SwitchEnd(SRC, &StackSwitch, &CurrentStruct );
						break;
					case 0x0D:	// Break
						CB_Break(SRC, &StackFor, &StackWhileDo, &StackSwitch, &CurrentStruct );
						dspflag=0;
						break;
					case 0x0C:	// Return
						if ( GosubNestN > 0 ) { ExecPtr=StackGosubAdrs[--GosubNestN] ; break; } //	 return form subroutin 
						if ( ProgEntryN ) { return -2 ; }	//	return from  sub Prog
						cont=0;
						break;
					case 0x10:	// Locate
						CB_Locate(SRC);
						dspflag=1;
						break;
					case 0xFFFFFFA5:	// Text
						dspflag=0;
						CB_Text(SRC);
						UseGraphic=99;
						break;
					case 0xFFFFFFAB:	// PxlOn
						if (CB_INT)	CBint_PxlOn(SRC); else CB_PxlOn(SRC);
						dspflag=0;
						UseGraphic=99;
						break;
					case 0xFFFFFFAC:	// PxlOff
						if (CB_INT)	CBint_PxlOff(SRC); else CB_PxlOff(SRC);
						dspflag=0;
						UseGraphic=99;
						break;
					case 0xFFFFFFAD:	// PxlChg
						if (CB_INT)	CBint_PxlChg(SRC); else CB_PxlChg(SRC);
						dspflag=0;
						UseGraphic=99;
						break;
					case 0xFFFFFFA8:	// PlotOn
						CB_PlotOn(SRC);
						dspflag=0;
						UseGraphic=99;
						break;
					case 0xFFFFFFA9:	// PlotOff
						CB_PlotOff(SRC);
						dspflag=0;
						UseGraphic=99;
						break;
					case 0xFFFFFFAA:	// PlotChg
						CB_PlotChg(SRC);
						dspflag=0;
						UseGraphic=99;
						break;
					case 0xFFFFFFA7:	// F-Line
						CB_FLine(SRC);
						dspflag=0;
						UseGraphic=99;
						break;
					case 0xFFFFFFA3:	// Vertical
						CB_Vertical(SRC);
						dspflag=0;
						UseGraphic=99;
						break;
					case 0xFFFFFFA4:	// Horizontal
						CB_Horizontal(SRC);
						dspflag=0;
						UseGraphic=99;
						break;
					case 0xFFFFFFA6:	// Circle
						CB_Circle(SRC);
						dspflag=0;
						UseGraphic=99;
						break;
					case 0x1C:			// S-L-Normal
						S_L_Style = S_L_Normal;
						dspflag=0;
						break;
					case 0x1D:			// S-L-Thick
						S_L_Style = S_L_Thick;
						dspflag=0;
						break;
					case 0x1E:			// S-L-Broken
						S_L_Style = S_L_Broken;
						dspflag=0;
						break;
					case 0x1F:			// S-L-Dot
						S_L_Style = S_L_Dot;
						dspflag=0;
						break;
					case 0xFFFFFF8C:	// SketchNormal
						tmp_Style = S_L_Normal;
						dspflag=0;
						break;
					case 0xFFFFFF8D:	// SketchThick
						tmp_Style = S_L_Thick;
						dspflag=0;
						break;
					case 0xFFFFFF8E:	// SketchBroken
						tmp_Style = S_L_Broken;
						dspflag=0;
						break;
					case 0xFFFFFF8F:	// SketchDot
						tmp_Style = S_L_Dot;
						dspflag=0;
						break;
					case 0x18:			// ClrText
						CB_ClrText(SRC);
						dspflag=0;
						break;
					case 0x19:			// ClrGraph
						CB_ClrGraph(SRC);
						dspflag=0;
						UseGraphic=0;
						break;
					case 0x7A:			// GridOff
						Grid=0;
						dspflag=0;
						break;
					case 0x7D:			// GridOn
						Grid=1;
						dspflag=0;
						break;
					case 0xFFFFFFC2:	// AxesOn
						Axes=1;
						dspflag=0;
						break;
					case 0xFFFFFFC3:	// CoordOn
						Coord=1;
						dspflag=0;
						break;
					case 0xFFFFFFC4:	// LabelOn
						Label=1;
						dspflag=0;
						break;
					case 0xFFFFFFD2:	// AxesOff
						Axes=0;
						dspflag=0;
						break;
					case 0xFFFFFFD3:	// CoordOff
						Coord=0;
						dspflag=0;
						break;
					case 0xFFFFFFD4:	// LabelOff
						Label=0;
						dspflag=0;
						break;
					case 0x20:			// DrawGraph
						CB_DrawGraph(SRC);
						dspflag=0;
						UseGraphic=2;
						break;
					case 0x3F:			// DotGet(
						CB_DotGet(SRC);
						dspflag=0;
						break;
					case 0x4F:			// DotTirm(
						CB_DotTrim(SRC);
						dspflag=0;
						break;
					case 0xFFFFFFE0:	// DotLife(
						CB_DotLife(SRC);
						dspflag=0;
						break;
					case 0xFFFFFFE1:	// Rect
						CB_Rect(SRC);
						dspflag=0;
						UseGraphic=99;
						break;
					case 0xFFFFFFE2:	// FillRect
						CB_FillRect(SRC);
						dspflag=0;
						UseGraphic=99;
						break;
					case 0xFFFFFFE3:	// LocateYX
						CB_LocateYX(SRC);
						dspflag=0;
						UseGraphic=99;
						break;
					case 0xFFFFFFE9:	// WriteGraph
						CB_WriteGraph(SRC);
						dspflag=0;
						UseGraphic=99;
						break;
					case 0xFFFFFFE8:	// ReadGraph
						CB_ReadGraph(SRC);
						dspflag=0;
						UseGraphic=99;
						break;
					case 0xFFFFFFF0:	// DotShape(
						CB_DotShape(SRC);
						dspflag=0;
						UseGraphic=99;
						break;
					case 0xFFFFFF94:	// RclPict
						CB_RclPict(SRC);
						dspflag=0;
						UseGraphic=99;
						break;
					case 0xFFFFFF93:	// StoPict
						CB_StoPict(SRC);
						dspflag=0;
						UseGraphic=99;
						break;
					case 0xFFFFFFEE:	// Save
						CB_Save(SRC);
						dspflag=0;
						break;
					case 0xFFFFFFEF:	// Load(
						CB_Load(SRC);
						dspflag=0;
						break;
					case 0xFFFFFFF1:	// Local
						Skip_block(SRC);
						dspflag=0;
						break;
					case 0xFFFFFFFE:	// Local
						BackLight( CB_EvalInt( SRC ) );
						dspflag=0;
						break;
					case 0x01:	// Then
						ExecPtr-=2;
						CB_Error(ThenWithoutIfERR); // not Then error 
						break;
					case 0x17:	// ACBreak
						if ( ( SRC[ExecPtr]==0xFFFFFFF7 ) && ( SRC[ExecPtr+1]==0x0E ) ) {	// ACBreak Stop;
							ExecPtr+=2;
							ACBreak=0;
							break;
						}
						if ( ACBreak ) {
							BreakPtr=ExecPtr; 
//							DebugMode=2;	// enable debug mode
						}
						break;
					case 0x0E:	// Stop
						cont=0; 
						break;
					default:
						ExecPtr-=2;
						dspflagtmp=2;
						if (CB_INT)	CBint_CurrentValue = EvalIntsubTop( SRC );
						else		CB_CurrentValue    = EvalsubTop( SRC );
				}
				break;
				
			case 0x7F:	// 7F
				c=SRC[ExecPtr++];
				switch ( c ) {
					case 0x40:	// Mat
						dspflagtmp=CB_MatCalc(SRC);
						dspflag=0;
						break;
					case 0x41:	// Trn
						CB_MatTrn(SRC);
						dspflag=0;
						break;
					case 0x47:	// Fill(
						CB_MatFill(SRC);
						dspflag=0;
						break;
//					case 0x51:	// List
//						dspflagtmp=CB_ListCalc(SRC);
//						dspflag=0;
//						break;
					default:
						ExecPtr-=2;
						dspflagtmp=2;
						if (CB_INT)	CBint_CurrentValue = EvalIntsubTop( SRC );
						else		CB_CurrentValue    = EvalsubTop( SRC );
				}
				break;
				
			case 0xFFFFFFF9:	// F9
				c=SRC[ExecPtr++];
				switch ( c ) {
					case 0x30:	// StrJoin(
					case 0x34:	// StrLeft(
					case 0x35:	// StrRight(
					case 0x36:	// StrMid(
					case 0x37:	// Exp->Str(
					case 0x39:	// StrUpr(
					case 0x3A:	// StrLwr(
					case 0x3B:	// StrInv(
					case 0x3C:	// StrShift(
					case 0x3D:	// StrRotate(
					case 0x3E:	// ToStr(
					case 0x3F:	// Str
						ExecPtr-=2;
						CB_Str(SRC) ;
						dspflag=0;
						break;
					case 0x4B:	// DotPut(Z,x,y)
						CB_DotPut(SRC);
						break;
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
					default:
						ExecPtr-=2;
						dspflagtmp=2;
						if (CB_INT)	CBint_CurrentValue = EvalIntsubTop( SRC );
						else		CB_CurrentValue    = EvalsubTop( SRC );
				}
				break;
				
			case 0x27:	// ' rem
				if ( SRC[ExecPtr] == '/' ) { ExecPtr++; break; }	// '/ execute only C.Basic 
				CB_Rem(SRC, &CacheRem );
				dspflag=0;
				break;
			case 0xFFFFFFED:	// Prog "..."
				CB_Prog(SRC, localvarInt, localvarDbl );
				ClrCahche();
				goto jpgsb;
				break;
			case 0xFFFFFFFA:	// Gosub
				CB_Gosub(SRC, StackGotoAdrs, StackGosubAdrs );
		jpgsb:	if ( BreakPtr > 0 ) return BreakPtr;
				dspflag=0;
				break;
			case 0xFFFFFFD1:	// Cls
				CB_Cls(SRC);
				dspflag=0;
				UseGraphic=0;
				break;
			case 0xFFFFFFE0:	// Plot
				CB_Plot(SRC);
				dspflag=0;
				UseGraphic=1;
				break;
			case 0xFFFFFFE1:	// Line
				CB_Line(SRC);
				dspflag=0;
				UseGraphic=99;
				break;
			case 0xFFFFFFDA:	// Deg
				Angle = 0;
				dspflag=0;
				break;
			case 0xFFFFFFDB:	// Rad
				Angle = 1;
				dspflag=0;
				break;
			case 0xFFFFFFDC:	// Grad
				Angle = 2;
				dspflag=0;
				break;
				
//			case 0xDD:	// Eng
//				ENG=1-ENG;
//				dspflag=0;
//				break;
			case 0xFFFFFFD3:	// Rnd
				CB_Rnd();
				dspflagtmp=2;
				break;
			case 0xFFFFFFD9:	// Norm
				CBint_CurrentValue = CB_Norm(SRC);
				CB_CurrentValue    = CBint_CurrentValue ;
				dspflagtmp=2;
				break;
			case 0xFFFFFFE3:	// Fix
				CBint_CurrentValue = CB_Fix(SRC);
				CB_CurrentValue    = CBint_CurrentValue ;
				dspflagtmp=2;
				break;
			case 0xFFFFFFE4:	// Sci
				CBint_CurrentValue = CB_Sci(SRC);
				CB_CurrentValue    = CBint_CurrentValue ;
				dspflagtmp=2;
				break;
			case 0xFFFFFFEB:	// ViewWindow
				CB_ViewWindow(SRC);
				dspflag=0;
				UseGraphic=0;
				break;
			case 0xFFFFFFEE:	// Graph Y=
				CB_GraphY(SRC);
				dspflag=0;
				UseGraphic=2;
				break;
				
			case '$':	// $Mat
			case '&':	// &Mat
			case 0x22:	// " "
				ExecPtr--;
				CB_Str(SRC) ;
				dspflag=1;
				break;
			case 0x3F:	// ?
				CB_Input(SRC);
				CB_TicksStart=RTC_GetTicks();	// 
				dspflagtmp=2;
				if ( BreakPtr ) break;
				c=SRC[ExecPtr++];
				if ( c == 0x0E ) {		// ->
					if (CB_INT)	CBint_Store(SRC); else CB_Store(SRC);
				}
				else ExecPtr--;
				break;
			case 0x7B:	// { m.n }->Dim Mat A
				CB_MatrixInit(SRC);
				dspflag=0;
				break;
			case '[':	// [ [0,1,2][2,3,4] ]->Mat A
				CB_MatrixInit2(SRC);
				dspflag=0;
				break;
			case 0x0C:	// disps
				if ( CB_Disps(SRC,dspflag) ) BreakPtr=ExecPtr ;  // [AC] break
				CB_TicksStart=RTC_GetTicks();	// 
				c=SRC[ExecPtr]; while ( c==0x20 ) c=SRC[++ExecPtr]; // Skip Space
				break;
		
			default:
				ExecPtr--;
				dspflagtmp=2;
				if (CB_INT)	CBint_CurrentValue = EvalIntsubTop( SRC );
				else		CB_CurrentValue    = EvalsubTop( SRC );
				break;
		}
		c=SRC[ExecPtr];
		if ( c == 0x0E ) { 
			ExecPtr++;
			if (CB_INT)	CBint_Store(SRC);	// ->
			else		CB_Store(SRC);		// ->
			c=SRC[ExecPtr];
		}
		if ( c == 0x13 ) {					// =>
			ExecPtr++;
			dspflagtmp=0;
			if (CB_INT)	{ if ( CBint_CurrentValue == 0 ) Skip_block(SRC); }		// false
			else 		{ if ( CB_CurrentValue    == 0 ) Skip_block(SRC); }		// false
		}
		if ( dspflagtmp ) dspflag=dspflagtmp;
	}
	CB_TicksEnd=RTC_GetTicks();	// 
	CB_end(SRC);
	return -1;
}
//----------------------------------------------------------------------------------------------
void ClrCahche(){
	CacheIf.CNT=0;
	CacheElse.CNT=0;
	CacheElseIf.CNT=0;
	CacheRem.CNT=0;
	CacheSwitch.CNT=0;
}

int CB_interpreter( char *SRC ) {
	int flag;
	int c;
	int stat;

	CB_INT = CB_INTDefault;
	MatBase = MatBaseDefault;
	CB_TicksStart = RTC_GetTicks();	// 
	CB_TicksAdjust = 0 ;	// 
	srand( CB_TicksStart ) ;	// rand seed
	ScreenMode = 0;	// Text mode
	PxlMode = 1;		// Pxl  1:set  0:clear	
	CB_ClrText(SRC);
	ProgEntryN = 0;	// subroutin clear
	GosubNestN = 0;	// Gosub clear
	ErrorPtr = 0;
	ErrorNo = 0;
//	BreakPtr = 0;
//	defaultStrAry = 's'-'A';
//	defaultStrAryN = 20;
//	defaultStrArySize = 64+1;
//	defaultGraphAry = 'y'-'A';
//	defaultGraphAryN = 5;
//	defaultGraphArySize = 64+1;
	Bdisp_PutDisp_DD_DrawBusy();
	KeyRecover(); 
	Argc = 0;	// 
	stat = CB_interpreter_sub( SRC );
	KeyRecover(); 
//	if ( ErrorNo ) { CB_ErrMsg( ErrorNo ); }
	return stat;
}

//----------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------
int ObjectAlign4d( unsigned int n ){ return n; }	// align +4byte
//int ObjectAlign4e( unsigned int n ){ return n; }	// align +4byte
//int ObjectAlign4f( unsigned int n ){ return n; }	// align +4byte
//----------------------------------------------------------------------------------------------

void Skip_quot( char *SRC ){ // skip "..."
	int c;
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
			case 0xFFFFFFF7:	// 
			case 0xFFFFFFF9:	// 
			case 0xFFFFFFE5:	// 
			case 0xFFFFFFE6:	// 
			case 0xFFFFFFE7:	// 
//			case 0xFFFFFFFF:	// 
				ExecPtr++;
				break;
		}
	}
}
void Skip_block( char *SRC ){
	int c;
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
			case 0xFFFFFFF7:	// 
			case 0xFFFFFFF9:	// 
			case 0xFFFFFFE5:	// 
			case 0xFFFFFFE6:	// 
			case 0xFFFFFFE7:	// 
//			case 0xFFFFFFFF:	// 
				ExecPtr++;
				break;
		}
	}
}

//----------------------------------------------------------------------------------------------
void Skip_rem( char *SRC ){	// skip '...
	int c;
remloop:
	c=SRC[ExecPtr++];
	if ( c=='/' ) { 	// '/ execute C.Basic only
		return;
	} else 
	if ( c=='#' ) {
		c=SRC[ExecPtr++];
		if ( c=='C') {
			c=SRC[ExecPtr++];
			if ( c=='B' ) {
				c=SRC[ExecPtr++];
				if ( ( c=='i' ) || ( c=='I' ) ) CB_INT=1;
				else
				if ( ( c=='d' ) || ( c=='D' ) || ( c=='a' ) || ( c=='A' ) ) CB_INT=0;
			}
		} else
		if ( c==0xFFFFFFF9 ) {
			c=SRC[ExecPtr++];
			if ( c==0x3F ) {	// Str
				c=SRC[ExecPtr++];
				if ( ( 'A' <= c ) && ( c <= 'z' ) ) { 
					defaultStrAry= c-'A';
					if ( SRC[ExecPtr] == ',') {
						c=SRC[++ExecPtr];
						defaultStrAryN=Eval_atod( SRC, c );
						if ( SRC[ExecPtr] == ',') {
							c=SRC[++ExecPtr];
							defaultStrArySize=Eval_atod( SRC, c );
						}
					}
				}
			} else { ExecPtr++;  c=SRC[ExecPtr++]; }
		} else
		if ( c==0x7F ) {
			c=SRC[ExecPtr++];
			if ( c==0xFFFFFFF0 ) {	// Graph
				c=SRC[ExecPtr++];
				if ( ( 'A' <= c ) && ( c <= 'z' ) ) { 
					defaultGraphAry= c-'A';
					if ( SRC[ExecPtr] == ',') {
						c=SRC[++ExecPtr];
						defaultGraphAryN=Eval_atod( SRC, c );
						if ( SRC[ExecPtr] == ',') {
							c=SRC[++ExecPtr];
							defaultGraphArySize=Eval_atod( SRC, c );
						}
					}
				}
			} else
			if ( c==0x40 ) {	// Mat 0  : base 0
				c=SRC[ExecPtr++];
				if ( ( c=='0' ) || ( c=='1' ) ) { 
					MatBase= c-'0' ;
				}
			} else { ExecPtr++;  c=SRC[ExecPtr++]; }

		}
	}
	while (1){
		switch ( c ) {
			case 0x00:	// <EOF>
				ExecPtr--;
//			case 0x3A:	// <:>
			case 0x0C:	// dsps
				return ;
			case 0x0D:	// <CR>
				if ( SRC[ExecPtr] == 0x27 ) { ExecPtr++; goto remloop; }
				return;
			case 0x22:	// "
				Skip_quot(SRC);
				break;
			case 0x7F:	// 
			case 0xFFFFFFF7:	// 
			case 0xFFFFFFF9:	// 
			case 0xFFFFFFE5:	// 
			case 0xFFFFFFE6:	// 
			case 0xFFFFFFE7:	// 
//			case 0xFFFFFFFF:	// 
				ExecPtr++;
				break;
		}
		c=SRC[ExecPtr++];
	}
}

void CB_Rem( char *SRC, CchRem *CacheRem ){
	int i;

	for ( i=0; i<CacheRem->CNT; i++ ) {
		if ( CacheRem->Ptr[i]==ExecPtr ) { ExecPtr=CacheRem->Adrs[i]; return ; }	// adrs ok
	}
	CacheRem->Ptr[CacheRem->CNT]=ExecPtr;
	Skip_rem( SRC );
	if ( CacheRem->CNT < RemCntMax ) {
		CacheRem->Adrs[CacheRem->CNT]=ExecPtr;
		CacheRem->CNT++;
	} else CacheRem->CNT=0;	// over reset
}
//-----------------------------------------------------------------------------

void CB_Lbl( char *SRC, short *StackGotoAdrs ){
	int c;
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

int Search_Lbl( char *SRC, int lc ){
	int c;
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
				Skip_rem(SRC);
				break;
			case 0xFFFFFFE2:	// Lbl
				c=SRC[ExecPtr++];
				if ( c == lc ) return 1 ;
				break;
			case 0x7F:	// 
			case 0xFFFFFFF7:	// 
			case 0xFFFFFFF9:	// 
			case 0xFFFFFFE5:	// 
			case 0xFFFFFFE6:	// 
			case 0xFFFFFFE7:	// 
//			case 0xFFFFFFFF:	// 
				ExecPtr++;
				break;
		}
	}
	ExecPtr=bptr;
	return 0;
}

void CB_Goto( char *SRC, short *StackGotoAdrs){
	int c;
	int label;
	int ptr;
	c=SRC[ExecPtr];
	if ( ( '0' <= c ) && ( c <= '9' ) ) {
		ExecPtr++;
		label = c-'0';
	} else if ( ( 'A' <= c ) && ( c <= 'Z' ) ) {
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
void Search_IfEnd( char *SRC ){
	int c;
	while (1){
		c=SRC[ExecPtr++];
		switch ( c ) {
			case 0x00:	// <EOF>
				ExecPtr--;
				return  ;
			case 0x27:	// ' rem
				Skip_rem(SRC);
				break;
			case 0xFFFFFFF7:	// 
				c=SRC[ExecPtr++];
				if ( c == 0x00 ) { 			// If
					Search_IfEnd(SRC);
					break;
				} else
				if ( c == 0x03 ) return  ;	// IfEnd
				break ;
			case 0x7F:	// 
			case 0xFFFFFFF9:	// 
			case 0xFFFFFFE5:	// 
			case 0xFFFFFFE6:	// 
			case 0xFFFFFFE7:	// 
//			case 0xFFFFFFFF:	// 
				ExecPtr++;
				break;
		}
	}
	return ;
}

int Search_ElseIfEnd( char *SRC ){
	unsigned int c;
	while (1){	// Search  Else or IfEnd
		c=SRC[ExecPtr++];
		switch ( c ) {
			case 0x00:	// <EOF>
				ExecPtr--;
				return 0 ;
			case 0x27:	// ' rem
				Skip_rem(SRC);
				break;
			case 0xFFFFFFF7:	// 
				c=SRC[ExecPtr++];
				if ( c == 0x00 ) {			// If
					Search_IfEnd(SRC);
					break;
				} else
				if ( c == 0x02 ) return 1 ; 	// Else
				else
				if ( c == 0x03 ) return 2 ; 	// IfEnd
				else
				if ( c == 0x0F ) return 3 ; 	// ElseIf
				break;
			case 0x0000007F:	// 
			case 0xFFFFFFF9:	// 
			case 0xFFFFFFE5:	// 
			case 0xFFFFFFE6:	// 
			case 0xFFFFFFE7:	// 
//			case 0xFFFFFFFF:	// 
				ExecPtr++;
				break;
		}
	}
	return 0 ;
}

void CB_If( char *SRC, CchIf *CacheIf ){
	int c,i,stat;
	int value;
  loop:
	value = CB_EvalInt( SRC ) ;
	c =SRC[ExecPtr];
	if ( ( c == ':'  ) || ( c == 0x0D ) )  c=SRC[++ExecPtr];
	if ( c == 0x27 ) if ( SRC[++ExecPtr]=='/' )  c=SRC[++ExecPtr];
	if ( ( c != 0xFFFFFFF7 ) || ( SRC[ExecPtr+1] != 0x01 ) ) { CB_Error(ThenWithoutIfERR); return; } // not Then error 
	ExecPtr+=2 ;
	if ( value ) return ; // true
	
	for ( i=0; i<CacheIf->CNT; i++ ) {
		if ( CacheIf->Ptr[i]==ExecPtr ) { ExecPtr=CacheIf->Adrs[i]; return ; }	// adrs ok
	}
	CacheIf->Ptr[CacheIf->CNT]=ExecPtr;
	stat=Search_ElseIfEnd( SRC );
	if ( CacheIf->CNT < IfCntMax ) {
		CacheIf->Adrs[CacheIf->CNT]=ExecPtr;
		CacheIf->CNT++;
	} else CacheIf->CNT=0;	// over reset

	if ( stat == 3 ) goto loop;	// ElseIf
}

void CB_Else( char *SRC, CchIf *CacheElse ){
	int i;
	for ( i=0; i<CacheElse->CNT; i++ ) {
		if ( CacheElse->Ptr[i]==ExecPtr ) { ExecPtr=CacheElse->Adrs[i]; return ; }	// adrs ok
	}
	CacheElse->Ptr[CacheElse->CNT]=ExecPtr;
	Search_IfEnd( SRC );
	if ( CacheElse->CNT < IfCntMax ) {
		CacheElse->Adrs[CacheElse->CNT]=ExecPtr;
		CacheElse->CNT++;
	} else CacheElse->CNT=0;	// over reset
}

//-----------------------------------------------------------------------------
int Search_Next( char *SRC ){
	int c;
	while (1){
		c=SRC[ExecPtr++];
		switch ( c ) {
			case 0x00:	// <EOF>
				ExecPtr--;
				return 0 ;
			case 0x27:	// ' rem
				Skip_rem(SRC);
				break;
			case 0xFFFFFFF7:	// 
				if ( SRC[ExecPtr] == 0x04 ) { ExecPtr++;				// For
					Search_Next(SRC);
					break;
				}
				if ( SRC[ExecPtr] == 0x07 ) { ExecPtr++; return 1; }	// Next
			case 0x7F:	// 
			case 0xFFFFFFF9:	// 
			case 0xFFFFFFE5:	// 
			case 0xFFFFFFE6:	// 
			case 0xFFFFFFE7:	// 
//			case 0xFFFFFFFF:	// 
				ExecPtr++;
				break;
		}
	}
	return 0;
}

void CB_For( char *SRC ,StkFor *StackFor, CurrentStk *CurrentStruct ){
	int c;
	CB_CurrentValue = EvalsubTop( SRC );
	c=SRC[ExecPtr];
	if ( c == 0x0E ) {	// ->
		ExecPtr++;
		c=SRC[ExecPtr];
		if ( ( 'A' <= c ) && ( c <= 'Z' ) ) {
			StackFor->Var[StackFor->Ptr]=(int*)&REG[c-'A'];
			CB_Store(SRC);
		} else
		if ( ( 'a' <= c ) && ( c <= 'z' ) ) {
			StackFor->Var[StackFor->Ptr]=(int*)LocalDbl[c-'a'];
			CB_Store(SRC);
		} else { CB_Error(SyntaxERR); return; }	// Syntax error
	}
	if ( StackFor->Ptr >= StackForMax ) { CB_Error(NestingERR); return; } //  nesting error
	c=SRC[ExecPtr];
	if ( ( c != 0xFFFFFFF7 ) || ( SRC[ExecPtr+1] != 0x05 ) ) { CB_Error(SyntaxERR); return; }	// Syntax error
	ExecPtr+=2;
	StackFor->End[StackFor->Ptr] = EvalsubTop( SRC );
	c=SRC[ExecPtr];
	if ( ( c == 0xFFFFFFF7 ) && ( SRC[ExecPtr+1] == 0x06 ) ) {	// Step
		ExecPtr+=2;
		StackFor->Step[StackFor->Ptr] = EvalsubTop( SRC );
	} else {
		StackFor->Step[StackFor->Ptr] = 1;
	}
	StackFor->Adrs[StackFor->Ptr] = ExecPtr;
	StackFor->Ptr++;
	CurrentStruct->TYPE[CurrentStruct->CNT]=1;
	CurrentStruct->CNT++;
}

void CB_Next( char *SRC ,StkFor *StackFor, CurrentStk *CurrentStruct ){
	double step,end;
	double *dptr;
	if ( StackFor->Ptr <= 0 ) { CB_Error(NextWithoutForERR); return; } // Next without for error
	StackFor->Ptr--;
	CurrentStruct->CNT--;
	step = StackFor->Step[StackFor->Ptr];
	dptr=(double*)StackFor->Var[StackFor->Ptr];
	(*dptr) += step;
	if ( step > 0 ) { 	// step +
		if ( (*dptr) > StackFor->End[StackFor->Ptr] ) return ; // exit
		ExecPtr = StackFor->Adrs[StackFor->Ptr];
		StackFor->Ptr++;		// continue
	CurrentStruct->TYPE[CurrentStruct->CNT]=1;
	CurrentStruct->CNT++;
	}
	else {									// step -
		if ( (*dptr) < StackFor->End[StackFor->Ptr] ) return ; // exit
		ExecPtr = StackFor->Adrs[StackFor->Ptr];
		StackFor->Ptr++;		// continue
	CurrentStruct->TYPE[CurrentStruct->CNT]=1;
	CurrentStruct->CNT++;
	}
}

//-----------------------------------------------------------------------------
int Search_WhileEnd( char *SRC ){
	int c;
	while (1){
		c=SRC[ExecPtr++];
		switch ( c ) {
			case 0x00:	// <EOF>
				ExecPtr--;
				return 0 ;
			case 0x27:	// ' rem
				Skip_rem(SRC);
				break;
			case 0xFFFFFFF7:	// 
				if ( SRC[ExecPtr] == 0x08 ) { ExecPtr++;				// While
					Search_WhileEnd(SRC);
					break;
				}
				if ( SRC[ExecPtr] == 0x09 ) { ExecPtr++; return 1; }	// WhileEnd
			case 0x7F:	// 
			case 0xFFFFFFF9:	// 
			case 0xFFFFFFE5:	// 
			case 0xFFFFFFE6:	// 
			case 0xFFFFFFE7:	// 
//			case 0xFFFFFFFF:	// 
				ExecPtr++;
				break;
		}
	}
	return 0;
}
int Search_LpWhile( char *SRC ){
	unsigned int c;
	while (1){
		c=SRC[ExecPtr++];
		switch ( c ) {
			case 0x00:	// <EOF>
				ExecPtr--;
				return 0 ;
			case 0x27:	// ' rem
				Skip_rem(SRC);
				break;
			case 0xFFFFFFF7:	// 
				if ( SRC[ExecPtr] == 0x0A ) { ExecPtr++;				// Do
					Search_LpWhile(SRC);
					break;
				}
				if ( SRC[ExecPtr] == 0x0B ) { ExecPtr++; return 1; }	// LpWhile
			case 0x7F:	// 
			case 0xFFFFFFF9:	// 
			case 0xFFFFFFE5:	// 
			case 0xFFFFFFE6:	// 
			case 0xFFFFFFE7:	// 
//			case 0xFFFFFFFF:	// 
				ExecPtr++;
				break;
		}
	}
	return 0;
}
void CB_While( char *SRC, StkWhileDo *StackWhileDo, CurrentStk *CurrentStruct ) {
	int wPtr=ExecPtr;
	int i;
	i=CB_EvalInt( SRC );
	if ( i == 0 ) {		// false
		if ( Search_WhileEnd(SRC) == 0 ) { CB_Error(WhileWithoutWhileEndERR); return; }  // While without WhileEnd error
		return ; // exit
	}
	if ( StackWhileDo->WhilePtr >= StackWhileMax ) { CB_Error(NestingERR); return; }  //  nesting error
	StackWhileDo->WhileAdrs[StackWhileDo->WhilePtr] = wPtr;
	StackWhileDo->WhilePtr++;
	CurrentStruct->TYPE[CurrentStruct->CNT]=2;
	CurrentStruct->CNT++;
}

void CB_WhileEnd( char *SRC, StkWhileDo *StackWhileDo, CurrentStk *CurrentStruct ) {
	int exitPtr=ExecPtr;
	int i;
	if ( StackWhileDo->WhilePtr <= 0 ) { CB_Error(WhileEndWithoutWhileERR); return; }  // WhileEnd without While error
	StackWhileDo->WhilePtr--;
	CurrentStruct->CNT--;
	ExecPtr = StackWhileDo->WhileAdrs[StackWhileDo->WhilePtr] ;
	i=CB_EvalInt( SRC );
	if ( i == 0 ) {		// false
		ExecPtr=exitPtr;
		return ; // exit
	}
	StackWhileDo->WhilePtr++;
	CurrentStruct->TYPE[CurrentStruct->CNT]=2;
	CurrentStruct->CNT++;
}

void CB_Do( char *SRC, StkWhileDo *StackWhileDo, CurrentStk *CurrentStruct ) {
	if ( StackWhileDo->DoPtr >= StackDoMax ) { CB_Error(NestingERR); return; }  //  nesting error
	StackWhileDo->DoAdrs[StackWhileDo->DoPtr] = ExecPtr;
	StackWhileDo->DoPtr++;
	CurrentStruct->TYPE[CurrentStruct->CNT]=3;
	CurrentStruct->CNT++;
}

void CB_LpWhile( char *SRC, StkWhileDo *StackWhileDo, CurrentStk *CurrentStruct ) {
	int i;
	if ( StackWhileDo->DoPtr <= 0 ) { CB_Error(LpWhileWithoutDoERR); return; }  // LpWhile without Do error
	StackWhileDo->DoPtr--;
	CurrentStruct->CNT--;
	i=CB_EvalInt( SRC );
	if ( i == 0  ) return ; // exit
	ExecPtr = StackWhileDo->DoAdrs[StackWhileDo->DoPtr] ;				// true
	StackWhileDo->DoPtr++;
	CurrentStruct->TYPE[CurrentStruct->CNT]=3;
	CurrentStruct->CNT++;
}

//-----------------------------------------------------------------------------
int Search_SwitchEnd( char *SRC ){
	unsigned int c;
	while (1){
		c=SRC[ExecPtr++];
		switch ( c ) {
			case 0x00:	// <EOF>
				ExecPtr--;
				return 0 ;
			case 0x27:	// ' rem
				Skip_rem(SRC);
				break;
			case 0xFFFFFFF7:	// 
				if ( SRC[ExecPtr] == 0xFFffFFEA ) { ExecPtr++;				// Switch
					Search_SwitchEnd(SRC);
					break;
				}
				if ( SRC[ExecPtr] == 0xFFFFFFED ) { ExecPtr++; return 1; }	// SwitchEnd
			case 0x7F:	// 
			case 0xFFFFFFF9:	// 
			case 0xFFFFFFE5:	// 
			case 0xFFFFFFE6:	// 
			case 0xFFFFFFE7:	// 
//			case 0xFFFFFFFF:	// 
				ExecPtr++;
				break;
		}
	}
	return 0;
}
int Search_CaseEnd( char *SRC ){
	unsigned int c;
	while (1){
		c=SRC[ExecPtr++];
		switch ( c ) {
			case 0x00:	// <EOF>
				ExecPtr--;
				return 0 ;
			case 0x27:	// ' rem
				Skip_rem(SRC);
				break;
			case 0xFFFFFFF7:	// 
				if ( SRC[ExecPtr] == 0xFFffFFEA ) { ExecPtr++;				// Switch
					Search_SwitchEnd(SRC);
					break;
				}
				if ( SRC[ExecPtr] == 0xFFFFFFED ) { ExecPtr--; return 1; }	// SwitchEnd
				if ( SRC[ExecPtr] == 0xFFFFFFEB ) { ExecPtr--; return 2; }	// Case
				if ( SRC[ExecPtr] == 0xFFFFFFEC ) { ExecPtr--; return 3; }	// Default
			case 0x7F:	// 
			case 0xFFFFFFF9:	// 
			case 0xFFFFFFE5:	// 
			case 0xFFFFFFE6:	// 
			case 0xFFFFFFE7:	// 
//			case 0xFFFFFFFF:	// 
				ExecPtr++;
				break;
		}
	}
	return 0;
}

void CB_Switch( char *SRC, StkSwitch *StackSwitch, CurrentStk *CurrentStruct ,CchIf *CacheSwitch ) {
	int wPtr;
	int c,i,j,value;
	value=CB_EvalInt( SRC );
//	c=SRC[ExecPtr];
//	if ( ( c!=0x0D ) && ( c!=':' ) ) { CB_Error(SyntaxERR); return; }	// Syntax error
//	ExecPtr++;
	wPtr=ExecPtr;

	for ( i=0; i<CacheSwitch->CNT; i++ ) {
		if ( CacheSwitch->Ptr[i]==ExecPtr ) break; 	// adrs ok
	}
	if ( i >= CacheSwitch->CNT ) {
		CacheSwitch->Ptr[CacheSwitch->CNT]=ExecPtr;
		if ( Search_SwitchEnd(SRC) == 0 ) { CB_Error(SwitchWithoutSwitchEndERR); return; }  // Switch without SwitchEnd error
		if ( CacheSwitch->CNT < IfCntMax ) {
			CacheSwitch->Adrs[CacheSwitch->CNT]=ExecPtr;
			CacheSwitch->CNT++;
		} else CacheSwitch->CNT=0;	// over reset
	} else  ExecPtr = CacheSwitch->Adrs[i];	// Break out adrs set
	
	StackSwitch->EndAdrs[StackSwitch->Ptr] = ExecPtr;	// Break out adrs set
	StackSwitch->Value[StackSwitch->Ptr] = value;
	StackSwitch->flag[StackSwitch->Ptr] = 0;	// case through clear
	
	if ( StackSwitch->Ptr >= StackSwitchMax ) { CB_Error(NestingERR); return; }  //  nesting error
	StackSwitch->Ptr++;
	CurrentStruct->TYPE[CurrentStruct->CNT]=4;
	CurrentStruct->CNT++;
	ExecPtr=wPtr;
	
}
void CB_Case( char *SRC, StkSwitch *StackSwitch, CurrentStk *CurrentStruct ) {
	int exitPtr=ExecPtr;
	int c,value;
	if ( StackSwitch->Ptr <= 0 ) { CB_Error(CaseWithoutSwitchERR); return; }  // Case without Switch error
	StackSwitch->Ptr--;

	c=SRC[ExecPtr];
	value=Eval_atod( SRC, c );
//	c=SRC[ExecPtr];
//	if ( ( c!=0x0D ) && ( c!=':' ) ) { CB_Error(SyntaxERR); return; }	// Syntax error
//	ExecPtr++;

	if ( StackSwitch->flag[StackSwitch->Ptr] == 0 ) {
		if ( value != StackSwitch->Value[StackSwitch->Ptr] ) {		// false
			Search_CaseEnd(SRC);
		} else {	// true
			StackSwitch->flag[StackSwitch->Ptr]=1;	// case through set
		}
	}
	StackSwitch->Ptr++;
	CurrentStruct->CNT--;
	CurrentStruct->TYPE[CurrentStruct->CNT]=4;
	CurrentStruct->CNT++;
}

void CB_Default( char *SRC, StkSwitch *StackSwitch, CurrentStk *CurrentStruct ) {
	int exitPtr=ExecPtr;
	int c,i;
	if ( StackSwitch->Ptr <= 0 ) { CB_Error(DefaultWithoutSwitchERR); return; }  // Default without Switch error
	CurrentStruct->CNT--;
	CurrentStruct->TYPE[CurrentStruct->CNT]=4;
	CurrentStruct->CNT++;
}

void CB_SwitchEnd( char *SRC, StkSwitch *StackSwitch, CurrentStk *CurrentStruct ) {
	int exitPtr=ExecPtr;
	int c,i;
	if ( StackSwitch->Ptr <= 0 ) { CB_Error(SwitchEndWithoutSwitchERR); return; }  // SwitchEnd without Switch error
	StackSwitch->Ptr--;
	CurrentStruct->CNT--;
}

void CB_Break( char *SRC, StkFor *StackFor, StkWhileDo *StackWhileDo, StkSwitch *StackSwitch, CurrentStk *CurrentStruct ) {
	
	if ( CurrentStruct->CNT <=0 ) { CB_Error(NotLoopERR); return; }  // Not Loop error
	CurrentStruct->CNT--;
	switch ( CurrentStruct->TYPE[CurrentStruct->CNT] ) {
		case 1:	// For Next
			if ( Search_Next(SRC) == 0 )     { CB_Error(ForWithoutNextERR); return; }  // For without Next error
			StackFor->Ptr--;
			return ;
		case 2:	// While WhileEnd
			if ( Search_WhileEnd(SRC) == 0 ) { CB_Error(WhileWithoutWhileEndERR); return; }  // While without WhileEnd error
			StackWhileDo->WhilePtr--;
			return ;
		case 3:	// DO LpWhile
			if ( Search_LpWhile(SRC) == 0 )  { CB_Error(DoWithoutLpWhileERR); return; }  // Do without LpWhile error
			StackWhileDo->DoPtr--;
			return ;
		case 4:	// Switch
			StackSwitch->Ptr--;
			ExecPtr=StackSwitch->EndAdrs[StackSwitch->Ptr];
			return ;
		default:
			break;
	}
}


//----------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------

void CB_Dsz( char *SRC ) { //	Dsz
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
		if ( c=='%' ) {
			ExecPtr++;
			REGINT[reg] --;
			CB_CurrentValue = REGINT[reg] ;
		} else
		if ( c=='[' ) { 
			ExecPtr++;
			MatOprand2( SRC, reg, &dimA, &dimB );
			goto Matrix;
		} else {
			if ( c=='#' ) ExecPtr++;
			REG[reg] --;
			CB_CurrentValue = REG[reg] ;
		}
	} else 
	if ( ( 'a' <= c ) && ( c <= 'z' ) ) {
		ExecPtr++;
		reg=c-'a';
		c=SRC[ExecPtr];
		if ( c=='%' ) {
			ExecPtr++;
			LocalInt[reg][0] --;
			CB_CurrentValue = LocalInt[reg][0] ;
		} else
		if ( c=='[' ) { 
			ExecPtr++;
			MatOprand2( SRC, reg, &dimA, &dimB );
			goto Matrix;
		} else {
			if ( c=='#' ) ExecPtr++;
			LocalDbl[reg][0] --;
			CB_CurrentValue = LocalDbl[reg][0] ;
		}
	} else 
	if ( c==0x7F ) {
			MatrixOprand( SRC, &reg, &dimA, &dimB );
		Matrix:
			if ( ErrorNo ) return ; // error
			CB_CurrentValue = ReadMatrix( reg, dimA,dimB ) ;
			CB_CurrentValue --;
			WriteMatrix( reg, dimA,dimB, CB_CurrentValue ) ;
	} else { CB_Error(SyntaxERR); return; }	// Syntax error

	c=SRC[ExecPtr];
	if ( ( c==':' ) || ( c==0x0D ) ) {
		ExecPtr++;
		if ( CB_CurrentValue ) return ;
		else {
			Skip_block(SRC);
		}
	} else if ( c==0x0C ) {  // dsps
		ExecPtr++;
		CB_Disps( SRC ,2);
		if ( CB_CurrentValue ) return ;
		else {
			Skip_block(SRC);
		}
	} else { CB_Error(SyntaxERR); return; }	// Syntax error
}

void CB_Isz( char *SRC ) { //	Isz
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
		if ( c=='%' ) {
			ExecPtr++;
			REGINT[reg] ++;
			CB_CurrentValue = REGINT[reg] ;
		} else
		if ( c=='[' ) { 
			ExecPtr++;
			MatOprand2( SRC, reg, &dimA, &dimB );
			goto Matrix;
		} else {
			if ( c=='#' ) ExecPtr++;
			REG[reg] ++;
			CB_CurrentValue = REG[reg] ;
		}
	} else 
	if ( ( 'a' <= c ) && ( c <= 'z' ) ) {
		ExecPtr++;
		reg=c-'a';
		c=SRC[ExecPtr];
		if ( c=='%' ) {
			ExecPtr++;
			LocalInt[reg][0] ++;
			CB_CurrentValue = LocalInt[reg][0] ;
		} else
		if ( c=='[' ) { 
			ExecPtr++;
			MatOprand2( SRC, reg, &dimA, &dimB );
			goto Matrix;
		} else {
			if ( c=='#' ) ExecPtr++;
			LocalDbl[reg][0] ++;
			CB_CurrentValue = LocalDbl[reg][0] ;
		}
	} else 
	if ( c==0x7F ) {
			MatrixOprand( SRC, &reg, &dimA, &dimB );
		Matrix:
			if ( ErrorNo ) return ; // error
			CB_CurrentValue = ReadMatrix( reg, dimA,dimB ) ;
			CB_CurrentValue ++;
			WriteMatrix( reg, dimA,dimB, CB_CurrentValue ) ;
	} else { CB_Error(SyntaxERR); return; }	// Syntax error

	c=SRC[ExecPtr];
	if ( ( c==':' ) || ( c==0x0D ) ) {
		ExecPtr++;
		if ( CB_CurrentValue ) return ;
		else {
			Skip_block(SRC);
		}
	} else if ( c==0x0C ) {  // dsps
		ExecPtr++;
		CB_Disps( SRC ,2);
		if ( CB_CurrentValue ) return ;
		else {
			Skip_block(SRC);
		}
	} else { CB_Error(SyntaxERR); return; }	// Syntax error
}
//----------------------------------------------------------------------------------------------

void CB_Store( char *SRC ){	// ->
	int	en,i,j;
	int dimA,dimB,reg;
	int mptr;
	char*	MatAryC;
	short*	MatAryW;
	int*	MatAryI;
	
	int c=SRC[ExecPtr];
	if ( ( 'A' <= c ) && ( c <= 'Z' ) ) {
		reg=c-'A';
		ExecPtr++;
		if ( SRC[ExecPtr] == 0x7E ) {		// '~'
			ExecPtr++;
			c=SRC[ExecPtr];
			if ( ( 'A' <= c ) && ( c <= 'Z' ) ) {
				en=c-'A';
				if ( en<reg ) { CB_Error(SyntaxERR); return; }	// Syntax error
				c=SRC[++ExecPtr];
				if ( c=='%' ) { ExecPtr++;  for ( i=reg; i<=en; i++) REGINT[ i ] = CB_CurrentValue; }
				else
				if ( c=='#' ) ExecPtr++;
				for ( i=reg; i<=en; i++) REG[ i ] = CB_CurrentValue;
			}
		} else {					// 
			c=SRC[ExecPtr];
			if ( c=='%' ) { ExecPtr++;  REGINT[reg] = CB_CurrentValue ; }
			else
			if ( c=='[' ) goto Matrix;
			else {
				if ( c=='#' ) ExecPtr++;
				REG[reg] = CB_CurrentValue;
			}
		}
	} else
	if ( ( 'a' <= c ) && ( c <= 'z' ) ) {
		reg=c-'a';
		ExecPtr++;
		if ( SRC[ExecPtr] == 0x7E ) {		// '~'
			ExecPtr++;
			c=SRC[ExecPtr];
			if ( ( 'a' <= c ) && ( c <= 'z' ) ) {
				en=c-'a';
				if ( en<reg ) { CB_Error(SyntaxERR); return; }	// Syntax error
				c=SRC[++ExecPtr];
				if ( c=='%' ) { ExecPtr++;  for ( i=reg; i<=en; i++) LocalInt[ i ][0] = CB_CurrentValue; }
				else
				if ( c=='#' ) ExecPtr++;
				for ( i=reg; i<=en; i++) LocalDbl[ i ][0] = CB_CurrentValue;
			}
		} else {					// 
			c=SRC[ExecPtr];
			if ( c=='%' ) { ExecPtr++;  LocalInt[reg][0] = CB_CurrentValue ; }
			else
			if ( c=='[' ) goto Matrix;
			else {
				if ( c=='#' ) ExecPtr++;
				LocalDbl[reg][0] = CB_CurrentValue;
			}
		}
	} else
	if ( c==0x7F ) {
		c = SRC[ExecPtr+1] ; 
		if ( c == 0x40 ) {	// Mat A[a,b]
			ExecPtr+=2;
			c=SRC[ExecPtr]; if ( ( 'A'<=c )&&( c<='z' ) ) { reg=c-'A'; ExecPtr++; } else CB_Error(SyntaxERR) ; // Syntax error 
			if ( SRC[ExecPtr] != '[' ) { 
				if ( MatAry[reg].SizeA == 0 ) { CB_Error(NoMatrixArrayERR); return; }	// No Matrix Array error
				InitMatSub( reg, CB_CurrentValue);
			} else {
			Matrix:
				ExecPtr++;
				MatOprand2( SRC, reg, &dimA, &dimB);
				if ( ErrorNo ) return ; // error
				WriteMatrix( reg, dimA, dimB, CB_CurrentValue);
			}
		} else if ( c == 0x46 ) {	// Dim A
				ExecPtr+=2;
				if ( CB_CurrentValue ) 
						CB_MatrixInitsubNoMat( SRC, &reg, CB_CurrentValue, 1, 0 );
				else {
					c = SRC[ExecPtr];
					if ( ( 'A' <= c ) && ( c <= 'z' ) ) {
						ExecPtr++;
						DeleteMatrix( c-'A' );
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
	if ( c=='%' ) {
		CB_TicksStart=RTC_GetTicks()-CB_TicksStart;
		SetRtc( CB_CurrentValue );
		i=RTC_GetTicks();
		CB_TicksAdjust=i-CB_CurrentValue ;	// 
		CB_TicksStart+=(i-1);	// 
		skip_count=0;
	} else
	if ( c==0xFFFFFFF9 ) {
		c = SRC[ExecPtr+1] ; 
		if ( c == 0x21 ) {	// Xdot
				if ( CB_CurrentValue == 0 ) { CB_Error(RangeERR); return; }	// Range error
				ExecPtr+=2;
				Xdot = CB_CurrentValue ;
				Xmax = Xmin + Xdot*126.;
		} else { CB_Error(SyntaxERR); return; }	// Syntax error
	} else { CB_Error(SyntaxERR); return; }	// Syntax error
}

//----------------------------------------------------------------------------------------------
int  CB_Input( char *SRC ){
	unsigned int key;
	int c;
	double DefaultValue=0;
	int flag=0,flagint=0;
	int reg,bptr,mptr;
	int dimA,dimB;
	char buffer[32];
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
		c=CB_IsStr( SRC, ExecPtr+1 );
		if ( ( c==2 ) || ( c==0x3F ) ) flag=2;
	} else
	if ( ( 'A' <= c ) && ( c <= 'Z' ) ) {
		flag=1;
		reg=c-'A';
		c=SRC[ExecPtr+1];
		if ( CB_INT ) {
			if ( c=='#' ) {
				ExecPtr++;
				DefaultValue = REG[reg] ;
			} else
			if ( c=='[' ) {
				ExecPtr++;
				MatOprandInt2( SRC, reg, &dimA, &dimB );
				goto Matrix;
			} else { flagint=1; 
				if ( c=='%' ) ExecPtr++;
				DefaultValue = REGINT[reg] ;
			}
		} else {
			if ( c=='%' ) { flagint=1; 
				ExecPtr++;
				DefaultValue = REGINT[reg] ;
			} else
			if ( c=='[' ) {
				ExecPtr++;
				MatOprand2( SRC, reg, &dimA, &dimB );
				goto Matrix;
			} else { 
				if ( c=='#' ) ExecPtr++;
				DefaultValue = REG[reg] ;
			}
		}
	} else
	if ( ( 'a' <= c ) && ( c <= 'z' ) ) {
		flag=1;
		reg=c-'a';
		c=SRC[ExecPtr+1];
		if ( CB_INT ) {
			if ( c=='#' ) {
				ExecPtr++;
				DefaultValue = LocalDbl[reg][0] ;
			} else
			if ( c=='[' ) {
				ExecPtr++;
				MatOprandInt2( SRC, reg, &dimA, &dimB );
				goto Matrix;
			} else { flagint=1; 
				if ( c=='%' )  ExecPtr++;
				DefaultValue = LocalInt[reg][0] ;
			}
		} else {
			if ( c=='%' ) {flagint=1; 
				ExecPtr++;
				DefaultValue = LocalInt[reg][0] ;
			} else
			if ( c=='[' ) {
				ExecPtr++;
				MatOprand2( SRC, reg, &dimA, &dimB );
				goto Matrix;
			} else { 
				if ( c=='#' ) ExecPtr++;
				DefaultValue = LocalDbl[reg][0] ;
			}
		}
	} else
	if ( c==0x7F ) {
		c = SRC[ExecPtr+1] ; 
		if ( c == 0x40 ) {	// Mat A[a,b]
			MatrixOprand( SRC, &reg, &dimA, &dimB );
		Matrix:
			if ( ErrorNo ) return ; // error
			if ( MatAry[reg].SizeA == 0 ) { CB_Error(NoMatrixArrayERR); return; }	// No Matrix Array error
			DefaultValue = ReadMatrix( reg, dimA, dimB);
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
		} else if ( c == 0xFFFFFFF0 ) {	// GraphY
			ExecPtr+=2;
			reg=defaultGraphAry;
			if ( MatAry[reg].SizeA == 0 ) { CB_Error(MemoryERR); return 0; }	// Memory error
			dimA = CB_EvalInt( SRC );	// str no : Mat s[n,len]
			if ( ( dimA < 1 ) || ( dimA > MatAry[reg].SizeA ) ) { CB_Error(ArgumentERR); return; }  // Argument error
			dimB=1;
			flag=3;
			ExecPtr=bptr;
		} else { CB_Error(SyntaxERR); return; }	// Syntax error
	} else
	if ( c==0xFFFFFFF9 ) {
		c = SRC[ExecPtr+1] ; 
		if ( c == 0x3F ) {	// Str 1-20
			ExecPtr+=2;
			reg=defaultStrAry;
			if ( MatAry[reg].SizeA == 0 ) { CB_Error(MemoryERR); return 0; }	// Memory error
			dimA = CB_EvalInt( SRC );	// str no : Mat s[n,len]
			if ( ( dimA < 1 ) || ( dimA > MatAry[reg].SizeA ) ) { CB_Error(ArgumentERR); return; }  // Argument error
			dimB=1;
			flag=3;
			ExecPtr=bptr;
		} else
		if ( c == 0x21 ) {	// Xdot
				DefaultValue = Xdot ;
				flag=1;
		} else { CB_Error(SyntaxERR); return; }	// Syntax error
	} else
	if ( ( c=='$' ) || ( c=='&' ) ) {
		ExecPtr++;
		MatrixOprand( SRC, &reg, &dimA, &dimB );
		if ( ErrorNo ) return ; // error
		if ( MatAry[reg].SizeA == 0 ) { CB_Error(NoMatrixArrayERR); return; }	// No Matrix Array error
		if ( MatAry[reg].ElementSize != 8 ) { CB_Error(ArgumentERR); return; }	// element size error
		flag=3;
		ExecPtr=bptr;
	} else { CB_Error(SyntaxERR); return; }	// Syntax error

	switch ( flag ) {
		case 0:	// value
			CB_CurrentValue = InputNumD_CB( 1, CursorY, 21, 0 );
			ErrorNo=0; // error cancel
			if ( BreakPtr ) { BreakPtr--; ExecPtr=BreakPtr; return 0; }
			CBint_CurrentValue = CB_CurrentValue ;
			break;
		case 1:	// value
			sprintGR(buffer, DefaultValue, 22-CursorX,RIGHT_ALIGN, CB_Round.MODE, CB_Round.DIGIT);
			locate( CursorX, CursorY); Print((unsigned char*)buffer);
			Scrl_Y();
			CB_CurrentValue = InputNumD_CB1( 1, CursorY, 21, DefaultValue );
			ErrorNo=0; // error cancel
			if ( BreakPtr ) { BreakPtr--; ExecPtr=BreakPtr; return 0; }
			CBint_CurrentValue = CB_CurrentValue ;
			if ( flagint ) {
				CBint_Store( SRC );
			} else {
				CB_Store( SRC );
			}
			break;
		case 2:	// ? -> str 
			CB_CurrentStr=CB_StrBuffer[0];
			CB_CurrentStr[0]='\0';
			key=InputStr( 1, CursorY, CB_StrBufferMax-1,  CB_CurrentStr, ' ', REV_OFF);
			ErrorNo=0; // error cancel
			if ( key==KEY_CTRL_AC  ) { ExecPtr--; BreakPtr=ExecPtr;  return 0; }
			if ( SRC[ExecPtr]==0x0E ) ExecPtr++;	// -> skip
			CB_StorStr( SRC );
			break;
		case 3:	// ?&Mat  ?Str1-20
			MatAryC=MatrixPtr( reg, dimA, dimB );
			OpcodeStringToAsciiString(buffer, MatAryC, 31);
			locate( CursorX, CursorY); Print((unsigned char*)buffer);
			Scrl_Y();
			CB_CurrentStr=MatAryC;
			key=InputStr( 1, CursorY, MatAry[reg].SizeB-1,  CB_CurrentStr, ' ', REV_OFF);
			ErrorNo=0; // error cancel
			if ( key==KEY_CTRL_AC  ) { ExecPtr--; BreakPtr=ExecPtr;  return 0; }
			CB_StorStr( SRC );
			break;
	}
	Scrl_Y();
	Bdisp_PutDisp_DD_DrawBusy();
	return 0 ;
}
//----------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------
/*
int GetCmmOpcode(char *SRC, char *buffer, int Maxlen) {
	int c;
	int ptr=0;
	while (1){
		c = SRC[ExecPtr++];
		buffer[ptr++]=c;
		switch ( c ) {
			case 0x00:	// <EOF>
			case 0x0C:	//
			case 0x0D:	//
			case ':':	//
			case ',':	// ,
				buffer[--ptr]='\0' ;
				ExecPtr--;
				return ptr;
			case 0x5C:	// escape
				buffer[ptr-1]=SRC[ExecPtr++];
				break;
			case 0x7F:	// 
			case 0xFFFFFFF7:	// 
			case 0xFFFFFFF9:	// 
			case 0xFFFFFFE5:	// 
			case 0xFFFFFFE6:	// 
			case 0xFFFFFFE7:	// 
			case 0xFFFFFFFF:	// 
				buffer[ptr++]=SRC[ExecPtr++];
				break;
			default:
				break;
		}
		if ( ptr >= Maxlen-1 ) { CB_Error(StringTooLongERR); break; }	// String too Long error
	}
	return ptr;
}
void CB_arg( char *SRC ) {
	int c=1,i,j;
	Argc=0;

	c=SRC[ExecPtr];
	while ( (c!=0)&&(c!=0x0C)&&(c!=0x0D)&&(c!=':') ) {
		if ( c == 0x22 ) {	// String
			ExecPtr++;
			CB_GetQuotOpcode( SRC, argv[Argc], 32 );	// 
			
		} else {
			GetCmmOpcode( SRC, argv[Argc], 32 );
		}
		Argc++;
		if ( Argc > ArgcMAX ) { CB_Error(TooMuchData); return; }	// too much error
		c=SRC[ExecPtr];
	}
}
*/
void CB_argNum( char *SRC ) {
	int c=1,i,j;
	Argc=0;
	while ( (c!=0)&&(c!=0x0C)&&(c!=0x0D)&&(c!=':') ) {
		if ( CB_INT ) LocalInttmp[Argc]=EvalIntsubTop( SRC ); else LocalDbltmp[Argc]=CB_EvalDbl( SRC );
		Argc++;
		c=SRC[ExecPtr];
		if ( c != ',' ) break; 	// 
		ExecPtr++;
		if ( Argc > ArgcMAX ) { CB_Error(TooMuchData); return; }	// too much error
	}
}

int CB_SearchProg( char *name ) { //	Prog search
	int j,i=1;
	char *ptr;
	while ( ProgfileAdrs[i] ) {
		ptr=ProgfileAdrs[i]+0x3C;
		for (j=0;j<8;j++) if ( ptr[j] != name[j] ) break;
		if ( j==8 )	return i ; // ok
		i++;
	}
	return -1; // fault
}
void CB_Prog( char *SRC, int *localvarInt, double *localvarDbl ) { //	Prog "..."
	int c,i,j;
	char buffer[32]="\0\0\0\0\0\0\0\0";
	char *src;
	char *StackProgSRC;
	int StackProgExecPtr;
	char stat;
	char ProgNo_bk;
	char BreakPtr_bk; 
	char StepOutProgNo=0;

	c=SRC[ExecPtr];
	if ( c == 0x22 ) {	// String
		ExecPtr++;
		CB_GetQuotOpcode(SRC, buffer,16);	// Prog name
	}

	c=SRC[ExecPtr];
	if ( c == ',' ) {	// arg
		ExecPtr++;
		CB_argNum( SRC );
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

	if ( DebugMode == 3 ) {		// step over
		BreakPtr_bk = BreakPtr;
		BreakPtr = 0;
	}
	
	stat=CB_interpreter_sub( SRC ) ;	// --- execute sub program

	if ( DebugMode == 3 ) {		// step over
		BreakPtr = BreakPtr_bk;
	}
	if ( DebugMode == 4 ) { DebugMode = 2;  BreakPtr = -1; }	// step out
	
	if ( stat ) {
		if ( ( ErrorNo ) && ( ErrorNo != StackERR ) )return ;			// error
		else if ( BreakPtr > 0 ) return ;	// break
	}

	ProgEntryN--;
	SRC     = StackProgSRC ;
	ExecPtr = StackProgExecPtr ;
	ProgNo  = ProgNo_bk;
	
	if ( ProgNo ) {			// restore local variable
		for ( i=0; i<26; i++ ) {
				LocalDbl[i]=&REGsmall[i];
				LocalInt[i]=&REGINTsmall[i];
		}
		for ( i=0; i<ProgLocalN[ProgNo]; i++ ) {
			j=ProgLocalVar[ProgNo][i];
			if ( j>=0 ) { 
				LocalDbl[j]=&localvarDbl[i];
				LocalInt[j]=&localvarInt[i];
			}
		}
	} else {
		for ( i=0; i<26; i++ ) {
				LocalDbl[i]=&REGsmall[i];		// local var init
				LocalInt[i]=&REGINTsmall[i];	// local var init
		}
	}
	if ( ErrorNo == StackERR ) { ErrorPtr=ExecPtr; }
}

void CB_Gosub( char *SRC, short *StackGotoAdrs, short *StackGosubAdrs ){ //	Gosub N
	int c,i,j;
	int execptr=ExecPtr;

	c=SRC[ExecPtr+1];
	if ( c == ',' ) {	// arg
		ExecPtr+=2;
		CB_argNum( SRC );
		StackGosubAdrs[GosubNestN] = ExecPtr;	// return adrs
		ExecPtr=execptr;
	} else {
		StackGosubAdrs[GosubNestN] = ExecPtr+1;	// return adrs
	}
	GosubNestN++;
	if ( GosubNestN > StackGosubMax ) { CB_Error(NestingERR);  return; }	// Nesting  error
	CB_Goto( SRC, &(*StackGotoAdrs) );	// Goto Sub label
	
}

//----------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------
//int GObjectAlign4a( unsigned int n ){ return n; }	// align +4byte
//int GObjectAlign4b( unsigned int n ){ return n; }	// align +4byte
//int GObjectAligncf( unsigned int n ){ return n; }	// align +4byte
//----------------------------------------------------------------------------------------------
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
/*
int CB_GotoEndPtr( char *SRC ) {		// goto Program End Ptr
	int size;
	char *src;
	src=SRC-0x56;
	size=(src[0x47]&0xFF)*256+(src[0x48]&0xFF)+0x4C;
	return ( size -0x56 - 1 ) ;
}
*/

//----------------------------------------------------------------------------------------------

void CB_Cls( char *SRC ){
	CB_SelectGraphVRAM();	// Select Graphic Screen
	ViewWindow( Xmin, Xmax, Xscl, Ymin, Ymax, Yscl);
//	Bdisp_AllClr_VRAM();
//	GraphAxesGrid();
	Previous_PX=-1;   Previous_PY=-1; 		// ViewWindow Previous PXY init
	CB_SelectTextVRAM();	// Select Text Screen
//	Bdisp_PutDisp_DD_DrawBusy_through(SRC);
}
void CB_ClrText( char *SRC ){
	CB_SelectTextVRAM();	// Select Text Screen
	CursorX=1;
	CursorY=1;
	Bdisp_AllClr_VRAM();
	Bdisp_PutDisp_DD_DrawBusy_through(SRC);
}
void CB_ClrGraph( char *SRC ){
	CB_SelectGraphVRAM();	// Select Graphic Screen
	SetVeiwWindowInit();
	CB_Cls(SRC);
}
//----------------------------------------------------------------------------------------------
int CB_Fix( char *SRC ){
	int tmp;
	tmp=CB_EvalInt( SRC );
	if ( tmp < 0 ) if ( CB_Round.MODE == Fix ) return CB_Round.DIGIT; else return -1;
	if ( ( tmp < 0 ) || ( tmp > 15 ) ) { CB_Error(SyntaxERR); return; }	// Syntax error
	CB_Round.MODE = Fix ;
	CB_Round.DIGIT= tmp ;
	return tmp ;
}
int CB_Sci( char *SRC ){
	int tmp;
	tmp=CB_EvalInt( SRC );
	if ( tmp < 0 ) if ( CB_Round.MODE == Sci ) return CB_Round.DIGIT; else return -1;
	if ( ( tmp < 0 ) || ( tmp > 15 ) ) { CB_Error(SyntaxERR); return; }	// Syntax error
	CB_Round.MODE = Sci ;
	CB_Round.DIGIT= tmp ;
	return tmp ;
}
int CB_Norm( char *SRC ){
	int tmp;
	tmp=CB_EvalInt( SRC );
	if ( tmp < 0 ) if ( CB_Round.MODE == Norm ) return CB_Round.DIGIT; else return -1;
	if ( ( tmp < 0 ) || ( tmp > 15 ) ) { CB_Error(SyntaxERR); return; }	// Syntax error
	CB_Round.MODE = Norm ;
	CB_Round.DIGIT= tmp ;
	return tmp ;
}
void CB_Rnd(){
	CB_CurrentValue = Round( CB_CurrentValue, CB_Round.MODE, CB_Round.DIGIT );
}

//----------------------------------------------------------------------------------------------
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
	if ( Previous_PX > 0 ) LinesubSetPoint(Previous_PX, Previous_PY,1);
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

int CB_Disps( char *SRC ,short dspflag){
	char buffer[32];
	int c;
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
		if ( ( 'A' <= c ) && ( c <= 'z' ) ) {
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

void CB_end( char *SRC ){
	char buffer[32];
	int c,t;
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
		if ( ( 'A' <= c ) && ( c <= 'z' ) ) {
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

//----------------------------------------------------------------------------------------------
int CB_LocateMode( char *SRC) {
	int c=SRC[ExecPtr];
	if ( c != ',' ) return 0;	// normal
	ExecPtr++;	
	c=SRC[ExecPtr++];
	if ( c == 'N' ) return 0;	// normal
	else
	if ( c == 'R' ) return 1;	// reverse
	ExecPtr--; CB_Error(SyntaxERR); return;   // Syntax error
}

void CB_Locate( char *SRC ){
	char buffer[64];
	int c;
	int lx,ly;
	double value;
	int mode;
	int maxoplen;
	
	CB_SelectTextVRAM();	// Select Text Screen
	lx = CB_EvalInt( SRC );
	if ( ( lx < 1 ) || ( lx > 21 ) ) { CB_Error(ArgumentERR); return; }	// Argument error
	c=SRC[ExecPtr];
	if ( c != ',' ) { CB_Error(SyntaxERR); return; }	// Syntax error
	ExecPtr++;
	ly = CB_EvalInt( SRC );
	if ( ( ly < 1 ) || ( ly > 7 ) ) { CB_Error(ArgumentERR); return; }	// Argument error
	c=SRC[ExecPtr];
	if ( c != ',' ) { CB_Error(SyntaxERR); return; }	// Syntax error
	ExecPtr++;
	
	c=CB_IsStr( SRC, ExecPtr );
	if ( c ) {	// string
		CB_CurrentStr=CB_GetOpStr( SRC, &maxoplen ) ;		// String -> buffer	return 
		OpcodeStringToAsciiString( buffer, CB_CurrentStr, 63 );
	} else {	// expression
		value = CB_EvalDbl( SRC );
		sprintGR(buffer, value, 22-lx,LEFT_ALIGN, CB_Round.MODE, CB_Round.DIGIT);
	}
	if ( CB_LocateMode(SRC) ) CB_PrintRev( lx,ly, (unsigned char*)buffer );
	else 		   			  CB_Print( lx,ly, (unsigned char*)buffer );
	Bdisp_PutDisp_DD_DrawBusy_through(SRC);
}
//-----------------------------------------------------------------------------
int RangeErrorCK( char *SRC ) {
	if ( UseGraphic == 0 ) {
		if ( ScreenMode == 0 ) CB_SelectGraphVRAM();	// Select Graphic Screen
		Bdisp_AllClr_VRAM();
		GraphAxesGrid();
	}
	if ( ( Xdot == 0 ) || ( Ydot == 0 )  ) { CB_Error(RangeERR); PrevOpcode( SRC, &ExecPtr ); return ErrorNo; }	// Range error
	return 0;
}
void CB_TextOprand( char *SRC, int *py, int *px) {
	int x,y;
	*py=CB_EvalInt( SRC );
	if ( ( (*py)<0 ) || ( (*py)>63 ) ) { CB_Error(ArgumentERR); return; }  // Argument error
	if ( SRC[ExecPtr] != ',' ) { CB_Error(SyntaxERR); return; }  // Syntax error
	ExecPtr++;
	*px=CB_EvalInt( SRC );
	if ( ( (*px)<0 ) || ( (*px)>127 ) ) { CB_Error(ArgumentERR); return; }  // Argument error}
}
void CB_Text( char *SRC ) { //	Text
	unsigned int key;
	char buffer[128];
	int c;
	int px,py,d;
	double value;
	int mode;
	int maxoplen;
	int kanamini=1;

	if ( RangeErrorCK(SRC) ) return;
	CB_SelectGraphVRAM();	// Select Graphic Screen
	CB_TextOprand( SRC, &py, &px);
	c=SRC[ExecPtr];
	if ( c != ',' ) { CB_Error(SyntaxERR); return; }	// Syntax error
	c=SRC[++ExecPtr];
	if ( ( c == 0xFFFFFF87 )||( c == 0xFFFFFF99 ) ) { ExecPtr++; kanamini=0; }
	c=CB_IsStr( SRC, ExecPtr );
	if ( c ) {	// string
		CB_CurrentStr=CB_GetOpStr( SRC, &maxoplen ) ;		// String -> buffer	return 
		OpcodeStringToAsciiString( buffer, CB_CurrentStr, 127 );
	} else {	// expression
		d=(128-px)/4;
		if (d>24) d=24;	// digit max
		value = CB_EvalDbl( SRC );
		sprintGR(buffer, value, d,LEFT_ALIGN, CB_Round.MODE, CB_Round.DIGIT);
	}
	c=SRC[ExecPtr];
	if ( c != ',' )	mode=MINI_OVER;
	else {
		ExecPtr++;	
		c=SRC[ExecPtr++];
		if ( c == 'N' ) mode=MINI_OVER;	// Normal
		else
		if ( c == 'R' ) mode=MINI_REV;	// Reverse
		else
		if ( c == 'O' ) mode=MINI_OR;	// Or
		else
		if ( c == 'V' ) mode=MINI_REVOR;	// reVerse or
		else { ExecPtr--; CB_Error(SyntaxERR); return; }  // Syntax error
	}
	if ( kanamini ) CB_PrintMini( px, py, (unsigned char*)buffer, mode);
		else 	       PrintMini( px, py, (unsigned char*)buffer, mode);
	Bdisp_PutDisp_DD_DrawBusy_through(SRC);
}
//-----------------------------------------------------------------------------
void CB_LocateYX( char *SRC ){
	char buffer[64];
	int c;
	int px,py,d;
	double value;
	int mode;
	int maxoplen;

	if ( RangeErrorCK(SRC) ) return;
	CB_SelectGraphVRAM();	// Select Graphic Screen
	CB_TextOprand( SRC, &py, &px);
	c=SRC[ExecPtr];
	if ( c != ',' ) { CB_Error(SyntaxERR); return; }	// Syntax error
	ExecPtr++;
	c=CB_IsStr( SRC, ExecPtr );
	if ( c ) {	// string
		CB_CurrentStr=CB_GetOpStr( SRC, &maxoplen ) ;		// String -> buffer	return 
		OpcodeStringToAsciiString( buffer, CB_CurrentStr, 63 );
	} else {	// expression
		d=(128-px)/6;
		if (d>21) d=21;	// digit max
		value = CB_EvalDbl( SRC );
		sprintGR(buffer, value, d,LEFT_ALIGN, CB_Round.MODE, CB_Round.DIGIT);
	}
	CB_PrintXY(  px,py, (unsigned char*)buffer, CB_LocateMode(SRC));	
	Bdisp_PutDisp_DD_DrawBusy_through(SRC);
}
//----------------------------------------------------------------------------------------------

void CB_ViewWindow( char *SRC ) { //	ViewWindow
	int c;
	int reg=0;
	while ( reg <= 10 ) {
		c=SRC[ExecPtr];
		if ( (c==':') || (c==0x0C) || (c==0x0D) || (c==0x00) ) break;
		REGv[reg]=CB_EvalDbl( SRC );
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

int CB_SetPointMode( char *SRC) {
	int c=SRC[ExecPtr];
	if ( c != ',' ) return 1;
	ExecPtr++;	
	c=SRC[ExecPtr++];
	if ( c == 'C' ) return 0;
	else
	if ( c == 'X' ) return 2;
	else
	if ( c == 'P' ) return 1;
	ExecPtr--; CB_Error(SyntaxERR); return;   // Syntax error
}

void CB_FLine( char *SRC) { //	F-Line
	int c;
	double x1,y1,x2,y2;
	int style=S_L_Style;

	if ( RangeErrorCK(SRC) ) return;
	x1=CB_EvalDbl( SRC );
	c=SRC[ExecPtr];
	if ( c != ',' ) { CB_Error(SyntaxERR); return; }  // Syntax error
	ExecPtr++;
	y1=CB_EvalDbl( SRC );
	c=SRC[ExecPtr];
	if ( c != ',' ) { CB_Error(SyntaxERR); return; }  // Syntax error
	ExecPtr++;
	x2=CB_EvalDbl( SRC );
	c=SRC[ExecPtr];
	if ( c != ',' ) { CB_Error(SyntaxERR); return; }  // Syntax error
	ExecPtr++;
	y2=CB_EvalDbl( SRC );

	if ( tmp_Style >= 0 ) style=tmp_Style;
	CB_SelectGraphVRAM();	// Select Graphic Screen
	F_Line(x1, y1, x2, y2, style, CB_SetPointMode(SRC) );
	Bdisp_PutDisp_DD_DrawBusy_skip_through(SRC);
	tmp_Style = -1;
}

void CB_Line( char *SRC ) { //	Line
	int style=S_L_Style;
	if ( tmp_Style >= 0 ) style=tmp_Style;
	if ( RangeErrorCK(SRC) ) return;
	CB_SelectGraphVRAM();	// Select Graphic Screen
	Line( style, CB_SetPointMode(SRC) , 1 );	// error check
	Bdisp_PutDisp_DD_DrawBusy_skip_through(SRC);
	tmp_Style = -1;
}

void CB_Vertical( char *SRC ) { //	Vertical
	double x;
	int style=S_L_Style;
	if ( tmp_Style >= 0 ) style=tmp_Style;
	if ( RangeErrorCK(SRC) ) return;
	x=CB_EvalDbl( SRC );
	CB_SelectGraphVRAM();	// Select Graphic Screen
	Vertical(x, style, CB_SetPointMode(SRC) );
	Bdisp_PutDisp_DD_DrawBusy_skip_through(SRC);
	tmp_Style = -1;
}
void CB_Horizontal( char *SRC ) { //	Horizontal
	double y;
	int style=S_L_Style;
	if ( tmp_Style >= 0 ) style=tmp_Style;
	if ( RangeErrorCK(SRC) ) return;
	y=CB_EvalDbl( SRC );
	CB_SelectGraphVRAM();	// Select Graphic Screen
	Horizontal(y, style, CB_SetPointMode(SRC) );
	Bdisp_PutDisp_DD_DrawBusy_skip_through(SRC);
	tmp_Style = -1;
}

void CB_Plot( char *SRC ) { //	Plot
	int c;
	double x,y;
	
	if ( RangeErrorCK(SRC) ) return;
	
	CB_SelectGraphVRAM();	// Select Graphic Screen
	c=SRC[ExecPtr];
	if ( ( c==':' ) || (c==0x0D) || ( c==0x0C ) || (c==0x00) ) {
		x=(Xmax+Xmin)/2;
		y=(Ymax+Ymin)/2;
	} else {
		x=CB_EvalDbl( SRC );
		c=SRC[ExecPtr];
		if ( c != ',' ) { CB_Error(SyntaxERR); return; }  // Syntax error
		ExecPtr++;
		y=CB_EvalDbl( SRC );
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

void CB_PlotOprand( char *SRC, double  *x, double *y) {
	*x=CB_EvalDbl( SRC );
	if ( SRC[ExecPtr] != ',' ) { CB_Error(SyntaxERR); return; }  // Syntax error
	ExecPtr++;
	*y=CB_EvalDbl( SRC );
	CB_SelectGraphVRAM();	// Select Graphic Screen
}

void CB_PlotOn( char *SRC ) { //	PlotOn
	double x,y;
	if ( RangeErrorCK(SRC) ) return;
	CB_PlotOprand( SRC, &x, &y);
	PlotOn_VRAM(x,y);
	Bdisp_PutDisp_DD_DrawBusy_skip_through(SRC);
}
void CB_PlotOff( char *SRC ) { //	PlotOff
	double x,y;
	if ( RangeErrorCK(SRC) ) return;
	CB_PlotOprand( SRC, &x, &y);
	PlotOff_VRAM(x,y);
	Bdisp_PutDisp_DD_DrawBusy_skip_through(SRC);
}
void CB_PlotChg( char *SRC ) { //	PlotChg
	double x,y;
	if ( RangeErrorCK(SRC) ) return;
	CB_PlotOprand( SRC, &x, &y);
	PlotChg_VRAM(x,y);
	Bdisp_PutDisp_DD_DrawBusy_skip_through(SRC);
}

//----------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------
int ObjectAlign4p( unsigned int n ){ return n; }	// align +4byte
//----------------------------------------------------------------------------------------------
void CB_PxlOprand( char *SRC, int *py, int *px) {
	double x,y;
	y = (EvalsubTop( SRC ));
	*py=y;
	if ( ( y-floor(y) ) || ( (*py)<MatBase ) || ( (*py)>63 ) ) { CB_Error(ArgumentERR); return; }  // Argument error
	if ( SRC[ExecPtr] != ',' ) { CB_Error(SyntaxERR); return; }  // Syntax error
	ExecPtr++;
	x = (EvalsubTop( SRC ));
	*px=x;
	if ( ( x-floor(x) ) || ( (*px)<MatBase ) || ( (*px)>127 ) ) { CB_Error(ArgumentERR); return; }  // Argument error}
	CB_SelectGraphVRAM();	// Select Graphic Screen
}
void CB_PxlOn( char *SRC ) { //	PxlOn
	int px,py;
	if ( RangeErrorCK(SRC) ) return;
	CB_PxlOprand( SRC, &py, &px);
	PxlOn_VRAM(py,px);
//	BdispSetPointVRAM2(px, py, 1);
//	PXYtoVW(px, py, &regX, &regY);
	Bdisp_PutDisp_DD_DrawBusy_skip_through(SRC);
}
void CB_PxlOff( char *SRC ) { //	PxlOff
	int px,py;
	if ( RangeErrorCK(SRC) ) return;
	CB_PxlOprand( SRC, &py, &px);
	PxlOff_VRAM(py,px);
//	BdispSetPointVRAM2(px, py, 0);
//	PXYtoVW(px, py, &regX, &regY);
	Bdisp_PutDisp_DD_DrawBusy_skip_through(SRC);
}
void CB_PxlChg( char *SRC ) { //	PxlChg
	int px,py;
	if ( RangeErrorCK(SRC) ) return;
	CB_PxlOprand( SRC, &py, &px);
	PxlChg_VRAM(py,px);
//	BdispSetPointVRAM2(px, py, 2);
//	PXYtoVW(px, py, &regX, &regY);
	Bdisp_PutDisp_DD_DrawBusy_skip_through(SRC);
}

//----------------------------------------------------------------------------------------------

void CB_Circle( char *SRC ) { //	Circle
	int c,mode;
	double x,y,r;
	int style=S_L_Style;
	if ( tmp_Style >= 0 ) style=tmp_Style;
	if ( RangeErrorCK(SRC) ) return;
	CB_SelectGraphVRAM();	// Select Graphic Screen
	x=CB_EvalDbl( SRC );
	c=SRC[ExecPtr];
	if ( c != ',' ) { CB_Error(SyntaxERR); return; }  // Syntax error
	ExecPtr++;
	y=CB_EvalDbl( SRC );
	c=SRC[ExecPtr];
	if ( c != ',' ) { CB_Error(SyntaxERR); return; }  // Syntax error
	ExecPtr++;
	r=CB_EvalDbl( SRC );
	mode=CB_SetPointMode(SRC) ;
	c=SRC[ExecPtr];
	if ( c==':' ) 	Circle(x, y, r, style, 0, mode);
		else 		Circle(x, y, r, style, 1, mode);
	Bdisp_PutDisp_DD_DrawBusy_skip_through(SRC);
	tmp_Style = -1;
}

//----------------------------------------------------------------------------------------------
void CB_DrawGraph(  char *SRC ){
	if ( RangeErrorCK(SRC) ) return;
	CB_SelectGraphVRAM();	// Select Graphic Screen
	Graph_Draw();
}

void CB_GraphY( char *SRC ){
	if ( RangeErrorCK(SRC) ) return;
	CB_Str(SRC);
	GraphY=CB_CurrentStr;
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
	stat=SavePicture( (char *)pict, pictNo );
	if ( stat != 0 ) { CB_Error(MemoryERR); return; }	// Memory error
	return stat;
}

void RclPict( int pictNo){
	unsigned char *pict;
	int i;
	pict=(unsigned char *)LoadPicture( pictNo );
	if ( pict == NULL ) { CB_Error(MemoryERR); return; }	// Memory error
	WriteVram( pict+0x4C );
	free(pict);
}

void StoPictM( int pictNo){
	int i,stat;
	unsigned char *pict;
	
	if ( PictAry[pictNo] != NULL ) { // already exist
		pict = PictAry[pictNo] ;						// Matrix array ptr*
	} else {
		pict = (unsigned char *) malloc( 1024 );
		if( pict == NULL ) { CB_Error(NotEnoughMemoryERR); return; }	// Not enough memory error
		PictAry[pictNo] = pict ;						// Pict array ptr*
	}
	ReadVram(pict);
}

void RclPictM( int pictNo){
	unsigned char *pict;
	int i;
	if ( PictAry[pictNo] == NULL ) { CB_Error(MemoryERR); return; }	// Memory error
	pict=PictAry[pictNo];
	WriteVram( pict );
}

void CB_StoPict( char *SRC ) { //	StoPict
	int n;
	CB_SelectGraphVRAM();	// Select Graphic Screen
	n=CB_EvalInt( SRC );
	if ( (n<1) || (20<n) ){ CB_Error(ArgumentERR); return; }	// Argument error
	if ( PictMode ) StoPictM(n); else StoPict(n);
	Bdisp_PutDisp_DD_DrawBusy_through(SRC);
}
void CB_RclPict( char *SRC ) { //	RclPict
	int n;
	CB_SelectGraphVRAM();	// Select Graphic Screen
	n=CB_EvalInt( SRC );
	if ( (n<1) || (20<n) ){ CB_Error(ArgumentERR); return; }	// Argument error
	if ( PictMode ) RclPictM(n); else RclPict(n);
	Bdisp_PutDisp_DD_DrawBusy_through(SRC);
}


//----------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------
void CB_DotOprand( char *SRC, int *px, int *py) {
	int x,y;
	*px=CB_EvalInt( SRC );
	if ( ( (*px)<0 ) || ( (*px)>127 ) ) { CB_Error(ArgumentERR); }  // Argument error}
	if ( SRC[ExecPtr] != ',' ) { CB_Error(SyntaxERR); return; }  // Syntax error
	ExecPtr++;
	*py=CB_EvalInt( SRC );
	if ( ( (*py)<0 ) || ( (*py)>63 ) ) { CB_Error(ArgumentERR); }  // Argument error
}

//----------------------------------------------------------------------------------------------

void CB_ReadGraph( char *SRC ){	// ReadGraph(px1,py1, px2,py2)->Mat C
	int c;
	int dx,dy,sx;
	int px1,py1,px2,py2;
	int reg,i;
	int dimA,dimB;
	unsigned char *ptr;
	int ElementSize;
	int scrmode=ScreenMode;
	int base;
	DISPBOX box;
	
	if ( RangeErrorCK(SRC) ) return;
	CB_SelectGraphVRAM();	// Select Graphic Screen
	CB_DotOprand( SRC, &px1, &py1);
	if ( SRC[ExecPtr] != ',' ) { CB_Error(SyntaxERR); return; }  // Syntax error
	ExecPtr++;
	CB_DotOprand( SRC, &px2, &py2);
	if ( ErrorNo ) return ; // error
	if ( SRC[ExecPtr] == ')' ) ExecPtr++;
	if ( SRC[ExecPtr] == 0x0E ) {  // -> Mat C
		ExecPtr++;
		c =SRC[ExecPtr];
		if ( ( c != 0x7F ) || ( SRC[ExecPtr+1]!=0x40 ) ) { CB_Error(SyntaxERR); return; }	// Syntax error
		ExecPtr+=2;
		c=SRC[ExecPtr];
		if ( ( 'A' <= c ) && ( c <= 'z' ) ) {
			reg=c-'A';
			ExecPtr++;

			if (px1>px2) { i=px1; px1=px2; px2=i; }
			if (py1>py2) { i=py1; py1=py2; py2=i; }

			box.left  =px1;
			box.right =px2;
			box.top   =py1;
			box.bottom=py2;
			
			ElementSize=ElementSizeSelect( SRC, reg, &base );
			if ( ( px1 == 0 ) || ( py1 == 0 ) ) base=0;
			if ( ( ElementSize == 2 ) || ( ElementSize > 0x100 ) ) ElementSize=1;	// 1 bit matrix
			ElementSize &= 0xFF;
			dx= px2-px1;
			dy= py2-py1;
			switch ( ElementSize ) {
				case  1:
					dimA=dx+1;
					dimB=dy+1;
					break;
				case  8:
					dimA=dy+1;
					dimB=(dx/8)+1;
					break;
				case 16:
					dimA=dy+1;
					dimB=(dx/16)+1;
					break;
				case 32:
					dimA=dy+1;
					dimB=(dx/32)+1;
					break;
				case 64:
					dimA=dy+1;
					dimB=(dx/64)+1;
					break;
			}
			DimMatrixSub( reg, ElementSize, dimA, dimB, base ) ;
			if ( ErrorNo )  return ; 	// error
			
			ptr=(unsigned char*)MatAry[reg].Adrs;
			Bdisp_ReadArea_VRAM( &box, ptr);
		}
	} else { CB_Error(SyntaxERR); return; }	// Syntax error
	if ( scrmode==0 ) CB_SelectTextVRAM();	// Select Text Screen
}

void CB_WriteGraph( char *SRC ){	// WriteGraph x,y,wx,wy,Mat A ([2,2]),modify,kind
	int c;
	DISPGRAPH Gpict;
	int px,py,dx,dy;
	unsigned char Modify;
	unsigned char Kind=IMB_WRITEKIND_OVER;
	char*	MatAryC;
	short*	MatAryW;
	int*	MatAryI;
	double*	MatAryD;
	int reg,i,dimA;
	int x,y;
	unsigned char  *Gptr;
	int ElementSize;
	
	if ( RangeErrorCK(SRC) ) return;
	CB_SelectGraphVRAM();	// Select Graphic Screen
	CB_DotOprand( SRC, &px, &py);
	if ( SRC[ExecPtr] != ',' ) { CB_Error(SyntaxERR); return; }  // Syntax error
	ExecPtr++;
	CB_DotOprand( SRC, &dx, &dy);
	if ( ErrorNo ) return ; // error
	if ( SRC[ExecPtr] != ',' ) { CB_Error(SyntaxERR); return; }  // Syntax error
	ExecPtr++;
	MatrixOprand( SRC, &reg, &x, &y );
	if ( ErrorNo ) return ; // error
	ElementSize=MatAry[reg].ElementSize;
	Gptr=(unsigned char *)MatrixPtr( reg, x, y );

	if ( SRC[ExecPtr] != ',' ) { c='N'; } else { ExecPtr++; c=SRC[ExecPtr++]; }
	if ( c == 'N' ) Modify=IMB_WRITEMODIFY_NORMAL;	// Normal
	else
	if ( c == 'R' ) Modify=IMB_WRITEMODIFY_REVERCE;	// Reverse
	else
	if ( c == 'M' ) Modify=IMB_WRITEMODIFY_MESH;	// Mesh
	else { ExecPtr--; CB_Error(SyntaxERR); return; }  // Syntax error

	if ( SRC[ExecPtr] != ',' ) { c='N'; } else { ExecPtr++; c=SRC[ExecPtr++]; }
	if ( c == 'N' ) Kind=IMB_WRITEKIND_OVER;	// Normal
	else
	if ( c == 'O' ) Kind=IMB_WRITEKIND_OR;		// Or
	else
	if ( c == 'A' ) Kind=IMB_WRITEKIND_AND;		// And
	else
	if ( c == 'X' ) Kind=IMB_WRITEKIND_XOR;		// Xor
	else { ExecPtr--; CB_Error(SyntaxERR); return; }  // Syntax error

	Gpict.x =   px; 
	Gpict.y =   py; 
	Gpict.GraphData.width   = dx;
	Gpict.GraphData.height  = dy;
	Gpict.GraphData.pBitmap = Gptr;
	Gpict.WriteModify = Modify; 
	Gpict.WriteKind   = Kind;

	Bdisp_WriteGraph_VRAM(&Gpict);
	Bdisp_PutDisp_DD_DrawBusy_skip_through(SRC);
}

void CB_Rect( char *SRC ) { // Rect x1,y1,x2,y2,mode 
	int c;
	int px1,py1,px2,py2;
	int mode;
	int style=S_L_Style;
	if ( tmp_Style >= 0 ) style=tmp_Style;
	if ( RangeErrorCK(SRC) ) return;
	CB_SelectGraphVRAM();	// Select Graphic Screen
	CB_DotOprand( SRC, &px1, &py1);
	if ( SRC[ExecPtr] != ',' ) { CB_Error(SyntaxERR); return; }  // Syntax error
	ExecPtr++;
	CB_DotOprand( SRC, &px2, &py2);
	mode=CB_SetPointMode(SRC) ;
	if ( ErrorNo ) return ; // error

	if ( px1 > px2 ) { c=px1; px1=px2; px2=c; }
	if ( py1 > py2 ) { c=py1; py1=py2; py2=c; }
	
	Linesub(px1, py1, px2, py1, style, mode) ;	// upper line
	if ( py1 != py2 ) {
		Linesub(px1, py2, px2, py2, style, mode) ;	// lower line
		if ( py2-py1 >= 2 ) {
			Linesub(px1, py1+1, px1, py2-1, style, mode) ;	// left 
			Linesub(px2, py1+1, px2, py2-1, style, mode) ;	// right
		}
	}
	Bdisp_PutDisp_DD_DrawBusy_skip_through(SRC);
}

void CB_FillRect( char *SRC ) { // FillRect x1,y1,x2,y2,mode 
	int c;
	int px1,py1,px2,py2;
	int x,y;
	int mode;
	int style=S_L_Style;
	if ( tmp_Style >= 0 ) style=tmp_Style;
	if ( RangeErrorCK(SRC) ) return;
	CB_SelectGraphVRAM();	// Select Graphic Screen
	CB_DotOprand( SRC, &px1, &py1);
	if ( SRC[ExecPtr] != ',' ) { CB_Error(SyntaxERR); return; }  // Syntax error
	ExecPtr++;
	CB_DotOprand( SRC, &px2, &py2);
	mode=CB_SetPointMode(SRC) ;
	if ( ErrorNo ) return ; // error

	if ( px1 > px2 ) { c=px1; px1=px2; px2=c; }
	if ( py1 > py2 ) { c=py1; py1=py2; py2=c; }
	
	for ( y=py1 ; y<=py2 ; y++)	Linesub(px1, y, px2, y, style, mode) ;

	Bdisp_PutDisp_DD_DrawBusy_skip_through(SRC);
}

void CB_DotShape( char *SRC ) { // DotShape (x1,y1,x2,y2,typ,mode,pattern1,pattern2)
	TShape Shape;
	int c;
	int px1,py1,px2,py2;
	int typ;
	int mode,mode1,mode2;
	int pat1,pat2;
	
	if ( RangeErrorCK(SRC) ) return;
	CB_SelectGraphVRAM();	// Select Graphic Screen
	CB_DotOprand( SRC, &px1, &py1);
	if ( SRC[ExecPtr] != ',' ) { CB_Error(SyntaxERR); return; }  // Syntax error
	ExecPtr++;
	CB_DotOprand( SRC, &px2, &py2);
	if ( SRC[ExecPtr] != ',' ) { CB_Error(SyntaxERR); return; }  // Syntax error
	ExecPtr++;
	typ=CB_EvalInt( SRC );
	if ( SRC[ExecPtr] != ',' ) { CB_Error(SyntaxERR); return; }  // Syntax error
	ExecPtr++;
	mode1=CB_EvalInt( SRC );
	if ( SRC[ExecPtr] != ',' ) { CB_Error(SyntaxERR); return; }  // Syntax error
	ExecPtr++;
	mode2=CB_EvalInt( SRC );
	if ( SRC[ExecPtr] != ',' ) { CB_Error(SyntaxERR); return; }  // Syntax error
	ExecPtr++;
	pat1=CB_EvalInt( SRC );
	if ( SRC[ExecPtr] != ',' ) { CB_Error(SyntaxERR); return; }  // Syntax error
	ExecPtr++;
	pat2=CB_EvalInt( SRC );
	if ( SRC[ExecPtr] == ')' ) ExecPtr++;
/*
	if ( mode == 0 ) { mode1=2; mode2=1; }	// clear
	else
	if ( mode == 1 ) { mode1=1; mode2=1; }	// set
	else
	if ( mode == 2 ) { mode1=1; mode2=4; }	// invert
*/
	Shape.x1=px1;
	Shape.y1=py1;
	Shape.x2=px2;
	Shape.y2=py2;
	Shape.f[0]=2;
	Shape.f[1]=typ;
	Shape.f[2]=mode1;
	Shape.f[3]=mode2;
    Shape.on_bits=pat1;
    Shape.off_bits=pat2;
		
	Bdisp_ShapeToVRAM( &Shape );
	Bdisp_PutDisp_DD_DrawBusy_skip_through(SRC);
}


void CB_DotGet( char *SRC ){	// DotGet(px1,py1, px2,py2)->Mat B [x,y]
	int c;
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
	int scrmode=ScreenMode;
	
	if ( RangeErrorCK(SRC) ) return;
	CB_SelectGraphVRAM();	// Select Graphic Screen
	CB_DotOprand( SRC, &px1, &py1);
	if ( SRC[ExecPtr] != ',' ) { CB_Error(SyntaxERR); return; }  // Syntax error
	ExecPtr++;
	CB_DotOprand( SRC, &px2, &py2);
	if ( ErrorNo ) return ; // error
	if ( SRC[ExecPtr] == ')' ) ExecPtr++;
	if ( SRC[ExecPtr] == 0x0E ) {  // -> Mat C
			ExecPtr++;
			MatrixOprand( SRC, &reg, &x, &y );
			if ( ErrorNo ) return ; // error

			dimA=MatAry[reg].SizeA;
			dimB=MatAry[reg].SizeB;
			if ( dimA < x ) return;
			if ( dimB < y ) return;
			if (px1>px2) { i=px1; px1=px2; px2=i; }
			if (py1>py2) { i=py1; py1=py2; py2=i; }
			if ( dimA-x < px2-px1 ) px2=px1+dimA-x;
			if ( dimB-y < py2-py1 ) py2=py1+dimB-y;

			i=x;
			for ( py=py1; py<=py2 ; py++) {
				x=i;
				for ( px=px1; px<=px2 ; px++) WriteMatrixInt( reg, x++, y, Bdisp_GetPoint_VRAM(px,py) );
				y++;
			}
	} else { CB_Error(SyntaxERR); return; }	// Syntax error
	if ( scrmode==0 ) CB_SelectTextVRAM();	// Select Text Screen
}

void CB_DotPut( char *SRC ){	// DotPut(Mat B[x,y], px1,py1, px2,py2)
	int c;
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
	int mode;
	
	if ( RangeErrorCK(SRC) ) return;
	MatrixOprand( SRC, &reg, &x, &y );
	if ( ErrorNo ) return ; // error
	
	if ( SRC[ExecPtr] != ',' ) { CB_Error(SyntaxERR); return; }  // Syntax error
	ExecPtr++;
	CB_DotOprand( SRC, &px1, &py1);

	if ( SRC[ExecPtr] != ',' ) { CB_Error(SyntaxERR); return; }  // Syntax error
	ExecPtr++;
	CB_DotOprand( SRC, &px2, &py2);
	if ( ErrorNo ) return ; // error		
	if ( SRC[ExecPtr] == ')' ) ExecPtr++;

	dimA=MatAry[reg].SizeA;
	dimB=MatAry[reg].SizeB;
	if (px1>px2) { i=px1; px1=px2; px2=i; }
	if (py1>py2) { i=py1; py1=py2; py2=i; }
	if ( dimA < px2-px1 ) px2=px1+dimA;
	if ( dimB < py2-py1 ) py2=py1+dimB;
		
	mptr=0;
	
	if ( SRC[ExecPtr] == 0x0E ) {  // -> Mat C
			ExecPtr++;
			MatrixOprand( SRC, &reg2, &dimA, &dimB );
			if ( ErrorNo ) return ; // error
			if ( MatAry[reg2].SizeA == 0 ) { CB_Error(NoMatrixArrayERR); return; }	// No Matrix Array error
			
			dimA=MatAry[reg2].SizeA;
			dimB=MatAry[reg2].SizeB;
			if ( dimA+1 < px1 ) return;
			if ( dimB+1 < py1 ) return;
			if ( dimA+1 < px2 ) px2=dimA;
			if ( dimB+1 < py2 ) py2=dimB;
			if ( dimA < px2-px1 ) px2=px1+dimA;
			if ( dimB < py2-py1 ) py2=py1+dimB;

			i=x;
			for ( py=py1; py<=py2 ; py++) {	
				x=i;
				for ( px=px1; px<=px2 ; px++) WriteMatrixInt( reg2, px, py, ReadMatrixInt( reg, x++, y));
				y++;
			}
			dspflag=0;
	} else {	//			-> VRAM
		CB_SelectGraphVRAM();	// Select Graphic Screen
		mode=CB_SetPointMode(SRC);
		i=x;
		for ( py=py1; py<=py2 ; py++) {	
			x=i;
			for ( px=px1; px<=px2 ; px++) 
				if ( ReadMatrixInt( reg, x++, y ) ) BdispSetPointVRAM2(px, py, mode);
					else							BdispSetPointVRAM2(px, py, 0);
			y++;
		}
		
		Bdisp_PutDisp_DD_DrawBusy_skip_through(SRC);
		dspflag=0;
		UseGraphic=99;
	}
}

void CB_DotTrim( char *SRC ){	// DotTrim(Mat A,x1,y1,x2,y2)->Mat B    =>[X,Y]
	int c;
	int x,y, px,py;
	int x1,y1,x2,y2;
	int px1,py1,px2,py2;
	int startx=0,starty=0,endx=0,endy=0;
	double	*Matary2;
	char	*MatAryC,*MatAryC2;
	short	*MatAryW,*MatAryW2;
	int		*MatAryI,*MatAryI2;
	int reg=-1,reg2,i;
	int dimA,dimB;
	int mptr,mptr2;
	int value;
	int ElementSize;
	int scrmode=ScreenMode;
	
	c =SRC[ExecPtr];
	if ( ( c == 0x7F ) && ( SRC[ExecPtr+1]==0x40 ) ) {	// Mat A
		ExecPtr+=2;
		c=SRC[ExecPtr];
		if ( ( 'A' > c ) || ( c > 'z' ) ) { CB_Error(SyntaxERR); return; }	// Syntax error
		reg=c-'A';
		if ( MatAry[reg].SizeA == 0 ) { CB_Error(NoMatrixArrayERR); return; }	// No Matrix Array error
		ExecPtr++;
		c=SRC[ExecPtr];
		if ( c != ',' ) { CB_Error(SyntaxERR); return; }  // Syntax error
		ExecPtr++;
		px1=CB_EvalInt( SRC );
		if ( ( px1 < 1 ) || ( MatAry[reg].SizeA < px1 ) ) CB_Error(RangeERR) ; // Range error 
		if ( SRC[ExecPtr] != ',' ) { CB_Error(SyntaxERR); return; }  // Syntax error
		ExecPtr++;
		py1=CB_EvalInt( SRC );
		if ( ( py1 < 1 ) || ( MatAry[reg].SizeB < py1 ) ) CB_Error(RangeERR) ; // Range error 
		
		if ( SRC[ExecPtr] != ',' ) { CB_Error(SyntaxERR); return; }  // Syntax error
		ExecPtr++;
		CB_DotOprand( SRC, &px2, &py2);
		if ( ErrorNo ) return ; // error

		if ( SRC[ExecPtr] == ')' ) ExecPtr++;

		dimA=MatAry[reg].SizeA;
		dimB=MatAry[reg].SizeB;
		if (px1>px2) { i=px1; px1=px2; px2=i; }
		if (py1>py2) { i=py1; py1=py2; py2=i; }
		if ( dimA < px2-px1 ) px2=px1+dimA;
		if ( dimB < py2-py1 ) py2=py1+dimB;
		startx=px2; endx=px1;
		starty=py2; endy=py1;

		i=MatAry[reg].Base;
		y=i;
		for ( py=py1; py<=py2 ; py++) {	
			x=i;
			for ( px=px1; px<=px2 ; px++) 
				if ( ReadMatrixInt( reg, x++, y ) ){
					if ( py<starty ) starty=py;
					if ( py>endy   ) endy  =py;
					if ( px<startx ) startx=px;
					if ( px>endx   ) endx  =px;
				}
			y++;
		}

	} else {
			if ( RangeErrorCK(SRC) ) return;
			CB_SelectGraphVRAM();	// Select Graphic Screen
			CB_DotOprand( SRC, &px1, &py1);
			if ( SRC[ExecPtr] != ',' ) { CB_Error(SyntaxERR); return; }  // Syntax error
			ExecPtr++;
			CB_DotOprand( SRC, &px2, &py2);
			if ( SRC[ExecPtr] == ')' ) ExecPtr++;
			if ( ErrorNo ) return ; // error

			if (px1>px2) { i=px1; x1=px2; px2=i; }
			if (py1>py2) { i=py1; y1=py2; py2=i; }
			startx=px2; endx=px1;
			starty=py2; endy=py1;
				
			for ( py=py1+1; py<=py2+1 ; py++) {
				for ( px=px1+1; px<=px2+1 ; px++) {
					if ( Bdisp_GetPoint_VRAM(px,py) ) {
						if ( py<starty ) starty=py;
						if ( py>endy   ) endy  =py;
						if ( px<startx ) startx=px;
						if ( px>endx   ) endx  =px;
					}
				}
			}
	}

	if ( SRC[ExecPtr] != 0x0E ) { CB_Error(SyntaxERR); return; }	// Syntax error
	ExecPtr++;

	if ( ( startx > endx ) || ( starty > endy ) ) { dimA=0; dimB=0; startx=0; starty=0; goto dottret; }
	dimA=endx-startx+1;
	dimB=endy-starty+1;
	CB_MatrixInitsub( SRC, &reg2, dimA, dimB, 0 );
	if ( ErrorNo )  return ; 	// error

	px1=startx;
	py1=starty;
	px2=endx;
	py2=endy;
	x=MatAry[reg2].Base;
	y=x;

	if ( reg >= 0 ) {
		i=x;
		for ( py=py1; py<=py2 ; py++) {	
			x=i;
			for ( px=px1  ; px<=px2  ; px++) WriteMatrixInt( reg2, x++, y, ReadMatrixInt( reg, px, py ) );
			y++;
		}
	} else {
		i=x;
		for ( py=py1+1; py<=py2+1 ; py++) {
			x=i;
			for ( px=px1+1; px<=px2+1; px++) WriteMatrixInt( reg2, x++, y, Bdisp_GetPoint_VRAM(px, py ) );
			y++;
		}
	}

	startx++; starty++;
	dottret:
	LocalInt[23][0]=startx;	// regint x
	LocalInt[24][0]=starty;	// regint y
	regintX=dimA;			// regint X
	regintY=dimB;			// regint Y
	if ( CB_INT == 0 ) { 
		LocalDbl[23][0]=startx;	// reg x
		LocalDbl[24][0]=starty;	// reg y
		regX=dimA;			// reg X
		regY=dimB;			// reg Y
	}
	if ( scrmode==0 ) CB_SelectTextVRAM();	// Select Text Screen
}



void CB_DotLife( char *SRC ){	// DotLife(Mat A,x1,y1,x2,y2)->Mat B    =>[X,Y]
	int c;
	int x,y, px,py;
	int x1,y1,x2,y2;
	int px1,py1,px2,py2;
	int startx=0,starty=0,endx=0,endy=0;
	double	*Matary2;
	char	*MatAryC,*MatAryC2;
	short	*MatAryW,*MatAryW2;
	int		*MatAryI,*MatAryI2;
	int reg=-1,reg2,i;
	int dimA,dimB;
	int mptr,mptr2;
	int value;
	int ElementSize;
	int scrmode=ScreenMode;
	
	MatrixOprand( SRC, &reg, &dimA, &dimB );
	if ( ErrorNo ) return ; // error
	if ( MatAry[reg].SizeA == 0 ) { CB_Error(NoMatrixArrayERR); return; }	// No Matrix Array error

	if ( SRC[ExecPtr] != ',' ) { CB_Error(SyntaxERR); return; }  // Syntax error
	ExecPtr++;
	CB_DotOprand( SRC, &px1, &py1);
	if ( SRC[ExecPtr] != ',' ) { CB_Error(SyntaxERR); return; }  // Syntax error
	ExecPtr++;
	CB_DotOprand( SRC, &px2, &py2);
	if ( SRC[ExecPtr] == ')' ) ExecPtr++;
	if ( ErrorNo ) return ; // error

	dimA=MatAry[reg].SizeA;
	dimB=MatAry[reg].SizeB;
	if (px1>px2) { i=px1; px1=px2; px2=i; }
	if (py1>py2) { i=py1; py1=py2; py2=i; }
	if ( dimA < px2 ) px2=dimA;
	if ( dimB < py2 ) py2=dimB;

	if ( SRC[ExecPtr] != 0x0E ) { CB_Error(SyntaxERR); return; }	// Syntax error
	ExecPtr++;
	MatrixOprand( SRC, &reg2, &dimA, &dimB );
	if ( ErrorNo ) return ; // error
	if ( MatAry[reg2].SizeA == 0 ) { CB_Error(NoMatrixArrayERR); return; }	// No Matrix Array error

	for ( y=py1; y<=py2; y++) {
		for ( x=px1; x<=px2; x++) {
			c=Cellsum( reg, x, y );
			if ( ReadMatrixInt( reg, x, y ) ) {
				if ( (c<=1)||(c>=4) ) WriteMatrixInt(reg2, x, y, 0);
			} else {
				if ( c==3 ) WriteMatrixInt(reg2, x, y, 1);
			}
			if ( c ) {
				if (regintL<=2) regintL=2;
				else if (x<=regintL) regintL--;
				if (regintR>=126) regintR=126;
				else if (x>=regintR) regintR++;
				if (regintT<=2) regintT=2;
				else if (y<=regintT) regintT--;
				if (regintB>=62) regintB=62;
				else if (y>=regintB) regintB++;
			}
		}
	}
}


