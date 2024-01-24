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

#include "MonochromeLib.h"
#include <stdlib.h>
#include "fxlib.h"
#include "mathf.h"

#include "fx_syscall.h"
extern  char *TVRAM;
extern  char *GVRAM;

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
	#define ML_BMP_OR_CL
	#define ML_BMP_AND_CL
	#define ML_BMP_XOR_CL
	#define ML_BMP_8_OR
	#define ML_BMP_8_AND
	#define ML_BMP_8_XOR
	#define ML_BMP_8_OR_CL
	#define ML_BMP_8_AND_CL
	#define ML_BMP_8_XOR_CL
	#define ML_BMP_16_OR
	#define ML_BMP_16_AND
	#define ML_BMP_16_XOR
	#define ML_BMP_16_OR_CL
	#define ML_BMP_16_AND_CL
	#define ML_BMP_16_XOR_CL
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
//----------------------------------------------------------------------------------------------

#ifdef ML_CLEAR_VRAM
void ML_clear_vram()
{
	int i, end, *pointer_long, vram;
	char *pointer_byte;
	vram = (int)ML_vram_adress();
	end = 4-vram&3;
	pointer_byte = (char*)vram;
	for(i=0 ; i<end ; i++) pointer_byte[i] = 0;
	pointer_long = (int*) (vram+end);
	for(i=0 ; i<255 ; i++) pointer_long[i] = 0;
	pointer_byte += 1020+end;
	end = vram&3;
	for(i=0 ; i<end ; i++) pointer_byte[i] = 0;
}
#endif

#ifdef ML_CLEAR_SCREEN
void ML_clear_screen()
{
	char *LCD_register_selector = (char*)0xB4000000, *LCD_data_register = (char*)0xB4010000;
	int i, j;
	for(i=0 ; i<64 ; i++)
	{
		*LCD_register_selector = 4;
		*LCD_data_register = i|192;
		*LCD_register_selector = 4;
		*LCD_data_register = 0;
		*LCD_register_selector = 7;
		for(j=0 ; j<16 ; j++) *LCD_data_register = 0;
	}
}
#endif

#ifdef ML_DISPLAY_VRAM
void ML_display_vram()
{
	char *LCD_register_selector = (char*)0xB4000000, *LCD_data_register = (char*)0xB4010000, *vram;
	int i, j;
	vram = ML_vram_adress();
	for(i=0 ; i<64 ; i++)
	{
		*LCD_register_selector = 4;
		*LCD_data_register = i|192;
		*LCD_register_selector = 4;
		*LCD_data_register = 0;
		*LCD_register_selector = 7;
		for(j=0 ; j<16 ; j++) *LCD_data_register = *vram++;
	}
}
#endif

#ifdef ML_SET_CONTRAST
void ML_set_contrast(unsigned char contrast)
{
	char *LCD_register_selector = (char*)0xB4000000, *LCD_data_register = (char*)0xB4010000;
	*LCD_register_selector = 6;
	*LCD_data_register = contrast;
}
#endif

#ifdef ML_GET_CONTRAST
unsigned char ML_get_contrast()
{
	char *LCD_register_selector = (char*)0xB4000000, *LCD_data_register = (char*)0xB4010000;
	*LCD_register_selector = 6;
	return *LCD_data_register;
}
#endif

#ifdef ML_PIXEL
void ML_pixel(int x, int y, ML_Color color)
{
	char* vram = ML_vram_adress();
	if(x&~127 || y&~63) return;
	switch(color)
	{
		case ML_BLACK:
		  black:
			vram[(y<<4)+(x>>3)] |= 128>>(x&7);
			break;
		case ML_WHITE:
		  white:
			vram[(y<<4)+(x>>3)] &= ~(128>>(x&7));
			break;
		case ML_XOR:
		  xor:
			vram[(y<<4)+(x>>3)] ^= 128>>(x&7);
			break;
		case ML_CHECKER:
		  checker:
			if(y&1^x&1) vram[(y<<4)+(x>>3)] &= ~(128>>(x&7));
			else vram[(y<<4)+(x>>3)] |= 128>>(x&7);
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

/*
		case ML_22DOT21X:
			switch ( (x&1)+(y&1)*2 ) {
				case 0:
					goto xor;
				case 1:
//					ML_pixel( x  , y  , ML_WHITE);
					ML_pixel( x+1, y  , ML_XOR);
//					ML_pixel( x  , y+1, ML_WHITE);
//					ML_pixel( x+1, y+1, ML_WHITE);
					break;
				case 2:
//					ML_pixel( x  , y  , ML_WHITE);
//					ML_pixel( x+1, y  , ML_WHITE);
					ML_pixel( x  , y+1, ML_XOR);
//					ML_pixel( x+1, y+1, ML_WHITE);
					break;
				case 3:
//					ML_pixel( x  , y  , ML_WHITE);
//					ML_pixel( x+1, y  , ML_WHITE);
//					ML_pixel( x  , y+1, ML_WHITE);
					ML_pixel( x+1, y+1, ML_XOR);
					break;
			}
			break;
		case ML_22DOT22X:
			switch ( (x&1)+(y&1)*2 ) {
				case 0:
					ML_pixel( x  , y  , ML_XOR);
//					ML_pixel( x+1, y  , ML_WHITE);
//					ML_pixel( x  , y+1, ML_WHITE);
					ML_pixel( x+1, y+1, ML_XOR);
					break;
				case 1:
//					ML_pixel( x  , y  , ML_WHITE);
					ML_pixel( x+1, y  , ML_XOR);
					ML_pixel( x  , y+1, ML_XOR);
//					ML_pixel( x+1, y+1, ML_WHITE);
					break;
				case 2:
//					ML_pixel( x  , y  , ML_WHITE);
					ML_pixel( x+1, y  , ML_XOR);
					ML_pixel( x  , y+1, ML_XOR);
//					ML_pixel( x+1, y+1, ML_WHITE);
					break;
				case 3:
					ML_pixel( x  , y  , ML_XOR);
//					ML_pixel( x+1, y  , ML_WHITE);
//					ML_pixel( x  , y+1, ML_WHITE);
					ML_pixel( x+1, y+1, ML_XOR);
					break;
			}
			break;
		case ML_22DOT23X:
			switch ( (x&1)+(y&1)*2 ) {
				case 0:
					ML_pixel( x  , y  , ML_XOR);
					ML_pixel( x+1, y  , ML_XOR);
					ML_pixel( x  , y+1, ML_XOR);
//					ML_pixel( x+1, y+1, ML_WHITE);
					break;
				case 1:
					ML_pixel( x  , y  , ML_XOR);
					ML_pixel( x+1, y  , ML_XOR);
//					ML_pixel( x  , y+1, ML_WHITE);
					ML_pixel( x+1, y+1, ML_XOR);
					break;
				case 2:
					ML_pixel( x  , y  , ML_XOR);
//					ML_pixel( x+1, y  , ML_WHITE);
					ML_pixel( x  , y+1, ML_XOR);
					ML_pixel( x+1, y+1, ML_XOR);
					break;
				case 3:
//					ML_pixel( x  , y  , ML_WHITE);
					ML_pixel( x+1, y  , ML_XOR);
					ML_pixel( x  , y+1, ML_XOR);
					ML_pixel( x+1, y+1, ML_XOR);
					break;
			}
			break;
		case ML_22DOT24X:
			ML_pixel( x  , y  , ML_XOR);
			ML_pixel( x+1, y  , ML_XOR);
			ML_pixel( x  , y+1, ML_XOR);
			ML_pixel( x+1, y+1, ML_XOR);
			break;
*/
	}
}
#endif

#ifdef ML_POINT
void ML_point(int x, int y, int width, ML_Color color)
{
	if(width < 1) return;
	if(width == 1) ML_pixel(x, y, color);
	else
	{
		int padding, pair;
		padding = width>>1;
		pair = !(width&1);
		ML_rectangle(x-padding+pair, y-padding+pair, x+padding, y+padding, 0, 0, color);
	}
}
#endif

#ifdef ML_PIXEL_TEST
ML_Color ML_pixel_test(int x, int y)
{
	char *vram, byte;
	if(x&~127 || y&~63) return ML_TRANSPARENT;
	vram = ML_vram_adress();
	byte = 1<<(7-(x&7));
	return (vram[(y<<4)+(x>>3)] & byte ? ML_BLACK : ML_WHITE);
}
ML_Color ML_pixel_test_TVRAM(int x, int y)
{
	char *vram, byte;
	if(x&~127 || y&~63) return ML_TRANSPARENT;
	vram = TVRAM;
	byte = 1<<(7-(x&7));
	return (vram[(y<<4)+(x>>3)] & byte ? ML_BLACK : ML_WHITE);
}
ML_Color ML_pixel_test_GVRAM(int x, int y)
{
	char *vram, byte;
	if(x&~127 || y&~63) return ML_TRANSPARENT;
	vram = GVRAM;
	byte = 1<<(7-(x&7));
	return (vram[(y<<4)+(x>>3)] & byte ? ML_BLACK : ML_WHITE);
}
#endif

#ifdef ML_LINE

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
void ML_horizontal_line(int y, int x1, int x2, ML_Color color)
{
    int i;
    char checker;
    char* vram = ML_vram_adress();
    if(y&~63 || (x1<0 && x2<0) || (x1>127 && x2>127)) return;
    if(x1 > x2)
    {
    	i = x1;
    	x1 = x2;
    	x2 = i;
    }
    if(x1 < 0) x1 = 0;
    if(x2 > 127) x2 = 127;
    switch(color)
    {
    	case ML_BLACK:
    	  black:
			if(x1>>3 != x2>>3)
			{
				vram[(y<<4)+(x1>>3)] |= 255 >> (x1&7);
				vram[(y<<4)+(x2>>3)] |= 255 << 7-(x2&7);
				for(i=(x1>>3)+1 ; i<x2>>3 ; i++)
					vram[(y<<4) + i] = 255;
			}
			else vram[(y<<4)+(x1>>3)] |= (255>>(x1%8 + 7-x2%8))<<(7-(x2&7));
			break;
		case ML_WHITE:
		  white:
			if(x1>>3 != x2>>3)
			{
				vram[(y<<4)+(x1>>3)] &= 255 << 8-(x1&7);
				vram[(y<<4)+(x2>>3)] &= 255 >> 1+(x2&7);
				for(i=(x1>>3)+1 ; i<x2>>3 ; i++)
					vram[(y<<4) + i] = 0;
			}
			else vram[(y<<4)+(x1>>3)] &= (255<<8-(x1&7)) | (255>>1+(x2&7));
			break;
		case ML_XOR:
		  xor:
			if(x1>>3 != x2>>3)
			{
				vram[(y<<4)+(x1>>3)] ^= 255 >> (x1&7);
				vram[(y<<4)+(x2>>3)] ^= 255 << 7-(x2&7);
				for(i=(x1>>3)+1 ; i<(x2>>3) ; i++)
					vram[(y<<4) + i] ^= 255;
			}
			else vram[(y<<4)+(x1>>3)] ^= (255>>((x1&7) + 7-(x2&7)))<<(7-(x2&7));
			break;
		case ML_CHECKER:
		  checker:
			checker = (y&1 ? 85 : 170);
			if(x1>>3 != x2>>3)
			{
				vram[(y<<4)+(x1>>3)] &= 255 << 8-(x1&7);
				vram[(y<<4)+(x2>>3)] &= 255 >> 1+(x2&7);
				vram[(y<<4)+(x1>>3)] |= checker & 255>>(x1&7);
				vram[(y<<4)+(x2>>3)] |= checker & 255<<7-(x2&7);
				for(i=(x1>>3)+1 ; i<x2>>3 ; i++)
					vram[(y<<4) + i] = checker;
			}
			else
			{
				vram[(y<<4)+(x1>>3)] &= (255<<8-(x1&7)) | (255>>1+(x2&7));
				vram[(y<<4)+(x1>>3)] |= checker & (255>>(x1%8 + 7-x2%8))<<(7-(x2&7));
			}
			break;
			
		case ML_RANDPX:
			for ( i=x1; i<=x2; i++ ) {
//				if ( MLV_rand >= rand() ) ML_pixel(i, y, ML_BLACK );
				if ( MLV_rand >= rand() ) 	vram[(y<<4)+(i>>3)] |= 128>>(i&7);		// BLACK
				else						vram[(y<<4)+(i>>3)] &= ~(128>>(i&7));	// WHITE
			}
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
/*
		case ML_22DOT1X:
				for ( i=x1; i<=x2; i+=2) ML_pixel( i, y, ML_22DOT1X );
				break;
		case ML_22DOT2X:
				for ( i=x1; i<=x2; i+=2) ML_pixel( i, y, ML_22DOT2X );
				break;
		case ML_22DOT3X:
				for ( i=x1; i<=x2; i+=2) ML_pixel( i, y, ML_22DOT3X );
				break;
		case ML_22DOT4X:
				for ( i=x1; i<=x2; i+=2) ML_pixel( i, y, ML_22DOT4X );
				break;
*/
    }
}

#endif

#ifdef ML_VERTICAL_LINE
void ML_vertical_line(int x, int y1, int y2, ML_Color color)
{
    int i, j;
    char checker, byte, *vram = ML_vram_adress();
	if(x&~127 || (y1<0 && y2<0) || (y1>63 && y2>63)) return;
	if(y1 > y2)
	{
		int tmp = y1;
		y1 = y2;
		y2 = tmp;
	}
	if(y1 < 0) y1 = 0;
	if(y2 > 63) y2 = 63;

	i = (y1<<4)+(x>>3);
	j = (y2<<4)+(x>>3);
	switch(color)
	{
		case ML_BLACK:
		  black:
			byte = 128>>(x&7);
			for( ; i<=j ; i+=16)
				vram[i] |= byte;
			break;
		case ML_WHITE:
		  white:
			byte = ~(128>>(x&7));
			for( ; i<=j ; i+=16)
				vram[i] &= byte;
			break;
		case ML_XOR:
		  xor:
			byte = 128>>(x&7);
			for( ; i<=j ; i+=16)
				vram[i] ^= byte;
			break;
		case ML_CHECKER:
		  checker:
			byte = 128>>(x&7);
			checker = y1&1^x&1;
			for( ; i<=j ; i+=16)
			{
				if(checker) vram[i] &= ~byte;
				else vram[i] |= byte;
				checker = !checker;
			}
			break;
			
		case ML_RANDPX:
			for ( i=y1; i<=y2; i++ ) {
				if ( MLV_rand >= rand() ) 	vram[(i<<4)+(x>>3)] |= 128>>(x&7);		// BLACK
				else						vram[(i<<4)+(x>>3)] &= ~(128>>(x&7));	// WHITE
			}
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
/*
		case ML_22DOT1X:
				for ( i=y1; i<=y2; i+=2) ML_pixel( x, i, ML_22DOT1X );
				break;
		case ML_22DOT2X:
				for ( i=y1; i<=y2; i+=2) ML_pixel( x, i, ML_22DOT2X );
				break;
		case ML_22DOT3X:
				for ( i=y1; i<=y2; i+=2) ML_pixel( x, i, ML_22DOT3X );
				break;
		case ML_22DOT4X:
				for ( i=y1; i<=y2; i+=2) ML_pixel( x, i, ML_22DOT4X );
				break;
*/
	}
}
#endif

#ifdef ML_RECTANGLE
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
#endif

#ifdef ML_ELLIPSE_IN_RECT
void ML_ellipse_in_rect(int x1, int y1, int x2, int y2, ML_Color color)
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
	ML_ellipse(x1+radius1, y1+radius2, radius1, radius2, color);
}
#endif

#ifdef ML_FILLED_ELLIPSE
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
#endif

#ifdef ML_FILLED_ELLIPSE_IN_RECT
void ML_filled_ellipse_in_rect(int x1, int y1, int x2, int y2, ML_Color color)
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
	ML_filled_ellipse(x1+radius1, y1+radius2, radius1, radius2, color);
}
#endif

#ifdef ML_HORIZONTAL_SCROLL
void ML_horizontal_scroll(int scroll)
{
	int i, j;
	char line[16], shift, *vram;
	unsigned char next;
	unsigned short word;
	vram = ML_vram_adress();
	scroll %= 128;
	shift = 8-(scroll&7);
	for(i=0 ; i<64 ; i++)
	{
		for(j=0 ; j<16 ; j++) line[j] = vram[(i<<4)+((j-(scroll>>3)+15)&15)];
		next = line[15];
		vram[(i<<4)+15] = 0;
		for(j=15 ; j>0 ; j--)
		{
			word = next << shift;
			next = line[j-1];
			vram[(i<<4)+j]  |= *((char*)&word+1);
			vram[(i<<4)+j-1] = *((char*)&word);
		}
		word = next << shift;
		vram[(i<<4)]    |= *((char*)&word+1);
		vram[(i<<4)+15] |= *((char*)&word);
	}
}

void ML_horizontal_scroll2(int scroll, int x1, int y1, int x2, int y2)
{
	int i, j;
	char line[32], shift, *vram;
	unsigned char next;
	unsigned short word;
	int height,width;
	int tmp;
	int xscroll,xmask;
	int j1,j2;
	unsigned char buf1[1024],buf2[1024];
	DISPBOX box,box2;
	vram = ML_vram_adress();

	if(x1&~127 || (y1<0 && y2<0) || (y1>63 && y2>63)) return;
	if(y1 > y2) {
		tmp = y1;
		y1 = y2;
		y2 = tmp;
	}
	if(y1 < 0) y1 = 0;
	if(y2 > 63) y2 = 63;
	height=y2-y1+1;
	
	if(x1 > x2) {
		tmp = x1;
		x1 = x2;
		x2 = tmp;
	}
	if(x1 < 0) x1 = 0;
	if(x2 > 127) x2 = 127;
	width=x2-x1+1;

	scroll %= width;
	shift = 8-(scroll&7);
	j1=x1>>3; j2=x2>>3;
	if ( ( (x1&7)==0 ) && ( (x2&7)==7 ) && ( j1 != j2) ) {
		for(i=y1 ; i<=y2 ; i++)
		{
			width=j2-j1+1;
			xscroll=(scroll>>3)-(width-1);
			for(j=j1 ; j<=j2 ; j++) {
				tmp = j-xscroll;
				if ( tmp < j1 ) tmp += width;
				if ( tmp > j2 ) tmp -= width;
				line[j] = vram[(i<<4)+tmp];
			}
			next = line[j2];
			vram[(i<<4)+j2] = 0;
			for(j=j2 ; j>j1 ; j--)
			{
				word = next << shift;
				next = line[j-1];
				vram[(i<<4)+j]  |= *((char*)&word+1);
				vram[(i<<4)+j-1] = *((char*)&word);
			}
			word = next << shift;
			vram[(i<<4)+j1] |= *((char*)&word+1);
			vram[(i<<4)+j2] |= *((char*)&word);
		}
	} else { 
		if ( scroll==0 ) return;
		if ( scroll > 0 ) {
			box.left   =x2-scroll;
			box.right  =x2;
			box.top    =y1;
			box.bottom =y2;
			Bdisp_ReadArea_VRAM( &box,  buf1);
			box2.left  =x1;
			box2.right =x2-scroll;
			box2.top   =y1;
			box2.bottom=y2;
			Bdisp_ReadArea_VRAM( &box2, buf2);
			ML_rectangle(x1, y1, x2, y2, 0, ML_TRANSPARENT, ML_WHITE);
			ML_bmp_or( buf1, x1, y1, scroll, height);
			ML_bmp_or( buf2, x1+scroll, y1, width-scroll, height);
		} else {
			scroll=-scroll;
			box.left   =x1+scroll;
			box.right  =x2;
			box.top    =y1;
			box.bottom =y2;
			Bdisp_ReadArea_VRAM( &box,  buf1);
			box2.left  =x1;
			box2.right =x1+scroll;
			box2.top   =y1;
			box2.bottom=y2;
			Bdisp_ReadArea_VRAM( &box2, buf2);
			ML_rectangle(x1, y1, x2, y2, 0, ML_TRANSPARENT, ML_WHITE);
			ML_bmp_or( buf1, x1, y1, width-scroll, height);
			ML_bmp_or( buf2, x2-scroll, y1, scroll, height);
		}
/*		
		for(i=y1 ; i<=y2 ; i++) {		// 
			for(j=j1 ; j<=j2 ; j++) line[j] = vram[(i<<4)+j];
			for(j=x1 ; j<=x2 ; j++) {
				tmp = j - scroll;
				if ( tmp < x1 ) tmp += width;
				if ( tmp > x2 ) tmp -= width;
				if ( line[(tmp>>3)] & (1<<(7-(tmp&7))) )		// pixel test
						vram[(i<<4)+(j>>3)] |= 128>>(j&7);		// pixel black
				else	vram[(i<<4)+(j>>3)] &= ~(128>>(j&7));	// pixel white
			}
		}
*/
	}
}

#endif

#ifdef ML_VERTICAL_SCROLL
void ML_vertical_scroll(int scroll)
{
	int i, j;
	char column[64], *vram = ML_vram_adress();
	scroll %= 64;
	for(i=0 ; i<16 ; i++)
	{
		for(j=0 ; j<64 ; j++) column[j] = vram[(j<<4)+i];
		for(j=0 ; j<64 ; j++) vram[(j<<4)+i] = column[(j-scroll+64)&63];
	}
}

void ML_vertical_scroll2(int scroll, int x1, int y1, int x2, int y2)
{
	int i, j;
	char column[64], *vram = ML_vram_adress();
	int height,width;
	int tmp;
	int xshift,xmask;
	int x,y,i1,i2;
	
	if(x1&~127 || (y1<0 && y2<0) || (y1>63 && y2>63)) return;
	if(y1 > y2) {
		tmp = y1;
		y1 = y2;
		y2 = tmp;
	}
	if(y1 < 0) y1 = 0;
	if(y2 > 63) y2 = 63;
	height=y2-y1+1;
	
	if(x1 > x2) {
		tmp = x1;
		x1 = x2;
		x2 = tmp;
	}
	if(x1 < 0) x1 = 0;
	if(x2 > 127) x2 = 127;
	width=x2-x1+1;
	
	scroll %= height;
	i1=x1>>3; i2=x2>>3;
	xshift=x1&7;
	if ( xshift>0 ) {		// left bit vscroll
		xmask=255>>xshift;
		if ( i1==i2 ) {
			xmask &= (255<<(7-(x2&7)));
		}
		i=i1;
			for(j=y1 ; j<=y2 ; j++) column[j] = vram[(j<<4)+i];
			for(j=y1 ; j<=y2 ; j++) {
				vram[(j<<4)+i] &=(~xmask);
				tmp = j - scroll;
				if ( tmp < y1 ) tmp += height;
				if ( tmp > y2 ) tmp -= height;
				vram[(j<<4)+i] |= column[tmp] & xmask;
			}
		x1=x1-xshift+8;
		i1=x1>>3;
	}
	if ( x1>x2 ) return;
	
	xshift=x2&7;
	if ( xshift<7 ) {		// right bit vscroll
		xmask=~(255>>xshift);
		i=i2;
			for(j=y1 ; j<=y2 ; j++) column[j] = vram[(j<<4)+i];
			for(j=y1 ; j<=y2 ; j++) {
				vram[(j<<4)+i] &=(~xmask);
				tmp = j - scroll;
				if ( tmp < y1 ) tmp += height;
				if ( tmp > y2 ) tmp -= height;
				vram[(j<<4)+i] |= column[tmp] & xmask;
			}
		x2=x2-xshift;
		i2=x2>>3;
	}
	if ( i1>i2 ) return;

	for(i=i1 ; i<=i2 ; i++) {	// center byte vscroll
			for(j=y1 ; j<=y2 ; j++) column[j] = vram[(j<<4)+i];
			for(j=y1 ; j<=y2 ; j++) {
				tmp = j - scroll;
				if ( tmp < y1 ) tmp += height;
				if ( tmp > y2 ) tmp -= height;
				vram[(j<<4)+i] = column[tmp];
			}
	}
}
#endif

#ifdef ML_BMP_OR
void ML_bmp_or(const unsigned char *bmp, int x, int y, int width, int height)
{
	unsigned short line;
	char shift, *screen, *p=(char*)&line;
	int i, j, begin=0, end=height, real_width=(width-1>>3<<3)+8;
	if(!bmp || x<0 || x>128-width || y<1-height || y>63 || width<1 || height<1) return;
	if(y < 0) begin = -y;
	if(y+height > 64) end = 64-y;
	shift = 8-(x&7);
	screen = ML_vram_adress()+(y+begin<<4)+(x>>3);
	for(i=begin ; i<end ; i++)
	{
		for(j=0 ; j<width-1>>3 ; j++)
		{
			line = bmp[i*(real_width>>3)+j]<<shift;
			screen[j] |= *p;
			if(shift!=8) screen[j+1] |= *(p+1);
		}
		line = (bmp[i*(real_width>>3)+j] & -1<<(real_width-width))<<shift;
		screen[j] |= *p;
		if(shift!=8 && x+real_width<129) screen[j+1] |= *(p+1);
		screen += 16;
	}
}
#endif

#ifdef ML_BMP_AND
void ML_bmp_and(const unsigned char *bmp, int x, int y, int width, int height)
{
	unsigned short line;
	char shift, *screen, *p=(char*)&line;
	int i, j, begin=0, end=height, real_width=(width-1>>3<<3)+8;
	if(!bmp || x<0 || x>128-width || y<1-height || y>63 || width<1 || height<1) return;
	if(y < 0) begin = -y;
	if(y+height > 64) end = 64-y;
	shift = 8-(x&7);
	screen = ML_vram_adress()+(y+begin<<4)+(x>>3);
	for(i=begin ; i<end ; i++)
	{
		for(j=0 ; j<width-1>>3 ; j++)
		{
			line = ~((unsigned char)~bmp[i*(real_width>>3)+j]<<shift);
			screen[j] &= *p;
			if(shift!=8) screen[j+1] &= *(p+1);
		}
		line = ~((unsigned char)~(bmp[i*(real_width>>3)+j] | (unsigned char)-1>>8-(width&7))<<shift);
		screen[j] &= *p;
		if(shift!=8 && x+real_width<129) screen[j+1] &= *(p+1);
		screen += 16;
	}
}
#endif

#ifdef ML_BMP_XOR
void ML_bmp_xor(const unsigned char *bmp, int x, int y, int width, int height)
{
	unsigned short line;
	char shift, *screen, *p=(char*)&line;
	int i, j, begin=0, end=height, real_width=(width-1>>3<<3)+8;
	if(!bmp || x<0 || x>128-width || y<1-height || y>63 || width<1 || height<1) return;
	if(y < 0) begin = -y;
	if(y+height > 64) end = 64-y;
	shift = 8-(x&7);
	screen = ML_vram_adress()+(y+begin<<4)+(x>>3);
	for(i=begin ; i<end ; i++)
	{
		for(j=0 ; j<width-1>>3 ; j++)
		{
			line = bmp[i*(real_width>>3)+j]<<shift;
			screen[j] ^= *p;
			if(shift!=8) screen[j+1] ^= *(p+1);
		}
		line = (bmp[i*(real_width>>3)+j] & -1<<(real_width-width))<<shift;
		screen[j] ^= *p;
		if(shift!=8 && x+real_width<129) screen[j+1] ^= *(p+1);
		screen += 16;
	}
}
#endif

#ifdef ML_BMP_OR_CL
void ML_bmp_or_cl(const unsigned char *bmp, int x, int y, int width, int height)
{
	unsigned short line;
	char shift, *screen, *p;
	int i, j, real_width, begin_x, end_x, begin_y, end_y;
	char bool1=1, bool2=1, bool3;
	if(!bmp || x<1-width || x>127 || y<1-height || y>63 || height<1 || width<1) return;
	p = (char*)&line;
	real_width = (width-1>>3<<3)+8;
	if(y < 0) begin_y = -y;
	else begin_y = 0;
	if(y+height > 64) end_y = 64-y;
	else end_y = height;
	shift = 8-(x&7);
	if(x<0)
	{
		begin_x = -x>>3;
		if(shift != 8) bool1 = 0;
	} else begin_x = 0;
	if(x+real_width > 128) end_x = 15-(x>>3), bool2 = 0;
	else end_x = real_width-1>>3;
	bool3 = (end_x == real_width-1>>3);
	screen = ML_vram_adress()+(y+begin_y<<4)+(x>>3);

	for(i=begin_y ; i<end_y ; i++)
	{
		if(begin_x < end_x)
		{
			line = bmp[i*(real_width>>3)+begin_x] << shift;
			if(bool1) screen[begin_x] |= *p;
			if(shift!=8) screen[begin_x+1] |= *(p+1);
			for(j=begin_x+1 ; j<end_x ; j++)
			{
				line = bmp[i*(real_width>>3)+j] << shift;
				screen[j] |= *p;
				if(shift!=8) screen[j+1] |= *(p+1);
			}
		}
		line = bmp[i*(real_width>>3)+end_x];
		if(bool3) line &= -1<<real_width-width;
		line <<= shift;
		if(begin_x < end_x || bool1) screen[end_x] |= *p;
		if(bool2) screen[end_x+1] |= *(p+1);
		screen += 16;
	}
}
#endif

#ifdef ML_BMP_AND_CL
void ML_bmp_and_cl(const unsigned char *bmp, int x, int y, int width, int height)
{
	unsigned short line;
	char shift, *screen, *p;
	int i, j, real_width, begin_x, end_x, begin_y, end_y;
	char bool1=1, bool2=1, bool3;
	if(!bmp || x<1-width || x>127 || y<1-height || y>63 || height<1 || width<1) return;
	p = (char*)&line;
	real_width = (width-1>>3<<3)+8;
	if(y < 0) begin_y = -y;
	else begin_y = 0;
	if(y+height > 64) end_y = 64-y;
	else end_y = height;
	shift = 8-(x&7);
	if(x<0)
	{
		begin_x = -x>>3;
		if(shift != 8) bool1 = 0;
	} else begin_x = 0;
	if(x+real_width > 128) end_x = 15-(x>>3), bool2 = 0;
	else end_x = real_width-1>>3;
	bool3 = (end_x == real_width-1>>3);
	screen = ML_vram_adress()+(y+begin_y<<4)+(x>>3);

	for(i=begin_y ; i<end_y ; i++)
	{
		if(begin_x < end_x)

		{
			line = ~((unsigned char)~bmp[i*(real_width>>3)+begin_x]<<shift);
			if(bool1) screen[begin_x] &= *p;
			if(shift!=8) screen[begin_x+1] &= *(p+1);
			for(j=begin_x+1 ; j<end_x ; j++)
			{
				line = ~((unsigned char)~bmp[i*(real_width>>3)+j]<<shift);
				screen[j] &= *p;
				if(shift!=8) screen[j+1] &= *(p+1);
			}
		}
		line = (unsigned char)~bmp[i*(real_width>>3)+end_x];
		if(bool3) line &= -1<<real_width-width;
		line = ~(line << shift);
		if(begin_x < end_x || bool1) screen[end_x] &= *p;
		if(bool2) screen[end_x+1] &= *(p+1);
		screen += 16;
	}
}
#endif

#ifdef ML_BMP_XOR_CL
void ML_bmp_xor_cl(const unsigned char *bmp, int x, int y, int width, int height)
{
	unsigned short line;
	char shift, *screen, *p;
	int i, j, real_width, begin_x, end_x, begin_y, end_y;
	char bool1=1, bool2=1, bool3;
	if(!bmp || x<1-width || x>127 || y<1-height || y>63 || height<1 || width<1) return;
	p = (char*)&line;
	real_width = (width-1>>3<<3)+8;
	if(y < 0) begin_y = -y;
	else begin_y = 0;
	if(y+height > 64) end_y = 64-y;
	else end_y = height;
	shift = 8-(x&7);
	if(x<0)
	{
		begin_x = -x>>3;
		if(shift != 8) bool1 = 0;
	} else begin_x = 0;
	if(x+real_width > 128) end_x = 15-(x>>3), bool2 = 0;
	else end_x = real_width-1>>3;
	bool3 = (end_x == real_width-1>>3);
	screen = ML_vram_adress()+(y+begin_y<<4)+(x>>3);

	for(i=begin_y ; i<end_y ; i++)
	{
		if(begin_x < end_x)
		{
			line = bmp[i*(real_width>>3)+begin_x] << shift;
			if(bool1) screen[begin_x] ^= *p;
			if(shift!=8) screen[begin_x+1] ^= *(p+1);
			for(j=begin_x+1 ; j<end_x ; j++)
			{
				line = bmp[i*(real_width>>3)+j] << shift;
				screen[j] ^= *p;
				if(shift!=8) screen[j+1] ^= *(p+1);
			}
		}
		line = bmp[i*(real_width>>3)+end_x];
		if(bool3) line &= -1<<real_width-width;
		line <<= shift;
		if(begin_x < end_x || bool1) screen[end_x] ^= *p;
		if(bool2) screen[end_x+1] ^= *(p+1);
		screen += 16;
	}
}

#endif

#ifdef ML_BMP_8_OR
void ML_bmp_8_or(const unsigned char *bmp, int x, int y)
{
	unsigned short line;
	char i, shift, begin=0, end=8, *screen, *p=(char*)&line;
	if(!bmp || x<0 || x>120 || y<-7 || y>63) return;
	if(y < 0) begin = -y;
	if(y > 56) end = 64-y;
	shift = 8-(x&7);
	screen = ML_vram_adress()+(y+begin<<4)+(x>>3);
	for(i=begin ; i<end ; i++)
	{
		line = bmp[i]<<shift;
		screen[0] |= *p;
		if(shift!=8) screen[1] |= *(p+1);
		screen += 16;
	}
}
#endif

#ifdef ML_BMP_8_AND
void ML_bmp_8_and(const unsigned char *bmp, int x, int y)
{
	unsigned short line;
	char i, shift, begin=0, end=8, *screen, *p=(char*)&line;
	if(!bmp || x<0 || x>120 || y<-7 || y>63) return;
	if(y < 0) begin = -y;
	if(y > 56) end = 64-y;
	shift = 8-(x&7);
	screen = ML_vram_adress()+(y+begin<<4)+(x>>3);
	for(i=begin ; i<end ; i++)
	{
		line = ~((unsigned char)~bmp[i]<<shift);
		screen[0] &= *p;
		if(shift!=8) screen[1] &= *(p+1);
		screen += 16;
	}
}
#endif

#ifdef ML_BMP_8_XOR
void ML_bmp_8_xor(const unsigned char *bmp, int x, int y)
{
	unsigned short line;
	char i, shift, begin=0, end=8, *screen, *p=(char*)&line;
	if(!bmp || x<0 || x>120 || y<-7 || y>63) return;
	if(y < 0) begin = -y;
	if(y > 56) end = 64-y;
	shift = 8-(x&7);
	screen = ML_vram_adress()+(y+begin<<4)+(x>>3);
	for(i=begin ; i<end ; i++)
	{
		line = bmp[i]<<shift;
		screen[0] ^= *p;
		if(shift<8) screen[1] ^= *(p+1);
		screen += 16;
	}
}
#endif

#ifdef ML_BMP_8_OR_CL
void ML_bmp_8_or_cl(const unsigned char *bmp, int x, int y)
{
	unsigned short line;
	char i, shift, begin=0, end=8, bool1=1, bool2=1, *screen, *p=(char*)&line;
	if(!bmp || x<-7 || x>127 || y<-7 || y>63) return;
	if(y < 0) begin = -y;
	if(y > 56) end = 64-y;
	shift = 8-(x&7);
	if(x < 0) bool1 = 0;
	if(x>120 || shift==8) bool2 = 0;
	screen = ML_vram_adress()+(y+begin<<4)+(x>>3);
	for(i=begin ; i<end ; i++)
	{
		line = bmp[i]<<shift;
		if(bool1) screen[0] |= *p;
		if(bool2) screen[1] |= *(p+1);
		screen += 16;
	}
}
#endif

#ifdef ML_BMP_8_AND_CL
void ML_bmp_8_and_cl(const unsigned char *bmp, int x, int y)
{
	unsigned short line;
	char i, shift, begin=0, end=8, bool1=1, bool2=1, *screen, *p=(char*)&line;
	if(!bmp || x<-7 || x>127 || y<-7 || y>63) return;
	if(y < 0) begin = -y;
	if(y > 56) end = 64-y;
	shift = 8-(x&7);
	if(x < 0) bool1 = 0;
	if(x>120 || shift==8) bool2 = 0;
	screen = ML_vram_adress()+(y+begin<<4)+(x>>3);
	for(i=begin ; i<end ; i++)
	{
		line = ~((unsigned char)~bmp[i]<<shift);
		if(bool1) screen[0] &= *p;
		if(bool2) screen[1] &= *(p+1);
		screen += 16;
	}
}
#endif

#ifdef ML_BMP_8_XOR_CL
void ML_bmp_8_xor_cl(const unsigned char *bmp, int x, int y)
{
	unsigned short line;
	char i, shift, begin=0, end=8, bool1=1, bool2=1, *screen, *p=(char*)&line;
	if(!bmp || x<-7 || x>127 || y<-7 || y>63) return;
	if(y < 0) begin = -y;
	if(y > 56) end = 64-y;
	shift = 8-(x&7);
	if(x < 0) bool1 = 0;
	if(x>120 || shift==8) bool2 = 0;
	screen = ML_vram_adress()+(y+begin<<4)+(x>>3);
	for(i=begin ; i<end ; i++)
	{
		line = bmp[i]<<shift;
		if(bool1) screen[0] ^= *p;
		if(bool2) screen[1] ^= *(p+1);
		screen += 16;
	}
}
#endif

#ifdef ML_BMP_16_OR
void ML_bmp_16_or(const unsigned short *bmp, int x, int y)
{
	unsigned long line;
	char i, shift, begin=0, end=16, *screen, *p=(char*)&line+1;
	if(!bmp || x<0 || x>112 || y<-15 || y>63) return;
	if(y < 0) begin = -y;
	if(y > 48) end = 64-y;
	shift = 8-(x&7);
	screen = ML_vram_adress()+(y+begin<<4)+(x>>3);
	for(i=begin ; i<end ; i++)
	{
		line = bmp[i]<<shift;
		screen[0] |= *p;
		screen[1] |= *(p+1);
		if(shift!=8) screen[2] |= *(p+2);
		screen += 16;
	}
}
#endif

#ifdef ML_BMP_16_AND
void ML_bmp_16_and(const unsigned short *bmp, int x, int y)
{
	unsigned long line;
	char i, shift, begin=0, end=16, *screen, *p=(char*)&line+1;
	if(!bmp || x<0 || x>112 || y<-15 || y>63) return;
	if(y < 0) begin = -y;
	if(y > 48) end = 64-y;
	shift = 8-(x&7);
	screen = ML_vram_adress()+(y+begin<<4)+(x>>3);
	for(i=begin ; i<end ; i++)
	{
		line = ~((unsigned short)~bmp[i]<<shift);
		screen[0] &= *p;
		screen[1] &= *(p+1);
		if(shift!=8) screen[2] &= *(p+2);
		screen += 16;
	}
}
#endif

#ifdef ML_BMP_16_XOR
void ML_bmp_16_xor(const unsigned short *bmp, int x, int y)
{
	unsigned long line;
	char i, shift, begin=0, end=16, *screen, *p=(char*)&line+1;
	if(!bmp || x<0 || x>112 || y<-15 || y>63) return;
	if(y < 0) begin = -y;
	if(y > 48) end = 64-y;
	shift = 8-(x&7);
	screen = ML_vram_adress()+(y+begin<<4)+(x>>3);
	for(i=begin ; i<end ; i++)
	{
		line = bmp[i]<<shift;
		screen[0] ^= *p;
		screen[1] ^= *(p+1);
		if(shift!=8) screen[2] ^= *(p+2);
		screen += 16;
	}
}
#endif

#ifdef ML_BMP_16_OR_CL
void ML_bmp_16_or_cl(const unsigned short *bmp, int x, int y)
{
	unsigned long line;
	char i, shift, begin=0, end=16, bool1=1, bool2=1, bool3=1, *screen, *p=(char*)&line+1;
	if(!bmp || x<-15 || x>127 || y<-15 || y>63) return;
	if(y < 0) begin = -y;
	if(y > 48) end = 64-y;
	shift = 8-(x&7);
	if(x < 0) bool1 = 0;
	if(x<-8 || x>119) bool2 = 0;
	if(x>111 || shift==8) bool3 = 0;
	screen = ML_vram_adress()+(y+begin<<4)+(x>>3);
	for(i=begin ; i<end ; i++)
	{
		line = bmp[i]<<shift;
		if(bool1) screen[0] |= *p;
		if(bool2) screen[1] |= *(p+1);
		if(bool3) screen[2] |= *(p+2);
		screen += 16;
	}
}
#endif

#ifdef ML_BMP_16_AND_CL
void ML_bmp_16_and_cl(const unsigned short *bmp, int x, int y)
{
	unsigned long line;
	char i, shift, begin=0, end=16, bool1=1, bool2=1, bool3=1, *screen, *p=(char*)&line+1;
	if(!bmp || x<-15 || x>127 || y<-15 || y>63) return;
	if(y < 0) begin = -y;
	if(y > 48) end = 64-y;
	shift = 8-(x&7);
	if(x < 0) bool1 = 0;
	if(x<-8 || x>119) bool2 = 0;
	if(x>111 || shift==8) bool3 = 0;
	screen = ML_vram_adress()+(y+begin<<4)+(x>>3);
	for(i=begin ; i<end ; i++)
	{
		line = ~((unsigned short)~bmp[i]<<shift);
		if(bool1) screen[0] &= *p;
		if(bool2) screen[1] &= *(p+1);
		if(bool3) screen[2] &= *(p+2);
		screen += 16;
	}
}
#endif

#ifdef ML_BMP_16_XOR_CL
void ML_bmp_16_xor_cl(const unsigned short *bmp, int x, int y)
{
	unsigned long line;
	char i, shift, begin=0, end=16, bool1=1, bool2=1, bool3=1, *screen, *p=(char*)&line+1;
	if(!bmp || x<-15 || x>127 || y<-15 || y>63) return;
	if(y < 0) begin = -y;
	if(y > 48) end = 64-y;
	shift = 8-(x&7);
	if(x < 0) bool1 = 0;
	if(x<-8 || x>119) bool2 = 0;
	if(x>111 || shift==8) bool3 = 0;
	screen = ML_vram_adress()+(y+begin<<4)+(x>>3);
	for(i=begin ; i<end ; i++)
	{
		line = bmp[i]<<shift;
		if(bool1) screen[0] ^= *p;
		if(bool2) screen[1] ^= *(p+1);
		if(bool3) screen[2] ^= *(p+2);
		screen += 16;
	}
}
#endif

#ifdef ML_BMP_ZOOM
void ML_bmp_zoom(const unsigned char *bmp, int x, int y, int width, int height, float zoom_w, float zoom_h, ML_Color color)
{
    int i, j, iz, jz, width_z, height_z, nb_width, i3, bit, x_screen, pixel;
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
    if (x+width_z > 128) end_x = 128-x;
    else end_x = width_z;
    if (y < 0) begin_y = -y;
    else begin_y = 0;
    if (y+height_z > 64) end_y = 64-y;
    else end_y = height_z;

	switch ( color ){
		case ML_BLACK:
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

					if (pixel != 0) vram[(y+jz<<4)+(x_screen>>3)] |= 128>>(x_screen&7);
                }
            }
            break;
		case ML_WHITE:
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

					if (pixel != 0) vram[(y+jz<<4)+(x_screen>>3)] &= ~(128>>(x_screen&7));
                }
            }
            break;
		case ML_XOR:
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

					if (pixel != 0) vram[(y+jz<<4)+(x_screen>>3)] ^= 128>>(x_screen&7);
                }
            }
            break;
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
#endif


#ifdef ML_BMP_ROTATE
void ML_bmp_rotate(const unsigned char *bmp, int x, int y, int width, int height, int angle, ML_Color color)
{
    int i, j, i3, dx, dy, ox, oy, xr, yr, nb_width, pixel, bit;
    int cosinus, sinus;
    char* vram = ML_vram_adress();

    if (!bmp) return;
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
        cosinus = cosf(-3.14 * angle / 180.0) * 16384;
        sinus = sinf(-3.14 * angle / 180.0) * 16384;
    }
    nb_width = width + 7 >> 3;

	switch ( color ){
		case ML_BLACK:
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
                    if (!(xr < 0 || xr > 127 || yr < 0 || yr > 63))
                    {
                        pixel = bmp[i3 + nb_width * j] & bit;
						if (pixel != 0) vram[(yr<<4)+(xr>>3)] |= 128>>(xr&7);
                    }
                }
            }
			break;
		case ML_WHITE:
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
                    if (!(xr < 0 || xr > 127 || yr < 0 || yr > 63))
                    {
                        pixel = bmp[i3 + nb_width * j] & bit;
						if (pixel != 0) vram[(yr<<4)+(xr>>3)] &= ~(128>>(xr&7));
                    }
                }
            }
            break;
		case ML_XOR:
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
                    if (!(xr < 0 || xr > 127 || yr < 0 || yr > 63))
                    {
                        pixel = bmp[i3 + nb_width * j] & bit;
						if (pixel != 0) vram[(yr<<4)+(xr>>3)] ^= 128>>(xr&7);
                    }
                }
            }
            break;
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
                    if (!(xr < 0 || xr > 127 || yr < 0 || yr > 63))
                    {
                        pixel = bmp[i3 + nb_width * j] & bit;
						if (pixel != 0) ML_pixel(xr, yr, color);
                    }
                }
            }
			break;
	}
}
#endif



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
	else
	{
		int padding, pair;
		padding = width>>1;
		pair = !(width&1);
		return MLTest_rectangle(x-padding+pair, y-padding+pair, x+padding, y+padding);
	}
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
			byte = 1<<(7-(x&7));
			count+=(vram[(y<<4)+(x>>3)] & byte ? ML_BLACK : ML_WHITE);
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
			byte = 1<<(7-(x&7));
			count+=(vram[(y<<4)+(x>>3)] & byte ? ML_BLACK : ML_WHITE);
		}
	}
	return count;
}

int MLTest_horizontal_line(int y, int x1, int x2 )
{
    int i,x,count=0;
	int byte;
    char* vram = ML_vram_adress();
    if(y&~63 || (x1<0 && x2<0) || (x1>127 && x2>127)) return 0;
    if(x1 > x2)
    {
    	i = x1;
    	x1 = x2;
    	x2 = i;
    }
    if(x1 < 0) x1 = 0;
    if(x2 > 127) x2 = 127;
	for ( x=x1; x<=x2; x++) {
		byte = 1<<(7-(x&7));
		count+=(vram[(y<<4)+(x>>3)] & byte ? ML_BLACK : ML_WHITE);
	}
	return count;
}

int MLTest_rectangle(int x1, int y1, int x2, int y2 )
{
	int i,x,y,count=0;
	int byte;
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
	if ( 127<x2 ) x2=127;
	if (  63<y1 ) y2= 63;
	
	for ( y=y1; y<=y2; y++) {
		for ( x=x1; x<=x2; x++) {
			byte = 1<<(7-(x&7));
			count+=(vram[(y<<4)+(x>>3)] & byte ? ML_BLACK : ML_WHITE);
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
	return count;
}

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
