#include <ctype.h>
#include <fxlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "KeyScan.h"
#include "CB_io.h"
#include "CB_error.h"
#include "CB_interpreter.h"

//-----------------------------------------------------------------------------
// Casio Basic inside
//-----------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------
int ErrorPtr=0;		// Error ptr
int ErrorNo;		// Error No
//----------------------------------------------------------------------------------------------

void ERROR(char *buffer) {
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

void CB_ErrMsg(int ErrNo) {
	switch (ErrNo) {
		case SyntaxERR:
			ERROR("Syntax ERROR");
			break;
		case MathERR:
			ERROR("Math ERROR");
			break;
		case GoERR:
			ERROR("Go ERROR");
			break;
		case NestingERR:
			ERROR("Nesting ERROR");
			break;
		case StackERR:
			ERROR("Stack ERROR");
			break;
		case MemoryERR:
			ERROR("Memory ERROR");
			break;
		case ArgumentERR:
			ERROR("Argument ERROR");
			break;
		case DimensionERR:
			ERROR("Dimension ERROR");
			break;
		case RangeERR:
			ERROR("Range ERROR");
			break;
		case NextWithoutForERR:
			ERROR("Next without For");
			break;
		case ForWithoutNextERR:
			ERROR("For without Next");
			break;
		case WhileWithoutWhileEndERR:
			ERROR("While withot WEnd");
			break;
		case WhileEndWithoutWhileERR:
			ERROR("WEnd withot While");
			break;
		case LpWhileWithoutDoERR:
			ERROR("LpWhile withot Do");
			break;
		case DoWithoutLpWhileERR:
			ERROR("Do withot LpWhile");
			break;
		case NotLoopERR:
			ERROR("Not Loop ERROR");
			break;
		case DivisionByZeroERR:
			ERROR("Division By Zero");
			break;
		case UndefinedLabelERR:
			ERROR("Undefined Label");
			break;
		case NotEnoughMemoryERR:
			ERROR("Not enough Memory");
			break;
		case StringTooLongERR:
			ERROR("String Too Long");
			break;
		case NoMatrixArrayERR:
			ERROR("No Matrix array");
			break;
		case ArraySizeERR:
			ERROR("Illegal Ary size");
			break;
		default:
			break;
	}
}

//-----------------------------------------------------------------------------
void CB_Error(int ErrNo) { // error
	if ( ErrorNo ) return ;
	ErrorNo=ErrNo;
	ErrorPtr=ExecPtr;
	if ( ErrNo==MathERR ) ErrorPtr--;
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

