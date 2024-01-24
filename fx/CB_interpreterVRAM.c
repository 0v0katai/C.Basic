/*
===============================================================================

 Casio Basic interpreter for fx-9860G series    v0.99

 copyright(c)2015 by sentaro21
 e-mail sentaro21@pm.matrix.jp

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

//----------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------
//int GObjectAlign4a( unsigned int n ){ return n; }	// align +4byte
//int GObjectAlign4b( unsigned int n ){ return n; }	// align +4byte
//int GObjectAlign4c( unsigned int n ){ return n; }	// align +4byte
//----------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------
// unsigned char GVRAM[1024];

void SaveVRAM( char* vram){
	Bdisp_GetDisp_VRAM( (unsigned char*)vram );
}
void RestoreVRAM( char* vram){
	DISPGRAPH Gpict;
	Gpict.x =   0; 
	Gpict.y =   0; 
	Gpict.GraphData.width   = 128;
	Gpict.GraphData.height  = 64;
	Gpict.GraphData.pBitmap = (unsigned char*)vram;
	Gpict.WriteModify = IMB_WRITEMODIFY_NORMAL; 
	Gpict.WriteKind   = IMB_WRITEKIND_OVER;
	Bdisp_WriteGraph_VRAM(&Gpict);
}
//----------------------------------------------------------------------------------------------
void CB_SaveTextVRAM() {
	SaveVRAM(TVRAM);		// ------ SaveDisp2 Text screen
//	SaveDisp(SAVEDISP_PAGE2);		// ------ SaveDisp2 Text screen
}	
void CB_RestoreTextVRAM() {
	RestoreVRAM(TVRAM);		// ------ RestoreDisp2 Text screen
//	RestoreDisp(SAVEDISP_PAGE2);		// ------ RestoreDisp2 Text screen
}	
void CB_SelectTextVRAM() {
	if ( ScreenMode == 0 ) return;
	SaveVRAM(GVRAM);		// ------ Save Graphic screen 
//	SaveDisp(SAVEDISP_PAGE3);		// ------ SaveDisp3 Graphic screen (NG: damage CATALOG key )
	RestoreVRAM(TVRAM);		// ------ RestoreDisp2 Text screen
//	RestoreDisp(SAVEDISP_PAGE2);		// ------ RestoreDisp2 Text screen
	ScreenMode=0;	// Text mode
}
void CB_SelectTextDD() {
	CB_SelectTextVRAM();
	Bdisp_PutDisp_DD();
}
void CB_SaveGraphVRAM() {
	SaveVRAM(GVRAM);		// ------ Save Graphic screen 
}	
void CB_RestoreGraphVRAM() {
	RestoreVRAM(GVRAM);		// ------ Restore Graphic screen
}	
void CB_SelectGraphVRAM() {
	if ( ScreenMode == 1 ) return;
	SaveVRAM(TVRAM);		// ------ SaveDisp2 Text screen
//	SaveDisp(SAVEDISP_PAGE2);		// ------ SaveDisp2 Text screen
	RestoreVRAM(GVRAM);		// ------ Restore Graphic screen
//	RestoreDisp(SAVEDISP_PAGE2);		// ------ RestoreDisp3 Graphic screen (NG: damage CATALOG key )
	ScreenMode=1;	// Graphic mode
}
//void CB_SelectGraphDD() {
//	CB_SelectGraphVRAM();
//	Bdisp_PutDisp_DD();
//}

void CB_RefreshCtrl( char *SRC ){	// PutDispDD Refresh control
	int c,n;
	switch ( CB_EvalInt( SRC ) ) {
		case 0:
			RefreshCtrl=0;	// off
			break;
		case 1:
			RefreshCtrl=1;	// graphics only
			break;
		case 2:
			RefreshCtrl=2;	// all
			break;
		default:
			CB_Error(ArgumentERR);	// Argument error
	}
}
void CB_RefreshTime( char *SRC ){	// PutDispDD Refresh time
	int c,n;
	n=CB_EvalInt( SRC );
	if ( ( 1<=n ) && ( n<=128 ) ) Refreshtime=n-1; 
	else 
	{ CB_Error(ArgumentERR); return; }	// Argument error
}

void CB_Screen( char *SRC ){
	int c;
	c=SRC[ExecPtr++];
	if ( c == '.' ) { c=SRC[ExecPtr++];
		if ( ( c=='G' ) || ( c=='g' ) ) { CB_SelectGraphVRAM(); return; }	// Select Graphic Screen
		if ( ( c=='T' ) || ( c=='t' ) ) { CB_SelectTextVRAM();  return; }	// Select Text Screen
		{ ExecPtr--; CB_Error(SyntaxERR); return; }	// Syntax error
	} else 
	if ( ( c==0 ) || ( c==0x0D ) || ( c==0x0C ) || ( c==':' ) ) {
			if ( ScreenMode == 0 )  { CB_SelectGraphVRAM(); return; }	// Select Graphic Screen
			else					{ CB_SelectTextVRAM();  return; }	// Select Text Screen

	} else { ExecPtr--;
		switch ( CB_EvalInt( SRC ) ) {
			case 0:
				CB_SelectTextVRAM();	// Select Text Screen
				break;
			case 1:
				CB_SelectGraphVRAM();	// Select Graphic Screen
				break;
		}
	}
}

void Scrl_Y(){
	unsigned char pDATA[1024];
	DISPGRAPH Gscrl;
	DISPBOX sBOX;
	
	CursorY++;
	if ( CursorY > 7 ) {		// scroll
		sBOX.left  =0;
		sBOX.top   =8;
		sBOX.right =127;
		sBOX.bottom=63;
		Bdisp_ReadArea_VRAM(&sBOX,pDATA);

	    Gscrl.x =   0; 
		Gscrl.y =   0; 
		Gscrl.GraphData.width   = 128;
		Gscrl.GraphData.height  = 56;
		Gscrl.GraphData.pBitmap = pDATA;
		Gscrl.WriteModify = IMB_WRITEMODIFY_NORMAL; 
		Gscrl.WriteKind   = IMB_WRITEKIND_OVER;
		Bdisp_WriteGraph_VRAM(&Gscrl);
		
		sBOX.left  =0;
		sBOX.top   =56;
		sBOX.right =127;
		sBOX.bottom=63;
		Bdisp_AreaClr_VRAM(&sBOX);
		CursorY=7;
	}
	CursorX=1;
}
/*
int CB_GotoEndPtr( char *SRC ) {		// goto Program End Ptr
	int size;
	char *src;
	src=SRC-0x56;
	size=(src[0x47]&0xFF)*256+(src[0x48]&0xFF)+0x4C;
	return ( size -0x56 - 1 ) ;
}
*/

//----------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------

void CB_Cls( char *SRC ){
	CB_SelectGraphVRAM();	// Select Graphic Screen
	ViewWindow( Xmin, Xmax, Xscl, Ymin, Ymax, Yscl);
//	Bdisp_AllClr_VRAM();
//	GraphAxesGrid();
	Previous_PX=-1;   Previous_PY=-1; 		// ViewWindow Previous PXY init
	CB_SelectTextVRAM();	// Select Text Screen
//	Bdisp_PutDisp_DD_DrawBusy_through(SRC);
}
void CB_ClrText( char *SRC ){
	CB_SelectTextVRAM();	// Select Text Screen
	CursorX=1;
	CursorY=1;
	Bdisp_AllClr_VRAM();
	Bdisp_PutDisp_DD_DrawBusy_skip_through_text(SRC);
}
void CB_ClrGraph( char *SRC ){
	CB_SelectGraphVRAM();	// Select Graphic Screen
	SetVeiwWindowInit();
	CB_Cls(SRC);
}
//----------------------------------------------------------------------------------------------
int CB_Fix( char *SRC ){
	int tmp;
	tmp=CB_EvalInt( SRC );
	if ( tmp < 0 ) if ( CB_Round.MODE == Fix ) return CB_Round.DIGIT; else return -1;
	if ( ( tmp < 0 ) || ( tmp > 15 ) ) { CB_Error(SyntaxERR); return; }	// Syntax error
	CB_Round.MODE = Fix ;
	CB_Round.DIGIT= tmp ;
	return tmp ;
}
int CB_Sci( char *SRC ){
	int tmp;
	tmp=CB_EvalInt( SRC );
	if ( tmp < 0 ) if ( CB_Round.MODE == Sci ) return CB_Round.DIGIT; else return -1;
	if ( ( tmp < 0 ) || ( tmp > 15 ) ) { CB_Error(SyntaxERR); return; }	// Syntax error
	CB_Round.MODE = Sci ;
	CB_Round.DIGIT= tmp ;
	return tmp ;
}
int CB_Norm( char *SRC ){
	int tmp;
	tmp=CB_EvalInt( SRC );
	if ( tmp < 0 ) if ( CB_Round.MODE == Norm ) return CB_Round.DIGIT; else return -1;
	if ( ( tmp < 0 ) || ( tmp > 15 ) ) { CB_Error(SyntaxERR); return; }	// Syntax error
	CB_Round.MODE = Norm ;
	CB_Round.DIGIT= tmp ;
	return tmp ;
}
void CB_Rnd(){
	CB_CurrentValue = Round( CB_CurrentValue, CB_Round.MODE, CB_Round.DIGIT );
}

//----------------------------------------------------------------------------------------------
void PrintDone() {
	if ( CursorX >1 ) Scrl_Y();
	locate( CursorX, CursorY); Print((unsigned char*)"                 Done");
	CursorX=21;
}
void CB_Done(){
	if ( ScreenMode == 1 ) {	// Graphic mode
		CB_SelectTextVRAM();	// Select Text Screen
		PrintDone();
		CB_SelectGraphVRAM();	// Select Graphic Screen
	} else {
		PrintDone();
	}
}

void PlotXYtoPrevPXY() {
//		Previous_X=Plot_X;
//		Previous_Y=Plot_Y;
		VWtoPXY( Plot_X, Plot_Y, &Previous_PX, &Previous_PY );
}
void PlotPreviousPXY() {
	if ( Previous_PX > 0 ) LinesubSetPoint(Previous_PX, Previous_PY,1);
	PlotXYtoPrevPXY();
}
void PlotCurrentXY(){
	PlotXYtoPrevPXY();
	if ( ScreenMode == 0 ) {	//Text mode
		CB_SelectGraphVRAM();	// Select Graphic Screen
		PlotPreviousPXY();
		CB_SelectTextVRAM();	// Select Text Screen
	} else {
		PlotPreviousPXY();
	}
}


//----------------------------------------------------------------------------------------------
int CB_LocateMode( char *SRC) {
	int c=SRC[ExecPtr];
	if ( c != ',' ) return 0;	// normal
	ExecPtr++;	
	c=SRC[ExecPtr++];
	if ( c == 'N' ) return 0;	// normal
	else
	if ( c == 'R' ) return 1;	// reverse
	ExecPtr--; CB_Error(SyntaxERR); return;   // Syntax error
}

void CB_Locate( char *SRC ){
	char buffer[CB_StrBufferMax];
	int c;
	int lx,ly;
	double value;
	int mode;
	int maxoplen;
	
	CB_SelectTextVRAM();	// Select Text Screen
	lx = CB_EvalInt( SRC );
	if ( ( lx < 1 ) || ( lx > 21 ) ) { CB_Error(ArgumentERR); return; }	// Argument error
	c=SRC[ExecPtr];
	if ( c != ',' ) { CB_Error(SyntaxERR); return; }	// Syntax error
	ExecPtr++;
	ly = CB_EvalInt( SRC );
	if ( ( ly < 1 ) || ( ly > 7 ) ) { CB_Error(ArgumentERR); return; }	// Argument error
	c=SRC[ExecPtr];
	if ( c != ',' ) { CB_Error(SyntaxERR); return; }	// Syntax error
	ExecPtr++;
	
	c=CB_IsStr( SRC, ExecPtr );
	if ( c ) {	// string
		CB_GetLocateStr( SRC, buffer, CB_StrBufferMax-1 );		// String -> buffer	return 
	} else {	// expression
		value = CB_EvalDbl( SRC );
		sprintGR(buffer, value, 22-lx,LEFT_ALIGN, CB_Round.MODE, CB_Round.DIGIT);
	}
	if ( CB_LocateMode(SRC) ) CB_PrintRev( lx,ly, (unsigned char*)buffer );
	else 		   			  CB_Print( lx,ly, (unsigned char*)buffer );
	Bdisp_PutDisp_DD_DrawBusy_skip_through_text(SRC);
}
//-----------------------------------------------------------------------------
int RangeErrorCK( char *SRC ) {
	if ( UseGraphic == 0 ) {
		if ( ScreenMode == 0 ) CB_SelectGraphVRAM();	// Select Graphic Screen
		Bdisp_AllClr_VRAM();
		GraphAxesGrid();
	}
	if ( ( Xdot == 0 ) || ( Ydot == 0 )  ) { CB_Error(RangeERR); PrevOpcode( SRC, &ExecPtr ); return ErrorNo; }	// Range error
	return 0;
}
void CB_TextOprand( char *SRC, int *py, int *px) {
	int x,y;
	*py=CB_EvalInt( SRC );
	if ( ( (*py)<0 ) || ( (*py)>63 ) ) { CB_Error(ArgumentERR); return; }  // Argument error
	if ( SRC[ExecPtr] != ',' ) { CB_Error(SyntaxERR); return; }  // Syntax error
	ExecPtr++;
	*px=CB_EvalInt( SRC );
	if ( ( (*px)<0 ) || ( (*px)>127 ) ) { CB_Error(ArgumentERR); return; }  // Argument error}
}
void CB_Text( char *SRC ) { //	Text
	unsigned int key;
	char buffer[CB_StrBufferMax];
	int c;
	int px,py,d;
	double value;
	int mode;
	int maxoplen;
	int kanamini=1;

	if ( RangeErrorCK(SRC) ) return;
	CB_SelectGraphVRAM();	// Select Graphic Screen
	CB_TextOprand( SRC, &py, &px);
	c=SRC[ExecPtr];
	if ( c != ',' ) { CB_Error(SyntaxERR); return; }	// Syntax error
	c=SRC[++ExecPtr];
	if ( ( c == 0xFFFFFF87 )||( c == 0xFFFFFF99 ) ) { ExecPtr++; kanamini=0; }
	c=CB_IsStr( SRC, ExecPtr );
	if ( c ) {	// string
		CB_GetLocateStr( SRC, buffer, CB_StrBufferMax-1 );		// String -> buffer	return 
	} else {	// expression
		d=(128-px)/4;
		if (d>24) d=24;	// digit max
		value = CB_EvalDbl( SRC );
		sprintGR(buffer, value, d,LEFT_ALIGN, CB_Round.MODE, CB_Round.DIGIT);
	}
	c=SRC[ExecPtr];
	if ( c != ',' )	mode=MINI_OVER;
	else {
		ExecPtr++;	
		c=SRC[ExecPtr++];
		if ( c == 'N' ) mode=MINI_OVER;	// Normal
		else
		if ( c == 'R' ) mode=MINI_REV;	// Reverse
		else
		if ( c == 'O' ) mode=MINI_OR;	// Or
		else
		if ( c == 'V' ) mode=MINI_REVOR;	// reVerse or
		else { ExecPtr--; CB_Error(SyntaxERR); return; }  // Syntax error
	}
	if ( kanamini ) CB_PrintMini( px, py, (unsigned char*)buffer, mode);
		else 	       PrintMini( px, py, (unsigned char*)buffer, mode);
	Bdisp_PutDisp_DD_DrawBusy_skip_through_text(SRC);
}
//-----------------------------------------------------------------------------
void CB_LocateYX( char *SRC ){
	char buffer[CB_StrBufferMax];
	int c;
	int px,py,d;
	double value;
	int mode;
	int maxoplen;

	if ( RangeErrorCK(SRC) ) return;
	CB_SelectGraphVRAM();	// Select Graphic Screen
	CB_TextOprand( SRC, &py, &px);
	c=SRC[ExecPtr];
	if ( c != ',' ) { CB_Error(SyntaxERR); return; }	// Syntax error
	ExecPtr++;
	c=CB_IsStr( SRC, ExecPtr );
	if ( c ) {	// string
		CB_GetLocateStr( SRC, buffer, CB_StrBufferMax-1 );		// String -> buffer	return 
	} else {	// expression
		d=(128-px)/6;
		if (d>21) d=21;	// digit max
		value = CB_EvalDbl( SRC );
		sprintGR(buffer, value, d,LEFT_ALIGN, CB_Round.MODE, CB_Round.DIGIT);
	}
	CB_PrintXY(  px,py, (unsigned char*)buffer, CB_LocateMode(SRC));	
	Bdisp_PutDisp_DD_DrawBusy_skip_through_text(SRC);

}


//----------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------

void CB_ViewWindow( char *SRC ) { //	ViewWindow
	int c;
	int reg=0;
	while ( reg <= 10 ) {
		c=SRC[ExecPtr];
		if ( (c==':') || (c==0x0C) || (c==0x0D) || (c==0x00) ) break;
		REGv[reg]=CB_EvalDbl( SRC );
		c=SRC[ExecPtr];
		if ( (c==':') || (c==0x0C) || (c==0x0D) || (c==0x00) ) break;
		if ( c != ',' ) { CB_Error(SyntaxERR); return; }  // Syntax error
		ExecPtr++;
		reg++;
	}
	CB_SelectGraphVRAM();	// Select Graphic Screen
	ViewWindow( Xmin, Xmax, Xscl, Ymin, Ymax, Yscl);
	CB_SelectTextVRAM();	// Select Text Screen
//	Bdisp_PutDisp_DD_DrawBusy_skip();
}

int CB_SetPointMode( char *SRC) {
	int c=SRC[ExecPtr];
	if ( c != ',' ) return 1;
	ExecPtr++;	
	c=SRC[ExecPtr++];
	if ( c == 'C' ) return 0;
	else
	if ( c == 'X' ) return 2;
	else
	if ( c == 'P' ) return 1;
	ExecPtr--; CB_Error(SyntaxERR); return;   // Syntax error
}

void CB_FLine( char *SRC) { //	F-Line
	int c;
	double x1,y1,x2,y2;
	int style=S_L_Style;

	if ( RangeErrorCK(SRC) ) return;
	x1=CB_EvalDbl( SRC );
	c=SRC[ExecPtr];
	if ( c != ',' ) { CB_Error(SyntaxERR); return; }  // Syntax error
	ExecPtr++;
	y1=CB_EvalDbl( SRC );
	c=SRC[ExecPtr];
	if ( c != ',' ) { CB_Error(SyntaxERR); return; }  // Syntax error
	ExecPtr++;
	x2=CB_EvalDbl( SRC );
	c=SRC[ExecPtr];
	if ( c != ',' ) { CB_Error(SyntaxERR); return; }  // Syntax error
	ExecPtr++;
	y2=CB_EvalDbl( SRC );

	if ( tmp_Style >= 0 ) style=tmp_Style;
	CB_SelectGraphVRAM();	// Select Graphic Screen
	F_Line(x1, y1, x2, y2, style, CB_SetPointMode(SRC) );
	Bdisp_PutDisp_DD_DrawBusy_skip_through(SRC);
	tmp_Style = -1;
}

void CB_Line( char *SRC ) { //	Line
	int style=S_L_Style;
	if ( tmp_Style >= 0 ) style=tmp_Style;
	if ( RangeErrorCK(SRC) ) return;
	CB_SelectGraphVRAM();	// Select Graphic Screen
	Line( style, CB_SetPointMode(SRC) , 1 );	// error check
	Bdisp_PutDisp_DD_DrawBusy_skip_through(SRC);
	tmp_Style = -1;
}

void CB_Vertical( char *SRC ) { //	Vertical
	double x;
	int style=S_L_Style;
	if ( tmp_Style >= 0 ) style=tmp_Style;
	if ( RangeErrorCK(SRC) ) return;
	x=CB_EvalDbl( SRC );
	CB_SelectGraphVRAM();	// Select Graphic Screen
	Vertical(x, style, CB_SetPointMode(SRC) );
	Bdisp_PutDisp_DD_DrawBusy_skip_through(SRC);
	tmp_Style = -1;
}
void CB_Horizontal( char *SRC ) { //	Horizontal
	double y;
	int style=S_L_Style;
	if ( tmp_Style >= 0 ) style=tmp_Style;
	if ( RangeErrorCK(SRC) ) return;
	y=CB_EvalDbl( SRC );
	CB_SelectGraphVRAM();	// Select Graphic Screen
	Horizontal(y, style, CB_SetPointMode(SRC) );
	Bdisp_PutDisp_DD_DrawBusy_skip_through(SRC);
	tmp_Style = -1;
}
//----------------------------------------------------------------------------------------------
void CB_PxlSub( char *SRC, int mode ) { //	mode  1:PxlOn  0:PxlOff  2:PxlChg
	double x,y;
	int px,py;
	if ( RangeErrorCK(SRC) ) return;
	y = (EvalsubTop( SRC ));
	py=y;
	if ( ( y-floor(y) ) || ( (py)<MatBase ) || ( (py)>63 ) ) { CB_Error(ArgumentERR); return; }  // Argument error
	if ( SRC[ExecPtr] != ',' ) { CB_Error(SyntaxERR); return; }  // Syntax error
	ExecPtr++;
	x = (EvalsubTop( SRC ));
	px=x;
	if ( ( x-floor(x) ) || ( (px)<MatBase ) || ( (px)>127 ) ) { CB_Error(ArgumentERR); return; }  // Argument error}
	CB_SelectGraphVRAM();	// Select Graphic Screen

	BdispSetPointVRAM2(px, py, mode);
	PXYtoVW(px, py, &regX, &regY);
	Bdisp_PutDisp_DD_DrawBusy_skip_through(SRC);
}
//----------------------------------------------------------------------------------------------
//int ObjectAlign4p( unsigned int n ){ return n; }	// align +4byte
//----------------------------------------------------------------------------------------------

void CB_Plot( char *SRC ) { //	Plot
	int c;
	double x,y;
	
	if ( RangeErrorCK(SRC) ) return;
	
	CB_SelectGraphVRAM();	// Select Graphic Screen
	c=SRC[ExecPtr];
	if ( ( c==':' ) || (c==0x0D) || ( c==0x0C ) || (c==0x00) ) {
		x=(Xmax+Xmin)/2;
		y=(Ymax+Ymin)/2;
	} else {
		x=CB_EvalDbl( SRC );
		c=SRC[ExecPtr];
		if ( c != ',' ) { CB_Error(SyntaxERR); return; }  // Syntax error
		ExecPtr++;
		y=CB_EvalDbl( SRC );
	}
	Plot_X = x;
	Plot_Y = y;
	regX = x;
	regY = y;
	regintX = x;
	regintY = y;
	PlotPreviousPXY();
	Bdisp_PutDisp_DD_DrawBusy_skip_through(SRC);
}

void CB_PlotSub( char *SRC, int mode ) { //	mode  1:PlotOn  0:PlotOff  2:PlotChg
	double x,y;
	if ( RangeErrorCK(SRC) ) return;
	x=CB_EvalDbl( SRC );
	if ( SRC[ExecPtr] != ',' ) { CB_Error(SyntaxERR); return; }  // Syntax error
	ExecPtr++;
	y=CB_EvalDbl( SRC );
	CB_SelectGraphVRAM();	// Select Graphic Screen
	PlotSub(x,y,mode);
	Bdisp_PutDisp_DD_DrawBusy_skip_through(SRC);
}

//----------------------------------------------------------------------------------------------

void CB_Circle( char *SRC ) { //	Circle
	int c,mode;
	double x,y,r;
	int style=S_L_Style;
	if ( tmp_Style >= 0 ) style=tmp_Style;
	if ( RangeErrorCK(SRC) ) return;
	CB_SelectGraphVRAM();	// Select Graphic Screen
	x=CB_EvalDbl( SRC );
	c=SRC[ExecPtr];
	if ( c != ',' ) { CB_Error(SyntaxERR); return; }  // Syntax error
	ExecPtr++;
	y=CB_EvalDbl( SRC );
	c=SRC[ExecPtr];
	if ( c != ',' ) { CB_Error(SyntaxERR); return; }  // Syntax error
	ExecPtr++;
	r=CB_EvalDbl( SRC );
	mode=CB_SetPointMode(SRC) ;
	c=SRC[ExecPtr];
	if ( c==':' ) 	Circle(x, y, r, style, 0, mode);
		else 		Circle(x, y, r, style, 1, mode);
	Bdisp_PutDisp_DD_DrawBusy_skip_through(SRC);
	tmp_Style = -1;
}

//----------------------------------------------------------------------------------------------
void CB_DrawGraph(  char *SRC ){
	if ( RangeErrorCK(SRC) ) return;
	CB_SelectGraphVRAM();	// Select Graphic Screen
	Graph_Draw();
}

void CB_GraphY( char *SRC ){
	if ( RangeErrorCK(SRC) ) return;
	CB_Str(SRC);
	GraphY=CB_CurrentStr;
	CB_SelectGraphVRAM();	// Select Graphic Screen
	Graph_Draw();
}

//----------------------------------------------------------------------------------------------
void ReadVram( unsigned char *pDATA ){
	Bdisp_GetDisp_VRAM( pDATA ) ;
}
void WriteVram( unsigned char *pDATA ){
	DISPGRAPH Gpict;
	
	Gpict.x =   0; 
	Gpict.y =   0; 
	Gpict.GraphData.width   = 128;
	Gpict.GraphData.height  = 64;
	Gpict.GraphData.pBitmap = pDATA;
	Gpict.WriteModify = IMB_WRITEMODIFY_NORMAL; 
	Gpict.WriteKind   = IMB_WRITEKIND_OR;
	Bdisp_WriteGraph_VRAM(&Gpict);
}

int StoPict( int pictNo){
	unsigned char pict[2048+0x4C+4];
	int i,stat;
	ReadVram(pict+0x4C);
	for(i=1024+0x4c; i<2048+0x4c+2; i++) pict[i]=0;
	stat=SavePicture( (char *)pict, pictNo );
	if ( stat != 0 ) { CB_Error(MemoryERR); return; }	// Memory error
	return stat;
}

void RclPict( int pictNo){
	unsigned char *pict;
	int i;
	pict=(unsigned char *)LoadPicture( pictNo );
	if ( pict == NULL ) { CB_Error(MemoryERR); return; }	// Memory error
	WriteVram( pict+0x4C );
	free(pict);
}

void StoPictM( int pictNo){
	int i,stat;
	unsigned char *pict;
	
	if ( PictAry[pictNo] != NULL ) { // already exist
		pict = PictAry[pictNo] ;						// Matrix array ptr*
	} else {
		pict = (unsigned char *) malloc( 1024 );
		if( pict == NULL ) { CB_Error(NotEnoughMemoryERR); return; }	// Not enough memory error
		PictAry[pictNo] = pict ;						// Pict array ptr*
	}
	ReadVram(pict);
}

void RclPictM( int pictNo){
	unsigned char *pict;
	int i;
	if ( PictAry[pictNo] == NULL ) { CB_Error(MemoryERR); return; }	// Memory error
	pict=PictAry[pictNo];
	WriteVram( pict );
}

void CB_StoPict( char *SRC ) { //	StoPict
	int n;
	CB_SelectGraphVRAM();	// Select Graphic Screen
	n=CB_EvalInt( SRC );
	if ( (n<1) || (20<n) ){ CB_Error(ArgumentERR); return; }	// Argument error
	if ( PictMode ) StoPictM(n); else StoPict(n);
	Bdisp_PutDisp_DD_DrawBusy_skip_through(SRC);
}
void CB_RclPict( char *SRC ) { //	RclPict
	int n;
	CB_SelectGraphVRAM();	// Select Graphic Screen
	n=CB_EvalInt( SRC );
	if ( (n<1) || (20<n) ){ CB_Error(ArgumentERR); return; }	// Argument error
	if ( PictMode ) RclPictM(n); else RclPict(n);
	Bdisp_PutDisp_DD_DrawBusy_skip_through(SRC);
}


//----------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------
int CB_DotOprandRect( char *SRC, int *px, int *py) {
	int x,y;
	*px=CB_EvalInt( SRC );
	if ( SRC[ExecPtr] != ',' ) { CB_Error(SyntaxERR); return; }  // Syntax error
	ExecPtr++;
	*py=CB_EvalInt( SRC );
}
//--------------------------------------------------------------------
void CB_RectSub( char *SRC , int RectMode ) { // RectMode  0:Rect  1:RectFill
	int c;
	int px1,py1,px2,py2;
	int x,y;
	int mode;
	int style=S_L_Style;
	if ( tmp_Style >= 0 ) style=tmp_Style;
	if ( RangeErrorCK(SRC) ) return;
	CB_SelectGraphVRAM();	// Select Graphic Screen
	CB_DotOprandRect( SRC, &px1, &py1);
	if ( SRC[ExecPtr] != ',' ) { CB_Error(SyntaxERR); return; }  // Syntax error
	ExecPtr++;
	CB_DotOprandRect( SRC, &px2, &py2);
	mode=CB_SetPointMode(SRC) ;
	if ( ErrorNo ) return ; // error

	if ( px1 > px2 ) { c=px1; px1=px2; px2=c; }
	if ( py1 > py2 ) { c=py1; py1=py2; py2=c; }
	if ( ( px2 < 0 ) || ( px1 > 127 ) || ( py2 < 0 ) || ( py1 > 63 ) ) return ; 	// out of area
	if ( px1 < 0 ) px1=0;
	if ( py1 < 0 ) py1=0;
	if ( px2 > 127 ) px2=127;
	if ( py2 >  63 ) py2= 63;

	if ( RectMode ) {
		for ( y=py1 ; y<=py2 ; y++)	Linesub(px1, y, px2, y, style, mode) ;	// FillRect
	} else {
		Linesub(px1, py1, px2, py1, style, mode) ;	// upper line		 Rect
		if ( py1 != py2 ) {
			Linesub(px1, py2, px2, py2, style, mode) ;	// lower line
			if ( py2-py1 >= 2 ) {
				Linesub(px1, py1+1, px1, py2-1, style, mode) ;	// left 
				Linesub(px2, py1+1, px2, py2-1, style, mode) ;	// right
			}
		}
	}
	Bdisp_PutDisp_DD_DrawBusy_skip_through(SRC);
}

void CB_Rect( char *SRC ) { // Rect x1,y1,x2,y2,mode 
	CB_RectSub( SRC , 0 );	// Rect
}

void CB_FillRect( char *SRC ) { // FillRect x1,y1,x2,y2,mode 
	CB_RectSub( SRC , 1 );	// Rect
}

void CB_DotShape( char *SRC ) { // DotShape (x1,y1,x2,y2,typ,mode,pattern1,pattern2)
	TShape Shape;
	int c;
	int px1,py1,px2,py2;
	int typ;
	int mode,mode1,mode2;
	int pat1,pat2;
	
	if ( RangeErrorCK(SRC) ) return;
	CB_SelectGraphVRAM();	// Select Graphic Screen
	CB_DotOprandRect( SRC, &px1, &py1);
	if ( SRC[ExecPtr] != ',' ) { CB_Error(SyntaxERR); return; }  // Syntax error
	ExecPtr++;
	CB_DotOprandRect( SRC, &px2, &py2);
	if ( SRC[ExecPtr] != ',' ) { CB_Error(SyntaxERR); return; }  // Syntax error
	ExecPtr++;
	typ=CB_EvalInt( SRC );
	if ( SRC[ExecPtr] != ',' ) { CB_Error(SyntaxERR); return; }  // Syntax error
	ExecPtr++;
	mode1=CB_EvalInt( SRC );
	if ( SRC[ExecPtr] != ',' ) { CB_Error(SyntaxERR); return; }  // Syntax error
	ExecPtr++;
	mode2=CB_EvalInt( SRC );
	if ( SRC[ExecPtr] != ',' ) { CB_Error(SyntaxERR); return; }  // Syntax error
	ExecPtr++;
	pat1=CB_EvalInt( SRC );
	if ( SRC[ExecPtr] != ',' ) { CB_Error(SyntaxERR); return; }  // Syntax error
	ExecPtr++;
	pat2=CB_EvalInt( SRC );
	if ( SRC[ExecPtr] == ')' ) ExecPtr++;
/*
	if ( mode == 0 ) { mode1=2; mode2=1; }	// clear
	else
	if ( mode == 1 ) { mode1=1; mode2=1; }	// set
	else
	if ( mode == 2 ) { mode1=1; mode2=4; }	// invert
*/
	Shape.x1=px1;
	Shape.y1=py1;
	Shape.x2=px2;
	Shape.y2=py2;
	Shape.f[0]=2;
	Shape.f[1]=typ;
	Shape.f[2]=mode1;
	Shape.f[3]=mode2;
    Shape.on_bits=pat1;
    Shape.off_bits=pat2;
		
	Bdisp_ShapeToVRAM( &Shape );
	Bdisp_PutDisp_DD_DrawBusy_skip_through(SRC);
}


//----------------------------------------------------------------------------------------------
void CB_DotOprand( char *SRC, int *px, int *py) {
	int x,y;
	*px=CB_EvalInt( SRC );
	if ( ( (*px)<0 ) || ( (*px)>127 ) ) { CB_Error(ArgumentERR); }  // Argument error}
	if ( SRC[ExecPtr] != ',' ) { CB_Error(SyntaxERR); return; }  // Syntax error
	ExecPtr++;
	*py=CB_EvalInt( SRC );
	if ( ( (*py)<0 ) || ( (*py)>63 ) ) { CB_Error(ArgumentERR); }  // Argument error
}
//--------------------------------------------------------------------

void CB_ReadGraph( char *SRC ){	// ReadGraph(px1,py1, px2,py2)->Mat C
	int c;
	int dx,dy,sx;
	int px1,py1,px2,py2;
	int reg,i;
	int dimA,dimB;
	unsigned char *ptr;
	int ElementSize;
	int scrmode=ScreenMode;
	int base;
	DISPBOX box;
	
	if ( RangeErrorCK(SRC) ) return;
	CB_SelectGraphVRAM();	// Select Graphic Screen
	CB_DotOprand( SRC, &px1, &py1);
	if ( SRC[ExecPtr] != ',' ) { CB_Error(SyntaxERR); return; }  // Syntax error
	ExecPtr++;
	CB_DotOprand( SRC, &px2, &py2);
	if ( ErrorNo ) return ; // error
	if ( SRC[ExecPtr] == ')' ) ExecPtr++;
	if ( SRC[ExecPtr] == 0x0E ) {  // -> Mat C
		ExecPtr++;
		c =SRC[ExecPtr];
		if ( ( c != 0x7F ) || ( SRC[ExecPtr+1]!=0x40 ) ) { CB_Error(SyntaxERR); return; }	// Syntax error
		ExecPtr+=2;
		c=SRC[ExecPtr];
		if ( ( 'A'<=c )&&( c<='z' ) ) {
			reg=c-'A';
			ExecPtr++;

			if (px1>px2) { i=px1; px1=px2; px2=i; }
			if (py1>py2) { i=py1; py1=py2; py2=i; }

			box.left  =px1;
			box.right =px2;
			box.top   =py1;
			box.bottom=py2;
			
			ElementSize=ElementSizeSelect( SRC, reg, &base );
			if ( ( px1 == 0 ) || ( py1 == 0 ) ) base=0;
			if ( ( ElementSize <= 4 ) || ( ElementSize > 0x100 ) ) ElementSize=1;	// 1 bit matrix
			ElementSize &= 0xFF;
			dx= px2-px1;
			dy= py2-py1;
			switch ( ElementSize ) {
				case  1:
					dimA=dx+1;
					dimB=dy+1;
					break;
				case  8:
					dimA=dy+1;
					dimB=(dx/8)+1;
					break;
				case 16:
					dimA=dy+1;
					dimB=(dx/16)+1;
					break;
				case 32:
					dimA=dy+1;
					dimB=(dx/32)+1;
					break;
				case 64:
					dimA=dy+1;
					dimB=(dx/64)+1;
					break;
			}
			DimMatrixSub( reg, ElementSize, dimA, dimB, base ) ;
			if ( ErrorNo )  return ; 	// error
			
			ptr=(unsigned char*)MatAry[reg].Adrs;
			Bdisp_ReadArea_VRAM( &box, ptr);
		}
	} else { CB_Error(SyntaxERR); return; }	// Syntax error
	if ( scrmode==0 ) CB_SelectTextVRAM();	// Select Text Screen
}

void CB_WriteGraph( char *SRC ){	// WriteGraph x,y,wx,wy,Mat A ([2,2]),modify,kind
	int c;
	DISPGRAPH Gpict;
	int px,py,dx,dy;
	unsigned char Modify;
	unsigned char Kind=IMB_WRITEKIND_OVER;
	char*	MatAryC;
	short*	MatAryW;
	int*	MatAryI;
	double*	MatAryD;
	int reg,i,dimA;
	int x,y;
	unsigned char  *Gptr;
	int ElementSize;
	
	if ( RangeErrorCK(SRC) ) return;
	CB_SelectGraphVRAM();	// Select Graphic Screen
	CB_DotOprand( SRC, &px, &py);
	if ( SRC[ExecPtr] != ',' ) { CB_Error(SyntaxERR); return; }  // Syntax error
	ExecPtr++;
	CB_DotOprand( SRC, &dx, &dy);
	if ( ErrorNo ) return ; // error
	if ( SRC[ExecPtr] != ',' ) { CB_Error(SyntaxERR); return; }  // Syntax error
	ExecPtr++;
	MatrixOprand( SRC, &reg, &x, &y );
	if ( ErrorNo ) return ; // error
	ElementSize=MatAry[reg].ElementSize;
	Gptr=(unsigned char *)MatrixPtr( reg, x, y );

	if ( SRC[ExecPtr] != ',' ) { c='N'; } else { ExecPtr++; c=SRC[ExecPtr++]; }
	if ( c == 'N' ) Modify=IMB_WRITEMODIFY_NORMAL;	// Normal
	else
	if ( c == 'R' ) Modify=IMB_WRITEMODIFY_REVERCE;	// Reverse
	else
	if ( c == 'M' ) Modify=IMB_WRITEMODIFY_MESH;	// Mesh
	else { ExecPtr--; CB_Error(SyntaxERR); return; }  // Syntax error

	if ( SRC[ExecPtr] != ',' ) { c='N'; } else { ExecPtr++; c=SRC[ExecPtr++]; }
	if ( c == 'N' ) Kind=IMB_WRITEKIND_OVER;	// Normal
	else
	if ( c == 'O' ) Kind=IMB_WRITEKIND_OR;		// Or
	else
	if ( c == 'A' ) Kind=IMB_WRITEKIND_AND;		// And
	else
	if ( c == 'X' ) Kind=IMB_WRITEKIND_XOR;		// Xor
	else { ExecPtr--; CB_Error(SyntaxERR); return; }  // Syntax error

	Gpict.x =   px; 
	Gpict.y =   py; 
	Gpict.GraphData.width   = dx;
	Gpict.GraphData.height  = dy;
	Gpict.GraphData.pBitmap = Gptr;
	Gpict.WriteModify = Modify; 
	Gpict.WriteKind   = Kind;

	Bdisp_WriteGraph_VRAM(&Gpict);
	Bdisp_PutDisp_DD_DrawBusy_skip_through(SRC);
}


void CB_DotGet( char *SRC ){	// DotGet(px1,py1, px2,py2)->Mat B [x,y]
	int c;
	int x,y,px,py;
	int px1,py1,px2,py2;
	char*	MatAryC;
	short*	MatAryW;
	int*	MatAryI;
	int reg,i;
	int dimA,dimB;
	int mptr;
	int value;
	int ElementSize;
	int scrmode=ScreenMode;
	
	if ( RangeErrorCK(SRC) ) return;
	CB_SelectGraphVRAM();	// Select Graphic Screen
	CB_DotOprand( SRC, &px1, &py1);
	if ( SRC[ExecPtr] != ',' ) { CB_Error(SyntaxERR); return; }  // Syntax error
	ExecPtr++;
	CB_DotOprand( SRC, &px2, &py2);
	if ( ErrorNo ) return ; // error
	if ( SRC[ExecPtr] == ')' ) ExecPtr++;
	if ( SRC[ExecPtr] == 0x0E ) {  // -> Mat C
			ExecPtr++;
			MatrixOprand( SRC, &reg, &x, &y );
			if ( ErrorNo ) return ; // error

			dimA=MatAry[reg].SizeA;
			dimB=MatAry[reg].SizeB;
			if ( dimA < x ) return;
			if ( dimB < y ) return;
			if (px1>px2) { i=px1; px1=px2; px2=i; }
			if (py1>py2) { i=py1; py1=py2; py2=i; }
			if ( dimA-x < px2-px1 ) px2=px1+dimA-x;
			if ( dimB-y < py2-py1 ) py2=py1+dimB-y;

			i=x;
			for ( py=py1; py<=py2 ; py++) {
				x=i;
				for ( px=px1; px<=px2 ; px++) WriteMatrixInt( reg, x++, y, Bdisp_GetPoint_VRAM(px,py) );
				y++;
			}
	} else { CB_Error(SyntaxERR); return; }	// Syntax error
	if ( scrmode==0 ) CB_SelectTextVRAM();	// Select Text Screen
}

void CB_DotPut( char *SRC ){	// DotPut(Mat B[x,y], px1,py1, px2,py2)
	int c;
	int x,y,px,py;
	int px1,py1,px2,py2;
	double	*Matary2;
	char	*MatAryC,*MatAryC2;
	short	*MatAryW,*MatAryW2;
	int		*MatAryI,*MatAryI2;
	int reg,reg2,i;
	int dimA,dimB;
	int mptr,mptr2;
	int value;
	int mode;
	
	if ( RangeErrorCK(SRC) ) return;
	MatrixOprand( SRC, &reg, &x, &y );
	if ( ErrorNo ) return ; // error
	
	if ( SRC[ExecPtr] != ',' ) { CB_Error(SyntaxERR); return; }  // Syntax error
	ExecPtr++;
	CB_DotOprand( SRC, &px1, &py1);

	if ( SRC[ExecPtr] != ',' ) { CB_Error(SyntaxERR); return; }  // Syntax error
	ExecPtr++;
	CB_DotOprand( SRC, &px2, &py2);
	if ( ErrorNo ) return ; // error		
	if ( SRC[ExecPtr] == ')' ) ExecPtr++;

	dimA=MatAry[reg].SizeA;
	dimB=MatAry[reg].SizeB;
	if (px1>px2) { i=px1; px1=px2; px2=i; }
	if (py1>py2) { i=py1; py1=py2; py2=i; }
	if ( dimA < px2-px1 ) px2=px1+dimA;
	if ( dimB < py2-py1 ) py2=py1+dimB;
		
	mptr=0;
	
	if ( SRC[ExecPtr] == 0x0E ) {  // -> Mat C
			ExecPtr++;
			MatrixOprand( SRC, &reg2, &dimA, &dimB );
			if ( ErrorNo ) return ; // error
			if ( MatAry[reg2].SizeA == 0 ) { CB_Error(NoMatrixArrayERR); return; }	// No Matrix Array error
			
			dimA=MatAry[reg2].SizeA;
			dimB=MatAry[reg2].SizeB;
			if ( dimA+1 < px1 ) return;
			if ( dimB+1 < py1 ) return;
			if ( dimA+1 < px2 ) px2=dimA;
			if ( dimB+1 < py2 ) py2=dimB;
			if ( dimA < px2-px1 ) px2=px1+dimA;
			if ( dimB < py2-py1 ) py2=py1+dimB;

			i=x;
			for ( py=py1; py<=py2 ; py++) {	
				x=i;
				for ( px=px1; px<=px2 ; px++) WriteMatrixInt( reg2, px, py, ReadMatrixInt( reg, x++, y));
				y++;
			}
			dspflag=0;
	} else {	//			-> VRAM
		CB_SelectGraphVRAM();	// Select Graphic Screen
		mode=CB_SetPointMode(SRC);
		i=x;
		for ( py=py1; py<=py2 ; py++) {	
			x=i;
			for ( px=px1; px<=px2 ; px++) 
				if ( ReadMatrixInt( reg, x++, y ) ) BdispSetPointVRAM2(px, py, mode);
					else							BdispSetPointVRAM2(px, py, 0);
			y++;
		}
		
		Bdisp_PutDisp_DD_DrawBusy_skip_through(SRC);
		dspflag=0;
		UseGraphic=99;
	}
}

void CB_DotTrim( char *SRC ){	// DotTrim(Mat A,x1,y1,x2,y2)->Mat B    =>[X,Y]
	int c;
	int x,y, px,py;
	int x1,y1,x2,y2;
	int px1,py1,px2,py2;
	int startx=0,starty=0,endx=0,endy=0;
	double	*Matary2;
	char	*MatAryC,*MatAryC2;
	short	*MatAryW,*MatAryW2;
	int		*MatAryI,*MatAryI2;
	int reg=-1,reg2,i;
	int dimA,dimB;
	int mptr,mptr2;
	int value;
	int ElementSize;
	int scrmode=ScreenMode;
	
	c =SRC[ExecPtr];
	if ( ( c == 0x7F ) && ( SRC[ExecPtr+1]==0x40 ) ) {	// Mat A
		ExecPtr+=2;
		c=SRC[ExecPtr];
		if ( ( 'A' > c ) || ( c > 'z' ) ) { CB_Error(SyntaxERR); return; }	// Syntax error
		reg=c-'A';
		if ( MatAry[reg].SizeA == 0 ) { CB_Error(NoMatrixArrayERR); return; }	// No Matrix Array error
		ExecPtr++;
		c=SRC[ExecPtr];
		if ( c != ',' ) { CB_Error(SyntaxERR); return; }  // Syntax error
		ExecPtr++;
		px1=CB_EvalInt( SRC );
		if ( ( px1 < 1 ) || ( MatAry[reg].SizeA < px1 ) ) CB_Error(RangeERR) ; // Range error 
		if ( SRC[ExecPtr] != ',' ) { CB_Error(SyntaxERR); return; }  // Syntax error
		ExecPtr++;
		py1=CB_EvalInt( SRC );
		if ( ( py1 < 1 ) || ( MatAry[reg].SizeB < py1 ) ) CB_Error(RangeERR) ; // Range error 
		
		if ( SRC[ExecPtr] != ',' ) { CB_Error(SyntaxERR); return; }  // Syntax error
		ExecPtr++;
		CB_DotOprand( SRC, &px2, &py2);
		if ( ErrorNo ) return ; // error

		if ( SRC[ExecPtr] == ')' ) ExecPtr++;

		dimA=MatAry[reg].SizeA;
		dimB=MatAry[reg].SizeB;
		if (px1>px2) { i=px1; px1=px2; px2=i; }
		if (py1>py2) { i=py1; py1=py2; py2=i; }
		if ( dimA < px2-px1 ) px2=px1+dimA;
		if ( dimB < py2-py1 ) py2=py1+dimB;
		startx=px2; endx=px1;
		starty=py2; endy=py1;

		i=MatAry[reg].Base;
		y=i;
		for ( py=py1; py<=py2 ; py++) {	
			x=i;
			for ( px=px1; px<=px2 ; px++) 
				if ( ReadMatrixInt( reg, x++, y ) ){
					if ( py<starty ) starty=py;
					if ( py>endy   ) endy  =py;
					if ( px<startx ) startx=px;
					if ( px>endx   ) endx  =px;
				}
			y++;
		}

	} else {
			if ( RangeErrorCK(SRC) ) return;
			CB_SelectGraphVRAM();	// Select Graphic Screen
			CB_DotOprand( SRC, &px1, &py1);
			if ( SRC[ExecPtr] != ',' ) { CB_Error(SyntaxERR); return; }  // Syntax error
			ExecPtr++;
			CB_DotOprand( SRC, &px2, &py2);
			if ( SRC[ExecPtr] == ')' ) ExecPtr++;
			if ( ErrorNo ) return ; // error

			if (px1>px2) { i=px1; x1=px2; px2=i; }
			if (py1>py2) { i=py1; y1=py2; py2=i; }
			startx=px2; endx=px1;
			starty=py2; endy=py1;
				
			for ( py=py1+1; py<=py2+1 ; py++) {
				for ( px=px1+1; px<=px2+1 ; px++) {
					if ( Bdisp_GetPoint_VRAM(px,py) ) {
						if ( py<starty ) starty=py;
						if ( py>endy   ) endy  =py;
						if ( px<startx ) startx=px;
						if ( px>endx   ) endx  =px;
					}
				}
			}
	}

	if ( SRC[ExecPtr] != 0x0E ) { CB_Error(SyntaxERR); return; }	// Syntax error
	ExecPtr++;

	if ( ( startx > endx ) || ( starty > endy ) ) { dimA=0; dimB=0; startx=0; starty=0; goto dottret; }
	dimA=endx-startx+1;
	dimB=endy-starty+1;
	CB_MatrixInitsub( SRC, &reg2, dimA, dimB, 0 );
	if ( ErrorNo )  return ; 	// error

	px1=startx;
	py1=starty;
	px2=endx;
	py2=endy;
	x=MatAry[reg2].Base;
	y=x;

	if ( reg >= 0 ) {
		i=x;
		for ( py=py1; py<=py2 ; py++) {	
			x=i;
			for ( px=px1  ; px<=px2  ; px++) WriteMatrixInt( reg2, x++, y, ReadMatrixInt( reg, px, py ) );
			y++;
		}
	} else {
		i=x;
		for ( py=py1+1; py<=py2+1 ; py++) {
			x=i;
			for ( px=px1+1; px<=px2+1; px++) WriteMatrixInt( reg2, x++, y, Bdisp_GetPoint_VRAM(px, py ) );
			y++;
		}
	}

	startx++; starty++;
	dottret:
	LocalInt[23][0]=startx;	// regint x
	LocalInt[24][0]=starty;	// regint y
	regintX=dimA;			// regint X
	regintY=dimB;			// regint Y
	if ( CB_INT == 0 ) { 
		LocalDbl[23][0]=startx;	// reg x
		LocalDbl[24][0]=starty;	// reg y
		regX=dimA;			// reg X
		regY=dimB;			// reg Y
	}
	if ( scrmode==0 ) CB_SelectTextVRAM();	// Select Text Screen
}



void CB_DotLife( char *SRC ){	// DotLife(Mat A,x1,y1,x2,y2)->Mat B    =>[X,Y]
	int c;
	int x,y, px,py;
	int x1,y1,x2,y2;
	int px1,py1,px2,py2;
	int startx=0,starty=0,endx=0,endy=0;
	double	*Matary2;
	char	*MatAryC,*MatAryC2;
	short	*MatAryW,*MatAryW2;
	int		*MatAryI,*MatAryI2;
	int reg=-1,reg2,i;
	int dimA,dimB;
	int mptr,mptr2;
	int value;
	int ElementSize;
	int scrmode=ScreenMode;
	
	MatrixOprand( SRC, &reg, &dimA, &dimB );
	if ( ErrorNo ) return ; // error
	if ( MatAry[reg].SizeA == 0 ) { CB_Error(NoMatrixArrayERR); return; }	// No Matrix Array error

	if ( SRC[ExecPtr] != ',' ) { CB_Error(SyntaxERR); return; }  // Syntax error
	ExecPtr++;
	CB_DotOprand( SRC, &px1, &py1);
	if ( SRC[ExecPtr] != ',' ) { CB_Error(SyntaxERR); return; }  // Syntax error
	ExecPtr++;
	CB_DotOprand( SRC, &px2, &py2);
	if ( SRC[ExecPtr] == ')' ) ExecPtr++;
	if ( ErrorNo ) return ; // error

	dimA=MatAry[reg].SizeA;
	dimB=MatAry[reg].SizeB;
	if (px1>px2) { i=px1; px1=px2; px2=i; }
	if (py1>py2) { i=py1; py1=py2; py2=i; }
	if ( dimA < px2 ) px2=dimA;
	if ( dimB < py2 ) py2=dimB;

	if ( SRC[ExecPtr] != 0x0E ) { CB_Error(SyntaxERR); return; }	// Syntax error
	ExecPtr++;
	MatrixOprand( SRC, &reg2, &dimA, &dimB );
	if ( ErrorNo ) return ; // error
	if ( MatAry[reg2].SizeA == 0 ) { CB_Error(NoMatrixArrayERR); return; }	// No Matrix Array error

	for ( y=py1; y<=py2; y++) {
		for ( x=px1; x<=px2; x++) {
			c=Cellsum( reg, x, y );
			if ( ReadMatrixInt( reg, x, y ) ) {
				if ( (c<=1)||(c>=4) ) WriteMatrixInt(reg2, x, y, 0);
			} else {
				if ( c==3 ) WriteMatrixInt(reg2, x, y, 1);
			}
			if ( c ) {
				if (regintL<=2) regintL=2;
				else if (x<=regintL) regintL--;
				if (regintR>=126) regintR=126;
				else if (x>=regintR) regintR++;
				if (regintT<=2) regintT=2;
				else if (y<=regintT) regintT--;
				if (regintB>=62) regintB=62;
				else if (y>=regintB) regintB++;
			}
		}
	}
}


//----------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------
unsigned int GWait( int exit_cancel ) {
	unsigned int key=0,key2=0;
	int cont=1;
	if (ScreenMode) {
		if ( (UseGraphic==1)||(UseGraphic==2) ) { key=KEY_CTRL_SHIFT; key2=KEY_CTRL_F6; CB_SelectTextVRAM(); }
	}
	while (cont) {
		if (key==0) GetKey(&key);
		switch ( key ) {
			case KEY_CTRL_AC:
				if ( ScreenMode==0 ) cont =0 ;
				else { if (exit_cancel) cont=0; 
						else { key=KEY_CTRL_SHIFT; key2=KEY_CTRL_F6; cont=1; }
				}
				break;
			case KEY_CTRL_EXE:
				if (exit_cancel==0) { 
					if ( ScreenMode==0 ) cont=0;
					else { key=KEY_CTRL_SHIFT; key2=KEY_CTRL_F6; }	// G<>T
				} else { cont=0;
					if ( UseGraphic == 1 ) PlotXYtoPrevPXY();
				}
				break;
			case KEY_CTRL_EXIT:
				if (exit_cancel==0) { 
					if ( ScreenMode==0 ) cont =0 ;
					else { key=KEY_CTRL_SHIFT; key2=KEY_CTRL_F6; }	// G<>T
				} else { key=0; key2=0;
					if ( UseGraphic == 1 ) PlotXYtoPrevPXY();
				}
				break;
			case KEY_CTRL_SHIFT:
				SaveDisp(SAVEDISP_PAGE1);		// ------ SaveDisp1
				locate(1,8); PrintLine((unsigned char*)" ",21);
				Fkey_dispR( 0, "Var");
				Fkey_dispR( 1, "Mat");
				Fkey_dispR( 2, "V-W");
				Fkey_Clear( 3 );
				Fkey_Clear( 4 );
				Fkey_dispN( 5, "G<>T");
				if (key2==0) GetKey(&key2);
				switch (key2) {
					case KEY_CTRL_SETUP:
							selectSetup=SetupG(selectSetup);
							key=0; key2=0;
							break;
					case KEY_CTRL_F1:
							selectVar=SetVar(selectVar);		// A - 
							key=0; key2=0;
							break;
					case KEY_CTRL_F2:
							selectMatrix=SetMatrix(selectMatrix);		// 
							key=0; key2=0;
							break;
					case KEY_CTRL_F3:
							SetViewWindow();
							key=0; key2=0;
							break;
					case KEY_CTRL_F6:
							RestoreDisp(SAVEDISP_PAGE1);	// ------ RestoreDisp1
							key=0; key2=0;
							if (ScreenMode) CB_SelectTextVRAM();	// Select Text Screen
								else		CB_SelectGraphVRAM();	// Select Graphic Screen
							if (ScreenMode) {
								if (UseGraphic==1) {
									key=Plot();	// Plot
									if ( key==KEY_CTRL_EXE ) {
//										if (exit_cancel==0) { //  end program
											PlotCurrentXY(); // current plot
//										}
									}
								}
								if (UseGraphic==2) key=Graph_main();
								if ( key==KEY_CTRL_AC   ) { if (exit_cancel) cont=0; 
																else { key=KEY_CTRL_SHIFT; key2=KEY_CTRL_F6; } }
								if ( key==KEY_CTRL_EXIT ) { key=KEY_CTRL_SHIFT; key2=KEY_CTRL_F6; }
								if ( key==KEY_CTRL_F6   ) { key=KEY_CTRL_SHIFT; key2=KEY_CTRL_F6; }
							}
							SaveDisp(SAVEDISP_PAGE1);		// ------ SaveDisp1
							break;
					default:
						key=0; key2=0;
						break;
				}
				RestoreDisp(SAVEDISP_PAGE1);	// ------ RestoreDisp1
				break;
			case KEY_CTRL_OPTN:	//
				SaveDisp(SAVEDISP_PAGE1);		// ------ SaveDisp1
				key=Pict();
				switch (key) {
					case KEY_CTRL_AC:
					case KEY_CTRL_EXE:
						cont=0;
						break;
					case KEY_CTRL_SHIFT:
						break;
					default:
						break;
				}
				break;
			default:
				key=0; key2=0;
				break;
		}
	}
	return key;
}


int CB_Disp( char *SRC ){		// Disp command
	char buffer[CB_StrBufferMax];
	int c;
	double value;
	
	CB_SelectTextVRAM();	// Select Text Screen
	if ( CursorX >1 ) Scrl_Y();
	
	c=CB_IsStr( SRC, ExecPtr );
	if ( c ) {	// string
		CB_GetLocateStr( SRC, buffer, CB_StrBufferMax-1 );		// String -> buffer	return 
	} else {	// expression
		value = CB_EvalDbl( SRC );
		sprintGR(buffer, value, 22-CursorX,RIGHT_ALIGN, CB_Round.MODE, CB_Round.DIGIT);
	}
	locate( CursorX, CursorY); Print((unsigned char*)buffer);
	CursorX=21;
	Bdisp_PutDisp_DD_DrawBusy_skip_through_text(SRC);

	return 0;
}

int CB_Disps( char *SRC ,short dspflag){	// Disps command
	char buffer[32];
	int c;
	unsigned int key=0;
	int scrmode;
	
	KeyRecover();
	scrmode=ScreenMode;
	if ( dspflag == 2 ) {
		CB_SelectTextVRAM();	// Select Text Screen
		if ( CursorX >1 ) Scrl_Y();
		if ( CB_INT )
			sprintGR(buffer, CBint_CurrentValue, 22-CursorX,RIGHT_ALIGN, CB_Round.MODE, CB_Round.DIGIT);
		else
			sprintGR(buffer, CB_CurrentValue, 22-CursorX,RIGHT_ALIGN, CB_Round.MODE, CB_Round.DIGIT);
		locate( CursorX, CursorY); Print((unsigned char*)buffer);
		CursorX=21;
		scrmode=ScreenMode;
	} else
	if ( dspflag == 3 ) { 	// Matrix display
		c=SRC[ExecPtr-2];
		if ( ( 'A'<=c )&&( c<='z' ) ) {
			CB_SelectTextVRAM();	// Select Text Screen
			CB_SaveTextVRAM();
			EditMatrix( c -'A' );
			CB_RestoreTextVRAM();	// Resotre Graphic screen
			if ( scrmode  ) CB_SelectGraphVRAM();	// Select Graphic screen
			scrmode=ScreenMode;
		}
	}
	if (scrmode) {
		CB_SelectTextVRAM();	// Select Text Screen
		PrintDone();
	}
	if ( CursorX >1 ) Scrl_Y();
	locate( CursorX, CursorY); Print((unsigned char*)"             - Disp -");
	if ( scrmode ) CB_SelectGraphVRAM();	// Select Graphic Screen
	if (UseGraphic>0x100) UseGraphic=UseGraphic&0xFF; 
	Bdisp_PutDisp_DD();
	
	while ( 1 ) {
		key=GWait(EXIT_CANCEL_ON);
		if ( key == KEY_CTRL_EXE ) break ;
		if ( key == KEY_CTRL_AC  ) { ExecPtr--; return 1 ; }	// break;
	}

	CB_SelectTextVRAM();	// Select Text Screen
	locate( CursorX, CursorY); Print((unsigned char*)"                     ");
	CursorX=1;
	if ( scrmode ) CB_SelectGraphVRAM();	// Select Graphic Screen

	if ( UseGraphic == 1 ) PlotXYtoPrevPXY(); // Plot
	if ( UseGraphic ) UseGraphic=UseGraphic | 0x100;  // 
	Bdisp_PutDisp_DD_DrawBusy();
	return 0;
}

void CB_end( char *SRC ){
	char buffer[32];
	int c,t;
	unsigned int key=0;
	int scrmode=ScreenMode;

	if ( ProgEntryN ) return ; //	in sub Prog
	ExecPtr++;
	
	KeyRecover();
	CB_SelectTextVRAM();	// Select Text Screen
	if ( dspflag == 2 ) {
		if ( CursorX >1 ) Scrl_Y();
		if ( CB_INT )
			sprintGR(buffer, CBint_CurrentValue, 22-CursorX,RIGHT_ALIGN, CB_Round.MODE, CB_Round.DIGIT);
		else
			sprintGR(buffer, CB_CurrentValue, 22-CursorX,RIGHT_ALIGN, CB_Round.MODE, CB_Round.DIGIT);
		locate( CursorX, CursorY); Print((unsigned char*)buffer);
		CursorX=21;
	} else
	if ( dspflag == 3 ) { 	// Matrix display
		c=SRC[ExecPtr-2];
		if ( ( 'A'<=c )&&( c<='z' ) ) {
			CB_SaveTextVRAM();
			EditMatrix( c -'A' );
			CB_RestoreTextVRAM();	// Resotre Graphic screen
			CB_Done();
		}
	}
	if ( dspflag == 0 ) CB_Done();
	if ( scrmode ) CB_SelectGraphVRAM();	// Select Graphic Screen
	if ( UseGraphic ) if ( ( UseGraphic != 1 ) &&( UseGraphic != 2 ) && ( UseGraphic != 99 ) ) { UseGraphic=UseGraphic&0xFF; CB_SelectTextVRAM(); }
	Bdisp_PutDisp_DD();

	while ( 1 ) {
		key=GWait(EXIT_CANCEL_OFF);
		if ( key == KEY_CTRL_EXE ) break ;
		if ( key == KEY_CTRL_EXIT ) break ;
		if ( key == KEY_CTRL_AC  ) return ;
	}
	
	CB_SelectTextVRAM();	// Select Text Screen
	if ( TimeDsp ) {
		if ( CursorX >1 ) Scrl_Y();
		sprintGRS(buffer, (double)(CB_TicksEnd-CB_TicksStart)/128.0, 8,RIGHT_ALIGN, Fix, 2);  // Fix:2
		locate(  1, CursorY); Print((unsigned char*)"Execute Time=");
		locate( 14, CursorY); Print((unsigned char*)buffer);
		while ( 1 ) {
			key=GWait(EXIT_CANCEL_OFF);
			if ( key == KEY_CTRL_EXE ) break ;
			if ( key == KEY_CTRL_EXIT ) break ;
			if ( key == KEY_CTRL_AC  ) return ;
		}
	}
	if ( (UseGraphic&0xFF) == 1 ) {	// Plot 
		PlotXYtoPrevPXY();
	}
}

//----------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------

void CB_FkeyMenu( char *SRC) {		// FkeyMenu(6,"ABC",R)
	char buffer[9];
	int c;
	int n;
	int vflag=1;

	n=CB_EvalInt( SRC );
	if ( ( n<1 )||(n>6) ) { CB_Error(ArgumentERR); return; }	// Argumenterror
	c=SRC[ExecPtr];
	if ( c != ',' ) { CB_Error(SyntaxERR); return; }	// Syntax error
	ExecPtr++;
	CB_GetLocateStr( SRC, buffer, 8 );		// String -> buffer	return 
	c=SRC[ExecPtr];
	if ( c != ',' )	 {
		if ( RangeErrorCK(SRC) ) return;
		CB_SelectGraphVRAM();	// Select Graphic Screen
		Fkey_dispN( n-1 ,buffer);	// Normal
	} else {
		c=SRC[++ExecPtr];
		if ( ( c=='T' ) || ( c=='t' ) ) {
			vflag=0;
			UseGraphic=0;
			CB_SelectTextVRAM();	// Select Text Screen
			c=SRC[++ExecPtr];
		} else {
			if ( RangeErrorCK(SRC) ) return;
			CB_SelectGraphVRAM();	// Select Graphic Screen
		}
		switch ( c ) {
			case 'C':
			case 'c':
				ExecPtr++;	
				Fkey_Clear( n-1 );			// clear
				break;
			case 'R':
			case 'r':
				ExecPtr++;	
				Fkey_dispR( n-1 ,buffer);	// Reverse
				break;
			case 'N':
			case 'n':
				ExecPtr++;	
			default:
				Fkey_dispN( n-1 ,buffer);	// Normal
				break;
		}
	}
	if ( SRC[ExecPtr] == ')' ) ExecPtr++;
	Bdisp_PutDisp_DD_DrawBusy_skip_through(SRC);
}

//----------------------------------------------------------------------------------------------
//int GObjectAlign4d( unsigned int n ){ return n; }	// align +4byte
//int GObjectAlign4e( unsigned int n ){ return n; }	// align +4byte
//int GObjectAlign4f( unsigned int n ){ return n; }	// align +4byte
//----------------------------------------------------------------------------------------------
