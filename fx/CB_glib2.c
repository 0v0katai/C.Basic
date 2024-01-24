
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
	unsigned char buffer[32];
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
	unsigned char buffer[21];
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
						GCursorSetFlashMode(0);	// graphic cursor flashing off
						selectSetup=SetupG(selectSetup);
						break;
					case KEY_CTRL_F1:
						GCursorSetFlashMode(0);	// graphic cursor flashing off
						selectVar=SetVar(selectVar);		// A - 
						break;
					case KEY_CTRL_F3:
						GCursorSetFlashMode(0);	// graphic cursor flashing off
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
	unsigned char buffer[21];
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
	unsigned char buffer[21];
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
double Graph_Eval( unsigned char *SRC) {		// Eval Graph
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
		traceAry[i]=Graph_Eval((unsigned char *)GraphY);		// function
		if ( ErrorPtr ) return ;
		//-----------------------------
		if ( fabs(traceAry[i])*1e10<Ydot ) traceAry[i]=0;	// zero adjust
		if ( i==0 ) { Previous_X = regX; Previous_Y = traceAry[0]; }
		if ( ( 0<i ) && ( i<128 ) ) {
			PlotOn_VRAM( regX, traceAry[i]);
			Plot_X=regX;
			Plot_Y=regY;
			if ( DrawType == 0 ) {	// 1:Plot	// 0:connect
				Line( S_L_Default );
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

//----------------------------------------------------------------------------------------------
//		Setup
//----------------------------------------------------------------------------------------------

void FkeyS_L_(){
	int temp;
	temp=S_L_Style;
	Fkey_Clear( 0 );
	Fkey_dispN( 0, ""); Linesub(0*21+5,7*8+4,0*21+18,7*8+4, S_L_Normal);	// -----
	Fkey_Clear( 1 );
	Fkey_dispN( 1, ""); Linesub(1*21+5,7*8+4,1*21+18,7*8+4, S_L_Thick);	// =====
	Fkey_Clear( 2 );
	Fkey_dispN( 2, ""); Linesub(2*21+5,7*8+4,2*21+18,7*8+4, S_L_Broken);	// : : : :
	Fkey_Clear( 3 );
	Fkey_dispN( 3, ""); Linesub(3*21+5,7*8+4,3*21+18,7*8+4, S_L_Dot);	// . . . .
	Fkey_Clear( 4 );
	Fkey_Clear( 5 );
	S_L_Style=temp;
}

void SetLineStyle() {
	unsigned int key;
	
	FkeyS_L_();
	
	GetKey(&key);
	switch (key) {
		case KEY_CTRL_EXIT:
			break;
		case KEY_CTRL_F1:
			S_L_Style=S_L_Normal;
			break;
		case KEY_CTRL_F2:
			S_L_Style=S_L_Thick ;
			break;
		case KEY_CTRL_F3:
			S_L_Style=S_L_Broken;
			break;
		case KEY_CTRL_F4:
			S_L_Style=S_L_Dot;
			break;
		default:
		break;
	}
}

//-----------------------------------------------------------------------------

void SetVeiwWindowInit(){	// Initialize	retrun 0: no change  -1 : change
		Xmin  =-6.3;
		Xmax  = 6.3;
		Xscl= 1;
		Xdot  = 0.1;
		Ymin  =-3.1;
		Ymax  = 3.1;
		Yscl= 1;
		Ydot = (Ymax-Ymin)/ 62.0;
		TThetamin = 0;
		TThetamax = 6.2831853071796;
		TThetaptch= 0.062831853071796;
}
void SetVeiwWindowTrig(){	// trig Initialize
		Xmin  =-9.4247779607694;
		Xmax  = 9.4247779607694;
		Xscl= 1.5707963267949;
		Xdot  = 0.149599650170942;
		Ymin  =-1.6;
		Ymax  = 1.6;
		Yscl= 0.5;
		Ydot = (Ymax-Ymin)/ 62.0;
		TThetamin = 0;
		TThetamax = 6.2831853071796;
		TThetaptch= 0.062831853071796;
}
void SetVeiwWindowSTD(){	// STD Initialize
		Xmin  =-10;
		Xmax  = 10;
		Xscl= 1;
		Xdot  =  0.158730158730159;
		Ymin  =-10;
		Ymax  = 10;
		Yscl= 1;
		Ydot = (Ymax-Ymin)/ 62.0;
		TThetamin = 0;
		TThetamax = 6.2831853071796;
		TThetaptch= 0.062831853071796;
}


unsigned int MathKey( unsigned int  key) {
	switch ( key ) {
			case KEY_CHAR_0:
			case KEY_CHAR_1:
			case KEY_CHAR_2:
			case KEY_CHAR_3:
			case KEY_CHAR_4:
			case KEY_CHAR_5:
			case KEY_CHAR_6:
			case KEY_CHAR_7:
			case KEY_CHAR_8:
			case KEY_CHAR_9:
			case KEY_CHAR_DP:
			case KEY_CHAR_EXP:
			case KEY_CHAR_PLUS:
			case KEY_CHAR_MINUS:
			case KEY_CHAR_PMINUS:
			case KEY_CHAR_ANS:
			case KEY_CHAR_A:
			case KEY_CHAR_B:
			case KEY_CHAR_C:
			case KEY_CHAR_D:
			case KEY_CHAR_E:
			case KEY_CHAR_F:
			case KEY_CHAR_G:
			case KEY_CHAR_H:
			case KEY_CHAR_I:
			case KEY_CHAR_J:
			case KEY_CHAR_K:
			case KEY_CHAR_L:
			case KEY_CHAR_M:
			case KEY_CHAR_N:
			case KEY_CHAR_O:
			case KEY_CHAR_P:
			case KEY_CHAR_Q:
			case KEY_CHAR_R:
			case KEY_CHAR_S:
			case KEY_CHAR_T:
			case KEY_CHAR_U:
			case KEY_CHAR_V:
			case KEY_CHAR_W:
			case KEY_CHAR_X:
			case KEY_CHAR_Y:
			case KEY_CHAR_Z:
			case KEY_CTRL_XTT:
			case KEY_CHAR_FRAC:   //    0xbb
			case KEY_CHAR_LPAR:   //    0x28
			case KEY_CHAR_LOG:    //    0x95
			case KEY_CHAR_LN:     //    0x85
			case KEY_CHAR_SIN:    //    0x81
			case KEY_CHAR_COS:    //    0x82
			case KEY_CHAR_TAN:    //    0x83
			case KEY_CHAR_SQUARE: //    0x8b
			case KEY_CHAR_POW:    //    0xa8
			case KEY_CHAR_PI:     //    0xd0
			case KEY_CHAR_CUBEROOT: //  0x96
			case KEY_CHAR_EXPN10: //    0xb5
			case KEY_CHAR_EXPN:   //    0xa5
			case KEY_CHAR_ASIN:   //    0x91
			case KEY_CHAR_ACOS:   //    0x92
			case KEY_CHAR_ATAN:   //    0x93
			case KEY_CHAR_ROOT:   //    0x86
			case KEY_CHAR_POWROOT: //   0xb8
				if ( key == KEY_CTRL_XTT ) key='X';
//				if ( key == KEY_CHAR_PLUS  )  key='+';
//				if ( key == KEY_CHAR_MINUS )  key='-';
//				if ( key == KEY_CHAR_PMINUS ) key=0x87; // (-)
				return key;
			default:
				break;
		}
	return 0;
}


int SetViewWindow(void){		// ----------- Set  View Window variable	retrun 0: no change  -1 : change
	unsigned char buffer[22];
	unsigned int key;
	int	cont=1;
	int select=0;
	int scrl=0;
	int y;

	double	xmin      = Xmin      ;
	double	xmax      = Xmax      ;
	double	xscl      = Xscl      ;
	double	xdot      = Xdot      ;
	double	ymin      = Ymin      ;
	double	ymax      = Ymax      ;
	double	ydot      = Ydot      ;
	double	yscl      = Yscl      ;
	double	tThetamin = TThetamin ;
	double	tThetamax = TThetamax ;
	double	tThetaptch= TThetaptch;

	while (cont) {
		Bdisp_AllClr_VRAM();
		locate( 1,1);Print((unsigned char*)"View Window");

		if ( scrl <=0 ) {
			locate( 1, 2-scrl); Print((unsigned char*)"Xmin  :");
			sprintG(buffer,Xmin,  10,LEFT_ALIGN); locate( 8, 2-scrl); Print(buffer);
		}
		if ( scrl <=1 ) {
			locate( 1, 3-scrl); Print((unsigned char*)" max  :");
			sprintG(buffer,Xmax,  10,LEFT_ALIGN); locate( 8, 3-scrl); Print(buffer);
		}
		if ( scrl <=2 ) {
			locate( 1, 4-scrl); Print((unsigned char*)" scale:");
			sprintG(buffer,Xscl,10,LEFT_ALIGN); locate( 8, 4-scrl); Print(buffer);
		}
		if ( scrl <=3 ) {
			locate( 1, 5-scrl); Print((unsigned char*)" dot  :");
			sprintG(buffer,Xdot,  10,LEFT_ALIGN); locate( 8, 5-scrl); Print(buffer);
		}
		if ( scrl <=4 ) {
			locate( 1, 6-scrl); Print((unsigned char*)"Ymin  :");
			sprintG(buffer,Ymin,  10,LEFT_ALIGN); locate( 8, 6-scrl); Print(buffer);
		}
		if ( scrl <=5 ) {
			locate( 1, 7-scrl); Print((unsigned char*)" max  :");
			sprintG(buffer,Ymax,  10,LEFT_ALIGN); locate( 8, 7-scrl); Print(buffer);
		}
		if ( scrl >=1 ) {
			locate( 1, 8-scrl); Print((unsigned char*)" scale:");
			sprintG(buffer,Yscl,10,LEFT_ALIGN); locate( 8, 8-scrl); Print(buffer);
		}
		if ( scrl >=2 ) {
			locate( 1, 9-scrl); Print((unsigned char*)"Temin :");
			sprintG(buffer,TThetamin,  10,LEFT_ALIGN); locate( 8, 9-scrl); Print(buffer);
		}
		if ( scrl >=3 ) {
			locate( 1, 10-scrl); Print((unsigned char*)"  max :");
			sprintG(buffer,TThetamax,  10,LEFT_ALIGN); locate( 8, 10-scrl); Print(buffer);
		}
		if ( scrl >=4 ) {
			locate( 1, 11-scrl); Print((unsigned char*)"  ptch:");
			sprintG(buffer,TThetaptch,  10,LEFT_ALIGN); locate( 8, 11-scrl); Print(buffer);
		}

		y = select-scrl+1;
		Bdisp_AreaReverseVRAM(0, y*8, 127, y*8+7);	// reverse select line 
		
		Fkey_dispN(0,"Init");
		Fkey_dispN(1,"Trig");
		Fkey_dispN(2,"STD");

		Bdisp_PutDisp_DD();

		GetKey( &key );
		switch (key) {
			case KEY_CTRL_EXIT:
				return 0;	// no change
				break;
			case KEY_CTRL_EXE:
				cont=0;
				break;
		
			case KEY_CTRL_UP:
				select-=1;
				if ( select < 0 ) {select=9; scrl=4;}
				if ( select < scrl ) scrl-=1;
				if ( scrl < 0 ) scrl=0;
				break;
			case KEY_CTRL_DOWN:
				select+=1;
				if ( select > 9 ) {select=0; scrl=0;}
				if ((select - scrl) > 5 ) scrl+=1;
				if ( scrl > 4 ) scrl=4;
				break;
				
			case KEY_CTRL_F1:	// Initialize
				SetVeiwWindowInit();
				break;
			case KEY_CTRL_F2:	// trig Initialize
				SetVeiwWindowTrig();
				break;
			case KEY_CTRL_F3:	// STD Initialize
				SetVeiwWindowSTD();
				break;
			
			case KEY_CTRL_RIGHT:
				Bdisp_AreaReverseVRAM(0, y*8, 127, y*8+7);	// reverse select line 
				Fkey_Clear( 0 );
				Fkey_Clear( 1 );
				Fkey_Clear( 2 );
				y++;
				switch (select) {
					case 0: // Xmin
						Xmin      =InputNumD_full( 8, y, 14, Xmin);	// 
						Xdot = (Xmax-Xmin)/126.0;
						break;
					case 1: // Xmax
						Xmax      =InputNumD_full( 8, y, 14, Xmax);	// 
						Xdot = (Xmax-Xmin)/126.0;
						break;
					case 2: // Xscl
						Xscl      =InputNumD_full( 8, y, 14, Xscl);	// 
						break;
					case 3: // Xdot
						Xdot      =InputNumD_full( 8, y, 14, Xdot);	// 
						Xmax = Xmin + Xdot*126.;
						break;
					case 4: // Ymin
						Ymin      =InputNumD_full( 8, y, 14, Ymin);	// 
						Ydot = (Ymax-Ymin)/62.0;
						break;
					case 5: // Ymax
						Ymax      =InputNumD_full( 8, y, 14, Ymax);	// 
						Ydot = (Ymax-Ymin)/62.0;
						break;
					case 6: // Yscl
						Yscl      =InputNumD_full( 8, y, 14, Yscl);	// 
						break;
					case 7: // TThetamin
						TThetamin =InputNumD_full( 8, y, 14, TThetamin);	// 
						break;
					case 8: // TThetamax
						TThetamax =InputNumD_full( 8, y, 14, TThetamax);	// 
						break;
					case 9: // TThetaptch
						TThetaptch=InputNumD_full( 8, y, 14, TThetaptch);	// 
						break;
					default:
						break;
				}
				break;
			default:
				break;
		}
		key=MathKey( key );
		if ( key ) {
				Bdisp_AreaReverseVRAM(0, y*8, 127, y*8+7);	// reverse select line 
				Fkey_Clear( 0 );
				Fkey_Clear( 1 );
				Fkey_Clear( 2 );
				y++;
				switch (select) {
					case 0: // Xmin
						Xmin      =InputNumD_Char( 8, y, 14, Xmin, key);	// 
						Xdot = (Xmax-Xmin)/126.0;
						break;
					case 1: // Xmax
						Xmax      =InputNumD_Char( 8, y, 14, Xmax, key);	// 
						Xdot = (Xmax-Xmin)/126.0;
						break;
					case 2: // Xscl
						Xscl      =InputNumD_Char( 8, y, 14, Xscl, key);	// 
						break;
					case 3: // Xdot
						Xdot      =InputNumD_Char( 8, y, 14, Xdot, key);	// 
						Xmax = Xmin + Xdot*126.;
						break;
					case 4: // Ymin
						Ymin      =InputNumD_Char( 8, y, 14, Ymin, key);	// 
						Ydot = (Ymax-Ymin)/62.0;
						break;
					case 5: // Ymax
						Ymax      =InputNumD_Char( 8, y, 14, Ymax, key);	// 
						Ydot = (Ymax-Ymin)/62.0;
						break;
					case 6: // Yscl
						Yscl      =InputNumD_Char( 8, y, 14, Yscl, key);	// 
						break;
					case 7: // TThetamin
						TThetamin =InputNumD_Char( 8, y, 14, TThetamin, key);	// 
						break;
					case 8: // TThetamax
						TThetamax =InputNumD_Char( 8, y, 14, TThetamax, key);	// 
						break;
					case 9: // TThetaptch
						TThetaptch=InputNumD_Char( 8, y, 14, TThetaptch, key);	// 
						break;
					default:
						break;
				}
			}
	}

	if( ( xmin      != Xmin      ) ||
	    ( xmax      != Xmax      ) ||
	    ( xscl      != Xscl      ) ||
	    ( xdot      != Xdot      ) ||
	    ( ymin      != Ymin      ) ||
	    ( ymax      != Ymax      ) ||
	    ( ydot      != Ydot      ) ||
	    ( yscl      != Yscl      ) ||
	    ( tThetamin != TThetamin ) ||
	    ( tThetamax != TThetamax ) ||
	    ( tThetaptch!= TThetaptch) )  {
			ViewWindow( Xmin, Xmax, Xscl, Ymin, Ymax, Yscl);
			SaveConfig();
			return -1; // change value
	}
	return 0;	// no change
}

//-----------------------------------------------------------------------------
void SetFactor(){
	unsigned char buffer[22];
	unsigned int key;
	int	cont=1;
	int select=0;
	int y;

	PopUpWin(3);

	while (cont) {
		locate(3,3); Print((unsigned char *)"Factor");
		locate(3,4); Print((unsigned char *) "Xfact:           ");
		sprintG(buffer,Xfct,  10,LEFT_ALIGN); locate( 9, 4); Print(buffer);
		locate(3,5); Print((unsigned char *) "Yfact:           ");
		sprintG(buffer,Yfct,  10,LEFT_ALIGN); locate( 9, 5); Print(buffer);

		y = select + 3 ;
		Bdisp_AreaReverseVRAM(12, y*8, 113, y*8+7);	// reverse select line 
		Bdisp_PutDisp_DD();

		GetKey( &key );
		switch (key) {
			case KEY_CTRL_EXIT:
			case KEY_CTRL_EXE:
				cont=0;
				break;
		
			case KEY_CTRL_UP:
				select-=1;
				if ( select < 0 ) select=1;
				break;
			case KEY_CTRL_DOWN:
				select+=1;
				if ( select > 1 ) select=0;
				break;

			case KEY_CTRL_RIGHT:
				Bdisp_AreaReverseVRAM(12, y*8, 113, y*8+7);	// reverse select line 
				Bdisp_PutDisp_DD();
				y++;
				switch (select) {
					case 0: // Xfct
						Xfct  =InputNumD_full( 9, y, 10, Xfct);	// 
						select+=1;
						break;
					case 1: // Yfct
						Xmax  =InputNumD_full( 9, y, 10, Yfct);	// 
						break;
					default:
						break;
				}
				break;
			default:
				break;
		}
		key=MathKey( key );
		if ( key ) {
				Bdisp_AreaReverseVRAM(12, y*8, 113, y*8+7);	// reverse select line 
				Bdisp_PutDisp_DD();
				y++;
				switch (select) {
					case 0: // Xfct
						Xfct  =InputNumD_Char( 9, y, 10, Xfct, key);	// 
						select+=1;
						break;
					case 1: // Yfct
						Yfct  =InputNumD_Char( 9, y, 10, Yfct, key);	// 
						break;
					default:
						break;
				}
		}

	}
	SaveConfig();
}

//-----------------------------------------------------------------------------

int SetVar(int select){		// ----------- Set Variable
	unsigned char buffer[22];
	unsigned int key;
	int	cont=1;
	int scrl=0;
	int seltop=select;
	int i,y;
	int selectreplay=-1;
	int opNum=25;
	int small=0;
	int VarMode=CB_INT;	// 0:double  1:int

	if (select>25) { small=32; select-=32; }

	while (cont) {
		Bdisp_AllClr_VRAM();
		
		if (  select<seltop ) seltop = select;
		if ( (select-seltop) > 6 ) seltop = select-6;
		if ( (opNum -seltop) < 6 ) seltop = opNum -6; 
		for ( i=0; i<7; i++ ) {
			buffer[0]='A'+seltop+i+small;
			buffer[1]='=';
			buffer[2]='\0';
			locate(1,1+i); Print(buffer);
			if ( VarMode )
				sprintG(buffer, (double)REGINT[seltop+i+small], 19,LEFT_ALIGN);
			else
				sprintG(buffer, REG[seltop+i+small], 19,LEFT_ALIGN);
			locate(3,1+i); Print(buffer);
		}
		Fkey_dispN( 0, "A<>a");
		Fkey_dispN( 1, "DBL");
		Fkey_dispN( 2, "INT");

		locate(11,8); if ( VarMode ) Print((unsigned char*)"(int)"); else Print((unsigned char*)"(double)");

		y = (select-seltop) ;
		Bdisp_AreaReverseVRAM(0, y*8, 127, y*8+7);	// reverse select line 
		Bdisp_PutDisp_DD();

		GetKey( &key );
		switch (key) {
			case KEY_CTRL_EXIT:
			case KEY_CTRL_EXE:
				cont=0;
				break;
		
			case KEY_CTRL_F1:
				small=32-small;
				break;
			case KEY_CTRL_F2:
				VarMode=0;
				break;
			case KEY_CTRL_F3:
				VarMode=1;
				break;
			case KEY_CTRL_UP:
				select--;
				if ( select < 0 ) select = opNum;
				selectreplay = -1; // replay cancel
				break;
			case KEY_CTRL_DOWN:
				select++;
				if ( select > opNum ) select =0;
				selectreplay = -1; // replay cancel
				break;
				
			case KEY_CTRL_RIGHT:
				Bdisp_AreaReverseVRAM(0, y*8, 127, y*8+7);	// reverse select line 
				locate(1,8); PrintLine((unsigned char *)" ",21);
				locate(1,8); if ( VarMode ) Print((unsigned char*)"(int)"); else Print((unsigned char*)"(double)");
				y++;
				selectreplay = select; 
				if ( ( 0 <= select ) && ( select <=25 ) ) {	// regA to regZ
					if ( VarMode ) 
						REGINT[select+small]= InputNumD_full( 3, y, 19, (double)REGINT[select+small]);
					else
						REG[select+small]   = InputNumD_full( 3, y, 19, REG[select+small]);
				} else {
						selectreplay = -1; // replay cancel 
				}
				break;
				
			case KEY_CTRL_LEFT:
				if (selectreplay<0) break;
				Bdisp_AreaReverseVRAM(0, y*8, 127, y*8+7);	// reverse select line 
				locate(1,8); PrintLine((unsigned char *)" ",21);
				locate(1,8); if ( VarMode ) Print((unsigned char*)"(int)"); else Print((unsigned char*)"(double)");
				y++;
				selectreplay = select; 
				if ( ( 0 <= select ) && ( select <=25 ) ) {	// regA to regZ
					if ( VarMode ) 
						REGINT[select+small]= InputNumD_replay( 3, y, 19, (double)REGINT[select+small]);
					else
						REG[select+small]= InputNumD_replay( 3, y, 19, REG[select+small]);
				} else {
						selectreplay = -1; // replay cancel 
				}
				break;
			default:
				break;
		}
		key=MathKey( key );
		if ( key ) {
				Bdisp_AreaReverseVRAM(0, y*8, 127, y*8+7);	// reverse select line 
				locate(1,8); PrintLine((unsigned char *)" ",21);
				locate(1,8); if ( VarMode ) Print((unsigned char*)"(int)"); else Print((unsigned char*)"(double)");
				y++;
				selectreplay = select; 
				if ( ( 0 <= select ) && ( select <=25 ) ) {	// regA to regZ
					if ( VarMode ) 
						REGINT[select+small]= InputNumD_Char( 3, y, 19, (double)REGINT[select+small], key);
					else
						REG[select+small]   = InputNumD_Char( 3, y, 19, REG[select+small], key);
				} else {
						selectreplay = -1; // replay cancel 
				}
		}
	}
	SaveConfig();
	return select;
}

//-----------------------------------------------------------------------------
int SelectNum2( char*msg, int n ) {		// 
	unsigned int key;
	PopUpWin(3);
	locate( 3,3); Print((unsigned char *)"Select Number");
	locate( 6,5); Print((unsigned char *)msg);
	locate( 9,5); Print((unsigned char *)"[0~15]:");
	while (1) {
		n=InputNumD(17, 5, 2, n, ' ', REV_OFF, FLOAT_OFF, EXP_OFF);		// 0123456789
 		if ( (0<=n)&&(n<=15) ) break;
 		n=0;
 	}

	return n ; // ok
}

int SetupG(int select){		// ----------- Setup 
    char *onoff[]   ={"off","on"};
    char *draw[]    ={"Connect","Plot"};
    char *style[]   ={"Normal","Thick","Broken","Dot"};
    char *degrad[]  ={"Deg","Rad","Grad"};
    char *display[] ={"Nrm","Fix","Sci"};
    char *mode[]    ={"Double","Int"};
	unsigned char buffer[22];
	unsigned int key;
	int	cont=1;
	int scrl=select-6;
	int y;
	int listmax=11;
	
	if ( select > listmax ) select=0;
	if ( select < scrl ) scrl-=1;
	if ( scrl < 0 ) scrl=0;

	while (cont) {
		Bdisp_AllClr_VRAM();
		if ( scrl <=0 ) {
			locate( 1, 1-scrl); Print((unsigned char*)"Draw Type   :");
			locate(14, 1-scrl); Print((unsigned char*)draw[(int)DrawType]);
		}
		if ( scrl <=1 ) {
			locate( 1, 2-scrl); Print((unsigned char*)"Coord:      :");
			locate(14, 2-scrl); Print((unsigned char*)onoff[Coord]);
		}
		if ( scrl <=2 ) {
			locate( 1, 3-scrl); Print((unsigned char*)"Grid        :");
			locate(14, 3-scrl); Print((unsigned char*)onoff[Grid]);
		}
		if ( scrl <=3 ) {
			locate( 1, 4-scrl); Print((unsigned char*)"Axes        :");
			locate(14, 4-scrl); Print((unsigned char*)onoff[Axes]);
		}
		if ( scrl <=4 ) {
			locate( 1, 5-scrl); Print((unsigned char*)"Label       :");
			locate(14, 5-scrl); Print((unsigned char*)onoff[Label]);
		}
		if ( scrl <=5 ) {
			locate( 1, 6-scrl); Print((unsigned char*)"Derivative  :");
			locate(14, 6-scrl); Print((unsigned char*)onoff[Derivative]);
		}
		if ( scrl <=6 ) {
			locate( 1, 7-scrl); Print((unsigned char*)"Sketch Line :");
			locate(14, 7-scrl); Print((unsigned char*)style[S_L_Style]);
		}
		if ( ( scrl >=1 ) && ( 8-scrl > 0 ) ){
			locate( 1, 8-scrl); Print((unsigned char*)"Angle       :");
			locate(14, 8-scrl); Print((unsigned char*)degrad[Angle]);
		}
		if ( ( scrl >=2 ) && ( 9-scrl > 0 ) ){
			locate( 1, 9-scrl); Print((unsigned char*)"Display     :");
			locate(14, 9-scrl); Print((unsigned char*)display[CB_Round.MODE]);
			buffer[0]='\0';
			sprintf((char*)buffer,"%d",CB_Round.DIGIT);
			locate(17, 9-scrl); Print(buffer);
			locate(19, 9-scrl); 
			if (ENG) Print((unsigned char*)"/E");
		}
		if ( ( scrl >=3 ) && (10-scrl > 0 ) ){
			locate( 1,10-scrl); Print((unsigned char*)"Break Stop  :");
			locate(14,10-scrl); Print((unsigned char*)onoff[BreakCheck]);
		}
		if ( ( scrl >=4 ) && (11-scrl > 0 ) ){
			locate( 1,11-scrl); Print((unsigned char*)"TimeDsp     :");
			locate(14,11-scrl); Print((unsigned char*)onoff[TimeDsp]);
		}
		if ( ( scrl >=5 ) && (12-scrl > 0 ) ){
			locate( 1,12-scrl); Print((unsigned char*)"Execute mode:");
			locate(14,12-scrl); Print((unsigned char*)mode[CB_INT]);
		}

		y = select-scrl;
		Bdisp_AreaReverseVRAM(0, y*8, 127, y*8+7);	// reverse select line 
		switch (select) {
			case 0: // Draw Type
				Fkey_dispN( 0, "Con");
				Fkey_dispN( 1, "Plot");
				Fkey_Clear( 2 );
				Fkey_Clear( 3 );
				break;
			case 1: // Coord
			case 2: // Grid	
			case 3: // Axes
			case 4: // Label
			case 5: // Derivative
			case 9: // BreakCheck
			case 10: // TimeDsp
				Fkey_dispN( 0, " On ");
				Fkey_dispN( 1, " Off");
				Fkey_Clear( 2 );
				Fkey_Clear( 3 );
				break;
			case 6: // S_L_ Line	normal
				FkeyS_L_();
				break;
			case 7: // Angle
				Fkey_dispN( 0, "Deg ");
				Fkey_dispN( 1, "Rad ");
				Fkey_dispN( 2, "Grad");
				Fkey_Clear( 3 );
				break;
			case 8: // Display
				Fkey_dispR( 0, "Fix ");
				Fkey_dispR( 1, "Sci ");
				Fkey_dispR( 2, "Nrm ");
				Fkey_dispN( 3, "Eng ");
				break;
			case 11: // Display
				Fkey_dispN( 0, "DBL ");
				Fkey_dispN( 1, "Int ");
				break;
			default:
				break;
		}
		
		Bdisp_PutDisp_DD();

		GetKey( &key );
		switch (key) {
			case KEY_CTRL_EXIT:
			case KEY_CTRL_EXE:
				cont=0;
				break;
		
			case KEY_CTRL_UP:
				select-=1;
				if ( select < 0 ) {select=(listmax); scrl=select-6;}
				if ( select < scrl ) scrl-=1;
				if ( scrl < 0 ) scrl=0;
				break;
			case KEY_CTRL_DOWN:
				select+=1;
				if ( select > (listmax) ) {select=0; scrl=0;}
				if ((select - scrl) > 6 ) scrl+=1;
				if ( scrl > (listmax) ) scrl=(listmax)-6;
				break;
				
			case KEY_CTRL_F1:
				Bdisp_AreaReverseVRAM(0, y*8, 127, y*8+7);	// reverse select line 
				switch (select) {
					case 0: // Draw Type connect
						DrawType = 0 ;
						break;
					case 1: // Coord	on
						Coord = 1 ;
						break;
					case 2: // Grid		on
						Grid = 1 ;
						break;
					case 3: // Axes		on
						Axes = 1 ;
						break;
					case 4: // Label	on
						Label = 1 ;
						break;
					case 5: // 	Derivative on
						Derivative = 1 ;
						break;
					case 6: // Sketch Line	normal
						S_L_Style = 0 ;
						break;
					case 7: // Angle
						Angle = 0 ; // Deg
						break;
					case 8: // Display
						CB_Round.DIGIT=SelectNum2("Fix",CB_Round.DIGIT);
						CB_Round.MODE =Fix;
						break;
					case 9: // Break
						BreakCheck = 1 ; // on
						break;
					case 10: // TimeDsp
						TimeDsp = 1 ; // on
						break;
					case 11: // CB mode
						CB_INT = 0 ; // normal
						break;
					default:
						break;
				}
				break;
			case KEY_CTRL_F2:
				Bdisp_AreaReverseVRAM(0, y*8, 127, y*8+7);	// reverse select line 
				switch (select) {
					case 0: // Draw Type Plot
						DrawType = 1 ;
						break;
					case 1: // Coord	off
						Coord = 0 ;
						break;
					case 2: // Grid		off
						Grid = 0 ;
						break;
					case 3: // Axes		off
						Axes = 0 ;
						break;
					case 4: // Label	off
						Label = 0 ;
						break;
					case 5: // 	Derivative off
						Derivative = 0 ;
						break;
					case 6: // Sketch Line	Thick
						S_L_Style = 1 ; 
						break;
					case 7: // Angle
						Angle = 1 ; // Rad
						break;
					case 8: // Display
						CB_Round.DIGIT=SelectNum2("Sci",CB_Round.DIGIT);
						CB_Round.MODE =Sci;
						break;
					case 9: // Break
						BreakCheck = 0 ; // off
						break;
					case 10: // TimeDsp
						TimeDsp = 0 ; // off
						break;
					case 11: // CB mode
						CB_INT = 1 ; // int
						break;
					default:
						break;
				}
				break;
			case KEY_CTRL_F3:
				Bdisp_AreaReverseVRAM(0, y*8, 127, y*8+7);	// reverse select line 
				switch (select) {
					case 6: // Sketch Line	Broken
						S_L_Style = 2 ; 
						break;
					case 7: // Angle
						Angle = 2 ; // Grad
						break;
					case 8: // Display
						CB_Round.DIGIT=SelectNum2("Nrm",CB_Round.DIGIT);
						CB_Round.MODE =Norm;
						break;
					default:
						break;
				}
				break;
			case KEY_CTRL_F4:
				Bdisp_AreaReverseVRAM(0, y*8, 127, y*8+7);	// reverse select line 
				switch (select) {
					case 6: // Sketch  Line	Dot
						S_L_Style = 3 ; 
						break;
					case 8: // Display
						ENG=1-ENG;
						break;
					default:
						break;
				}
				break;
			default:
				break;
		}
	}
	SaveConfig();
	return select;
}
