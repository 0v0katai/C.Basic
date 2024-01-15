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
		case SyntaxERR:					//  1
			ERROR(" Syntax ERROR");
			break;
		case MathERR:					//  2
			ERROR(" Ma ERROR");
			break;
		case GoERR:						//  3
			ERROR(" Go ERROR");
			break;
		case NestingERR:				//  4
			ERROR(" Nesting ERROR");
			break;
		case StackERR:					//  5
			ERROR(" Stack ERROR");
			break;
		case MemoryERR:					//  6
			ERROR(" Memory ERROR");
			break;
		case ArgumentERR:				//  7
			ERROR(" Argument ERROR");
			break;
		case DimensionERR:				//  8
			ERROR(" Dimension ERROR");
			break;
		case RangeERR:					//  9
			ERROR(" Range ERROR");
			break;
		case NonRealERR:				// 11
			ERROR(" Non-Real ERROR");
			break;
		case ComERR:					// 20
			ERROR(" Com ERROR");
			break;
		case TransmitERR:				// 21
			ERROR(" Transmit ERROR");
			break;
		case ReceiveERR:				// 22
			ERROR(" Receive ERROR");
			break;
		case TooMuchData:				// 28
			ERROR(" Too Much Data");
			break;
		case InvalidType:				// 30
			ERROR(" Invalid Type");
			break;

		case NextWithoutForERR:			// 33
			ERROR("Missing Next");
			break;
		case ForWithoutNextERR:			// 34
			ERROR("Missing For");
			break;
		case WhileWithoutWhileEndERR:	// 35
			ERROR("Missing WhileEnd");
			break;
		case WhileEndWithoutWhileERR:	// 36
			ERROR("Missing While");
			break;
		case LpWhileWithoutDoERR:		// 37
			ERROR("Missing LpWhile");
			break;
		case DoWithoutLpWhileERR:		// 38
			ERROR("Missing Do");
			break;
		case NotLoopERR:				// 39
			ERROR("Not Loop ERROR");
			break;
		case DivisionByZeroERR:			// 40
			ERROR("Zero Division");
			break;
		case UndefinedLabelERR:			// 41
			ERROR("Undefined Label");
			break;
		case NotEnoughMemoryERR:		// 42
			ERROR("Not Enough Memory");
			break;
		case StringTooLongERR:			// 43
			ERROR("String Too Long");
			break;
		case NoMatrixArrayERR:			// 44
			ERROR("No Matrix Array");
			break;
		case ArraySizeERR:				// 45
			ERROR("Invalid Ary Size");
			break;
		case NotfoundProgERR:			// 46
			ERROR("Prog Not Found");
			break;
		case TooManyProgERR:			// 47
			ERROR("Too Many SubProg");
			break;
		case IfWithoutIfEndERR:			// 48
			ERROR("Missing IfEnd");
			break;
		case ThenWithoutIfERR:			// 49
			ERROR("Missing If");
			break;
		case CaseWithoutSwitchERR:		// 50
			ERROR("Missing Switch ");
			break;
		case DefaultWithoutSwitchERR:	// 51
			ERROR("Missing Switch ");
			break;
		case SwitchWithoutSwitchEndERR:	// 52
			ERROR("Missing SwitchEnd");
			break;
		case SwitchEndWithoutSwitchERR:	// 53
			ERROR("Missing Switch ");
			break;
		case CantFindFileERR:			// 54
			ERROR("File Not Found");
			break;
		case ElementSizeERR:			// 55
			ERROR("Illegal Element");
			break;
		case AlreadyOpenERR:			// 56
			ERROR("Com Opened");
			break;
		case ComNotOpenERR:				// 57
			ERROR("Com Closed");
			break;
		case TypeMismatchERR:			// 58
			ERROR("Invalid Type");
			break;
		case OutOfDomainERR:			// 59
			ERROR("Out of Domain ");
			break;
		case UndefinedVarERR:			// 60
			ERROR("Var Undefined");
			break;
		case UndefinedFuncERR:			// 61
			ERROR("Func Undefined");
			break;
		case NotSupportERR:				// 62
			ERROR("Not Supported");
			break;
		case TooManyVarERR:				// 63
			ERROR("VarLimit Exceeded");
			break;
		case DuplicateDefERR:			// 64
			ERROR("Duplicated Def");
			break;
		case AlignmentERR:				// 65
			ERROR("Address Align ERR");
			break;
		case NotAccuracyERR:			// 66
			ERROR("Time Out");
			break;
		case TryWithoutExceptERR:		// 70
			ERROR("Missing Except");
			break;
		case ExcpetWithoutTryERR:		// 71
			ERROR("Missing Try");
			break;
//		case TryEndWithoutTryERR:		// 72
//			ERROR("Missing Try");
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


