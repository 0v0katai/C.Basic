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
		case TryEndWithoutTryERR:
			ERROR("TryEnd w/o Try ");
			break;
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
