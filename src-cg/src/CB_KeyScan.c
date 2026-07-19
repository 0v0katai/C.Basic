

//
// fx-CG series KeyScan
//
// original source by SuperH-based fx calculators
//
// this modified source is written by sentaro21
//

//    06    05   04   03   02   01   00        (SH4A)
//--------------------------------------
//0B                  DIAG     OSUPD     0B    A44B000B
//0A                                     0A    A44B000A
//09  F1    F2   F3   F4   F5   F6       09    A44B0009
//08  SHIFT OPTN VARS MENU ��  ��        08    A44B0008
//07  ALPHA ^2   ^    EXIT ��   ��       07    A44B0007
//06  XTT   log  ln   sin  cos  tan      06    A44B0006
//05  ab/c  F<>D  (    )    ,   ��       05    A44B0005
//04  7     8    9    DEL                04    A44B0004
//03  4     5    6    x    div           03    A44B0003
//02  1     2    3    +    -             02    A44B0002
//01  0     .    EXP  (-)  EXE           01    A44B0001
//00                                 AC  00    A44B0000
//--------------------------------------
//    06    05   04   03   02   01   00        (SH4A)
//
//
#include "prizm.h"
#include "CB.h"

//#include "keyboard.hpp"
//#include "keyboard_syscalls.h"

//#include "CB_KeyScan.h"

#define	Bkey_GetKeyWait GetKeyWait_OS

//----------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------
char Getkey_shift = 0;
uint8_t  Recent_code = 0;

void KeyRecover() {
//	CB_Getkey();
//	KeyCheckAC();				//SH4
//	KeyCheckEXE();
//	KeyCheckEXIT();
	keydown(KEY_SHIFT);
    keydown(KEY_4);
	Keyboard_ClrBuffer();
	Getkey_shift=0;
	Recent_code=0;
//	Sleep(10);
}

int GetKey_DisableMenu( int *key ) {
	int r;
	DisableGetkeyToMainFunctionReturn();
	// SetGetkeyToMainFunctionReturnFlag( 0 );	// disabled
	r=GetKey(key);
	EnableGetkeyToMainFunctionReturn();
	// SetGetkeyToMainFunctionReturnFlag( 1 );	// enabled
	return r;
}
int GetKey_DisableMenu_pushpop( int *key ) {
	int r;
	SaveDisp(SAVEDISP_PAGE2);
	r=GetKey_DisableMenu(key);
	RestoreDisp(SAVEDISP_PAGE2);
	return r;
}
int GetKey_DisableMenuCatalog( int *key ) {
	int r;
	Bkey_SetFlag( 0x80 );	// disable Catalog function
	r=GetKey_DisableMenu(key);
	Bkey_ClrFlag( 0x80 ) ;	// enable Catalog function
	return r;
}
int GetKey_DisableCatalog( int *key ) {
	int r;
	Bkey_SetFlag( 0x80 );		// disable Catalog function
	r=GetKey(key);
	Bkey_ClrFlag( 0x80 );		// enable Catalog function
	return r;
}

int CB_Getkey1() {			// CasioBasic Getkey SDK compatible
	int t,th;
	// Getkey_shift=0;
	// Recent_code=0;
	t =RTC_GetTicks()-CB_TicksStart;				// halt ticks count
	th=(int)GetTicks32768()-CB_HiTicksStart;		// halt ticks count
	Bdisp_PutDisp_DD();
	int row, col;
	GetKeyWait_OS(&row, &col,
		KEYWAIT_HALTON_TIMEROFF,
		0, 0, &(unsigned short){0});
	if ( TimeDsp & 0x02 ) {
		CB_TicksStart=RTC_GetTicks();				// reset ticks count
		CB_HiTicksStart=(int)GetTicks32768();		// reset ticks count
	} else  {
		CB_TicksStart=RTC_GetTicks()-t;				// restart ticks count
		CB_HiTicksStart=(int)GetTicks32768()-th;	// restart ticks count
	}
	return getkey_value(row, col - 1);
}

int CB_Getkey2() {			// CasioBasic Getkey SDK compatible with buffer clear
	KeyRecover();
	return CB_Getkey1() ;
}


int CB_GetkeyN( int n, int disableCatalog, int sdkcode ) {			// CasioBasic Getkey 
	int key;
	switch ( n ) {
		case 0:
			KeyRecover();
			return 0;
		case 1:
		case 2:
			CB_StatusDisp_Run();
			// if ( disableCatalog ) Bkey_SetFlag( 0x80 );		// disable Catalog function
			if ( n==1 ) key=CB_Getkey1();
			else		key=CB_Getkey2();
			// if ( disableCatalog ) Bkey_ClrFlag( 0x80 ) ;	// enable Catalog function
			DrawFrame( CB_FrameColor );
			break;
		default:
			return 0;
	}
	return key;
}