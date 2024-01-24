
#include <ctype.h>
#include <fxlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <timer.h>
#include "fx_syscall.h"
#include "KeyScan.h"
#include "CB_io.h"
#include "CB_inp.h"
#include "CB_glib.h"
#include "CB_glib2.h"
#include "CB_Eval.h"
#include "CB_interpreter.h"
#include "CBI_interpreter.h"
#include "CB_file.h"
#include "CB_Edit.h"
#include "CB_error.h"
#include "CB_setup.h"

//------------------------------------------------------------------------------
int GCursorflag = 1;	// GCursor Pixel ON:1 OFF:0
int GCursorX;
int GCursorY;

void GCursorFlashing()		// timer IRQ handler
{
	if ( ( 0<GCursorX ) && ( GCursorX<128 ) && ( 0<GCursorY ) && ( GCursorY<64 ) ) {
		switch (GCursorflag) {
			case 0:
				Bdisp_SetPoint_DDVRAM(GCursorX, GCursorY,1);
				GCursorflag=1;
				break;
			case 1:
				Bdisp_SetPoint_DDVRAM(GCursorX, GCursorY,0);
				GCursorflag=0;
				break;
			default:
				break;
		}
	}
}

void GCursorSetFlashMode(int set)	// 1:on  0:off
{
	switch (set) {
		case 0:
			KillTimer(ID_USER_TIMER1);
			break;
		case 1:
			SetTimer(ID_USER_TIMER1, 250, &GCursorFlashing);
			GCursorflag=1;		// graphic cursor initialize
			break;
		default:
			break;
	}
}

void DrawGCSR( int x, int y )
{
	unsigned char PlotCsrDATA[]={ 0x1C,0x14,0x77,0x41,0x77,0x14,0x1C };
	unsigned char PlotCsrMASK[]={ 0x1C,0x1C,0x7F,0x7F,0x7F,0x1C,0x1C };

    DISPGRAPH GCSR; 
    
	if ( (0<x) && (x<128) && (0<y) && (y<64) ) {
		GCSR.x = x-4; 
		GCSR.y = y-3; 
		GCSR.GraphData.width =	8; if ( x>124) GCSR.GraphData.width = 8-(x-124);
		GCSR.GraphData.height = 7;
	
		GCSR.GraphData.pBitmap = PlotCsrMASK; 	// mask pattern
		GCSR.WriteModify = IMB_WRITEMODIFY_NORMAL; 
		GCSR.WriteKind = IMB_WRITEKIND_OR;
		Bdisp_WriteGraph_VRAM(&GCSR);
	
		GCSR.WriteKind = IMB_WRITEKIND_XOR;
		Bdisp_WriteGraph_VRAM(&GCSR);
	
		GCSR.GraphData.pBitmap = PlotCsrDATA; 	// cursor pattern
		GCSR.WriteModify = IMB_WRITEMODIFY_NORMAL; 
		GCSR.WriteKind = IMB_WRITEKIND_OR;
		Bdisp_WriteGraph_VRAM(&GCSR); 
	
		Bdisp_SetPoint_VRAM(x,y,1);				// center dot
	}
	GCursorflag=1;		// graphic cursor initialize
}

//--------------------------------------------------------------

int PictSelectNum2( char*msg ) {		// 
	char buffer[32];
	unsigned int key;
	int n;

	PopUpWin(4);
	locate( 3,2); Print((unsigned char *)msg);
	locate( 6,3); Print((unsigned char *)"Picture Memory");
	locate( 5,5); Print((unsigned char *)"Pict[1~20]:");

	buffer[0]='\0';
	while (1) {
		key= InputStrSub( 17, 5, 2, 0, buffer, 2, ' ', REV_OFF, FLOAT_OFF, EXP_OFF, ALPHA_OFF, HEX_OFF, PAL_ON, EXIT_CANCEL_OFF) ;
		if ( ( key == KEY_CTRL_EXIT ) || ( key != KEY_CTRL_EXE ) ) return -1;  // exit
		n=atof( (char*)buffer );
 		if ( (1<=n)&&(n<=20) ) break;
 		n=0;
 	}
	return n ; // ok
}

unsigned int Pict() {
	int cont=1;
	unsigned int key;
	int n;

	while ( cont ) {
		locate(1,8); PrintLine((unsigned char *)" ",21);
		Fkey_dispR( 0, "PICT");
		GetKey(&key);
		switch (key) {
			case KEY_CTRL_EXIT:
				key=0;
			case KEY_CTRL_AC:
			case KEY_CTRL_EXE:
				cont=0;
				break;
			case KEY_CTRL_F1:
				Fkey_dispR( 0, "STO");
				Fkey_dispR( 1, "RCL");
				GetKey(&key);
				switch (key) {
					case KEY_CTRL_AC:
					case KEY_CTRL_EXE:
						cont=0;
					case KEY_CTRL_EXIT:
						break;
					case KEY_CTRL_F1:
						n=PictSelectNum2( "Store In" );
						if (n>0) { 
							RestoreDisp(SAVEDISP_PAGE1);	// ------ RestoreDisp1
							StoPict(n);
							cont=0;
						}
						break;
					case KEY_CTRL_F2:
						n=PictSelectNum2( "Recall From" );
						if (n>0) { 
							RestoreDisp(SAVEDISP_PAGE1);	// ------ RestoreDisp1
							RclPict(n);
							SaveDisp(SAVEDISP_PAGE1);		// ------ SaveDisp1
							cont=0;
						}
						break;
					default:
						break;
				}
			case KEY_CTRL_SHIFT:
				cont=0;
				break;
			default:
				break;
		}
	}

	RestoreDisp(SAVEDISP_PAGE1);	// ------ RestoreDisp1
	return key;
}

//--------------------------------------------------------------
unsigned int Plot()
{
	int cont=1;
	char buffer[21];
	unsigned int key;
	int retcode=0;
	int px1,py1;
	
	long FirstCount;	// pointer to repeat time of first repeat
	long NextCount; 	// pointer to repeat time of second repeat

//	while( KeyCheckEXE() );
//	while( KeyCheckEXIT() );
//	while( KeyCheckAC() );
	KeyRecover();
	
	if ( VWtoPXY( Plot_X, Plot_Y, &GCursorX, &GCursorY) ) return;	// VW(X,Y) to  graphic cursor XY
	
	Bkey_Get_RepeatTime(&FirstCount,&NextCount);	// repeat time
	Bkey_Set_RepeatTime(FirstCount,2);				// set graphic cursor repeat time  (count * 25ms)

	SaveDisp(SAVEDISP_PAGE1);	// ------ SaveDisp1
	while ( cont ) {	
		GCursorSetFlashMode(1);	// graphic cursor flashing on
		RestoreDisp(SAVEDISP_PAGE1);	// ------ RestoreDisp1
		SaveDisp(SAVEDISP_PAGE1);		// ------ SaveDisp1
		PXYtoVW(GCursorX, GCursorY, &Plot_X, &Plot_Y);	// graphic cursor XY  to  VW(X,Y)
		if ( fabs(Plot_X*1e10)<Xdot ) Plot_X=0;	// zero adjust
		if ( fabs(Plot_Y*1e10)<Ydot ) Plot_Y=0;	// zero adjust
		if ( Coord ) {
			PrintMini(  0,58,(unsigned char*)"X=",MINI_OVER);
			sprintGRS(buffer, Plot_X, 13,LEFT_ALIGN, Norm,10); PrintMini(  8,58,(unsigned char*)buffer,MINI_OVER);
			PrintMini( 64,58,(unsigned char*)"Y=",MINI_OVER);
			sprintGRS(buffer, Plot_Y, 13,LEFT_ALIGN, Norm,10); PrintMini( 72,58,(unsigned char*)buffer,MINI_OVER);
		}
		DrawGCSR(GCursorX,GCursorY); 	// draw graphic cursor
//		Bdisp_PutDisp_DD();

		GetKey(&key);
		if ( key==KEY_CTRL_OPTN ) key=Pict();
		switch (key) {
			case KEY_CTRL_EXE:
			case KEY_CTRL_AC:
			case KEY_CTRL_EXIT:
				cont=0;
				break;
			case KEY_CTRL_F3:	// setViewWindow
				GCursorSetFlashMode(0);	// graphic cursor flashing off
				if ( SetViewWindow() ) cont=0;
				break;
			case KEY_CTRL_LEFT:
				if ( GCursorX >   1 ) GCursorX--;
				break;
			case KEY_CTRL_RIGHT:
				if ( GCursorX < 127 ) GCursorX++;
				break;
			case KEY_CTRL_UP:
				if ( GCursorY >   1 ) GCursorY--;
				break;
			case KEY_CTRL_DOWN:
				if ( GCursorY <  63 ) GCursorY++;
				break;
			case KEY_CTRL_SHIFT:
				locate(1,8); PrintLine((unsigned char *)" ",21);
				Fkey_dispR( 0, "Var");
				Fkey_dispR( 2, "V-W");
				Fkey_dispN( 5, "G<>T");
				GetKey(&key);
				switch (key) {
					case KEY_CTRL_EXIT:
						break;
					case KEY_CTRL_SETUP:
						selectSetup=SetupG(selectSetup);
						break;
					case KEY_CTRL_F1:
						selectVar=SetVar(selectVar);		// A - 
						break;
					case KEY_CTRL_F3:
						selectMatrix=SetMatrix(selectMatrix);		// 
						break;
					case KEY_CTRL_F6:
						cont=0;
						break;
					default:
					break;
				}
				break;
			default:
				break;
		}
	}
	GCursorSetFlashMode(0);	// graphic cursor flashing off
	Bkey_Set_RepeatTime(FirstCount,NextCount);	// restore repeat time
	RestoreDisp(SAVEDISP_PAGE1);	// ------ RestoreDisp1
//	if ( retcode==0 ) Bdisp_SetPoint_VRAM( GCursorX, GCursorY, 1);
	regX = Plot_X ;
	regY = Plot_Y ;
	regintX=regX; regintY=regY;
	return key ;
}

//----------------------------------------------------------------------------------------------
// Graph function
//----------------------------------------------------------------------------------------------
void FkeyZoom(){
	locate(1,8); PrintLine((unsigned char *)" ",21);
	Fkey_dispR( 1, "FACT");
	Fkey_dispN( 2, " IN");
	Fkey_dispN( 3, "OUT");
}
void FkeyGraph(){
	locate(1,8); PrintLine((unsigned char *)" ",21);
	Fkey_dispN( 0, "TRCE");
	Fkey_dispR( 1, "ZOOM");
	Fkey_dispR( 2, "V-W");
	Fkey_dispN( 5, "G<>T");
}
//--------------------------------------------------------------
unsigned int ZoomXY() {
	int cont=1;
	char buffer[21];
	unsigned int key;
	int retcode=0;
	
	long FirstCount;	// pointer to repeat time of first repeat
	long NextCount; 	// pointer to repeat time of second repeat

	regX=(Xmax+Xmin)/2; regY=(Ymax+Ymin)/2;		// center
	if ( VWtoPXY( regX, regY, &GCursorX, &GCursorY) ) return;	// VW(X,Y) to  graphic cursor XY
	
	Bkey_Get_RepeatTime(&FirstCount,&NextCount);	// repeat time
	Bkey_Set_RepeatTime(FirstCount,2);				// set graphic cursor repeat time  (count * 25ms)

	SaveDisp(SAVEDISP_PAGE1);	// ------ SaveDisp1
	GCursorSetFlashMode(1);	// graphic cursor flashing on
	while ( cont ) {	
		RestoreDisp(SAVEDISP_PAGE1);	// ------ RestoreDisp1
		SaveDisp(SAVEDISP_PAGE1);		// ------ SaveDisp1
		PXYtoVW(GCursorX, GCursorY, &regX, &regY);	// graphic cursor XY  to  VW(X,Y)
		if ( fabs(regX*1e10)<Xdot ) regX=0;	// zero adjust
		if ( fabs(regY*1e10)<Ydot ) regY=0;	// zero adjust
		if ( Coord ) {
			PrintMini(  0,58,(unsigned char*)"X=",MINI_OVER);
			sprintGRS(buffer, regX, 13,LEFT_ALIGN, Norm,10); PrintMini(  8,58,(unsigned char*)buffer,MINI_OVER);
			PrintMini( 64,58,(unsigned char*)"Y=",MINI_OVER);
			sprintGRS(buffer, regY, 13,LEFT_ALIGN, Norm,10); PrintMini( 72,58,(unsigned char*)buffer,MINI_OVER);
		}
		DrawGCSR(GCursorX,GCursorY); 	// draw graphic cursor
		Bdisp_PutDisp_DD();

		KeyRecover();
		GetKey(&key);
		switch (key) {
			case KEY_CTRL_AC:
			case KEY_CTRL_EXIT:
			case KEY_CTRL_EXE:
			case KEY_CTRL_F1:	// trace
			case KEY_CTRL_F6:	//
				cont=0;
				break;
			case KEY_CTRL_F3:	// setViewWindow
				if ( SetViewWindow() ) cont=0;
				break;
			case KEY_CTRL_LEFT:
				if ( GCursorX >   1 ) GCursorX--;
				break;
			case KEY_CTRL_RIGHT:
				if ( GCursorX < 127 ) GCursorX++;
				break;
			case KEY_CTRL_UP:
				if ( GCursorY >   1 ) GCursorY--;
				break;
			case KEY_CTRL_DOWN:
				if ( GCursorY <  63 ) GCursorY++;
				break;
			default:
				break;
		}
	}
	GCursorSetFlashMode(0);	// graphic cursor flashing off
	Bkey_Set_RepeatTime(FirstCount,NextCount);	// restore repeat time
	RestoreDisp(SAVEDISP_PAGE1);	// ------ RestoreDisp1
	regintX=regX; regintY=regY;
	return key ;
}
//--------------------------------------------------------------
unsigned int Zoom_sub(unsigned int key){
	double x,y;

	FkeyZoom();

	if (key==0) GetKey(&key);
	switch (key) {
		case KEY_CTRL_AC:
		case KEY_CTRL_EXIT:
		case KEY_CTRL_F6:
			break;
		case KEY_CTRL_F2:
			SetFactor();
			break;
		case KEY_CTRL_F3:
			RestoreDisp(SAVEDISP_PAGE1);	// ------ RestoreDisp1
			key=ZoomXY();
			if ( key == KEY_CTRL_EXIT ) return key ;		// select zoom center 
			if ( key == KEY_CTRL_F1   ) return key ;		// trace
			ZoomIn();
			break;
		case KEY_CTRL_F4:
			RestoreDisp(SAVEDISP_PAGE1);	// ------ RestoreDisp1
			key=ZoomXY();
			if ( key == KEY_CTRL_EXIT ) return key ;		// select zoom center 
			if ( key == KEY_CTRL_F1   ) return key ;		// trace
			ZoomOut();
			break;
		default:
		break;
	}
	return key; 
}

//----------------------------------------------------------------------------------------------
unsigned int Trace(int *index ) {
	int cont=1;
	char buffer[21];
	unsigned int key;
	double dydx;
	
	long FirstCount;	// pointer to repeat time of first repeat
	long NextCount; 	// pointer to repeat time of second repeat

	if ( *index <   1 ) *index=  0;
	if ( *index > 127 ) *index=128;
	PXYtoVW(*index, 0, &regX, &regY);	// graphic cursor X  to  VW(X,dummy)
	VWtoPXY( regX, traceAry[*index], &GCursorX, &GCursorY);	// VW(X,Y) to  graphic cursor XY
	
	Bkey_Get_RepeatTime(&FirstCount,&NextCount);	// repeat time
	Bkey_Set_RepeatTime(FirstCount,2);				// set graphic cursor repeat time  (count * 25ms)

	SaveDisp(SAVEDISP_PAGE1);	// ------ SaveDisp1
	GCursorSetFlashMode(1);	// graphic cursor flashing on
	while ( cont ) {	
		RestoreDisp(SAVEDISP_PAGE1);	// ------ RestoreDisp1
		SaveDisp(SAVEDISP_PAGE1);		// ------ SaveDisp1
		PXYtoVW(GCursorX, 0, &regX, &regY);	// graphic cursor X  to  VW(X,dummy)
		if ( fabs(regX)*1e10<Xdot ) regX=0;	// zero adjust
		VWtoPXY( regX, traceAry[GCursorX], &GCursorX, &GCursorY);	// VW(X,Y) to  graphic cursor XY
		if ( Coord ) {
			sprintf((char*)buffer, "PX=%d", GCursorX);	PrintMini(  0,0,(unsigned char*)buffer,MINI_OVER);
			sprintf((char*)buffer, "PY=%d", GCursorY);	PrintMini( 64,0,(unsigned char*)buffer,MINI_OVER);
			PrintMini(  0,58,(unsigned char*)"X=",MINI_OVER);
			sprintGRS(buffer, regX,             13,LEFT_ALIGN, Norm,10); PrintMini(  8,58,(unsigned char*)buffer,MINI_OVER);
			PrintMini( 64,58,(unsigned char*)"Y=",MINI_OVER);
			sprintGRS(buffer,traceAry[GCursorX],13,LEFT_ALIGN, Norm,10); PrintMini( 72,58,(unsigned char*)buffer,MINI_OVER);
		}
		if ( Derivative ) {
			PrintMini( 64,50,(unsigned char*)"dY/dX=",MINI_OVER);
			dydx = (traceAry[GCursorX+1]-traceAry[GCursorX-1]) / (Xdot*2);
			if ( fabs(dydx)<1.0e-13 ) dydx=0;	// zero adjust
			sprintGRS(buffer, dydx, 6,LEFT_ALIGN, Norm,5); PrintMini( 88,50,(unsigned char*)buffer,MINI_OVER);
		}
		DrawGCSR(GCursorX,GCursorY); 	// draw graphic cursor
		Bdisp_PutDisp_DD();

		KeyRecover();
		GetKey(&key);
		if ( key==KEY_CTRL_OPTN ) key=Pict();
		switch (key) {
			case KEY_CTRL_AC:
			case KEY_CTRL_F1:
			case KEY_CTRL_EXE:
			case KEY_CTRL_EXIT:
				cont=0;
				break;
			case KEY_CTRL_F2:
				FkeyZoom();
				GetKey(&key);
				switch (key) {
					case KEY_CTRL_EXIT:
					case KEY_CTRL_F1:
					case KEY_CTRL_F2:
					case KEY_CTRL_F3:
					case KEY_CTRL_F4:
					case KEY_CTRL_F6:
						cont=0;
						break;
					default:
						break;
				}
				break;
			case KEY_CTRL_F3:	// setViewWindow
				key=KEY_CHAR_3;
				cont=0;
				break;
			case KEY_CTRL_LEFT:
				GCursorX--;
				if ( GCursorX <   1 )  cont=0;
				break;
			case KEY_CTRL_RIGHT:
				GCursorX++;
				if ( GCursorX > 127 )  cont=0;
				break;
			case KEY_CTRL_SHIFT:
				FkeyGraph();
				GetKey(&key);
				switch (key) {
					case KEY_CTRL_F3:
						key=KEY_CHAR_3;
					case KEY_CTRL_EXIT:
					case KEY_CTRL_F2:
					case KEY_CTRL_F4:
					case KEY_CTRL_F6:
						cont=0;
						break;
					default:
						break;
				}
				break;
			default:
				break;
		}
	}
	GCursorSetFlashMode(0);	// graphic cursor flashing off
	Bkey_Set_RepeatTime(FirstCount,NextCount);	// restore repeat time
	RestoreDisp(SAVEDISP_PAGE1);	// ------ RestoreDisp1
	*index=GCursorX;
	regintX=regX; regintY=regY;
	return key ;
}

//----------------------------------------------------------------------------------------------
double Graph_Eval( char *SRC) {		// Eval Graph
	double result;
	int execptr=ExecPtr;
    ExecPtr = 0;
    ErrorPtr= 0;
	ErrorNo = 0;
    result = EvalsubTop( SRC );
    ExecPtr=execptr;
	return result;
}

void Graph_Draw(){
	int i;
	GraphAxesGrid( Xmin, Xmax, Xscl, Ymin, Ymax, Yscl);
	regX   = Xmin-Xdot;
	for ( i=0; i<=128; i++) {
		//-----------------------------
		traceAry[i]=Graph_Eval(GraphY);		// function
		if ( ErrorPtr ) return ;
		//-----------------------------
		if ( fabs(traceAry[i])*1e10<Ydot ) traceAry[i]=0;	// zero adjust
		if ( i==0 ) { Previous_X = regX; Previous_Y = traceAry[0]; }
		if ( ( 0<i ) && ( i<128 ) ) {
			PlotOn_VRAM( regX, traceAry[i]);
			Plot_X=regX;
			Plot_Y=regY;
			if ( DrawType == 0 ) {	// 1:Plot	// 0:connect
				Line( S_L_Default , 1);
			}
			Bdisp_PutDisp_DD_DrawBusy_skip();
		}
		regX += Xdot;
	}
	regintX=regX; regintY=regY;
	SaveDisp(SAVEDISP_PAGE1);	// ------ SaveDisp1
}
void Graph_reDraw(){
	int i;
	ViewWindow( Xmin, Xmax, Xscl, Ymin, Ymax, Yscl);
	Bdisp_AllClr_VRAM();			// ------ Clear VRAM 
	Graph_Draw();
}
//--------------------------------------------------------------
unsigned int Graph_trace_sub(int *tracex){
	unsigned int key;
	int tx,ty; // dummy
	tx=*tracex;
	while (1) {
		if ( tx <   1 ) tx=  0;
		if ( tx > 127 ) tx=128;
		key=Trace(&tx);			// trace
		if ( key==KEY_CTRL_EXIT) break; //exit
		if ( key==KEY_CTRL_EXE ) break; //exe
		if ( key==KEY_CTRL_AC  ) break; // AC
		if ( key==KEY_CTRL_F6  ) break; // F6
		if ( key==KEY_CTRL_F1  ) break; // F1
		if ( key==KEY_CTRL_F2  ) {
			key=Zoom_sub(KEY_CTRL_F2); // F2  Zoom fact
			Graph_reDraw();
			break;
		}
		if ( key==KEY_CTRL_F3 ) { // Zoom in
			key=Zoom_sub(KEY_CTRL_F3); // F3  Zoom in
			if ( key==KEY_CTRL_EXIT) break; //exit
//			if ( key==KEY_CTRL_EXE ) break; //exe
			if ( key==KEY_CTRL_AC  ) break; // AC
			if ( key==KEY_CTRL_F6  ) break; // F6
//			if ( key!=KEY_CTRL_F1  ) {
//				Graph_reDraw();
//				break;
//			}
			Graph_reDraw();
		}
		if ( key==KEY_CTRL_F4 ) { // Zoom out
			key=Zoom_sub(KEY_CTRL_F4); // F4  Zoom out
			if ( key==KEY_CTRL_EXIT) break; //exit
//			if ( key==KEY_CTRL_EXE ) break; //exe
			if ( key==KEY_CTRL_AC  ) break; // AC
			if ( key==KEY_CTRL_F6  ) break; // F6
//			if ( key!=KEY_CTRL_F1  ) {
//				Graph_reDraw();
//				break;
//			}
			Graph_reDraw();
		}
		if ( key==KEY_CHAR_3 ) { // SetViewWindow
			SetViewWindow();
			Graph_reDraw();
			break;
		}
		if ( key==KEY_CTRL_LEFT ) { 
			Xmin-=Xdot*8;
			Xmax-=Xdot*8;
			Graph_reDraw();
			VWtoPXY(Xmin+Xdot*7, 0, &tx, &ty);
		}
		if ( key==KEY_CTRL_RIGHT ) { 
			Xmin+=Xdot*8;
			Xmax+=Xdot*8;
			Graph_reDraw();
			VWtoPXY(Xmax-Xdot*7, 0, &tx, &ty);
		}
	}
	*tracex=tx;
	regintX=regX; regintY=regY;
	return key;
}

//--------------------------------------------------------------
//--------------------------------------------------------------
unsigned int Graph_main(){
	char buffer[21];
	unsigned int key;
	int cont=1;
	int tx=64;	// trace center 
	
	Graph_Draw();
	
//	while( KeyCheckEXE() );
//	while( KeyCheckEXIT() );
//	while( KeyCheckAC() );
	KeyRecover();

	while (cont) {
		SaveDisp(SAVEDISP_PAGE1);		// ------ SaveDisp1
		if ( key==KEY_CTRL_OPTN ) key=Pict();
		GetKey(&key);
		switch (key) {
			case KEY_CTRL_AC:
			case KEY_CTRL_EXE:
			case KEY_CTRL_EXIT:
			case KEY_CTRL_F6:
				cont=0;
				break;
			case KEY_CTRL_LEFT:
				Xmin-=Xdot*12;
				Xmax-=Xdot*12;
				Graph_reDraw();
				break;
			case KEY_CTRL_RIGHT:
				Xmin+=Xdot*12;
				Xmax+=Xdot*12;
				Graph_reDraw();
				break;
			case KEY_CTRL_UP:
				Ymin+=Ydot*12;
				Ymax+=Ydot*12;
				Graph_reDraw();
				break;
			case KEY_CTRL_DOWN:
				Ymin-=Ydot*12;
				Ymax-=Ydot*12;
				Graph_reDraw();
				break;
			case KEY_CTRL_SHIFT:
				FkeyGraph();
				GetKey(&key);
				switch (key) {
					case KEY_CTRL_EXIT:
					case KEY_CTRL_F6:
						cont=0;
						break;
					case KEY_CTRL_SETUP:
						selectSetup=SetupG(selectSetup);
						Graph_reDraw();
						break;
					case KEY_CTRL_F1:
						RestoreDisp(SAVEDISP_PAGE1);	// ------ RestoreDisp1
						key=Graph_trace_sub(&tx);	// trace
						if ( key==KEY_CTRL_EXIT ) cont=0; //exit
						if ( key==KEY_CTRL_EXE  ) cont=0; //exe
						if ( key==KEY_CTRL_AC   ) cont=0; // AC
						if ( key==KEY_CTRL_F6   ) cont=0; // F6
						break;
					case KEY_CTRL_F2:
						RestoreDisp(SAVEDISP_PAGE1);	// ------ RestoreDisp1
						key=Zoom_sub(0);	// zoom
						if ( key==KEY_CTRL_F1   ) key=Graph_trace_sub(&tx);	// trace
						if ( key==KEY_CTRL_EXIT ) cont=0; //exit
						if ( key==KEY_CTRL_AC   ) cont=0; // AC
						if ( key==KEY_CTRL_F6   ) cont=0; // F6
						Graph_reDraw();
						break;
					case KEY_CTRL_F3:
						SetViewWindow();
						Graph_reDraw();
						break;
					default:
					break;
				}
				RestoreDisp(SAVEDISP_PAGE1);	// ------ RestoreDisp1
				break;
			case KEY_CTRL_F1:
				RestoreDisp(SAVEDISP_PAGE1);	// ------ RestoreDisp1
				key=Graph_trace_sub(&tx);	// trace
				if ( key==KEY_CTRL_EXIT ) cont=0; //exit
				if ( key==KEY_CTRL_EXE  ) cont=0; //exe
				if ( key==KEY_CTRL_AC   ) cont=0; // AC
				if ( key==KEY_CTRL_F6   ) cont=0; // F6
				break;
			case KEY_CTRL_F2:
				RestoreDisp(SAVEDISP_PAGE1);	// ------ RestoreDisp1
				key=Zoom_sub(0);	// zoom
				if ( key==KEY_CTRL_F1   ) key=Graph_trace_sub(&tx);	// trace
				if ( key==KEY_CTRL_EXIT ) cont=0; //exit
				if ( key==KEY_CTRL_AC   ) cont=0; // AC
				if ( key==KEY_CTRL_F6   ) cont=0; // F6
				Graph_reDraw();
				break;
			case KEY_CTRL_F3:
				SetViewWindow();
				Graph_reDraw();
				break;
			default:
				break;
		}
		
		Bdisp_PutDisp_DD();
	}
	return key;
}

