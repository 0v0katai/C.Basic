//-----------------------------------------------------------------------------
#define Bdisp_SetPoint_VRAM  BdispSetPointVRAM2
void BdispSetPointVRAM2( int px, int py, int mode);

#define Bdisp_GetPoint_VRAM  BdispGetPointVRAM2
int BdispGetPointVRAM2( int px, int py);
//-----------------------------------------------------------------------------
double MOD(double numer, double denom);

void Text(int y, int x, unsigned char*str);

//-----------------------------------------------------------------------------
extern unsigned int skip_count;

void DrawBusy();		// BusyInd=0: running indicator off,  BusyInd=1: on
void Bdisp_PutDisp_DD_DrawBusy();
void Bdisp_PutDisp_DD_DrawBusy_skip();
void Bdisp_PutDisp_DD_DrawBusy_through( char *SRC ) ;
void Bdisp_PutDisp_DD_DrawBusy_skip_through( char *SRC ) ;

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

void PxlOn_VRAM(int py, int px);
void PxlOn_DD(int py, int px);
void PxlOn_DDVRAM(int py, int px);

void PxlOff_VRAM(int py, int px);
void PxlOff_DD(int py, int px);
void PxlOff_DDVRAM(int py, int px);

int PxlTest(int py, int px);
void PxlChg_VRAM(int py, int px);

void LinesubSetPoint(int px, int py, int mode) ;
void Linesub(int px1, int py1, int px2, int py2, int style, int mode) ;
void Line(int style, int mode);
void F_Line(double x1, double y1, double x2, double y2, int style, int mode);
void Vertical(double x ,int style, int mode);
void Horizontal(double y, int style, int mode);
void Circle(double x, double y, double r, int style, int drawflag, int mode);

