//
// fx-9860G/II series KeyScan
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

#include <fxlib.h>
#include "KeyScan.h"


void delay( void ){
int i;
  for (i=0;i<5;i++){};
}

//
int CheckKeyRow( int row ){
  int result=0;
  short*PORTB_CTRL=(void*)0xA4000102;
  short*PORTM_CTRL=(void*)0xA4000118;
  char*PORTB=(void*)0xA4000122;
  char*PORTM=(void*)0xA4000138;
  char*PORTA=(void*)0xA4000120;
  short smask;
  char cmask;

  smask = 0x0003 << ((row%8)*2);
  cmask = ~( 1 << (row%8) );
  if (row<8){
// configure port B as input, except for the "row to check"-bit, which has to be an output.
        *PORTB_CTRL = 0xAAAA ^ smask;    
// configure port M as input; port M is inactive with row < 8
        *PORTM_CTRL = (*PORTM_CTRL & 0xFF00 ) | 0x00AA;  
        delay();
        *PORTB = cmask;    // set the "row to check"-bit to 0 on port B
        *PORTM = (*PORTM & 0xF0 ) | 0x0F;    // port M is inactive with row < 8
  }else{
        *PORTB_CTRL = 0xAAAA;  // configure port B as input; port B is inactive with row >= 8
// configure port M as input, except for the "row to check"-bit, which has to be an output.
        *PORTM_CTRL = ((*PORTM_CTRL & 0xFF00 ) | 0x00AA)  ^ smask;  
        delay();
        *PORTB = 0xFF;    // port B is inactive with row >= 8 (all to 1)
        *PORTM = (*PORTM & 0xF0 ) | cmask;  // set the "row to check"-bit to 0
  };
  delay();
  result = ~(*PORTA);   // a pressed key in the row-to-check draws the corresponding bit to 0
  delay();
  *PORTB_CTRL = 0xAAAA;  
  *PORTM_CTRL = (*PORTM_CTRL & 0xFF00 ) | 0x00AA;
  delay();
  *PORTB_CTRL = 0x5555;
  *PORTM_CTRL = (*PORTM_CTRL & 0xFF00 ) | 0x0055;
  delay();

  return result;
}


int CheckKeyRow7305( int row ){
	short*KEYPORT=(void*)0xA44B0000;
	short result=KEYPORT[row>>1];
	if ( row & 1 ) result/=0x100;
	return result & 0xFF ;
}


int KeyScanDown(int keyscan_code){
	int row,col,rowdata;
	row = keyscan_code & 0x0F;
	col = keyscan_code >> 4;

	if (*(volatile unsigned short*)0xFFFFFF80 != 0) {
		return ( CheckKeyRow(row) & col ) ;			//SH3
	}
	else {
		return ( CheckKeyRow7305(row) & col ) ;		//SH4A
	}
}

//----------------------------------------------------------------------------------------------
char Getkey_shift=0;

int BackLight( int n ){		// 0:off  1:on   2:xor
	volatile unsigned char *adrs;
	unsigned char bit;
	int result;
	
	if (*(volatile unsigned short*)0xFFFFFF80 != 0) {
		adrs=(volatile unsigned char *)0xA400012C; bit=0x80;		// SH3
	} else {
		adrs=(volatile unsigned char *)0xA4050138; bit=0x10;		// SH4A
	}
	switch ( n ) {
		case 0:
			*adrs &= (~bit);	// off
			break;
		case 1:
			*adrs |= ( bit);	// on
			break;
		case 2:
			*adrs ^= ( bit);	// reverse
			break;
		default:
			break;
	}
	if ( (*adrs) && (~bit) ) return 1; else return 0;
}

int CB_Getkey() {			// CasioBasic Getkey compatible
	unsigned int key;
	int i,row,c,SH3;
	int code=0;
	row=1;
	SH3= (*(volatile unsigned short*)0xFFFFFF80 != 0) ;
	
	for ( row=1; row<10; row++) {
		if (SH3) c=CheckKeyRow(row);  else  c=CheckKeyRow7305(row);
		if ( c & 0x40 ) { code=70+row; break; }	//
		if ( c & 0x20 ) { code=60+row; break; }	//
		if ( c & 0x10 ) { code=50+row; break; }	//
		if ( c & 0x08 ) { code=40+row; break; }	//
		if ( c & 0x04 ) { code=30+row; break; }	//
		if ( c & 0x02 ) { code=20+row; break; }	//
	}
	if ( KeyScanDown(KEYSC_AC) ) code=34;
	
//	if (SH3) {		//SH3
//		KeyCheckAC();
//		IsKeyDown( KEY_CTRL_AC );
//	}
//	Keyboard_ClrBuffer();

	if ( ( code ) && ( Getkey_shift ) ) {
		Getkey_shift=0;
		if ( code == 68 ) {	// 68
			Keyboard_ClrBuffer();
			if ( SH3 == 0 ) BackLight(2);	// SH4 only
		}
//		if ( code == 34 ) {	// AC
//			PutKey( KEY_CTRL_SHIFT,1);	GetKey(&key);
//			PutKey( KEY_CTRL_AC,1);		GetKey(&key);
//			code=0;
//		}
	}
	if ( code == 78 ) // shift
		Getkey_shift=1;
	return code;
}


int KeyCheckAC() {		// [AC]
	int kcode1 = 0, kcode2 = 0, flag0 = 0;
	short unused = 0;

	if ( Bkey_GetKeyWait(&kcode1,&kcode2,KEYWAIT_HALTOFF_TIMEROFF,0,1,&unused ) == KEYREP_KEYEVENT ) {
		if ( ( kcode1 == 1 ) && (kcode2 == 1 ) ) flag0=1; // [AC] is down
	}
	return flag0;
}
int KeyCheckEXE() {		// [EXE]
	int kcode1 = 0, kcode2 = 0, flag0 = 0;
	short unused = 0;

	if ( Bkey_GetKeyWait(&kcode1,&kcode2,KEYWAIT_HALTOFF_TIMEROFF,0,1,&unused ) == KEYREP_KEYEVENT ) {
		if ( ( kcode1 == 3 ) && (kcode2 == 2 ) ) flag0=1; // [EXE] is down
	}
	return flag0;
}
int KeyCheckEXIT() {		// [EXIT]
	int kcode1 = 0, kcode2 = 0, flag0 = 0;
	short unused = 0;

	if ( Bkey_GetKeyWait(&kcode1,&kcode2,KEYWAIT_HALTOFF_TIMEROFF,0,1,&unused ) == KEYREP_KEYEVENT ) {
		if ( ( kcode1 == 4 ) && (kcode2 == 8 ) ) flag0=1; // [EXE] is down
	}
	return flag0;
}
int KeyCheckSHIFT() {		// [SHIFT]
	int kcode1 = 0, kcode2 = 0, flag0 = 0;
	short unused = 0;

	if ( Bkey_GetKeyWait(&kcode1,&kcode2,KEYWAIT_HALTOFF_TIMEROFF,0,1,&unused ) == KEYREP_KEYEVENT ) {
		if ( ( kcode1 == 7 ) && (kcode2 == 9 ) ) flag0=1; // [SHIFT] is down
	}
	return flag0;
}
int KeyCheckCHAR4() {		// [4]
	int kcode1 = 0, kcode2 = 0, flag0 = 0;
	short unused = 0;

	if ( Bkey_GetKeyWait(&kcode1,&kcode2,KEYWAIT_HALTOFF_TIMEROFF,0,1,&unused ) == KEYREP_KEYEVENT ) {
		if ( ( kcode1 == 7 ) && (kcode2 == 4 ) ) flag0=1; // [SHIFT] is down
	}
	return flag0;
}

void KeyRecover() {
//	CB_Getkey();
	if (*(volatile unsigned short*)0xFFFFFF80 != 0) 
		IsKeyDown( KEY_CTRL_AC );		//SH3
	else	KeyCheckAC();				//SH4
//	KeyCheckAC();
//	KeyCheckEXE();
//	KeyCheckEXIT();
	KeyCheckSHIFT();
	KeyCheckCHAR4();
	Keyboard_ClrBuffer();
	Getkey_shift=0;
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
			code=27;
			break;
		case KEY_CTRL_SHIFT:
			code=78;
			break;
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

int CB_Getkey1() {			// CasioBasic Getkey SDK compatible
	unsigned int key;
	int code;
	
	Getkey_shift=0;
	GetKey(&key);
	return CB_KeyCodeCnvt( key ) ;
}

int CB_Getkey2() {			// CasioBasic Getkey SDK compatible with buffer clear
	KeyRecover();
	return CB_Getkey1() ;
}


int CB_GetkeyN( int n) {			// CasioBasic Getkey 
	switch ( n ) {
		case 0:
			KeyRecover();
			return 0;
		case 1:
			return CB_Getkey1();
		case 2:
			return CB_Getkey2();
	}
}

