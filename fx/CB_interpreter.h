//-----------------------------------------------------------------------------
// Casio Basic inside
//-----------------------------------------------------------------------------
//extern int	BusyInd   ;	// BusyInd=0: running indicator off,  BusyInd=1: on

extern int	DrawType  ;	// 0:connect  1:Plot
extern int	Coord     ;	// 0:off 1:on
extern int	Grid      ;	// 0:off 1:on
extern int	Axes      ;	// 0:off 1:on
extern int	Label     ;	// 0:off 1:on
extern int	Derivative     ;	// 0:off 1:on

#define S_L_Normal   0
#define S_L_Thick    1
#define S_L_Broken   2
#define S_L_Dot      3
#define S_L_Default  -1

extern int S_L_Style;		// set line style 
extern int tmp_Style;		// set line style 
extern int Angle;			// 0:deg  1:rad  2:grad

extern double Previous_X ;	// Plot Previous X
extern double Previous_Y ;	// Plot Previous Y
extern int Previous_PX   ;	// Plot Previous PX
extern int Previous_PY   ;	// Plot Previous PY
extern double Plot_X     ;	// Plot Current X
extern double Plot_Y     ;	// Plot Current Y

extern int TimeDsp;

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

extern unsigned char *GraphY;
extern unsigned char GraphY1[];
extern unsigned char GraphY2[];
extern unsigned char GraphY3[];

#define PI 3.1415926535897932

#define MatAryMax 26
extern short	MatArySizeA[MatAryMax];		// Matrix array status
extern short	MatArySizeB[MatAryMax];		// Matrix array status
extern char		MatAryElementSize[MatAryMax];		// Matrix array status
extern double *MatAry[MatAryMax];			// Matrix array ptr*

//------------------------------------------------------------------------------
extern double CursorX;
extern double CursorY;

extern int ExecPtr;
extern int BreakPtr;

extern char   CB_CurrentStr[128];	//
extern double CB_CurrentValue;	// Ans

extern int CB_round;	// round mode    0:normal  1:Fix  2:Sci
extern int CB_fix;
extern int CB_sci;

#define ProgMax 10
extern int ProgEntryN;		// how many subroutin
extern int ProgNo;			// current Prog No
extern unsigned char *ProgfileAdrs[ProgMax+1];
extern int ProgfileMax[ProgMax+1] ;	// Max edit filesize 
extern int ProgfileEdit[ProgMax+1];	// no change : 0     edited : 1

#define StackGotoMax 10+26+6+26+1
#define StackForMax 8
#define StackWhileMax 8
#define StackDoMax 8

extern int BreakCheck;	// Break Stop on/off

//------------------------------------------------------------------------------
int GetQuotOpcode(unsigned char *SRC, unsigned char *buffer ) ;
void GetLocateStr(unsigned char *SRC, unsigned char *buffer ) ;
void Skip_block( unsigned char *SRC );

int StoPict( int pictNo );
void RclPict( int pictNo );

int CB_interpreter( unsigned char *SRC) ;

void CB_test() ;
