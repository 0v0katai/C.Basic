/*
===============================================================================

 Casio Basic interpreter for fx-9860G series    v1.8x

 copyright(c)2015/2016/2017/2018 by sentaro21
 e-mail sentaro21@pm.matrix.jp

===============================================================================
*/
#include "CB.h"

//-----------------------------------------------------------------------------
// Casio Basic inside
//-----------------------------------------------------------------------------
char CB_INTDefault=0;	// default mode  0:normal  1: integer mode

char	DrawType    = 0;	// 0:connect  1:Plot
char	Coord       = 1;	// 0:off 1:on
char	Grid        = 0;	// 0:off 1:on
char	Axes        = 1;	// 0:off 1:on
char	Label       = 0;	// 0:off 1:on
char	Derivative  = 0;	// 0:off 1:on
char	S_L_Style   = S_L_Normal;
char	tmp_Style   = S_L_Normal;
char	Angle       = 1;	// 0:deg   1:rad  2:grad

char PxlMode=1;		// Pxl  1:set  0:clear

char BreakCheckDefault=1;	// Break Stop on/off
char BreakCheck=1;	// Break Stop on/off
char ACBreak=1;		// AC Break on/off

char TimeDsp=0;		// Execution Time Display  0:off 1:on
char MatXYmode=0;		// 0: normal  1:reverse
char PictMode=3;	// StoPict/RclPict  StrageMem:0  heap:1  both:2  MSC:3(default)
char CheckIfEnd=0;	// If...IfEnd check  0:off  1:on

char CommandInputMethod=1;	//	0:C.Basic  1:Genuine

char  RefreshCtrl=0;	// 0:no refresh Ctrl     1: GrphicsCMD refresh Ctrl      2: all refresh Ctrl
char  Refreshtime=2;	// Refresh time  (Refreshtime+1)/128s

short DefaultWaitcount=0;	// wait control
short Waitcount=0;	// current wait control

double Previous_X=1e308 ;	// Line Previous X
double Previous_Y=1e308 ;	// Line Previous Y
double Previous_X2=1e308 ;	// Line Previous X2
double Previous_Y2=1e308 ;	// Line Previous Y2
int    Previous_PX=-1   ;	// Plot Previous PX
int    Previous_PY=-1   ;	// Plot Previous PY
double Plot_X    =1e308 ;	// Plot Current X
double Plot_Y    =1e308 ;	// Plot Current Y

//-----------------------------------------------------------------------------
// Casio Basic Gloval variable
//-----------------------------------------------------------------------------
complex REG[VARMAXSIZE];
double  REGv[11];
int  	REGINT[VARMAXSIZE];
char    REGtype[VARMAXSIZE];

double	Xfct     =  2;					// 
double	Yfct     =  2;					// 

short 	Argc=0;
complex	LocalDbltmp[ArgcMAX];
int		LocalInttmp[ArgcMAX];

complex *LocalDbl[VARMAXSIZE];
int		*LocalInt[VARMAXSIZE];

double	*traceAry;		// Graph array X

char *GraphY;
char *GraphX;

unsigned char *PictAry[PictMax+1];		// Pict array ptr
char BG_Pict_No=0;

//----------------------------------------------------------------------------------------------
//		Interpreter inside
//----------------------------------------------------------------------------------------------
int	CB_TicksStart;
int	CB_TicksAdjust;
int	CB_HiTicksStart;
int	CB_HiTicksAdjust;

int CB_INT=0;		// current mode  0:normal  1: integer mode
int ExecPtr=0;		// Basic execute ptr
int BreakPtr=0;		// Basic break ptr

int CBint_CurrentValue;	// 
complex CB_CurrentValue;	//

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
char  ProgLocalVar[ProgMax+1][ArgcMAX];

char *HeapRAM;
char *TVRAM;
char *GVRAM;
//----------------------------------------------------------------------------------------------
int StackGotoAdrs[StackGotoMax];

CchIf	CacheIf;
CchIf	CacheElse;
CchIf	CacheElseIf;
CchIf	CacheSwitch;
CchIf	CacheRem;

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
int CB_F7sub( char *SRC, int c ) ;

int CB_interpreter_sub( char *SRC ) {
	char cont=1;
	int dspflagtmp=0;
	int dspflagtmp2;
	int c,j;
	int excptr;
	int breakcount=BREAKCOUNT;

	int StackGosubAdrs[StackGosubMax];
	
	CurrentStk	CurrentStruct;
	
	complex	localvarDbl[ArgcMAX];	//	local var
	int		localvarInt[ArgcMAX];	//	local var

	if ( 0x88020200 > (int)&cont ) { CB_Error(StackERR); return -1; } //  stack error
	
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
	CurrentStruct.ForPtr=0;
	CurrentStruct.WhilePtr=0;
	CurrentStruct.DoPtr=0;
	CurrentStruct.SwitchPtr=0;
	
	while (cont) {
		dspflagtmp=0;
		dspflagtmp2=dspflag;
		CB_StrBufferCNT=0;			// Quot String buffer clear
		if ( ErrorNo || BreakPtr ) { 
			if ( BreakPtr == -8 ) goto iend;
			if ( CB_BreakStop() ) return -7 ;
			if ( SRC[ExecPtr] == 0x0C ) if ( ( ErrorNo==0 ) && ( DebugMode==0 ) ) ExecPtr++; // disps
			ClrCahche();
		}
		c=SRC[ExecPtr++];
		if ( ( c==':'  ) || ( c==0x0D )|| ( c==0x20 ) ) {
				c=SRC[ExecPtr++];
				while ( ( c==0x0D ) || ( c==0x20 ) ) c=SRC[ExecPtr++];
				if  ( breakcount==0 ) {
					if ( ( BreakCheck ) && ( KeyScanDownAC() ) ) {	// [AC] break?
						KeyRecover();
						if ( BreakPtr == 0 ) BreakPtr=ExecPtr-1;
					} 
					breakcount=BREAKCOUNT;
				} else breakcount--;
		}
		if ( c==0x00 ) { ExecPtr--;
			if ( ProgEntryN ) return -1;
			else  break;
		}
		
//		while ( c==0x20 ) c=SRC[ExecPtr++]; // Skip Space

		switch (c) {
			case 0xFFFFFFEC:	// Goto
				CB_Goto(SRC, StackGotoAdrs, &CurrentStruct );
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
						CB_Next(SRC, &CurrentStruct );
						break;
					case 0x09:	// WhileEnd
						CB_WhileEnd(SRC, &CurrentStruct );
						break;
					case 0x0B:	// LpWhile
						CB_LpWhile(SRC, &CurrentStruct );
						break;
					case 0x04:	// For
						CB_For(SRC, &CurrentStruct );
//						ClrCahche();
						break;
					case 0x08:	// While
						CB_While(SRC, &CurrentStruct );
//						ClrCahche();
						break;
					case 0x0A:	// Do
						CB_Do(SRC, &CurrentStruct );
//						ClrCahche();
						break;
					case 0xFFFFFFEA:	// Switch
						CB_Switch(SRC, &CurrentStruct, &CacheSwitch );
						break;
					case 0xFFFFFFEB:	// Case
						CB_Case(SRC, &CurrentStruct );
						break;
					case 0xFFFFFFEC:	// Default
						CB_Default(SRC, &CurrentStruct );
						break;
					case 0xFFFFFFED:	// SwitchEnd
						CB_SwitchEnd(SRC, &CurrentStruct );
						break;
					case 0x0D:	// Break
						CB_Break(SRC, &CurrentStruct );
						dspflag=0;
						break;
					case 0x0C:	// Return
						c=SRC[ExecPtr];
						if ( (c!=0)&&(c!=0x0C)&&(c!=0x0D)&&(c!=':') ) { 
							dspflag=2;
							if (CB_INT==1)	CBint_CurrentValue   = EvalIntsubTop( SRC );
							else
							if (CB_INT==0)	CB_CurrentValue.real = EvalsubTop( SRC );
							else			CB_CurrentValue      = Cplx_EvalsubTop( SRC );
							dspflagtmp=dspflag;
							if ( EvalEndCheck( SRC[ExecPtr] ) == 0 ) CB_Error(SyntaxERR) ; // Syntax error 
						}
						if ( GosubNestN > 0 ) {				//	return from subroutin 
							ExecPtr=StackGosubAdrs[--GosubNestN] ; break; }
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
						if (CB_INT==1)	CBint_PxlSub(SRC,1); else CB_PxlSub(SRC,1);
						dspflag=0;
						UseGraphic=9;
						break;
					case 0xFFFFFFAC:	// PxlOff
						if (CB_INT==1)	CBint_PxlSub(SRC,0); else CB_PxlSub(SRC,0);
						dspflag=0;
						UseGraphic=9;
						break;
					case 0xFFFFFFAD:	// PxlChg
						if (CB_INT==1)	CBint_PxlSub(SRC,2); else CB_PxlSub(SRC,2);
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
					case 0xFFFFFFFC:	// PutDispDD
						CB_PutDispDD(SRC);
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
					case 0xFFFFFFDD:	// Beep
						CB_Beep(SRC);
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
					case 0x23:			// DrawStat
						CB_DrawStat(SRC);
						dspflag=0;
						UseGraphic=9;
						break;
					case 0x20:			// DrawGraph
						CB_DrawGraph(SRC);
						UseGraphic=2;
						dspflag=0;
						break;
					case 0xFFFFFF9E:	// Menu
						CB_Menu( SRC, StackGotoAdrs );
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
						if ( ( ( 0x11 <= c ) && ( c <=0x7D ) ) || ( ( 0xFFFFFF8C <= c ) && ( c <=0xFFFFFFDF ) ) ) { 
							if ( CB_F7sub( SRC, c ) ) { dspflag=0; break; }
						}
						goto Evalexit2;
				}
				break;
				
			case 0x7F:	// 7F
				c=SRC[ExecPtr++];
				switch ( c ) {
					case 0x40:	// Mat
					case 0xFFFFFF84:	// Vct
						dspflagtmp=CB_MatCalc(SRC,c);
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
					case 0x34 :				// Red
					case 0x35 :				// Blue
					case 0x36 :				// Green
						break;
					default:
						goto Evalexit2;
				}
				break;
				
			case 0xFFFFFFF9:	// F9
				c=SRC[ExecPtr++];
				if ( ( 0xFFFFFFC0 <= c ) && ( c <= 0xFFFFFFDF ) && ( c != 0xFFFFFFC6 ) && ( c != 0xFFFFFFD8 ) ) { CB_ML_command( SRC, c ); break; }
				else
				if ( ( 0x38 != c ) && ( 0x3E != c ) && ( 0x34 <= c ) && ( c <= 0x49 ) )  goto strjp;
				else
				if ( ( 0xFFFFFF9B <= c ) && ( c <= 0xFFFFFF9F ) ) break;	// color command   Black/(White)/Magenta/Cyan/Yellow
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
//					case 0x44:	// StrChar(
//					case 0x45:	// StrCenter(
					case 0x4D:	// StrSplit(
					strjp:
						ExecPtr-=2;
						CB_Str(SRC) ;
//						dspflag=0;
						break;

					case 0x08:	// Real
						ComplexMode = 0;
						dspflag=0;
						break;
					case 0x09:	// a+bi
						ComplexMode = 1;
						dspflag=0;
						break;
					case 0x0A:	// r_theta
						ComplexMode = 2;
						dspflag=0;
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
					case 0x1C:	// File 
						CB_ListFile(SRC);
						dspflag=0;
						break;
					case 0x1E:	// ClrMat
						CB_ClrMat(SRC);
						dspflag=0;
						break;
					case 0x3E:	// ClrVct
						CB_ClrVct(SRC);
						dspflag=0;
						break;
					case 0x4F:	// Wait
						CB_Wait(SRC);
						dspflag=0;
						break;
					case 0x61:	// SetFont 
						CB_SetFont(SRC);
						break;
					case 0x63:	// SetFontMini 
						CB_SetFontMini(SRC);
						break;
					case 0x2C:	// Graph X>
					case 0x2D:	// Graph X<
					case 0x2E:	// Graph X>=
					case 0x2F:	// Graph X<=
						CB_GraphX(SRC);
						dspflag=0;
						UseGraphic=9;
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
//				dspflag=0;
				break;
			case 0xFFFFFFED:	// Prog "..."
				CB_Prog(SRC, localvarInt, localvarDbl );
				ClrCahche();
				if ( BreakPtr > 0 ) return BreakPtr;
				goto jpgsb;
			case 0xFFFFFFFA:	// Gosub
				CB_Gosub(SRC, StackGotoAdrs, StackGosubAdrs );
		jpgsb:	dspflagtmp=dspflag;
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
				goto retvalue;
				break;
			case 0xFFFFFFE3:	// Fix
				CBint_CurrentValue = CB_Fix(SRC);
				goto retvalue;
				break;
			case 0xFFFFFFE4:	// Sci
				CBint_CurrentValue = CB_Sci(SRC);
			  retvalue:
				CB_CurrentValue    = Int2Cplx( CBint_CurrentValue );
				dspflagtmp=2;
				break;
			case 0xFFFFFFEB:	// ViewWindow
				CB_ViewWindow(SRC);
				dspflag=0;
				UseGraphic=3;
				break;
			case 0xFFFFFFEE:	// Graph Y=
			case 0xFFFFFFEF:	// Graph Integral
			case 0xFFFFFFF0:	// Graph Y>
			case 0xFFFFFFF1:	// Graph Y<
			case 0xFFFFFFF2:	// Graph Y>=
			case 0xFFFFFFF3:	// Graph Y<=
			case 0xFFFFFFF4:	// Graph r=
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
				CB_ResetExecTicks();
				dspflagtmp=2;
//				if ( BreakPtr ) break;
//				c=SRC[ExecPtr++];
//				if ( c == 0x0E ) {		// ->
//					if (CB_INT==1)	CBint_Store(SRC); else CB_Store(SRC);
//				}
//				else ExecPtr--;
				break;
			case 0x0C:	// disps
				if ( CB_Disps(SRC, dspflag) ) BreakPtr=ExecPtr ;  // [AC] break
				CB_ResetExecTicks();
//				c=SRC[ExecPtr]; while ( c==0x20 ) c=SRC[++ExecPtr]; // Skip Space
				break;
		
			default:
				ExecPtr--;
			  Evalexit:
				excptr=ExecPtr;
				dspflag=2;
				dspflagtmp=2;
				if (CB_INT==1)	{	// int
					CBint_CurrentValue = EvalIntsubTop( SRC );
					if ( dspflag>=3 ) {
						ExecPtr=excptr;
						dspflagtmp=ListEvalIntsubTopAns(SRC);	// List calc	dspflag; //	2:nomal  3:mat  4:list
					}
				} else {
					if (CB_INT==0)	CB_CurrentValue.real = EvalsubTop( SRC );		// double
					else 			CB_CurrentValue      = Cplx_EvalsubTop( SRC );	// complex
					if ( dspflag>=3 ) {
						ExecPtr=excptr;
						if (CB_INT==0)	dspflagtmp=ListEvalsubTopAns(SRC);	// List calc	dspflag; //	2:nomal  3:mat  4:list
						else			dspflagtmp=Cplx_ListEvalsubTopAns(SRC);	// List calc	dspflag; //	2:nomal  3:mat  4:list
					}
				}
				c=SRC[ExecPtr]; 
				if (c==0x0E) goto inext1;
				if (c==0x13) goto inext2;
				if ( (c==':')||(c==0x0D) ) goto inext3;
				if ( (c==0x0C)||(c==0x00) ) goto inext3;
				if ( c==0xFFFFFFF9 ) {
					c=SRC[ExecPtr+1];
					if ( (0x05<=c)&&(c<=0x07) ) { ExecPtr+=2; goto inext3; }	// >DMS // >a+bi // >r_theta
				}
				CB_Error(SyntaxERR) ; // Syntax error 
				goto inext3;
		}
		c=SRC[ExecPtr];
//		while ( c==0x20 ) c=SRC[++ExecPtr]; // Skip Space
		if ( c == 0x0E ) { 
		  inext1:
			ExecPtr++;
			if (CB_INT==1)	CBint_Store(SRC);	// ->
			else		CB_Store(SRC);		// ->
			dspflagtmp=dspflag+0x10; //	12:nomal  13:mat  14:list
			c=SRC[ExecPtr];
		}
		if ( c == 0x13 ) {					// =>
		  inext2:
			ExecPtr++;
			dspflag=dspflagtmp2;
			dspflagtmp=0;
			if (CB_INT==1)	{ 
				if ( CBint_CurrentValue   == 0 ) Skip_block(SRC);	// false
			} else
			if (CB_INT==0)	{	// double
				if ( CB_CurrentValue.real == 0 ) Skip_block(SRC);	// false
			} else {   //	complex
				if ( CB_CurrentValue.imag != 0 ) { CB_Error(NonRealERR) ; }	// Input value must be a real number
				else
				if ( CB_CurrentValue.real == 0 ) Skip_block(SRC); 	// false
			}
		}
	  inext3:
		if ( ( 0 < dspflagtmp ) && ( dspflagtmp < 0x10 ) ) {
			if ( dspflagtmp>=3 ) CopyMatList2AnsTop( CB_MatListAnsreg );	// MatListResult -> MatList Ans top
			else {
				if (CB_INT==1) regint_Ans=CBint_CurrentValue ;
				else		   reg_Ans   =CB_CurrentValue    ;
			}
			
		}
		if ( dspflagtmp ) dspflag=dspflagtmp & 0x0F;
	}
	iend:
	if ( ProgEntryN == 0 ) CB_end(SRC);
	return -1;
}

//----------------------------------------------------------------------------------------------
int CB_F7sub( char *SRC, int c ) {
	switch ( c ) {
					case 0x11:			// Send(
						CB_Send( SRC );;
						break;
					case 0x12:			// Receive(
						CB_Receive( SRC );;
						break;
					case 0x13:			// OpenComport38k
						CB_OpenComport38k( SRC );;
						break;
					case 0x14:			// CloseComport38k
						CB_CloseComport38k( SRC );;
						break;
					case 0x15:			// Send38k
						CB_Send38k( SRC );;
						break;
					case 0x16:			// Receive38k
						CB_Receive38k( SRC );;
						break;
					case 0x1C:			// S-L-Normal
						S_L_Style = S_L_Normal;
						break;
					case 0x1D:			// S-L-Thick
						S_L_Style = S_L_Thick;
						break;
					case 0x1E:			// S-L-Broken
						S_L_Style = S_L_Broken;
						break;
					case 0x1F:			// S-L-Dot
						S_L_Style = S_L_Dot;
						break;
					case 0x4A:			// S-Gph1
						CB_S_Gph(SRC, 0);
						break;
					case 0x4B:			// S-Gph2
						CB_S_Gph(SRC, 1);
						break;
					case 0x4C:			// S-Gph3
						CB_S_Gph(SRC, 2);
						break;
					case 0x60:			// S-WindAuto
						CB_S_WindAuto(SRC);
						break;
					case 0x61:			// S-WindMan
						CB_S_WindMan(SRC);
						break;
					case 0x63:			// Y=Type
						break;
					case 0x70:			// G-Connect
						DrawType=0;
						break;
					case 0x71:			// G-Plot
						DrawType=1;
						break;
					case 0x78:	// BG-None
						CB_BG_None( SRC );
						break;
					case 0x79:	// BG-Pict
						CB_BG_Pict( SRC );
						break;
					case 0x7A:			// GridOff
						Grid=0;
						break;
					case 0x7D:			// GridOn
						Grid=1;
						break;
					case 0x62:			// Graph X=
						CB_GraphX(SRC);
						dspflag=0;
						UseGraphic=9;
						break;
					case 0xFFFFFF93:	// StoPict
						CB_StoPict(SRC);
						UseGraphic=9;
						break;
					case 0xFFFFFF94:	// RclPict
						CB_RclPict(SRC);
						UseGraphic=9;
						break;
					case 0xFFFFFF9D:	// StoCapt
						CB_StoCapt( SRC );
						UseGraphic=9;
						break;
					case 0xFFFFFF9F:	// RclCapt
						CB_RclCapt( SRC );
						UseGraphic=9;
						break;
					case 0xFFFFFF8C:	// SketchNormal
						tmp_Style = S_L_Normal;
						break;
					case 0xFFFFFF8D:	// SketchThick
						tmp_Style = S_L_Thick;
						break;
					case 0xFFFFFF8E:	// SketchBroken
						tmp_Style = S_L_Broken;
						break;
					case 0xFFFFFF8F:	// SketchDot
						tmp_Style = S_L_Dot;
						break;
					case 0xFFFFFF97:	// Sto-VWin
						CB_StoVWin(SRC);
						break;
					case 0xFFFFFF98:	// Rcl-VWin
						CB_RclVWin(SRC);
						break;
					case 0xFFFFFFB0:	// SortA(
						CB_SortAD(SRC, 1);
						break;
					case 0xFFFFFFB1:	// SortD(
						CB_SortAD(SRC, 0);
						break;
					case 0xFFFFFFC2:	// AxesOn
						Axes=1;
						break;
					case 0xFFFFFFC3:	// CoordOn
						Coord=1;
						break;
					case 0xFFFFFFC4:	// LabelOn
						Label=1;
						break;
					case 0xFFFFFFC5:	// DerivOn
						Derivative=1;
						break;
					case 0xFFFFFFD2:	// AxesOff
						Axes=0;
						break;
					case 0xFFFFFFD3:	// CoordOff
						Coord=0;
						break;
					case 0xFFFFFFD5:	// DerivOff
						Derivative=0;
						break;
					case 0xFFFFFFD4:	// LabelOff
						Label=0;
					case 0xFFFFFFC0:	// FunOn
					case 0xFFFFFFD0:	// FunOFF
					case 0xFFFFFFD1:	// SimulOff
						break;
					case 0xFFFFFFDF:	// Delete
						CB_Delete( SRC );
						break;
					default:
						if ( ( 0xFFFFFFB8 <= c )&& ( c <= 0xFFFFFFBD ) ) { CB_ListFile(SRC); dspflag=0; break; }	// File1~File6
						return 0;
	}
	return 1;
}
//----------------------------------------------------------------------------------------------
void ClrCahche(){
	int i;
	for (i=0; i<StackGotoMax; i++) StackGotoAdrs[i]=0;	// init goto
	CacheIf.CNT=0;
	CacheIf.TOP=0;
	CacheElse.CNT=0;
	CacheElse.TOP=0;
	CacheElseIf.CNT=0;
	CacheElseIf.TOP=0;
	CacheRem.CNT=0;
	CacheRem.TOP=0;
	CacheSwitch.CNT=0;
	CacheSwitch.TOP=0;
}

void InitLocalVar() {
	int c;
	for ( c=0; c<VARMAXSIZE; c++ ) {			// init Local variable
		LocalDbl[c]=&REG[c];
		LocalInt[c]=&REGINT[c];
	}
}

//----------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------
int ObjectAlign4d( unsigned int n ){ return n; }	// align +4byte
//int ObjectAlign4f( unsigned int n ){ return n; }	// align +4byte
//int ObjectAlign4g( unsigned int n ){ return n; }	// align +4byte
//int ObjectAlign4h( unsigned int n ){ return n; }	// align +4byte
//int ObjectAlign4i( unsigned int n ){ return n; }	// align +4byte
//int ObjectAlign4j( unsigned int n ){ return n; }	// align +4byte
//int ObjectAlign4k( unsigned int n ){ return n; }	// align +4byte
int ObjectAlign6e( unsigned int n ){ return n+n; }	// align +6byte
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
				if ( ( c=='c' ) || ( c=='C' ) ) CB_INT=2;
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
						defaultStrAryN=Eval_atoi( SRC, c );
						if ( SRC[ExecPtr] == ',') {
							c=SRC[++ExecPtr];
							defaultStrArySize=Eval_atoi( SRC, c );
						}
					}
				  DelStrBuffer:
					DeleteStrBuffer();
				}
			} else
			if ( c==0x1B ) {	// fn
				reg=RegVarAliasEx(SRC);
				if ( reg>=0 ) {
					defaultFnAry= reg;
					if ( SRC[ExecPtr] == ',') {
						c=SRC[++ExecPtr];
						defaultFnAryN=Eval_atoi( SRC, c );
						if ( SRC[ExecPtr] == ',') {
							c=SRC[++ExecPtr];
							defaultFnArySize=Eval_atoi( SRC, c );
						}
					}
					goto DelStrBuffer;
				}
			} else { ExecPtr++;  c=SRC[ExecPtr++]; }
		} else
		if ( c==0xFFFFFFF7 ) {
			c=SRC[ExecPtr++];
			if ( c==0x0D ) {	// Break
				c=SRC[ExecPtr++];
				if ( ( c=='0' ) || ( c=='1' ) ) {
					BreakCheck= (c-'0') ;
				}
			} else { ExecPtr++;  c=SRC[ExecPtr++]; }
		} else
		if ( c=='5' ){	// 
				c=SRC[ExecPtr++];
				if ( ( c=='8' ) )  CB_fx5800P = 1;		// fx-5800P mode on
		} else
		if ( c=='9' ){	// 
				c=SRC[ExecPtr++];
				if ( ( c=='8' ) )  CB_fx5800P = 0;		// fx-5800P mode off
		} else
		if ( c==0x7F ) {
			c=SRC[ExecPtr++];
			if ( c==0xFFFFFFF0 ) {	// Graph
				reg=RegVarAliasEx(SRC);
				if ( reg>=0 ) {
					defaultGraphAry= reg;
					if ( SRC[ExecPtr] == ',') {
						c=SRC[++ExecPtr];
						defaultGraphAryN=Eval_atoi( SRC, c );
						if ( SRC[ExecPtr] == ',') {
							c=SRC[++ExecPtr];
							defaultGraphArySize=Eval_atoi( SRC, c );
						}
					}
					goto DelStrBuffer;
				}
			} else
			if ( c==0x40 ) {	// Mat 0  : base 0
				c=SRC[ExecPtr++];
				if ( ( c=='0' ) || ( c=='1' ) ) {
					MatBase= c-'0' ;
				}
			} else
			if ( c==0xFFFFFF8F ) {	// GetKey
				c=SRC[ExecPtr++];
				if ( ( c=='R' ) || ( c=='r' ) ) {
					if ( TimeDsp ) TimeDsp |= 0x2 ; // on  reset GetKey1/2 
				} else {
					if ( TimeDsp ) TimeDsp &= ~0x2 ; // on  cont
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
				c=SRC[ExecPtr++];
				if ( ( c!=0x0F ) || ( SRC[ExecPtr]!=0xFFFFFFE2 ) )  break;
				// Alias Lbl skip
			case 0xFFFFFFE5:	// 
			case 0xFFFFFFE6:	// 
			case 0xFFFFFFE7:	// 
			case 0xFFFFFFFF:	// 
				ExecPtr++;
				break;
		}
		c=SRC[ExecPtr++];
	}
}

void CB_Rem( char *SRC, CchIf *Cache ){
	int i,ii,j,execptr;
	int c=SRC[ExecPtr];
	if ( c=='#' ) { 	// C.Basic command
		Skip_rem( SRC );
		return;
	}
	j=0; i=Cache->TOP;
	while ( j<Cache->CNT ) {
		if ( Cache->Ptr[i]==ExecPtr ) { 	// adrs ok
			if ( i==Cache->TOP ) { ExecPtr=Cache->Adrs[i]; return ; }	// top of cache
			ii=i-1; if ( ii < 0 ) ii=IfCntMax-1;
			execptr=Cache->Adrs[i];	// 2nd- of cache level up
			Cache->Ptr[i]=Cache->Ptr[ii];
			Cache->Adrs[i]=Cache->Adrs[ii];
			Cache->Ptr[ii]=ExecPtr;
			Cache->Adrs[ii]=execptr;
			ExecPtr=execptr; return;
		}
		j++;
		i++; if ( i >= IfCntMax ) i=0;
	}
	execptr=ExecPtr;
	Skip_rem( SRC );
	if ( Cache->CNT < IfCntMax ) Cache->CNT++;
	Cache->TOP--; if ( Cache->TOP<0 ) Cache->TOP=IfCntMax-1;
	Cache->Ptr[Cache->TOP] =execptr;
	Cache->Adrs[Cache->TOP]=ExecPtr;
}

//-----------------------------------------------------------------------------

int CB_CheckLbl( char * SRC ){
	int i,j,len=32;
	int alias_code, org_reg;
	char name[32+1];
	int c=SRC[ExecPtr];
	if ( ( '0'<=c )&&( c<='9' ) ) {
		ExecPtr++;
		return c-'0';
	} else if ( ( ( 'A'<=c )&&( c<='Z' ) ) || ( ( 'a'<=c )&&( c<='z' ) ) ) {
		ExecPtr++;
		return  c-'A'+10;
	} else 	if ( ( c == 0xFFFFFFCD ) || ( c == 0xFFFFFFCE ) ) {	// <r> or Theta
		ExecPtr++;
		return  c-0xFFFFFFCD+10+26;
	} else if ( ( c=='_' ) ) {	// Alias Lbl
		ExecPtr++;
		if ( GetVarName( SRC, &ExecPtr, name, &len) ) {
			for ( i=0; i<=AliasVarMAXLbl; i++ ) {
				if ( ( len == AliasVarCodeLbl[i].len ) && ( AliasVarCodeLbl[i].alias == 0x4040 ) ) {	// @@
					for (j=0; j<len; j++) {
						if ( AliasVarCodeLbl[i].name[j] != name[j] ) break;
					}
					if ( j==len ) return AliasVarCodeLbl[i].org;	// Macth!!
				}
			}
		}
	}
	ExecPtr += GetOpcodeLen( SRC, ExecPtr ,&alias_code );
	for ( i=0; i<=AliasVarMAXLbl; i++ ) {
		if ( AliasVarCodeLbl[i].alias==(short)alias_code ) return AliasVarCodeLbl[i].org;
	}
	{ CB_Error(SyntaxERR); return -1; }	// syntax error
}

void CB_Lbl( char *SRC, int *StackGotoAdrs ){
	int c;
	int label;
	label = CB_CheckLbl( SRC );
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
				c = CB_CheckLbl( SRC );
				if ( c == lc ) return 1 ;
				break;
			case 0xFFFFFFF9:	// 
				c=SRC[ExecPtr++];
				if ( ( c!=0x0F ) || ( SRC[ExecPtr]!=0xFFFFFFE2 ) )  break;
				// Alias Lbl skip
			case 0x7F:	// 
			case 0xFFFFFFF7:	// 
			case 0xFFFFFFE5:	// 
			case 0xFFFFFFE6:	// 
			case 0xFFFFFFE7:	// 
			case 0xFFFFFFFF:	// 
				ExecPtr++;
				break;
		}
	}
	ExecPtr=bptr;
	return 0;
}

int Search_Next( char *SRC );
int Search_WhileEnd( char *SRC );
int Search_LpWhile( char *SRC );
int Search_SwitchEnd( char *SRC );

void CB_Goto( char *SRC, int *StackGotoAdrs, CurrentStk *CurrentStruct ) {
	int c;
	int label;
	int ptr;
	int execbuf;
	int endp;
	label = CB_CheckLbl( SRC );
	if ( label < 0 ) { CB_Error(SyntaxERR); return; }	// syntax error
	
	ptr = StackGotoAdrs[label] ;
	if ( ptr == 0 ) {
		if ( Search_Lbl(SRC, label) == 0 ) { CB_Error(UndefinedLabelERR); return; }	// undefined label error
		ExecPtr++;
		StackGotoAdrs[label]=ExecPtr;
	} else  ExecPtr = ptr ;

	if ( CurrentStruct->CNT <= 0 ) return;  // Not in Loop
	do {
		if ( CurrentStruct->GosubNest[CurrentStruct->CNT-1] < GosubNestN ) break; 	//	Check Gosub level
		switch ( CurrentStruct->TYPE[CurrentStruct->CNT-1] ) {
			case TYPE_For_Next:			// For Next
				c = CurrentStruct->ForPtr-1;
				endp = CurrentStruct->NextAdrs[c];
				if ( endp==0 ) {
					execbuf = ExecPtr;
					if ( Search_Next(SRC) == 0 ) endp=0x7FFFFFF;   // For without Next 
					else {
						endp = ExecPtr;
						CurrentStruct->NextAdrs[c] = endp;
					}
					ExecPtr = execbuf;
				}
				if ( ( CurrentStruct->ForAdrs[c] < ExecPtr ) && ( ExecPtr < endp ) ) return;
				CurrentStruct->ForPtr--;
				break ;
			case TYPE_While_WhileEnd:	// While WhileEnd
				c = CurrentStruct->WhilePtr-1;
				endp = CurrentStruct->WhileEndAdrs[c];
				if ( endp==0 ) {
					execbuf = ExecPtr;
					if ( Search_WhileEnd(SRC) == 0 ) endp=0x7FFFFFF;   // While without WhileEnd 
					else {
						endp = ExecPtr;
						CurrentStruct->WhileEndAdrs[c] = endp;
					}
					ExecPtr = execbuf;
				}
				if ( ( CurrentStruct->WhileAdrs[c] < ExecPtr ) && ( ExecPtr < endp ) ) return;
				CurrentStruct->WhilePtr--;
				break ;
			case TYPE_Do_LpWhile:		// DO LpWhile
				c = CurrentStruct->DoPtr-1;
				endp = CurrentStruct->LpWhileAdrs[c];
				if ( endp==0 ) {
					execbuf = ExecPtr;
					if (  Search_LpWhile(SRC) == 0 ) endp=0x7FFFFFF;   // Do without LpWhile 
					else {
						endp = ExecPtr;
						CurrentStruct->LpWhileAdrs[c] = endp;
					}
					ExecPtr = execbuf;
				}
				if ( ( CurrentStruct->DoAdrs[c] < ExecPtr ) && ( ExecPtr < endp ) ) return;
				CurrentStruct->DoPtr--;
				break ;
			case TYPE_Switch_Case:		// Switch
				c = CurrentStruct->SwitchPtr-1;
				if ( ( CurrentStruct->SwitchAdrs[c] < ExecPtr ) && ( ExecPtr < CurrentStruct->SwitchEndAdrs[c] ) ) return;
				CurrentStruct->SwitchPtr--;
				break ;
			default:
				break;
		}
		CurrentStruct->CNT--;
	} while ( CurrentStruct->CNT > 0 );
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

void CB_If( char *SRC, CchIf *Cache ){
	int c,i,ii,j,stat,execptr;
	int judge;
  loop:
	judge  = CB_EvalCheckZero( SRC );
	c =SRC[ExecPtr];
	if ( ( c == ':'  ) || ( c == 0x0D ) )  { c=SRC[++ExecPtr]; while ( c==' ' ) c=SRC[++ExecPtr]; }
	if ( c == 0x27 ) { Skip_rem(SRC); c=SRC[++ExecPtr]; while ( c==' ' ) c=SRC[++ExecPtr]; }
	if ( ( c == 0xFFFFFFF7 ) && ( SRC[ExecPtr+1] == 0x01 ) ) ExecPtr+=2 ;	// "Then" skip
	if ( judge ) return ; // true
	
	j=0; i=Cache->TOP;
	while ( j<Cache->CNT ) {
		if ( Cache->Ptr[i]==ExecPtr ) { 	// adrs ok
			if ( i==Cache->TOP ) { ExecPtr=Cache->Adrs[i]; return ; }	// top of cache
			ii=i-1; if ( ii < 0 ) ii=IfCntMax-1;
			execptr=Cache->Adrs[i];	// 2nd- of cache level up
			Cache->Ptr[i]=Cache->Ptr[ii];
			Cache->Adrs[i]=Cache->Adrs[ii];
			Cache->Ptr[ii]=ExecPtr;
			Cache->Adrs[ii]=execptr;
			ExecPtr=execptr; return;
		}
		j++;
		i++; if ( i >= IfCntMax ) i=0;
	}
	execptr=ExecPtr;
	stat=Search_ElseIfEnd( SRC );
	if ( stat == 3 ) goto loop; 	// ElseIf
	if ( Cache->CNT < IfCntMax ) Cache->CNT++;
	Cache->TOP--; if ( Cache->TOP<0 ) Cache->TOP=IfCntMax-1;
	Cache->Ptr[Cache->TOP] =execptr;
	Cache->Adrs[Cache->TOP]=ExecPtr;
}

void CB_Else( char *SRC, CchIf *Cache ){
	int i,ii,j,execptr;
	j=0; i=Cache->TOP;
	while ( j<Cache->CNT ) {
		if ( Cache->Ptr[i]==ExecPtr ) { 	// adrs ok
			if ( i==Cache->TOP ) { ExecPtr=Cache->Adrs[i]; return ; }	// top of cache
			ii=i-1; if ( ii < 0 ) ii=IfCntMax-1;
			execptr=Cache->Adrs[i];	// 2nd- of cache level up
			Cache->Ptr[i]=Cache->Ptr[ii];
			Cache->Adrs[i]=Cache->Adrs[ii];
			Cache->Ptr[ii]=ExecPtr;
			Cache->Adrs[ii]=execptr;
			ExecPtr=execptr; return;
		}
		j++;
		i++; if ( i >= IfCntMax ) i=0;
	}
	execptr=ExecPtr;
	Search_IfEnd( SRC );
	if ( Cache->CNT < IfCntMax ) Cache->CNT++;
	Cache->TOP--; if ( Cache->TOP<0 ) Cache->TOP=IfCntMax-1;
	Cache->Ptr[Cache->TOP] =execptr;
	Cache->Adrs[Cache->TOP]=ExecPtr;
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
			case 0x13:	// =>
				Skip_block(SRC);
				break;
			case 0xFFFFFFF7:	// 
				c=SRC[ExecPtr++];
				if ( c == 0x00 ) { 		// If
					Search_IfEnd(SRC);
				} else
				if ( c == 0x04 ) {		// For
					Search_Next(SRC);
				} else
				if ( c == 0x07 ) { 	// Next
					return 1; 
				}
				break;
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

void CB_For( char *SRC ,CurrentStk *CurrentStruct ){
	int c,reg,expbuf;
	if ( CurrentStruct->ForPtr >= StackForMax ) { CB_Error(NestingERR); return; } //  nesting error
	if (CB_INT==1) {		//					------------ INT mode
		CBint_CurrentValue = EvalIntsubTop( SRC );
		c=SRC[ExecPtr];
		if ( c != 0x0E ) { CB_Error(SyntaxERR); return; }	// Syntax error	// ->
		ExecPtr++;
		expbuf=ExecPtr;
		reg=RegVarAliasEx(SRC);
		if ( reg<0 ) { CB_Error(SyntaxERR); return; }	// Syntax error
		ExecPtr=expbuf;
		CurrentStruct->Var[CurrentStruct->ForPtr]=LocalInt[reg];
		CBint_Store(SRC);
		c=SRC[ExecPtr];
		if ( ( c != 0xFFFFFFF7 ) || ( SRC[ExecPtr+1] != 0x05 ) ) { CB_Error(SyntaxERR); return; }	// Syntax error
		ExecPtr+=2;
		CurrentStruct->IntEnd[CurrentStruct->ForPtr] = EvalIntsubTop( SRC );
		c=SRC[ExecPtr];
		if ( ( c == 0xFFFFFFF7 ) && ( SRC[ExecPtr+1] == 0x06 ) ) {	// Step
			ExecPtr+=2;
			CurrentStruct->IntStep[CurrentStruct->ForPtr] = EvalIntsubTop( SRC );
		} else {
			CurrentStruct->IntStep[CurrentStruct->ForPtr] = 1;
		}
		
		CurrentStruct->NextAdrs[CurrentStruct->ForPtr] = 0;
		
		if ( CurrentStruct->IntStep[CurrentStruct->ForPtr] > 0 ) { 	// step +
			if ( CBint_CurrentValue > CurrentStruct->IntEnd[CurrentStruct->ForPtr] ) {  // for next cancel
				if ( Search_Next(SRC) == 0 ) { CB_Error(ForWithoutNextERR); return; }  // For without Next error
				return;
			}
		}
		else {									// step -
			if ( CBint_CurrentValue < CurrentStruct->IntEnd[CurrentStruct->ForPtr] ) {  // for next cancel
				if ( Search_Next(SRC) == 0 ) { CB_Error(ForWithoutNextERR); return; }  // For without Next error
				return;
			}
		}

	} else {			//					------------ Double mode
		CB_CurrentValue.real = EvalsubTopReal( SRC );
		CB_CurrentValue.imag = 0;
		c=SRC[ExecPtr];
		if ( c != 0x0E ) { CB_Error(SyntaxERR); return; }	// Syntax error	// ->
		ExecPtr++;
		expbuf=ExecPtr;
		reg=RegVarAliasEx(SRC);
		if ( reg<0 ) { CB_Error(SyntaxERR); return; }	// Syntax error
		ExecPtr=expbuf;
		CurrentStruct->Var[CurrentStruct->ForPtr]=(int*)LocalDbl[reg];
		CB_Store(SRC);
		c=SRC[ExecPtr];
		if ( ( c != 0xFFFFFFF7 ) || ( SRC[ExecPtr+1] != 0x05 ) ) { CB_Error(SyntaxERR); return; }	// Syntax error
		ExecPtr+=2;
		CurrentStruct->End[CurrentStruct->ForPtr] = EvalsubTopReal( SRC );
		c=SRC[ExecPtr];
		if ( ( c == 0xFFFFFFF7 ) && ( SRC[ExecPtr+1] == 0x06 ) ) {	// Step
			ExecPtr+=2;
			CurrentStruct->Step[CurrentStruct->ForPtr] = EvalsubTopReal( SRC );
		} else {
			CurrentStruct->Step[CurrentStruct->ForPtr] = 1;
		}
		
		CurrentStruct->NextAdrs[CurrentStruct->ForPtr] = 0;
		
		if ( CurrentStruct->Step[CurrentStruct->ForPtr] > 0 ) { 	// step +
			if ( CB_CurrentValue.real > CurrentStruct->End[CurrentStruct->ForPtr] ) { // for next cancel
				if ( Search_Next(SRC) == 0 ) { CB_Error(ForWithoutNextERR); return; }  // For without Next error
				return;
			}
		}
		else {									// step -
			if ( CB_CurrentValue.real < CurrentStruct->End[CurrentStruct->ForPtr] ) { // for next cancel
				if ( Search_Next(SRC) == 0 ) { CB_Error(ForWithoutNextERR); return; }  // For without Next error
				return;
			}
		}
	}
	CurrentStruct->ForAdrs[CurrentStruct->ForPtr] = ExecPtr;
	CurrentStruct->ForPtr++;
	CurrentStruct->TYPE[CurrentStruct->CNT]=TYPE_For_Next;
	CurrentStruct->GosubNest[CurrentStruct->CNT] = GosubNestN;
	CurrentStruct->CNT++;
}

void CB_Next( char *SRC ,CurrentStk *CurrentStruct ){
	double step;
	double *dptr;
	int stepint;
	int i;
	int *iptr;
//	if ( CurrentStruct->ForPtr <= 0 ) { CB_Error(NextWithoutForERR); return; } // Next without for error
	if ( CurrentStruct->ForPtr <= 0 ) { return; } // Next without for through (no error)
	CurrentStruct->ForPtr--;
	CurrentStruct->CNT--;
	if (CB_INT==1) {		//					------------ INT mode
		stepint = CurrentStruct->IntStep[CurrentStruct->ForPtr];
		iptr=CurrentStruct->Var[CurrentStruct->ForPtr];
		(*iptr) += stepint;
		if ( stepint > 0 ) { 	// step +
			if ( *iptr > CurrentStruct->IntEnd[CurrentStruct->ForPtr] ) { (*iptr) -= step; return ;} // exit
		}
		else {					// step -
			if ( *iptr < CurrentStruct->IntEnd[CurrentStruct->ForPtr] ) { (*iptr) -= step; return ;} // exit
		}
	} else {			//					------------ Double mode
		step = CurrentStruct->Step[CurrentStruct->ForPtr];
		dptr=(double*)CurrentStruct->Var[CurrentStruct->ForPtr];
		(*dptr) += step;
		if ( step > 0 ) { 		// step +
			if ( (*dptr) > CurrentStruct->End[CurrentStruct->ForPtr] ) { (*dptr) -= step; return ;} // exit
		}
		else {					// step -
			if ( (*dptr) < CurrentStruct->End[CurrentStruct->ForPtr] ) { (*dptr) -= step; return ;} // exit
		}
	}
	CurrentStruct->NextAdrs[CurrentStruct->ForPtr] = ExecPtr;
	ExecPtr = CurrentStruct->ForAdrs[CurrentStruct->ForPtr];
	CurrentStruct->ForPtr++;		// continue
//	CurrentStruct->TYPE[CurrentStruct->CNT]=TYPE_For_Next;
	CurrentStruct->CNT++;
}

//----------------------------------------------------------------------------------------------
int ObjectAligni4( unsigned int n ){ return n; }	// align +4byte
//int ObjectAligni6a( unsigned int n ){ return n+n; }	// align +6byte
//int ObjectAligni4b( unsigned int n ){ return n; }	// align +4byte
//int ObjectAligni4c( unsigned int n ){ return n; }	// align +4byte
//----------------------------------------------------------------------------------------------
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
			case 0x13:	// =>
				Skip_block(SRC);
				break;
			case 0xFFFFFFF7:	// 
				c=SRC[ExecPtr++];
				if ( c == 0x00 ) {		// If
					Search_IfEnd(SRC);
				} else
				if ( c == 0x08 ) {		// While
					Search_WhileEnd(SRC);
				} else
				if ( c == 0x09 ) {		// WhileEnd
					return 1;
				}
				break;
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
			case 0x13:	// =>
				Skip_block(SRC);
				break;
			case 0xFFFFFFF7:	// 
				c=SRC[ExecPtr++];
				if ( c == 0x00 ) {		// If
					Search_IfEnd(SRC);
				} else
				if ( c == 0x0A ) {		// Do
					Search_LpWhile(SRC);
				} else
				if ( c == 0x0B ) {		// LpWhile
					return 1; 
				}
				break;
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
void CB_While( char *SRC, CurrentStk *CurrentStruct ) {
	int wPtr=ExecPtr;
	if ( CB_EvalCheckZero( SRC ) == 0 ) {		// false
		if ( Search_WhileEnd(SRC) == 0 ) { CB_Error(WhileWithoutWhileEndERR); return; }  // While without WhileEnd error
		return ; // exit
	}
	if ( CurrentStruct->WhilePtr >= StackWhileMax ) { CB_Error(NestingERR); return; }  //  nesting error
	
	CurrentStruct->WhileEndAdrs[CurrentStruct->WhilePtr] = 0;
	CurrentStruct->WhileAdrs[CurrentStruct->WhilePtr] = wPtr;
	CurrentStruct->WhilePtr++;
	CurrentStruct->TYPE[CurrentStruct->CNT]=TYPE_While_WhileEnd;
	CurrentStruct->GosubNest[CurrentStruct->CNT] = GosubNestN;
	CurrentStruct->CNT++;
}

void CB_WhileEnd( char *SRC, CurrentStk *CurrentStruct ) {
	int exitPtr=ExecPtr;
//	if ( CurrentStruct->WhilePtr <= 0 ) { CB_Error(WhileEndWithoutWhileERR); return; }  // WhileEnd without While error
	if ( CurrentStruct->WhilePtr <= 0 ) { return; }  // WhileEnd without While  through (no error)
	CurrentStruct->WhilePtr--;
	CurrentStruct->CNT--;
	CurrentStruct->WhileEndAdrs[CurrentStruct->WhilePtr] = exitPtr;
	ExecPtr = CurrentStruct->WhileAdrs[CurrentStruct->WhilePtr] ;
	if ( CB_EvalCheckZero( SRC ) == 0 ) {		// false
		ExecPtr=exitPtr;
		return ; // exit
	}
	CurrentStruct->WhilePtr++;
//	CurrentStruct->TYPE[CurrentStruct->CNT]=TYPE_While_WhileEnd;
	CurrentStruct->CNT++;
}

void CB_Do( char *SRC, CurrentStk *CurrentStruct ) {
	if ( CurrentStruct->DoPtr >= StackDoMax ) { CB_Error(NestingERR); return; }  //  nesting error

	CurrentStruct->LpWhileAdrs[CurrentStruct->DoPtr] = 0;
	CurrentStruct->DoAdrs[CurrentStruct->DoPtr] = ExecPtr;
	CurrentStruct->DoPtr++;
	CurrentStruct->TYPE[CurrentStruct->CNT]=TYPE_Do_LpWhile;
	CurrentStruct->GosubNest[CurrentStruct->CNT] = GosubNestN;
	CurrentStruct->CNT++;
}

void CB_LpWhile( char *SRC, CurrentStk *CurrentStruct ) {
//	if ( CurrentStruct->DoPtr <= 0 ) { CB_Error(LpWhileWithoutDoERR); return; }  // LpWhile without Do error
	if ( CurrentStruct->DoPtr <= 0 ) { return; }  // LpWhile without Do  through (no error)
	CurrentStruct->DoPtr--;
	CurrentStruct->CNT--;
	if ( CB_EvalCheckZero( SRC ) == 0  ) return ; // exit
	CurrentStruct->LpWhileAdrs[CurrentStruct->DoPtr] = ExecPtr;
	ExecPtr = CurrentStruct->DoAdrs[CurrentStruct->DoPtr] ;				// true
	CurrentStruct->DoPtr++;
//	CurrentStruct->TYPE[CurrentStruct->CNT]=TYPE_Do_LpWhile;
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

void CB_Switch( char *SRC, CurrentStk *CurrentStruct ,CchIf *Cache ) {
	int wPtr,execptr;
	int c,i,ii,j,value;
	value=CB_EvalInt( SRC );
//	c=SRC[ExecPtr];
//	if ( ( c!=0x0D ) && ( c!=':' ) ) { CB_Error(SyntaxERR); return; }	// Syntax error
//	ExecPtr++;
	wPtr=ExecPtr;
	
	j=0; i=Cache->TOP;
	while ( j<Cache->CNT ) {
		if ( Cache->Ptr[i]==ExecPtr ) { 	// adrs ok
			if ( i==Cache->TOP ) { ExecPtr=Cache->Adrs[i]; goto next; }	// top of cache
			ii=i-1; if ( ii < 0 ) ii=IfCntMax-1;
			execptr=Cache->Adrs[i];	// 2nd- of cache level up
			Cache->Ptr[i]=Cache->Ptr[ii];
			Cache->Adrs[i]=Cache->Adrs[ii];
			Cache->Ptr[ii]=ExecPtr;
			Cache->Adrs[ii]=execptr;
			ExecPtr=execptr; goto next;
		}
		j++;
		i++; if ( i >= IfCntMax ) i=0;
	}
	execptr=ExecPtr;
	if ( Search_SwitchEnd(SRC) == 0 ) { CB_Error(SwitchWithoutSwitchEndERR); return; }  // Switch without SwitchEnd error
	if ( Cache->CNT < IfCntMax ) Cache->CNT++;
	Cache->TOP--; if ( Cache->TOP<0 ) Cache->TOP=IfCntMax-1;
	Cache->Ptr[Cache->TOP] =execptr;
	Cache->Adrs[Cache->TOP]=ExecPtr;
  next:
	CurrentStruct->SwitchEndAdrs[CurrentStruct->SwitchPtr] = ExecPtr;	// Break out adrs set
	CurrentStruct->SwitchValue[CurrentStruct->SwitchPtr] = value;
	CurrentStruct->Switchflag[CurrentStruct->SwitchPtr] = 0;	// case through clear
	
	if ( CurrentStruct->SwitchPtr >= StackSwitchMax ) { CB_Error(NestingERR); return; }  //  nesting error
	CurrentStruct->SwitchPtr++;
	CurrentStruct->TYPE[CurrentStruct->CNT]=TYPE_Switch_Case;
	CurrentStruct->GosubNest[CurrentStruct->CNT] = GosubNestN;
	CurrentStruct->CNT++;
	ExecPtr=wPtr;
	
}

void CB_Case( char *SRC, CurrentStk *CurrentStruct ) {
	int exitPtr=ExecPtr;
	int c,value;
	if ( CurrentStruct->SwitchPtr <= 0 ) { CB_Error(CaseWithoutSwitchERR); return; }  // Case without Switch error
	CurrentStruct->SwitchPtr--;

//	c=SRC[ExecPtr];
//	value=Eval_atoi( SRC, c );
	value=CB_EvalInt( SRC );
//	c=SRC[ExecPtr];
//	if ( ( c!=0x0D ) && ( c!=':' ) ) { CB_Error(SyntaxERR); return; }	// Syntax error
//	ExecPtr++;

	if ( CurrentStruct->Switchflag[CurrentStruct->SwitchPtr] == 0 ) {
		if ( value != CurrentStruct->SwitchValue[CurrentStruct->SwitchPtr] ) {		// false
			Search_CaseEnd(SRC);
		} else {	// true
			CurrentStruct->Switchflag[CurrentStruct->SwitchPtr]=1;	// case through set
		}
	}
	CurrentStruct->SwitchPtr++;
//	CurrentStruct->CNT--;
//	CurrentStruct->TYPE[CurrentStruct->CNT]=TYPE_Switch_Case;
//	CurrentStruct->CNT++;
}

void CB_Default( char *SRC, CurrentStk *CurrentStruct ) {
	int exitPtr=ExecPtr;
	int c,i;
	if ( CurrentStruct->SwitchPtr <= 0 ) { CB_Error(DefaultWithoutSwitchERR); return; }  // Default without Switch error
//	CurrentStruct->CNT--;
//	CurrentStruct->TYPE[CurrentStruct->CNT]=TYPE_Switch_Case;
//	CurrentStruct->CNT++;
}

void CB_SwitchEnd( char *SRC, CurrentStk *CurrentStruct ) {
	int exitPtr=ExecPtr;
	int c,i;
	if ( CurrentStruct->SwitchPtr <= 0 ) { CB_Error(SwitchEndWithoutSwitchERR); return; }  // SwitchEnd without Switch error
	CurrentStruct->SwitchPtr--;
	CurrentStruct->CNT--;
}


void CB_Break( char *SRC, CurrentStk *CurrentStruct ) {
	int expbuf=ExecPtr;
	if ( CurrentStruct->CNT <=0 ) { CB_Error(NotLoopERR); return; }  // Not Loop error
	CurrentStruct->CNT--;
	switch ( CurrentStruct->TYPE[CurrentStruct->CNT] ) {
		case TYPE_For_Next:			// For Next
			CurrentStruct->ForPtr--;
			ExecPtr = CurrentStruct->NextAdrs[CurrentStruct->ForPtr];
			if ( ExecPtr==0 ) {
				ExecPtr = expbuf;
				if ( Search_Next(SRC) == 0 ) { CB_Error(ForWithoutNextERR); return; }  // For without Next error
			}
			return ;
		case TYPE_While_WhileEnd:	// While WhileEnd
			CurrentStruct->WhilePtr--;
			ExecPtr = CurrentStruct->WhileEndAdrs[CurrentStruct->WhilePtr];
			if ( ExecPtr==0 ) {
				ExecPtr = expbuf;
				if ( Search_WhileEnd(SRC) == 0 ) { CB_Error(WhileWithoutWhileEndERR); return; }  // While without WhileEnd error
			}
			return ;
		case TYPE_Do_LpWhile:		// DO LpWhile
			CurrentStruct->DoPtr--;
			ExecPtr = CurrentStruct->LpWhileAdrs[CurrentStruct->DoPtr];
			if ( ExecPtr==0 ) {
				ExecPtr = expbuf;
				if ( Search_LpWhile(SRC) == 0 )  { CB_Error(DoWithoutLpWhileERR); return; }  // Do without LpWhile error
			}
			return ;
		case TYPE_Switch_Case:		// Switch
			CurrentStruct->SwitchPtr--;
			ExecPtr = CurrentStruct->SwitchEndAdrs[CurrentStruct->SwitchPtr];
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
	if (CB_INT==1) { CBint_Dsz(SRC) ; return; }
	c=SRC[ExecPtr];
	if ( ( ( 'A'<=c )&&( c<='Z' ) ) || ( ( 'a'<=c )&&( c<='z' ) ) ) {
		ExecPtr++;
		reg=c-'A';
	  regj:
		c=SRC[ExecPtr];
		if ( c=='%' ) {
			ExecPtr++;
			LocalInt[reg][0] --;
			CB_CurrentValue = Int2Cplx( LocalInt[reg][0] );
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
			LocalDbl[reg][0].real --;
			CB_CurrentValue = LocalDbl[reg][0] ;
		}
	} else 
	if ( ( c==0x7F ) && ( ( SRC[ExecPtr+1]==0x40 ) || ( SRC[ExecPtr+1]==0xFFFFFF84 ) || ( SRC[ExecPtr+1]==0x51 ) ) ) {	// Mat or Vct or List
			MatrixOprand( SRC, &reg, &dimA, &dimB );
		Matrix:
			if ( ErrorNo ) {  // error
				if ( MatAry[reg].SizeA == 0 ) ErrorNo=NoMatrixArrayERR;	// No Matrix Array error
				return ;
			}
			CB_CurrentValue.real = ReadMatrix( reg, dimA,dimB ) ;
			CB_CurrentValue.real --;
			WriteMatrix( reg, dimA,dimB, CB_CurrentValue.real ) ;
	} else {
		reg=RegVarAliasEx(SRC);	if ( reg>=0 ) goto regj;
		{ CB_Error(SyntaxERR); return; }	// Syntax error
	}

	c=SRC[ExecPtr];
	if ( ( c==':' ) || ( c==0x0D ) ) {
		ExecPtr++;
		if ( CB_CurrentValue.real ) return ;
		else {
			Skip_block(SRC);
		}
	} else if ( c==0x0C ) {  // dsps
		ExecPtr++;
		if ( CB_Disps(SRC, 2) ) { BreakPtr=ExecPtr ; return ; }  // [AC] break
		if ( CB_CurrentValue.real ) return ;
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
	if (CB_INT==1) { CBint_Isz(SRC) ; return; }
	c=SRC[ExecPtr];
	if ( ( ( 'A'<=c )&&( c<='Z' ) ) || ( ( 'a'<=c )&&( c<='z' ) ) ) {
		ExecPtr++;
		reg=c-'A';
	  regj:
		c=SRC[ExecPtr];
		if ( c=='%' ) {
			ExecPtr++;
			LocalInt[reg][0] ++;
			CB_CurrentValue = Int2Cplx( LocalInt[reg][0] );
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
			LocalDbl[reg][0].real ++;
			CB_CurrentValue = LocalDbl[reg][0] ;
		}
	} else 
	if ( ( c==0x7F ) && ( ( SRC[ExecPtr+1]==0x40 ) || ( SRC[ExecPtr+1]==0xFFFFFF84 ) || ( SRC[ExecPtr+1]==0x51 ) ) ) {	// Mat or Vct or List
			MatrixOprand( SRC, &reg, &dimA, &dimB );
		Matrix:
			if ( ErrorNo ) {  // error
				if ( MatAry[reg].SizeA == 0 ) ErrorNo=NoMatrixArrayERR;	// No Matrix Array error
				return ;
			}
			CB_CurrentValue.real = ReadMatrix( reg, dimA,dimB ) ;
			CB_CurrentValue.real ++;
			WriteMatrix( reg, dimA,dimB, CB_CurrentValue.real ) ;
	} else {
		reg=RegVarAliasEx(SRC);	if ( reg>=0 ) goto regj;
		{ CB_Error(SyntaxERR); return; }	// Syntax error
	}

	c=SRC[ExecPtr];
	if ( ( c==':' ) || ( c==0x0D ) ) {
		ExecPtr++;
		if ( CB_CurrentValue.real ) return ;
		else {
			Skip_block(SRC);
		}
	} else if ( c==0x0C ) {  // dsps
		ExecPtr++;
		if ( CB_Disps(SRC, 2) ) { BreakPtr=ExecPtr ; return ; }  // [AC] break
		if ( CB_CurrentValue.real ) return ;
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
	int dimdim=0;
	
	int c=SRC[ExecPtr];
	if ( ( ( 'A'<=c )&&( c<='Z' ) ) || ( ( 'a'<=c )&&( c<='z' ) ) ) {
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
				if ( c=='%' ) { ExecPtr++;  for ( i=reg; i<=en; i++) { if ( REGtype[i] == 0 ) LocalInt[ i ][0] = CB_CurrentValue.real; }
				} else
				if ( c=='#' ) ExecPtr++;
				for ( i=reg; i<=en; i++) { if ( REGtype[i] == 0 ) LocalDbl[ i ][0] = CB_CurrentValue; }
			}
		} else {					// 
			if ( REGtype[reg] == 1 ) { CB_Error(DuplicateDefERR); return; }	// Duplicate Definition	// const Var
			if ( c=='%' ) { ExecPtr++;  LocalInt[reg][0] = CB_CurrentValue.real ; }
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
		if ( c == 0x40 ) {	// Mat A[a,b]  Vct A[a,b]
			ExecPtr+=2;
			c=SRC[ExecPtr];
			if ( ( ( 'A'<=c )&&( c<='Z' ) ) || ( ( 'a'<=c )&&( c<='z' ) ) ) { reg=c-'A'; ExecPtr++; } 
			else { reg=MatRegVar(SRC); if ( reg<0 ) CB_Error(SyntaxERR) ; } // Syntax error 
			Matrix0:
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
				if ( CB_INT==0) WriteMatrix( reg, dimA, dimB, CB_CurrentValue.real); else Cplx_WriteMatrix( reg, dimA, dimB, CB_CurrentValue);
			}
		} else if ( c==0xFFFFFF84 ) {	//  Vct A[a,b]
			ExecPtr+=2;
			reg=VctRegVar(SRC); if ( reg<0 ) CB_Error(SyntaxERR) ; // Syntax error 
			goto Matrix0;
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
			reg=c+(58-0x6A);
			goto Listj;
			
		} else if ( c == 0x46 ) {	// -> Dim
				ExecPtr+=2;
				if ( ( SRC[ExecPtr]==0x7F ) && ( SRC[ExecPtr+1]==0x46 ) ) { ExecPtr+=2; dimdim=1; }	// {24,18}->dim dim
				if ( ( SRC[ExecPtr]==0x7F ) && ( SRC[ExecPtr+1]==0x51 ) ) {	// n -> Dim List
					ExecPtr+=2;
					if ( CB_CurrentValue.real )  		// 15->Dim List 1
							CB_ListInitsub( SRC, &reg, CB_CurrentValue.real, 0, dimdim );
					else {							//  0->Dim List 1
						reg=ListRegVar( SRC );
						if ( reg>=0 ) DeleteMatrix( reg );
					}
				} else
				if ( ( SRC[ExecPtr]==0x7F ) && ( ( SRC[ExecPtr+1]==0x40 ) || ( SRC[ExecPtr+1]==0xFFFFFF84 ) ) ) {	// {10,5} -> Dim Mat A  -> Dim Vct A
					CB_MatrixInit( SRC, dimdim );
					return ;
				} else {
					if ( CB_CurrentValue.real ) {		// 15->Dim Mat A
						reg=MatRegVar(SRC);
						CB_MatrixInitsubNoMat( SRC, reg, CB_CurrentValue.real, 1, 0, dimdim );
					} else {							//  0->Dim Mat A
						reg=MatRegVar(SRC);
						if ( reg>=0 ) DeleteMatrix( reg );
					}
				}
		} else if ( c == 0x5F ) {	// Ticks
				ExecPtr+=2;
				goto StoreTicks;
		} else if ( c == 0x00 ) {	// Xmin
				Xmin = CB_CurrentValue.real ;
				SetXdotYdot();
				goto Graphj;
		} else if ( c == 0x01 ) {	// Xmax
				Xmax = CB_CurrentValue.real ;
				SetXdotYdot();
				goto Graphj;
		} else if ( c == 0x02 ) {	// Xscl
				Xscl = fabs(CB_CurrentValue.real) ;
				goto Graphj;
		} else if ( c == 0x04 ) {	// Ymin
				Ymin = CB_CurrentValue.real ;
				SetXdotYdot();
				goto Graphj;
		} else if ( c == 0x05 ) {	// Ymax
				Ymax = CB_CurrentValue.real ;
				SetXdotYdot();
				goto Graphj;
		} else if ( c == 0x06) {	// Yscl
				Yscl = fabs(CB_CurrentValue.real) ;
				goto Graphj;
		} else if ( c == 0x08) {	// Thetamin
				TThetamin = CB_CurrentValue.real ;
				goto Graphj;
		} else if ( c == 0x09) {	// Thetamax
				TThetamax = CB_CurrentValue.real ;
				goto Graphj;
		} else if ( c == 0x0A) {	// Thetaptch
				if ( CB_CurrentValue.real == 0 ) { CB_Error(RangeERR); return; }	// Range error
				TThetaptch = CB_CurrentValue.real ;
				goto Graphj;
		} else if ( c == 0x0B ) {	// Xfct
				Xfct = CB_CurrentValue.real ;
				goto Graphj;
		} else if ( c == 0x0C ) {	// Yfct
				Yfct = CB_CurrentValue.real ;
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
		CB_StoreTicks( SRC, CB_CurrentValue.real );
		skip_count=0;
	} else
	if ( c==0xFFFFFFF9 ) {
		c = SRC[ExecPtr+1] ; 
		if ( c == 0x21 ) {	// Xdot
				if ( CB_CurrentValue.real == 0 ) { CB_Error(RangeERR); return; }	// Range error
				ExecPtr+=2;
				Xdot = CB_CurrentValue.real ;
				Xmax = Xmin + Xdot*126.;
		} else
		if ( c == 0x0E ) {	// 123->Const _ABC
			ExecPtr+=2;
			reg=RegVarAliasEx( SRC ) ;
			REGtype[reg]=1;		// const
			if ( c=='%' ) { ExecPtr++;
				LocalInt[reg][0] = CB_CurrentValue.real ;
			} else {
				if ( c=='#' ) ExecPtr++;
				LocalDbl[reg][0] = CB_CurrentValue ;
			}
		} else goto exitj;
	} else { 
	  exitj:
		reg=RegVarAliasEx( SRC );
	  exitj2:
		if ( reg>=0 ) {
			goto aliasj;	// variable alias
		} else { CB_Error(SyntaxERR); return; }	// Syntax error
	}
}

//----------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------
void CB_argNum( char *SRC ) { // 
	int c=1,i,j;
	Argc=0;
	while ( (c!=0)&&(c!=0x0C)&&(c!=0x0D)&&(c!=':') ) {
		if (CB_INT==1) LocalInttmp[Argc]=EvalIntsubTop( SRC ); else if (CB_INT==0) LocalDbltmp[Argc].real=EvalsubTop(  SRC ); else LocalDbltmp[Argc]=Cplx_EvalsubTop(  SRC );
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
void CB_Prog( char *SRC, int *localvarInt, complex *localvarDbl ) { //	Prog "..."
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

	if ( BreakPtr != -8 ) {	// not Stop
		if ( DebugMode == 3 ) {		// step over
			BreakPtr = BreakPtr_bk;
		}
		if ( DebugMode == 4 ) { DebugMode = 2;  BreakPtr = -1; }	// step out
		
		if ( stat ) {
			if ( ( DebugMode == 0 ) && ( DisableDebugMode==0 ) ) BreakPtr = 0;
			if ( ( ErrorNo ) && ( ErrorNo != StackERR ) ) return ;		// error
			else if ( BreakPtr > 0 ) return ;	// break
		}
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

	if ( ErrorNo == StackERR ) { ErrorPtr=ExecPtr;  }
}

void CB_Gosub( char *SRC, int *StackGotoAdrs, int *StackGosubAdrs ){ //	Gosub N
	int c,i,j;
	int label;
	int ptr;
	int execptr=ExecPtr;
	
	label = CB_CheckLbl( SRC );
	if ( label < 0 ) { CB_Error(SyntaxERR); return; }	// syntax error

	c=SRC[ExecPtr];
	if ( c == ',' ) {	// arg
		ExecPtr++;
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
		StackGosubAdrs[GosubNestN] = ExecPtr;	// return adrs
	}
	GosubNestN++;
	if ( GosubNestN > StackGosubMax ) { CB_Error(NestingERR);  return; }	// Nesting  error

	ptr = StackGotoAdrs[label] ;
	if ( ptr == 0 ) {
		if ( Search_Lbl(SRC, label) == 0 ) { CB_Error(UndefinedLabelERR); return; }	// undefined label error
		ExecPtr++;
		StackGotoAdrs[label]=ExecPtr;
	} else  ExecPtr = ptr ;
}

//----------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------
ALIAS_VAR	*AliasVarCode;
ALIAS_VAR	*AliasVarCodeMat;
ALIAS_VAR	*AliasVarCodeLbl;
int AliasVarMAX;
int AliasVarMAXMat;
int AliasVarMAXLbl;
unsigned char IsExtVar=26+6+26-1;

void CB_AliasVarClr(){
	int i;
	for ( i=0; i<ALIASVARMAX; i++ ) {
		AliasVarCode[i].org = -1 ;
		AliasVarCode[i].alias = -1 ;
		AliasVarCode[i].name[0] = '\0' ;
	}
	for ( i=0; i<ALIASVARMAXMAT; i++ ) {
		AliasVarCodeMat[i].org = -1 ;
		AliasVarCodeMat[i].alias = -1 ;
		AliasVarCodeMat[i].name[0] = '\0' ;
	}
	for ( i=0; i<ALIASVARMAXLBL; i++ ) {
		AliasVarCodeLbl[i].org = -1 ;
		AliasVarCodeLbl[i].alias = -1 ;
		AliasVarCodeLbl[i].name[0] = '\0' ;
	}
	AliasVarMAX=-1;
	AliasVarMAXMat=-1;
	AliasVarMAXLbl=-1;
	
	IsExtVar=26+6+26-1;		// Ext Var init
	for ( i=IsExtVar+1; i<VARMAXSIZE; i++ ) {
		REG[i].real=0;
		REG[i].imag=0;
		REGINT[i]=0;
	}
	for ( i=0; i<VARMAXSIZE; i++ ) {
		REGtype[i]=0;
	}
}

void CB_AliasVar( char *SRC ) {	// Alias A=ƒ¿  or A=_ABCD
	int c,i,j,reg,len;
	int alias_code, org_reg;
	char name[32+1];
	
	if ( ( SRC[ExecPtr]==0x7F ) && ( ( SRC[ExecPtr+1]==0x40 ) || ( SRC[ExecPtr+1]==0xFFFFFF84 ) ) ) {	// 	Alias Mat A=ƒ¿  or Mat A=_ABCD / Vct
		ExecPtr+=2;
		c=SRC[ExecPtr];
		reg=RegVar(c);
		org_reg=reg;
		if ( reg>=0 ) {
			ExecPtr++;
			c=SRC[ExecPtr];
			if ( c != '=' ) { CB_Error(SyntaxERR); return; }	// Syntax error
			ExecPtr++;
			c=SRC[ExecPtr];
			if ( c=='_' ) {	//	_ABCDE   var name
				ExecPtr++; len=32;
				if ( GetVarName( SRC, &ExecPtr, name, &len) == 0 ) { CB_Error(SyntaxERR); return; }	// Syntax error
				for ( i=0; i<=AliasVarMAXMat; i++ ) {	// check already 
					if ( ( len == AliasVarCodeMat[i].len ) && ( AliasVarCodeMat[i].alias == 0x4040 ) ) {	// @@
						for (j=0; j<len; j++) {
							if ( AliasVarCodeMat[i].name[j] != name[j] ) break;
						}
						if ( j==len ) return ;	// macth!!
					}
				}
				if ( AliasVarMAXMat >= ALIASVARMAXMAT-1 ) { CB_Error(TooMuchData); return; }
				AliasVarMAXMat++; 
				AliasVarCodeMat[AliasVarMAXMat].org  =org_reg;
				AliasVarCodeMat[AliasVarMAXMat].alias=0x4040;	// @@
				if ( len > MAXNAMELEN ) len=MAXNAMELEN;
				AliasVarCodeMat[AliasVarMAXMat].len=len;
				memcpy( &AliasVarCodeMat[AliasVarMAXMat].name[0], name, len );
			} else {
				i=RegVar(c); if ( i>=0 ) { CB_Error(ArgumentERR); return; }  // Argument error	 ( = default built-in variable )
				len = GetOpcodeLen( SRC, ExecPtr ,&alias_code );
				if ( len == 1 ) alias_code &=0xFF;
				ExecPtr += len;
				for ( i=0; i<=AliasVarMAXMat; i++ ) {	// check already 
					if ( AliasVarCodeMat[i].alias==(short)alias_code ) return ; 	// macth!!
				}
				if ( AliasVarMAXMat >= ALIASVARMAXMAT-1 ) { CB_Error(TooMuchData); return; }
				AliasVarMAXMat++; 
				AliasVarCodeMat[AliasVarMAXMat].org  =org_reg;
				AliasVarCodeMat[AliasVarMAXMat].alias=alias_code;
			}
		}
	} else 
	if ( ( SRC[ExecPtr]==0xFFFFFFE2 ) ) {	// 	Alias Lbl A=ƒ¿  or Lbl A=_ABCD
		ExecPtr++;
		c=SRC[ExecPtr];
		reg=RegVar(c);
		org_reg=reg;
		if ( reg>=0 ) {
			ExecPtr++;
			c=SRC[ExecPtr];
			if ( c != '=' ) { CB_Error(SyntaxERR); return; }	// Syntax error
			ExecPtr++;
			c=SRC[ExecPtr];
			if ( c=='_' ) {	//	_ABCDE   var name
				ExecPtr++; len=32;
				if ( GetVarName( SRC, &ExecPtr, name, &len) == 0 ) { CB_Error(SyntaxERR); return; }	// Syntax error
				for ( i=0; i<=AliasVarMAXLbl; i++ ) {	// check already 
					if ( ( len == AliasVarCodeLbl[i].len ) && ( AliasVarCodeLbl[i].alias == 0x4040 ) ) {	// @@
						for (j=0; j<len; j++) {
							if ( AliasVarCodeLbl[i].name[j] != name[j] ) break;
						}
						if ( j==len ) return ;	// macth!!
					}
				}
				if ( AliasVarMAXLbl >= ALIASVARMAXLBL-1 ) { CB_Error(TooMuchData); return; }
				AliasVarMAXLbl++; 
				AliasVarCodeLbl[AliasVarMAXLbl].org  =org_reg+10;
				AliasVarCodeLbl[AliasVarMAXLbl].alias=0x4040;	// @@
				if ( len > MAXNAMELEN ) len=MAXNAMELEN;
				AliasVarCodeLbl[AliasVarMAXLbl].len=len;
				memcpy( &AliasVarCodeLbl[AliasVarMAXLbl].name[0], name, len );
			} else {
				i=RegVar(c); if ( i>=0 ) { CB_Error(ArgumentERR); return; }  // Argument error	 ( = default built-in variable )
				len = GetOpcodeLen( SRC, ExecPtr ,&alias_code );
				if ( len == 1 ) alias_code &=0xFF;
				ExecPtr += len;
				for ( i=0; i<=AliasVarMAXLbl; i++ ) {	// check already 
					if ( AliasVarCodeLbl[i].alias==(short)alias_code ) return ; 	// macth!!
				}
				if ( AliasVarMAXLbl >= ALIASVARMAXLBL-1 ) { CB_Error(TooMuchData); return; }
				AliasVarMAXLbl++; 
				AliasVarCodeLbl[AliasVarMAXLbl].org  =org_reg+10;
				AliasVarCodeLbl[AliasVarMAXLbl].alias=alias_code;
			}
		}
	} else {
		c=SRC[ExecPtr];						// Alias A=ƒ¿  or A=_ABCD
		reg=RegVar(c);
		org_reg=reg;
		if ( reg>=0 ) {
			ExecPtr++;
			c=SRC[ExecPtr];
			if ( c != '=' ) { CB_Error(SyntaxERR); return; }	// Syntax error
			ExecPtr++;
			c=SRC[ExecPtr];
			if ( c=='_' ) {	//	_ABCDE   var name
				ExecPtr++; len=32;
				if ( GetVarName( SRC, &ExecPtr, name, &len) == 0 ) { CB_Error(SyntaxERR); return; }	// Syntax error
				for ( i=0; i<=AliasVarMAX; i++ ) {	// check already 
					if ( ( len == AliasVarCode[i].len ) && ( AliasVarCode[i].alias == 0x4040 ) ) {	// @@
						for (j=0; j<len; j++) {
							if ( AliasVarCode[i].name[j] != name[j] ) break;
						}
						if ( j==len ) return ;	// macth!!
					}
				}
				if ( AliasVarMAX >= ALIASVARMAX-1 ) { CB_Error(TooMuchData); return; }
				AliasVarMAX++; 
				AliasVarCode[AliasVarMAX].org  =org_reg;
				AliasVarCode[AliasVarMAX].alias=0x4040;	// @@
				if ( len > MAXNAMELEN ) len=MAXNAMELEN;
				AliasVarCode[AliasVarMAX].len=len;
				memcpy( &AliasVarCode[AliasVarMAX].name[0], name, len );
			} else {
				i=RegVar(c); if ( i>=0 ) { CB_Error(ArgumentERR); return; }  // Argument error	 ( = default built-in variable )
				len = GetOpcodeLen( SRC, ExecPtr ,&alias_code );
				if ( len == 1 ) alias_code &=0xFF;
				ExecPtr += len;
				for ( i=0; i<=AliasVarMAX; i++ ) {	// check already 
					if ( AliasVarCode[i].alias==(short)alias_code ) return ; 	// macth!!
				}
				if ( AliasVarMAX >= ALIASVARMAX-1 ) { CB_Error(TooMuchData); return; }
				AliasVarMAX++; 
				AliasVarCode[AliasVarMAX].org  =org_reg;
				AliasVarCode[AliasVarMAX].alias=alias_code;
			}
		}
	}
}


//----------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------
void CB_ResetExecTicks(){
	CB_TicksStart = RTC_GetTicks();	// 
	CB_HiTicksStart = (int)GetTicks32768();	// 
}

int CB_interpreter( char *SRC ) {
	int flag;
	int c;
	int stat;

	char	bk_RoundMODE   = CB_Round.MODE;
	char	bk_RoundDIGIT  = CB_Round.DIGIT;
	char 	bk_ENG         = ENG;	// ENG flag
	char	bk_DrawType    = DrawType;	// 0:connect  1:Plot
	char	bk_Coord       = Coord;	// 0:off 1:on
	char	bk_Grid        = Grid;	// 0:off 1:on
	char	bk_Axes        = Axes;	// 0:off 1:on
	char	bk_Label       = Label;	// 0:off 1:on
	char	bk_Derivative  = Derivative;	// 0:off 1:on
	char	bk_S_L_Style   = S_L_Style;
	char	bk_Angle       = Angle;	// 0:deg   1:rad  2:grad
	char	bk_ComplexMode = ComplexMode;

	char	bk_ACBreak    =ACBreak;		// AC Break on/off
	char	bk_RefreshCtrl=RefreshCtrl;	// 0:no refresh   1: GrphicsCMD refresh     2: all refresh
	char	bk_Refreshtime=Refreshtime;	// Refresh time  (Refreshtime+1)/128s
	char	bk_CB_fx5800P = CB_fx5800P;

	CB_INT = CB_INTDefault;
	Waitcount=DefaultWaitcount;
	MatBase = MatBaseDefault;
	BreakCheck =BreakCheckDefault;	// Break Stop on/off
	ScreenMode = 0;	// Text mode
	UseGraphic = 0;
	PxlMode = 1;		// Pxl  1:set  0:clear	
	BG_Pict_No=0;		// BG-None
	CB_ClrText(SRC);
	ProgEntryN = 0;	// subroutin clear
	GosubNestN = 0;	// Gosub clear
	ErrorPtr = 0;
	ErrorNo = 0;
	CB_CurrentValue.imag = 0;
	ListFileNo  = 0;	// List File No
	ListFilePtr = 0;	// List File Ptr

	defaultStrAry=26;	// <r>
	defaultFnAry=57;		// z
	defaultGraphAry=27;		// Theta
	DeleteMatrix( Mattmp_traceAry );
	if ( MaxMemMode ) DeleteMatrix( Mattmp_clipBuffer );
	
	CB_MatListAnsreg=27;	//	ListAns init
	Bdisp_PutDisp_DD_DrawBusy();
	KeyRecover(); 
	Argc = 0;	// 
	CB_AliasVarClr();
	DeleteMatListAnsAll();	// Ans init	
	for ( c=0; c<3; c++ ) CB_S_Gph_init( c );

	CB_ClrGraphStat();

	CB_TicksAdjust = 0 ;	// 
	CB_HiTicksAdjust = 0 ;	// 
	CB_ResetExecTicks();
	srand( CB_TicksStart ) ;	// rand seed
	
	stat = CB_interpreter_sub( SRC );
	
	ACBreak    =bk_ACBreak;		// AC Break on/off
	RefreshCtrl=bk_RefreshCtrl;	// 0:no refresh   1: GrphicsCMD refresh     2: all refresh
	Refreshtime=bk_Refreshtime;	// Refresh time  (Refreshtime+1)/128s
	CB_fx5800P =bk_CB_fx5800P;

	if ( CB_RecoverSetup ) {
		CB_Round.MODE =bk_RoundMODE;
		CB_Round.DIGIT=bk_RoundDIGIT;
		ENG         = bk_ENG;	// ENG flag
		DrawType    = bk_DrawType;	// 0:connect  1:Plot
		Coord       = bk_Coord;	// 0:off 1:on
		Grid        = bk_Grid;	// 0:off 1:on
		Axes        = bk_Axes;	// 0:off 1:on
		Label       = bk_Label;	// 0:off 1:on
		Derivative  = bk_Derivative;	// 0:off 1:on
		S_L_Style   = bk_S_L_Style;
		Angle       = bk_Angle;	// 0:deg   1:rad  2:grad
		ComplexMode = bk_ComplexMode;
	}
	KeyRecover(); 
	if ( ErrorNo==StackERR ) CB_ErrMsg(StackERR);
	return stat;
}

//----------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------
void  CB_Input( char *SRC ){
	unsigned int key;
	int c;
	complex DefaultValue={0,0};
	int flag=0,flagint=0;
	int reg,bptr,mptr;
	int dimA,dimB,base;
	char buffer[256];
	char*	MatAryC;
	short*	MatAryW;
	int*	MatAryI;
	int		width=255,length=ExpMax-1,option=0,rev=REV_OFF;
	char	spcchr[]={0x20,0,0};
	char buffer2[64];

	KeyRecover();
	HiddenRAM_MatAryStore();	// MatAry ptr -> HiddenRAM
	CB_SelectTextDD();	// Select Text Screen
	if ( CursorX==22 ) CursorX=1;
	if ( CursorX==23 ) Scrl_Y();
	
	c=SRC[ExecPtr];
	if ( c=='(' ) {	// ?option([csrX][,csrY][,width][,spcchr][,length][,R])
		c=SRC[++ExecPtr];
		if ( ( c==')' ) || ( c==0x0E ) ) goto optionexit;
		if ( c!=',' ) {
			CursorX=CB_EvalInt( SRC ); if ( ( CursorX<1 ) || ( 21<CursorX ) ) { CB_Error(ArgumentERR); return ; } // Argument error
			c=SRC[ExecPtr];
			if ( c!=',' ) goto optionexit;
		}
		c=SRC[++ExecPtr];
		if ( c!=',' ) {
			CursorY=CB_EvalInt( SRC ); if ( ( CursorY<1 ) || (  8<CursorY ) ) { CB_Error(ArgumentERR); return ; } // Argument error
			c=SRC[ExecPtr];
			if ( c!=',' ) goto optionexit;
		}
		c=SRC[++ExecPtr];
		if ( c!=',' ) {
			width=CB_EvalInt( SRC ); if ( ( width<1 ) || (  21<width ) ) { CB_Error(ArgumentERR); return ; } // Argument error
			c=SRC[ExecPtr];
			if ( c!=',' ) goto optionexit;
		}
		c=SRC[++ExecPtr];
		if ( c!=',' ) {	
			c=CB_IsStr( SRC, ExecPtr );
			if ( c ) {	// string
				CB_GetLocateStr( SRC, buffer, 256-1 );		// String -> buffer	return 
			} else { CB_Error(ArgumentERR); return ; } // Argument error
			spcchr[0]=buffer[0];
			spcchr[1]=buffer[1];
			spcchr[2]=buffer[2];
			c=SRC[ExecPtr];
			if ( c!=',' ) goto optionexit;
		}
		c=SRC[++ExecPtr];
		if ( c!=',' ) {
			length=CB_EvalInt( SRC ); if ( ( length<1 ) || ( ExpMax-1<length ) ) { CB_Error(ArgumentERR); return ; } // Argument error
			c=SRC[ExecPtr];
			if ( c!=',' ) goto optionexit;
		}
		c=SRC[++ExecPtr];
		if ( c!=')' ) {	
			if ( ( c=='R' ) || ( c=='r' ) ) { // reverse
				ExecPtr++;
				rev=REV_ON;
			}
		}
	  optionexit:
		option=1;
		if ( SRC[ExecPtr]==')' ) ExecPtr++;
	} else {
		locate( CursorX, CursorY); Print((unsigned char*)"?");
		Scrl_Y();
	}
	
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
		if (CB_INT==1) {
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
				} else DefaultValue = Int2Cplx( LocalInt[reg][0] );
			}
		} else {
			if ( c=='%' ) { flagint=1; 
				DefaultValue = Int2Cplx( LocalInt[reg][0] );
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
		if ( ( c == 0x40 ) || ( c == 0xFFFFFF84 ) || ( c == 0x51 ) ) {	// Mat A[a,b] or Vct A[a] or List 1[a]
			MatrixOprand( SRC, &reg, &dimA, &dimB );
		Matrix:
			if ( ErrorNo ) {  // error
				if ( MatAry[reg].SizeA == 0 ) ErrorNo=NoMatrixArrayERR;	// No Matrix Array error
				return ;
			}
			DefaultValue = Cplx_ReadMatrix( reg, dimA, dimB);
			flag=1;
			ExecPtr=bptr;
		} else if ( c == 0x00 ) {	// Xmin
				DefaultValue.real = Xmin ;
				flag=1;
		} else if ( c == 0x01 ) {	// Xmax
				DefaultValue.real = Xmax ;
				flag=1;
		} else if ( c == 0x02 ) {	// Xscl
				DefaultValue.real = Xscl ;
				flag=1;
		} else if ( c == 0x04 ) {	// Ymin
				DefaultValue.real = Ymin ;
				flag=1;
		} else if ( c == 0x05 ) {	// Ymax
				DefaultValue.real = Ymax ;
				flag=1;
		} else if ( c == 0x06) {	// Yscl
				DefaultValue.real = Yscl ;
				flag=1;
		} else if ( c == 0x08) {	// Thetamin
				DefaultValue.real  = TThetamin ;
				flag=1;
		} else if ( c == 0x09) {	// Thetamax
				DefaultValue.real  = TThetamax ;
				flag=1;
		} else if ( c == 0x0A) {	// Thetaptch
				DefaultValue.real  = TThetaptch ;
				flag=1;
		} else if ( c == 0x0B ) {	// Xfct
				DefaultValue.real = Xfct ;
				flag=1;
		} else if ( c == 0x0C ) {	// Yfct
				DefaultValue.real = Yfct ;
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
			if ( MatAry[reg].SizeA == 0 ) { 	//	CB_Error(MemoryERR); return; }	// Memory error
				DimMatrixSub( reg, 8, defaultStrAryN+1-MatBase, defaultStrArySize, MatBase );	// byte matrix
			}
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
				DefaultValue.real = Xdot ;
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
			if ( option ) {
				InputNumC_CB2( CursorX, CursorY, width, length, spcchr, rev, Int2Cplx(0), 0 );	// zero not disp
			} else {
				CB_CurrentValue = InputNumC_CB( CursorX, CursorY, width, length, spcchr, rev, Int2Cplx(0) );
			}
			ExecPtr++;
			if ( CB_INT==1 ) flagint=1;
		  vinp:
			ErrorNo=0; // error cancel
			if ( BreakPtr > 0 ) { ExecPtr=BreakPtr; return ; }
			CBint_CurrentValue = CB_CurrentValue.real ;
			if ( flagint ) {
				CBint_Store( SRC );
				break;
			}
			CB_Store( SRC );
			break;
		case 1:	// ?A value
			if ( option ) {
				CB_CurrentValue = InputNumC_CB2( CursorX, CursorY, width, length, spcchr, rev, DefaultValue, 1 );	// zero disp
			} else {
				buffer2[0]='\0';
				Cplx_sprintGR2( buffer, buffer2, DefaultValue, 22-CursorX, RIGHT_ALIGN, CB_Round.MODE, CB_Round.DIGIT );
				locate( CursorX, CursorY); Print((unsigned char*)buffer);
				if ( buffer2[0] != '\0' ){
					Scrl_Y();
					locate( CursorX, CursorY); Print((unsigned char*)buffer2);
				}
				Scrl_Y();
				CB_CurrentValue = InputNumC_CB1( CursorX, CursorY, width, length, spcchr, rev, DefaultValue );
			}
			goto vinp;
			break;
		case 2:	// ? -> str 
			CB_CurrentStr=buffer;
			CB_CurrentStr[0]='\0';
	Inpj1:	if ( option == 0 ) CursorX=1;
			key=InputStr( CursorX, CursorY, width, CB_CurrentStr, length, spcchr, rev);
			ErrorNo=0; // error cancel
			if ( key==KEY_CTRL_AC  ) { BreakPtr=ExecPtr;  return ; }
			if ( SRC[ExecPtr]==0x0E ) ExecPtr++;	// -> skip
			CB_StorStr( SRC );
			break;
		case 3:	// ?$Mat  ?Str1-20
			MatAryC=MatrixPtr( reg, dimA, dimB );
			OpcodeStringToAsciiString(buffer, MatAryC, 255);
			CB_CurrentStr=MatAryC;
//			CB_Print(CursorX, CursorY, (unsigned char*)buffer);
//			Scrl_Y();
			if ( width > MatAry[reg].SizeB-1 ) width=MatAry[reg].SizeB-1;
			key=InputStr( CursorX, CursorY, width,  CB_CurrentStr, length, spcchr, rev);
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
	if ( option == 0 ) Scrl_Y();
	Bdisp_PutDisp_DD_DrawBusy();
	return ;
}
//----------------------------------------------------------------------------------------------
int iObjectAlign4a( unsigned int n ){ return n; }	// align +4byte
int iObjectAlign4b( unsigned int n ){ return n; }	// align +4byte
int iObjectAlign4c( unsigned int n ){ return n; }	// align +4byte
int iObjectAlign4d( unsigned int n ){ return n; }	// align +4byte
int iObjectAlign4e( unsigned int n ){ return n; }	// align +4byte
int iObjectAlign4f( unsigned int n ){ return n; }	// align +4byte
int iObjectAlign4g( unsigned int n ){ return n; }	// align +4byte
int iObjectAlign4h( unsigned int n ){ return n; }	// align +4byte
int iObjectAlign4i( unsigned int n ){ return n; }	// align +4byte
//int iObjectAlign4j( unsigned int n ){ return n; }	// align +4byte
//int iObjectAlign4k( unsigned int n ){ return n; }	// align +4byte
//----------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------
