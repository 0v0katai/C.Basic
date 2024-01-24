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

#include "CB.h"

void delay( int wait ){
int i;
  for (i=0;i<wait;i++){};
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
  char PORTBtmp = *PORTB;
  char PORTMtmp = *PORTM;
  int  wait=0+(IsSH3==2)*10;	// 9860G or Slim

  smask = 0x0003 << ((row%8)*2);
  cmask = ~( 1 << (row%8) );
  if (row<8){
// configure port B as input, except for the "row to check"-bit, which has to be an output.
        *PORTB_CTRL = 0xAAAA ^ smask;    
// configure port M as input; port M is inactive with row < 8
        *PORTM_CTRL = (*PORTM_CTRL & 0xFF00 ) | 0x00AA;  
        delay(wait);
        *PORTB = cmask;    // set the "row to check"-bit to 0 on port B
        *PORTM = (*PORTM & 0xF0 ) | 0x0F;    // port M is inactive with row < 8
  }else{
        *PORTB_CTRL = 0xAAAA;  // configure port B as input; port B is inactive with row >= 8
// configure port M as input, except for the "row to check"-bit, which has to be an output.
        *PORTM_CTRL = ((*PORTM_CTRL & 0xFF00 ) | 0x00AA)  ^ smask;  
        delay(wait);
        *PORTB = 0xFF;    // port B is inactive with row >= 8 (all to 1)
        *PORTM = (*PORTM & 0xF0 ) | cmask;  // set the "row to check"-bit to 0
  };
  delay(wait);
  result = ~(*PORTA);   // a pressed key in the row-to-check draws the corresponding bit to 0
  delay(wait);
//  *PORTB_CTRL = 0xAAAA;  
//  *PORTM_CTRL = (*PORTM_CTRL & 0xFF00 ) | 0x00AA;
//  delay(wait);
  *PORTB_CTRL = 0x5555;
  *PORTM_CTRL = (*PORTM_CTRL & 0xFF00 ) | 0x0055;
  delay(wait);
  *PORTB = PORTBtmp;
  *PORTM = PORTMtmp;
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

	if ( IsSH3 ) {
		return ( CheckKeyRow(row) & col ) ;			//SH3
	}
	else {
		return ( CheckKeyRow7305(row) & col ) ;		//SH4A
	}
}
int KeyScanDownAC(){
	int result=0;
	int n,s,t;
	n=Waitcount;
	if ( n<=0 ) return KeyScanDown(KEYSC_AC) ;
	if ( n>1  ) n*=BREAKCOUNT;
	if ( IsSH3==0 ) n*=5;	// SH4 adjust
	while ( n ) {
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

int BackLight( int n ){		// 0:off  1:on   2:xor
	volatile unsigned char *adrs;
	unsigned char bit;
	int result;

	switch ( IsSH3 ) {
		case 0:
			adrs=(volatile unsigned char *)0xA4050138; bit=0x10;		// SH4A
			break;
		case 1:
			adrs=(volatile unsigned char *)0xA400012C; bit=0x80;		// SH3
			break;
		case 2:
			adrs=(volatile unsigned char *)0xA4000126; bit=0x20;		// SH3 slim
			break;
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

int KeyConvert2Slim( int code ) {
	int row=code%10;
	int col=code/10;
	const unsigned char keyrow_slim_table[9][6]={
		{KEYS_MENU,	KEYS_XTT,	KEYS_SHIFT,	KEYS_ALPHA,	0,			0			},
		{KEYS_F1,	KEYS_LOG,	KEYS_SQUARE,KEYS_POW,	KEYS_LEFT,	KEYS_UP	 	},
		{KEYS_F2,	KEYS_LN,	KEYS_COMMA,	KEYS_OPTN,	KEYS_DOWN,	KEYS_RIGHT	},
		{KEYS_F3,	KEYS_SIN,	KEYS_STORE,	KEYS_VARS,	11,			21			},
		{KEYS_F4,	KEYS_COS,	KEYS_7,		KEYS_4,		KEYS_1,		KEYS_0		},
		{KEYS_F5,	KEYS_TAN,	KEYS_8,		KEYS_5,		KEYS_2,		KEYS_DP	 	},
		{KEYS_F6,	KEYS_FRAC,	KEYS_9,		KEYS_6,		KEYS_3,		KEYS_EXP	},
		{KEYS_EXIT,	KEYS_FD,	KEYS_DEL,	KEYS_MULT,	KEYS_PLUS,	KEYS_PMINUS },
		{0,			KEYS_LPAR,	KEYS_RPAR,	KEYS_DIV,	KEYS_EXE,	KEYS_MINUS  }
	};
	return keyrow_slim_table[9-row][7-col];
}


int CB_Getkey() {			// CasioBasic Getkey compatible
	unsigned int key;
	int i,row,c,SH3;
	int code=0;
	row=1;
	SH3= IsSH3 ;

	
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
	
	if ( (IsSH3==2)&&(code!=0) ) {	// slim
		code = KeyConvert2Slim( code );
	}
	
	Recent_rowcode=(c<<4)+row;
	Recent_code   =code;
	
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

int Bkey_GetKeyWait_sub( int kcode1, int kcode2 ) {		//
	int flag0 = 0;
	short unused = 0;
	if ( Bkey_GetKeyWait(&kcode1,&kcode2,KEYWAIT_HALTOFF_TIMEROFF,0,1,&unused ) == KEYREP_KEYEVENT ) {
		if ( ( kcode1 == 1 ) && (kcode2 == 1 ) ) flag0=1; // [AC] is down
	}
	return flag0;
}

int KeyCheckAC() {		// [AC]
	if (IsSH3 ) return IsKeyDown(KEY_CTRL_AC);
	return Bkey_GetKeyWait_sub( 1, 1 ); // [AC] is down
}
int KeyCheckEXE() {		// [EXE]
//	if (IsSH3 ) return IsKeyDown(KEY_CTRL_EXE);
	return Bkey_GetKeyWait_sub( 3, 2 ); // [EXE] is down
}
int KeyCheckEXIT() {		// [EXIT]
//	if (IsSH3 ) return IsKeyDown(KEY_CTRL_EXIT);
	return Bkey_GetKeyWait_sub( 4, 8 ); // [EXIT] is down
}
int KeyCheckSHIFT() {		// [SHIFT]
	if (IsSH3 ) return IsKeyDown(KEY_CTRL_SHIFT);
//	if ( IsSH3==2 ) return Bkey_GetKeyWait_sub( 5, 10 ); // [SHIFT] is down by slim
	return Bkey_GetKeyWait_sub( 7, 9 ); // [SHIFT] is down
}
int KeyCheckCHAR4() {		// [4]
	if (IsSH3 ) return IsKeyDown(KEY_CHAR_4);
//	if ( IsSH3==2 ) return Bkey_GetKeyWait_sub( 4, 6 ); // [4] is down by slim
	return Bkey_GetKeyWait_sub( 7, 4 ); // [4] is down
}
int KeyCheckCHAR3() {		// [3]
	if (IsSH3 ) return IsKeyDown(KEY_CHAR_3);
//	if ( IsSH3==2 ) return Bkey_GetKeyWait_sub( 3, 4 ); // [3] is down by slim
//	return Bkey_GetKeyWait_sub( 5, 3 ); // [3] is down
	return KeyScanDown(KEYSC_3); // [3] is down
}
int KeyCheckCHAR6() {		// [6]
	if (IsSH3 ) return IsKeyDown(KEY_CHAR_6);
//	if ( IsSH3==2 ) return Bkey_GetKeyWait_sub( 4, 4 ); // [6] is down by slim
	return Bkey_GetKeyWait_sub( 5, 4 ); // [6] is down
}
int KeyCheckF1() {		// [F1]
	if (IsSH3 ) return IsKeyDown(KEY_CTRL_F1);
//	if ( IsSH3==2 ) return Bkey_GetKeyWait_sub( 7, 9 ); // [F1] is down by slim
	return Bkey_GetKeyWait_sub( 7, 10 ); // [F1] is down
}
int KeyCheckDEL() {		// [DEL]
	if (IsSH3 ) return IsKeyDown(KEY_CTRL_DEL);
//	if ( IsSH3==2 ) return Bkey_GetKeyWait_sub( 7, 9 ); // [F1] is down by slim
	return Bkey_GetKeyWait_sub( 4, 5 ); // [DEL] is down
}

void KeyRecover() {
//	CB_Getkey();
	if ( IsSH3 ) 
		IsKeyDown( KEY_CTRL_AC );		//SH3
	else	KeyCheckAC();				//SH4
//	KeyCheckAC();
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
		case 30045:		// KEY_CTRL_UNDO:
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

int GetKey_DisableMenu( unsigned int *key ) {
	int r;
	DisableGetkeyToMainFunctionReturn(); 
	r=GetKey(key);
	EnableGetkeyToMainFunctionReturn(); 
	return r;
}

int CB_Getkey1(int sdkcode) {			// CasioBasic Getkey SDK compatible
	unsigned int key;
	int code;
	int t,th;
	Getkey_shift=0;
	Recent_code=0;
	t=RTC_GetTicks()-CB_TicksStart;					// halt ticks count
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
	switch ( n ) {
		case 0:
			KeyRecover();
			return 0;
		case 1:
			return CB_Getkey1(sdkcode);
		case 2:
			return CB_Getkey2(sdkcode);
		default:
			return 0;
	}
}

//----------------------------------------------------------------------------------------------
int kObjectAlign4a( unsigned int n ){ return n; }	// align +4byte
//int kObjectAlign4b( unsigned int n ){ return n; }	// align +4byte
int kObjectAlign4c( unsigned int n ){ return n; }	// align +4byte
int kObjectAlign4d( unsigned int n ){ return n; }	// align +4byte
//int kObjectAlign4e( unsigned int n ){ return n; }	// align +4byte
//int kObjectAlign4f( unsigned int n ){ return n; }	// align +4byte
//int kObjectAlign4g( unsigned int n ){ return n; }	// align +4byte
//int kObjectAlign4h( unsigned int n ){ return n; }	// align +4byte
//int kObjectAlign4i( unsigned int n ){ return n; }	// align +4byte
//int kObjectAlign4j( unsigned int n ){ return n; }	// align +4byte
//int kObjectAlign4k( unsigned int n ){ return n; }	// align +4byte
//int kObjectAlign4l( unsigned int n ){ return n; }	// align +4byte
//int kObjectAlign4m( unsigned int n ){ return n; }	// align +4byte
//int kObjectAlign4n( unsigned int n ){ return n; }	// align +4byte
//int kObjectAlign4o( unsigned int n ){ return n; }	// align +4byte
//int kObjectAlign4p( unsigned int n ){ return n; }	// align +4byte
//int kObjectAlign4q( unsigned int n ){ return n; }	// align +4byte
//int kObjectAlign4r( unsigned int n ){ return n; }	// align +4byte
//int kObjectAlign4s( unsigned int n ){ return n; }	// align +4byte
//int kObjectAlign4t( unsigned int n ){ return n; }	// align +4byte
//int kObjectAlign4u( unsigned int n ){ return n; }	// align +4byte
//int kObjectAlign4v( unsigned int n ){ return n; }	// align +4byte
//int kObjectAlign4w( unsigned int n ){ return n; }	// align +4byte
//int kObjectAlign4x( unsigned int n ){ return n; }	// align +4byte
//----------------------------------------------------------------------------------------------

