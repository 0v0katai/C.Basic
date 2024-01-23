/*
===============================================================================

 Casio Basic interpreter for fx-9860G series    v0.22

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
#include "CB_lib.h"
#include "CB_Eval.h"
#include "CB_file.h"
#include "CB_edit.h"

#include "CB_interpreter.h"
#include "CB_error.h"
#include "TIMER_FX2.H"

//-----------------------------------------------------------------------------
// Casio Basic inside
//-----------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------
//		Interpreter
//----------------------------------------------------------------------------------------------
int ScreenMode;	//  0:Text  1:Graphic
int UseGraphic;		// use Graph  ( no use :0    plot:1   graph:2   cls:3   other:99

double CursorX=1;	// text cursor X
double CursorY=1;	// text cursor X

int ExecPtr=0;		// Basic execute ptr
int BreakPtr=0;		// Basic break ptr

double CB_CurrentValue=0;	// Ans
char   CB_CurrentStr[128];	//

int ProgEntryPtr=0;		// Basic Program ptr (for subroutin)
int ProgNo=0;			// current Prog No
unsigned char *ProgfileAdrs[ProgMax+1];
int ProgfileMax[ProgMax+1] ;	// Max edit filesize 
int ProgfileEdit[ProgMax+1];	// no change : 0     edited : 1

#define StackGotoMax 10+26+2
#define StackForMax 8
#define StackWhileMax 8
#define StackDoMax 8

int	CB_TicksStart;
int	CB_TicksEnd;
//----------------------------------------------------------------------------------------------
void CB_SelectTextVRAM() {
	if ( ScreenMode == 0 ) return;
	SaveDisp(SAVEDISP_PAGE3);		// ------ SaveDisp3 Graphic screen
	RestoreDisp(SAVEDISP_PAGE2);		// ------ RestoreDisp2 Text screen
	ScreenMode=0;	// Text mode
}
void CB_SelectTextDD() {
	CB_SelectTextVRAM();
	Bdisp_PutDisp_DD();
}
void CB_SelectGraphVRAM() {
	if ( ScreenMode == 1 ) return;
	SaveDisp(SAVEDISP_PAGE2);		// ------ SaveDisp2 Text screen
	RestoreDisp(SAVEDISP_PAGE3);		// ------ RestoreDisp3 Graphic screen
	ScreenMode=1;	// Graphic mode
}
void CB_SelectGraphDD() {
	CB_SelectGraphVRAM();
	Bdisp_PutDisp_DD();
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

int CB_GotoEndPtr( unsigned char *SRC ) {		// goto Program End Ptr
	int size;
	unsigned char *src;
	src=SRC-0x56;
	size=(src[0x47]&0xFF)*256+(src[0x48]&0xFF)+0x4C;
	return ( size -0x56 - 1 ) ;
}

//----------------------------------------------------------------------------------------------

void CB_Cls( unsigned char *SRC ){
	CB_SelectGraphVRAM();	// Select Graphic Screen
	ViewWindow( Xmin, Xmax, Xscl, Ymin, Ymax, Yscl);
//	Bdisp_AllClr_VRAM();
//	GraphAxesGrid();
	Previous_PX=-1;   Previous_PY=-1; 		// ViewWindow Previous PXY init
	CB_SelectTextVRAM();	// Select Text Screen
//	Bdisp_PutDisp_DD_DrawBusy_through(SRC);
}
void CB_ClrText( unsigned char *SRC ){
	CB_SelectTextVRAM();	// Select Text Screen
	CursorX=1;
	CursorY=1;
	Bdisp_AllClr_VRAM();
	Bdisp_PutDisp_DD_DrawBusy_through(SRC);
}
void CB_ClrGraph( unsigned char *SRC ){
	CB_SelectGraphVRAM();	// Select Graphic Screen
	SetVeiwWindowInit();
	CB_Cls(SRC);
}

void CB_Deg(){
	Angle = 0;
}
void CB_Rad(){
	Angle = 1;
}
void CB_Grad(){
	Angle = 2;
}

int RangeErrorCK( unsigned char *SRC ) {
	if ( UseGraphic == 0 ) {
		if ( ScreenMode == 0 ) CB_SelectGraphVRAM();	// Select Graphic Screen
		Bdisp_AllClr_VRAM();
		GraphAxesGrid();
	}
	if ( ( Xdot == 0 ) || ( Ydot == 0 )  ) { ErrorNo=RangeERR; PrevOpcode( SRC, &ExecPtr ); ErrorPtr=ExecPtr; return ErrorNo; }	// Range error
	return 0;
}

//-----------------------------------------------------------------------------
double CB_Eval( unsigned char *expbuf ) {		// Eval Basic
	unsigned int key;
	double result;
    ExpPtr= 0;
    result = EvalsubTop( (char*)expbuf );
    if ( ErrorNo  ) {
	    ExecPtr += ErrorPtr;
	    ErrorPtr=ExecPtr;
		return result;
    }
    ExecPtr += ExpPtr;
	return result;
}

//-----------------------------------------------------------------------------
void CB_Fix( unsigned char *SRC ){
	int tmp;
	tmp=CB_Eval( SRC+ExecPtr );
	if ( ( tmp < 0 ) || ( tmp > 15 ) ) { ErrorNo=SyntaxERR; ErrorPtr=ExecPtr; return; }	// Syntax error
	CB_Round.MODE = Fix ;
	CB_Round.DIGIT= tmp ;
}
void CB_Sci( unsigned char *SRC ){
	int tmp;
	tmp=CB_Eval( SRC+ExecPtr );
	if ( ( tmp < 0 ) || ( tmp > 15 ) ) { ErrorNo=SyntaxERR; ErrorPtr=ExecPtr; return; }	// Syntax error
	CB_Round.MODE = Sci ;
	CB_Round.DIGIT= tmp ;
}
void CB_Norm( unsigned char *SRC ){
	int tmp;
	tmp=CB_Eval( SRC+ExecPtr );
	if ( ( tmp < 0 ) || ( tmp > 15 ) ) { ErrorNo=SyntaxERR; ErrorPtr=ExecPtr; return; }	// Syntax error
	CB_Round.MODE = Norm ;
	CB_Round.DIGIT= tmp ;
}
void CB_Rnd(){
	CB_CurrentValue = Round( CB_CurrentValue, CB_Round.MODE, CB_Round.DIGIT );
}

//-----------------------------------------------------------------------------
int MatOprand( unsigned char *SRC, int *reg){ 
	int dimA,dimB;
	unsigned int c=SRC[ExecPtr];
	if ( ( 'A'<=c )&&( c<='Z' ) ) {
		(*reg)=c-'A';
		ExecPtr++ ;
		if ( SRC[ExecPtr] != '[' ) { ErrorNo=SyntaxERR; ErrorPtr=ExecPtr; return; }	// Syntax error
		ExecPtr++ ;
		dimA=(CB_Eval( SRC+ExecPtr ));
		if ( MatArySizeA[(*reg)] < dimA ) { ErrorNo=DimensionERR; ErrorPtr=ExecPtr; return; }	// Dimension error
		if ( SRC[ExecPtr] != ',' ) { ErrorNo=SyntaxERR; ErrorPtr=ExecPtr; return; }	// Syntax error
		ExecPtr++ ;
		dimB=(CB_Eval( SRC+ExecPtr ));
		if ( MatArySizeB[(*reg)] < dimB ) { ErrorNo=DimensionERR; ErrorPtr=ExecPtr; return; }	// Dimension error
		if ( SRC[ExecPtr] != ']' ) { ErrorNo=SyntaxERR; ErrorPtr=ExecPtr; return; }	// Syntax error
		ExecPtr++ ;
		return (dimA-1)*MatArySizeB[(*reg)]+dimB-1;
	} else
	{ ErrorNo=SyntaxERR; ErrorPtr=ExecPtr; return; }	// Syntax error
	return -1; //
}
//-----------------------------------------------------------------------------

void CB_Store( unsigned char *SRC ){	// ->
	int	st,en,i;
	int dimA,dimB,reg;
	int mptr;
	unsigned int c=SRC[ExecPtr];
	if ( ( 'A' <= c ) && ( c <='Z' ) ) {
		st=c-'A';
		ExecPtr++;
		if ( SRC[ExecPtr] == 0x7E ) {		// '~'
			ExecPtr++;
			c=SRC[ExecPtr];
			if ( ( 'A' <= c ) && ( c <='Z' ) ) {
				en=c-'A';
				if ( en<st ) { ErrorNo=SyntaxERR; ErrorPtr=ExecPtr; return; }	// Syntax error
				ExecPtr++;	
				for ( i=st; i<=en; i++) REG[ i ] = CB_CurrentValue;
			}
		} else {
			REG[ st ] = CB_CurrentValue;
		}
	} else
	if ( c==0x7F ) {
		c = SRC[ExecPtr+1] ; 
		if ( c == 0x40 ) {	// Mat A[a,b]
			ExecPtr+=2;
			mptr=MatOprand( SRC, &reg);
			MatAry[reg][mptr] =  CB_CurrentValue;			// Matrix array
		} else if ( c == 0x00 ) {	// Xmin
				ExpPtr+=2;
				Xmin = CB_CurrentValue ;
				Xdot = (Xmax-Xmin)/126.0;
		} else if ( c == 0x01 ) {	// Xmax
				ExpPtr+=2;
				Xmax = CB_CurrentValue ;
				Xdot = (Xmax-Xmin)/126.0;
		} else if ( c == 0x02 ) {	// Xscl
				ExpPtr+=2;
				Xscl = CB_CurrentValue ;
		} else if ( c == 0x04 ) {	// Ymin
				ExpPtr+=2;
				Ymin = CB_CurrentValue ;
				Ydot = (Ymax-Ymin)/62.0;
		} else if ( c == 0x05 ) {	// Ymax
				ExpPtr+=2;
				Ymax = CB_CurrentValue ;
				Ydot = (Ymax-Ymin)/62.0;
		} else if ( c == 0x06) {	// Yscl
				ExpPtr+=2;
				Yscl = CB_CurrentValue ;
		} else if ( c == 0x0B ) {	// Xfct
				ExpPtr+=2;
				Xfct = CB_CurrentValue ;
		} else if ( c == 0x0C ) {	// Yfct
				ExpPtr+=2;
				Yfct = CB_CurrentValue ;
		}
	} else
	if ( c==0xF9 ) {
		c = SRC[ExecPtr+1] ; 
		if ( c == 0x21 ) {	// Xdot
				if ( CB_CurrentValue == 0 ) { ErrorNo=RangeERR; ErrorPtr=ExecPtr; return; }	// Range error
				ExpPtr+=2;
				Xdot = CB_CurrentValue ;
				Xmax = Xmin + Xdot*126.;
		}
	} else { ErrorNo=SyntaxERR; ErrorPtr=ExecPtr; return; }	// Syntax error
}

int  CB_Input( unsigned char *SRC ){
	unsigned int c;
	double DefaultValue=0;
	int flag=0;
	int reg,bptr,mptr;
	char buffer[32];
	
	KeyRecover();
	CB_SelectTextDD();	// Select Text Screen
	if ( CursorX==22 ) CursorX=1;
	locate( CursorX, CursorY); Print((unsigned char*)"?");
	Scrl_Y();

	c=SRC[ExecPtr];
	bptr=ExecPtr;
	if ( c==0x0E ) {
		flag=0;
	} else
	if ( ( 'A' <= c ) && ( c <='Z' ) ) {
		DefaultValue = REG[ c-'A' ];
		flag=1;
	} else
	if ( c==0x7F ) {
		c = SRC[ExecPtr+1] ; 
		if ( c == 0x40 ) {	// Mat A[a,b]
			ExecPtr+=2;
			mptr=MatOprand( SRC, &reg);
			DefaultValue = MatAry[reg][mptr];			// Matrix array
			flag=1;
			ExecPtr=bptr;
		} else if ( c == 0x00 ) {	// Xmin
				DefaultValue = Xmin ;
				flag=1;
		} else if ( c == 0x01 ) {	// Xmax
				DefaultValue = Xmax ;
				flag=1;
		} else if ( c == 0x02 ) {	// Xscl
				DefaultValue = Xscl ;
				flag=1;
		} else if ( c == 0x04 ) {	// Ymin
				DefaultValue = Ymin ;
				flag=1;
		} else if ( c == 0x05 ) {	// Ymax
				DefaultValue = Ymax ;
				flag=1;
		} else if ( c == 0x06) {	// Yscl
				DefaultValue = Yscl ;
				flag=1;
		} else if ( c == 0x0B ) {	// Xfct
				DefaultValue = Xfct ;
				flag=1;
		} else if ( c == 0x0C ) {	// Yfct
				DefaultValue = Yfct ;
				flag=1;
		} else { ErrorNo=SyntaxERR; ErrorPtr=ExecPtr; return; }	// Syntax error	} else
	} else
	if ( c==0xF9 ) {
		c = SRC[ExecPtr+1] ; 
		if ( c == 0x21 ) {	// Xdot
				DefaultValue = Xdot ;
				flag=1;
		} else { ErrorNo=SyntaxERR; ErrorPtr=ExecPtr; return; }	// Syntax error
	} else { ErrorNo=SyntaxERR; ErrorPtr=ExecPtr; return; }	// Syntax error

//	if ( flag ) {
//			CB_CurrentValue = InputNumD_CB2( 1, CursorY, 21, DefaultValue );
//			CB_Store( SRC );
//	} else	CB_CurrentValue = InputNumD_CB( 1, CursorY, 21, 0 );

	if ( flag ) {
			sprintGR(buffer, DefaultValue, 22-CursorX,RIGHT_ALIGN, CB_Round.MODE, CB_Round.DIGIT);
			locate( CursorX, CursorY); Print((unsigned char*)buffer);
			Scrl_Y();
			CB_CurrentValue = InputNumD_CB1( 1, CursorY, 21, DefaultValue );
			CB_Store( SRC );
	} else	CB_CurrentValue = InputNumD_CB( 1, CursorY, 21, 0 );
	Scrl_Y();
	Bdisp_PutDisp_DD_DrawBusy();
	return 0 ;
}
//-----------------------------------------------------------------------------


void QuotStrOpcode(unsigned char *SRC, char *buffer ) {
	char tmpbuf[18];
	int i,j=0,len,ptr=0;
	unsigned short opcode;
	unsigned char  c=1;
	while ( c != '\0' ) {
		c = SRC[ExecPtr++] ; 
		opcode = c & 0x00FF ;
		if ( c==0x22 ) break ; // "
		if ( c==0x0D ) break ; // <CR>
		if ( ( c==0x7F ) || ( c==0xF7 ) ||( c==0xF9 ) ||( c==0xE5 ) ||( c==0xE6 ) ||( c==0xE7 ) ) {
				opcode = ( ( c & 0x00FF )<<8 )  + ( SRC[ExecPtr++] & 0x00FF );
		}
		OpcodeToStr( opcode, (unsigned char*)tmpbuf ) ;	// SYSCALL
		len = strlen( tmpbuf ) ;
		i=0;
		while ( i < len ) {
			buffer[ptr++]=tmpbuf[i++] ;
		}
	}
	buffer[ptr]='\0' ;
}
void GetQuotStr(unsigned char *SRC, char *buffer ) {
	unsigned int c;
	int ptr=0;
	while (1){
		c=SRC[ptr];
		buffer[ptr]=SRC[ptr];
		switch ( c ) {
			case 0x00:	// <EOF>
				return;
			case 0x22:	// "
			case 0x0C:	// dsps
			case 0x0D:	// <CR>
				buffer[ptr]='\0' ;
				return;
				break;
			case 0x7F:	// 
			case 0xF7:	// 
			case 0xF9:	// 
			case 0xE5:	// 
			case 0xE6:	// 
			case 0xE7:	// 
				ptr++;
				buffer[ptr]=SRC[ptr];
				break;
		}
		ptr++;
	}
}

void CB_Quot( unsigned char *SRC ){		// "" ""
	char buffer[128];
	unsigned int c,d;
	int ptr,len,i=0;
	ptr=ExecPtr;
	QuotStrOpcode(SRC, buffer);
	c = SRC[ExecPtr] ; 
	if ( c == 0x0E ) {	// -> Assign str
		ExecPtr++;
		c = SRC[ExecPtr] ; 
		d = SRC[ExecPtr+1] ; 
		if ( ( c == 0x7F ) && ( d == 0xF0 ) ) { // GraphY(n)
			ExecPtr+=2;
			c = SRC[ExecPtr] ; 
			switch (c) {
				case '1':
					GraphY=GraphY1;
					break;
				case '2':
					GraphY=GraphY2;
					break;
				case '3':
					GraphY=GraphY3;
					break;
				default:
					{ ErrorNo=ArgumentERR; ErrorPtr=ExecPtr; return; }	// Argument error
					break;
			}
			GetQuotStr(SRC+ptr, GraphY);
		}
	} else {			// display str
		CB_SelectTextVRAM();	// Select Text Screen
		if ( CursorX >1 ) Scrl_Y();
		while ( buffer[i] ) {
			locate(CursorX,CursorY); CursorX++; if (CursorX > 21) Scrl_Y();
			PrintC( (unsigned char*)buffer+i );
			c = buffer[i] & 0xFF;
			if ( ( c==0x7F ) || ( c==0xF9 ) || ( c==0xE5 ) || ( c==0xE6 ) ) i++;
			i++;
		}
		if ( buffer[0]=='\0' ) CursorX=22;
		Bdisp_PutDisp_DD_DrawBusy();
	}
}


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
	if ( Previous_PX > 0 ) LinesubSetPoint(Previous_PX, Previous_PY);
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
				Fkey_Clear( 1 );
				Fkey_dispR( 2, "V-W");
				Fkey_Clear( 3 );
				Fkey_Clear( 4 );
				Fkey_dispN( 5, "G<>T");
				if (key2==0) GetKey(&key2);
				switch (key2) {
					case KEY_CTRL_SETUP:
							SetupG();
							key=0; key2=0;
							break;
					case KEY_CTRL_F1:
							SetVar(0);		// A - 
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
			default:
				key=0; key2=0;
				break;
		}
	}
	return key;
}

int CB_Disps( unsigned char *SRC ,int dspflag){
	char buffer[32];
	unsigned int c;
	unsigned int key=0;
	int scrmode;
	
	KeyRecover();
	scrmode=ScreenMode;
	if ( dspflag == 2 ) { CB_SelectTextVRAM();	// Select Text Screen
		if ( CursorX >1 ) Scrl_Y();
		sprintGR(buffer, CB_CurrentValue, 22-CursorX,RIGHT_ALIGN, CB_Round.MODE, CB_Round.DIGIT);
		locate( CursorX, CursorY); Print((unsigned char*)buffer);
		CursorX=21;
		scrmode=ScreenMode;
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

void CB_end( unsigned char *SRC, int dspflag ){
	char buffer[32];
	unsigned int c,t;
	unsigned int key=0;
	int scrmode=ScreenMode;

	if ( ProgEntryPtr ) return ; //	in sub Prog
	ExecPtr++;
	
	KeyRecover();
	CB_SelectTextVRAM();	// Select Text Screen
	if ( dspflag == 2 ) {
		if ( CursorX >1 ) Scrl_Y();
		sprintGR(buffer, CB_CurrentValue, 22-CursorX,RIGHT_ALIGN, CB_Round.MODE, CB_Round.DIGIT);
		locate( CursorX, CursorY); Print((unsigned char*)buffer);
		CursorX=21;
	}
	if ( dspflag == 0 ) CB_Done();
	if ( scrmode ) CB_SelectGraphVRAM();	// Select Graphic Screen
	if ( UseGraphic ) if ( ( UseGraphic != 1 ) && ( UseGraphic != 99 ) ) { UseGraphic=UseGraphic&0xFF; CB_SelectTextVRAM(); }
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
		sprintGR(buffer, (double)(CB_TicksEnd-CB_TicksStart)/128.0, 8,RIGHT_ALIGN, Fix, 2);  // Fix:2
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


void CB_Locate( unsigned char *SRC ){
	char buffer[128];
	unsigned int c;
	int lx,ly;
	double value;

	CB_SelectTextVRAM();	// Select Text Screen
	lx = CB_Eval(SRC+ExecPtr);
	c=SRC[ExecPtr];
	if ( c != ',' ) { ErrorNo=SyntaxERR; ErrorPtr=ExecPtr; return; }	// Syntax error
	ExecPtr++;
	ly = CB_Eval(SRC+ExecPtr);
	c=SRC[ExecPtr];
	if ( c != ',' ) { ErrorNo=SyntaxERR; ErrorPtr=ExecPtr; return; }	// Syntax error
	ExecPtr++;
	c=SRC[ExecPtr];
	if ( c == 0x22 ) {	// String
		ExecPtr++;
		QuotStrOpcode(SRC, buffer);
		locate(lx,ly);
		Print( (unsigned char*)buffer );
	} else {			// expression
		value = CB_Eval(SRC+ExecPtr);
		sprintGR(buffer, value, 22-lx,LEFT_ALIGN, CB_Round.MODE, CB_Round.DIGIT);
		locate(lx,ly);
		Print( (unsigned char*)buffer );
	}
	Bdisp_PutDisp_DD_DrawBusy_through(SRC);
}

void CB_Text( unsigned char *SRC, int *dspflag ) { //	Text
	unsigned int key;
	char buffer[128];
	unsigned int c;
	int px,py,d;
	double value;

	if ( RangeErrorCK(SRC) ) return;
	CB_SelectGraphVRAM();	// Select Graphic Screen
	py=CB_Eval(SRC+ExecPtr);
	c=SRC[ExecPtr];
	if ( c != ',' ) { ErrorNo=SyntaxERR; ErrorPtr=ExecPtr; return; }	// Syntax error
	ExecPtr++;
	px=CB_Eval(SRC+ExecPtr);
	c=SRC[ExecPtr];
	if ( c != ',' ) { ErrorNo=SyntaxERR; ErrorPtr=ExecPtr; return; }	// Syntax error
	ExecPtr++;
	c=SRC[ExecPtr];
	if ( c == 0x22 ) {	// String
		ExecPtr++;
		QuotStrOpcode(SRC, buffer);
		Text(py, px, buffer);
	} else {			// expression
		value = CB_Eval(SRC+ExecPtr);
		d=(128-px)/4;
		if (d>21) d=21;	// digit max
		sprintGR(buffer, value, d,LEFT_ALIGN, CB_Round.MODE, CB_Round.DIGIT);
		Text(py, px, buffer);
	}
	Bdisp_PutDisp_DD_DrawBusy_through(SRC);
}

//-----------------------------------------------------------------------------

void Skip_quot( unsigned char *SRC ){ // skip "..."
	unsigned int c;
	while (1){
		c=SRC[ExecPtr++];
		switch ( c ) {
			case 0x00:	// <EOF>
				ExecPtr--;
			case 0x22:	// "
			case 0x3A:	// <:>
			case 0x0C:	// dsps
			case 0x0D:	// <CR>
				return ;
				break;
			case 0x7F:	// 
			case 0xF7:	// 
			case 0xF9:	// 
			case 0xE5:	// 
			case 0xE6:	// 
			case 0xE7:	// 
				ExecPtr++;
				break;
		}
	}
}
void Skip_block( unsigned char *SRC ){
	unsigned int c;
	while (1){
		c=SRC[ExecPtr++];
		switch ( c ) {
			case 0x00:	// <EOF>
				ExecPtr--;
			case 0x3A:	// <:>
			case 0x0C:	// dsps
			case 0x0D:	// <CR>
				return ;
				break;
			case 0x22:	// "
				Skip_quot(SRC);
				break;
			case 0x7F:	// 
			case 0xF7:	// 
			case 0xF9:	// 
			case 0xE5:	// 
			case 0xE6:	// 
			case 0xE7:	// 
				ExecPtr++;
				break;
		}
	}
}

//-----------------------------------------------------------------------------

void CB_Lbl( unsigned char *SRC, int *StackGotoAdrs ){
	unsigned int c;
	int label;
	c=SRC[ExecPtr];
	if ( ( '0' <= c ) && ( c <= '9' ) ) {
		ExecPtr++;
		label = c-'0';
	} else if ( ( 'A' <= c ) && ( c <= 'Z' ) ) {
		ExecPtr++;
		label = c-'A'+10;
	} else { ErrorNo=SyntaxERR; ErrorPtr=ExecPtr; return; }	// goto error
	if ( StackGotoAdrs[label] == 0 ) StackGotoAdrs[label]=ExecPtr;
}

int Search_Lbl( unsigned char *SRC, unsigned int lc ){
	unsigned int c;
	int bptr=ExecPtr;
	ExecPtr=0;
	while (1){
		c=SRC[ExecPtr++];
		switch ( c ) {
			case 0x00:	// <EOF>
				ExecPtr=bptr;
				return 0 ;
				break;
			case 0x27:	// ' rem
				Skip_block(SRC);
				break;
			case 0xE2:	// Lbl
				c=SRC[ExecPtr++];
				if ( c == lc ) return 1 ;
				break;
			case 0xF7:	// 
			case 0x7F:	// 
			case 0xF9:	// 
			case 0xE5:	// 
			case 0xE6:	// 
			case 0xE7:	// 
				ExecPtr++;
				break;
		}
	}
	ExecPtr=bptr;
	return 0;
}

void CB_Goto( unsigned char *SRC, int *StackGotoAdrs){
	unsigned int c;
	int label;
	int ptr;
	c=SRC[ExecPtr];
	if ( ( '0' <= c ) && ( c <= '9' ) ) {
		ExecPtr++;
		label = c-'0';
	} else if ( ( 'A' <= c ) && ( c <= 'Z' ) ) {
		ExecPtr++;
		label = c-'A'+10;
	} else { ErrorNo=SyntaxERR; ErrorPtr=ExecPtr; return; }	// goto error
	
	ptr = StackGotoAdrs[label] ;
	if ( ptr == 0 ) {
		if ( Search_Lbl(SRC, c) == 0 ) { ErrorNo=GoERR; ErrorPtr=ExecPtr; return; }	// undefined label error
		StackGotoAdrs[label]=ExecPtr;
	} else  ExecPtr = ptr ;
}

//-----------------------------------------------------------------------------
int Search_IfEnd( unsigned char *SRC ){
	unsigned int c;
	while (1){
		c=SRC[ExecPtr++];
		switch ( c ) {
			case 0x00:	// <EOF>
				ExecPtr--;
				return 0 ;
				break;
			case 0x27:	// ' rem
				Skip_block(SRC);
				break;
			case 0xF7:	// 
				if ( SRC[ExecPtr] == 0x00 ) { ExecPtr++;				// If
					Search_IfEnd(SRC);
					break;
				}
				if ( SRC[ExecPtr] == 0x03 ) { ExecPtr++; return 1; }	// IfEnd
			case 0x7F:	// 
			case 0xF9:	// 
			case 0xE5:	// 
			case 0xE6:	// 
			case 0xE7:	// 
				ExecPtr++;
				break;
		}
	}
	return 0;
}

int Search_Else( unsigned char *SRC ){
	unsigned int c;
	int sPtr=ExecPtr;	// save Ptr
	while (1){
		c=SRC[ExecPtr++];
		switch ( c ) {
			case 0x00:	// <EOF>
				ExecPtr=sPtr;	// restore Ptr
				return 0 ;
				break;
			case 0x27:	// ' rem
				Skip_block(SRC);
				break;
			case 0xF7:	// 
				if ( SRC[ExecPtr] == 0x00 ) { ExecPtr++;				// If
					Search_IfEnd(SRC);
					break;
				}
				if ( SRC[ExecPtr] == 0x02 ) { ExecPtr++; return 1; }	// Else
			case 0x7F:	// 
			case 0xF9:	// 
			case 0xE5:	// 
			case 0xE6:	// 
			case 0xE7:	// 
				ExecPtr++;
				break;
		}
	}
	ExecPtr=sPtr;	// restore Ptr
	return 0;
}

//-----------------------------------------------------------------------------
void CB_If( unsigned char *SRC ){
	unsigned int c,c2;
	double value;
	value = CB_Eval(SRC+ExecPtr);
	c =SRC[ExecPtr];
	if ( ( c == ':'  ) || ( c == 0x0D ) )  { ExecPtr++;  c=SRC[ExecPtr]; }
	c2=SRC[ExecPtr+1];
	if ( ( c != 0xF7 ) || ( c2 != 0x01 ) ) { ErrorNo=SyntaxERR; ErrorPtr=ExecPtr; return; } // not Then error 
	ExecPtr+=2 ;
	if ( value==0 ) {		// false
		if ( Search_Else(SRC) ) return ;	// Else
		if ( Search_IfEnd(SRC) ) return;	// IfEnd
	}
}

void CB_Else( unsigned char *SRC ){
	Search_IfEnd(SRC);
}

void CB_IfEnd( unsigned char *SRC ){
}

//-----------------------------------------------------------------------------
void CB_For( unsigned char *SRC ,int *StackForPtr, int *StackForAdrs, int *StackForVar, double *StackForEnd, double *StackForStep, int *CurrentStructCNT, int *CurrentStructloop){
	unsigned int c;
	CB_CurrentValue = CB_Eval(SRC+ExecPtr);
	c=SRC[ExecPtr];
	if ( c == 0x0E ) {	// ->
		ExecPtr++;
		c=SRC[ExecPtr];
		StackForVar[*StackForPtr]=c-'A';
		CB_Store(SRC);
	}
	if ( *StackForPtr >= StackForMax-1 ) { ErrorNo=NestingERR; ErrorPtr=ExecPtr; return; } //  nesting error
	c=SRC[ExecPtr];
	if ( ( c != 0xF7 ) || ( SRC[ExecPtr+1] != 0x05 ) ) { ErrorNo=SyntaxERR; ErrorPtr=ExecPtr; return; }	// Syntax error
	ExecPtr+=2;
	StackForEnd[*StackForPtr] = CB_Eval(SRC+ExecPtr);
	c=SRC[ExecPtr];
	if ( ( c == 0xF7 ) && ( SRC[ExecPtr+1] == 0x06 ) ) {	// Step
		ExecPtr+=2;
		StackForStep[*StackForPtr] = CB_Eval(SRC+ExecPtr);
	} else {
		StackForStep[*StackForPtr] = 1;
	}
	StackForAdrs[*StackForPtr] = ExecPtr;
	(*StackForPtr)++;
	CurrentStructloop[*CurrentStructCNT]=1;
	(*CurrentStructCNT)++;
}

void CB_Next( unsigned char *SRC ,int *StackForPtr, int *StackForAdrs, int *StackForVar, double *StackForEnd, double *StackForStep, int *CurrentStructCNT, int *CurrentStructloop ){
	double step,end;
	if ( *StackForPtr <= 0 ) { ErrorNo=SyntaxERR; ErrorPtr=ExecPtr; return; } // Next without for error
	(*StackForPtr)--;
	(*CurrentStructCNT)--;
	step = StackForStep[*StackForPtr];
	REG[StackForVar[*StackForPtr]]+=step;
	if ( step > 0 ) { 	// step +
		if ( REG[StackForVar[*StackForPtr]] > StackForEnd[*StackForPtr] ) return ; // exit
		ExecPtr = StackForAdrs[*StackForPtr];
		(*StackForPtr)++;		// continue
	CurrentStructloop[*CurrentStructCNT]=1;
	(*CurrentStructCNT)++;
	}
	else {									// step -
		if ( REG[StackForVar[*StackForPtr]] < StackForEnd[*StackForPtr] ) return ; // exit
		ExecPtr = StackForAdrs[*StackForPtr];
		(*StackForPtr)++;		// continue
	CurrentStructloop[*CurrentStructCNT]=1;
	(*CurrentStructCNT)++;
	}
}
//-----------------------------------------------------------------------------
int Search_Next( unsigned char *SRC ){
	unsigned int c;
	while (1){
		c=SRC[ExecPtr++];
		switch ( c ) {
			case 0x00:	// <EOF>
				ExecPtr--;
				return 0 ;
				break;
			case 0x27:	// ' rem
				Skip_block(SRC);
				break;
			case 0xF7:	// 
				if ( SRC[ExecPtr] == 0x04 ) { ExecPtr++;				// For
					Search_Next(SRC);
					break;
				}
				if ( SRC[ExecPtr] == 0x07 ) { ExecPtr++; return 1; }	// Next
			case 0x7F:	// 
			case 0xF9:	// 
			case 0xE5:	// 
			case 0xE6:	// 
			case 0xE7:	// 
				ExecPtr++;
				break;
		}
	}
	return 0;
}
int Search_WhileEnd( unsigned char *SRC ){
	unsigned int c;
	while (1){
		c=SRC[ExecPtr++];
		switch ( c ) {
			case 0x00:	// <EOF>
				ExecPtr--;
				return 0 ;
				break;
			case 0x27:	// ' rem
				Skip_block(SRC);
				break;
			case 0xF7:	// 
				if ( SRC[ExecPtr] == 0x08 ) { ExecPtr++;				// While
					Search_WhileEnd(SRC);
					break;
				}
				if ( SRC[ExecPtr] == 0x09 ) { ExecPtr++; return 1; }	// WhileEnd
			case 0x7F:	// 
			case 0xF9:	// 
			case 0xE5:	// 
			case 0xE6:	// 
			case 0xE7:	// 
				ExecPtr++;
				break;
		}
	}
	return 0;
}
int Search_LpWhile( unsigned char *SRC ){
	unsigned int c;
	while (1){
		c=SRC[ExecPtr++];
		switch ( c ) {
			case 0x00:	// <EOF>
				ExecPtr--;
				return 0 ;
				break;
			case 0x27:	// ' rem
				Skip_block(SRC);
				break;
			case 0xF7:	// 
				if ( SRC[ExecPtr] == 0x0A ) { ExecPtr++;				// Do
					Search_LpWhile(SRC);
					break;
				}
				if ( SRC[ExecPtr] == 0x0B ) { ExecPtr++; return 1; }	// LpWhile
			case 0x7F:	// 
			case 0xF9:	// 
			case 0xE5:	// 
			case 0xE6:	// 
			case 0xE7:	// 
				ExecPtr++;
				break;
		}
	}
	return 0;
}

void CB_While( unsigned char *SRC, int *StackWhilePtr, int *StackWhileAdrs, int *CurrentStructCNT, int *CurrentStructloop  ) {
	int wPtr=ExecPtr;
	if ( CB_Eval(SRC+wPtr) == 0 ) {		// false
		if ( Search_WhileEnd(SRC) == 0 ) { ErrorNo=SyntaxERR; ErrorPtr=ExecPtr; return; }  // While without WhileEnd error
		return ; // exit
	}
	if ( *StackWhilePtr >= StackWhileMax-1 ) { ErrorNo=NestingERR; ErrorPtr=ExecPtr; return; }  //  nesting error
	StackWhileAdrs[*StackWhilePtr] = wPtr;
	(*StackWhilePtr)++;
	CurrentStructloop[*CurrentStructCNT]=2;
	(*CurrentStructCNT)++;
}

void CB_WhileEnd( unsigned char *SRC, int *StackWhilePtr, int *StackWhileAdrs, int *CurrentStructCNT, int *CurrentStructloop  ) {
	int exitPtr=ExecPtr;
	if ( *StackWhilePtr <= 0 ) { ErrorNo=SyntaxERR; ErrorPtr=ExecPtr; return; }  // WhileEnd without While error
	(*StackWhilePtr)--;
	(*CurrentStructCNT)--;
	ExecPtr = StackWhileAdrs[*StackWhilePtr] ;
	if ( CB_Eval(SRC+ExecPtr) == 0 ) {		// false
		ExecPtr=exitPtr;
		return ; // exit
	}
	(*StackWhilePtr)++;
	CurrentStructloop[*CurrentStructCNT]=2;
	(*CurrentStructCNT)++;
}

void CB_Do( unsigned char *SRC, int *StackDoPtr, int *StackDoAdrs, int *CurrentStructCNT, int *CurrentStructloop ) {
	if ( *StackDoPtr >= StackDoMax-1 ) { ErrorNo=NestingERR; ErrorPtr=ExecPtr; return; }  //  nesting error
	StackDoAdrs[*StackDoPtr] = ExecPtr;
	(*StackDoPtr)++;
	CurrentStructloop[*CurrentStructCNT]=3;
	(*CurrentStructCNT)++;
}

void CB_LpWhile( unsigned char *SRC, int *StackDoPtr, int *StackDoAdrs, int *CurrentStructCNT, int *CurrentStructloop ) {
	if ( *StackDoPtr <= 0 ) { ErrorNo=SyntaxERR; ErrorPtr=ExecPtr; return; }  // LpWhile without Do error
	(*StackDoPtr)--;
	(*CurrentStructCNT)--;
	if ( CB_Eval(SRC+ExecPtr) == 0  ) return ; // exit
	ExecPtr = StackDoAdrs[*StackDoPtr] ;				// true
	(*StackDoPtr)++;
	CurrentStructloop[*CurrentStructCNT]=3;
	(*CurrentStructCNT)++;
}

void CB_Break( unsigned char *SRC, int *StackForPtr, int *StackWhilePtr, int *StackDoPtr, int *CurrentStructCNT, int *CurrentStructloop ) {
	
	if ( *CurrentStructCNT <=0 ) { ErrorNo=SyntaxERR; ErrorPtr=ExecPtr; return; }  // For without Next error
	(*CurrentStructCNT)--;
	switch ( CurrentStructloop[*CurrentStructCNT] ) {
		case 1:	// For Next
			if ( Search_Next(SRC) == 0 )     { ErrorNo=SyntaxERR; ErrorPtr=ExecPtr; return; }  // For without Next error
			(*StackForPtr)--;
			return ;
			break;
		case 2:	// While WhileEnd
			if ( Search_WhileEnd(SRC) == 0 ) { ErrorNo=SyntaxERR; ErrorPtr=ExecPtr; return; }  // While without WhileEnd error
			(*StackWhilePtr)--;
			return ;
			break;
		case 3:	// DO LpWhile
			if ( Search_LpWhile(SRC) == 0 )  { ErrorNo=SyntaxERR; ErrorPtr=ExecPtr; return; }  // Do without LpWhile error
			(*StackDoPtr)--;
			return ;
			break;
		default:
			break;
	}
}

//-----------------------------------------------------------------------------
void CB_Cond( unsigned char *SRC ) { //	=> Conditional jump
	unsigned int c;
	if ( CB_CurrentValue ) {		// true
		return ;
	} else {							// false
		Skip_block(SRC);
	}
}

void CB_Dsz( unsigned char *SRC ) { //	Dsz
	unsigned int c;
	int reg;
	c=SRC[ExecPtr];
	if ( ( 'A' <= c ) && ( c <= 'Z' ) ) {
		ExecPtr++;
		reg=c-'A';
		REG[reg] -- ;
		c=SRC[ExecPtr];
		if ( ( c==':' ) || ( c==0x0D ) ) {
			ExecPtr++;
			if ( REG[reg] ) return ;
			else {
				Skip_block(SRC);
				return ;
			}
		}
	}
	{ ErrorNo=SyntaxERR; ErrorPtr=ExecPtr; return; }  // Syntax error
}

void CB_Isz( unsigned char *SRC ) { //	Isz
	unsigned int c;
	int reg;
	c=SRC[ExecPtr];
	if ( ( 'A' <= c ) && ( c <= 'Z' ) ) {
		ExecPtr++;
		reg=c-'A';
		REG[reg] ++ ;
		c=SRC[ExecPtr];
		if ( ( c==':' ) || ( c==0x0D ) ) {
			ExecPtr++;
			if ( REG[reg] ) return ;
			else {
				Skip_block(SRC);
				return ;
			}
		}
	}
	{ ErrorNo=SyntaxERR; ErrorPtr=ExecPtr; return; }  // Syntax error
}


//----------------------------------------------------------------------------------------------
void CB_MatrixInit( unsigned char *SRC ) { //	{n,m}->Dim Mat A
	unsigned int c,d;
	int dimA,dimB,i;
	int reg;
	double *ptr;
	
	dimA=(CB_Eval(SRC+ExecPtr));
	c=SRC[ExecPtr];
	if ( c != ',' ) { ErrorNo=SyntaxERR; ErrorPtr=ExecPtr; return; }  // Syntax error
	ExecPtr++;
	dimB=(CB_Eval(SRC+ExecPtr));
	c=SRC[ExecPtr];
	if ( c != '}' ) { ErrorNo=SyntaxERR; ErrorPtr=ExecPtr; return; }  // Syntax error
	ExecPtr++;
	c=SRC[ExecPtr];
	if ( c != 0x0E ) { ErrorNo=SyntaxERR; ErrorPtr=ExecPtr; return; }  // Syntax error
	ExecPtr++;
	c =SRC[ExecPtr];
	d =SRC[ExecPtr+1];
	if ( ( c != 0x7F ) || ( d !=0x46 ) ) { ErrorNo=SyntaxERR; ErrorPtr=ExecPtr; return; }  // no Dim  Syntax error
	ExecPtr+=2;
	c =SRC[ExecPtr];
	d =SRC[ExecPtr+1];
	if ( ( c != 0x7F ) || ( d !=0x40 ) ) { ErrorNo=SyntaxERR; ErrorPtr=ExecPtr; return; }  // no Mat  Syntax error
	ExecPtr+=2;
	c =SRC[ExecPtr];
	if ( ( 'A' <= c ) && ( c <= 'Z' ) ) {
		ExecPtr++;
		reg=c-'A';
		if ( ( MatAry[reg] != NULL ) && ( MatArySizeA[reg] >= dimA ) && ( MatArySizeB[reg] >= dimB ) ) { // already exist
			ptr = MatAry[reg] ;							// Matrix array ptr*
		} else {
			if ( MatAry[reg] != NULL ) 	free(MatAry[reg]);	// free
			MatArySizeA[reg]=dimA;						// Matrix array size
			MatArySizeB[reg]=dimB;						// Matrix array size
			ptr = (double *)malloc( dimA*dimB*sizeof(double) );
	   		if( ptr == NULL ) { ErrorNo=MemoryERR; ErrorPtr=ExecPtr; return; }	// memory error
			MatAry[reg] = ptr ;							// Matrix array ptr*
		}
	} else { ErrorNo=SyntaxERR; ErrorPtr=ExecPtr; return; }  // Syntax error

	for (i=0; i< dimA*dimB; i++) ptr[i]=0;	// initialize

}
void CB_ClrMat( unsigned char *SRC ) { //	ClrMat A
	unsigned int c,c2;
	int dimA,dimB;
	int reg;
	double *ptr;

	c =SRC[ExecPtr];
	if ( ( 'A' <= c ) && ( c <= 'Z' ) ) {
		ExecPtr++;
		reg=c-'A';
			ptr = MatAry[reg] ;							// Matrix array ptr*
			if (ptr != NULL ) free(ptr);		
			MatArySizeA[reg]=0;							// Matrix array size
			MatArySizeB[reg]=0;							// Matrix array size
			MatAry[reg]=NULL ;							// Matrix array ptr*
	} else {
		for ( reg=0; reg<26; reg++){
			ptr = MatAry[reg] ;							// Matrix array ptr*
			if (ptr != NULL ) free(ptr);		
			MatArySizeA[reg]=0;							// Matrix array size
			MatArySizeB[reg]=0;							// Matrix array size
			MatAry[reg]=NULL ;							// Matrix array ptr*		
		}
	}
}

//----------------------------------------------------------------------------------------------

void CB_ViewWindow( unsigned char *SRC ) { //	ViewWindow
	unsigned int c;
	int reg=26;
	while ( reg <= 34 ) {
		c=SRC[ExecPtr];
		if ( (c==0x0C) || (c==0x0D) || (c==0x00) ) break;
		REG[reg]=CB_Eval(SRC+ExecPtr);
		c=SRC[ExecPtr];
		if ( (c==0x0C) || (c==0x0D) || (c==0x00) ) break;
		if ( c != ',' ) { ErrorNo=SyntaxERR; ErrorPtr=ExecPtr; return; }  // Syntax error
		ExecPtr++;
		reg++;
	}
	CB_SelectGraphVRAM();	// Select Graphic Screen
	ViewWindow( Xmin, Xmax, Xscl, Ymin, Ymax, Yscl);
	CB_SelectTextVRAM();	// Select Text Screen
//	Bdisp_PutDisp_DD_DrawBusy_skip();
}

void CB_GridOff() {
	Grid=0;
}
void CB_GridOn() {
	Grid=1;
}
void CB_AxesOff() {
	Axes=0;
}
void CB_AxesOn() {
	Axes=1;
}
void CB_CoordOff() {
	Coord=0;
}
void CB_CoordOn() {
	Coord=1;
}
void CB_LabelOff() {
	Label=0;
}
void CB_LabelOn() {
	Label=1;
}

void CB_S_L_Normal() { //	S-L-Normal
	S_L_Style = S_L_Normal;
}
void CB_S_L_Thick() { //	S-L-Thick
	S_L_Style = S_L_Thick;
}
void CB_S_L_Broken() { //	S-L-Broken
	S_L_Style = S_L_Broken;
}
void CB_S_L_Dot() { //	S-L-Dot
	S_L_Style = S_L_Dot;
}
void CB_SketchNormal() { //	SketchNormal
	tmp_Style = S_L_Normal;
}
void CB_SketchThick() { //	SketchThick
	tmp_Style = S_L_Thick;
}
void CB_SketchBroken() { //	SketchBroken
	tmp_Style = S_L_Broken;
}
void CB_SketchDot() { //	SketchDot
	tmp_Style = S_L_Dot;
}

void CB_FLine( unsigned char *SRC) { //	F-Line
	int c;
	double x1,y1,x2,y2;
	int style=S_L_Style;

	if ( RangeErrorCK(SRC) ) return;
	x1=CB_Eval(SRC+ExecPtr);
	c=SRC[ExecPtr];
	if ( c != ',' ) { ErrorNo=SyntaxERR; ErrorPtr=ExecPtr; return; }  // Syntax error
	ExecPtr++;
	y1=CB_Eval(SRC+ExecPtr);
	c=SRC[ExecPtr];
	if ( c != ',' ) { ErrorNo=SyntaxERR; ErrorPtr=ExecPtr; return; }  // Syntax error
	ExecPtr++;
	x2=CB_Eval(SRC+ExecPtr);
	c=SRC[ExecPtr];
	if ( c != ',' ) { ErrorNo=SyntaxERR; ErrorPtr=ExecPtr; return; }  // Syntax error
	ExecPtr++;
	y2=CB_Eval(SRC+ExecPtr);

	if ( tmp_Style >= 0 ) style=tmp_Style;
	CB_SelectGraphVRAM();	// Select Graphic Screen
	F_Line(x1, y1, x2, y2, style);
	Bdisp_PutDisp_DD_DrawBusy_skip_through(SRC);
	tmp_Style = -1;
}

void CB_Line( unsigned char *SRC ) { //	Line
	int style=S_L_Style;
	if ( tmp_Style >= 0 ) style=tmp_Style;
	if ( RangeErrorCK(SRC) ) return;
	CB_SelectGraphVRAM();	// Select Graphic Screen
	Line(style);
	Bdisp_PutDisp_DD_DrawBusy_skip_through(SRC);
	tmp_Style = -1;
}

void CB_Vertical( unsigned char *SRC ) { //	Vertical
	double x;
	int style=S_L_Style;
	if ( tmp_Style >= 0 ) style=tmp_Style;
	if ( RangeErrorCK(SRC) ) return;
	x=CB_Eval(SRC+ExecPtr);
	CB_SelectGraphVRAM();	// Select Graphic Screen
	Vertical(x, style);
	Bdisp_PutDisp_DD_DrawBusy_skip_through(SRC);
	tmp_Style = -1;
}
void CB_Horizontal( unsigned char *SRC ) { //	Horizontal
	double y;
	int style=S_L_Style;
	if ( tmp_Style >= 0 ) style=tmp_Style;
	if ( RangeErrorCK(SRC) ) return;
	y=CB_Eval(SRC+ExecPtr);
	CB_SelectGraphVRAM();	// Select Graphic Screen
	Horizontal(y, style);
	Bdisp_PutDisp_DD_DrawBusy_skip_through(SRC);
	tmp_Style = -1;
}

void CB_Plot( unsigned char *SRC ) { //	Plot
	unsigned int c;
	double x,y;
	
	if ( RangeErrorCK(SRC) ) return;
	
	CB_SelectGraphVRAM();	// Select Graphic Screen
	c=SRC[ExecPtr];
	if ( ( c==':' ) || (c==0x0D) || ( c==0x0C ) || (c==0x00) ) {
		x=(Xmax+Xmin)/2;
		y=(Ymax+Ymin)/2;
	} else {
		x=CB_Eval(SRC+ExecPtr);
		c=SRC[ExecPtr];
		if ( c != ',' ) { ErrorNo=SyntaxERR; ErrorPtr=ExecPtr; return; }  // Syntax error
		ExecPtr++;
		y=CB_Eval(SRC+ExecPtr);
	}
	Plot_X = x;
	Plot_Y = y;
	regX = x;
	regY = y;
	PlotPreviousPXY();
	Bdisp_PutDisp_DD_DrawBusy_skip_through(SRC);
}

void CB_PlotOprand( unsigned char *SRC, double  *x, double *y) {
	*x=CB_Eval(SRC+ExecPtr);
	if ( SRC[ExecPtr] != ',' ) { ErrorNo=SyntaxERR; ErrorPtr=ExecPtr; return; }  // Syntax error
	ExecPtr++;
	*y=CB_Eval(SRC+ExecPtr);
	CB_SelectGraphVRAM();	// Select Graphic Screen
}

void CB_PlotOn( unsigned char *SRC ) { //	PlotOn
	double x,y;
	if ( RangeErrorCK(SRC) ) return;
	CB_PlotOprand( SRC, &x, &y);
	PlotOn_VRAM(x,y);
	Bdisp_PutDisp_DD_DrawBusy_skip_through(SRC);
}
void CB_PlotOff( unsigned char *SRC ) { //	PlotOff
	double x,y;
	if ( RangeErrorCK(SRC) ) return;
	CB_PlotOprand( SRC, &x, &y);
	PlotOff_VRAM(x,y);
	Bdisp_PutDisp_DD_DrawBusy_skip_through(SRC);
}
void CB_PlotChg( unsigned char *SRC ) { //	PlotChg
	double x,y;
	if ( RangeErrorCK(SRC) ) return;
	CB_PlotOprand( SRC, &x, &y);
	PlotChg_VRAM(x,y);
	Bdisp_PutDisp_DD_DrawBusy_skip_through(SRC);
}

void CB_PxlOprand( unsigned char *SRC, int *py, int *px) {
	double x,y;
	y = (CB_Eval(SRC+ExecPtr));
	*py=y;
	if ( ( y-floor(y) ) || ( (*py)<1 ) || ( (*py)>63 ) ) { ErrorNo=ArgumentERR; ErrorPtr=ExecPtr; return; }  // Argument error
	if ( SRC[ExecPtr] != ',' ) { ErrorNo=SyntaxERR; ErrorPtr=ExecPtr; return; }  // Syntax error
	ExecPtr++;
	x = (CB_Eval(SRC+ExecPtr));
	*px=x;
	if ( ( x-floor(x) ) || ( (*px)<1 ) || ( (*px)>127 ) ) { ErrorNo=ArgumentERR; ErrorPtr=ExecPtr; return; }  // Argument error}
	CB_SelectGraphVRAM();	// Select Graphic Screen
}
void CB_PxlOn( unsigned char *SRC ) { //	PxlOn
	int x,y;
	if ( RangeErrorCK(SRC) ) return;
	CB_PxlOprand( SRC, &y, &x);
	PxlOn_VRAM(y,x);
	Bdisp_PutDisp_DD_DrawBusy_skip_through(SRC);
}
void CB_PxlOff( unsigned char *SRC ) { //	PxlOff
	int x,y;
	if ( RangeErrorCK(SRC) ) return;
	CB_PxlOprand( SRC, &y, &x);
	PxlOff_VRAM(y,x);
	Bdisp_PutDisp_DD_DrawBusy_skip_through(SRC);
}
void CB_PxlChg( unsigned char *SRC ) { //	PxlChg
	int x,y;
	if ( RangeErrorCK(SRC) ) return;
	CB_PxlOprand( SRC, &y, &x);
	PxlChg_VRAM(y,x);
	Bdisp_PutDisp_DD_DrawBusy_skip_through(SRC);
}

//----------------------------------------------------------------------------------------------
void CB_Circle( unsigned char *SRC ) { //	Circle
	unsigned int c;
	double x,y,r;
	int style=S_L_Style;
	if ( tmp_Style >= 0 ) style=tmp_Style;
	if ( RangeErrorCK(SRC) ) return;
	CB_SelectGraphVRAM();	// Select Graphic Screen
	x=CB_Eval(SRC+ExecPtr);
	c=SRC[ExecPtr];
	if ( c != ',' ) { ErrorNo=SyntaxERR; ErrorPtr=ExecPtr; return; }  // Syntax error
	ExecPtr++;
	y=CB_Eval(SRC+ExecPtr);
	c=SRC[ExecPtr];
	if ( c != ',' ) { ErrorNo=SyntaxERR; ErrorPtr=ExecPtr; return; }  // Syntax error
	ExecPtr++;
	r=CB_Eval(SRC+ExecPtr);
	Circle(x, y, r, style);
	tmp_Style = -1;
}

//----------------------------------------------------------------------------------------------
void GetGraphStr(  unsigned char *SRC ) {
	int ptr;
	int len,i;
	unsigned int c,d;
	ptr=ExecPtr;
	c=SRC[ExecPtr];
	d=SRC[ExecPtr+1];
	if ( ( c == 0x7F ) && ( d == 0xF0 ) ) { // GraphY
		ExecPtr+=2;
		c=SRC[ExecPtr++];
		switch (c) {
			case '1':
				GraphY=GraphY1;
				break;
			case '2':
				GraphY=GraphY2;
				break;
			case '3':
				GraphY=GraphY3;
				break;
			default:
				{ ErrorNo=ArgumentERR; ErrorPtr=ExecPtr; return; }	// Argument error
				break;
		}
	} else {
		CB_Eval(SRC+ExecPtr);
		len=(int)(ExecPtr-ptr);
		GraphY=GraphY1;
		for (i=0; i<len; i++ ) GraphY[i]=SRC[ptr+i];
		GraphY[i]='\0';
	}
}

void CB_DrawGraph(  unsigned char *SRC ){
	unsigned int key;
	unsigned int c;
	if ( RangeErrorCK(SRC) ) return;
	CB_SelectGraphVRAM();	// Select Graphic Screen
	key=Graph_main();
}

void CB_GraphY( unsigned char *SRC ){
	GetGraphStr(SRC);
	CB_DrawGraph(SRC);
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
	stat=SavePicture( pict, pictNo );
	return stat;
}

void RclPict( int pictNo){
	unsigned char *pict;
	int i;
	pict=LoadPicture( pictNo );
	if ( pict == NULL ) { ErrorNo=MemoryERR; ErrorPtr=ExecPtr; return; }	// Memory error
	WriteVram( pict+0x4C );
	free(pict);
}

void CB_StoPict( unsigned char *SRC ) { //	StoPict
	int n;
	CB_SelectGraphVRAM();	// Select Graphic Screen
	Bdisp_PutDisp_DD_DrawBusy();
	n=CB_Eval(SRC+ExecPtr);
	if ( (n<1) || (20<n) ){ ErrorNo=ArgumentERR; ErrorPtr=ExecPtr; return; }	// Argument error
	StoPict(n);
}
void CB_RclPict( unsigned char *SRC ) { //	RclPict
	int n;
	CB_SelectGraphVRAM();	// Select Graphic Screen
	n=CB_Eval(SRC+ExecPtr);
	if ( (n<1) || (20<n) ){ ErrorNo=ArgumentERR; ErrorPtr=ExecPtr; return; }	// Argument error
	RclPict(n);
	Bdisp_PutDisp_DD_DrawBusy();
}

//----------------------------------------------------------------------------------------------
int CB_SearchProg( char *name ) { //	Prog search
	int j,i=1;
	unsigned char *ptr;
	
	while ( ProgfileAdrs[i] ) {
		ptr=ProgfileAdrs[i]+0x3C;
		for (j=0;j<8;j++) if ( ptr[j] != name[j] ) break;
		if ( j==8 )	return i ; // ok
		i++;
	}
	return -1; // fault
}

void CB_Prog( unsigned char *SRC ) { //	Prog "..."
	unsigned int c;
	char buffer[32]="";
	unsigned char *src;
	unsigned char *StackProgSRC;
	int StackProgExecPtr;
	int stat;

	c=SRC[ExecPtr];
	if ( c == 0x22 ) {	// String
		ExecPtr++;
		QuotStrOpcode(SRC, buffer);	// Prog name
	}
	StackProgSRC     = SRC;
	StackProgExecPtr = ExecPtr;
	
	ProgNo = CB_SearchProg( buffer );
	if ( ProgNo < 0 ) { ErrorNo=GoERR; ErrorPtr=ExecPtr; return; }  // undefined Prog
	src = ProgfileAdrs[ProgNo];
	SRC = src + 0x56 ;
	ExecPtr=0;
	
	ProgEntryPtr++;
	
	stat=CB_interpreter_sub( SRC ) ;
	if ( stat > 0 ) return ;	// error or break
	
	ProgEntryPtr--;
	SRC     = StackProgSRC ;
	ExecPtr = StackProgExecPtr ;
}

//----------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------
int dspflag=0;		// 0:nondsp  1:str  2:num

int CB_interpreter_sub( unsigned char *SRC ) {
	int cont=1;
	int i;
	unsigned int c;
	
	int StackGotoAdrs[StackGotoMax];

	int	CurrentStructCNT=0;
	int	CurrentStructloop[20];

	int StackForPtr=0;
	int StackForAdrs[StackForMax];
	int StackForVar[StackForMax];
	double StackForEnd[StackForMax];
	double StackForStep[StackForMax];

	int StackWhilePtr=0;
	int StackWhileAdrs[StackWhileMax];

	int StackDoPtr=0;
	int StackDoAdrs[StackDoMax];


	for (i=0; i<StackGotoMax; i++) StackGotoAdrs[i]=0;
	ExecPtr=0;
	StackForPtr=0;
	StackWhilePtr=0;
	StackDoPtr=0;
	CurrentStructCNT=0;
	tmp_Style = -1;
	dspflag=0;
	UseGraphic=0;

	while (cont) {
		if ( ErrorNo  ) return ErrorPtr;
		if ( BreakPtr ) { CB_BreakStop(); return BreakPtr; }
		c=SRC[ExecPtr++];
		if ( c==0x00 ) { ExecPtr--; if ( ProgEntryPtr )  return -1;  else  break; }
		if ( c==':'  )   c=SRC[ExecPtr++]; 
		switch (c) {
//			case 0x3A:	// <:>
//				break;
			case 0x0D:	// <CR>
				if ( KeyScanDown(KEYSC_AC) ) { BreakPtr=ExecPtr-1; KeyRecover(); }	// [AC] break?
				break;
			case 0x0E:	// ->
				CB_Store(SRC);
				break;
			case 0xE2:	// Lbl
				CB_Lbl(SRC, StackGotoAdrs );
				break;
			case 0xEC:	// Goto
				CB_Goto(SRC, StackGotoAdrs );
				break;
			case 0xE8:	// Dsz
				CB_Dsz(SRC) ;
				break;
			case 0xE9:	// Isz
				CB_Isz(SRC) ;
				break;
			case 0x13:	// =>
				CB_Cond(SRC);
				dspflag=0;
				break;
			case 0xF7:	// F7
				c=SRC[ExecPtr++];
				switch ( c ) {
					case 0x00:	// If
						CB_If(SRC);
						break;
					case 0x02:	// Else
						CB_Else(SRC);
						break;
					case 0x03:	// IfEnd
						CB_IfEnd(SRC);
						break;
					case 0x04:	// For
						CB_For(SRC, &StackForPtr, StackForAdrs, StackForVar, StackForEnd, StackForStep, &CurrentStructCNT, CurrentStructloop );
						break;
					case 0x07:	// Next
						CB_Next(SRC, &StackForPtr, StackForAdrs, StackForVar, StackForEnd, StackForStep, &CurrentStructCNT, CurrentStructloop );
						break;
					case 0x08:	// While
						CB_While(SRC, &StackWhilePtr, StackWhileAdrs, &CurrentStructCNT, CurrentStructloop );
						break;
					case 0x09:	// WhileEnd
						CB_WhileEnd(SRC, &StackWhilePtr, StackWhileAdrs, &CurrentStructCNT, CurrentStructloop );
						break;
					case 0x0A:	// Do
						CB_Do(SRC, &StackDoPtr, StackDoAdrs, &CurrentStructCNT, CurrentStructloop );
						break;
					case 0x0B:	// LpWhile
						CB_LpWhile(SRC, &StackDoPtr, StackDoAdrs, &CurrentStructCNT, CurrentStructloop );
						break;
					case 0x0C:	// Return
						if ( ProgEntryPtr ) return -1 ; //	in sub Prog
						dspflag=0;
						cont=0;
						break;
					case 0x0D:	// Break
						CB_Break(SRC, &StackForPtr, &StackWhilePtr, &StackDoPtr, &CurrentStructCNT, CurrentStructloop );
						dspflag=0;
						break;
					case 0x0E:	// Stop
						BreakPtr=ExecPtr;
						cont=0; 
						dspflag=0;
						break;
					case 0x10:	// Locate
						CB_Locate(SRC);
						dspflag=3;
						break;
					case 0x18:	// ClrText
						CB_ClrText(SRC);
						dspflag=0;
						break;
					case 0x19:	// ClrGraph
						CB_ClrGraph(SRC);
						dspflag=0;
						UseGraphic=0;
						break;
					case 0x1C:	// S-L-Normal
						CB_S_L_Normal();
						dspflag=0;
						break;
					case 0x1D:	// S-L-Thick
						CB_S_L_Thick();
						dspflag=0;
						break;
					case 0x1E:	// S-L-Broken
						CB_S_L_Broken();
						dspflag=0;
						break;
					case 0x1F:	// S-L-Dot
						CB_S_L_Dot();
						dspflag=0;
						break;
					case 0x8C:	// SketchNormal
						CB_SketchNormal();
						dspflag=0;
						break;
					case 0x8D:	// SketchThick
						CB_SketchThick();
						dspflag=0;
						break;
					case 0x8E:	// SketchBroken
						CB_SketchBroken();
						dspflag=0;
						break;
					case 0x8F:	// SketchDot
						CB_SketchDot();
						dspflag=0;
						break;
					case 0x93:	// StoPict
						CB_StoPict(SRC);
						dspflag=0;
						UseGraphic=99;
						break;
					case 0x94:	// RclPict
						CB_RclPict(SRC);
						dspflag=0;
						UseGraphic=99;
						break;
					case 0xA3:	// Vertical
						CB_Vertical(SRC);
						dspflag=0;
						UseGraphic=99;
						break;
					case 0xA4:	// Horizontal
						CB_Horizontal(SRC);
						dspflag=0;
						UseGraphic=99;
						break;
					case 0xA5:	// Text
						dspflag=0;
						CB_Text(SRC, &dspflag);
						UseGraphic=99;
						break;
					case 0xA6:	// Circle
						CB_Circle(SRC);
						dspflag=0;
						UseGraphic=99;
						break;
					case 0xA7:	// F-Line
						CB_FLine(SRC);
						dspflag=0;
						UseGraphic=99;
						break;
					case 0xA8:	// PlotOn
						CB_PlotOn(SRC);
						dspflag=0;
						UseGraphic=99;
						break;
					case 0xA9:	// PlotOff
						CB_PlotOff(SRC);
						dspflag=0;
						UseGraphic=99;
						break;
					case 0xAA:	// PlotChg
						CB_PlotChg(SRC);
						dspflag=0;
						UseGraphic=99;
						break;
					case 0xAB:	// PxlOn
						CB_PxlOn(SRC);
						dspflag=0;
						UseGraphic=99;
						break;
					case 0xAC:	// PxlOff
						CB_PxlOff(SRC);
						dspflag=0;
						UseGraphic=99;
						break;
					case 0xAD:	// PxlChg
						CB_PxlChg(SRC);
						dspflag=0;
						UseGraphic=99;
						break;
					case 0x7A:	// GridOff
						CB_GridOff();
						dspflag=0;
						break;
					case 0x7D:	// GridOn
						CB_GridOn();
						dspflag=0;
						break;
					case 0xC2:	// AxesOn
						CB_AxesOn();
						dspflag=0;
						break;
					case 0xC3:	// CoordOn
						CB_CoordOn();
						dspflag=0;
						break;
					case 0xC4:	// LabelOn
						CB_LabelOn();
						dspflag=0;
						break;
					case 0xD2:	// AxesOff
						CB_AxesOff();
						dspflag=0;
						break;
					case 0xD3:	// CoordOff
						CB_CoordOff();
						dspflag=0;
						break;
					case 0xD4:	// LabelOff
						CB_LabelOff();
						dspflag=0;
						break;
					case 0x20:	// DrawGraph
						CB_DrawGraph(SRC);
						dspflag=0;
						UseGraphic=2;
						break;
					default:
						ExecPtr-=2;
						CB_CurrentValue = CB_Eval(SRC+ExecPtr);
						dspflag=2;
						break;
				}
				break;
			case 0xF9:	// F9
				c=SRC[ExecPtr++];
				switch ( c ) {
					case 0x0B:	// EngOn
//						CB_EngOn(SRC);
						dspflag=0;
						break;
					case 0x0C:	// EngOff
//						CB_EngOff(SRC);
						dspflag=0;
						break;
					case 0x1E:	// ClrMat
						CB_ClrMat(SRC);
						dspflag=0;
						break;
					default:
						ExecPtr-=2;
						CB_CurrentValue = CB_Eval(SRC+ExecPtr);
						dspflag=2;
						break;
				}
				break;
			case 0x22:	// " "
				CB_Quot(SRC) ;
				dspflag=1;
				break;
			case 0x3F:	// ?
				CB_Input(SRC);
				CB_TicksStart=RTC_GetTicks();	// 
				dspflag=2;
				break;
			case 0x7B:	// {
				CB_MatrixInit(SRC);
				dspflag=0;
				break;
			case 0x0C:	// dsps
				if ( CB_Disps(SRC, dspflag) ) BreakPtr=ExecPtr ;  // [AC] break
				break;
			case 0xD1:	// Cls
				CB_Cls(SRC);
				dspflag=0;
				UseGraphic=0;
				break;
			case 0xDA:	// Deg
				CB_Deg();
				dspflag=0;
				break;
			case 0xDB:	// Rad
				CB_Rad();
				dspflag=0;
				break;
			case 0xDC:	// Grad
				CB_Grad();
				dspflag=0;
				break;
			case 0xE0:	// Plot
				CB_Plot(SRC);
				dspflag=0;
				UseGraphic=1;
				break;
			case 0xE1:	// Line
				CB_Line(SRC);
				dspflag=0;
				UseGraphic=99;
				break;
			case 0xEB:	// ViewWindow
				CB_ViewWindow(SRC);
				dspflag=0;
				UseGraphic=0;
				break;
			case 0xEE:	// Graph Y=
				CB_GraphY(SRC);
				dspflag=0;
				UseGraphic=2;
				break;
			case 0xED:	// Prog "..."
				CB_Prog(SRC);
				dspflag=0;
				break;
			case 0x27:	// ' rem
				Skip_block(SRC);
				dspflag=0;
				break;
				
			case 0xD3:	// Rnd
				CB_Rnd();
				break;
			case 0xD9:	// Norm
				CB_Norm(SRC);
				break;
			case 0xE3:	// Fix
				CB_Fix(SRC);
				break;
			case 0xE4:	// Sci
				CB_Sci(SRC);
				break;
		
			default:
				ExecPtr--;
				CB_CurrentValue = CB_Eval(SRC+ExecPtr);
				dspflag=2;
				break;
		}
	}
	CB_TicksEnd=RTC_GetTicks();	// 
	CB_end(SRC, dspflag);
	return -1;
}
//----------------------------------------------------------------------------------------------
int CB_interpreter( unsigned char *SRC ) {
	int flag;
	unsigned int c;
	int stat;

	CB_TicksStart=RTC_GetTicks();	// 
	random( CB_TicksStart ) ;	// rand seed
	CB_ClrText(SRC);
	ProgEntryPtr=0;	// subroutin clear
	ErrorPtr=0;
	ErrorNo= 0;
	BreakPtr=0;
	Bdisp_PutDisp_DD_DrawBusy();
	stat = CB_interpreter_sub( SRC );
	KeyRecover(); 
    if ( ErrorNo ) { CB_ErrNo( ErrorNo ); }
	return stat;
}

//----------------------------------------------------------------------------------------------

double q_un_sub(void) {
     int i;
     double a[9];
     for (i=0;i<9;i++) a[i]=0;
     regN=1;  
 l5: regR=8;
     regS=0;
     regX=0;
 l0: if (regX==regR) goto l4;
     regX++;
     a[(int)regX]=regR;
 l1: ++regS;
     regY=regX;
 l2: if (!--regY) goto l0;
     if (!(regT=a[(int)regX]-a[(int)regY])) goto l3;
     if (regX-regY!=fabs(regT)) goto l2;
 l3: if (--a[(int)regX]) goto l1;
     if (--regX) goto l3;
 l4: if (--regN) goto l5;

  return (regS);
}

double asctt4(){
	regK=1.0;
	lbl0:
	regK=regK+1.0;
	regA=regK/2.0*regK+4.0-regK;
	if ( regK<10000 ) goto lbl0;
	return regA;;
}


void CB_test() {
	int i, s, t, result;
	char	buffer[32];
	unsigned int key=0;

	Bdisp_AllClr_DDVRAM();
	locate(1,1); Print((unsigned char*)"8queen ");
	Bdisp_PutDisp_DD();
	
	start_timer();	// --------------------------------------

	regS=q_un_sub();

	t=get_timer();	// --------------------------------------

	locate(1,2);
	sprintG(buffer, regS, 21, RIGHT_ALIGN);
		Print((unsigned char*)buffer);
	locate(1,3);sprintf(buffer,"time=%5.3fs",(float)t/1000); Print((unsigned char*)buffer);
	Bdisp_PutDisp_DD();

	
	locate(1,5); Print((unsigned char*)"Asciit4 ");
	Bdisp_PutDisp_DD();
	
	start_timer();	// --------------------------------------

	regS=asctt4();

	t=get_timer();	// --------------------------------------

	locate(1,6);
	sprintG(buffer, regS, 21, RIGHT_ALIGN);
		Print((unsigned char*)buffer);
	locate(1,7);sprintf(buffer,"time=%5.3fs",(float)t/1000); Print((unsigned char*)buffer);

	Bdisp_PutDisp_DD();
	GetKey(&key);
}


