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

int CB_Getkey() {			// CasioBasic Getkey compatible
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
	
	if (SH3) IsKeyDown( code );		//SH3
	
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

void KeyRecover() {
//	CB_Getkey();
	if (*(volatile unsigned short*)0xFFFFFF80 != 0) 
		IsKeyDown( KEY_CTRL_AC );		//SH3
	else	KeyCheckAC();				//SH4
//	while(KeyCheckEXIT());
	KeyCheckSHIFT();
	Keyboard_ClrBuffer();
//	Sleep(10);
}

