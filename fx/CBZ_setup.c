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
#include "CB_Matrix.h"
#include "CB_Str.h"
#include "MonochromeLib.h"

//---------------------------------------------------------------------------------------------
int KeyRepeatFirstCount=20;		// pointer to repeat time of first repeat (20:default)
int KeyRepeatNextCount = 5;		// pointer to repeat time of second repeat( 5:default)

short PageUpDownNum = PageUpDownNumDefault;	// PageUp/Down counter
char  EditTopLine = 0;	// 1: use top of line (edit)
char  EditFontSize = 0;	// 0:standard  1:mini
char  EditListChar = 0;	// List character change
char  EditExtFont=0;	// Edit ext Font enable:1

char  ExtendPict=0;	// 0:20  21~99						( use hidden ram only )
char  ExtendList=0;	// 0:52(default) 1:+16 ~ 63:+1008	( use hidden ram only )
char  ForceReturnMode=0;	// 0:none  1:F1 2:EXE  3:Both
char  ForceReturn;		// 0:none  1:return
char  CB_RecoverSetup=1;	// setup recover flag 0:none 1:recover

int selectSetup=0;
int selectVar=0;
int selectMatrix=0;

const char VerMSG[]="C.Basic  v1.73\xE6\x41";

//---------------------------------------------------------------------------------------------

void VerDispSub() {
	int freearea;
	char buffer[32];
	PopUpWin( 6 );
	locate( 3, 2 ); Print( (unsigned char*)VerMSG );
	locate( 3, 3 ); Print( (unsigned char*)"(Casio Basic" );
	locate( 3, 4 ); Print( (unsigned char*)"     compatible+)" );
	locate( 3, 6 ); Print( (unsigned char*)"     by sentaro21" );
	locate( 3, 7 ); Print( (unsigned char*)"          (c)2018" );

	if ( ( UseHiddenRAM ) && ( IsHiddenRAM ) ) {
		freearea = HiddenRAM_MatTopPtr - HiddenRAM_ProgNextPtr ;
		sprintf(buffer,"%d bytes free",freearea);
		PrintMini( 2*6+2, 4*8+2, (unsigned char*)buffer, MINI_OVER );
//		locate( 3, 5 ); Print( (unsigned char*)buffer );
	}
		
	Bdisp_PutDisp_DD();
}
void VerDisp() {
	unsigned int key;
	VerDispSub();
	GetKey(&key);
}

//----------------------------------------------------------------------------------------------
//		Setup
//----------------------------------------------------------------------------------------------

void FkeyS_L_(){
	int temp;
	temp=S_L_Style;
//	FkeyClear( FKeyNo1 );
	Fkey_Icon( FKeyNo1, 977 );	//	Fkey_dispN( FKeyNo1, ""); Linesub(0*21+5,7*8+4,0*21+18,7*8+4, S_L_Normal,1);	// -----
//	FkeyClear( FKeyNo2 );
	Fkey_Icon( FKeyNo2, 978 );	//	Fkey_dispN( FKeyNo2, ""); Linesub(1*21+5,7*8+4,1*21+18,7*8+4, S_L_Thick,1);	// =====
//	FkeyClear( FKeyNo3 );
	Fkey_Icon( FKeyNo3, 979 );	//	Fkey_dispN( FKeyNo3, ""); Linesub(2*21+5,7*8+4,2*21+18,7*8+4, S_L_Broken,1);	// : : : :
//	FkeyClear( FKeyNo4 );
	Fkey_Icon( FKeyNo4, 982 );	//	Fkey_dispN( FKeyNo4, ""); Linesub(3*21+5,7*8+4,3*21+18,7*8+4, S_L_Dot,1);	// . . . .
	FkeyClear( FKeyNo5 );
	FkeyClear( FKeyNo6 );
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

void SetVeiwWindowInit(){	// Initialize	return 0: no change  -1 : change
		Xmin  =-6.3;
		Xmax  = 6.3;
		Xscl  = 1.0;
		Xdot  = 0.1;
		Ymin  =-3.1;
		Ymax  = 3.1;
		Yscl  = 1.0;
		Ydot  = 0.1;
		TThetamin = 0;
		TThetamax = 6.2831853071796;
		TThetaptch= 0.062831853071796;
}
void SetVeiwWindowTrig(){	// trig Initialize
		Xmin  = finvdegree(-540);
		Xmax  = finvdegree( 540);
		Xscl  = finvdegree(  90);
		Xdot  = (Xmax-Xmin)/126.0;
		Ymin  =-1.6;
		Ymax  = 1.6;
		Yscl  = 0.5;
		Ydot  = (Ymax-Ymin)/ 62.0;
		TThetamin = 0;
		TThetamax = finvdegree( 360);
		TThetaptch= TThetamax/1000;
}
void SetVeiwWindowSTD(){	// STD Initialize
		Xmin  =-10;
		Xmax  = 10;
		Xscl  =  1;
		Xdot  =  0.158730158730159;
		Ymin  =-10;
		Ymax  = 10;
		Yscl  =  1;
		Ydot  = (Ymax-Ymin)/ 62.0;
		TThetamin = 0;
		TThetamax = 6.2831853071796;
		TThetaptch= 0.062831853071796;
}


unsigned int MathKey( unsigned int  key) {
	if ( ( (  KEY_CHAR_0 <= key ) && ( key <= KEY_CHAR_9 ) ) ||
		 ( (  KEY_CHAR_A <= key ) && ( key <= KEY_CHAR_Z ) ) ) return key;
	switch ( key ) {
			case KEY_CHAR_DP:
			case KEY_CHAR_EXP:
			case KEY_CHAR_PLUS:
			case KEY_CHAR_MINUS:
			case KEY_CHAR_MULT:
			case KEY_CHAR_DIV:
			case KEY_CHAR_PMINUS:
			case KEY_CHAR_ANS:
			case KEY_CTRL_XTT:
			case KEY_CHAR_VALR:
			case KEY_CHAR_THETA:
			case KEY_CHAR_IMGNRY:
			case KEY_CHAR_LIST:
			case KEY_CHAR_MAT:
			case KEY_CHAR_EQUAL:
			case KEY_CHAR_FRAC:   //    0xbb
			case KEY_CHAR_LPAR:   //    0x28
			case KEY_CHAR_RPAR:   //    0x29
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
			case KEY_CHAR_LBRCKT: // 
			case KEY_CHAR_RBRCKT: //
			case KEY_CHAR_LBRACE: //
			case KEY_CHAR_RBRACE: //
			case KEY_CHAR_RECIP: //
			case KEY_CHAR_ANGLE: //
			case KEY_CHAR_DQUATE: //
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


int SetViewWindow(void){		// ----------- Set  View Window variable	return 0: no change  -1 : change
	char buffer[32];
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

	Cursor_SetFlashMode(0); 		// cursor flashing off
	while (cont) {
		Bdisp_AllClr_VRAM();
		locate( 1,1);Print((unsigned char*)"View Window");

		if ( scrl <=0 ) {
			locate( 1, 2-scrl); Print((unsigned char*)"Xmin  :");
			sprintG(buffer,Xmin,  10,LEFT_ALIGN); locate( 8, 2-scrl); Print((unsigned char*)buffer);
		}
		if ( scrl <=1 ) {
			locate( 1, 3-scrl); Print((unsigned char*)" max  :");
			sprintG(buffer,Xmax,  10,LEFT_ALIGN); locate( 8, 3-scrl); Print((unsigned char*)buffer);
		}
		if ( scrl <=2 ) {
			locate( 1, 4-scrl); Print((unsigned char*)" scale:");
			sprintG(buffer,Xscl,10,LEFT_ALIGN); locate( 8, 4-scrl); Print((unsigned char*)buffer);
		}
		if ( scrl <=3 ) {
			locate( 1, 5-scrl); Print((unsigned char*)" dot  :");
			sprintG(buffer,Xdot,  10,LEFT_ALIGN); locate( 8, 5-scrl); Print((unsigned char*)buffer);
		}
		if ( scrl <=4 ) {
			locate( 1, 6-scrl); Print((unsigned char*)"Ymin  :");
			sprintG(buffer,Ymin,  10,LEFT_ALIGN); locate( 8, 6-scrl); Print((unsigned char*)buffer);
		}
		if ( scrl <=5 ) {
			locate( 1, 7-scrl); Print((unsigned char*)" max  :");
			sprintG(buffer,Ymax,  10,LEFT_ALIGN); locate( 8, 7-scrl); Print((unsigned char*)buffer);
		}
		if ( scrl >=1 ) {
			locate( 1, 8-scrl); Print((unsigned char*)" scale:");
			sprintG(buffer,Yscl,10,LEFT_ALIGN); locate( 8, 8-scrl); Print((unsigned char*)buffer);
		}
		if ( scrl >=2 ) {
			locate( 1, 9-scrl); Print((unsigned char*)"T\xE6\x47min :");
			sprintG(buffer,TThetamin,  10,LEFT_ALIGN); locate( 8, 9-scrl); Print((unsigned char*)buffer);
		}
		if ( scrl >=3 ) {
			locate( 1, 10-scrl); Print((unsigned char*)"  max :");
			sprintG(buffer,TThetamax,  10,LEFT_ALIGN); locate( 8, 10-scrl); Print((unsigned char*)buffer);
		}
		if ( scrl >=4 ) {
			locate( 1, 11-scrl); Print((unsigned char*)"  ptch:");
			sprintG(buffer,TThetaptch,  10,LEFT_ALIGN); locate( 8, 11-scrl); Print((unsigned char*)buffer);
		}

		y = select-scrl+1;
		Bdisp_AreaReverseVRAM(0, y*8, 127, y*8+7);	// reverse select line 
		
		Fkey_Icon( FKeyNo1,  95 );	//	Fkey_dispN( FKeyNo1, "Init");
		Fkey_Icon( FKeyNo2,  96 );	//	Fkey_dispN( FKeyNo2, "Trig");
		Fkey_Icon( FKeyNo3,  97 );	//	Fkey_dispN( FKeyNo3, "STD");

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
				FkeyClear( FKeyNo1 );
				FkeyClear( FKeyNo2 );
				FkeyClear( FKeyNo3 );
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
				FkeyClear( FKeyNo1 );
				FkeyClear( FKeyNo2 );
				FkeyClear( FKeyNo3 );
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
	char buffer[32];
	unsigned int key;
	int	cont=1;
	int select=0;
	int y;

	PopUpWin(3);

	Cursor_SetFlashMode(0); 		// cursor flashing off
	while (cont) {
		locate(3,3); Print((unsigned char *)"Factor");
		locate(3,4); Print((unsigned char *) "Xfact:           ");
		sprintG(buffer,Xfct,  10,LEFT_ALIGN); locate( 9, 4); Print((unsigned char*)buffer);
		locate(3,5); Print((unsigned char *) "Yfact:           ");
		sprintG(buffer,Yfct,  10,LEFT_ALIGN); locate( 9, 5); Print((unsigned char*)buffer);

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
void SetVarDsp(int VarMode) {
	if ( VarMode ) Print((unsigned char*)"[int%]"); else Print((unsigned char*)"[dbl#]");
}
void InitVar( double value, int VarMode, int small) {
	char buffer[32];
	unsigned int key;
	int	cont=1;
	int i,reg;
	PopUpWin(3);

	Cursor_SetFlashMode(0); 		// cursor flashing off
	while (cont) {
		locate( 3,3); Print((unsigned char *)"Init All Variable");
		locate( 3,5); Print((unsigned char *)"value:           ");
		sprintG(buffer,value,  10,LEFT_ALIGN); locate( 9, 5); Print((unsigned char*)buffer);
		locate(1,8); PrintLine((unsigned char *)" ",21);
		locate(1,8); SetVarDsp(VarMode);
//		Bdisp_PutDisp_DD();

		GetKey( &key );
		switch (key) {
			case KEY_CTRL_EXIT:
				return ;
				break;
			case KEY_CTRL_EXE:
				cont=0;
				break;
			case KEY_CTRL_RIGHT:
				value  =InputNumD_full( 9, 5, 10, value);	// 
				break;
			default:
				break;
		}
		key=MathKey( key );
		if ( key ) {
			value  =InputNumD_Char( 9, 5, 10, value, key);	// 
		}
	}

	if ( YesNo("Initialize Ok?") ) {
		if ( VarMode ) {
			for ( i=0; i<26; i++) LocalInt[i+small][0]=value;
		} else {
			for ( i=0; i<26; i++) LocalDbl[i+small][0]=value;
		}
	}
}

void SetVarSel(int VarMode, int y) {
	Bdisp_AreaReverseVRAM(0, y*8, 127, y*8+7);	// reverse select line 
	locate(1,8); PrintLine((unsigned char *)" ",21);
	locate(1,8); SetVarDsp(VarMode);
}

void SetVarDblToHex( char * buffer, double n ) {
	if ( ( (n-floor(n))==0 ) && ( -2147483648. <= n ) && ( n <= 2147483647. ) ) {
		sprintf(buffer,"0x%08X        ",(int)n);
	} else {
		sprintG(buffer, n,      18,LEFT_ALIGN);
	}
}
int CmpAliasVar( int reg ) {	// AliasVar ?
	int c,i,j;
	for ( i=0; i<=AliasVarMAX; i++ ) {
		if ( AliasVarCode[i].org==reg ) return i;
		if ( AliasVarCode[i].org<0 ) return -1;
	}
	return -1;
}

int CmpAliasVarMat( int reg ) {	// AliasVar Mat?
	int c,i,j;
	for ( i=0; i<=AliasVarMAXMat; i++ ) {
		if ( AliasVarCodeMat[i].org==reg ) return i;
		if ( AliasVarCodeMat[i].org<0 ) return -1;
	}
	return -1;
}

int SetVarCharMat( char *buffer, int c ) {
	int ptr=0,len;
	short alias_code;
	char buffer2[32];
	int i=CmpAliasVarMat( c );
	if ( i>=0 ) {	// 	Alias variable
		if ( alias_code=AliasVarCodeMat[i].alias == 0x4040 ) {	// @ABCD
			len=AliasVarCodeMat[i].len;
			memcpy( buffer, AliasVarCodeMat[i].name, len);
			buffer[len]='\0';
			ptr=len;
		} else {
			CB_OpcodeToStr( alias_code, buffer2 ) ;		// SYSCALL+
			StrMid( buffer, buffer2, 1, 8 );
			ptr=CB_MB_strlen( buffer );
		}
	} else
	if ( ( c == 26 ) || ( c == 26+32 ) || ( c == 26+26+32 ) ) {	// <r>
		buffer[ptr++]=0xCD;	//	'r'
	} else
	if ( ( c == 27 ) || ( c == 27+32 ) || ( c == 27+26+32 ) ) { // Theta
		buffer[ptr++]=0xE6;		// Theta
		buffer[ptr++]=0x47;
	} else
	if ( ( ( 28 <= c ) && ( c <= 31 ) ) || ( ( 28+32 <= c ) && ( c <= 31+32 ) ) || ( ( 28+32+26 <= c ) && ( c <= 31+32+26 ) ) ) { // Ans
		buffer[ptr++]='A';		//
		buffer[ptr++]='n';
		buffer[ptr++]='s';
	} else { 
		buffer[ptr++]='A'+c;
	}
	return ptr;
}
int SetVarChar( char *buffer, int c ) {
	int ptr=0,len;
	short alias_code;
	char buffer2[32];
	int i=CmpAliasVar( c );
	if ( i>=0 ) {	// 	Alias variable
		alias_code=AliasVarCode[i].alias ;
		if ( alias_code == 0x4040 ) {	// _ABCD
			len=AliasVarCode[i].len;
			memcpy( buffer, AliasVarCode[i].name, len);
			buffer[len]='\0';
			ptr=len;
		} else {
			CB_OpcodeToStr( alias_code, buffer2 ) ;		// SYSCALL+
			StrMid( buffer, buffer2, 1, 8 );
			ptr=CB_MB_strlen( buffer );
		}
	} else
	if ( ( c == 26 ) ) {	// <r>
		buffer[ptr++]=0xCD;	//	'r'
	} else
	if ( ( c == 27 ) ) { // Theta
		buffer[ptr++]=0xE6;		// Theta
		buffer[ptr++]=0x47;
	} else
	if ( ( 28 <= c ) && ( c <= 31 ) ) { // Ans
		buffer[ptr++]='A';		//
		buffer[ptr++]='n';
		buffer[ptr++]='s';
	} else { 
		buffer[ptr++]='A'+c;
	}
	return ptr;
}
int SetVarCharStr( char *buffer, int VarMode, int k) {
	int f,j=0;
	j+=SetVarChar( buffer+j, k );
	if ( VarMode ) {		// Int variable
		f=( LocalInt[k] == &REGINT[k] );
	} else {				// Double variable
		f=( LocalDbl[k] == &REG[k] );
	}
	if ( VarMode ) {		// Int variable
		buffer[j++]='%';
	}
	if ( f ) {
		buffer[j++]='=';
	} else {
		buffer[j++]=0xE5;		// reverse '='
		buffer[j++]=0xB8;
	}
	
	buffer[j++]='\0';
	return CB_MB_ElementCount( buffer )+1;
}

int SetVar(int select){		// ----------- Set Variable
	char buffer[32];
	unsigned int key;
	int	cont=1;
	int scrl=0;
	int seltop=select;
	int i,j,k,f,y,x;
	int selectreplay=-1;
	int opNum=25+3,lnum;
	int small=0;
	double value=0;
	int VarMode=CB_INT;	// 0:double  1:int
	int hex=0;	// 0:normal  1:hex

	Cursor_SetFlashMode(0); 		// cursor flashing off
	
	if ( select >= IsExtVar ) select=IsExtVar;
	if ( select >= 58 ) { small=58; }
	else
	if ( select >= 32 ) { small=32; }
	select-=small;
	
	while (cont) {
		Bdisp_AllClr_VRAM();
		
		opNum=25+3;
		if ( small==58 ) { opNum=IsExtVar-57;}
		if ( opNum < 6 ) lnum=opNum; else lnum=6;
		if (  select<seltop ) seltop = select;
		if ( (select-seltop) > lnum ) seltop = select-lnum;
		if ( (opNum -seltop) < lnum ) seltop = opNum -lnum; 
		
		for ( i=0; i<7; i++ ) {
			k=seltop+i+small;
			if ( ( small==58 ) && ( k > IsExtVar+1 ) ) break;	// ext   Var
			if ( ( small==32 ) && ( k >= 58 ) ) k-=(32);						// small Var
			else
			if ( ( small==58 ) && ( k > IsExtVar ) ) k=28;	// ext   Var
			x=SetVarCharStr( buffer, VarMode, k);
			CB_Print( 1, 1+i, (unsigned char*)buffer);

			if ( VarMode ) {
				locate(x, 1+i);		// int
				if ( hex )	sprintf(buffer,"0x%08X        ",(int)LocalInt[k][0]);
					else	sprintG(buffer, (double)LocalInt[k][0], 18,LEFT_ALIGN);
			} else {
				locate(x, 1+i);		// dbl
				if ( hex )	SetVarDblToHex( buffer, LocalDbl[k][0] );
					else	sprintG(buffer, (double)LocalDbl[k][0], 18,LEFT_ALIGN);
			}
			Print((unsigned char*)buffer);
		}
		Fkey_Icon( FKeyNo1, 775 );	//	Fkey_dispN( FKeyNo1, "A<>a");
		Fkey_Icon( FKeyNo2,  95 );	//	Fkey_dispN( FKeyNo2, "Init");
		if ( VarMode ) Fkey_dispN_aA( 2, "D<>I"); else Fkey_dispN_Aa( 2, "D<>I");
		if ( hex ) Fkey_dispN( FKeyNo6, "\xE6\x91\x44\x65\x63"); else Fkey_dispN( FKeyNo6, "\xE6\x91Hex");

		locate(12,8); SetVarDsp(VarMode);

		y = (select-seltop) ;
		Bdisp_AreaReverseVRAM(0, y*8, 127, y*8+7);	// reverse select line 
		Bdisp_PutDisp_DD();

		k=select; if ( select<=25 ) k+=small;
		if ( ( small==58 ) && ( k > IsExtVar ) ) k=28;	// ext   Var
		GetKey( &key );
		switch (key) {
			case KEY_CTRL_EXIT:
			case KEY_CTRL_EXE:
				cont=0;
				break;
		
			case KEY_CTRL_F1:
				if ( small ==  0  ) small=32;
				else
				if ( small == 32  ) { if ( IsExtVar < 58 ) small =0; else { small=58; if ( select > IsExtVar-58 ) select=IsExtVar-58; } }
				else
				if ( small == 58  ) small= 0;
				break;
			case KEY_CTRL_F2:
				InitVar(value,VarMode, small);
				break;
			case KEY_CTRL_F3:
				VarMode=1-VarMode;
				break;
			case KEY_CTRL_F6:
				hex=1-hex;
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
				SetVarSel(VarMode,y);
				x=SetVarCharStr( buffer, VarMode, k);
				y++;
				selectreplay = select; 
				if ( ( 0 <= select ) && ( select <=opNum ) ) {	// regA to regZ
					if ( VarMode ) 
						LocalInt[k][0]= InputNumD_fullhex( x, y, 18, (double)LocalInt[k][0], hex);
					else
						LocalDbl[k][0]= InputNumD_fullhex( x, y, 19, (double)LocalDbl[k][0], hex);
				} else {
						selectreplay = -1; // replay cancel 
				}
				break;
				
			case KEY_CTRL_LEFT:
				if (selectreplay<0) break;
				SetVarSel(VarMode,y);
				x=SetVarCharStr( buffer, VarMode, k);
				y++;
				selectreplay = select; 
				if ( ( 0 <= select ) && ( select <=opNum ) ) {	// regA to regZ
					if ( VarMode ) 
						LocalInt[k][0]= InputNumD_replay( x, y, 18, (double)LocalInt[k][0]);
					else
						LocalDbl[k][0]= InputNumD_replay( x, y, 19, (double)LocalDbl[k][0]);
				} else {
						selectreplay = -1; // replay cancel 
				}
				break;
			default:
				break;
		}
		key=MathKey( key );
		if ( key ) {
				SetVarSel(VarMode,y);
				x=SetVarCharStr( buffer, VarMode, k);
				y++;
				selectreplay = select; 
				if ( ( 0 <= select ) && ( select <=opNum ) ) {	// regA to regZ
					if ( VarMode ) 
						LocalInt[k][0]= InputNumD_Char( x, y, 18, (double)LocalInt[k][0], key);
					else
						LocalDbl[k][0]= InputNumD_Char( x, y, 19, (double)LocalDbl[k][0], key);
				} else {
						selectreplay = -1; // replay cancel 
				}
		}
	}
	SaveConfig();
	if ( small ) select+=small;
	return select;
}

//-----------------------------------------------------------------------------
//--------------------------------------------------------------
int DateCursorY;
int TimeCursorY;

void DateTimePrintSub(){		// timer IRQ handler
	char buffer[32];
	char DateStr[16];
	char TimeStr[16];
	int cy,y;
	cy=DateCursorY & 0xFF;
	y =DateCursorY / 0x100;
	if ( ( 1<=cy ) && ( cy<=7 ) ) {
			DateToStr(DateStr);
			sprintf(buffer,"DATE : %s ",DateStr);
			locate( 1, cy); Print((unsigned char*)buffer);
			if ( (y+1)==cy ) Bdisp_AreaReverseVRAM(0, y*8, 127, y*8+7);	// reverse select line 
	}
	cy=TimeCursorY & 0xFF;
	y =TimeCursorY / 0x100;
	if ( ( 1<=cy ) && ( cy<=7 ) ) {
			TimeToStr(TimeStr);
			sprintf(buffer,"TIME : %s       ",TimeStr);
			locate( 1, cy); Print((unsigned char*)buffer);
			if ( (y+1)==cy ) Bdisp_AreaReverseVRAM(0, y*8, 127, y*8+7);	// reverse select line 
	}
}
void DateTimePrint(){		// timer IRQ handler
	DateTimePrintSub();
	Bdisp_PutDisp_DD();
}

void TimePrintSetMode(int set){	// 1:on  0:off
//	int dy,ty;
//	dy=DateCursorY & 0xFF;
//	ty=TimeCursorY & 0xFF;
	switch (set) {
		case 0:
			KillTimer(ID_USER_TIMER1);
			break;
		case 1:
//			if ( ( (0<=dy)&&(dy<=7) ) || ( (0<=ty)&&(ty<=7) ) )
				SetTimer(ID_USER_TIMER1, 1000, (void*)&DateTimePrint);
			break;
		default:
			break;
	}
}
//--------------------------------------------------------------

int SelectNum2( char*msg, int n ,int min, int max) {		// 
	char buffer[32];
	unsigned int key;
	int n0=n;
	PopUpWin(3);
	locate( 3,3); Print((unsigned char *)"Select Number");
	locate( 3,5); sprintf(buffer,"%s[%d~%d]:",msg,min,max); Print((unsigned char *)buffer);
	while (1) {
		n=InputNumD(3+strlen(buffer), 5, log10(max)+1, n, " ", REV_OFF, FLOAT_OFF, EXP_OFF);		// 0123456789
		if ( (min<=n)&&(n<=max) ) break;
		n=n0;
	}
	return n ; // ok
}
int SelectNum3( int n ) {		// 
	unsigned int key;
	int n0=n;
	PopUpWin(3);
	locate( 3,3); Print((unsigned char *)"SkipUp/Dw Number");
	locate( 6,5); Print((unsigned char *)"[1~9999]:");
	while (1) {
		n=InputNumD(15, 5, 4, n, " ", REV_OFF, FLOAT_OFF, EXP_OFF);		// 0123456789
		if ( (1<=n)&&(n<=9999) ) break;
		n=n0;
	}
	return n ; // ok
}
int SelectNum4( int n ) {		// 
	unsigned int key;
	int n0=n;
	PopUpWin(3);
	locate( 3,3); Print((unsigned char *)"Select Number");
	locate( 3,3); Print((unsigned char *)"RefrshTime n/128s");
	locate( 4,5); Print((unsigned char *)"[0=\x7F\x53,1~128]:");
	while (1) {
		n=InputNumD(16, 5, 3, n, " ", REV_OFF, FLOAT_OFF, EXP_OFF);		// 0123456789
		if ( (0<=n)&&(n<=128) ) break;
		n=n0;
	}
	return n ; // ok
}

//--------------------------------------------------------------

#define SETUP_Angle			0
#define SETUP_DrawType		1
#define SETUP_Coord			2
#define SETUP_Grid			3
#define SETUP_Axes			4
#define SETUP_Label			5
#define SETUP_Derivative	6
#define SETUP_Background	7
#define SETUP_Sketch		8
#define SETUP_Display		9
#define SETUP_RecoverSetup	10
#define SETUP_CMDINPUT		11
#define SETUP_EditTopLine	12
#define SETUP_EditExtFont	13
#define SETUP_EditFontSize	14
#define SETUP_EditLineNum	15
#define SETUP_EditListChar	16
#define SETUP_UseHidnRam	17
#define SETUP_HidnRamInit	18
#define SETUP_ExtendPict		19
#define SETUP_ExtendList		20
#define SETUP_AutoDebugMode	21
#define SETUP_BreakStop		22
#define SETUP_ExecTimeDsp	23
#define SETUP_IfEndCheck	24
#define SETUP_ACBreak		25
#define SETUP_ForceReturnMode	26
#define SETUP_Key1sttime	27
#define SETUP_KeyReptime	28
#define SETUP_SkipUpDown	29
#define SETUP_MatDspmode	30
#define SETUP_Matrixbase	31
#define SETUP_Pictmode		32
#define SETUP_DATE			33
#define SETUP_TIME			34
#define SETUP_Storagemode	35
#define SETUP_AutoSaveMode	36
#define SETUP_Forceg1msave	37
#define SETUP_RefrshCtlDD	38
#define SETUP_DefaultWaitcount	39
#define SETUP_Executemode	40

int SetupG(int select){		// ----------- Setup 
    const char *onoff[]   ={"off","on"};
    const char *draw[]    ={"Connect","Plot"};
    const char *style[]   ={"Normal","Thick","Broken","Dot"};
    const char *degrad[]  ={"Deg","Rad","Grad"};
    const char *display[] ={"Nrm","Fix","Sci"};
    const char *ENGmode[] ={"  ","/E","  ","/3"};
    const char *CMDinput[] ={"C.Basic","Standard"};
    const char *CharSize[] ={"Standard","Mini","Mini Rev","Mini_","Mini_Rev"};
    const char *mode[]    ={"DBL#","INT%"};
    const char *Matmode[]    ={"[m,n]","[X,Y]"};
    const char *Matbase[]    ={"0","1"};
    const char *Pictmode[]    ={"S.Mem","Heap","Both"};
    const char *PictmodeSD[]  ={"SDcard ","Heap","Both"};
    const char *Stragemode[]  ={"S.Mem","SDcard "};
    const char *DDmode[]    ={"off","Grph","All"};
    const char *ListChar[]    ={"List"," \xE5\xB7"," \xFF\xE0"};
    const char *Returnmode[]    ={"None","F1","EXE","F1&EXE"};
    const char *ExecTimemode[]    ={"off","on","off%","on reset","  %","on%","  % reset","on%reset"};
	char buffer[22];
	unsigned int key;
	int	cont=1;
	int scrl=select-6;
	int i,y,cnt;
	char DateStr[16];
	char TimeStr[16];
	int year,month,day,hour,min,sec;
	int listmax=SETUP_Executemode;
	
	Cursor_SetFlashMode(0); 		// cursor flashing off
	
	if ( select > listmax ) select=0;
	if ( select < scrl ) scrl-=1;
	if ( scrl < 0 ) scrl=0;

	DateCursorY=-1;
	TimeCursorY=-1;

	while (cont) {
		Bdisp_AllClr_VRAM();
		
		DateToStr(DateStr);
		TimeToStr(TimeStr);
		DateCursorY = 0x909;
		TimeCursorY = 0x909;
		
		cnt=1;
		if ( scrl <=(cnt-1) ) {
			locate( 1, cnt-scrl); Print((unsigned char*)"Angle       :");		// 0
			locate(14, cnt-scrl); Print((unsigned char*)degrad[Angle]);
		} cnt++;
		if ( scrl <=(cnt-1) ) {
			locate( 1, cnt-scrl); Print((unsigned char*)"Draw Type   :");		// 1
			locate(14, cnt-scrl); Print((unsigned char*)draw[(int)DrawType]);
		} cnt++;
		if ( scrl <=(cnt-1) ) {
			locate( 1, cnt-scrl); Print((unsigned char*)"Coord:      :");		// 2
			locate(14, cnt-scrl); Print((unsigned char*)onoff[Coord]);
		} cnt++;
		if ( scrl <=(cnt-1) ) {
			locate( 1, cnt-scrl); Print((unsigned char*)"Grid        :");		// 3
			locate(14, cnt-scrl); Print((unsigned char*)onoff[Grid]);
		} cnt++;
		if ( scrl <=(cnt-1) ) {
			locate( 1, cnt-scrl); Print((unsigned char*)"Axes        :");		// 4
			locate(14, cnt-scrl); Print((unsigned char*)onoff[Axes]);
		} cnt++;
		if ( scrl <=(cnt-1) ) {
			locate( 1, cnt-scrl); Print((unsigned char*)"Label       :");		// 5
			locate(14, cnt-scrl); Print((unsigned char*)onoff[Label]);
		} cnt++;
		if ( scrl <=(cnt-1) ) {
			locate( 1, cnt-scrl); Print((unsigned char*)"Derivative  :");		// 6
			locate(14, cnt-scrl); Print((unsigned char*)onoff[Derivative]);
		} cnt++;
		if ( (0<(cnt-scrl))&&((cnt-scrl)<=7) ){
			locate( 1, cnt-scrl); Print((unsigned char*)"Background  :");		// 7
			if ( BG_Pict_No == 0 )	sprintf((char*)buffer,"None");
			else					sprintf((char*)buffer,"Pict%d",BG_Pict_No);
			locate(14,cnt-scrl); Print((unsigned char*)buffer);
		} cnt++;
		if ( (0<(cnt-scrl))&&((cnt-scrl)<=7) ){
			locate( 1, cnt-scrl); Print((unsigned char*)"Sketch Line :");		// 8
			locate(14, cnt-scrl); Print((unsigned char*)style[S_L_Style]);
		} cnt++;
		if ( (0<(cnt-scrl))&&((cnt-scrl)<=7) ){
			locate( 1, cnt-scrl); Print((unsigned char*)"Display     :");		// 9
			locate(14, cnt-scrl); Print((unsigned char*)display[CB_Round.MODE]);
			buffer[0]='\0';
			sprintf((char*)buffer,"%d",CB_Round.DIGIT);
			locate(17, cnt-scrl); Print((unsigned char*)buffer);
			locate(19, cnt-scrl); 
			Print((unsigned char*)ENGmode[ENG]);
		} cnt++;
		if ( (0<(cnt-scrl))&&((cnt-scrl)<=7) ){
			locate( 1, cnt-scrl); Print((unsigned char*)"SetupRecover:");		// 10
			locate(14, cnt-scrl); Print((unsigned char*)onoff[CB_RecoverSetup]);
		} cnt++;
		if ( (0<(cnt-scrl))&&((cnt-scrl)<=7) ){
			locate( 1, cnt-scrl); Print((unsigned char*)"Command Inpt:");		// 11
			locate(14, cnt-scrl); Print((unsigned char*)CMDinput[CommandInputMethod]);
		} cnt++;
		if ( (0<(cnt-scrl))&&((cnt-scrl)<=7) ){
			locate( 1, cnt-scrl); Print((unsigned char*)"Edit TopLine:");		// 12
			locate(14, cnt-scrl); Print((unsigned char*)onoff[EditTopLine]);
		} cnt++;
		if ( (0<(cnt-scrl))&&((cnt-scrl)<=7) ){
			CB_Print_ext( 1,cnt-scrl,(unsigned char*)"Edit ExtFont:", EditExtFont );		// 13
			CB_Print_ext(14,cnt-scrl,(unsigned char*)onoff[EditExtFont], EditExtFont );
		} cnt++;
		if ( (0<(cnt-scrl))&&((cnt-scrl)<=7) ){
			locate( 1, cnt-scrl); Print((unsigned char*)"EditFontSize:");		// 14
			locate(14, cnt-scrl); Print((unsigned char*)CharSize[EditFontSize & 0x0F ]);
		} cnt++;
		if ( (0<(cnt-scrl))&&((cnt-scrl)<=7) ){
			locate( 1, cnt-scrl); Print((unsigned char*)"Edit LineNum:");		// 15
			locate(14, cnt-scrl); Print((unsigned char*)onoff[(EditFontSize & 0xF0)>>4 ]);
		} cnt++;
		if ( (0<(cnt-scrl))&&((cnt-scrl)<=7) ){
			locate( 1, cnt-scrl); Print((unsigned char*)"EditListChar:");		// 16
			CB_Print(14, cnt-scrl, (unsigned char*)ListChar[EditListChar]);
		} cnt++;
		if ( (0<(cnt-scrl))&&((cnt-scrl)<=7) ){
			locate( 1,cnt-scrl); Print((unsigned char*)"Use Hidn RAM:");		// 17
			locate(14,cnt-scrl); Print((unsigned char*)onoff[UseHiddenRAM&0x0F]);
		} cnt++;
		if ( (0<(cnt-scrl))&&((cnt-scrl)<=7) ){
			locate( 1,cnt-scrl); Print((unsigned char*)"HidnRAM Init:");		// 18
			locate(14,cnt-scrl);
			if ( UseHiddenRAM&0x0F ) Print((unsigned char*)onoff[!(UseHiddenRAM&0xF0)]);
			else                     Print((unsigned char*)"---");
		} cnt++;
		if ( (0<(cnt-scrl))&&((cnt-scrl)<=7) ){
			locate( 1, cnt-scrl); Print((unsigned char*)"Max Pict No:");		// 19
			sprintf((char*)buffer,"%d",20+ExtendPict);
			locate(14, cnt-scrl); Print((unsigned char*)buffer);
		} cnt++;
		if ( (0<(cnt-scrl))&&((cnt-scrl)<=7) ){
			locate( 1, cnt-scrl); Print((unsigned char*)"Max List No:");		// 20
			sprintf((char*)buffer,"%d",52+ExtendList*52);
			locate(14, cnt-scrl); Print((unsigned char*)buffer);
		} cnt++;
		if ( (0<(cnt-scrl))&&((cnt-scrl)<=7) ){
			locate( 1,cnt-scrl); Print((unsigned char*)"AT DebugMode:");		// 21
			locate(14,cnt-scrl); Print((unsigned char*)onoff[!AutoDebugMode]);
		} cnt++;
		if ( (0<(cnt-scrl))&&((cnt-scrl)<=7) ){
			locate( 1,cnt-scrl); Print((unsigned char*)"Break Stop  :");		// 22
			locate(14,cnt-scrl); Print((unsigned char*)onoff[BreakCheckDefault]);
		} cnt++;
		if ( (0<(cnt-scrl))&&((cnt-scrl)<=7) ){
			locate( 1,cnt-scrl); Print((unsigned char*)"Exec TimeDsp:");		// 23
			locate(14,cnt-scrl); Print((unsigned char*)ExecTimemode[TimeDsp]);
		} cnt++;
		if ( (0<(cnt-scrl))&&((cnt-scrl)<=7) ){
			locate( 1,cnt-scrl); Print((unsigned char*)"IfEnd Check :");		// 24
			locate(14,cnt-scrl); Print((unsigned char*)onoff[CheckIfEnd]);
		} cnt++;
		if ( (0<(cnt-scrl))&&((cnt-scrl)<=7) ){
			locate( 1,cnt-scrl); Print((unsigned char*)"ACBreak     :");		// 25
			locate(14,cnt-scrl); Print((unsigned char*)onoff[ACBreak]);
		} cnt++;
		if ( (0<(cnt-scrl))&&((cnt-scrl)<=7) ){
			locate( 1,cnt-scrl); Print((unsigned char*)"Force Return:");		// 26
			locate(14,cnt-scrl); Print((unsigned char*)Returnmode[ForceReturnMode]);
		} cnt++;
		if ( (0<(cnt-scrl))&&((cnt-scrl)<=7) ){
			locate( 1,cnt-scrl); Print((unsigned char*)"Key 1st time:");		// 27
			sprintf((char*)buffer,"%dms",KeyRepeatFirstCount*25);
			locate(14,cnt-scrl); Print((unsigned char*)buffer);
		} cnt++;
		if ( (0<(cnt-scrl))&&((cnt-scrl)<=7) ){
			locate( 1,cnt-scrl); Print((unsigned char*)"Key Rep time:");		// 28
			sprintf((char*)buffer,"%dms",KeyRepeatNextCount*25);
			locate(14,cnt-scrl); Print((unsigned char*)buffer);
		} cnt++;
		if ( (0<(cnt-scrl))&&((cnt-scrl)<=7) ){
			locate( 1,cnt-scrl); Print((unsigned char*)"SkipUp/Down :");		// 29
			sprintf((char*)buffer,"%d",PageUpDownNum);
			locate(14,cnt-scrl); Print((unsigned char*)buffer);
		} cnt++;
		if ( (0<(cnt-scrl))&&((cnt-scrl)<=7) ){
			locate( 1,cnt-scrl); Print((unsigned char*)"Mat Dsp mode:");		// 30
			locate(14,cnt-scrl); Print((unsigned char*)Matmode[MatXYmode]);
		} cnt++;
		if ( (0<(cnt-scrl))&&((cnt-scrl)<=7) ){
			locate( 1,cnt-scrl); Print((unsigned char*)"Matrix base :");		// 31
			locate(14,cnt-scrl); Print((unsigned char*)Matbase[MatBaseDefault]);
		} cnt++;
		if ( (0<(cnt-scrl))&&((cnt-scrl)<=7) ){
			locate( 1,cnt-scrl); Print((unsigned char*)"Pict mode   :");		// 32
			locate(14,cnt-scrl); if ( StorageMode ) Print((unsigned char*)PictmodeSD[PictMode]); else Print((unsigned char*)Pictmode[PictMode]);
		} cnt++;
		if ( (0<(cnt-scrl))&&((cnt-scrl)<=7) ){		// DATE						// 33
			DateCursorY = cnt-scrl+0x900;
			DateTimePrintSub();
		} cnt++;
		if ( (0<(cnt-scrl))&&((cnt-scrl)<=7) ){		// TIME						// 34
			TimeCursorY = cnt-scrl+0x900;
			DateTimePrintSub();
		} cnt++;
		if ( (0<(cnt-scrl))&&((cnt-scrl)<=7) ){
			locate( 1,cnt-scrl); Print((unsigned char*)"Storage mode:");		// 35
			locate(14,cnt-scrl); Print((unsigned char*)Stragemode[StorageMode]);
		} cnt++;
		if ( (0<(cnt-scrl))&&((cnt-scrl)<=7) ){
			locate( 1,cnt-scrl); Print((unsigned char*)"Auto file save:");		// 36
			locate(16,cnt-scrl); Print((unsigned char*)onoff[AutoSaveMode]);
		} cnt++;
		if ( (0<(cnt-scrl))&&((cnt-scrl)<=7) ){
			locate( 1,cnt-scrl); Print((unsigned char*)"Force g1m save:");		// 37
			locate(16,cnt-scrl); Print((unsigned char*)onoff[ForceG1Msave]);
		} cnt++;
		if ( (0<(cnt-scrl))&&((cnt-scrl)<=7) ){
			locate( 1,cnt-scrl); Print((unsigned char*)"RefrshCtl DD:");		// 38
			locate(14,cnt-scrl); Print((unsigned char*)DDmode[RefreshCtrl]);
			buffer[0]='\0';
			sprintf((char*)buffer,"%2d/128",Refreshtime+1);
			if ( RefreshCtrl ) PrintMini(17*6+2,(cnt-scrl)*8-6,(unsigned char*)buffer,MINI_OVER);
		} cnt++;
		if ( (0<(cnt-scrl))&&((cnt-scrl)<=7) ){
			locate( 1,cnt-scrl); Print((unsigned char*)"Wait count  :");		// 39
			if ( DefaultWaitcount == 0 )	sprintf((char*)buffer,"No Wait");
			else					sprintf((char*)buffer,"%d",DefaultWaitcount);
			locate(14,cnt-scrl); Print((unsigned char*)buffer);
		} cnt++;
		if ( (0<(cnt-scrl))&&((cnt-scrl)<=7) ){
			locate( 1,cnt-scrl); Print((unsigned char*)"Execute mode:");		// 40
			locate(14,cnt-scrl); Print((unsigned char*)mode[CB_INTDefault]);
		}
		y = select-scrl;
		Bdisp_AreaReverseVRAM(0, y*8, 127, y*8+7);	// reverse select line 
		switch (select) {
			case SETUP_DrawType: // Draw Type
				Fkey_Icon( FKeyNo1, 357 );	//	Fkey_dispN( FKeyNo1, "Con");
				Fkey_Icon( FKeyNo2, 358 );	//	Fkey_dispN( FKeyNo2, "Plot");
				FkeyClear( FKeyNo3 );
				FkeyClear( FKeyNo4 );
				break;
			case SETUP_Coord: // Coord
			case SETUP_Grid: // Grid	
			case SETUP_Axes: // Axes
			case SETUP_Label: // Label
			case SETUP_Derivative: // Derivative
			case SETUP_EditTopLine: // Use Top of Line (edit)
			case SETUP_EditExtFont: // Use Ext Font (edit)
			case SETUP_EditLineNum: // Line number (edit)
			case SETUP_UseHidnRam: // UseHiddenRAM
			case SETUP_HidnRamInit: // HiddenRAMInit
			case SETUP_AutoDebugMode: // AutoDebugMode
			case SETUP_BreakStop: // BreakCheck
			case SETUP_IfEndCheck: // IfEnd Check
			case SETUP_ACBreak: // ACBreak Check
			case SETUP_AutoSaveMode: // Auto save
			case SETUP_Forceg1msave: // Force g1m save
			case SETUP_RecoverSetup: // Setup Mode 
				Fkey_Icon( FKeyNo1, 17 );	//	Fkey_dispN( FKeyNo1, " On ");
				Fkey_Icon( FKeyNo2, 18 );	//	Fkey_dispN( FKeyNo2, " Off");
				FkeyClear( FKeyNo3 );
				FkeyClear( FKeyNo4 );
				break;
			case SETUP_ExecTimeDsp: // TimeDsp
				Fkey_Icon( FKeyNo1, 17 );	//	Fkey_dispN( FKeyNo1, " On ");
				Fkey_Icon( FKeyNo2, 18 );	//	Fkey_dispN( FKeyNo2, " Off");
				if ( IsSH3 == 0 ) Fkey_dispN( FKeyNo3, " %HR ");
				Fkey_dispN( FKeyNo4, "reset");
				break;
			case SETUP_Background: // BG pict
				Fkey_Icon( FKeyNo1, 362 );	//	Fkey_dispN( FKeyNo1, "None");
				Fkey_Icon( FKeyNo2, 183 );	//	Fkey_dispR( FKeyNo2, "PICT");
				break;
			case SETUP_CMDINPUT: // Command input method
				Fkey_dispN( FKeyNo1, "CBas");
				Fkey_dispN( FKeyNo2, "Std");
				break;
			case SETUP_EditFontSize: // Edit Char Size
				Fkey_dispN( FKeyNo1, "Std");
				Fkey_dispN( FKeyNo2, "Mini");
				Fkey_dispN( FKeyNo3, "Rev");
				Fkey_dispN( FKeyNo4, "___");
				Fkey_dispN( FKeyNo5, "Rev_");
				break;
			case SETUP_Sketch: // S_L_ Line	normal
				FkeyS_L_();
				break;
			case SETUP_Angle: // Angle
				Fkey_Icon( FKeyNo1, 359 );	//	Fkey_dispN( FKeyNo1, "Deg ");
				Fkey_Icon( FKeyNo2, 360 );	//	Fkey_dispN( FKeyNo2, "Rad ");
				Fkey_Icon( FKeyNo3, 361 );	//	Fkey_dispN( FKeyNo3, "Grad");
				FkeyClear( FKeyNo4 );
				break;
			case SETUP_Display: // Display
				Fkey_Icon( FKeyNo1, 372 );	//	Fkey_dispR( FKeyNo1, "Fix ");
				Fkey_Icon( FKeyNo2, 373 );	//	Fkey_dispR( FKeyNo2, "Sci ");
				Fkey_Icon( FKeyNo3, 374 );	//	Fkey_dispR( FKeyNo3, "Norm ");
				Fkey_Icon( FKeyNo4, 375 );	//	Fkey_dispN( FKeyNo4, "Eng ");
				break;
			case SETUP_Key1sttime:	// Key Repeat mode
			case SETUP_KeyReptime:	// Key Repeat mode
			case SETUP_ExtendPict:		// Max Pict
			case SETUP_ExtendList:		// Max List
				Fkey_DISPN( FKeyNo1," +");
				Fkey_DISPN( FKeyNo2," -");
				Fkey_Icon( FKeyNo4,  95 );	//	Fkey_dispN( FKeyNo4, "Init");
				break;
			case SETUP_SkipUpDown: // SkipUp/Down number
			case SETUP_DefaultWaitcount: // Wait count number
				Fkey_DISPN( FKeyNo1," +");
				Fkey_DISPN( FKeyNo2," -");
				Fkey_dispR( FKeyNo3, "Num");
				Fkey_Icon( FKeyNo4,  95 );	//	Fkey_dispN( FKeyNo4, "Init");
				break;
			case SETUP_MatDspmode: // Mat display mode
				Fkey_dispN( FKeyNo1, "m.n ");
				Fkey_dispN( FKeyNo2, "X,Y ");
				break;
			case SETUP_Matrixbase: // Mat base
				Fkey_dispN( FKeyNo1, " 0 ");
				Fkey_dispN( FKeyNo2, " 1 ");
				break;
			case SETUP_DATE: // DATE
				Fkey_dispR( FKeyNo1, "Year");
				Fkey_dispR( FKeyNo2, "Mth");
				Fkey_dispR( FKeyNo3, "Day");
				break;
			case SETUP_TIME: // TIME
				Fkey_dispR( FKeyNo1, "Hour");
				Fkey_dispR( FKeyNo2, "Min");
				Fkey_dispR( FKeyNo3, "Sec");
				break;
			case SETUP_Pictmode: // Pict mode
				if ( StorageMode ) Fkey_dispN( FKeyNo1, " SD "); else Fkey_dispN( FKeyNo1, "MEM ");
				Fkey_dispN( FKeyNo2, "Heap");
				Fkey_dispN( FKeyNo3, "Both");
				break;
			case SETUP_Storagemode: // Strage mode
				Fkey_dispN( FKeyNo1, "MEM ");
				Fkey_dispN( FKeyNo2, " SD ");
				break;
			case SETUP_RefrshCtlDD: // Refresh Ctrl DD Mode
				Fkey_Icon( FKeyNo1, 18 );	//	Fkey_dispN( FKeyNo1, "off ");
				Fkey_dispN( FKeyNo2, "Grph");
				Fkey_dispN( FKeyNo3, "All ");
				if ( RefreshCtrl ) Fkey_dispR( FKeyNo4, "time");
				Fkey_Icon( FKeyNo5,  95 );	//	Fkey_dispN( FKeyNo5, "Init");
				break;
			case SETUP_Executemode: // Execute Mode
				Fkey_dispN( FKeyNo1, "DBL#");
				Fkey_dispN( FKeyNo2, "INT%");
				break;
			case SETUP_EditListChar:
				Fkey_dispN( FKeyNo1, "List");
				Fkey_dispN( FKeyNo2, " \xE5\xB7");
				Fkey_dispN( FKeyNo3, " \xFF\xE0");
				ML_vertical_line( 2*21+4+4, 7*8+2, 7*8+6, ML_BLACK);
				break;
			case SETUP_ForceReturnMode:
				Fkey_dispN( FKeyNo1, "None");
				Fkey_dispN( FKeyNo2, " F1");
				Fkey_dispN( FKeyNo3, "EXE");
				Fkey_dispN( FKeyNo4, "Both");
				break;
			default:
				break;
		}
		
		Fkey_Icon( FKeyNo6, 991 );	//	Fkey_dispN( FKeyNo6, "Ver.");
//		Bdisp_PutDisp_DD();
		
		DateCursorY = y*0x100+(DateCursorY&0xFF);
		TimeCursorY = y*0x100+(TimeCursorY&0xFF);
		TimePrintSetMode( 1 ) ;			// Date/Time print IRQ on
		GetKey_DisableMenu(&key);
		TimePrintSetMode( 0 ) ;			// Date/Time print IRQ off
		DateToStr(DateStr);
		TimeToStr(TimeStr);
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
			case KEY_CTRL_LEFT:
				select=0;
				scrl=0;
				break;
			case KEY_CTRL_RIGHT:
				select=(listmax);
				scrl=select-6;
				break;
				
			case KEY_CTRL_F1:
				Bdisp_AreaReverseVRAM(0, y*8, 127, y*8+7);	// reverse select line 
				switch (select) {
					case SETUP_DrawType: // Draw Type connect
						DrawType = 0 ;
						break;
					case SETUP_Coord: // Coord	on
						Coord = 1 ;
						break;
					case SETUP_Grid: // Grid		on
						Grid = 1 ;
						break;
					case SETUP_Axes: // Axes		on
						Axes = 1 ;
						break;
					case SETUP_Label: // Label	on
						Label = 1 ;
						break;
					case SETUP_Derivative: // 	Derivative on
						Derivative = 1 ;
						break;
					case SETUP_Background: // BG pict
						BG_Pict_No = 0 ;
						break;
					case SETUP_Sketch: // Sketch Line	normal
						S_L_Style = 0 ;
						break;
					case SETUP_Angle: // Angle
						Angle = 0 ; // Deg
						break;
					case SETUP_Display: // Display
						CB_Round.DIGIT=SelectNum2("Fix",CB_Round.DIGIT,0,15);
						CB_Round.MODE =Fix;
						break;
					case SETUP_RecoverSetup: // Setup Mode 
						CB_RecoverSetup = 1 ; // recover on
						break;
					case SETUP_CMDINPUT: // Command input method
						CommandInputMethod=0;
						break;
					case SETUP_EditTopLine: // Use Top of Line (edit)
						EditTopLine=1;
						break;
					case SETUP_EditExtFont: // Use Ext Font (edit)
						EditExtFont=1;
						break;
					case SETUP_EditFontSize: // Edit Char Size
						EditFontSize &= 0xF0;	// Standard
						UpdateLineNum=1;
						break;
					case SETUP_EditLineNum: // Line number (edit)
						EditFontSize |= 0x10;	// on
						break;
					case SETUP_EditListChar:
						EditListChar=0;		// normal
						break;
					case SETUP_ExtendPict:		// Max Pict
						if ( UseHiddenRAM == 0 )  break;	// Hidden RAM only
						ExtendPict+=10;
						if ( 79<ExtendPict ) ExtendPict=79;
						HiddenRAM_MatAryInit();
						break;
					case SETUP_ExtendList:		// Max List
						if ( UseHiddenRAM == 0 )  break;	// Hidden RAM only
						ExtendList++;
						if ( 19<ExtendList ) ExtendList=19;
						HiddenRAM_MatAryInit();
						break;
					case SETUP_UseHidnRam: // Hidden RAM
						if ( IsHiddenRAM ) UseHiddenRAM = 1 ; // on
						HiddenRAM_MatAryInit();
						break;
					case SETUP_HidnRamInit: // HiddenRAMInit
						if ( UseHiddenRAM ) UseHiddenRAM &= 0x0F;	// on
						HiddenRAM_MatAryInit();
						break;
					case SETUP_AutoDebugMode: // AutoDebugMode
						AutoDebugMode = 0 ; // on
						break;
					case SETUP_BreakStop: // Break
						BreakCheckDefault = 1 ; // on
						BreakCheck = 1 ; // on
						break;
					case SETUP_ExecTimeDsp: // TimeDsp
						TimeDsp = 1 ; // on
						break;
					case SETUP_IfEndCheck: // IfEnd Check
						CheckIfEnd = 1 ; // on
						break;
					case SETUP_ACBreak: // ACBreak
						ACBreak = 1 ; // on
						break;
					case SETUP_ForceReturnMode:
						ForceReturnMode = 0;
						break;
					case SETUP_Key1sttime: // Key Repeat First Count *ms
						KeyRepeatFirstCount += 1 ;
						if ( KeyRepeatFirstCount > 40 ) KeyRepeatFirstCount=40;
						break;
					case SETUP_KeyReptime: // Key Repeat Next Count *ms
						KeyRepeatNextCount += 1 ;
						if ( KeyRepeatNextCount > 20 ) KeyRepeatNextCount=20;
						break;
					case SETUP_SkipUpDown: // Skipup/down count +
						PageUpDownNum++; if ( PageUpDownNum > 9999 ) PageUpDownNum = 9999;
						break;
					case SETUP_MatDspmode: // Matrix Display mode
						MatXYmode = 0 ; // m,n
						break;
					case SETUP_Matrixbase: // Matrix base
						MatBaseDefault = 0 ; // 
						MatBase = MatBaseDefault;
						break;
					case SETUP_DATE: // DATE year
						year = (DateStr[0]-'0')*1000+(DateStr[1]-'0')*100+(DateStr[2]-'0')*10+(DateStr[3]-'0');
						year = SelectNum2("Year",year,0,9999);
						DateStr[0]=(year/1000)+'0';
						DateStr[1]=(year/100)%10+'0';
						DateStr[2]=(year/10)%10+'0';
						DateStr[3]=(year)%10+'0';
						StorDATE( DateStr );
						break;
					case SETUP_TIME: // Time hour
						hour = (TimeStr[0]-'0')*10+(TimeStr[1]-'0');
						hour = SelectNum2("Hour",hour,0,23);
						TimeStr[0]=(hour/10)%10+'0';
						TimeStr[1]=(hour)%10+'0';
						StorTIME( TimeStr );
						break;
					case SETUP_Pictmode: // Pict mode
						PictMode = 0 ; // Memory mode
						break;
					case SETUP_Storagemode: // Strage mode
						StorageMode = 0 ; // Memory mode
						break;
					case SETUP_AutoSaveMode: // Auto save
						AutoSaveMode = 1;
						break;
					case SETUP_Forceg1msave: // Force g1m save
						ForceG1Msave = 1 ; // g1m and text
						break;
					case SETUP_RefrshCtlDD: // Refresh Ctrl DD Mode
						RefreshCtrl = 0 ; // off
						break;
					case SETUP_DefaultWaitcount: // Wait count +
						DefaultWaitcount+=10; if ( DefaultWaitcount > 9999 ) DefaultWaitcount = 9999;
						Waitcount=DefaultWaitcount;
						break;
					case SETUP_Executemode: // CB mode
						CB_INTDefault = 0 ; // normal
						CB_INT = CB_INTDefault;
						break;
					default:
						break;
				}
				break;
			case KEY_CTRL_F2:
				Bdisp_AreaReverseVRAM(0, y*8, 127, y*8+7);	// reverse select line 
				switch (select) {
					case SETUP_DrawType: // Draw Type Plot
						DrawType = 1 ;
						break;
					case SETUP_Coord: // Coord	off
						Coord = 0 ;
						break;
					case SETUP_Grid: // Grid		off
						Grid = 0 ;
						break;
					case SETUP_Axes: // Axes		off
						Axes = 0 ;
						break;
					case SETUP_Label: // Label	off
						Label = 0 ;
						break;
					case SETUP_Derivative: // 	Derivative off
						Derivative = 0 ;
						break;
					case SETUP_Background: // BG pict
						BG_Pict_No = SelectNum2("Pict",BG_Pict_No,1,20);
						break;
					case SETUP_Sketch: // Sketch Line	Thick
						S_L_Style = 1 ; 
						break;
					case SETUP_Angle: // Angle
						Angle = 1 ; // Rad
						break;
					case SETUP_Display: // Display
						CB_Round.DIGIT=SelectNum2("Sci",CB_Round.DIGIT,0,15);
						CB_Round.MODE =Sci;
						break;
					case SETUP_RecoverSetup: // Setup Mode 
						CB_RecoverSetup = 0 ; // recover off
						break;
					case SETUP_CMDINPUT: // Command input method
						CommandInputMethod=1;
						break;
					case SETUP_EditTopLine: // Use Top of Line (edit)
						EditTopLine=0;
						break;
					case SETUP_EditExtFont: // Use Ext Font (edit)
						EditExtFont=0;
						break;
					case SETUP_EditFontSize: // Edit Char Size
						EditFontSize &= 0xF0;	// Standard
						EditFontSize |= 0x01;	// mini
						break;
					case SETUP_EditLineNum: // Line number (edit)
						EditFontSize &= 0x0F;	// off (default)
						UpdateLineNum=1;
						break;
					case SETUP_EditListChar:
						EditListChar=1;		// 
						break;
					case SETUP_ExtendPict:		// Max Pict
						if ( UseHiddenRAM == 0 )  break;	// Hidden RAM only
						if (ExtendPict>70) ExtendPict=70; else ExtendPict-=10;
						if (ExtendPict<0) ExtendPict=0;
						HiddenRAM_MatAryInit();
						break;
					case SETUP_ExtendList:		// Max List
						if ( UseHiddenRAM == 0 )  break;	// Hidden RAM only
						ExtendList--;
						if (ExtendList<0) ExtendList=0;
						HiddenRAM_MatAryInit();
						break;
					case SETUP_UseHidnRam: // Hidden RAM
						UseHiddenRAM = 0 ; // off
						ExtendPict=0;
						ExtendList=0;
						HiddenRAM_MatAryInit();
						break;
					case SETUP_HidnRamInit: // HiddenRAMInit
						if ( UseHiddenRAM ) UseHiddenRAM |= 0x10;	// off
						HiddenRAM_MatAryInit();
						break;
					case SETUP_AutoDebugMode: // AutoDebugMode
						AutoDebugMode = 1 ; // disable 
						break;
					case SETUP_BreakStop: // Break
						BreakCheckDefault = 0 ; // off
						BreakCheck = 0 ; // off
						break;
					case SETUP_ForceReturnMode:
						ForceReturnMode = 1;
						break;
					case SETUP_ExecTimeDsp: // TimeDsp
						TimeDsp = 0 ; // off
						break;
					case SETUP_IfEndCheck: // IfEnd Check
						CheckIfEnd = 0 ; // off
						break;
					case SETUP_ACBreak: // ACBreak
						ACBreak = 0 ; // off
						break;
					case SETUP_Key1sttime: // Key Repeat First Count *ms
						KeyRepeatFirstCount -= 1 ;
						if ( KeyRepeatFirstCount < 1 ) KeyRepeatFirstCount=1;
						break;
					case SETUP_KeyReptime: // Key Repeat Next Count *ms
						KeyRepeatNextCount -= 1 ;
						if ( KeyRepeatNextCount < 1 ) KeyRepeatNextCount=1;
						break;
					case SETUP_SkipUpDown: // Skipup/down count -
						PageUpDownNum--; if ( PageUpDownNum < PageUpDownNumDefault ) PageUpDownNum = PageUpDownNumDefault;
						break;
					case SETUP_MatDspmode: // Matrix display mode
						MatXYmode = 1 ; // x,y
						break;
					case SETUP_Matrixbase: // Matrix base
						MatBaseDefault = 1 ; // base
						MatBase = MatBaseDefault;
						break;
					case SETUP_Pictmode: // Pict mode
						PictMode = 1 ; // heap mode
						break;
					case SETUP_RefrshCtlDD: // Refresh Ctrl DD Mode
						RefreshCtrl = 1 ; // graphics
						break;
					case SETUP_DATE: // DATE month
						month = (DateStr[5]-'0')*10+(DateStr[6]-'0');
						month = SelectNum2("Month",month,1,12);
						DateStr[5]=(month/10)%10+'0';
						DateStr[6]=(month)%10+'0';
						StorDATE( DateStr );
						break;
					case SETUP_TIME: // Time min
						min = (TimeStr[3]-'0')*10+(TimeStr[4]-'0');
						min = SelectNum2("Min",min,0,59);
						TimeStr[3]=(min/10)%10+'0';
						TimeStr[4]=(min)%10+'0';
						StorTIME( TimeStr );
						break;
					case SETUP_Storagemode: // Strage mode
						StorageMode = CheckSD() ; // SD mode
						break;
					case SETUP_AutoSaveMode: // Auto save
						AutoSaveMode = 0;
						break;
					case SETUP_Forceg1msave: // Force g1m save
						ForceG1Msave = 0 ; // text only
						break;
					case SETUP_DefaultWaitcount: // Wait count -
						DefaultWaitcount-=10; if ( DefaultWaitcount < 0 ) DefaultWaitcount = 0;
						Waitcount=DefaultWaitcount;
						break;
					case SETUP_Executemode: // CB mode
						CB_INTDefault = 1 ; // int
						CB_INT = CB_INTDefault;
						break;
					default:
						break;
				}
				break;
			case KEY_CTRL_F3:
				Bdisp_AreaReverseVRAM(0, y*8, 127, y*8+7);	// reverse select line 
				switch (select) {
					case SETUP_Sketch: // Sketch Line	Broken
						S_L_Style = 2 ; 
						break;
					case SETUP_Angle: // Angle
						Angle = 2 ; // Grad
						break;
					case SETUP_Display: // Display
						CB_Round.DIGIT=SelectNum2("Nrm",CB_Round.DIGIT,0,15);
						CB_Round.MODE =Norm;
						break;
					case SETUP_EditFontSize: // Edit Char Size
						EditFontSize &= 0xF0;	// mini
						EditFontSize |= 0x02;	// reverse
						break;
					case SETUP_EditListChar:
						EditListChar=2;		// reverse L
						break;
					case SETUP_ExecTimeDsp: // TimeDsp
						if ( TimeDsp == 0 ) break;
						if ( TimeDsp & 0x4 )    TimeDsp &= ~0x4 ; // hires timer off
						else	{ if ( IsSH3 == 0 ) TimeDsp |=  0x4 ; } // hires timer on
						break;
					case SETUP_ForceReturnMode:
						ForceReturnMode = 2;
						break;
					case SETUP_SkipUpDown: // Skipup/down count init
						PageUpDownNum = SelectNum3( PageUpDownNum );
						break;
					case SETUP_RefrshCtlDD: // Refresh Ctrl DD Mode
						RefreshCtrl = 2 ; // graphics+text
						break;
					case SETUP_Pictmode: // Pict mode
						PictMode = 2 ; // Memory(read) & Smem(save) mode
						break;
					case SETUP_DATE: // DATE day
						day = (DateStr[8]-'0')*10+(DateStr[9]-'0');
						day = SelectNum2("Day",day,1,31);
						DateStr[8]=(day/10)%10+'0';
						DateStr[9]=(day)%10+'0';
						StorDATE( DateStr );
						break;
					case SETUP_TIME: // Time sec
						sec = (TimeStr[6]-'0')*10+(TimeStr[7]-'0');
						sec = SelectNum2("Min",sec,0,59);
						TimeStr[6]=(sec/10)%10+'0';
						TimeStr[7]=(sec)%10+'0';
						StorTIME( TimeStr );
						break;
					case SETUP_DefaultWaitcount: // Wait count set
						DefaultWaitcount =  SelectNum2( "Wait", DefaultWaitcount, 0, 9999);
						Waitcount=DefaultWaitcount;
						break;
					default:
						break;
				}
				break;
			case KEY_CTRL_F4:
				Bdisp_AreaReverseVRAM(0, y*8, 127, y*8+7);	// reverse select line 
				switch (select) {
					case SETUP_Sketch: // Sketch  Line	Dot
						S_L_Style = 3 ; 
						break;
					case SETUP_Display: // Display
						ENG++;
						if ( ENG>3 ) ENG=0;
						if ( ENG>1 ) ENG=3;
						break;
					case SETUP_EditFontSize: // Edit Char Size
						EditFontSize &= 0xF0;	// mini
						EditFontSize |= 0x03;	// underbar cursor
						break;
					case SETUP_ExtendPict:		// Max Pict
						ExtendPict=0;
						HiddenRAM_MatAryInit();
						break;
					case SETUP_ExecTimeDsp: // TimeDsp
						if ( TimeDsp == 0 ) break;
						if ( TimeDsp & 0x2 )    TimeDsp &= ~0x2 ; // off reset GetKey1/2 
						else					TimeDsp |=  0x2 ; // on  reset GetKey1/2 
						break;
					case SETUP_ExtendList:		// Max List
						ExtendList=0;
						ExtListMax=0;
						HiddenRAM_MatAryInit();
						break;
					case SETUP_ForceReturnMode:
						ForceReturnMode = 3;
						break;
					case SETUP_Key1sttime: // Key Repeat First Count *ms
						KeyRepeatFirstCount = 20 ;
						break;
					case SETUP_KeyReptime: // Key Repeat Next Count *ms
						KeyRepeatNextCount  = 5 ;
						break;
					case SETUP_SkipUpDown: // Skipup/down count init
						PageUpDownNum = PageUpDownNumDefault ;
						break;
					case SETUP_RefrshCtlDD: // Refresh Ctrl DD Mode
						if ( RefreshCtrl ) Refreshtime = SelectNum4( Refreshtime+1 )-1;
						break;
					case SETUP_DefaultWaitcount: // Wait count init
						DefaultWaitcount = 0;
						Waitcount=DefaultWaitcount;
						break;
					default:
						break;
				}
				break;
			case KEY_CTRL_F5:
				Bdisp_AreaReverseVRAM(0, y*8, 127, y*8+7);	// reverse select line 
				switch (select) {
					case SETUP_EditFontSize: // Edit Char Size
						EditFontSize &= 0xF0;	// mini
						EditFontSize |= 0x04;	// underbar cursor & reverse
						break;
					case SETUP_RefrshCtlDD: // Refresh Ctrl DD Mode init
						RefreshCtrl = 1 ; // graphics only
						Refreshtime = 3-1 ; // 3/128
						break;
					default:
						break;
				}
				break;
			case KEY_CTRL_F6:
				VerDisp();
				break;
			default:
				break;
		}
		Bkey_Set_RepeatTime(KeyRepeatFirstCount,KeyRepeatNextCount);		// set cursor rep
	}
	SaveConfig();
	return select;
}
