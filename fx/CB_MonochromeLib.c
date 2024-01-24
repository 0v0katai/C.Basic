/*
===============================================================================

 C.Basic interpreter for fx-9860G series    v1.00
 New extended COMMAND from MonochromeLib (by PierrotLL)
 
 ML_ClrVRAM
 ML_ClrScreen
 ML_DispVRAM
 ML_Contrast
 ML_Pixel 
 ML_Point 
 ML_PixelTest(
 ML_Line 
 ML_Horizontal 
 ML_Vertical 
 ML_Rect 
 ML_Polygon 
 ML_FillPolygon 
 ML_Circle 
 ML_FillCircle 
 ML_Ellipse 
 ML_EllipseInRect 
 ML_FillEllipse 
 ML_FillEllipseInRect 
 ML_H_Scroll 
 ML_V_Scroll 

 ML_Bmp
   ML_BMP_OR
   ML_BMP_AND
   ML_BMP_XOR
   ML_BMP_OR_CL
   ML_BMP_AND_CL
   ML_BMP_XOR_CL
 ML_Bmp8
   ML_BMP_8_OR
   ML_BMP_8_AND
   ML_BMP_8_XOR
   ML_BMP_8_OR_CL
   ML_BMP_8_AND_CL
   ML_BMP_8_XOR_CL
 ML_Bmp16
   ML_BMP_16_OR
   ML_BMP_16_AND
   ML_BMP_16_XOR
   ML_BMP_16_OR_CL
   ML_BMP_16_AND_CL
   ML_BMP_16_XOR_CL


===============================================================================
*/
#include <ctype.h>
#include <fxlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <timer.h>
#include "KeyScan.h"
#include "CB_io.h"
#include "CB_inp.h"
#include "CB_glib.h"
#include "CB_glib2.h"
#include "CB_Eval.h"
#include "CB_file.h"
#include "CB_edit.h"
#include "CB_setup.h"
#include "CB_Time.h"
#include "CB_Matrix.h"
#include "CB_Str.h"

#include "CB_interpreter.h"
#include "CBI_interpreter.h"
#include "CB_error.h"
#include "fx_syscall.h"
#include "MonochromeLib.h"


//----------------------------------------------------------------------------------------------
//void CB_ML_ClrVRAM() { // ML_ClrVRAM
//	ML_clear_vram();
//}
//void CB_ML_ClrScreen() { // ML_ClrScreen
//	ML_clear_screen();
//}
//void CB_ML_DispVRAM() { // ML_DispVRAM
//	ML_display_vram();
//}
int CB_ML_SetContrast( char *SRC ) { // ML_Contrast
//	ML_set_contrast( CB_EvalInt( SRC ) );
	DD_SetContrast( CB_EvalInt( SRC ) );
}
//int CB_ML_GetContrast() { // ML_Contrast
//	return ML_get_contrast();
//}

//----------------------------------------------------------------------------------------------
void CB_GetOprand2( char *SRC, int *px, int *py) {
	*px=CB_EvalInt( SRC );
	if ( SRC[ExecPtr] != ',' ) { CB_Error(SyntaxERR); return; }  // Syntax error
	ExecPtr++;
	*py=CB_EvalInt( SRC );
}
void CB_GetOprand4( char *SRC, int *px, int *py, int *px2, int *py2) {
	CB_GetOprand2( SRC, &(*px), &(*py) );
	if ( SRC[ExecPtr] != ',' ) { CB_Error(SyntaxERR); return; }  // Syntax error
	ExecPtr++;
	CB_GetOprand2( SRC, &(*px2), &(*py2) );
}
void CB_GetOprand2dbl( char *SRC, double *x, double *y) {
	*x=CB_EvalDbl( SRC );
	if ( SRC[ExecPtr] != ',' ) { CB_Error(SyntaxERR); return; }  // Syntax error
	ExecPtr++;
	*y=CB_EvalDbl( SRC );
}
void CB_GetOprand2VW( char *SRC, int *px, int *py) {
	double x,y;
	if ( SRC[ExecPtr] == '#' ) {	// ViewWwindow(x,y,x2,y2)->(px,py,px2,py2)
		ExecPtr++;
		CB_GetOprand2dbl( SRC, &x, &y );
		VWtoPXY( x, y, &(*px), &(*py)) ;	// ViewWwindow(x,y) -> pixel(px,py)
	} else {
		CB_GetOprand2( SRC, &(*px), &(*py) );
	}
}
void CB_GetOprand4VW( char *SRC, int *px, int *py, int *px2, int *py2) {
	double x,y;
	if ( SRC[ExecPtr] == '#' ) {	// ViewWwindow(x,y,x2,y2)->(px,py,px2,py2)
		ExecPtr++;
		CB_GetOprand2dbl( SRC, &x, &y );
		VWtoPXY( x, y, &(*px), &(*py)) ;	// ViewWwindow(x,y) -> pixel(px,py)
		if ( SRC[ExecPtr] != ',' ) { CB_Error(SyntaxERR); return; }  // Syntax error
		ExecPtr++;
		CB_GetOprand2dbl( SRC, &x, &y );
		VWtoPXY( x, y, &(*px2), &(*py2)) ;	// ViewWwindow(x,y) -> pixel(px,py)
	} else {
		CB_GetOprand4( SRC, &(*px), &(*py), &(*px2), &(*py2) );
	}
}
void CB_GetOprand4VWR( char *SRC, int *px, int *py, int *radius1, int *radius2) {
	double x,y;
	if ( SRC[ExecPtr] == '#' ) {	// ViewWwindow(x,y,x2,y2)->(px,py,px2,py2)
		ExecPtr++;
		CB_GetOprand2dbl( SRC, &x, &y );
		VWtoPXY( x, y, &(*px), &(*py)) ;	// ViewWwindow(x,y) -> pixel(px,py)
		if ( SRC[ExecPtr] != ',' ) { CB_Error(SyntaxERR); return; }  // Syntax error
		ExecPtr++;
		CB_GetOprand2dbl( SRC, &x, &y );
		*radius1=x/Xdot;
		*radius2=y/Ydot;
	} else {
		CB_GetOprand4( SRC, &(*px), &(*py), &(*radius1), &(*radius2) );
	}
}
void CB_GetOprand3VWR( char *SRC, int *px, int *py, int *radius1) {
	double x,y;
//	int radius2;
	if ( SRC[ExecPtr] == '#' ) {	// ViewWwindow(x,y,x2,y2)->(px,py,px2,py2)
		ExecPtr++;
		CB_GetOprand2dbl( SRC, &x, &y );
		VWtoPXY( x, y, &(*px), &(*py)) ;	// ViewWwindow(x,y) -> pixel(px,py)
		if ( SRC[ExecPtr] != ',' ) { CB_Error(SyntaxERR); return; }  // Syntax error
		ExecPtr++;
		x=CB_EvalDbl( SRC );
		*radius1=x/Xdot;
//		radius2=x/Ydot;
//		if ( (*radius1) != radius2 ) { CB_Error(ArgumentERR); return; } // Argument error
	} else {
		CB_GetOprand2( SRC, &(*px), &(*py) );
		if ( SRC[ExecPtr] != ',' ) { CB_Error(SyntaxERR); return; }  // Syntax error
		ExecPtr++;
		*radius1=CB_EvalInt( SRC );
	}
}
void CB_GetOprand3VWyxx( char *SRC, int *py, int *px1, int *px2) {
	double y,x1,x2;
	if ( SRC[ExecPtr] == '#' ) {	// ViewWwindow(y,x1,x2)->(py,px,px2)
		ExecPtr++;
		y=CB_EvalDbl( SRC );
		if ( SRC[ExecPtr] != ',' ) { CB_Error(SyntaxERR); return; }  // Syntax error
		ExecPtr++;
		x1=CB_EvalDbl( SRC );
		if ( SRC[ExecPtr] != ',' ) { CB_Error(SyntaxERR); return; }  // Syntax error
		ExecPtr++;
		x2=CB_EvalDbl( SRC );
		VWtoPXY( x1, y, &(*px1), &(*py)) ;	// ViewWwindow(x,y) -> pixel(px,py)
		VWtoPXY( x2, y, &(*px2), &(*py)) ;	// ViewWwindow(x,y) -> pixel(px,py)
	} else {
		CB_GetOprand2( SRC, &(*py), &(*px1) );
		if ( SRC[ExecPtr] != ',' ) { CB_Error(SyntaxERR); return; }  // Syntax error
		ExecPtr++;
		*px2=CB_EvalInt( SRC );
	}
}
void CB_GetOprand3VWxyy( char *SRC, int *px, int *py1, int *py2) {
	double x,y1,y2;
	if ( SRC[ExecPtr] == '#' ) {	// ViewWwindow(x,y1,y2)->(px,py,py2)
		ExecPtr++;
		x=CB_EvalDbl( SRC );
		if ( SRC[ExecPtr] != ',' ) { CB_Error(SyntaxERR); return; }  // Syntax error
		ExecPtr++;
		y1=CB_EvalDbl( SRC );
		if ( SRC[ExecPtr] != ',' ) { CB_Error(SyntaxERR); return; }  // Syntax error
		ExecPtr++;
		y2=CB_EvalDbl( SRC );
		VWtoPXY( x, y1, &(*px), &(*py1)) ;	// ViewWwindow(x,y) -> pixel(px,py)
		VWtoPXY( x, y2, &(*px), &(*py2)) ;	// ViewWwindow(x,y) -> pixel(px,py)
	} else {
		CB_GetOprand2( SRC, &(*px), &(*py1) );
		if ( SRC[ExecPtr] != ',' ) { CB_Error(SyntaxERR); return; }  // Syntax error
		ExecPtr++;
		*py2=CB_EvalInt( SRC );
	}
}
void CB_GetOprand_MLcolor( char *SRC, int *color) {
	*color=ML_BLACK;
	if ( SRC[ExecPtr] != ',' ) return;
	ExecPtr++;
	if ( SRC[ExecPtr] == ',' ) return;
	*color=CB_EvalInt( SRC );
}
void CB_GetOprand_MLrand( char *SRC ) {
	double d;
	int c;
	MLV_rand=(RAND_MAX+1);	//
	if ( SRC[ExecPtr] != ',' ) return;
	ExecPtr++;
	c=SRC[ExecPtr];
	if ( c == ',' ) return;
	if ( c == '%') { ExecPtr++;
		c=CB_EvalDbl( SRC );
		if ( c <   0 ) c=0;
		if ( c > 100 ) c=100;
		MLV_rand = c*(RAND_MAX+1)/100;
	} else {
		d=CB_EvalDbl( SRC );
		if ( d < 0 ) d=0;
		if ( d > 1 ) d=1;
		MLV_rand = d*(RAND_MAX+1);
	}
}
void CB_GetOprand_MLwidth( char *SRC ) {
	MLV_width=1;	// line width
	if ( SRC[ExecPtr] != ',' ) return;
	ExecPtr++;
	if ( SRC[ExecPtr] == ',' ) return;
	MLV_width=CB_EvalInt( SRC );
}

//----------------------------------------------------------------------------------------------
void CB_ML_Pixel( char *SRC ) { // ML_Pixel x, y, color
	int x,y;
	int color,rand;
	CB_GetOprand2VW( SRC, &x, &y );
	CB_GetOprand_MLcolor( SRC, &color);
	CB_GetOprand_MLrand( SRC );
	if ( ErrorNo ) return ;
	ML_pixel( x, y, color);
}

void CB_ML_Point( char *SRC ) { // ML_Point x, y, width, color
	int x,y;
	int color,rand,width2;
	CB_GetOprand2VW( SRC, &x, &y );
	CB_GetOprand_MLwidth( SRC );
	CB_GetOprand_MLcolor( SRC, &color);
	CB_GetOprand_MLrand( SRC );
	if ( ErrorNo ) return ;
	ML_point( x, y, MLV_width, color);
}

int CB_ML_PixelTest( char *SRC ) { // ML_PixelTest( x, y)
	int x,y;
	CB_GetOprand2VW( SRC, &x, &y );
	if ( SRC[ExecPtr] == ')' ) ExecPtr++;
	if ( ErrorNo ) return ;
	return ML_pixel_test( x, y);
}

//----------------------------------------------------------------------------------------------
void CB_ML_Line( char *SRC ) { // ML_Line x1, y1, x2, y2, color
	int x1,y1,x2,y2;
	int color;
	CB_GetOprand4VW( SRC, &x1, &y1, &x2, &y2);
	CB_GetOprand_MLcolor( SRC, &color);
	CB_GetOprand_MLrand( SRC );
	CB_GetOprand_MLwidth( SRC );
	if ( ErrorNo ) return ;
	ML_line( x1, y1, x2, y2, color);
}

void CB_ML_Horizontal( char *SRC ) { // ML_Horizontal y, x1, x2, color
	int y,x1,x2;
	int color;
	CB_GetOprand3VWyxx( SRC, &y, &x1, &x2);
	CB_GetOprand_MLcolor( SRC, &color);
	CB_GetOprand_MLrand( SRC );
	if ( ErrorNo ) return ;
	ML_horizontal_line( y, x1, x2, color);
}

void CB_ML_Vertical( char *SRC ) { // ML_Vertical x, y1, y2, color
	int x,y1,y2;
	int color;
	CB_GetOprand3VWxyy( SRC, &x, &y1, &y2);
	CB_GetOprand_MLcolor( SRC, &color);
	CB_GetOprand_MLrand( SRC );
	if ( ErrorNo ) return ;
	ML_vertical_line( x, y1, y2, color);
}

//----------------------------------------------------------------------------------------------
void CB_ML_Rect( char *SRC ) { // ML_Rectangle x1,y1,x2,y2, border_width, border_color, fill_color
	int x1,y1,x2,y2;
	int border_width;
	int border_color;
	int fill_color;
	
	CB_GetOprand4VW( SRC, &x1, &y1, &x2, &y2);
	if ( SRC[ExecPtr] != ',' ) { CB_Error(SyntaxERR); return; }  // Syntax error
	ExecPtr++;
	border_width=CB_EvalInt( SRC );
	if ( SRC[ExecPtr] != ',' ) { CB_Error(SyntaxERR); return; }  // Syntax error
	ExecPtr++;
	CB_GetOprand2( SRC, &border_color, &fill_color);
	CB_GetOprand_MLrand( SRC );
	if ( ErrorNo ) return ;
	ML_rectangle( x1, y1, x2, y2, border_width, border_color, fill_color);
}
//----------------------------------------------------------------------------------------------
void CB_ML_Polygon( char *SRC, int fill ) { // ML_Polygon &Mat X, &Mat Y, nb_vertices, color
	int ary_x,ary_y;
	int nb_vertices;
	int color;
	CB_GetOprand4( SRC, &ary_x, &ary_y, &nb_vertices, &color );
	CB_GetOprand_MLrand( SRC );
	if ( ErrorNo ) return ;
	if ( fill ) ML_filled_polygon( (int *)ary_x, (int *)ary_y, nb_vertices, color);
	else		ML_polygon( (int *)ary_x, (int *)ary_y, nb_vertices, color);
}

//----------------------------------------------------------------------------------------------
void CB_ML_Circle( char *SRC, int fill ) { // ML_Circle x, y, radius, color
	int x,y;
	int radius;
	int color;
	CB_GetOprand3VWR( SRC, &x, &y, &radius);
	CB_GetOprand_MLcolor( SRC, &color);
	CB_GetOprand_MLrand( SRC );
	if ( ErrorNo ) return ;
	if ( fill ) ML_filled_circle( x, y, radius, color);
	else		ML_circle( x, y, radius, color);
}

//----------------------------------------------------------------------------------------------
void CB_ML_Ellipse( char *SRC, int fill ) { // ML_Ellipse x, y, radius1, radius2, color
	int x,y;
	int radius1;
	int radius2;
	int color;
	CB_GetOprand4VWR( SRC, &x, &y, &radius1, &radius2);
	CB_GetOprand_MLcolor( SRC, &color);
	CB_GetOprand_MLrand( SRC );
	if ( ErrorNo ) return ;
	if ( fill ) ML_filled_ellipse( x, y, radius1, radius2, color);
	else		ML_ellipse( x, y, radius1, radius2, color);
}

void CB_ML_EllipseInRect( char *SRC, int fill ) { // ML_EllipseInRect  x1, y1, x2, y2, color
	int x1,y1,x2,y2;
	int color;
	CB_GetOprand4VW( SRC, &x1, &y1, &x2, &y2);
	CB_GetOprand_MLcolor( SRC, &color);
	CB_GetOprand_MLrand( SRC );
	if ( ErrorNo ) return ;
	if ( fill ) ML_filled_ellipse_in_rect( x1, y1, x2, y2, color);
	else		ML_ellipse_in_rect( x1, y1, x2, y2, color);
}

//----------------------------------------------------------------------------------------------
void CB_ML_H_Scroll( char *SRC ) { // ML_H_Scroll scroll
	int scroll;
	scroll=CB_EvalInt( SRC );
	if ( ErrorNo ) return ;
	ML_horizontal_scroll( scroll );
}
void CB_ML_V_Scroll( char *SRC ) { // ML_V_Scroll scroll
	int scroll;
	scroll=CB_EvalInt( SRC );
	if ( ErrorNo ) return ;
	ML_vertical_scroll( scroll );
}

//----------------------------------------------------------------------------------------------
void CB_ML_GetBmpMode( char *SRC, int *mode1, int *mode2 ) {
	int c=SRC[ExecPtr];
	(*mode1)=0;
	(*mode2)=0;
	if ( c != ',' )  return ;	// 
	c=SRC[++ExecPtr];
	if ( ( c == 'O' ) || ( c == 'o' ) )  (*mode1)=0;	// Or
	else
	if ( ( c == 'A' ) || ( c == 'a' ) )  (*mode1)=1;	// And
	else
	if ( ( c == 'X' ) || ( c == 'x' ) )  (*mode1)=2;	// Xor
	else
	if ( c != ',' ) { CB_Error(SyntaxERR); return; }  // Syntax error

	if ( SRC[++ExecPtr] != ',' ) return ;
	c=SRC[++ExecPtr];
	if ( ( c == 'C' ) || ( c == 'c' ) )  (*mode2)=1;	// Cl
	else { CB_Error(SyntaxERR); return; }  // Syntax error
	ExecPtr++;
}


void CB_ML_Bmp( char *SRC ) { // ML_Bmp( &Mat A,  x, y, width, height[,O/A/X] [,C])
	int arry;
	int x,y;
	int width;
	int height;
	int mode1;	// Or/And/Xor
	int mode2;	// Cl
	
	arry=CB_EvalInt( SRC );
	if ( SRC[ExecPtr] != ',' ) { CB_Error(SyntaxERR); return; }  // Syntax error
	ExecPtr++;
	CB_GetOprand4( SRC, &x, &y, &width, &height );
	CB_ML_GetBmpMode( SRC, &mode1, &mode2 );
	if ( ErrorNo ) return ;

	switch ( mode1 ) {
		case 0:		// Or
			switch ( mode2 ) {
				case 0:		// 
					ML_bmp_or( (unsigned char *)arry, x, y, width, height);
					break;
				case 1:		// Cl
					ML_bmp_or_cl( (unsigned char *)arry, x, y, width, height);
					break;
			}
			break;
		case 1:		// And
			switch ( mode2 ) {
				case 0:		// 
					ML_bmp_and( (unsigned char *)arry, x, y, width, height);
					break;
				case 1:		// Cl
					ML_bmp_and_cl( (unsigned char *)arry, x, y, width, height);
					break;
			}
			break;
		case 2:		// Xor
			switch ( mode2 ) {
				case 0:		// 
					ML_bmp_xor( (unsigned char *)arry, x, y, width, height);
					break;
				case 1:		// Cl
					ML_bmp_xor_cl( (unsigned char *)arry, x, y, width, height);
					break;
			}
			break;
	} 
}

void CB_ML_Bmp8( char *SRC ) { // ML_Bmp( &Mat A,  x, y [,O/A/X] [,C])
	int arry;
	int x,y;
	int mode1;	// Or/And/Xor
	int mode2;	// Cl
	
	arry=CB_EvalInt( SRC );
	if ( SRC[ExecPtr] != ',' ) { CB_Error(SyntaxERR); return; }  // Syntax error
	ExecPtr++;
	CB_GetOprand2( SRC, &x, &y);
	CB_ML_GetBmpMode( SRC, &mode1, &mode2 );
	if ( ErrorNo ) return ;

	switch ( mode1 ) {
		case 0:		// Or
			switch ( mode2 ) {
				case 0:		// 
					ML_bmp_8_or( (unsigned char *)arry, x, y);
					break;
				case 1:		// Cl
					ML_bmp_8_or_cl( (unsigned char *)arry, x, y);
					break;
			}
			break;
		case 1:		// And
			switch ( mode2 ) {
				case 0:		// 
					ML_bmp_8_and( (unsigned char *)arry, x, y);
					break;
				case 1:		// Cl
					ML_bmp_8_and_cl( (unsigned char *)arry, x, y);
					break;
			}
			break;
		case 2:		// Xor
			switch ( mode2 ) {
				case 0:		// 
					ML_bmp_8_xor( (unsigned char *)arry, x, y);
					break;
				case 1:		// Cl
					ML_bmp_8_xor_cl( (unsigned char *)arry, x, y);
					break;
			}
			break;
	} 
}

void CB_ML_Bmp16( char *SRC ) { // ML_Bmp( &Mat A,  x, y [,O/A/X] [,C])
	int arry;
	int x,y;
	int mode1;	// Or/And/Xor
	int mode2;	// Cl
	
	arry=CB_EvalInt( SRC );
	if ( SRC[ExecPtr] != ',' ) { CB_Error(SyntaxERR); return; }  // Syntax error
	ExecPtr++;
	CB_GetOprand2( SRC, &x, &y );
	CB_ML_GetBmpMode( SRC, &mode1, &mode2 );
	if ( ErrorNo ) return ;

	switch ( mode1 ) {
		case 0:		// Or
			switch ( mode2 ) {
				case 0:		// 
					ML_bmp_8_or( (unsigned char *)arry, x, y);
					break;
				case 1:		// Cl
					ML_bmp_8_or_cl( (unsigned char *)arry, x, y);
					break;
			}
			break;
		case 1:		// And
			switch ( mode2 ) {
				case 0:		// 
					ML_bmp_8_and( (unsigned char *)arry, x, y);
					break;
				case 1:		// Cl
					ML_bmp_8_and_cl( (unsigned char *)arry, x, y);
					break;
			}
			break;
		case 2:		// Xor
			switch ( mode2 ) {
				case 0:		// 
					ML_bmp_8_xor( (unsigned char *)arry, x, y);
					break;
				case 1:		// Cl
					ML_bmp_8_xor_cl( (unsigned char *)arry, x, y);
					break;
			}
			break;
	} 
}

//----------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------

void CB_ML_command( char *SRC, int c ) { // ML_command
	switch ( c ) {
			case 0xFFFFFFC0:	// _ClrVRAM
//			CB_ML_ClrVRAM();
			ML_clear_vram();
			break;
		case 0xFFFFFFC1:	// _ClrScreen
//			CB_ML_ClrScreen();
			ML_clear_screen();
			break;
		case 0xFFFFFFC2:	// _DispVRAM
//			CB_ML_DispVRAM();
			ML_display_vram();
			break;
		case 0xFFFFFFC3:	// _Contrast
			CB_ML_SetContrast( SRC );
			break;
		case 0xFFFFFFC4:	// _Pixel
			CB_ML_Pixel( SRC );
			break;
		case 0xFFFFFFC5:	// _Point
			CB_ML_Point( SRC );
			break;
		case 0xFFFFFFC7:	// _Line
			CB_ML_Line( SRC );
			break;
		case 0xFFFFFFC8:	// _Horizontal
			CB_ML_Horizontal( SRC );
			break;
		case 0xFFFFFFC9:	// _Vertical
			CB_ML_Vertical( SRC );
			break;
		case 0xFFFFFFCA:	// _Rectangle
			CB_ML_Rect( SRC );
			break;
		case 0xFFFFFFCB:	// _Polygon
			CB_ML_Polygon( SRC, 0 );
			break;
		case 0xFFFFFFCC:	// _FillPolygon
			CB_ML_Polygon( SRC, 1 );
			break;
		case 0xFFFFFFCD:	// _Circle
			CB_ML_Circle( SRC, 0 );
			break;
		case 0xFFFFFFCE:	// _FillCircle
			CB_ML_Circle( SRC, 1 );
			break;
		case 0xFFFFFFCF:	// _Elips
			CB_ML_Ellipse( SRC, 0 );
			break;
		case 0xFFFFFFD0:	// _FillElips
			CB_ML_Ellipse( SRC, 1 );
			break;
		case 0xFFFFFFD1:	// _ElipsInRct
			CB_ML_EllipseInRect( SRC, 0 );
			break;
		case 0xFFFFFFD2:	// _FElipsInRct
			CB_ML_EllipseInRect( SRC, 1 );
			break;
		case 0xFFFFFFD3:	// _Hscroll
			CB_ML_H_Scroll( SRC );
			break;
		case 0xFFFFFFD4:	// _Vscroll
			CB_ML_V_Scroll( SRC );
			break;
		case 0xFFFFFFD5:	// _Bmp
			CB_ML_Bmp( SRC );
			break;
		case 0xFFFFFFD6:	// _Bmp8
			CB_ML_Bmp8( SRC );
			break;
		case 0xFFFFFFD7:	// _Bmp16
			CB_ML_Bmp16( SRC );
			break;
	}
}


//----------------------------------------------------------------------------------------------
int MLObjectAlign4d( unsigned int n ){ return n; }	// align +4byte
int MLObjectAlign4e( unsigned int n ){ return n; }	// align +4byte
//int MLObjectAlign4f( unsigned int n ){ return n; }	// align +4byte
//----------------------------------------------------------------------------------------------
