
	
#include "prizm.h"
#include "CBP.h"

//---------------------------------------------------------------------------------------------
const char fkey_select[]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3F,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFC,0x60,0x00,0x00,0x00,0x00,0x00,0x00,0x06,0x4F,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xF2,0x5F,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFA,0x5F,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFA,0x5F,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFA,0x5F,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFA,0x5F,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFA,0x5F,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFA,0x5F,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFA,0x5F,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFA,0x5F,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFA,0x5F,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFA,0x5F,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFA,0x5F,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFA,0x5F,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFA,0x5F,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFA,0x5F,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFA,0x5F,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFA,0x4F,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xF2,0x60,0x00,0x00,0x00,0x00,0x00,0x00,0x06,0x3F,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFC,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
//---------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------
void FKey_Display_color( int n, unsigned int *data, int color, int backcolor ){
	int col=CB_ColorIndex;
	unsigned short bcol=CB_BackColorIndex;
//                       Black   Blue    Green   Cyan    Red     Magenta Yellow  White
    int colortable[16]={ 0x0000, 0x001F, 0x07E0, 0x07FF, 0xF800, 0xF81F, 0xFFE0, 0xFFFF, 0,0,0,0,0,0,0,0};
	CB_BackColorIndex = ( unsigned short )backcolor;
	if ( color>=0 ) {
		CB_ColorIndex=color; 
	} else {
		CB_ColorIndex=colortable[ Bdisp_FkeyColor( 1, 0) ] ;	// get system fkey color
	}
	ML_bmp_over((unsigned char *)data, n*64, 168, 64, 24, 0 );
	CB_ColorIndex=col;
	CB_BackColorIndex=bcol;
}

//---------------------------------------------------------------------------------------------

void Fkey_Icon(int n, int IconNo){
	unsigned int* iresult;	
	if ( ( IconNo==1325 ) && (OS_Version()<=300) )  { Fkey_dispN( n, "Extd"); return ; }
	GetFKeyPtr( IconNo, &iresult);	//  
	FKey_Display(n, iresult);
}
void Fkey_Icon_color(int n, int IconNo, int color, int backcolor){
	unsigned int* iresult;	
	if ( 
//		 ( ( IconNo>1248+6 ) && (OS_Version()<=104) ) ||
//		 ( ( IconNo>1276+6 ) && (OS_Version()<=300) ) ||
//		 ( ( IconNo>1378+6 ) && (OS_Version()<=310) ) ||
//		 ( ( IconNo>1379+6 ) && (OS_Version()<=311) ) ||
//		 ( ( IconNo>1394+6 ) && (OS_Version()<=320) ) ||
//		 ( ( IconNo>1400+6 ) && (OS_Version()<=330) ) ||
		 ( IconNo>1511 ) ) {
		 IconNo=0;
	}
	if ( ( IconNo==1325 ) && (OS_Version()<=300) )  { Fkey_dispN_ext( n, "Extd", 0, 0, color, backcolor); return ; }
	GetFKeyPtr( IconNo, &iresult);	//  
	FKey_Display_color(n, iresult, color, backcolor);
}

//---------------------------------------------------------------------------------------------
int GetFkeyColor(int n){
	unsigned short *VRAM=(unsigned short *)PictAry[0];
	return VRAM[ n*64+3 +193*384 ];
}
int GetFkeyBackColor(int n){
	unsigned short *VRAM=(unsigned short *)PictAry[0];
	return VRAM[ n*64+0 +192*384 ];
}

void Fkey_Line2( int n ){
	unsigned short *VRAM=(unsigned short *)PictAry[0];
	unsigned short c=GetFkeyColor(n);
	unsigned short bc=GetFkeyBackColor(n);
	int x,y;
	for(x=n*64+4;x<n*64+60;x++){
		VRAM[x+213*384]=c;
		VRAM[x+214*384]=c;
	}
	for(y=192;y<=215;y++){
		for(x=n*64+63;x<n*64+64;x++){
			VRAM[x+y*384]=bc;	// backcolor
		}
	}
}
void Fkey_Line3( int n ){
	unsigned short *VRAM=(unsigned short *)PictAry[0];
	unsigned short c=GetFkeyColor(n);
	unsigned short bc=GetFkeyBackColor(n);
	int x,y;
	for(x=n*64+4;x<n*64+55;x++){
		VRAM[x+213*384]=c;
		VRAM[x+214*384]=c;
	}
	for(y=208;y<=214;y++){
		for(x=n*64+63-y+208;x<n*64+63;x++){
			VRAM[x+y*384]=bc;	// backcolor
		}
	}
	for(y=192;y<=215;y++){
		for(x=n*64+63;x<n*64+64;x++){
			VRAM[x+y*384]=bc;	// backcolor
		}
	}
}
void Fkey_ClearBox( int n, int rev ){	// 
	unsigned short *VRAM=(unsigned short *)PictAry[0];
	unsigned short c;
	int x,y;
	if ( rev ) c=GetFkeyColor(n);	else c=GetFkeyBackColor(n);
	for(y=195;y<212;y++){
		for(x=n*64+20;x<n*64+40;x++){
			VRAM[x+y*384]=c;
		}
	}
}

void Fkey_Clear(int n) {
		unsigned int* iresult;	
		GetFKeyPtr(0x0, &iresult);	// 
		FKey_Display(n, iresult);
}

void Fkey_dispN(int n,char *buf) {
		unsigned int* iresult;	
		GetFKeyPtr(0x0476, &iresult);	//  .  		,white
		FKey_Display(n, iresult);
		Fkey_ClearBox( n, 0 );
		PrintMiniXY(n*64+3, 7*24+4, buf, GetFkeyColor(n), COLOR_WHITE, 0, 57 );
		Fkey_Line2( n );
}
/*
void Fkey_dispN2(int n,char *buf) {
		unsigned int* iresult;	
		GetFKeyPtr(0x0476, &iresult);	//  .  		,white
		FKey_Display(n, iresult);
		Fkey_ClearBox( n, 0 );
		PrintMiniXY(n*64+3, 7*24+4, buf, GetFkeyColor(n), COLOR_WHITE, 0, 57);
		Fkey_Line2( n );
}
*/
void Fkey_dispRR(int n,char *buf) {
		unsigned int* iresult;	
		GetFKeyPtr(0x0190, &iresult);	//  t		,black
		FKey_Display(n, iresult);
		Fkey_ClearBox( n, 1 );
		PrintMiniXY(n*64+3, 7*24+4, buf, COLOR_WHITE, GetFkeyColor(n), 0, 57 );
		Fkey_Line2( n );
}
void Fkey_dispRS(int n,char *buf ) {	// black select
		FKey_Display(n, (unsigned int*)fkey_select);
		PrintMiniXY(n*64+3, 7*24+4, buf, COLOR_WHITE, GetFkeyColor(n), 0, 57 );
}
void Fkey_dispR(int n,char *buf) {
		unsigned int* iresult;	
		GetFKeyPtr(0x0334, &iresult);	//  t		,black/
		FKey_Display(n, iresult);
		Fkey_ClearBox( n, 1 );
		PrintMiniXY(n*64+3, 7*24+4, buf, COLOR_WHITE, GetFkeyColor(n), 0, 57 );
		Fkey_Line3( n );
}
void Fkey_DISPN(int n,char *buf) {
		unsigned int* iresult;	
		GetFKeyPtr(0x0476, &iresult);	//  .  		,white
		FKey_Display(n, iresult);
		Fkey_ClearBox( n, 0 );
		PrintMiniXY(n*64+3, 7*24+4, buf, GetFkeyColor(n), COLOR_WHITE, 0, 57);
		Fkey_Line2( n );
}

void Fkey_DISPR(int n,char *buf) {
		unsigned int* iresult;	
		GetFKeyPtr(0x0190, &iresult);	//  t		,black
		FKey_Display(n, iresult);
		Fkey_ClearBox( n, 1 );
		PrintMiniXY(n*64+3, 7*24+4, buf, COLOR_WHITE, GetFkeyColor(n), 0, 57 );
		Fkey_Line2( n );
}



void Fkey_extend( int n, char *buf, int extend, int ofset, int colorflag ){
	unsigned short *VRAM=(unsigned short *)PictAry[0];
	int frontCol,backCol;
	unsigned short imagebuf[64*24];
	int i,j,x,y;
	if ( extend ) {
		for(y=0;y<24;y++){
			for(x=8;x<64;x++){
				imagebuf[x+y*64] = VRAM[(x+n*64)+(y+192)*384];
			}
		}
		for(i=0; i<extend; i++){
			for(y=0;y<24;y++){
				for(j=1; j<=2; j++) {
					for(x=8;x<64;x++){
						VRAM[(x+(n+i)*64+j*32)+(y+192)*384] = imagebuf[x+y*64];
					}
				}
			}
		}
	}
	
	if ( colorflag ) {
		frontCol=GetFkeyColor(n); 
		backCol =GetFkeyBackColor(n);
	} else {
		frontCol=GetFkeyBackColor(n);
		backCol =GetFkeyColor(n); 
	}
	if (ofset & 0xF0) x=1; else x=3;
	y= ofset & 0xF;
	PrintMiniXY(n*64+x, 7*24+4-y, buf, frontCol, backCol, 1, 63 -x*2 + 64*extend );	// ext char
}

void Fkey_dispN_ext(int n,char *buf, int ofset, int extend, int color, int backcolor ) {
		unsigned int* iresult;	
		GetFKeyPtr(0x0476, &iresult);	//  .  		,white
		FKey_Display_color(n, iresult, color, backcolor);
		Fkey_ClearBox( n, 0 );
		Fkey_extend( n, buf, extend, ofset, 1);
		Fkey_Line2( n+extend );
}
void Fkey_dispR_ext(int n,char *buf, int ofset, int extend, int color, int backcolor ) {
		unsigned int* iresult;	
		GetFKeyPtr(0x0334, &iresult);	//  t		,black/
		FKey_Display_color(n, iresult, color, backcolor);
		Fkey_ClearBox( n, 1 );
		Fkey_extend( n, buf, extend, ofset, 0);
		Fkey_Line3( n+extend );
}

void Fkey_dispRR_ext(int n,char *buf, int ofset, int extend, int color, int backcolor ) {
		unsigned int* iresult;	
		GetFKeyPtr(0x0190, &iresult);	//  t		,black
		FKey_Display_color(n, iresult, color, backcolor);
		Fkey_ClearBox( n, 1 );
		Fkey_extend( n, buf, extend, ofset, 0);
		Fkey_Line2( n+extend );
}
void Fkey_dispRS_ext(int n,char *buf, int ofset, int extend, int color, int backcolor ) {	// black select
		FKey_Display_color(n, (unsigned int*)fkey_select, color, backcolor);
		Fkey_extend( n, buf, extend, ofset, 0);
}

void Fkey_dispNInit(int n) {
		unsigned int* iresult;
		GetFKeyPtr(0x005f, &iresult);	//INITIAL 	,white
		FKey_Display(3, iresult);
}		

void FkeyMaskColor(int n, int backcolor ) {
	int tcolor=CB_TransparentColorIndex;
	CB_TransparentColorIndex = 1;
	ML_rectangle_CG(n*64, 168, (n+1)*64-1, 191, 0, ML_TRANSPARENT, ML_CHECKER, 1, backcolor  );
	CB_TransparentColorIndex = tcolor;
}
void FkeyMask(int n) {
	FkeyMaskColor( n, COLOR_WHITE );
}


void FkeyClear(int n) {
	Fkey_Icon(n, 0 );	// clear
}
void FkeyClearAll(){
	int n;
	for (n=0; n<6; n++ ) FkeyClear( n );
}

void Fkey_dspRB(int n,char *buffer) {
	Fkey_DISPR( n, buffer); 
}

void Fkey_AreaReverseVRAM( int n, int x1, int x2 ){
	unsigned short *VRAM=(unsigned short *)PictAry[0];
	unsigned short c =GetFkeyColor(n);
	unsigned short bc=GetFkeyBackColor(n);
	int x,y,d;
	for(y=192+3; y<=192+20; y++){
		for(x=n*64+x1; x<=n*64+x2; x++){
			d=VRAM[x+y*384];
			if ( d==c ) VRAM[x+y*384]=bc;
			if ( d==bc) VRAM[x+y*384]=c;
		}
	}
}
void Fkey_dispN_Aa(int n, char *buffer) {
	Fkey_dispN(n,buffer);
//	Bdisp_AreaReverseVRAMx3( n*21+3, (7+0)*8+1, n*21+7, (7+0)*8+6);	// reverse
//	Bdisp_AreaReverseVRAM( n*64+3, 192+3, n*64+20, 192+20 );	// reverse
	Fkey_AreaReverseVRAM( n, 3, 19 );
}
void Fkey_dispN_aA(int n, char *buffer) {
	Fkey_dispN(n,buffer);
//	Bdisp_AreaReverseVRAMx3( n*21+16, (7+0)*8+1, n*21+20, (7+0)*8+6);	// reverse
//	Bdisp_AreaReverseVRAM( n*64+46, 192+3, n*64+60, 192+20 );	// reverse
	Fkey_AreaReverseVRAM( n, 45, 60 );
}
