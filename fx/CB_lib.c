/*
===============================================================================

 Casio Basic RUNTIME library for fx-9860G series     v0.32

 copyright(c)2015 by sentaro21
 e-mail sentaro21@pm.matrix.jp

===============================================================================
*/
#include <ctype.h>
#include <fxlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <timer.h>
#include "fx_syscall.h"
#include "KeyScan.h"
#include "CB_io.h"
#include "CB_inp.h"
#include "CB_lib.h"
#include "CB_Eval.h"
#include "CB_interpreter.h"
#include "CB_error.h"

//-----------------------------------------------------------------------------
// Casio Basic inside
//-----------------------------------------------------------------------------
int BusyInd     = 1;	// BusyInd=0: running indicator off,  BusyInd=1: on

int	DrawType    = 0;	// 0:connect  1:Plot
int	Coord       = 1;	// 0:off 1:on
int	Grid        = 1;	// 0:off 1:on
int	Axes        = 1;	// 0:off 1:on
int	Label       = 1;	// 0:off 1:on
int	Derivative  = 1;	// 0:off 1:on
int S_L_Style   = S_L_Normal;
int tmp_Style   = S_L_Normal;
int Angle       = 1;	// 0:deg   1:rad  2:grad

double Previous_X=1e308 ;	// Plot Previous X
double Previous_Y=1e308 ;	// Plot Previous Y
int    Previous_PX=-1   ;	// Plot Previous PX
int    Previous_PY=-1   ;	// Plot Previous PY
double Plot_X    =1e308 ;	// Plot Current X
double Plot_Y    =1e308 ;	// Plot Current Y

int TimeDsp=0;		// Execution Time Display  0:off 1:on
//-----------------------------------------------------------------------------
// Casio Basic Gloval variable
//-----------------------------------------------------------------------------
double  REG[37];

double	Xfct     =  2;					// 
double	Yfct     =  2;					// 

double	traceAry[130];		// Graph array X

unsigned char *GraphY;
unsigned char GraphY1[32];
unsigned char GraphY2[32];
unsigned char GraphY3[32];

int	MatArySizeA[26];		// Matrix array status
int	MatArySizeB[26];		// Matrix array status
double *MatAry[26];		// Matrix array ptr*

//------------------------------------------------------------------------------
void DrawBusy()		// BusyInd=0: running indicator off,  BusyInd=1: on
{
	unsigned char BusyDATA[]={ 0xff,0xff,0xff,0xff };
	DISPGRAPH Gbsy;

    if ( BusyInd == 0 ) return;
    Gbsy.x = 124; 
    Gbsy.y =   0; 
    Gbsy.GraphData.width =  4;
    Gbsy.GraphData.height = 4;
    
    Gbsy.GraphData.pBitmap = BusyDATA; 	// Busy pattern
    Gbsy.WriteModify = IMB_WRITEMODIFY_NORMAL; 
    Gbsy.WriteKind = IMB_WRITEKIND_OR;
    Bdisp_WriteGraph_DD(&Gbsy); 		// drawing only display driver
}
//-----------------------------------------------------------------------------
int skip_count=0;

void Bdisp_PutDisp_DD_DrawBusy() {
	Bdisp_PutDisp_DD();
	HourGlass();
//	DrawBusy();
}
void Bdisp_PutDisp_DD_DrawBusy_skip() {
	int t=RTC_GetTicks();
	if ( t > skip_count ) { skip_count=t+2;
		Bdisp_PutDisp_DD_DrawBusy();
	}
}
void Bdisp_PutDisp_DD_DrawBusy_through( unsigned char * SRC ) {
	unsigned char c;
	if ( SRC[ExecPtr++] == ':' ) return ;
	ExecPtr--;
	Bdisp_PutDisp_DD_DrawBusy();
}
void Bdisp_PutDisp_DD_DrawBusy_skip_through( unsigned char * SRC ) {
	unsigned char c;
	if ( SRC[ExecPtr++] == ':' ) return ;
	ExecPtr--;
	Bdisp_PutDisp_DD_DrawBusy_skip();
}
//------------------------------------------------------------------------------
double MOD(double numer, double denom) {
	return ( fmod( numer, denom ) );
}
//------------------------------------------------------------------------------

void Text(int y, int x, unsigned char*str) {
	PrintMini(  x, y, str,MINI_OVER);
}

//----------------------------------------------------------------------------------------------
int VWtoPXY(double x, double y, int *px, int *py){	// ViewWwindow(x,y) -> pixel(x,y)
	if ( ( Xdot == 0 ) || ( Ydot == 0 ) || ( Xmax == Xmin ) || ( Ymax == Ymin ) ) { ErrorNo=RangeERR; ErrorPtr=ExecPtr; return ErrorNo ; }	// Range error
	*px =   1 + ( (x-Xmin)/Xdot + 0.5 ) ;
//	if ( Xmax >  Xmin )		*px =       (x-Xmin)/Xdot  +1.5 ;
//	if ( Xmax <  Xmin )		*px = 126 - (x-Xmax)/-Xdot +1.49999999999999 ;
	*py =  63 - ( (y-Ymin)/Ydot - 0.49999999999999 ) ;
//	if ( Ymax >  Ymin )		*py =  62 - (y-Ymin)/Ydot  +1.49999999999999 ;
//	if ( Ymax <  Ymin )		*py =       (y-Ymax)/-Ydot +1.5 ;
	if ( (*px<1) || (*px>127) || (*py<1) || (*py> 63) ) { return -1; }	// 
	return 0;
}
void PXYtoVW(int px, int py, double *x, double *y){	// pixel(x,y) -> ViewWwindow(x,y)
//	if ( Xmax == Xmin )		*x = Xmin ;
	*x = (    px-1)*Xdot  + Xmin ;
//	if ( Xmax >  Xmin )		*x = (    (double)px-1)*Xdot  + Xmin ;
//	if ( Xmax <  Xmin )		*x = (126-(double)px+1)*-Xdot + Xmax ;
//	if ( Ymax == Ymin )		*y = Ymin ;
	*y = ( 62-py+1)*Ydot  + Ymin ;
//	if ( Ymax >  Ymin )		*y = ( 62-(double)py+1)*Ydot  + Ymin ;
//	if ( Ymax <  Ymin )		*y = (    (double)py-1)*-Ydot + Ymax ;
	if ( fabs(*x) < 1.0e-13 ) *x=0;	// zero adjust
	if ( fabs(*y) < 1.0e-13 ) *y=0;	// zero adjust
}

void PlotGrid(double x, double y){
	int px,py;
	if ( ( x==0 ) && ( y==0 ) ) return;
	if ( VWtoPXY( x,y, &px, &py) ) return;
	if ( (px<1) || (px>127) ) return ;
	if ( (py<1) || (py> 63) ) return ;
	Bdisp_SetPoint_VRAM( px, py, 1);
}

void GraphAxesGrid(){
	double x,y;

	if ( Axes ) {
		Horizontal(0, S_L_Normal);
		Vertical(0, S_L_Normal);
			if ( Yscl > 0 ) {
				if ( Ymin <= 0 ) for ( y=0; y>=Ymin; y-=Yscl )	PlotGrid(Xdot,y);
				if ( Ymin >  0 ) for ( y=0; y<=Ymin; y+=Yscl )	PlotGrid(Xdot,y);
				if ( Ymax <= 0 ) for ( y=0; y>=Ymax; y-=Yscl )	PlotGrid(Xdot,y);
				if ( Ymax >  0 ) for ( y=0; y<=Ymax; y+=Yscl )	PlotGrid(Xdot,y);
			}
			if ( Xscl > 0 ) {
				if ( Xmin <= 0 ) for ( x=0; x>=Xmin; x-=Xscl )	PlotGrid(x,Ydot);
				if ( Xmin >  0 ) for ( x=0; x<=Xmin; x+=Xscl )	PlotGrid(x,Ydot);
				if ( Xmax <= 0 ) for ( x=0; x>=Xmax; x-=Xscl )	PlotGrid(x,Ydot);
				if ( Xmax >  0 ) for ( x=0; x<=Xmax; x+=Xscl )	PlotGrid(x,Ydot);
			}
	}

	if ( Grid ) {
		if ( ( Xscl > 0 ) && ( Yscl > 0 ) ) {
			if ( Xmin < 0 ) { 
					for ( x=0; x>=Xmin; x-=Xscl ) {
						if ( Ymin <= 0 ) for ( y=0; y>=Ymin; y-=Yscl )	PlotGrid(x,y);
						if ( Ymin >  0 ) for ( y=0; y<=Ymin; y+=Yscl )	PlotGrid(x,y);
						if ( Ymax <= 0 ) for ( y=0; y>=Ymax; y-=Yscl )	PlotGrid(x,y);
						if ( Ymax >  0 ) for ( y=0; y<=Ymax; y+=Yscl )	PlotGrid(x,y);
					}
			}
			if ( Xmin >= 0 ) {
					for ( x=0; x<=Xmin; x+=Xscl) {
						if ( Ymin <= 0 ) for ( y=0; y>=Ymin; y-=Yscl )	PlotGrid(x,y);
						if ( Ymin >  0 ) for ( y=0; y<=Ymin; y+=Yscl )	PlotGrid(x,y);
						if ( Ymax <= 0 ) for ( y=0; y>=Ymax; y-=Yscl )	PlotGrid(x,y);
						if ( Ymax >  0 ) for ( y=0; y<=Ymax; y+=Yscl )	PlotGrid(x,y);
					}
			}
			if ( Xmax < 0 ) {
					for ( x=0; x>=Xmax; x-=Xscl) {
						if ( Ymin <= 0 ) for ( y=0; y>=Ymin; y-=Yscl )	PlotGrid(x,y);
						if ( Ymin >  0 ) for ( y=0; y<=Ymin; y+=Yscl )	PlotGrid(x,y);
						if ( Ymax <= 0 ) for ( y=0; y>=Ymax; y-=Yscl )	PlotGrid(x,y);
						if ( Ymax >  0 ) for ( y=0; y<=Ymax; y+=Yscl )	PlotGrid(x,y);
					}
			}
			if ( Xmax >= 0 ) {
					for ( x=0; x<=Xmax; x+=Xscl) {
						if ( Ymin <= 0 ) for ( y=0; y>=Ymin; y-=Yscl )	PlotGrid(x,y);
						if ( Ymin >  0 ) for ( y=0; y<=Ymin; y+=Yscl )	PlotGrid(x,y);
						if ( Ymax <= 0 ) for ( y=0; y>=Ymax; y-=Yscl )	PlotGrid(x,y);
						if ( Ymax >  0 ) for ( y=0; y<=Ymax; y+=Yscl )	PlotGrid(x,y);
					}
			}
		}
	}

	if ( Label ) {
		PrintMini(  1,17,(unsigned char*)" ",MINI_OVER);
		PrintMini(  1,17,(unsigned char*)"Y",MINI_OVER);
		PrintMini(124,43,(unsigned char*)" ",MINI_OVER);
		PrintMini(124,43,(unsigned char*)"X",MINI_OVER);
	}
//	Bdisp_PutDisp_DD();
}

void ViewWindow( double xmin, double xmax, double xscl, double ymin, double ymax, double yscl){
		
	Xmin  =xmin;
	Xmax  =xmax;
	Xscl  =xscl;
	Xdot  =(Xmax-Xmin)/126.0;
	Ymin  =ymin;
	Ymax  =ymax;
	Yscl  =yscl;
	Ydot  =(Ymax-Ymin)/ 62.0;

	Bdisp_AllClr_VRAM();			// ------ Clear VRAM 
	
	GraphAxesGrid();

	Previous_X=1e308; Previous_Y=1e308; 	// ViewWindow Previous XY init
//	regX =fabs(Xmin+Xmax)/2; regY =fabs(Ymin+Ymax)/2;	// ViewWindow Current  XY
}


void ZoomIn(){
	double c,y,dx,dy,cx,cy;

	dx   = (Xmax+Xmin)/2-regX;
	Xmin = Xmin-dx;	// move center
	Xmax = Xmax-dx;
	
	cx   = (Xmax+Xmin)/2;	//
	dx   = (Xmax-Xmin)/Xfct/2;	// zoom in dx
	Xmin = cx-dx;
	Xmax = cx+dx;
	Xdot = (Xmax-Xmin)/126.0;
	
	dy   = (Ymax+Ymin)/2-regY;	// move center
	Ymin = Ymin-dy;
	Ymax = Ymax-dy;
	
	cy   = (Ymax+Ymin)/2;	// 
	dy   = (Ymax-Ymin)/Yfct/2;	// zoom in dy
	Ymin = cy-dy;
	Ymax = cy+dy;
	Ydot = (Ymax-Ymin)/ 62.0;
	
	regX = (Xmax+Xmin)/2; // center
	regY = (Ymax+Ymin)/2; // center
}

void ZoomOut(){
	double dx,dy,cx,cy;
	
	dx   = (Xmax+Xmin)/2-regX;
	Xmin = Xmin-dx;	// move center
	Xmax = Xmax-dx;
	
	cx   = (Xmax+Xmin)/2;	//
	dx   = (Xmax-Xmin)*Xfct/2;	// zoom out dx
	Xmin = cx-dx;
	Xmax = cx+dx;
	Xdot = (Xmax-Xmin)/126.0;
	
	dy   = (Ymax+Ymin)/2-regY;	// move center
	Ymin = Ymin-dy;
	Ymax = Ymax-dy;
	
	cy   = (Ymax+Ymin)/2;	// 
	dy   = (Ymax-Ymin)*Yfct/2;	// zoom out dy
	Ymin = cy-dy;
	Ymax = cy+dy;
	Ydot = (Ymax-Ymin)/ 62.0;
	
	regX = (Xmax+Xmin)/2; // center
	regY = (Ymax+Ymin)/2; // center
}

//------------------------------------------------------------------------------ PLOT
void Plotsub(double x, double y, int kind, int mode){
	int px,py;
	if ( VWtoPXY( x,y, &px, &py) == 0) {;
		switch (mode) {
			case 1:
				Bdisp_SetPoint_VRAM( px, py, kind);
				break;
			case 2:
				Bdisp_SetPoint_DD( px, py, kind);
				break;
			case 3:
				Bdisp_SetPoint_DDVRAM( px, py , kind);
				break;
		}		
	}
	regX=x; regY=y;
}

void PlotOn_VRAM(double x, double y){
	Plotsub( x, y, 1, 1);		// VRAM
}
void PlotOn_DD(double x, double y){
	Plotsub( x, y, 1, 2);		// DD
}
void PlotOn_DDVRAM(double x, double y){
	Plotsub( x, y, 1, 3);		// DDVRAM
}
void PlotOff_VRAM(double x, double y){
	Plotsub( x, y, 0, 1);		// VRAM
}
void PlotOff_DD(double x, double y){
	Plotsub( x, y, 0, 2);		// DD
}
void PlotOff_DDVRAM(double x, double y){
	Plotsub( x, y, 0, 3);		// DDVRAM
}
void PlotChg_VRAM(double x,  double y){
	int px,py;
	if ( VWtoPXY( x,y, &px, &py) ) return;
	if ( Bdisp_GetPoint_VRAM(px, py) )
		 Bdisp_SetPoint_VRAM(px, py, 0);
	else Bdisp_SetPoint_VRAM(px, py, 1);
	regX=x; regY=y;
}

void PlotChg_DDVRAM(double x, double y){
	PlotChg_VRAM(x, y);
	Bdisp_PutDisp_DD();
}
void PxlOn_VRAM(int py, int px){
	Bdisp_SetPoint_VRAM(px, py, 1);
	PXYtoVW(px, py, &regX, &regY);
}
void PxlOn_DD(int py, int px){
	Bdisp_SetPoint_DD(px, py, 1);
	PXYtoVW(px, py, &regX, &regY);
}
void PxlOn_DDVRAM(int py, int px){
	Bdisp_SetPoint_DDVRAM(px, py, 1);
	PXYtoVW(px, py, &regX, &regY);
}
void PxlOff_VRAM(int py, int px){
	Bdisp_SetPoint_VRAM(px, py, 0);
	PXYtoVW(px, py, &regX, &regY);
}
void PxlOff_DD(int py, int px){
	Bdisp_SetPoint_DD(px, py, 0);
	PXYtoVW(px, py, &regX, &regY);
}
void PxlOff_DDVRAM(int py, int px){
	Bdisp_SetPoint_DDVRAM(px, py, 0);
	PXYtoVW(px, py, &regX, &regY);
}
int PxlTest(int py, int px) {
	return	Bdisp_GetPoint_VRAM(px, py);
}
void PxlChg_VRAM(int py, int px){
	if (PxlTest(py,px)) 
		Bdisp_SetPoint_VRAM(px, py, 0);
	else
		Bdisp_SetPoint_VRAM(px, py, 1);
	PXYtoVW(px, py, &regX, &regY);
}

//------------------------------------------------------------------------------ LINE
void LinesubSetPoint(int px, int py) {
	if ( ( px <   1 ) || ( px > 127 ) || ( py <   1 ) || ( py >  63 ) ) return;
	Bdisp_SetPoint_VRAM(px, py, 1);
}
void LinesubSetPointThick(int px, int py) {
		LinesubSetPoint(px  , py  );
		LinesubSetPoint(px  , py-1);
		LinesubSetPoint(px-1, py  );
		LinesubSetPoint(px-1, py-1);
}

void Linesub(int px1, int py1, int px2, int py2, int style) {
	int i, j;
	int x, y;
	int dx, dy; // delta x,y
	int wx, wy; // width x,y
	int Styleflag=1;
	int tmp;

	if (px1==px2) { dx= 0; wx=0; }
	if (px1< px2) { dx= 1; wx=px2-px1; }
	if (px1> px2) { dx=-1; wx=px1-px2;}
	if (py1==py2) { dy= 0; wy=0; }
	if (py1< py2) { dy= 1; wy=py2-py1; }
	if (py1> py2) { dy=-1; wy=py1-py2; }

	tmp=S_L_Style;
	if ( style >=0 ) S_L_Style=style;

	switch (S_L_Style) {
		case S_L_Normal:	// ---------- Normal
			if (wx==0) {	// vertical line
					x=px1; y=py1;
					while( wy>=0 ) {
						LinesubSetPoint(x, y);
						wy--; y+=dy;
					}
			}
			if (wy==0) { // horizontal line
					x=px1; y=py1;
					while( wx>=0 ) {
						LinesubSetPoint(x, y);
						wx--; x+=dx;
					}
			}
			
			if (wx>=wy) {
				if (dy>0) {
					x=px1; y=py1; j=wx/2; i=wx;
					while( wx>=0 ) {
						LinesubSetPoint(x, y);
						wx--; x+=dx;
						j-=wy; if (j<0) { j+=i; y++; }
					}
				}
				if (dy<0) {
					x=px1; y=py1; j=wx/2; i=wx;
					while( wx>=0 ) {
						LinesubSetPoint(x, y);
						wx--; x+=dx;
						j-=wy; if (j<0) { j+=i; y--; }
					}
				}
				
			} else {
				if (dx>0) {
					x=px1; y=py1; j=wy/2; i=wy;
					while( wy>=0 ) {
						LinesubSetPoint(x, y);
						wy--; y+=dy;
						j-=wx; if (j<0) { j+=i; x++; }
					}
				}
				if (dx<0) {
					x=px1; y=py1; j=wy/2; i=wy;
					while( wy>=0 ) {
						LinesubSetPoint(x, y);
						wy--; y+=dy;
						j-=wx; if (j<0) { j+=i; x--; }
					}
				}
				
			}
			break;
		case S_L_Dot:		// ---------- Dot
			if (wx==0) {	// vertical line
					x=px1; y=py1;
					while( wy>=0 ) {
						if (Styleflag)
						LinesubSetPoint(x, y);
						wy--; y+=dy;
						Styleflag = 1-Styleflag;
					}
			}
			if (wy==0) { // horizontal line
					x=px1; y=py1;
					while( wx>=0 ) {
						if (Styleflag)
						LinesubSetPoint(x, y);
						wx--; x+=dx;
						Styleflag = 1-Styleflag;
					}
			}
			
			if (wx>wy) {
				if (dy>0) {
					x=px1; y=py1; j=wx/2; i=wx;
					while( wx>=0 ) {
						if (Styleflag)
						LinesubSetPoint(x, y);
						wx--; x+=dx;
						j-=wy; if (j<0) { j+=i; y++; }
						Styleflag = 1-Styleflag;
					}
				}
				if (dy<0) {
					x=px1; y=py1; j=wx/2; i=wx;
					while( wx>=0 ) {
						if (Styleflag)
						LinesubSetPoint(x, y);
						wx--; x+=dx;
						j-=wy; if (j<0) { j+=i; y--; }
						Styleflag = 1-Styleflag;
					}
				}
				
			} else {
				if (dx>0) {
					x=px1; y=py1; j=wy/2; i=wy;
					while( wy>=0 ) {
						if (Styleflag)
						LinesubSetPoint(x, y);
						wy--; y+=dy;
						j-=wx; if (j<0) { j+=i; x++; }
						Styleflag = 1-Styleflag;
					}
				}
				if (dx<0) {
					x=px1; y=py1; j=wy/2; i=wy;
					while( wy>=0 ) {
						if (Styleflag)
						LinesubSetPoint(x, y);
						wy--; y+=dy;
						j-=wx; if (j<0) { j+=i; x--; }
						Styleflag = 1-Styleflag;
					}
				}
				
			}
			break;
		case S_L_Thick:	// ---------- Thick
			if (wx==0) {	// vertical line
					x=px1; y=py1;
					while( wy>=0 ) {
						LinesubSetPointThick(x  , y  );
						wy--; y+=dy;
					}
			}
			if (wy==0) { // horizontal line
					x=px1; y=py1;
					while( wx>=0 ) {
						LinesubSetPointThick(x  , y  );
						wx--; x+=dx;
					}
			}
			
			if (wx>=wy) {
				if (dy>0) {
					x=px1; y=py1; j=wx/2; i=wx;
					while( wx>=0 ) {
						LinesubSetPointThick(x  , y  );
						wx--; x+=dx;
						j-=wy; if (j<0) { j+=i; y++; }
					}
				}
				if (dy<0) {
					x=px1; y=py1; j=wx/2; i=wx;
					while( wx>=0 ) {
						LinesubSetPointThick(x  , y  );
						wx--; x+=dx;
						j-=wy; if (j<0) { j+=i; y--; }
					}
				}
				
			} else {
				if (dx>0) {
					x=px1; y=py1; j=wy/2; i=wy;
					while( wy>=0 ) {
						LinesubSetPointThick(x  , y  );
						wy--; y+=dy;
						j-=wx; if (j<0) { j+=i; x++; }
					}
				}
				if (dx<0) {
					x=px1; y=py1; j=wy/2; i=wy;
					while( wy>=0 ) {
						LinesubSetPointThick(x  , y  );
						wy--; y+=dy;
						j-=wx; if (j<0) { j+=i; x--; }
					}
				}
				
			}
			break;
		case S_L_Broken:	// ---------- Broken
			if (wx==0) {	// vertical line
					x=px1; y=py1;
					while( wy>=0 ) {
						if (Styleflag==1) {
							LinesubSetPointThick(x  , y  );
						}
						wy--; y+=dy;
						Styleflag++; if (Styleflag>2) Styleflag=0;
					}
			}
			if (wy==0) { // horizontal line
					x=px1; y=py1;
					while( wx>=0 ) {
						if (Styleflag==1) {
							LinesubSetPointThick(x  , y  );
						}
						wx--; x+=dx;
						Styleflag++; if (Styleflag>2) Styleflag=0;
					}
			}
			
			if (wx>=wy) {
				if (dy>0) {
					x=px1; y=py1; j=wx/2; i=wx;
					while( wx>=0 ) {
						if (Styleflag==1) {
							LinesubSetPointThick(x  , y  );
						}
						wx--; x+=dx;
						j-=wy; if (j<0) { j+=i; y++; }
						Styleflag++; if (Styleflag>2) Styleflag=0;
					}
				}
				if (dy<0) {
					x=px1; y=py1; j=wx/2; i=wx;
					while( wx>=0 ) {
						if (Styleflag==1) {
							LinesubSetPointThick(x  , y  );
						}
						wx--; x+=dx;
						j-=wy; if (j<0) { j+=i; y--; }
						Styleflag++; if (Styleflag>2) Styleflag=0;
					}
				}
				
			} else {
				if (dx>0) {
					x=px1; y=py1; j=wy/2; i=wy;
					while( wy>=0 ) {
						if (Styleflag==1) {
							LinesubSetPointThick(x  , y  );
						}
						wy--; y+=dy;
						j-=wx; if (j<0) { j+=i; x++; }
						Styleflag++; if (Styleflag>2) Styleflag=0;
					}
				}
				if (dx<0) {
					x=px1; y=py1; j=wy/2; i=wy;
					while( wy>=0 ) {
						if (Styleflag==1) {
							LinesubSetPointThick(x  , y  );
						}
						wy--; y+=dy;
						j-=wx; if (j<0) { j+=i; x--; }
						Styleflag++; if (Styleflag>2) Styleflag=0;
					}
				}
				
			}
			break;
		default:
			break;
		}
	S_L_Style=tmp;
}


void Line(int style) {
	int px1,px2,py1,py2;
	int i,j;
	if ( Previous_X > 1e307 ) { 
		 Previous_X = Plot_X;
		 Previous_Y = Plot_Y;
		return;
	}
	i = VWtoPXY( Previous_X, Previous_Y, &px1, &py1) ;
	j = VWtoPXY(     Plot_X,      Plot_Y, &px2, &py2) ;
	Previous_X = Plot_X;
	Previous_Y = Plot_Y;
	Previous_PX = px2;
	Previous_PY = py2;
	if ( ( i < 0 ) ||  ( i==RangeERR ) ) return ;
	if ( ( j < 0 ) ||  ( j==RangeERR ) ) return ;
	Linesub( px1, py1, px2, py2, style);
}

void F_Line(double x1, double y1, double x2, double y2, int style) {
	int px1,px2,py1,py2;
	int i,j;
	i = VWtoPXY( x1, y1, &px1, &py1) ;
	j = VWtoPXY( x2, y2, &px2, &py2) ;
	if ( ( i < 0 ) ||  ( i==RangeERR ) ) return ;
	if ( ( j < 0 ) ||  ( j==RangeERR ) ) return ;
	Linesub( px2, py2, px1, py1 ,style);
}

void Vertical(double x, int style) {
	int px,py;
	VWtoPXY( x, 0, &px, &py);
	if ( px<  0 ) return;
	if ( px>127 ) return;
	Linesub( px, 1, px, 63, style);
}
void Horizontal(double y, int style) {
	int px,py;
	VWtoPXY( 0, y, &px, &py);
	if ( py<  0 ) return;
	if ( py> 63 ) return;
	Linesub( 1, py, 127, py, style);
}

void Circle(double x, double y, double r, int style, int drawflag ) {
	double	angle, k, x0,y0,x1,y1;
	int px,py;
	int	i,n;
	if (style==S_L_Normal) { k=8; if ( ( r/Xdot )  > 20 ) k=6; }
	if (style==S_L_Dot )   { k=4; if ( ( r/Xdot )  > 6  ) k=3; }
	if (style==S_L_Thick)  k=4;
	if (style==S_L_Broken) k=2;
	n=fabs(floor(r*k/Xdot));
	Plot_X = r+x;
	Plot_Y = 0+y;
	for(i=1;i<=n;i++){
		angle=PI*2*i/n;
		Plot_X=cos(angle)*r+x;
		Plot_Y=sin(angle)*r+y;
		if ( VWtoPXY( Plot_X, Plot_Y, &px, &py) ==0 ) {
			switch ( style ) {
				case S_L_Normal:
				case S_L_Dot:
					LinesubSetPoint(px, py);
					break;
				case S_L_Thick:
				case S_L_Broken:
					LinesubSetPointThick(px, py);
					break;
			}
		}
		if ( drawflag )	Bdisp_PutDisp_DD_DrawBusy_skip();
	}
	regX=Plot_X;
	regY=Plot_Y;
}

//------------------------------------------------------------------------------
int GCursorflag = 1;	// GCursor Pixel ON:1 OFF:0
int GCursorX;
int GCursorY;

void GCursorFlashing()		// timer IRQ handler
{
	if ( ( 0<GCursorX ) && ( GCursorX<128 ) && ( 0<GCursorY ) && ( GCursorY<64 ) ) {
		switch (GCursorflag) {
			case 0:
				Bdisp_SetPoint_DDVRAM(GCursorX, GCursorY,1);
				GCursorflag=1;
				break;
			case 1:
				Bdisp_SetPoint_DDVRAM(GCursorX, GCursorY,0);
				GCursorflag=0;
				break;
			default:
				break;
		}
	}
}

void GCursorSetFlashMode(int set)	// 1:on  0:off
{
	switch (set) {
		case 0:
			KillTimer(ID_USER_TIMER1);
			break;
		case 1:
			SetTimer(ID_USER_TIMER1, 250, &GCursorFlashing);
			GCursorflag=1;		// graphic cursor initialize
			break;
		default:
			break;
	}
}

void DrawGCSR( int x, int y )
{
	unsigned char PlotCsrDATA[]={ 0x1C,0x14,0x77,0x41,0x77,0x14,0x1C };
	unsigned char PlotCsrMASK[]={ 0x1C,0x1C,0x7F,0x7F,0x7F,0x1C,0x1C };

    DISPGRAPH GCSR; 
    
	if ( (0<x) && (x<128) && (0<y) && (y<64) ) {
		GCSR.x = x-4; 
		GCSR.y = y-3; 
		GCSR.GraphData.width =	8; if ( x>124) GCSR.GraphData.width = 8-(x-124);
		GCSR.GraphData.height = 7;
	
		GCSR.GraphData.pBitmap = PlotCsrMASK; 	// mask pattern
		GCSR.WriteModify = IMB_WRITEMODIFY_NORMAL; 
		GCSR.WriteKind = IMB_WRITEKIND_OR;
		Bdisp_WriteGraph_VRAM(&GCSR);
	
		GCSR.WriteKind = IMB_WRITEKIND_XOR;
		Bdisp_WriteGraph_VRAM(&GCSR);
	
		GCSR.GraphData.pBitmap = PlotCsrDATA; 	// cursor pattern
		GCSR.WriteModify = IMB_WRITEMODIFY_NORMAL; 
		GCSR.WriteKind = IMB_WRITEKIND_OR;
		Bdisp_WriteGraph_VRAM(&GCSR); 
	
		Bdisp_SetPoint_VRAM(x,y,1);				// center dot
	}
	GCursorflag=1;		// graphic cursor initialize
}

//--------------------------------------------------------------

int PictSelectNum2( char*msg ) {		// 
	unsigned char buffer[32];
	unsigned int key;
	int n;

	PopUpWin(4);
	locate( 3,2); Print((unsigned char *)msg);
	locate( 6,3); Print((unsigned char *)"Picture Memory");
	locate( 5,5); Print((unsigned char *)"Pict[1~20]:");

	buffer[0]='\0';
	while (1) {
		key= InputStrSub( 17, 5, 2, 0, buffer, 2, ' ', REV_OFF, FLOAT_OFF, EXP_OFF, ALPHA_OFF, HEX_OFF, PAL_ON, EXIT_CANCEL_OFF) ;
		if ( ( key == KEY_CTRL_EXIT ) || ( key != KEY_CTRL_EXE ) ) return -1;  // exit
		n=atof( (char*)buffer );
 		if ( (1<=n)&&(n<=20) ) break;
 		n=0;
 	}
	return n ; // ok
}

unsigned int Pict() {
	int cont=1;
	unsigned int key;
	int n;

	while ( cont ) {
		locate(1,8); PrintLine((unsigned char *)" ",21);
		Fkey_dispR( 0, "PICT");
		GetKey(&key);
		switch (key) {
			case KEY_CTRL_EXIT:
				key=0;
			case KEY_CTRL_AC:
			case KEY_CTRL_EXE:
				cont=0;
				break;
			case KEY_CTRL_F1:
				Fkey_dispR( 0, "STO");
				Fkey_dispR( 1, "RCL");
				GetKey(&key);
				switch (key) {
					case KEY_CTRL_AC:
					case KEY_CTRL_EXE:
						cont=0;
					case KEY_CTRL_EXIT:
						break;
					case KEY_CTRL_F1:
						n=PictSelectNum2( "Store In" );
						if (n>0) { 
							RestoreDisp(SAVEDISP_PAGE1);	// ------ RestoreDisp1
							StoPict(n);
							cont=0;
						}
						break;
					case KEY_CTRL_F2:
						n=PictSelectNum2( "Recall From" );
						if (n>0) { 
							RestoreDisp(SAVEDISP_PAGE1);	// ------ RestoreDisp1
							RclPict(n);
							SaveDisp(SAVEDISP_PAGE1);		// ------ SaveDisp1
							cont=0;
						}
						break;
					default:
						break;
				}
			case KEY_CTRL_SHIFT:
				cont=0;
				break;
			default:
				break;
		}
	}

	RestoreDisp(SAVEDISP_PAGE1);	// ------ RestoreDisp1
	return key;
}

//--------------------------------------------------------------
unsigned int Plot()
{
	int cont=1;
	unsigned char buffer[21];
	unsigned int key;
	int retcode=0;
	int px1,py1;
	
	long FirstCount;	// pointer to repeat time of first repeat
	long NextCount; 	// pointer to repeat time of second repeat

//	while( KeyCheckEXE() );
//	while( KeyCheckEXIT() );
//	while( KeyCheckAC() );
	KeyRecover();
	
	if ( VWtoPXY( Plot_X, Plot_Y, &GCursorX, &GCursorY) ) return;	// VW(X,Y) to  graphic cursor XY
	
	Bkey_Get_RepeatTime(&FirstCount,&NextCount);	// repeat time
	Bkey_Set_RepeatTime(FirstCount,2);				// set graphic cursor repeat time  (count * 25ms)

	SaveDisp(SAVEDISP_PAGE1);	// ------ SaveDisp1
	while ( cont ) {	
		GCursorSetFlashMode(1);	// graphic cursor flashing on
		RestoreDisp(SAVEDISP_PAGE1);	// ------ RestoreDisp1
		SaveDisp(SAVEDISP_PAGE1);		// ------ SaveDisp1
		PXYtoVW(GCursorX, GCursorY, &Plot_X, &Plot_Y);	// graphic cursor XY  to  VW(X,Y)
		if ( Coord ) {
			PrintMini(  0,58,(unsigned char*)"X=",MINI_OVER);
			sprintGRS(buffer, Plot_X, 13,LEFT_ALIGN, Norm,10); PrintMini(  8,58,(unsigned char*)buffer,MINI_OVER);
			PrintMini( 64,58,(unsigned char*)"Y=",MINI_OVER);
			sprintGRS(buffer, Plot_Y, 13,LEFT_ALIGN, Norm,10); PrintMini( 72,58,(unsigned char*)buffer,MINI_OVER);
		}
		DrawGCSR(GCursorX,GCursorY); 	// draw graphic cursor
//		Bdisp_PutDisp_DD();

		GetKey(&key);
		if ( key==KEY_CTRL_OPTN ) key=Pict();
		switch (key) {
			case KEY_CTRL_EXE:
			case KEY_CTRL_AC:
			case KEY_CTRL_EXIT:
				cont=0;
				break;
			case KEY_CTRL_F3:	// setViewWindow
				GCursorSetFlashMode(0);	// graphic cursor flashing off
				if ( SetViewWindow() ) cont=0;
				break;
			case KEY_CTRL_LEFT:
				if ( GCursorX >   1 ) GCursorX--;
				break;
			case KEY_CTRL_RIGHT:
				if ( GCursorX < 127 ) GCursorX++;
				break;
			case KEY_CTRL_UP:
				if ( GCursorY >   1 ) GCursorY--;
				break;
			case KEY_CTRL_DOWN:
				if ( GCursorY <  63 ) GCursorY++;
				break;
			case KEY_CTRL_SHIFT:
				locate(1,8); PrintLine((unsigned char *)" ",21);
				Fkey_dispR( 0, "Var");
				Fkey_dispR( 2, "V-W");
				Fkey_dispN( 5, "G<>T");
				GetKey(&key);
				switch (key) {
					case KEY_CTRL_EXIT:
						break;
					case KEY_CTRL_SETUP:
						GCursorSetFlashMode(0);	// graphic cursor flashing off
						SetupG();
						break;
					case KEY_CTRL_F1:
						GCursorSetFlashMode(0);	// graphic cursor flashing off
						SetVar(0);		// A - 
						break;
					case KEY_CTRL_F3:
						GCursorSetFlashMode(0);	// graphic cursor flashing off
						if ( SetViewWindow() ) cont=0;
						break;
					case KEY_CTRL_F6:
						cont=0;
						break;
					default:
					break;
				}
				break;
			default:
				break;
		}
	}
	GCursorSetFlashMode(0);	// graphic cursor flashing off
	Bkey_Set_RepeatTime(FirstCount,NextCount);	// restore repeat time
	RestoreDisp(SAVEDISP_PAGE1);	// ------ RestoreDisp1
//	if ( retcode==0 ) Bdisp_SetPoint_VRAM( GCursorX, GCursorY, 1);
	regX = Plot_X ;
	regY = Plot_Y ;
	return key ;
}

//----------------------------------------------------------------------------------------------
// Graph function
//----------------------------------------------------------------------------------------------
void FkeyZoom(){
	locate(1,8); PrintLine((unsigned char *)" ",21);
	Fkey_dispR( 1, "FACT");
	Fkey_dispN( 2, " IN");
	Fkey_dispN( 3, "OUT");
}
void FkeyGraph(){
	locate(1,8); PrintLine((unsigned char *)" ",21);
	Fkey_dispN( 0, "TRCE");
	Fkey_dispR( 1, "ZOOM");
	Fkey_dispR( 2, "V-W");
	Fkey_dispN( 5, "G<>T");
}
//--------------------------------------------------------------
unsigned int ZoomXY() {
	int cont=1;
	unsigned char buffer[21];
	unsigned int key;
	int retcode=0;
	
	long FirstCount;	// pointer to repeat time of first repeat
	long NextCount; 	// pointer to repeat time of second repeat

	regX=(Xmax+Xmin)/2; regY=(Ymax+Ymin)/2;		// center
	if ( VWtoPXY( regX, regY, &GCursorX, &GCursorY) ) return;	// VW(X,Y) to  graphic cursor XY
	
	Bkey_Get_RepeatTime(&FirstCount,&NextCount);	// repeat time
	Bkey_Set_RepeatTime(FirstCount,2);				// set graphic cursor repeat time  (count * 25ms)

	SaveDisp(SAVEDISP_PAGE1);	// ------ SaveDisp1
	GCursorSetFlashMode(1);	// graphic cursor flashing on
	while ( cont ) {	
		RestoreDisp(SAVEDISP_PAGE1);	// ------ RestoreDisp1
		SaveDisp(SAVEDISP_PAGE1);		// ------ SaveDisp1
		PXYtoVW(GCursorX, GCursorY, &regX, &regY);	// graphic cursor XY  to  VW(X,Y)
		if ( Coord ) {
			PrintMini(  0,58,(unsigned char*)"X=",MINI_OVER);
			sprintGRS(buffer, regX, 13,LEFT_ALIGN, Norm,10); PrintMini(  8,58,(unsigned char*)buffer,MINI_OVER);
			PrintMini( 64,58,(unsigned char*)"Y=",MINI_OVER);
			sprintGRS(buffer, regY, 13,LEFT_ALIGN, Norm,10); PrintMini( 72,58,(unsigned char*)buffer,MINI_OVER);
		}
		DrawGCSR(GCursorX,GCursorY); 	// draw graphic cursor
		Bdisp_PutDisp_DD();

		GetKey(&key);
		switch (key) {
			case KEY_CTRL_AC:
			case KEY_CTRL_EXIT:
			case KEY_CTRL_EXE:
			case KEY_CTRL_F1:	// trace
			case KEY_CTRL_F6:	//
				cont=0;
				break;
			case KEY_CTRL_F3:	// setViewWindow
				if ( SetViewWindow() ) cont=0;
				break;
			case KEY_CTRL_LEFT:
				if ( GCursorX >   1 ) GCursorX--;
				break;
			case KEY_CTRL_RIGHT:
				if ( GCursorX < 127 ) GCursorX++;
				break;
			case KEY_CTRL_UP:
				if ( GCursorY >   1 ) GCursorY--;
				break;
			case KEY_CTRL_DOWN:
				if ( GCursorY <  63 ) GCursorY++;
				break;
			default:
				break;
		}
	}
	GCursorSetFlashMode(0);	// graphic cursor flashing off
	Bkey_Set_RepeatTime(FirstCount,NextCount);	// restore repeat time
	RestoreDisp(SAVEDISP_PAGE1);	// ------ RestoreDisp1
	return key ;
}
//--------------------------------------------------------------
unsigned int Zoom_sub(unsigned int key){
	double x,y;

	FkeyZoom();

	if (key==0) GetKey(&key);
	switch (key) {
		case KEY_CTRL_AC:
		case KEY_CTRL_EXIT:
		case KEY_CTRL_F6:
			break;
		case KEY_CTRL_F2:
			SetFactor();
			break;
		case KEY_CTRL_F3:
			RestoreDisp(SAVEDISP_PAGE1);	// ------ RestoreDisp1
			key=ZoomXY();
			if ( key == KEY_CTRL_EXIT ) return key ;		// select zoom center 
			if ( key == KEY_CTRL_F1   ) return key ;		// trace
			ZoomIn();
			break;
		case KEY_CTRL_F4:
			RestoreDisp(SAVEDISP_PAGE1);	// ------ RestoreDisp1
			key=ZoomXY();
			if ( key == KEY_CTRL_EXIT ) return key ;		// select zoom center 
			if ( key == KEY_CTRL_F1   ) return key ;		// trace
			ZoomOut();
			break;
		default:
		break;
	}
	return key; 
}

//----------------------------------------------------------------------------------------------
unsigned int Trace(int *index ) {
	int cont=1;
	unsigned char buffer[21];
	unsigned int key;
	double dydx;
	
	long FirstCount;	// pointer to repeat time of first repeat
	long NextCount; 	// pointer to repeat time of second repeat

	if ( *index <   1 ) *index=  0;
	if ( *index > 127 ) *index=128;
	PXYtoVW(*index, 0, &regX, &regY);	// graphic cursor X  to  VW(X,dummy)
	VWtoPXY( regX, traceAry[*index], &GCursorX, &GCursorY);	// VW(X,Y) to  graphic cursor XY
	
	Bkey_Get_RepeatTime(&FirstCount,&NextCount);	// repeat time
	Bkey_Set_RepeatTime(FirstCount,2);				// set graphic cursor repeat time  (count * 25ms)

	SaveDisp(SAVEDISP_PAGE1);	// ------ SaveDisp1
	GCursorSetFlashMode(1);	// graphic cursor flashing on
	while ( cont ) {	
		RestoreDisp(SAVEDISP_PAGE1);	// ------ RestoreDisp1
		SaveDisp(SAVEDISP_PAGE1);		// ------ SaveDisp1
		PXYtoVW(GCursorX, 0, &regX, &regY);	// graphic cursor X  to  VW(X,dummy)
		VWtoPXY( regX, traceAry[GCursorX], &GCursorX, &GCursorY);	// VW(X,Y) to  graphic cursor XY
		if ( Coord ) {
			sprintf((char*)buffer, "PX=%d", GCursorX);	PrintMini(  0,0,(unsigned char*)buffer,MINI_OVER);
			sprintf((char*)buffer, "PY=%d", GCursorY);	PrintMini( 64,0,(unsigned char*)buffer,MINI_OVER);
			PrintMini(  0,58,(unsigned char*)"X=",MINI_OVER);
			sprintGRS(buffer, regX,             13,LEFT_ALIGN, Norm,10); PrintMini(  8,58,(unsigned char*)buffer,MINI_OVER);
			PrintMini( 64,58,(unsigned char*)"Y=",MINI_OVER);
			sprintGRS(buffer,traceAry[GCursorX],13,LEFT_ALIGN, Norm,10); PrintMini( 72,58,(unsigned char*)buffer,MINI_OVER);
		}
		if ( Derivative ) {
			PrintMini( 64,50,(unsigned char*)"dY/dX=",MINI_OVER);
			dydx = (traceAry[GCursorX+1]-traceAry[GCursorX-1]) / (Xdot*2);
			sprintGRS(buffer, dydx, 6,LEFT_ALIGN, Norm,5); PrintMini( 88,50,(unsigned char*)buffer,MINI_OVER);
		}
		DrawGCSR(GCursorX,GCursorY); 	// draw graphic cursor
		Bdisp_PutDisp_DD();

		GetKey(&key);
		if ( key==KEY_CTRL_OPTN ) key=Pict();
		switch (key) {
			case KEY_CTRL_AC:
			case KEY_CTRL_F1:
			case KEY_CTRL_EXE:
			case KEY_CTRL_EXIT:
				cont=0;
				break;
			case KEY_CTRL_F2:
				FkeyZoom();
				GetKey(&key);
				switch (key) {
					case KEY_CTRL_EXIT:
					case KEY_CTRL_F2:
					case KEY_CTRL_F3:
					case KEY_CTRL_F4:
					case KEY_CTRL_F6:
						cont=0;
						break;
					default:
						break;
				}
				break;
			case KEY_CTRL_F3:	// setViewWindow
				key=KEY_CHAR_3;
				cont=0;
				break;
			case KEY_CTRL_LEFT:
				GCursorX--;
				if ( GCursorX <   1 )  cont=0;
				break;
			case KEY_CTRL_RIGHT:
				GCursorX++;
				if ( GCursorX > 127 )  cont=0;
				break;
			case KEY_CTRL_SHIFT:
				FkeyGraph();
				GetKey(&key);
				switch (key) {
					case KEY_CTRL_EXIT:
					case KEY_CTRL_F2:
					case KEY_CTRL_F3:
					case KEY_CTRL_F4:
					case KEY_CTRL_F6:
						cont=0;
						break;
					default:
						break;
				}
				break;
			default:
				break;
		}
	}
	GCursorSetFlashMode(0);	// graphic cursor flashing off
	Bkey_Set_RepeatTime(FirstCount,NextCount);	// restore repeat time
	RestoreDisp(SAVEDISP_PAGE1);	// ------ RestoreDisp1
	*index=GCursorX;
	return key ;
}

//----------------------------------------------------------------------------------------------
double Graph_Eval( unsigned char *expbuf) {		// Eval Graph
	unsigned int key;
	double result;
    ExpPtr= 0;
    ErrorPtr= 0;
    result = EvalsubTop( expbuf );
	return result;
}

void Graph_Draw(){
	int i;
	GraphAxesGrid( Xmin, Xmax, Xscl, Ymin, Ymax, Yscl);
	regX   = Xmin-Xdot;
	Plot_X = regX;
	for ( i=0; i<=128; i++) {
		//-----------------------------
		traceAry[i]=Graph_Eval((unsigned char *)GraphY);		// function
		if ( ErrorPtr ) return ;
		//-----------------------------
//		if ( fabs(traceAry[i])<1.0e-13 ) traceAry[i]=0;	// zero adjust
		if ( i==0 ) { Previous_X = Plot_X; Previous_Y = traceAry[i]; }
		if ( ( 0<i ) && ( i<128 ) ) {
			Plot_Y=traceAry[i];
			if ( DrawType ) {	// 1:Plot
				PlotOn_VRAM( Plot_X, Plot_Y);
			} else {			// 0:connect
				Line( S_L_Default );
				regX = Plot_X;
				regY = Plot_Y;
			}
			Bdisp_PutDisp_DD_DrawBusy_skip();
		}
		Plot_X += Xdot;
	}
	SaveDisp(SAVEDISP_PAGE1);	// ------ SaveDisp1
}
void Graph_reDraw(){
	int i;
	ViewWindow( Xmin, Xmax, Xscl, Ymin, Ymax, Yscl);
	Bdisp_AllClr_VRAM();			// ------ Clear VRAM 
	Graph_Draw();
}
//--------------------------------------------------------------
unsigned int Graph_trace_sub(int *tracex){
	unsigned int key;
	int tx,ty; // dummy
	tx=*tracex;
	while (1) {
		if ( tx <   1 ) tx=  0;
		if ( tx > 127 ) tx=128;
		key=Trace(&tx);			// trace
		if ( key==KEY_CTRL_EXIT) break; //exit
		if ( key==KEY_CTRL_EXE ) break; //exe
		if ( key==KEY_CTRL_AC  ) break; // AC
		if ( key==KEY_CTRL_F6  ) break; // F6
		if ( key==KEY_CTRL_F2  ) {
			Zoom_sub(KEY_CTRL_F2); // F2  Zoom fact
			Graph_reDraw();
			break;
		}
		if ( key==KEY_CTRL_F3 ) { // Zoom in
			key=Zoom_sub(KEY_CTRL_F3); // F3  Zoom in
			if ( key==KEY_CTRL_EXIT) break; //exit
			if ( key==KEY_CTRL_EXE ) break; //exe
			if ( key==KEY_CTRL_AC  ) break; // AC
			if ( key==KEY_CTRL_F6  ) break; // F6
			if ( key!=KEY_CTRL_F1  ) {
				Graph_reDraw();
				break;
			}
		}
		if ( key==KEY_CTRL_F4 ) { // Zoom out
			key=Zoom_sub(KEY_CTRL_F4); // F4  Zoom out
			if ( key==KEY_CTRL_EXIT) break; //exit
			if ( key==KEY_CTRL_EXE ) break; //exe
			if ( key==KEY_CTRL_AC  ) break; // AC
			if ( key==KEY_CTRL_F6  ) break; // F6
			if ( key!=KEY_CTRL_F1  ) {
				Graph_reDraw();
				break;
			}
		}
		if ( key==KEY_CHAR_3 ) { // SetViewWindow
			SetViewWindow();
			Graph_reDraw();
			break;
		}
		if ( key==KEY_CTRL_LEFT ) { 
			Xmin-=Xdot*8;
			Xmax-=Xdot*8;
			Graph_reDraw();
			VWtoPXY(Xmin+Xdot*7, 0, &tx, &ty);
		}
		if ( key==KEY_CTRL_RIGHT ) { 
			Xmin+=Xdot*8;
			Xmax+=Xdot*8;
			Graph_reDraw();
			VWtoPXY(Xmax-Xdot*7, 0, &tx, &ty);
		}
	}
	*tracex=tx;
	return key;
}

//--------------------------------------------------------------
//--------------------------------------------------------------
unsigned int Graph_main(){
	char buffer[21];
	unsigned int key;
	int cont=1;
	int tx=64;	// trace center 
	
	Graph_Draw();
	
//	while( KeyCheckEXE() );
//	while( KeyCheckEXIT() );
//	while( KeyCheckAC() );
	KeyRecover();

	while (cont) {
		SaveDisp(SAVEDISP_PAGE1);		// ------ SaveDisp1
		if ( key==KEY_CTRL_OPTN ) key=Pict();
		GetKey(&key);
		switch (key) {
			case KEY_CTRL_AC:
			case KEY_CTRL_EXE:
			case KEY_CTRL_EXIT:
			case KEY_CTRL_F6:
				cont=0;
				break;
			case KEY_CTRL_LEFT:
				Xmin-=Xdot*12;
				Xmax-=Xdot*12;
				Graph_reDraw();
				break;
			case KEY_CTRL_RIGHT:
				Xmin+=Xdot*12;
				Xmax+=Xdot*12;
				Graph_reDraw();
				break;
			case KEY_CTRL_UP:
				Ymin+=Ydot*12;
				Ymax+=Ydot*12;
				Graph_reDraw();
				break;
			case KEY_CTRL_DOWN:
				Ymin-=Ydot*12;
				Ymax-=Ydot*12;
				Graph_reDraw();
				break;
			case KEY_CTRL_SHIFT:
				FkeyGraph();
				GetKey(&key);
				switch (key) {
					case KEY_CTRL_EXIT:
					case KEY_CTRL_F6:
						cont=0;
						break;
					case KEY_CTRL_SETUP:
						SetupG();
						Graph_reDraw();
						break;
					case KEY_CTRL_F1:
						RestoreDisp(SAVEDISP_PAGE1);	// ------ RestoreDisp1
						key=Graph_trace_sub(&tx);	// trace
						if ( key==KEY_CTRL_EXIT ) cont=0; //exit
						if ( key==KEY_CTRL_EXE  ) cont=0; //exe
						if ( key==KEY_CTRL_AC   ) cont=0; // AC
						if ( key==KEY_CTRL_F6   ) cont=0; // F6
						break;
					case KEY_CTRL_F2:
						RestoreDisp(SAVEDISP_PAGE1);	// ------ RestoreDisp1
						key=Zoom_sub(0);	// zoom
						if ( key==KEY_CTRL_F1   ) key=Graph_trace_sub(&tx);	// trace
						if ( key==KEY_CTRL_EXIT ) cont=0; //exit
						if ( key==KEY_CTRL_AC   ) cont=0; // AC
						if ( key==KEY_CTRL_F6   ) cont=0; // F6
						Graph_reDraw();
						break;
					case KEY_CTRL_F3:
						SetViewWindow();
						Graph_reDraw();
						break;
					default:
					break;
				}
				RestoreDisp(SAVEDISP_PAGE1);	// ------ RestoreDisp1
				break;
			case KEY_CTRL_F1:
				RestoreDisp(SAVEDISP_PAGE1);	// ------ RestoreDisp1
				key=Graph_trace_sub(&tx);	// trace
				if ( key==KEY_CTRL_EXIT ) cont=0; //exit
				if ( key==KEY_CTRL_EXE  ) cont=0; //exe
				if ( key==KEY_CTRL_AC   ) cont=0; // AC
				if ( key==KEY_CTRL_F6   ) cont=0; // F6
				break;
			case KEY_CTRL_F2:
				RestoreDisp(SAVEDISP_PAGE1);	// ------ RestoreDisp1
				key=Zoom_sub(0);	// zoom
				if ( key==KEY_CTRL_F1   ) key=Graph_trace_sub(&tx);	// trace
				if ( key==KEY_CTRL_EXIT ) cont=0; //exit
				if ( key==KEY_CTRL_AC   ) cont=0; // AC
				if ( key==KEY_CTRL_F6   ) cont=0; // F6
				Graph_reDraw();
				break;
			case KEY_CTRL_F3:
				SetViewWindow();
				Graph_reDraw();
				break;
			default:
				break;
		}
		
		Bdisp_PutDisp_DD();
	}
	return key;
}

//----------------------------------------------------------------------------------------------
//		Setup
//----------------------------------------------------------------------------------------------

void FkeyS_L_(){
	int temp;
	temp=S_L_Style;
	Fkey_Clear( 0 );
	Fkey_dispN( 0, ""); Linesub(0*21+5,7*8+4,0*21+18,7*8+4, S_L_Normal);	// -----
	Fkey_Clear( 1 );
	Fkey_dispN( 1, ""); Linesub(1*21+5,7*8+4,1*21+18,7*8+4, S_L_Thick);	// =====
	Fkey_Clear( 2 );
	Fkey_dispN( 2, ""); Linesub(2*21+5,7*8+4,2*21+18,7*8+4, S_L_Broken);	// : : : :
	Fkey_Clear( 3 );
	Fkey_dispN( 3, ""); Linesub(3*21+5,7*8+4,3*21+18,7*8+4, S_L_Dot);	// . . . .
	Fkey_Clear( 4 );
	Fkey_Clear( 5 );
	S_L_Style=temp;
}

void SetLineStyle() {
	unsigned int key;
	
	FkeyS_L_();
	
	GetKey(&key);
	switch (key) {
		case KEY_CTRL_EXIT:
			break;
		case KEY_CTRL_F1:
			S_L_Style=S_L_Normal;
			break;
		case KEY_CTRL_F2:
			S_L_Style=S_L_Thick ;
			break;
		case KEY_CTRL_F3:
			S_L_Style=S_L_Broken;
			break;
		case KEY_CTRL_F4:
			S_L_Style=S_L_Dot;
			break;
		default:
		break;
	}
}

//-----------------------------------------------------------------------------

void SetVeiwWindowInit(){	// Initialize	retrun 0: no change  -1 : change
		Xmin  =-6.3;
		Xmax  = 6.3;
		Xscl= 1;
		Xdot  = 0.1;
		Ymin  =-3.1;
		Ymax  = 3.1;
		Yscl= 1;
		Ydot = (Ymax-Ymin)/ 62.0;
		TThetamin = 0;
		TThetamax = 6.2831853071796;
		TThetaptch= 0.062831853071796;
}
void SetVeiwWindowTrig(){	// trig Initialize
		Xmin  =-9.4247779607694;
		Xmax  = 9.4247779607694;
		Xscl= 1.5707963267949;
		Xdot  = 0.149599650170942;
		Ymin  =-1.6;
		Ymax  = 1.6;
		Yscl= 0.5;
		Ydot = (Ymax-Ymin)/ 62.0;
		TThetamin = 0;
		TThetamax = 6.2831853071796;
		TThetaptch= 0.062831853071796;
}
void SetVeiwWindowSTD(){	// STD Initialize
		Xmin  =-10;
		Xmax  = 10;
		Xscl= 1;
		Xdot  =  0.158730158730159;
		Ymin  =-10;
		Ymax  = 10;
		Yscl= 1;
		Ydot = (Ymax-Ymin)/ 62.0;
		TThetamin = 0;
		TThetamax = 6.2831853071796;
		TThetaptch= 0.062831853071796;
}


unsigned int MathKey( unsigned int  key) {
	switch ( key ) {
			case KEY_CHAR_0:
			case KEY_CHAR_1:
			case KEY_CHAR_2:
			case KEY_CHAR_3:
			case KEY_CHAR_4:
			case KEY_CHAR_5:
			case KEY_CHAR_6:
			case KEY_CHAR_7:
			case KEY_CHAR_8:
			case KEY_CHAR_9:
			case KEY_CHAR_DP:
			case KEY_CHAR_EXP:
			case KEY_CHAR_PLUS:
			case KEY_CHAR_MINUS:
			case KEY_CHAR_PMINUS:
			case KEY_CHAR_A:
			case KEY_CHAR_B:
			case KEY_CHAR_C:
			case KEY_CHAR_D:
			case KEY_CHAR_E:
			case KEY_CHAR_F:
			case KEY_CHAR_G:
			case KEY_CHAR_H:
			case KEY_CHAR_I:
			case KEY_CHAR_J:
			case KEY_CHAR_K:
			case KEY_CHAR_L:
			case KEY_CHAR_M:
			case KEY_CHAR_N:
			case KEY_CHAR_O:
			case KEY_CHAR_P:
			case KEY_CHAR_Q:
			case KEY_CHAR_R:
			case KEY_CHAR_S:
			case KEY_CHAR_T:
			case KEY_CHAR_U:
			case KEY_CHAR_V:
			case KEY_CHAR_W:
			case KEY_CHAR_X:
			case KEY_CHAR_Y:
			case KEY_CHAR_Z:
			case KEY_CTRL_XTT:
			case KEY_CHAR_FRAC:   //    0xbb
			case KEY_CHAR_LPAR:   //    0x28
			case KEY_CHAR_LOG:    //    0x95
			case KEY_CHAR_LN:     //    0x85
			case KEY_CHAR_SIN:    //    0x81
			case KEY_CHAR_COS:    //    0x82
			case KEY_CHAR_TAN:    //    0x83
			case KEY_CHAR_SQUARE: //    0x8b
			case KEY_CHAR_POW:    //    0xa8
			case KEY_CHAR_PI:     //    0xd0
			case KEY_CHAR_CUBEROOT: //  0x96
			case KEY_CHAR_EXPN10: //    0xb5
			case KEY_CHAR_EXPN:   //    0xa5
			case KEY_CHAR_ASIN:   //    0x91
			case KEY_CHAR_ACOS:   //    0x92
			case KEY_CHAR_ATAN:   //    0x93
			case KEY_CHAR_ROOT:   //    0x86
			case KEY_CHAR_POWROOT: //   0xb8
				if ( key == KEY_CTRL_XTT ) key='X';
//				if ( key == KEY_CHAR_PLUS  )  key='+';
//				if ( key == KEY_CHAR_MINUS )  key='-';
//				if ( key == KEY_CHAR_PMINUS ) key=0x87; // (-)
				return key;
			default:
				break;
		}
	return 0;
}


int SetViewWindow(void){		// ----------- Set  View Window variable	retrun 0: no change  -1 : change
	unsigned char buffer[22];
	unsigned int key;
	int	cont=1;
	int select=0;
	int scrl=0;
	int y;

	double	xmin      = Xmin      ;
	double	xmax      = Xmax      ;
	double	xscl      = Xscl      ;
	double	xdot      = Xdot      ;
	double	ymin      = Ymin      ;
	double	ymax      = Ymax      ;
	double	ydot      = Ydot      ;
	double	yscl      = Yscl      ;
	double	tThetamin = TThetamin ;
	double	tThetamax = TThetamax ;
	double	tThetaptch= TThetaptch;

	while (cont) {
		Bdisp_AllClr_VRAM();
		locate( 1,1);Print((unsigned char*)"View Window");

		if ( scrl <=0 ) {
			locate( 1, 2-scrl); Print((unsigned char*)"Xmin  :");
			sprintG(buffer,Xmin,  10,LEFT_ALIGN); locate( 8, 2-scrl); Print(buffer);
		}
		if ( scrl <=1 ) {
			locate( 1, 3-scrl); Print((unsigned char*)" max  :");
			sprintG(buffer,Xmax,  10,LEFT_ALIGN); locate( 8, 3-scrl); Print(buffer);
		}
		if ( scrl <=2 ) {
			locate( 1, 4-scrl); Print((unsigned char*)" scale:");
			sprintG(buffer,Xscl,10,LEFT_ALIGN); locate( 8, 4-scrl); Print(buffer);
		}
		if ( scrl <=3 ) {
			locate( 1, 5-scrl); Print((unsigned char*)" dot  :");
			sprintG(buffer,Xdot,  10,LEFT_ALIGN); locate( 8, 5-scrl); Print(buffer);
		}
		if ( scrl <=4 ) {
			locate( 1, 6-scrl); Print((unsigned char*)"Ymin  :");
			sprintG(buffer,Ymin,  10,LEFT_ALIGN); locate( 8, 6-scrl); Print(buffer);
		}
		if ( scrl <=5 ) {
			locate( 1, 7-scrl); Print((unsigned char*)" max  :");
			sprintG(buffer,Ymax,  10,LEFT_ALIGN); locate( 8, 7-scrl); Print(buffer);
		}
		if ( scrl >=1 ) {
			locate( 1, 8-scrl); Print((unsigned char*)" scale:");
			sprintG(buffer,Yscl,10,LEFT_ALIGN); locate( 8, 8-scrl); Print(buffer);
		}
		if ( scrl >=2 ) {
			locate( 1, 9-scrl); Print((unsigned char*)"Temin :");
			sprintG(buffer,TThetamin,  10,LEFT_ALIGN); locate( 8, 9-scrl); Print(buffer);
		}
		if ( scrl >=3 ) {
			locate( 1, 10-scrl); Print((unsigned char*)"  max :");
			sprintG(buffer,TThetamax,  10,LEFT_ALIGN); locate( 8, 10-scrl); Print(buffer);
		}
		if ( scrl >=4 ) {
			locate( 1, 11-scrl); Print((unsigned char*)"  ptch:");
			sprintG(buffer,TThetaptch,  10,LEFT_ALIGN); locate( 8, 11-scrl); Print(buffer);
		}

		y = select-scrl+1;
		Bdisp_AreaReverseVRAM(0, y*8, 127, y*8+7);	// reverse select line 
		
		Fkey_dispN(0,"Init");
		Fkey_dispN(1,"Trig");
		Fkey_dispN(2,"STD");

		Bdisp_PutDisp_DD();

		GetKey( &key );
		switch (key) {
			case KEY_CTRL_EXIT:
				return 0;	// no change
				break;
			case KEY_CTRL_EXE:
				cont=0;
				break;
		
			case KEY_CTRL_UP:
				select-=1;
				if ( select < 0 ) {select=9; scrl=4;}
				if ( select < scrl ) scrl-=1;
				if ( scrl < 0 ) scrl=0;
				break;
			case KEY_CTRL_DOWN:
				select+=1;
				if ( select > 9 ) {select=0; scrl=0;}
				if ((select - scrl) > 5 ) scrl+=1;
				if ( scrl > 4 ) scrl=4;
				break;
				
			case KEY_CTRL_F1:	// Initialize
				SetVeiwWindowInit();
				break;
			case KEY_CTRL_F2:	// trig Initialize
				SetVeiwWindowTrig();
				break;
			case KEY_CTRL_F3:	// STD Initialize
				SetVeiwWindowSTD();
				break;
			
			case KEY_CTRL_RIGHT:
				Bdisp_AreaReverseVRAM(0, y*8, 127, y*8+7);	// reverse select line 
				Fkey_Clear( 0 );
				Fkey_Clear( 1 );
				Fkey_Clear( 2 );
				y++;
				switch (select) {
					case 0: // Xmin
						Xmin      =InputNumD_full( 8, y, 14, Xmin);	// 
						break;
					case 1: // Xmax
						Xmax      =InputNumD_full( 8, y, 14, Xmax);	// 
						break;
					case 2: // Xscl
						Xscl      =InputNumD_full( 8, y, 14, Xscl);	// 
						break;
					case 3: // Xdot
						Xdot      =InputNumD_full( 8, y, 14, Xdot);	// 
						break;
					case 4: // Ymin
						Ymin      =InputNumD_full( 8, y, 14, Ymin);	// 
						break;
					case 5: // Ymax
						Ymax      =InputNumD_full( 8, y, 14, Ymax);	// 
						break;
					case 6: // Yscl
						Yscl      =InputNumD_full( 8, y, 14, Yscl);	// 
						break;
					case 7: // TThetamin
						TThetamin =InputNumD_full( 8, y, 14, TThetamin);	// 
						break;
					case 8: // TThetamax
						TThetamax =InputNumD_full( 8, y, 14, TThetamax);	// 
						break;
					case 9: // TThetaptch
						TThetaptch=InputNumD_full( 8, y, 14, TThetaptch);	// 
						break;
					default:
						break;
				}
				break;
			default:
				break;
		}
		key=MathKey( key );
		if ( key ) {
				Bdisp_AreaReverseVRAM(0, y*8, 127, y*8+7);	// reverse select line 
				Fkey_Clear( 0 );
				Fkey_Clear( 1 );
				Fkey_Clear( 2 );
				y++;
				switch (select) {
					case 0: // Xmin
						Xmin      =InputNumD_Char( 8, y, 14, Xmin, key);	// 
						break;
					case 1: // Xmax
						Xmax      =InputNumD_Char( 8, y, 14, Xmax, key);	// 
						break;
					case 2: // Xscl
						Xscl      =InputNumD_Char( 8, y, 14, Xscl, key);	// 
						break;
					case 3: // Xdot
						Xdot      =InputNumD_Char( 8, y, 14, Xdot, key);	// 
						break;
					case 4: // Ymin
						Ymin      =InputNumD_Char( 8, y, 14, Ymin, key);	// 
						break;
					case 5: // Ymax
						Ymax      =InputNumD_Char( 8, y, 14, Ymax, key);	// 
						break;
					case 6: // Yscl
						Yscl      =InputNumD_Char( 8, y, 14, Yscl, key);	// 
						break;
					case 7: // TThetamin
						TThetamin =InputNumD_Char( 8, y, 14, TThetamin, key);	// 
						break;
					case 8: // TThetamax
						TThetamax =InputNumD_Char( 8, y, 14, TThetamax, key);	// 
						break;
					case 9: // TThetaptch
						TThetaptch=InputNumD_Char( 8, y, 14, TThetaptch, key);	// 
						break;
					default:
						break;
				}
			}
	}

	if( ( xmin      != Xmin      ) ||
	    ( xmax      != Xmax      ) ||
	    ( xscl      != Xscl      ) ||
	    ( xdot      != Xdot      ) ||
	    ( ymin      != Ymin      ) ||
	    ( ymax      != Ymax      ) ||
	    ( ydot      != Ydot      ) ||
	    ( yscl      != Yscl      ) ||
	    ( tThetamin != TThetamin ) ||
	    ( tThetamax != TThetamax ) ||
	    ( tThetaptch!= TThetaptch) )  {
			ViewWindow( Xmin, Xmax, Xscl, Ymin, Ymax, Yscl);
			return -1; // change value
	}
	return 0;	// no change
}

//-----------------------------------------------------------------------------
void SetFactor(){
	unsigned char buffer[22];
	unsigned int key;
	int	cont=1;
	int select=0;
	int y;

	PopUpWin(3);

	while (cont) {
		locate(3,3); Print((unsigned char *)"Factor");
		locate(3,4); Print((unsigned char *) "Xfact:           ");
		sprintG(buffer,Xfct,  10,LEFT_ALIGN); locate( 9, 4); Print(buffer);
		locate(3,5); Print((unsigned char *) "Yfact:           ");
		sprintG(buffer,Yfct,  10,LEFT_ALIGN); locate( 9, 5); Print(buffer);

		y = select + 3 ;
		Bdisp_AreaReverseVRAM(12, y*8, 113, y*8+7);	// reverse select line 
		Bdisp_PutDisp_DD();

		GetKey( &key );
		switch (key) {
			case KEY_CTRL_EXIT:
			case KEY_CTRL_EXE:
				cont=0;
				break;
		
			case KEY_CTRL_UP:
				select-=1;
				if ( select < 0 ) select=1;
				break;
			case KEY_CTRL_DOWN:
				select+=1;
				if ( select > 1 ) select=0;
				break;

			case KEY_CTRL_RIGHT:
				Bdisp_AreaReverseVRAM(12, y*8, 113, y*8+7);	// reverse select line 
				Bdisp_PutDisp_DD();
				y++;
				switch (select) {
					case 0: // Xfct
						Xfct  =InputNumD_full( 9, y, 10, Xfct);	// 
						break;
					case 1: // Yfct
						Xmax  =InputNumD_full( 9, y, 10, Yfct);	// 
						break;
					default:
						break;
				}
				break;
			default:
				break;
		}
		key=MathKey( key );
		if ( key ) {
				Bdisp_AreaReverseVRAM(12, y*8, 113, y*8+7);	// reverse select line 
				Bdisp_PutDisp_DD();
				y++;
				switch (select) {
					case 0: // Xfct
						Xfct  =InputNumD_Char( 9, y, 10, Xfct, key);	// 
						break;
					case 1: // Yfct
						Yfct  =InputNumD_Char( 9, y, 10, Yfct, key);	// 
						break;
					default:
						break;
				}
		}

	}
}

//-----------------------------------------------------------------------------

void SetVar(int select){		// ----------- Set Variable
	unsigned char buffer[22];
	unsigned int key;
	int	cont=1;
	int scrl=0;
	int y;
	int selectreplay=-1;
	
	scrl=select;
	if ( scrl > 19 ) scrl=19;
	
	while (cont) {
		Bdisp_AllClr_VRAM();

		if ( scrl <=0 ) {
			locate( 1, 1-scrl); Print((unsigned char*)"A=");
			sprintG(buffer, regA, 19,LEFT_ALIGN); locate( 3, 1-scrl); Print(buffer);
		}
		if ( scrl <=1 ) {
			locate( 1, 2-scrl); Print((unsigned char*)"B=");
			sprintG(buffer, regB, 19,LEFT_ALIGN); locate( 3, 2-scrl); Print(buffer);
		}
		if ( scrl <=2 ) {
			locate( 1, 3-scrl); Print((unsigned char*)"C=");
			sprintG(buffer, regC, 19,LEFT_ALIGN); locate( 3, 3-scrl); Print(buffer);
		}
		if ( scrl <=3 ) {
			locate( 1, 4-scrl); Print((unsigned char*)"D=");
			sprintG(buffer, regD, 19,LEFT_ALIGN); locate( 3, 4-scrl); Print(buffer);
		}
		if ( scrl <=4 ) {
			locate( 1, 5-scrl); Print((unsigned char*)"E=");
			sprintG(buffer, regE, 19,LEFT_ALIGN); locate( 3, 5-scrl); Print(buffer);
		}
		if ( scrl <=5 ) {
			locate( 1, 6-scrl); Print((unsigned char*)"F=");
			sprintG(buffer, regF, 19,LEFT_ALIGN); locate( 3, 6-scrl); Print(buffer);
		}
		if ( scrl <=6 ) {
			locate( 1, 7-scrl); Print((unsigned char*)"G=");
			sprintG(buffer, regG, 19,LEFT_ALIGN); locate( 3, 7-scrl); Print(buffer);
		}
		if ( ( scrl >=1 ) && ( 8-scrl > 0 ) ){
			locate( 1, 8-scrl); Print((unsigned char*)"H=");
			sprintG(buffer, regH, 19,LEFT_ALIGN); locate( 3, 8-scrl); Print(buffer);
		}
		if ( ( scrl >=2 ) && ( 9-scrl > 0 ) ) {
			locate( 1, 9-scrl); Print((unsigned char*)"I=");
			sprintG(buffer, regI, 19,LEFT_ALIGN); locate( 3, 9-scrl); Print(buffer);
		}
		if ( ( scrl >=3 ) && ( 10-scrl > 0 ) ) {
			locate( 1, 10-scrl); Print((unsigned char*)"J=");
			sprintG(buffer, regJ, 19,LEFT_ALIGN); locate( 3, 10-scrl); Print(buffer);
		}
		if ( ( scrl >=4 ) && ( 11-scrl > 0 ) ) {
			locate( 1, 11-scrl); Print((unsigned char*)"K=");
			sprintG(buffer, regK, 19,LEFT_ALIGN); locate( 3, 11-scrl); Print(buffer);
		}
		if ( ( scrl >=5 ) && ( 12-scrl > 0 ) ) {
			locate( 1, 12-scrl); Print((unsigned char*)"L=");
			sprintG(buffer, regL, 19,LEFT_ALIGN); locate( 3, 12-scrl); Print(buffer);
		}
		if ( ( scrl >=6 )  && ( 13-scrl > 0 ) ) {
			locate( 1, 13-scrl); Print((unsigned char*)"M=");
			sprintG(buffer, regM, 19,LEFT_ALIGN); locate( 3, 13-scrl); Print(buffer);
		}
		if ( ( scrl >=7 ) && ( 14-scrl > 0 ) ) {
			locate( 1, 14-scrl); Print((unsigned char*)"N=");
			sprintG(buffer, regN, 19,LEFT_ALIGN); locate( 3, 14-scrl); Print(buffer);
		}
		if ( ( scrl >=8 ) && ( 15-scrl > 0 ) ) {
			locate( 1, 15-scrl); Print((unsigned char*)"O=");
			sprintG(buffer, regO, 19,LEFT_ALIGN); locate( 3, 15-scrl); Print(buffer);
		}
		if ( ( scrl >=9 )  && ( 16-scrl > 0 ) ) {
			locate( 1, 16-scrl); Print((unsigned char*)"P=");
			sprintG(buffer, regP, 19,LEFT_ALIGN); locate( 3, 16-scrl); Print(buffer);
		}
		if ( ( scrl >=10 ) && ( 17-scrl > 0 ) ) {
			locate( 1, 17-scrl); Print((unsigned char*)"Q=");
			sprintG(buffer, regQ, 19,LEFT_ALIGN); locate( 3, 17-scrl); Print(buffer);
		}
		if ( ( scrl >=11 ) && ( 18-scrl > 0 ) ) {
			locate( 1, 18-scrl); Print((unsigned char*)"R=");
			sprintG(buffer, regR, 19,LEFT_ALIGN); locate( 3, 18-scrl); Print(buffer);
		}
		if ( ( scrl >=12 ) && ( 19-scrl > 0 ) ) {
			locate( 1, 19-scrl); Print((unsigned char*)"S=");
			sprintG(buffer, regS, 19,LEFT_ALIGN); locate( 3, 19-scrl); Print(buffer);
		}
		if ( ( scrl >=13 ) && ( 20-scrl > 0 ) ) {
			locate( 1, 20-scrl); Print((unsigned char*)"T=");
			sprintG(buffer, regT, 19,LEFT_ALIGN); locate( 3, 20-scrl); Print(buffer);
		}
		if ( ( scrl >=14 ) && ( 21-scrl > 0 ) ) {
			locate( 1, 21-scrl); Print((unsigned char*)"U=");
			sprintG(buffer, regU, 19,LEFT_ALIGN); locate( 3, 21-scrl); Print(buffer);
		}
		if ( ( scrl >=15 ) && ( 22-scrl > 0 ) ) {
			locate( 1, 22-scrl); Print((unsigned char*)"V=");
			sprintG(buffer, regV, 19,LEFT_ALIGN); locate( 3, 22-scrl); Print(buffer);
		}
		if ( ( scrl >=16 ) && ( 23-scrl > 0 ) ) {
			locate( 1, 23-scrl); Print((unsigned char*)"W=");
			sprintG(buffer, regW, 19,LEFT_ALIGN); locate( 3, 23-scrl); Print(buffer);
		}
		if ( ( scrl >=17 ) && ( 24-scrl > 0 ) ) {
			locate( 1, 24-scrl); Print((unsigned char*)"X=");
			sprintG(buffer, regX, 19,LEFT_ALIGN); locate( 3, 24-scrl); Print(buffer);
		}
		if ( ( scrl >=18 ) && ( 25-scrl > 0 ) ) {
			locate( 1, 25-scrl); Print((unsigned char*)"Y=");
			sprintG(buffer, regY, 19,LEFT_ALIGN); locate( 3, 25-scrl); Print(buffer);
		}
		if ( ( scrl >=19 ) && ( 26-scrl > 0 ) ) {
			locate( 1, 26-scrl); Print((unsigned char*)"Z=");
			sprintG(buffer, regZ, 19,LEFT_ALIGN); locate( 3, 26-scrl); Print(buffer);
		}

		y = select-scrl;
		Bdisp_AreaReverseVRAM(0, y*8, 127, y*8+7);	// reverse select line 
		
		Bdisp_PutDisp_DD();

		GetKey( &key );
		switch (key) {
			case KEY_CTRL_EXIT:
			case KEY_CTRL_EXE:
				cont=0;
				break;
		
			case KEY_CTRL_UP:
				select-=1;
				if ( select < 0 ) {select=25; scrl=19;}
				if ( select < scrl ) scrl-=1;
				if ( scrl < 0 ) scrl=0;
				selectreplay = -1; // replay cancel
				break;
			case KEY_CTRL_DOWN:
				select+=1;
				if ( select > 25 ) {select=0; scrl=0;}
				if ((select - scrl) > 6 ) scrl+=1;
				if ( scrl > 19 ) scrl=19;
				selectreplay = -1; // replay cancel
				break;
				
			case KEY_CTRL_RIGHT:
				Bdisp_AreaReverseVRAM(0, y*8, 127, y*8+7);	// reverse select line 
				Fkey_Clear( 0 );
				Fkey_Clear( 1 );
				Fkey_Clear( 2 );
				y++;
				selectreplay = select; 
				if ( ( 0 <= select ) && ( select <=25 ) ) {	// regA to regZ
						REG[select]= InputNumD_full( 3, y, 19, REG[select]);
				} else {
						selectreplay = -1; // replay cancel 
				}
				break;
				
			case KEY_CTRL_LEFT:
				if (selectreplay<0) break;
				Bdisp_AreaReverseVRAM(0, y*8, 127, y*8+7);	// reverse select line 
				Fkey_Clear( 0 );
				Fkey_Clear( 1 );
				Fkey_Clear( 2 );
				y++;
				selectreplay = select; 
				if ( ( 0 <= select ) && ( select <=25 ) ) {	// regA to regZ
						REG[select]= InputNumD_replay( 3, y, 19, REG[select]);
				} else {
						selectreplay = -1; // replay cancel 
				}
				break;
			default:
				break;
		}
		key=MathKey( key );
		if ( key ) {
				Bdisp_AreaReverseVRAM(0, y*8, 127, y*8+7);	// reverse select line 
				Fkey_Clear( 0 );
				Fkey_Clear( 1 );
				Fkey_Clear( 2 );
				y++;
				selectreplay = select; 
				if ( ( 0 <= select ) && ( select <=25 ) ) {	// regA to regZ
						REG[select]= InputNumD_Char( 3, y, 19, REG[select], key);
				} else {
						selectreplay = -1; // replay cancel 
				}
		}
	}
}

//-----------------------------------------------------------------------------
int SelectNum2( char*msg, int n ) {		// 
	unsigned int key;
	PopUpWin(3);
	locate( 3,3); Print((unsigned char *)"Select Number");
	locate( 6,5); Print((unsigned char *)msg);
	locate( 9,5); Print((unsigned char *)"[0~15]:");
	while (1) {
		n=InputNumD(17, 5, 2, n, ' ', REV_OFF, FLOAT_OFF, EXP_OFF);		// 0123456789
 		if ( (0<=n)&&(n<=15) ) break;
 		n=0;
 	}

	return n ; // ok
}

void SetupG(){		// ----------- Setup 
    char *onoff[]  ={"off","on"};
    char *draw[]   ={"Connect","Plot"};
    char *style[]  ={"Normal","Thick","Broken","Dot"};
    char *degrad[]  ={"Deg","Rad","Grad"};
    char *display[]  ={"Nrm","Fix","Sci"};
	unsigned char buffer[22];
	unsigned int key;
	int	cont=1;
	int select=0;
	int scrl=0;
	int y;

	while (cont) {
		Bdisp_AllClr_VRAM();
		if ( scrl <=0 ) {
			locate( 1, 1-scrl); Print((unsigned char*)"Draw Type   :");
			locate(14, 1-scrl); Print((unsigned char*)draw[(int)DrawType]);
		}
		if ( scrl <=1 ) {
			locate( 1, 2-scrl); Print((unsigned char*)"Coord:      :");
			locate(14, 2-scrl); Print((unsigned char*)onoff[Coord]);
		}
		if ( scrl <=2 ) {
			locate( 1, 3-scrl); Print((unsigned char*)"Grid        :");
			locate(14, 3-scrl); Print((unsigned char*)onoff[Grid]);
		}
		if ( scrl <=3 ) {
			locate( 1, 4-scrl); Print((unsigned char*)"Axes        :");
			locate(14, 4-scrl); Print((unsigned char*)onoff[Axes]);
		}
		if ( scrl <=4 ) {
			locate( 1, 5-scrl); Print((unsigned char*)"Label       :");
			locate(14, 5-scrl); Print((unsigned char*)onoff[Label]);
		}
		if ( scrl <=5 ) {
			locate( 1, 6-scrl); Print((unsigned char*)"Derivative  :");
			locate(14, 6-scrl); Print((unsigned char*)onoff[Derivative]);
		}
		if ( scrl <=6 ) {
			locate( 1, 7-scrl); Print((unsigned char*)"Sketch Line :");
			locate(14, 7-scrl); Print((unsigned char*)style[S_L_Style]);
		}
		if ( ( scrl >=1 ) && ( 8-scrl > 0 ) ){
			locate( 1, 8-scrl); Print((unsigned char*)"Angle       :");
			locate(14, 8-scrl); Print((unsigned char*)degrad[Angle]);
		}
		if ( ( scrl >=2 ) && ( 9-scrl > 0 ) ){
			locate( 1, 9-scrl); Print((unsigned char*)"Display     :");
			locate(14, 9-scrl); Print((unsigned char*)display[CB_Round.MODE]);
			buffer[0]='\0';
			sprintf((char*)buffer,"%d",CB_Round.DIGIT);
			locate(17, 9-scrl); Print(buffer);
			locate(19, 9-scrl); 
			if (ENG) Print((unsigned char*)"/E");
		}
		if ( ( scrl >=3 ) && (10-scrl > 0 ) ){
			locate( 1,10-scrl); Print((unsigned char*)"TimeDsp     :");
			locate(14,10-scrl); Print((unsigned char*)onoff[TimeDsp]);
		}

		y = select-scrl;
		Bdisp_AreaReverseVRAM(0, y*8, 127, y*8+7);	// reverse select line 
		switch (select) {
			case 0: // Draw Type
				Fkey_dispN( 0, "Con");
				Fkey_dispN( 1, "Plot");
				Fkey_Clear( 2 );
				Fkey_Clear( 3 );
				break;
			case 1: // Coord
			case 2: // Grid	
			case 3: // Axes
			case 4: // Label
			case 5: // Derivative
			case 9: // TimeDsp
				Fkey_dispN( 0, " On ");
				Fkey_dispN( 1, " Off");
				Fkey_Clear( 2 );
				Fkey_Clear( 3 );
				break;
			case 6: // S_L_ Line	normal
				FkeyS_L_();
				break;
			case 7: // Angle
				Fkey_dispN( 0, "Deg ");
				Fkey_dispN( 1, "Rad ");
				Fkey_dispN( 2, "Grad");
				Fkey_Clear( 3 );
				break;
			case 8: // Display
				Fkey_dispR( 0, "Fix ");
				Fkey_dispR( 1, "Sci ");
				Fkey_dispR( 2, "Nrm ");
				Fkey_dispN( 3, "Eng ");
				break;
			default:
				break;
		}
		
		Bdisp_PutDisp_DD();

		GetKey( &key );
		switch (key) {
			case KEY_CTRL_EXIT:
			case KEY_CTRL_EXE:
				cont=0;
				break;
		
			case KEY_CTRL_UP:
				select-=1;
				if ( select < 0 ) {select=9; scrl=select-6;}
				if ( select < scrl ) scrl-=1;
				if ( scrl < 0 ) scrl=0;
				break;
			case KEY_CTRL_DOWN:
				select+=1;
				if ( select > 9 ) {select=0; scrl=0;}
				if ((select - scrl) > 6 ) scrl+=1;
				if ( scrl > 3 ) scrl=3;
				break;
				
			case KEY_CTRL_F1:
				Bdisp_AreaReverseVRAM(0, y*8, 127, y*8+7);	// reverse select line 
				switch (select) {
					case 0: // Draw Type connect
						DrawType = 0 ;
						break;
					case 1: // Coord	on
						Coord = 1 ;
						break;
					case 2: // Grid		on
						Grid = 1 ;
						break;
					case 3: // Axes		on
						Axes = 1 ;
						break;
					case 4: // Label	on
						Label = 1 ;
						break;
					case 5: // 	Derivative on
						Derivative = 1 ;
						break;
					case 6: // Sketch Line	normal
						S_L_Style = 0 ;
						break;
					case 7: // Angle
						Angle = 0 ; // Deg
						break;
					case 8: // Display
						CB_Round.DIGIT=SelectNum2("Fix",CB_Round.DIGIT);
						CB_Round.MODE =Fix;
						break;
					case 9: // TimeDsp
						TimeDsp = 1 ; // on
						break;
					default:
						break;
				}
				break;
			case KEY_CTRL_F2:
				Bdisp_AreaReverseVRAM(0, y*8, 127, y*8+7);	// reverse select line 
				switch (select) {
					case 0: // Draw Type Plot
						DrawType = 1 ;
						break;
					case 1: // Coord	off
						Coord = 0 ;
						break;
					case 2: // Grid		off
						Grid = 0 ;
						break;
					case 3: // Axes		off
						Axes = 0 ;
						break;
					case 4: // Label	off
						Label = 0 ;
						break;
					case 5: // 	Derivative off
						Derivative = 0 ;
						break;
					case 6: // Sketch Line	Thick
						S_L_Style = 1 ; 
						break;
					case 7: // Angle
						Angle = 1 ; // Rad
						break;
					case 8: // Display
						CB_Round.DIGIT=SelectNum2("Sci",CB_Round.DIGIT);
						CB_Round.MODE =Sci;
						break;
					case 9: // Angle
						TimeDsp = 0 ; // off
						break;
					default:
						break;
				}
				break;
			case KEY_CTRL_F3:
				Bdisp_AreaReverseVRAM(0, y*8, 127, y*8+7);	// reverse select line 
				switch (select) {
					case 6: // Sketch Line	Broken
						S_L_Style = 2 ; 
						break;
					case 7: // Angle
						Angle = 2 ; // Grad
						break;
					case 8: // Display
						CB_Round.DIGIT=SelectNum2("Nrm",CB_Round.DIGIT);
						CB_Round.MODE =Norm;
						break;
					default:
						break;
				}
				break;
			case KEY_CTRL_F4:
				Bdisp_AreaReverseVRAM(0, y*8, 127, y*8+7);	// reverse select line 
				switch (select) {
					case 6: // Sketch  Line	Dot
						S_L_Style = 3 ; 
						break;
					case 8: // Display
						ENG=1-ENG;
						break;
					default:
						break;
				}
				break;
			default:
				break;
		}
	}
}

