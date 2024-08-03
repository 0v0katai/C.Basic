extern "C" {

#include "prizm.h"
#include "CBP.h"
//----------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------
int ReadBmpHeader( unsigned char *filebase, int *bfOffBits, int *biWidth, int *biHeight ){	// 1:1 bit mono bmp file   24:24bit   0:no bmp
	if ( (filebase[0x00]!='B') || (filebase[0x01]!='M') ) return 0;	// not Bmp file
	if ( ( filebase[0x0E] == 0x28 ) && ( filebase[0x1C]==1 ) ) {	// info header & 1bit mono 
		*bfOffBits = filebase[0x0A] +filebase[0x0B]*0x100 +filebase[0x0C]*0x10000 +filebase[0x0D]*0x1000000;
		*biWidth  = filebase[0x12] +filebase[0x13]*0x100 +filebase[0x14]*0x10000 +filebase[0x15]*0x1000000;
		*biHeight = filebase[0x16] +filebase[0x17]*0x100 +filebase[0x18]*0x10000 +filebase[0x19]*0x1000000;
		return 1;	// ok
	} else 
	if ( ( filebase[0x0E] == 0x28 ) && ( filebase[0x1C]==24 ) ) {	// info header & 24bit fullcolor 
		*bfOffBits = filebase[0x0A] +filebase[0x0B]*0x100 +filebase[0x0C]*0x10000 +filebase[0x0D]*0x1000000;
		*biWidth  = filebase[0x12] +filebase[0x13]*0x100 +filebase[0x14]*0x10000 +filebase[0x15]*0x1000000;
		*biHeight = filebase[0x16] +filebase[0x17]*0x100 +filebase[0x18]*0x10000 +filebase[0x19]*0x1000000;
		return 24;	// ok
	} else 
	if ( ( filebase[0x0E] == 0x38 ) && ( filebase[0x1C]==16 ) ) {	// info header & 16bit color BI_BITFIELD 
		*bfOffBits = filebase[0x0A] +filebase[0x0B]*0x100 +filebase[0x0C]*0x10000 +filebase[0x0D]*0x1000000;
		*biWidth  = filebase[0x12] +filebase[0x13]*0x100 +filebase[0x14]*0x10000 +filebase[0x15]*0x1000000;
		*biHeight = filebase[0x16] +filebase[0x17]*0x100 +filebase[0x18]*0x10000 +filebase[0x19]*0x1000000;
		return 16;	// ok
	}
	return 0;
}

int WriteBmpHeader( char *filebase, int biWidth, int biHeight, int bit ){	// 1 bit mono bmp file
	int   bfSize,biSizeImage;
	int   bfOffBits = 0x36;
	int   biSize          = 0x28;
//	short biPlanes        = 1;
//	short biBitCount      = 1;
//	int   biCompression   = 0;
//	int   biSizeImage     = 0;
//	int   biXPelsPerMeter = 0x0B12;
//	int   biYPelsPerMeter = 0x0B12;
//	int   biClrUsed       = 2;
//	int   biClrImportant  = 2;
//	char  rgbBlue         = 0xFF;
//	char  rgbGreen        = 0xFF;
//	char  rgbRed          = 0xFF;
//	char  rgbReserved     = 0xFF;
	if ( bit == 1 ) biSizeImage = ((((biWidth+7)/8)+3)/4)*4 *biHeight ;	//  1bit
	else
	if ( bit == 16) {
		biSizeImage = ((biWidth*2+3)/4)*4 *biHeight ;		// 161bit
		biSize = 0x38;
		bfOffBits = 0x46;
	} else {
		biSizeImage = ((biWidth*3+3)/4)*4 *biHeight ;		// 24bit
	}
	bfSize = biSizeImage +bfOffBits;
	
	filebase[0x00]='B';
	filebase[0x01]='M';
	filebase[0x02]=(bfSize & 0x000000FF);
	filebase[0x03]=(bfSize & 0x0000FF00) >>  8;
	filebase[0x04]=(bfSize & 0x00FF0000) >> 16;
	filebase[0x05]=(bfSize & 0xFF000000) >> 24;
	filebase[0x06]=0;
	filebase[0x07]=0;
	filebase[0x08]=0;
	filebase[0x09]=0;
	filebase[0x0A]=(bfOffBits & 0x000000FF);
	filebase[0x0B]=(bfOffBits & 0x0000FF00) >>  8;
	filebase[0x0C]=(bfOffBits & 0x00FF0000) >> 16;
	filebase[0x0D]=(bfOffBits & 0xFF000000) >> 24;
	
	filebase[0x0E]=biSize;	// BITMAPINFOHEADER size
	filebase[0x0F]=0;
	filebase[0x10]=0;
	filebase[0x11]=0;
	filebase[0x12]=(biWidth  & 0x000000FF);
	filebase[0x13]=(biWidth  & 0x0000FF00) >>  8;
	filebase[0x14]=(biWidth  & 0x00FF0000) >> 16;
	filebase[0x15]=(biWidth  & 0xFF000000) >> 24;
	filebase[0x16]=(biHeight & 0x000000FF);
	filebase[0x17]=(biHeight & 0x0000FF00) >>  8;
	filebase[0x18]=(biHeight & 0x00FF0000) >> 16;
	filebase[0x19]=(biHeight & 0xFF000000) >> 24;
	filebase[0x1A]=1;		// biPlanes
	filebase[0x1B]=0;
	filebase[0x1C]=bit;		// biBitCount
	filebase[0x1D]=0;
	filebase[0x1E]=3*(bit==16);		// biCompression
	filebase[0x1F]=0;
	
	filebase[0x20]=0;
	filebase[0x21]=0;
	filebase[0x22]=(biSizeImage & 0x000000FF);
	filebase[0x23]=(biSizeImage & 0x0000FF00) >>  8;
	filebase[0x24]=(biSizeImage & 0x00FF0000) >> 16;
	filebase[0x25]=(biSizeImage & 0xFF000000) >> 24;
	filebase[0x26]=0x12;	// biXPelsPerMeter
	filebase[0x27]=0x0B;
	filebase[0x28]=0;
	filebase[0x29]=0;
	filebase[0x2A]=0x12;	// biYPelsPerMeter
	filebase[0x2B]=0x0B;
	filebase[0x2C]=0;
	filebase[0x2D]=0;
	filebase[0x2E]=2;		// biClrUsed
	filebase[0x2F]=0;
	
	filebase[0x30]=0;
	filebase[0x31]=0;
	filebase[0x32]=2;		// biClrImportant
	filebase[0x33]=0;
	filebase[0x34]=0;
	filebase[0x35]=0;
	
	filebase[0x36]=0xFF;	// rgbBlue
	filebase[0x37]=0xFF;	// rgbGreen
	filebase[0x38]=0xFF;	// rgbRed
	filebase[0x39]=0xFF;	// rgbReserved
	
	if ( bit != 16 ) return bfSize;
	filebase[0x36]=0x00;	// bitfield Red
	filebase[0x37]=0xF8;	// bitfield Red
	filebase[0x38]=0x00;	// bitfield Red
	filebase[0x39]=0x00;	// bitfield Red
	filebase[0x3A]=0xE0;	// bitfield Green
	filebase[0x3B]=0x07;	// bitfield Green
	filebase[0x3C]=0x00;	// bitfield Green
	filebase[0x3D]=0x00;	// bitfield Green
	filebase[0x3E]=0x1F;	// bitfield Blue
	filebase[0x3F]=0x00;	// bitfield Blue
	filebase[0x40]=0x00;	// bitfield Blue
	filebase[0x41]=0x00;	// bitfield Blue
	filebase[0x42]=0x00;	// 
	filebase[0x43]=0x00;	// 
	filebase[0x44]=0x00;	// 
	filebase[0x45]=0x00;	// 
	return bfSize;
}

void DecodeBmp2Mat( int reg, char *buffer, int width, int height, int bit ){	//	bmpformat(buffer) -> Mat 
	int i,x,y;
	int xbyte,xbyte2;
	int b,g,r,data,H,L;
	char *buffer2;
	char bw=buffer[-2];
	switch ( bit ) {
		case 1:		// 1bit bmp
			buffer2=(char*)MatAry[reg].Adrs;
			xbyte =((((width+7)/8)+3)/4)*4;
			xbyte2=   (width+7)/8;
			for (y=0; y<height; y++) {
				for (x=0; x<xbyte2; x++) {
				buffer2[y*xbyte2+x] = buffer[(height-y-1)*xbyte+x] ^bw ;
				}
			}
			break;
		case 16:	// 16bit bmp
			xbyte =((width*2+3)/4)*4;
			xbyte2=  width;
			for (y=0; y<height; y++) {
				for (x=0; x<xbyte2; x++) {
					L = buffer[(height-y-1)*xbyte+x*2+0];
					H = buffer[(height-y-1)*xbyte+x*2+1];
					data = (H<<8)+L;;
					WriteMatrix( reg, y, x, data);
				}
			}
			break;
		case 24:	// 24bit bmp
			xbyte =((width*3+3)/4)*4;
			xbyte2=  width;
			for (y=0; y<height; y++) {
				for (x=0; x<xbyte2; x++) {
					b = buffer[(height-y-1)*xbyte+x*3+0];
					g = buffer[(height-y-1)*xbyte+x*3+1];
					r = buffer[(height-y-1)*xbyte+x*3+2];
					data = ((r<<8)&0xF800) | ((g<<3)&0x7E0) | ((b>>3)&0x1F);
					WriteMatrix( reg, y, x, data);
				}
			}
			break;
		default:
			break;
	}
}
int EncodeMat2Bmp( int reg , char *buffer , int width, int height, int bit ){	//	Mat -> bmpformat(buffer)
	int i,x,y;
	int xbyte,xbyte2;
	int offset;
	int b,g,r,d,H,L;
	char *buffer2;
	switch ( bit ) {
		case 1:		// 1bit bmp
			buffer2=(char*)MatAry[reg].Adrs;
			xbyte =((((width+7)/8)+3)/4)*4;
			xbyte2=   (width+7)/8;
			for (y=0; y<height; y++) {
				for (x=0; x<xbyte2; x++) {
					buffer[(height-y-1)*xbyte+x]=buffer2[y*xbyte2+x];
				}
			}
			return 1;
			break;
		case 16:	// 16bit bmp
			xbyte =((width*2+3)/4)*4;
			xbyte2=  width;
			for (y=0; y<height; y++) {
				for (x=0; x<xbyte2; x++) {
					d = ReadMatrix( reg, y, x);
					H = d >> 8;
					L = d & 0xFF;
					buffer[(height-y-1)*xbyte+x*2+0] = L;
					buffer[(height-y-1)*xbyte+x*2+1] = H;
				}
			}
			return 1;
			break;
		case 24:	// 24bit bmp
			xbyte =((width*3+3)/4)*4;
			xbyte2=  width;
			for (y=0; y<height; y++) {
				for (x=0; x<xbyte2; x++) {
					d = ReadMatrix( reg, y, x);
					b = (d&0x001F) << 3;
					g = (d&0x07E0) >> 3;
					r = (d&0xF800) >> 8;
					buffer[(height-y-1)*xbyte+x*3+0] = b;
					buffer[(height-y-1)*xbyte+x*3+1] = g;
					buffer[(height-y-1)*xbyte+x*3+2] = r;
				}
			}
			return 1;
			break;
		default:
			break;
	}
	return 0;
}
void DecodeBmp2mem( char *buffer2, char *buffer, int width, int height, int bit ){	//	bmpformat(buffer) -> bmp(buffer2)
	int i,x,y;
	int xbyte,xbyte2;
	int b,g,r;
	unsigned short *buffer3=(unsigned short *)buffer2;
	char bw=buffer[-2];
	switch ( bit ) {
		case 1:		// 1bit bmp
			xbyte =((((width+7)/8)+3)/4)*4;
			xbyte2=   (width+7)/8;
			for (y=0; y<height; y++) {
				for (x=0; x<xbyte2; x++) {
				buffer2[y*xbyte2+x] = buffer[(height-y-1)*xbyte+x] ^bw ;
				}
			}
			break;
//		case 16:	// 16bit bmp
//			xbyte =((width*2+3)/4)*4;
//			xbyte2=  width;
//			for (y=0; y<height; y++) {
//				for (x=0; x<xbyte2; x++) {
//					L = buffer[(height-y-1)*xbyte+x*2+0];
//					H = buffer[(height-y-1)*xbyte+x*2+1];
//					buffer3[x*xbyte2+y] = (H<<8)+L;;
//				}
//			}
//			break;
//		case 24:	// 24bit bmp
//			xbyte =((width*3+3)/4)*4;
//			xbyte2=  width;
//			for (y=0; y<height; y++) {
//				for (x=0; x<xbyte2; x++) {
//					b = buffer[(height-y-1)*xbyte+x*3+0];
//					g = buffer[(height-y-1)*xbyte+x*3+1];
//					r = buffer[(height-y-1)*xbyte+x*3+2];
//					buffer3[x*xbyte2+y] = ((r<<8)&0xF800) | ((g<<3)&0x7E0) | ((b>>3)&0x1F);
//				}
//			}
//			break;
		default:
			break;
	}
}

/*
int EncodeBmp2mem( char *buffer , char *buffer2 , int width, int height, int bit ){	//	bmp(buffer2) -> bmpformat(buffer)
	int i,x,y;
	int xbyte,xbyte2;
	int offset;
	int b,g,r;
	unsigned short d, *buffer3=(unsigned short *)buffer2;
	switch ( bit ) {
		case 1:		// 1bit bmp
			xbyte =((((width+7)/8)+3)/4)*4;
			xbyte2=   (width+7)/8;
			for (y=0; y<height; y++) {
				for (x=0; x<xbyte2; x++) {
					buffer[(height-y-1)*xbyte+x]=buffer2[y*xbyte2+x];
				}
			}
			return 1;
			break;
		case 24:	// 24bit bmp
			xbyte =((width*3+3)/4)*4;
			xbyte2=  width;
			for (y=0; y<height; y++) {
				for (x=0; x<xbyte2; x++) {
					d =buffer3[y*xbyte2+x];
					b = (d&0x001F) << 3;
					g = (d&0x07E0) >> 3;
					r = (d&0xF800) >> 8;
					buffer[(height-y-1)*xbyte+x*3+0] = b;
					buffer[(height-y-1)*xbyte+x*3+1] = g;
					buffer[(height-y-1)*xbyte+x*3+2] = r;
				}
			}
			return 1;
			break;
		default:
			break;
	}
	return 0;
}
*/

int EncodeVram2Bmp( char *buffer , int width, int height, int bit, int px, int py  ){	//	vram -> bmpformat(buffer)
	int i,x,y;
	int xbyte,xbyte2;
	int offset;
	int b,g,r,H,L;
	unsigned char buffer2[1024];
	DISPBOX box;
	unsigned short d, *buffer3=(unsigned short *)PictAry[0];
	switch ( bit ) {
		case 1:		// 1bit bmp
			box.left   =px;
			box.right  =px+width-1;
			box.top    =py;
			box.bottom =py+height-1;
			Bdisp_ReadArea_VRAM_fx( &box,  buffer2 );

			xbyte =((((width+7)/8)+3)/4)*4;
			xbyte2=   (width+7)/8;
			for (y=0; y<height; y++) {
				for (x=px; x<px+xbyte2; x++) {
					buffer[(height-y-1)*xbyte+x]=buffer2[y*xbyte2+x];
				}
			}
			return 1;
			break;
		case 16:	// 16bit bmp
			xbyte =((width*2+3)/4)*4;
			xbyte2=  width;
			for (y=0; y<height; y++) {
				for (x=0; x<xbyte2; x++) {
					d =buffer3[(y+py+24)*384+(x+px)];
					H = d>>8;
					L = d&0xFF;
					buffer[(height-y-1)*xbyte+x*2+0] = L;
					buffer[(height-y-1)*xbyte+x*2+1] = H;
				}
			}
			return 1;
			break;
		case 24:	// 24bit bmp
			xbyte =((width*3+3)/4)*4;
			xbyte2=  width;
			for (y=0; y<height; y++) {
				for (x=0; x<xbyte2; x++) {
					d =buffer3[(y+py+24)*384+(x+px)];
					b = (d&0x001F) << 3;
					g = (d&0x07E0) >> 3;
					r = (d&0xF800) >> 8;
					buffer[(height-y-1)*xbyte+x*3+0] = b;
					buffer[(height-y-1)*xbyte+x*3+1] = g;
					buffer[(height-y-1)*xbyte+x*3+2] = r;
				}
			}
			return 1;
			break;
		default:
			break;
	}
	return 0;
}


void DecodeBmp2vram( char *buffer, int width, int height, int bit, int px, int py, int mode, int alpha ){	//	bmpformat(buffer) -> bmp(vram)
	char *vram =(char*)PictAry[0];
	unsigned short *vram2=(unsigned short *)PictAry[0],d;
	int i,x,y;
	int xbyte,xbyte2;
	int data;
	int tx,ty;
	char bw=buffer[-2];
	int b,g,r,bg_b,bg_g,bg_r,H,L;
	switch ( bit ) {
		case 1:		// 1bit bmp
			xbyte =((((width+7)/8)+3)/4)*4;
			xbyte2=   (width+7)/8;
			for (y=0; y<height; y++) {
				for (x=0; x<xbyte2; x++) {
					data=buffer[(height-y-1)*xbyte+x]^bw; bit=128;
					for ( i=0; i<8; i++ ) {
						tx=x*8+i+px; ty=y+py;
						if ( CB_G1MorG3M==1 ) {
							if ( tx < width+px ) {
								if ( ( 0 <= tx )&&( tx < 128 )&&( 0 <= ty )&&(ty < 64 ) ) BdispSetPointVRAM2( tx, ty, (bit&data)!=0 );
							}
						} else {
							if ( tx < width+px ) {
								if ( ( 0 <= tx )&&( tx< 384 )&&( -24 <= ty )&&( ty < 192 ) ) BdispSetPointVRAM2( tx, ty, (bit&data)!=0 );
							}
						}
						bit>>=1;
					}
				}
			}
			break;
		case 16:	// 16bit bmp
			xbyte =((width*2+3)/4)*4;
			xbyte2=  width;
			py+=24;
			for (y=0; y<height; y++) {
				for (x=0; x<xbyte2; x++) {
					L = buffer[(height-y-1)*xbyte+x*2+0]&0xFF;
					H = buffer[(height-y-1)*xbyte+x*2+1]&0xFF;
					d = (H<<8)+L;
					b = (((int)d&0x001F) << 3);
					g = (((int)d&0x07E0) >> 3);
					r = (((int)d&0xF800) >> 8);
					if ( ( x+px < 384 )&&( y+py < 216 ) ) {
						d =vram2[x+px+(y+py)*384];
						bg_b = (((int)d&0x001F) << 3);
						bg_g = (((int)d&0x07E0) >> 3);
						bg_r = (((int)d&0xF800) >> 8);
						switch ( mode ) {
							case 0:
								if ( d != CB_BackColorIndex ) break;
							case 1:
								if ( alpha<255 ) {
									b = ( b*alpha + bg_b*(254-alpha) ) >>8;
									g = ( g*alpha + bg_g*(254-alpha) ) >>8;
									r = ( r*alpha + bg_r*(254-alpha) ) >>8;
								}
								vram2[x+px+(y+py)*384] =  ((r<<8)&0xF800) | ((g<<3)&0x7E0) | ((b>>3)&0x1F) ;
								break;
						}
					}
				}
			}
			break;
		case 24:	// 24bit bmp
			xbyte =((width*3+3)/4)*4;
			xbyte2=  width;
			py+=24;
			for (y=0; y<height; y++) {
				for (x=0; x<xbyte2; x++) {
					b = buffer[(height-y-1)*xbyte+x*3+0]&0xFF;
					g = buffer[(height-y-1)*xbyte+x*3+1]&0xFF;
					r = buffer[(height-y-1)*xbyte+x*3+2]&0xFF;
					if ( ( x+px < 384 )&&( y+py < 216 ) ) {
						d =vram2[x+px+(y+py)*384];
						bg_b = (((int)d&0x001F) << 3);
						bg_g = (((int)d&0x07E0) >> 3);
						bg_r = (((int)d&0xF800) >> 8);
						switch ( mode ) {
							case 0:
								if ( d != CB_BackColorIndex ) break;
							case 1:
								if ( alpha<255 ) {
									b = ( b*alpha + bg_b*(254-alpha) ) >>8;
									g = ( g*alpha + bg_g*(254-alpha) ) >>8;
									r = ( r*alpha + bg_r*(254-alpha) ) >>8;
								}
								vram2[x+px+(y+py)*384] =  ((r<<8)&0xF800) | ((g<<3)&0x7E0) | ((b>>3)&0x1F) ;
								break;
						}
					}
				}
			}
			break;
		default:
			break;
	}
}

int DecodeBmp2Vram( char *filebase, int px, int py ){	//	bmp -> vram
	int i,x,y;
	int width,height;
	int offset;
	int bit;
	bit=ReadBmpHeader( (unsigned char*)filebase, &offset, &width, &height );
	if (  bit == 0 ) return 0;	// not bmp
	if (  bit >= 16 ) py-=24;
	DecodeBmp2vram( filebase+offset, width, height, bit, px, py, 1, 255 );
	return 1;
}

/*
int CB_GetOprand_XY1XY2( char *SRC, int *x1, int *y1, int *x2, int *y2 ) {
	if ( SRC[ExecPtr] != ',' ) return 0;
	ExecPtr++;
	if ( SRC[ExecPtr] == ',' ) goto jp;
	*x1=CB_EvalInt( SRC );
	
	if ( SRC[ExecPtr] != ',' ) return 1;
  jp:
	ExecPtr++;
	if ( SRC[ExecPtr] == ',' ) goto jp2;
	*y1=CB_EvalInt( SRC );
	
	if ( SRC[ExecPtr] != ',' ) return 1;
  jp2:
	ExecPtr++;
	if ( SRC[ExecPtr] == ',' ) goto jp3;
	*x2=CB_EvalInt( SRC );
	
	if ( SRC[ExecPtr] != ',' ) return 1;
  jp3:
	ExecPtr++;
	if ( SRC[ExecPtr] == ',' ) return 0;
	*y2=CB_EvalInt( SRC );
	return 1;
}
*/

void CB_BmpSave( char *SRC ) { //	BmpSave "TEST.bmp",Mat A[,Q]		BmpSave "TEST.bmp",x1,y1,x2,y2		BmpSave@K BmpSave@G
	char sname[32];
	int c,d,i,reg,matsize;
	char* FilePtr;
	int check=0;
	int width,height;
	int bfOffBits = 0x36;
	int bfSize;
	char *bmpbuffer;
	int dx,dy;
	int dimA,dimB;
	int bit;
	int direct=0;
	int x1,y1,x2,y2;
	int folder=0;
	int gaiji=0,no=-1,flag;
	
	c =SRC[ExecPtr];
	if ( c == '@' ) { 			// font mode  1bit bmp
		c =SRC[++ExecPtr];
		if ( c == '@' ) { 	c =SRC[++ExecPtr]; folder=1; }
		if ( ( c=='K' )||( c=='k' ) ) { ExecPtr++; gaiji=0; }
		else
		if ( ( c=='G' )||( c=='g' ) ) { ExecPtr++; gaiji=1; }
		else
		if ( ( c=='A' )||( c=='a' ) ) { ExecPtr++; gaiji=2; }
		else
		{ CB_Error(SyntaxERR); return; }  // Syntax error

		c =SRC[ExecPtr];
		if ( ( c=='L' )||( c=='l' ) ) { ExecPtr++; flag=1; }
		else
		if ( ( c=='M' )||( c=='m' ) ) { ExecPtr++; flag=2; }
		else { flag=3; }  // both
		
		c=CB_IsStr( SRC, ExecPtr );
		if ( c ) {	// string
			if ( flag==3 ) { CB_Error(ArgumentERR); return; }  // Argument error
			CB_GetLocateStr(SRC, sname,22); if ( ErrorNo ) return ;	// error
		} else {	// expression
			c =SRC[ExecPtr];
			if ( ( '0'<=c )&&( c<='9' ) ) { ExecPtr++; no=c-'0'; }
			sname[0]='\0';
		}
		c =SRC[ExecPtr];
		if ( c == ',' ) {
			c =SRC[++ExecPtr];
			if ( ( c == 'Q' ) || ( c == 'q' ) ) check=1;
			ExecPtr++;
		}
		switch ( gaiji ) {	
			case 0:
				SaveExtFontKana(  flag, sname, folder, no, check ); return ;
			case 1:
				SaveExtFontGaiji( flag, sname, folder, no, check ); return ;
			case 2:
				SaveExtFontAnk(   flag, sname, folder, no, check ); return ;
		}
	}
	
	if ( CB_G1MorG3M==1 ) bit=1; else bit=16;
//	c =SRC[ExecPtr];
//	if ( c != 0x22 ) { CB_Error(SyntaxERR); return; }  // Syntax error

	CB_GetLocateStr(SRC, sname,22); if ( ErrorNo ) return ;	// error
	c =SRC[ExecPtr];
	if ( c != ',' ) { CB_Error(SyntaxERR); return; }  // Syntax error
	ExecPtr++;
	c =SRC[ExecPtr]; d =SRC[ExecPtr+1];
	if ( ( c == 0x7F ) && ( ( d==0x40 ) || ( d==0xFFFFFF84 ) || ( d==0x51 ) ) ) {	//	BmpSave "TEST.bmp",Mat A[,Q]
		FilePtr = CB_SaveLoadOprand( SRC, &reg, &matsize);
		dimA =MatAry[reg].SizeA;
		dimB =MatAry[reg].SizeB;
		if ( ErrorNo ) return; // error
		bit= MatAry[reg].ElementSize;
		if ( bit != 1 ) bit==16;
		width = dimA;
		height= dimB;
	} else {											//	BmpSave "TEST.bmp",x1,y1,x2,y2
		CB_GetOprand4( SRC, &x1, &y1, & x2, &y2 );
		direct=1;
	}
	c =SRC[ExecPtr];
	if ( c == ',' ) {
		c =SRC[++ExecPtr];
		if ( ( c == 'Q' ) || ( c == 'q' ) ) check=1;
		ExecPtr++;
	}
	c =SRC[ExecPtr];
	if ( c == ',' ) {
		c =SRC[++ExecPtr];
		if ( ( c == 'F' ) || ( c == 'f' ) ) bit=16;
		ExecPtr++;
	}
	if ( direct == 0 ) {
		if ( bit == 1 ) {
			switch ( MatAry[reg].ElementSize ) {
				case  1:
					width = dimA;
					height= dimB;
					break;
				case  8:
					width = dimB*8;
					height= dimA;
					break;
				case 16:
					width = dimB*16;
					height= dimA;
					break;
				case 32:
					width = dimB*32;
					height= dimA;
					break;
				case 64:
					width = dimB*64;
					height= dimA;
					break;
				default:
					CB_Error(ElementSizeERR);
					break;
			}
		} else {
			switch ( MatAry[reg].ElementSize ) {
				case  1:
					width = dimA/16;
					height= dimB;
					break;
				case  8:
					width = dimB/2;
					height= dimA;
					break;
				case 16:
					width = dimB;
					height= dimA;
					break;
				case 32:
					width = dimB*2;
					height= dimA;
					break;
				case 64:
					width = dimB*4;
					height= dimA;
					break;
				default:
					CB_Error(ElementSizeERR);
					break;
			}
		}
	} else {
		if ( x1 > x2 ) { i=x1; x1=x2; x2=i; }
		if ( y1 > y2 ) { i=y1; y1=y2; y2=i; }
		width = x2-x1+1;
		height= y2-y1+1;
	}
	if ( bit == 16) bfOffBits=0x46;
	if ( bit ==  1) bfSize = ((((width+7)/8)+3)/4)*4 *height +bfOffBits;	//  1bit
	else
	if ( bit == 16) bfSize = ((width*2+3)/4)*4 *height +bfOffBits;			// 16bit
	else			bfSize = ((width*3+3)/4)*4 *height +bfOffBits;			// 24bit
	bmpbuffer = ( char *)HiddenRAM_mallocTemp( bfSize+4 );
	if( bmpbuffer == NULL ) { CB_Error(NotEnoughMemoryERR); return ; }	// Not enough memory error
	memset( bmpbuffer, 0x00, bfSize+4 );
	WriteBmpHeader( bmpbuffer, width, height, bit );
	if ( direct == 0 )	EncodeMat2Bmp( reg, bmpbuffer+bfOffBits , width, height, bit );
	else 				EncodeVram2Bmp( bmpbuffer+bfOffBits , width, height, bit, x1, y1 );
	CB_SaveSub( sname, bmpbuffer, bfSize, check, "bmp" );
	HiddenRAM_freeTemp( bmpbuffer );	// free
}

void CB_BmpLoad( char *SRC ) { //	BmpLoad("TEST")[->Mat A]
	char sname[32];
	int c,i,reg,matsize,dimA,dimB,ElementSize,base;
	char* FilePtr;
	int width,height;
	int offset,ptr=0,size,bit;
	char* matptr;
	int direct=0;
	int px=0,py=0;
	int mode=1,alpha;
	int folder=0;
	int gaiji=0,no=-1,flag;

	c =SRC[ExecPtr];
	if ( c == '@' ) { 		// BmpLoad(@K1)
		c =SRC[++ExecPtr];
		if ( c == '@' ) { 	c =SRC[++ExecPtr]; folder=1; }
		if ( ( c=='K' )||( c=='k' ) ) { ExecPtr++; gaiji=0; }
		else
		if ( ( c=='G' )||( c=='g' ) ) { ExecPtr++; gaiji=1; }
		else
		if ( ( c=='A' )||( c=='a' ) ) { ExecPtr++; gaiji=2; }
		else
		{ CB_Error(SyntaxERR); return; }  // Syntax error
		c =SRC[ExecPtr];
		if ( ( c=='L' )||( c=='l' ) ) { ExecPtr++; flag=1; }
		else
		if ( ( c=='M' )||( c=='m' ) ) { ExecPtr++; flag=2; }
		else { flag=3; }  // both
		
		c=CB_IsStr( SRC, ExecPtr );
		if ( c ) {	// string
			if ( flag==3 ) { CB_Error(ArgumentERR); return; }  // Argument error
			CB_GetLocateStr(SRC, sname,22); if ( ErrorNo ) return ;	// error
		} else {	// expression
			c =SRC[ExecPtr];
			if ( ( '0'<=c )&&( c<='9' ) ) { ExecPtr++; no=c-'0'; }
			sname[0]='\0';
		}
		if ( SRC[ExecPtr] == ')' ) ExecPtr++;
		switch ( gaiji ) {	
			case 0:
				LoadExtFontKanafolder(  flag, sname, folder, no ); return ;
			case 1:
				LoadExtFontGaijifolder( flag, sname, folder, no ); return ;
			case 2:
				LoadExtFontAnkfolder(   flag, sname, folder, no ); return ;
		}
	}

	CB_GetLocateStr(SRC, sname,22); if ( ErrorNo ) return ;	// error
	
//	c =SRC[ExecPtr];
//	if ( c == ',' ) {
//		ExecPtr++;
//		ptr=CB_EvalInt( SRC );
//		if ( ptr < 0 ) { CB_Error(RangeERR); return; }	// Range error
//	}
	if ( SRC[ExecPtr] == ')' ) ExecPtr++;
	c =SRC[ExecPtr];
	if ( c == 0x0E ) {
		ExecPtr++;
		CB_SaveLoadOprand( SRC, &reg, &matsize );
		if ( ErrorNo == NoMatrixArrayERR ) { ErrorNo=0; }	// No Mat error cancel
	} else {
		direct=1;
		if ( c == ',' ) { 
			ExecPtr++;
			CB_GetOprand2( SRC, &px, &py);
		}
		c =SRC[ExecPtr];
		CB_GetOprand_MLcolor( SRC, &mode );
		alpha=(MLV_rand<<8)/(RAND_MAX+1);
		if ( alpha>255 ) alpha=255;
	
	}
	FilePtr=CB_LoadSub( sname, ptr, &size, "bmp" ) ;
	if ( ErrorNo ) return ; // error

	bit=ReadBmpHeader( (unsigned char*)FilePtr, &offset, &width, &height );
	if ( bit == 1 ) {		// 1bit bmp
		dimA = height;
		dimB = width;
		base = 0;
		ElementSize = 1;
	} else
	if ( bit >= 16 ) {		// 16/24bit bmp 
		dimA = width;
		dimB = height;
		base = 0;
		ElementSize = 16;
	} else { CB_Error(ArgumentERR); return; }	// Range error

	if ( direct ) {
		DecodeBmp2vram( FilePtr+offset, width, height, bit, px, py, mode, alpha );
//		Bdisp_PutDisp_DD_DrawBusy_skip_through(SRC);
		goto exit; 
	}
	DimMatrixSub( reg, ElementSize, dimB, dimA, base ) ;
	if ( ErrorNo ) goto exit; // error
	matptr=(char*)MatAry[reg].Adrs;
	if ( bit==1 ) DecodeBmp2mem( matptr , FilePtr+offset, width, height, bit );	//	bmpformat -> bmp
	else DecodeBmp2Mat( reg, FilePtr+offset, width, height, bit );
  exit:
	HiddenRAM_freeTemp( FilePtr );	// free
}

//----------------------------------------------------------------------------------------------
//--------------------------------------------------------------
//static const int TEXT_COLOR_BLACK	= 0;
//static const int TEXT_COLOR_BLUE	= 1;
//static const int TEXT_COLOR_GREEN	= 2;
//static const int TEXT_COLOR_CYAN	= 3;
//static const int TEXT_COLOR_RED	= 4;
//static const int TEXT_COLOR_PURPLE= 5;
//static const int TEXT_COLOR_YELLOW= 6;
//static const int TEXT_COLOR_WHITE	= 7;

void ConvertFULLtoC3( unsigned char *c3color, unsigned short *fullcolor, int dx, int dy ){	//	fullcolor(16bit color) -> c3color(4bit color)
	int ptr16;
	int ptr4;
	int x,y,col4,col16;
//                       Black   Blue    Green   Cyan    Red     Magenta Yellow  White
    int colortable[16]={ 0x0000, 0x001F, 0x07E0, 0x07FF, 0xF800, 0xF81F, 0xFFE0, 0xFFFF, 0,0,0,0,0,0,0,0};
//	int colortable[16]={ COLOR_BLACK, COLOR_BLUE, COLOR_LIME, COLOR_CYAN, COLOR_RED, COLOR_FUCHSIA, COLOR_YELLOW, COLOR_WHITE, 0,0,0,0,0,0,0,0};
	for( y=0; y<dy; y++){
		for( x=0; x<dx; x++){
			ptr4  = y*(dx>>1)+(x>>1);
			ptr16 = y*(dx   )+(x);
			col16 = fullcolor[ptr16];
			for( col4=0; col4<16; col4++ ) if ( colortable[col4] == col16 ) break;
			if ( x&1 ) {
				c3color[ptr4] &= 0xF0;
				c3color[ptr4] |= (col4);
			} else {
				c3color[ptr4] &= 0x0F;
				c3color[ptr4] |= (col4<<4);
			}
		}
	}
}
void ConvertC3toFULL( unsigned short *fullcolor, unsigned char *c3color, int dx, int dy ){	//	c3color(4bit color) -> fullcolor(16bit color)
	int ptr16;
	int ptr4;
	int x,y,col4,col16;
//                       Black   Blue    Green   Cyan    Red     Magenta Yellow  White
    int colortable[16]={ 0x0000, 0x001F, 0x07E0, 0x07FF, 0xF800, 0xF81F, 0xFFE0, 0xFFFF, 0,0,0,0,0,0,0,0};
//	int colortable[16]={ COLOR_BLACK, COLOR_BLUE, COLOR_LIME, COLOR_CYAN, COLOR_RED, COLOR_FUCHSIA, COLOR_YELLOW, COLOR_WHITE, 0,0,0,0,0,0,0,0};
	for( y=0; y<dy; y++){
		for( x=0; x<dx; x++){
			ptr4  = y*(dx>>1)+(x>>1);
			ptr16 = y*(dx   )+(x);
			if ( x&1 ) {
				col4 = c3color[ptr4] & 0xF;
			} else {
				col4 = c3color[ptr4] >> 4;
			}
			fullcolor[ptr16] = colortable[col4];
		}
	}
}

	TLoadG3P_array	loadg3p_array;
int LoadG3Psub( char *fname ){
	char *bmpbuffer;
	int *vram=(int*)PictAry[0];
	int *data2 ;
	FONTCHARACTER filename[0x10A];
	unsigned char c3buf[384*216/2];
	int i,r,s;
	char buf[32],buf2[32];
	
	CharToFont( fname, filename );
	bmpbuffer = ( char *)HiddenRAM_mallocTemp( 384*216*2 ) ;
	loadg3p_array.LoadG3B_buffer_ptr = bmpbuffer;
	loadg3p_array.c2=0;
	loadg3p_array.c3=0;
	loadg3p_array.c4=0;
	loadg3p_array.i3=0;
	loadg3p_array.width =384;
	loadg3p_array.height=216;
	loadg3p_array.i5=0;
	r = LoadG3P( &loadg3p_array, filename);
	if ( r ) return r;
	if ( loadg3p_array.colormode == 1 ) { //	C3 4bit color mode
		memcpy( c3buf, bmpbuffer, 384*216/2);
		ConvertC3toFULL( (unsigned short *)bmpbuffer, c3buf, loadg3p_array.width, loadg3p_array.height );
	}
	data2=(int*)bmpbuffer ;
	if ( loadg3p_array.height > 192 ) {	// Capt	384*216
		for( i=0; i<216*384/2; i++) *vram++ = *data2++;
	} else {			// Pict 384*192
		vram += 24*384/2;
		for( i=0; i<192*384/2; i++) *vram++ = *data2++;
	}

sprintf3(buf,"%d %d %d %d",loadg3p_array.c2,loadg3p_array.c3, loadg3p_array.c4, loadg3p_array.i3 );
sprintf3(buf2,"%d %d %d",loadg3p_array.width,loadg3p_array.height, loadg3p_array.i5 );
//OkMSGstr2(buf,buf2);

	return 0;	// ok
}

//----------------------------------------------------------------------------------------------
int BMPObjectAlign4a( unsigned int n ){ return n; }	// align +4byte
//int BMPObjectAlign4b( unsigned int n ){ return n; }	// align +4byte
//int BMPObjectAlign4c( unsigned int n ){ return n; }	// align +4byte
//----------------------------------------------------------------------------------------------

}
