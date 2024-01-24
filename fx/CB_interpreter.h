//-----------------------------------------------------------------------------
// Casio Basic inside
//-----------------------------------------------------------------------------
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

extern double Previous_X ;	// Plot Previous X
extern double Previous_Y ;	// Plot Previous Y
extern int Previous_PX   ;	// Plot Previous PX
extern int Previous_PY   ;	// Plot Previous PY
extern double Plot_X     ;	// Plot Current X
extern double Plot_Y     ;	// Plot Current Y

extern char BreakCheck;	// Break Stop on/off
extern char ACBreak;	// AC Break on/off

extern char TimeDsp;
extern char MatXYmode;
extern char PictMode;	// StoPict/RclPict  StrageMem:0  heap:1
extern char CheckIfEnd;	// If...IfEnd check  0:off  1:on
extern char RefreshCtrl;	// 0:no refresh   1: GrphicsCMD refresh     2: all refresh
extern char Refreshtime;	// Refresh time  n/128

//-----------------------------------------------------------------------------
// Casio Basic Gloval variable
//-----------------------------------------------------------------------------
extern double  REG[26];
extern double  REGsmall[26];
extern double  REGv[11];

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
extern	double 	*LocalDbl[26];		// local var ptr
extern	int		*LocalInt[26];		// local var ptr

extern double	traceAry[130];		// Graph trace array

#define GraphStrMAX 32
extern char *GraphY;
extern char GraphY1[];
extern char GraphY2[];
extern char GraphY3[];

#define PI 3.1415926535897932

#define PictMax 20
extern unsigned char *PictAry[PictMax+1];		// Pict array ptr

//------------------------------------------------------------------------------
extern int  REGINT[26];
extern int  REGINTsmall[26];

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

//------------------------------------------------------------------------------
extern int	CB_TicksStart;
extern int	CB_TicksEnd;
extern int	CB_TicksAdjust;

extern int ScreenMode;	//  0:Text  1:Graphic
extern int UseGraphic;	// use Graph  ( no use :0    plot:1   graph:2   cls:3   other:99
extern int dspflag;

extern int CursorX;	// text cursor X
extern int CursorY;	// text cursor X

extern int CB_INT;		// current mode  0:normal  1: integer mode
extern int ExecPtr;
extern int BreakPtr;

extern int CBint_CurrentValue;	// Ans
extern double CB_CurrentValue;	// Ans

#define ProgMax 30
extern char ProgEntryN;		// how many subroutin
extern char ProgNo;			// current Prog No
extern char *ProgfileAdrs[ProgMax+1];
extern int   ProgfileMax[ProgMax+1] ;	// Max edit filesize 
extern char  ProgfileEdit[ProgMax+1];	// no change : 0     edited : 1
extern char  ProgLocalN[ProgMax+1];
extern char  ProgLocalVar[ProgMax+1][26];

extern  char *TVRAM;
extern  char *GVRAM;
//------------------------------------------------------------------------------
#define StackGotoMax 10+26
#define StackGosubMax 4
#define IfCntMax 32
#define RemCntMax 32
#define StackForMax 4
#define StackWhileMax 4
#define StackDoMax 4
#define StackSwitchMax 4

typedef struct {		// 8 bytes
	short	CNT;
	short	Prg;
	short	Ptr[IfCntMax];
	short	Adrs[IfCntMax];
} CchIf;

typedef struct {		// 8 bytes
	short	CNT;
	short	Prg;
	short	Ptr[RemCntMax];
	short	Adrs[RemCntMax];
} CchRem;

typedef struct {		// 32 bytes
	short	Ptr;
	int		*Var[StackForMax];
	short	Adrs[StackForMax];
	int	IntEnd[StackForMax];
	int	IntStep[StackForMax];
	double End[StackForMax];
	double Step[StackForMax];
} StkFor;

typedef struct {		// 8 bytes
	short	WhilePtr;
	short	DoPtr;
	short	WhileAdrs[StackWhileMax];
	short	DoAdrs[StackDoMax];
} StkWhileDo;

typedef struct {		// 8 bytes
	char	Ptr;
	char	flag[StackSwitchMax];
	short	EndAdrs[StackSwitchMax];
	int		Value[StackSwitchMax];
} StkSwitch;

typedef struct {		// 16 bytes
	short	CNT;
	char	TYPE[14];
} CurrentStk;

//-----------------------------------------------------------------------------
#define SkipSpace(SRC) c=SRC[ExecPtr]; while ( c==0x20 ) c=SRC[++ExecPtr]
//------------------------------------------------------------------------------
void ClrCahche();
int CB_interpreter( char *SRC) ;
int CB_interpreter_sub( char *SRC ) ;
void CB_Prog( char *SRC, int *localvarInt, double *localvarDbl ) ; //	Prog "..."
void CB_Gosub( char *SRC, short *StackGotoAdrs, short *StackGosubAdrs ); //	Gosub N

void Skip_quot( char *SRC ); // skip "..."
void Skip_block( char *SRC );
void Skip_rem( char *SRC );	// skip '...
void CB_Rem( char *SRC, CchRem *CacheRem );
void CB_Lbl( char *SRC, short *StackGotoAdrs );
void CB_Goto( char *SRC, short *StackGotoAdrs);
void CB_If( char *SRC, CchIf *CacheIf );
void CB_Else( char *SRC, CchIf *CacheElse );
void CB_For( char *SRC ,StkFor *StackFor, CurrentStk *CurrentStruct );
void CB_Next( char *SRC ,StkFor *StackFor, CurrentStk *CurrentStruct );
void CB_While( char *SRC, StkWhileDo *StackWhileDo, CurrentStk *CurrentStruct ) ;
void CB_WhileEnd( char *SRC, StkWhileDo *StackWhileDo, CurrentStk *CurrentStruct ) ;
void CB_Do( char *SRC, StkWhileDo *StackWhileDo, CurrentStk *CurrentStruct ) ;
void CB_LpWhile( char *SRC, StkWhileDo *StackWhileDo, CurrentStk *CurrentStruct ) ;
void CB_Switch( char *SRC, StkSwitch *StackSwitch, CurrentStk *CurrentStruct ,CchIf *CacheSwitch ) ;
void CB_Case( char *SRC, StkSwitch *StackSwitch, CurrentStk *CurrentStruct ) ;
void CB_Default( char *SRC, StkSwitch *StackSwitch, CurrentStk *CurrentStruct ) ;
void CB_SwitchEnd( char *SRC, StkSwitch *StackSwitch, CurrentStk *CurrentStruct ) ;
void CB_Break( char *SRC, StkFor *StackFor, StkWhileDo *StackWhileDo, StkSwitch *StackSwitch, CurrentStk *CurrentStruct ) ;

void CB_SaveTextVRAM() ;
void CB_RestoreTextVRAM() ;
void CB_SelectTextVRAM() ;
void CB_SelectTextDD() ;
void CB_SaveGraphVRAM() ;
void CB_RestoreGraphVRAM() ;
void CB_SelectGraphVRAM() ;
void CB_SelectGraphDD() ;
void Scrl_Y();

void CB_Dsz( char *SRC ) ; //	Dsz
void CB_Isz( char *SRC ) ; //	Isz
void CB_Store( char *SRC );	// ->
int  CB_Input( char *SRC );
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

void CB_RefreshCtrl( char *SRC );	// PutDispDD Refresh control
void CB_RefreshTime( char *SRC );	// PutDispDD Refresh time
void CB_Screen( char *SRC );
void CB_ViewWindow( char *SRC ) ; //	ViewWindow
void CB_FLine( char *SRC) ; //	F-Line
void CB_Line( char *SRC ) ; //	Line
void CB_Vertical( char *SRC ) ; //	Vertical
void CB_Horizontal( char *SRC ) ; //	Horizontal
void CB_Plot( char *SRC ) ; //	Plot
void CB_PlotOn( char *SRC ) ; //	PlotOn
void CB_PlotOff( char *SRC ) ; //	PlotOff
void CB_PlotChg( char *SRC ) ; //	PlotChg
void CB_PxlOn( char *SRC ) ; //	PxlOn
void CB_PxlOff( char *SRC ) ; //	PxlOff
void CB_PxlChg( char *SRC ) ; //	PxlChg
void CB_Circle( char *SRC ) ; //	Circle

void CB_DrawGraph(  char *SRC );
void CB_GraphY( char *SRC );
void CB_StoPict( char *SRC ) ; //	StoPict
void CB_RclPict( char *SRC ) ; //	RclPict

void CB_Locate( char *SRC ) ;
void CB_LocateYX( char *SRC );
void CB_Text( char *SRC ) ; //	Text
void CB_ReadGraph( char *SRC );	// ReadGraph(px1,py1, px2,py2)->Mat C
void CB_WriteGraph( char *SRC );	// WriteGraph x,y,wx,wy,Mat A ([2,2]),modify,kind
void CB_Rect( char *SRC ) ; 	// Rect x1,y1,x2,y2,mode 
void CB_FillRect( char *SRC ) ; // FillRect x1,y1,x2,y2,mode 
void CB_DotShape( char *SRC ) ; // DotShape (x1,y1,x2,y2,typ,mode1,mode2,pattern1,pattern2)
void CB_DotGet( char *SRC );	// DotGet(px1,py1, px2,py2)->Mat B [x,y]
void CB_DotPut( char *SRC );	// DotPut(Mat B[x,y], px1,py1, px2,py2)
void CB_DotTrim( char *SRC );	// DotTrim(Mat A,x1,y1,x2,y2)->Mat B    =>[X,Y]
void CB_DotLife( char *SRC ) ;

void CB_FkeyMenu( char *SRC) ;
