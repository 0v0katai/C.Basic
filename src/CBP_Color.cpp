extern "C" {

#include "prizm.h"
#include "CBP.h"
//----------------------------------------------------------------------------------------------
//		Color convert etc.
//----------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------
int CB_GetRGB( char *SRC, int mode ){	// GetRGB/HSV/HSL() -> ListAns
	int c,d,r,g,b,m;
	int h,s,v;
	int base=MatBase;
	int errorCheck=1;
	int pipe=mode & 0xF0;
	mode &= 0x0F;
	d = CB_EvalInt( SRC );
	if ( SRC[ExecPtr] == ',' ) {
		c=SRC[++ExecPtr];
		if ( ( c == 'N' ) || ( c == 'N' ) ) { ExecPtr++; errorCheck=0; }
	}
	if ( SRC[ExecPtr] == ')' ) ExecPtr++;

	b = ((d&0x001F) << 3);
	g = ((d&0x07E0) >> 3);
	r = ((d&0xF800) >> 8);

	h=r; s=g; v=b;
	if ( mode ) {
		if ( ( errorCheck ) && ( pipe==0 ) ) {
			if ( (d==0x003F) || (d==0x041F) || (d==0xF820) || (d==0xFC00) ) { if ( CannotSupportERROR( d ) ) return 0; } // Can not support error
		}
		if ( mode==1 ) rgb2hsv(r,g,b,&h,&s,&v);	// rgb->HSV
		if ( mode==2 ) rgb2hsl(r,g,b,&h,&s,&v);	// rgb->HSL
	}
	dspflag=4;	// List ans
	NewMatListAns( 3, 1, base, 32 );		// List Ans[3]
	m=base;
	WriteMatrix( CB_MatListAnsreg, m++, base, h ) ;	//
	WriteMatrix( CB_MatListAnsreg, m++, base, s ) ;	// 
	WriteMatrix( CB_MatListAnsreg, m  , base, v ) ;	// 
	return d;
}

unsigned short rgb( int r, int g, int b){	//  r:0~255  g:0~255  b:0~255  ->color code
	return ((r<<8)&0xF800) | ((g<<3)&0x7E0) | ((b>>3)&0x1F);
}
unsigned short hsv( int H, int S, int V){	//  h:0~359  s:0~255  v:0~255  ->color code
	int r,g,b;
	hsv2rgb( H,S,V, &r,&g,&b);
	return rgb(r,g,b);
}
unsigned short hsl( int H, int S, int L){	//  h:0~359  s:0~255  l:0~255  ->color code
	int r,g,b;
	hsl2rgb( H,S,L, &r,&g,&b);
	return rgb(r,g,b);
}

void hsvhsl2rgb( int H, int S, int V, int *R, int *G, int *B, int hsvhsl ){	//  h:0~359  s:0~255  v:0~255  ->RGB(0~255)
	int max,min;
	if ( hsvhsl==248 ) {	// HSV
		max = V;
		min = ((max*255)-(S*max))/255;
	} else {				// HSL
		if ( V<128 ) {
			max = ((V*255)+V*S)/255;
			min = ((V*255)-V*S)/255;
		} else {
			max = ((V*255)+(255-V)*S)/255;
			min = ((V*255)-(255-V)*S)/255;
		}
	}
	max = (max+3)/4*4;
	min = (min+3)/4*4;
	if ( H<= 60 ) {
		*R = max;
		*G = ((H)*(max-min)+min*60+59)/60;
		*B = min;
	} else
	if ( H<=120 ) {
		*R = ((120-H)*(max-min)+min*60+59)/60;
		*G = max;
		*B = min;
	} else
	if ( H<=180 ) {
		*R = min;
		*G = max;
		*B = ((H-120)*(max-min)+min*60+59)/60;
	} else
	if ( H<=240 ) {
		*R = min;
		*G = ((240-H)*(max-min)+min*60+59)/60;
		*B = max;
	} else
	if ( H<=300 ) {
		*R = ((H-240)*(max-min)+min*60+59)/60;
		*G = min;
		*B = max;
	} else {
		*R = max;
		*G = min;
		*B = ((360-H)*(max-min)+min*60+59)/60;
	}
	if ( *R<0 ) *R=0;
	if ( *G<0 ) *G=0;
	if ( *B<0 ) *B=0;
	if ( *R>248 ) *R=248;
	if ( *G>252 ) *G=252;
	if ( *B>248 ) *B=248;
	*R = ((*R)+7)/8*8;
	*G = ((*G)+3)/4*4;
	*B = ((*B)+7)/8*8;
}

void hsv2rgb( int H, int S, int V, int *R, int *G, int *B ){	//  h:0~359  s:0~255  v:0~255  ->RGB(0~255)
	hsvhsl2rgb( H, S, V, &(*R), &(*G), &(*B), 248 );
}
void hsl2rgb( int H, int S, int L, int *R, int *G, int *B ){	//  H:0~359  s:0~255  l:0~255  ->RGB(0~255)
	hsvhsl2rgb( H, S, L, &(*R), &(*G), &(*B), 124 );
}


void rgb2hsvhsl( int R, int G, int B, int *H, int *S, int *V, int hsvhsl ){	//  RGB(0~255) ->  H:0~359  S:0~255  V:0~255  L:0~255
	int max,min;
	max = R;
	if ( G > max ) max = G;
	if ( B > max ) max = B;
	min = R;
	if ( G < min ) min = G;
	if ( B < min ) min = B;
	if ( R==max ) *H = (G-B)*60/(max-min);
	else
	if ( G==max ) *H = (B-R)*60/(max-min)+120;
	else
	if ( B==max ) *H = (R-G)*60/(max-min)+240;
	if ( *H < 0 ) *H += 360;

	if ( hsvhsl == 248 ) {	// ->HSV
		if ( max==0 ) *S = 0;
		else		  *S = (max-min)*255/max;
		*V = max;
	} else {				// ->HSL
		*V = (max+min)/2;
		if ( max==min ) *S = 0;
		else {
			if ( *V < 128 ) {
				*S = ((max-min)*255)/(max+min);
			} else {
				*S = ((max-min)*255)/(510-max-min);
			}
		}
	}
}

void rgb2hsv( int R, int G, int B, int *H, int *S, int *V ){	//  RGB(0~255) ->  H:0~359  S:0~255  V:0~255
	rgb2hsvhsl( R, G, B, &(*H), &(*S), &(*V), 248 );
}
void rgb2hsl( int R, int G, int B, int *H, int *S, int *L ){	//  RGB(0~255) ->  H:0~359  S:0~255  L:0~255
	rgb2hsvhsl( R, G, B, &(*H), &(*S), &(*L), 124 );
}


//----------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------
void CB_PlotLineColor( char *SRC ){
	int color=CB_GetColor( SRC );
	if ( color>=0 ) CB_ColorIndexPlot=color;
	CB_ColorIndex=-1;
}
void CB_BackColor( char *SRC ){
	int color;
	int c=SRC[ExecPtr];
	if ( c=='@' ) {
		ExecPtr++;
		color=CB_GetColor( SRC );
		DrawFrame( color  );
		CB_FrameColor=color;
//		DirectDrawRectangle(   0,   0,   5, 223, color );
//		DirectDrawRectangle(   6, 215, 397, 223, color );
//		DirectDrawRectangle( 390,   0, 397, 223, color );
	} else {
		color = CB_GetColor( SRC );
		if ( color>=0 ) CB_BackColorIndex = color;
	} 
}
void CB_TransparentColor( char *SRC ){
	int color;
	int c=SRC[ExecPtr];
	if ( ( c==':' ) || ( c==0x0D ) || ( c==0x0C ) || ( c==0 ) ) {
		CB_TransparentColorIndex=0xFF0000;	// no-color
	} else {
		CB_TransparentColorIndex = CB_GetColor( SRC );
		if ( ( -32768<=CB_TransparentColorIndex ) && ( CB_TransparentColorIndex <=65535 ) ) CB_TransparentColorIndex &= 0xFFFF;;
	}
}

int CB_GetColor( char *SRC ){
	int	c=SRC[ExecPtr++];
	switch ( c ) {
			case 0x7F:	// 7F
				c=SRC[ExecPtr++];
				switch ( c ) {
					case 0x34 :				// Red
						return 0xF800;	// Red
						break;
					case 0x35 :				// Blue
						return 0x001F;	// Blue
						break;
					case 0x36 :				// Green
						return 0x07E0;	// Green
						break;
					case 0x5E :				// RGB(
						return CB_RGB( SRC, 0 );
						break;
					case 0x71 :				// HSV(
						return CB_RGB( SRC, 1 );
						break;
					case 0x73 :				// HSL(
						return CB_RGB( SRC, 2 );
						break;
					default:
						ExecPtr-=2;
						{ CB_Error(SyntaxERR); return -1; }	// syntax error
						break;
				}
				break;
			case 0xFFFFFFF9:	// F9
				c=SRC[ExecPtr++];
				switch ( c ) {
					case 0xFFFFFF9B :			// Black
						return 0x0000;	// Black
						break;
					case 0xFFFFFF9C :			// White
						return 0xFFFF;	// White
						break;
					case 0xFFFFFF9D :			// Magenta
						return 0xF81F;	// Magenta
						break;
					case 0xFFFFFF9E :			// Cyan
						return 0x07FF;	// Cyan
						break;
					case 0xFFFFFF9F :			// Yellow
						return 0xFFE0;	// Yellow
						break;
					default:
						ExecPtr-=2;
						{ CB_Error(SyntaxERR); return -1; }	// syntax error
						break;
				}
				break;
			case '#':
				return CB_EvalInt( SRC );
				break;
			default:
				ExecPtr--;
				{ CB_Error(SyntaxERR); return -1; }	// syntax error
				break;
		
	}
	return -1;
}

int CB_RGBlistsub( char *SRC, int*r, int*g, int*b ){
	int reg;
	int m,n;
	int sizeA,sizeB;
	int ElementSize;
	int base;
	int dspflagtmp=dspflag;
	int execptr=ExecPtr;
	ListEvalsubTop(SRC);
	if ( dspflag <  3 )  { ExecPtr=execptr; return 1; } // List 1[0] etc
	if ( dspflag != 4 )  { CB_Error(ArgumentERR); return 0; } // Argument error
	reg = CB_MatListAnsreg;
	base=MatAry[reg].Base;
	m=base;
	*r = ReadMatrix( reg, m++, base ) ;
	*g = ReadMatrix( reg, m++, base ) ;
	*b = ReadMatrix( reg, m  , base ) ;
	DeleteMatListAns();
	dspflag=dspflagtmp; 
	return 4;	// List
}
unsigned short CB_RGB( char *SRC, int mode ) {	// n or (r,g,b)    return : color code	// mode 0:RGB  1:HSV 2:HSL
	int reg;
	int m,n;
	int sizeA,sizeB;
	int ElementSize;
	int base;
	int dspflagtmp=dspflag;
	int r,g,b;
	int h,s,v;
	int	c=SRC[ExecPtr];
	if ( c=='#' ) { 	// direct 16bit color #12345
		ExecPtr++;
		r = CB_EvalInt( SRC );
		if ( SRC[ExecPtr] == ')' ) ExecPtr++;
		return r;
	}
	c=CB_RGBlistsub( SRC, &r, &g, &b );
	if ( c==0 ) return 0;	// error
	if ( mode == 0 ) {	// RGB
		if ( c==4 ) goto exit;	// List
		r=CB_EvalInt( SRC );
//		if ( r<  0 ) r=  0;
//		if ( r>255 ) r=255;
		if ( SRC[ExecPtr] != ',' ) { CB_Error(SyntaxERR); return 0; }  // Syntax error
		ExecPtr++;
		g=CB_EvalInt( SRC );
//		if ( g<  0 ) g=  0;
//		if ( g>255 ) g=255;
		if ( SRC[ExecPtr] != ',' ) { CB_Error(SyntaxERR); return 0; }  // Syntax error
		ExecPtr++;
		b=CB_EvalInt( SRC );
//		if ( b<  0 ) b=  0;
//		if ( b>255 ) b=255;
	} else {		// HSV/HSL
		if ( c==4 ) { h=r; s=g; v=b; goto exithsv; }	// List
		h=CB_EvalInt( SRC );
		if ( ( h<0 ) || ( h>359 ) ) h = h%360;
		if ( SRC[ExecPtr] != ',' ) { CB_Error(SyntaxERR); return 0; }  // Syntax error
		ExecPtr++;
		s=CB_EvalInt( SRC );
		if ( s<  0 ) s=  0;
		if ( s>255 ) s=255;
		if ( SRC[ExecPtr] != ',' ) { CB_Error(SyntaxERR); return 0; }  // Syntax error
		ExecPtr++;
		v=CB_EvalInt( SRC );
		if ( v<  0 ) v=  0;
		if ( v>255 ) v=255;
	 exithsv:
		if ( mode==1 ) 	hsv2rgb( h, s, v, &r, &g, &b);
		else			hsl2rgb( h, s, v, &r, &g, &b);
	}
  exit:
	if ( SRC[ExecPtr] == ')' ) ExecPtr++;
	return rgb( r, g, b);

}


}
