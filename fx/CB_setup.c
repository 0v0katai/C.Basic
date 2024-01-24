
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

//---------------------------------------------------------------------------------------------
int KeyRepeatFirstCount=20;		// pointer to repeat time of first repeat (20:default)
int KeyRepeatNextCount = 5;		// pointer to repeat time of second repeat( 5:default)

int selectSetup=0;
int selectVar=0;
int selectMatrix=0;

//---------------------------------------------------------------------------------------------

void VerDisp() {
	unsigned int key;
	PopUpWin( 6 );
	locate( 3, 2 ); Print( (unsigned char*)"Basic Interpreter" );
	locate( 3, 3 ); Print( (unsigned char*)"&(Basic Compiler)" );
	locate( 3, 4 ); Print( (unsigned char*)"           v0.93 " );
	locate( 3, 6 ); Print( (unsigned char*)"     by sentaro21" );
	locate( 3, 7 ); Print( (unsigned char*)"          (c)2015" );
	GetKey(&key);
}

//----------------------------------------------------------------------------------------------
//		Setup
//----------------------------------------------------------------------------------------------

void FkeyS_L_(){
	int temp;
	temp=S_L_Style;
	Fkey_Clear( 0 );
	Fkey_dispN( 0, ""); Linesub(0*21+5,7*8+4,0*21+18,7*8+4, S_L_Normal,1);	// -----
	Fkey_Clear( 1 );
	Fkey_dispN( 1, ""); Linesub(1*21+5,7*8+4,1*21+18,7*8+4, S_L_Thick,1);	// =====
	Fkey_Clear( 2 );
	Fkey_dispN( 2, ""); Linesub(2*21+5,7*8+4,2*21+18,7*8+4, S_L_Broken,1);	// : : : :
	Fkey_Clear( 3 );
	Fkey_dispN( 3, ""); Linesub(3*21+5,7*8+4,3*21+18,7*8+4, S_L_Dot,1);	// . . . .
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
			locate( 1, 9-scrl); Print((unsigned char*)"Temin :");
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
	if ( VarMode ) Print((unsigned char*)"[int%]"); else Print((unsigned char*)"[double]");
}
void InitVar( double value, int VarMode, int vartop ) {
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
			for ( i=vartop; i<vartop+26; i++) REGINT[i]=value;
		} else {
			for ( i=vartop; i<vartop+26; i++) REG[i]=value;
		}
	}
}

void SetVarSel(int VarMode, int y) {
	Bdisp_AreaReverseVRAM(0, y*8, 127, y*8+7);	// reverse select line 
	locate(1,8); PrintLine((unsigned char *)" ",21);
	locate(1,8); SetVarDsp(VarMode);
}

int SetVar(int select){		// ----------- Set Variable
	char buffer[32];
	unsigned int key;
	int	cont=1;
	int scrl=0;
	int seltop=select;
	int i,y;
	int selectreplay=-1;
	int opNum=25;
	int small=0;
	double value=0;
	int VarMode=CB_INT;	// 0:double  1:int

	Cursor_SetFlashMode(0); 		// cursor flashing off

	if (select>25) { small=32; select-=32; }

	while (cont) {
		Bdisp_AllClr_VRAM();
		
		if (  select<seltop ) seltop = select;
		if ( (select-seltop) > 6 ) seltop = select-6;
		if ( (opNum -seltop) < 6 ) seltop = opNum -6; 
		for ( i=0; i<7; i++ ) {
			buffer[0]='A'+seltop+i+small;
			if ( VarMode ) {
				buffer[1]='%';
				buffer[2]='=';
				buffer[3]='\0';
				
			} else {
				buffer[1]='=';
				buffer[2]='\0';
			}
			locate(1,1+i); Print((unsigned char*)buffer);
			if ( VarMode ) {
				locate(4 ,1+i);
				sprintG(buffer, (double)REGINT[seltop+i+small], 18,LEFT_ALIGN);
			} else {
				locate(3 ,1+i);
				sprintG(buffer, REG[seltop+i+small], 19,LEFT_ALIGN);
			}
			Print((unsigned char*)buffer);
		}
		Fkey_dispN( 0, "A<>a");
		Fkey_dispN( 1, "Init");
		if ( VarMode ) Fkey_dispN_aA( 2, "D<>I"); else Fkey_dispN_Aa( 2, "D<>I");

		locate(12,8); SetVarDsp(VarMode);

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
				InitVar(value,VarMode, small);
				break;
			case KEY_CTRL_F3:
				VarMode=1-VarMode;
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
				y++;
				selectreplay = select; 
				if ( ( 0 <= select ) && ( select <=25 ) ) {	// regA to regZ
					if ( VarMode ) 
						REGINT[select+small]= InputNumD_full( 4, y, 18, (double)REGINT[select+small]);
					else
						REG[select+small]   = InputNumD_full( 3, y, 19, REG[select+small]);
				} else {
						selectreplay = -1; // replay cancel 
				}
				break;
				
			case KEY_CTRL_LEFT:
				if (selectreplay<0) break;
				SetVarSel(VarMode,y);
				y++;
				selectreplay = select; 
				if ( ( 0 <= select ) && ( select <=25 ) ) {	// regA to regZ
					if ( VarMode ) 
						REGINT[select+small]= InputNumD_replay( 4, y, 18, (double)REGINT[select+small]);
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
				SetVarSel(VarMode,y);
				y++;
				selectreplay = select; 
				if ( ( 0 <= select ) && ( select <=25 ) ) {	// regA to regZ
					if ( VarMode ) 
						REGINT[select+small]= InputNumD_Char( 4, y, 18, (double)REGINT[select+small], key);
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
    char *Matmode[]    ={"[m,n]","[X,Y]"};
    char *Pictmode[]    ={"S.Mem","Heap"};
	char buffer[22];
	unsigned int key;
	int	cont=1;
	int scrl=select-6;
	int y,cnt;
	int listmax=16;
	
	Cursor_SetFlashMode(0); 		// cursor flashing off
	
	if ( select > listmax ) select=0;
	if ( select < scrl ) scrl-=1;
	if ( scrl < 0 ) scrl=0;

	while (cont) {
		Bdisp_AllClr_VRAM();
		cnt=1;
		if ( scrl <=(cnt-1) ) {
			locate( 1, cnt-scrl); Print((unsigned char*)"Draw Type   :");		// 0
			locate(14, cnt-scrl); Print((unsigned char*)draw[(int)DrawType]);
		} cnt++;
		if ( scrl <=(cnt-1) ) {
			locate( 1, cnt-scrl); Print((unsigned char*)"Coord:      :");		// 1
			locate(14, cnt-scrl); Print((unsigned char*)onoff[Coord]);
		} cnt++;
		if ( scrl <=(cnt-1) ) {
			locate( 1, cnt-scrl); Print((unsigned char*)"Grid        :");		// 2
			locate(14, cnt-scrl); Print((unsigned char*)onoff[Grid]);
		} cnt++;
		if ( scrl <=(cnt-1) ) {
			locate( 1, cnt-scrl); Print((unsigned char*)"Axes        :");		// 3
			locate(14, cnt-scrl); Print((unsigned char*)onoff[Axes]);
		} cnt++;
		if ( scrl <=(cnt-1) ) {
			locate( 1, cnt-scrl); Print((unsigned char*)"Label       :");		// 4
			locate(14, cnt-scrl); Print((unsigned char*)onoff[Label]);
		} cnt++;
		if ( scrl <=(cnt-1) ) {
			locate( 1, cnt-scrl); Print((unsigned char*)"Derivative  :");		// 5
			locate(14, cnt-scrl); Print((unsigned char*)onoff[Derivative]);
		} cnt++;
		if ( scrl <=(cnt-1) ) {
			locate( 1, cnt-scrl); Print((unsigned char*)"Sketch Line :");		// 6
			locate(14, cnt-scrl); Print((unsigned char*)style[S_L_Style]);
		} cnt++;
		if ( ( scrl >=(cnt-7) ) && ( cnt-scrl > 0 ) ){
			locate( 1, cnt-scrl); Print((unsigned char*)"Angle       :");		// 7
			locate(14, cnt-scrl); Print((unsigned char*)degrad[Angle]);
		} cnt++;
		if ( ( scrl >=(cnt-7) ) && ( cnt-scrl > 0 ) ){
			locate( 1, cnt-scrl); Print((unsigned char*)"Display     :");		// 8
			locate(14, cnt-scrl); Print((unsigned char*)display[CB_Round.MODE]);
			buffer[0]='\0';
			sprintf((char*)buffer,"%d",CB_Round.DIGIT);
			locate(17, cnt-scrl); Print((unsigned char*)buffer);
			locate(19, cnt-scrl); 
			if (ENG) Print((unsigned char*)"/E");
		} cnt++;
		if ( ( scrl >=(cnt-7) ) && ( cnt-scrl > 0 ) ){
			locate( 1,cnt-scrl); Print((unsigned char*)"Break Stop  :");		// 9
			locate(14,cnt-scrl); Print((unsigned char*)onoff[BreakCheck]);
		} cnt++;
		if ( ( scrl >=(cnt-7) ) && ( cnt-scrl > 0 ) ){
			locate( 1,cnt-scrl); Print((unsigned char*)"Exec TimeDsp:");		// 10
			locate(14,cnt-scrl); Print((unsigned char*)onoff[TimeDsp]);
		} cnt++;
		if ( ( scrl >=(cnt-7) ) && ( cnt-scrl > 0 ) ){
			locate( 1,cnt-scrl); Print((unsigned char*)"IfEnd  Check:");		// 11
			locate(14,cnt-scrl); Print((unsigned char*)onoff[CheckIfEnd]);
		} cnt++;
		if ( ( scrl >=(cnt-7) ) && ( cnt-scrl > 0 ) ){
			locate( 1,cnt-scrl); Print((unsigned char*)"Key 1st time:");		// 12
			sprintf((char*)buffer,"%dms",KeyRepeatFirstCount*25);
			locate(14,cnt-scrl); Print((unsigned char*)buffer);
		} cnt++;
		if ( ( scrl >=(cnt-7) ) && ( cnt-scrl > 0 ) ){
			locate( 1,cnt-scrl); Print((unsigned char*)"Key Rep time:");		// 13
			sprintf((char*)buffer,"%dms",KeyRepeatNextCount*25);
			locate(14,cnt-scrl); Print((unsigned char*)buffer);
		} cnt++;
		if ( ( scrl >=(cnt-7) ) && ( cnt-scrl > 0 ) ){
			locate( 1,cnt-scrl); Print((unsigned char*)"Matrix  mode:");		// 14
			locate(14,cnt-scrl); Print((unsigned char*)Matmode[MatXYmode]);
		} cnt++;
		if ( ( scrl >=(cnt-7) ) && ( cnt-scrl > 0 ) ){
			locate( 1,cnt-scrl); Print((unsigned char*)"Pict    mode:");		// 15
			locate(14,cnt-scrl); Print((unsigned char*)Pictmode[PictMode]);
		} cnt++;
		if ( ( scrl >=(cnt-7) ) && ( cnt-scrl > 0 ) ){
			locate( 1,cnt-scrl); Print((unsigned char*)"Execute mode:");		// 16
			locate(14,cnt-scrl); Print((unsigned char*)mode[CB_INTDefault]);
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
			case 11: // IfEnd Check
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
			case 12: // Key Repeat mode
			case 13: // Key Repeat mode
				Fkey_DISPN( 0," +");
				Fkey_DISPN( 1," -");
				Fkey_dispN( 3, "Init");
				break;
			case 14: // Mat mode
				Fkey_dispN( 0, "m.n ");
				Fkey_dispN( 1, "X,Y ");
				break;
			case 15: // Mat mode
				Fkey_dispN( 0, "MEM ");
				Fkey_dispN( 1, "Heap");
				break;
			case 16: // Execute Mode
				Fkey_dispN( 0, "DBL ");
				Fkey_dispN( 1, "Int ");
				break;
			default:
				break;
		}
		
		Fkey_dispN( 5, "Ver.");
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
					case 11: // IfEnd Check
						CheckIfEnd = 1 ; // on
						break;
					case 12: // Key Repeat First Count *ms
						KeyRepeatFirstCount += 1 ;
						if ( KeyRepeatFirstCount > 40 ) KeyRepeatFirstCount=40;
						break;
					case 13: // Key Repeat Next Count *ms
						KeyRepeatNextCount += 1 ;
						if ( KeyRepeatNextCount > 20 ) KeyRepeatNextCount=20;
						break;
					case 14: // Matrix mode
						MatXYmode = 0 ; // m,n
						break;
					case 15: // Pict mode
						PictMode = 0 ; // Memory mode
						break;
					case 16: // CB mode
						CB_INTDefault = 0 ; // normal
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
					case 11: // IfEnd Check
						CheckIfEnd = 0 ; // off
						break;
					case 12: // Key Repeat First Count *ms
						KeyRepeatFirstCount -= 1 ;
						if ( KeyRepeatFirstCount < 1 ) KeyRepeatFirstCount=1;
						break;
					case 13: // Key Repeat Next Count *ms
						KeyRepeatNextCount -= 1 ;
						if ( KeyRepeatNextCount < 1 ) KeyRepeatNextCount=1;
						break;
					case 14: // Matrix mode
						MatXYmode = 1 ; // x,y
						break;
					case 15: // Pict mode
						PictMode = 1 ; // heap mode
						break;
					case 16: // CB mode
						CB_INTDefault = 1 ; // int
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
					case 12: // Key Repeat First Count *ms
						KeyRepeatFirstCount = 20 ;
						break;
					case 13: // Key Repeat Next Count *ms
						KeyRepeatNextCount  = 5 ;
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
