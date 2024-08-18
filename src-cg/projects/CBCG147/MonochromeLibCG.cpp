extern "C" {

/*************************************************************/
/** MonochromeLib - monochrome graphic library for fx-9860G **/
/** MonochromeLib is free software                          **/
/**                                                         **/
/** @author Pierre "PierrotLL" Le Gall                      **/
/** @contact legallpierre89@gmail.com                       **/
/**                                                         **/
/** @file MonochromeLib.c                                   **/
/** Code file of MonochromeLib                              **/
/**                                                         **/
/** @date 11-22-2011                                        **/
/*************************************************************/

// modified for C.Basic CG
#include "prizm.h"
#include "CBP.h"

/******************************/
/** Dependencies management  **/
/******************************/

#ifdef ML_ALL
	#define ML_CLEAR_VRAM
	#define ML_CLEAR_SCREEN
	#define ML_DISPLAY_VRAM
	#define ML_SET_CONTRAST
	#define ML_GET_CONTRAST
	#define ML_PIXEL
	#define ML_POINT
	#define ML_PIXEL_TEST
	#define ML_LINE
	#define ML_HORIZONTAL_LINE
	#define ML_VERTICAL_LINE
	#define ML_RECTANGLE
	#define ML_POLYGON
	#define ML_FILLED_POLYGON
	#define ML_CIRCLE
	#define ML_FILLED_CIRCLE
	#define ML_ELLIPSE
	#define ML_ELLIPSE_IN_RECT
	#define ML_FILLED_ELLIPSE
	#define ML_FILLED_ELLIPSE_IN_RECT
	#define ML_HORIZONTAL_SCROLL
	#define ML_VERTICAL_SCROLL
	#define ML_BMP_OR
	#define ML_BMP_AND
	#define ML_BMP_XOR
//	#define ML_BMP_OR_CL
//	#define ML_BMP_AND_CL
//	#define ML_BMP_XOR_CL
	#define ML_BMP_8_OR
	#define ML_BMP_8_AND
	#define ML_BMP_8_XOR
//	#define ML_BMP_8_OR_CL
//	#define ML_BMP_8_AND_CL
//	#define ML_BMP_8_XOR_CL
	#define ML_BMP_16_OR
	#define ML_BMP_16_AND
	#define ML_BMP_16_XOR
//	#define ML_BMP_16_OR_CL
//	#define ML_BMP_16_AND_CL
//	#define ML_BMP_16_XOR_CL
	#define ML_BMP_ZOOM
	#define ML_BMP_ROTATE
#endif

#ifdef ML_POLYGON
	#define ML_LINE
#endif

#ifdef ML_LINE
	#define ML_PIXEL
#endif

#ifdef ML_POINT
	#define ML_PIXEL
	#define ML_RECTANGLE
#endif

#ifdef ML_RECTANGLE
	#define ML_HORIZONTAL_LINE
#endif

#ifdef ML_FILLED_POLYGON
	#define ML_HORIZONTAL_LINE
#endif

#ifdef ML_CIRCLE
	#define ML_PIXEL
#endif

#ifdef ML_FILLED_CIRCLE
	#define ML_HORIZONTAL_LINE
#endif

#ifdef ML_ELLIPSE_IN_RECT
	#define ML_ELLIPSE
#endif

#ifdef ML_ELLIPSE
	#define ML_PIXEL
#endif

#ifdef ML_FILLED_ELLIPSE_IN_RECT
	#define ML_FILLED_ELLIPSE
#endif

#ifdef ML_FILLED_ELLIPSE
	#define ML_HORIZONTAL_LINE
#endif


/***************/
/** Functions **/
/***************/

#define sgn(x)	(x<0?-1:1)
#define rnd(x)	((int)(x+0.5))

//Thanks to Simon Lothar for this function
//static int SysCallCode[] = {0xD201422B,0x60F20000,0x80010070};
//static int (*SysCall)( int R4, int R5, int R6, int R7, int FNo ) = (void*)&SysCallCode;
//char* ML_vram_adress()
//{
//	return (char*)((*SysCall)(0, 0, 0, 0, 309));
//}

//	measures of the error OS ver 2.09 
#define ML_vram_adress (char*)GetVRAMAddress

//----------------------------------------------------------------------------------------------
int MLV_rand;	// randam pixel 0~RAND_MAX
int MLV_width;	// ML_line width option
unsigned short MLV_color=0x0000;	// ML color for CG
//----------------------------------------------------------------------------------------------

#ifdef ML_CLEAR_VRAM
void ML_clear_vram()
{
//	Bdisp_AllClr_VRAM();
//	unsigned short *vram=(unsigned short*)PictAry[0];
//	unsigned short backcolor=CB_BackColorIndex;
//	int i;
//	for (i=0; i<216*384/2; i++ ) {
//		*(vram++) = backcolor;
//		*(vram++) = backcolor;
//	}
	int *vram=(int *)PictAry[0];
	int backcolor=(CB_BackColorIndex<<16)|CB_BackColorIndex;
	int i;
	for (i=0; i<216*384/2; i++ ) *(vram++) = backcolor;
}
#endif

#ifdef ML_CLEAR_SCREEN
void ML_clear_screen()
{
	Bdisp_Fill_DD( CB_BackColorIndex, 3 );
//	Bdisp_AllClr_VRAM();
//	Bdisp_PutDisp_DD();
//	Bdisp_AllClr_VRAM();
//	Bdisp_PutDisp_DD_stripe( 0, 215);
}
#endif

#ifdef ML_DISPLAY_VRAM
void ML_display_vram()
{
//	Bdisp_PutDisp_DD();
	Bdisp_PutDisp_DD_stripe_BG( 0, 215);
}
#endif

#ifdef ML_SET_CONTRAST
void ML_set_contrast(unsigned char contrast)
{
//	Bdisp_SetBacklightLevel( contrast );
}
#endif

#ifdef ML_GET_CONTRAST
unsigned char ML_get_contrast()
{
	return -1;
}
#endif

#ifdef ML_PIXEL
void ML_pixel_FX(int x, int y, ML_Color color, int RGBcolor, int RGBbackcolor ){
	int	tmp=CB_G1MorG3M;
	int tmp2=CB_ColorIndex;
	int tmp3=CB_BackColorIndex;
	CB_G1MorG3M=1;
	CB_ColorIndex=RGBcolor;
	CB_BackColorIndex=RGBbackcolor;
	ML_pixel( x, y, color );
	CB_G1MorG3M=tmp;
	CB_ColorIndex=tmp2;
	CB_BackColorIndex=tmp3;
}
void ML_pixel_CG(int x, int y, ML_Color color, int RGBcolor, int RGBbackcolor ){
	int	tmp=CB_G1MorG3M;
	int tmp2=CB_ColorIndex;
	int tmp3=CB_BackColorIndex;
	CB_G1MorG3M=3;
	CB_ColorIndex=RGBcolor;
	CB_BackColorIndex=RGBbackcolor;
	ML_pixel( x, y, color );
	CB_G1MorG3M=tmp;
	CB_ColorIndex=tmp2;
	CB_BackColorIndex=tmp3;
}
void ML_pixel(int x, int y, ML_Color color )
{
	unsigned short *vram=(unsigned short *)PictAry[0];
//	unsigned short CGcolor;
//	unsigned short backcolor=CB_BackColorIndex;
	int CGcolor,col;
	int backcolor=CB_BackColorIndex;
	int i,j,d;
	int xt,yt;
	int k;
	int b,g,r,bg_b,bg_g,bg_r,alpha;
	int tcolor=CB_TransparentColorIndex;
	if ( CB_ColorIndex >= 0 ) CGcolor=CB_ColorIndex; else CGcolor=CB_ColorIndexPlot;

	if ( CB_G1MorG3M==1 ) {
		if (x&~127 || y&~63) return;
		k=3;
	} else {
		if ( ( x<0 ) || ( 383<x ) || ( y<-24 ) || ( 191<y ) ) return;
		k=1;
	}
	switch(color)
	{
		case ML_ALPHA:
			alpha=(MLV_rand<<8)/(RAND_MAX+1);
			xt=x*k;
			yt=(y*k+24)*384;
			vram+=xt+yt;
			d =*vram;
			b = (((int)CGcolor&0x001F) << 3);
			g = (((int)CGcolor&0x07E0) >> 3);
			r = (((int)CGcolor&0xF800) >> 8);
			bg_b = (((int)d&0x001F) << 3);
			bg_g = (((int)d&0x07E0) >> 3);
			bg_r = (((int)d&0xF800) >> 8);
			b = ( b*alpha + bg_b*(256-alpha) ) >>8;
			g = ( g*alpha + bg_g*(256-alpha) ) >>8;
			r = ( r*alpha + bg_r*(256-alpha) ) >>8;
			CGcolor = ((r<<8)&0xF800) | ((g<<3)&0x7E0) | ((b>>3)&0x1F);
			goto blackpset;
			
		case ML_BLACK:
		  black:
			xt=x*k;
			yt=(y*k+24)*384;
			vram+=xt+yt;
		  blackpset:
			if ( k==1 ) {
				if ( CGcolor != tcolor ) *vram = CGcolor;
			} else {
			  	*(vram++) = CGcolor;
			  	*(vram++) = CGcolor;
			  	*(vram++) = CGcolor;
			  	vram+=381;
			  	*(vram++) = CGcolor;
			  	*(vram++) = CGcolor;
			  	*(vram++) = CGcolor;
			  	vram+=381;
			  	*(vram++) = CGcolor;
			  	*(vram++) = CGcolor;
			  	*(vram++) = CGcolor;
			}
			break;
		case ML_WHITE:
		  white:
			xt=x*k;
			yt=(y*k+24)*384;
			vram+=xt+yt;
			if ( k==1 ) {
				if ( backcolor != tcolor ) *vram = backcolor;
			} else {
			  	*(vram++) = backcolor;
			  	*(vram++) = backcolor;
			  	*(vram++) = backcolor;
			  	vram+=381;
			  	*(vram++) = backcolor;
			  	*(vram++) = backcolor;
			  	*(vram++) = backcolor;
			  	vram+=381;
			  	*(vram++) = backcolor;
			  	*(vram++) = backcolor;
			  	*(vram++) = backcolor;
			}
			break;
		case ML_XOR:
		  xor1:
			xt=x*k;
			yt=(y*k+24)*384;
			vram+=xt+yt;
			if ( k==1 ) {
				*vram ^= 0xFFFF;
			} else {
			  	*(vram++) ^= 0xFFFF;
			  	*(vram++) ^= 0xFFFF;
			  	*(vram++) ^= 0xFFFF;
			  	vram+=381;
			  	*(vram++) ^= 0xFFFF;
			  	*(vram++) ^= 0xFFFF;
			  	*(vram++) ^= 0xFFFF;
			  	vram+=381;
			  	*(vram++) ^= 0xFFFF;
			  	*(vram++) ^= 0xFFFF;
			  	*(vram++) ^= 0xFFFF;
			}
			break;
		case ML_CHECKER:
		  checker:
			if(y&1^x&1) goto black;
			else goto white;
			break;

		case ML_RANDPX:
			if ( MLV_rand >= rand() ) goto black; else goto white;
			break;
			
		case ML_22DOT10:
					goto white;
		case ML_22DOT11:
			switch ( (x&1)+(y&1)*2 ) {
				case 0:
					goto black;
				case 1:
					goto white;
				case 2:
					goto white;
				case 3:
					goto white;
			}
			break;
		case ML_22DOT12:
			switch ( (x&1)+(y&1)*2 ) {
				case 0:
					goto black;
				case 1:
					goto white;
				case 2:
					goto white;
				case 3:
					goto black;
			}
			break;
		case ML_22DOT13:
			switch ( (x&1)+(y&1)*2 ) {
				case 0:
					goto black;
				case 1:
					goto black;
				case 2:
					goto black;
				case 3:
					goto white;
			}
			break;
		case ML_22DOT14:
					goto black;
			
		case ML_22DOT20:
					ML_pixel( x  , y  , ML_WHITE);
					ML_pixel( x+1, y  , ML_WHITE);
					ML_pixel( x  , y+1, ML_WHITE);
					ML_pixel( x+1, y+1, ML_WHITE);
			break;
		case ML_22DOT21:
			switch ( (x&1)+(y&1)*2 ) {
				case 0:
					ML_pixel( x  , y  , ML_BLACK);
					ML_pixel( x+1, y  , ML_WHITE);
					ML_pixel( x  , y+1, ML_WHITE);
					ML_pixel( x+1, y+1, ML_WHITE);
					break;
				case 1:
					ML_pixel( x  , y  , ML_WHITE);
					ML_pixel( x+1, y  , ML_BLACK);
					ML_pixel( x  , y+1, ML_WHITE);
					ML_pixel( x+1, y+1, ML_WHITE);
					break;
				case 2:
					ML_pixel( x  , y  , ML_WHITE);
					ML_pixel( x+1, y  , ML_WHITE);
					ML_pixel( x  , y+1, ML_BLACK);
					ML_pixel( x+1, y+1, ML_WHITE);
					break;
				case 3:
					ML_pixel( x  , y  , ML_WHITE);
					ML_pixel( x+1, y  , ML_WHITE);
					ML_pixel( x  , y+1, ML_WHITE);
					ML_pixel( x+1, y+1, ML_BLACK);
					break;
			}
			break;
		case ML_22DOT22:
			switch ( (x&1)+(y&1)*2 ) {
				case 0:
					ML_pixel( x  , y  , ML_BLACK);
					ML_pixel( x+1, y  , ML_WHITE);
					ML_pixel( x  , y+1, ML_WHITE);
					ML_pixel( x+1, y+1, ML_BLACK);
					break;
				case 1:
					ML_pixel( x  , y  , ML_WHITE);
					ML_pixel( x+1, y  , ML_BLACK);
					ML_pixel( x  , y+1, ML_BLACK);
					ML_pixel( x+1, y+1, ML_WHITE);
					break;
				case 2:
					ML_pixel( x  , y  , ML_WHITE);
					ML_pixel( x+1, y  , ML_BLACK);
					ML_pixel( x  , y+1, ML_BLACK);
					ML_pixel( x+1, y+1, ML_WHITE);
					break;
				case 3:
					ML_pixel( x  , y  , ML_BLACK);
					ML_pixel( x+1, y  , ML_WHITE);
					ML_pixel( x  , y+1, ML_WHITE);
					ML_pixel( x+1, y+1, ML_BLACK);
					break;
			}
			break;
		case ML_22DOT23:
			switch ( (x&1)+(y&1)*2 ) {
				case 0:
					ML_pixel( x  , y  , ML_BLACK);
					ML_pixel( x+1, y  , ML_BLACK);
					ML_pixel( x  , y+1, ML_BLACK);
					ML_pixel( x+1, y+1, ML_WHITE);
					break;
				case 1:
					ML_pixel( x  , y  , ML_BLACK);
					ML_pixel( x+1, y  , ML_BLACK);
					ML_pixel( x  , y+1, ML_WHITE);
					ML_pixel( x+1, y+1, ML_BLACK);
					break;
				case 2:
					ML_pixel( x  , y  , ML_BLACK);
					ML_pixel( x+1, y  , ML_WHITE);
					ML_pixel( x  , y+1, ML_BLACK);
					ML_pixel( x+1, y+1, ML_BLACK);
					break;
				case 3:
					ML_pixel( x  , y  , ML_WHITE);
					ML_pixel( x+1, y  , ML_BLACK);
					ML_pixel( x  , y+1, ML_BLACK);
					ML_pixel( x+1, y+1, ML_BLACK);
					break;
			}
			break;
		case ML_22DOT24:
					ML_pixel( x  , y  , ML_BLACK);
					ML_pixel( x+1, y  , ML_BLACK);
					ML_pixel( x  , y+1, ML_BLACK);
					ML_pixel( x+1, y+1, ML_BLACK);
			break;
	}
}
#endif

#ifdef ML_POINT
void ML_point(int x, int y, int width, ML_Color color)
{
	int i,j;
	if(width < 1) return;
	if(width == 1) ML_pixel(x, y, color);
	else
	{
		int padding, pair;
		padding = width>>1;
		pair = !(width&1);
		for ( i=y-padding+pair; i<=y+padding; i++) {
			for ( j=x-padding+pair; j<=x+padding; j++) ML_pixel(j, i, color);
		}
	}
}
#endif

#ifdef ML_PIXEL_TEST
int ML_pixel_test(int x, int y)
{
	unsigned short *vram=(unsigned short *)PictAry[0];
	if ( CB_G1MorG3M==1 ) { x*=3; y*=3; 
		return vram[x+(y+24)*384]!=CB_BackColorIndex;
	} else {
		return vram[x+(y+24)*384];
	}
}
int ML_pixel_test_TVRAM(int x, int y)
{
	unsigned short *vram=(unsigned short *)TVRAM;
	if ( CB_G1MorG3M==1 ) { x*=3; y*=3; 
		return vram[x+(y+24)*384]!=CB_BackColorIndex;
	} else {
		return vram[x+(y+24)*384];
	}
}
int ML_pixel_test_GVRAM(int x, int y)
{
	unsigned short *vram=(unsigned short *)GVRAM;
	if ( CB_G1MorG3M==1 ) { x*=3; y*=3; 
		return vram[x+(y+24)*384]!=CB_BackColorIndex;
	} else {
		return vram[x+(y+24)*384];
	}
}
#endif




#ifdef ML_LINE

void ML_line_FX(int x1, int y1, int x2, int y2, ML_Color color, int RGBcolor, int RGBbackcolor ){
	int	tmp=CB_G1MorG3M;
	int tmp2=CB_ColorIndex;
	int tmp3=CB_BackColorIndex;
	CB_G1MorG3M=1;
	CB_ColorIndex=RGBcolor;
	CB_BackColorIndex=RGBbackcolor;
	MLV_width=1;
	ML_line( x1, y1, x2, y2, color );
	CB_G1MorG3M=tmp;
	CB_ColorIndex=tmp2;
	CB_BackColorIndex=tmp3;
}
void ML_line_CG(int x1, int y1, int x2, int y2, ML_Color color, int RGBcolor, int RGBbackcolor ){
	int	tmp=CB_G1MorG3M;
	int tmp2=CB_ColorIndex;
	int tmp3=CB_BackColorIndex;
	CB_G1MorG3M=3;
	CB_ColorIndex=RGBcolor;
	CB_BackColorIndex=RGBbackcolor;
	MLV_width=1;
	ML_line( x1, y1, x2, y2, color );
	CB_G1MorG3M=tmp;
	CB_ColorIndex=tmp2;
	CB_BackColorIndex=tmp3;
}
void ML_line(int x1, int y1, int x2, int y2, ML_Color color)
{
	int i, x, y, dx, dy, sx, sy, cumul;
	int width = ( MLV_width );
	x = x1;
	y = y1;
	dx = x2 - x1;
	dy = y2 - y1;
	sx = sgn(dx);
	sy = sgn(dy);
	dx = abs(dx);
	dy = abs(dy);
//	if (dx==0) {	// vertical line
//		ML_vertical_line( x1, y1, y2, color);
//		return;
//	} else
//	if (dy==0) { // horizontal line
//		ML_horizontal_line( y1, x1, x2, color);
//		return;
//	}
	ML_pixel(x, y, color);
	if(dx > dy)
	{
		cumul = dx / 2;
		for(i=1 ; i<=dx ; i++)		// i<dx  ->  i<=dx	(modified)
		{
			x += sx;
			cumul += dy;
			if(cumul >= dx)			// > dx  ->  >= dx	(modified)
			{
				cumul -= dx;
				y += sy;
			}
			if ( width > 1 ) 	ML_point(x, y, width, color);		// 	(modified)
			else					ML_pixel(x, y, color);			// 	(modified)
		}
	}
	else
	{
		cumul = dy / 2;
		for(i=1 ; i<=dy ; i++)		// i<dy  ->  i<=dy	(modified)
		{
			y += sy;
			cumul += dx;
			if(cumul >= dy)			// > dy  ->  >= dy	(modified)
			{
				cumul -= dy;
				x += sx;
			}
			if ( width > 1 ) 	ML_point(x, y, width, color);		// 	(modified)
			else					ML_pixel(x, y, color);			// 	(modified)
		}
	}
}
/*
void ML_line(int px2, int py2, int px1, int py1, ML_Color color) {	// modified near to CasioBasic F-Line
	int i, j;
	int x, y;
	int dx, dy; // delta x,y
	int wx, wy; // width x,y
	int width = ( MLV_width );

	if (px1==px2) { dx= 0; wx=0; }
	if (px1< px2) { dx= 1; wx=px2-px1; }
	if (px1> px2) { dx=-1; wx=px1-px2;}
	if (py1==py2) { dy= 0; wy=0; }
	if (py1< py2) { dy= 1; wy=py2-py1; }
	if (py1> py2) { dy=-1; wy=py1-py2; }

	if (wx==0) {	// vertical line
		ML_vertical_line( px1, py1, py2, color);
	} else
	if (wy==0) { // horizontal line
		ML_horizontal_line( py1, px1, px2, color);
	} else
	if (wx>=wy) {
		x=px1; y=py1; j=wx/2; i=wx;
		while( wx>=0 ) {
			if ( width > 1 ) 	ML_point(x, y, width, color);		// 	(modified)
			else					ML_pixel(x, y, color);			// 	(modified)
			wx--; x+=dx;
			j-=wy; if (j<0) { j+=i; y+=dy; }
		}
	} else {
		x=px1; y=py1; j=wy/2; i=wy;
		while( wy>=0 ) {
			if ( width > 1 ) 	ML_point(x, y, width, color);		// 	(modified)
			else					ML_pixel(x, y, color);			// 	(modified)
			wy--; y+=dy;
			j-=wx; if (j<0) { j+=i; x+=dx; }
		}
	}
}
*/
#endif

#ifdef ML_HORIZONTAL_LINE
/*
void ML_horizontal_line(int y, int x1, int x2, ML_Color color){
	ML_line( x1, y, x2, y, color);
}
*/
void ML_horizontal_line(int y, int x1, int x2, ML_Color color)
{
	unsigned short *VRAM=(unsigned short *)PictAry[0];
	unsigned short *vram;
	unsigned short CGcolor;
	unsigned short backcolor=CB_BackColorIndex;
	int i,j;
	int k;
	if ( CB_ColorIndex >= 0 ) CGcolor=CB_ColorIndex; else CGcolor=CB_ColorIndexPlot;
//	if ( CB_G1MorG3M==1 ) {
//		if (x1&~127 || x2&~127 || y&~63) return;
//		k=3;
//	} else {
//		if ( ( x1<0 ) || ( 383<x1 ) || ( x2<0 ) || ( 383<x2 ) || ( y<0 ) || ( 191<y ) ) return;
//		k=1;
//	}
    if(x1 > x2)
    {
    	i = x1;
    	x1 = x2;
    	x2 = i;
    }
    if(x1 < 0) x1 = 0;
    if ( CB_G1MorG3M==1 ) {
	    if(y < -8) return;
	    if(y > 63) return;
		if(x2 > 127) x2 = 127;
		k=3;
    } else {
	    if(y < -24) return;
	    if(y > 191) return;
		if(x2 > 383) x2 = 383;
		k=1;
	}
    switch(color)
    {
		case ML_BLACK:
		  black:
		  	vram=VRAM+x1*k+(y*k+24)*384;
			if ( k==1 ) for ( i=x1; i<=x2; i++ ) *(vram++) = CGcolor;
			else {
				for ( i=x1*k; i<=x2*k+2; i++ ) *(vram++) = CGcolor;
		  		vram=VRAM+x1*k+(y*k+24+1)*384;
				for ( i=x1*k; i<=x2*k+2; i++ ) *(vram++) = CGcolor;
		  		vram=VRAM+x1*k+(y*k+24+2)*384;
				for ( i=x1*k; i<=x2*k+2; i++ ) *(vram++) = CGcolor;
			}
			break;
		case ML_WHITE:
		  white:
		  	vram=VRAM+x1*k+(y*k+24)*384;
			if ( k==1 ) for ( i=x1; i<=x2; i++ ) *(vram++) = backcolor;
			else {
				for ( i=x1*k; i<=x2*k+2; i++ ) *(vram++) = backcolor;
		  		vram=VRAM+x1*k+(y*k+24+1)*384;
				for ( i=x1*k; i<=x2*k+2; i++ ) *(vram++) = backcolor;
		  		vram=VRAM+x1*k+(y*k+24+2)*384;
				for ( i=x1*k; i<=x2*k+2; i++ ) *(vram++) = backcolor;
			}
			break;
		case ML_XOR:
		  	vram=VRAM+x1*k+(y*k+24)*384;
			if ( k==1 ) for ( i=x1; i<=x2; i++ ) *(vram++) ^=0xFFFF;
			else {
				for ( i=x1*k; i<=x2*k+2; i++ ) *(vram++) ^=0xFFFF;
		  		vram=VRAM+x1*k+(y*k+24+1)*384;
				for ( i=x1*k; i<=x2*k+2; i++ ) *(vram++) ^=0xFFFF;
		  		vram=VRAM+x1*k+(y*k+24+2)*384;
				for ( i=x1*k; i<=x2*k+2; i++ ) *(vram++) ^=0xFFFF;
			}
			break;
		case ML_CHECKER:
		  checker:
		  	for ( i=x1; i<=x2; i++ )	{
				if(y&1^i&1) ML_pixel(i, y, ML_BLACK);
				else ML_pixel(i, y, ML_WHITE);
			}
			break;
			
		case ML_RANDPX:
			for ( i=x1; i<=x2; i++ ) {
				if ( MLV_rand >= rand() ) ML_pixel(i, y, ML_BLACK );
			}
			break;
			
		case ML_ALPHA:
			for ( i=x1; i<=x2; i++ ) ML_pixel(i, y, ML_ALPHA );
			break;
			
		case ML_22DOT10:
				goto white;
			break;
		case ML_22DOT11:
			switch ( y&1 ) {
				case 0:
					goto checker;
				case 1:
					goto white;
			}
			break;
		case ML_22DOT12:
					goto checker;
			break;
		case ML_22DOT13:
			switch ( y&1 ) {
				case 0:
					goto black;
				case 1:
					goto checker;
			}
			break;
		case ML_22DOT14:
					goto black;
			break;
			
		case ML_22DOT20:
					ML_horizontal_line( y  , x1, x2, ML_WHITE);
					ML_horizontal_line( y+1, x1, x2, ML_WHITE);
			break;
		case ML_22DOT21:
			switch ( y&1 ) {
				case 0:
					ML_horizontal_line( y  , x1, x2, ML_CHECKER);
					ML_horizontal_line( y+1, x1, x2, ML_WHITE);
					break;
				case 1:
					ML_horizontal_line( y  , x1, x2, ML_WHITE);
					ML_horizontal_line( y+1, x1, x2, ML_CHECKER);
					break;
			}
			break;
		case ML_22DOT22:
					ML_horizontal_line( y  , x1, x2, ML_CHECKER);
					ML_horizontal_line( y+1, x1, x2, ML_CHECKER);
			break;
		case ML_22DOT23:
			switch ( y&1 ) {
				case 0:
					ML_horizontal_line( y  , x1, x2, ML_BLACK);
					ML_horizontal_line( y+1, x1, x2, ML_CHECKER);
					break;
				case 1:
					ML_horizontal_line( y  , x1, x2, ML_CHECKER);
					ML_horizontal_line( y+1, x1, x2, ML_BLACK);
					break;
			}
			break;
		case ML_22DOT24:
					ML_horizontal_line( y  , x1, x2, ML_BLACK);
					ML_horizontal_line( y+1, x1, x2, ML_BLACK);
			break;
    }
}

#endif

#ifdef ML_VERTICAL_LINE
/*
void ML_vertical_line(int x, int y1, int y2, ML_Color color){
	ML_line( x, y1, x, y2, color);
}
*/
void ML_vertical_line(int x, int y1, int y2, ML_Color color)
{
	unsigned short *VRAM=(unsigned short *)PictAry[0];
	unsigned short *vram;
	unsigned short CGcolor;
	unsigned short backcolor=CB_BackColorIndex;
	int i,j;
	int k;
	if ( CB_ColorIndex >= 0 ) CGcolor=CB_ColorIndex; else CGcolor=CB_ColorIndexPlot;
//	if ( CB_G1MorG3M==1 ) {
//		if (x&~127 || y1&~63 || y2&~63) return;
//		k=3;
//	} else {
//		if ( ( y1<0 ) || ( 191<y1 ) || ( y2<0 ) || ( 191<y2 ) || ( x<0 ) || ( 383<x ) ) return;
//		k=1;
//	}
    if(y1 > y2)
    {
    	i = y1;
    	y1 = y2;
    	y2 = i;
    }
	if( x < 0 ) return ;
    if ( CB_G1MorG3M==1 ) {
		if(x>127) return;
	    if(y1 < -8)  y1 = -8;
		if(y2 >  63) y2 =  63;
		k=3;
    } else {
		if(x>383) return;
	    if(y1 < -24) y1 = -24;
		if(y2 > 191) y2 = 191;
		k=1;
	}
    switch(color)
    {
		case ML_BLACK:
		  black:
		  	vram=VRAM+x*k+(y1*k+23)*384;
			if ( k==1 ) for ( i=y1; i<=y2; i++ ) *(vram+=384) = CGcolor;
			else {
				for ( i=y1*k; i<=y2*k+2; i++ ) *(vram+=384) = CGcolor;
		  		vram=VRAM+x*k+1+(y1*k+23)*384;
				for ( i=y1*k; i<=y2*k+2; i++ ) *(vram+=384) = CGcolor;
		  		vram=VRAM+x*k+2+(y1*k+23)*384;
				for ( i=y1*k; i<=y2*k+2; i++ ) *(vram+=384) = CGcolor;
			}
			break;
		case ML_WHITE:
		  white:
		  	vram=VRAM+x*k+(y1*k+23)*384;
			if ( k==1 ) for ( i=y1; i<=y2; i++ ) *(vram+=384) = backcolor;
			else {
				for ( i=y1*k; i<=y2*k+2; i++ ) *(vram+=384) = backcolor;
		  		vram=VRAM+x*k+1+(y1*k+23)*384;
				for ( i=y1*k; i<=y2*k+2; i++ ) *(vram+=384) = backcolor;
		  		vram=VRAM+x*k+2+(y1*k+23)*384;
				for ( i=y1*k; i<=y2*k+2; i++ ) *(vram+=384) = backcolor;
			}
			break;
		case ML_XOR:
		  	vram=VRAM+x*k+(y1*k+23)*384;
			if ( k==1 ) for ( i=y1; i<=y2; i++ ) *(vram+=384) ^=0xFFFF;
			else {
				for ( i=y1*k; i<=y2*k+2; i++ ) *(vram+=384) ^=0xFFFF;
		  		vram=VRAM+x*k+1+(y1*k+23)*384;
				for ( i=y1*k; i<=y2*k+2; i++ ) *(vram+=384) ^=0xFFFF;
		  		vram=VRAM+x*k+2+(y1*k+23)*384;
				for ( i=y1*k; i<=y2*k+2; i++ ) *(vram+=384) ^=0xFFFF;
			}
			break;
		case ML_CHECKER:
		  checker:
		  	for ( i=y1; i<=y2; i++ )	{
				if(x&1^i&1) ML_pixel(x, i, ML_BLACK);
				else ML_pixel(x, i, ML_WHITE);
			}
			break;
		case ML_RANDPX:
			for ( i=y1; i<=y2; i++ ) {
				if ( MLV_rand >= rand() ) ML_pixel(x, i, ML_BLACK );
			}
			break;
			
		case ML_ALPHA:
			for ( i=y1; i<=y2; i++ ) ML_pixel(x, i, ML_ALPHA );
			break;
			
		case ML_22DOT10:
					goto white;
			break;
		case ML_22DOT11:
			switch ( x&1 ) {
				case 0:
					goto checker;
					break;
				case 1:
					goto white;
					break;
			}
			break;
		case ML_22DOT12:
					goto checker;
			break;
		case ML_22DOT13:
			switch ( x&1 ) {
				case 0:
					goto black;
				case 1:
					goto checker;
			}
			break;
		case ML_22DOT14:
					goto black;
					break;

		case ML_22DOT20:
					ML_vertical_line( x  , y1, y2, ML_WHITE);
					ML_vertical_line( x+1, y1, y2, ML_WHITE);
			break;
		case ML_22DOT21:
			switch ( x&1 ) {
				case 0:
					ML_vertical_line( x  , y1, y2, ML_CHECKER);
					ML_vertical_line( x+1, y1, y2, ML_WHITE);
					break;
				case 1:
					ML_vertical_line( x  , y1, y2, ML_WHITE);
					ML_vertical_line( x+1, y1, y2, ML_CHECKER);
					break;
			}
			break;
		case ML_22DOT22:
					ML_vertical_line( x  , y1, y2, ML_CHECKER);
					ML_vertical_line( x+1, y1, y2, ML_CHECKER);
			break;
		case ML_22DOT23:
			switch ( x&1 ) {
				case 0:
					ML_vertical_line( x  , y1, y2, ML_BLACK);
					ML_vertical_line( x+1, y1, y2, ML_CHECKER);
					break;
				case 1:
					ML_vertical_line( x  , y1, y2, ML_CHECKER);
					ML_vertical_line( x+1, y1, y2, ML_BLACK);
					break;
			}
			break;
		case ML_22DOT24:
					ML_vertical_line( x  , y1, y2, ML_BLACK);
					ML_vertical_line( x+1, y1, y2, ML_BLACK);
					break;
	}
}
#endif

#ifdef ML_RECTANGLE
void ML_rectangle_FX(int x1, int y1, int x2, int y2, int border_width, ML_Color border_color, ML_Color fill_color, int RGBcolor, int RGBbackcolor ){
	int	tmp=CB_G1MorG3M;
	int tmp2=CB_ColorIndex;
	int tmp3=CB_BackColorIndex;
	CB_G1MorG3M=1;
	CB_ColorIndex=RGBcolor;
	CB_BackColorIndex=RGBbackcolor;
	ML_rectangle( x1, y1, x2, y2, border_width, border_color, fill_color );
	CB_G1MorG3M=tmp;
	CB_ColorIndex=tmp2;
	CB_BackColorIndex=tmp3;
}
void ML_rectangle_CG(int x1, int y1, int x2, int y2, int border_width, ML_Color border_color, ML_Color fill_color, int RGBcolor, int RGBbackcolor ){
	int	tmp=CB_G1MorG3M;
	int tmp2=CB_ColorIndex;
	int tmp3=CB_BackColorIndex;
	CB_G1MorG3M=3;
	CB_ColorIndex=RGBcolor;
	CB_BackColorIndex=RGBbackcolor;
	ML_rectangle( x1, y1, x2, y2, border_width, border_color, fill_color );
	CB_G1MorG3M=tmp;
	CB_ColorIndex=tmp2;
	CB_BackColorIndex=tmp3;
}
void ML_rectangle(int x1, int y1, int x2, int y2, int border_width, ML_Color border_color, ML_Color fill_color)
{
	int i;
	if(x1 > x2)
	{
		i = x1;
		x1 = x2;
		x2 = i;
	}
	if(y1 > y2)
	{
		i = y1;
		y1 = y2;
		y2 = i;
	}
	if(border_width > (x2-x1)/2+1) border_width = (x2-x1)/2+1;
	if(border_width > (y2-y1)/2+1) border_width = (y2-y1)/2+1;
	if(border_color != ML_TRANSPARENT && border_width > 0)
	{
		for(i=0 ; i<border_width ; i++)
		{
			ML_horizontal_line(y1+i, x1, x2, border_color);
			ML_horizontal_line(y2-i, x1, x2, border_color);
		}
		for(i=y1+border_width ; i<=y2-border_width ; i++)
		{
			ML_horizontal_line(i, x1, x1+border_width-1, border_color);
			ML_horizontal_line(i, x2-border_width+1, x2, border_color);
		}
	}
	if(fill_color != ML_TRANSPARENT)
	{
		for(i=y1+border_width ; i<=y2-border_width ; i++)
			ML_horizontal_line(i, x1+border_width, x2-border_width, fill_color);
	}
}

//------------------------------------------------------------------- Rotate process
void ML_angle( int angle, int *sinus, int *cosinus ) {
	/*
    angle %= 360;
    if (angle < 0) angle += 360;
    if (angle < 0) angle += 360;
    if (angle == 0) { *cosinus = 16384*4; *sinus = 0;}
    else if (angle == 90) { *cosinus = 0; *sinus = -16384*4;}
    else if (angle == 180) { *cosinus = -16384*4; *sinus = 0;}
    else if (angle == 270) { *cosinus = 0; *sinus = 16384*4;}
    else
    {
        *cosinus = (cos(-3.14159265359 * (double)angle / 180.0) ) * 16384*4;
        *sinus   = (sin(-3.14159265359 * (double)angle / 180.0) ) * 16384*4;
    }
    */
    *cosinus = icos( angle, 1024 ) << 6;
    *sinus   = isin( angle, 1024 ) << 6;

}
void ML_pixel_Rotate( int x, int y, int dx, int dy, int sinus, int cosinus, ML_Color color ){
//x' = x*cos(θ) - y*sin(θ) 
//y' = x*sin(θ) + y*cos(θ) 
	int nx,ny;
	nx = ( dx*cosinus - dy*sinus ) >>16;
	ny = ( dx*sinus + dy*cosinus ) >>16;
	ML_pixel( x+nx, y+ny, color);
}

void ML_horizontal_line_Rotate( int y, int dy, int x, int dx,  int sinus, int cosinus, ML_Color color ){
//x' = x*cos(θ) - y*sin(θ) 
//y' = x*sin(θ) + y*cos(θ) 
	int nx1,ny1,nx2,ny2;
	nx1 = ( ( -dx*cosinus - dy*sinus ) >>16 ) + x;
	ny1 = ( ( -dx*sinus + dy*cosinus ) >>16 ) + y;
	nx2 = ( ( +dx*cosinus - dy*sinus ) >>16 ) + x;
	ny2 = ( ( +dx*sinus + dy*cosinus ) >>16 ) + y;
	ML_line( nx1, ny1, nx2, ny2, color);
}

void ML_Rotate( int x, int y, int *dx, int *dy, int sinus, int cosinus ){
//x' = x*cos(θ) - y*sin(θ) 
//y' = x*sin(θ) + y*cos(θ) 
	int nx,ny;
	nx = ( ( (*dx)*cosinus - (*dy)*sinus ) >>16) + x;
	ny = ( ( (*dx)*sinus + (*dy)*cosinus ) >>16) + y;
	*dx = nx;
	*dy = ny;
}

void ML_polygon_Rotate( int *x, int *y, int nb_vertices, ML_Color color, int angle , int center_x, int center_y, int percent, int fill ) {	// flag 1:fill
    int i,sx,sy;
	int ax[128],ay[128];
    int cosinus, sinus;
	if ( nb_vertices>(128-1) ) return ;
	if ( nb_vertices < 1 ) return;
	if ( percent == 0 ) return ;
	ML_angle( angle, &sinus, &cosinus );
	if ( ( center_x==-2147483648 ) || ( center_y==-2147483648 ) ) {
		sx=0; sy=0;
		for(i=0 ; i<nb_vertices ; i++) { sx+=x[i]; sy+=y[i]; }
		center_x=sx/nb_vertices;
		center_y=sy/nb_vertices;
	}
	for(i=0 ; i<nb_vertices ; i++) {
		ax[i]=x[i]-center_x;
		ay[i]=y[i]-center_y;
		if ( percent != 100 ) {
			ax[i] = ax[i]*percent/100;
			ay[i] = ay[i]*percent/100;
		}
		ML_Rotate( center_x, center_y, &ax[i], &ay[i], sinus, cosinus );
		
	}
	if ( fill ) ML_filled_polygon( (const int *)ax, (const int *)ay, nb_vertices, color);
	else 		       ML_polygon( (const int *)ax, (const int *)ay, nb_vertices, color);
}
void ML_box_Rotate( int x1, int y1, int x2, int y2, int center_x, int center_y, int sinus, int cosinus, ML_Color color, int fill ) {	// flag 1:fill
    int ax[4],ay[4];
	ax[0]=x1-center_x;
	ay[0]=y1-center_y;
	ML_Rotate( center_x, center_y, &ax[0], &ay[0], sinus, cosinus );
	ax[1]=x2-center_x;
	ay[1]=y1-center_y;
	ML_Rotate( center_x, center_y, &ax[1], &ay[1], sinus, cosinus );
	ax[2]=x2-center_x;
	ay[2]=y2-center_y;
	ML_Rotate( center_x, center_y, &ax[2], &ay[2], sinus, cosinus );
	ax[3]=x1-center_x;
	ay[3]=y2-center_y;
	ML_Rotate( center_x, center_y, &ax[3], &ay[3], sinus, cosinus );
	if ( fill ) ML_filled_polygon( (const int *)ax, (const int *)ay, 4, color);
	else 		       ML_polygon( (const int *)ax, (const int *)ay, 4, color);
}

void ML_rectangle_Rotate(int x1, int y1, int x2, int y2, int border_width, ML_Color border_color, ML_Color fill_color, int angle , int center_x, int center_y, int percent ){
	int i;

	int x,y;
    int cosinus, sinus;

	ML_angle( angle, &sinus, &cosinus );

	if(x1 > x2)
	{
		i = x1;
		x1 = x2;
		x2 = i;
	}
	if(y1 > y2)
	{
		i = y1;
		y1 = y2;
		y2 = i;
	}
	
	if ( ( center_x==-2147483648 ) || ( center_y==-2147483648 ) ) {
		center_x=(x1+x2)/2;
		center_y=(y1+y2)/2;
	}

	if ( percent != 100 ) {
			if ( percent == 0 ) return ;
			x1 = (x1-center_x)*percent/100 +center_x;
			x2 = (x2-center_x)*percent/100 +center_x;
			y1 = (y1-center_y)*percent/100 +center_y;
			y2 = (y2-center_y)*percent/100 +center_y;
	}
	
	if(border_width > (x2-x1)/2+1) border_width = (x2-x1)/2+1;
	if(border_width > (y2-y1)/2+1) border_width = (y2-y1)/2+1;
	if(border_color != ML_TRANSPARENT && border_width > 0) {
		if ( border_width == 1 ) {
			ML_box_Rotate( x1, y1, x2, y2, center_x, center_y, sinus, cosinus, border_color, 0 );	//	BOX
		} else {
			ML_box_Rotate( x1, y1, x2, y1+border_width, center_x, center_y, sinus, cosinus, border_color, 1 );	// filled		// upper
			ML_box_Rotate( x1, y2, x2, y2-border_width, center_x, center_y, sinus, cosinus, border_color, 1 );	// filled		// down
			ML_box_Rotate( x1, y1, x1+border_width, y2, center_x, center_y, sinus, cosinus, border_color, 1 );	// filled		// left
			ML_box_Rotate( x2, y1, x2-border_width, y2, center_x, center_y, sinus, cosinus, border_color, 1 );	// filled		// right

		}
	}
	if(fill_color != ML_TRANSPARENT) {
		ML_box_Rotate( x1+border_width, y1+border_width, x2-border_width, y2-border_width, center_x, center_y, sinus, cosinus, fill_color, 1 );	// filled
	}
}
#endif

#ifdef ML_POLYGON
void ML_polygon(const int *x, const int *y, int nb_vertices, ML_Color color)
{
	int i;
	if(nb_vertices < 1) return;
	for(i=0 ; i<nb_vertices-1 ; i++)
		ML_line(x[i], y[i], x[i+1], y[i+1], color);
	ML_line(x[i], y[i], x[0], y[0], color);
}
#endif

#ifdef ML_FILLED_POLYGON
static int ML_filled_polygon_quicksord_partition(int *t, int p, int r)
{
	int i, j, x, tmp;
	j = p - 1;
	x = t[r];
	for(i=p ; i<r ; i++)
	{
		if(x > t[i])
		{
			j++;
			tmp = t[j];
			t[j] = t[i];
			t[i] = tmp;
		}
	}
	t[r] = t[j+1];
	t[j+1] = x;
	return j + 1;
}

static void ML_filled_polygon_quicksord(int* t, int p, int r)
{
	int q;
	if(p < r)
	{
		q = ML_filled_polygon_quicksord_partition(t, p, r);
		ML_filled_polygon_quicksord(t, p, q-1);
		ML_filled_polygon_quicksord(t, q+1, r);
	}
}


void ML_filled_polygon(const int *x, const int *y, int nb_vertices, ML_Color color)
{
	int i, j, dx, dy, ymin, ymax;
	int cut_in_line[128+1], nb_cut;
	if( (nb_vertices < 3) || ( 128 < nb_vertices ) ) return ;
	ymin = ymax = y[0];
	for(i=1 ; i<nb_vertices ; i++)
	{
		if(y[i] < ymin) ymin = y[i];
		if(y[i] > ymax) ymax = y[i];
	}
	for(i=ymin ; i<=ymax ; i++)
	{
		nb_cut = 0;
		for(j=0 ; j<nb_vertices ; j++)
		{
			if((y[j]<=i && y[(j+1)%nb_vertices]>=i) || (y[j]>=i && y[(j+1)%nb_vertices]<=i))
			{
				dy = abs(y[j]-y[(j+1)%nb_vertices]);
				if(dy)
				{
					dx = x[(j+1)%nb_vertices]-x[j];
					cut_in_line[nb_cut] = x[j] + rnd(abs(i-y[j]+sgn(i-y[j])/2)*dx/dy);
					nb_cut++;
				}
			}
		}
		ML_filled_polygon_quicksord(cut_in_line, 0, nb_cut-1);
		j = 0;
		while(j<nb_cut-2 && cut_in_line[j]==cut_in_line[j+1]) j++;
		while(j < nb_cut)
		{
			if(j == nb_cut-1) ML_horizontal_line(i, cut_in_line[j-1]+1, cut_in_line[j], color);
			else
			{
				dx = 1;
				while(j+dx<nb_cut-1 && cut_in_line[j+dx]==cut_in_line[j+dx+1]) dx++;
				ML_horizontal_line(i, cut_in_line[j], cut_in_line[j+dx], color);
				j += dx;
			}
			j++;
		}
	}
//	free(cut_in_line);
}
#endif

#ifdef ML_CIRCLE
void ML_circle(int x, int y, int radius, ML_Color color)
{
	int plot_x, plot_y, d;

	if(radius < 0) return;
	plot_x = 0;
	plot_y = radius;
	d = 1 - radius;

	ML_pixel(x, y+plot_y, color);
	if(radius)
	{
		ML_pixel(x, y-plot_y, color);
		ML_pixel(x+plot_y, y, color);
		ML_pixel(x-plot_y, y, color);
	}
	while(plot_y > plot_x)
	{
		if(d < 0)
			d += 2*plot_x+3;
		else
		{
			d += 2*(plot_x-plot_y)+5;
			plot_y--;
		}
		plot_x++;
		if(plot_y >= plot_x)
		{
			ML_pixel(x+plot_x, y+plot_y, color);
			ML_pixel(x-plot_x, y+plot_y, color);
			ML_pixel(x+plot_x, y-plot_y, color);
			ML_pixel(x-plot_x, y-plot_y, color);
		}
		if(plot_y > plot_x)
		{
			ML_pixel(x+plot_y, y+plot_x, color);
			ML_pixel(x-plot_y, y+plot_x, color);
			ML_pixel(x+plot_y, y-plot_x, color);
			ML_pixel(x-plot_y, y-plot_x, color);
		}
	}
}

#endif

#ifdef ML_FILLED_CIRCLE
void ML_filled_circle(int x, int y, int radius, ML_Color color)
{
	int plot_x, plot_y, d;

	if(radius < 0) return;
	plot_x = 0;
	plot_y = radius;
	d = 1 - radius;

	ML_horizontal_line(y, x-plot_y, x+plot_y, color);
	while(plot_y > plot_x)
	{
		if(d < 0)
			d += 2*plot_x+3;
		else {
			d += 2*(plot_x-plot_y)+5;
			plot_y--;
			ML_horizontal_line(y+plot_y+1, x-plot_x, x+plot_x, color);
			ML_horizontal_line(y-plot_y-1, x-plot_x, x+plot_x, color);
		}
		plot_x++;
		if(plot_y >= plot_x)
		{
			ML_horizontal_line(y+plot_x, x-plot_y, x+plot_y, color);
			ML_horizontal_line(y-plot_x, x-plot_y, x+plot_y, color);
		}
	}
}
#endif

#ifdef ML_ELLIPSE
/*
void ML_ellipse(int x, int y, int radius1, int radius2, ML_Color color)
{
	int plot_x, plot_y;
	float d1, d2;
	if(radius1 < 1 || radius2 < 1) return;
	plot_x = 0;
	plot_y = radius2;
	d1 = radius2*radius2 - radius1*radius1*radius2 + radius1*radius1/4;
	ML_pixel(x, y+plot_y, color);
	ML_pixel(x, y-plot_y, color);
	while(radius1*radius1*(plot_y-.5) > radius2*radius2*(plot_x+1))
	{
		if(d1 < 0)
		{
			d1 += radius2*radius2*(2*plot_x+3);
			plot_x++;
		} else {
			d1 += radius2*radius2*(2*plot_x+3) + radius1*radius1*(-2*plot_y+2);
			plot_x++;
			plot_y--;
		}
		ML_pixel(x+plot_x, y+plot_y, color);
		ML_pixel(x-plot_x, y+plot_y, color);
		ML_pixel(x+plot_x, y-plot_y, color);
		ML_pixel(x-plot_x, y-plot_y, color);
	}
	d2 = radius2*radius2*(plot_x+.5)*(plot_x+.5) + radius1*radius1*(plot_y-1)*(plot_y-1) - radius1*radius1*radius2*radius2;
	while(plot_y > 0)
	{
		if(d2 < 0)
		{
			d2 += radius2*radius2*(2*plot_x+2) + radius1*radius1*(-2*plot_y+3);
			plot_y--;
			plot_x++;
		} else {
			d2 += radius1*radius1*(-2*plot_y+3);
			plot_y--;
		}
		ML_pixel(x+plot_x, y+plot_y, color);
		ML_pixel(x-plot_x, y+plot_y, color);
		if(plot_y > 0)
		{
			ML_pixel(x+plot_x, y-plot_y, color);
			ML_pixel(x-plot_x, y-plot_y, color);
		}
	}
}
*/

void ML_ellipse(int x, int y, int radius1, int radius2, ML_Color color, int angle )
{
	int plot_x, plot_y;
	float d1, d2;
    int cosinus, sinus;
	if(radius1 < 1 || radius2 < 1) return;
	plot_x = 0;
	plot_y = radius2;
	d1 = radius2*radius2 - radius1*radius1*radius2 + radius1*radius1/4;

	ML_angle( angle, &sinus, &cosinus );

	ML_pixel_Rotate( x, y, 0,  plot_y, sinus, cosinus, color );
	ML_pixel_Rotate( x, y, 0, -plot_y, sinus, cosinus, color );
	while(radius1*radius1*(plot_y-.5) > radius2*radius2*(plot_x+1))
	{
		if(d1 < 0)
		{
			d1 += radius2*radius2*(2*plot_x+3);
			plot_x++;
		} else {
			d1 += radius2*radius2*(2*plot_x+3) + radius1*radius1*(-2*plot_y+2);
			plot_x++;
			plot_y--;
		}
		ML_pixel_Rotate( x, y,  plot_x,  plot_y, sinus, cosinus, color );
		ML_pixel_Rotate( x, y, -plot_x,  plot_y, sinus, cosinus, color );
		ML_pixel_Rotate( x, y,  plot_x, -plot_y, sinus, cosinus, color );
		ML_pixel_Rotate( x, y, -plot_x, -plot_y, sinus, cosinus, color );
	}
	d2 = radius2*radius2*(plot_x+.5)*(plot_x+.5) + radius1*radius1*(plot_y-1)*(plot_y-1) - radius1*radius1*radius2*radius2;
	while(plot_y > 0)
	{
		if(d2 < 0)
		{
			d2 += radius2*radius2*(2*plot_x+2) + radius1*radius1*(-2*plot_y+3);
			plot_y--;
			plot_x++;
		} else {
			d2 += radius1*radius1*(-2*plot_y+3);
			plot_y--;
		}
		ML_pixel_Rotate( x, y,  plot_x,  plot_y, sinus, cosinus, color );
		ML_pixel_Rotate( x, y, -plot_x,  plot_y, sinus, cosinus, color );
		if(plot_y > 0)
		{
			ML_pixel_Rotate( x, y,  plot_x, -plot_y, sinus, cosinus, color );
			ML_pixel_Rotate( x, y, -plot_x, -plot_y, sinus, cosinus, color );
		}
	}
}

#endif

#ifdef ML_ELLIPSE_IN_RECT
void ML_ellipse_in_rect(int x1, int y1, int x2, int y2, ML_Color color, int angle)
{
	int radius1, radius2;
	if(x1 > x2)
	{
		int tmp = x1;
		x1 = x2;
		x2 = tmp;
	}
	if(y1 > y2)
	{
		int tmp = y1;
		y1 = y2;
		y2 = tmp;
	}
	radius1 = (x2-x1)/2;
	radius2 = (y2-y1)/2;
	ML_ellipse(x1+radius1, y1+radius2, radius1, radius2, color, angle);
}
#endif

#ifdef ML_FILLED_ELLIPSE
/*
void ML_filled_ellipse(int x, int y, int radius1, int radius2, ML_Color color)
{
	int plot_x, plot_y;
	float d1, d2;
	if(radius1 < 1 || radius2 < 1) return;
	plot_x = 0;
	plot_y = radius2;
	d1 = radius2*radius2 - radius1*radius1*radius2 + radius1*radius1/4;
	while(radius1*radius1*(plot_y-.5) > radius2*radius2*(plot_x+1))
	{
		if(d1 < 0)
		{
			d1 += radius2*radius2*(2*plot_x+3);
			plot_x++;
		} else {
			d1 += radius2*radius2*(2*plot_x+3) + radius1*radius1*(-2*plot_y+2);
			ML_horizontal_line(y+plot_y, x-plot_x, x+plot_x, color);
			ML_horizontal_line(y-plot_y, x-plot_x, x+plot_x, color);
			plot_x++;
			plot_y--;
		}
	}
	ML_horizontal_line(y+plot_y, x-plot_x, x+plot_x, color);
	ML_horizontal_line(y-plot_y, x-plot_x, x+plot_x, color);
	d2 = radius2*radius2*(plot_x+.5)*(plot_x+.5) + radius1*radius1*(plot_y-1)*(plot_y-1) - radius1*radius1*radius2*radius2;
	while(plot_y > 0)
	{
		if(d2 < 0)
		{
			d2 += radius2*radius2*(2*plot_x+2) + radius1*radius1*(-2*plot_y+3);
			plot_y--;
			plot_x++;
		} else {
			d2 += radius1*radius1*(-2*plot_y+3);
			plot_y--;
		}
		ML_horizontal_line(y+plot_y, x-plot_x, x+plot_x, color);
		if(plot_y > 0)
			ML_horizontal_line(y-plot_y, x-plot_x, x+plot_x, color);
	}
}
*/
void ML_filled_ellipse(int x, int y, int radius1, int radius2, ML_Color color, int angle)
{
	int plot_x, plot_y;
	float d1, d2;
    int cosinus, sinus;
	ML_angle( angle, &sinus, &cosinus );
	if(radius1 < 1 || radius2 < 1) return;
	plot_x = 0;
	plot_y = radius2;
	d1 = radius2*radius2 - radius1*radius1*radius2 + radius1*radius1/4;
	while(radius1*radius1*(plot_y-.5) > radius2*radius2*(plot_x+1))
	{
		if(d1 < 0)
		{
			d1 += radius2*radius2*(2*plot_x+3);
			plot_x++;
		} else {
			d1 += radius2*radius2*(2*plot_x+3) + radius1*radius1*(-2*plot_y+2);
			ML_horizontal_line_Rotate( y, -plot_y, x, plot_x, sinus, cosinus, color );
			ML_horizontal_line_Rotate( y, +plot_y, x, plot_x, sinus, cosinus, color );
			plot_x++;
			plot_y--;
		}
	}
	ML_horizontal_line_Rotate( y, +plot_y, x, plot_x, sinus, cosinus, color );
	ML_horizontal_line_Rotate( y, -plot_y, x, plot_x, sinus, cosinus, color );
	d2 = radius2*radius2*(plot_x+.5)*(plot_x+.5) + radius1*radius1*(plot_y-1)*(plot_y-1) - radius1*radius1*radius2*radius2;
	while(plot_y > 0)
	{
		if(d2 < 0)
		{
			d2 += radius2*radius2*(2*plot_x+2) + radius1*radius1*(-2*plot_y+3);
			plot_y--;
			plot_x++;
		} else {
			d2 += radius1*radius1*(-2*plot_y+3);
			plot_y--;
		}
		ML_horizontal_line_Rotate( y, +plot_y, x, plot_x, sinus, cosinus, color );
		if(plot_y > 0)
			ML_horizontal_line_Rotate( y, -plot_y, x, plot_x, sinus, cosinus, color );
	}
}
#endif

#ifdef ML_FILLED_ELLIPSE_IN_RECT
void ML_filled_ellipse_in_rect(int x1, int y1, int x2, int y2, ML_Color color, int angle)
{
	int radius1, radius2;
	if(x1 > x2)
	{
		int tmp = x1;
		x1 = x2;
		x2 = tmp;
	}
	if(y1 > y2)
	{
		int tmp = y1;
		y1 = y2;
		y2 = tmp;
	}
	radius1 = (x2-x1)/2;
	radius2 = (y2-y1)/2;
	ML_filled_ellipse(x1+radius1, y1+radius2, radius1, radius2, color, angle);
}
#endif

#ifdef ML_HORIZONTAL_SCROLL
void ML_horizontal_scroll(int scroll)
{
	ML_horizontal_scroll2( scroll, 0, 0, ScreenWidth, ScreenHeight);
}

void ML_horizontal_scroll2(int scroll, int x1, int y1, int x2, int y2)
{
	int i, j;
	int height,width;
	int tmp,ptr;
	unsigned short line[384], *vram = (unsigned short *)PictAry[0];
	unsigned short *srcPtr,*dstPtr;

	if ( CB_G1MorG3M==1 ) {
		x1*=3;
		y1*=3;
		x2*=3;
		y2*=3;
		x2+=2;
		y2+=2;
		scroll*=3;
	}
	if(y1 > y2) {
		tmp = y1;
		y1 = y2;
		y2 = tmp;
	}
	if(y1 < 0) y1 = 0;
	if(y2 > 191) y2 = 191;
	height=y2-y1+1;
	
	if(x1 > x2) {
		tmp = x1;
		x1 = x2;
		x2 = tmp;
	}
	if(x1 < 0) x1 = 0;
	if(x2 > 383) x2 = 383;
	width=x2-x1+1;

	if ( ( x1<0 ) || ( 384<x1 ) || ( y1<0 ) || ( 191<y1 ) ) return;
	if ( ( x2<0 ) || ( 384<x1 ) || ( y2<0 ) || ( 191<y2 ) ) return;
	y1+=24;
	y2+=24;

	scroll %= width;

	if ( scroll > 0 ) {	// >>>
	//  >>>>>>>>>>>>++
	//	++>>>>>>>>>>>>
		for(j=y1 ; j<=y2 ; j++) {	// 
			srcPtr = vram+j*384+x2;
			dstPtr = srcPtr;
			for(i=0; i<      scroll; i++) line[i]   = *srcPtr--;
			for(i=0; i<width-scroll; i++) *dstPtr-- = *srcPtr--;
			for(i=0; i<      scroll; i++) *dstPtr-- = line[i];
		}
	} else {			// <<<
	//	++<<<<<<<<<<<<
	//  <<<<<<<<<<<<++
		scroll = -scroll;
		for(j=y1 ; j<=y2 ; j++) {	// 
			srcPtr = vram+j*384+x1;
			dstPtr = srcPtr;
			for(i=0; i<      scroll; i++) line[i]   = *srcPtr++;
			for(i=0; i<width-scroll; i++) *dstPtr++ = *srcPtr++;
			for(i=0; i<      scroll; i++) *dstPtr++ = line[i];
		}
	}
/*	
	for(j=y1 ; j<=y2 ; j++) {	// 
			ptr=j*384+x1;
			for(i=x1 ; i<=x2 ; i++) line[i] = vram[ptr++];
			for(i=x1 ; i<=x2 ; i++) {
				tmp = i - scroll;
				if ( tmp < x1 ) tmp += width;
				if ( tmp > x2 ) tmp -= width;
				vram[j*384+i] = line[tmp];
			}
	}
*/
}

#endif

#ifdef ML_VERTICAL_SCROLL
void ML_vertical_scroll(int scroll)
{
	ML_vertical_scroll2( scroll, 0, 0, ScreenWidth, ScreenHeight);
}

void ML_vertical_scroll2(int scroll, int x1, int y1, int x2, int y2)
{
	int i, j;
	int height,width;
	int tmp,ptr;
	unsigned short column[216], *vram = (unsigned short *)PictAry[0];
	unsigned short *srcPtr,*dstPtr;
	
	if ( CB_G1MorG3M==1 ) {
		x1*=3;
		y1*=3;
		x2*=3;
		y2*=3;
		x2+=2;
		y2+=2;
		scroll*=3;
	}
	if(y1 > y2) {
		tmp = y1;
		y1 = y2;
		y2 = tmp;
	}
	if(y1 < 0) y1 = 0;
	if(y2 > 191) y2 = 191;
	height=y2-y1+1;
	
	if(x1 > x2) {
		tmp = x1;
		x1 = x2;
		x2 = tmp;
	}
	if(x1 < 0) x1 = 0;
	if(x2 > 383) x2 = 383;
	width=x2-x1+1;

	if ( ( x1<0 ) || ( 384<x1 ) || ( y1<-24 ) || ( 191<y1 ) ) return;
	if ( ( x2<0 ) || ( 384<x1 ) || ( y2<-24 ) || ( 191<y2 ) ) return;
	y1+=24;
	y2+=24;
	
	scroll %= height;

	if ( scroll > 0 ) {	// >>>
	//  >>>>>>>>>>>>++
	//	++>>>>>>>>>>>>
		for(i=x1 ; i<=x2 ; i++) {	// 
			srcPtr = vram+y2*384+i;
			dstPtr = srcPtr;
			for(j=0; j<       scroll; j++) { column[j] = *srcPtr; srcPtr-=384; }
			for(j=0; j<height-scroll; j++) { *dstPtr   = *srcPtr; srcPtr-=384; dstPtr-=384; }
			for(j=0; j<       scroll; j++) { *dstPtr   = column[j];			   dstPtr-=384; }
		}
	} else {			// <<<
	//	++<<<<<<<<<<<<
	//  <<<<<<<<<<<<++
		scroll = -scroll;
		for(i=x1 ; i<=x2 ; i++) {	// 
			srcPtr = vram+y1*384+i;
			dstPtr = srcPtr;
			for(j=0; j<       scroll; j++) { column[j] = *srcPtr; srcPtr+=384; }
			for(j=0; j<height-scroll; j++) { *dstPtr   = *srcPtr; srcPtr+=384; dstPtr+=384; }
			for(j=0; j<       scroll; j++) { *dstPtr   = column[j];			   dstPtr+=384; }
		}
	}
/*
	for(i=x1 ; i<=x2 ; i++) {	// center byte vscroll
			ptr=j*384+y1;
			for(j=y1 ; j<=y2 ; j++) { column[j] = vram[ptr]; ptr+=384;}
			for(j=y1 ; j<=y2 ; j++) {
				tmp = j - scroll;
				if ( tmp < y1 ) tmp += height;
				if ( tmp > y2 ) tmp -= height;
				vram[j*384+i] = column[tmp];
			}
	}
*/
}
#endif


#ifdef ML_BMP_OR
void ML_bmp_sub(const unsigned char *bmp, int x, int y, int width, int height, int kind ){
	DISPGRAPH Gpict;
	Gpict.x =   x; 
	Gpict.y =   y; 
	Gpict.GraphData.width   = width;
	Gpict.GraphData.height  = height;
	Gpict.GraphData.pBitmap = (unsigned char*)bmp;
	Gpict.WriteModify = IMB_WRITEMODIFY_NORMAL; 
	Gpict.WriteKind   = kind & 0xFF;
	if ( kind >> 8 ) Bdisp_WriteGraph_VRAM(&Gpict);
	else 			 Bdisp_WriteGraph_VRAM_CG_1bit(&Gpict);
}
void ML_bmp_or(const unsigned char *bmp, int x, int y, int width, int height, int flag ){
	ML_bmp_sub( bmp, x, y, width, height, (flag<<8) | IMB_WRITEKIND_OR );
}
void ML_bmp_over(const unsigned char *bmp, int x, int y, int width, int height, int flag ){
	ML_bmp_sub( bmp, x, y, width, height, (flag<<8) | IMB_WRITEKIND_OVER );
}
#endif

#ifdef ML_BMP_AND
void ML_bmp_and(const unsigned char *bmp, int x, int y, int width, int height, int flag ){
	ML_bmp_sub( bmp, x, y, width, height, (flag<<8) | IMB_WRITEKIND_AND );
}
#endif

#ifdef ML_BMP_XOR
void ML_bmp_xor(const unsigned char *bmp, int x, int y, int width, int height, int flag ){
	ML_bmp_sub( bmp, x, y, width, height, (flag<<8) | IMB_WRITEKIND_XOR );
}
#endif

#ifdef ML_BMP_OR_CL
void ML_bmp_or_cl(const unsigned char *bmp, int x, int y, int width, int height, int flag ){
	ML_bmp_sub( bmp, x, y, width, height, (flag<<8) | IMB_WRITEKIND_OR );
}
#endif

#ifdef ML_BMP_AND_CL
void ML_bmp_and_cl(const unsigned char *bmp, int x, int y, int width, int height, int flag ){
	ML_bmp_sub( bmp, x, y, width, height, (flag<<8) | IMB_WRITEKIND_AND );
}
#endif

#ifdef ML_BMP_XOR_CL
void ML_bmp_xor_cl(const unsigned char *bmp, int x, int y, int width, int height, int flag ){
	ML_bmp_sub( bmp, x, y, width, height, (flag<<8) | IMB_WRITEKIND_XOR );
}
#endif

#ifdef ML_BMP_8_OR
void ML_bmp_8_sub(const unsigned char *bmp, int x, int y, int kind ){
	DISPGRAPH Gpict;
	Gpict.x =   x; 
	Gpict.y =   y; 
	Gpict.GraphData.width   = 8;
	Gpict.GraphData.height  = 8;
	Gpict.GraphData.pBitmap = (unsigned char*)bmp;
	Gpict.WriteModify = IMB_WRITEMODIFY_NORMAL; 
	Gpict.WriteKind   = kind & 0xFF;
	if ( kind >> 8 ) Bdisp_WriteGraph_VRAM(&Gpict);
	else 			 Bdisp_WriteGraph_VRAM_CG_1bit(&Gpict);
}
void ML_bmp_8_over(const unsigned char *bmp, int x, int y, int flag ){
	ML_bmp_8_sub( bmp, x, y, (flag<<8) | IMB_WRITEKIND_OVER );
}
void ML_bmp_8_or(const unsigned char *bmp, int x, int y, int flag ){
	ML_bmp_8_sub( bmp, x, y, (flag<<8) | IMB_WRITEKIND_OR );
}
#endif

#ifdef ML_BMP_8_AND
void ML_bmp_8_and(const unsigned char *bmp, int x, int y, int flag ){
	ML_bmp_8_sub( bmp, x, y, (flag<<8) | IMB_WRITEKIND_AND );
}
#endif

#ifdef ML_BMP_8_XOR
void ML_bmp_8_xor(const unsigned char *bmp, int x, int y, int flag ){
	ML_bmp_8_sub( bmp, x, y, (flag<<8) | IMB_WRITEKIND_XOR );
}
#endif

#ifdef ML_BMP_8_OR_CL
void ML_bmp_8_or_cl(const unsigned char *bmp, int x, int y, int flag ){
	ML_bmp_8_sub( bmp, x, y, (flag<<8) | IMB_WRITEKIND_OR );
}
#endif

#ifdef ML_BMP_8_AND_CL
void ML_bmp_8_and_cl(const unsigned char *bmp, int x, int y, int flag ){
	ML_bmp_8_sub( bmp, x, y, (flag<<8) | IMB_WRITEKIND_AND );
}
#endif

#ifdef ML_BMP_8_XOR_CL
void ML_bmp_8_xor_cl(const unsigned char *bmp, int x, int y, int flag ){
	ML_bmp_8_sub( bmp, x, y, (flag<<8) | IMB_WRITEKIND_XOR );
}
#endif

#ifdef ML_BMP_16_OR
void ML_bmp_16_sub(const unsigned short *bmp, int x, int y, int kind ){
	DISPGRAPH Gpict;
	Gpict.x =   x; 
	Gpict.y =   y; 
	Gpict.GraphData.width   = 16;
	Gpict.GraphData.height  = 16;
	Gpict.GraphData.pBitmap = (unsigned char*)bmp;
	Gpict.WriteModify = IMB_WRITEMODIFY_NORMAL; 
	Gpict.WriteKind   = kind & 0xFF;
	if ( kind >> 8 ) Bdisp_WriteGraph_VRAM(&Gpict);
	else 			 Bdisp_WriteGraph_VRAM_CG_1bit(&Gpict);
}
void ML_bmp_16_or(const unsigned short *bmp, int x, int y, int flag ){
	ML_bmp_16_sub( bmp, x, y, (flag<<8) | IMB_WRITEKIND_OR );
}
void ML_bmp_16_over(const unsigned short *bmp, int x, int y, int flag ){
	ML_bmp_16_sub( bmp, x, y, (flag<<8) | IMB_WRITEKIND_OVER );
}
#endif

#ifdef ML_BMP_16_AND
void ML_bmp_16_and(const unsigned short *bmp, int x, int y, int flag ){
	ML_bmp_16_sub( bmp, x, y, (flag<<8) | IMB_WRITEKIND_AND );
}
#endif

#ifdef ML_BMP_16_XOR
void ML_bmp_16_xor(const unsigned short *bmp, int x, int y, int flag ){
	ML_bmp_16_sub( bmp, x, y, (flag<<8) | IMB_WRITEKIND_XOR );
}
#endif

#ifdef ML_BMP_16_OR_CL
void ML_bmp_16_or_cl(const unsigned short *bmp, int x, int y, int flag ){
	ML_bmp_16_sub( bmp, x, y, (flag<<8) | IMB_WRITEKIND_OR );
}
#endif

#ifdef ML_BMP_16_AND_CL
void ML_bmp_16_and_cl(const unsigned short *bmp, int x, int y, int flag ){
	ML_bmp_16_sub( bmp, x, y, (flag<<8) | IMB_WRITEKIND_AND );
}
#endif

#ifdef ML_BMP_16_XOR_CL
void ML_bmp_16_xor_cl(const unsigned short *bmp, int x, int y, int flag ){
	ML_bmp_16_sub( bmp, x, y, (flag<<8) | IMB_WRITEKIND_XOR );
}
#endif

#ifdef ML_BMP_ZOOM
void ML_bmp_zoom(const unsigned char *bmp, int x, int y, int width, int height, float zoom_w, float zoom_h, ML_Color color)
{
    int i, j, iz, jz, width_z, height_z, nb_width, i3, bit, x_screen, pixel, k=0;
    int zoom_w14, zoom_h14;
    int begin_x, end_x, begin_y, end_y;
    char* vram = ML_vram_adress();

    if (!bmp) return;
    if (zoom_h < 0) zoom_h = 0;
    if (zoom_w < 0) zoom_w = 0;
    zoom_w14 = zoom_w * 16384;
    zoom_h14 = zoom_h * 16384;
    width_z = width * zoom_w14 >> 14 ;
    height_z = height * zoom_h14 >> 14;
    nb_width = width + 7 >> 3;

    if (x < 0) begin_x = -x;
    else begin_x = 0;
	if ( CB_G1MorG3M==1 ) {
	    if (x+width_z > 128) end_x = 128-x;
	    else end_x = width_z;
	    if (y < 0) begin_y = -y;
	    else begin_y = 0;
	    if (y+height_z > 64) end_y = 64-y;
	    else end_y = height_z;
	} else { 
		y+=24;
	    if (x+width_z > 384) end_x = 384-x;
	    else end_x = width_z;
	    if (y < 0) begin_y = -y;
	    else begin_y = 0;
	    if (y+height_z > 216) end_y = 216-y;
	    else end_y = height_z;
	    y-=24;
    }

	switch ( color ){
		default:
            for (iz=begin_x; iz<end_x; iz++)
            {
                i = (iz << 14) / zoom_w14;
                i3 = i >> 3;
                bit = 0x80 >> (i & 7);
                x_screen = x+iz;

                for (jz=begin_y; jz<end_y; jz++)
                {
                    j = (jz << 14) / zoom_h14;
                    pixel = bmp[i3 + nb_width * j] & bit;

					if (pixel != 0) ML_pixel(x_screen, y+jz, color);
                }
            }
			break;
	}
}
void ML_bmp_zoomCG(const unsigned char *bmp, int x, int y, int width, int height, float zoom_w, float zoom_h, ML_Color color)
{
    int i, j, iz, jz, width_z, height_z, nb_width, i3, bit, x_screen, pixel;
    int zoom_w14, zoom_h14;
    int begin_x, end_x, begin_y, end_y;
    unsigned short* vram = ( unsigned short* )ML_vram_adress();
    unsigned short* bmp2 = ( unsigned short* )bmp;
    int colindex=CB_ColorIndex;

    if (!bmp) return;
    y+=24;
    if (zoom_h < 0) zoom_h = 0;
    if (zoom_w < 0) zoom_w = 0;
    zoom_w14 = zoom_w * 16384;
    zoom_h14 = zoom_h * 16384;
    width_z = width * zoom_w14 >> 14 ;
    height_z = height * zoom_h14 >> 14;
    nb_width = width;

	if (x < 0) begin_x = -x;
	else begin_x = 0;
	if (x+width_z > 384) end_x = 384-x;
	else end_x = width_z;
	if (y < 0) begin_y = -y;
	else begin_y = 0;
	if (y+height_z > 216) end_y = 216-y;
	else end_y = height_z;

	switch ( color ){
		default:
            for (iz=begin_x; iz<end_x; iz++)
            {
                i = (iz << 14) / zoom_w14;
                i3 = i;
                x_screen = x+iz;

                for (jz=begin_y; jz<end_y; jz++)
                {
                    j = (jz << 14) / zoom_h14;
                    CB_ColorIndex = bmp2[i3 + nb_width * j];
					ML_pixel(x_screen, y+jz-24, color);
                }
            }
			break;
	}
	CB_ColorIndex = colindex;
}
#endif


#ifdef ML_BMP_ROTATE
void ML_bmp_rotate(const unsigned char *bmp, int x, int y, int width, int height, int angle, ML_Color color)
{
    int i, j, i3, dx, dy, ox, oy, xr, yr, nb_width, pixel, bit;
    int cosinus, sinus;
    char* vram = ML_vram_adress();

    if (!bmp) return;
	if ( CB_G1MorG3M==3 ) { y+=24; }
    ox = x + width / 2;
    oy = y + height / 2;
    angle %= 360;
    if (angle < 0) angle += 360;
    if (angle == 0) {cosinus = 16384; sinus = 0;}
    else if (angle == 90) {cosinus = 0; sinus = -16384;}
    else if (angle == 180) {cosinus = -16384; sinus = 0;}
    else if (angle == 270) {cosinus = 0; sinus = 16384;}
    else
    {
        cosinus = icos( angle, 1024 ) * 16;
        sinus   = isin( angle, 1024 ) * 16;
    }
    nb_width = width + 7 >> 3;

	switch ( color ){
		default:
            for (i=0; i<width; i++)
            {
                bit = 0x80 >> (i & 7);
                i3 = i >> 3;
                dx = x + i - ox;
                for (j=0; j<height; j++)
                {
                    dy = y + j - oy;
                    xr = ox + (dx * cosinus - dy * sinus >> 14);
                    yr = oy + (dx * sinus + dy * cosinus >> 14);
                    if ( CB_G1MorG3M==1 ) {
 	                   if (!(xr < 0 || xr > 127 || yr < 0 || yr > 63)) {
        	                pixel = bmp[i3 + nb_width * j] & bit;
							if (pixel != 0) ML_pixel(xr, yr, color);
                	    }
                	} else {
 	                   if (!(xr < 0 || xr > 383 || yr < 0 || yr > 215)) {
        	                pixel = bmp[i3 + nb_width * j] & bit;
							if (pixel != 0) ML_pixel(xr, yr-24, color);
                	    }
                	}
                }
            }
			break;
	}
}
void ML_bmp_rotateCG(const unsigned char *bmp, int x, int y, int width, int height, int angle, ML_Color color)
{
    int i, j, i3, dx, dy, ox, oy, xr, yr, nb_width, pixel, bit;
    int xr1,xr2,yr1,yr2;
    int cosinus, sinus;
    unsigned short* vram = ( unsigned short* )ML_vram_adress();
    unsigned short* bmp2 = ( unsigned short* )bmp;
	int colindex=CB_ColorIndex;

    if (!bmp) return;
    y+=24;
    ox = x + width / 2;
    oy = y + height / 2;
    
    ML_angle( angle, &sinus, &cosinus );

    nb_width = width ;

	switch ( color ){
		default:
            for (i=0; i<width; i++)
            {
                i3 = i;
                dx = x + i - ox;
                for (j=0; j<height; j++)
                {
                    dy = y + j - oy;
                    xr = ox + (dx * cosinus - dy * sinus >> 16);
                    yr = oy + (dx * sinus + dy * cosinus >> 16);
//                    xr1=(dx * cosinus - dy * sinus >> 13);
//                    xr2= xr1 & 1; if ( xr1<0 ) xr2=-xr2;
//                    xr = ox + (xr1 >> 1);
//                    yr1=(dx * sinus + dy * cosinus >> 13);
//                    yr2= yr1 & 1; if ( yr1<0 ) yr2=-yr2;
//                    yr = oy + (yr1 >> 1);
                    if (!(xr < 0 || xr > 383 || yr < 0 || yr > 215))
                    {
                    	CB_ColorIndex = bmp2[i3 + nb_width * j];
						ML_pixel(xr, yr-24, color);
//						if ( xr2 || yr2 ) ML_pixel(xr+xr2, yr-24+yr2, color);
                    }
                }
            }
			break;
	}
	CB_ColorIndex = colindex;
}
#endif



//------------------------------------------------------------------- MLpaint
// http://fussy.web.fc2.com/algo/index.htm
// Paintサンプルプログラムを利用させて頂いています。

#define MAXSIZE	256 /* バッファサイズ */

/* 画面サイズは 384 X 216 とする */
#define MINX 0
#define MINY -24
#define MAXX 384
#define MAXY 192

struct BufStr {
  int lx; /* 領域右端のX座標 */
  int rx; /* 領域右端のX座標 */
  int y;  /* 領域のY座標 */
  int oy; /* 親ラインのY座標 */
};
struct BufStr buff[MAXSIZE]; /* シード登録用バッファ */
struct BufStr *sIdx, *eIdx;  /* buffの先頭・末尾ポインタ */

/*
  scanLine : 線分からシードを探索してバッファに登録する

  int lx, rx : 線分のX座標の範囲
  int y : 線分のY座標
  int oy : 親ラインのY座標
  unsigned int col : 領域色
*/
void scanLine( int lx, int rx, int y, int oy, unsigned int col )
{
  while ( lx <= rx ) {

    /* 非領域色を飛ばす */
    for ( ; lx < rx ; lx++ )
      if ( ML_pixel_test( lx, y ) == col ) break;
    if ( ML_pixel_test( lx, y ) != col ) break;

    eIdx->lx = lx;

    /* 領域色を飛ばす */
    for ( ; lx <= rx ; lx++ )
      if ( ML_pixel_test( lx, y ) != col ) break;

    eIdx->rx = lx - 1;
    eIdx->y = y;
    eIdx->oy = oy;

    if ( ++eIdx == &buff[MAXSIZE] )
      eIdx = buff;
  }
}

/*
  paint : 塗り潰し処理(高速版)

  int x, y : 開始座標
  unsigned int paintCol : 塗り潰す時の色(描画色)
*/
void ML_paint( int x, int y, ML_Color color )
{
  int lx, rx; /* 塗り潰す線分の両端のX座標 */
  int ly;     /* 塗り潰す線分のY座標 */
  int oy;     /* 親ラインのY座標 */
  int i;
  unsigned int paintCol ; // 塗り潰す時の色(描画色)
  if ( CB_ColorIndex >= 0 ) paintCol=CB_ColorIndex; else paintCol=CB_ColorIndexPlot;
  unsigned int col = ML_pixel_test( x, y ); /* 閉領域の色(領域色) */
  if ( col == paintCol ) return;    /* 領域色と描画色が等しければ処理不要 */

  sIdx = buff;
  eIdx = buff + 1;
  sIdx->lx = sIdx->rx = x;
  sIdx->y = sIdx->oy = y;

  do {
    lx = sIdx->lx;
    rx = sIdx->rx;
    ly = sIdx->y;
    oy = sIdx->oy;

    int lxsav = lx - 1;
    int rxsav = rx + 1;

    if ( ++sIdx == &buff[MAXSIZE] ) sIdx = buff;

    /* 処理済のシードなら無視 */
    if ( ML_pixel_test( lx, ly ) != col )
      continue;

    /* 右方向の境界を探す */
    while ( rx < MAXX ) {
      if ( ML_pixel_test( rx + 1, ly ) != col ) break;
      rx++;
    }
    /* 左方向の境界を探す */
    while ( lx > MINX ) {
      if ( ML_pixel_test( lx - 1, ly ) != col ) break;
      lx--;
    }
    /* lx-rxの線分を描画 */
    for ( i = lx; i <= rx; i++ ) ML_pixel( i, ly, color );

    /* 真上のスキャンラインを走査する */
    if ( ly - 1 >= MINY ) {
      if ( ly - 1 == oy ) {
        scanLine( lx, lxsav, ly - 1, ly, col );
        scanLine( rxsav, rx, ly - 1, ly, col );
      } else {
        scanLine( lx, rx, ly - 1, ly, col );
      }
    }

    /* 真下のスキャンラインを走査する */
    if ( ly + 1 <= MAXY ) {
      if ( ly + 1 == oy ) {
        scanLine( lx, lxsav, ly + 1, ly, col );
        scanLine( rxsav, rx, ly + 1, ly, col );
      } else {
        scanLine( lx, rx, ly + 1, ly, col );
      }
    }

  } while ( sIdx != eIdx );
}


//------------------------------------------------------------------- MLtest
/*
ML_Color ML_pixel_test(int x, int y)
{
	char *vram, byte;
	if(x&~127 || y&~63) return 0;
	vram = ML_vram_adress();
	byte = 1<<(7-(x&7));
	return (vram[(y<<4)+(x>>3)] & byte ? ML_BLACK : ML_WHITE);
	
}
*/
int MLTest_point(int x, int y, int width )
{
	int count=0;
	if(width < 1) return 0;
	if(width == 1) count=ML_pixel_test(x, y);
	int padding, pair;
	padding = width>>1;
	pair = !(width&1);
	return MLTest_rectangle(x-padding+pair, y-padding+pair, x+padding, y+padding);
}

int MLTest_line(int x1, int y1, int x2, int y2)
{
	int i, x, y, dx, dy, sx, sy, cumul;
	int byte,count=0;
    char* vram = ML_vram_adress();
	x = x1;
	y = y1;
	dx = x2 - x1;
	dy = y2 - y1;
	sx = sgn(dx);
	sy = sgn(dy);
	dx = abs(dx);
	dy = abs(dy);
	if(dx > dy)
	{
		cumul = dx / 2;
		for(i=1 ; i<=dx ; i++)		// i<dx  ->  i<=dx	(modified)
		{
			x += sx;
			cumul += dy;
			if(cumul >= dx)			// > dx  ->  >= dx	(modified)
			{
				cumul -= dx;
				y += sy;
			}
			count += ML_pixel_test( x, y);
		}
	}
	else
	{
		cumul = dy / 2;
		for(i=1 ; i<=dy ; i++)		// i<dy  ->  i<=dy	(modified)
		{
			y += sy;
			cumul += dx;
			if(cumul >= dy)			// > dy  ->  >= dy	(modified)
			{
				cumul -= dy;
				x += sx;
			}
			count += ML_pixel_test( x, y);
		}
	}
	return count;
}

int MLTest_horizontal_line(int y, int x1, int x2 )
{
    int i,x,count=0;
	int byte;
	int xmax=128*CB_G1MorG3M-1;
    char* vram = ML_vram_adress();
//	if ( CB_G1MorG3M==1 ) { 
//	    if(y&~63 || (x1<0 && x2<0) || (x1>127 && x2>127)) return 0;
//	} else {
//		if ( ( x<0 ) || ( 383<x ) || ( y<-24 ) || ( 191<y ) ) return 0;
//	}
    if(x1 > x2)
    {
    	i = x1;
    	x1 = x2;
    	x2 = i;
    }
    if(x1 < 0) x1 = 0;
    if(x2 > xmax ) x2 = xmax;
	for ( x=x1; x<=x2; x++) {
		count += ML_pixel_test( x, y);
	}
	return count;
}

int MLTest_rectangle(int x1, int y1, int x2, int y2 )
{
	int i,x,y,count=0;
	int byte;
	int xmax=128*CB_G1MorG3M-1;
	int ymax= 64*CB_G1MorG3M-1;
    char* vram = ML_vram_adress();
	if(x1 > x2)
	{
		i = x1;
		x1 = x2;
		x2 = i;
	}
	if(y1 > y2)
	{
		i = y1;
		y1 = y2;
		y2 = i;
	}
	if (  x1<0  ) x1=0;
	if (  y1<0  ) y1=0;
    if(x1 > xmax ) x1 = xmax;
    if(x2 > xmax ) x2 = xmax;
	if (  ymax<y1 ) y1 = ymax;
	if (  ymax<y2 ) y2 = ymax;
	
	for ( y=y1; y<=y2; y++) {
		for ( x=x1; x<=x2; x++) {
			count += ML_pixel_test( x, y);
		}
	}
	return count;
}

int MLTest_filled_polygon(const int *x, const int *y, int nb_vertices )
{
	int i, j, dx, dy, ymin, ymax;
	int count=0;
	int cut_in_line[128+1], nb_cut;

	if( (nb_vertices < 3) || ( 128 < nb_vertices ) ) return 0;
	ymin = ymax = y[0];
	for(i=1 ; i<nb_vertices ; i++)
	{
		if(y[i] < ymin) ymin = y[i];
		if(y[i] > ymax) ymax = y[i];
	}
	for(i=ymin ; i<=ymax ; i++)
	{
		nb_cut = 0;
		for(j=0 ; j<nb_vertices ; j++)
		{
			if((y[j]<=i && y[(j+1)%nb_vertices]>=i) || (y[j]>=i && y[(j+1)%nb_vertices]<=i))
			{
				dy = abs(y[j]-y[(j+1)%nb_vertices]);
				if(dy)
				{
					dx = x[(j+1)%nb_vertices]-x[j];
					cut_in_line[nb_cut] = x[j] + rnd(abs(i-y[j]+sgn(i-y[j])/2)*dx/dy);
					nb_cut++;
				}
			}
		}
		ML_filled_polygon_quicksord(cut_in_line, 0, nb_cut-1);
		j = 0;
		while(j<nb_cut-2 && cut_in_line[j]==cut_in_line[j+1]) j++;
		while(j < nb_cut)
		{
			if(j == nb_cut-1) count+=MLTest_horizontal_line(i, cut_in_line[j-1]+1, cut_in_line[j]);
			else
			{
				dx = 1;
				while(j+dx<nb_cut-1 && cut_in_line[j+dx]==cut_in_line[j+dx+1]) dx++;
				count+=MLTest_horizontal_line(i, cut_in_line[j], cut_in_line[j+dx]);
				j += dx;
			}
			j++;
		}
	}
//	free(cut_in_line);
	return count;
}
/*
int MLTest_filled_circle(int x, int y, int radius)
{
	int plot_x, plot_y, d,count=0;

	if(radius < 0) return 0;
	plot_x = 0;
	plot_y = radius;
	d = 1 - radius;

	count+=MLTest_horizontal_line(y, x-plot_y, x+plot_y);
	while(plot_y > plot_x)
	{
		if(d < 0)
			d += 2*plot_x+3;
		else {
			d += 2*(plot_x-plot_y)+5;
			plot_y--;
			count+=MLTest_horizontal_line(y+plot_y+1, x-plot_x, x+plot_x);
			count+=MLTest_horizontal_line(y-plot_y-1, x-plot_x, x+plot_x);
		}
		plot_x++;
		if(plot_y >= plot_x)
		{
			count+=MLTest_horizontal_line(y+plot_x, x-plot_y, x+plot_y);
			count+=MLTest_horizontal_line(y-plot_x, x-plot_y, x+plot_y);
		}
	}
	return count;
}
*/
int MLTest_filled_ellipse(int x, int y, int radius1, int radius2 )
{
	int count=0;
	int plot_x, plot_y;
	float d1, d2;
	if(radius1 < 1 || radius2 < 1) return 0;
	plot_x = 0;
	plot_y = radius2;
	d1 = radius2*radius2 - radius1*radius1*radius2 + radius1*radius1/4;
	while(radius1*radius1*(plot_y-.5) > radius2*radius2*(plot_x+1))
	{
		if(d1 < 0)
		{
			d1 += radius2*radius2*(2*plot_x+3);
			plot_x++;
		} else {
			d1 += radius2*radius2*(2*plot_x+3) + radius1*radius1*(-2*plot_y+2);
			count+=MLTest_horizontal_line(y+plot_y, x-plot_x, x+plot_x);
			count+=MLTest_horizontal_line(y-plot_y, x-plot_x, x+plot_x);
			plot_x++;
			plot_y--;
		}
	}
	count+=MLTest_horizontal_line(y+plot_y, x-plot_x, x+plot_x);
	count+=MLTest_horizontal_line(y-plot_y, x-plot_x, x+plot_x);
	d2 = radius2*radius2*(plot_x+.5)*(plot_x+.5) + radius1*radius1*(plot_y-1)*(plot_y-1) - radius1*radius1*radius2*radius2;
	while(plot_y > 0)
	{
		if(d2 < 0)
		{
			d2 += radius2*radius2*(2*plot_x+2) + radius1*radius1*(-2*plot_y+3);
			plot_y--;
			plot_x++;
		} else {
			d2 += radius1*radius1*(-2*plot_y+3);
			plot_y--;
		}
		count+=MLTest_horizontal_line(y+plot_y, x-plot_x, x+plot_x);
		if(plot_y > 0)
			count+=MLTest_horizontal_line(y-plot_y, x-plot_x, x+plot_x);
	}
	return count;
}
int MLTest_filled_circle(int x, int y, int radius) {
	return MLTest_filled_ellipse( x, y, radius, radius );
}

int MLTest_filled_ellipse_in_rect(int x1, int y1, int x2, int y2)
{
	int radius1, radius2;
	if(x1 > x2)
	{
		int tmp = x1;
		x1 = x2;
		x2 = tmp;
	}
	if(y1 > y2)
	{
		int tmp = y1;
		y1 = y2;
		y2 = tmp;
	}
	radius1 = (x2-x1)/2;
	radius2 = (y2-y1)/2;
	return MLTest_filled_ellipse(x1+radius1, y1+radius2, radius1, radius2);
}
//----------------------------------------------------------------------------------------------
int MLLObjectAlign4a( unsigned int n ){ return n; }	// align +4byte
int MLLObjectAlign4b( unsigned int n ){ return n; }	// align +4byte
int MLLObjectAlign4c( unsigned int n ){ return n; }	// align +4byte
int MLLObjectAlign4d( unsigned int n ){ return n; }	// align +4byte
int MLLObjectAlign4e( unsigned int n ){ return n; }	// align +4byte
int MLLObjectAlign4f( unsigned int n ){ return n; }	// align +4byte
int MLLObjectAlign4g( unsigned int n ){ return n; }	// align +4byte
int MLLObjectAlign4h( unsigned int n ){ return n; }	// align +4byte
int MLLObjectAlign4i( unsigned int n ){ return n; }	// align +4byte
int MLLObjectAlign4j( unsigned int n ){ return n; }	// align +4byte
int MLLObjectAlign4k( unsigned int n ){ return n; }	// align +4byte
int MLLObjectAlign4l( unsigned int n ){ return n; }	// align +4byte
int MLLObjectAlign4m( unsigned int n ){ return n; }	// align +4byte
int MLLObjectAlign4n( unsigned int n ){ return n; }	// align +4byte
int MLLObjectAlign4o( unsigned int n ){ return n; }	// align +4byte
int MLLObjectAlign4p( unsigned int n ){ return n; }	// align +4byte
int MLLObjectAlign4q( unsigned int n ){ return n; }	// align +4byte
int MLLObjectAlign4r( unsigned int n ){ return n; }	// align +4byte
int MLLObjectAlign4s( unsigned int n ){ return n; }	// align +4byte
int MLLObjectAlign4t( unsigned int n ){ return n; }	// align +4byte
//----------------------------------------------------------------------------------------------

}
