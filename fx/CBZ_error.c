#include "CB.h"

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

	if ( TryFlag ) return ;
	
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

	WaitKeyAC();
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
			ERROR(" Ma ERROR");
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
		case NonRealERR:
			ERROR(" Non-Real ERROR");
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
		case InvalidType:
			ERROR(" Invalid Type");
			break;

		case NextWithoutForERR:
			ERROR("Next without For");
			break;
		case ForWithoutNextERR:
			ERROR("For without Next");
			break;
		case WhileWithoutWhileEndERR:
			ERROR("While w/o W.End");
			break;
		case WhileEndWithoutWhileERR:
			ERROR("W.End w/o While");
			break;
		case LpWhileWithoutDoERR:
			ERROR("LpWhile w/o Do");
			break;
		case DoWithoutLpWhileERR:
			ERROR("Do w/o LpWhile");
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
			ERROR("Case w/o Switch  ");
			break;
		case DefaultWithoutSwitchERR:
			ERROR("Default w/o Swtch");
			break;
		case SwitchWithoutSwitchEndERR:
			ERROR("Switch w/o S.End ");
			break;
		case SwitchEndWithoutSwitchERR:
			ERROR("S.End w/o Switch ");
			break;
		case CantFindFileERR:
			ERROR("Can't find file");
			break;
		case ElementSizeERR:
			ERROR("Illegal Element");
			break;
		case AlreadyOpenERR:
			ERROR("Already Opened");
			break;
		case ComNotOpenERR:
			ERROR("Com Not Opened");
			break;
		case TypeMismatchERR:
			ERROR("Type Mismatched");
			break;
		case OutOfDomainERR:
			ERROR("Out of Domain ");
			break;
		case UndefinedVarERR:
			ERROR("Undefined Var.");
			break;
		case UndefinedFuncERR:
			ERROR("Undefined Func.");
			break;
		case NotSupportERR:
			ERROR("Not Supported");
			break;
		case TooManyVarERR:
			ERROR("Exceed Var. limit");
			break;
		case DuplicateDefERR:
			ERROR("Duplicated Def.");
			break;
		case AlignmentERR:
//			ERROR("Alignment ERR");
			ERROR("Address Align ERR");
			break;
		case NotAccuracyERR:
			ERROR("Not met Accuracy");
			break;
		case TryWithoutExceptERR:
			ERROR("Try w/o Except  ");
			break;
		case ExcpetWithoutTryERR:
			ERROR("Except w/o Try  ");
			break;
//		case TryEndWithoutTryERR:
//			ERROR("TryEnd w/o Try ");
//			break;
			
		default:
//		case FileERR:
			ERROR("File ERROR");
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


