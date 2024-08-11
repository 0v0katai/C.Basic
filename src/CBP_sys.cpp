extern "C" {

#include "prizm.h"
#include "CBP.h"
//--------------------------------------------------------------------------------------------- sys call
//--------------------------------------------------------------------------------------------- 
int OS_Version(){	// 03.10.0010 -> 0310
	char *OSverStr = (char*)0x80020020;
	return ( (OSverStr[0]-'0')*1000 + (OSverStr[1]-'0')*100 + (OSverStr[3]-'0')*10 + (OSverStr[4]-'0') ) ;
}

//--------------------------------------------------------------------------------------------- std
unsigned int lastrandom=0x12345678;
unsigned int random( int seed ) {
    if (seed) lastrandom=seed;
    lastrandom = ( 0x41C64E6D*lastrandom ) + 0x3039;
    return ( lastrandom >> 16 );
}

int rand() {
	return random(0);
}
int srand( int seed ) {
	return random(seed);
}

//--------------------------------------------------------------------------------------------- std

int strncmp(const char* s1, const char* s2, size_t n){
    while(n--)
        if(*s1++!=*s2++)
            return *(unsigned char*)(s1 - 1) - *(unsigned char*)(s2 - 1);
    return 0;
}

char* strstr2(const char *str1, const char *str2){
    int i,j,k;
    for(i=j=k=0;str2[j]!='\0';i++){
        if (str1[i]=='\0') return (char *)'\0';
        for(j=0,k=i;str2[j]!='\0' && str1[i+j]==str2[j];j++);
    }
    return (char*)&str1[k];
}

void * memcpy2(void *dest, const void *src, size_t len){
	char *d = (char*)dest+len-1;
	const char *s = (char*)src+len-1;
	while (len--)
    	*d-- = *s--;
  	return dest;
}

//--------------------------------------------------------------------------------------------- math
double atof( char* buf ) {
	int expbuf=ExecPtr;
	ExecPtr=0;
	double r=Eval_atof( buf, *buf);
	ExecPtr=expbuf;
	return r;
}
/*
double fabs(double x){
	if ( x < 0 ) return -x; else return x;
}

double floor( double x ) {
	double result;
	unsigned char *row;
	int tmp,exp;
	int i,j,bt;
	unsigned char *ptr;

	if ( fabs(x)<1 ) { result=0; goto exit; }

	ptr=(unsigned char *)(&x);
	row=(unsigned char *)(&result);
	result=x;

	tmp= ( (row[0]&0x7f)<<4 ) + ( (row[1]&0xf0)>>4 ) ;
	exp=tmp-1023; // 
	if ( ( 0 <= exp ) && ( exp <= 51 ) ) {
		tmp=52-exp;
		i=7; j=0; bt=0xFE;
		while ( tmp ) {
			if (tmp>7) {
				 tmp-=8; row[i]=0; i--;
			} else {
				row[i]&=bt;
				bt=bt<<1;
				j++;
				tmp--;
			}
		}
	}
	exit:
	if ( result < 0 ) result-=1;
	return ( result ); 
}

double fmod(double x,double y){
	if ( y==0 ) return 0;
	return x-floor(x/y)*y;
}


double acos(double x){
	return x;
}
double asin(double x){
	return x;
}
double atan(double x){
	return x;
}
double atan2(double x,double y){
	return x;
}
double cos(double x){
	return icos( (int)(x*180./const_PI+.5), 1024 )/1024.;
//	return cordic_cos( (x*(180./const_PI)) );
//	return cos_fast( x );
}
double sin(double x){
	return isin( (int)(x*180./const_PI+.5), 1024 )/1024.;
//	return cordic_sin( (x*(180./const_PI)) );
//	return sin_fast( x );
}
double tan(double x){
	double c=icos( (int)(x*180./const_PI+.5), 1024 );
	if ( c ) return isin( (int)(x*180./const_PI+.5), 1024 )/c;
	return 0;
//	return cordic_tan( (x*(180./const_PI)) );
//	return tan_fast( x );
}
double cosh(double x){
	return x;
}
double tanh(double x){
	return x;
}
double sinh(double x){
	return x;
}
double exp(double x){
	return x;
}
double log(double x){
	double t2;
	int i,j,k;
	if ( x>=1 ) {
		i=1;
		while ( x>1.00001 ) {
			x=sqrt(x);
			i++; if ( i>64 ) break;
		}
	} else {
		i=1;
		while ( x<0.9999 ) {
			x=sqrt(x);
			i++; if ( i>64 ) break;
		}
	}
	x=(x-1.0)/(x+1.0);
	x=x+x*x*x/3;
	t2=pow(2,i);
	return x*t2;
}
double log10(double x){
	return log(x)/log(10);
}
double pow(double x,double y){
	int i;
	double r=x;
	if ( x==1 ) return 1;
	if ( x==-1 ) {
		if ( fmod(y,2)==0 ) return 1;
		else return -1;
	}
	if ( y>=0 ) {
		for ( i=1; i<(int)y; i++ ) r*=x;
	} else {
		r=1;
		for ( i=0; i<(int)(fabs(y)); i++ ) r/=x;
	}
	return r;
}
double sqrt(double x){
	double a=x,b=x;
	int i=0;
	if ( x==0 ) return 0;
	while (i++<32){
		b=(b+x/b)/2;
		if ( fabs(a-b)<.5e-15 ) break;
		a=b;
	}
	return b;
}
*/
//--------------------------------------------------------------------------------------------- Bfile
int Bfile_OpenFile(const FONTCHARACTER *filename, int mode){
	return Bfile_OpenFile_OS( filename, mode );
}
int Bfile_DeleteFile(const FONTCHARACTER *filename){
	return Bfile_DeleteEntry( filename);
}
int Bfile_DeleteDirectory(const FONTCHARACTER *pathname){
	return Bfile_DeleteEntry( pathname);
}
int Bfile_CreateFile(const FONTCHARACTER *filename, int size){
	size=0;	// refer to http://www.casiopeia.net/forum/viewtopic.php?f=11&t=1740&start=50#p14530
	return Bfile_CreateEntry_OS(  filename, 1, &size  );	// file
}
int Bfile_CreateDirectory(const FONTCHARACTER *pathname){
	int size=0;
	return Bfile_CreateEntry_OS(  pathname, 5, &size  );	// Directory
}
int Bfile_GetMediaFree(enum DEVICE_TYPE devicetype, int *freebytes){
	if  ( devicetype == DEVICE_SD_CARD ) return -1;
	freebytes[0]=1;			// hight
	freebytes[1]=1234567;	// low
	return 0;
}
int Bfile_GetFileSize(int HANDLE){
	return Bfile_GetFileSize_OS( HANDLE, 0 );
}
//--------------------------------------------------------------------------------------------- Bdisp
void SaveDisp( int n ){
//	SaveVRAM_1();
	switch ( n ) {
	case SAVEDISP_PAGE1:
		memcpy( BufVRAM, PictAry[0], VRAMSIZE );
		break;
	case SAVEDISP_PAGE2:
		memcpy( WorkBuf, PictAry[0], VRAMSIZE );
		break;
	}
}

void RestoreDisp(int n){
//	LoadVRAM_1();
	switch ( n ) {
	case SAVEDISP_PAGE1:
		memcpy( PictAry[0], BufVRAM, VRAMSIZE );
		break;
	case SAVEDISP_PAGE2:
		memcpy( PictAry[0], WorkBuf, VRAMSIZE );
		break;
	}
//	unsigned short *buf=(unsigned short *)BufVRAM, *vram=(unsigned short *)PictAry[0];
//	int i;
//	for (i=0; i<216*384; i++ ) *vram++=*buf++;
}

void Bdisp_PutDisp_DD_stripe_BG( int y1, int y2 ) {
	unsigned short *vram;
	unsigned short *tvram;
	unsigned short *pict;
	int *vram2;
	int *tvram2;
	int i;
	unsigned short c,b=CB_BackColorIndex;
	if ( IsEmu ) { y1=0; y2=215; }
	if ( ( CB_G1MorG3M==3 ) && ( CB_BackPict & 1 ) ) {
		vram =(unsigned short *)PictAry[0]+y1*384;
		tvram=(unsigned short *)WorkBuf   +y1*384;
		pict =(unsigned short *)PictBuf   +y1*384;
		for (i=y1*384; i<(y2+1)*384; i++ ) {
			c = *(vram);
			*(tvram++) = c;		// vram -> TVRAM
			if ( c == b ) *(vram) = *(pict) ;
			vram++; pict++; 
		}
		Bdisp_PutDisp_DD_stripe( y1, y2 );

		if ( CB_BackPict & 2 ) {	// Screen.OT
			vram2 =(int *)PictAry[0]+y1*384/2;
			tvram2=(int *)WorkBuf   +y1*384/2;
			for (i=y1*384/2; i<(y2+1)*384/2; i++ ) {
				*(vram2++) = *(tvram2++);	// TVRAM -> vram
			}
		}
		return ;
	} else {
		Bdisp_PutDisp_DD_stripe( y1, y2);
	}
	if ( BreakCheck )if ( KeyScanDownAC() ) { KeyRecover(); BreakPtr=ExecPtr; }	// [AC] break?
}
//------------------------------------------------------------------------
int SysObjectAlign4a( unsigned int n ){ return n; }	// align +4byte
//int SysObjectAlign4b( unsigned int n ){ return n; }	// align +4byte
//int SysObjectAlign4c( unsigned int n ){ return n; }	// align +4byte
//int SysObjectAlign4d( unsigned int n ){ return n; }	// align +4byte
//int SysObjectAlign4e( unsigned int n ){ return n; }	// align +4byte
//------------------------------------------------------------------------

void Bdisp_AllClr_VRAM1(){
	unsigned short *vram=(unsigned short *)PictAry[0]+24*384;
	int i;
	for (i=0; i<192*384; i++ ) *(vram++) = 0xFFFF;
}
void Bdisp_AllClr_VRAM2(){
//	unsigned short *vram=(unsigned short *)PictAry[0]+24*384;
//	unsigned short backcolor=CB_BackColorIndex;
//	int i;
//	for (i=0; i<192*384; i++ ) *(vram++) = backcolor;
	ML_clear_vram();
}
void Bdisp_AllClr_VRAM3( int y0, int y1 ){
	unsigned short *vram=(unsigned short *)PictAry[0]+24*384;
	unsigned short backcolor=CB_BackColorIndex;
	int i;
	for (i=y0*384; i<=y1*384; i++ ) *(vram++) = backcolor;
}
/*
void Bdisp_AllClr_VRAM_mesh( int y0, int y1 , unsigned short color ){
	unsigned short *vram=(unsigned short *)PictAry[0]+24*384;
	unsigned short backcolor=CB_BackColorIndex;
	int i,j;
	for (i=y0; i<=y1; i+=2 ) {
		for (j=0; j<384; j++) {
			*(vram++) = color;
			*(vram++) = backcolor;
		}
		for (j=0; j<384; j++) {
			*(vram++) = backcolor;
			*(vram++) = color;
		}
	}
}
*/
void Bdisp_AllClr_DDVRAM(){
	Bdisp_AllClr_VRAM();
	Bdisp_PutDisp_DD();
}

void Bdisp_AreaReverseVRAM( int x1, int y1, int x2, int y2 ) {
	int x,y;
	unsigned short *vram=(unsigned short *)PictAry[0];
	for ( y=y1; y<=y2; y++) {
		for ( x=x1; x<=x2; x++) {
			vram[x+y*384]^=0xFFFF ;
		}
	}
}
void Bdisp_AreaClr_VRAM( int x1, int y1, int x2, int y2 ) {
	int x,y;
	unsigned short *vram=(unsigned short *)PictAry[0];
	unsigned short color,backcolor=CB_BackColorIndex;
	for ( y=y1; y<=y2; y++) {
		for ( x=x1; x<=x2; x++) {
			vram[x+y*384]=backcolor;
		}
	}
}

void Bdisp_AreaReverseVRAMx3( int x1, int y1, int x2, int y2 ) {
	Bdisp_AreaReverseVRAM( x1*3, y1*3+24, x2*3+2, y2*3+2+24 );
}
void Bdisp_SetPoint_DDVRAM(int x, int y, unsigned char point){
}
void Bdisp_GetDisp_VRAM(unsigned char *pData){
}
void Bdisp_WriteGraph_DD(const DISPGRAPH *WriteGraph){
}
/*
void Bdisp_WriteGraph_VRAM_CG_BitmapWidth(const DISPGRAPH *WriteGraph, int bitmapWidth ){	// 16bit{384,192} -> CG {384,192} 16bit
//	unsigned char  *vram =(unsigned char *)PictAry[0];
	unsigned short *vram2=(unsigned short *)PictAry[0];
	int ptr1,ptr2;
	int i,j,x,y,c;	
	int px = WriteGraph->x ; 
	int py = WriteGraph->y ; 
	int dx = WriteGraph->GraphData.width ;
	int dy = WriteGraph->GraphData.height ;
	unsigned char *Gptr = WriteGraph->GraphData.pBitmap ;
	int Modify = WriteGraph->WriteModify; 
	int Kind =   WriteGraph->WriteKind;
	unsigned short color,backcolor=CB_BackColorIndex;
	unsigned short *Gptr2=(unsigned short *)Gptr;
	if ( CB_ColorIndex >= 0 ) color=CB_ColorIndex; else color=CB_ColorIndexPlot;

	for ( y=py; y<py+dy; y++ ) {	// CG mode
			ptr1=(y-py)*bitmapWidth; ptr2=px+(y+24)*384;
			for ( x=0; x<dx; x++ ) {
				vram2[ptr2++]=Gptr2[ptr1++];
			}
	}
}
*/
void Bdisp_WriteGraph_VRAM_CG_NORMAL_OVER(const DISPGRAPH *WriteGraph, int bitmapWidth ){	// 16bit {384,192} -> RAM  1*1 dot {384,192}
//	unsigned char  *vram =(unsigned char *)PictAry[0];
	unsigned short *vram2=(unsigned short *)PictAry[0];
	int ptr1,ptr2;
	int i,j,x,y,yk;
	int px = WriteGraph->x  ; 
	int py = WriteGraph->y  ; 
	int dx = WriteGraph->GraphData.width ;
	int dy = WriteGraph->GraphData.height ;
	unsigned short *Gptr = (unsigned short *)WriteGraph->GraphData.pBitmap ;
	int Modify = WriteGraph->WriteModify; 
	int Kind =   WriteGraph->WriteKind;
	int dimA,dimB,SizeA;
	unsigned short c,GraphColor;
	unsigned short color,backcolor=CB_BackColorIndex;
	int tcolor=CB_TransparentColorIndex;
	for ( y=0; y<dy; y++ ) {
		i=(y+py) +24;
		if ( (0<=i) && (i<216) ) {
			yk=y*bitmapWidth;
			for ( x=0; x<dx; x++ ) {
				j=x+px;
				if ( (0<=j) && (j<384) ) {
					c=Gptr[ yk + x ];
					if ( tcolor != c ) {
						vram2[i*384+j] =  c ;
					}
				}
			}
		}
	}
}
void Bdisp_WriteGraph_VRAM_CG_OR(const DISPGRAPH *WriteGraph, int bitmapWidth ){	// 16bit {384,192} -> RAM  1*1 dot {384,192}
//	unsigned char  *vram =(unsigned char *)PictAry[0];
	unsigned short *vram2=(unsigned short *)PictAry[0];
	int ptr1,ptr2;
	int i,j,x,y;
	int px = WriteGraph->x  ; 
	int py = WriteGraph->y  ; 
	int dx = WriteGraph->GraphData.width ;
	int dy = WriteGraph->GraphData.height ;
	unsigned short *Gptr = (unsigned short *)WriteGraph->GraphData.pBitmap ;
	int Modify = WriteGraph->WriteModify; 
	int Kind =   WriteGraph->WriteKind;
	int dimA,dimB,SizeA;
	unsigned short c,GraphColor;
	unsigned short color,backcolor=CB_BackColorIndex;
	int tcolor=CB_TransparentColorIndex;
//	if ( (Modify==IMB_WRITEMODIFY_NORMAL) && (Kind==IMB_WRITEKIND_OVER) && (tcolor!=0xFF0000) ){
//		for ( y=py; y<py+dy; y++ ) {	// CG mode
//				ptr1=(y-py)*bitmapWidth; ptr2=px+(y+24)*384;
//				for ( x=0; x<dx; x++ ) {
//					vram2[ptr2++]=Gptr[ptr1++];
//				}
//		}
//	} else 
	for ( y=0; y<dy; y++ ) {
		i=(y+py) +24;
		if ( (0<=i) && (i<216) ) {
			for ( x=0; x<dx; x++ ) {
				GraphColor=Gptr[ y*bitmapWidth+x ];
				switch ( Modify ) {
					case IMB_WRITEMODIFY_NORMAL:
						c=GraphColor;
						break;
					case IMB_WRITEMODIFY_REVERCE:
						c=~GraphColor;
						break;
					case IMB_WRITEMODIFY_MESH:
						if ( (x+px)&1 ^ (y+py)&1 ) c=GraphColor; else goto next;
						break;
				}
				j=x+px;
				if ( (0<=j) && (j<384) ) {
					if ( tcolor != c ) {
						ptr1=j+i*384;
						switch ( Kind ) {
							case IMB_WRITEKIND_OR:
								vram2[ptr1] |= c ;
								break;
							case IMB_WRITEKIND_OVER:
								vram2[ptr1] =  c ;
								break;
							case IMB_WRITEKIND_AND:
								vram2[ptr1] &= c ;
								break;
							case IMB_WRITEKIND_XOR:
								vram2[ptr1] ^= c ;
								break;
						}
					}
				}
				next:{}
			}
		}
	}
}
void Bdisp_WriteGraph_VRAM_CG(const DISPGRAPH *WriteGraph, int bitmapWidth ){	// 16bit {384,192} -> RAM  1*1 dot {384,192}
	if ( WriteGraph->WriteModify == IMB_WRITEMODIFY_NORMAL ) {
		switch ( WriteGraph->WriteKind ) {
//			case IMB_WRITEKIND_OR:
//				Bdisp_WriteGraph_RAM_CG_1bit_NORMAL_OR( WriteGraph, (unsigned char*)PictAry[0] );
//				return;
			case IMB_WRITEKIND_OVER:
				Bdisp_WriteGraph_VRAM_CG_NORMAL_OVER( WriteGraph, bitmapWidth );
				return;
		}
	}
	Bdisp_WriteGraph_VRAM_CG_OR( WriteGraph, bitmapWidth );

}
/*
void Bdisp_WriteGraph_RAM_CG_1bit_NORMAL_OR(const DISPGRAPH *WriteGraph, unsigned char* ram ){	// 1bit {384,192} -> RAM  1*1 dot {384,192}
	unsigned char  *vram = (unsigned char *)ram;
	unsigned short *vram2= (unsigned short *)ram;
	int ptr1,ptr2;
	int i,j,x,y,c,yi;	
	int px = WriteGraph->x  ; 
	int py = WriteGraph->y  ; 
	int dx = WriteGraph->GraphData.width ;
	int dy = WriteGraph->GraphData.height ;
	unsigned char *Gptr = WriteGraph->GraphData.pBitmap ;
	unsigned short color,backcolor=CB_BackColorIndex;
	if ( CB_ColorIndex >= 0 ) color=CB_ColorIndex; else color=CB_ColorIndexPlot;
	for ( y=0; y<dy; y++ ) {
		yi=((y+py) +24);
		if ( (0<=yi) && (yi<216) ) {
			for ( x=0; x<dx; x++ ) {
				i=Gptr[ y*(((dx-1)>>3)+1)+(x>>3) ] & (128>>(x&7));
				if ( i ) c=color; else c=backcolor;
				j=x+px;
				if ( (0<=j) && (j<384) ) {
					if ( c == color ) vram2[j+yi*384]=c;
				}
			}
		}
	}
}
*/
void Bdisp_WriteGraph_RAM_CG_1bit_NORMAL_OVER(const DISPGRAPH *WriteGraph, unsigned char* ram ){	// 1bit {384,192} -> RAM  1*1 dot {384,192}
	unsigned char  *vram = (unsigned char *)ram;
	unsigned short *vram2= (unsigned short *)ram;
	int ptr1,ptr2;
	int i,j,x,y,c,yi;	
	int px = WriteGraph->x  ; 
	int py = WriteGraph->y  ; 
	int dx = WriteGraph->GraphData.width ;
	int dy = WriteGraph->GraphData.height ;
	unsigned char *Gptr = WriteGraph->GraphData.pBitmap ;
	unsigned short color,backcolor=CB_BackColorIndex;
	if ( CB_ColorIndex >= 0 ) color=CB_ColorIndex; else color=CB_ColorIndexPlot;
	j=px+dx;
	if ( (0<=px) && (px+dx<384) && (-24<=py) && (py+dy<192) ) {
		for ( y=0; y<dy; y++ ) {
			yi=y+py+24;
				for ( x=0; x<dx; x++ ) {
					i=Gptr[ y*(((dx-1)>>3)+1)+(x>>3) ] & (128>>(x&7));
					if ( i ) c=color; else c=backcolor;
					j=x+px;
					vram2[j+yi*384]=c;
				}
		}
	} else {
		for ( y=0; y<dy; y++ ) {
			yi=y+py+24;
			if ( (0<=yi) && (yi<216) ) {
				for ( x=0; x<dx; x++ ) {
					i=Gptr[ y*(((dx-1)>>3)+1)+(x>>3) ] & (128>>(x&7));
					if ( i ) c=color; else c=backcolor;
					j=x+px;
					if ( (0<=j) && (j<384) ) {
						vram2[j+yi*384]=c;
					}
				}
			}
		}
	}
}

void Bdisp_WriteGraph_VRAM_CG_1bit(const DISPGRAPH *WriteGraph){	// 1bit {384,192} -> VRAM  1*1 dot {384,192}
	if ( WriteGraph->WriteModify == IMB_WRITEMODIFY_NORMAL ) {
		switch ( WriteGraph->WriteKind ) {
//			case IMB_WRITEKIND_OR:
//				Bdisp_WriteGraph_RAM_CG_1bit_NORMAL_OR( WriteGraph, (unsigned char*)PictAry[0] );
//				return;
			case IMB_WRITEKIND_OVER:
				Bdisp_WriteGraph_RAM_CG_1bit_NORMAL_OVER( WriteGraph, (unsigned char*)PictAry[0] );
				return;
		}
	}
	Bdisp_WriteGraph_RAM_CG_1bit( WriteGraph, (unsigned char*)PictAry[0] );
}

void Bdisp_WriteGraph_RAM_CG_1bit(const DISPGRAPH *WriteGraph, unsigned char* ram ){	// 1bit {384,192} -> RAM  1*1 dot {384,192}
	unsigned char  *vram = (unsigned char *)ram;
	unsigned short *vram2= (unsigned short *)ram;
	int ptr1,ptr2;
	int i,j,x,y,c,yi;	
	int px = WriteGraph->x  ; 
	int py = WriteGraph->y  ; 
	int dx = WriteGraph->GraphData.width ;
	int dy = WriteGraph->GraphData.height ;
	unsigned char *Gptr = WriteGraph->GraphData.pBitmap ;
	int Modify = WriteGraph->WriteModify; 
	int Kind =   WriteGraph->WriteKind;
//	int dimA,dimB,SizeA;
	unsigned short color,backcolor=CB_BackColorIndex;
	if ( CB_ColorIndex >= 0 ) color=CB_ColorIndex; else color=CB_ColorIndexPlot;
//	dimA=(px+dx)/8;
//	dimB=dy;
//	SizeA=dx;
	for ( y=0; y<dy; y++ ) {
		yi=y+py+24;
		if ( (0<=yi) && (yi<216) ) {
			for ( x=0; x<dx; x++ ) {
				i=Gptr[ y*(((dx-1)>>3)+1)+(x>>3) ] & (128>>(x&7));
				switch ( Modify ) {
					case IMB_WRITEMODIFY_NORMAL:
						if ( i ) c=color; else c=backcolor;
						break;
					case IMB_WRITEMODIFY_REVERCE:
						if ( i ) c=backcolor; else c=color;
						break;
					case IMB_WRITEMODIFY_MESH:
						if ( i ) c=color; else c=backcolor;
						if ( (x+px)&1 ^ (y+py)&1 ) c=backcolor;
						break;
				}
				j=x+px;
				if ( (0<=j) && (j<384) ) {
					ptr1=j+yi*384;
					switch ( Kind ) {
						case IMB_WRITEKIND_OR:
							if ( c==backcolor ) break;
						case IMB_WRITEKIND_OVER:
							vram2[ptr1]=c;
							break;
						case IMB_WRITEKIND_AND:
							if ( vram2[ptr1] == backcolor ) c=backcolor;
							vram2[ptr1]=c;
							break;
						case IMB_WRITEKIND_XOR:
							if ( ( c==color ) ^ ( vram2[ptr1]==backcolor ) ) c=backcolor; else c=color;
							vram2[ptr1]=c;
							break;
					}
				}
			}
		}
	}
}

void Bdisp_WriteGraph_RAM_CG_1bit_75(const DISPGRAPH *WriteGraph, unsigned char* ram ){	// 1bit {384,192} -> RAM  1*1 dot {384,192} 75% reduce
	unsigned char  *vram = (unsigned char *)ram;
	unsigned short *vram2= (unsigned short *)ram;
	int ptr1,ptr2;
	int i,j,x,y,c,k;	
	int px = WriteGraph->x  ; 
	int py = WriteGraph->y  ; 
	int dx = WriteGraph->GraphData.width ;
	int dy = WriteGraph->GraphData.height ;
//	int xx[]={ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15,16,17,18,19,20,21,22,23};
	int xx[]={ 0, 1, 2,    4, 5, 6,    8, 9,10,   12,13,14,   16,17,18,   20,21,22};
	unsigned char *Gptr = WriteGraph->GraphData.pBitmap ;
	int Modify = WriteGraph->WriteModify; 
	int Kind =   WriteGraph->WriteKind;
//	int dimA,dimB,SizeA;
	unsigned short color,backcolor=CB_BackColorIndex;
	if ( CB_ColorIndex >= 0 ) color=CB_ColorIndex; else color=CB_ColorIndexPlot;
//	dimA=(px+dx)/8;
//	dimB=dy;
//	SizeA=dx;
	for ( y=0; y<dy; y++ ) {
		for ( x=0; x<dx; x++ ) {
//			i=(x*87382)>>16;	// 24->18dot
//			i=Gptr[ y*(((dx-1)>>3)+1)+(i>>3) ] & (192>>(i&7));
			i=xx[x];
			if ( (i&3)==2 ) {
				i=Gptr[ y*(((dx-1)>>3)+1)+(i>>3) ] & (192>>(i&7));
			} else {
				i=Gptr[ y*(((dx-1)>>3)+1)+(i>>3) ] & (128>>(i&7));
			}
			switch ( Modify ) {
				case IMB_WRITEMODIFY_NORMAL:
					if ( i ) c=color; else c=backcolor;
					break;
				case IMB_WRITEMODIFY_REVERCE:
					if ( i ) c=backcolor; else c=color;
					break;
				case IMB_WRITEMODIFY_MESH:
					if ( i ) c=color; else c=backcolor;
					if ( (x+px)&1 ^ (y+py)&1 ) c=backcolor;
					break;
			}
			i=(y+py) +24; j=x+px;
			if ( (0<=j) && (0<=i) && (j<384) && (i<216) ) {
				ptr1=j+i*384;
				switch ( Kind ) {
					case IMB_WRITEKIND_OR:
						if ( c==backcolor ) break;
					case IMB_WRITEKIND_OVER:
						vram2[ptr1]=c;
						break;
					case IMB_WRITEKIND_AND:
						if ( vram2[ptr1] == backcolor ) c=backcolor;
						vram2[ptr1]=c;
						break;
					case IMB_WRITEKIND_XOR:
						if ( ( c==color ) ^ ( vram2[ptr1]==backcolor ) ) c=backcolor; else c=color;
						vram2[ptr1]=c;
						break;
				}
			}
		}
	}
}
void Bdisp_WriteGraph_VRAM_CG_1bit_75(const DISPGRAPH *WriteGraph){	// 1bit {384,192} -> VRAM  1*1 dot {384,192} 75% reduce
	Bdisp_WriteGraph_RAM_CG_1bit_75( WriteGraph, (unsigned char*)PictAry[0] );
}



void Bdisp_WriteGraph_RAM_fx_xy1(const DISPGRAPH *WriteGraph, unsigned char* ram, int factor ){	// 1bit {128,64}or{384,192} ->  RAM 3*3 dot {384,192}
	unsigned char  *vram = (unsigned char *)ram;
	unsigned short *vram2= (unsigned short *)ram;
	int ptr1,ptr2;
	int i,j,x,y,c;	
	int px = WriteGraph->x *factor ; 
	int py = WriteGraph->y *factor ; 
	int dx = WriteGraph->GraphData.width ;
	int dy = WriteGraph->GraphData.height ;
	unsigned char *Gptr = WriteGraph->GraphData.pBitmap ;
	int Modify = WriteGraph->WriteModify; 
	int Kind =   WriteGraph->WriteKind;
//	int dimA,dimB,SizeA;
	unsigned short color,backcolor=CB_BackColorIndex;
	if ( CB_ColorIndex >= 0 ) color=CB_ColorIndex; else color=CB_ColorIndexPlot;
//	dimA=(px+dx)/8;
//	dimB=dy;
//	SizeA=dx;
//			return ( MatAryC[ dimB*(((SizeA-1)>>3)+1)+(dimA>>3) ] & ( 128>>(dimA&7) ) ) != 0 ;
//			return ( MatAryC[ dimB*(((SizeA-1)>>3)+1)+(dimA>>3) ] & (128>>(dimA&7))  ) != 0 ;
//			return ( MatAryC[ y*(((dx-1)>>3)+1)+(x>>3) ] & (128>>(x&7))  ) != 0 ;
	for ( y=0; y<dy; y++ ) {
		for ( x=0; x<dx; x++ ) {
			i=Gptr[ y*(((dx-1)>>3)+1)+(x>>3) ] & (128>>(x&7));
			switch ( Modify ) {
				case IMB_WRITEMODIFY_NORMAL:
					if ( i ) c=color; else c=backcolor;
					break;
				case IMB_WRITEMODIFY_REVERCE:
					if ( i ) c=backcolor; else c=color;
					break;
				case IMB_WRITEMODIFY_MESH:
					if ( i ) c=color; else c=backcolor;
					if ( (x+px)&1 ^ (y+py)&1 ) c=backcolor;
					break;
			}
			i=(y*3+py) +24; j=x*3+px;
			if ( (0<=j) && (0<=i) && (j<382) && (i<214) ) {
				ptr1=j+i*384;
				switch ( Kind ) {
					case IMB_WRITEKIND_OR:
						if ( c==backcolor ) break;
					case IMB_WRITEKIND_OVER:
					  pset:
						vram2[ptr1++]=c;
						vram2[ptr1++]=c;
						vram2[ptr1++]=c;
						ptr1+=381;
						vram2[ptr1++]=c;
						vram2[ptr1++]=c;
						vram2[ptr1++]=c;
						ptr1+=381;
						vram2[ptr1++]=c;
						vram2[ptr1++]=c;
						vram2[ptr1++]=c;
						break;
					case IMB_WRITEKIND_AND:
						if ( vram2[ptr1] == backcolor ) c=backcolor;
						goto pset;
						break;
					case IMB_WRITEKIND_XOR:
						if ( ( c==color ) ^ ( vram2[ptr1]==backcolor ) ) c=backcolor; else c=color;
						goto pset;
						break;
				}
			}
		}
	}
}
void Bdisp_WriteGraph_RAM_fx( const DISPGRAPH *WriteGraph, unsigned char* ram  ){	// 1bit {128,64} ->  RAM 3*3 dot {384,192}
	Bdisp_WriteGraph_RAM_fx_xy1( WriteGraph, ram, 3 );
}
void Bdisp_WriteGraph_VRAM_fx( const DISPGRAPH *WriteGraph){	// 1bit {128,64} ->  VRAM 3*3 dot {384,192}
	Bdisp_WriteGraph_RAM_fx_xy1( WriteGraph, (unsigned char*)PictAry[0], 3 );
}
void Bdisp_WriteGraph_VRAM_fx_xy1(const DISPGRAPH *WriteGraph){	// 1bit {384,192} -> VRAM 3*3 dot {384,192}
	Bdisp_WriteGraph_RAM_fx_xy1( WriteGraph, (unsigned char*)PictAry[0], 1 );
}

void Bdisp_WriteGraph_VRAM(const DISPGRAPH *WriteGraph){	//
	if ( CB_G1MorG3M==1 )	Bdisp_WriteGraph_VRAM_fx(WriteGraph);
	else					Bdisp_WriteGraph_VRAM_CG(WriteGraph, WriteGraph->GraphData.width);
}


void Bdisp_ReadArea_VRAM_CG(const DISPBOX *ReadArea, unsigned char *ReadData){	// CG (384,192) 16bit -> 16bit data
	unsigned char  *vram =(unsigned char*)PictAry[0];
	unsigned short *vram2=(unsigned short*)PictAry[0];
	int ptr1,ptr2;
	int i,j,x,y,c;	
	int x1=ReadArea->left; 
	int y1=ReadArea->top; 
	int x2=ReadArea->right;
	int y2=ReadArea->bottom;
	unsigned short color,backcolor=CB_BackColorIndex;
	unsigned short *ReadData2=(unsigned short *)ReadData;
	if ( CB_ColorIndex >= 0 ) color=CB_ColorIndex; else color=CB_ColorIndexPlot;

	for ( y=y1; y<=y2; y++ ) {	// CG mode
			ptr1=(y-y1)*(x2-x1+1); ptr2=x1+(y+24)*384;
			for ( x=x1; x<=x2; x++ ) {
				ReadData2[ptr1++]=vram2[ptr2++];
			}
	}
}
void Bdisp_ReadArea_RAM_fx(const DISPBOX *ReadArea, unsigned char *ReadData, unsigned char* ram ){	// fx 3*3 dot {384,192} mem -> 1bit data {128,64}
	unsigned char  *vram =(unsigned char*)ram;
	unsigned short *vram2=(unsigned short*)ram;
	int ptr1,ptr2;
	int i,j,x,y,c;	
	int x1=ReadArea->left; 
	int y1=ReadArea->top; 
	int x2=ReadArea->right;
	int y2=ReadArea->bottom;
	int dx=x2-x1;
	unsigned short color,backcolor=CB_BackColorIndex;
	if ( CB_ColorIndex >= 0 ) color=CB_ColorIndex; else color=CB_ColorIndexPlot;

	for ( i=y1; i<=y2; i++ ) {	// fx mode
		for ( j=x1; j<=x2; j++ ) {
			x=j-x1; y=i-y1;
			if ( vram2[j*3+1+(i*3+25)*384] != backcolor ) {
				ReadData[y*(((dx)>>3)+1)+(x>>3)] |= 128>>(x&7);		// 1
			} else {
				ReadData[y*(((dx)>>3)+1)+(x>>3)] &= ~(128>>(x&7));	// 0
			}
		}
	}
}
void Bdisp_ReadArea_RAM_CG_1bit(const DISPBOX *ReadArea, unsigned char *ReadData, unsigned char* ram ){	// CG {384,192} mem -> 1bit data {384,192}
	unsigned char  *vram =(unsigned char*)ram;
	unsigned short *vram2=(unsigned short*)ram;
	int ptr1,ptr2;
	int i,j,x,y,c;	
	int x1=ReadArea->left; 
	int y1=ReadArea->top; 
	int x2=ReadArea->right;
	int y2=ReadArea->bottom;
	int dx=x2-x1;
	unsigned short color,backcolor=CB_BackColorIndex;
	if ( CB_ColorIndex >= 0 ) color=CB_ColorIndex; else color=CB_ColorIndexPlot;

	for ( i=y1; i<=y2; i++ ) {	// fx mode
		for ( j=x1; j<=x2; j++ ) {
			x=j-x1; y=i-y1;
			if ( vram2[j+(i+24)*384] != backcolor ) {
				ReadData[y*(((dx)>>3)+1)+(x>>3)] |= 128>>(x&7);		// 1
			} else {
				ReadData[y*(((dx)>>3)+1)+(x>>3)] &= ~(128>>(x&7));	// 0
			}
		}
	}
}

void Bdisp_ReadArea_VRAM_fx(const DISPBOX *ReadArea, unsigned char *ReadData){	// fx 3*3 dot {384,192} mem -> 1bit data {128,64}
	Bdisp_ReadArea_RAM_fx( ReadArea, ReadData, (unsigned char*)PictAry[0] );
}
void Bdisp_ReadArea_VRAM_CG_1bit(const DISPBOX *ReadArea, unsigned char *ReadData){	// CG 1*1 fot {384,192} VRAM -> 1bit data{384,192}
	Bdisp_ReadArea_RAM_CG_1bit( ReadArea, ReadData, (unsigned char*)PictAry[0] );
}

void Bdisp_ReadArea_VRAM(const DISPBOX *ReadArea, unsigned char *ReadData){	// CG or fx
	if ( CB_G1MorG3M==1 ) 
		Bdisp_ReadArea_VRAM_fx( ReadArea, ReadData );
	else
		Bdisp_ReadArea_VRAM_CG( ReadArea, ReadData );
}

void Bdisp_DrawLineVRAM( int x1, int y1, int x2, int y2 ) {
	LinesubFX(x1, y1, x2, y2, S_L_Normal, 1);
}
void Bdisp_ClearLineVRAM( int x1, int y1, int x2, int y2 ) {
	LinesubFX(x1, y1, x2, y2, S_L_Normal, 0);
}

void DD_SetContrast( int n ) {
}

void Prints( const unsigned char *msg ){
	Print_OS( (unsigned char*)msg, 0, 0 );
}

void PrintRevs( const unsigned char *msg ){
	Print_OS( (unsigned char*)msg, 1, 0 );
}

void PrintCXY1(int x, int y, unsigned char*cptr, int mode_flags, int P5, int color, int back_color, int P8, int P9 ){
	int mode=0;
	if ( y<0 ) { mode=0x40; y+=24; }
	PrintCXY( x, y, cptr, mode_flags|mode, P5, color, back_color, P8, P9 );
}

int PrintMinix1( int X, int Y, unsigned char*buf, int mode_flags, unsigned short  color, unsigned short  backcolor ) {
	int x0=X;
	int mode=0;
	if ( Y<0 ) { mode=0x40; Y+=24; }
	switch ( mode_flags & 0xFF ) {
		case MINI_OVER:
			PrintMini(&X, &Y, (unsigned char*)buf, 0|mode, 0xFFFFFFFF, 0, 0, color, backcolor, 1, 0);
			break;
		case MINI_OR:
			PrintMini(&X, &Y, (unsigned char*)buf, 2|mode, 0xFFFFFFFF, 0, 0, color, backcolor, 1, 0);
			break;
		case MINI_REV:
			PrintMini(&X, &Y, (unsigned char*)buf, 0|mode, 0xFFFFFFFF, 0, 0, backcolor, color, 1, 0);
			break;
		case MINI_REVOR:
			PrintMini(&X, &Y, (unsigned char*)buf, 2|mode, 0xFFFFFFFF, 0, 0, ~color, ~backcolor, 1, 0);
			break;
	}
	return (X-x0);
}

int PrintMinix3( int X, int Y, unsigned char*buf, int mode ) {
	unsigned short color=0x0000;	// black
	unsigned short backcolor=0xFFFF;	// white
	X*=3;
	Y*=3;
	return PrintMinix1( X, Y, buf, mode, color, backcolor ) /3;
}

int PrintMinix1orWB( int X, int Y, unsigned char*buf ) {
	unsigned short color=0x0000;	// black
	unsigned short backcolor=0xFFFF;	// white
	PrintMinix1( X+1, Y+1, buf, MINI_OR, backcolor, backcolor );
	return PrintMinix1( X, Y, buf, MINI_OR, color, backcolor );
}
int PrintMinix3orWB( int X, int Y, unsigned char*buf ) {
	return PrintMinix1orWB( X*3, Y*3, buf ) /3;
}

int GetColorIndex( int color ) {
	int i;
	EnableDisplayStatusArea();
	i=ColorIndexDialog0( 0 );
	switch ( i ) {
		case 0:	// black
			color=0x0000;	// Black
			break;
		case 1:	// blue
			color=0x001F;	// Blue
			break;
		case 2:	// green
			color=0x07E0;	// Green
			break;
		case 3:	// cyan
			color=0x07FF;	// Cyan
			break;
		case 4:	// red
			color=0xF800;	// Red
			break;
		case 5:	// magenta
			color=0xF81F;	// Magenta
			break;
		case 6:	// yellow
			color=0xFFE0;	// Yellow
			break;
		case 7:	// white
			color=0xFFFF;	
			break;
		default:
			break;
	}
	return color;
}
int GetColor2Index( int colorRGB ) {
	int color;
	switch ( colorRGB ) {
		case 0:	// black
			color=0x0000;	// Black
			break;
		case 0x001F:	// blue
			color=1;	// Blue
			break;
		case 0x07E0:	// green
			color=2;	// Green
			break;
		case 0x07FF:	// cyan
			color=3;	// Cyan
			break;
		case 0xF800:	// red
			color=4;	// Red
			break;
		case 0xF81F:	// magenta
			color=5;	// Magenta
			break;
		case 0xFFE0:	// yellow
			color=6;	// Yellow
			break;
		case 0xFFFF:	// white
			color=7;	// white
			break;
		default:
			color=8;	// color
			break;
	}
	return color;
}

void CB_ScrollBar(int max, int pos, int height, int x1, int y1, int dx, int dy ) {
	TScrollbar scrollbar;
	scrollbar.I1=0;
	scrollbar.indicatormaximum= max;
	scrollbar.indicatorheight=height;
	scrollbar.indicatorpos= pos;
	scrollbar.I5=0;
	scrollbar.barleft= x1;		//  384-6;
	scrollbar.bartop=  y1;		//  0;
	scrollbar.barheight= dx;	// 168;
	scrollbar.barwidth=  dy;	//   6;
	
	Scrollbar( &scrollbar );
}

void CB_StandardScrollBar(int max, int pos ) {
	short TStandardScrollbar[0x12];
	TStandardScrollbar[0x09]=pos;
	TStandardScrollbar[0x10]=max;
	StandardScrollbar( &TStandardScrollbar );
}


//---------------------------------------------------------------------------------------------

void EnableDisplayStatusArea(){
    EnableStatusArea(0);	// enable StatusArea
	StatusArea_SetGlyph( 0 );
	if ( DebugMode >=1 ) // debug mode
		SysCalljmp( 3,7,6,7,0x1D75);	// backcolor of status line to check color (by Colon)
	DisplayStatusArea();
}
void EnableDisplayStatusArea2(){
	char stbuf[384*2*24];
	if ( CB_StatusDisp ) {
//		memcpy( stbuf, PictAry[0], 384*2*24);		// status line image save
		StatusArea_Run();
		Bdisp_PutDisp_DD_stripe( 0, 23);
//		EnableDisplayStatusArea();
//		EnableStatusArea(3);	// disable StatusArea
//		memcpy( PictAry[0], stbuf, 384*2*24);		// status line image restore
	}
	return ;
}

void SetStatusMessage_( char*msg, char short_color ){
//	DefineStatusMessage( msg, 1, short_color, 7 );
	char spc[]="                     ";
	char buf[64];
	buf[0]='\0';
	strcpy( buf, spc);
	strcat( buf, msg);
	DefineStatusMessage( buf, 1, short_color, 0 );
	DisplayStatusArea();
}
//
void StatusArea_( char*msg, char short_color ){
//	Bdisp_AllClr_VRAM();
	SetStatusMessage_( msg, short_color );
}

void StatusArea_sub(){
    EnableStatusArea(0);	// enable StatusArea
    DefineStatusAreaFlags(DSA_SETDEFAULT, 0, 0, 0);
    DefineStatusAreaFlags(3, SAF_BATTERY | SAF_TEXT | SAF_GLYPH | SAF_ALPHA_SHIFT, 0, 0);
	StatusArea_( (char*)VerMSG,TEXT_COLOR_PURPLE);
	EnableStatusArea(3);	// disable StatusArea
}
void StatusArea_Edit( char *name, int num ){
	char buf[64];
    EnableStatusArea(0);	// enable StatusArea
    DefineStatusAreaFlags(DSA_SETDEFAULT, 0, 0, 0);
    DefineStatusAreaFlags(3, SAF_BATTERY | SAF_TEXT | SAF_GLYPH | SAF_ALPHA_SHIFT , 0, 0);
    sprintf3(buf,"%s  free:%d", name, num);
	StatusArea_( buf,TEXT_COLOR_PURPLE);
	EnableStatusArea(3);	// disable StatusArea
}
void StatusArea_Run_sub( char *str, int intmode, int g1morg3m ){
	char buf[64];
    const char *angle[]  ={"Deg","Rad","Gra"};
    EnableStatusArea(0);	// enable StatusArea
    DefineStatusAreaFlags(DSA_SETDEFAULT, 0, 0, 0);
    DefineStatusAreaFlags(3, SAF_BATTERY | SAF_TEXT | SAF_GLYPH | SAF_ALPHA_SHIFT , 0, 0);
	StatusArea_SetGlyph( 0 );
//	sprintf3(buf, "[%s %s%d %s] %-8s", angle[Angle], CBmode[CB_INT], MatBase, CB_G1MorG3M==1 ? "g1m" : "g3m", str);
	StatusArea_( str,TEXT_COLOR_PURPLE);
	EnableStatusArea(3);	// disable StatusArea
}
void StatusArea_Run(){
	char buf2[32];
	strncpy(buf2,(const char*)ProgfileAdrs[ProgNo]+0x3C,8);
	buf2[8]='\0';
	StatusArea_Run_sub( buf2, CB_INT, CB_G1MorG3M );
}
void StatusArea_Time(){
	char buf1[16];
	char buf2[16];
	char buf3[32];
	DateToStr( buf1 );
	TimeToStr( buf2 ); buf2[5]='\0';
    sprintf3(buf3,"%s %s", buf1+5, buf2);
	StatusArea_Run_sub( buf3, CB_INT, CB_G1MorG3M );
}

//---------------------------------------------------------------------------------------------

void Cursor_SetFlashMode(int mode){ 			// cursor 
	Cursor_SetFlashOn( mode );
}
int Cursor_GetFlashStyle(){
	unsigned int settingsarray[5];
	Cursor_GetSettings( settingsarray);
	return settingsarray[2];	// flashstyle
}


//--------------------------------------------------------------------------------------------- Timer
int timer_id0 = 0;	// Time id
int timer_id1 = 0;	// dummy
int timer_id  = 0;
int timer_id2 = 0;	// dummy

int SetTimer(int ID, int elapse, void (*handler)(void)){
	int id;
	id=Timer_Install( ID,  (void*)handler, elapse ); 
	Timer_Start( id );
	return id;
}

int KillTimer(int ID){
	Timer_Stop( ID );
	return Timer_Deinstall( ID );
}
void Timer_Dummy(){
}
void Set_Timer_id(){
	timer_id0 = SetTimer(timer_id0, 1000, &Timer_Dummy );
	KillTimer(timer_id0);
}

//--------------------------------------------------------------------------------------------- Serial

//int Serial_GetFreeTransmitSpace_tmp(){
//	return 1;
//} 
//int Serial_BufferedTransmitOneByte_tmp( int c ){
//	return 1;
//}
//int Serial_ReadOneByte_tmp( char c ){
//	return 1;
//}
//int Serial_GetReceivedBytesAvailable_tmp(){
//	return 1;
//}
//void Serial_ClearTransmitBuffer_tmp(){
//}

//---------------------------------------------------------------------------------------------
// the function assumes, that the RAM-pointer to GetkeyToMainFunctionReturnFlag is loaded 
// immediately by a "Move Immediate Data"-instruction
unsigned int SetGetkeyToMainFunctionReturnFlag( unsigned int enabled ){
int addr, addr2;
// get the pointer to the syscall table
	addr = *(unsigned char*)0x80020071;	// get displacement
	addr++;
	addr *= 4;
	addr += 0x80020070;
	addr = *(unsigned int*)addr;
	if ( addr < 0x80020070 ) return 0x101;
	if ( addr >= 0x81000000 ) return 0x102;
// get the pointer to syscall 1E99
	addr += 0x1E99*4;
	if ( addr < 0x80020070 ) return 0x103;
	if ( addr >= 0x81000000 ) return 0x104;

	addr = *(unsigned int*)addr;
	if ( addr < 0x80020070 ) return 0x105;
	if ( addr >= 0x81000000 ) return 0x106;

	switch ( *(unsigned char*)addr ){
		case 0xD0 : // MOV.L @( disp, PC), Rn (REJ09B0317-0400 Rev. 4.00 May 15, 2006 page 216)
		case 0xD1 :
		case 0xD2 :
		case 0xD3 :
		case 0xD4 :
		case 0xD5 :
		case 0xD6 :
		case 0xD7 :
		case 0xD8 :
			addr2 = *(unsigned char*)( addr + 1 );	// get displacement
			addr2++;
			addr2 *= 4;
			addr2 += addr;
			addr2 &= ~3;

			if ( addr2 < 0x80020070 ) return 0x107;
			if ( addr2 >= 0x81000000 ) return 0x108;

			addr = *(unsigned int*)addr2;
			if ( ( addr & 0xF8000000 ) != 0x88000000 ) return 0x109;

// finally perform the desired operation and set the flag:
			if ( enabled ) *(unsigned char*)addr = 0;
			else *(unsigned char*)addr = 3;

			break;

		default : addr = 0x100;
	}
	return addr;
}
//---------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------
int Ticks32768=0;
unsigned int GetTicks32768(){
	return *(unsigned int*)P7305_EXTRA_TMU5_COUNT;
}
void WaitTimer32768() {
	unsigned int t=GetTicks32768();
	while ( GetTicks32768() == t  );
}


}
