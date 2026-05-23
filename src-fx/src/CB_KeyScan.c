#include "CB.h"

//----------------------------------------------------------------------------------------------
char Getkey_shift=0;
uint8_t Recent_code = 0;

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

void KeyRecover() {
//	CB_Getkey();
	keydown(KEY_AC);
//	KeyCheckAC();
//	KeyCheckEXE();
//	KeyCheckEXIT();
	keydown(KEY_SHIFT);
	keydown(KEY_4);
	Keyboard_ClrBuffer();
	Getkey_shift = 0;
	Recent_code = 0;
//	Sleep(10);
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
	if ( TimeDsp & 0x02 ) {
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
//----------------------------------------------------------------------------------------------

