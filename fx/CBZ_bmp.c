#include "CB.h"
//----------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------
int ReadBmpHeader( unsigned char *filebase, int *bfOffBits, int *biWidth, int *biHeight ){	// 1 bit mono bmp file 
	if ( (filebase[0x00]!='B') || (filebase[0x01]!='M') ) return 0;	// not Bmp file
	if ( ( filebase[0x0E] == 0x28 ) && ( filebase[0x1A]==1 ) ) {	// info header & 1bit mono 
		*bfOffBits = filebase[0x0A] +filebase[0x0B]*0x100 +filebase[0x0C]*0x10000 +filebase[0x0D]*0x1000000;
		*biWidth  = filebase[0x12] +filebase[0x13]*0x100 +filebase[0x14]*0x10000 +filebase[0x15]*0x1000000;
		*biHeight = filebase[0x16] +filebase[0x17]*0x100 +filebase[0x18]*0x10000 +filebase[0x19]*0x1000000;
		return 1;	// ok
	}
	return 0;
}

int WriteBmpHeader( char *filebase, int biWidth, int biHeight ){	// 1 bit mono bmp file
	int   bfSize,biSizeImage;
	int   bfOffBits = 0x3E;
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
	biSizeImage = ((((biWidth+7)/8)+3)/4)*4 *biHeight ;
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
	filebase[0x1C]=1;		// biBitCount
	filebase[0x1D]=0;
	filebase[0x1E]=0;		// biCompression
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
	filebase[0x39]=0x00;	// rgbReserved
	filebase[0x3A]=0x00;	// rgbBlue
	filebase[0x3B]=0x00;	// rgbGreen
	filebase[0x3C]=0x00;	// rgbRed
	filebase[0x3D]=0x00;	// rgbReserved
	return bfSize;
}

void DecodeBmp2mem( char *buffer2, char *buffer, int width, int height ){	//	bmpformat(buffer) -> bmp(buffer2)
	int i,x,y;
	int xbyte,xbyte2;
	char bw=buffer[-2];
	xbyte =((((width+7)/8)+3)/4)*4;
	xbyte2=   (width+7)/8;
	for (y=0; y<height; y++) {
		for (x=0; x<xbyte2; x++) {
			buffer2[y*xbyte2+x]=buffer[(height-y-1)*xbyte+x] ^bw ;
		}
	}
}
int EncodeBmp2mem( char *buffer , char *buffer2 , int width, int height ){	//	bmp(buffer2) -> bmpformat(buffer)
	int i,x,y;
	int xbyte,xbyte2;
	int offset;
	xbyte =((((width+7)/8)+3)/4)*4;
	xbyte2=   (width+7)/8;
	for (y=0; y<height; y++) {
		for (x=0; x<xbyte2; x++) {
			buffer[(height-y-1)*xbyte+x]=buffer2[y*xbyte2+x];
		}
	}
	return 1;
}
int EncodeMat2Bmp( int reg , char *buffer , int width, int height ){	//	Mat -> bmpformat(buffer)
	int i,x,y;
	int xbyte,xbyte2;
	int offset;
	int b,g,r,d;
	char *buffer2;
	buffer2=(char*)MatAry[reg].Adrs;
	xbyte =((((width+7)/8)+3)/4)*4;
	xbyte2=   (width+7)/8;
	for (y=0; y<height; y++) {
		for (x=0; x<xbyte2; x++) {
			buffer[(height-y-1)*xbyte+x]=buffer2[y*xbyte2+x];
		}
	}
	return 1;
}

int EncodeVram2Bmp( char *buffer , int width, int height, int px, int py  ){	//	vram -> bmpformat(buffer)
	int i,x,y;
	int xbyte,xbyte2;
	int offset;
	int b,g,r;
	unsigned char buffer2[1024];
	DISPBOX box;
	box.left   =px;
	box.right  =px+width-1;
	box.top    =py;
	box.bottom =py+height-1;
	Bdisp_ReadArea_VRAM( &box,  buffer2 );

	xbyte =((((width+7)/8)+3)/4)*4;
	xbyte2=   (width+7)/8;
	for (y=0; y<height; y++) {
		for (x=px; x<px+xbyte2; x++) {
			buffer[(height-y-1)*xbyte+x]=buffer2[y*xbyte2+x];
		}
	}
	return 1;
}

/*
void DecodeBmp2vram( char *vram, char *buffer, int width, int height ){	//	bmpformat(buffer) -> bmp(vram)
	int i,x,y;
	int xbyte,xbyte2;
	xbyte =((((width+7)/8)+3)/4)*4;
	xbyte2=   (width+7)/8;
	for (y=0; y<height; y++) {
		for (x=0; x<xbyte2; x++) {
			if ( (0<=x)&&(x<128)&&(0<=y)&&(y<128) ) vram[(y<<4)+x]=buffer[(height-y-1)*xbyte+x];
		}
	}
}
*/
void DecodeBmp2vram( char *vram, char *buffer, int width, int height, int bit, int px, int py ){	//	bmpformat(buffer) -> bmp(vram)
	int i,x,y;
	int xbyte,xbyte2;
	int data;
	int b,g,r;
	char bw=buffer[-2];
	switch ( bit ) {
		case 1:		// 1bit bmp
			xbyte =((((width+7)/8)+3)/4)*4;
			xbyte2=   (width+7)/8;
			for (y=0; y<height; y++) {
				for (x=0; x<xbyte2; x++) {
					if ( (0<=x)&&(x<128)&&(0<=y)&&(y<128) ) {
//						vram[(y<<4)+x]=buffer[(height-y-1)*xbyte+x];
						data=buffer[(height-y-1)*xbyte+x]^bw; bit=128;
						for ( i=0; i<8; i++ ) {
							if ( ( x*8+i+px < 192 )&&( y+py < 64 ) ) BdispSetPointVRAM2( x*8+i+px, y+py, (bit&data)!=0 );
							bit>>=1;
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
	char *vram=(char*)PictAry[0];
	bit=ReadBmpHeader( (unsigned char*)filebase, &offset, &width, &height );
	if (  bit == 0 ) return 0;	// not bmp
	DecodeBmp2vram( vram, filebase+offset, width, height, bit, px, py );
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

void CB_BmpSave( char *SRC ) { //	BmpSave "TEST.bmp",Mat A[,Q]
	char fname[32],sname[16];
	int c,i,reg,matsize;
	char* FilePtr;
	int check=0;
	int width,height;
	int bfOffBits = 0x3E;
	int bfSize;
	char *bmpbuffer;
	int dx,dy;
	int dimA,dimB;
	int direct=0;
	int x1,y1,x2,y2;
	int folder=0;
	int gaiji=0,no=-1,flag;

	c =SRC[ExecPtr];
	if ( c == '@' ) { 
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
//	c =SRC[ExecPtr];
//	if ( c != 0x22 ) { CB_Error(SyntaxERR); return; }  // Syntax error

	CB_GetLocateStr(SRC, sname,22); if ( ErrorNo ) return ;	// error
	c =SRC[ExecPtr];
	if ( c != ',' ) { CB_Error(SyntaxERR); return; }  // Syntax error
	ExecPtr++;
	c =SRC[ExecPtr];
	if ( ( c == 0x7F ) || ( SRC[ExecPtr] == 0x40 ) ) {	//	BmpSave "TEST.bmp",Mat A[,Q]
		FilePtr = CB_SaveLoadOprand( SRC, &reg, &matsize);
		dimA =MatAry[reg].SizeA;
		dimB =MatAry[reg].SizeB;
		if ( ErrorNo ) return; // error
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
	if ( direct == 0 ) {
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
			case 128:
				width = dimB*128;
				height= dimA;
				break;
			default:
				CB_Error(ElementSizeERR);
				break;
		}
	} else {
		if ( x1 > x2 ) { i=x1; x1=x2; x2=i; }
		if ( y1 > y2 ) { i=y1; y1=y2; y2=i; }
		width = x2-x1+1;
		height= y2-y1+1;
	}
	c =SRC[ExecPtr];
	if ( c == ',' ) {
		c =SRC[++ExecPtr];
		if ( ( c == 'Q' ) || ( c == 'q' ) ) check=1;
		ExecPtr++;
	}
	
	bfSize = ((((width+7)/8)+3)/4)*4 *height +bfOffBits;
	bmpbuffer = ( char *)HiddenRAM_mallocProg( bfSize+4 );
	if( bmpbuffer == NULL ) Abort();
	memset( bmpbuffer, 0x00, bfSize+4 );
	WriteBmpHeader( bmpbuffer, width, height );
	if ( direct == 0 )	EncodeBmp2mem( bmpbuffer+bfOffBits , FilePtr, width, height );
	else 				EncodeVram2Bmp( bmpbuffer+bfOffBits , width, height, x1, y1 );
	CB_SaveSub( sname, bmpbuffer, bfSize, check, "bmp" );
	HiddenRAM_freeProg( bmpbuffer );	// free

}

void CB_BmpLoad( char *SRC ) { //	BmpLoad("TEST.bmp")[->Mat A]
	char fname[32],sname[16];
	int c,i,reg,matsize,dimA,dimB,ElementSize,base;
	char* FilePtr;
	int width,height;
	int offset,ptr=0,size;
	char* matptr;
	int direct=0;
	int px=0,py=0;
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
	}
	FilePtr=CB_LoadSub( sname, ptr, &size, "bmp" ) ;
	if ( ErrorNo ) return ; // error

	ReadBmpHeader( (unsigned char*)FilePtr, &offset, &width, &height );
	dimA = width;
	dimB = height;
	base = 0;
	ElementSize = 1;
	if ( direct ) {
		DecodeBmp2Vram( FilePtr, px, py );
//		Bdisp_PutDisp_DD_DrawBusy_skip_through(SRC);
		goto exit; 
	}
	DimMatrixSub( reg, ElementSize, dimA, dimB, base ) ;
	if ( ErrorNo ) goto exit; // error
	matptr=(char*)MatAry[reg].Adrs;
	DecodeBmp2mem( matptr , FilePtr+offset, width, height );	//	bmpformat -> bmp
  exit:
	HiddenRAM_freeProg( FilePtr );	// free

}

