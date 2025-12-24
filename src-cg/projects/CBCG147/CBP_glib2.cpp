extern "C" {

#include "prizm.h"
#include "CBP.h"

//------------------------------------------------------------------------------
int GCursorflag = 1;	// GCursor Pixel ON:1 OFF:0
int GCursorX;
int GCursorY;

void GCursorX1( int x, int y, int color ){
	Bdisp_SetPoint_DD( x  , y+23, color );
	Bdisp_SetPoint_DD( x-1, y+24, color );
	Bdisp_SetPoint_DD( x  , y+24, color );
	Bdisp_SetPoint_DD( x+1, y+24, color );
	Bdisp_SetPoint_DD( x  , y+25, color );
}
void GCursorX3( int x, int y, int color ){
	int i,j;
	for ( i=y+24; i<=y+24+2; i++ ) {
		for ( j=x; j<=x+2; j++ ) {
			Bdisp_SetPoint_DD( j, i, color );
		}
	}
}

void GCursorFlashing(){		// timer IRQ handler
	int color;	//
	if ( PlotColor >= 0 ) color=PlotColor; else color=CB_ColorIndexPlot;
	if ( ( 0<GCursorX ) && ( GCursorX<=ScreenWidth ) && ( 0<GCursorY ) && ( GCursorY<=ScreenHeight ) ) {
		switch (GCursorflag) {
			case 0:
				if ( CB_G1MorG3M==1 )	{
					GCursorX3( GCursorX*3, GCursorY*3, color);	// FX
				} else {
					if ( Plot_Style==S_L_Thin ) GCursorX1( GCursorX+2, GCursorY+2, color );	// CG 1dot
					else						GCursorX3( GCursorX+1, GCursorY+1, color );	// CG 3x3dot
				}
				GCursorflag=1;
				break;
			case 1:
//				Bdisp_PutDisp_DD_stripe( (GCursorY-4)*3+24, (GCursorY+4)*3+24);
				color=CB_BackColorIndex;
				if ( CB_G1MorG3M==1 )	{
					GCursorX3( GCursorX*3, GCursorY*3, color);	// FX
				} else {
					if ( Plot_Style==S_L_Thin ) GCursorX1( GCursorX+2, GCursorY+2, color );	// CG 1dot
					else						GCursorX3( GCursorX+1, GCursorY+1, color );	// CG 3x3dot
				}
				GCursorflag=0;
				break;
			default:
				break;
		}
	}
}

void GCursorSetFlashMode(int set){	// 1:on  0:off
	switch (set) {
		case 0:
			if ( timer_id ) KillTimer(timer_id);
			timer_id=0;
			break;
		case 1:
			if ( timer_id ) KillTimer(timer_id);
			timer_id=SetTimer(timer_id0, 400, &(GCursorFlashing));
			GCursorflag=1;		// graphic cursor initialize
			break;
		default:
			break;
	}
}

void DrawGCSR( int x, int y ){
	unsigned char PlotCsrDATA[]={ 0x1C,0x14,0x77,0x41,0x77,0x14,0x1C };
	unsigned char PlotCsrMASK[]={ 0x1C,0x1C,0x7F,0x7F,0x7F,0x1C,0x1C };
    DISPGRAPH GCSR; 
    int bk_CB_ColorIndex=CB_ColorIndex;
//  int color=0xF81F;	// Magenta;
//  int color=0xF81F;	// Magenta;
    
	if ( (MatBase<=x) && (x<=ScreenWidth) && (MatBase<=y) && (y<=ScreenHeight) ) {
		GCSR.GraphData.height = 7;
		GCSR.WriteModify = IMB_WRITEMODIFY_NORMAL; 
		GCSR.WriteKind = IMB_WRITEKIND_OR;
		if ( CB_G1MorG3M==1 )	{
			GCSR.x = x-4; 
			GCSR.y = y-3; 
			GCSR.GraphData.width = 8; // if ( x>(ScreenWidth-3)) GCSR.GraphData.width = 8-(x-(ScreenWidth-3));
			CB_ColorIndex=CB_BackColorIndex;
			GCSR.GraphData.pBitmap = PlotCsrMASK; 	// mask pattern
			Bdisp_WriteGraph_VRAM_fx(&GCSR);
	
			CB_ColorIndex=PlotColor;
			GCSR.GraphData.pBitmap = PlotCsrDATA; 	// cursor pattern
			Bdisp_WriteGraph_VRAM_fx(&GCSR); 
			BdispSetPointVRAMx9( x*3, y*3, 1);	// FX mode
		} else {
			GCSR.x = x-4*CB_G1MorG3M+1; 
			GCSR.y = y-3*CB_G1MorG3M+1; 
			GCSR.GraphData.width = 8; // if ( x>(ScreenWidth-3*3+1)) GCSR.GraphData.width = 8-(x-(ScreenWidth-3*3+1)/3);
			CB_ColorIndex=CB_BackColorIndex;
			GCSR.GraphData.pBitmap = PlotCsrMASK; 	// mask pattern
			Bdisp_WriteGraph_VRAM_fx_xy1(&GCSR); 

			CB_ColorIndex=PlotColor;
			GCSR.GraphData.pBitmap = PlotCsrDATA; 	// cursor pattern
			Bdisp_WriteGraph_VRAM_fx_xy1(&GCSR); 
//			CB_SetPixelPointVRAM2( x, y, 1); 	// FX or CG
			if ( Plot_Style==S_L_Thin ) BdispSetPointVRAM( x+2, y+2, 1);
			else	BdispSetPointVRAMx9( x+1, y+1, 1);
		}
	}
	GCursorflag=1;		// graphic cursor initialize
	CB_ColorIndex=bk_CB_ColorIndex;
}

//--------------------------------------------------------------

int PictSelectNum2( char*msg ) {		// 
	char buffer[32];
	int key;
	int n;

	PopUpWin(4);
	locate( 3,2); Prints((unsigned char *)msg);
	locate( 6,3); Prints((unsigned char *)"Picture Memory");
	locate( 5,5); Prints((unsigned char *)"Pict[1~20]:");

	buffer[0]='\0';
	while (1) {
		key= InputStrSub( 17, 5, 2, 0, buffer, 2, " ", REV_OFF, FLOAT_OFF, EXP_OFF, ALPHA_OFF, HEX_OFF, PAL_ON, EXIT_CANCEL_OFF, AC_CANCEL_OFF ) ;
		if ( ( key == KEY_CTRL_EXIT ) || ( key != KEY_CTRL_EXE ) ) 	{ return -1; }  // exit
		n=atof( (char*)buffer );
 		if ( (1<=n)&&(n<=20) ) break;
 		n=0;
 	}
	return n ; // ok
}

unsigned int Pict() {
	int cont=1;
	int key;
	int n;

	while ( cont ) {
		locate(1,8); PrintLine((unsigned char *)" ",21);
		Fkey_Icon( FKeyNo1, 183 );	//	Fkey_dispR( FKeyNo1, "PICT");
		FkeyClear( FKeyNo6 );
		GetKey_DisableCatalog( &key );
		switch (key) {
			case KEY_CTRL_EXIT:
				key=0;
			case KEY_CTRL_AC:
			case KEY_CTRL_EXE:
				cont=0;
				break;
			case KEY_CTRL_F1:
				Fkey_Icon( FKeyNo1, 699 );	//	Fkey_dispR( FKeyNo1, "STO");
				Fkey_Icon( FKeyNo2, 700 );	//	Fkey_dispR( FKeyNo2, "RCL");
				GetKey_DisableCatalog( &key );
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
							StoPictSmem(n, 0x4C);
							cont=0;
						}
						break;
					case KEY_CTRL_F2:
						n=PictSelectNum2( "Recall From" );
						if (n>0) { 
							RestoreDisp(SAVEDISP_PAGE1);	// ------ RestoreDisp1
							RclPict(n, 1);
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
int Plot()
{
	int cont=1;
	char buffer[64];
	int key;
	int retcode=0;
	int px1,py1;
	int dx=CB_G1MorG3M;
	int dy=CB_G1MorG3M;
//	int colortmp=CB_ColorIndex;

	long FirstCount;	// pointer to repeat time of first repeat
	long NextCount; 	// pointer to repeat time of second repeat

//	while( KeyCheckEXE() );
//	while( KeyCheckEXIT() );
//	while( KeyCheckAC() );
	KeyRecover();
	
	CB_StatusDisp_Run();
	if ( Plot_Style==S_L_Thin ) { dx=1; dy=1; }
	
	if ( VWtoPXY( Plot_X, Plot_Y, &GCursorX, &GCursorY) ) return 0;	// VW(X,Y) to  graphic cursor XY
	
	Bkey_Get_RepeatTime(&FirstCount,&NextCount);	// repeat time

	SaveDisp(SAVEDISP_PAGE1);	// ------ SaveDisp1
	while ( cont ) {	
		if ( dx==1 ) Bkey_Set_RepeatTime(16,1);				// set graphic cursor repeat time  (count * 25ms)
		else 		 Bkey_Set_RepeatTime(16,2);				// set graphic cursor repeat time  (count * 25ms)
		
		RestoreDisp(SAVEDISP_PAGE1);	// ------ RestoreDisp1
//		SaveDisp(SAVEDISP_PAGE1);		// ------ SaveDisp1
		PXYtoVW(GCursorX, GCursorY, &Plot_X, &Plot_Y);	// graphic cursor XY  to  VW(X,Y)
		if ( fabs(Plot_X*1e10)<Xdot ) Plot_X=0;	// zero adjust
		if ( fabs(Plot_Y*1e10)<Ydot ) Plot_Y=0;	// zero adjust
		if ( Coord ) {
			PrintMinix3orWB(  0,58,(unsigned char*)"X=");
			sprintGRS(buffer, Plot_X, 13,LEFT_ALIGN, Norm,10); PrintMinix3orWB(  8,58,(unsigned char*)buffer);
			PrintMinix3orWB( 64,58,(unsigned char*)"Y=");
			sprintGRS(buffer, Plot_Y, 13,LEFT_ALIGN, Norm,10); PrintMinix3orWB( 72,58,(unsigned char*)buffer);
		}
		DrawGCSR(GCursorX,GCursorY); 	// draw graphic cursor
//		Bdisp_PutDisp_DD();
		GCursorSetFlashMode(1);	// graphic cursor flashing on
		StatusArea_Run();
		GetKey_DisableMenu(&key);
		GCursorSetFlashMode(0);	// graphic cursor flashing off
		if ( key==KEY_CTRL_OPTN ) key=Pict();
		switch (key) {
			case KEY_CTRL_EXE:
			case KEY_CTRL_AC:
			case KEY_CTRL_EXIT:
				cont=0;
				break;
			case KEY_CTRL_F3:	// setViewWindow
				if ( SetViewWindow() ) cont=0;
				break;
			case KEY_CTRL_LEFT:
				if ( GCursorX > MatBase ) GCursorX-=dx;
				break;
			case KEY_CTRL_RIGHT:
				if ( CB_G1MorG3M==1 ) {
					if ( GCursorX < 127 ) GCursorX+=dx;
				} else {
					if ( GCursorX < 379 ) GCursorX+=dx;
				}
				break;
			case KEY_CTRL_UP:
				if ( GCursorY > MatBase ) GCursorY-=dy;
				break;
			case KEY_CTRL_DOWN:
				if ( CB_G1MorG3M==1 ) {
					if ( GCursorY < 63 ) GCursorY+=dy;
				} else {
					if ( GCursorY < 187 ) GCursorY+=dy;
				}
				break;
			case KEY_CTRL_SHIFT:
				locate(1,8); PrintLine((unsigned char *)" ",21);
				Fkey_Icon( FKeyNo1, 877 );	//	Fkey_dispN( FKeyNo1, "Var");
				Fkey_Icon( FKeyNo2, 286 );	//	Fkey_dispN( FKeyNo2, "Mat");
				Fkey_Icon( FKeyNo3, 560 );	//	Fkey_dispR( FKeyNo3, "VWIN");
				Fkey_Icon( FKeyNo6, 563 );	//	Fkey_dispN( FKeyNo6, "G<>T");
				GetKey_DisableMenu_pushpop(&key);
				GCursorSetFlashMode(0);	// graphic cursor flashing off
				switch (key) {
					case KEY_CTRL_EXIT:
						break;
					case KEY_CTRL_SETUP:
						selectSetup=SetupG(selectSetup, 1);
						break;
					case KEY_CTRL_F1:
						selectVar=SetVar(selectVar);		// A - 
						break;
					case KEY_CTRL_F2:
						selectMatrix=SetMatrix(selectMatrix);		// 
						break;
					case KEY_CTRL_F3:
						SetViewWindow();
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
//	CB_ColorIndex=colortmp;
//	if ( retcode==0 ) BdispSetPointVRAM2( GCursorX, GCursorY, 1);
	regX.real = Plot_X ;
	regY.real = Plot_Y ;
	regintX=regX.real; regintY=regY.real;
	return key ;
}

//----------------------------------------------------------------------------------------------
// Graph function
//----------------------------------------------------------------------------------------------
void FkeyZoom(){
	locate(1,8); PrintLine((unsigned char *)" ",21);
	Fkey_Icon( FKeyNo2, 565 );	//	Fkey_dispR( FKeyNo2, "FACT");
	Fkey_Icon( FKeyNo3, 566 );	//	Fkey_dispN( FKeyNo3, " IN");
	Fkey_Icon( FKeyNo4, 567 );	//	Fkey_dispN( FKeyNo4, "OUT");
	FkeyClear( FKeyNo6 );
}
void FkeyGraph(){
	locate(1,8); PrintLine((unsigned char *)" ",21);
	Fkey_Icon( FKeyNo1, 558 );	//	Fkey_dispN( FKeyNo1, "TRCE");
	Fkey_Icon( FKeyNo2, 559 );	//	Fkey_dispR( FKeyNo2, "ZOOM");
	Fkey_Icon( FKeyNo3, 560 );	//	Fkey_dispR( FKeyNo3, "V-W");
	Fkey_Icon( FKeyNo6, 563 );	//	Fkey_dispN( FKeyNo6, "G<>T");
}
//--------------------------------------------------------------
int ZoomXY() {
	int cont=1;
	char buffer[21];
	int key;
	int retcode=0;
	
	long FirstCount;	// pointer to repeat time of first repeat
	long NextCount; 	// pointer to repeat time of second repeat

	regX.real=(Xmax+Xmin)/2; regY.real=(Ymax+Ymin)/2;		// center
	if ( VWtoPXY( regX.real, regY.real, &GCursorX, &GCursorY) ) return 0;	// VW(X,Y) to  graphic cursor XY
	
	Bkey_Get_RepeatTime(&FirstCount,&NextCount);	// repeat time
	Bkey_Set_RepeatTime(FirstCount,2);				// set graphic cursor repeat time  (count * 25ms)

	SaveDisp(SAVEDISP_PAGE1);	// ------ SaveDisp1
	GCursorSetFlashMode(1);	// graphic cursor flashing on
	while ( cont ) {	
		RestoreDisp(SAVEDISP_PAGE1);	// ------ RestoreDisp1
//		SaveDisp(SAVEDISP_PAGE1);		// ------ SaveDisp1
		PXYtoVW(GCursorX, GCursorY, &regX.real, &regY.real);	// graphic cursor XY  to  VW(X,Y)
		if ( fabs(regX.real*1e10)<Xdot ) regX.real=0;	// zero adjust
		if ( fabs(regY.real*1e10)<Ydot ) regY.real=0;	// zero adjust
		if ( Coord ) {
			PrintMinix3orWB(  0,58,(unsigned char*)"X=");
			sprintGRS(buffer, regX.real, 13,LEFT_ALIGN, Norm,10); PrintMinix3orWB(  8,58,(unsigned char*)buffer);
			PrintMinix3orWB( 64,58,(unsigned char*)"Y=");
			sprintGRS(buffer, regY.real, 13,LEFT_ALIGN, Norm,10); PrintMinix3orWB( 72,58,(unsigned char*)buffer);
		}
		DrawGCSR(GCursorX,GCursorY); 	// draw graphic cursor
//		Bdisp_PutDisp_DD();

		KeyRecover();
		GetKey_DisableMenu(&key);
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
				if ( GCursorX < ScreenWidth ) GCursorX++;
				break;
			case KEY_CTRL_UP:
				if ( GCursorY >   1 ) GCursorY--;
				break;
			case KEY_CTRL_DOWN:
				if ( GCursorY <  ScreenHeight ) GCursorY++;
				break;
			default:
				break;
		}
	}
	GCursorSetFlashMode(0);	// graphic cursor flashing off
	Bkey_Set_RepeatTime(FirstCount,NextCount);	// restore repeat time
	RestoreDisp(SAVEDISP_PAGE1);	// ------ RestoreDisp1
	regintX=regX.real; regintY=regY.real;
	return key ;
}
//--------------------------------------------------------------
int Zoom_sub( int key){
	double x,y;

	FkeyZoom();

	if (key==0) GetKey_DisableMenu(&key);
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
int Trace(int *index ) {
	int cont=1;
	char buffer[64];
	int key;
	double dydx;
	int kx=CB_G1MorG3M;
//	int bk_Xdot=Xdot;
//	Xdot*=kx;
//	if ( Plot_Style==S_L_Thin ) { kx=CB_G1MorG3M; }
	
	long FirstCount;	// pointer to repeat time of first repeat
	long NextCount; 	// pointer to repeat time of second repeat

	if ( (*index) <   1 ) (*index)=  0;
	if ( (*index) > 127 ) (*index)=127;
	if ( kx==3 ) 	PXYtoVW((*index)*kx-2, 0, &regX.real, &regY.real);	// graphic cursor X  to  VW(X,dummy)
	else			PXYtoVW((*index),      0, &regX.real, &regY.real);	// graphic cursor X  to  VW(X,dummy)
	VWtoPXY( regX.real, traceAry[(*index)], &GCursorX, &GCursorY);	// VW(X,Y) to  graphic cursor XY
	
	Bkey_Get_RepeatTime(&FirstCount,&NextCount);	// repeat time
	Bkey_Set_RepeatTime(FirstCount,2);				// set graphic cursor repeat time  (count * 25ms)

	CB_StatusDisp_Run();
	SaveDisp(SAVEDISP_PAGE1);	// ------ SaveDisp1
	GCursorSetFlashMode(1);	// graphic cursor flashing on
	while ( cont ) {	
		RestoreDisp(SAVEDISP_PAGE1);	// ------ RestoreDisp1
//		SaveDisp(SAVEDISP_PAGE1);		// ------ SaveDisp1
		if ( kx==3 ) 	PXYtoVW((*index)*kx-2, 0, &regX.real, &regY.real);	// graphic cursor X  to  VW(X,dummy)
		else			PXYtoVW((*index),      0, &regX.real, &regY.real);	// graphic cursor X  to  VW(X,dummy)
		regY.real=traceAry[(*index)];
		if ( fabs(regX.real)*1e10<Xdot ) regX.real=0;	// zero adjust
		VWtoPXY( regX.real, regY.real, &GCursorX, &GCursorY);	// VW(X,Y) to  graphic cursor XY
		if ( Coord ) {
//			sprintf3((char*)buffer, "PX=%d", GCursorX);	PrintMinix3orWB(  0,-7,(unsigned char*)buffer);
//			sprintf3((char*)buffer, "PY=%d", GCursorY);	PrintMinix3orWB( 32,-7,(unsigned char*)buffer);
			PrintMinix3orWB(  0,58,(unsigned char*)"X=");
			sprintGRS(buffer, regX.real,        13,LEFT_ALIGN, Norm,10); PrintMinix3orWB(  8,58,(unsigned char*)buffer);
			PrintMinix3orWB( 64,58,(unsigned char*)"Y=");
			sprintGRS(buffer,traceAry[(*index)],13,LEFT_ALIGN, Norm,10); PrintMinix3orWB( 72,58,(unsigned char*)buffer);
		}
		if ( Derivative ) {
			PrintMinix3orWB( 64,50,(unsigned char*)"dY/dX=");
			dydx = (traceAry[(*index)+1]-traceAry[(*index)-1]) / (Xdot*kx*2);
			if ( fabs(dydx)<1.0e-13 ) dydx=0;	// zero adjust
			sprintGRS(buffer, dydx, 6,LEFT_ALIGN, Norm,5); PrintMinix3orWB( 88,50,(unsigned char*)buffer);
		}
		DrawGCSR(GCursorX,GCursorY); 	// draw graphic cursor
//		Bdisp_PutDisp_DD();
		KeyRecover();
		GetKey_DisableMenu(&key);
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
				GetKey_DisableMenu(&key);
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
				(*index)--;
				if ( (*index) <   1 )  cont=0;
				break;
			case KEY_CTRL_RIGHT:
				(*index)++;
				if ( (*index) > 127 )  cont=0;
				break;
			case KEY_CTRL_SHIFT:
				FkeyGraph();
				GetKey_DisableMenu_pushpop(&key);
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
	regintX=regX.real; regintY=regY.real;
//	Xdot=bk_Xdot;
	return key ;
}

//----------------------------------------------------------------------------------------------
/*
void Graph_Draw(){
	int i;
	double p_x=Previous_X;
	double p_y=Previous_Y;
	int style=S_L_Style;
	int kx=CB_G1MorG3M;
//	if ( Plot_Style==S_L_Thin ) { kx=CB_G1MorG3M; }
	if ( tmp_Style >= 0 ) style=tmp_Style;
	GraphAxesGrid();
	regX.real   = Xmin-Xdot*kx;
	for ( i=0; i<=127; i++) {
		//-----------------------------
		traceAry[i]=CB_EvalStrDBL(GraphY,1);		// function
		if ( ErrorNo ) return ;
		//-----------------------------
		if ( fabs(traceAry[i])*1e10<Ydot*kx ) traceAry[i]=0;	// zero adjust
		if ( i==0 ) { Previous_X = regX.real; Previous_Y = traceAry[0]; }
		if ( ( 0<i ) && ( i<=127 ) ) {
			PlotOn_VRAM( regX.real, traceAry[i]);
			Plot_X=regX.real;
			Plot_Y=regY.real;
			if ( DrawType == 0 ) {	// 1:Plot	// 0:connect
				Lines( style , 1, 0);	// No error check
			}
			Bdisp_PutDisp_DD_DrawBusy_skip();
		}
		regX.real += Xdot*kx;
	}
	regX.real = Plot_X;
	regY.real = Plot_Y;
	regintX=regX.real; regintY=regY.real;
	Previous_X=p_x;
	Previous_Y=p_y;
	SaveDisp(SAVEDISP_PAGE1);	// ------ SaveDisp1
}
*/
void Graph_checker_line( double vx1, double vy1, double vx2, double vy2 ){
	int px1,py1,px2,py2;
	VWtoPXY( vx1, vy1, &px1, &py1);	// VW(X,Y) to  graphic cursor XY
	VWtoPXY( vx2, vy2, &px2, &py2);	// VW(X,Y) to  graphic cursor XY
	ML_line( px1, py1, px2, py2, ML_CHECKER);
}

void Graph_Draw_X(){	//	62:Graph X= 2C:Graph X>  2D:Graph X<  2E:Graph X>=  2F:Graph X<=
	int i;
	int gptr=GraphPtr;
	double p_x=Previous_X;
	double p_y=Previous_Y;
	int px1,py1,px2,py2,flag;
	double tx,vx1,vx2;
	int kx=CB_G1MorG3M;
	int style=GraphStat[gptr].style;
//	if ( tmp_Style >= 0 ) style=tmp_Style;
	int bk_Plot_Style=Plot_Style;
	Plot_Style=style;
	regY.real   = Ymin-Ydot*kx;
	GraphAxesGrid();
	for ( i=0; i<=63*kx; i++) {
		//-----------------------------
		tx=CB_EvalStrDBL(GraphY,1);		// function
		if ( ErrorNo ) return ;
		//-----------------------------
//		if ( fabs(tx)*1e10<Xdot*kx ) tx=0;	// zero adjust
//		traceAry[i/kx]=tx;
		if ( i<kx ) { Previous_X = tx; Previous_Y = regY.real; }
		else {
			flag=1;
			switch ( GraphStat[gptr].type ) {
				case 0x2C:	// Graph X>
					flag=2;
				case 0x2E:	// Graph X>=
					vx1=tx;
					vx2=Xmax;
					break;
				case 0x2D:	// Graph X<
					flag=2;
				case 0x2F:	// Graph X<=
					vx1=tx;
					vx2=Xmin;
					break;
				default:
					flag=0;
			}
			if ( flag==1 ) Graph_checker_line( vx1, regY.real, vx2, regY.real);
			PlotOn_VRAM( tx, regY.real );
			Plot_X=regX.real;
			Plot_Y=regY.real;
			if ( DrawType == 0 ) {	// 1:Plot	// 0:connect
				Lines( style , 1, 0);	// No error check
			}
			if ( flag==2 ) Graph_checker_line( vx1, regY.real, vx2, regY.real);
		}
		regY.real += Ydot;
		Bdisp_PutDisp_DD_DrawBusy_skip();
	}
	regY.real=Plot_Y;
	regintX=regX.real; regintY=regY.real;
	Previous_X=p_x;
	Previous_Y=p_y;
	Plot_Style=bk_Plot_Style;
}

void Graph_Draw(){	//	EE:Graph Y= F0:Graph Y>  F1:Graph Y<  F2:Graph Y>=  F3:Graph Y<=
	int i;
	int gptr=GraphPtr;
	double p_x=Previous_X;
	double p_y=Previous_Y;
	int px1,py1,px2,py2,flag;
	double ty,vy1,vy2;
	int kx=CB_G1MorG3M;
	int style=GraphStat[gptr].style;
//	if ( tmp_Style >= 0 ) style=tmp_Style;
	int bk_Plot_Style=Plot_Style;
	Plot_Style=style;
	regX.real   = Xmin-Xdot*kx;
	GraphAxesGrid();
	for ( i=0; i<=127*kx; i++) {
		//-----------------------------
		ty=CB_EvalStrDBL(GraphY,1);		// function
		if ( ErrorNo ) return ;
		//-----------------------------
		if ( fabs(ty)*1e10<Ydot*kx ) ty=0;	// zero adjust
		traceAry[i/kx]=ty;
		if ( i<kx ) { Previous_X = regX.real; Previous_Y = ty; }
		else {
			flag=1;
			switch ( GraphStat[gptr].type ) {
				case 0xFFFFFFEF:	// Graph Integral
					vy1=0;
					vy2=ty;
					flag=( (IntegralStart <= regX.real) && (regX.real <= IntegralEnd) );
					break;
				case 0xFFFFFFF0:	// Graph Y>
					flag=2;
				case 0xFFFFFFF2:	// Graph Y>=
					vy1=ty;
					vy2=Ymax;
					break;
				case 0xFFFFFFF1:	// Graph Y<
					flag=2;
				case 0xFFFFFFF3:	// Graph Y<=
					vy1=ty;
					vy2=Ymin;
					break;
				default:
					flag=0;
					break;
			}
			if ( flag==1 ) Graph_checker_line( regX.real, vy1, regX.real, vy2 );
			PlotOn_VRAM( regX.real, ty);
			Plot_X=regX.real;
			Plot_Y=regY.real;
			if ( DrawType == 0 ) {	// 1:Plot	// 0:connect
				Lines( style , 1, 0);	// No error check
			}
			if ( flag==2 ) Graph_checker_line( regX.real, vy1, regX.real, vy2 );
		}
		regX.real += Xdot;
		Bdisp_PutDisp_DD_DrawBusy_skip();
	}
	regX.real=Plot_X;
	regintX=regX.real; regintY=regY.real;
	Previous_X=p_x;
	Previous_Y=p_y;
	Plot_Style=bk_Plot_Style;
}


void Graph_reDraw(){
	int i;
	ViewWindow( Xmin, Xmax, Xscl, Ymin, Ymax, Yscl);
	Bdisp_AllClr_VRAM2();			// ------ Clear VRAM 
	Graph_Draw();
//	SaveDisp(SAVEDISP_PAGE1);	// ------ SaveDisp1
}
//--------------------------------------------------------------
unsigned int Graph_trace_sub(int *tracex){
	int key;
	int tx,ty; // dummy
	int kx=CB_G1MorG3M;
	tx=*tracex;
	while (1) {
		CB_StatusDisp_Run();
		if ( tx <   1 ) tx=  0;
		if ( tx > 127 ) tx=127;
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
			Xmin-=Xdot*8*kx;
			Xmax-=Xdot*8*kx;
			Graph_reDraw();
			VWtoPXY(Xmin+Xdot*7*kx, 0, &tx, &ty);
			if ( kx==3 ) tx=(tx+2)/kx;
		}
		if ( key==KEY_CTRL_RIGHT ) { 
			Xmin+=Xdot*8*kx;
			Xmax+=Xdot*8*kx;
			Graph_reDraw();
			VWtoPXY(Xmax-Xdot*7*kx, 0, &tx, &ty);
			if ( kx==3 ) tx=(tx+2)/kx;
		}
	}
	*tracex=tx;
	regintX=regX.real; regintY=regY.real;
	return key;
}

//--------------------------------------------------------------
//--------------------------------------------------------------
unsigned int Graph_main(){
	char buffer[64];
	int key;
	int cont=1;
	int kx=CB_G1MorG3M*12;
	int tx=64;	// trace center 
	Graph_Draw();
//	while( KeyCheckEXE() );
//	while( KeyCheckEXIT() );
//	while( KeyCheckAC() );
	KeyRecover();

	while (cont) {
		CB_StatusDisp_Run();
		SaveDisp(SAVEDISP_PAGE1);		// ------ SaveDisp1
		if ( key==KEY_CTRL_OPTN ) key=Pict();
		GetKey_DisableCatalog( &key );
		switch (key) {
			case KEY_CTRL_AC:
			case KEY_CTRL_EXE:
			case KEY_CTRL_EXIT:
			case KEY_CTRL_F6:
				cont=0;
				break;
			case KEY_CTRL_LEFT:
				Xmin-=Xdot*kx;
				Xmax-=Xdot*kx;
				Graph_reDraw();
				break;
			case KEY_CTRL_RIGHT:
				Xmin+=Xdot*kx;
				Xmax+=Xdot*kx;
				Graph_reDraw();
				break;
			case KEY_CTRL_UP:
				Ymin+=Ydot*kx;
				Ymax+=Ydot*kx;
				Graph_reDraw();
				break;
			case KEY_CTRL_DOWN:
				Ymin-=Ydot*kx;
				Ymax-=Ydot*kx;
				Graph_reDraw();
				break;
			case KEY_CTRL_SHIFT:
				FkeyGraph();
//				GetKey_DisableMenu_pushpop(&key);
				GetKey_DisableCatalog( &key );
				switch (key) {
					case KEY_CTRL_EXIT:
					case KEY_CTRL_F6:
						cont=0;
						break;
					case KEY_CTRL_SETUP:
						selectSetup=SetupG(selectSetup, 1);
						Graph_reDraw();
						SaveDisp(SAVEDISP_PAGE1);		// ------ SaveDisp1
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
						SaveDisp(SAVEDISP_PAGE1);		// ------ SaveDisp1
						break;
					case KEY_CTRL_F3:
						SetViewWindow();
						Graph_reDraw();
						SaveDisp(SAVEDISP_PAGE1);		// ------ SaveDisp1
						break;
					default:
						break;
				}
				RestoreDisp(SAVEDISP_PAGE1);	// ------ RestoreDisp1
				break;
			case KEY_CTRL_F1:
//				RestoreDisp(SAVEDISP_PAGE1);	// ------ RestoreDisp1
				key=Graph_trace_sub(&tx);	// trace
				if ( key==KEY_CTRL_EXIT ) cont=0; //exit
				if ( key==KEY_CTRL_EXE  ) cont=0; //exe
				if ( key==KEY_CTRL_AC   ) cont=0; // AC
				if ( key==KEY_CTRL_F6   ) cont=0; // F6
				break;
			case KEY_CTRL_F2:
//				RestoreDisp(SAVEDISP_PAGE1);	// ------ RestoreDisp1
				key=Zoom_sub(0);	// zoom
				if ( key==KEY_CTRL_F1   ) key=Graph_trace_sub(&tx);	// trace
				if ( key==KEY_CTRL_EXIT ) cont=0; //exit
				if ( key==KEY_CTRL_AC   ) cont=0; // AC
				if ( key==KEY_CTRL_F6   ) cont=0; // F6
				Graph_reDraw();
				SaveDisp(SAVEDISP_PAGE1);		// ------ SaveDisp1
				break;
			case KEY_CTRL_F3:
				SetViewWindow();
				Graph_reDraw();
				SaveDisp(SAVEDISP_PAGE1);		// ------ SaveDisp1
				break;
			default:
				break;
		}
		
//		Bdisp_PutDisp_DD();
	}
	return key;
}

//----------------------------------------------------------------------------------------------
/*
double GraphXYEval( char *buffer, int *add ) {
	double result;
	int excptr=ExecPtr;
	int Ansreg=CB_MatListAnsreg;
	dspflag=0;
	ExecPtr=0;
	result=EvalsubTopReal( buffer );
	if ( dspflag>=3 ) {
		CB_MatListAnsreg=Ansreg;
		ExecPtr=0; Cplx_ListEvalsubTop( buffer );	// List calc
		if ( dspflag != 4 ) { CB_Error(ArgumentERR); return 0; } // Argument error
		result=ReadMatrix( CB_MatListAnsreg, regT.real, MatAry[CB_MatListAnsreg].Base );
	}
	*add = ExecPtr+1;
	ExecPtr=excptr;
	if ( ErrorNo ) { ErrorPtr=ExecPtr; return 0; }
	return result;
}
*/
double GraphXYEval( char *buffer, int *add ) {
	double result;
	int excptr=ExecPtr;
	int Ansreg=CB_MatListAnsreg;
	dspflag=0;
	ExecPtr=0;
	if ( CB_MatListAnsreg >=28 ) CB_MatListAnsreg=28;
	result = Cplx_ListEvalsubTop( buffer ).real;
	*add = ExecPtr+1;
	ExecPtr=excptr;
	if ( ErrorNo ) { ErrorPtr=ExecPtr; return 0; }
	return result;
}
int Graph_Draw_XY_List_sub( char *graph, double *dadd, double *dmul, int *listdirect ) {
	int c,reg,gp = 0;
	int excptr = ExecPtr;
	dspflag = 0;
	ExecPtr = 0;
	*dadd = 0.0;
	*dmul = 1.0;
	if (graph[0]=='{') {
		goto checklist;
	} else {		// A+{ etc
		CB_MatListAnsreg=28;
		*dadd = Cplx_Evalsub1(graph).real;
		if ( dspflag<3 ) {
			c = graph[ExecPtr];
			if ( c == 0xFFFFFF89 ) {	// +
				ExecPtr++;
				if ( graph[ExecPtr] == '{' ) {
				  checklist:
					gp = ExecPtr +1;
					Cplx_ListEvalsub1(graph);
					c = graph[ExecPtr];
					if ( ( c==',' ) || ( c==':' ) || ( c==')' ) || ( c==0x0D ) || ( c==0x0C ) || ( c==0x00 ) ) {
						(*listdirect) = 1;
						goto exit;
					}
				} else {
				 checkmul:
					*dmul = Cplx_Evalsub1(graph).real;
					if ( dspflag<3 ) {
						if ( graph[ExecPtr] == 0xFFFFFFA9 ) ExecPtr++;	// x
						if ( graph[ExecPtr] == '{' ) goto checklist;	//A*{...}
					}
				}
			} else {
				if ( c == 0xFFFFFFA9 ) ExecPtr++;	// x
				*dmul = *dadd;
				*dadd = 0.0;
				if ( graph[ExecPtr] == '{' ) goto checklist;	//A*{...}
			}
		}
		gp = 0;
		*dadd = 0;
		*dmul = 1.0;
	}
  exit:
	ExecPtr = excptr;
	return gp;
}

void Graph_Draw_XY_List(int xlistreg, int ylistreg, int drawf ){	// Graph XY ( List 1, List 2)
	double tmpX,tmpY;
	double p_x=Previous_X;
	double p_y=Previous_Y;
	char *graphX=GraphX;
	char *graphY=GraphY;
	double xdadd=0,ydadd=0;
	double xdmul=1,ydmul=1;
	int XlistDirect=0,YlistDirect=0;
	int sizeA,sizeA2;
	int base=0,base2=0;
	int c;
	int addX,addY;
	int at1st;
	int style=SetStyle();
	if ( xlistreg==0 ) {
		sizeA = 1;
	} else {
		sizeA = MatAry[xlistreg].SizeA; base =MatAry[xlistreg].Base;
		graphX += Graph_Draw_XY_List_sub( graphX, &xdadd, &xdmul, &XlistDirect );
	}
	if ( ylistreg==0 ) {
		sizeA2 = 1;
	} else {
		sizeA2 = MatAry[ylistreg].SizeA; base2=MatAry[ylistreg].Base;
		graphY += Graph_Draw_XY_List_sub( graphY, &ydadd, &ydmul, &YlistDirect );
	}
	if ( xlistreg && ylistreg ) {
		if ( base != base2 ) { CB_Error(ArgumentERR); return ; } // Argument error
		if ( sizeA  > sizeA2 )  sizeA = sizeA2 ;
	} else {
		if ( ( sizeA < sizeA2  ) && ( xlistreg==0 ) ) sizeA = sizeA2 ;
	}
	
//	GraphAxesGrid( Xmin, Xmax, Xscl, Ymin, Ymax, Yscl);
	c = base;
	for ( c=base; c<sizeA+base; c++ ) {
		regT.real=TThetamin;
		regintT  =TThetamin;
		if ( TThetaptch == 0 ) return ;
		at1st=0;
		while ( regT.real<=TThetamax ) {
			//-----------------------------
			regX.real = GraphXYEval(graphX, &addX)*xdmul + xdadd;		// function
			if ( ( xlistreg ) && ( XlistDirect==0 ) ) regX.real = ReadMatrix( CB_MatListAnsreg, c, base );
			regY.real = GraphXYEval(graphY, &addY)*ydmul + ydadd;		// function
			if ( ( ylistreg ) && ( YlistDirect==0 ) ) regY.real = ReadMatrix( CB_MatListAnsreg, c, base2 );
//			if ( ErrorNo ) return ;
			//-----------------------------
//			if ( fabs(regX.real)*1e10<Xdot ) regX.real=0;	// zero adjust
//			if ( fabs(regY.real)*1e10<Ydot ) regY.real=0;	// zero adjust
			if ( at1st==0 ) { Previous_X = regX.real; Previous_Y = regY.real; at1st=1; }
			PlotOn_VRAM( regX.real, regY.real );
			Plot_X=regX.real;
			Plot_Y=regY.real;
			if ( DrawType == 0 ) {	// 1:Plot	// 0:connect
				Lines( style , 1, 0);	// No error check
			}
			if ( drawf ) Bdisp_PutDisp_DD_DrawBusy_skip();
			regT.real += TThetaptch;
			regintT   += TThetaptch;
		}
		if ( XlistDirect )	graphX += addX;
		if ( YlistDirect )	graphY += addY;
	}
	Previous_X=p_x;
	Previous_Y=p_y;
	regintX=regX.real; regintY=regY.real;
//	SaveDisp(SAVEDISP_PAGE1);	// ------ SaveDisp1
}


//----------------------------------------------------------------------------------------------

void DrawStat_PlotOn_VRAM(double x, double y, int Type){
	int px,py;
	if ( VWtoPXY( x,y, &px, &py) == 0) {
		if ( CB_G1MorG3M == 1 ) {
			switch ( Type ) {
				case 0:	// Square
					BdispSetPointVRAM2(px-1, py-1, 1);
					BdispSetPointVRAM2(px  , py-1, 1);
					BdispSetPointVRAM2(px+1, py-1, 1);
					BdispSetPointVRAM2(px-1, py  , 1);
//					BdispSetPointVRAM2(px  , py  , 0);
					BdispSetPointVRAM2(px+1, py  , 1);
					BdispSetPointVRAM2(px-1, py+1, 1);
					BdispSetPointVRAM2(px  , py+1, 1);
					BdispSetPointVRAM2(px+1, py+1, 1);
					break;
				case 1:	// Cross
					BdispSetPointVRAM2(px-1, py-1, 1);
//					BdispSetPointVRAM2(px  , py-1, 1);
					BdispSetPointVRAM2(px+1, py-1, 1);
//					BdispSetPointVRAM2(px-1, py  , 1);
					BdispSetPointVRAM2(px  , py  , 1);
//					BdispSetPointVRAM2(px+1, py  , 1);
					BdispSetPointVRAM2(px-1, py+1, 1);
//					BdispSetPointVRAM2(px  , py+1, 1);
					BdispSetPointVRAM2(px+1, py+1, 1);
					break;
				case 2:	// Dot
				default:
					BdispSetPointVRAM2(px  , py  , 1);
					break;
			}
		} else {
			switch ( Type ) {
				case 0:	// Square
					ML_rectangle( px-2, py-2, px+6, py+6, 2, ML_BLACK, ML_WHITE);
					break;
				case 1:	// Cross
					CB_PrintModeC( px-7, py-8, (unsigned char*)"\xE6\xAE", MINI_OR );
					break;
				case 2:	// Dot
				default:
					ML_point( px+2, py+2, 7, ML_BLACK );
//					BdispSetPointVRAM2(px  , py  , 1);
					break;
			}
		}
	}
	regX.real=x; regintX =x;
	regY.real=y; regintY =y;
}

//								  Black  Blue   Red    Green  Magenta  Cyan  Yellow White
static	unsigned short colortable[]={ 0x0000,0x001F,0xF800,0x07E0,0xF81F,0x07FF,0xFFE0,0xFFFF };

void DrawStat(){	// DrawStat
	double tmpX,tmpY;
	double p_x=Previous_X;
	double p_y=Previous_Y;
	int sizeA,sizeA2;
	int base=0,base2=0;
	int c,No=0;
	int at1st;
	int xlistreg,ylistreg;
	int bk_CB_ColorIndex=CB_ColorIndex;
	int cno=1;

	tmpX=regX.real; tmpY=regY.real;
//	ML_clear_vram();
//	GraphAxesGrid( Xmin, Xmax, Xscl, Ymin, Ymax, Yscl);

	for ( No=0; No<3; No++ ) {
		at1st=0;
		if ( Sgraph[No].Draw == 1 ) {
			xlistreg=Sgraph[No].xList;
			if ( ListFilePtr ) {
				if ( 58 <= xlistreg ) xlistreg-=58; else xlistreg-=32+26;
				xlistreg += ListFilePtr;
			}
			ylistreg=Sgraph[No].yList;
			if ( ListFilePtr ) {
				if ( 58 <= ylistreg ) ylistreg-=58; else ylistreg-=32+26;
				ylistreg += ListFilePtr;
			}
			if ( xlistreg==0 ) sizeA =1; else { sizeA =MatAry[xlistreg].SizeA; base =MatAry[xlistreg].Base; }
			if ( ylistreg==0 ) sizeA2=1; else { sizeA2=MatAry[ylistreg].SizeA; base2=MatAry[ylistreg].Base; }
			if ( base != base2 ) { CB_Error(ArgumentERR); return ; } // Argument error
			if ( sizeA > sizeA2 ) sizeA=sizeA2;
			c = base;
			for ( c=base; c<sizeA+base; c++ ) {
				regX.real=ReadMatrix( xlistreg, c, base );
				regY.real=ReadMatrix( ylistreg, c, base2 );
				if ( fabs(regX.real)*1e10<Xdot ) regX.real=0;	// zero adjust
				if ( fabs(regY.real)*1e10<Ydot ) regY.real=0;	// zero adjust
				if ( at1st==0 ) { Previous_X = regX.real; Previous_Y = regY.real; at1st=1; }
				if ( Sgraph[No].ColorAuto == 0 ) {
					CB_ColorIndex = Sgraph[No].Color;
				} else {
					do {
						CB_ColorIndex = colortable[cno++];
						if ( cno > 4 ) cno=0;
					} while ( CB_ColorIndex == CB_BackColorIndex );
				}
				DrawStat_PlotOn_VRAM(regX.real, regY.real, Sgraph[No].MarkType);
				Plot_X=regX.real;
				Plot_Y=regY.real;
				if ( Sgraph[No].GraphType == 1 ) {	// 1:xyLine
					if ( CB_G1MorG3M==1 ) Lines( S_L_Normal, 1, 0);	// No error check
					else				  Lines( S_L_Thin  , 1, 0);	// No error check
				}
//				Bdisp_PutDisp_DD();
			}
		}
	}
//	regintX=regX.real; regintY=regY.real;
	Previous_X=p_x;
	Previous_Y=p_y;
	regX.real=tmpX; regY.real=tmpY;
//	SaveDisp(SAVEDISP_PAGE1);	// ------ SaveDisp1
	CB_ColorIndex = bk_CB_ColorIndex;
}

/*
	CB_ColorIndex=0x0000;	// Black
	CB_ColorIndex=0x001F;	// Blue
	CB_ColorIndex=0xF800;	// Red
	CB_ColorIndex=0xF81F;	// Magenta
	CB_ColorIndex=0x07E0;	// Green
	CB_ColorIndex=0x07FF;	// Cyan
	CB_ColorIndex=0xFFE0;	// Yellow
	CB_ColorIndex=0xFFFF;	// White
*/

//---------------------------------------------------------------------------------------------- align dummy
int glib2ObjectAlign4a( unsigned int n ){ return n; }	// align +4byte
int glib2ObjectAlign4b( unsigned int n ){ return n; }	// align +4byte
int glib2ObjectAlign4c( unsigned int n ){ return n; }	// align +4byte
int glib2ObjectAlign4d( unsigned int n ){ return n; }	// align +4byte
int glib2ObjectAlign4e( unsigned int n ){ return n; }	// align +4byte
int glib2ObjectAlign4f( unsigned int n ){ return n; }	// align +4byte
int glib2ObjectAlign4g( unsigned int n ){ return n; }	// align +4byte
int glib2ObjectAlign4h( unsigned int n ){ return n; }	// align +4byte
//int glib2ObjectAlign4i( unsigned int n ){ return n; }	// align +4byte
int glib2ObjectAlign4j( unsigned int n ){ return n; }	// align +4byte
int glib2ObjectAlign4k( unsigned int n ){ return n; }	// align +4byte
int glib2ObjectAlign4l( unsigned int n ){ return n; }	// align +4byte
int glib2ObjectAlign4m( unsigned int n ){ return n; }	// align +4byte
int glib2ObjectAlign4n( unsigned int n ){ return n; }	// align +4byte
int glib2ObjectAlign4o( unsigned int n ){ return n; }	// align +4byte
int glib2ObjectAlign4p( unsigned int n ){ return n; }	// align +4byte
//int glib2ObjectAlign4q( unsigned int n ){ return n; }	// align +4byte
//int glib2ObjectAlign4r( unsigned int n ){ return n; }	// align +4byte
//int glib2ObjectAlign4s( unsigned int n ){ return n; }	// align +4byte
//int glib2ObjectAlign4t( unsigned int n ){ return n; }	// align +4byte
//int glib2ObjectAlign4u( unsigned int n ){ return n; }	// align +4byte
//int glib2ObjectAlign4v( unsigned int n ){ return n; }	// align +4byte
//int glib2ObjectAlign4w( unsigned int n ){ return n; }	// align +4byte
//int glib2ObjectAlign4x( unsigned int n ){ return n; }	// align +4byte
//int glib2ObjectAlign4y( unsigned int n ){ return n; }	// align +4byte
//int glib2ObjectAlign4z( unsigned int n ){ return n; }	// align +4byte
//int glib2ObjectAlign4A( unsigned int n ){ return n; }	// align +4byte
//int glib2ObjectAlign4B( unsigned int n ){ return n; }	// align +4byte
//int glib2ObjectAlign4C( unsigned int n ){ return n; }	// align +4byte
//int glib2ObjectAlign4D( unsigned int n ){ return n; }	// align +4byte
//int glib2ObjectAlign4E( unsigned int n ){ return n; }	// align +4byte
//int glib2ObjectAlign4F( unsigned int n ){ return n; }	// align +4byte
//int glib2ObjectAlign4G( unsigned int n ){ return n; }	// align +4byte
//int glib2ObjectAlign4H( unsigned int n ){ return n; }	// align +4byte
//int glib2ObjectAlign4I( unsigned int n ){ return n; }	// align +4byte
//int glib2ObjectAlign4J( unsigned int n ){ return n; }	// align +4byte
//int glib2ObjectAlign4K( unsigned int n ){ return n; }	// align +4byte
//int glib2ObjectAlign4L( unsigned int n ){ return n; }	// align +4byte
//int glib2ObjectAlign4M( unsigned int n ){ return n; }	// align +4byte
//int glib2ObjectAlign4N( unsigned int n ){ return n; }	// align +4byte
}
