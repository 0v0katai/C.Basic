/*
===============================================================================

 Casio Basic RUNTIME library for fx-9860G series     v1.8x

 copyright(c)2015/2016/2017/2018 by sentaro21
 e-mail sentaro21@pm.matrix.jp

===============================================================================
*/
#include "CB.h"

//-----------------------------------------------------------------------------
void BdispSetPointVRAM2( int px, int py, int mode){
	unsigned char *VRAM=PictAry[0]+(px>>3)+(py<<4);
	int m=(128>>(px&0x7));
	switch ( mode ) {
		case 0:	// Clear
			*VRAM &= ~m;
			break;
		case 1:	// set
			*VRAM |= m ;
			break;
		case 2:	// xor
			*VRAM ^= m ;
			
			break;
	}
}
//-----------------------------------------------------------------------------
int BdispGetPointVRAM2( char px, char py){
	unsigned char *VRAM=PictAry[0]+(px>>3)+(py<<4);
	int m=(128>>(px&0x7));
	if ( ( (*VRAM) & m ) == 0 ) return 0; 
	return 1;
}

//------------------------------------------------------------------------------
/*
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
*/

//-----------------------------------------------------------------------------
int skip_count=0;

void Bdisp_PutDisp_DD_DrawBusy() {
	Bdisp_PutDisp_DD();
	HourGlass();
	if ( BreakCheck )if ( KeyScanDownAC() ) { KeyRecover(); BreakPtr=ExecPtr; }	// [AC] break?
//	DrawBusy();
}
int Check_skip_count( void ) {
									// 1:DD  0;none
	unsigned int t;
	if ( Refreshtime >= 0 ) {
		t=RTC_GetTicks();
		if ( abs(t-skip_count)>Refreshtime ) { skip_count=t;		// default 128/3=42  1/42s
			return 1;
		}
	}
	return 0;
}
void Bdisp_PutDisp_DD_DrawBusy_skip() {
	unsigned int t;
	if ( Check_skip_count() ) {
		Bdisp_PutDisp_DD_DrawBusy();
	}
}
//void Bdisp_PutDisp_DD_DrawBusy_through( char *SRC ) {
//	unsigned char c;
//	if ( SRC[ExecPtr++] == ':' ) return ;
//	ExecPtr--;
//	Bdisp_PutDisp_DD_DrawBusy();
//}
//void Bdisp_PutDisp_DD_DrawBusy_through( char *SRC ) {
//	char c=SRC[ExecPtr++];
//	if ( c == ':' ) return ;
//	if ( c == ';' ) { Bdisp_PutDisp_DD_DrawBusy_skip(); return ; }
//	ExecPtr--;
//	Bdisp_PutDisp_DD_DrawBusy();
//}
void Bdisp_PutDisp_DD_DrawBusy_skip_through_text( char *SRC ) {	// Locate text Clrtext ""...
	char c=SRC[ExecPtr++];
	if ( ( c == ':' ) && (SRC[ExecPtr]==0x0D) ) return ;
	if ( c == ';' ) { Bdisp_PutDisp_DD_DrawBusy_skip(); return ; }
	ExecPtr--;
	if ( RefreshCtrl == 2 ) 	// refresh control all
			Bdisp_PutDisp_DD_DrawBusy_skip();
	else
			Bdisp_PutDisp_DD_DrawBusy();
}
void Bdisp_PutDisp_DD_DrawBusy_skip_through( char *SRC ) {	// graphics command
	char c=SRC[ExecPtr++];
	if ( ( c == ':' ) && (SRC[ExecPtr]==0x0D) ) return ;
	if ( c == ';' ) { Bdisp_PutDisp_DD_DrawBusy_skip(); return ; }
	ExecPtr--;
	if ( RefreshCtrl == 0 ) 	// refresh control off
			Bdisp_PutDisp_DD_DrawBusy();
	else
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
void PXYtoVW(int px, int py, double *x, double *y){	// pixel(x,y) -> ViewWwindow(x,y)
//	if ( Xmax == Xmin )		*x = Xmin ;
	*x = (    px-1)*Xdot  + Xmin ;
//	if ( Xmax >  Xmin )		*x = (    (double)px-1)*Xdot  + Xmin ;
//	if ( Xmax <  Xmin )		*x = (126-(double)px+1)*-Xdot + Xmax ;
//	if ( Ymax == Ymin )		*y = Ymin ;
	*y = ( 62-py+1)*Ydot  + Ymin ;
//	if ( Ymax >  Ymin )		*y = ( 62-(double)py+1)*Ydot  + Ymin ;
//	if ( Ymax <  Ymin )		*y = (    (double)py-1)*-Ydot + Ymax ;
//	if ( fabs(*x)*1e10 < xdot ) *x=0;	// zero adjust
//	if ( fabs(*y)*1e10 < ydot ) *y=0;	// zero adjust
}
int VWtoPXY(double x, double y, int *px, int *py){	// ViewWwindow(x,y) -> pixel(x,y)
	if ( ( Xdot == 0 ) || ( Ydot == 0 ) || ( Xmax == Xmin ) || ( Ymax == Ymin ) ) { ErrorNo=RangeERR; ErrorPtr=ExecPtr; return -1 ; }	// Range error
	*px =   1 + ( (x-Xmin)/Xdot + 0.5 ) ;
//	if ( Xmax >  Xmin )		*px =       (x-Xmin)/Xdot  +1.5 ;
//	if ( Xmax <  Xmin )		*px = 126 - (x-Xmax)/-Xdot +1.49999999999999 ;
	*py =  63 - ( (y-Ymin)/Ydot - 0.49999999999999 ) ;
//	if ( Ymax >  Ymin )		*py =  62 - (y-Ymin)/Ydot  +1.49999999999999 ;
//	if ( Ymax <  Ymin )		*py =       (y-Ymax)/-Ydot +1.5 ;
	if ( (*px<MatBase) || (*px>127) || (*py<MatBase) || (*py> 63) ) { return -1; }	// 
	return 0;
}

//-----------------------------------------------------------------------------
//short prev_Grid_px;
//short prev_Grid_py;

void PlotGrid(double x, double y){
	int px,py;
//	if ( ( x==0 ) && ( y==0 ) ) return;
	VWtoPXY( x,y, &px, &py);
	if ( px<MatBase ) px=MatBase;
	if ( py<MatBase ) py=MatBase;
	if (px>127) px=127;
	if (py> 63) py= 63;
	Bdisp_SetPoint_VRAM( px, py, 1);
}
void PlotGrid2(double x, double y){
	int px,py;
//	if ( ( x==0 ) && ( y==0 ) ) return;
	VWtoPXY( x,y, &px, &py);
	if ( px<MatBase ) px=MatBase;
	if ( py<MatBase ) py=MatBase;
	if (px>127) px=127;
	if (py> 63) py= 63;
//	if ( ( abs(px-prev_Grid_px) >=5 ) || ( abs(py-prev_Grid_py) >=5 ) ) {
		Bdisp_SetPoint_VRAM( px, py, 1);
//	}
//	prev_Grid_px=px;
//	prev_Grid_py=py;
}

void GraphAxesGrid(){
	double x,y;

//	prev_Grid_px=1000;
//	prev_Grid_py=1000;
	if ( BG_Pict_No ) RclPict(BG_Pict_No, 0);	// no error check
	if ( Axes ) {
		Horizontal(0, S_L_Normal,1);
		Vertical(0, S_L_Normal,1);
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
						if ( Ymin <= 0 ) for ( y=0; y>=Ymin; y-=Yscl )	PlotGrid2(x,y);
						if ( Ymin >  0 ) for ( y=0; y<=Ymin; y+=Yscl )	PlotGrid2(x,y);
						if ( Ymax <= 0 ) for ( y=0; y>=Ymax; y-=Yscl )	PlotGrid2(x,y);
						if ( Ymax >  0 ) for ( y=0; y<=Ymax; y+=Yscl )	PlotGrid2(x,y);
					}
			}
			if ( Xmin >= 0 ) {
					for ( x=0; x<=Xmin; x+=Xscl) {
						if ( Ymin <= 0 ) for ( y=0; y>=Ymin; y-=Yscl )	PlotGrid2(x,y);
						if ( Ymin >  0 ) for ( y=0; y<=Ymin; y+=Yscl )	PlotGrid2(x,y);
						if ( Ymax <= 0 ) for ( y=0; y>=Ymax; y-=Yscl )	PlotGrid2(x,y);
						if ( Ymax >  0 ) for ( y=0; y<=Ymax; y+=Yscl )	PlotGrid2(x,y);
					}
			}
			if ( Xmax < 0 ) {
					for ( x=0; x>=Xmax; x-=Xscl) {
						if ( Ymin <= 0 ) for ( y=0; y>=Ymin; y-=Yscl )	PlotGrid2(x,y);
						if ( Ymin >  0 ) for ( y=0; y<=Ymin; y+=Yscl )	PlotGrid2(x,y);
						if ( Ymax <= 0 ) for ( y=0; y>=Ymax; y-=Yscl )	PlotGrid2(x,y);
						if ( Ymax >  0 ) for ( y=0; y<=Ymax; y+=Yscl )	PlotGrid2(x,y);
					}
			}
			if ( Xmax >= 0 ) {
					for ( x=0; x<=Xmax; x+=Xscl) {
						if ( Ymin <= 0 ) for ( y=0; y>=Ymin; y-=Yscl )	PlotGrid2(x,y);
						if ( Ymin >  0 ) for ( y=0; y<=Ymin; y+=Yscl )	PlotGrid2(x,y);
						if ( Ymax <= 0 ) for ( y=0; y>=Ymax; y-=Yscl )	PlotGrid2(x,y);
						if ( Ymax >  0 ) for ( y=0; y<=Ymax; y+=Yscl )	PlotGrid2(x,y);
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
	Ymin  =ymin;
	Ymax  =ymax;
	Yscl  =yscl;
	SetXdotYdot();

//	Bdisp_AllClr_VRAM();			// ------ Clear VRAM 
//	ML_clear_vram();
	
	GraphAxesGrid();

	Previous_X=1e308; Previous_Y=1e308; 	// ViewWindow Previous XY init
//	regX.real =fabs(Xmin+Xmax)/2; regY.real =fabs(Ymin+Ymax)/2;	// ViewWindow Current  XY
}


void ZoomIn(){
	double c,y,dx,dy,cx,cy;

	dx   = (Xmax+Xmin)/2-regX.real;
	Xmin = Xmin-dx;	// move center
	Xmax = Xmax-dx;
	
	cx   = (Xmax+Xmin)/2;	//
	dx   = (Xmax-Xmin)/Xfct/2;	// zoom in dx
	Xmin = cx-dx;
	Xmax = cx+dx;
	
	dy   = (Ymax+Ymin)/2-regY.real;	// move center
	Ymin = Ymin-dy;
	Ymax = Ymax-dy;
	
	cy   = (Ymax+Ymin)/2;	// 
	dy   = (Ymax-Ymin)/Yfct/2;	// zoom in dy
	Ymin = cy-dy;
	Ymax = cy+dy;
	SetXdotYdot();
	
	regX.real = (Xmax+Xmin)/2; // center
	regY.real = (Ymax+Ymin)/2; // center
	regintX=regX.real; regintY=regY.real;
}

void ZoomOut(){
	double dx,dy,cx,cy;
	
	dx   = (Xmax+Xmin)/2-regX.real;
	Xmin = Xmin-dx;	// move center
	Xmax = Xmax-dx;
	
	cx   = (Xmax+Xmin)/2;	//
	dx   = (Xmax-Xmin)*Xfct/2;	// zoom out dx
	Xmin = cx-dx;
	Xmax = cx+dx;
	
	dy   = (Ymax+Ymin)/2-regY.real;	// move center
	Ymin = Ymin-dy;
	Ymax = Ymax-dy;
	
	cy   = (Ymax+Ymin)/2;	// 
	dy   = (Ymax-Ymin)*Yfct/2;	// zoom out dy
	Ymin = cy-dy;
	Ymax = cy+dy;
	SetXdotYdot();
	
	regX.real = (Xmax+Xmin)/2; // center
	regY.real = (Ymax+Ymin)/2; // center
	regintX=regX.real; regintY=regY.real;
}

//------------------------------------------------------------------------------ PLOT
void PlotSub(double x, double y, int mode){
	int px,py;
	if ( VWtoPXY( x,y, &px, &py) == 0) {
		BdispSetPointVRAM2(px, py, mode);
		regX.real=x; regY.real=y;
		regintX=regX.real; regintY=regY.real;
	}
}

void PlotOn_VRAM(double x, double y){
	PlotSub( x, y, 1);		// VRAM
}
/*//void PlotOn_DD(double x, double y){
//	Plotsub( x, y, 1, 2);		// DD
//}
//void PlotOn_DDVRAM(double x, double y){
//	Plotsub( x, y, 1, 3);		// DDVRAM
//}
void PlotOff_VRAM(double x, double y){
	Plotsub( x, y, 0, 1);		// VRAM
}
//void PlotOff_DD(double x, double y){
//	Plotsub( x, y, 0, 2);		// DD
//}
//void PlotOff_DDVRAM(double x, double y){
//	Plotsub( x, y, 0, 3);		// DDVRAM
//}
void PlotChg_VRAM(double x,  double y){
	int px,py;
	if ( VWtoPXY( x,y, &px, &py) ) return;
	if ( Bdisp_GetPoint_VRAM(px, py) )
		 Bdisp_SetPoint_VRAM(px, py, 0);
	else Bdisp_SetPoint_VRAM(px, py, 1);
	regX.real=x; regY.real=y;
	regintX=regX.real; regintY=regY.real;
}
*/
//void PlotChg_DDVRAM(double x, double y){
//	PlotChg_VRAM(x, y);
//	Bdisp_PutDisp_DD();
//}
/*
void PxlOn_VRAM(int py, int px){
	Bdisp_SetPoint_VRAM(px, py, 1);
	PXYtoVW(px, py, &regX.real, &regY.real);
}
//void PxlOn_DD(int py, int px){
//	Bdisp_SetPoint_DD(px, py, 1);
//	PXYtoVW(px, py, &regX.real, &regY.real);
//}
//void PxlOn_DDVRAM(int py, int px){
//	Bdisp_SetPoint_DDVRAM(px, py, 1);
//	PXYtoVW(px, py, &regX.real, &regY.real);
//}
void PxlOff_VRAM(int py, int px){
	Bdisp_SetPoint_VRAM(px, py, 0);
	PXYtoVW(px, py, &regX.real, &regY.real);
}
//void PxlOff_DD(int py, int px){
//	Bdisp_SetPoint_DD(px, py, 0);
//	PXYtoVW(px, py, &regX.real, &regY.real);
//}
//void PxlOff_DDVRAM(int py, int px){
//	Bdisp_SetPoint_DDVRAM(px, py, 0);
//	PXYtoVW(px, py, &regX.real, &regY.real);
//}
void PxlChg_VRAM(int py, int px){
//	if (Bdisp_GetPoint_VRAM(px, py)) 
//		Bdisp_SetPoint_VRAM(px, py, 0);
//	else
//		Bdisp_SetPoint_VRAM(px, py, 1);
	BdispSetPointVRAM2(px, py, 2);
	PXYtoVW(px, py, &regX.real, &regY.real);
}
*/
int PxlTest(int py, int px) {
	return	Bdisp_GetPoint_VRAM(px, py);
}
//------------------------------------------------------------------------------ LINE
void LinesubSetPoint(int px, int py, int mode) {
	if ( ( px <  MatBase ) || ( px > 127 ) || ( py < MatBase ) || ( py >  63 ) ) return;
//	if ( mode == 2 ) {
//		if (Bdisp_GetPoint_VRAM(px, py)) 
//			Bdisp_SetPoint_VRAM(px, py, 0);
//		else
//			Bdisp_SetPoint_VRAM(px, py, 1);
//	} else
	BdispSetPointVRAM2(px, py, mode);
}

int prev_px0=-1,prev_py0=-1;	// Prev Thick dot 
int prev_px1=-1,prev_py1=-1;
int prev_px2=-1,prev_py2=-1;
int prev_px3=-1,prev_py3=-1;

void LinesubSetPointThick(int px, int py, int mode) {
	if ( mode == 2 ) {	// xor 
		if ( ((prev_px0!=px  )||(prev_py0!=py  ))&&((prev_px1!=px  )||(prev_py1!=py  ))&&((prev_px2!=px  )||(prev_py2!=py  ))&&((prev_px3!=px  )||(prev_py3!=py  )) ) LinesubSetPoint(px  , py  , mode);
		if ( ((prev_px0!=px  )||(prev_py0!=py-1))&&((prev_px1!=px  )||(prev_py1!=py-1))&&((prev_px2!=px  )||(prev_py2!=py-1))&&((prev_px3!=px  )||(prev_py3!=py-1)) ) LinesubSetPoint(px  , py-1, mode);
		if ( ((prev_px0!=px-1)||(prev_py0!=py  ))&&((prev_px1!=px-1)||(prev_py1!=py  ))&&((prev_px2!=px-1)||(prev_py2!=py  ))&&((prev_px3!=px-1)||(prev_py3!=py  )) ) LinesubSetPoint(px-1, py  , mode);
		if ( ((prev_px0!=px-1)||(prev_py0!=py-1))&&((prev_px1!=px-1)||(prev_py1!=py-1))&&((prev_px2!=px-1)||(prev_py2!=py-1))&&((prev_px3!=px-1)||(prev_py3!=py-1)) ) LinesubSetPoint(px-1, py-1, mode);
		prev_px0=px  ; prev_py0=py;
		prev_px1=px  ; prev_py1=py-1;
		prev_px2=px-1; prev_py2=py;
		prev_px3=px-1; prev_py3=py-1;
	} else {
		LinesubSetPoint(px  , py  , mode);
		LinesubSetPoint(px  , py-1, mode);
		LinesubSetPoint(px-1, py  , mode);
		LinesubSetPoint(px-1, py-1, mode);
	}
}

void Linesub(int px1, int py1, int px2, int py2, int style, int mode) {
	int i, j;
	int x, y;
	int dx, dy; // delta x,y
	int wx, wy; // width x,y
	int Styleflag=1;
	int tmp;

	if ( ( px1 <  -500 ) || ( px1 > 500 ) || ( py1 < -500 ) || ( py1 >  500 ) ) return;
	if ( ( px2 <  -500 ) || ( px2 > 500 ) || ( py2 < -500 ) || ( py2 >  500 ) ) return;

	prev_px0=-1;	// Prev Thick dot clear
	prev_px1=-1;
	prev_px2=-1;
	prev_px3=-1;

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
						LinesubSetPoint(x, y, mode);
						wy--; y+=dy;
					}
			} else
			if (wy==0) { // horizontal line
					x=px1; y=py1;
					while( wx>=0 ) {
						LinesubSetPoint(x, y, mode);
						wx--; x+=dx;
					}
			} else
			if (wx>=wy) {
					x=px1; y=py1; j=wx/2; i=wx;
					while( wx>=0 ) {
						LinesubSetPoint(x, y, mode);
						wx--; x+=dx;
						j-=wy; if (j<0) { j+=i; y+=dy; }
					}
			} else {
					x=px1; y=py1; j=wy/2; i=wy;
					while( wy>=0 ) {
						LinesubSetPoint(x, y, mode);
						wy--; y+=dy;
						j-=wx; if (j<0) { j+=i; x+=dx; }
					}
			}
			break;
		case S_L_Dot:		// ---------- Dot
			if (wx==0) {	// vertical line
					x=px1; y=py1;
					while( wy>=0 ) {
						if (Styleflag)
						LinesubSetPoint(x, y, mode);
						wy--; y+=dy;
						Styleflag = 1-Styleflag;
					}
			} else
			if (wy==0) { // horizontal line
					x=px1; y=py1;
					while( wx>=0 ) {
						if (Styleflag)
						LinesubSetPoint(x, y, mode);
						wx--; x+=dx;
						Styleflag = 1-Styleflag;
					}
			} else
			if (wx>wy) {
					x=px1; y=py1; j=wx/2; i=wx;
					while( wx>=0 ) {
						if (Styleflag)
						LinesubSetPoint(x, y, mode);
						wx--; x+=dx;
						j-=wy; if (j<0) { j+=i; y+=dy; }
						Styleflag = 1-Styleflag;
					}
			} else {
					x=px1; y=py1; j=wy/2; i=wy;
					while( wy>=0 ) {
						if (Styleflag)
						LinesubSetPoint(x, y, mode);
						wy--; y+=dy;
						j-=wx; if (j<0) { j+=i; x+=dx; }
						Styleflag = 1-Styleflag;
					}
			}
			break;
		case S_L_Thick:	// ---------- Thick
			if (wx==0) {	// vertical line
					x=px1; y=py1;
					while( wy>=0 ) {
						LinesubSetPointThick(x, y, mode);;
						wy--; y+=dy;
					}
			} else
			if (wy==0) { // horizontal line
					x=px1; y=py1;
					while( wx>=0 ) {
						LinesubSetPointThick(x, y, mode);;
						wx--; x+=dx;
					}
			} else
			if (wx>=wy) {
					x=px1; y=py1; j=wx/2; i=wx;
					while( wx>=0 ) {
						LinesubSetPointThick(x, y, mode);;
						wx--; x+=dx;
						j-=wy; if (j<0) { j+=i; y+=dy; }
					}
			} else {
					x=px1; y=py1; j=wy/2; i=wy;
					while( wy>=0 ) {
						LinesubSetPointThick(x, y, mode);;
						wy--; y+=dy;
						j-=wx; if (j<0) { j+=i; x+=dx; }
					}
			}
			break;
		case S_L_Broken:	// ---------- Broken
			if (wx==0) {	// vertical line
					x=px1; y=py1;
					while( wy>=0 ) {
						if (Styleflag==1) LinesubSetPointThick(x, y, mode);
						wy--; y+=dy;
						Styleflag++; if (Styleflag>2) Styleflag=0;
					}
			} else
			if (wy==0) { // horizontal line
					x=px1; y=py1;
					while( wx>=0 ) {
						if (Styleflag==1) LinesubSetPointThick(x, y, mode);
						wx--; x+=dx;
						Styleflag++; if (Styleflag>2) Styleflag=0;
					}
			} else
			if (wx>=wy) {
					x=px1; y=py1; j=wx/2; i=wx;
					while( wx>=0 ) {
						if (Styleflag==1) LinesubSetPointThick(x, y, mode);
						wx--; x+=dx;
						j-=wy; if (j<0) { j+=i; y+=dy; }
						Styleflag++; if (Styleflag>2) Styleflag=0;
					}
			} else {
					x=px1; y=py1; j=wy/2; i=wy;
					while( wy>=0 ) {
						if (Styleflag==1) LinesubSetPointThick(x, y, mode);
						wy--; y+=dy;
						j-=wx; if (j<0) { j+=i; x+=dx; }
						Styleflag++; if (Styleflag>2) Styleflag=0;
					}
			}
			break;
		default:
			break;
		}
	S_L_Style=tmp;
}


void Line(int style, int mode, int errorcheck ) {
	int px1,px2,py1,py2;
	int i,j;
	if ( ( Previous_X > 1e8 ) || ( Previous_X > 1e8 ) ) { 
		 Previous_X = Plot_X;
		 Previous_Y = Plot_Y;
		return;
	}
	i = VWtoPXY( Previous_X, Previous_Y, &px1, &py1) ;
	j = VWtoPXY(     Plot_X,     Plot_Y, &px2, &py2) ;
	Previous_X = Plot_X;
	Previous_Y = Plot_Y;
	Previous_X2= Plot_X;
	Previous_Y2= Plot_Y;
	Previous_PX = px2;
	Previous_PY = py2;
	if ( errorcheck ) {
		if ( i || j ) return ;
//	} else {
//		if ( i && j ) return ;
	}
	Linesub( px1, py1, px2, py2, style, mode);
}

void F_Line(double x1, double y1, double x2, double y2, int style, int mode) {
	int px1,px2,py1,py2;
	int i,j;
	i = VWtoPXY( x1, y1, &px1, &py1) ;
	j = VWtoPXY( x2, y2, &px2, &py2) ;
	if ( i || j ) return ;
	Linesub( px2, py2, px1, py1 ,style, mode);
}

void Vertical(double x, int style, int mode) {
	int px,py;
	VWtoPXY( x, 0, &px, &py);
	if ( px<  0 ) return;
	if ( px>127 ) return;
	Linesub( px, MatBase, px, 63, style, mode);
}
void Horizontal(double y, int style, int mode) {
	int px,py;
	VWtoPXY( 0, y, &px, &py);
	if ( py<  0 ) return;
	if ( py> 63 ) return;
	Linesub( MatBase, py, 127, py, style, mode);
}

void Circle(double x, double y, double r, int style, int drawflag, int mode ) {
	double	angle, k, x0,y0,x1,y1;
	int px,py;
	int	i,n;
	prev_px0=-1;	// Prev Thick dot clear
	prev_px1=-1;
	prev_px2=-1;
	prev_px3=-1;
	if (style==S_L_Normal) { k=8; if ( ( r/Xdot )  > 20 ) k=6; }
	if (style==S_L_Dot )   { k=4; if ( ( r/Xdot )  > 6  ) k=3; }
	if (style==S_L_Thick)  k=4;
	if (style==S_L_Broken) k=2;
	n=fabs(floor(r*k/Xdot));
	Plot_X = r+x;
	Plot_Y = 0+y;
	for(i=1;i<=n;i++){
		angle=const_PI*2*i/n;
		Plot_X=cos(angle)*r+x;
		Plot_Y=sin(angle)*r+y;
		if ( ( VWtoPXY( Plot_X, Plot_Y, &px, &py) ==0 ) && ( (prev_px0!=px)||(prev_py0!=py) ) ) {
 			switch ( style ) {
				case S_L_Normal:
				case S_L_Dot:
					LinesubSetPoint(px, py, mode);
					break;
				case S_L_Thick:
				case S_L_Broken:
					LinesubSetPointThick(px, py, mode);
					break;
				}
			prev_px0=px  ; prev_py0=py;
		}
		if ( drawflag )	Bdisp_PutDisp_DD_DrawBusy_skip();
	}
	regX.real=Plot_X;
	regY.real=Plot_Y;
	regintX=regX.real; regintY=regY.real;
}

//----------------------------------------------------------------------------------------------
//int ObjectAlignG1( unsigned int n ){ return n; }	// align +4byte
//int ObjectAlignG2( unsigned int n ){ return n; }	// align +4byte
//int ObjectAlignG3( unsigned int n ){ return n; }	// align +4byte
//int ObjectAlignG4( unsigned int n ){ return n; }	// align +4byte
//int ObjectAlignG5( unsigned int n ){ return n; }	// align +4byte
//int ObjectAlignG6( unsigned int n ){ return n; }	// align +4byte
//int ObjectAlignG7( unsigned int n ){ return n; }	// align +4byte
//int ObjectAlignG8( unsigned int n ){ return n; }	// align +4byte
//int ObjectAlignG9( unsigned int n ){ return n; }	// align +4byte

