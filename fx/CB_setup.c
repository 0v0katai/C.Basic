
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

//---------------------------------------------------------------------------------------------
int KeyRepeatFirstCount=20;		// pointer to repeat time of first repeat (20:default)
int KeyRepeatNextCount = 5;		// pointer to repeat time of second repeat( 5:default)

short PageUpDownNum = PageUpDownNumDefault;	// PageUp/Down counter

int selectSetup=0;
int selectVar=0;
int selectMatrix=0;

//---------------------------------------------------------------------------------------------

void VerDisp() {
	unsigned int key;
	PopUpWin( 6 );
	locate( 3, 2 ); Print( (unsigned char*)"Basic Interpreter" );
	locate( 3, 3 ); Print( (unsigned char*)"&(Basic Compiler)" );
	locate( 3, 4 ); Print( (unsigned char*)"          v1.00\xE6\x41\x36" );
	locate( 3, 6 ); Print( (unsigned char*)"     by sentaro21" );
	locate( 3, 7 ); Print( (unsigned char*)"          (c)2017" );
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

void SetVeiwWindowInit(){	// Initialize	return 0: no change  -1 : change
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

int SetVarChar( char *buffer, int c ) {
	int ptr=0;
	if ( ( c == 26 ) || ( c == 26+32 ) ) {	// <r>
		buffer[ptr++]=0xCD;	//	'r'
	} else
	if ( ( c == 27 ) || ( c == 27+32 ) ) { // Theta
		buffer[ptr++]=0xE6;		// Theta
		buffer[ptr++]=0x47;
	} else { 
		buffer[ptr++]='A'+c;
	}
	return ptr;
}

int SetVar(int select){		// ----------- Set Variable
	char buffer[32];
	unsigned int key;
	int	cont=1;
	int scrl=0;
	int seltop=select;
	int i,j,k,f,y;
	int selectreplay=-1;
	int opNum=25+2;
	int small=0;
	double value=0;
	int VarMode=CB_INT;	// 0:double  1:int
	int hex=0;	// 0:normal  1:hex

	Cursor_SetFlashMode(0); 		// cursor flashing off
	
	if (select>=32) { small=32; select-=32; }

	while (cont) {
		Bdisp_AllClr_VRAM();
		
		if (  select<seltop ) seltop = select;
		if ( (select-seltop) > 6 ) seltop = select-6;
		if ( (opNum -seltop) < 6 ) seltop = opNum -6; 
		
		for ( i=0; i<7; i++ ) {
			j=0;
			k=seltop+i+small;
			if ( VarMode ) {		// Int variable
				buffer[j++]='%';
			}
			
			j+=SetVarChar( buffer+j, k );
			
			if ( ( k == 26+32 ) || ( k == 27+32 ) ) k=k-32;
			if ( VarMode ) {		// Int variable
				f=( LocalInt[k] == &REGINT[k] );
			} else {				// Double variable
				f=( LocalDbl[k] == &REG[k] );
			}
			if ( f ) {
				buffer[j++]='=';
			} else {
				buffer[j++]=0xE5;		// reverse '='
				buffer[j++]=0xB8;
			}
			
			buffer[j++]='\0';
			locate(1,1+i); Print((unsigned char*)buffer);
			
			if ( VarMode ) {
				locate(4 ,1+i);		// int
				if ( hex )	sprintf(buffer,"0x%08X        ",(int)LocalInt[k][0]);
					else	sprintG(buffer, (double)LocalInt[k][0], 18,LEFT_ALIGN);
			} else {
				locate(3 ,1+i);		// dbl
				if ( hex )	SetVarDblToHex( buffer, LocalDbl[k][0] );
					else	sprintG(buffer, (double)LocalDbl[k][0], 18,LEFT_ALIGN);
			}
			Print((unsigned char*)buffer);
		}
		Fkey_dispN( 0, "A<>a");
		Fkey_dispN( 1, "Init");
		if ( VarMode ) Fkey_dispN_aA( 2, "D<>I"); else Fkey_dispN_Aa( 2, "D<>I");
		if ( hex ) Fkey_dispN( 5, "\xE6\x91\x44\x65\x63"); else Fkey_dispN( 5, "\xE6\x91Hex");

		locate(12,8); SetVarDsp(VarMode);

		y = (select-seltop) ;
		Bdisp_AreaReverseVRAM(0, y*8, 127, y*8+7);	// reverse select line 
		Bdisp_PutDisp_DD();

		k=select; if ( select<=25 ) k+=small;
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
				y++;
				selectreplay = select; 
				if ( ( 0 <= select ) && ( select <=25+2 ) ) {	// regA to regZ
					if ( VarMode ) 
						LocalInt[k][0]= InputNumD_fullhex( 4, y, 18, (double)LocalInt[k][0], hex);
					else
						LocalDbl[k][0]= InputNumD_fullhex( 3, y, 19, (double)LocalDbl[k][0], hex);
				} else {
						selectreplay = -1; // replay cancel 
				}
				break;
				
			case KEY_CTRL_LEFT:
				if (selectreplay<0) break;
				SetVarSel(VarMode,y);
				y++;
				selectreplay = select; 
				if ( ( 0 <= select ) && ( select <=25+2 ) ) {	// regA to regZ
					if ( VarMode ) 
						LocalInt[k][0]= InputNumD_replay( 4, y, 18, (double)LocalInt[k][0]);
					else
						LocalDbl[k][0]= InputNumD_replay( 3, y, 19, (double)LocalDbl[k][0]);
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
				if ( ( 0 <= select ) && ( select <=25+2 ) ) {	// regA to regZ
					if ( VarMode ) 
						LocalInt[k][0]= InputNumD_Char( 4, y, 18, (double)LocalInt[k][0], key);
					else
						LocalDbl[k][0]= InputNumD_Char( 3, y, 19, (double)LocalDbl[k][0], key);
				} else {
						selectreplay = -1; // replay cancel 
				}
		}
	}
	SaveConfig();
	if ( small ) select+=32;
	return select;
}

//-----------------------------------------------------------------------------
//--------------------------------------------------------------
int DateCursorY;
int TimeCursorY;

void DateTimePrint(){		// timer IRQ handler
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
	Bdisp_PutDisp_DD();
}

void TimePrintSetMode(int set){	// 1:on  0:off
	switch (set) {
		case 0:
			KillTimer(ID_USER_TIMER1);
			break;
		case 1:
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
		n=InputNumD(3+strlen(buffer), 5, log10(max)+1, n, ' ', REV_OFF, FLOAT_OFF, EXP_OFF);		// 0123456789
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
		n=InputNumD(15, 5, 4, n, ' ', REV_OFF, FLOAT_OFF, EXP_OFF);		// 0123456789
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
	locate( 6,5); Print((unsigned char *)"[1~128]:");
	while (1) {
		n=InputNumD(14, 5, 3, n, ' ', REV_OFF, FLOAT_OFF, EXP_OFF);		// 0123456789
		if ( (1<=n)&&(n<=128) ) break;
		n=n0;
	}
	return n ; // ok
}

//--------------------------------------------------------------

int SetupG(int select){		// ----------- Setup 
    const char *onoff[]   ={"off","on"};
    const char *draw[]    ={"Connect","Plot"};
    const char *style[]   ={"Normal","Thick","Broken","Dot"};
    const char *degrad[]  ={"Deg","Rad","Grad"};
    const char *display[] ={"Nrm","Fix","Sci"};
    const char *ENGmode[] ={"  ","/E","  ","/3"};
    const char *mode[]    ={"Dbl#","Int%"};
    const char *Matmode[]    ={"[m,n]","[X,Y]"};
    const char *Matbase[]    ={"0","1"};
    const char *Pictmode[]    ={"S.Mem","Heap"};
    const char *PictmodeSD[]  ={"SDcard ","Heap"};
    const char *Stragemode[]  ={"S.Mem","SDcard "};
    const char *DDmode[]    ={"off","Grph","All"};
	char buffer[22];
	unsigned int key;
	int	cont=1;
	int scrl=select-6;
	int y,cnt;
	char DateStr[16];
	char TimeStr[16];
	int year,month,day,hour,min,sec;
	int listmax=25;
	
	Cursor_SetFlashMode(0); 		// cursor flashing off
	
	if ( select > listmax ) select=0;
	if ( select < scrl ) scrl-=1;
	if ( scrl < 0 ) scrl=0;

	while (cont) {
		Bdisp_AllClr_VRAM();
		
		DateToStr(DateStr);
		TimeToStr(TimeStr);
		DateCursorY=-1;
		TimeCursorY=-1;
		
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
			Print((unsigned char*)ENGmode[ENG]);
		} cnt++;
		if ( ( scrl >=(cnt-7) ) && ( cnt-scrl > 0 ) ){
			locate( 1,cnt-scrl); Print((unsigned char*)"Use Hidn RAM:");		// 9
			locate(14,cnt-scrl); Print((unsigned char*)onoff[UseHiddenRAM]);
		} cnt++;
		if ( ( scrl >=(cnt-7) ) && ( cnt-scrl > 0 ) ){
			locate( 1,cnt-scrl); Print((unsigned char*)"Break Stop  :");		// 10
			locate(14,cnt-scrl); Print((unsigned char*)onoff[BreakCheck]);
		} cnt++;
		if ( ( scrl >=(cnt-7) ) && ( cnt-scrl > 0 ) ){
			locate( 1,cnt-scrl); Print((unsigned char*)"Exec TimeDsp:");		// 11
			locate(14,cnt-scrl); Print((unsigned char*)onoff[TimeDsp]);
		} cnt++;
		if ( ( scrl >=(cnt-7) ) && ( cnt-scrl > 0 ) ){
			locate( 1,cnt-scrl); Print((unsigned char*)"IfEnd Check :");		// 12
			locate(14,cnt-scrl); Print((unsigned char*)onoff[CheckIfEnd]);
		} cnt++;
		if ( ( scrl >=(cnt-7) ) && ( cnt-scrl > 0 ) ){
			locate( 1,cnt-scrl); Print((unsigned char*)"Key 1st time:");		// 13
			sprintf((char*)buffer,"%dms",KeyRepeatFirstCount*25);
			locate(14,cnt-scrl); Print((unsigned char*)buffer);
		} cnt++;
		if ( ( scrl >=(cnt-7) ) && ( cnt-scrl > 0 ) ){
			locate( 1,cnt-scrl); Print((unsigned char*)"Key Rep time:");		// 14
			sprintf((char*)buffer,"%dms",KeyRepeatNextCount*25);
			locate(14,cnt-scrl); Print((unsigned char*)buffer);
		} cnt++;
		if ( ( scrl >=(cnt-7) ) && ( cnt-scrl > 0 ) ){
			locate( 1,cnt-scrl); Print((unsigned char*)"SkipUp/Down :");		// 15
			sprintf((char*)buffer,"%d",PageUpDownNum);
			locate(14,cnt-scrl); Print((unsigned char*)buffer);
		} cnt++;
		if ( ( scrl >=(cnt-7) ) && ( cnt-scrl > 0 ) ){
			locate( 1,cnt-scrl); Print((unsigned char*)"Mat Dsp mode:");		// 16
			locate(14,cnt-scrl); Print((unsigned char*)Matmode[MatXYmode]);
		} cnt++;
		if ( ( scrl >=(cnt-7) ) && ( cnt-scrl > 0 ) ){
			locate( 1,cnt-scrl); Print((unsigned char*)"Matrix base :");		// 17
			locate(14,cnt-scrl); Print((unsigned char*)Matbase[MatBaseDefault]);
		} cnt++;
		if ( ( scrl >=(cnt-7) ) && ( cnt-scrl > 0 ) ){
			locate( 1,cnt-scrl); Print((unsigned char*)"Pict mode   :");		// 18
			locate(14,cnt-scrl); if ( StorageMode ) Print((unsigned char*)PictmodeSD[PictMode]); else Print((unsigned char*)Pictmode[PictMode]);
		} cnt++;
		if ( ( scrl >=(cnt-7) ) && ( cnt-scrl > 0 ) ){
			locate( 1,cnt-scrl); Print((unsigned char*)"ACBreak     :");		// 19
			locate(14,cnt-scrl); Print((unsigned char*)onoff[ACBreak]);
		} cnt++;
		if ( ( scrl >=(cnt-7) ) && ( cnt-scrl > 0 ) ){
			locate( 1,cnt-scrl); Print((unsigned char*)"RefrshCtl DD:");		// 20
			locate(14,cnt-scrl); Print((unsigned char*)DDmode[RefreshCtrl]);
			buffer[0]='\0';
			sprintf((char*)buffer,"%2d/128",Refreshtime+1);
			if ( RefreshCtrl ) PrintMini(17*6+2,(cnt-scrl)*8-6,(unsigned char*)buffer,MINI_OVER);
		} cnt++;
		if ( ( scrl >=(cnt-7) ) && ( cnt-scrl > 0 ) ){							// 21
			DateCursorY=cnt-scrl;
			DateTimePrint();
		} cnt++;
		if ( ( scrl >=(cnt-7) ) && ( cnt-scrl > 0 ) ){							// 22
			TimeCursorY=cnt-scrl;
			DateTimePrint();
		} cnt++;
		if ( ( scrl >=(cnt-7) ) && ( cnt-scrl > 0 ) ){
			locate( 1,cnt-scrl); Print((unsigned char*)"Storage mode:");		// 23
			locate(14,cnt-scrl); Print((unsigned char*)Stragemode[StorageMode]);
		} cnt++;
		if ( ( scrl >=(cnt-7) ) && ( cnt-scrl > 0 ) ){
			locate( 1,cnt-scrl); Print((unsigned char*)"Force g1m save:");		// 24
			locate(16,cnt-scrl); Print((unsigned char*)onoff[ForceG1Msave]);
		} cnt++;
		if ( ( scrl >=(cnt-7) ) && ( cnt-scrl > 0 ) ){
			locate( 1,cnt-scrl); Print((unsigned char*)"Execute mode:");		// 25
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
			case 9: // UseHiddenRAM
			case 10: // BreakCheck
			case 11: // TimeDsp
			case 12: // IfEnd Check
			case 19: // ACBreak Check
			case 24: // Force g1m save
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
			case 13: // Key Repeat mode
			case 14: // Key Repeat mode
				Fkey_DISPN( 0," +");
				Fkey_DISPN( 1," -");
				Fkey_dispN( 3, "Init");
				break;
			case 15: // RollUp/Down number
				Fkey_DISPN( 0," +");
				Fkey_DISPN( 1," -");
				Fkey_dispN( 2, "Num");
				Fkey_dispN( 3, "Init");
				break;
			case 16: // Mat display mode
				Fkey_dispN( 0, "m.n ");
				Fkey_dispN( 1, "X,Y ");
				break;
			case 17: // Mat base
				Fkey_dispN( 0, " 0 ");
				Fkey_dispN( 1, " 1 ");
				break;
			case 18: // Pict mode
				if ( StorageMode ) Fkey_dispN( 0, " SD "); else Fkey_dispN( 0, "MEM ");
				Fkey_dispN( 1, "Heap");
				break;
			case 20: // Refresh Ctrl DD Mode
				Fkey_dispN( 0, "off ");
				Fkey_dispN( 1, "Grph");
				Fkey_dispN( 2, "All ");
				if ( RefreshCtrl ) Fkey_dispN( 3, "time");
				Fkey_dispN( 4, "Init");
				break;
			case 21: // DATE
				Fkey_dispN( 0, "Year");
				Fkey_dispN( 1, "Mth");
				Fkey_dispN( 2, "Day");
				break;
			case 22: // TIME
				Fkey_dispN( 0, "Hour");
				Fkey_dispN( 1, "Min");
				Fkey_dispN( 2, "Sec");
				break;
			case 23: // Strage mode
				Fkey_dispN( 0, "MEM ");
				Fkey_dispN( 1, " SD ");
				break;
			case 25: // Execute Mode
				Fkey_dispN( 0, "DBL ");
				Fkey_dispN( 1, "Int ");
				break;
			default:
				break;
		}
		
		Fkey_dispN( 5, "Ver.");
		Bdisp_PutDisp_DD();
		
		DateCursorY+=(select-scrl)*0x100;
		TimeCursorY+=(select-scrl)*0x100;
		TimePrintSetMode( 1 ) ;			// Date/Time print IRQ on
		GetKey( &key );
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
						CB_Round.DIGIT=SelectNum2("Fix",CB_Round.DIGIT,0,15);
						CB_Round.MODE =Fix;
						break;
					case 9: // Hidden RAM
						if ( IsHiddenRAM ) UseHiddenRAM = 1 ; // on
						break;
					case 10: // Break
						BreakCheck = 1 ; // on
						break;
					case 11: // TimeDsp
						TimeDsp = 1 ; // on
						break;
					case 12: // IfEnd Check
						CheckIfEnd = 1 ; // on
						break;
					case 13: // Key Repeat First Count *ms
						KeyRepeatFirstCount += 1 ;
						if ( KeyRepeatFirstCount > 40 ) KeyRepeatFirstCount=40;
						break;
					case 14: // Key Repeat Next Count *ms
						KeyRepeatNextCount += 1 ;
						if ( KeyRepeatNextCount > 20 ) KeyRepeatNextCount=20;
						break;
					case 15: // Roolup/down count +
						PageUpDownNum++; if ( PageUpDownNum > 9999 ) PageUpDownNum = 9999;
						break;
					case 16: // Matrix Display mode
						MatXYmode = 0 ; // m,n
						break;
					case 17: // Matrix base
						MatBaseDefault = 0 ; // 
						MatBase = MatBaseDefault;
						break;
					case 18: // Pict mode
						PictMode = 0 ; // Memory mode
						break;
					case 19: // ACBreak
						ACBreak = 1 ; // on
						break;
					case 20: // Refresh Ctrl DD Mode
						RefreshCtrl = 0 ; // off
						break;
					case 21: // DATE year
						year = (DateStr[0]-'0')*1000+(DateStr[1]-'0')*100+(DateStr[2]-'0')*10+(DateStr[3]-'0');
						year = SelectNum2("Year",year,0,9999);
						DateStr[0]=(year/1000)+'0';
						DateStr[1]=(year/100)%10+'0';
						DateStr[2]=(year/10)%10+'0';
						DateStr[3]=(year)%10+'0';
						StorDATE( DateStr );
						break;
					case 22: // Time hour
						hour = (TimeStr[0]-'0')*10+(TimeStr[1]-'0');
						hour = SelectNum2("Hour",hour,0,23);
						TimeStr[0]=(hour/10)%10+'0';
						TimeStr[1]=(hour)%10+'0';
						StorTIME( TimeStr );
						break;
					case 23: // Strage mode
						StorageMode = 0 ; // Memory mode
						break;
					case 24: // Force g1m save
						ForceG1Msave = 1 ; // g1m or text
						break;
					case 25: // CB mode
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
						CB_Round.DIGIT=SelectNum2("Sci",CB_Round.DIGIT,0,15);
						CB_Round.MODE =Sci;
						break;
					case 9: // Hidden RAM
						UseHiddenRAM = 0 ; // off
						break;
					case 10: // Break
						BreakCheck = 0 ; // off
						break;
					case 11: // TimeDsp
						TimeDsp = 0 ; // off
						break;
					case 12: // IfEnd Check
						CheckIfEnd = 0 ; // off
						break;
					case 13: // Key Repeat First Count *ms
						KeyRepeatFirstCount -= 1 ;
						if ( KeyRepeatFirstCount < 1 ) KeyRepeatFirstCount=1;
						break;
					case 14: // Key Repeat Next Count *ms
						KeyRepeatNextCount -= 1 ;
						if ( KeyRepeatNextCount < 1 ) KeyRepeatNextCount=1;
						break;
					case 15: // Roolup/down count -
						PageUpDownNum--; if ( PageUpDownNum < PageUpDownNumDefault ) PageUpDownNum = PageUpDownNumDefault;
						break;
					case 16: // Matrix display mode
						MatXYmode = 1 ; // x,y
						break;
					case 17: // Matrix base
						MatBaseDefault = 1 ; // base
						MatBase = MatBaseDefault;
						break;
					case 18: // Pict mode
						PictMode = 1 ; // heap mode
						break;
					case 19: // ACBreak
						ACBreak = 0 ; // off
						break;
					case 20: // Refresh Ctrl DD Mode
						RefreshCtrl = 1 ; // graphics
						break;
					case 21: // DATE month
						month = (DateStr[5]-'0')*10+(DateStr[6]-'0');
						month = SelectNum2("Month",month,1,12);
						DateStr[5]=(month/10)%10+'0';
						DateStr[6]=(month)%10+'0';
						StorDATE( DateStr );
						break;
					case 22: // Time min
						min = (TimeStr[3]-'0')*10+(TimeStr[4]-'0');
						min = SelectNum2("Min",min,0,59);
						TimeStr[3]=(min/10)%10+'0';
						TimeStr[4]=(min)%10+'0';
						StorTIME( TimeStr );
						break;
					case 23: // Strage mode
						StorageMode = CheckSD() ; // SD mode
						break;
					case 24: // Force g1m save
						ForceG1Msave = 0 ; // g1m and text
						break;
					case 25: // CB mode
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
					case 6: // Sketch Line	Broken
						S_L_Style = 2 ; 
						break;
					case 7: // Angle
						Angle = 2 ; // Grad
						break;
					case 8: // Display
						CB_Round.DIGIT=SelectNum2("Nrm",CB_Round.DIGIT,0,15);
						CB_Round.MODE =Norm;
						break;
					case 15: // Roolup/down count init
						PageUpDownNum = SelectNum3( PageUpDownNum );
						break;
					case 20: // Refresh Ctrl DD Mode
						RefreshCtrl = 2 ; // graphics+text
						break;
					case 21: // DATE day
						day = (DateStr[8]-'0')*10+(DateStr[9]-'0');
						day = SelectNum2("Day",day,1,31);
						DateStr[8]=(day/10)%10+'0';
						DateStr[9]=(day)%10+'0';
						StorDATE( DateStr );
						break;
					case 22: // Time sec
						sec = (TimeStr[6]-'0')*10+(TimeStr[7]-'0');
						sec = SelectNum2("Min",sec,0,59);
						TimeStr[6]=(sec/10)%10+'0';
						TimeStr[7]=(sec)%10+'0';
						StorTIME( TimeStr );
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
						ENG++;
						if ( ENG>3 ) ENG=0;
						if ( ENG>1 ) ENG=3;
						break;
					case 13: // Key Repeat First Count *ms
						KeyRepeatFirstCount = 20 ;
						break;
					case 14: // Key Repeat Next Count *ms
						KeyRepeatNextCount  = 5 ;
						break;
					case 15: // Roolup/down count init
						PageUpDownNum = PageUpDownNumDefault ;
						break;
					case 20: // Refresh Ctrl DD Mode
						if ( RefreshCtrl ) Refreshtime = SelectNum4( Refreshtime+1 )-1;
						break;
					default:
						break;
				}
				break;
			case KEY_CTRL_F5:
				Bdisp_AreaReverseVRAM(0, y*8, 127, y*8+7);	// reverse select line 
				switch (select) {
					case 20: // Refresh Ctrl DD Mode init
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
