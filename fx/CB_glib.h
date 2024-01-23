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

void PxlOn_VRAM(int py, int px);
void PxlOn_DD(int py, int px);
void PxlOn_DDVRAM(int py, int px);

void PxlOff_VRAM(int py, int px);
void PxlOff_DD(int py, int px);
void PxlOff_DDVRAM(int py, int px);

int PxlTest(int py, int px);
void PxlChg_VRAM(int py, int px);

void LinesubSetPoint(int px, int py) ;
void Line(int style);
void F_Line(double x1, double y1, double x2, double y2, int style);
void Vertical(double x ,int style);
void Horizontal(double y, int style);
void Circle(double x, double y, double r, int style, int drawflag);

