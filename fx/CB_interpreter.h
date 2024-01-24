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

extern char TimeDsp;
extern char MatXYmode;

//-----------------------------------------------------------------------------
// Casio Basic Gloval variable
//-----------------------------------------------------------------------------
extern double  REG[59];
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

extern double	traceAry[130];		// Graph trace array

#define GraphStrMAX 64
extern char *GraphY;
extern char GraphY1[];
extern char GraphY2[];
extern char GraphY3[];

#define PI 3.1415926535897932

#define MatAryMax 26
extern short	MatArySizeA[MatAryMax];		// Matrix array size
extern short	MatArySizeB[MatAryMax];		// Matrix array size
extern char		MatAryElementSize[MatAryMax];		// Matrix array word size
extern double *MatAry[MatAryMax];			// Matrix array ptr*

//------------------------------------------------------------------------------
extern int	CB_TicksStart;
extern int	CB_TicksEnd;

extern int CB_INT;		// current mode  0:normal  1: integer mode
extern int ExecPtr;
extern int BreakPtr;

extern int ScreenMode;	//  0:Text  1:Graphic
extern int UseGraphic;	// use Graph  ( no use :0    plot:1   graph:2   cls:3   other:99
extern int dspflag;

extern int CursorX;	// text cursor X
extern int CursorY;	// text cursor X

#define ProgMax 10
extern char ProgEntryN;		// how many subroutin
extern char ProgNo;			// current Prog No
extern char *ProgfileAdrs[ProgMax+1];
extern int   ProgfileMax[ProgMax+1] ;	// Max edit filesize 
extern char  ProgfileEdit[ProgMax+1];	// no change : 0     edited : 1

extern char   CB_CurrentStr[128];	//
extern double CB_CurrentValue;	// Ans

//------------------------------------------------------------------------------
#define StackGotoMax 10+26+6+26+2
#define IfCntMax 16
#define RemCntMax 16
#define StackForMax 8
#define StackWhileMax 8
#define StackDoMax 8

typedef struct {
	int	CNT;
	short	Ptr[IfCntMax];
	short	Adrs[IfCntMax];
} CchIf;

typedef struct {
	int	CNT;
	short	Ptr[RemCntMax];
	short	Adrs[RemCntMax];
} CchRem;

typedef struct {
	int	Ptr;
	short	Adrs[StackForMax];
	short	Var[StackForMax];
	int	IntEnd[StackForMax];
	int	IntStep[StackForMax];
	double End[StackForMax];
	double Step[StackForMax];
} StkFor;

typedef struct {
	short	WhilePtr;
	short	DoPtr;
	short	WhileAdrs[StackWhileMax];
	short	DoAdrs[StackDoMax];
} StkWhileDo;

typedef struct {
	int	CNT;
	char	loop[28];
} CurrentStk;

//------------------------------------------------------------------------------
void CB_SaveTextVRAM() ;
void CB_RestoreTextVRAM() ;
void CB_SelectTextVRAM() ;
void CB_SelectTextDD() ;
void CB_SaveGraphVRAM() ;
void CB_RestoreGraphVRAM() ;
void CB_SelectGraphVRAM() ;
void CB_SelectGraphDD() ;
void Scrl_Y();
void CB_Cls( char *SRC );
void CB_ClrText( char *SRC );
void CB_ClrGraph( char *SRC );
int RangeErrorCK( char *SRC ) ;

void Skip_quot( char *SRC ); // skip "..."
void Skip_rem( char *SRC );	// skip '...

void PlotXYtoPrevPXY() ;
void PlotPreviousPXY() ;
void PlotCurrentXY();

int CB_interpreter( char *SRC) ;

double CB_BinaryEval( char *SRC ) ;	// eval 2
double CB_UnaryEval( char *SRC ) ;	// eval 1

void CB_Prog( char *SRC ) ; //	Prog "..."

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

void CB_DotGet( char *SRC );	// DotGet(px1,py1, px2,py2)->Mat B [x,y]
void CB_DotPut( char *SRC );	// DotPut(Mat B[x,y], px1,py1, px2,py2)
void CB_DotTrim( char *SRC );	// DotTrim(Mat A,x1,y1,x2,y2)->Mat B    =>[X,Y]
void CB_DotLife( char *SRC ) ;

void CB_test() ;
