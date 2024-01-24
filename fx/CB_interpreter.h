//-----------------------------------------------------------------------------
// Casio Basic inside
//-----------------------------------------------------------------------------
extern short CB_INTDefault;	// default mode  0:normal  1: integer mode
extern short CB_INT;		// current mode  0:normal  1: integer mode

extern short	DrawType  ;	// 0:connect  1:Plot
extern short	Coord     ;	// 0:off 1:on
extern short	Grid      ;	// 0:off 1:on
extern short	Axes      ;	// 0:off 1:on
extern short	Label     ;	// 0:off 1:on
extern short	Derivative     ;	// 0:off 1:on

#define S_L_Normal   0
#define S_L_Thick    1
#define S_L_Broken   2
#define S_L_Dot      3
#define S_L_Default  -1

extern short S_L_Style;		// set line style 
extern short tmp_Style;		// set line style 
extern short Angle;			// 0:deg  1:rad  2:grad

extern double Previous_X ;	// Plot Previous X
extern double Previous_Y ;	// Plot Previous Y
extern int Previous_PX   ;	// Plot Previous PX
extern int Previous_PY   ;	// Plot Previous PY
extern double Plot_X     ;	// Plot Current X
extern double Plot_Y     ;	// Plot Current Y

extern short TimeDsp;

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
extern unsigned char *GraphY;
extern unsigned char GraphY1[];
extern unsigned char GraphY2[];
extern unsigned char GraphY3[];

#define PI 3.1415926535897932

#define MatAryMax 26
extern short	MatArySizeA[MatAryMax];		// Matrix array size
extern short	MatArySizeB[MatAryMax];		// Matrix array size
extern char		MatAryElementSize[MatAryMax];		// Matrix array word size
extern double *MatAry[MatAryMax];			// Matrix array ptr*

//------------------------------------------------------------------------------
extern short ScreenMode;	//  0:Text  1:Graphic
extern short UseGraphic;	// use Graph  ( no use :0    plot:1   graph:2   cls:3   other:99

extern short CursorX;	// text cursor X
extern short CursorY;	// text cursor X

extern int ExecPtr;
extern int BreakPtr;

extern char   CB_CurrentStr[128];	//
extern double CB_CurrentValue;	// Ans

extern short CB_round;	// round mode    0:normal  1:Fix  2:Sci
extern short CB_fix;
extern short CB_sci;

#define ProgMax 10
extern short ProgEntryN;		// how many subroutin
extern short ProgNo;			// current Prog No
extern unsigned char *ProgfileAdrs[ProgMax+1];
extern int   ProgfileMax[ProgMax+1] ;	// Max edit filesize 
extern short ProgfileEdit[ProgMax+1];	// no change : 0     edited : 1

#define StackGotoMax 10+26+6+26+1
#define StackForMax 8
#define StackWhileMax 8
#define StackDoMax 8

extern int	CB_TicksStart;
extern int	CB_TicksEnd;

extern short BreakCheck;	// Break Stop on/off

extern short dspflag;
extern short MatXYmode;

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
void CB_Cls( unsigned char *SRC );
void CB_ClrText( unsigned char *SRC );
void CB_ClrGraph( unsigned char *SRC );
int RangeErrorCK( unsigned char *SRC ) ;

int CB_Fix( unsigned char *SRC );
int CB_Sci( unsigned char *SRC );
int CB_Norm( unsigned char *SRC );
void CB_Rnd();


void CB_ClrMat( unsigned char *SRC ) ; //	ClrMat A
int  CB_Input( unsigned char *SRC );
int GetQuotOpcode(unsigned char *SRC, unsigned char *buffer, int Maxlen) ;
void GetQuotStr(unsigned char *SRC, unsigned char *buffer, int Maxlen ) ;
void GetLocateStr(unsigned char *SRC, unsigned char *buffer, int Maxlen ) ;
void CB_Quot( unsigned char *SRC );		// "" ""
void PrintDone() ;
void CB_Done();
void PlotXYtoPrevPXY() ;
void PlotPreviousPXY() ;
void PlotCurrentXY();
unsigned int GWait( int exit_cancel ) ;

void Skip_quot( unsigned char *SRC ); // skip "..."
void Skip_block( unsigned char *SRC );
void Skip_rem( unsigned char *SRC );	// skip '...
void CB_Lbl( unsigned char *SRC, int *StackGotoAdrs );
int Search_Lbl( unsigned char *SRC, unsigned int lc );
void CB_Goto( unsigned char *SRC, int *StackGotoAdrs);
int Search_IfEnd( unsigned char *SRC );
int Search_ElseIfend( unsigned char *SRC );
int Search_Next( unsigned char *SRC );
int Search_WhileEnd( unsigned char *SRC );
int Search_LpWhile( unsigned char *SRC );
void CB_Break( unsigned char *SRC, int *StackForPtr, int *StackWhilePtr, int *StackDoPtr, int *CurrentStructCNT, int *CurrentStructloop ) ;

void CB_GridOff() ;
void CB_GridOn() ;
void CB_AxesOff() ;
void CB_AxesOn() ;
void CB_CoordOff() ;
void CB_CoordOn() ;
void CB_LabelOff() ;
void CB_LabelOn() ;

void CB_S_L_Normal() ; //	S-L-Normal
void CB_S_L_Thick() ; //	S-L-Thick
void CB_S_L_Broken() ; //	S-L-Broken
void CB_S_L_Dot() ; //	S-L-Dot
void CB_SketchNormal() ; //	SketchNormal
void CB_SketchThick() ; //	SketchThick
void CB_SketchBroken() ; //	SketchBroken
void CB_SketchDot() ; //	SketchDot

void GetGraphStr(  unsigned char *SRC ) ;
void CB_DrawGraph(  unsigned char *SRC );
void CB_GraphY( unsigned char *SRC );

int StoPict( int pictNo );
void RclPict( int pictNo );
void CB_StoPict( unsigned char *SRC ) ; //	StoPict
void CB_RclPict( unsigned char *SRC ) ; //	RclPict

void CB_MatCalc( unsigned char *SRC ) ; //	Mat A -> Mat B  etc

int CB_SearchProg( unsigned char *name ) ; //	Prog search
void CB_Prog( unsigned char *SRC ) ; //	Prog "..."

int CB_interpreter( unsigned char *SRC) ;

void CB_test() ;
