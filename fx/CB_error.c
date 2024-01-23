#include <ctype.h>
#include <fxlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "KeyScan.h"
#include "CB_lib.h"
#include "CB_io.h"

//-----------------------------------------------------------------------------
// Casio Basic inside
//-----------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------
int ErrorPtr=0;		// Error ptr
int ErrorNo;		// Error No
//----------------------------------------------------------------------------------------------

void CB_ERROR(char *buffer) {
	unsigned int key;
	char buf[22];

	CB_SelectTextDD();	// Select Text Screen
	SaveDisp(SAVEDISP_PAGE1);
	PopUpWin(5);
	locate(3,2); Print((unsigned char *)buffer);
	sprintf(buf," ptr:0x%X(%d)",ErrorPtr,ErrorPtr);
	locate(3,4); Print((unsigned char *)buf);
	locate(3,6); Print((unsigned char *) "   Press:[EXIT]");
	Bdisp_PutDisp_DD();

	while(KeyCheckAC());
	KeyRecover(); 
	while ( 1 ) {
		GetKey(&key);
		if ( key == KEY_CTRL_EXIT  ) break ;
		if ( key == KEY_CTRL_AC    ) break ;
		if ( key == KEY_CTRL_RIGHT ) break ;
		if ( key == KEY_CTRL_LEFT  ) break ;
	}
	
	RestoreDisp(SAVEDISP_PAGE1);
	Bdisp_PutDisp_DD();
}

void CB_ErrNo(int ErrorNo) {
	switch (ErrorNo) {
		case 1:
			CB_ERROR("Syntax ERROR");
			break;
		case 2:
			CB_ERROR("Math ERROR");
			break;
		case 3:
			CB_ERROR("Go ERROR");
			break;
		case 4:
			CB_ERROR("Nesting ERROR");
			break;
		case 5:
			CB_ERROR("Stack ERROR");
			break;
		case 6:
			CB_ERROR("Memory ERROR");
			break;
		case 7:
			CB_ERROR("Argument ERROR");
			break;
		case 8:
			CB_ERROR("Dimension ERROR");
			break;
		case 9:
			CB_ERROR("Range ERROR");
			break;
		default:
			break;
	}
}

//----------------------------------------------------------------------------------------------

void CB_BreakStop() {
	unsigned int key=1;
	char buf[22];

	CB_SelectTextDD();	// Select Text Screen
	SaveDisp(SAVEDISP_PAGE1);
	PopUpWin(4);
	locate(9,3); Print((unsigned char *)"Break");
	locate(6,5); Print((unsigned char *) "Press:[EXIT]");
	Bdisp_PutDisp_DD();
	
	KeyRecover(); 
	while ( KeyCheckAC() ) ;
	while ( 1 ) {
		GetKey(&key);
		if ( key == KEY_CTRL_EXIT  ) break ;
		if ( key == KEY_CTRL_AC    ) break ;
		if ( key == KEY_CTRL_RIGHT ) break ;
		if ( key == KEY_CTRL_LEFT  ) break ;
	}
	RestoreDisp(SAVEDISP_PAGE1);
	Bdisp_PutDisp_DD();
}

