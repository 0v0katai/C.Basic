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

ML_BmpZoom
   ML_BMP_OR_ZOOM
   ML_BMP_AND_ZOOM
   ML_BMP_XOR_ZOOM

ML_BmpRotate
   ML_BMP_OR_ROTATE
   ML_BMP_AND_ROTATE
   ML_BMP_XOR_ROTATE

===============================================================================
*/
#include "CB.h"

//----------------------------------------------------------------------------------------------
//void CB_ML_ClrVRAM() { // ML_ClrVRAM
//	ML_clear_vram();
//}
//void CB_ML_ClrScreen() { // ML_ClrScreen
//	ML_clear_screen();
//}
void CB_ML_DispVRAM( char*SRC ){	// ML_DispVRAM
	int y1,y2,i;
	int c=SRC[g_exec_ptr];
	if ( c == ';' ) {
		g_exec_ptr++;
		if ( Check_skip_count() == 0 ) return ;
	}
	if ( Is35E2 ) Bdisp_PutDisp_DD();
	else	ML_display_vram();
	if ( BreakCheck )if ( KeyScanDownAC() ) { KeyRecover(); BreakPtr=g_exec_ptr; }	// [AC] break?

}
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
	if ( SRC[g_exec_ptr] != ',' ) { CB_Error(SyntaxERR); return; }  // Syntax error
	g_exec_ptr++;
	*py=CB_EvalInt( SRC );
}
void CB_GetOprand4( char *SRC, int *px, int *py, int *px2, int *py2) {
	CB_GetOprand2( SRC, &(*px), &(*py) );
	if ( SRC[g_exec_ptr] != ',' ) { CB_Error(SyntaxERR); return; }  // Syntax error
	g_exec_ptr++;
	CB_GetOprand2( SRC, &(*px2), &(*py2) );
}
void CB_GetOprand2dbl( char *SRC, double *x, double *y) {
	*x=CB_EvalDbl( SRC );
	if ( SRC[g_exec_ptr] != ',' ) { CB_Error(SyntaxERR); return; }  // Syntax error
	g_exec_ptr++;
	*y=CB_EvalDbl( SRC );
}
void CB_GetOprand2VW( char *SRC, int *px, int *py) {
	double x,y;
	if ( SRC[g_exec_ptr] == '#' ) {	// ViewWwindow(x,y,x2,y2)->(px,py,px2,py2)
		g_exec_ptr++;
		CB_GetOprand2dbl( SRC, &x, &y );
		VWtoPXY( x, y, &(*px), &(*py)) ;	// ViewWwindow(x,y) -> pixel(px,py)
	} else {
		CB_GetOprand2( SRC, &(*px), &(*py) );
	}
}
void CB_GetOprand4VW( char *SRC, int *px, int *py, int *px2, int *py2) {
	double x,y;
	if ( SRC[g_exec_ptr] == '#' ) {	// ViewWwindow(x,y,x2,y2)->(px,py,px2,py2)
		g_exec_ptr++;
		CB_GetOprand2dbl( SRC, &x, &y );
		VWtoPXY( x, y, &(*px), &(*py)) ;	// ViewWwindow(x,y) -> pixel(px,py)
		if ( SRC[g_exec_ptr] != ',' ) { CB_Error(SyntaxERR); return; }  // Syntax error
		g_exec_ptr++;
		CB_GetOprand2dbl( SRC, &x, &y );
		VWtoPXY( x, y, &(*px2), &(*py2)) ;	// ViewWwindow(x,y) -> pixel(px,py)
	} else {
		CB_GetOprand4( SRC, &(*px), &(*py), &(*px2), &(*py2) );
	}
}
void CB_GetOprand4VWR( char *SRC, int *px, int *py, int *radius1, int *radius2) {
	double x,y;
	if ( SRC[g_exec_ptr] == '#' ) {	// ViewWwindow(x,y,x2,y2)->(px,py,px2,py2)
		g_exec_ptr++;
		CB_GetOprand2dbl( SRC, &x, &y );
		VWtoPXY( x, y, &(*px), &(*py)) ;	// ViewWwindow(x,y) -> pixel(px,py)
		if ( SRC[g_exec_ptr] != ',' ) { CB_Error(SyntaxERR); return; }  // Syntax error
		g_exec_ptr++;
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
	if ( SRC[g_exec_ptr] == '#' ) {	// ViewWwindow(x,y,x2,y2)->(px,py,px2,py2)
		g_exec_ptr++;
		CB_GetOprand2dbl( SRC, &x, &y );
		VWtoPXY( x, y, &(*px), &(*py)) ;	// ViewWwindow(x,y) -> pixel(px,py)
		if ( SRC[g_exec_ptr] != ',' ) { CB_Error(SyntaxERR); return; }  // Syntax error
		g_exec_ptr++;
		x=CB_EvalDbl( SRC );
		*radius1=x/Xdot;
//		radius2=x/Ydot;
//		if ( (*radius1) != radius2 ) { CB_Error(ArgumentERR); return; } // Argument error
	} else {
		CB_GetOprand2( SRC, &(*px), &(*py) );
		if ( SRC[g_exec_ptr] != ',' ) { CB_Error(SyntaxERR); return; }  // Syntax error
		g_exec_ptr++;
		*radius1=CB_EvalInt( SRC );
	}
}
void CB_GetOprand3VWyxx( char *SRC, int *py, int *px1, int *px2) {
	double y,x1,x2;
	if ( SRC[g_exec_ptr] == '#' ) {	// ViewWwindow(y,x1,x2)->(py,px,px2)
		g_exec_ptr++;
		y=CB_EvalDbl( SRC );
		if ( SRC[g_exec_ptr] != ',' ) { CB_Error(SyntaxERR); return; }  // Syntax error
		g_exec_ptr++;
		x1=CB_EvalDbl( SRC );
		if ( SRC[g_exec_ptr] != ',' ) { CB_Error(SyntaxERR); return; }  // Syntax error
		g_exec_ptr++;
		x2=CB_EvalDbl( SRC );
		VWtoPXY( x1, y, &(*px1), &(*py)) ;	// ViewWwindow(x,y) -> pixel(px,py)
		VWtoPXY( x2, y, &(*px2), &(*py)) ;	// ViewWwindow(x,y) -> pixel(px,py)
	} else {
		CB_GetOprand2( SRC, &(*py), &(*px1) );
		if ( SRC[g_exec_ptr] != ',' ) { CB_Error(SyntaxERR); return; }  // Syntax error
		g_exec_ptr++;
		*px2=CB_EvalInt( SRC );
	}
}
void CB_GetOprand3VWxyy( char *SRC, int *px, int *py1, int *py2) {
	double x,y1,y2;
	if ( SRC[g_exec_ptr] == '#' ) {	// ViewWwindow(x,y1,y2)->(px,py,py2)
		g_exec_ptr++;
		x=CB_EvalDbl( SRC );
		if ( SRC[g_exec_ptr] != ',' ) { CB_Error(SyntaxERR); return; }  // Syntax error
		g_exec_ptr++;
		y1=CB_EvalDbl( SRC );
		if ( SRC[g_exec_ptr] != ',' ) { CB_Error(SyntaxERR); return; }  // Syntax error
		g_exec_ptr++;
		y2=CB_EvalDbl( SRC );
		VWtoPXY( x, y1, &(*px), &(*py1)) ;	// ViewWwindow(x,y) -> pixel(px,py)
		VWtoPXY( x, y2, &(*px), &(*py2)) ;	// ViewWwindow(x,y) -> pixel(px,py)
	} else {
		CB_GetOprand2( SRC, &(*px), &(*py1) );
		if ( SRC[g_exec_ptr] != ',' ) { CB_Error(SyntaxERR); return; }  // Syntax error
		g_exec_ptr++;
		*py2=CB_EvalInt( SRC );
	}
}

int CB_GetOprand_int1( char *SRC, int *value ) {
	if ( SRC[g_exec_ptr] != ',' ) return 0;
	g_exec_ptr++;
	if ( SRC[g_exec_ptr] == ',' ) return 1;
	*value =CB_EvalInt( SRC );
	return 1;
}
int CB_GetOprand_MLwidth( char *SRC ) {
	MLV_width=1;	// line width
	return CB_GetOprand_int1( SRC, &MLV_width );
}
int CB_GetOprand_MLangle( char *SRC, int *start, int *end, int *n ) {
	int r;
	*start=0;
	*end=360;
	*n=0;
	r=CB_GetOprand_int1( SRC, &(*start) );
	if ( r == 0 ) return 0;
	r=CB_GetOprand_int1( SRC, &(*end) );
	if ( r == 0 ) return 0;
	r=CB_GetOprand_int1( SRC, &(*n) );
	return 1;
}

int CB_GetOprand_percent( char *SRC ) {	// 0~100
	double d;
	int c;
	int value=100;
	if ( SRC[g_exec_ptr] != ',' ) return value;	// no rand oprand
	g_exec_ptr++;
	c=SRC[g_exec_ptr];
	if ( c == ',' ) return value;
	if ( c == '%') { g_exec_ptr++;
		c=CB_EvalInt( SRC );
		if ( c <   0 ) c=0;
		if ( c > 25600 ) c=25600;
		value=c;
	} else {
		d=CB_EvalDbl( SRC );
		if ( d < 0 ) d=0;
		if ( d > 256 ) d=256;
		value=d*100;
	}
	return value;
}

int CB_GetOprand_angle_center_percent( char *SRC, int *angle, int *center_x, int *center_y, int *percent ) {
	int r;
	*angle=0;
	*center_x=-2147483648;
	*center_y=-2147483648;
	*percent=100;
	r=CB_GetOprand_int1( SRC, &(*angle) );
	if ( r == 0 ) return 0;
	r=CB_GetOprand_int1( SRC, &(*center_x) );
	if ( r == 0 ) return 0;
	r=CB_GetOprand_int1( SRC, &(*center_y) );
	if ( r == 0 ) return 0;
	*percent = CB_GetOprand_percent( SRC );
	return 1;
}

void CB_GetOprand_MLcolor( char *SRC, int *color) {
	double d;
	int c;
	*color=ML_BLACK;
	MLV_rand=(RAND_MAX+1);	//
	if ( SRC[g_exec_ptr] != ',' ) return;	// no oprand
	g_exec_ptr++;
	if ( SRC[g_exec_ptr] == ',' ) goto jp;
	*color=CB_EvalInt( SRC );
  jp:
	if ( SRC[g_exec_ptr] != ',' ) return;	// no rand oprand
	g_exec_ptr++;
	c=SRC[g_exec_ptr];
	if ( c == ',' ) return;
	if ( c == '%') { g_exec_ptr++;
		c=CB_EvalInt( SRC );
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
//----------------------------------------------------------------------------------------------
void CB_ML_Pixel( char *SRC ) { // ML_Pixel x, y, color
	int x,y;
	int color,rand;
	CB_GetOprand2VW( SRC, &x, &y );
	CB_GetOprand_MLcolor( SRC, &color);
	if ( g_error_type ) return ;
	ML_pixel( x, y, color);
}

void CB_ML_Point( char *SRC ) { // ML_Point x, y, width, color
	int x,y;
	int color,rand,width2;
	CB_GetOprand2VW( SRC, &x, &y );
	CB_GetOprand_MLwidth( SRC );
	CB_GetOprand_MLcolor( SRC, &color);
	if ( g_error_type ) return ;
	ML_point( x, y, MLV_width, color);
}

int CB_ML_PixelTest( char *SRC ) { // ML_PixelTest( x, y[,T/G])
	int x,y,c,f=0;
	CB_GetOprand2VW( SRC, &x, &y );
	c=SRC[g_exec_ptr];
	if ( c == ',' ) {
		g_exec_ptr++;
		c=SRC[g_exec_ptr];
		if ( ( c=='T' )||( c=='t' ) ) {
			g_exec_ptr++;
			f=1;	// select Text Vram
		} else
		if ( ( c=='G' )||( c=='g' ) ) {
			g_exec_ptr++;
			f=2;	// select Graphic Vram
		}
	}
	if ( SRC[g_exec_ptr] == ')' ) g_exec_ptr++;
	if ( g_error_type ) return 0;
	switch ( f ) {
		case 0:		// Vram
			f=ML_pixel_test( x, y);
			break;
		case 1:		// Text Vram
			f=ML_pixel_test_TVRAM( x, y);
			break;
		case 2:		// Graphic Vram
			f=ML_pixel_test_GVRAM( x, y);
			break;
	}
	return f;
}

//----------------------------------------------------------------------------------------------
void CB_ML_Line( char *SRC ) { // ML_Line x1, y1, x2, y2, color
	int x1,y1,x2,y2;
	int color;
	CB_GetOprand4VW( SRC, &x1, &y1, &x2, &y2);
	CB_GetOprand_MLcolor( SRC, &color);
	CB_GetOprand_MLwidth( SRC );
	if ( g_error_type ) return ;
	ML_line( x1, y1, x2, y2, color);
}

void CB_ML_Horizontal( char *SRC ) { // ML_Horizontal y, x1, x2, color
	int y,x1,x2;
	int color;
	CB_GetOprand3VWyxx( SRC, &y, &x1, &x2);
	CB_GetOprand_MLcolor( SRC, &color);
	if ( g_error_type ) return ;
	ML_horizontal_line( y, x1, x2, color);
}

void CB_ML_Vertical( char *SRC ) { // ML_Vertical x, y1, y2, color
	int x,y1,y2;
	int color;
	CB_GetOprand3VWxyy( SRC, &x, &y1, &y2);
	CB_GetOprand_MLcolor( SRC, &color);
	if ( g_error_type ) return ;
	ML_vertical_line( x, y1, y2, color);
}

//----------------------------------------------------------------------------------------------
void CB_ML_Rect( char *SRC ) { // ML_Rectangle x1,y1,x2,y2, border_width, border_color, fill_color[, chance[, angle, center_x, center_y, percent]
	int x1,y1,x2,y2;
	int border_width;
	int border_color;
	int fill_color;
	int angle,center_x, center_y, percent;
	CB_GetOprand4VW( SRC, &x1, &y1, &x2, &y2);
	if ( SRC[g_exec_ptr] != ',' ) { CB_Error(SyntaxERR); return; }  // Syntax error
	g_exec_ptr++;
	border_width=CB_EvalInt( SRC );
	if ( SRC[g_exec_ptr] != ',' ) { CB_Error(SyntaxERR); return; }  // Syntax error
	g_exec_ptr++;
	border_color=CB_EvalInt( SRC );
	CB_GetOprand_MLcolor( SRC, &fill_color );
	CB_GetOprand_angle_center_percent( SRC, &angle, &center_x, &center_y, &percent );
	MLV_width=1;	// line width
	if ( g_error_type ) return ;
	if ( ( angle ) || ( percent!=100 ) ) ML_rectangle_Rotate( x1, y1, x2, y2, border_width, border_color, fill_color, angle, center_x,center_y, percent );
	else	ML_rectangle( x1, y1, x2, y2, border_width, border_color, fill_color );
}
//----------------------------------------------------------------------------------------------
void CB_ML_Polygon( char *SRC, int fill ) { // ML_Polygon &Mat X, &Mat Y, nb_vertices[, color, chance[, angle, center_x, center_y, percent]
	int ary_x,ary_y;
	int nb_vertices;
	int color;
	int angle,center_x, center_y, percent;
	CB_GetOprand2( SRC, &ary_x, &ary_y );
	if ( SRC[g_exec_ptr] != ',' ) { CB_Error(SyntaxERR); return; }  // Syntax error
	g_exec_ptr++;
	nb_vertices=CB_EvalInt( SRC );
	CB_GetOprand_MLcolor( SRC, &color);
	CB_GetOprand_angle_center_percent( SRC, &angle, &center_x, &center_y, &percent );
	MLV_width=1;	// line width
	if ( g_error_type ) return ;
	if ( ( angle ) || ( percent!=100 ) ) ML_polygon_Rotate( (int *)ary_x, (int *)ary_y, nb_vertices, color, angle, center_x, center_y, percent, fill);
	else {
		if ( fill ) ML_filled_polygon( (int *)ary_x, (int *)ary_y, nb_vertices, color);
		else		ML_polygon( (int *)ary_x, (int *)ary_y, nb_vertices, color);
	}
}

//----------------------------------------------------------------------------------------------
void CB_ML_Circle( char *SRC, int fill ) { // ML_Circle x, y, radius, color
	int x,y;
	int radius;
	int color;
	int Iswidth,Isangle,start,end, n;
	CB_GetOprand3VWR( SRC, &x, &y, &radius);
	CB_GetOprand_MLcolor( SRC, &color);
	Iswidth=CB_GetOprand_MLwidth( SRC );
	Isangle=CB_GetOprand_MLangle( SRC, &start, &end, &n ) ;
	if ( g_error_type ) return ;
	if ( fill ) ML_filled_circle( x, y, radius, color);
	else {
		if ( Iswidth || Isangle )	ML_circle2( x, y, radius, color, start, end, n );
		else			ML_circle( x, y, radius, color);
	}
}

//----------------------------------------------------------------------------------------------
void CB_ML_Ellipse( char *SRC, int fill ) { // ML_Ellipse x, y, radius1, radius2, color, angle
	int x,y;
	int radius1;
	int radius2;
	int color;
	int angle=0;
	CB_GetOprand4VWR( SRC, &x, &y, &radius1, &radius2);
	CB_GetOprand_MLcolor( SRC, &color);
	CB_GetOprand_int1( SRC, &angle );
	if ( g_error_type ) return ;
	if ( fill ) ML_filled_ellipse( x, y, radius1, radius2, color, angle);
	else		ML_ellipse( x, y, radius1, radius2, color, angle);
}

void CB_ML_EllipseInRect( char *SRC, int fill ) { // ML_EllipseInRect  x1, y1, x2, y2, color, angle
	int x1,y1,x2,y2;
	int color;
	int angle=0;
	CB_GetOprand4VW( SRC, &x1, &y1, &x2, &y2);
	CB_GetOprand_MLcolor( SRC, &color);
	CB_GetOprand_int1( SRC, &angle );
	if ( g_error_type ) return ;
	if ( fill ) ML_filled_ellipse_in_rect( x1, y1, x2, y2, color, angle);
	else		ML_ellipse_in_rect( x1, y1, x2, y2, color, angle);
}

//----------------------------------------------------------------------------------------------
void CB_ML_H_Scroll( char *SRC ) { // ML_H_Scroll scroll
	int c;
	int scroll;
	int x1,y1,x2,y2;
	scroll=CB_EvalInt( SRC );
	c=SRC[g_exec_ptr];
	if ( c == ',' ) {
		g_exec_ptr++;
		CB_GetOprand4VW( SRC, &x1, &y1, &x2, &y2);
		if ( g_error_type ) return ;
		ML_horizontal_scroll2( scroll, x1, y1, x2, y2 );
	} else {
		if ( g_error_type ) return ;
		ML_horizontal_scroll( scroll );
	}
}
void CB_ML_V_Scroll( char *SRC ) { // ML_V_Scroll scroll
	int c;
	int scroll;
	int x1,y1,x2,y2;
	scroll=CB_EvalInt( SRC );
	c=SRC[g_exec_ptr];
	if ( c == ',' ) {
		g_exec_ptr++;
		CB_GetOprand4VW( SRC, &x1, &y1, &x2, &y2);
		if ( g_error_type ) return ;
		ML_vertical_scroll2( scroll, x1, y1, x2, y2 );
	} else {
		if ( g_error_type ) return ;
		ML_vertical_scroll( scroll );
	}
}

//----------------------------------------------------------------------------------------------
void CB_ML_GetBmpMode( char *SRC, int *mode1, int *mode2 ) {
	int c=SRC[g_exec_ptr];
	(*mode1)=0;
	(*mode2)=0;
	if ( c != ',' )  return ;	// 
	c=SRC[++g_exec_ptr];
	if ( ( c == 'O' ) || ( c == 'o' ) )  (*mode1)=0;	// Or
	else
	if ( ( c == 'A' ) || ( c == 'a' ) )  (*mode1)=1;	// And
	else
	if ( ( c == 'X' ) || ( c == 'x' ) )  (*mode1)=2;	// Xor
	else
	if ( c != ',' ) { CB_Error(SyntaxERR); return; }  // Syntax error

	if ( SRC[++g_exec_ptr] != ',' ) return ;
	c=SRC[++g_exec_ptr];
	if ( ( c == 'C' ) || ( c == 'c' ) )  (*mode2)=1;	// Cl
	else { CB_Error(SyntaxERR); return; }  // Syntax error
	g_exec_ptr++;
}

/*
unsigned char* CB_ML_GetMatAdrs( char *SRC ) {
	int reg,x,y;
	int c=SRC[ExecPtr];
	if ( ( c=='&' ) || ( ( c==0x7F)&&(SRC[ExecPtr+1]==0xFFFFFFF8) ) {	// & or VarPtr(
		return (unsigned char *)CB_EvalInt( SRC );
	} else {
		MatrixOprand( SRC, &reg, &x, &y );
		if ( ErrorNo ) return NULL; // error
		return 	(unsigned char*)MatAry[reg].Adrs;
	}
}
*/

void CB_ML_Bmp( char *SRC ) { // ML_Bmp( &Mat A,  x, y, width, height[,O/A/X] [,C])
	unsigned char* array;
	int x,y;
	int width;
	int height;
	int mode1;	// Or/And/Xor
	int mode2;	// Cl
	
	array=(unsigned char *)CB_EvalInt( SRC );
	if ( SRC[g_exec_ptr] != ',' ) { CB_Error(SyntaxERR); return; }  // Syntax error
	g_exec_ptr++;
	CB_GetOprand4( SRC, &x, &y, &width, &height );
	CB_ML_GetBmpMode( SRC, &mode1, &mode2 );
	if ( g_error_type ) return ;

	switch ( mode1 ) {
		case 0:		// Or
			switch ( mode2 ) {
				case 0:		// 
					ML_bmp_or( array, x, y, width, height);
					break;
				case 1:		// Cl
					ML_bmp_or_cl( array, x, y, width, height);
					break;
			}
			break;
		case 1:		// And
			switch ( mode2 ) {
				case 0:		// 
					ML_bmp_and( array, x, y, width, height);
					break;
				case 1:		// Cl
					ML_bmp_and_cl( array, x, y, width, height);
					break;
			}
			break;
		case 2:		// Xor
			switch ( mode2 ) {
				case 0:		// 
					ML_bmp_xor( array, x, y, width, height);
					break;
				case 1:		// Cl
					ML_bmp_xor_cl( array, x, y, width, height);
					break;
			}
			break;
	} 
}

void CB_ML_Bmp8( char *SRC ) { // ML_Bmp( &Mat A,  x, y [,O/A/X] [,C])
	unsigned char* array;
	int x,y;
	int mode1;	// Or/And/Xor
	int mode2;	// Cl
	
	array=(unsigned char *)CB_EvalInt( SRC );
	if ( SRC[g_exec_ptr] != ',' ) { CB_Error(SyntaxERR); return; }  // Syntax error
	g_exec_ptr++;
	CB_GetOprand2( SRC, &x, &y);
	CB_ML_GetBmpMode( SRC, &mode1, &mode2 );
	if ( g_error_type ) return ;

	switch ( mode1 ) {
		case 0:		// Or
			switch ( mode2 ) {
				case 0:		// 
					ML_bmp_8_or( array, x, y);
					break;
				case 1:		// Cl
					ML_bmp_8_or_cl( array, x, y);
					break;
			}
			break;
		case 1:		// And
			switch ( mode2 ) {
				case 0:		// 
					ML_bmp_8_and( array, x, y);
					break;
				case 1:		// Cl
					ML_bmp_8_and_cl( array, x, y);
					break;
			}
			break;
		case 2:		// Xor
			switch ( mode2 ) {
				case 0:		// 
					ML_bmp_8_xor( array, x, y);
					break;
				case 1:		// Cl
					ML_bmp_8_xor_cl( array, x, y);
					break;
			}
			break;
	} 
}

void CB_ML_Bmp16( char *SRC ) { // ML_Bmp( &Mat A,  x, y [,O/A/X] [,C])
	unsigned char* array;
	int x,y;
	int mode1;	// Or/And/Xor
	int mode2;	// Cl
	
	array=(unsigned char *)CB_EvalInt( SRC );
	if ( SRC[g_exec_ptr] != ',' ) { CB_Error(SyntaxERR); return; }  // Syntax error
	g_exec_ptr++;
	CB_GetOprand2( SRC, &x, &y );
	CB_ML_GetBmpMode( SRC, &mode1, &mode2 );
	if ( g_error_type ) return ;

	switch ( mode1 ) {
		case 0:		// Or
			switch ( mode2 ) {
				case 0:		// 
					ML_bmp_8_or( array, x, y);
					break;
				case 1:		// Cl
					ML_bmp_8_or_cl( array, x, y);
					break;
			}
			break;
		case 1:		// And
			switch ( mode2 ) {
				case 0:		// 
					ML_bmp_8_and( array, x, y);
					break;
				case 1:		// Cl
					ML_bmp_8_and_cl( array, x, y);
					break;
			}
			break;
		case 2:		// Xor
			switch ( mode2 ) {
				case 0:		// 
					ML_bmp_8_xor( array, x, y);
					break;
				case 1:		// Cl
					ML_bmp_8_xor_cl( array, x, y);
					break;
			}
			break;
	} 
}

//----------------------------------------------------------------------------------------------
void CB_ML_BmpZoom( char *SRC ) { // ML_BmpZoom( &Mat A,  x, y, width, height, zoomwidth, zoomheight [,color][,rand])
	unsigned char* array;
	int x,y;
	int width;
	int height;
	int color;
	float zoomwidth, zoomheight;
	
	array=(unsigned char *)CB_EvalInt( SRC );
	if ( SRC[g_exec_ptr] != ',' ) { CB_Error(SyntaxERR); return; }  // Syntax error
	g_exec_ptr++;
	CB_GetOprand4( SRC, &x, &y, &width, &height );
  	zoomwidth  = (float)CB_GetOprand_percent( SRC ) / (float)100 ;
  	zoomheight = (float)CB_GetOprand_percent( SRC ) / (float)100 ;
	CB_GetOprand_MLcolor( SRC, &color);
	if ( g_error_type ) return ;

	ML_bmp_zoom( array, x, y, width, height, zoomwidth,zoomheight, color);
}
void CB_ML_BmpRotate( char *SRC ) { // ML_BmpRotate( &Mat A,  x, y, width, height, angle [,color][,rand])
	unsigned char* array;
	int x,y;
	int width;
	int height;
	int angle=0;
	int color;
	
	array=(unsigned char *)CB_EvalInt( SRC );
	if ( SRC[g_exec_ptr] != ',' ) { CB_Error(SyntaxERR); return; }  // Syntax error
	g_exec_ptr++;
	CB_GetOprand4( SRC, &x, &y, &width, &height );
	CB_GetOprand_int1( SRC, &angle );
	CB_GetOprand_MLcolor( SRC, &color);
	if ( g_error_type ) return ;

	ML_bmp_rotate( array, x, y, width, height, angle, color);
}
void ML_bmp_zoommem( unsigned char *vram, const unsigned char *bmp, int width, int height, float zoom_w, float zoom_h, int *width2, int *height2 ) {
    int i, j, iz, jz, width_z, height_z, nb_width, i3, bit, x_screen, pixel;
    int zoom_w14, zoom_h14;
    int begin_x, end_x, begin_y, end_y;

    if (!bmp) return;
    if (zoom_h < 0) zoom_h = 0;
    if (zoom_w < 0) zoom_w = 0;
    zoom_w14 = zoom_w * 16384;
    zoom_h14 = zoom_h * 16384;
    width_z = width * zoom_w14 >> 14 ;
    height_z = height * zoom_h14 >> 14;
    nb_width = width + 7 >> 3;

    if (width_z > 144) end_x = 144;
    else end_x = width_z;
    if (height_z > 72) end_y = 72;
    else end_y = height_z;

    for (iz=0; iz<end_x; iz++)
    {
        i = (iz << 14) / zoom_w14;
        i3 = i >> 3;
        bit = 0x80 >> (i & 7);
        x_screen = iz;

        for (jz=0; jz<end_y; jz++)
        {
            j = (jz << 14) / zoom_h14;
            pixel = bmp[i3 + nb_width * j] & bit;

			if (pixel != 0)	vram[(jz<<5)+(x_screen>>3)] |= 128>>(x_screen&7);
			else			vram[(jz<<5)+(x_screen>>3)] &= ~(128>>(x_screen&7));
        }
    }
    *width2 =end_x;
    *height2=end_y;
}

void CB_ML_BmpZoomRotate( char *SRC ) { // ML_BmpZoom( &Mat A,  x, y, width, height, zoomwidth, zoomheight, angle [,color][,rand])
	unsigned char* array;
	unsigned char vram2[4096];	// vram buffer2
	int x,y;
	int width;
	int height;
	int angle=0;
	int width2;
	int height2;
	int color;
	float zoomwidth, zoomheight;
	int i,j,k,k2,w,w2;
	
	array=(unsigned char *)CB_EvalInt( SRC );
	if ( SRC[g_exec_ptr] != ',' ) { CB_Error(SyntaxERR); return; }  // Syntax error
	g_exec_ptr++;
	CB_GetOprand4( SRC, &x, &y, &width, &height );
  	zoomwidth  = (float)CB_GetOprand_percent( SRC ) / (float)100 ;
  	zoomheight = (float)CB_GetOprand_percent( SRC ) / (float)100 ;
	CB_GetOprand_int1( SRC, &angle );
	CB_GetOprand_MLcolor( SRC, &color);
	if ( g_error_type ) return ;
	
	ML_bmp_zoommem( vram2, array, width, height, zoomwidth, zoomheight, &width2, &height2 );
	w=(width2+7)/8;
	w2=256/8;
	for ( i=1; i<height2; i++) {
		k =i*w;
		k2=i*w2;
		for ( j=0; j<w; j++) {
			vram2[k+j]=vram2[k2+j];
		}
	}
	ML_bmp_rotate( vram2, x, y, width2, height2, angle, color);
}

//----------------------------------------------------------------------------------------------

void CB_DrawMat( char *SRC ) { // DrawMat Mat A[x,y],px,py,width, height[,zoomwidth][,zoomheight][,ML_Color color][,chance])
	unsigned char* array;
	int wx,wy;
	int x,y,px,py;
	int width;
	int height;
	int px1,py1,px2,py2;
	int color;
	float zoomwidth, zoomheight;
	int reg,reg2=Mattmpreg;
	int i;
	int dimA,dimB,ElementSize,base;
	int xx,yy;
		
	MatrixOprand( SRC, &reg, &x, &y );
	if ( g_error_type ) return ; // error
//	array=(unsigned char *)CB_EvalInt( SRC );
	if ( SRC[g_exec_ptr] != ',' ) { CB_Error(SyntaxERR); return; }  // Syntax error
	g_exec_ptr++;
	CB_GetOprand4( SRC, &px1, &py1, &width, &height );
	if ( (px1<0)||(py1<0)||(width<1)||(height<1) ) { CB_Error(ArgumentERR); return; } // Argument error
  	zoomwidth  = (float)CB_GetOprand_percent( SRC ) / (float)100 ;
  	zoomheight = (float)CB_GetOprand_percent( SRC ) / (float)100 ;
	CB_GetOprand_MLcolor( SRC, &color);
	if ( g_error_type ) return ;

	xx=px1;
	yy=py1;
	
	dimA=MatAry[reg].SizeA;
	dimB=MatAry[reg].SizeB;
	if ( width  > dimA ) width  =dimA;
	if ( height > dimB ) height =dimB;
	px2=px1+width-1;
	py2=py1+height-1;
	if ( dimA <= x ) x=dimA-1;
	if ( dimB <= y ) y=dimB-1;
	if ( px1+dimA-x < px2 ) px2=px1+dimA-x;
	if ( py1+dimB-y < py2 ) py2=py1+dimB-y;
	width =px2-px1+1;
	height=py2-py1+1;

	ElementSize=1;
	base=0;

	DimMatrixSub( reg2, ElementSize, width, height, base ) ;
	if ( g_error_type ) return ; // error

	px1=0;
	py1=0;
	px2=width-1;
	py2=height-1;
	i=x;
	for ( py=py1; py<=py2 ; py++) {	
		x=i;
		for ( px=px1; px<=px2 ; px++) WriteMatrixInt( reg2, px, py, ReadMatrixInt( reg, x++, y ));
		y++;
	}
	ML_bmp_zoom( (unsigned char*)MatAry[reg2].Adrs, xx, yy, width, height, zoomwidth,zoomheight, color) ;
//	DeleteMatrix( reg2 );
	dspflag=0;
}

//-----------------------------------------------------------------------------------------MLtest

int CB_MLTest_Point( char *SRC ) { // MLTest_Point x, y, width
	int x,y;
	CB_GetOprand2VW( SRC, &x, &y );
	CB_GetOprand_MLwidth( SRC );
	if ( g_error_type ) return 0;
	return MLTest_point( x, y, MLV_width);
}
int CB_MLTest_Line( char *SRC ) { // MLTest_Line x1, y1, x2, y2
	int x1,y1,x2,y2;
	CB_GetOprand4VW( SRC, &x1, &y1, &x2, &y2);
	if ( g_error_type ) return 0;
	return MLTest_line( x1, y1, x2, y2);
}

int CB_MLTest_Horizontal( char *SRC ) { // MLTest_Horizontal y, x1, x2
	int y,x1,x2;
	CB_GetOprand3VWyxx( SRC, &y, &x1, &x2);
	if ( g_error_type ) return 0;
	return MLTest_horizontal_line( y, x1, x2);
}

int CB_MLTest_Vertical( char *SRC ) { // MLTest_Vertical x, y1, y2
	int x,y1,y2;
	CB_GetOprand3VWxyy( SRC, &x, &y1, &y2);
	if ( g_error_type ) return 0;
	return MLTest_rectangle( x, y1, x, y2);
}

int CB_MLTest_Rect( char *SRC ) { // MLTest_Rectangle x1,y1,x2,y2
	int x1,y1,x2,y2;
	CB_GetOprand4VW( SRC, &x1, &y1, &x2, &y2);
	if ( g_error_type ) return 0;
	return MLTest_rectangle( x1, y1, x2, y2);
}

int CB_MLTest_Polygon( char *SRC ) { // MLTest_Polygon &Mat X, &Mat Y, nb_vertices
	int ary_x,ary_y;
	int nb_vertices;
	CB_GetOprand2( SRC, &ary_x, &ary_y );
	if ( SRC[g_exec_ptr] != ',' ) { CB_Error(SyntaxERR); return 0; }  // Syntax error
	g_exec_ptr++;
	nb_vertices=CB_EvalInt( SRC );
	if ( g_error_type ) return 0;
	return MLTest_filled_polygon( (int *)ary_x, (int *)ary_y, nb_vertices);
}

int CB_MLTest_Circle( char *SRC ) { // MLTest_Circle x, y, radius
	int x,y;
	int radius;
	CB_GetOprand3VWR( SRC, &x, &y, &radius);
	if ( g_error_type ) return 0;
	return MLTest_filled_circle( x, y, radius);
}

int CB_MLTest_Ellipse( char *SRC ) { // MLTest_Ellipse x, y, radius1, radius2
	int x,y;
	int radius1;
	int radius2;
	CB_GetOprand4VWR( SRC, &x, &y, &radius1, &radius2);
	if ( g_error_type ) return 0;
	return MLTest_filled_ellipse( x, y, radius1, radius2);
}

int CB_MLTest_EllipseInRect( char *SRC ) { // MLTest_EllipseInRect  x1, y1, x2, y2
	int x1,y1,x2,y2;
	CB_GetOprand4VW( SRC, &x1, &y1, &x2, &y2);
	if ( g_error_type ) return 0;
	return MLTest_filled_ellipse_in_rect( x1, y1, x2, y2);
}
//----------------------------------------------------------------------------------------------
int CB_MLTest( char *SRC) { // MLTest_command
	int c;
	c=SRC[g_exec_ptr];
	if ( c != 0xFFFFFFF9 ) { CB_Error(SyntaxERR); return 0; }  // Syntax error
	g_exec_ptr++;
	c=SRC[g_exec_ptr++];
	switch ( c ) {
		case 0xFFFFFFC5:	// _Point
			return CB_MLTest_Point( SRC );
		case 0xFFFFFFC7:	// _Line
			return CB_MLTest_Line( SRC );
		case 0xFFFFFFC8:	// _Horizontal
			return CB_MLTest_Horizontal( SRC );
		case 0xFFFFFFC9:	// _Vertical
			return CB_MLTest_Vertical( SRC );
		case 0xFFFFFFCA:	// _Rectangle
			return CB_MLTest_Rect( SRC );
		case 0xFFFFFFCB:	// _Polygon
		case 0xFFFFFFCC:	// _FillPolygon
			return CB_MLTest_Polygon( SRC );
		case 0xFFFFFFCD:	// _Circle
		case 0xFFFFFFCE:	// _FillCircle
			return CB_MLTest_Circle( SRC );
		case 0xFFFFFFCF:	// _Elips
		case 0xFFFFFFD0:	// _FillElips
			return CB_MLTest_Ellipse( SRC );
		case 0xFFFFFFD1:	// _ElipsInRct
		case 0xFFFFFFD2:	// _FElipsInRct
			return CB_MLTest_EllipseInRect( SRC );
	}
	g_exec_ptr-=2;
	{ CB_Error(SyntaxERR); return 0; }  // Syntax error
}
//----------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------

void CB_ML_command( char *SRC, int c ) { // ML_command
	switch ( c ) {
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
		case 0xFFFFFFD5:	// _Bmp
			CB_ML_Bmp( SRC );
			break;
		case 0xFFFFFFD6:	// _Bmp8
			CB_ML_Bmp8( SRC );
			break;
		case 0xFFFFFFD7:	// _Bmp16
			CB_ML_Bmp16( SRC );
			break;
		case 0xFFFFFFD9:	// _BmpZoom
			CB_ML_BmpZoom( SRC );
			break;
		case 0xFFFFFFDA:	// _BmpRotate
			CB_ML_BmpRotate( SRC );
			break;
		case 0xFFFFFFDE:	// _BmpZoomRotate
			CB_ML_BmpZoomRotate( SRC );
			break;
			
		case 0xFFFFFFC0:	// _ClrVRAM
//			CB_ML_ClrVRAM();
			ML_clear_vram();
			break;
		case 0xFFFFFFC1:	// _ClrScreen
//			CB_ML_ClrScreen();
			if ( Is35E2 == 0 ) ML_clear_screen();
			break;
		case 0xFFFFFFC2:	// _DispVRAM
			CB_ML_DispVRAM( SRC );
//			ML_display_vram();
			break;
		case 0xFFFFFFC3:	// _Contrast
			CB_ML_SetContrast( SRC );
			break;
		case 0xFFFFFFD3:	// _Hscroll
			CB_ML_H_Scroll( SRC );
			break;
		case 0xFFFFFFD4:	// _Vscroll
			CB_ML_V_Scroll( SRC );
			break;
		case 0xFFFFFFDB:	// BmpSave
			CB_BmpSave( SRC );
			break;
		case 0xFFFFFFDC:	// BmpLoad
			CB_BmpLoad( SRC );
			break;
		case 0xFFFFFFDD:	// DrawMat
			CB_DrawMat( SRC );
			break;
	}
}

//----------------------------------------------------------------------------------------------
const short sintable1024[91]={0,18,36,54,71,89,107,125,143,160,178,195,213,230,248,265,282,299,316,333,350,367,384,400,416,433,449,465,481,496,512,527,543,558,573,587,602,616,630,644,658,672,685,698,711,724,737,749,761,773,784,796,807,818,828,839,849,859,868,878,887,896,904,912,920,928,935,943,949,956,962,968,974,979,984,989,994,998,1002,1005,1008,1011,1014,1016,1018,1020,1022,1023,1023,1024,1024};

int isin( int angle, int x ){
	while( angle < 0 ) angle+=360;
	if ( 360 < angle ) angle%=360;
	if ( 180 < angle ) x=-x;
	if ( (  90 < angle ) && ( angle <= 180 ) ) { angle=180-angle; }
	else
	if ( ( 180 < angle ) && ( angle <= 270 ) ) { angle-=180; }
	else
	if ( 270 < angle ) { angle=360-angle; }
	return ((int)sintable1024[angle]*x+512) >>10;
}
int icos( int angle, int x ){
//	if ( angle < 0 ) angle=-angle;
//	angle-=90;
//	if ( angle < 0 ) angle+=360;
	return isin( 90-angle, x );
}

void ML_circle2(int x, int y, int radius, ML_Color color, int start, int end, int n ) {	// start:end  0~360
	double	angle;
	int plot_x, plot_y, plot2_x, plot2_y;
	int	i,k,d,minus=1;
	char rd[]={0,4,12,20,20,20,36,40,56,56,60};
	int width = ( MLV_width );
	int draw = DrawType;
	if(radius < 0) return;
//	if ( start==end ) { CB_Error(ArgumentERR); return; }  // Argument error
	if ( start>end ) minus=-1;
	k=710*radius/113; if ( radius<=10 ) k=rd[radius];
	if ( n < 1 ) d=360/k*minus; else d=(end-start)/n;
	if ( d==0 ) d=minus;
	i=start;
	plot2_x= isin(i,radius)+x;
	plot2_y= icos(i,radius)+y;
	i+=d;
	if ( width )	ML_point(plot2_x, plot2_y, width, color);		// 1st plot
	else			ML_pixel(plot2_x, plot2_y, color);
	while ( 1 ) {
		plot_x= isin(i,radius)+x;
		plot_y= icos(i,radius)+y;
		if ( ( plot_x != plot2_x ) || ( plot_y != plot2_y ) ) {
			if ( draw==0 ) ML_line( plot2_x, plot2_y, plot_x, plot_y, color );	// connect
			else {
				if ( width )	ML_point(plot_x, plot_y, width, color);		// plot
				else			ML_pixel(plot_x, plot_y, color);
			}
		}
		if ( i == end ) break;
		plot2_x=plot_x;
		plot2_y=plot_y;
		i+=d;
		if ( minus>0 ) { if ( i>end ) i=end; } else { if ( i<end ) i=end; } 
	} ;
}

//----------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------
