//-----------------------------------------------------------------------------
// Casio Basic inside
//-----------------------------------------------------------------------------
#define BREAKCOUNT 100

extern char CB_INTDefault;	// default mode  0:normal  1: integer mode

extern char	DrawType  ;	// 0:connect  1:Plot
extern char	Coord     ;	// 0:off 1:on
extern char	Grid      ;	// 0:off 1:on
extern char	Axes      ;	// 0:off 1:on
extern char	Label     ;	// 0:off 1:on
extern char	Derivative     ;	// 0:off 1:on

#define S_L_Normal   0
#define S_L_Thick    1
#define S_L_Broken   2
#define S_L_Dot      3
#define S_L_Default  -1

extern char S_L_Style;		// set line style 
extern char tmp_Style;		// set line style 
extern char Angle;			// 0:deg  1:rad  2:grad

extern double Previous_X ;	// Line Previous X
extern double Previous_Y ;	// Line Previous Y
extern double Previous_X2 ;	// Line Previous X2
extern double Previous_Y2 ;	// Line Previous Y2
extern int Previous_PX   ;	// Plot Previous PX
extern int Previous_PY   ;	// Plot Previous PY
extern double Plot_X     ;	// Plot Current X
extern double Plot_Y     ;	// Plot Current Y

extern char BreakCheckDefault;	// Break Stop on/off
extern char BreakCheck;	// Break Stop on/off
extern char ACBreak;	// AC Break on/off

extern char TimeDsp;
extern char MatXYmode;
extern char PictMode;	// StoPict/RclPict  StrageMem:0  heap:1  MCS:3
extern char CheckIfEnd;	// If...IfEnd check  0:off  1:on
extern char RefreshCtrl;	// 0:no refresh Ctrl     1: GrphicsCMD refresh Ctrl      2: all refresh Ctrl
extern char Refreshtime;	// Refresh time  n/128

extern short DefaultWaitcount;	// wait control
extern short Waitcount;			// current wait control
extern char  CommandInputMethod;	//	0:C.Basic  1:Genuine
//-----------------------------------------------------------------------------
// Casio Basic Gloval variable
//-----------------------------------------------------------------------------
#define VARMAXSIZE 26+6+26+26+6
extern complex  REG[VARMAXSIZE];
extern double  REGv[11];
extern double  VWIN[6][11];
extern char VWinflag[6];		// VWin flag

#define regA REG[ 0]
#define regB REG[ 1]
#define regC REG[ 2]
#define regD REG[ 3]
#define regE REG[ 4]
#define regF REG[ 5]
#define regG REG[ 6]
#define regH REG[ 7]
#define regI REG[ 8]
#define regJ REG[ 9]
#define regK REG[10]
#define regL REG[11]
#define regM REG[12]
#define regN REG[13]
#define regO REG[14]
#define regP REG[15]
#define regQ REG[16]
#define regR REG[17]
#define regS REG[18]
#define regT REG[19]
#define regU REG[20]
#define regV REG[21]
#define regW REG[22]
#define regX REG[23]
#define regY REG[24]
#define regZ REG[25]
#define reg_r 		REG[26]	// [	<r>
#define reg_Theta	REG[27]	// \	Theta
#define reg_Ans		REG[28]	// ]	Ans
#define reg29 REG[29]	// ^
#define reg30 REG[30]	// _
#define reg31 REG[31]	//


#define Xmin REGv[ 0]
#define Xmax REGv[ 1]
#define Xscl REGv[ 2]
#define Ymin REGv[ 3]
#define Ymax REGv[ 4]
#define Yscl REGv[ 5]
#define TThetamin  REGv[ 6]
#define TThetamax  REGv[ 7]
#define TThetaptch REGv[ 8]

#define Xdot REGv[ 9]
#define Ydot REGv[10]


extern	double Xfct;
extern	double Yfct;

#define ArgcMAX 10
extern	complex 	*LocalDbl[VARMAXSIZE];		// local var ptr
extern	int			*LocalInt[VARMAXSIZE];		// local var ptr

extern double	*traceAry;		// Graph trace array

#define GraphStrMAX 256
extern char *GraphY;
extern char *GraphX;

#define const_PI  3.1415926535897932
#define const_hPI 1.5707963267948966
#define const_qPI 0.78539816339744831

#define PictMax 99
extern unsigned char *PictAry[PictMax+1];		// Pict array ptr

extern char BG_Pict_No;

//------------------------------------------------------------------------------
extern int  REGINT[VARMAXSIZE];

#define regintA REGINT[ 0]
#define regintB REGINT[ 1]
#define regintC REGINT[ 2]
#define regintD REGINT[ 3]
#define regintE REGINT[ 4]
#define regintF REGINT[ 5]
#define regintG REGINT[ 6]
#define regintH REGINT[ 7]
#define regintI REGINT[ 8]
#define regintJ REGINT[ 9]
#define regintK REGINT[10]
#define regintL REGINT[11]
#define regintM REGINT[12]
#define regintN REGINT[13]
#define regintO REGINT[14]
#define regintP REGINT[15]
#define regintQ REGINT[16]
#define regintR REGINT[17]
#define regintS REGINT[18]
#define regintT REGINT[19]
#define regintU REGINT[20]
#define regintV REGINT[21]
#define regintW REGINT[22]
#define regintX REGINT[23]
#define regintY REGINT[24]
#define regintZ REGINT[25]
#define regint_r 		REGINT[26]	// [	<r>
#define regint_Theta	REGINT[27]	// \	Theta
#define regint_Ans		REGINT[28]	// ]	Ans
#define regint29 REGINT[29]	// ^
#define regint30 REGINT[30]	// _
#define regint31 REGINT[31]	//

extern char    REGtype[VARMAXSIZE];		// 0:normal  1:const

//------------------------------------------------------------------------------
extern int	CB_TicksStart;
extern int	CB_TicksEnd;
extern int	CB_TicksAdjust;
extern int	CB_HiTicksStart;
extern int	CB_HiTicksEnd;
extern int	CB_HiTicksAdjust;

extern char ScreenMode;	//  0:Text  1:Graphic
extern char UseGraphic;	// use Graph  ( no use :0    plot:1   graph:2   cls:3   other:99
extern char dspflag;	// 0:nondsp  1:str  2:num  3:mat 4:list
extern char MatdspNo;	// 

extern int CursorX;	// text cursor X
extern int CursorY;	// text cursor X

extern int CB_INT;		// current mode  0:normal  1: integer mode
extern int ExecPtr;
extern int BreakPtr;

extern int CBint_CurrentValue;	// Ans
extern complex CB_CurrentValue;	// Ans

#define ProgMax 40
extern char ProgEntryN;		// how many subroutin
extern char ProgNo;			// current Prog No
extern char *ProgfileAdrs[ProgMax+1];
extern int   ProgfileMax[ProgMax+1] ;	// Max edit filesize 
extern char  ProgfileEdit[ProgMax+1];	// no change : 0     edited : 1
extern char  ProgfileMode[ProgMax+1];	// g1m : 0    text : 1
extern char  ProgLocalN[ProgMax+1];
extern char  ProgLocalVar[ProgMax+1][ArgcMAX];

#define MAXHEAP 47*1024		// use heap max byte
extern  char *HeapRAM;
extern  char *TVRAM;
extern  char *GVRAM;
//------------------------------------------------------------------------------
#define StackGotoMax 10+26+2+6+26
#define StackGosubMax 16
#define IfCntMax 32
#define StackForMax 7
#define StackWhileMax 7
#define StackDoMax 7
#define StackSwitchMax 7

#define TYPE_For_Next 			1
#define TYPE_While_WhileEnd		2
#define TYPE_Do_LpWhile			3
#define TYPE_Switch_Case		4

typedef struct {		// 10 bytes
	char	CNT;
	char	TOP;
	int		Ptr[IfCntMax];
	int		Adrs[IfCntMax];
} CchIf;

typedef struct {		// 34+4 bytes
	char	CNT;
	char	ForPtr;
	char	TYPE[20];
	char	GosubNest[20];
	
	int	*Var[StackForMax];
	int	ForAdrs[StackForMax];
	int	NextAdrs[StackForMax];
	int	IntEnd[StackForMax];
	int	IntStep[StackForMax];
	double End[StackForMax];
	double Step[StackForMax];

	char	WhilePtr;
	char	DoPtr;
	int	WhileAdrs[StackWhileMax];
	int	WhileEndAdrs[StackWhileMax];
	int	DoAdrs[StackDoMax];
	int	LpWhileAdrs[StackDoMax];

	char	SwitchPtr;
	char	Switchflag[StackSwitchMax];
	int		SwitchAdrs[StackSwitchMax];
	int		SwitchEndAdrs[StackSwitchMax];
	int		SwitchValue[StackSwitchMax];
	
} CurrentStk;

//-----------------------------------------------------------------------------
#define SkipSpace(SRC) c=SRC[ExecPtr]; while ( c==0x20 ) c=SRC[++ExecPtr]
//------------------------------------------------------------------------------
void ClrCahche();
void InitLocalVar();
int CB_interpreter( char *SRC) ;
int CB_interpreter_sub( char *SRC ) ;
void CB_Prog( char *SRC, int *localvarInt, complex *localvarDbl ) ; //	Prog "..."
void CB_Gosub( char *SRC, int *StackGotoAdrs, int *StackGosubAdrs ); //	Gosub N

void Skip_quot( char *SRC ); // skip "..."
void Skip_block( char *SRC );
void Skip_rem( char *SRC );	// skip '...
void CB_Rem( char *SRC, CchIf *CacheRem );
void CB_Lbl( char *SRC, int *StackGotoAdrs );
void CB_Goto( char *SRC, int *StackGotoAdrs, CurrentStk *CurrentStruct ) ;
void CB_If( char *SRC, CchIf *CacheIf );
void CB_Else( char *SRC, CchIf *CacheElse );
void CB_For( char *SRC, CurrentStk *CurrentStruct );
void CB_Next( char *SRC, CurrentStk *CurrentStruct );
void CB_While( char *SRC, CurrentStk *CurrentStruct ) ;
void CB_WhileEnd( char *SRC, CurrentStk *CurrentStruct ) ;
void CB_Do( char *SRC, CurrentStk *CurrentStruct ) ;
void CB_LpWhile( char *SRC, CurrentStk *CurrentStruct ) ;
void CB_Switch( char *SRC, CurrentStk *CurrentStruct ,CchIf *CacheSwitch ) ;
void CB_Case( char *SRC, CurrentStk *CurrentStruct ) ;
void CB_Default( char *SRC, CurrentStk *CurrentStruct ) ;
void CB_SwitchEnd( char *SRC, CurrentStk *CurrentStruct ) ;
void CB_Break( char *SRC, CurrentStk *CurrentStruct ) ;

void CB_SaveTextVRAM() ;
void CB_RestoreTextVRAM() ;
void CB_SelectTextVRAM() ;
void CB_SelectTextDD() ;
void CB_SaveGraphVRAM() ;
void CB_RestoreGraphVRAM() ;
void CB_SelectGraphVRAM() ;
void CB_SelectGraphDD() ;
void Scrl_Y();

void CB_ChangeViewWindow() ;
int CB_ChangeGraphicMode( char *SRC ) ;

void CB_Dsz( char *SRC ) ; //	Dsz
void CB_Isz( char *SRC ) ; //	Isz
void CB_Store( char *SRC );	// ->
void CB_Input( char *SRC );
int CB_Fix( char *SRC );
int CB_Sci( char *SRC );
int CB_Norm( char *SRC );
void CB_Rnd();
int CB_Disps( char *SRC ,short dspflag);
void CB_end( char *SRC );

void CB_Cls( char *SRC );
void CB_ClrText( char *SRC );
void CB_ClrGraph( char *SRC );
int RangeErrorCK( char *SRC ) ;

void PlotXYtoPrevPXY() ;
void PlotPreviousPXY() ;
void PlotCurrentXY();

void CB_GetOprand2( char *SRC, int *px, int *py) ;
void CB_GetOprand4( char *SRC, int *px, int *py, int *px2, int *py2) ;
void CB_GetOprand2dbl( char *SRC, double *x, double *y) ;

void CB_RefreshCtrl( char *SRC );	// PutDispDD Refresh control
void CB_RefreshTime( char *SRC );	// PutDispDD Refresh time
void CB_Screen( char *SRC );
void CB_ViewWindow( char *SRC ) ; //	ViewWindow
void CB_FLine( char *SRC) ; //	F-Line
void CB_Line( char *SRC ) ; //	Line
void CB_Vertical( char *SRC ) ; //	Vertical
void CB_Horizontal( char *SRC ) ; //	Horizontal
void CB_Plot( char *SRC ) ; //	Plot
void CB_PlotSub( char *SRC, int mode ); //	mode  1:PlotOn  0:PlotOff  2:PlotChg
void CB_Circle( char *SRC ) ; //	Circle
void CB_PxlSub( char *SRC, int mode ) ; //	mode  1:PxlOn  0:PxlOff  2:PxlChg

void StoPict( int pictNo);
void RclPict( int pictNo, int errorcheck);
void CB_DrawGraph(  char *SRC );
void CB_GraphY( char *SRC );
void CB_GraphX( char *SRC );
int  CB_GraphXYEval( char *SRC ) ;
void CB_GraphXY( char *SRC );
void CB_StoPict( char *SRC ) ; //	StoPict
void CB_RclPict( char *SRC ) ; //	RclPict
void CB_BG_None( char *SRC ) ; //	BG_None
void CB_BG_Pict( char *SRC ) ; //	BG_Pict

void CB_Locate( char *SRC ) ;
void CB_LocateYX( char *SRC );
void CB_Text( char *SRC ) ; //	Text
void CB_ReadGraph( char *SRC );	// ReadGraph(px1,py1, px2,py2)->Mat C
void CB_WriteGraph( char *SRC );	// WriteGraph x,y,wx,wy,Mat A ([2,2]),modify,kind
void CB_RectSub( char *SRC , int RectMode ) ; // RectMode  0:Rect  1:RectFill
void CB_Rect( char *SRC ) ; 	// Rect x1,y1,x2,y2,mode 
void CB_FillRect( char *SRC ) ; // FillRect x1,y1,x2,y2,mode 
void CB_DotShape( char *SRC ) ; // DotShape (x1,y1,x2,y2,typ,mode1,mode2,pattern1,pattern2)
void CB_DotGet( char *SRC );	// DotGet(px1,py1, px2,py2)->Mat B [x,y]
void CB_DotPut( char *SRC );	// DotPut(Mat B[x,y], px1,py1, px2,py2)
void CB_DotTrim( char *SRC );	// DotTrim(Mat A,x1,y1,x2,y2)->Mat B    =>[X,Y]
void CB_DotLife( char *SRC ) ;

void CB_FkeyMenu( char *SRC) ;
int CB_PopUpWin( char *SRC );	//
void CB_Menu( char *SRC, int *StackGotoAdrs) ;		// Menu "title name","Branch name1",1,"Branch name2",2,"Branch name3",3,...
void CB_Wait( char *SRC ) ;

//-----------------------------------------------------------------------------
#define MAXNAMELEN 8

typedef struct {		// 16 bytes
	short	alias;
	short	org;
	char	len;
	char	name[9];
} ALIAS_VAR;

#define ALIASVARMAX 64
#define ALIASVARMAXMAT 26
#define ALIASVARMAXLBL 26
extern	ALIAS_VAR	*AliasVarCode;
extern	ALIAS_VAR	*AliasVarCodeMat;
extern	ALIAS_VAR	*AliasVarCodeLbl;
extern	int AliasVarMAX;
extern	int AliasVarMAXMat;
extern	int AliasVarMAXLbl;
extern	unsigned char IsExtVar;

void CB_AliasVarClr();
int CB_GetAliasRegVar( char *SRC ) ;	// AliasVar ?
void CB_AliasVar( char *SRC ) ;	// AliasVar A=ƒ¿
int GetVarName( char *SRC, int *ptr, char *name, int *len );

//-----------------------------------------------------------------------------
#define GRAPHMAX	6
#define GRAPHLENMAX	64
typedef struct {
	char en;		// 
	char type;
	char style;			//
	unsigned short color;	//
	char gstr[GRAPHLENMAX];		//
} tgraphstat;
extern	tgraphstat GraphStat[GRAPHMAX];
extern	int	GraphPtr;
extern double IntegralStart,IntegralEnd;

typedef struct {
	char Draw;			// 0:off  1:on
	char GraphType;		// 0;Scatter 1:xyline  2:
	short xList;			// List 1-26
	short yList;			// List 1-26
	char Freq;			// -1:1  1-26:List 1-26
	char MarkType;		// 0:Square  1:Cross  2:Dot
} tdrawstat;
extern	tdrawstat Sgraph[3];

void CB_ClrGraphStat();
void CB_S_Gph_init( int No ) ;	// S-Gph1 DrawOff,Scatter,List 1,List 2,1,Square
void CB_S_Gph( char *SRC, int No ) ;
void CB_S_WindAuto( char *SRC ) ;
void CB_S_WindMan( char *SRC ) ;
void CB_DrawStat( char *SRC ) ;

void StoVwin( int n ) ;
void RclVwin( int n ) ;
void CB_StoVWin( char *SRC ) ;
void CB_RclVWin( char *SRC ) ;

void CB_Send( char *SRC );				// Receive(
void CB_Receive( char *SRC );			// OpenComport38k
void CB_OpenComport38k( char *SRC );	// CloseComport38k
void CB_CloseComport38k( char *SRC );	// Send38k
void CB_Send38k( char *SRC );			// Send38k
void CB_Receive38k( char *SRC );		// Receive38k

void CB_Beep( char *SRC );

void CB_ResetExecTicks();
