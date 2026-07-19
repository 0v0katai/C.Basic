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

int GetKey_DisableMenu( unsigned int *key ) {
	int r;
	DisableGetkeyToMainFunctionReturn(); 
	r=GetKey(key);
	EnableGetkeyToMainFunctionReturn(); 
	return r;
}

int CB_Getkey1() {			// CasioBasic Getkey SDK compatible
	int t,th;
	Getkey_shift=0;
	Recent_code=0;
	t=RTC_GetTicks()-CB_TicksStart;					// halt ticks count
	th=(int)GetTicks32768()-CB_HiTicksStart;		// halt ticks count
	int kcode1, kcode2;
	Bdisp_PutDisp_DD();
	Bkey_GetKeyWait(&kcode1, &kcode2,
		KEYWAIT_HALTON_TIMEROFF,
		0, 0, &(short){0});
	if ( TimeDsp & 0x02 ) {
		CB_TicksStart=RTC_GetTicks();				// reset ticks count
		CB_HiTicksStart=(int)GetTicks32768();		// reset ticks count
	} else  {
		CB_TicksStart=RTC_GetTicks()-t;				// restart ticks count
		CB_HiTicksStart=(int)GetTicks32768()-th;	// restart ticks count
	}
	return getkey_value(kcode1, kcode2 - 1);
}

int CB_Getkey2() {			// CasioBasic Getkey SDK compatible with buffer clear
	KeyRecover();
	return CB_Getkey1() ;
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

