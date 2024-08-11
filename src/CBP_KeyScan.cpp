extern "C" {

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
//08  SHIFT OPTN VARS MENU Å©  Å™        08    A44B0008
//07  ALPHA ^2   ^    EXIT Å´   Å®       07    A44B0007
//06  XTT   log  ln   sin  cos  tan      06    A44B0006
//05  ab/c  F<>D  (    )    ,   Å®       05    A44B0005
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
#include "CBP.h"

//#include "keyboard.hpp"
//#include "keyboard_syscalls.h"

//#include "CBP_KeyScan.h"

#define	Bkey_GetKeyWait GetKeyWait_OS

//----------------------------------------------------------------------------------------------



int CheckKeyRow7305( int row ){
	short*KEYPORT=(short*)0xA44B0000;
	short result=KEYPORT[row>>1];
	if ( row & 1 ) result/=0x100;
	return result & 0xFF ;
}

int CheckKeyRow( int row ){
	return CheckKeyRow7305( row );
}

int KeyScanDown(int keyscan_code){
	int row,col,rowdata;
	row = keyscan_code & 0x0F;
	col = keyscan_code >> 4;

	return ( CheckKeyRow7305(row) & col ) ;		//SH4A
}

int KeyScanDownAC(){
	int result=0;
	int n,s,t;
	n=Waitcount;
	if ( n<=0 ) return KeyScanDown(KEYSC_AC) ; 
	if ( n>1  ) n*=BREAKCOUNT;
	while ( n ) {
		HourGlass();
		result = KeyScanDown(KEYSC_AC);
		if ( result ) break;
		n--;
	}
	return result;
}

//----------------------------------------------------------------------------------------------
char Getkey_shift=0;
short  Recent_rowcode=0;
short  Recent_code=0;


int CB_Getkey() {			// CasioBasic Getkey compatible
	int key;
	int i,row,c,SH3;
	int code=0;
	row=1;
	
	if ( Recent_code ) {
		if ( KeyScanDown(Recent_rowcode) ) return Recent_code ;
		else {
			Recent_rowcode=0;
			Recent_code=0;
		}
	}

	for ( row=1; row<10; row++) {
		if (SH3) c=CheckKeyRow(row);  else  c=CheckKeyRow7305(row);
		if ( c & 0x40 ) { code=70+row; break; }	//
		if ( c & 0x20 ) { code=60+row; break; }	//
		if ( c & 0x10 ) { code=50+row; break; }	//
		if ( c & 0x08 ) { code=40+row; break; }	//
		if ( c & 0x04 ) { code=30+row; break; }	//
		if ( c & 0x02 ) { code=20+row; break; }	//
	}
	
	Recent_rowcode=(c<<4)+row;
	Recent_code   =code;
	
	if ( KeyScanDown(KEYSC_AC) ) code=34;

	return code;
}

int KeyCheckAC() {		// [AC]
	int kcode1 = 0, kcode2 = 0, flag0 = 0;
	unsigned short unused = 0;

	if ( Bkey_GetKeyWait(&kcode1,&kcode2,KEYWAIT_HALTOFF_TIMEROFF,0,1,&unused ) == KEYREP_KEYEVENT ) {
		if ( ( kcode1 == 1 ) && (kcode2 == 1 ) ) flag0=1; // [AC] is down
	}
	return flag0;
}
int KeyCheckEXE() {		// [EXE]
	int kcode1 = 0, kcode2 = 0, flag0 = 0;
	unsigned short unused = 0;

	if ( Bkey_GetKeyWait(&kcode1,&kcode2,KEYWAIT_HALTOFF_TIMEROFF,0,1,&unused ) == KEYREP_KEYEVENT ) {
		if ( ( kcode1 == 3 ) && (kcode2 == 2 ) ) flag0=1; // [EXE] is down
	}
	return flag0;
}
int KeyCheckEXIT() {		// [EXIT]
	int kcode1 = 0, kcode2 = 0, flag0 = 0;
	unsigned short unused = 0;

	if ( Bkey_GetKeyWait(&kcode1,&kcode2,KEYWAIT_HALTOFF_TIMEROFF,0,1,&unused ) == KEYREP_KEYEVENT ) {
		if ( ( kcode1 == 4 ) && (kcode2 == 8 ) ) flag0=1; // [EXIT] is down
	}
	return flag0;
}
int KeyCheckSHIFT() {		// [SHIFT]
	int kcode1 = 0, kcode2 = 0, flag0 = 0;
	unsigned short unused = 0;

	if ( Bkey_GetKeyWait(&kcode1,&kcode2,KEYWAIT_HALTOFF_TIMEROFF,0,1,&unused ) == KEYREP_KEYEVENT ) {
		if ( ( kcode1 == 7 ) && (kcode2 == 9 ) ) flag0=1; // [SHIFT] is down
	}
	return flag0;
}
int KeyCheckCHAR4() {		// [4]
	int kcode1 = 0, kcode2 = 0, flag0 = 0;
	unsigned short unused = 0;

	if ( Bkey_GetKeyWait(&kcode1,&kcode2,KEYWAIT_HALTOFF_TIMEROFF,0,1,&unused ) == KEYREP_KEYEVENT ) {
		if ( ( kcode1 == 7 ) && (kcode2 == 4 ) ) flag0=1; // [4] is down
	}
	return flag0;
}
int KeyCheckCHAR3() {		// [3]
	int kcode1 = 0, kcode2 = 0, flag0 = 0;
	unsigned short unused = 0;

	if ( Bkey_GetKeyWait(&kcode1,&kcode2,KEYWAIT_HALTOFF_TIMEROFF,0,1,&unused ) == KEYREP_KEYEVENT ) {
		if ( ( kcode1 == 5 ) && (kcode2 == 3 ) ) flag0=1; // [3] is down
	}
	return flag0;
}
int KeyCheckCHAR6() {		// [6]
	int kcode1 = 0, kcode2 = 0, flag0 = 0;
	unsigned short unused = 0;

	if ( Bkey_GetKeyWait(&kcode1,&kcode2,KEYWAIT_HALTOFF_TIMEROFF,0,1,&unused ) == KEYREP_KEYEVENT ) {
		if ( ( kcode1 == 5 ) && (kcode2 == 4 ) ) flag0=1; // [6] is down
	}
	return flag0;
}

int Bkey_GetKeyWait_sub( int kcode1, int kcode2 ) {		//
	int flag0 = 0;
	unsigned short unused = 0;
	if ( Bkey_GetKeyWait(&kcode1,&kcode2,KEYWAIT_HALTOFF_TIMEROFF,0,1,&unused ) == KEYREP_KEYEVENT ) {
		if ( ( kcode1 == 1 ) && (kcode2 == 1 ) ) flag0=1; // [AC] is down
	}
	return flag0;
}

int KeyCheckF1() {		// [F1]
	return Bkey_GetKeyWait_sub( 7, 10 ); // [F1] is down
}
//int KeyCheckDEL() {		// [DEL]
//	return Bkey_GetKeyWait_sub( 4, 5 ); // [DEL] is down
//}
int KeyCheckPMINUS() {		// [(-)]
	return CheckKeyRow7305(1) & 0x08; // [(-)] is down
//	return Bkey_GetKeyWait_sub( 4, 2 ); // [(-)] is down
}

void KeyRecover() {
//	CB_Getkey();
//	KeyCheckAC();				//SH4
//	KeyCheckEXE();
//	KeyCheckEXIT();
	KeyCheckSHIFT();
	KeyCheckCHAR4();
	Keyboard_ClrBuffer();
	Getkey_shift=0;
	Recent_code=0;
//	Sleep(10);
}

int CB_Getkey0() {			// CasioBasic Getkey 
	int key=0;
//	key=CB_Getkey();
	KeyRecover();
	return key;
}

int CB_KeyCodeCnvt( unsigned int key ) {			// CasioBasic Getkey SDK compatible
	int code;
	switch (key) {
		case KEY_CHAR_0:
		case KEY_CHAR_IMGNRY:
		case KEY_CHAR_Z:
			code=71;
			break;
		case KEY_CHAR_DP:
		case KEY_CHAR_EQUAL:
		case KEY_CHAR_SPACE:
			code=61;
			break;
		case KEY_CHAR_EXP:
		case KEY_CHAR_PI:
		case KEY_CHAR_DQUATE:
			code=51;
			break;
		case KEY_CHAR_PMINUS:
		case KEY_CHAR_ANS:
		case 30070:	// ALPHA+(-)
			code=41;
			break;
		case KEY_CTRL_EXE:
		case KEY_CHAR_CR:
			code=31;
			break;
		case KEY_CHAR_1:
		case KEY_CHAR_LIST:
		case KEY_CHAR_U:
			code=72;
			break;
		case KEY_CHAR_2:
		case KEY_CHAR_MAT:
		case KEY_CHAR_V:
			code=62;
			break;
		case KEY_CHAR_3:
		case KEY_CHAR_W:
			code=52;
			break;
		case KEY_CHAR_PLUS:
		case KEY_CHAR_LBRCKT:
		case KEY_CHAR_X:
			code=42;
			break;
		case KEY_CHAR_MINUS:
		case KEY_CHAR_RBRCKT:
		case KEY_CHAR_Y:
			code=32;
			break;
		case KEY_CHAR_4:
		case KEY_CTRL_CATALOG:
		case KEY_CHAR_P:
			code=73;
			break;
		case KEY_CHAR_5:
		case KEY_CHAR_Q:
		case 30101:		// SHIFT_FORMAT
			code=63;
			break;
		case KEY_CHAR_6:
		case KEY_CHAR_R:
			code=53;
			break;
		case KEY_CHAR_MULT:
		case KEY_CHAR_LBRACE:
		case KEY_CHAR_S:
			code=43;
			break;
		case KEY_CHAR_DIV:
		case KEY_CHAR_RBRACE:
		case KEY_CHAR_T:
			code=33;
			break;
		case KEY_CHAR_7:
		case KEY_CTRL_CAPTURE:
		case KEY_CHAR_M:
			code=74;
			break;
		case KEY_CHAR_8:
		case KEY_CTRL_CLIP:
		case KEY_CHAR_N:
			code=64;
			break;
		case KEY_CHAR_9:
		case KEY_CTRL_PASTE:
		case KEY_CHAR_O:
			code=54;
			break;
		case KEY_CTRL_DEL:
		case KEY_CTRL_INS:
		case KEY_CTRL_UNDO:
			code=44;
			break;
		case KEY_CTRL_AC:
			code=34;
			break;
		case KEY_CHAR_FRAC:
		case KEY_CTRL_MIXEDFRAC:
		case KEY_CHAR_G:
			code=75;
			break;
		case KEY_CTRL_FD:
		case KEY_CTRL_FRACCNVRT:
		case KEY_CHAR_H:
			code=65;
			break;
		case KEY_CHAR_LPAR:
		case KEY_CHAR_CUBEROOT:
		case KEY_CHAR_I:
			code=55;
			break;
		case KEY_CHAR_RPAR:
		case KEY_CHAR_RECIP:
		case KEY_CHAR_J:
			code=45;
			break;
		case KEY_CHAR_COMMA:
		case KEY_CHAR_K:
			code=35;
			break;
		case KEY_CHAR_STORE:
		case KEY_CHAR_L:
			code=25;
			break;
		case KEY_CTRL_XTT:
		case KEY_CHAR_ANGLE:
		case KEY_CHAR_A:
			code=76;
			break;
		case KEY_CHAR_LOG:
		case KEY_CHAR_EXPN10:
		case KEY_CHAR_B:
			code=66;
			break;
		case KEY_CHAR_LN:
		case KEY_CHAR_EXPN:
		case KEY_CHAR_C:
			code=56;
			break;
		case KEY_CHAR_SIN:
		case KEY_CHAR_ASIN:
		case KEY_CHAR_D:
			code=46;
			break;
		case KEY_CHAR_COS:
		case KEY_CHAR_ACOS:
		case KEY_CHAR_E:
			code=36;
			break;
		case KEY_CHAR_TAN:
		case KEY_CHAR_ATAN:
		case KEY_CHAR_F:
			code=26;
			break;
		case KEY_CTRL_ALPHA:
			code=77;
			break;
		case KEY_CHAR_SQUARE:
		case KEY_CHAR_ROOT:
		case KEY_CHAR_VALR:
			code=67;
			break;
		case KEY_CHAR_POW:
		case KEY_CHAR_POWROOT:
		case KEY_CHAR_THETA:
			code=57;
			break;
		case KEY_CTRL_EXIT:
		case KEY_CTRL_QUIT:
			code=47;
			break;
		case KEY_CTRL_DOWN:
		case KEY_CTRL_PAGEDOWN:
			code=37;
			break;
		case KEY_CTRL_RIGHT:
		case KEY_SHIFT_RIGHT:
			code=27;
			break;
		case KEY_CTRL_SHIFT:
			code=78;
			break;
		case KEY_SHIFT_OPTN:
		case KEY_CTRL_OPTN:
			code=68;
			break;
		case KEY_CTRL_VARS:
		case KEY_CTRL_PRGM:
			code=58;
			break;
		case KEY_CTRL_MENU:
		case KEY_CTRL_SETUP:
			code=48;
			break;
		case KEY_CTRL_LEFT:
		case KEY_SHIFT_LEFT:
			code=38;
			break;
		case KEY_CTRL_UP:
		case KEY_CTRL_PAGEUP:
			code=28;
			break;
		case KEY_CTRL_F1:
			code=79;
			break;
		case KEY_CTRL_F2:
			code=69;
			break;
		case KEY_CTRL_F3:
			code=59;
			break;
		case KEY_CTRL_F4:
			code=49;
			break;
		case KEY_CTRL_F5:
			code=39;
			break;
		case KEY_CTRL_F6:
			code=29;
			break;
		default:
			code=0;
			break;
	}
	return code;
}

int GetKey_DisableMenu( int *key ) {
	int r;
//	DisableGetkeyToMainFunctionReturn(); 
	SetGetkeyToMainFunctionReturnFlag( 0 );	// disabled
	r=GetKey(key);
//	EnableGetkeyToMainFunctionReturn(); 
	SetGetkeyToMainFunctionReturnFlag( 1 );	// enabled
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

int CB_Getkey1(int sdkcode) {			// CasioBasic Getkey SDK compatible
	int key;
	int code;
	int t,th;
	Getkey_shift=0;
	Recent_code=0;
	t =RTC_GetTicks()-CB_TicksStart;				// halt ticks count
	th=(int)GetTicks32768()-CB_HiTicksStart;		// halt ticks count
	GetKey_DisableMenu(&key);
	if ( TimeDsp && 0x02 ) {
		CB_TicksStart=RTC_GetTicks();				// reset ticks count
		CB_HiTicksStart=(int)GetTicks32768();		// reset ticks count
	} else  {
		CB_TicksStart=RTC_GetTicks()-t;				// restart ticks count
		CB_HiTicksStart=(int)GetTicks32768()-th;	// restart ticks count
	}
	if ( sdkcode ) return key;
	return CB_KeyCodeCnvt( key ) ;
}

int CB_Getkey2(int sdkcode) {			// CasioBasic Getkey SDK compatible with buffer clear
	KeyRecover();
	return CB_Getkey1(sdkcode) ;
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
			if ( disableCatalog ) Bkey_SetFlag( 0x80 );		// disable Catalog function
			if ( n==1 ) key=CB_Getkey1(sdkcode);
			else		key=CB_Getkey2(sdkcode);
			if ( disableCatalog ) Bkey_ClrFlag( 0x80 ) ;	// enable Catalog function
			DrawFrame( CB_FrameColor );
			break;
		default:
			return 0;
	}
	return key;
}
//---------------------------------------------------------------------------------------------- align dummy
int keyObjectAlign4a( unsigned int n ){ return n; }	// align +4byte
int ketObjectAlign4b( unsigned int n ){ return n; }	// align +4byte
int keyObjectAlign4c( unsigned int n ){ return n; }	// align +4byte
int keyObjectAlign4d( unsigned int n ){ return n; }	// align +4byte
//int keyObjectAlign4e( unsigned int n ){ return n; }	// align +4byte
//int keyObjectAlign4f( unsigned int n ){ return n; }	// align +4byte

}
