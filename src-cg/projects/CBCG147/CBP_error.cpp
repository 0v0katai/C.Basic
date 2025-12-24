extern "C" {

#include "prizm.h"
#include "CBP.h"

//-----------------------------------------------------------------------------
// Casio Basic inside
//-----------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------
int ErrorPtr=0;		// Error ptr
int ErrorProg=0;	// Error Prog
int ErrorNo;		// Error No
//----------------------------------------------------------------------------------------------

void ERROR(char *buffer) {
	int key;
	int color=CB_ColorIndex;	// current color index 
	unsigned short bcolor=CB_BackColorIndex;	// current color index 
//	short buf[ 330*(179-30+1) ];
//	short *vram=(short *)PictAry[0];
//	int x,y,i,p;
//	i=0;
//	for( y=30; y<179; y++) {
//		p=26+384*y; for ( x=26; x<355; x++) buf[i++]=vram[p++];
//	}
	if ( TryFlag ) return ;

//	CB_SelectTextDD();	// Select Text Screen
//	SaveDisp(SAVEDISP_PAGE1);
	CB_ColorIndex=-1;	// current color index reset
	CB_BackColorIndex=0xFFFF;	// current color index reset
	FkeyClearAll();
//	PopUpWin( 4 );
	MsgBoxPush( 4 );
	locate(3,3); Prints((unsigned char *)buffer);
	locate(3,5); Prints((unsigned char *) "   Press:[EXIT]");

//	Bdisp_PutDisp_DD();

	WaitKeyAC();
	KeyRecover(); 
	while ( 1 ) {
		EnableDisplayStatusArea();
		GetKey_DisableCatalog(&key);
		if ( key == KEY_CTRL_EXIT  ) break ;
		if ( key == KEY_CTRL_AC    ) break ;
		if ( key == KEY_CTRL_RIGHT ) break ;
		if ( key == KEY_CTRL_LEFT  ) break ;
	}
	
//	RestoreDisp(SAVEDISP_PAGE1);
//	Bdisp_PutDisp_DD();
	MsgBoxPop();
//	i=0;
//	for( y=30; y<179; y++) {
//		p=26+384*y; for ( x=26; x<355; x++) vram[p++]=buf[i++];
//	}
	CB_ColorIndex=color;	// current color index restore
	CB_BackColorIndex=bcolor;	// current color index restore
}


int CannotSupportERROR( int c ) {
	int key;
	int n=0,r=0;
	char hex1[][5]={"----","003F","041F","F820","FC00"};
	char hex2[][5]={"----","001F","03FF","F800","FBE0"};
	int color=CB_ColorIndex;	// current color index 
	unsigned short bcolor=CB_BackColorIndex;	// current color index 
	if ( TryFlag ) goto error;

	switch (c) {
		case 0x003F:
			n=1;
			break;
		case 0x041F:
			n=2;
			break;
		case 0xF820:
			n=3;
			break;
		case 0xFC00:
			n=4;
			break;
	}
	CB_ColorIndex=-1;	// current color index reset
	CB_BackColorIndex=0xFFFF;	// current color index reset
	FkeyClearAll();
	MsgBoxPush( 6 );
	locate( 3,2); Prints((unsigned char *)"Cannot support");
	locate(11,3); Prints((unsigned char *)"#0x");
	locate(14,3); Prints((unsigned char *)hex1[n]);
	locate( 3,4); Prints((unsigned char *)"Change to");
	locate( 8,5); Prints((unsigned char *)"#0x      Ok?");
	locate(11,5); Prints((unsigned char *)hex2[n]);
	locate( 3,6); Prints((unsigned char *)"   Yes:[F1]");
	locate( 3,7); Prints((unsigned char *)"   No :[F6]");
	CB_ColorIndex = c;
	CB_Prints( 10, 3, (unsigned char*)"\xE6\xA6");
	CB_Prints( 18, 3, (unsigned char*)"\xE6\xA6");
	CB_ColorIndex = c-0x20;
	CB_Prints(  7, 5, (unsigned char*)"\xE6\xA6");
	CB_Prints( 15, 5, (unsigned char*)"\xE6\xA6");

//	Bdisp_PutDisp_DD();

	WaitKeyAC();
	KeyRecover(); 
	while ( 1 ) {
		EnableDisplayStatusArea();
		GetKey_DisableCatalog(&key);
		if ( key == KEY_CTRL_F1    ) break;
		if ( key == KEY_CTRL_EXE   ) break;
		if ( key == KEY_CTRL_EXIT  ) break;
		if ( key == KEY_CTRL_F6    ) break;
		if ( key == KEY_CTRL_AC    ) break;
	}
	MsgBoxPop();
 	if ( key == KEY_CTRL_F1    ) goto exit;	// no error
	if ( key == KEY_CTRL_EXE   ) goto exit;	// no error
 error:
	r = 1;	// error
	switch (n) {
		case 1:
			CB_Error(CanNotSupport_003F_ERR);
			break;
		case 2:
			CB_Error(CanNotSupport_041F_ERR);
			break;
		case 3:
			CB_Error(CanNotSupport_F820_ERR);
			break;
		case 4:
			CB_Error(CanNotSupport_FC00_ERR);
			break;
	}
  exit:
	CB_ColorIndex=color;	// current color index restore
	CB_BackColorIndex=bcolor;	// current color index restore
	return r;
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
		case CanNotSupport_003F_ERR:
		case CanNotSupport_041F_ERR:
		case CanNotSupport_F820_ERR:
		case CanNotSupport_FC00_ERR:
			break;
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

}
