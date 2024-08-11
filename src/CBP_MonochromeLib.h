/*
===============================================================================

 C.Basic interpreter for fx-9860G series    v0.99
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
//----------------------------------------------------------------------------------------------
void CB_ML_ClrVRAM() ; // ML_ClrVRAM
void CB_ML_ClrScreen() ; // ML_ClrScreen
void CB_ML_DispVRAM( char*SRC ) ; // ML_DispVRAM
int CB_ML_SetContrast( char *SRC ) ; // ML_Contrast
int CB_ML_GetContrast() ; // ML_Contrast
//----------------------------------------------------------------------------------------------
void CB_ML_Pixel( char *SRC ) ; // ML_Pixel x, y, color
void CB_ML_Point( char *SRC ) ; // ML_Point x, y, width, color
int CB_ML_PixelTest( char *SRC ) ; // ML_PixelTest( x, y)
void CB_ML_Paint( char *SRC ) ; // ML_Paint x, y, color
//----------------------------------------------------------------------------------------------
void CB_ML_Line( char *SRC ) ; // ML_Line x1, y1, x2, y2, color
void CB_ML_Horizontal( char *SRC ) ; // ML_Horizontal y, x1, x2, color
void CB_ML_Vertical( char *SRC ) ; // ML_Vertical x, y1, y2, color
//----------------------------------------------------------------------------------------------
void CB_ML_Rect( char *SRC ) ; // ML_Rect( x1,y1,x2,y2, border_width, border_color, fill_color)
//----------------------------------------------------------------------------------------------
void CB_ML_Polygon( char *SRC, int fill ) ; // ML_Polygon &Mat X, &Mat Y, nb_vertices, color
//----------------------------------------------------------------------------------------------
void CB_ML_Circle( char *SRC, int fill ) ; // ML_Circle x, y, radius, color
//----------------------------------------------------------------------------------------------
void CB_ML_Ellipse( char *SRC, int fill  ) ; // ML_Ellipse x, y, radius1, radius2, color
void CB_ML_EllipseInRect( char *SRC, int fill  ) ; // ML_EllipseInRect  x1, y1, x2, y2, color
//----------------------------------------------------------------------------------------------
void CB_ML_H_Scroll( char *SRC ) ; // ML_H_Scroll scroll
void CB_ML_V_Scroll( char *SRC ) ; // ML_V_Scroll scroll
//----------------------------------------------------------------------------------------------
void CB_ML_SetBmpMode( char *SRC, int *mode1, int *mode2 ) ;
void CB_ML_Bmp( char *SRC ) ; // ML_Bmp( &Mat A,  x, y, width, height[,O/A/X] [,C])
void CB_ML_Bmp8( char *SRC ) ; // ML_Bmp( &Mat A,  x, y [,O/A/X] [,C])
void CB_ML_Bmp16( char *SRC ) ; // ML_Bmp( &Mat A,  x, y [,O/A/X] [,C])

//----------------------------------------------------------------------------------------------
void CB_ML_BmpZoom( char *SRC ) ; // ML_BmpZoom( &Mat A,  x, y, width, height, zoomwidth, zoomheight  [,color][,rand])
void CB_ML_BmpRotate( char *SRC ) ; // ML_BmpZoom( &Mat A,  x, y, width, height, angle  [,color][,rand])
void CB_ML_DrawMat( char *SRC ) ; // ML_DrawMat( x, y, width, height, &Mat A,mx1,my1,mx2,my2, ML_Color color)
int CB_MLTest( char *SRC) ; // MLTest_command
void CB_ML_command( char *SRC, int c ) ; // ML_command

int isin( int angle, int x );
int icos( int angle, int x );
//----------------------------------------------------------------------------------------------
void CB_GetOprand2( char *SRC, int *px, int *py) ;
void CB_GetOprand4( char *SRC, int *px, int *py, int *px2, int *py2) ;
void CB_GetOprand_MLcolor( char *SRC, int *color) ;

