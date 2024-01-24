/*
===============================================================================

 Casio Basic interpreter for fx-9860G series    v1.2

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
#include "MonochromeLib.h"

//----------------------------------------------------------------------------------------------
//int VGObjectAlign4a( unsigned int n ){ return n; }	// align +4byte
//int VGObjectAlign4b( unsigned int n ){ return n; }	// align +4byte
//int VGObjectAlign4c( unsigned int n ){ return n; }	// align +4byte
//----------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------
// unsigned char GVRAM[1024];

void SaveVRAM( char* vram){
	Bdisp_GetDisp_VRAM( (unsigned char*)vram );
}
//void RestoreVRAM( char* vram){
//	DISPGRAPH Gpict;
//	Gpict.x =   0; 
//	Gpict.y =   0; 
//	Gpict.GraphData.width   = 128;
//	Gpict.GraphData.height  = 64;
//	Gpict.GraphData.pBitmap = (unsigned char*)vram;
//	Gpict.WriteModify = IMB_WRITEMODIFY_NORMAL; 
//	Gpict.WriteKind   = IMB_WRITEKIND_OVER;
//	Bdisp_WriteGraph_VRAM(&Gpict);
//}
void RestoreVRAM( char* vram){
	int i;
	char *vramadrs=(char*)GetVRAMAddress();
	for ( i=0; i<1024; i++) 	vramadrs[i] = vram[i];
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
void CB_PopUpWin( char *SRC ){	// PopUpWin(
	int n;
	n=CB_EvalInt( SRC );
	if ( ( n<1 ) || ( n>5 ) ) { CB_Error(ArgumentERR); return; }	// Argument error
	if ( SRC[ExecPtr] == ')' ) ExecPtr++;
	PopUpWin(n); 
}

void Scrl_Y(){
	CursorY++;
	if ( CursorY > 7 ) {		// scroll
		ML_vertical_scroll(56);
		memset(GetVRAMAddress()+16*56, 0, 16*8);
		CursorY=7;
	}
	CursorX=1;
}
/*
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
int CB_ChangeTextMode( char *SRC ) {
	int c=SRC[ExecPtr];
	if ( c == '@' ) {	// Only Vram Operation
		ExecPtr++;
		return 1 ;
	} else {
		CB_SelectTextVRAM();	// Select Text Screen
	}
	return 0;
}

int CB_LocateMode( char *SRC) {
	int c=SRC[ExecPtr];
	if ( c != ',' ) return 0;	// normal
	ExecPtr++;	
	c=SRC[ExecPtr++];
	if ( ( c == 'N' ) || ( c == 'n' ) ) return 0;	// normal
	else
	if ( ( c == 'R' ) || ( c == 'r' ) ) return 1;	// reverse
	ExecPtr--; CB_Error(SyntaxERR); return;   // Syntax error
}

void CB_Locate( char *SRC ){
	char buffer[CB_StrBufferMax];
	int c;
	int lx,ly;
	double value;
	int mode;
	int maxoplen;
	
	CB_ChangeTextMode( SRC );	// Select Text Mode
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
	if ( CB_LocateMode( SRC ) ) CB_PrintRev( lx,ly, (unsigned char*)buffer );
	else 		   			  CB_Print( lx,ly, (unsigned char*)buffer );
	Bdisp_PutDisp_DD_DrawBusy_skip_through_text( SRC );
}

//----------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------
int CB_ChangeGraphicMode( char *SRC ) {
	int c=SRC[ExecPtr];
	if ( c == '@' ) {	// Only Vram Operation
		ExecPtr++;
		return 1 ;
	} else {
		if ( UseGraphic == 3 ) {	// Cls or ViewWindow
			CB_SelectGraphVRAM();	// Select Graphic Screen
		//	Bdisp_AllClr_VRAM();			// ------ Clear VRAM 
			ML_clear_vram();
			GraphAxesGrid();
			return 0;
		}
		if ( ScreenMode == 1 ) {	// Graphic Mode
			return 0;
		} else {
			CB_SelectGraphVRAM();	// Select Graphic Screen
			if ( UseGraphic == 0 ) {
				GraphAxesGrid();
			}
		}
	}
	return 0;
}
int RangeErrorCK( char *SRC ) {
	if ( ( Xdot == 0 ) || ( Ydot == 0 )  ) { CB_Error(RangeERR); PrevOpcode( SRC, &ExecPtr ); return ErrorNo; }	// Range error
	return 0;
}
int CB_RangeErrorCK_ChangeGraphicMode( char *SRC ) {
	if ( RangeErrorCK( SRC ) ) return RangeERR;
	CB_ChangeGraphicMode( SRC );	// Select Graphic Mode
	return 0;
}

void CB_Cls( char *SRC ){
	if ( CB_RangeErrorCK_ChangeGraphicMode( SRC ) ) return;	// Select Graphic Mode
	CB_ChangeViewWindow();
//	ViewWindow( Xmin, Xmax, Xscl, Ymin, Ymax, Yscl);
//	Bdisp_AllClr_VRAM();
//	ML_clear_vram();
//	GraphAxesGrid();
	Previous_PX=-1;   Previous_PY=-1; 		// ViewWindow Previous PXY init
//	CB_SelectTextVRAM();	// Select Text Screen
//	Bdisp_PutDisp_DD_DrawBusy_through( SRC );
}
void CB_ClrText( char *SRC ){
	CB_SelectTextVRAM();	// Select Text Screen
	CursorX=1;
	CursorY=1;
//	Bdisp_AllClr_VRAM();
	ML_clear_vram();
	Bdisp_PutDisp_DD_DrawBusy_skip_through_text( SRC );
}
void CB_ClrGraph( char *SRC ){
	SetVeiwWindowInit();
	CB_Cls( SRC );
}
void CB_Screen( char *SRC ){	// Screen.G   Screen.T
	int c;
	c=SRC[ExecPtr++];
	if ( c == '.' ) { c=SRC[ExecPtr++];
		if ( ( c=='G' ) || ( c=='g' ) ) { CB_ChangeGraphicMode( SRC ); UseGraphic=9; return; }	// Select Graphic Screen
		if ( ( c=='T' ) || ( c=='t' ) ) { CB_SelectTextVRAM();  return; }	// Select Text Screen
		{ ExecPtr--; CB_Error(SyntaxERR); return; }	// Syntax error
	} else 
	if ( ( c==0 ) || ( c==0x0D ) || ( c==0x0C ) || ( c==':' ) ) {
			if ( ScreenMode == 0 )  { CB_ChangeGraphicMode( SRC ); UseGraphic=9; return; }	// Select Graphic Screen
			else					{ CB_SelectTextVRAM();  return; }	// Select Text Screen

	} else { ExecPtr--;
		switch ( CB_EvalInt( SRC ) ) {
			case 0:
				CB_SelectTextVRAM();	// Select Text Screen
				break;
			case 1:
				CB_ChangeGraphicMode( SRC ); UseGraphic=9;	// Select Graphic Screen
				break;
		}
	}
}
//----------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------

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

	if ( CB_RangeErrorCK_ChangeGraphicMode( SRC ) ) return;	// Select Graphic Mode
	if ( SRC[ExecPtr] == '_' ) { ExecPtr++; kanamini=0; }		// OS PrintMini
	CB_TextOprand( SRC, &py, &px);
	c=SRC[ExecPtr];
	if ( c != ',' ) { CB_Error(SyntaxERR); return; }	// Syntax error
	c=SRC[++ExecPtr];
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
		if ( ( c == 'N' ) || ( c == 'n' ) )  mode=MINI_OVER;	// Normal
		else
		if ( ( c == 'R' ) || ( c == 'r' ) )  mode=MINI_REV;	// Reverse
		else
		if ( ( c == 'O' ) || ( c == 'o' ) )  mode=MINI_OR;	// Or
		else
		if ( ( c == 'V' ) || ( c == 'v' ) )  mode=MINI_REVOR;	// reVerse or
		else { ExecPtr--; CB_Error(SyntaxERR); return; }  // Syntax error
	}
	if ( kanamini ) CB_PrintMini( px, py, (unsigned char*)buffer, mode);
		else 	       PrintMini( px, py, (unsigned char*)buffer, mode);
	Bdisp_PutDisp_DD_DrawBusy_skip_through_text( SRC );
}
//-----------------------------------------------------------------------------
void CB_LocateYX( char *SRC ){
	char buffer[CB_StrBufferMax];
	int c;
	int px,py,d;
	double value;
	int mode;
	int maxoplen;

	if ( CB_RangeErrorCK_ChangeGraphicMode( SRC ) ) return;	// Select Graphic Mode
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
	CB_PrintXY(  px,py, (unsigned char*)buffer, CB_LocateMode( SRC ));	
	Bdisp_PutDisp_DD_DrawBusy_skip_through_text( SRC );

}

//----------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------

void CB_ChangeViewWindow() {
//	int scrmode=ScreenMode;
//	if ( ScreenMode == 0 ) CB_SelectGraphVRAM();	// Select Graphic Screen
	CB_SelectGraphVRAM();	// Select Graphic Screen
	ViewWindow( Xmin, Xmax, Xscl, Ymin, Ymax, Yscl);
	UseGraphic=3;
//	if ( scrmode == 0 )	CB_SelectTextVRAM();	// Select Text Screen
}
void CB_ViewWindow( char *SRC ) { //	ViewWindow
	int c;
	int reg=0;
	while ( reg <= 10 ) {
		c=SRC[ExecPtr];
		if ( c == ',' ) goto next;
		if ( (c==':') || (c==0x0C) || (c==0x0D) || (c==0x00) ) break;
		REGv[reg]=CB_EvalDbl( SRC );
		c=SRC[ExecPtr];
		if ( (c==':') || (c==0x0C) || (c==0x0D) || (c==0x00) ) break;
		if ( c != ',' ) { CB_Error(SyntaxERR); return; }  // Syntax error
	  next:
		ExecPtr++;
		reg++;
	}
//	CB_SelectGraphVRAM();	// Select Graphic Screen
//	ViewWindow( Xmin, Xmax, Xscl, Ymin, Ymax, Yscl);
//	CB_SelectTextVRAM();	// Select Text Screen
	CB_ChangeViewWindow() ;
}

int CB_SetPointMode( char *SRC) {
	int c=SRC[ExecPtr];
	if ( c != ',' ) return 1;
	ExecPtr++;	
	c=SRC[ExecPtr++];
	if ( ( c == 'C' ) || ( c == 'c' ) ) return 0;	// Clear
	else
	if ( ( c == 'X' ) || ( c == 'x' ) )  return 2;	// Xor
	else
	if ( ( c == 'P' ) || ( c == 'p' ) )  return 1;	// setPoint
	ExecPtr--; CB_Error(SyntaxERR); return;   // Syntax error
}

void CB_FLine( char *SRC) { //	F-Line
	int c;
	double x1,y1,x2,y2;
	int style=S_L_Style;

	if ( CB_RangeErrorCK_ChangeGraphicMode( SRC ) ) return;	// Select Graphic Mode
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
	F_Line(x1, y1, x2, y2, style, CB_SetPointMode( SRC ) );
	Bdisp_PutDisp_DD_DrawBusy_skip_through( SRC );
	tmp_Style = -1;
}

void CB_Line( char *SRC ) { //	Line
	int style=S_L_Style;
	if ( tmp_Style >= 0 ) style=tmp_Style;
	if ( CB_RangeErrorCK_ChangeGraphicMode( SRC ) ) return;	// Select Graphic Mode
	Line( style, CB_SetPointMode( SRC ) , 1 );	// error check
	Bdisp_PutDisp_DD_DrawBusy_skip_through( SRC );
	tmp_Style = -1;
}

void CB_Vertical( char *SRC ) { //	Vertical
	double x;
	int style=S_L_Style;
	if ( tmp_Style >= 0 ) style=tmp_Style;
	if ( CB_RangeErrorCK_ChangeGraphicMode( SRC ) ) return;	// Select Graphic Mode
	x=CB_EvalDbl( SRC );
	Vertical(x, style, CB_SetPointMode( SRC ) );
	Bdisp_PutDisp_DD_DrawBusy_skip_through( SRC );
	tmp_Style = -1;
}
void CB_Horizontal( char *SRC ) { //	Horizontal
	double y;
	int style=S_L_Style;
	if ( tmp_Style >= 0 ) style=tmp_Style;
	if ( CB_RangeErrorCK_ChangeGraphicMode( SRC ) ) return;	// Select Graphic Mode
	y=CB_EvalDbl( SRC );
	Horizontal(y, style, CB_SetPointMode( SRC ) );
	Bdisp_PutDisp_DD_DrawBusy_skip_through( SRC );
	tmp_Style = -1;
}

//----------------------------------------------------------------------------------------------
void CB_PxlSub( char *SRC, int mode ) { //	mode  1:PxlOn  0:PxlOff  2:PxlChg
	double x,y;
	int px,py;
	if ( CB_RangeErrorCK_ChangeGraphicMode( SRC ) ) return;	// Select Graphic Mode
	y = (EvalsubTop( SRC ));
	py=y;
	if ( ( y-floor(y) ) || ( (py)<MatBase ) || ( (py)>63 ) ) { CB_Error(ArgumentERR); return; }  // Argument error
	if ( SRC[ExecPtr] != ',' ) { CB_Error(SyntaxERR); return; }  // Syntax error
	ExecPtr++;
	x = (EvalsubTop( SRC ));
	px=x;
	if ( ( x-floor(x) ) || ( (px)<MatBase ) || ( (px)>127 ) ) { CB_Error(ArgumentERR); return; }  // Argument error}

	BdispSetPointVRAM2(px, py, mode);
	PXYtoVW(px, py, &regX, &regY);
	Bdisp_PutDisp_DD_DrawBusy_skip_through( SRC );
}
//----------------------------------------------------------------------------------------------
//int ObjectAlign4p( unsigned int n ){ return n; }	// align +4byte
//----------------------------------------------------------------------------------------------
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

void CB_Plot( char *SRC ) { //	Plot
	int c;
	double x,y;
	
	if ( CB_RangeErrorCK_ChangeGraphicMode( SRC ) ) return;	// Select Graphic Mode
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
	Bdisp_PutDisp_DD_DrawBusy_skip_through( SRC );
}

void CB_PlotSub( char *SRC, int mode ) { //	mode  1:PlotOn  0:PlotOff  2:PlotChg
	double x,y;
	if ( CB_RangeErrorCK_ChangeGraphicMode( SRC ) ) return;	// Select Graphic Mode
	x=CB_EvalDbl( SRC );
	if ( SRC[ExecPtr] != ',' ) { CB_Error(SyntaxERR); return; }  // Syntax error
	ExecPtr++;
	y=CB_EvalDbl( SRC );
	PlotSub(x,y,mode);
	Bdisp_PutDisp_DD_DrawBusy_skip_through( SRC );
}

//----------------------------------------------------------------------------------------------

void CB_Circle( char *SRC ) { //	Circle
	int c,mode;
	double x,y,r;
	int style=S_L_Style;
	if ( tmp_Style >= 0 ) style=tmp_Style;
	if ( CB_RangeErrorCK_ChangeGraphicMode( SRC ) ) return;	// Select Graphic Mode
	x=CB_EvalDbl( SRC );
	c=SRC[ExecPtr];
	if ( c != ',' ) { CB_Error(SyntaxERR); return; }  // Syntax error
	ExecPtr++;
	y=CB_EvalDbl( SRC );
	c=SRC[ExecPtr];
	if ( c != ',' ) { CB_Error(SyntaxERR); return; }  // Syntax error
	ExecPtr++;
	r=CB_EvalDbl( SRC );
	mode=CB_SetPointMode( SRC ) ;
	c=SRC[ExecPtr];
	if ( c==':' ) 	Circle(x, y, r, style, 0, mode);
		else 		Circle(x, y, r, style, 1, mode);
	Bdisp_PutDisp_DD_DrawBusy_skip_through( SRC );
	tmp_Style = -1;
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

void StoPictSmem( int pictNo){
	unsigned char pict[2048+0x4C+4];
	int i,stat;
	ReadVram(pict+0x4C);
	for(i=1024+0x4c; i<2048+0x4c+2; i++) pict[i]=0;
	stat=SavePicture( (char *)pict, pictNo );
	if ( stat != 0 ) { CB_Error(MemoryERR); return; }	// Memory error
}

void RclPictSmem( int pictNo){
	unsigned char *pict;
	int i;
	pict=(unsigned char *)LoadPicture( pictNo );
	if ( pict == NULL ) { CB_Error(MemoryERR); return; }	// Memory error
	WriteVram( pict+0x4C );
	free(pict);
}

unsigned char * GetheapPict(){
	unsigned char *pict;
	if ( PictbaseCount >= PictbaseCountMAX ) {
		if ( PictbasePtr < PictbaseMAX ) Pictbase[++PictbasePtr] = (unsigned char *) malloc( 1024 * PictbaseMAX +4 );
		else { CB_Error(NotEnoughMemoryERR); return NULL; }	// Not enough memory error
		if( Pictbase[PictbasePtr] == NULL ) { CB_Error(NotEnoughMemoryERR); return NULL; }	// Not enough memory error
		PictbaseCount=0;
	}
	pict = Pictbase[PictbasePtr] + 1024 * PictbaseCount;
	PictbaseCount++;
	return pict;
}

void StoPict( int pictNo){
	int i,stat;
	unsigned char *pict;
	
	if ( PictMode == 0 ) { StoPictSmem(pictNo); return; }	// strage memory mode
	
	if ( PictAry[pictNo] == NULL ) { //
		PictAry[pictNo] = GetheapPict() ;						// New Pict array ptr*
		if ( ErrorNo ) return;
	}
	pict = PictAry[pictNo];	//  heap mode
	ReadVram(pict);
}

void RclPict( int pictNo, int errorcheck){
	unsigned char *pict;
	unsigned char *pict2;
	int i;
	if ( PictMode == 0 ) {	// strage memory mode
		RclPictSmem(pictNo);
		if ( errorcheck ) return;
		ErrorNo=0;
		return;
	}
	
	pict = PictAry[pictNo];	//  heap mode
	if ( pict == NULL ) {	//  not exist : read smem
		pict2=(unsigned char *)LoadPicture( pictNo );
		if ( pict2 == NULL ) { CB_Error(MemoryERR);
			if ( errorcheck ) return;
			ErrorNo=0;
			return;
		}
		pict = GetheapPict() ;						// Pict array ptr*
		if ( pict != NULL ) {
			PictAry[pictNo] = pict;			//  heap mode pict
			memcpy(pict, pict2+0x4C, 1024 );
			WriteVram( pict );
			free(pict2);
			return;
		} else {
			WriteVram( pict2+0x4C );
			free(pict2);
			return ;
		}
	}
	WriteVram( pict );
}

void CB_StoPict( char *SRC ) { //	StoPict
	int n;
	if ( CB_RangeErrorCK_ChangeGraphicMode( SRC ) ) return;	// Select Graphic Mode
	n=CB_EvalInt( SRC );
	if ( (n<1) || (20<n) ){ CB_Error(ArgumentERR); return; }	// Argument error
	StoPict(n);
	Bdisp_PutDisp_DD_DrawBusy_skip_through( SRC );
}
void CB_RclPict( char *SRC ) { //	RclPict
	int n;
	if ( CB_RangeErrorCK_ChangeGraphicMode( SRC ) ) return;	// Select Graphic Mode
	n=CB_EvalInt( SRC );
	if ( (n<1) || (20<n) ){ CB_Error(ArgumentERR); return; }	// Argument error
	RclPict(n, 1);
	Bdisp_PutDisp_DD_DrawBusy_skip_through( SRC );
}

void CB_BG_None( char *SRC ) { //	BG_None
//	if ( CB_RangeErrorCK_ChangeGraphicMode( SRC ) ) return;	// Select Graphic Mode
	BG_Pict_No=0;
}
void CB_BG_Pict( char *SRC ) { //	BG_Pict
	int n;
	n=CB_EvalInt( SRC );
	if ( (n<1) || (20<n) ){ CB_Error(ArgumentERR); return; }	// Argument error
//	if ( CB_RangeErrorCK_ChangeGraphicMode( SRC ) ) return;	// Select Graphic Mode
	BG_Pict_No=n;
	if ( UseGraphic != 3 ) UseGraphic = 0;	// Cls or ViewWindow
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

void CB_Rect( char *SRC ) { // Rect x1,y1,x2,y2,mode 
	CB_RectSub( SRC , 0 );	// Rect
}

void CB_FillRect( char *SRC ) { // FillRect x1,y1,x2,y2,mode 
	CB_RectSub( SRC , 1 );	// FillRect
}

void CB_RectSub( char *SRC , int RectMode ) { // RectMode  0:Rect  1:RectFill
	int c;
	int px1,py1,px2,py2;
	int x,y;
	int mode;
	int style=S_L_Style;
	if ( tmp_Style >= 0 ) style=tmp_Style;
	if ( CB_RangeErrorCK_ChangeGraphicMode( SRC ) ) return;	// Select Graphic Mode
	CB_DotOprandRect( SRC, &px1, &py1);
	if ( SRC[ExecPtr] != ',' ) { CB_Error(SyntaxERR); return; }  // Syntax error
	ExecPtr++;
	CB_DotOprandRect( SRC, &px2, &py2);
	mode=CB_SetPointMode( SRC ) ;
	if ( ErrorNo ) return ; // error

	if ( px1 > px2 ) { c=px1; px1=px2; px2=c; }
	if ( py1 > py2 ) { c=py1; py1=py2; py2=c; }
	if ( ( px2 < 0 ) || ( px1 > 127 ) || ( py2 < 0 ) || ( py1 > 63 ) ) return ; 	// out of area
	if ( px1 < 0 ) px1=-1;
	if ( py1 < 0 ) py1=-1;
	if ( px2 > 127 ) px2=128;
	if ( py2 >  63 ) py2= 64;

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
	Bdisp_PutDisp_DD_DrawBusy_skip_through( SRC );
}


void CB_DotShape( char *SRC ) { // DotShape (x1,y1,x2,y2,typ,mode1,mode2,pattern1,pattern2)
	TShape Shape;
	int c;
	int px1,py1,px2,py2;
	int typ;
	int mode,mode1,mode2;
	int pat1,pat2;
	
	if ( CB_RangeErrorCK_ChangeGraphicMode( SRC ) ) return;	// Select Graphic Mode

	CB_DotOprandRect( SRC, &px1, &py1);
	if ( SRC[ExecPtr] != ',' ) { CB_Error(SyntaxERR); return; }  // Syntax error
	ExecPtr++;
	CB_DotOprandRect( SRC, &px2, &py2);
	if ( SRC[ExecPtr] != ',' ) { CB_Error(SyntaxERR); return; }  // Syntax error
	ExecPtr++;

	typ=CB_EvalInt( SRC );
	if ( SRC[ExecPtr] != ',' ) { CB_Error(SyntaxERR); return; }  // Syntax error
	ExecPtr++;

	CB_DotOprandRect( SRC, &mode1, &mode2);
	if ( SRC[ExecPtr] != ',' ) { CB_Error(SyntaxERR); return; }  // Syntax error
	ExecPtr++;
	
	CB_DotOprandRect( SRC, &pat1, &pat2);
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
	Bdisp_PutDisp_DD_DrawBusy_skip_through( SRC );
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
	
//	if ( RangeErrorCK( SRC ) ) return;
	CB_ChangeGraphicMode( SRC );	// Select Graphic Mode
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
		reg=RegVarAliasEx(SRC);
		if ( reg>=0 ) {
			if (px1>px2) { i=px1; px1=px2; px2=i; }
			if (py1>py2) { i=py1; py1=py2; py2=i; }

			box.left  =px1;
			box.right =px2;
			box.top   =py1;
			box.bottom=py2;
			
			ElementSize=ElementSizeSelect( SRC, &base, 0 );
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
	
//	if ( RangeErrorCK( SRC ) ) return;
	CB_ChangeGraphicMode( SRC );	// Select Graphic Mode
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
	if ( ( c == 'N' ) || ( c == 'n' ) )  Modify=IMB_WRITEMODIFY_NORMAL;	// Normal
	else
	if ( ( c == 'R' ) || ( c == 'r' ) )  Modify=IMB_WRITEMODIFY_REVERCE;	// Reverse
	else
	if ( ( c == 'M' ) || ( c == 'm' ) )  Modify=IMB_WRITEMODIFY_MESH;	// Mesh
	else { ExecPtr--; CB_Error(SyntaxERR); return; }  // Syntax error

	if ( SRC[ExecPtr] != ',' ) { c='N'; } else { ExecPtr++; c=SRC[ExecPtr++]; }
	if ( ( c == 'N' ) || ( c == 'n' ) )  Kind=IMB_WRITEKIND_OVER;	// Normal
	else
	if ( ( c == 'O' ) || ( c == 'o' ) )  Kind=IMB_WRITEKIND_OR;		// Or
	else
	if ( ( c == 'A' ) || ( c == 'a' ) )  Kind=IMB_WRITEKIND_AND;		// And
	else
	if ( ( c == 'X' ) || ( c == 'x' ) )  Kind=IMB_WRITEKIND_XOR;		// Xor
	else { ExecPtr--; CB_Error(SyntaxERR); return; }  // Syntax error

	Gpict.x =   px; 
	Gpict.y =   py; 
	Gpict.GraphData.width   = dx;
	Gpict.GraphData.height  = dy;
	Gpict.GraphData.pBitmap = Gptr;
	Gpict.WriteModify = Modify; 
	Gpict.WriteKind   = Kind;

	Bdisp_WriteGraph_VRAM(&Gpict);
	Bdisp_PutDisp_DD_DrawBusy_skip_through( SRC );
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
	
	if ( CB_RangeErrorCK_ChangeGraphicMode( SRC ) ) return;	// Select Graphic Mode
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
	
	if ( RangeErrorCK( SRC ) ) return;
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
//			dspflag=0;
			UseGraphic=0;
	} else {	//			-> VRAM
		CB_ChangeGraphicMode( SRC );	// Select Graphic Mode
		mode=CB_SetPointMode( SRC );
		i=x;
		for ( py=py1; py<=py2 ; py++) {	
			x=i;
			for ( px=px1; px<=px2 ; px++) 
				if ( ReadMatrixInt( reg, x++, y ) ) BdispSetPointVRAM2(px, py, mode);
					else							BdispSetPointVRAM2(px, py, 0);
			y++;
		}
		
		Bdisp_PutDisp_DD_DrawBusy_skip_through( SRC );
//		dspflag=0;
		UseGraphic=9;
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
		reg=RegVarAliasEx(SRC); if ( reg<0 ) { CB_Error(SyntaxERR); return; }	// Syntax error
		if ( MatAry[reg].SizeA == 0 ) { CB_Error(NoMatrixArrayERR); return; }	// No Matrix Array error
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
			if ( CB_RangeErrorCK_ChangeGraphicMode( SRC ) ) return;	// Select Graphic Mode
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
	if ( EvalEndCheck(SRC[ExecPtr]) ) { 
		tmp=CB_EvalInt( SRC );
		if ( tmp < 0 ) if ( CB_Round.MODE == Norm ) return CB_Round.DIGIT; else return -1;
		if ( ( tmp < 0 ) || ( tmp > 15 ) ) { CB_Error(SyntaxERR); return; }	// Syntax error
	} else tmp=1;
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


int CB_Disp( char *SRC ){		// Disp "A=",A
	char buffer[CB_StrBufferMax];
	int c;
	double value;
	
	CB_ChangeTextMode( SRC );	// Select Text Mode
	loop:
		if ( CursorX >1 ) Scrl_Y();
		
		CB_StrBufferCNT=0;			// String buffer clear
		c=CB_IsStr( SRC, ExecPtr );
		if ( c ) {	// string
			CB_GetLocateStr( SRC, buffer, CB_StrBufferMax-1 );		// String -> buffer	return 
		} else {	// expression
			value = CB_EvalDbl( SRC );
			sprintGR(buffer, value, 22-CursorX,RIGHT_ALIGN, CB_Round.MODE, CB_Round.DIGIT);
		}
		locate( CursorX, CursorY); Print((unsigned char*)buffer);
		CursorX=21;
		
	c=SRC[ExecPtr];
	if ( c == ',' ) { ExecPtr++; goto loop; }

	Bdisp_PutDisp_DD_DrawBusy_skip_through_text( SRC );

	return 0;
}

int CB_Disps( char *SRC , short dspflag ){	// Disps command
	char buffer[32];
	int c,reg;
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
	if ( dspflag >= 3 ) { 	// Matrix List display		Mat A  List 1
		CB_SelectTextVRAM();	// Select Text Screen
		CB_SaveTextVRAM();
		EditMatrix( MatdspNo, 1);	// Ans
		CB_RestoreTextVRAM();	// Resotre Graphic screen
		PrintDone();
		if ( scrmode  ) CB_SelectGraphVRAM();	// Select Graphic screen
		scrmode=ScreenMode;
		goto exitj;
	}
	if ( (scrmode) || ( UseGraphic == 3 ) ) {	// Graphic mode
		CB_SelectTextVRAM();	// Select Text Screen
		PrintDone();
	}
	
	if ( CursorX >1 ) Scrl_Y();
	locate( CursorX, CursorY); Print((unsigned char*)"             - Disp -");
	if ( ( scrmode ) && ( UseGraphic != 3 ) ) CB_SelectGraphVRAM();	// Select Graphic Screen	not ( clrgraph )
	if (UseGraphic>0x10) UseGraphic=UseGraphic&0xF; 
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
  exitj:
	if ( UseGraphic == 1 ) PlotXYtoPrevPXY(); // Plot
	if ( UseGraphic ) UseGraphic=(UseGraphic | 0x10);  // 
	Bdisp_PutDisp_DD_DrawBusy();
	return 0;
}

void CB_end( char *SRC ){
	char buffer[32];
	int c,t,reg;
	unsigned int key=0;
	int scrmode=ScreenMode;

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
	if ( dspflag >= 3 ) { 	// Matrix List display		Mat A  List 1
		CB_SaveTextVRAM();
		EditMatrix( MatdspNo, 1 );	// Ans
		CB_RestoreTextVRAM();	// Resotre Graphic screen
		CB_Done();
	}
//	if ( ( dspflag == 0 ) && ( BreakPtr == 0 ) ) CB_Done();
	if ( ( dspflag == 0 ) ) CB_Done();
	if ( scrmode ) CB_SelectGraphVRAM();	// Select Graphic Screen
	if ( UseGraphic ) if ( ( UseGraphic != 1 ) &&( UseGraphic != 2 ) && ( UseGraphic != 9 ) ) { UseGraphic=UseGraphic&0xF; CB_SelectTextVRAM(); }
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
		t=CB_TicksEnd-CB_TicksStart; if ( t<0 ) t+=11059200;
		sprintGRS(buffer, (double)(t)/128.0, 8,RIGHT_ALIGN, Fix, 2);  // Fix:2
		locate(  1, CursorY); Print((unsigned char*)"Execute Time=");
		locate( 14, CursorY); Print((unsigned char*)buffer);
		while ( 1 ) {
			key=GWait(EXIT_CANCEL_OFF);
			if ( key == KEY_CTRL_EXE ) break ;
			if ( key == KEY_CTRL_EXIT ) break ;
			if ( key == KEY_CTRL_AC  ) return ;
		}
	}
	if ( (UseGraphic&0xF) == 1 ) {	// Plot 
		PlotXYtoPrevPXY();
	}
}

//----------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------

void CB_FkeyMenu( char *SRC) {		// FkeyMenu(6,"ABC",R)
	char buffer[9];
	int c;
	int n;

	n=CB_EvalInt( SRC );
	if ( ( n<1 )||(n>6) ) { CB_Error(ArgumentERR); return; }	// Argumenterror
	c=SRC[ExecPtr];
	if ( c != ',' ) { CB_Error(SyntaxERR); return; }	// Syntax error
	ExecPtr++;
	CB_GetLocateStr( SRC, buffer, 8 );		// String -> buffer	return 
	c=SRC[ExecPtr];
	if ( c != ',' )	 {
//		if ( RangeErrorCK( SRC ) ) return;
//		CB_SelectGraphVRAM();	// Select Graphic Screen
		Fkey_dispN( n-1 ,buffer);	// Normal
	} else {
		c=SRC[++ExecPtr];
		if ( ( c=='T' ) || ( c=='t' ) ) {
			c=SRC[++ExecPtr];
			UseGraphic=0;
			CB_SelectTextVRAM();	// Select Text Screen
			c=SRC[++ExecPtr];
		} else {
//			if ( RangeErrorCK( SRC ) ) return;
//			CB_SelectGraphVRAM();	// Select Graphic Screen
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
	Bdisp_PutDisp_DD_DrawBusy_skip_through( SRC );
}

//----------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------

void CB_Menu( char *SRC, short *StackGotoAdrs) {		// Menu "title name","Branch name1",1,"Branch name2",2,"Branch name3",3,...
	int c,i,j,n;
	unsigned int key;
	char buffer[CB_StrBufferMax];
	char TitleName[18];
	char BranchName[10][18];
	int  Branch[10];
	int label;
	int ptr;

	int	cont=1;
	int scrl=-6;
	int y,cnt;
	int listmax;
	int select;
	
	c=CB_IsStr( SRC, ExecPtr );
	if ( c ) {	// string
		CB_GetLocateStr( SRC, buffer, CB_StrBufferMax-1 );		// String -> buffer	return 
	} else { CB_Error(SyntaxERR); return; }	// Syntax error
	for (i=0; i<16; i++ ) TitleName[i]=buffer[i];
	TitleName[i]='\0';

	c=SRC[ExecPtr];
	if ( c != ',' ) { CB_Error(SyntaxERR); return; }	// Syntax error
	ExecPtr++;

	n=0;
	while ( n<9 ) {
		CB_StrBufferCNT=0;			// String buffer clear
		c=CB_IsStr( SRC, ExecPtr );
		if ( c ) {	// string
			CB_GetLocateStr( SRC, buffer, CB_StrBufferMax-1 );		// String -> buffer	return 
		} else { CB_Error(SyntaxERR); return; }	// Syntax error
		for (i=0; i<16; i++ ) BranchName[n][i]=buffer[i];
		BranchName[n][i]='\0';
		c=SRC[ExecPtr];
		if ( c != ',' ) { CB_Error(SyntaxERR); return; }	// Syntax error
		ExecPtr++;
		Branch[n++]=SRC[ExecPtr++];
		c=SRC[ExecPtr];
		if ( c != ',' ) break;
		if ( n==9 )	 { CB_Error(SyntaxERR); return; }	// Syntax error
		ExecPtr++;
	}
	if ( n<2 ) { CB_Error(SyntaxERR); return; }	// Syntax error

	SaveDisp(SAVEDISP_PAGE1);		//

	listmax=n-1;
	select=0;
	scrl=0;
	KeyRecover();

	while ( cont ){
		ML_rectangle( 4, 1, 124, 63, 1, 1, 0);
		ML_line( 125,  2, 125, 63, 1);
		ML_line(   4, 12, 124, 12, 1);
		CB_PrintXY( 8, 4, (unsigned char*)TitleName, 0 );
		n=scrl+6; if (n>=listmax+1) n=listmax+1;
		y=0;
		for ( i=scrl; i<n; i++) {
			sprintf(buffer,"%d:%s               ", i+1, &BranchName[i][0]) ;
			buffer[18]='\0';
			CB_PrintXY( 8, y*8+14, (unsigned char*)buffer, 0 );
			y++;
		}
		if ( scrl > 0 ) PrintXY( 116, 14, (unsigned char*)"\xE6\x92", 0 );
		if ( listmax > scrl+5 ) PrintXY( 116, 54, (unsigned char*)"\xE6\x93", 0);
		y = select-scrl+1;
		Bdisp_AreaReverseVRAM( 8, y*8+6, 122, y*8+13);	// reverse select line 
		GetKey_DisableMenu(&key);
		switch (key) {
//			case KEY_CTRL_EXIT:
			case KEY_CTRL_AC:
				BreakPtr=ExecPtr;
			case KEY_CTRL_EXE:
				cont=0;
				break;
		
			case KEY_CTRL_UP:
				select-=1;
				if ( select < 0 ) {select=(listmax); scrl=select-5;}
				if ( select < scrl ) scrl-=1;
				if ( scrl < 0 ) scrl=0;
				break;
			case KEY_CTRL_DOWN:
				select+=1;
				if ( select > (listmax) ) {select=0; scrl=0;}
				if ((select - scrl) > 5 ) scrl+=1;
				if ( scrl > (listmax) ) scrl=(listmax)-5;
				break;
			default:
				break;
		}
	}

	RestoreDisp(SAVEDISP_PAGE1);	//
	
	if ( BreakPtr ) return ;
	
	c=Branch[select];
	if ( ( '0'<=c )&&( c<='9' ) ) {
		ExecPtr++;
		label = c-'0';
	} else if ( ( 'A'<=c )&&( c<='Z' ) ) {
		ExecPtr++;
		label = c-'A'+10;
	} else { CB_Error(SyntaxERR); return; }	// goto error
	
	ptr = StackGotoAdrs[label] ;
	if ( ptr == 0 ) {
		if ( Search_Lbl(SRC, c) == 0 ) { CB_Error(UndefinedLabelERR); return; }	// undefined label error
		ExecPtr++;
		StackGotoAdrs[label]=ExecPtr;
	} else  ExecPtr = ptr ;
}

//----------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------
void CB_DrawGraph(  char *SRC ){
	int reg,dimA,base;
	int i;
	if ( CB_RangeErrorCK_ChangeGraphicMode( SRC ) ) return;	// Select Graphic Mode
	reg=defaultGraphAry;
	if ( MatAry[reg].SizeA == 0 ) { CB_Error(MemoryERR); return; }	// Memory error
	base=MatAry[reg].Base;
	dimA=MatAry[reg].SizeA;
	for ( i=base; i<dimA+base; i++ ) {
		GraphY=MatrixPtr( reg, i, base );
		if ( GraphY[0] != 0 ) Graph_Draw();
	}
}

void CB_GraphY( char *SRC ){
	CB_Str( SRC );				// graph text print
	if ( CB_RangeErrorCK_ChangeGraphicMode( SRC ) ) return;	// Select Graphic Mode
	GraphY=CB_CurrentStr;
	Graph_Draw();
}

int CB_GraphXYEval( char *SRC ) {
	double result;
	int excptr=ExecPtr;
	int Ansreg=CB_MatListAnsreg;
	dspflag=0;
	result=CB_EvalDbl( SRC );
	if ( dspflag>=3 ) {
		CB_MatListAnsreg=Ansreg;
		ExecPtr=excptr; ListEvalsubTop(SRC);	// List calc
		if ( dspflag != 4 ) { CB_Error(ArgumentERR); return ; } // Argument error
		return CB_MatListAnsreg;	// List
	}
	return 0;
}

void CB_GraphXY( char *SRC ){	// GraphXY(X,Y)=( Xexp , Yexp )
	int listreg1,listreg2;
	int errflag=0;
	double regTback=regT;
	
	if ( CB_RangeErrorCK_ChangeGraphicMode( SRC ) ) return;	// Select Graphic Mode
	
	GraphX=SRC+ExecPtr;
	regT=TThetamin;
	if ( CB_MatListAnsreg >=28 ) CB_MatListAnsreg=28;
	listreg1=CB_GraphXYEval( SRC );
	regT=regTback;
    ErrorPtr= 0;
	errflag=ErrorNo;	// error cancel

	if ( SRC[ExecPtr] != ',' ) { CB_Error(SyntaxERR); return; }  // Syntax error
	ExecPtr++;

	GraphY=SRC+ExecPtr;
	regT=TThetamin;
	listreg2=CB_GraphXYEval( SRC );
	regT=regTback;
    ErrorPtr= 0;
	ErrorNo = 0;	// error cancel

	Graph_Draw_XY_List( listreg1, listreg2 );

	if ( SRC[ExecPtr] == ')' ) ExecPtr++;
	if ( CB_MatListAnsreg >=28 ) CB_MatListAnsreg=28;
	Bdisp_PutDisp_DD_DrawBusy_skip_through( SRC );
}
//----------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------
tdrawstat Sgraph[3];

void CB_S_Gph_init( int No ) {	// S-Gph1 DrawOff,Scatter,List 1,List 2,1,Square
	Sgraph[No].Draw=0;	// DrawOff
	Sgraph[No].GraphType=0;	// Scatter
	Sgraph[No].xList=1;
	Sgraph[No].yList=1;
	Sgraph[No].Freq=-1;
	Sgraph[No].MarkType=0;	// Square
}

int GetListNo( char *SRC ) {
	int c,d,reg=-1;
	c=SRC[ExecPtr];
	d=SRC[ExecPtr+1];
	if ( c==0x7F ) {
		if ( d==0x51 ) {	// List
			ExecPtr+=2;
			reg=ListRegVar( SRC );
		} else
		if ( (0x6A<=d) && (d<=0x6F) ) {
			ExecPtr+=2;
			reg=d+(32-0x6A);
		}
	}
	return reg;
}

void CB_S_Gph( char *SRC, int No ) {	// S-Gph1 DrawOn,xyLine,List 1,List 2,1,Square
	int c,d;
	int reg;
	c=SRC[ExecPtr];
	d=SRC[ExecPtr+1];
	if ( c==0xFFFFFFF7 ) {
		if ( d==0xFFFFFFCC ) {
			ExecPtr+=2;
			Sgraph[No].Draw=1;	// DrawOn
		} else
		if ( d==0xFFFFFFDC ) {
			ExecPtr+=2;
			Sgraph[No].Draw=0;	// DrawOff
		}
	}
	if ( SRC[ExecPtr] != ',' ) return;
	ExecPtr++;
	
	c=SRC[ExecPtr];
	d=SRC[ExecPtr+1];
	if ( c==0xFFFFFFF7 ) {
		if ( d==0x50 ) {
			ExecPtr+=2;
			Sgraph[No].GraphType=0;	// Scatter
		} else
		if ( d==0x51 ) {
			ExecPtr+=2;
			Sgraph[No].GraphType=1;	// xyLine
		}
	}
	if ( SRC[ExecPtr] != ',' ) return;
	ExecPtr++;
	
	reg = GetListNo( SRC );
	if ( reg<0 ) { CB_Error(SyntaxERR); return; }	// Syntax error
	Sgraph[No].xList = reg;
	if ( SRC[ExecPtr] != ',' ) return;
	ExecPtr++;
	
	reg = GetListNo( SRC );
	if ( reg<0 ) { CB_Error(SyntaxERR); return; }	// Syntax error
	Sgraph[No].yList = reg;
	if ( SRC[ExecPtr] != ',' ) return;
	ExecPtr++;
	
	c=SRC[ExecPtr];
	if ( c=='1' ) {
		ExecPtr++;
		Sgraph[No].Freq=-1;
	} else {
		reg = GetListNo( SRC );
		if ( reg<0 ) { CB_Error(SyntaxERR); return; }	// Syntax error
		Sgraph[No].Freq=reg;
	}
	if ( SRC[ExecPtr] != ',' ) return;
	ExecPtr++;

	c=SRC[ExecPtr];
	d=SRC[ExecPtr+1];
	if ( c==0xFFFFFFF7 ) {
		if ( d==0x4D ) {
			ExecPtr+=2;
			Sgraph[No].MarkType=0;	// Square
		} else
		if ( d==0x4E ) {
			ExecPtr+=2;
			Sgraph[No].MarkType=1;	// Cross
		} else
		if ( d==0x4F ) {
			ExecPtr+=2;
			Sgraph[No].MarkType=2;	// Dot
		}
	}
}

void CB_S_WindAuto( char *SRC ) {
	return;
}
void CB_S_WindMan( char *SRC ) {
	return;
}
void CB_DrawStat( char *SRC ) {
	if ( CB_RangeErrorCK_ChangeGraphicMode( SRC ) ) return;	// Select Graphic Mode
	DrawStat();
	Bdisp_PutDisp_DD_DrawBusy_skip_through( SRC );
}

//----------------------------------------------------------------------------------------------
void StoVwin( int n ) {
	memcpy( &VWIN[n][0], &REGv[0], 11*8 ) ;
	VWinflag[n] = 1;
}
void RclVwin( int n ) {
	if ( VWinflag[n] == 0 ) { CB_Error(MemoryERR); return; }	// Memory error
	memcpy( &REGv[0], &VWIN[n][0], 11*8 ) ;
}

void CB_StoVWin( char *SRC ) {
	int n=CB_EvalInt( SRC );
	if ( ( n<0 ) || ( n>5 ) ) { CB_Error(ArgumentERR); return ; } // Argument error
	StoVwin( n );
}
void CB_RclVWin( char *SRC ) {
	int n=CB_EvalInt( SRC );
	if ( ( n<0 ) || ( n>5 ) ) { CB_Error(ArgumentERR); return ; } // Argument error
	RclVwin( n );
	CB_ChangeViewWindow() ;
}

//----------------------------------------------------------------------------------------------
int GObjectAlign4d( unsigned int n ){ return n; }	// align +4byte
int GObjectAlign4e( unsigned int n ){ return n; }	// align +4byte
int GObjectAlign4f( unsigned int n ){ return n; }	// align +4byte
//int GObjectAlign4g( unsigned int n ){ return n; }	// align +4byte
//int GObjectAlign4h( unsigned int n ){ return n; }	// align +4byte
//int GObjectAlign4i( unsigned int n ){ return n; }	// align +4byte
//int GObjectAlign4j( unsigned int n ){ return n; }	// align +4byte
//----------------------------------------------------------------------------------------------
