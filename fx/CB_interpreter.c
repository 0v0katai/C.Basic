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
#include "CB_MonochromeLib.h"

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

char PxlMode=1;		// Pxl  1:set  0:clear

char BreakCheck=1;	// Break Stop on/off
char ACBreak=1;		// AC Break on/off

char TimeDsp=0;		// Execution Time Display  0:off 1:on
char MatXYmode=0;		// 0: normal  1:reverse
char PictMode=1;	// StoPict/RclPict  StrageMem:0  heap:1
char CheckIfEnd=0;	// If...IfEnd check  0:off  1:on

char CommandInputMethod=1;	//	0:C.Basic  1:Genuine

char  RefreshCtrl=1;	// 0:no refresh   1: GrphicsCMD refresh     2: all refresh
char  Refreshtime=2;	// Refresh time  (Refreshtime+1)/128s

short DefaultWaitcount=0;	// wait control
short Waitcount=0;	// current wait control

double Previous_X=1e308 ;	// Plot Previous X
double Previous_Y=1e308 ;	// Plot Previous Y
int    Previous_PX=-1   ;	// Plot Previous PX
int    Previous_PY=-1   ;	// Plot Previous PY
double Plot_X    =1e308 ;	// Plot Current X
double Plot_Y    =1e308 ;	// Plot Current Y

//-----------------------------------------------------------------------------
// Casio Basic Gloval variable
//-----------------------------------------------------------------------------
double  REG[26+6+26];
double  REGv[11];
int  	REGINT[26+6+26];

double	Xfct     =  2;					// 
double	Yfct     =  2;					// 

short 	Argc=0;
double 	LocalDbltmp[ArgcMAX];
int		LocalInttmp[ArgcMAX];

double 	*LocalDbl[26+6+26];
int		*LocalInt[26+6+26];

double	*traceAry;		// Graph array X

char *GraphY;
char *GraphX;

unsigned char *PictAry[PictMax+1];		// Pict array ptr
char BG_Pict_No=0;

unsigned char *Pictbase[PictbaseMAX];
short PictbasePtr;
short PictbaseCount;

//----------------------------------------------------------------------------------------------
//		Interpreter inside
//----------------------------------------------------------------------------------------------
int	CB_TicksStart;
int	CB_TicksEnd;
int	CB_TicksAdjust;

int CB_INT=0;		// current mode  0:normal  1: integer mode
int ExecPtr=0;		// Basic execute ptr
int BreakPtr=0;		// Basic break ptr

int CBint_CurrentValue;	// 
double CB_CurrentValue;	//

char ScreenMode;	//  0:Text  1:Graphic
char UseGraphic=0;	// use Graph  ( no use :0    plot:1   graph:2   clrgraph:3  other:9
char dspflag=0;		// 0:nondsp  1:str  2:num  3:mat 4:list
char MatdspNo=0;		// 

int CursorX=1;	// text cursor X
int CursorY=1;	// text cursor X

char GosubNestN=0;		// Gosub Nesting lebel (for subroutin)
char ProgEntryN=0;		// Basic Program ptr (for subroutin)
char ProgNo=0;			// current Prog No
char *ProgfileAdrs[ProgMax+1];
int   ProgfileMax[ProgMax+1] ;	// Max edit filesize 
char  ProgfileEdit[ProgMax+1];	// no change : 0     edited : 1
char  ProgfileMode[ProgMax+1];	// g1m : 0    text : 1
char  ProgLocalN[ProgMax+1];
char  ProgLocalVar[ProgMax+1][26];

char *TVRAM;
char *GVRAM;
//----------------------------------------------------------------------------------------------
short StackGotoAdrs[StackGotoMax];

CchIf	CacheIf;
CchIf	CacheElse;
CchIf	CacheElseIf;
CchIf	CacheSwitch;
CchRem	CacheRem;

void ClrCahche();

double  VWIN[6][11];			// 
char VWinflag[6];				// VWin flag

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
	int excptr;

	short StackGosubAdrs[StackGosubMax];
	
	CurrentStk	CurrentStruct;
	StkFor		StackFor;
	StkWhileDo	StackWhileDo;
	StkSwitch	StackSwitch;
	
	double	localvarDbl[ArgcMAX];	//	local var
	int		localvarInt[ArgcMAX];	//	local var

	if ( 0x88020200 > (int)&cont ) { CB_Error(StackERR); return -1; } //  stack error
	
	for (c=0; c<StackGotoMax; c++) StackGotoAdrs[c]=0;	// init goto
	
	ClrCahche();
	
	tmp_Style = -1;
	dspflag=0;

	InitLocalVar();		// init Local variable
	
	for ( c=0; c<ProgLocalN[ProgNo]; c++ ) {		// set local variable
		j=ProgLocalVar[ProgNo][c];
		if ( j>=0 ) { 
			LocalDbl[j]=&localvarDbl[c]; LocalDbl[j][0]=LocalDbltmp[c];
			LocalInt[j]=&localvarInt[c]; LocalInt[j][0]=LocalInttmp[c];
		}
	}
	
	CurrentStruct.CNT=0;
	StackFor.Ptr=0;
	StackWhileDo.WhilePtr=0;
	StackWhileDo.DoPtr=0;
	StackSwitch.Ptr=0;
	
	while (cont) {
		dspflagtmp=0;
		CB_StrBufferCNT=0;			// Quot String buffer clear
		if ( ErrorNo || BreakPtr ) { 
			if ( BreakPtr == -8 ) goto iend;
			if ( CB_BreakStop() ) return -7 ;
			if ( SRC[ExecPtr] == 0x0C ) if ( ErrorNo==0 ) ExecPtr++; // disps
			ClrCahche();
		}
		c=SRC[ExecPtr++];
		if ( ( c==':'  ) || ( c==0x0D ) ) {
				c=SRC[ExecPtr++];
				while ( c==0x0D ) c=SRC[ExecPtr++];
				if ( BreakCheck )if ( KeyScanDownAC() ) { KeyRecover(); if ( BreakPtr == 0 ) BreakPtr=ExecPtr-1; }	// [AC] break?
		}
		if ( c==0x00 ) { ExecPtr--;
			if ( ProgEntryN ) return -1;
			else  break;
		}
		
		while ( c==0x20 ) c=SRC[ExecPtr++]; // Skip Space
		
		switch (c) {
			case 0xFFFFFFEC:	// Goto
				CB_Goto(SRC, StackGotoAdrs );
				break;
			case 0xFFFFFFE8:	// Dsz
				CB_Dsz(SRC) ;
				break;
			case 0xFFFFFFE9:	// Isz
				CB_Isz(SRC) ;
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
					case 0x07:	// Next
						CB_Next(SRC, &StackFor, &CurrentStruct );
						break;
					case 0x09:	// WhileEnd
						CB_WhileEnd(SRC, &StackWhileDo, &CurrentStruct );
						break;
					case 0x0B:	// LpWhile
						CB_LpWhile(SRC, &StackWhileDo, &CurrentStruct );
						break;
					case 0x04:	// For
						CB_For(SRC, &StackFor, &CurrentStruct );
//						ClrCahche();
						break;
					case 0x08:	// While
						CB_While(SRC, &StackWhileDo, &CurrentStruct );
//						ClrCahche();
						break;
					case 0x0A:	// Do
						CB_Do(SRC, &StackWhileDo, &CurrentStruct );
//						ClrCahche();
						break;
					case 0xFFFFFFEA:	// Switch
						CB_Switch(SRC, &StackSwitch, &CurrentStruct, &CacheSwitch );
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
						c=SRC[ExecPtr];
						if ( (c!=0)&&(c!=0x0C)&&(c!=0x0D)&&(c!=':') ) {
							if (CB_INT)	CBint_CurrentValue = EvalIntsubTop( SRC );
							else		CB_CurrentValue    = EvalsubTop( SRC );
						}
						if ( GosubNestN > 0 ) { 
							ExecPtr=StackGosubAdrs[--GosubNestN] ; break; } //	 return from subroutin 
						if ( ProgEntryN ) { return -2 ; }	//	return from  sub Prog
						cont=0;
						break;
					case 0x10:	// Locate
						CB_Locate(SRC);
						dspflag=1;
						break;
					case 0xFFFFFFE4:	// Disp
						CB_Disp(SRC);
						dspflag=1;
						break;
					case 0xFFFFFFA5:	// Text
						dspflag=0;
						CB_Text(SRC);
						UseGraphic=9;
						break;
					case 0xFFFFFFAB:	// PxlOn
						if (CB_INT)	CBint_PxlSub(SRC,1); else CB_PxlSub(SRC,1);
						dspflag=0;
						UseGraphic=9;
						break;
					case 0xFFFFFFAC:	// PxlOff
						if (CB_INT)	CBint_PxlSub(SRC,0); else CB_PxlSub(SRC,0);
						dspflag=0;
						UseGraphic=9;
						break;
					case 0xFFFFFFAD:	// PxlChg
						if (CB_INT)	CBint_PxlSub(SRC,2); else CB_PxlSub(SRC,2);
						dspflag=0;
						UseGraphic=9;
						break;
					case 0xFFFFFFA8:	// PlotOn
						CB_PlotSub(SRC,1);
						dspflag=0;
						UseGraphic=9;
						break;
					case 0xFFFFFFA9:	// PlotOff
						CB_PlotSub(SRC,0);
						dspflag=0;
						UseGraphic=9;
						break;
					case 0xFFFFFFAA:	// PlotChg
						CB_PlotSub(SRC,2);
						dspflag=0;
						UseGraphic=9;
						break;
					case 0xFFFFFFA7:	// F-Line
						CB_FLine(SRC);
						dspflag=0;
						UseGraphic=9;
						break;
					case 0xFFFFFFA3:	// Vertical
						CB_Vertical(SRC);
						dspflag=0;
						UseGraphic=9;
						break;
					case 0xFFFFFFA4:	// Horizontal
						CB_Horizontal(SRC);
						dspflag=0;
						UseGraphic=9;
						break;
					case 0xFFFFFFA6:	// Circle
						CB_Circle(SRC);
						dspflag=0;
						UseGraphic=9;
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
						break;
					case 0xFFFFFFE1:	// Rect
						CB_Rect(SRC);
						dspflag=0;
						UseGraphic=9;
						break;
					case 0xFFFFFFE2:	// FillRect
						CB_FillRect(SRC);
						dspflag=0;
						UseGraphic=9;
						break;
					case 0xFFFFFFE3:	// LocateYX
						CB_LocateYX(SRC);
						dspflag=0;
						UseGraphic=9;
						break;
					case 0xFFFFFFE9:	// WriteGraph
						CB_WriteGraph(SRC);
						dspflag=0;
						UseGraphic=9;
						break;
					case 0xFFFFFFE8:	// ReadGraph
						CB_ReadGraph(SRC);
						dspflag=0;
						UseGraphic=9;
						break;
					case 0xFFFFFFF0:	// DotShape(
						CB_DotShape(SRC);
						dspflag=0;
						UseGraphic=9;
						break;
					case 0xFFFFFF94:	// RclPict
						CB_RclPict(SRC);
						dspflag=0;
						UseGraphic=9;
						break;
					case 0xFFFFFF93:	// StoPict
						CB_StoPict(SRC);
						dspflag=0;
						UseGraphic=9;
						break;
					case 0xFFFFFF97:	// Sto-VWin
						CB_StoVWin(SRC);
						dspflag=0;
						break;
					case 0xFFFFFF98:	// Rcl-VWin
						CB_RclVWin(SRC);
						dspflag=0;
						break;
					case 0xFFFFFFFC:	// PutDispDD
						Bdisp_PutDisp_DD_DrawBusy();
						dspflag=0;
						break;
					case 0xFFFFFFFD:	// Fkeyfunc(
						CB_FkeyMenu( SRC );
						dspflag=0;
						break;
					case 0xFFFFFFFB:	// Screen
						CB_Screen(SRC);
						dspflag=0;
						break;
					case 0xFFFFFFF1:	// Local
						Skip_block(SRC);
						dspflag=0;
						break;
					case 0xFFFFFFF6:	// Poke
						CB_Poke(SRC);
						dspflag=0;
						break;
					case 0xFFFFFFF2:	// PopUpWin(
						CB_PopUpWin(SRC);
						dspflag=0;
						break;
					case 0x3D:			// DotTirm(
						CB_DotTrim(SRC);
						dspflag=0;
						break;
					case 0x3E:			// DotGet(
						CB_DotGet(SRC);
						dspflag=0;
						break;
					case 0xFFFFFFE0:	// DotLife(
						CB_DotLife(SRC);
						dspflag=0;
						break;
					case 0x3B:			// DotPut(Z,x,y)
						CB_DotPut(SRC);
						dspflag=0;
						UseGraphic=9;
						break;
					case 0xFFFFFFB0:	// SortA(
						CB_SortAD(SRC, 1);
						dspflag=0;
						break;
					case 0xFFFFFFB1:	// SortD(
						CB_SortAD(SRC, 0);
						dspflag=0;
						break;
					case 0x70:			// G-Connect
						DrawType=0;
						dspflag=0;
						break;
					case 0x71:			// G-Plot
						DrawType=1;
						dspflag=0;
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
					case 0xFFFFFFC5:	// DerivOn
						Derivative=1;
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
					case 0xFFFFFFD5:	// DerivOff
						Derivative=0;
						dspflag=0;
						break;
					case 0xFFFFFFD4:	// LabelOff
						Label=0;
					case 0xFFFFFFC0:	// FunOn
					case 0xFFFFFFD0:	// FunOFF
					case 0xFFFFFFD1:	// SimulOff
						dspflag=0;
						break;
					case 0x4A:			// S-Gph1
						CB_S_Gph(SRC, 0);
						dspflag=0;
						break;
					case 0x4B:			// S-Gph2
						CB_S_Gph(SRC, 1);
						dspflag=0;
						break;
					case 0x4C:			// S-Gph3
						CB_S_Gph(SRC, 2);
						dspflag=0;
						break;
					case 0x60:			// S-WindAuto
						CB_S_WindAuto(SRC);
						dspflag=0;
						break;
					case 0x61:			// S-WindMan
						CB_S_WindMan(SRC);
						dspflag=0;
						break;
					case 0x23:			// DrawStat
						CB_DrawStat(SRC);
						dspflag=0;
						UseGraphic=9;
						break;
					case 0x63:			// Y=Type
						dspflag=0;
						break;
					case 0x20:			// DrawGraph
						CB_DrawGraph(SRC);
						UseGraphic=2;
						dspflag=0;
						break;
					case 0xFFFFFFDF:	// Delete
						CB_Delete( SRC );
						dspflag=0;
						break;
					case 0xFFFFFFEE:	// Save
						CB_Save(SRC);
						dspflag=0;
						break;
					case 0xFFFFFFEF:	// Load(
						CB_Load(SRC);
						dspflag=0;
						break;
					case 0xFFFFFFF8:	// RefreshCtrl
						CB_RefreshCtrl(SRC);
						dspflag=0;
						break;
					case 0xFFFFFFFA:	// RefreshTime
						CB_RefreshTime(SRC);
						dspflag=0;
						break;
					case 0xFFFFFFFE:	// BackLight
						BackLight( CB_EvalInt( SRC ) );
						dspflag=0;
						break;
					case 0xFFFFFF9E:	// Menu
						CB_Menu( SRC, StackGotoAdrs );
						dspflag=0;
						break;
					case 0x78:	// BG-None
						CB_BG_None( SRC );
						dspflag=0;
						break;
					case 0x79:	// BG-Pict
						CB_BG_Pict( SRC );
						dspflag=0;
						break;
					case 0x1A:	// ClrList
						CB_ClrList(SRC);
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
						BreakPtr=-8;	// program end
						break;
					default:
						goto Evalexit2;
				}
				break;
				
			case 0x7F:	// 7F
				c=SRC[ExecPtr++];
				switch ( c ) {
					case 0x40:	// Mat
						dspflagtmp=CB_MatCalc(SRC);
						if ( dspflagtmp ) goto Evalexit2;
						break;
					case 0x51:	// List
						dspflagtmp=CB_ListCalc(SRC);
						if ( dspflagtmp ) goto Evalexit2;
						break;
					case 0x45 :				// Swap A,2,3
						CB_MatSwap( SRC );
						break;
					case 0x42 :				// *Row 5,A,2
						CB_MatxRow( SRC );
						break;
					case 0x43 :				// *Row+ 5,A,2,3
						CB_MatxRowPlus( SRC );
						break;
					case 0x44 :				// Row+ A,2,3
						CB_MatRowPlus( SRC );
						break;
					default:
						goto Evalexit2;
				}
				break;
				
			case 0xFFFFFFF9:	// F9
				c=SRC[ExecPtr++];
				if ( ( 0xFFFFFFC0 <= c ) && ( c <= 0xFFFFFFDF ) ) { CB_ML_command( SRC, c ); break; }
				else
				if ( ( 0x34 <= c ) && ( c <= 0x37 ) )  goto strjp;
				else
				if ( ( 0x39 <= c ) && ( c <= 0x43 ) )  goto strjp;
				switch ( c ) {
					case 0x30:	// StrJoin(
//					case 0x34:	// StrLeft(
//					case 0x35:	// StrRight(
//					case 0x36:	// StrMid(
//					case 0x37:	// Exp->Str(
//					case 0x39:	// StrUpr(
//					case 0x3A:	// StrLwr(
//					case 0x3B:	// StrInv(
//					case 0x3C:	// StrShift(
//					case 0x3D:	// StrRotate(
//					case 0x3F:	// Str
//					case 0x40:	// Str(
//					case 0x41:	// DATE
//					case 0x42:	// TIME
//					case 0x43:	// Sprintf(
					strjp:
						ExecPtr-=2;
						CB_Str(SRC) ;
//						dspflag=0;
						break;

					case 0x05:	// >DMS
						dspflag=1;
						CB_StrDMS(SRC);
						break;
					case 0x0F:	// AliasVar
						CB_AliasVar(SRC);
						dspflag=0;
						break;
					case 0x0B:	// EngOn
						ENG=1;
						if ( SRC[ExecPtr]=='3' ) { ExecPtr++; ENG=3; } // 3 digit separate
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
					case 0x4F:	// Wait
						CB_Wait(SRC);
						dspflag=0;
						break;
					default:
						Evalexit2:
						ExecPtr-=2;
						goto Evalexit;
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
				break;
			case 0xFFFFFFE0:	// Plot
				CB_Plot(SRC);
				dspflag=0;
				UseGraphic=1;
				break;
			case 0xFFFFFFE1:	// Line
				CB_Line(SRC);
				UseGraphic=9;
				dspflag=0;
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
				
			case 0xFFFFFFDD:	// Eng
				if ( ENG ) ENG=0; else ENG=1;
				dspflag=0;
				break;
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
				UseGraphic=3;
				break;
			case 0xFFFFFFEE:	// Graph Y=
				CB_GraphY(SRC);
				dspflag=0;
				UseGraphic=2;
				break;
			case 0xFFFFFFF5:	// Graph(X,Y)=(
				CB_GraphXY(SRC);
				dspflag=0;
				UseGraphic=9;
				break;
				
			case '$':	// $Mat
			case 0x22:	// " "
				ExecPtr--;
				CB_Str(SRC) ;
//				dspflag=1;
				break;
			case '?':	// ?
				CB_Input(SRC);
				CB_TicksStart=RTC_GetTicks();	// init
				dspflagtmp=2;
//				if ( BreakPtr > 0 ) break;
				c=SRC[ExecPtr++];
				if ( c == 0x0E ) {		// ->
					if (CB_INT)	CBint_Store(SRC); else CB_Store(SRC);
				}
				else ExecPtr--;
				break;
			case 0x0C:	// disps
				if ( CB_Disps(SRC, dspflag) ) BreakPtr=ExecPtr ;  // [AC] break
				CB_TicksStart=RTC_GetTicks();	// init
				c=SRC[ExecPtr]; while ( c==0x20 ) c=SRC[++ExecPtr]; // Skip Space
				break;
		
			default:
				ExecPtr--;
			  Evalexit:
				excptr=ExecPtr;
				dspflag=2;
				dspflagtmp=2;
				if (CB_INT)	{
					CBint_CurrentValue = EvalIntsubTop( SRC );
					if ( dspflag>=3 ) {
						ExecPtr=excptr;
						dspflagtmp=ListEvalIntsubTopAns(SRC);	// List calc	dspflag; //	2:nomal  3:mat  4:list
					}
				}
				else {
					CB_CurrentValue    = EvalsubTop( SRC );
					if ( dspflag>=3 ) {
						ExecPtr=excptr;
						dspflagtmp=ListEvalsubTopAns(SRC);	// List calc	dspflag; //	2:nomal  3:mat  4:list
					}
				}
				break;
		}
		c=SRC[ExecPtr];
		if ( c == 0x0E ) { 
			ExecPtr++;
			if (CB_INT)	CBint_Store(SRC);	// ->
			else		CB_Store(SRC);		// ->
			dspflagtmp=dspflag+0x10; //	12:nomal  13:mat  14:list
			c=SRC[ExecPtr];
		}
		if ( c == 0x13 ) {					// =>
			ExecPtr++;
			dspflagtmp=0;
			if (CB_INT)	{ if ( CBint_CurrentValue == 0 ) Skip_block(SRC); }		// false
			else 		{ if ( CB_CurrentValue    == 0 ) Skip_block(SRC); }		// false
		}
		if ( ( 0 < dspflagtmp ) && ( dspflagtmp < 0x10 ) ) {
			if (CB_INT)	regint_Ans=CBint_CurrentValue ;
			else		reg_Ans   =CB_CurrentValue    ;
			if ( dspflagtmp>=3 ) CopyMatList2AnsTop( CB_MatListAnsreg );	// MatListResult -> MatList Ans top
			
		}
		if ( dspflagtmp ) dspflag=dspflagtmp & 0x0F;
	}
	iend:
	CB_TicksEnd=RTC_GetTicks();	// 
	if ( ProgEntryN == 0 ) CB_end(SRC);
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

void InitLocalVar() {
	int c;
	for ( c=0; c<58; c++ ) {			// init Local variable
		LocalDbl[c]=&REG[c];
		LocalInt[c]=&REGINT[c];
	}
}

//----------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------
int ObjectAlign4d( unsigned int n ){ return n; }	// align +4byte
int ObjectAlign4f( unsigned int n ){ return n; }	// align +4byte
int ObjectAlign4g( unsigned int n ){ return n; }	// align +4byte
int ObjectAlign4h( unsigned int n ){ return n; }	// align +4byte
//int ObjectAlign4i( unsigned int n ){ return n; }	// align +4byte
//int ObjectAlign4j( unsigned int n ){ return n; }	// align +4byte
//int ObjectAlign4k( unsigned int n ){ return n; }	// align +4byte
//int ObjectAlign6e( unsigned int n ){ return n+n; }	// align +6byte
//----------------------------------------------------------------------------------------------

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

void Skip_quot( char *SRC ){ // skip "..."
	int c;
	c=SRC[ExecPtr-2];
	if ( ( c==0x27 ) || ( c==' ' ) || ( c==0x0D ) || ( c==':' ) ) {
		while (1){
			c=SRC[ExecPtr++];
			switch ( c ) {
				case 0x00:	// <EOF>
					ExecPtr--;
				case 0x22:	// "
//				case 0x3A:	// <:>
//				case 0x0C:	// dsps
//				case 0x0D:	// <CR>
					return ;
					break;
				case 0x7F:	// 
				case 0xFFFFFFF7:	// 
				case 0xFFFFFFF9:	// 
				case 0xFFFFFFE5:	// 
				case 0xFFFFFFE6:	// 
				case 0xFFFFFFE7:	// 
//				case 0xFFFFFFFF:	// 
					ExecPtr++;
					break;
			}
		}
	} else {
		while (1){
			c=SRC[ExecPtr++];
			switch ( c ) {
				case 0x00:	// <EOF>
					ExecPtr--;
				case 0x22:	// "
//				case 0x3A:	// <:>
//				case 0x0C:	// dsps
				case 0x0D:	// <CR>
					return ;
					break;
				case 0x7F:	// 
				case 0xFFFFFFF7:	// 
				case 0xFFFFFFF9:	// 
				case 0xFFFFFFE5:	// 
				case 0xFFFFFFE6:	// 
				case 0xFFFFFFE7:	// 
//				case 0xFFFFFFFF:	// 
					ExecPtr++;
					break;
			}
		}
	}
}

//----------------------------------------------------------------------------------------------
void Skip_rem( char *SRC ){	// skip '...
	int c,reg;
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
				reg=RegVarAliasEx(SRC);
				if ( reg>=0 ) {
					defaultStrAry= reg;
					if ( SRC[ExecPtr] == ',') {
						c=SRC[++ExecPtr];
						defaultStrAryN=Eval_atod( SRC, c );
						if ( SRC[ExecPtr] == ',') {
							c=SRC[++ExecPtr];
							defaultStrArySize=Eval_atod( SRC, c );
						}
					}
				}
			} else
			if ( c==0x1B ) {	// fn
				reg=RegVarAliasEx(SRC);
				if ( reg>=0 ) {
					defaultFnAry= reg;
					if ( SRC[ExecPtr] == ',') {
						c=SRC[++ExecPtr];
						defaultFnAryN=Eval_atod( SRC, c );
						if ( SRC[ExecPtr] == ',') {
							c=SRC[++ExecPtr];
							defaultFnArySize=Eval_atod( SRC, c );
						}
					}
				}
			} else { ExecPtr++;  c=SRC[ExecPtr++]; }
		} else
		if ( c==0x7F ) {
			c=SRC[ExecPtr++];
			if ( c==0xFFFFFFF0 ) {	// Graph
				reg=RegVarAliasEx(SRC);
				if ( reg>=0 ) {
					defaultGraphAry= reg;
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
	int c=SRC[ExecPtr];
	if ( c=='#' ) { 	// C.Basic command
		Skip_rem( SRC );
		return;
	}
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

int CB_CheckLbl( int c ){
	if ( ( '0'<=c )&&( c<='9' ) ) {
		ExecPtr++;
		return c-'0';
	} else if ( ( 'A'<=c )&&( c<='z' ) ) {
		ExecPtr++;
		return  c-'A'+10;
	} else 	if ( ( c == 0xFFFFFFCD ) || ( c == 0xFFFFFFCE ) ) {	// <r> or Theta
		ExecPtr++;
		return  c-0xFFFFFFCD+10+26;
	} else { CB_Error(SyntaxERR); return -1; }	// syntax error
}

void CB_Lbl( char *SRC, short *StackGotoAdrs ){
	int c;
	int label;
	c=SRC[ExecPtr];
	label = CB_CheckLbl( c );
	if ( label < 0 ) { CB_Error(SyntaxERR); return; }	// syntax error
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
			case 0x22:	// "
				Skip_quot(SRC);
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

void CB_Goto( char *SRC, short *StackGotoAdrs ){
	int c;
	int label;
	int ptr;
	c=SRC[ExecPtr];
	label = CB_CheckLbl( c );
	if ( label < 0 ) { CB_Error(SyntaxERR); return; }	// syntax error
	
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
			case 0x22:	// "
				Skip_quot(SRC);
				break;
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
			case 0x22:	// "
				Skip_quot(SRC);
				break;
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
	if ( ( c == ':'  ) || ( c == 0x0D ) )  { c=SRC[++ExecPtr]; while ( c==' ' ) c=SRC[++ExecPtr]; }
	if ( c == 0x27 ) { Skip_rem(SRC); c=SRC[++ExecPtr]; while ( c==' ' ) c=SRC[++ExecPtr]; }
	if ( ( c != 0xFFFFFFF7 ) || ( SRC[ExecPtr+1] != 0x01 ) ) { CB_Error(ThenWithoutIfERR); return; } // not Then error 
	ExecPtr+=2 ;
	if ( value ) return ; // true
	
	for ( i=0; i<CacheIf->CNT; i++ ) {
		if ( CacheIf->Ptr[i]==ExecPtr ) { ExecPtr=CacheIf->Adrs[i]; return ; }	// adrs ok
	} 
	CacheIf->Ptr[CacheIf->CNT]=ExecPtr;
	stat=Search_ElseIfEnd( SRC );
	if ( stat == 3 ) { CacheIf->Ptr[CacheIf->CNT]++; goto loop; }	// ElseIf
	
	if ( CacheIf->CNT < IfCntMax ) {
		CacheIf->Adrs[CacheIf->CNT]=ExecPtr;
		CacheIf->CNT++;
	} else CacheIf->CNT=0;	// over reset
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
			case 0x22:	// "
				Skip_quot(SRC);
				break;
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
	int c,reg,expbuf;
	if ( StackFor->Ptr >= StackForMax ) { CB_Error(NestingERR); return; } //  nesting error
	if (CB_INT) {		//					------------ INT mode
		CBint_CurrentValue = EvalIntsubTop( SRC );
		c=SRC[ExecPtr];
		if ( c == 0x0E ) {	// ->
			ExecPtr++;
			expbuf=ExecPtr;
			reg=RegVarAliasEx(SRC);
			if ( reg>=0 ) { ExecPtr=expbuf;
				StackFor->Var[StackFor->Ptr]=LocalInt[reg];
				CBint_Store(SRC);
			} else { CB_Error(SyntaxERR); return; }	// Syntax error
		}
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
		if ( StackFor->IntStep[StackFor->Ptr] > 0 ) { 	// step +
			if ( CBint_CurrentValue > StackFor->IntEnd[StackFor->Ptr] ) {  // for next cancel
				if ( Search_Next(SRC) == 0 )     { CB_Error(ForWithoutNextERR); return; }  // For without Next error
				return;
			}
		}
		else {									// step -
			if ( CBint_CurrentValue < StackFor->IntEnd[StackFor->Ptr] ) {  // for next cancel
				if ( Search_Next(SRC) == 0 )     { CB_Error(ForWithoutNextERR); return; }  // For without Next error
				return;
			}
		}
		
	} else {			//					------------ Double mode
		CB_CurrentValue = EvalsubTop( SRC );
		c=SRC[ExecPtr];
		if ( c == 0x0E ) {	// ->
			ExecPtr++;
			expbuf=ExecPtr;
			reg=RegVarAliasEx(SRC);
			if ( reg>=0 ) { ExecPtr=expbuf;
				StackFor->Var[StackFor->Ptr]=(int*)LocalDbl[reg];
				CB_Store(SRC);
			} else { CB_Error(SyntaxERR); return; }	// Syntax error
		}
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
		if ( StackFor->Step[StackFor->Ptr] > 0 ) { 	// step +
			if ( CB_CurrentValue > StackFor->End[StackFor->Ptr] ) { // for next cancel
				if ( Search_Next(SRC) == 0 )     { CB_Error(ForWithoutNextERR); return; }  // For without Next error
				return;
			}
		}
		else {									// step -
			if ( CB_CurrentValue < StackFor->End[StackFor->Ptr] ) { // for next cancel
				if ( Search_Next(SRC) == 0 )     { CB_Error(ForWithoutNextERR); return; }  // For without Next error
				return;
			}
		}
	}
	StackFor->Adrs[StackFor->Ptr] = ExecPtr;
	StackFor->Ptr++;
	CurrentStruct->TYPE[CurrentStruct->CNT]=TYPE_For_Next;
	CurrentStruct->CNT++;
}

void CB_Next( char *SRC ,StkFor *StackFor, CurrentStk *CurrentStruct ){
	double step;
	double *dptr;
	int stepint;
	int i;
	int *iptr;
	if (CB_INT) {		//					------------ INT mode
		if ( StackFor->Ptr <= 0 ) { ErrorNo=NextWithoutForERR; ErrorPtr=ExecPtr; return; } // Next without for error
		StackFor->Ptr--;
		CurrentStruct->CNT--;
		stepint = StackFor->IntStep[StackFor->Ptr];
		iptr=StackFor->Var[StackFor->Ptr];
		(*iptr) += stepint;
		if ( stepint > 0 ) { 	// step +
			if ( *iptr > StackFor->IntEnd[StackFor->Ptr] ) { (*iptr) -= step; return ;} // exit
		}
		else {					// step -
			if ( *iptr < StackFor->IntEnd[StackFor->Ptr] ) { (*iptr) -= step; return ;} // exit
		}
		ExecPtr = StackFor->Adrs[StackFor->Ptr];
		StackFor->Ptr++;		// continue
		CurrentStruct->TYPE[CurrentStruct->CNT]=TYPE_For_Next;
		CurrentStruct->CNT++;
		
	} else {			//					------------ Double mode
		if ( StackFor->Ptr <= 0 ) { CB_Error(NextWithoutForERR); ErrorPtr=ExecPtr; return; } // Next without for error
		StackFor->Ptr--;
		CurrentStruct->CNT--;
		step = StackFor->Step[StackFor->Ptr];
		dptr=(double*)StackFor->Var[StackFor->Ptr];
		(*dptr) += step;
		if ( step > 0 ) { 		// step +
			if ( (*dptr) > StackFor->End[StackFor->Ptr] ) { (*dptr) -= step; return ;} // exit
		}
		else {					// step -
			if ( (*dptr) < StackFor->End[StackFor->Ptr] ) { (*dptr) -= step; return ;} // exit
		}
		ExecPtr = StackFor->Adrs[StackFor->Ptr];
		StackFor->Ptr++;		// continue
		CurrentStruct->TYPE[CurrentStruct->CNT]=TYPE_For_Next;
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
			case 0x22:	// "
				Skip_quot(SRC);
				break;
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
			case 0x22:	// "
				Skip_quot(SRC);
				break;
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
	CurrentStruct->TYPE[CurrentStruct->CNT]=TYPE_While_WhileEnd;
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
	CurrentStruct->TYPE[CurrentStruct->CNT]=TYPE_While_WhileEnd;
	CurrentStruct->CNT++;
}

void CB_Do( char *SRC, StkWhileDo *StackWhileDo, CurrentStk *CurrentStruct ) {
	if ( StackWhileDo->DoPtr >= StackDoMax ) { CB_Error(NestingERR); return; }  //  nesting error
	StackWhileDo->DoAdrs[StackWhileDo->DoPtr] = ExecPtr;
	StackWhileDo->DoPtr++;
	CurrentStruct->TYPE[CurrentStruct->CNT]=TYPE_Do_LpWhile;
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
	CurrentStruct->TYPE[CurrentStruct->CNT]=TYPE_Do_LpWhile;
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
			case 0x22:	// "
				Skip_quot(SRC);
				break;
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
			case 0x22:	// "
				Skip_quot(SRC);
				break;
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
	CurrentStruct->TYPE[CurrentStruct->CNT]=TYPE_Switch_Case;
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
	CurrentStruct->TYPE[CurrentStruct->CNT]=TYPE_Switch_Case;
	CurrentStruct->CNT++;
}

void CB_Default( char *SRC, StkSwitch *StackSwitch, CurrentStk *CurrentStruct ) {
	int exitPtr=ExecPtr;
	int c,i;
	if ( StackSwitch->Ptr <= 0 ) { CB_Error(DefaultWithoutSwitchERR); return; }  // Default without Switch error
	CurrentStruct->CNT--;
	CurrentStruct->TYPE[CurrentStruct->CNT]=TYPE_Switch_Case;
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
		case TYPE_For_Next:			// For Next
			if ( Search_Next(SRC) == 0 )     { CB_Error(ForWithoutNextERR); return; }  // For without Next error
			StackFor->Ptr--;
			return ;
		case TYPE_While_WhileEnd:	// While WhileEnd
			if ( Search_WhileEnd(SRC) == 0 ) { CB_Error(WhileWithoutWhileEndERR); return; }  // While without WhileEnd error
			StackWhileDo->WhilePtr--;
			return ;
		case TYPE_Do_LpWhile:		// DO LpWhile
			if ( Search_LpWhile(SRC) == 0 )  { CB_Error(DoWithoutLpWhileERR); return; }  // Do without LpWhile error
			StackWhileDo->DoPtr--;
			return ;
		case TYPE_Switch_Case:		// Switch
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
	if (CB_INT) { CBint_Dsz(SRC) ; return; }
	c=SRC[ExecPtr];
	if ( ( 'A'<=c )&&( c<='z' ) ) {
		ExecPtr++;
		reg=c-'A';
	  regj:
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
		} else
		if ( ( '0'<=c )&&( c<='9' ) ) {
			ExecPtr++;
			dimA=c-'0';
			MatOprand1num( SRC, reg, &dimA, &dimB );
			goto Matrix;
		} else {
			if ( c=='#' ) ExecPtr++;
			LocalDbl[reg][0] --;
			CB_CurrentValue = LocalDbl[reg][0] ;
		}
	} else 
	if ( ( ( c==0x7F ) && ( SRC[ExecPtr+1]==0x40 ) ) || ( ( c==0x7F ) && ( SRC[ExecPtr+1]==0x51 ) ) ) {	// Mat or List
			MatrixOprand( SRC, &reg, &dimA, &dimB );
		Matrix:
			if ( ErrorNo ) {  // error
				if ( MatAry[reg].SizeA == 0 ) ErrorNo=NoMatrixArrayERR;	// No Matrix Array error
				return ;
			}
			CB_CurrentValue = ReadMatrix( reg, dimA,dimB ) ;
			CB_CurrentValue --;
			WriteMatrix( reg, dimA,dimB, CB_CurrentValue ) ;
	} else {
		reg=RegVarAliasEx(SRC);	if ( reg>=0 ) goto regj;
		{ CB_Error(SyntaxERR); return; }	// Syntax error
	}

	c=SRC[ExecPtr];
	if ( ( c==':' ) || ( c==0x0D ) ) {
		ExecPtr++;
		if ( CB_CurrentValue ) return ;
		else {
			Skip_block(SRC);
		}
	} else if ( c==0x0C ) {  // dsps
		ExecPtr++;
		if ( CB_Disps(SRC, 2) ) { BreakPtr=ExecPtr ; return ; }  // [AC] break
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
	if (CB_INT) { CBint_Isz(SRC) ; return; }
	c=SRC[ExecPtr];
	if ( ( 'A'<=c )&&( c<='z' ) ) {
		ExecPtr++;
		reg=c-'A';
	  regj:
		c=SRC[ExecPtr];
		if ( c=='%' ) {
			ExecPtr++;
			LocalInt[reg][0] ++;
			CB_CurrentValue = LocalInt[reg][0] ;
		} else
		if ( c=='[' ) { 
			ExecPtr++;
			MatOprand2( SRC, reg , &dimA, &dimB );
			goto Matrix;
		} else
		if ( ( '0'<=c )&&( c<='9' ) ) {
			ExecPtr++;
			dimA=c-'0';
			MatOprand1num( SRC, reg, &dimA, &dimB );
			goto Matrix;
		} else {
			if ( c=='#' ) ExecPtr++;
			LocalDbl[reg][0] ++;
			CB_CurrentValue = LocalDbl[reg][0] ;
		}
	} else 
	if ( ( ( c==0x7F ) && ( SRC[ExecPtr+1]==0x40 ) ) || ( ( c==0x7F ) && ( SRC[ExecPtr+1]==0x51 ) ) ) {	// Mat or List
			MatrixOprand( SRC, &reg, &dimA, &dimB );
		Matrix:
			if ( ErrorNo ) {  // error
				if ( MatAry[reg].SizeA == 0 ) ErrorNo=NoMatrixArrayERR;	// No Matrix Array error
				return ;
			}
			CB_CurrentValue = ReadMatrix( reg, dimA,dimB ) ;
			CB_CurrentValue ++;
			WriteMatrix( reg, dimA,dimB, CB_CurrentValue ) ;
	} else {
		reg=RegVarAliasEx(SRC);	if ( reg>=0 ) goto regj;
		{ CB_Error(SyntaxERR); return; }	// Syntax error
	}

	c=SRC[ExecPtr];
	if ( ( c==':' ) || ( c==0x0D ) ) {
		ExecPtr++;
		if ( CB_CurrentValue ) return ;
		else {
			Skip_block(SRC);
		}
	} else if ( c==0x0C ) {  // dsps
		ExecPtr++;
		if ( CB_Disps(SRC, 2) ) { BreakPtr=ExecPtr ; return ; }  // [AC] break
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
				if ( c=='%' ) { ExecPtr++;  for ( i=reg; i<=en; i++) LocalInt[ i ][0] = CB_CurrentValue; }
				else
				if ( c=='#' ) ExecPtr++;
				for ( i=reg; i<=en; i++) LocalDbl[ i ][0] = CB_CurrentValue;
			}
		} else {					// 
			if ( c=='%' ) { ExecPtr++;  LocalInt[reg][0] = CB_CurrentValue ; }
			else
			if ( c=='[' ) { goto Matrix; }
			else
			if ( ( '0'<=c )&&( c<='9' ) ) {
				ExecPtr++;
				dimA=c-'0';
				MatOprand1num( SRC, reg, &dimA, &dimB );
				goto Matrix2;
			} else {
				if ( c=='#' ) ExecPtr++;
				LocalDbl[reg][0] = CB_CurrentValue;
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
				if ( dspflag ==3 ) { CopyAns2MatList( SRC, reg ); MatdspNo=reg;  return ; }	// MatAns -> Mat A
				if ( MatAry[reg].SizeA == 0 ) { CB_Error(NoMatrixArrayERR); return; }	// No Matrix Array error
				InitMatSub( reg, CB_CurrentValue);		// 10 -> Mat A
			} else {
			Matrix:
				ExecPtr++;
				MatOprand2( SRC, reg, &dimA, &dimB);
			Matrix2:
				if ( ErrorNo ) {  // error
					if ( MatAry[reg].SizeA == 0 ) ErrorNo=NoMatrixArrayERR;	// No Matrix Array error
					return ;
				}
				WriteMatrix( reg, dimA, dimB, CB_CurrentValue);
			}
		} else if ( c == 0x51 ) {	// List
			ExecPtr+=2;
			reg=ListRegVar( SRC );
		  Listj:
			if ( SRC[ExecPtr] != '[' ) { 
				if ( dspflag ==4 ) { CopyAns2MatList( SRC, reg ) ; MatdspNo=reg; dspflag=0; return ; }	// ListAns -> List 1
				if ( MatAry[reg].SizeA == 0 ) { CB_Error(NoMatrixArrayERR); return; }	// No Matrix Array error
				InitMatSub( reg, CB_CurrentValue);		// 10 -> List 1
			} else {
				ExecPtr++;
				MatOprand1( SRC, reg, &dimA, &dimB);
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
					if ( CB_CurrentValue ) 
							CB_ListInitsub( SRC, &reg, CB_CurrentValue, 0 );
					else {
						reg=ListRegVar( SRC );
						if ( reg>=0 ) DeleteMatrix( reg );
					}
				} else
				if ( ( SRC[ExecPtr]==0x7F ) && ( SRC[ExecPtr+1]==0x40 ) ) {	// {10,5} -> Dim Mat A
					CB_MatrixInit( SRC );
					return ;
				} else {
					if ( CB_CurrentValue ) 
							CB_MatrixInitsubNoMat( SRC, &reg, CB_CurrentValue, 1, 0 );
					else {
						reg=RegVarAliasEx(SRC);
						if ( reg>=0 ) DeleteMatrix( reg );
					}
				}
		} else if ( c == 0x5F ) {	// Ticks
				ExecPtr+=2;
				goto StoreTicks;
		} else if ( c == 0x00 ) {	// Xmin
				Xmin = CB_CurrentValue ;
				Xdot = (Xmax-Xmin)/126.0;
				goto Graphj;
		} else if ( c == 0x01 ) {	// Xmax
				Xmax = CB_CurrentValue ;
				Xdot = (Xmax-Xmin)/126.0;
				goto Graphj;
		} else if ( c == 0x02 ) {	// Xscl
				Xscl = CB_CurrentValue ;
				goto Graphj;
		} else if ( c == 0x04 ) {	// Ymin
				Ymin = CB_CurrentValue ;
				Ydot = (Ymax-Ymin)/62.0;
				goto Graphj;
		} else if ( c == 0x05 ) {	// Ymax
				Ymax = CB_CurrentValue ;
				Ydot = (Ymax-Ymin)/62.0;
				goto Graphj;
		} else if ( c == 0x06) {	// Yscl
				Yscl = CB_CurrentValue ;
				goto Graphj;
		} else if ( c == 0x08) {	// Thetamin
				TThetamin = CB_CurrentValue ;
				goto Graphj;
		} else if ( c == 0x09) {	// Thetamax
				TThetamax = CB_CurrentValue ;
				goto Graphj;
		} else if ( c == 0x0A) {	// Thetaptch
				TThetaptch = CB_CurrentValue ;
				goto Graphj;
		} else if ( c == 0x0B ) {	// Xfct
				Xfct = CB_CurrentValue ;
				goto Graphj;
		} else if ( c == 0x0C ) {	// Yfct
				Yfct = CB_CurrentValue ;
			Graphj:
				ExecPtr+=2;
				CB_ChangeViewWindow() ;
		} else goto exitj;
	} else
	if ( c==0xFFFFFFF7 ) {
		c = SRC[ExecPtr+1] ; 
		if ( c == 0xFFFFFFF6 ) {	// Poke(A)
			ExecPtr+=2;
			CB_PokeSub( SRC, CB_CurrentValue, EvalsubTop( SRC ) );
		} else goto exitj;
	} else
	if ( c=='*' ) { ExecPtr++;
			CB_PokeSub( SRC, CB_CurrentValue, Evalsub1( SRC ) );
	} else
	if ( c=='%' ) { ExecPtr++;
		StoreTicks:
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
		} else goto exitj;
	} else { 
	  exitj:
		reg=RegVarAliasEx( SRC ); if ( reg>=0 ) goto aliasj;	// variable alias
		CB_Error(SyntaxERR); return; }	// Syntax error
}

//----------------------------------------------------------------------------------------------
void  CB_Input( char *SRC ){
	unsigned int key;
	int c;
	double DefaultValue=0;
	int flag=0,flagint=0;
	int reg,bptr,mptr;
	int dimA,dimB,base;
	char buffer[32];
	char*	MatAryC;
	short*	MatAryW;
	int*	MatAryI;
	
	KeyRecover();
	HiddenRAM_MatAryStore();	// MatAry ptr -> HiddenRAM
	CB_SelectTextDD();	// Select Text Screen
	if ( CursorX==22 ) CursorX=1;
	locate( CursorX, CursorY); Print((unsigned char*)"?");
	Scrl_Y();

	c=SRC[ExecPtr];
	bptr=ExecPtr;
	reg=RegVar(c);
	if ( c==0x0E ) {	// ->
		flag=0;
		c=CB_IsStr( SRC, ExecPtr+1 );
		if ( ( c==2 ) || ( c==0x3F ) || ( c==0x41 ) || ( c==0x42 ) ) flag=2;
	} else 
	if ( reg>=0 ) {
	  regj:
		flag=1;
		c=SRC[ExecPtr+1];
		if ( CB_INT ) {
			if ( c=='#' ) {
				DefaultValue = LocalDbl[reg][0] ;
			} else { flagint=1; 
				if ( c=='[' ) {
					ExecPtr+=2;
					MatOprandInt2( SRC, reg, &dimA, &dimB );
					goto Matrix;
				} else
				if ( ( '0'<=c )&&( c<='9' ) ) {
					ExecPtr++;
					dimA=c-'0';
					MatOprand1num( SRC, reg, &dimA, &dimB );
					goto Matrix;
				} else DefaultValue = LocalInt[reg][0] ;
			}
		} else {
			if ( c=='%' ) { flagint=1; 
				DefaultValue = LocalInt[reg][0] ;
			} else {
				if ( c=='[' ) {
					ExecPtr+=2;
					MatOprand2( SRC, reg, &dimA, &dimB );
					goto Matrix;
				} else
				if ( ( '0'<=c )&&( c<='9' ) ) {
					ExecPtr++;
					dimA=c-'0';
					MatOprand1num( SRC, reg, &dimA, &dimB );
					goto Matrix;
				} else DefaultValue = LocalDbl[reg][0] ;
			}
		}
	} else
	if ( c==0x7F ) {
		c = SRC[ExecPtr+1] ; 
		if ( ( c == 0x40 ) || ( c == 0x51 ) ) {	// Mat A[a,b] or List 1[a]
			MatrixOprand( SRC, &reg, &dimA, &dimB );
		Matrix:
			if ( ErrorNo ) {  // error
				if ( MatAry[reg].SizeA == 0 ) ErrorNo=NoMatrixArrayERR;	// No Matrix Array error
				return ;
			}
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
		} else if ( c == 0x08) {	// Thetamin
				DefaultValue  = TThetamin ;
				flag=1;
		} else if ( c == 0x09) {	// Thetamax
				DefaultValue  = TThetamax ;
				flag=1;
		} else if ( c == 0x0A) {	// Thetaptch
				DefaultValue  = TThetaptch ;
				flag=1;
		} else if ( c == 0x0B ) {	// Xfct
				DefaultValue = Xfct ;
				flag=1;
		} else if ( c == 0x0C ) {	// Yfct
				DefaultValue = Yfct ;
				flag=1;
		} else if ( c == 0xFFFFFFF0 ) {	// GraphY
			reg=defaultGraphAry;
			goto strj;
		} else goto exitj;
	} else
	if ( c==0xFFFFFFF9 ) {
		c = SRC[ExecPtr+1] ; 
		if ( c == 0x3F ) {	// Str 1-20
			reg=defaultStrAry;
		  strj:
			ExecPtr+=2;
			if ( MatAry[reg].SizeA == 0 ) { CB_Error(MemoryERR); return; }	// Memory error
			dimA = CB_EvalInt( SRC );	// str no : Mat s[n,len]
			base = MatAry[reg].Base;
			if ( ( dimA < base ) || ( dimA > MatAry[reg].SizeA-1+base ) ) { CB_Error(ArgumentERR); return; }  // Argument error
			dimB=1;
			flag=3;
			ExecPtr=bptr;
		} else
		if ( c == 0x1B ) {	// fn
			reg=defaultFnAry;
			goto strj;
		} else
		if ( c == 0x41 ) {	// DATE
			ExecPtr+=2;
			flag=4;
			ExecPtr=bptr;
		} else
		if ( c == 0x42 ) {	// TIME
			ExecPtr+=2;
			flag=5;
			ExecPtr=bptr;
		} else
		if ( c == 0x21 ) {	// Xdot
				DefaultValue = Xdot ;
				flag=1;
		} else goto exitj;
	} else
	if ( c=='$' ) {
		ExecPtr++;
		MatrixOprand( SRC, &reg, &dimA, &dimB );
		if ( ErrorNo ) return ; // error
		if ( MatAry[reg].SizeA == 0 ) { CB_Error(NoMatrixArrayERR); return; }	// No Matrix Array error
		if ( MatAry[reg].ElementSize != 8 ) { CB_Error(ArgumentERR); return; }	// element size error
		flag=3;
		ExecPtr=bptr;
	} else {
	  exitj:
		reg=RegVarAliasEx( SRC ); if ( reg>=0 ) goto regj;	// variable alias
		CB_Error(SyntaxERR); return; }	// Syntax error

	switch ( flag ) {
		case 0:	// ? -> A value
			CB_CurrentValue = InputNumD_CB( 1, CursorY, 21, 0 );
			ErrorNo=0; // error cancel
			if ( BreakPtr > 0 ) { ExecPtr=BreakPtr; return ; }
			CBint_CurrentValue = CB_CurrentValue ;
			break;
		case 1:	// ?A value
			sprintGR(buffer, DefaultValue, 22-CursorX,RIGHT_ALIGN, CB_Round.MODE, CB_Round.DIGIT);
			locate( CursorX, CursorY); Print((unsigned char*)buffer);
			Scrl_Y();
			CB_CurrentValue = InputNumD_CB1( CursorX, CursorY, 21, DefaultValue );
			ErrorNo=0; // error cancel
			if ( BreakPtr > 0 ) { ExecPtr=BreakPtr; return ; }
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
	Inpj1:	key=InputStr( 1, CursorY, CB_StrBufferMax-1,  CB_CurrentStr, ' ', REV_OFF);
			ErrorNo=0; // error cancel
			if ( key==KEY_CTRL_AC  ) { BreakPtr=ExecPtr;  return ; }
			if ( SRC[ExecPtr]==0x0E ) ExecPtr++;	// -> skip
			CB_StorStr( SRC );
			break;
		case 3:	// ?&Mat  ?Str1-20
			MatAryC=MatrixPtr( reg, dimA, dimB );
			OpcodeStringToAsciiString(buffer, MatAryC, 31);
			CB_CurrentStr=MatAryC;
//			CB_Print(CursorX, CursorY, (unsigned char*)buffer);
//			Scrl_Y();
			key=InputStr( 1, CursorY, MatAry[reg].SizeB-1,  CB_CurrentStr, ' ', REV_OFF);
			ErrorNo=0; // error cancel
			if ( key==KEY_CTRL_AC  ) { BreakPtr=ExecPtr;  return ; }
			CB_StorStr( SRC );
			break;
		case 4:	// ?DATE
			CB_DateToStr();
			CB_CurrentStr[10]='\0';	// week cancel
	Inpj2:
//			CB_Print(CursorX, CursorY, (unsigned char*)CB_CurrentStr);
//			Scrl_Y();
			goto Inpj1;
			break;
		case 5:	// ?TIME
			CB_TimeToStr();
			goto Inpj2;
			break;
	}
	Scrl_Y();
	Bdisp_PutDisp_DD_DrawBusy();
	return ;
}
//----------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------
//int ObjectAlign4g( unsigned int n ){ return n; }	// align +4byte
/*
int ObjectAlign4h( unsigned int n ){ 	// align +16 
	switch ( n ) {
		case 1:		// +4byte
		case 100: return 1000;	// +12byte
		case 101: return 1001;	// +12byte
		case 102: return 1002;	// +12byte
		case 103: return 1003;	// +12byte
		case 104: return 1004;	// +12byte
		case 0: return 1;
		default: return 0;
	}
}
*/
//----------------------------------------------------------------------------------------------
/*
int GetCmmOpcode(char *SRC, char *buffer, int Maxlen) {		// dummy
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
			case 0xFFFFFFF1:	// 
				buffer[ptr++]=SRC[ExecPtr++];
				break;
			default:
				break;
		}
		if ( ptr >= Maxlen-1 ) { CB_Error(StringTooLongERR); break; }	// String too Long error
	}
	return ptr;
}

void CB_arg( char *SRC ) {		// dummy
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
void CB_argNum( char *SRC ) { // 
	int c=1,i,j;
	Argc=0;
	while ( (c!=0)&&(c!=0x0C)&&(c!=0x0D)&&(c!=':') ) {
		if ( CB_INT ) LocalInttmp[Argc]=EvalIntsubTop( SRC ); else LocalDbltmp[Argc]=EvalsubTop(  SRC );
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
		ptr=ProgfileAdrs[i]+0x3C-8;
		for (j=0;j<16;j++) if ( ptr[j] != name[j] ) break;
		if ( j==16 )	return i ; // ok
		i++;
	}
	return -1; // fault
}
void CB_Prog( char *SRC, int *localvarInt, double *localvarDbl ) { //	Prog "..."
	int c,i,j;
	char buffer[32],folder16[21];
	char *src;
	char *StackProgSRC;
	int StackProgExecPtr;
	char stat;
	char ProgNo_bk;
	char BreakPtr_bk; 
	char StepOutProgNo=0;

	c=SRC[ExecPtr];
	if ( c != 0x22 ) { CB_Error(SyntaxERR); return; }	// Syntax error
	ExecPtr++;
	CB_GetQuotOpcode(SRC, buffer,32-1);	// Prog name

	c=SRC[ExecPtr];
	if ( c == ',' ) {	// arg
		ExecPtr++;
		CB_argNum( SRC );	// get local value
	}
	
	StackProgSRC     = SRC;
	StackProgExecPtr = ExecPtr;
	ProgNo_bk = ProgNo;
	
	Setfoldername16( folder16, buffer );
	ProgNo = CB_SearchProg( folder16 );
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
	
	InitLocalVar();		// init Local variable
	for (c=0; c<StackGotoMax; c++) StackGotoAdrs[c]=0;	// init goto

	for ( i=0; i<ProgLocalN[ProgNo]; i++ ) {		// restore local variable
		j=ProgLocalVar[ProgNo][i];
		if ( j>=0 ) { 
			LocalDbl[j]=&localvarDbl[i];
			LocalInt[j]=&localvarInt[i];
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
		CB_argNum( SRC );	// get local value
		
		for ( i=0; i<ProgLocalN[ProgNo]; i++ ) {	// set local variable
			j=ProgLocalVar[ProgNo][i];
			if ( j>=0 ) { 
				LocalDbl[j][0]=LocalDbltmp[i];
				LocalInt[j][0]=LocalInttmp[i];
			}
		}
		
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
ALIAS_VAR	AliasVarCode[AliasVarMAX];

void CB_AliasVarClr(){
	int i;
	for ( i=0; i<AliasVarMAX; i++ ) AliasVarCode[i].org = -1 ;
}

void CB_AliasVar( char *SRC ) {	// AliasVar A=ƒ¿
	int c,i,reg,len;
	int alias_code, org_reg;
	
	c=SRC[ExecPtr];
	reg=RegVar(c);
	org_reg=reg;
	if ( reg>=0 ) {
		ExecPtr++;
		c=SRC[ExecPtr];
		if ( c != '=' ) { CB_Error(SyntaxERR); return; }	// Syntax error
		ExecPtr++;
		c=SRC[ExecPtr];
		i=RegVar(c); if ( i>=0 ) { CB_Error(ArgumentERR); return; }  // Argument error	 ( = default built-in variable )
		len = GetOpcodeLen( SRC, ExecPtr ,&alias_code );
		if ( len == 1 ) alias_code &=0xFF;
		ExecPtr += len;
		for ( i=0; i<AliasVarMAX; i++ ) {
			if ( AliasVarCode[i].org<0 ) break;
			if ( AliasVarCode[i].alias==(short)alias_code ) {
				if ( AliasVarCode[i].org==org_reg ) return; 	// same : alias cancel
				break;
			}
		}
		if ( i >= AliasVarMAX ) { CB_Error(TooMuchData); return; }
		AliasVarCode[i].org  =org_reg;
		AliasVarCode[i].alias=alias_code;
	}
}



//----------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------
int CB_interpreter( char *SRC ) {
	int flag;
	int c;
	int stat;

	CB_INT = CB_INTDefault;
	Waitcount=DefaultWaitcount;
	MatBase = MatBaseDefault;
	CB_TicksStart = RTC_GetTicks();	// 
	CB_TicksAdjust = 0 ;	// 
	srand( CB_TicksStart ) ;	// rand seed
	ScreenMode = 0;	// Text mode
	UseGraphic = 0;
	PxlMode = 1;		// Pxl  1:set  0:clear	
	BG_Pict_No=0;		// BG-None
	CB_ClrText(SRC);
	ProgEntryN = 0;	// subroutin clear
	GosubNestN = 0;	// Gosub clear
	ErrorPtr = 0;
	ErrorNo = 0;

	defaultStrAry=26;	// <r>
	defaultStrAryN=20;
	defaultStrArySize=255+1;
	defaultFnAry=27;		// Theta
	defaultGraphAry=27;		// Theta

	CB_MatListAnsreg=27;	//	ListAns init
	Bdisp_PutDisp_DD_DrawBusy();
	KeyRecover(); 
	Argc = 0;	// 
	CB_AliasVarClr();
	DeleteMatListAnsAll();	// Ans init	
	for ( c=0; c<3; c++ ) CB_S_Gph_init( c );
		
	stat = CB_interpreter_sub( SRC );
	KeyRecover(); 
//	if ( ErrorNo ) { CB_ErrMsg( ErrorNo ); }
	return stat;
}

//----------------------------------------------------------------------------------------------
int iObjectAlign4a( unsigned int n ){ return n; }	// align +4byte
int iObjectAlign4b( unsigned int n ){ return n; }	// align +4byte
int iObjectAlign4c( unsigned int n ){ return n; }	// align +4byte
int iObjectAlign4d( unsigned int n ){ return n; }	// align +4byte
int iObjectAlign4e( unsigned int n ){ return n; }	// align +4byte
int iObjectAlign4f( unsigned int n ){ return n; }	// align +4byte
//int iObjectAlign4g( unsigned int n ){ return n; }	// align +4byte
//int iObjectAlign4h( unsigned int n ){ return n; }	// align +4byte
//int iObjectAlign4i( unsigned int n ){ return n; }	// align +4byte
//----------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------
