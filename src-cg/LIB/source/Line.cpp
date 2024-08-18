extern "C" {

#include "stdlib.h"
#include "disp_tools.hpp"
#include "display_tools.h"
#include "display_syscalls.h"

//
void Line( int x1, int y1, int x2, int y2, int color ){
int dx, dy, dd, xx, yy;
	if ( x1 == x2 ){
		if ( y1 == y2 ){
			return;
		}else{
			if ( y1 < y2 ) dd = 1;
			else dd = -1;
			while ( y1 != y2 ){
				Bdisp_SetPoint_VRAM( x1, y1, color );
				y1 += dd;
			}
		}
		return;
	}
	dx = x2 - x1;
	dy = y2 - y1;
	if ( abs( dx ) > abs( dy ) ){
		if ( x1 < x2 ) dd = 1;
		else dd = -1;
		xx = x1;
		yy = y1;
		while ( xx != x2 ){
			Bdisp_SetPoint_VRAM( xx, yy, color );
			xx += dd;
			yy = y1 + ( x2 - xx )*dy/dx;
		}
	}else{
		if ( y1 < y2 ) dd = 1;
		else dd = -1;
		yy = y1;
		while ( yy != y2 ){
			Bdisp_SetPoint_VRAM( xx, yy, color );
			yy += dd;
			xx = x1 + ( y2 - yy )*dx/dy; 
		}
	}
}

}

