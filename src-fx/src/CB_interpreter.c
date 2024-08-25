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

char	FuncType	= 0;
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
int g_exec_ptr=0;		// Basic execute ptr
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
char g_current_prog=0;			// current Prog No
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

double  REGf[4];		// F_Start  F_End  F_pitch
double  VWIN[6][11];			// 
char VWinflag[6];				// VWin flag

//----------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------
int CB_F7sub( char *SRC, int c ) ;

int CB_interpreter_sub( char *SRC ) {
	char cont=1;
	int dspflagtmp=0;
	int dspflagtmp2;
	int c,j;
	int excptr;
	int breakcount=0;

	int StackGosubAdrs[StackGosubMax];
	
	CurrentStk	CurrentStruct;
	
	complex	localvarDbl[ArgcMAX];	//	local var
	int		localvarInt[ArgcMAX];	//	local var

	StackPtr = (int)&cont;
	c = (int)&cont & 0xFFFF;
	if ( Is35E2*0xA000+0x400 > c ) { CB_Error(StackERR); return -1; } //  stack error
	
	ClrCahche();
	
	tmp_Style = -1;
	dspflag=0;

	InitLocalVar();		// init Local variable
	
	for ( c=0; c<ProgLocalN[g_current_prog]; c++ ) {		// set local variable
		j=ProgLocalVar[g_current_prog][c];
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
		if ( g_error_type || BreakPtr ) { 
			if ( BreakPtr == -8 ) goto iend;
			if ( CB_BreakStop() ) return -7 ;
			if ( ( BreakPtr ==  0 ) && ( TryFlag ) ) goto except;
			if ( SRC[g_exec_ptr] == 0x0C ) if ( ( g_error_type==0 ) && ( DebugMode==0 ) ) g_exec_ptr++; // disps
			ClrCahche();
		}
		c=SRC[g_exec_ptr++];
		if ( ( c==':'  ) || ( c==0x0D )|| ( c==0x20 ) ) {
				c=SRC[g_exec_ptr++];
				while ( ( c==0x0D ) || ( c==0x20 ) ) c=SRC[g_exec_ptr++];
				if  ( breakcount==0 ) {
					if ( ( BreakCheck ) && ( KeyScanDownAC() ) ) {	// [AC] break?
						KeyRecover();
						if ( BreakPtr == 0 ) BreakPtr=g_exec_ptr-1;
					} 
					breakcount=BREAKCOUNT;
				} else breakcount--;
		}
		if ( c==0x00 ) { g_exec_ptr--;
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
				c=SRC[g_exec_ptr++];
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
						c=SRC[g_exec_ptr];
						if ( (c!=0)&&(c!=0x0C)&&(c!=0x0D)&&(c!=':') ) { 
							dspflag=2;
							if (CB_INT==1)	CBint_CurrentValue   = EvalIntsubTop( SRC );
							else
							if (CB_INT==0)	CB_CurrentValue.real = EvalsubTop( SRC );
							else			CB_CurrentValue      = Cplx_EvalsubTop( SRC );
							dspflagtmp=dspflag;
							if ( EvalEndCheck( SRC[g_exec_ptr] ) == 0 ) CB_Error(SyntaxERR) ; // Syntax error 
						}
						if ( GosubNestN > 0 ) {				//	return from subroutin 
							g_exec_ptr=StackGosubAdrs[--GosubNestN] ; break; }
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
//					case 0xFFFFFFF0:	// DotShape(
//						CB_DotShape(SRC);
//						dspflag=0;
//						UseGraphic=9;
//						break;
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
						CB_Menu( SRC, StackGotoAdrs, &CurrentStruct );
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
						g_exec_ptr-=2;
						CB_Error(MissingIf); // not Then error 
						break;
						
					case 0x37:	// Try
						CB_Try();
						break;
					case 0x38:	// Except
					  except:
						CB_Except( SRC );
						break;
					case 0x39:	// TryEnd
						CB_TryEnd();
						break;
						
					case 0x2E:			// DispF-Tbl
						CB_DispF_Tbl( SRC );
						break;
					case 0xFFFFFF88:	// VarRange
						CB_VarRange( SRC );
						break;
					
					case 0x2B:			// NormalG
					case 0x2C:			// ThickG
					case 0x2D:			// BrokenThickG
					case 0x3F:			// DotG
					case 0xFFFFFFC8:	// G SelOn
					case 0xFFFFFFC9:	// T SelOn
					case 0xFFFFFFD8:	// G SelOff
					case 0xFFFFFFD9:	// T SelOff
					case 0x63:			// Y=Type
					case 0x64:			// r=Type
					case 0x65:			// ParamType
//					case 0x66:			// 
					case 0x67:			// X=Type
					case 0x68:			// X>Type
					case 0x69:			// X<Type
					case 0x6A:			// Y>Type
					case 0x6B:			// Y<Type
					case 0x6C:			// Y>=Type
					case 0x6D:			// Y<=Type
					case 0x6E:			// X>=Type
					case 0x6F:			// X<=Type
						CB_GraphFunc(SRC,c);
						break;

					case 0x17:	// ACBreak
						if ( ( SRC[g_exec_ptr]==0xFFFFFFF7 ) && ( SRC[g_exec_ptr+1]==0x0E ) ) {	// ACBreak Stop;
							g_exec_ptr+=2;
							ACBreak=0;
							break;
						}
						if ( ACBreak ) {
							BreakPtr=g_exec_ptr; 
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
				c=SRC[g_exec_ptr++];
				switch ( c ) {
					case 0x40:	// Mat
					case 0xFFFFFF84:	// Vct
						dspflagtmp=CB_MatCalc(SRC,c);
						if ( dspflagtmp ) goto Evalexit2;
						break;
					case 0xFFFFFFF0:	// GraphY
						goto fnjmp;
					case 0x51:	// List
					case 0x6A:	// List1
					case 0x6B:	// List2
					case 0x6C:	// List3
					case 0x6D:	// List4
					case 0x6E:	// List5
					case 0x6F:	// List6
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
					case 0x5E :				// RGB(
						CB_RGB( SRC, 0 );
						break;
					case 0x71 :				// HSV(
						CB_RGB( SRC, 1 );
						break;
					case 0x73 :				// HSL(
						CB_RGB( SRC, 2 );
						break;
					default:
						goto Evalexit2;
				}
				break;
				
			case 0xFFFFFFF9:	// F9
				c=SRC[g_exec_ptr++];
				if ( ( 0xFFFFFFC0 <= c ) && ( c <= 0xFFFFFFDF ) && ( c != 0xFFFFFFC6 ) && ( c != 0xFFFFFFD8 ) ) { CB_ML_command( SRC, c ); break; }
				else
				if ( ( 0x38 != c ) && ( 0x3E != c ) && ( 0x34 <= c ) && ( c <= 0x49 ) )  goto strjp;
				else
				if ( ( 0xFFFFFF98 <= c ) && ( c <= 0xFFFFFF9F ) ) break;	// color command   Black/(White)/Magenta/Cyan/Yellow
				switch ( c ) {
					case 0x1B:	// fn
					  fnjmp:
						if ( CB_CheckYfn( SRC ) == 0 ) goto Evalexit2;
						break;
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
						g_exec_ptr-=2;
						CB_Str(SRC) ;
//						dspflag=0;
						break;

					case 0xFFFFFF9B :			// Black
					case 0xFFFFFF9C :			// White
					case 0xFFFFFF9D :			// Magenta
					case 0xFFFFFF9E :			// Cyan
					case 0xFFFFFF9F :			// Yellow
					case 0xFFFFFF9A:			// AxesScale
					case 0x7D :					// ColorClr
					case 0xFFFFFFF4:			// S-L-Thin
					case 0xFFFFFFF3:			// SketchThin
						break;
					case 0xFFFFFF99 :			// Plot/Line-Color
						CB_PlotLineColor( SRC );
						break;
					case 0xFFFFFFBE :			// Back-Color
						CB_BackColor( SRC );
						break;
					case 0xFFFFFFBF :			// Transp-Color
						CB_TransparentColor( SRC );
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
						if ( SRC[g_exec_ptr]=='3' ) { g_exec_ptr++; ENG=3; } // 3 digit separate
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
					case 0xFFFFFF98:	// SetG-Color
					case 0xFFFFFFF5:	// ThinG
						CB_GraphFunc(SRC,c);
						break;

					case 0x10:	// VarList
						CB_VarList( SRC );
						break;
						
					default:
						Evalexit2:
						g_exec_ptr-=2;
						goto Evalexit;
				}
				break;
				
			case 0x27:	// ' rem
				if ( SRC[g_exec_ptr] == '/' ) { g_exec_ptr++; break; }	// '/ execute only C.Basic 
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
				g_exec_ptr--;
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
				if ( CB_Disps(SRC, dspflag) ) BreakPtr=g_exec_ptr ;  // [AC] break
				CB_ResetExecTicks();
//				c=SRC[ExecPtr]; while ( c==0x20 ) c=SRC[++ExecPtr]; // Skip Space
				break;
		
			default:
				g_exec_ptr--;
			  Evalexit:
				excptr=g_exec_ptr;
				dspflag=2;
				dspflagtmp=2;
				if (CB_INT==1)	{	// int
					CBint_CurrentValue = EvalIntsubTop( SRC );
					if ( dspflag>=3 ) {
						g_exec_ptr=excptr;
						ListEvalIntsubTopAns(SRC);	// List calc	dspflag; //	2:nomal  3:mat  4:list
					}
				} else {
					if (CB_INT==0)	CB_CurrentValue.real = EvalsubTop( SRC );		// double
					else 			CB_CurrentValue      = Cplx_EvalsubTop( SRC );	// complex
					if ( dspflag>=3 ) {
						g_exec_ptr=excptr;
						if (CB_INT==0)	ListEvalsubTopAns(SRC);	// List calc	dspflag; //	2:nomal  3:mat  4:list
						else			Cplx_ListEvalsubTopAns(SRC);	// List calc	dspflag; //	2:nomal  3:mat  4:list
					}
				}
				dspflagtmp = dspflag;
				c=SRC[g_exec_ptr]; 
				if (c==0x0E) goto inext1;
				if (c==0x13) goto inext2;
				if ( (c==':')||(c==0x0D) ) goto inext3;
				if ( (c==0x0C)||(c==0x00) ) goto inext3;
				if ( c==0xFFFFFFF9 ) {
					c=SRC[g_exec_ptr+1];
					if ( (0x05<=c)&&(c<=0x07) ) { g_exec_ptr+=2; goto inext3; }	// >DMS // >a+bi // >r_theta
				}
				CB_Error(SyntaxERR) ; // Syntax error 
				goto inext3;
		}
		c=SRC[g_exec_ptr];
//		while ( c==0x20 ) c=SRC[++ExecPtr]; // Skip Space
		if ( c == 0x0E ) { 
		  inext1:
			g_exec_ptr++;
			if (CB_INT==1)	CBint_Store(SRC);	// ->
			else		CB_Store(SRC);		// ->
			dspflagtmp=dspflag+0x10; //	12:nomal  13:mat  14:list
			c=SRC[g_exec_ptr];
			if ( c == 0x0E ) goto inext1;
		}
		if ( c == 0x13 ) {					// =>
		  inext2:
			g_exec_ptr++;
			dspflag=dspflagtmp2;
			dspflagtmp=0;
			if (CB_INT==1)	{ 
				if ( CBint_CurrentValue   == 0 ) Skip_block(SRC);	// false
				CBint_CurrentValue = regint_Ans;
			} else {
				if (CB_INT==0)	{	// double
					if ( CB_CurrentValue.real == 0 ) Skip_block(SRC);	// false
				} else {   //	complex
					if ( CB_CurrentValue.imag != 0 ) { CB_Error(NonRealERR) ; }	// Input value must be a real number
					else
					if ( CB_CurrentValue.real == 0 ) Skip_block(SRC); 	// false
				}
				CB_CurrentValue.real = reg_Ans.real; 
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
	if ( ProgEntryN == 0 ) return CB_end(SRC);
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
//----------------------------------------------------------------------------------------------

void Skip_block( char *SRC ){
	int c;
	while (1){
		c=SRC[g_exec_ptr++];
		switch ( c ) {
			case 0x00:	// <EOF>
				g_exec_ptr--;
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
				g_exec_ptr++;
				break;
		}
	}
}

void Skip_quot( char *SRC ){ // skip "..."
	int c;
	c=SRC[g_exec_ptr-2];
	if ( ( c==0x27 ) || ( c==' ' ) || ( c==0x0D ) || ( c==':' ) ) {
		while (1){
			c=SRC[g_exec_ptr++];
			switch ( c ) {
				case 0x00:	// <EOF>
					g_exec_ptr--;
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
					g_exec_ptr++;
					break;
			}
		}
	} else {
		while (1){
			c=SRC[g_exec_ptr++];
			switch ( c ) {
				case 0x00:	// <EOF>
					g_exec_ptr--;
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
					g_exec_ptr++;
					break;
			}
		}
	}
}

//----------------------------------------------------------------------------------------------
void Skip_rem_sub( char *SRC, int nonopflag ){	// skip '...
	int c,reg;
remloop:
	c=SRC[g_exec_ptr++];
	if ( c=='/' ) { 	// '/ execute C.Basic only
		return;
	} else 
	if ( nonopflag ) goto next;
	else
	if ( c=='#' ) {
		c=SRC[g_exec_ptr++];
		if ( c=='C') {
			c=SRC[g_exec_ptr++];
			if ( c=='B' ) {
				c=SRC[g_exec_ptr++];
				if ( ( c=='i' ) || ( c=='I' ) ) CB_INT=1;
				else
				if ( ( c=='c' ) || ( c=='C' ) ) CB_INT=2;
				else
				if ( ( c=='d' ) || ( c=='D' ) || ( c=='a' ) || ( c=='A' ) ) CB_INT=0;
			}
		} else
		if ( c==0xFFFFFFF9 ) {
			c=SRC[g_exec_ptr++];
			if ( c==0x3F ) {	// Str
				reg=RegVarAliasEx(SRC);
				if ( reg>=0 ) {
					defaultStrAry= reg;
					if ( SRC[g_exec_ptr] == ',') {
						c=SRC[++g_exec_ptr];
						defaultStrAryN=CB_EvalInt( SRC );
						if ( SRC[g_exec_ptr] == ',') {
							c=SRC[++g_exec_ptr];
							defaultStrArySize=CB_EvalInt( SRC );
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
					if ( SRC[g_exec_ptr] == ',') {
						c=SRC[++g_exec_ptr];
						defaultFnAryN=CB_EvalInt( SRC );
						if ( SRC[g_exec_ptr] == ',') {
							c=SRC[++g_exec_ptr];
							defaultFnArySize=CB_EvalInt( SRC );
						}
					}
					goto DelStrBuffer;
				}
			} else { c=SRC[g_exec_ptr++]; }
		} else
		if ( c==0xFFFFFFF7 ) {
			c=SRC[g_exec_ptr++];
			if ( c==0x0D ) {	// Break
				c=SRC[g_exec_ptr++];
				if ( ( c=='0' ) || ( c=='1' ) ) {
					BreakCheck= (c-'0') ;
				}
			} else { c=SRC[g_exec_ptr++]; }
		} else
		if ( c=='5' ){	// 
				c=SRC[g_exec_ptr++];
				if ( ( c=='8' ) )  CB_fx5800P = 1;		// fx-5800P mode on
		} else
		if ( c=='9' ){	// 
				c=SRC[g_exec_ptr++];
				if ( ( c=='8' ) )  CB_fx5800P = 0;		// fx-5800P mode off
		} else
		if ( c=='R' ){	// Root folder
				c = SetRoot2( SRC );
		} else
		if ( c==0x7F ) {
			c=SRC[g_exec_ptr++];
			if ( c==0xFFFFFFF0 ) {	// Graph
				reg=RegVarAliasEx(SRC);
				if ( reg>=0 ) {
					defaultGraphAry= reg;
					if ( SRC[g_exec_ptr] == ',') {
						c=SRC[++g_exec_ptr];
						defaultGraphAryN=CB_EvalInt( SRC );
						if ( SRC[g_exec_ptr] == ',') {
							c=SRC[++g_exec_ptr];
							defaultGraphArySize=CB_EvalInt( SRC );
						}
					}
					goto DelStrBuffer;
				}
			} else
			if ( c==0x40 ) {	// Mat 0  : base 0
				c=SRC[g_exec_ptr++];
				if ( ( c=='0' ) || ( c=='1' ) ) {
					MatBase= c-'0' ;
				}
			} else
			if ( c==0xFFFFFF8F ) {	// GetKey
				c=SRC[g_exec_ptr++];
				if ( ( c=='R' ) || ( c=='r' ) ) {
					if ( TimeDsp ) TimeDsp |= 0x2 ; // on  reset GetKey1/2 
				} else {
					if ( TimeDsp ) TimeDsp &= ~0x2 ; // on  cont
				}
			} else { c=SRC[g_exec_ptr++]; }

		}
	}
  next:
	while (1){
		switch ( c ) {
			case 0x00:	// <EOF>
				g_exec_ptr--;
//			case 0x3A:	// <:>
			case 0x0C:	// dsps
				return ;
			case 0x0D:	// <CR>
				if ( SRC[g_exec_ptr] == 0x27 ) { g_exec_ptr++; goto remloop; }
				return;
			case 0x22:	// "
				Skip_quot(SRC);
				break;
			case 0x7F:	// 
			case 0xFFFFFFF7:	// 
			case 0xFFFFFFF9:	// 
				c=SRC[g_exec_ptr++];
				if ( ( c!=0x0F ) || ( SRC[g_exec_ptr]!=0xFFFFFFE2 ) )  break;
				// Alias Lbl skip
			case 0xFFFFFFE5:	// 
			case 0xFFFFFFE6:	// 
			case 0xFFFFFFE7:	// 
			case 0xFFFFFFFF:	// 
				g_exec_ptr++;
				break;
		}
		c=SRC[g_exec_ptr++];
	}
}

void Skip_rem_no_op( char *SRC ){
	Skip_rem_sub( SRC, 1 );
}

void CB_Rem( char *SRC, CchIf *Cache ){
	int i,ii,j,execptr;
	int c=SRC[g_exec_ptr];
	if ( c=='#' ) { 	// C.Basic command
		Skip_rem_sub( SRC, 0 );
		return;
	}
	j=0; i=Cache->TOP;
	while ( j<Cache->CNT ) {
		if ( Cache->Ptr[i]==g_exec_ptr ) { 	// adrs ok
			if ( i==Cache->TOP ) { g_exec_ptr=Cache->Adrs[i]; return ; }	// top of cache
			ii=i-1; if ( ii < 0 ) ii=IfCntMax-1;
			execptr=Cache->Adrs[i];	// 2nd- of cache level up
			Cache->Ptr[i]=Cache->Ptr[ii];
			Cache->Adrs[i]=Cache->Adrs[ii];
			Cache->Ptr[ii]=g_exec_ptr;
			Cache->Adrs[ii]=execptr;
			g_exec_ptr=execptr; return;
		}
		j++;
		i++; if ( i >= IfCntMax ) i=0;
	}
	execptr=g_exec_ptr;
	Skip_rem_sub( SRC, 0 );
	if ( Cache->CNT < IfCntMax ) Cache->CNT++;
	Cache->TOP--; if ( Cache->TOP<0 ) Cache->TOP=IfCntMax-1;
	Cache->Ptr[Cache->TOP] =execptr;
	Cache->Adrs[Cache->TOP]=g_exec_ptr;
}

//-----------------------------------------------------------------------------

int CB_CheckLbl( char * SRC ){
	int i,j,len=32;
	int alias_code, org_reg;
	char name[32+1];
	int c=SRC[g_exec_ptr];
	if ( ( '0'<=c )&&( c<='9' ) ) {
		g_exec_ptr++;
		return c-'0';
	} else if ( ( ( 'A'<=c )&&( c<='Z' ) ) || ( ( 'a'<=c )&&( c<='z' ) ) ) {
		g_exec_ptr++;
		return  c-'A'+10;
	} else 	if ( ( c == 0xFFFFFFCD ) || ( c == 0xFFFFFFCE ) ) {	// <r> or Theta
		g_exec_ptr++;
		return  c-0xFFFFFFCD+10+26;
	} else if ( ( c=='_' ) ) {	// Alias Lbl
		g_exec_ptr++;
		if ( GetVarName( SRC, &g_exec_ptr, name, &len) ) {
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
	g_exec_ptr += GetOpcodeLen( SRC, g_exec_ptr ,&alias_code );
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
	if ( StackGotoAdrs[label] == 0 ) StackGotoAdrs[label]=g_exec_ptr;
}

int Search_Lbl( char *SRC, int lc ){
	int c;
	int bptr=g_exec_ptr;
	g_exec_ptr=0;
	while (1){
		c=SRC[g_exec_ptr++];
		switch ( c ) {
			case 0x00:	// <EOF>
				g_exec_ptr=bptr;
				return 0 ;
				break;
			case 0x22:	// "
				Skip_quot(SRC);
				break;
			case 0x27:	// ' rem
				Skip_rem_no_op(SRC);
				break;
			case 0xFFFFFFE2:	// Lbl
				c = CB_CheckLbl( SRC );
				if ( c == lc ) return 1 ;
				break;
			case 0xFFFFFFF9:	// 
				c=SRC[g_exec_ptr++];
				if ( ( c!=0x0F ) || ( SRC[g_exec_ptr]!=0xFFFFFFE2 ) )  break;
				// Alias Lbl skip
			case 0x7F:	// 
			case 0xFFFFFFF7:	// 
			case 0xFFFFFFE5:	// 
			case 0xFFFFFFE6:	// 
			case 0xFFFFFFE7:	// 
			case 0xFFFFFFFF:	// 
				g_exec_ptr++;
				break;
		}
	}
	g_exec_ptr=bptr;
	return 0;
}

int Search_Next( char *SRC );
int Search_WhileEnd( char *SRC );
int Search_LpWhile( char *SRC );
int Search_SwitchEnd( char *SRC );

void CB_Goto_sub( char *SRC, int *StackGotoAdrs, CurrentStk *CurrentStruct ) {
	int c;
	int ptr;
	int execbuf;
	int endp;
	do {
		if ( CurrentStruct->GosubNest[CurrentStruct->CNT-1] < GosubNestN ) break; 	//	Check Gosub level
		switch ( CurrentStruct->TYPE[CurrentStruct->CNT-1] ) {
			case TYPE_For_Next:			// For Next
				c = CurrentStruct->ForPtr-1;
				endp = CurrentStruct->NextAdrs[c];
				if ( endp==0 ) {
					execbuf = g_exec_ptr;
					g_exec_ptr = CurrentStruct->ForAdrs[c];
					if ( Search_Next(SRC) == 0 ) endp=0x7FFFFFF;   // For without Next 
					else {
						endp = g_exec_ptr;
						CurrentStruct->NextAdrs[c] = endp;
					}
					g_exec_ptr = execbuf;
				}
				if ( ( CurrentStruct->ForAdrs[c] < g_exec_ptr ) && ( g_exec_ptr < endp ) ) return;
				CurrentStruct->ForPtr--;
				break ;
			case TYPE_While_WhileEnd:	// While WhileEnd
				c = CurrentStruct->WhilePtr-1;
				endp = CurrentStruct->WhileEndAdrs[c];
				if ( endp==0 ) {
					execbuf = g_exec_ptr;
					g_exec_ptr = CurrentStruct->WhileAdrs[c];
					if ( Search_WhileEnd(SRC) == 0 ) endp=0x7FFFFFF;   // While without WhileEnd 
					else {
						endp = g_exec_ptr;
						CurrentStruct->WhileEndAdrs[c] = endp;
					}
					g_exec_ptr = execbuf;
				}
				if ( ( CurrentStruct->WhileAdrs[c] < g_exec_ptr ) && ( g_exec_ptr < endp ) ) return;
				CurrentStruct->WhilePtr--;
				break ;
			case TYPE_Do_LpWhile:		// DO LpWhile
				c = CurrentStruct->DoPtr-1;
				endp = CurrentStruct->LpWhileAdrs[c];
				if ( endp==0 ) {
					execbuf = g_exec_ptr;
					g_exec_ptr = CurrentStruct->DoAdrs[c];
					if (  Search_LpWhile(SRC) == 0 ) endp=0x7FFFFFF;   // Do without LpWhile 
					else {
						endp = g_exec_ptr;
						CurrentStruct->LpWhileAdrs[c] = endp;
					}
					g_exec_ptr = execbuf;
				}
				if ( ( CurrentStruct->DoAdrs[c] < g_exec_ptr ) && ( g_exec_ptr < endp ) ) return;
				CurrentStruct->DoPtr--;
				break ;
			case TYPE_Switch_Case:		// Switch
				c = CurrentStruct->SwitchPtr-1;
				endp = CurrentStruct->SwitchEndAdrs[c];
				if ( endp==0 ) {
					execbuf = g_exec_ptr;
					g_exec_ptr = CurrentStruct->SwitchAdrs[c];
					if (  Search_SwitchEnd(SRC) == 0 ) endp=0x7FFFFFF;   // Switch without SwitchEnd
					else {
						endp = g_exec_ptr;
						CurrentStruct->SwitchEndAdrs[c] = endp;
					}
					g_exec_ptr = execbuf;
				}
				if ( ( CurrentStruct->SwitchAdrs[c] < g_exec_ptr ) && ( g_exec_ptr < endp ) ) return;
				CurrentStruct->SwitchPtr--;
				break ;
			default:
				break;
		}
		CurrentStruct->CNT--;
	} while ( CurrentStruct->CNT > 0 );
}

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
		if ( Search_Lbl(SRC, label) == 0 ) { CB_Error(UndefinedLabel); return; }	// undefined label error
		g_exec_ptr++;
		StackGotoAdrs[label]=g_exec_ptr;
	} else  g_exec_ptr = ptr ;

	if ( CurrentStruct->CNT > 0 ) CB_Goto_sub( SRC, StackGotoAdrs, CurrentStruct ) ;
}

//----------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void Search_IfEnd( char *SRC ){
	int c;
	while (1){
		c=SRC[g_exec_ptr++];
		switch ( c ) {
			case 0x00:	// <EOF>
				g_exec_ptr--;
				return  ;
			case 0x22:	// "
				Skip_quot(SRC);
				break;
			case 0x27:	// ' rem
				Skip_rem_no_op(SRC);
				break;
			case 0xFFFFFFF7:	// 
				c=SRC[g_exec_ptr++];
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
				g_exec_ptr++;
				break;
		}
	}
	return ;
}

int Search_ElseIfEnd( char *SRC ){
	int c;
	while (1){	// Search  Else or IfEnd
		c=SRC[g_exec_ptr++];
		switch ( c ) {
			case 0x00:	// <EOF>
				g_exec_ptr--;
				return 0 ;
			case 0x22:	// "
				Skip_quot(SRC);
				break;
			case 0x27:	// ' rem
				Skip_rem_no_op(SRC);
				break;
			case 0xFFFFFFF7:	// 
				c=SRC[g_exec_ptr++];
				if ( c == 0x00 ) {			// If
					Search_IfEnd(SRC);
					break;
				} else
				if ( c == 0x02 ) return c ; 	// Else
				else
				if ( c == 0x03 ) return c ; 	// IfEnd
				else
				if ( c == 0x0F ) return c ; 	// ElseIf
				break;
			case 0x0000007F:	// 
			case 0xFFFFFFF9:	// 
			case 0xFFFFFFE5:	// 
			case 0xFFFFFFE6:	// 
			case 0xFFFFFFE7:	// 
//			case 0xFFFFFFFF:	// 
				g_exec_ptr++;
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
	c =SRC[g_exec_ptr];
	if ( ( c == ':'  ) || ( c == 0x0D ) )  { c=SRC[++g_exec_ptr]; while ( c==' ' ) c=SRC[++g_exec_ptr]; }
	if ( c == 0x27 ) { Skip_rem_no_op(SRC); c=SRC[g_exec_ptr]; while ( c==' ' ) c=SRC[++g_exec_ptr]; }
	if ( ( c == 0xFFFFFFF7 ) && ( SRC[g_exec_ptr+1] == 0x01 ) ) g_exec_ptr+=2 ;	// "Then" skip
	if ( judge ) return ; // true
	
	j=0; i=Cache->TOP;
	while ( j<Cache->CNT ) {
		if ( Cache->Ptr[i]==g_exec_ptr ) { 	// adrs ok
			if ( i==Cache->TOP ) { g_exec_ptr=Cache->Adrs[i]; goto exit; }	// top of cache
			ii=i-1; if ( ii < 0 ) ii=IfCntMax-1;
			execptr=Cache->Adrs[i];	// 2nd- of cache level up
			Cache->Ptr[i]=Cache->Ptr[ii];
			Cache->Adrs[i]=Cache->Adrs[ii];
			Cache->Ptr[ii]=g_exec_ptr;
			Cache->Adrs[ii]=execptr;
			g_exec_ptr=execptr;
		  exit:
			if ( SRC[g_exec_ptr-1]==0x0F ) goto loop;	// ElseIf
			return;
		}
		j++;
		i++; if ( i >= IfCntMax ) i=0;
	}
	execptr=g_exec_ptr;
	stat=Search_ElseIfEnd( SRC );
	if ( Cache->CNT < IfCntMax ) Cache->CNT++;
	Cache->TOP--; if ( Cache->TOP<0 ) Cache->TOP=IfCntMax-1;
	Cache->Ptr[Cache->TOP] =execptr;
	Cache->Adrs[Cache->TOP]=g_exec_ptr;
	if ( stat == 0x0F )  goto loop;	// ElseIf
}

void CB_Else( char *SRC, CchIf *Cache ){
	int i,ii,j,execptr;
	j=0; i=Cache->TOP;
	while ( j<Cache->CNT ) {
		if ( Cache->Ptr[i]==g_exec_ptr ) { 	// adrs ok
			if ( i==Cache->TOP ) { g_exec_ptr=Cache->Adrs[i]; return ; }	// top of cache
			ii=i-1; if ( ii < 0 ) ii=IfCntMax-1;
			execptr=Cache->Adrs[i];	// 2nd- of cache level up
			Cache->Ptr[i]=Cache->Ptr[ii];
			Cache->Adrs[i]=Cache->Adrs[ii];
			Cache->Ptr[ii]=g_exec_ptr;
			Cache->Adrs[ii]=execptr;
			g_exec_ptr=execptr; return;
		}
		j++;
		i++; if ( i >= IfCntMax ) i=0;
	}
	execptr=g_exec_ptr;
	Search_IfEnd( SRC );
	if ( Cache->CNT < IfCntMax ) Cache->CNT++;
	Cache->TOP--; if ( Cache->TOP<0 ) Cache->TOP=IfCntMax-1;
	Cache->Ptr[Cache->TOP] =execptr;
	Cache->Adrs[Cache->TOP]=g_exec_ptr;
}

//-----------------------------------------------------------------------------
int Search_Next( char *SRC ){
	int c;
	while (1){
		c=SRC[g_exec_ptr++];
		switch ( c ) {
			case 0x00:	// <EOF>
				g_exec_ptr--;
				return 0 ;
			case 0x22:	// "
				Skip_quot(SRC);
				break;
			case 0x27:	// ' rem
				Skip_rem_no_op(SRC);
				break;
			case 0x13:	// =>
				Skip_block(SRC);
				break;
			case 0xFFFFFFF7:	// 
				c=SRC[g_exec_ptr++];
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
				g_exec_ptr++;
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
		c=SRC[g_exec_ptr];
		if ( c != 0x0E ) { CB_Error(SyntaxERR); return; }	// Syntax error	// ->
		g_exec_ptr++;
		expbuf=g_exec_ptr;
		reg=RegVarAliasEx(SRC);
		if ( reg<0 ) { CB_Error(SyntaxERR); return; }	// Syntax error
		if ( SRC[g_exec_ptr] == '#' ) { CB_CurrentValue.real = CBint_CurrentValue; goto forDBL; }
	  forINT:
		g_exec_ptr=expbuf;
		CurrentStruct->Var[CurrentStruct->ForPtr]=LocalInt[reg];
		CBint_Store(SRC);
		c=SRC[g_exec_ptr];
		if ( ( c != 0xFFFFFFF7 ) || ( SRC[g_exec_ptr+1] != 0x05 ) ) { CB_Error(SyntaxERR); return; }	// Syntax error
		g_exec_ptr+=2;
		CurrentStruct->IntEnd[CurrentStruct->ForPtr] = CB_EvalInt( SRC );
		c=SRC[g_exec_ptr];
		if ( ( c == 0xFFFFFFF7 ) && ( SRC[g_exec_ptr+1] == 0x06 ) ) {	// Step
			g_exec_ptr+=2;
			CurrentStruct->IntStep[CurrentStruct->ForPtr] = CB_EvalInt( SRC );
		} else {
			CurrentStruct->IntStep[CurrentStruct->ForPtr] = 1;
		}
		
		CurrentStruct->NextAdrs[CurrentStruct->ForPtr] = 0;
		CurrentStruct->ForType[CurrentStruct->ForPtr]  = 1;	// int mode
		
		if ( CurrentStruct->IntStep[CurrentStruct->ForPtr] > 0 ) { 	// step +
			if ( CBint_CurrentValue > CurrentStruct->IntEnd[CurrentStruct->ForPtr] ) {  // for next cancel
				if ( Search_Next(SRC) == 0 ) { CB_Error(MissingNext); return; }  // For without Next error
				return;
			}
		}
		else {									// step -
			if ( CBint_CurrentValue < CurrentStruct->IntEnd[CurrentStruct->ForPtr] ) {  // for next cancel
				if ( Search_Next(SRC) == 0 ) { CB_Error(MissingNext); return; }  // For without Next error
				return;
			}
		}

	} else {			//					------------ Double mode
		CB_CurrentValue.real = EvalsubTopReal( SRC );
		c=SRC[g_exec_ptr];
		if ( c != 0x0E ) { CB_Error(SyntaxERR); return; }	// Syntax error	// ->
		g_exec_ptr++;
		expbuf=g_exec_ptr;
		reg=RegVarAliasEx(SRC);
		if ( reg<0 ) { CB_Error(SyntaxERR); return; }	// Syntax error
		if ( SRC[g_exec_ptr] == '%' ) { CBint_CurrentValue = CB_CurrentValue.real; goto forINT; }
	 forDBL:
		g_exec_ptr=expbuf;
		CurrentStruct->Var[CurrentStruct->ForPtr]=(int*)LocalDbl[reg];
		CB_CurrentValue.imag = 0;
		CB_Store(SRC);
		c=SRC[g_exec_ptr];
		if ( ( c != 0xFFFFFFF7 ) || ( SRC[g_exec_ptr+1] != 0x05 ) ) { CB_Error(SyntaxERR); return; }	// Syntax error
		g_exec_ptr+=2;
		CurrentStruct->End[CurrentStruct->ForPtr] = CB_EvalDbl( SRC );
		c=SRC[g_exec_ptr];
		if ( ( c == 0xFFFFFFF7 ) && ( SRC[g_exec_ptr+1] == 0x06 ) ) {	// Step
			g_exec_ptr+=2;
			CurrentStruct->Step[CurrentStruct->ForPtr] = CB_EvalDbl( SRC );
		} else {
			CurrentStruct->Step[CurrentStruct->ForPtr] = 1;
		}
		
		CurrentStruct->NextAdrs[CurrentStruct->ForPtr] = 0;
		CurrentStruct->ForType[CurrentStruct->ForPtr]  = 0;	// dbl mode
		
		if ( CurrentStruct->Step[CurrentStruct->ForPtr] > 0 ) { 	// step +
			if ( CB_CurrentValue.real > CurrentStruct->End[CurrentStruct->ForPtr] ) { // for next cancel
				if ( Search_Next(SRC) == 0 ) { CB_Error(MissingNext); return; }  // For without Next error
				return;
			}
		}
		else {									// step -
			if ( CB_CurrentValue.real < CurrentStruct->End[CurrentStruct->ForPtr] ) { // for next cancel
				if ( Search_Next(SRC) == 0 ) { CB_Error(MissingNext); return; }  // For without Next error
				return;
			}
		}
	}
	CurrentStruct->ForAdrs[CurrentStruct->ForPtr] = g_exec_ptr;
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
	int exitPtr;
//	if ( CurrentStruct->ForPtr <= 0 ) { CB_Error(MissingFor); return; } // Next without for error
	if ( CurrentStruct->ForPtr <= 0 ) { return; } // Next without for through (no error)
	CurrentStruct->ForPtr--;
	CurrentStruct->CNT--;
	if (CurrentStruct->ForType[CurrentStruct->ForPtr]) {		//					------------ INT mode
		stepint = CurrentStruct->IntStep[CurrentStruct->ForPtr];
		iptr=CurrentStruct->Var[CurrentStruct->ForPtr];
		(*iptr) += stepint;
		if ( stepint > 0 ) { 	// step +
			if ( (*iptr) > CurrentStruct->IntEnd[CurrentStruct->ForPtr] ) { (*iptr) -= stepint; goto exit;} // exit
		}
		else {					// step -
			if ( (*iptr) < CurrentStruct->IntEnd[CurrentStruct->ForPtr] ) { (*iptr) -= stepint; goto exit;} // exit
		}
	} else {			//					------------ Double mode
		step = CurrentStruct->Step[CurrentStruct->ForPtr];
		dptr=(double*)CurrentStruct->Var[CurrentStruct->ForPtr];
		(*dptr) += step;
		if ( step > 0 ) { 		// step +
			if ( (*dptr) > CurrentStruct->End[CurrentStruct->ForPtr] ) { (*dptr) -= step; goto exit;} // exit
		}
		else {					// step -
			if ( (*dptr) < CurrentStruct->End[CurrentStruct->ForPtr] ) { (*dptr) -= step;
		  exit:
		  	if ( SRC[g_exec_ptr-3] == 0x13 ) {	// =>
				exitPtr=g_exec_ptr;
				if ( Search_Next(SRC) == 0 ) g_exec_ptr=exitPtr;	// For without Next
			}
			return ;} // exit
		}
	}
	CurrentStruct->NextAdrs[CurrentStruct->ForPtr] = g_exec_ptr;
	g_exec_ptr = CurrentStruct->ForAdrs[CurrentStruct->ForPtr];
	CurrentStruct->ForPtr++;		// continue
//	CurrentStruct->TYPE[CurrentStruct->CNT]=TYPE_For_Next;
	CurrentStruct->CNT++;
}

//----------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------
int Search_WhileEnd( char *SRC ){
	int c;
	while (1){
		c=SRC[g_exec_ptr++];
		switch ( c ) {
			case 0x00:	// <EOF>
				g_exec_ptr--;
				return 0 ;
			case 0x22:	// "
				Skip_quot(SRC);
				break;
			case 0x27:	// ' rem
				Skip_rem_no_op(SRC);
				break;
			case 0x13:	// =>
				Skip_block(SRC);
				break;
			case 0xFFFFFFF7:	// 
				c=SRC[g_exec_ptr++];
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
				g_exec_ptr++;
				break;
		}
	}
	return 0;
}
int Search_LpWhile( char *SRC ){
	unsigned int c;
	while (1){
		c=SRC[g_exec_ptr++];
		switch ( c ) {
			case 0x00:	// <EOF>
				g_exec_ptr--;
				return 0 ;
			case 0x22:	// "
				Skip_quot(SRC);
				break;
			case 0x27:	// ' rem
				Skip_rem_no_op(SRC);
				break;
			case 0x13:	// =>
				Skip_block(SRC);
				break;
			case 0xFFFFFFF7:	// 
				c=SRC[g_exec_ptr++];
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
				g_exec_ptr++;
				break;
		}
	}
	return 0;
}
void CB_While( char *SRC, CurrentStk *CurrentStruct ) {
	int wPtr=g_exec_ptr;
	if ( CB_EvalCheckZero( SRC ) == 0 ) {		// false
		if ( Search_WhileEnd(SRC) == 0 ) { CB_Error(MissingWhileEnd); return; }  // While without WhileEnd error
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
	int exitPtr=g_exec_ptr;
//	if ( CurrentStruct->WhilePtr <= 0 ) { CB_Error(MissingWhile); return; }  // WhileEnd without While error
	if ( CurrentStruct->WhilePtr <= 0 ) { return; }  // WhileEnd without While  through (no error)
	CurrentStruct->WhilePtr--;
	CurrentStruct->CNT--;
	CurrentStruct->WhileEndAdrs[CurrentStruct->WhilePtr] = exitPtr;
	g_exec_ptr = CurrentStruct->WhileAdrs[CurrentStruct->WhilePtr] ;
	if ( CB_EvalCheckZero( SRC ) == 0 ) {		// false
		g_exec_ptr=exitPtr;
		if ( SRC[g_exec_ptr-3] == 0x13 ) {	// =>
			if ( Search_WhileEnd(SRC) == 0 ) g_exec_ptr=exitPtr;  // While without WhileEnd
		}
		return ; // exit
	}
	CurrentStruct->WhilePtr++;
//	CurrentStruct->TYPE[CurrentStruct->CNT]=TYPE_While_WhileEnd;
	CurrentStruct->CNT++;
}

void CB_Do( char *SRC, CurrentStk *CurrentStruct ) {
	if ( CurrentStruct->DoPtr >= StackDoMax ) { CB_Error(NestingERR); return; }  //  nesting error

	CurrentStruct->LpWhileAdrs[CurrentStruct->DoPtr] = 0;
	CurrentStruct->DoAdrs[CurrentStruct->DoPtr] = g_exec_ptr;
	CurrentStruct->DoPtr++;
	CurrentStruct->TYPE[CurrentStruct->CNT]=TYPE_Do_LpWhile;
	CurrentStruct->GosubNest[CurrentStruct->CNT] = GosubNestN;
	CurrentStruct->CNT++;
}

void CB_LpWhile( char *SRC, CurrentStk *CurrentStruct ) {
	int exitPtr;
//	if ( CurrentStruct->DoPtr <= 0 ) { CB_Error(MissingDo); return; }  // LpWhile without Do error
	if ( CurrentStruct->DoPtr <= 0 ) { return; }  // LpWhile without Do  through (no error)
	CurrentStruct->DoPtr--;
	CurrentStruct->CNT--;
	if ( CB_EvalCheckZero( SRC ) == 0  ) {
		if ( SRC[g_exec_ptr-3] == 0x13 ) {	// =>
			exitPtr=g_exec_ptr;
			if ( Search_LpWhile(SRC) == 0 )  g_exec_ptr=exitPtr;  // Do without LpWhile
		}
		return ; // exit
	}
	CurrentStruct->LpWhileAdrs[CurrentStruct->DoPtr] = g_exec_ptr;
	g_exec_ptr = CurrentStruct->DoAdrs[CurrentStruct->DoPtr] ;				// true
	CurrentStruct->DoPtr++;
//	CurrentStruct->TYPE[CurrentStruct->CNT]=TYPE_Do_LpWhile;
	CurrentStruct->CNT++;
}

//-----------------------------------------------------------------------------
int Search_SwitchEnd( char *SRC ){
	unsigned int c;
	while (1){
		c=SRC[g_exec_ptr++];
		switch ( c ) {
			case 0x00:	// <EOF>
				g_exec_ptr--;
				return 0 ;
			case 0x22:	// "
				Skip_quot(SRC);
				break;
			case 0x27:	// ' rem
				Skip_rem_no_op(SRC);
				break;
			case 0xFFFFFFF7:	// 
				if ( SRC[g_exec_ptr] == 0xFFffFFEA ) { g_exec_ptr++;				// Switch
					Search_SwitchEnd(SRC);
					break;
				}
				if ( SRC[g_exec_ptr] == 0xFFFFFFED ) { g_exec_ptr++; return 1; }	// SwitchEnd
			case 0x7F:	// 
			case 0xFFFFFFF9:	// 
			case 0xFFFFFFE5:	// 
			case 0xFFFFFFE6:	// 
			case 0xFFFFFFE7:	// 
//			case 0xFFFFFFFF:	// 
				g_exec_ptr++;
				break;
		}
	}
	return 0;
}
int Search_CaseEnd( char *SRC ){
	unsigned int c;
	while (1){
		c=SRC[g_exec_ptr++];
		switch ( c ) {
			case 0x00:	// <EOF>
				g_exec_ptr--;
				return 0 ;
			case 0x22:	// "
				Skip_quot(SRC);
				break;
			case 0x27:	// ' rem
				Skip_rem_no_op(SRC);
				break;
			case 0xFFFFFFF7:	// 
				if ( SRC[g_exec_ptr] == 0xFFffFFEA ) { g_exec_ptr++;				// Switch
					Search_SwitchEnd(SRC);
					break;
				}
				if ( SRC[g_exec_ptr] == 0xFFFFFFED ) { g_exec_ptr--; return 1; }	// SwitchEnd
				if ( SRC[g_exec_ptr] == 0xFFFFFFEB ) { g_exec_ptr--; return 2; }	// Case
				if ( SRC[g_exec_ptr] == 0xFFFFFFEC ) { g_exec_ptr--; return 3; }	// Default
			case 0x7F:	// 
			case 0xFFFFFFF9:	// 
			case 0xFFFFFFE5:	// 
			case 0xFFFFFFE6:	// 
			case 0xFFFFFFE7:	// 
//			case 0xFFFFFFFF:	// 
				g_exec_ptr++;
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
	wPtr=g_exec_ptr;
	CurrentStruct->SwitchAdrs[CurrentStruct->SwitchPtr] = g_exec_ptr;
	
	j=0; i=Cache->TOP;
	while ( j<Cache->CNT ) {
		if ( Cache->Ptr[i]==g_exec_ptr ) { 	// adrs ok
			if ( i==Cache->TOP ) { g_exec_ptr=Cache->Adrs[i]; goto next; }	// top of cache
			ii=i-1; if ( ii < 0 ) ii=IfCntMax-1;
			execptr=Cache->Adrs[i];	// 2nd- of cache level up
			Cache->Ptr[i]=Cache->Ptr[ii];
			Cache->Adrs[i]=Cache->Adrs[ii];
			Cache->Ptr[ii]=g_exec_ptr;
			Cache->Adrs[ii]=execptr;
			g_exec_ptr=execptr; goto next;
		}
		j++;
		i++; if ( i >= IfCntMax ) i=0;
	}
	execptr=g_exec_ptr;
	if ( Search_SwitchEnd(SRC) == 0 ) { CB_Error(MissingSwitchEnd); return; }  // Switch without SwitchEnd error
	if ( Cache->CNT < IfCntMax ) Cache->CNT++;
	Cache->TOP--; if ( Cache->TOP<0 ) Cache->TOP=IfCntMax-1;
	Cache->Ptr[Cache->TOP] =execptr;
	Cache->Adrs[Cache->TOP]=g_exec_ptr;
  next:
	CurrentStruct->SwitchEndAdrs[CurrentStruct->SwitchPtr] = g_exec_ptr;	// Break out adrs set
	CurrentStruct->SwitchValue[CurrentStruct->SwitchPtr] = value;
	CurrentStruct->Switchflag[CurrentStruct->SwitchPtr] = 0;	// case through clear
	
	if ( CurrentStruct->SwitchPtr >= StackSwitchMax ) { CB_Error(NestingERR); return; }  //  nesting error
	CurrentStruct->SwitchPtr++;
	CurrentStruct->TYPE[CurrentStruct->CNT]=TYPE_Switch_Case;
	CurrentStruct->GosubNest[CurrentStruct->CNT] = GosubNestN;
	CurrentStruct->CNT++;
	g_exec_ptr=wPtr;
	
}

void CB_Case( char *SRC, CurrentStk *CurrentStruct ) {
	int exitPtr=g_exec_ptr;
	int c,value;
	if ( CurrentStruct->SwitchPtr <= 0 ) { CB_Error(MissingSwitch); return; }
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
	int exitPtr=g_exec_ptr;
	int c,i;
	if ( CurrentStruct->SwitchPtr <= 0 ) { CB_Error(MissingSwitch); return; }  // Default without Switch error
//	CurrentStruct->CNT--;
//	CurrentStruct->TYPE[CurrentStruct->CNT]=TYPE_Switch_Case;
//	CurrentStruct->CNT++;
}

void CB_SwitchEnd( char *SRC, CurrentStk *CurrentStruct ) {
	int exitPtr=g_exec_ptr;
	int c,i;
	if ( CurrentStruct->SwitchPtr <= 0 ) { CB_Error(MissingSwitch); return; }  // SwitchEnd without Switch error
	CurrentStruct->SwitchPtr--;
	CurrentStruct->CNT--;
}


void CB_Break( char *SRC, CurrentStk *CurrentStruct ) {
	int expbuf=g_exec_ptr;
	if ( CurrentStruct->CNT <=0 ) { CB_Error(NotInLoop); return; }  // Not Loop error
	CurrentStruct->CNT--;
	switch ( CurrentStruct->TYPE[CurrentStruct->CNT] ) {
		case TYPE_For_Next:			// For Next
			CurrentStruct->ForPtr--;
			g_exec_ptr = CurrentStruct->NextAdrs[CurrentStruct->ForPtr];
			if ( g_exec_ptr==0 ) {
				g_exec_ptr = expbuf;
				if ( Search_Next(SRC) == 0 ) { CB_Error(MissingNext); return; }  // For without Next error
			}
			return ;
		case TYPE_While_WhileEnd:	// While WhileEnd
			CurrentStruct->WhilePtr--;
			g_exec_ptr = CurrentStruct->WhileEndAdrs[CurrentStruct->WhilePtr];
			if ( g_exec_ptr==0 ) {
				g_exec_ptr = expbuf;
				if ( Search_WhileEnd(SRC) == 0 ) { CB_Error(MissingWhileEnd); return; }  // While without WhileEnd error
			}
			return ;
		case TYPE_Do_LpWhile:		// DO LpWhile
			CurrentStruct->DoPtr--;
			g_exec_ptr = CurrentStruct->LpWhileAdrs[CurrentStruct->DoPtr];
			if ( g_exec_ptr==0 ) {
				g_exec_ptr = expbuf;
				if ( Search_LpWhile(SRC) == 0 )  { CB_Error(MissingLpWhile); return; }  // Do without LpWhile error
			}
			return ;
		case TYPE_Switch_Case:		// Switch
			CurrentStruct->SwitchPtr--;
			g_exec_ptr = CurrentStruct->SwitchEndAdrs[CurrentStruct->SwitchPtr];
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
	c=SRC[g_exec_ptr];
	if ( ( ( 'A'<=c )&&( c<='Z' ) ) || ( ( 'a'<=c )&&( c<='z' ) ) ) {
		g_exec_ptr++;
		reg=c-'A';
	  regj:
		c=SRC[g_exec_ptr];
		if ( c=='%' ) {
			g_exec_ptr++;
			LocalInt[reg][0] --;
			CB_CurrentValue = Int2Cplx( LocalInt[reg][0] );
		} else
		if ( c=='[' ) { 
			g_exec_ptr++;
			MatOprand2( SRC, reg, &dimA, &dimB );
			goto Matrix;
		} else
		if ( ( '0'<=c )&&( c<='9' ) ) {
			g_exec_ptr++;
			dimA=c-'0';
			MatOprand1num( SRC, reg, &dimA, &dimB );
			goto Matrix;
		} else {
			if ( c=='#' ) g_exec_ptr++;
			LocalDbl[reg][0].real --;
			CB_CurrentValue = LocalDbl[reg][0] ;
		}
	} else 
	if ( ( c==0x7F ) && ( ( SRC[g_exec_ptr+1]==0x40 ) || ( SRC[g_exec_ptr+1]==0xFFFFFF84 ) || ( SRC[g_exec_ptr+1]==0x51 ) ) ) {	// Mat or Vct or List
			MatrixOprand( SRC, &reg, &dimA, &dimB );
		Matrix:
			if ( g_error_type ) {  // error
				if ( MatAry[reg].SizeA == 0 ) g_error_type=UndefinedMatrix;	// No Matrix Array error
				return ;
			}
			CB_CurrentValue.real = ReadMatrix( reg, dimA,dimB ) ;
			CB_CurrentValue.real --;
			WriteMatrix( reg, dimA,dimB, CB_CurrentValue.real ) ;
	} else {
		reg=RegVarAliasEx(SRC);	if ( reg>=0 ) goto regj;
		{ CB_Error(SyntaxERR); return; }	// Syntax error
	}

	c=SRC[g_exec_ptr];
	if ( ( c==':' ) || ( c==0x0D ) ) {
		g_exec_ptr++;
		if ( CB_CurrentValue.real ) return ;
		else {
			Skip_block(SRC);
		}
	} else if ( c==0x0C ) {  // dsps
		g_exec_ptr++;
		if ( CB_Disps(SRC, 2) ) { BreakPtr=g_exec_ptr ; return ; }  // [AC] break
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
	c=SRC[g_exec_ptr];
	if ( ( ( 'A'<=c )&&( c<='Z' ) ) || ( ( 'a'<=c )&&( c<='z' ) ) ) {
		g_exec_ptr++;
		reg=c-'A';
	  regj:
		c=SRC[g_exec_ptr];
		if ( c=='%' ) {
			g_exec_ptr++;
			LocalInt[reg][0] ++;
			CB_CurrentValue = Int2Cplx( LocalInt[reg][0] );
		} else
		if ( c=='[' ) { 
			g_exec_ptr++;
			MatOprand2( SRC, reg , &dimA, &dimB );
			goto Matrix;
		} else
		if ( ( '0'<=c )&&( c<='9' ) ) {
			g_exec_ptr++;
			dimA=c-'0';
			MatOprand1num( SRC, reg, &dimA, &dimB );
			goto Matrix;
		} else {
			if ( c=='#' ) g_exec_ptr++;
			LocalDbl[reg][0].real ++;
			CB_CurrentValue = LocalDbl[reg][0] ;
		}
	} else 
	if ( ( c==0x7F ) && ( ( SRC[g_exec_ptr+1]==0x40 ) || ( SRC[g_exec_ptr+1]==0xFFFFFF84 ) || ( SRC[g_exec_ptr+1]==0x51 ) ) ) {	// Mat or Vct or List
			MatrixOprand( SRC, &reg, &dimA, &dimB );
		Matrix:
			if ( g_error_type ) {  // error
				if ( MatAry[reg].SizeA == 0 ) g_error_type=UndefinedMatrix;	// No Matrix Array error
				return ;
			}
			CB_CurrentValue.real = ReadMatrix( reg, dimA,dimB ) ;
			CB_CurrentValue.real ++;
			WriteMatrix( reg, dimA,dimB, CB_CurrentValue.real ) ;
	} else {
		reg=RegVarAliasEx(SRC);	if ( reg>=0 ) goto regj;
		{ CB_Error(SyntaxERR); return; }	// Syntax error
	}

	c=SRC[g_exec_ptr];
	if ( ( c==':' ) || ( c==0x0D ) ) {
		g_exec_ptr++;
		if ( CB_CurrentValue.real ) return ;
		else {
			Skip_block(SRC);
		}
	} else if ( c==0x0C ) {  // dsps
		g_exec_ptr++;
		if ( CB_Disps(SRC, 2) ) { BreakPtr=g_exec_ptr ; return ; }  // [AC] break
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
	
	int c=SRC[g_exec_ptr],d;
	if ( ( ( 'A'<=c )&&( c<='Z' ) ) || ( ( 'a'<=c )&&( c<='z' ) ) ) {
		reg=c-'A';
		g_exec_ptr++;
	  aliasj:
		c=SRC[g_exec_ptr];
		if ( c == 0x7E ) {		// '~'
			g_exec_ptr++;
			en=RegVarAliasEx(SRC);
			if ( en>=0 ) {
				if ( en<reg ) { CB_Error(SyntaxERR); return; }	// Syntax error
				c=SRC[g_exec_ptr];
				if ( c=='%' ) { g_exec_ptr++;  for ( i=reg; i<=en; i++) { if ( REGtype[i] == 0 ) LocalInt[ i ][0] = CB_CurrentValue.real; }
				} else
				if ( c=='#' ) g_exec_ptr++;
				for ( i=reg; i<=en; i++) { if ( REGtype[i] == 0 ) LocalDbl[ i ][0] = CB_CurrentValue; }
			}
		} else {					// 
			if ( REGtype[reg] == 1 ) { CB_Error(AssignERR); return; }	// Duplicate Definition	// const Var
			if ( c=='%' ) { g_exec_ptr++;  LocalInt[reg][0] = CB_CurrentValue.real ; }
			else
			if ( c=='[' ) { goto Matrix; }
			else
			if ( ( '0'<=c )&&( c<='9' ) ) {
				g_exec_ptr++;
				dimA=c-'0';
				MatOprand1num( SRC, reg, &dimA, &dimB );
				goto Matrix2;
			} else {
				if ( c=='#' ) g_exec_ptr++;
				LocalDbl[reg][0] = CB_CurrentValue;
			}
		}
	} else
	if ( c==0x7F ) {
		c = SRC[g_exec_ptr+1] ; 
		if ( c == 0x40 ) {	// Mat A[a,b]  Vct A[a,b]
			g_exec_ptr+=2;
			c=SRC[g_exec_ptr];
			if ( ( ( 'A'<=c )&&( c<='Z' ) ) || ( ( 'a'<=c )&&( c<='z' ) ) ) { reg=c-'A'; g_exec_ptr++; } 
			else { reg=MatRegVar(SRC); if ( reg<0 ) CB_Error(SyntaxERR) ; } // Syntax error 
			Matrix0:
			if ( SRC[g_exec_ptr] != '[' ) { 
				if ( dspflag ==3 ) { CopyAns2MatList( SRC, reg ); MatdspNo=reg;  return ; }	// MatAns -> Mat A
				if ( MatAry[reg].SizeA == 0 ) { CB_Error(UndefinedMatrix); return; }	// No Matrix Array error
				InitMatSub( reg, CB_CurrentValue);		// 10 -> Mat A
			} else {
			Matrix:
				g_exec_ptr++;
				MatOprand2( SRC, reg, &dimA, &dimB);
			Matrix2:
				if ( g_error_type ) {  // error
					if ( MatAry[reg].SizeA == 0 ) g_error_type=UndefinedMatrix;	// No Matrix Array error
					return ;
				}
				if ( CB_INT==0) WriteMatrix( reg, dimA, dimB, CB_CurrentValue.real); else Cplx_WriteMatrix( reg, dimA, dimB, CB_CurrentValue);
			}
		} else if ( c==0xFFFFFF84 ) {	//  Vct A[a,b]
			g_exec_ptr+=2;
			reg=VctRegVar(SRC); if ( reg<0 ) CB_Error(SyntaxERR) ; // Syntax error 
			goto Matrix0;
		} else if ( ( c == 0x51 ) || ( (0x6A<=c)&&(c<=0x6F) ) ) {	// List or List1~List6
			g_exec_ptr+=2;
			reg=ListRegVar( SRC );
		  Listj:
			if ( SRC[g_exec_ptr] != '[' ) { 
				if ( dspflag ==4 ) { CopyAns2MatList( SRC, reg ) ; MatdspNo=reg; dspflag=0; return ; }	// ListAns -> List 1
				if ( MatAry[reg].SizeA == 0 ) { CB_Error(UndefinedMatrix); return; }	// No Matrix Array error
				InitMatSub( reg, CB_CurrentValue);		// 10 -> List 1
			} else {
				g_exec_ptr++;
				MatOprand1( SRC, reg, &dimA, &dimB);
				goto Matrix2;
			}
			
		} else if ( c == 0x46 ) {	// -> Dim
				g_exec_ptr+=2;
				c = SRC[g_exec_ptr];
				d = SRC[g_exec_ptr+1];
				if ( ( c==0x7F ) && ( d==0x46 ) ) {	// {24,18}->dim dim
					g_exec_ptr+=2; dimdim=1; 
					c = SRC[g_exec_ptr];
					d = SRC[g_exec_ptr+1];
					}
				if ( ( c==0x7F ) && ( ( d==0x51 ) || ( (0x6A<=d)&&(d<=0x6F) ) ) ) {	// n -> Dim List
					g_exec_ptr+=2;
					if ( CB_CurrentValue.real )  		// 15->Dim List 1
							CB_ListInitsub( SRC, &reg, CB_CurrentValue.real, 0, dimdim );
					else {							//  0->Dim List 1
						reg=ListRegVar( SRC );
						if ( reg>=0 ) DeleteMatrix( reg );
					}
				} else
				if ( ( c==0x7F ) && ( ( d==0x40 ) || ( d==0xFFFFFF84 ) ) ) {	// {10,5} -> Dim Mat A  -> Dim Vct A
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
				g_exec_ptr+=2;
				goto StoreTicks;
		} else if ( ( 0xFFFFFF91 <= c ) && ( c <= 0xFFFFFF93 ) ) {	// F Start~F pitch
				g_exec_ptr+=2;
				REGf[c-0xFFFFFF90] = CB_CurrentValue.real ;
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
				g_exec_ptr+=2;
				CB_ChangeViewWindow() ;
				if ( g_error_type == RangeERR ) g_error_type=0;	// Cancel Range error
		} else goto exitj;
	} else
	if ( c==0xFFFFFFF7 ) {
		c = SRC[g_exec_ptr+1] ; 
		if ( c == 0xFFFFFFF6 ) {	// Poke(A)
			g_exec_ptr+=2;
			CB_PokeSub( SRC, CB_CurrentValue, EvalsubTop( SRC ) );
		} else goto exitj;
	} else
	if ( c=='*' ) { g_exec_ptr++;
			CB_PokeSub( SRC, CB_CurrentValue, Evalsub1( SRC ) );
	} else
	if ( c=='%' ) { g_exec_ptr++;
		StoreTicks:
		CB_StoreTicks( SRC, CB_CurrentValue.real );
		skip_count=0;
	} else
	if ( c==0xFFFFFFF9 ) {
		c = SRC[g_exec_ptr+1] ; 
		if ( c == 0x21 ) {	// Xdot
				if ( CB_CurrentValue.real == 0 ) { CB_Error(RangeERR); return; }	// Range error
				g_exec_ptr+=2;
				Xdot = CB_CurrentValue.real ;
				Xmax = Xmin + Xdot*126.;
		} else
		if ( c == 0x0E ) {	// 123->Const _ABC
			g_exec_ptr+=2;
			reg=RegVarAliasEx( SRC ) ;
			REGtype[reg]=1;		// const
			if ( c=='%' ) { g_exec_ptr++;
				LocalInt[reg][0] = CB_CurrentValue.real ;
			} else {
				if ( c=='#' ) g_exec_ptr++;
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
		c=SRC[g_exec_ptr];
		if ( c != ',' ) break; 	// 
		g_exec_ptr++;
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

	c=SRC[g_exec_ptr];
	if ( c == 0x22 ) {
		g_exec_ptr++;
		CB_GetQuotOpcode(SRC, buffer,32-1);	// Prog name
	} else CB_GetLocateStr(SRC, buffer,32-1); 
	if ( g_error_type ) return ;	// error

	c=SRC[g_exec_ptr];
	if ( c == ',' ) {	// arg
		g_exec_ptr++;
		CB_argNum( SRC );	// get local value
	}
	
	StackProgSRC     = SRC;
	StackProgExecPtr = g_exec_ptr;
	ProgNo_bk = g_current_prog;
	
	Setfoldername16( folder16, buffer );
	g_current_prog = CB_SearchProg( folder16 );
	if ( g_current_prog < 0 ) { 
		g_current_prog = CB_GetProgEntry( SRC, buffer );
		if ( g_current_prog < 0 ) { g_current_prog=ProgNo_bk; g_error_type=GoERR; g_error_ptr=g_exec_ptr; return; }  // undefined Prog
	}
	src = ProgfileAdrs[g_current_prog];
	SRC = src + 0x56 ;
	g_exec_ptr=0;
	
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
			if ( ( g_error_type ) && ( g_error_type != StackERR ) ) return ;		// error
			else if ( BreakPtr > 0 ) return ;	// break
		}
	}

	ProgEntryN--;
	SRC     = StackProgSRC ;
	g_exec_ptr = StackProgExecPtr ;
	g_current_prog  = ProgNo_bk;
	
	InitLocalVar();		// init Local variable
	for (c=0; c<StackGotoMax; c++) StackGotoAdrs[c]=0;	// init goto

	for ( i=0; i<ProgLocalN[g_current_prog]; i++ ) {		// restore local variable
		j=ProgLocalVar[g_current_prog][i];
		if ( j>=0 ) { 
			LocalDbl[j]=&localvarDbl[i];
			LocalInt[j]=&localvarInt[i];
		}
	}

	if ( g_error_type == StackERR ) { g_error_ptr=g_exec_ptr;  }
}

void CB_Gosub( char *SRC, int *StackGotoAdrs, int *StackGosubAdrs ){ //	Gosub N
	int c,i,j;
	int label;
	int ptr;
	int execptr=g_exec_ptr;
	
	label = CB_CheckLbl( SRC );
	if ( label < 0 ) { CB_Error(SyntaxERR); return; }	// syntax error

	c=SRC[g_exec_ptr];
	if ( c == ',' ) {	// arg
		g_exec_ptr++;
		CB_argNum( SRC );	// get local value
		
		for ( i=0; i<ProgLocalN[g_current_prog]; i++ ) {	// set local variable
			j=ProgLocalVar[g_current_prog][i];
			if ( j>=0 ) { 
				LocalDbl[j][0]=LocalDbltmp[i];
				LocalInt[j][0]=LocalInttmp[i];
			}
		}
		StackGosubAdrs[GosubNestN] = g_exec_ptr;	// return adrs
		g_exec_ptr=execptr;
	} else {
		StackGosubAdrs[GosubNestN] = g_exec_ptr;	// return adrs
	}
	GosubNestN++;
	if ( GosubNestN > StackGosubMax ) { CB_Error(NestingERR);  return; }	// Nesting  error

	ptr = StackGotoAdrs[label] ;
	if ( ptr == 0 ) {
		if ( Search_Lbl(SRC, label) == 0 ) { CB_Error(UndefinedLabel); return; }	// undefined label error
		g_exec_ptr++;
		StackGotoAdrs[label]=g_exec_ptr;
	} else  g_exec_ptr = ptr ;
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

int CB_CheckAliasVar( int c, int *del ) {
	if ( c=='_' ) { CB_Error(SyntaxERR); return 1; }	// Syntax error
	if ( c==0xFFFFFF99 ) *del=1; // -
	return 0;
}

void CB_AliasVar( char *SRC ) {	// Alias A=��  or A=_ABCD	// (delete) Alias -A=_ABCD  Alias Mat -A=_ABCD
	int c,i,j,reg,len;
	int alias_code, org_reg;
	int del=0;
	char name[32+1];
	if ( ( SRC[g_exec_ptr]==0x7F ) && ( ( SRC[g_exec_ptr+1]==0x40 ) || ( SRC[g_exec_ptr+1]==0xFFFFFF84 ) ) ) {	// 	Alias Mat A=��  or Mat A=_ABCD / Vct
		g_exec_ptr+=2;
		c=SRC[g_exec_ptr];
		if ( CB_CheckAliasVar( c, &del ) ) return ;	// Syntax error
		reg=RegVar(c);
		org_reg=reg;
		if ( reg>=0 ) {
			g_exec_ptr++;
			c=SRC[g_exec_ptr];
			if ( c != '=' ) { CB_Error(SyntaxERR); return; }	// Syntax error
			g_exec_ptr++;
			c=SRC[g_exec_ptr];
			if ( c=='_' ) {	//	_ABCDE   var name
				g_exec_ptr++; len=32;
				if ( GetVarName( SRC, &g_exec_ptr, name, &len) == 0 ) { CB_Error(SyntaxERR); return; }	// Syntax error
				for ( i=0; i<=AliasVarMAXMat; i++ ) {	// check already 
					if ( ( len == AliasVarCodeMat[i].len ) && ( AliasVarCodeMat[i].alias == 0x4040 ) ) {	// @@
						for (j=0; j<len; j++) {
							if ( AliasVarCodeMat[i].name[j] != name[j] ) break;
						}
						if ( j==len ) return ;	// macth!!
					}
				}
				if ( del ) { AliasVarCodeMat[i].alias=-1; return; }	// delete
				if ( AliasVarMAXMat >= ALIASVARMAXMAT-1 ) { CB_Error(TooMuchData); return; }
				AliasVarMAXMat++; 
				AliasVarCodeMat[AliasVarMAXMat].org  =org_reg;
				AliasVarCodeMat[AliasVarMAXMat].alias=0x4040;	// @@
				if ( len > MAXNAMELEN ) len=MAXNAMELEN;
				AliasVarCodeMat[AliasVarMAXMat].len=len;
				memcpy( &AliasVarCodeMat[AliasVarMAXMat].name[0], name, len );
			} else {
				i=RegVar(c); if ( i>=0 ) { CB_Error(ArgumentERR); return; }  // Argument error	 ( = default built-in variable )
				len = GetOpcodeLen( SRC, g_exec_ptr ,&alias_code );
				if ( len == 1 ) alias_code &=0xFF;
				g_exec_ptr += len;
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
	if ( ( SRC[g_exec_ptr]==0xFFFFFFE2 ) ) {	// 	Alias Lbl A=��  or Lbl A=_ABCD
		g_exec_ptr++;
		c=SRC[g_exec_ptr];
//		if ( CB_CheckAliasVar( c, &del ) ) return ;	// Syntax error
		reg=RegVar(c);
		org_reg=reg;
		if ( reg>=0 ) {
			g_exec_ptr++;
			c=SRC[g_exec_ptr];
			if ( c != '=' ) { CB_Error(SyntaxERR); return; }	// Syntax error
			g_exec_ptr++;
			c=SRC[g_exec_ptr];
			if ( c=='_' ) {	//	_ABCDE   var name
				g_exec_ptr++; len=32;
				if ( GetVarName( SRC, &g_exec_ptr, name, &len) == 0 ) { CB_Error(SyntaxERR); return; }	// Syntax error
				for ( i=0; i<=AliasVarMAXLbl; i++ ) {	// check already 
					if ( ( len == AliasVarCodeLbl[i].len ) && ( AliasVarCodeLbl[i].alias == 0x4040 ) ) {	// @@
						for (j=0; j<len; j++) {
							if ( AliasVarCodeLbl[i].name[j] != name[j] ) break;
						}
						if ( j==len ) return ;	// macth!!
					}
				}
//				if ( del ) { AliasVarCodeLbl[i].alias=-1; return; }	// delete
				if ( AliasVarMAXLbl >= ALIASVARMAXLBL-1 ) { CB_Error(TooMuchData); return; }
				AliasVarMAXLbl++; 
				AliasVarCodeLbl[AliasVarMAXLbl].org  =org_reg+10;
				AliasVarCodeLbl[AliasVarMAXLbl].alias=0x4040;	// @@
				if ( len > MAXNAMELEN ) len=MAXNAMELEN;
				AliasVarCodeLbl[AliasVarMAXLbl].len=len;
				memcpy( &AliasVarCodeLbl[AliasVarMAXLbl].name[0], name, len );
			} else {
				i=RegVar(c); if ( i>=0 ) { CB_Error(ArgumentERR); return; }  // Argument error	 ( = default built-in variable )
				len = GetOpcodeLen( SRC, g_exec_ptr ,&alias_code );
				if ( len == 1 ) alias_code &=0xFF;
				g_exec_ptr += len;
				for ( i=0; i<=AliasVarMAXLbl; i++ ) {	// check already 
					if ( AliasVarCodeLbl[i].alias==(short)alias_code ) return ; 	// macth!!
				}
				if ( AliasVarMAXLbl >= ALIASVARMAXLBL-1 ) { CB_Error(TooMuchData); return; }
				AliasVarMAXLbl++; 
				AliasVarCodeLbl[AliasVarMAXLbl].org  =org_reg+10;
				AliasVarCodeLbl[AliasVarMAXLbl].alias=alias_code;
			}
		}
	} else {						// Alias A=��  or A=_ABCD
		c=SRC[g_exec_ptr];
		if ( CB_CheckAliasVar( c, &del ) ) return ;	// Syntax error
		reg=RegVar(c);
		org_reg=reg;
		if ( reg>=0 ) {
			g_exec_ptr++;
			c=SRC[g_exec_ptr];
			if ( c != '=' ) { CB_Error(SyntaxERR); return; }	// Syntax error
			g_exec_ptr++;
			c=SRC[g_exec_ptr];
			if ( c=='_' ) {	//	_ABCDE   var name
				g_exec_ptr++; len=32;
				if ( GetVarName( SRC, &g_exec_ptr, name, &len) == 0 ) { CB_Error(SyntaxERR); return; }	// Syntax error
				for ( i=0; i<=AliasVarMAX; i++ ) {	// check already 
					if ( ( len == AliasVarCode[i].len ) && ( AliasVarCode[i].alias == 0x4040 ) ) {	// @@
						for (j=0; j<len; j++) {
							if ( AliasVarCode[i].name[j] != name[j] ) break;
						}
						if ( j==len ) return ;	// macth!!
					}
				}
				if ( del ) { AliasVarCode[i].alias=-1; return; }	// delete
				if ( AliasVarMAX >= ALIASVARMAX-1 ) { CB_Error(TooMuchData); return; }
				AliasVarMAX++; 
				AliasVarCode[AliasVarMAX].org  =org_reg;
				AliasVarCode[AliasVarMAX].alias=0x4040;	// @@
				if ( len > MAXNAMELEN ) len=MAXNAMELEN;
				AliasVarCode[AliasVarMAX].len=len;
				memcpy( &AliasVarCode[AliasVarMAX].name[0], name, len );
			} else {
				i=RegVar(c); if ( i>=0 ) { CB_Error(ArgumentERR); return; }  // Argument error	 ( = default built-in variable )
				len = GetOpcodeLen( SRC, g_exec_ptr ,&alias_code );
				if ( len == 1 ) alias_code &=0xFF;
				g_exec_ptr += len;
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
	char	bk_FuncType    = FuncType;
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
	g_error_ptr = 0;
	g_error_type = 0;
	CB_CurrentValue.imag = 0;
	ListFileNo  = 0;	// List File No
	ListFilePtr = 0;	// List File Ptr

	IsDispsMat  = 0;

	defaultStrAry=26;	// <r>
	defaultFnAry=57;		// z
	defaultGraphAry=27;		// Theta
	DeleteMatrix( Mattmp_traceAry );
	if ( MaxMemMode ) DeleteMatrix( Mattmp_clipBuffer );
	
	CB_MatListAnsreg=27;	//	ListAns init
	Bdisp_PutDisp_DD_DrawBusy();
	KeyRecover(); 
	Argc = 0;	// 
//	CB_AliasVarClr();
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
		FuncType    = bk_FuncType;
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
	if ( g_error_type==StackERR ) CB_ErrMsg(StackERR);
	return stat;
}

//----------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------
