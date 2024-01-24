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
int ErrorProg=0;	// Error Prog
int ErrorNo;		// Error No
//----------------------------------------------------------------------------------------------

void ERROR(char *buffer) {
	unsigned int key;
	char buf[22];

	CB_SelectTextDD();	// Select Text Screen
	SaveDisp(SAVEDISP_PAGE1);

//	PopUpWin(5);
//	locate(3,2); Print((unsigned char *)buffer);
//	sprintf(buf," ptr:0x%X(%d)",ErrorPtr,ErrorPtr);
//	locate(3,4); Print((unsigned char *)buf);
//	locate(3,6); Print((unsigned char *) "   Press:[EXIT]");

	PopUpWin(4);
	locate(3,3); Print((unsigned char *)buffer);
	locate(3,5); Print((unsigned char *) "   Press:[EXIT]");

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
			ERROR(" Syntax ERROR");
			break;
		case MathERR:
			ERROR(" Math ERROR");
			break;
		case GoERR:
			ERROR(" Go ERROR");
			break;
		case NestingERR:
			ERROR(" Nesting ERROR");
			break;
		case StackERR:
			ERROR(" Stack ERROR");
			break;
		case MemoryERR:
			ERROR(" Memory ERROR");
			break;
		case ArgumentERR:
			ERROR(" Argument ERROR");
			break;
		case DimensionERR:
			ERROR(" Dimension ERROR");
			break;
		case RangeERR:
			ERROR(" Range ERROR");
			break;
		case TooMuchData:
			ERROR(" Too Much Data");
			break;
		case ComERR:
			ERROR(" Com ERROR");
			break;
		case TransmitERR:
			ERROR(" Transmit ERROR");
			break;
		case ReceiveERR:
			ERROR(" Receive ERROR");
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
		case NotfoundProgERR:
			ERROR("Not found Prog");
			break;
		case TooManyProgERR:
			ERROR("Too many SubProg");
			break;
		case IfWithoutIfEndERR:
			ERROR("If without IfEnd");
			break;
		case ThenWithoutIfERR:
			ERROR("Then without If");
			break;
		case CaseWithoutSwitchERR:
			ERROR("Case w/out Switch");
			break;
		case DefaultWithoutSwitchERR:
			ERROR("Default wo Switch");
			break;
		case SwitchWithoutSwitchEndERR:
			ERROR("Without SwitchEnd");
			break;
		case SwitchEndWithoutSwitchERR:
			ERROR("Without Switch");
			break;
		case CantFindFileERR:
			ERROR("Can't find file");
			break;
		case ElementSizeERR:
			ERROR("Illegal Element");
			break;
		case AlreadyOpenERR:
			ERROR("Already Open ERR");
			break;
		case ComNotOpenERR:
			ERROR("Com Not Open ERR");
			break;
		case TypeMismatchERR:
			ERROR("Type Mismatch ERR");
			break;
		case OutOfDomainERR:
			ERROR("Out of Domain ERR");
			break;
		case UndefinedVarERR:
			ERROR("Undefined Var");
			break;
		case UndefinedFuncERR:
			ERROR("Undefined Func");
			break;
		case NotSupportERR:
			ERROR("Not Support ERR");
			break;
		case TooManyVarERR:
			ERROR("Too many Var ERR");
			break;
		case DuplicateDefERR:
			ERROR("Duplicate Def ERR");
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
	ErrorProg=ProgNo;
	if ( ErrNo==MathERR ) ErrorPtr--;
}


