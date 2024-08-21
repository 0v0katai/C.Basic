

#include "prizm.h"
#include "CBP.h"

//-----------------------------------------------------------------------------
void BdispSetPointVRAM( int px, int py, int mode){	// {0,0}-{191,383}
	unsigned short *VRAM=(unsigned short *)PictAry[0];
	unsigned short *BACK=(unsigned short *)BG_Buf;
	unsigned short color,backcolor=CB_BackColorIndex;
	int offset;
	if ( CB_ColorIndex >= 0 ) color=CB_ColorIndex; else color=CB_ColorIndexPlot;
	py+=24;
	offset = px  +(py)*384;
	switch ( mode ) {
		case 0:	// Clear
		  set0:
		  if ( BACK == NULL ) VRAM[offset] = backcolor;
		  else                VRAM[offset] = BACK[offset];
			break;
		case 1:	// set
		  set1:
			VRAM[offset] = color;
			break;
		case 2:	// xor
			if ( VRAM[offset] == backcolor    ) goto set1;
			if ( BACK != NULL ) if ( VRAM[offset] == BACK[offset] ) goto set1;
			goto set0;
	}
}
void BdispSetPointVRAMx9( int px, int py, int mode){	// 3x3 dot
	unsigned short *VRAM=(unsigned short *)PictAry[0];
	unsigned short *BACK=(unsigned short *)BG_Buf;
	int offset;
	int x,y;
	unsigned short color,backcolor=CB_BackColorIndex;
	if ( CB_ColorIndex >= 0 ) color=CB_ColorIndex; else color=CB_ColorIndexPlot;
	py+=24;
	offset = 384*py+px;
	switch ( mode ) {
		case 0:	// Clear
		  set0:
			if ( BACK == NULL ) {
				VRAM += offset;
				*(VRAM++) = backcolor;
				*(VRAM++) = backcolor;
				*(VRAM++) = backcolor;
				VRAM += 381;
				*(VRAM++) = backcolor;
				*(VRAM++) = backcolor;
				*(VRAM++) = backcolor;
				VRAM += 381;
				*(VRAM++) = backcolor;
				*(VRAM++) = backcolor;
				*(VRAM++) = backcolor;
			} else {
				VRAM += offset;		BACK += offset;
				*(VRAM++) = *(BACK++);
				*(VRAM++) = *(BACK++);
				*(VRAM++) = *(BACK++);
				VRAM += 381;		BACK += 381;
				*(VRAM++) = *(BACK++);
				*(VRAM++) = *(BACK++);
				*(VRAM++) = *(BACK++);
				VRAM += 381;		BACK += 381;
				*(VRAM++) = *(BACK++);
				*(VRAM++) = *(BACK++);
				*(VRAM++) = *(BACK++);
			}
			break;
		case 1:	// set
		  set1:
			VRAM += 384*py+px;
			*(VRAM++) = color;
			*(VRAM++) = color;
			*(VRAM++) = color;
			VRAM += 381;
			*(VRAM++) = color;
			*(VRAM++) = color;
			*(VRAM++) = color;
			VRAM += 381;
			*(VRAM++) = color;
			*(VRAM++) = color;
			*(VRAM++) = color;
			break;
		case 2:	// xor
			if ( VRAM[offset] == backcolor    ) goto set1;
			if ( BACK != NULL ) if ( VRAM[offset] == BACK[offset] ) goto set1;
			goto set0;
	}
}
void BdispSetPointVRAMx4( int px, int py, int mode){	// 2x2 dot
	unsigned short *VRAM=(unsigned short *)PictAry[0];
	unsigned short *BACK=(unsigned short *)BG_Buf;
	int offset;
	int x,y;
	unsigned short color,backcolor=CB_BackColorIndex;
	if ( CB_ColorIndex >= 0 ) color=CB_ColorIndex; else color=CB_ColorIndexPlot;
	py+=24;
	offset = 384*py+px;
	switch ( mode ) {
		case 0:	// Clear
		  set0:
			if ( BACK == NULL ) {
				VRAM += offset;
				*(VRAM++) = backcolor;
				*(VRAM++) = backcolor;
				VRAM += 382;
				*(VRAM++) = backcolor;
				*(VRAM++) = backcolor;
			} else {
				VRAM += offset;		BACK += offset;
				*(VRAM++) = *(BACK++);
				*(VRAM++) = *(BACK++);
				VRAM += 382;		BACK += 382;
				*(VRAM++) = *(BACK++);
				*(VRAM++) = *(BACK++);
			}
		case 1:	// set
		  set1:
			VRAM += 384*py+px;
			*(VRAM++) = color;
			*(VRAM++) = color;
			VRAM += 382;
			*(VRAM++) = color;
			*(VRAM++) = color;
			break;
		case 2:	// xor
			if ( VRAM[offset] == backcolor    ) goto set1;
			if ( BACK != NULL ) if ( VRAM[offset] == BACK[offset] ) goto set1;
			goto set0;
	}
}
void BdispSetPointVRAM2( int px, int py, int mode){	// g1m or g3m both
	if ( CB_G1MorG3M==1 ) BdispSetPointVRAMx9( px*3, py*3, mode);	// FX mode
	else				  BdispSetPointVRAM(   px, py, mode);		// CG mode
	return;
}
void CB_SetPixelPointVRAM2( int px, int py, int mode){	// g1m or g3m both
	if ( CB_G1MorG3M==1 ) BdispSetPointVRAMx9( px*3, py*3, mode);	// FX mode
	else				  BdispSetPointVRAM(   px+2, py+2, mode);	// CG mode
	return;
}
//-----------------------------------------------------------------------------
int BdispGetPointVRAM2( int px, int py){	// g1m or g3m both
	unsigned short *VRAM=(unsigned short *)PictAry[0];
	unsigned short *BACK=(unsigned short *)BG_Buf;
	int offset;
	unsigned short color,backcolor=CB_BackColorIndex;
	if ( CB_G1MorG3M==1 ) { px*=3; py*=3; }
	py+=24;
	offset = 384*py+px;
	if ( VRAM[offset] == backcolor    ) return 0;
	if ( BACK != NULL ) if ( VRAM[offset] == BACK[offset] ) return 0;
	return 1;
}
int BdispGetPointVRAM( int px, int py){		// {0,0}-{191,383}
	unsigned short *VRAM=(unsigned short *)PictAry[0];
	unsigned short *BACK=(unsigned short *)BG_Buf;
	int offset;
	unsigned short color,backcolor=CB_BackColorIndex;
	py+=24;
	offset = 384*py+px;
	if ( VRAM[offset] == backcolor    ) return 0;
	if ( BACK != NULL ) if ( VRAM[offset] == BACK[offset] ) return 0;
	return 1;
}

//-----------------------------------------------------------------------------
int skip_count=0;

void Bdisp_PutDisp_DD_DrawBusy_stripe( int py, int py2 ) {	// y:0~191
	if ( py2>191 ) py2=191;
	Bdisp_PutDisp_DD_stripe_BG( py+24, py2+24);
//	if ( CB_StatusDisp ) {
//		StatusArea_Run();
//		Bdisp_PutDisp_DD_stripe_BG( 0, 23);
//	}
	HourGlass();
}
void Bdisp_PutDisp_DD_DrawBusy() {
	Bdisp_PutDisp_DD_DrawBusy_stripe( 0, 191);
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
void Bdisp_PutDisp_DD_DrawBusy_skip_through_text_stripe( char *SRC, int py, int py2 ) {	// Locate text  py:0~191
	char c=SRC[ExecPtr++];
	if ( ( c == ':' ) && (SRC[ExecPtr]==0x0D) ) return ;
	if ( c == ';' ) { Bdisp_PutDisp_DD_DrawBusy_skip(); return ; }
	ExecPtr--;
	if ( RefreshCtrl == 2 ) 	// refresh control all
			Bdisp_PutDisp_DD_DrawBusy_skip();
	else
			Bdisp_PutDisp_DD_DrawBusy_stripe( py, py2 );
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
	PrintMinix3(  x, y, str,MINI_OVER);
}
//----------------------------------------------------------------------------------------------
void PXYtoVW(int px, int py, double *x, double *y){	// pixel(x,y) -> ViewWwindow(x,y)
//	if ( Xmax == Xmin )		*x = Xmin ;
//	*x = (    px-1)*Xdot  + Xmin ;
	if ( CB_G1MorG3M==1 ) {
		*x = (    px-1)*Xdot  + Xmin ;
		*y = ( 62-py+1)*Ydot  + Ymin ;
	} else {
		*x = (    px-1)*Xdot  + Xmin ;
		*y = (186-py+1)*Ydot  + Ymin ;
	}
//	*x = ( px-CB_G1MorG3M )*Xdot  + Xmin ;
//	if ( Xmax >  Xmin )		*x = (    (double)px-1)*Xdot  + Xmin ;
//	if ( Xmax <  Xmin )		*x = (126-(double)px+1)*-Xdot + Xmax ;
//	if ( Ymax == Ymin )		*y = Ymin ;
//	*y = ( 63-py)*Ydot  + Ymin ;
//	Ydot*(62*CB_G1MorG3M)+Ymin  =Ymax;
//	*y = ( ScreenHeight-py)*Ydot  + Ymin ;
//	if ( Ymax >  Ymin )		*y = ( 62-(double)py+1)*Ydot  + Ymin ;
//	if ( Ymax <  Ymin )		*y = (    (double)py-1)*-Ydot + Ymax ;
//	if ( fabs(*x)*1e10 < xdot ) *x=0;	// zero adjust
//	if ( fabs(*y)*1e10 < ydot ) *y=0;	// zero adjust
}
int VWtoPXY(double x, double y, int *px, int *py){	// ViewWwindow(x,y) -> pixel(x,y)
	if ( ( Xdot == 0 ) || ( Ydot == 0 ) || ( Xmax == Xmin ) || ( Ymax == Ymin ) ) { ErrorNo=RangeERR; ErrorPtr=ExecPtr; return -1 ; }	// Range error
	if ( CB_G1MorG3M==1 ) {
		*px =   1 + ( (x-Xmin)/Xdot + 0.5 ) ;
		*py =  63 - ( (y-Ymin)/Ydot - 0.49999999999999 ) ;
	} else {
		*px =   1 + ( (x-Xmin)/Xdot + 0.5 ) ;
		*py = 187 - ( (y-Ymin)/Ydot - 0.49999999999999 ) ;
	}
//	*px =   1 + ( (x-Xmin)/Xdot + 0.5 ) ;
//	*px =   CB_G1MorG3M + ( (x-Xmin)/Xdot + 0.5 ) ;
//	if ( Xmax >  Xmin )		*px =       (x-Xmin)/Xdot  +1.5 ;
//	if ( Xmax <  Xmin )		*px = 126 - (x-Xmax)/-Xdot +1.49999999999999 ;
//	*py =  63 - ( (y-Ymin)/Ydot - 0.49999999999999 ) ;
//	*y = ( 63-py)*CB_G1MorG3M*Ydot  + Ymin ;
//	(y-Ymin)/CB_G1MorG3M/Ydot=63-py;
//	*py = ScreenHeight - ( (y-Ymin)/Ydot - 0.49999999999999 ) ;
//	if ( Ymax >  Ymin )		*py =  62 - (y-Ymin)/Ydot  +1.49999999999999 ;
//	if ( Ymax <  Ymin )		*py =       (y-Ymax)/-Ydot +1.5 ;
	if ( (*px<MatBase) || (*px>ScreenWidth) || (*py<MatBase) || (*py> ScreenHeight) ) { return -1; }	// 
	return 0;
}

//-----------------------------------------------------------------------------
//short prev_Grid_px;
//short prev_Grid_py;

void PlotGrid(double x, double y){
	int px,py;
	int d=0;
	VWtoPXY( x,y, &px, &py);
	if ( px<MatBase ) px=MatBase;
	if ( py<MatBase ) py=MatBase;
	if (px> ScreenWidth ) px=ScreenWidth;
	if (py> ScreenHeight) py= ScreenHeight;
//	if ( ( abs(px-prev_Grid_px) >=CB_G1MorG3M*5 ) || ( abs(py-prev_Grid_py) >=CB_G1MorG3M*5 ) ) {
		if ( CB_G1MorG3M==3 ) BdispSetPointVRAMx9( px+1, py+1, 1);	// 3x3
		else	BdispSetPointVRAM2( px, py, 1);	// FX
//	}
//	prev_Grid_px=px;
//	prev_Grid_py=py;
}
void PlotGrid_CGline(double x, double y){
	int px,py,x1,y1,x2,y2;
	int d=0;
	VWtoPXY( x,y, &px, &py);
//	if ( ( abs(px-prev_Grid_px) >=CB_G1MorG3M*5 ) || ( abs(py-prev_Grid_py) >=CB_G1MorG3M*5 ) ) {
		if ( px<MatBase ) { px=MatBase;	if ( Grid==2 ) goto next; }
		if (px> ScreenWidth ) px=ScreenWidth;
		LinesubCG( px, MatBase, px, ScreenHeight, S_L_Thin, 1 );
	  next:
		if ( py<MatBase ) { py=MatBase;	if ( Grid==2 ) return; }
		if (py> ScreenHeight) py= ScreenHeight;
		LinesubCG( MatBase, py, ScreenWidth, py,  S_L_Thin, 1 );
//	}
//	prev_Grid_px=px;
//	prev_Grid_py=py;
}
void PlotGrid_Thin(double x, double y, int length, int xy, int *left, int *right ){
	int px,py;
	int i,w,h,x0,x1,y0,y1;
	char buffer[32];
	VWtoPXY( x,y, &px, &py);
	if ( px<MatBase ) px=MatBase;
	if ( py<MatBase ) py=MatBase;
	if (px> ScreenWidth ) px=ScreenWidth;
	if (py> ScreenHeight) py= ScreenHeight;
	if ( CB_G1MorG3M==3 ) {
		if ( xy==0 ) {
//			if ( ( Axes==2 ) && ( y!=0 ) ) {
//				if ( y<1 ) i=4; else i=5;
//				sprintGRS( buffer, y, 8, LEFT_ALIGN, Norm, i);
//				w= CB_PrintMiniMiniLengthStr( (unsigned char *)buffer );
//				if ( (2<px-w)&&(px<376)&&(2<py)&&(py<183) ) CB_PrintMiniMiniorWB( px-w, py-2, (unsigned char *)buffer );
//			}
			if ( ( Axes==2 ) && ( y!=0 ) ) {
				if ( y<1 ) i=4; else i=5;
				sprintGRS( buffer, y, 8, LEFT_ALIGN, Norm, i);
				w= CB_PrintMiniMiniLengthStr( (unsigned char *)buffer );
				h= 10;
				y0=py-2; y1=py-2+h;
				if ( ( length )&&(2<px-w)&&(px<376)&&(2<py)&&(py<183) ) {
					if ( ( *left ==0 ) && ( *right==0 ) && ( (h+2)<length ) ) CB_PrintMiniMiniorWB( px-w, y0, (unsigned char *)buffer );
					else
					if ( ( *left ==0 ) && ( y0<length     ) ) { *left =1; CB_PrintMiniMiniorWB( px-w, y0, (unsigned char *)buffer ); }
					else
					if ( ( *right==0 ) && ( y1>188-length ) ) { *right=1; CB_PrintMiniMiniorWB( px-w, y0, (unsigned char *)buffer ); }
				}
			}
			if ( px>376 ) px=376;
			if ( y>=0 ) for(i=0;i<3;i++) BdispSetPointVRAM( px+2+i, py+2, 1);	// 1x1
			if ( y<=0 ) for(i=0;i<3;i++) BdispSetPointVRAM( px+4-i, py+2, 1);	// 1x1
		} else {
			if ( ( Axes==2 ) && ( x!=0 ) ) {
				if ( x<1 ) i=4; else i=5;
				sprintGRS( buffer, x, 8, LEFT_ALIGN, Norm, i);
				w= CB_PrintMiniMiniLengthStr( (unsigned char *)buffer );
				x0=px-w/2; x1=px+w/2;
				if ( ( length )&&(0<x0)&&(x1<380)&&(0<py)&&(py<183) ) {
					if ( ( *left ==0 ) && ( *right==0 ) && ( (w+2)<length ) ) CB_PrintMiniMiniorWB( x0, py+6, (unsigned char *)buffer );
					else
					if ( ( *left ==0 ) && ( x0<length     ) ) { *left =1; CB_PrintMiniMiniorWB( x0, py+6, (unsigned char *)buffer ); }
					else
					if ( ( *right==0 ) && ( x1>378-length ) ) { *right=1; CB_PrintMiniMiniorWB( x0, py+6, (unsigned char *)buffer ); }
				}
			}
			if ( py>188 ) py=188;
			if ( x>=0 ) for(i=0;i<3;i++) BdispSetPointVRAM( px+2, py  +i, 1);	// 1x1
			if ( x<=0 ) for(i=0;i<3;i++) BdispSetPointVRAM( px+2, py+2-i, 1);	// 1x1
		}
	} else BdispSetPointVRAM2( px, py, 1);	// FX
}
	
void GraphAxesGrid(){
	double x,y;
	int px,py;
	int cbmode=CB_G1MorG3M;
	int colortmp=CB_ColorIndex;
	int length,left,right;

	if ( BG_Pict_No ) RclPict(BG_Pict_No, 0);	// no error check

//	CB_ColorIndex=0x0000;	// Black
//	CB_ColorIndex=0xF81F;	// Magenta
	CB_ColorIndex=0x07FF;	// Cyan
//	CB_ColorIndex=0x001F;	// Blue
//	prev_Grid_px=1000;
//	prev_Grid_py=1000;
	if ( ( CB_G1MorG3M == 3 ) && ( Grid==2 ) ) {	/// Line
		if ( ( Xscl > 0 ) && ( Yscl > 0 ) ) {
			if ( Xmin < 0 ) { 
					for ( x=0; x>=Xmin; x-=Xscl ) PlotGrid_CGline(x,y);
					if ( Ymin <= 0 ) for ( y=0; y>=Ymin; y-=Yscl )	PlotGrid_CGline(x,y);
					if ( Ymin >  0 ) for ( y=0; y<=Ymin; y+=Yscl )	PlotGrid_CGline(x,y);
					if ( Ymax <= 0 ) for ( y=0; y>=Ymax; y-=Yscl )	PlotGrid_CGline(x,y);
					if ( Ymax >  0 ) for ( y=0; y<=Ymax; y+=Yscl )	PlotGrid_CGline(x,y);
			}
			if ( Xmin >= 0 ) {
					for ( x=0; x<=Xmin; x+=Xscl) PlotGrid_CGline(x,y);
					if ( Ymin <= 0 ) for ( y=0; y>=Ymin; y-=Yscl )	PlotGrid_CGline(x,y);
					if ( Ymin >  0 ) for ( y=0; y<=Ymin; y+=Yscl )	PlotGrid_CGline(x,y);
					if ( Ymax <= 0 ) for ( y=0; y>=Ymax; y-=Yscl )	PlotGrid_CGline(x,y);
					if ( Ymax >  0 ) for ( y=0; y<=Ymax; y+=Yscl )	PlotGrid_CGline(x,y);
			}
			if ( Xmax < 0 ) {
					for ( x=0; x>=Xmax; x-=Xscl) PlotGrid_CGline(x,y);
					if ( Ymin <= 0 ) for ( y=0; y>=Ymin; y-=Yscl )	PlotGrid_CGline(x,y);
					if ( Ymin >  0 ) for ( y=0; y<=Ymin; y+=Yscl )	PlotGrid_CGline(x,y);
					if ( Ymax <= 0 ) for ( y=0; y>=Ymax; y-=Yscl )	PlotGrid_CGline(x,y);
					if ( Ymax >  0 ) for ( y=0; y<=Ymax; y+=Yscl )	PlotGrid_CGline(x,y);
			}
			if ( Xmax >= 0 ) {
					for ( x=0; x<=Xmax; x+=Xscl) PlotGrid_CGline(x,y);
					if ( Ymin <= 0 ) for ( y=0; y>=Ymin; y-=Yscl )	PlotGrid_CGline(x,y);
					if ( Ymin >  0 ) for ( y=0; y<=Ymin; y+=Yscl )	PlotGrid_CGline(x,y);
					if ( Ymax <= 0 ) for ( y=0; y>=Ymax; y-=Yscl )	PlotGrid_CGline(x,y);
					if ( Ymax >  0 ) for ( y=0; y<=Ymax; y+=Yscl )	PlotGrid_CGline(x,y);
			}
		}
	} else
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
	CB_G1MorG3M=cbmode;
	CB_ColorIndex=0x0000;	// Black
	if ( Axes ) {
		if ( CB_G1MorG3M==1  ) {
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
		} else {
			Horizontal(0, S_L_Thin,1);
			Vertical(0, S_L_Thin,1);
			if ( Yscl > 0 ) {
				length = 188/(Ymax-Ymin)*Yscl;
				left=0; right=0;
				if ( Ymin <= 0 ) for ( y=0; y>=Ymin; y-=Yscl )	PlotGrid_Thin(Xdot,y,length,0,&left,&right);
				if ( Ymin >  0 ) for ( y=0; y<=Ymin; y+=Yscl )	PlotGrid_Thin(Xdot,y,length,0,&left,&right);
				if ( Ymax <= 0 ) for ( y=0; y>=Ymax; y-=Yscl )	PlotGrid_Thin(Xdot,y,length,0,&left,&right);
				if ( Ymax >  0 ) for ( y=0; y<=Ymax; y+=Yscl )	PlotGrid_Thin(Xdot,y,length,0,&left,&right);
			}
			if ( Xscl > 0 ) {
				length = 378/(Xmax-Xmin)*Xscl;
				left=0; right=0;
				if ( Xmin <= 0 ) for ( x=0; x>=Xmin; x-=Xscl ) PlotGrid_Thin(x,Ydot,length,1,&left,&right);
				if ( Xmin >  0 ) for ( x=0; x<=Xmin; x+=Xscl ) PlotGrid_Thin(x,Ydot,length,1,&left,&right);
				if ( Xmax <= 0 ) for ( x=0; x>=Xmax; x-=Xscl ) PlotGrid_Thin(x,Ydot,length,1,&left,&right);
				if ( Xmax >  0 ) for ( x=0; x<=Xmax; x+=Xscl ) PlotGrid_Thin(x,Ydot,length,1,&left,&right);
			}
		}
	}
	CB_ColorIndex=0x0000;	// Black
	if ( Label ) {
		if ( CB_G1MorG3M==1  ) {
//			PrintMinix3(  1,17,(unsigned char*)" ",MINI_OVER);
			PrintMinix3(  1,17,(unsigned char*)"Y",MINI_OVER);
//			PrintMinix3(124,43,(unsigned char*)" ",MINI_OVER);
			PrintMinix3(124,43,(unsigned char*)"X",MINI_OVER);
		} else {
			VWtoPXY( 0,0, &px, &py);;
			if ( ( 0<px) && ( px<124*3 ) ) PrintMinix1orWB(   px+6, -2,   (unsigned char*)"y");
			if ( ( 0<py) && ( py< 58*3 ) ) PrintMinix1orWB(123*3+3, py-16,(unsigned char*)"x");
			if ( ( 0<px) && ( px<124*3 ) ) PrintMinix1orWB(  px-14, py+ 6,(unsigned char*)"O");
		}
	}
	
	CB_G1MorG3M=cbmode;
	CB_ColorIndex=colortmp;
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

//	Bdisp_AllClr_VRAM2();			// ------ Clear VRAM 
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
			if ( Plot_Style==S_L_Thin ) LinesubSetPoint_Thin(px, py, mode);	// 1*1 dot
			else	LinesubSetPoint(px, py,mode);	// 3x3 dot
			regX.real=x; regY.real=y;
			regintX=regX.real; regintY=regY.real;
	}
}

void PlotOn_VRAM(double x, double y){
	PlotSub( x, y, 1);		// VRAM
}

int PxlTest(int py, int px) {
	if ( CB_G1MorG3M==1 ) 	return BdispGetPointVRAM2( px, py );			// FX mode
	else {
//		if ( CB_G3M_TEXT ) { px*=3; py*=3; }
		return BdispGetPointVRAM2( px+2, py+2 );		// CG mode
	}
}
//------------------------------------------------------------------------------ LINE
void LinesubSetPoint_Thin(int px, int py, int mode) {	// (1,1)-(379,187)
	int lim=MatBase;
	if ( CB_G1MorG3M==1 ) {
		if ( ( px <  MatBase ) || ( px > 127 ) || ( py < MatBase ) || ( py >  63 ) ) return;
		BdispSetPointVRAMx9( px*3, py*3, mode);
	} else {
		if ( ( px <  MatBase ) || ( px > 379 ) || ( py < MatBase ) || ( py > 187 ) ) return;
		BdispSetPointVRAM( px+2, py+2, mode);
	}
}
void LinesubSetPointx9(int px, int py, int mode) {	// (1,1)-(379,187) 3*3 dot
	int x,y;
	int lim=MatBase+2;
	for ( y=py+1; y<=py+3; y++ ) {
		for ( x=px+1; x<=px+3; x++ ) {
			if ( ( x >= lim ) && ( x <= 381 ) && ( y >= lim  ) && ( y <= 189  ) ) BdispSetPointVRAM( x, y, mode);
		}
	}
}
void LinesubSetPoint(int px, int py, int mode) {	// (1,1)-(379,187)
	if ( CB_G1MorG3M==1 ) {
		if ( ( px <  MatBase ) || ( px > 127 ) || ( py < MatBase ) || ( py >  63 ) ) return;
		BdispSetPointVRAMx9( px*3, py*3, mode);
	} else {
		if ( ( px <=  MatBase ) || ( px >= 379 ) || ( py <= MatBase ) || ( py >= 187 ) ) LinesubSetPointx9( px, py, mode);
		else	BdispSetPointVRAMx9( px+1, py+1, mode);
	}
}

int prev_px0=-1,prev_py0=-1;	// Prev Thick dot 
int prev_px1=-1,prev_py1=-1;
int prev_px2=-1,prev_py2=-1;
int prev_px3=-1,prev_py3=-1;

void LinesubSetPointThick(int px, int py, int mode) {	// fx 2*2
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
void LinesubSetPointThick5x5(int px, int py, int mode) {	// cg 5x5
	px++; py++;
	if ( mode == 2 ) {	// xor 
		if ( ((prev_px0!=px  )||(prev_py0!=py  ))&&((prev_px1!=px  )||(prev_py1!=py  ))&&((prev_px2!=px  )||(prev_py2!=py  ))&&((prev_px3!=px  )||(prev_py3!=py  )) ) LinesubSetPoint(px  , py  , mode);
		if ( ((prev_px0!=px  )||(prev_py0!=py-2))&&((prev_px1!=px  )||(prev_py1!=py-2))&&((prev_px2!=px  )||(prev_py2!=py-2))&&((prev_px3!=px  )||(prev_py3!=py-2)) ) LinesubSetPoint(px  , py-2, mode);
		if ( ((prev_px0!=px-2)||(prev_py0!=py  ))&&((prev_px1!=px-2)||(prev_py1!=py  ))&&((prev_px2!=px-2)||(prev_py2!=py  ))&&((prev_px3!=px-2)||(prev_py3!=py  )) ) LinesubSetPoint(px-2, py  , mode);
		if ( ((prev_px0!=px-2)||(prev_py0!=py-2))&&((prev_px1!=px-2)||(prev_py1!=py-2))&&((prev_px2!=px-2)||(prev_py2!=py-2))&&((prev_px3!=px-2)||(prev_py3!=py-2)) ) LinesubSetPoint(px-2, py-2, mode);
		prev_px0=px  ; prev_py0=py;
		prev_px1=px  ; prev_py1=py-2;
		prev_px2=px-2; prev_py2=py;
		prev_px3=px-2; prev_py3=py-2;
	} else {
		LinesubSetPoint(px  , py  , mode);
		LinesubSetPoint(px  , py-2, mode);
		LinesubSetPoint(px-2, py  , mode);
		LinesubSetPoint(px-2, py-2, mode);
	}
}

void LinesubFX(int px1, int py1, int px2, int py2, int style, int mode) {
	int i, j;
	int x, y;
	int dx, dy; // delta x,y
	int wx, wy; // width x,y
	int Styleflag=1;
	int tmp;
	int lim=500;
	
	if ( ( px1 <  -lim ) || ( px1 > lim ) || ( py1 < -lim ) || ( py1 >  lim ) ) return;
	if ( ( px2 <  -lim ) || ( px2 > lim ) || ( py2 < -lim ) || ( py2 >  lim ) ) return;

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
		case S_L_Thin:	// ---------- Thin
			if (wx==0) {	// vertical line
					x=px1; y=py1;
					while( wy>=0 ) {
						LinesubSetPoint_Thin(x, y, mode);
						wy--; y+=dy;
					}
			} else
			if (wy==0) { // horizontal line
					x=px1; y=py1;
					while( wx>=0 ) {
						LinesubSetPoint_Thin(x, y, mode);
						wx--; x+=dx;
					}
			} else
			if (wx>=wy) {
					x=px1; y=py1; j=wx/2; i=wx;
					while( wx>=0 ) {
						LinesubSetPoint_Thin(x, y, mode);
						wx--; x+=dx;
						j-=wy; if (j<0) { j+=i; y+=dy; }
					}
			} else {
					x=px1; y=py1; j=wy/2; i=wy;
					while( wy>=0 ) {
						LinesubSetPoint_Thin(x, y, mode);
						wy--; y+=dy;
						j-=wx; if (j<0) { j+=i; x+=dx; }
					}
			}
			break;
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

void LinesubCG(int px1, int py1, int px2, int py2, int style, int mode) {
	int i, j;
	int x, y;
	int dx, dy; // delta x,y
	int wx, wy; // width x,y
	int Styleflag=1;
	int tmp;
	int lim=3*500;
	
	if ( ( px1 <  -lim ) || ( px1 > lim ) || ( py1 < -lim ) || ( py1 >  lim ) ) return;
	if ( ( px2 <  -lim ) || ( px2 > lim ) || ( py2 < -lim ) || ( py2 >  lim ) ) return;

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
		case S_L_Thin:	// ---------- Thin	1*1
			if (wx==0) {	// vertical line
					x=px1; y=py1;
					while( wy>=0 ) {
						LinesubSetPoint_Thin(x, y, mode);
						wy--; y+=dy;
					}
			} else
			if (wy==0) { // horizontal line
					x=px1; y=py1;
					while( wx>=0 ) {
						LinesubSetPoint_Thin(x, y, mode);
						wx--; x+=dx;
					}
			} else
			if (wx>=wy) {
					x=px1; y=py1; j=wx/2; i=wx;
					while( wx>=0 ) {
						LinesubSetPoint_Thin(x, y, mode);
						wx--; x+=dx;
						j-=wy; if (j<0) { j+=i; y+=dy; }
					}
			} else {
					x=px1; y=py1; j=wy/2; i=wy;
					while( wy>=0 ) {
						LinesubSetPoint_Thin(x, y, mode);
						wy--; y+=dy;
						j-=wx; if (j<0) { j+=i; x+=dx; }
					}
			}
			break;
		case S_L_Normal:	// ---------- Normal	3*3
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
		case S_L_Dot:		// ---------- Dot	3*3
			if (wx==0) {	// vertical line
					x=px1; y=py1;
					while( wy>=0 ) {
						if (Styleflag==0)
						LinesubSetPoint(x, y, mode);
						wy--; y+=dy;
						Styleflag++; if ( Styleflag>5 ) Styleflag =0;
					}
			} else
			if (wy==0) { // horizontal line
					x=px1; y=py1;
					while( wx>=0 ) {
						if (Styleflag==0)
						LinesubSetPoint(x, y, mode);
						wx--; x+=dx;
						Styleflag++; if ( Styleflag>5 ) Styleflag =0;
					}
			} else
			if (wx>wy) {
					x=px1; y=py1; j=wx/2; i=wx;
					while( wx>=0 ) {
						if (Styleflag==0)
						LinesubSetPoint(x, y, mode);
						wx--; x+=dx;
						j-=wy; if (j<0) { j+=i; y+=dy; }
						Styleflag++; if ( Styleflag>5 ) Styleflag =0;
					}
			} else {
					x=px1; y=py1; j=wy/2; i=wy;
					while( wy>=0 ) {
						if (Styleflag==0)
						LinesubSetPoint(x, y, mode);
						wy--; y+=dy;
						j-=wx; if (j<0) { j+=i; x+=dx; }
						Styleflag++; if ( Styleflag>5 ) Styleflag =0;
					}
			}
			break;
		case S_L_Thick:	// ---------- Thick	5*5
			if (wx==0) {	// vertical line
					x=px1; y=py1;
					while( wy>=0 ) {
						LinesubSetPointThick5x5(x, y, mode);;
						wy--; y+=dy;
					}
			} else
			if (wy==0) { // horizontal line
					x=px1; y=py1;
					while( wx>=0 ) {
						LinesubSetPointThick5x5(x, y, mode);;
						wx--; x+=dx;
					}
			} else
			if (wx>=wy) {
					x=px1; y=py1; j=wx/2; i=wx;
					while( wx>=0 ) {
						LinesubSetPointThick5x5(x, y, mode);;
						wx--; x+=dx;
						j-=wy; if (j<0) { j+=i; y+=dy; }
					}
			} else {
					x=px1; y=py1; j=wy/2; i=wy;
					while( wy>=0 ) {
						LinesubSetPointThick5x5(x, y, mode);;
						wy--; y+=dy;
						j-=wx; if (j<0) { j+=i; x+=dx; }
					}
			}
			break;
		case S_L_Broken:	// ---------- Broken	3*3
			if (wx==0) {	// vertical line
					x=px1; y=py1;
					while( wy>=0 ) {
						if (Styleflag<4) LinesubSetPoint(x, y, mode);
						wy--; y+=dy;
						Styleflag++; if (Styleflag>8) Styleflag=0;
					}
			} else
			if (wy==0) { // horizontal line
					x=px1; y=py1;
					while( wx>=0 ) {
						if (Styleflag<4) LinesubSetPoint(x, y, mode);
						wx--; x+=dx;
						Styleflag++; if (Styleflag>8) Styleflag=0;
					}
			} else
			if (wx>=wy) {
					x=px1; y=py1; j=wx/2; i=wx;
					while( wx>=0 ) {
						if (Styleflag<4) LinesubSetPoint(x, y, mode);
						wx--; x+=dx;
						j-=wy; if (j<0) { j+=i; y+=dy; }
						Styleflag++; if (Styleflag>8) Styleflag=0;
					}
			} else {
					x=px1; y=py1; j=wy/2; i=wy;
					while( wy>=0 ) {
						if (Styleflag<4) LinesubSetPoint(x, y, mode);
						wy--; y+=dy;
						j-=wx; if (j<0) { j+=i; x+=dx; }
						Styleflag++; if (Styleflag>8) Styleflag=0;
					}
			}
			break;
		default:
			break;
		}
	S_L_Style=tmp;
}

void Linesub(int px1, int py1, int px2, int py2, int style, int mode) {
	if ( CB_G1MorG3M==1 ) LinesubFX(px1, py1, px2, py2, style, mode) ;
	else				  LinesubCG(px1, py1, px2, py2, style, mode) ;
}



void Lines(int style, int mode, int errorcheck ) {
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
	if ( px>ScreenWidth ) return;
	Linesub( px, ScreenHeight, px, MatBase,  style, mode);
}
void Horizontal(double y, int style, int mode) {
	int px,py;
	VWtoPXY( 0, y, &px, &py);
	if ( py<  0 ) return;
	if ( py> ScreenHeight ) return;
	Linesub( ScreenWidth, py, MatBase, py,  style, mode);
}

void Circle(double x, double y, double r, int style, int drawflag, int mode ) {
	double	angle, k, x0,y0,x1,y1;
	int px,py;
	int	i,n;
	prev_px0=-1;	// Prev Thick dot clear
	prev_px1=-1;
	prev_px2=-1;
	prev_px3=-1;
	if (style==S_L_Thin  ) { k=12;if ( ( r/Xdot )  > 20 ) k=8; }
	if (style==S_L_Normal) { k=8; if ( ( r/Xdot )  > 20 ) k=6; }
	if (style==S_L_Dot   ) { k=4; if ( ( r/Xdot )  > 6  ) k=3;
		if ( CB_G1MorG3M==3 ) k/=3;
	}
	if (style==S_L_Thick ) k=4;
	if (style==S_L_Broken) { k=2; if ( CB_G1MorG3M==3 ) k/=2; }
	n=fabs(floor(r*k/Xdot));
	Plot_X = r+x;
	Plot_Y = 0+y;
	for(i=1;i<=n;i++){
		angle=const_PI*2*i/n;
		Plot_X=cos(angle)*r+x;
		Plot_Y=sin(angle)*r+y;
//		Plot_X=(icos((angle*180./const_PI+.5),r*1024)>>10) +x;
//		Plot_Y=(isin((angle*180./const_PI+.5),r*1024)>>10) +y;
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
				case S_L_Thin:
					LinesubSetPoint_Thin(px, py, mode);
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
int ObjectAlignG1( unsigned int n ){ return n; }	// align +4byte
int ObjectAlignG2( unsigned int n ){ return n; }	// align +4byte
int ObjectAlignG3( unsigned int n ){ return n; }	// align +4byte
int ObjectAlignG4( unsigned int n ){ return n; }	// align +4byte
//int ObjectAlignG5( unsigned int n ){ return n; }	// align +4byte
//int ObjectAlignG6( unsigned int n ){ return n; }	// align +4byte
//int ObjectAlignG7( unsigned int n ){ return n; }	// align +4byte
//int ObjectAlignG8( unsigned int n ){ return n; }	// align +4byte
//int ObjectAlignG9( unsigned int n ){ return n; }	// align +4byte
