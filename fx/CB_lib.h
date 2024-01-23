/*
===============================================================================

 Casio Basic RUNTIME library for fx-9860G series      v0.20

===============================================================================
*/

//-----------------------------------------------------------------------------
// Casio Basic inside
//-----------------------------------------------------------------------------
extern int	BusyInd   ;	// BusyInd=0: running indicator off,  BusyInd=1: on

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
extern double  REG[37];

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

#define Xmin REG[26]
#define Xmax REG[27]
#define Xscl REG[28]
#define Ymin REG[29]
#define Ymax REG[30]
#define Yscl REG[31]
#define TThetamin  REG[32]
#define TThetamax  REG[33]
#define TThetaptch REG[34]

#define Xdot REG[35]
#define Ydot REG[36]


extern	double Xfct;
extern	double Yfct;

extern double	traceAry[130];		// Graph trace array

extern unsigned char *GraphY;
extern unsigned char GraphY1[];
extern unsigned char GraphY2[];
extern unsigned char GraphY3[];

#define PI 3.1415926535897932

#define MatAryMax 26
extern int	MatArySizeA[MatAryMax];		// Matrix array status
extern int	MatArySizeB[MatAryMax];		// Matrix array status
extern double *MatAry[MatAryMax];			// Matrix array ptr*

//-----------------------------------------------------------------------------
double MOD(double numer, double denom);

void Text(int y, int x, unsigned char*str);

void DrawBusy();		// BusyInd=0: running indicator off,  BusyInd=1: on
void Bdisp_PutDisp_DD_DrawBusy();
void Bdisp_PutDisp_DD_DrawBusy_skip();
void Bdisp_PutDisp_DD_DrawBusy_through( unsigned char * SRC ) ;
void Bdisp_PutDisp_DD_DrawBusy_skip_through( unsigned char * SRC ) ;

//-----------------------------------------------------------------------------
int VWtoPXY(double x, double y, int *px, int *py);	// ViewWwindow(x,y) -> pixel(x,y)

void ViewWindow( double xmin, double xmax, double xscl, double ymin, double ymax, double yscl);
void ZoomIn();
void ZoomOut();

void PlotOn_VRAM(double x, double y);
void PlotOn_DD(double x, double y);
void PlotOn_DDVRAM(double x, double y);

void PlotOff_VRAM(double x, double y);
void PlotOff_DD(double x, double y);
void PlotOff_DDVRAM(double x, double y);

void PlotChg_VRAM(double x,  double y);
void PlotChg_DDVRAM(double x, double y);

void LinesubSetPoint(int px, int py) ;
void Line(int style);
void F_Line(double x1, double y1, double x2, double y2, int style);
void Vertical(double x ,int style);
void Horizontal(double y, int style);
void Circle(double x, double y, double r, int style, int drawflag);

unsigned int Plot();
unsigned int Zoom_sub(unsigned int key);
unsigned int ZoomXY();
unsigned int Trace(int *index );
unsigned int Graph_main();
//------------------------------------------------------------------------------
void PxlOn_VRAM(int py, int px);
void PxlOn_DD(int py, int px);
void PxlOn_DDVRAM(int py, int px);

void PxlOff_VRAM(int py, int px);
void PxlOff_DD(int py, int px);
void PxlOff_DDVRAM(int py, int px);

int PxlTest(int py, int px);
void PxlChg_VRAM(int py, int px);

//------------------------------------------------------------------------------
void SetVeiwWindowInit();
void SetVeiwWindowTrig();
void SetVeiwWindowSTD();

int SetViewWindwo(void);
void SetFactor();
void SetVar(int select);
void SetupG();

//------------------------------------------------------------------------------
