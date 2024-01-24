/*****************************************************************/
/*                                                               */
/*   Genuine Command input Library  ver 2.x                      */
/*                                                               */
/*   written by sentaro21                                        */
/*                                                               */
/*****************************************************************/
#include "CB.h"

//----------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------
int  CommandType=0;
char CommandPage=0;

void Menu_SHIFT_MENU(){
					FkeyClear( FKeyNo1 );
					FkeyClear( FKeyNo2 );
					Fkey_Icon( FKeyNo3, 560 );	//	Fkey_dispR( FKeyNo3, "VWIN");
					Fkey_Icon( FKeyNo4, 561 );	//	Fkey_dispR( FKeyNo4, "SKTCH");
					FkeyClear( FKeyNo5 );
					FkeyClear( FKeyNo6 );
}
void Menu_CMD_PRGM_REL(){
					Fkey_Icon( FKeyNo1, 548 );	//	Fkey_DISPR( FKeyNo1, " = ");
					Fkey_Icon( FKeyNo2, 549 );	//	Fkey_DISPR( FKeyNo2, " \x11 ");
					Fkey_Icon( FKeyNo3, 550 );	//	Fkey_DISPR( FKeyNo3, " > ");
					Fkey_Icon( FKeyNo4, 551 );	//	Fkey_DISPR( FKeyNo4, " < ");
					Fkey_Icon( FKeyNo5, 552 );	//	Fkey_DISPR( FKeyNo5, " \x12 ");
					Fkey_Icon( FKeyNo6, 553 );	//	Fkey_DISPR( FKeyNo6, " \x10 ");
}
void Menu_CMD_MENU_EXT(){
					Fkey_Icon( FKeyNo1, 509 );	//	Fkey_DISPR( FKeyNo1, " ? ");
					Fkey_Icon( FKeyNo2, 510 );	//	Fkey_DISPR( FKeyNo2, " \x0C ");
					Fkey_Icon( FKeyNo3, 515 );	//	Fkey_DISPR( FKeyNo3, " :");
					Fkey_Icon( FKeyNo4, 534 );	//	Fkey_DISPR( FKeyNo4, " \x13 ");
					Fkey_Icon( FKeyNo5, 398 );	//	Fkey_DISPR( FKeyNo5, " ' ");
					Fkey_Icon( FKeyNo6, 402 );	//	Fkkey_DISPR( FKeyNo6, " / ");
}

void DispGenuineCmdMenu(){
	switch ( CommandType ) {
		case CMD_PRGM:		//	------------------------------------------------------------PRGM
			switch ( CommandPage ) {
				case 0:
					Fkey_Icon( FKeyNo1, 506 );	//	Fkey_dispR( FKeyNo1, "COM");
					Fkey_Icon( FKeyNo2, 507 );	//	Fkey_dispR( FKeyNo2, "CTL");
					Fkey_Icon( FKeyNo3, 508 );	//	Fkey_dispR( FKeyNo3, "JUMP");
					Fkey_Icon( FKeyNo4, 509 );	//	Fkey_DISPR( FKeyNo4, " ? ");
					Fkey_Icon( FKeyNo5, 510 );	//	Fkey_DISPR( FKeyNo5, " \x0C ");
					Fkey_Icon(FKeyNo6, 6 ); //Fkey_DISPN( FKeyNo6," \xE6\x9E ");
					break;
				case 1:
					Fkey_Icon( FKeyNo1, 511 );	//	Fkey_dispR( FKeyNo1, "CLR ");
					Fkey_Icon( FKeyNo2, 512 );	//	Fkey_dispR( FKeyNo2, "DISP");
					Fkey_Icon( FKeyNo3, 513 );	//	Fkey_dispR( FKeyNo3, "REL ");
					Fkey_Icon( FKeyNo4, 514 );	//	Fkey_dispR( FKeyNo4, "I/O ");
					Fkey_Icon( FKeyNo5, 515 );	//	Fkey_DISPR( FKeyNo5, " :");
					Fkey_Icon(FKeyNo6, 6 ); //Fkey_DISPN( FKeyNo6," \xE6\x9E ");
					break;
				case 2:
					Fkey_Icon( FKeyNo1, 467 );	//	Fkey_dispR( FKeyNo1, "STR ");
					Fkey_Icon( FKeyNo2, 952 );	//	Fkey_DISPR( FKeyNo2, " $ ");		// $
					Fkey_dispN( FKeyNo3, "ExStr");
					FkeyClear( FKeyNo4 );
					Fkey_dispR( FKeyNo5, "EXEC");
					Fkey_Icon(FKeyNo6, 6 ); //Fkey_DISPN( FKeyNo6," \xE6\x9E ");
					break;
			}
			break;
		case CMD_PRGM_COM:
			switch ( CommandPage ) {
				case 0:
					Fkey_Icon( FKeyNo1, 516 );	//	Fkey_dspRB( FKeyNo1, "If ");
					Fkey_Icon( FKeyNo2, 517 );	//	Fkey_dspRB( FKeyNo2, "Then");
					Fkey_Icon( FKeyNo3, 518 );	//	Fkey_dspRB( FKeyNo3, "Else");
					Fkey_Icon( FKeyNo4, 519 );	//	Fkey_dspRB( FKeyNo4, "IEnd");
					Fkey_dspRB( FKeyNo5, "E.If");
					Fkey_Icon(FKeyNo6, 6 ); //Fkey_DISPN( FKeyNo6," \xE6\x9E ");
					break;
				case 1:
					Fkey_Icon( FKeyNo1, 520 );	//	Fkey_dspRB( FKeyNo1, "For ");
					Fkey_Icon( FKeyNo2, 521 );	//	Fkey_dspRB( FKeyNo2, " To ");
					Fkey_Icon( FKeyNo3, 522 );	//	Fkey_dspRB( FKeyNo3, "Step");
					Fkey_Icon( FKeyNo4, 523 );	//	Fkey_dspRB( FKeyNo4, "Next");
					FkeyClear( FKeyNo5 );
					Fkey_Icon(FKeyNo6, 6 ); //Fkey_DISPN( FKeyNo6," \xE6\x9E ");
					break;
				case 2:
					Fkey_Icon( FKeyNo1, 524 );	//	Fkey_dspRB( FKeyNo1, "Whle");
					Fkey_Icon( FKeyNo2, 525 );	//	Fkey_dspRB( FKeyNo2, "WEnd");
					Fkey_Icon( FKeyNo3, 526 );	//	Fkey_dspRB( FKeyNo3, " Do");
					Fkey_Icon( FKeyNo4, 527 );	//	Fkey_dspRB( FKeyNo4, "Lp.W");
					FkeyClear( FKeyNo5 );
					Fkey_Icon(FKeyNo6, 6 ); //Fkey_DISPN( FKeyNo6," \xE6\x9E ");
					break;
				case 3:
					Fkey_dspRB( FKeyNo1, "Swit");
					Fkey_dspRB( FKeyNo2, "Case");
					Fkey_dspRB( FKeyNo3, "Deft");
					Fkey_Icon( FKeyNo4, 530 );	//	Fkey_dspRB( FKeyNo4, "Brk");
					Fkey_dspRB( FKeyNo5, "S.End");
					Fkey_Icon(FKeyNo6, 6 ); //Fkey_DISPN( FKeyNo6," \xE6\x9E ");
					break;
			}
			break;
		case CMD_PRGM_CTL:
			switch ( CommandPage ) {
				case 0:
					Fkey_Icon( FKeyNo1, 528 );	//	Fkey_dspRB( FKeyNo1, "Prog");
					Fkey_Icon( FKeyNo2, 529 );	//	Fkey_dspRB( FKeyNo2, "Rtrn");
					Fkey_Icon( FKeyNo3, 530 );	//	Fkey_dspRB( FKeyNo3, "Brk");
					Fkey_Icon( FKeyNo4, 531 );	//	Fkey_dspRB( FKeyNo4, "Stop");
					Fkey_dspRB( FKeyNo5, "Local");
					Fkey_Icon(FKeyNo6, 6 ); //Fkey_DISPN( FKeyNo6," \xE6\x9E ");
					break;
				case 1:
					Fkey_dspRB( FKeyNo1, "Gosub");
					Fkey_dspRB( FKeyNo2, "ACBrk");
					Fkey_dspRB( FKeyNo3, "Try");
					Fkey_dspRB( FKeyNo4, "Except ");
					Fkey_dspRB( FKeyNo5, "TryEnd");
//					Fkey_dspRB( FKeyNo5, "IsError(");
					Fkey_Icon(FKeyNo6, 6 ); //Fkey_DISPN( FKeyNo6," \xE6\x9E ");
					break;
			}
			break;
		case CMD_PRGM_JUMP:
			switch ( CommandPage ) {
				case 0:
					Fkey_Icon( FKeyNo1, 532 );	//	Fkey_dspRB( FKeyNo1, "Lbl");
					Fkey_Icon( FKeyNo2, 533 );	//	Fkey_dspRB( FKeyNo2, "Goto");
					Fkey_Icon( FKeyNo3, 534 );	//	Fkey_DISPR( FKeyNo3, " \x13 ");
					Fkey_Icon( FKeyNo4, 535 );	//	Fkey_dspRB( FKeyNo4, "ISZ");
					Fkey_Icon( FKeyNo5, 536 );	//	Fkey_dspRB( FKeyNo5, "DSZ");
					Fkey_Icon( FKeyNo6, 253 );	//	Fkey_dspRB( FKeyNo6, "Menu");
					break;
			}
			break;
		case CMD_PRGM_CLR:
			switch ( CommandPage ) {
				case 0:
					Fkey_Icon( FKeyNo1, 537 );	//	Fkey_dspRB( FKeyNo1, "Text");		// ClrText
					Fkey_Icon( FKeyNo2, 541 );	//	Fkey_dspRB( FKeyNo2, "Grph");		// ClrGraph
					Fkey_Icon( FKeyNo3, 162 );	//	Fkey_dspRB( FKeyNo3, "List");		// ClrList
					Fkey_Icon( FKeyNo4, 200 );	//	Fkey_dspRB( FKeyNo4, "Mat ");		// ClrMat
					Fkey_Icon( FKeyNo5,1112 );	//	Fkey_dspRB( FKeyNo5, "Vct ");		// ClrVct
					FkeyClear( FKeyNo6 );
					break;
			}
			break;
		case CMD_PRGM_DISP:
			switch ( CommandPage ) {
				case 0:
					Fkey_Icon( FKeyNo1, 540 );	//	Fkey_dspRB( FKeyNo1, "Stat");		// DrawStat
					Fkey_Icon( FKeyNo2, 541 );	//	Fkey_dspRB( FKeyNo2, "Grph");		// DrawGraph
					FkeyClear( FKeyNo3 );
					FkeyClear( FKeyNo4 );
					FkeyClear( FKeyNo5 );
					Fkey_dspRB( FKeyNo6, "Disp");		// Disp
					break;
			}
			break;
		case CMD_PRGM_REL:
			switch ( CommandPage ) {
				case 0:
					Menu_CMD_PRGM_REL();
					break;
			}
			break;
		case CMD_PRGM_I_O:
			switch ( CommandPage ) {
				case 0:
					Fkey_Icon( FKeyNo1, 554 );	//	Fkey_dspRB( FKeyNo1, "Lcte");
					Fkey_Icon( FKeyNo2, 555 );	//	Fkey_dspRB( FKeyNo2, "Gtky");
					Fkey_Icon( FKeyNo3, 556 );	//	Fkey_dspRB( FKeyNo3, "Send");
					Fkey_Icon( FKeyNo4, 557 );	//	Fkey_dspRB( FKeyNo4, "Recv");
					Fkey_dspRB( FKeyNo5, "KeyR");
					Fkey_Icon(FKeyNo6, 6 ); //Fkey_DISPN( FKeyNo6," \xE6\x9E ");
					break;
				case 1:
					Fkey_Icon( FKeyNo1, 264 );	//	Fkey_dspRB( FKeyNo1, "S38k");
					Fkey_Icon( FKeyNo2, 265 );	//	Fkey_dspRB( FKeyNo2, "R38k");
					Fkey_Icon( FKeyNo3, 266 );	//	Fkey_dspRB( FKeyNo3, "Open");
					Fkey_Icon( FKeyNo4, 267 );	//	Fkey_dspRB( FKeyNo4, "Close");
					Fkey_dspRB( FKeyNo5, "Beep ");
					Fkey_Icon(FKeyNo6, 6 ); //Fkey_DISPN( FKeyNo6," \xE6\x9E ");
					break;
				case 2:
					Fkey_dspRB( FKeyNo1, "IsExs");
					Fkey_dspRB( FKeyNo2, "Save");
					Fkey_dspRB( FKeyNo3, "Load");
					Fkey_dspRB( FKeyNo4, "Delet");
					FkeyClear( FKeyNo5 );
					Fkey_Icon(FKeyNo6, 6 ); //Fkey_DISPN( FKeyNo6," \xE6\x9E ");
					break;
			}
			break;
		case CMD_PRGM_STR:
			switch ( CommandPage ) {
				case 0:
					Fkey_Icon( FKeyNo1, 371 );	//	Fkey_dspRB( FKeyNo1, "Join");
					Fkey_Icon( FKeyNo2, 377 );	//	Fkey_dspRB( FKeyNo2, "Len");
					Fkey_Icon( FKeyNo3, 378 );	//	Fkey_dspRB( FKeyNo3, "Cmp");
					Fkey_Icon( FKeyNo4, 379 );	//	Fkey_dspRB( FKeyNo4, "Src");
					Fkey_dspRB( FKeyNo5, "Repl");
					Fkey_Icon(FKeyNo6, 6 ); //Fkey_DISPN( FKeyNo6," \xE6\x9E ");
					break;
				case 1:
					Fkey_Icon( FKeyNo1, 868 );	//	Fkey_dspRB( FKeyNo1, "Left");
					Fkey_Icon( FKeyNo2, 869 );	//	Fkey_dspRB( FKeyNo2, "Right");
					Fkey_Icon( FKeyNo3, 380 );	//	Fkey_dspRB( FKeyNo3, "Mid");
					Fkey_Icon( FKeyNo4, 381 );	//	Fkey_dspRB( FKeyNo4, "E\xE6\x9ES");
					Fkey_Icon( FKeyNo5, 439 );	//	Fkey_dspRB( FKeyNo5, "Exp");
					Fkey_Icon(FKeyNo6, 6 ); //Fkey_DISPN( FKeyNo6," \xE6\x9E ");
					break;
				case 2:
					Fkey_Icon( FKeyNo1, 382 );	//	Fkey_dspRB( FKeyNo1, "Upr");
					Fkey_Icon( FKeyNo2, 383 );	//	Fkey_dspRB( FKeyNo2, "Lwr");
					Fkey_Icon( FKeyNo3, 640 );	//	Fkey_dspRB( FKeyNo3, "Inv");
					Fkey_Icon( FKeyNo4, 395 );	//	Fkey_dspRB( FKeyNo4, "Shift");
					Fkey_Icon( FKeyNo5, 414 );	//	Fkey_dspRB( FKeyNo5, "Rot");
					Fkey_Icon(FKeyNo6, 6 ); //Fkey_DISPN( FKeyNo6," \xE6\x9E ");
					break;
			}
			break;
		case CMD_PRGM_EXSTR:
			switch ( CommandPage ) {
				case 0:
					Fkey_Icon( FKeyNo1, 268 );	//	Fkey_dspRB( FKeyNo1, "Str");		// Str
					Fkey_dspRB( FKeyNo2, "ToStr");		// Str(
					Fkey_dspRB( FKeyNo3, "Sptf");		// Sprintf
					Fkey_Icon( FKeyNo4, 176 );	//	Fkey_DISPR( FKeyNo4, " % ");
					Fkey_Icon( FKeyNo5, 403 );	//	Fkey_DISPR( FKeyNo5, " # ");
					Fkey_Icon(FKeyNo6, 6 ); //Fkey_DISPN( FKeyNo6," \xE6\x9E ");
					break;
				case 1:
					Fkey_dspRB( FKeyNo1, "Char");
					Fkey_dspRB( FKeyNo2, "Centr");
					Fkey_dspRB( FKeyNo3, "Base");		// StrBase(
					Fkey_dspRB( FKeyNo4, "Hex(");
					Fkey_dspRB( FKeyNo5, "Bin(");
//					FkeyClear( FKeyNo5 );
					Fkey_Icon(FKeyNo6, 6 ); //Fkey_DISPN( FKeyNo6," \xE6\x9E ");
					break;
				case 2:
					Fkey_dspRB( FKeyNo1, "Split");
					Fkey_dspRB( FKeyNo2, "Asc(");
//					FkeyClear( FKeyNo2 );
					Fkey_dspRB( FKeyNo3, "Repl");
					FkeyClear( FKeyNo4 );
					FkeyClear( FKeyNo5 );
					Fkey_Icon(FKeyNo6, 6 ); //Fkey_DISPN( FKeyNo6," \xE6\x9E ");
					break;
			}
			break;
		case CMD_PRGM_EXEC:
			switch ( CommandPage ) {
				case 0:
					Fkey_dspRB( FKeyNo1, "SysCl");
					Fkey_dspRB( FKeyNo2, "Call");
					Fkey_dspRB( FKeyNo3, "Poke");
					Fkey_dspRB( FKeyNo4, "Peek");
					FkeyClear( FKeyNo5 );
					Fkey_Icon(FKeyNo6, 6 ); //Fkey_DISPN( FKeyNo6," \xE6\x9E ");
					break;
				case 1:
					Fkey_dspRB( FKeyNo1, "VarPt");
					Fkey_dspRB( FKeyNo2, "PrgPt");
					Fkey_Icon( FKeyNo3, 401 );	//	Fkey_DISPR( FKeyNo3, " *");
					Fkey_DISPR( FKeyNo4, " &");
					FkeyClear( FKeyNo5 );
					Fkey_Icon(FKeyNo6, 6 ); //Fkey_DISPN( FKeyNo6," \xE6\x9E ");
					break;
			}
			break;
			
		case CMD_OPTN:		//	------------------------------------------------------------OPTN
			switch ( CommandPage ) {
				case 0:
					Fkey_Icon( FKeyNo1, 178 );	//	Fkey_dispR( FKeyNo1, "LIST");
					Fkey_Icon( FKeyNo2, 179 );	//	Fkey_dispR( FKeyNo2, "MAT ");
					Fkey_Icon( FKeyNo3, 180  );	//	Fkey_dispR( FKeyNo3, "CPLX");
					Fkey_Icon( FKeyNo4, 2 );	//	Fkey_dispR( FKeyNo4, "CALC");
					Fkey_dispN( FKeyNo5, "Extd");
//					FkeyClear( FKeyNo5 );
					Fkey_Icon(FKeyNo6, 6 ); //Fkey_DISPN( FKeyNo6," \xE6\x9E ");
					break;
				case 1:
					Fkey_dispN( FKeyNo1, " @ ");
					Fkey_Icon( FKeyNo2, 142 );	//	Fkey_dispR( FKeyNo2, "HYP");
					Fkey_Icon( FKeyNo3, 143 );	//	Fkey_dispR( FKeyNo3, "PROB");
					Fkey_Icon( FKeyNo4, 144 );	//	Fkey_dispR( FKeyNo4, "NUM");
					Fkey_Icon( FKeyNo5, 145 );	//	Fkey_dispR( FKeyNo5, "ANGL");
					Fkey_Icon(FKeyNo6, 6 ); //Fkey_DISPN( FKeyNo6," \xE6\x9E ");
					break;
				case 2:
					Fkey_Icon( FKeyNo1, 146 );	//	Fkey_dispR( FKeyNo1, "ESYM");
					Fkey_Icon( FKeyNo2, 183 );	//	Fkey_dispR( FKeyNo2, "PICT");
					Fkey_Icon( FKeyNo3, 147 );	//	Fkey_dispR( FKeyNo3, "fMEM");
					Fkey_Icon( FKeyNo4, 148 );	//	Fkey_dispR( FKeyNo4, "LOGIC");
					Fkey_Icon( FKeyNo5,  45 );	//	Fkey_dispR( FKeyNo5, "CAPT");
//					FkeyClear( FKeyNo5 );
					Fkey_Icon(FKeyNo6, 6 ); //Fkey_DISPN( FKeyNo6," \xE6\x9E ");
					break;
			}
			break;
		case CMD_OPTN_LIST:
			switch ( CommandPage ) {
				case 0:
					Fkey_Icon( FKeyNo1, 162 );	//	Fkey_dspRB( FKeyNo1, "List");
					Fkey_Icon( FKeyNo2, 167 );	//	Fkey_dspRB( FKeyNo2, "L\xE6\x91M");
					Fkey_Icon( FKeyNo3, 164 );	//	Fkey_dspRB( FKeyNo3, "Dim ");
					Fkey_Icon( FKeyNo4, 206 );	//	Fkey_dspRB( FKeyNo4, "Fill");
					Fkey_Icon( FKeyNo5, 166 );	//	Fkey_dspRB( FKeyNo5, "Seq ");
					Fkey_Icon(FKeyNo6, 6 ); //Fkey_DISPN( FKeyNo6," \xE6\x9E ");
					break;
				case 1:
					Fkey_Icon( FKeyNo1, 169 );	//	Fkey_dspRB( FKeyNo1, "Min ");
					Fkey_Icon( FKeyNo2, 170 );	//	Fkey_dspRB( FKeyNo2, "Max ");
					Fkey_Icon( FKeyNo3, 171 );	//	Fkey_dspRB( FKeyNo3, "Mean");
					FkeyClear( FKeyNo4 );
					Fkey_Icon( FKeyNo5, 204 );	//	Fkey_dspRB( FKeyNo5, "Aug ");
					Fkey_Icon(FKeyNo6, 6 ); //Fkey_DISPN( FKeyNo6," \xE6\x9E ");
					break;
				case 2:
					Fkey_Icon( FKeyNo1, 173 );	//	Fkey_dspRB( FKeyNo1, "Sum ");
					Fkey_Icon( FKeyNo2, 174 );	//	Fkey_dspRB( FKeyNo2, "Prod");
					FkeyClear( FKeyNo3 );
					FkeyClear( FKeyNo4 );
//					FkeyClear( FKeyNo5 );
					Fkey_Icon( FKeyNo5, 378 );	//	Fkey_dispN( FKeyNo5, "Comp");
					Fkey_Icon(FKeyNo6, 6 ); //Fkey_DISPN( FKeyNo6," \xE6\x9E ");
					break;
			}
			break;
		case CMD_OPTN_MAT:
			switch ( CommandPage ) {
				case 0:
					Fkey_Icon( FKeyNo1, 200 );	//	Fkey_dspRB( FKeyNo1, "Mat");
					Fkey_Icon( FKeyNo2, 201 );	//	Fkey_dspRB( FKeyNo2, "M\xE6\x91L");
					Fkey_Icon( FKeyNo3, 202 );	//	Fkey_dspRB( FKeyNo3, "Det ");
					Fkey_Icon( FKeyNo4, 203 );	//	Fkey_dspRB( FKeyNo4, "Trn ");
					Fkey_Icon( FKeyNo5, 204 );	//	Fkey_dspRB( FKeyNo5, "Aug ");
					Fkey_Icon(FKeyNo6, 6 ); //Fkey_DISPN( FKeyNo6," \xE6\x9E ");
					break;
				case 1:
					Fkey_Icon( FKeyNo1, 205 );	//	Fkey_dspRB( FKeyNo1, "Iden");
					Fkey_Icon( FKeyNo2, 164 );	//	Fkey_dspRB( FKeyNo2, "Dim ");
					Fkey_Icon( FKeyNo3, 206 );	//	Fkey_dspRB( FKeyNo3, "Fill");
					FkeyClear( FKeyNo4 );
					Fkey_Icon( FKeyNo5, 790 );	//	Fkey_dispR( FKeyNo5, "SIZE");
					Fkey_Icon(FKeyNo6, 6 ); //Fkey_DISPN( FKeyNo6," \xE6\x9E ");
					break;
				case 2:
					Fkey_Icon( FKeyNo1,1112 );	//	Fkey_dspRB( FKeyNo1, "Vct");
					Fkey_Icon( FKeyNo2,1111 );	//	Fkey_dspRB( FKeyNo2, "DotP(");
					Fkey_Icon( FKeyNo3,1110 );	//	Fkey_dspRB( FKeyNo3, "CrossP(");
					Fkey_Icon( FKeyNo4,1109 );	//	Fkey_dspRB( FKeyNo3, "Angle(");
					Fkey_Icon( FKeyNo5,1107 );	//	Fkey_dispR( FKeyNo5, "UnitV(");
					Fkey_Icon(FKeyNo6, 6 ); //Fkey_DISPN( FKeyNo6," \xE6\x9E ");
					break;
				case 3:
					Fkey_Icon( FKeyNo1,1108 );	//	Fkey_dspRB( FKeyNo1, "Norm(");
					FkeyClear( FKeyNo2 );
					FkeyClear( FKeyNo3 );
					FkeyClear( FKeyNo4 );
					FkeyClear( FKeyNo5 );
					Fkey_Icon(FKeyNo6, 6 ); //Fkey_DISPN( FKeyNo6," \xE6\x9E ");
					break;
			}
			break;
		case CMD_OPTN_MAT_SIZE:
			switch ( CommandPage ) {
				case 0:
					Fkey_dspRB( FKeyNo1, "Base");
					Fkey_dspRB( FKeyNo2, "Elem");
					Fkey_dspRB( FKeyNo3, "Row");
					Fkey_dspRB( FKeyNo4, "Col");
					FkeyClear( FKeyNo5 );
					FkeyClear( FKeyNo6 );
					break;
			}
			break;
		case CMD_OPTN_CPLX:
			switch ( CommandPage ) {
				case 0:
					Fkey_Icon( FKeyNo1, 207 );	//	Fkey_dspRB( FKeyNo1, " i");
					Fkey_Icon( FKeyNo2, 157 );	//	Fkey_dspRB( FKeyNo2, "Abs");
					Fkey_Icon( FKeyNo3, 209 );	//	Fkey_dspRB( FKeyNo3, "Arg");
					Fkey_Icon( FKeyNo4, 210 );	//	Fkey_dspRB( FKeyNo4, "Conjg");
					FkeyClear( FKeyNo5 );
					Fkey_Icon(FKeyNo6, 6 ); //Fkey_DISPN( FKeyNo6," \xE6\x9E ");
					break;
				case 1:
					Fkey_Icon( FKeyNo1, 211 );	//	Fkey_dspRB( FKeyNo1, "ReP");
					Fkey_Icon( FKeyNo2, 212 );	//	Fkey_dspRB( FKeyNo2, "ImP");
					Fkey_Icon( FKeyNo3, 109 );	//	Fkey_dspRB( FKeyNo3, ">R_Theta");
					Fkey_Icon( FKeyNo4, 110 );	//	Fkey_dspRB( FKeyNo4, ">a+bi");
					FkeyClear( FKeyNo5 );
					Fkey_Icon(FKeyNo6, 6 ); //Fkey_DISPN( FKeyNo6," \xE6\x9E ");
					break;
			}
			break;
		
		case CMD_OPTN_CALC:
			switch ( CommandPage ) {
				case 1:
					FkeyClear( FKeyNo1 );
//					Fkey_Icon( FKeyNo1, 185 );	//  d/dx
					Fkey_Icon( FKeyNo2, 187 );	//  Integral
					Fkey_Icon( FKeyNo3, 190 );	//	Fkey_dspRB( FKeyNo3, " \xE5\x51( ");	// sigma
					Fkey_Icon( FKeyNo4, 410 );	//	Fkey_dspRB( FKeyNo4, "logab");
					FkeyClear( FKeyNo5 );
					Fkey_Icon(FKeyNo6, 6 ); //Fkey_DISPN( FKeyNo6," \xE6\x9E ");
					break;
				case 2:
					Fkey_Icon( FKeyNo1, 168 );	//	Fkey_dspRB( FKeyNo1, "Int\xB9");
					Fkey_Icon( FKeyNo2, 669 );	//	Fkey_dspRB( FKeyNo2, "Rmdr");
					FkeyClear( FKeyNo3 );
					FkeyClear( FKeyNo4 );
					FkeyClear( FKeyNo5 );
					Fkey_Icon(FKeyNo6, 6 ); //Fkey_DISPN( FKeyNo6," \xE6\x9E ");
					break;
			}
			break;
		case CMD_OPTN_HYP:
			switch ( CommandPage ) {
				case 0:
					Fkey_Icon( FKeyNo1, 234 );	//	Fkey_dspRB( FKeyNo1, "sinh");
					Fkey_Icon( FKeyNo2, 235 );	//	Fkey_dspRB( FKeyNo2, "cosh");
					Fkey_Icon( FKeyNo3, 236 );	//	Fkey_dspRB( FKeyNo3, "tanh");
					Fkey_Icon( FKeyNo4, 237 );	//	Fkey_dspRB( FKeyNo4, "sih\xE5\xCA");
					Fkey_Icon( FKeyNo5, 238 );	//	Fkey_dspRB( FKeyNo5, "coh\xE5\xCA");
					Fkey_Icon( FKeyNo6, 239 );	//	Fkey_dspRB( FKeyNo6, "tah\xE5\xCA");
					break;
			}
			break;
		case CMD_OPTN_PROB:
			switch ( CommandPage ) {
				case 0:
					Fkey_Icon( FKeyNo1, 149 );	//	Fkey_dspRB( FKeyNo1, " X !");
//					FkeyClear( FKeyNo2 );
//					FkeyClear( FKeyNo3 );
					Fkey_Icon( FKeyNo2, 150 );	//	Fkey_dspRB( FKeyNo2, "nPr");
					Fkey_Icon( FKeyNo3, 151 );	//	Fkey_dspRB( FKeyNo3, "nCr");
					Fkey_Icon( FKeyNo4, 345 );	//	Fkey_dispR( FKeyNo4, "RAND");
					FkeyClear( FKeyNo5 );
					FkeyClear( FKeyNo6 );
					break;
			}
			break;
		case CMD_OPTN_PROB_RAND:
			switch ( CommandPage ) {
				case 0:
					Fkey_Icon( FKeyNo1, 152 );	//	Fkey_dspRB( FKeyNo1, "Ran#");
					Fkey_Icon( FKeyNo2, 158 );	//	Fkey_dspRB( FKeyNo2, "Int ");
					Fkey_Icon( FKeyNo3, 487 );	//	Fkey_dspRB( FKeyNo2, "Norm");
					Fkey_Icon( FKeyNo4, 504 );	//	Fkey_dspRB( FKeyNo2, "Bin ");
					Fkey_Icon( FKeyNo5, 162 );	//	Fkey_dspRB( FKeyNo5, "List");
					FkeyClear( FKeyNo6 );
					break;
			}
			break;
		case CMD_OPTN_NUM:
			switch ( CommandPage ) {
				case 0:
					Fkey_Icon( FKeyNo1, 157 );	//	Fkey_dspRB( FKeyNo1, "Abs ");
					Fkey_Icon( FKeyNo2, 158 );	//	Fkey_dspRB( FKeyNo2, "Int ");
					Fkey_Icon( FKeyNo3, 159 );	//	Fkey_dspRB( FKeyNo3, "Frac");
					Fkey_Icon( FKeyNo4, 160 );	//	Fkey_dspRB( FKeyNo4, "Rnd ");
					Fkey_Icon( FKeyNo5, 161 );	//	Fkey_dspRB( FKeyNo5, "Intg");
					Fkey_Icon(FKeyNo6, 6 ); //Fkey_DISPN( FKeyNo6," \xE6\x9E ");
					break;
				case 1:
					Fkey_Icon( FKeyNo1, 107 );	//	Fkey_dspRB( FKeyNo1, "RndFi");
					Fkey_Icon( FKeyNo2, 883 );	//	Fkey_dspRB( FKeyNo2, "GCD");
					Fkey_Icon( FKeyNo3, 574 );	//	Fkey_dspRB( FKeyNo3, "LCM");
					Fkey_Icon( FKeyNo4, 884 );	//	Fkey_dspRB( FKeyNo4, "MOD ");
					FkeyClear( FKeyNo5 );
					Fkey_Icon(FKeyNo6, 6 ); //Fkey_DISPN( FKeyNo6," \xE6\x9E ");
					break;
			}
			break;
		case CMD_OPTN_ANGL:
			switch ( CommandPage ) {
				case 0:
					Fkey_Icon( FKeyNo1, 193 );	//	Fkey_DISPR( FKeyNo1, " \x9C ");
					Fkey_Icon( FKeyNo2, 194 );	//	Fkey_DISPR( FKeyNo2, " \xAC ");
					Fkey_Icon( FKeyNo3, 195 );	//	Fkey_DISPR( FKeyNo3, " \xBC ");
					Fkey_Icon( FKeyNo4, 196 );	//	Fkey_dspRB( FKeyNo4, "\x9C,,,");
					FkeyClear( FKeyNo5 );
					Fkey_Icon(FKeyNo6, 6 ); //Fkey_DISPN( FKeyNo6," \xE6\x9E ");
					break;
				case 1:
					Fkey_Icon( FKeyNo1, 198 );	//	Fkey_dspRB( FKeyNo1, "Pol(");
					Fkey_Icon( FKeyNo2, 199 );	//	Fkey_dspRB( FKeyNo2, "Rec(");
					Fkey_Icon( FKeyNo3, 108 );	//	Fkey_dspRB( FKeyNo3, "\xE6\x9E.DMS");
					FkeyClear( FKeyNo4 );
					FkeyClear( FKeyNo5 );
					Fkey_Icon(FKeyNo6, 6 ); //Fkey_DISPN( FKeyNo6," \xE6\x9E ");
					break;
			}
			break;
		case CMD_OPTN_ESYM:
			switch ( CommandPage ) {
				case 0:
					Fkey_Icon( FKeyNo1, 215 );	//	Fkey_DISPR( FKeyNo1, " m ");
					Fkey_Icon( FKeyNo2, 216 );	//	Fkey_DISPR( FKeyNo2, " \xE6\x4B ");
					Fkey_Icon( FKeyNo3, 217 );	//	Fkey_DISPR( FKeyNo3, " \x03 ");
					Fkey_Icon( FKeyNo4, 218 );	//	Fkey_DISPR( FKeyNo4, " p ");
					Fkey_Icon( FKeyNo5, 219 );	//	Fkey_DISPR( FKeyNo5, " \x01 ");
					Fkey_Icon(FKeyNo6, 6 ); //Fkey_DISPN( FKeyNo6," \xE6\x9E ");
					break;
				case 1:
					Fkey_Icon( FKeyNo1, 220 );	//	Fkey_DISPR( FKeyNo1, " K ");
					Fkey_Icon( FKeyNo2, 221 );	//	Fkey_DISPR( FKeyNo2, " \x07 ");
					Fkey_Icon( FKeyNo3, 222 );	//	Fkey_DISPR( FKeyNo3, " \x08 ");
					Fkey_Icon( FKeyNo4, 223 );	//	Fkey_DISPR( FKeyNo4, " \x09 ");
					Fkey_Icon( FKeyNo5, 224 );	//	Fkey_DISPR( FKeyNo5, " \x0A ");
					Fkey_Icon(FKeyNo6, 6 ); //Fkey_DISPN( FKeyNo6," \xE6\x9E ");
					break;
				case 2:
					Fkey_Icon( FKeyNo1, 225 );	//	Fkey_DISPR( FKeyNo1, " \x0B ");
					FkeyClear( FKeyNo2 );
					FkeyClear( FKeyNo3 );
					FkeyClear( FKeyNo4 );
					FkeyClear( FKeyNo5 );
					Fkey_Icon(FKeyNo6, 6 ); //Fkey_DISPN( FKeyNo6," \xE6\x9E ");
					break;
			}
			break;
		case CMD_OPTN_PICT:
			switch ( CommandPage ) {
				case 0:
					Fkey_Icon( FKeyNo1, 699 );	//	Fkey_dspRB( FKeyNo1, "Sto ");
					Fkey_Icon( FKeyNo2, 700 );	//	Fkey_dspRB( FKeyNo2, "Rcl ");
					FkeyClear( FKeyNo3 );
					FkeyClear( FKeyNo4 );
					FkeyClear( FKeyNo5 );
					FkeyClear( FKeyNo6 );
					break;
			}
			break;
		case CMD_OPTN_FMEM:
			switch ( CommandPage ) {
				case 0:
					Fkey_Icon( FKeyNo1,  69 );	//	Fkey_dspRB( FKeyNo1, "STORE");
					Fkey_Icon( FKeyNo2,  70 );	//	Fkey_dspRB( FKeyNo2, "RECALL");
					Fkey_Icon( FKeyNo3, 240 );	//	Fkey_dspRB( FKeyNo3, " fn ");
					Fkey_Icon( FKeyNo4, 241 );	//	Fkey_dispN( FKeyNo4, " SEE");
					FkeyClear( FKeyNo5 );
					FkeyClear( FKeyNo6 );
					break;
			}
			break;
		case CMD_OPTN_LOGIC:
			switch ( CommandPage ) {
				case 0:
					Fkey_Icon( FKeyNo1, 254 );	//	Fkey_dspRB( FKeyNo1, "And");
					Fkey_Icon( FKeyNo2, 255 );	//	Fkey_dspRB( FKeyNo2, " Or");
					Fkey_Icon( FKeyNo3, 256 );	//	Fkey_dspRB( FKeyNo3, "Not");
					Fkey_Icon( FKeyNo4, 610 );	//	Fkey_dspRB( FKeyNo4, "Xor");
					FkeyClear( FKeyNo5 );
					Fkey_Icon(FKeyNo6, 6 ); //Fkey_DISPN( FKeyNo6," \xE6\x9E ");
					break;
				case 1:
					Fkey_Icon( FKeyNo1,337 );	//	Fkey_dspRB( FKeyNo1, "and");
					Fkey_Icon( FKeyNo2,338 );	//	Fkey_dspRB( FKeyNo2, "or");
					Fkey_dspRB( FKeyNo3, "not");
					Fkey_Icon( FKeyNo4,339 );	//	Fkey_dspRB( FKeyNo4, "xor");
					FkeyClear( FKeyNo5 );
					Fkey_Icon(FKeyNo6, 6 ); //Fkey_DISPN( FKeyNo6," \xE6\x9E ");
					break;
			}
			break;
		case CMD_OPTN_CAPT:
			switch ( CommandPage ) {
				case 0:
					Fkey_Icon( FKeyNo1, 699 );	//	Fkey_dspRB( FKeyNo1, "Sto ");
					Fkey_Icon( FKeyNo2, 700 );	//	Fkey_dspRB( FKeyNo2, "Rcl ");
					FkeyClear( FKeyNo3 );
					FkeyClear( FKeyNo4 );
					FkeyClear( FKeyNo5 );
					FkeyClear( FKeyNo6 );
					break;
			}
			break;
		case CMD_OPTN_EXT:
			switch ( CommandPage ) {
				case 0:
					Fkey_dspRB( FKeyNo1, "Alias");
					Fkey_DISPR( FKeyNo2, " _ ");
//					Fkey_Icon( FKeyNo2, 400 );	//	Fkey_DISPR( FKeyNo2, " ~ ");
					Fkey_dspRB( FKeyNo3, "Const");
					Fkey_DISPR( FKeyNo4, " & ");
					Fkey_Icon( FKeyNo5, 403 );	//	Fkey_DISPR( FKeyNo5, " # ");
					Fkey_Icon( FKeyNo6, 176 );	//	Fkey_DISPR( FKeyNo6, " % ");
					break;
			}
			break;
			
		case CMD_VARS:		//	------------------------------------------------------------VARS
			switch ( CommandPage ) {
				case 0:
					Fkey_Icon( FKeyNo1, 560 );	//	Fkey_dispR( FKeyNo1, "VWIN");
					Fkey_Icon( FKeyNo2, 565 );	//	Fkey_dispR( FKeyNo2, "FACT");
					Fkey_dispN( FKeyNo3, "Extd");
					Fkey_Icon( FKeyNo4,   1 );	//	Fkey_dispR( FKeyNo4, "GRPH");
					Fkey_Icon( FKeyNo5, 172 );	//	Fkey_dispN( FKeyNo5, "SETUP");
					Fkey_Icon( FKeyNo6,   6 );	//	Fkey_DISPN( FKeyNo6," \xE6\x9E ");
					break;
				case 1:
					Fkey_Icon( FKeyNo1, 416 );	//	Fkey_dispR( FKeyNo1, "TABLE");
					Fkey_Icon( FKeyNo2, 403 );	//	Fkey_DISPR( FKeyNo5, " # ");
//					Fkey_Icon( FKeyNo2, 176 );	//	Fkey_DISPR( FKeyNo6, " % ");
					Fkey_Icon( FKeyNo3, 400 );	//	Fkey_DISPR( FKeyNo2, " ~ ");
					Fkey_Icon( FKeyNo4, 952 );	//	Fkey_DISPR( FKeyNo4, " $ ");
					Fkey_Icon( FKeyNo5, 268 );	//	Fkey_dspRB( FKeyNo5, "Str ");
					Fkey_Icon(FKeyNo6, 6 ); //Fkey_DISPN( FKeyNo6," \xE6\x9E ");
					break;
			}
			break;
		case CMD_VARS_VWIN:
			switch ( CommandPage ) {
				case 0:
					Fkey_Icon( FKeyNo1, 580 );	//	Fkey_DISPR( FKeyNo1, " X ");
					Fkey_Icon( FKeyNo2, 581 );	//	Fkey_DISPR( FKeyNo2, " Y ");
					Fkey_Icon( FKeyNo3, 582 );	//	Fkey_DISPR( FKeyNo3, "T,\xE6\x47");
					FkeyClear( FKeyNo4 );
					FkeyClear( FKeyNo5 );
					FkeyClear( FKeyNo6 );
					break;
			}
			break;
		case CMD_VARS_VWIN_X:
			switch ( CommandPage ) {
				case 0:
					Fkey_Icon( FKeyNo1, 577 );	//	Fkey_dspRB( FKeyNo1, "min");
					Fkey_Icon( FKeyNo2, 578 );	//	Fkey_dspRB( FKeyNo2, "max");
					Fkey_Icon( FKeyNo3, 579 );	//	Fkey_dspRB( FKeyNo3, "scal");
					Fkey_Icon( FKeyNo4, 281 );	//	Fkey_dspRB( FKeyNo4, "dot" );
					FkeyClear( FKeyNo5 );
					FkeyClear( FKeyNo6 );
					break;
			}
			break;
		case CMD_VARS_VWIN_Y:
			switch ( CommandPage ) {
				case 0:
					Fkey_Icon( FKeyNo1, 577 );	//	Fkey_dspRB( FKeyNo1, "min");
					Fkey_Icon( FKeyNo2, 578 );	//	Fkey_dspRB( FKeyNo2, "max");
					Fkey_Icon( FKeyNo3, 579 );	//	Fkey_dspRB( FKeyNo3, "scal");
					FkeyClear( FKeyNo4 );
					FkeyClear( FKeyNo5 );
					FkeyClear( FKeyNo6 );
					break;
			}
			break;
		case CMD_VARS_VWIN_T:
			switch ( CommandPage ) {
				case 0:
					Fkey_Icon( FKeyNo1, 577 );	//	Fkey_dspRB( FKeyNo1, "min");
					Fkey_Icon( FKeyNo2, 578 );	//	Fkey_dspRB( FKeyNo2, "max");
					Fkey_Icon( FKeyNo3, 586 );	//	Fkey_dspRB( FKeyNo3, "ptch");
					FkeyClear( FKeyNo4 );
					FkeyClear( FKeyNo5 );
					FkeyClear( FKeyNo6 );
					break;
			}
			break;
		case CMD_VARS_FACT:
			switch ( CommandPage ) {
				case 0:
					Fkey_Icon( FKeyNo1, 587 );	//	Fkey_dspRB( FKeyNo1, "Xfct");
					Fkey_Icon( FKeyNo2, 588 );	//	Fkey_dspRB( FKeyNo2, "Yfct");
					FkeyClear( FKeyNo3 );
					FkeyClear( FKeyNo4 );
					FkeyClear( FKeyNo5 );
					FkeyClear( FKeyNo6 );
					break;
			}
			break;
		case CMD_VARS_GRPH:
			switch ( CommandPage ) {
				case 0:
					Fkey_Icon( FKeyNo1, 622 );	//	Fkey_DISPR( FKeyNo1, " Y ");
					Fkey_Icon( FKeyNo2, 623 );	//	Fkey_DISPR( FKeyNo1, " r ");
					Fkey_Icon( FKeyNo3, 624 );	//	Fkey_DISPR( FKeyNo1, " Xt ");
					Fkey_Icon( FKeyNo4, 625 );	//	Fkey_DISPR( FKeyNo1, " Yt ");
					Fkey_Icon( FKeyNo5, 626 );	//	Fkey_DISPR( FKeyNo1, " X ");
					FkeyClear( FKeyNo6 );
					break;
			}
			break;
		case CMD_VARS_TABL:
			switch ( CommandPage ) {
				case 0:
					Fkey_Icon( FKeyNo1, 627 );	//	Fkey_dspRB( FKeyNo1, "Start");
					Fkey_Icon( FKeyNo2, 628 );	//	Fkey_dspRB( FKeyNo2, "End");
					Fkey_Icon( FKeyNo3, 629 );	//	Fkey_dspRB( FKeyNo3, "Pitch");
//					Fkey_Icon( FKeyNo4, 630 );	//	Fkey_dspRB( FKeyNo3, "Result");
					FkeyClear( FKeyNo4 );
					FkeyClear( FKeyNo5 );
					FkeyClear( FKeyNo6 );
					break;
			}
			break;
		case CMD_VARS_EXT:
			switch ( CommandPage ) {
				case 0:
					Fkey_dspRB( FKeyNo1, "Ticks");
					Fkey_dspRB( FKeyNo2, "Wait");
					Fkey_dspRB( FKeyNo3, "DATE");
					Fkey_dspRB( FKeyNo4, "TIME");
					Fkey_dspRB( FKeyNo5, "BackL");
					Fkey_dspRB( FKeyNo6, "Batt");
//					FkeyClear( FKeyNo6 );
//					Fkey_Icon(FKeyNo6, 6 ); //Fkey_DISPN( FKeyNo6," \xE6\x9E ");
					break;
//				case 1:
//					Fkey_dspRB( FKeyNo1, "RfCtl");
//					Fkey_dspRB( FKeyNo2, "RfTm");
//					FkeyClear( FKeyNo3 );
//					FkeyClear( FKeyNo4 );
//					FkeyClear( FKeyNo5 );
//					Fkey_Icon(FKeyNo6, 6 ); //Fkey_DISPN( FKeyNo6," \xE6\x9E ");
//					break;
			}
			break;

		case CMD_SHIFT:		//	-----------------------------------------------------------SHIFT
			switch ( CommandPage ) {
				case 0:
					Menu_SHIFT_MENU();
					break;
			}
			break;
		case CMD_SHIFT_VWIN:
			switch ( CommandPage ) {
				case 0:
					Fkey_Icon( FKeyNo1, 698 );	//	Fkey_dspRB( FKeyNo1, "VWin");
					Fkey_Icon( FKeyNo2, 699 );	//	Fkey_dspRB( FKeyNo2, "Sto");
					Fkey_Icon( FKeyNo3, 700 );	//	Fkey_dspRB( FKeyNo3, "Rcl");
					FkeyClear( FKeyNo4 );
					FkeyClear( FKeyNo5 );
					FkeyClear( FKeyNo6 );
					break;
			}
			break;
		case CMD_SHIFT_SKTCH:
			switch ( CommandPage ) {
				case 0:
					Fkey_Icon( FKeyNo1, 638 );	//	Fkey_dspRB( FKeyNo1, "Cls");
					Fkey_dispN( FKeyNo2, "Extd");
					Fkey_dispN( FKeyNo3, " ML ");
					Fkey_dispN( FKeyNo4, "BMP ");
//					FkeyClear( FKeyNo4 );
					Fkey_Icon(FKeyNo5, 1 );		// Fkey_dispR( FKeyNo5, "GRPH");
					Fkey_Icon(FKeyNo6, 6 );		//Fkey_DISPN( FKeyNo6," \xE6\x9E ");
					break;
				case 1:
					Fkey_Icon( FKeyNo1, 641 );	//	Fkey_dispR( FKeyNo1, "PLOT");
					Fkey_Icon( FKeyNo2, 642 );	//	Fkey_dispR( FKeyNo2, "LINE");
					Fkey_Icon( FKeyNo3, 643 );	//	Fkey_dspRB( FKeyNo3, "Crcl");
					Fkey_Icon( FKeyNo4, 644 );	//	Fkey_dspRB( FKeyNo4, "Vert");
					Fkey_Icon( FKeyNo5, 645 );	//	Fkey_dspRB( FKeyNo5, "Hztl");
					Fkey_Icon(FKeyNo6, 6 ); //Fkey_DISPN( FKeyNo6," \xE6\x9E ");
					break;
				case 2:
					FkeyClear( FKeyNo1 );
					Fkey_Icon( FKeyNo2, 537 );	//	Fkey_dspRB( FKeyNo2, "Text");
					Fkey_Icon( FKeyNo3, 701 );	//	Fkey_dispR( FKeyNo3, "PIXL");
					Fkey_Icon( FKeyNo4, 702 );	//	Fkey_dspRB( FKeyNo4, "Test");
					Fkey_Icon( FKeyNo5, 305 );	//	Fkey_dispR( FKeyNo5, "STYL");
					Fkey_Icon(FKeyNo6, 6 ); //Fkey_DISPN( FKeyNo6," \xE6\x9E ");
					break;
			}
			break;
		case CMD_SHIFT_SKTCH_GRPH:
			switch ( CommandPage ) {
				case 0:
					Fkey_Icon( FKeyNo1, 472 );	//	Fkey_dspRB( FKeyNo1, "Y= ");
//					Fkey_Icon( FKeyNo2, 473 );	//	Fkey_dspRB( FKeyNo2, "r= ");
					FkeyClear( FKeyNo2 );
					Fkey_Icon( FKeyNo3, 474 );	//	Fkey_dspRB( FKeyNo3, "Parm");
					Fkey_Icon( FKeyNo4, 468 );	//	Fkey_dspRB( FKeyNo4, "X= ");
					Fkey_Icon( FKeyNo5, 652 );	//	Fkey_dspRB( FKeyNo5, "integral= ");
//					FkeyClear( FKeyNo5 );
					Fkey_Icon(FKeyNo6, 6 ); //Fkey_DISPN( FKeyNo6," \xE6\x9E ");
					break;
				case 1:
					Fkey_Icon( FKeyNo1, 476 );	//	Fkey_dspRB( FKeyNo1, "Y> ");
					Fkey_Icon( FKeyNo2, 477 );	//	Fkey_dspRB( FKeyNo1, "Y< ");
					Fkey_Icon( FKeyNo3, 478 );	//	Fkey_dspRB( FKeyNo1, "Y>=");
					Fkey_Icon( FKeyNo4, 479 );	//	Fkey_dspRB( FKeyNo1, "Y<=");
					FkeyClear( FKeyNo5 );
					Fkey_Icon(FKeyNo6, 6 ); //Fkey_DISPN( FKeyNo6," \xE6\x9E ");
					break;
				case 2:
					Fkey_Icon( FKeyNo1, 469 );	//	Fkey_dspRB( FKeyNo1, "X> ");
					Fkey_Icon( FKeyNo2, 470 );	//	Fkey_dspRB( FKeyNo1, "X< ");
					Fkey_Icon( FKeyNo3, 481 );	//	Fkey_dspRB( FKeyNo1, "X>=");
					Fkey_Icon( FKeyNo4, 482 );	//	Fkey_dspRB( FKeyNo1, "X<=");
					FkeyClear( FKeyNo5 );
					Fkey_Icon(FKeyNo6, 6 ); //Fkey_DISPN( FKeyNo6," \xE6\x9E ");
					break;
			}
			break;
		case CMD_SHIFT_SKTCH_PLOT:
			switch ( CommandPage ) {
				case 0:
					Fkey_Icon( FKeyNo1, 497 );	//	Fkey_dspRB( FKeyNo1, "Plot");
					Fkey_Icon( FKeyNo2, 654 );	//	Fkey_dspRB( FKeyNo2, "PlOn");
					Fkey_Icon( FKeyNo3, 655 );	//	Fkey_dspRB( FKeyNo3, "PlOff");
					Fkey_Icon( FKeyNo4, 656 );	//	Fkey_dspRB( FKeyNo4, "PlChg");
					FkeyClear( FKeyNo5 );
					FkeyClear( FKeyNo6 );
					break;
			}
			break;
		case CMD_SHIFT_SKTCH_LINE:
			switch ( CommandPage ) {
				case 0:
					Fkey_Icon( FKeyNo1, 664 );	//	Fkey_dspRB( FKeyNo1, "Line");
					Fkey_Icon( FKeyNo2, 665 );	//	Fkey_dspRB( FKeyNo2, "FLine");
					FkeyClear( FKeyNo3 );
					FkeyClear( FKeyNo4 );
					FkeyClear( FKeyNo5 );
					FkeyClear( FKeyNo6 );
					break;
			}
			break;
		case CMD_SHIFT_SKTCH_PIXL:
			switch ( CommandPage ) {
				case 0:
					Fkey_Icon( FKeyNo1, 480 );	//	Fkey_dspRB( FKeyNo1, "On ");
					Fkey_Icon( FKeyNo2, 422 );	//	Fkey_dspRB( FKeyNo2, "Off");
					Fkey_Icon( FKeyNo3, 660 );	//	Fkey_dspRB( FKeyNo3, "Chg");
					FkeyClear( FKeyNo4 );
					FkeyClear( FKeyNo5 );
					FkeyClear( FKeyNo6 );
					break;
			}
			break;
		case CMD_SHIFT_SKTCH_STYL:
			switch ( CommandPage ) {
				case 0:
					FkeyS_L_();
					break;
			}
			break;
		case CMD_SHIFT_SKTCH_EXT:
			switch ( CommandPage ) {
				case 0:
					Fkey_dspRB( FKeyNo1, "Scrn");
					Fkey_dspRB( FKeyNo2, "DspDD");
					Fkey_dspRB( FKeyNo3, "LctYX");
					Fkey_dspRB( FKeyNo4, "Fkey(");
					Fkey_dspRB( FKeyNo5, "PupW");
					Fkey_Icon(FKeyNo6, 6 ); //Fkey_DISPN( FKeyNo6," \xE6\x9E ");
					break;
				case 1:
					Fkey_dspRB( FKeyNo1, "Rect");
					Fkey_dspRB( FKeyNo2, "FRect");
					Fkey_dspRB( FKeyNo3, "RdGR");
					Fkey_dspRB( FKeyNo4, "WrGR");
					FkeyClear( FKeyNo5 );
					Fkey_Icon(FKeyNo6, 6 ); //Fkey_DISPN( FKeyNo6," \xE6\x9E ");
					break;
				case 2:
					Fkey_dspRB( FKeyNo1, "DGet");
					Fkey_dspRB( FKeyNo2, "DPut");
					Fkey_dspRB( FKeyNo3, "Dtrim");
					Fkey_dspRB( FKeyNo4, "Dlife");
					Fkey_dspRB( FKeyNo5, "CeSum");
					Fkey_Icon(FKeyNo6, 6 ); //Fkey_DISPN( FKeyNo6," \xE6\x9E ");
					break;
			}
			break;
		case CMD_SHIFT_SKTCH_ML:
			switch ( CommandPage ) {
				case 0:
					Fkey_dspRB( FKeyNo1, "ClVra");
					Fkey_dspRB( FKeyNo2, "ClScr");
					Fkey_dspRB( FKeyNo3, "DspVr");
					Fkey_dspRB( FKeyNo4, "PxTest");
					Fkey_dispN( FKeyNo5, "Test");
					Fkey_Icon(FKeyNo6, 6 ); //Fkey_DISPN( FKeyNo6," \xE6\x9E ");
					break;
				case 1:
					Fkey_dspRB( FKeyNo1, "Pixel");
					Fkey_dspRB( FKeyNo2, "Point");
					Fkey_dspRB( FKeyNo3, "Line");
					Fkey_dspRB( FKeyNo4, "Horiz");
					Fkey_dspRB( FKeyNo5, "Verti");
					Fkey_Icon(FKeyNo6, 6 ); //Fkey_DISPN( FKeyNo6," \xE6\x9E ");
					break;
				case 2:
					Fkey_dspRB( FKeyNo1, "Rect");
					Fkey_dspRB( FKeyNo2, "Poly");
					Fkey_dspRB( FKeyNo3, "FPoly");
					Fkey_dspRB( FKeyNo4, "Circl");
					Fkey_dspRB( FKeyNo5, "FCirc");
					Fkey_Icon(FKeyNo6, 6 ); //Fkey_DISPN( FKeyNo6," \xE6\x9E ");
					break;
				case 3:
					Fkey_dspRB( FKeyNo1, "Elips");
					Fkey_dspRB( FKeyNo2, "FElip");
					Fkey_dspRB( FKeyNo3, "EInR");
					Fkey_dspRB( FKeyNo4, "FEInR");
					FkeyClear( FKeyNo5 );
					Fkey_Icon(FKeyNo6, 6 ); //Fkey_DISPN( FKeyNo6," \xE6\x9E ");
					break;
				case 4:
					Fkey_dspRB( FKeyNo1, "Hscrl");
					Fkey_dspRB( FKeyNo2, "Vscrl");
					FkeyClear( FKeyNo3 );
					FkeyClear( FKeyNo4 );
					Fkey_dspRB( FKeyNo5, "Contr");
					Fkey_Icon(FKeyNo6, 6 ); //Fkey_DISPN( FKeyNo6," \xE6\x9E ");
					break;
			}
			break;
		case CMD_SHIFT_SKTCH_BMP:
			switch ( CommandPage ) {
				case 0:
					Fkey_dspRB( FKeyNo1, "Bmp");
					Fkey_dspRB( FKeyNo2, "Bm8");
					Fkey_dspRB( FKeyNo3, "Bm16");
					Fkey_dspRB( FKeyNo4, "Zoom");
					Fkey_dspRB( FKeyNo5, "Rota");
					Fkey_Icon(FKeyNo6, 6 ); //Fkey_DISPN( FKeyNo6," \xE6\x9E ");
					break;
				case 1:
					Fkey_dspRB( FKeyNo1, "Save");
					Fkey_dspRB( FKeyNo2, "Load");
					Fkey_dspRB( FKeyNo3, "Draw");
					Fkey_dspRB( FKeyNo4, "ZmRt");
					FkeyClear( FKeyNo5 );
					Fkey_Icon(FKeyNo6, 6 ); //Fkey_DISPN( FKeyNo6," \xE6\x9E ");
					break;
				case 2:
					Fkey_dspRB( FKeyNo1, "GetFt(");
					Fkey_dspRB( FKeyNo2, "SetFt");
					Fkey_dspRB( FKeyNo3, "GetMin(");
					Fkey_dspRB( FKeyNo4, "SetMini");
					FkeyClear( FKeyNo5 );
					Fkey_Icon(FKeyNo6, 6 ); //Fkey_DISPN( FKeyNo6," \xE6\x9E ");
					break;
			}
			break;

		case CMD_MENU:		//	------------------------------------------------------------MENU
			switch ( CommandPage ) {
				case 0:
					Fkey_Icon( FKeyNo1, 181 );	//	Fkey_dispR( FKeyNo1, "STAT");
					Fkey_Icon( FKeyNo2, 179 );	//	Fkey_dispR( FKeyNo2, "MAT ");
					Fkey_Icon( FKeyNo3, 178 );	//	Fkey_dispR( FKeyNo3, "LIST");
					Fkey_Icon( FKeyNo4,   1 );	//	Fkey_dispR( FKeyNo4, "GRAPH");
//					Fkey_Icon( FKeyNo5, 415 );	//	Fkey_dispR( FKeyNo4, "DYNA");
					Fkey_dispN( FKeyNo5, "extc");
					Fkey_Icon(FKeyNo6, 6 ); //Fkey_DISPN( FKeyNo6," \xE6\x9E ");
					break;
				case 1:
					Fkey_Icon( FKeyNo1, 416 );	//	Fkey_dispR( FKeyNo1, "TABLE");
					FkeyClear( FKeyNo2 );
					FkeyClear( FKeyNo3 );
//					Fkey_Icon( FKeyNo2, 417 );	//	Fkey_dispR( FKeyNo2, "RECURSION");
					Fkey_Icon( FKeyNo4, 403 );	//	Fkey_DISPN( FKeyNo6, " # ");
					Fkey_Icon( FKeyNo5, 176 );	//	Fkey_DISPN( FKeyNo5, " % ");
					Fkey_Icon(FKeyNo6, 6 ); //Fkey_DISPN( FKeyNo6," \xE6\x9E ");
					break;
			}
			break;
		case CMD_MENU_STAT:
			switch ( CommandPage ) {
				case 0:
					Fkey_Icon( FKeyNo1, 759 );	//	Fkey_dispR( FKeyNo1, "DRAW");
					Fkey_Icon( FKeyNo2,   1 );	//	Fkey_dispR( FKeyNo2, "GRPH ");
					Fkey_Icon( FKeyNo3, 162 );	//	Fkey_dspRB( FKeyNo3, "List");
					Fkey_Icon( FKeyNo4,  57 );	//	Fkey_dispR( FKeyNo4, "TYPE");
					FkeyClear( FKeyNo5 );	// DIST
					FkeyClear( FKeyNo6 );	// CALC
					break;
			}
			break;
		case CMD_MENU_STAT_DRAW:
			switch ( CommandPage ) {
				case 0:
					Fkey_Icon( FKeyNo1, 480 );	//	Fkey_dspRB( FKeyNo1, "On ");
					Fkey_Icon( FKeyNo2, 422 );	//	Fkey_dspRB( FKeyNo2, "Off");
					FkeyClear( FKeyNo3 );
					FkeyClear( FKeyNo4 );
					FkeyClear( FKeyNo5 );
					FkeyClear( FKeyNo6 );
					break;
			}
			break;
		case CMD_MENU_STAT_GRPH:
			switch ( CommandPage ) {
				case 0:
					Fkey_Icon( FKeyNo1, 423 );	//	Fkey_dspRB( FKeyNo1, "GPH1");
					Fkey_Icon( FKeyNo2, 424 );	//	Fkey_dspRB( FKeyNo2, "GPH2");
					Fkey_Icon( FKeyNo3, 425 );	//	Fkey_dspRB( FKeyNo3, "GPH3");
					Fkey_Icon( FKeyNo4, 426 );	//	Fkey_dspRB( FKeyNo4, "Scat");
					Fkey_Icon( FKeyNo5, 427 );	//	Fkey_dspRB( FKeyNo5, "xy");
					FkeyClear( FKeyNo6 );
					break;
			}
			break;
		case CMD_MENU_STAT_TYPE:
			switch ( CommandPage ) {
				case 0:
					Fkey_Icon( FKeyNo1, 449 );	//	Fkey_dspRB( FKeyNo1, " \xE6\xA5 ");	// Square
					Fkey_Icon( FKeyNo2, 450 );	//	Fkey_dspRB( FKeyNo2, " \xA9 ");		// Cross
					Fkey_Icon( FKeyNo3, 451 );	//	Fkey_dspRB( FKeyNo3, " \xE6\xAA ");	// Dot
					FkeyClear( FKeyNo4 );
					FkeyClear( FKeyNo5 );
					FkeyClear( FKeyNo6 );
					break;
			}
			break;
		case CMD_MENU_MAT:
			switch ( CommandPage ) {
				case 0:
					Fkey_Icon( FKeyNo1, 456 );	//	Fkey_dspRB( FKeyNo1, "Swap");
					Fkey_Icon( FKeyNo2, 457 );	//	Fkey_dspRB( FKeyNo2, "*Rw");
					Fkey_Icon( FKeyNo3, 458 );	//	Fkey_dspRB( FKeyNo3, "*Rw+");
					Fkey_Icon( FKeyNo4, 459 );	//	Fkey_dspRB( FKeyNo4, "Rw+");
					FkeyClear( FKeyNo5 );
					FkeyClear( FKeyNo6 );
					break;
			}
			break;
		case CMD_MENU_LIST:
			switch ( CommandPage ) {
				case 0:
					Fkey_Icon( FKeyNo1, 460 );	//	Fkey_dspRB( FKeyNo1, "SrtA");
					Fkey_Icon( FKeyNo2, 461 );	//	Fkey_dspRB( FKeyNo2, "SrtD");
					FkeyClear( FKeyNo3 );
					FkeyClear( FKeyNo4 );
					FkeyClear( FKeyNo5 );
					FkeyClear( FKeyNo6 );
					break;
			}
			break;
		case CMD_MENU_GRPH:
			switch ( CommandPage ) {
				case 0:
					Fkey_Icon( FKeyNo1,  15 );	//	Fkey_dispR( FKeyNo1, "SELECT");
					FkeyClear( FKeyNo2 );
					Fkey_Icon( FKeyNo3,  57 );	//	Fkey_dispR( FKeyNo3, "TYPE");
					Fkey_Icon( FKeyNo4, 305 );	//	Fkey_dispR( FKeyNo4, "STYLE");
					FkeyClear( FKeyNo5 );
					FkeyClear( FKeyNo6 );
					break;
			}
			break;
		case CMD_MENU_GRPH_SEL:
			switch ( CommandPage ) {
				case 0:
					Fkey_Icon( FKeyNo1, 480 );	//	Fkey_dspRB( FKeyNo1, "On ");
					Fkey_Icon( FKeyNo2, 422 );	//	Fkey_dspRB( FKeyNo2, "Off");
					FkeyClear( FKeyNo3 );
					FkeyClear( FKeyNo4 );
					FkeyClear( FKeyNo5 );
					FkeyClear( FKeyNo6 );
					break;
			}
			break;
		case CMD_MENU_GRPH_TYPE:
			switch ( CommandPage ) {
				case 0:
					Fkey_Icon( FKeyNo1, 472 );	//	Fkey_dspRB( FKeyNo1, "Y=");
					Fkey_Icon( FKeyNo2, 473 );	//	Fkey_dspRB( FKeyNo2, "r=");
					Fkey_Icon( FKeyNo3, 474 );	//	Fkey_dspRB( FKeyNo3, "Param");
					Fkey_Icon( FKeyNo4, 468 );	//	Fkey_dspRB( FKeyNo4, "X=");
					FkeyClear( FKeyNo5 );
					Fkey_Icon(FKeyNo6, 6 ); //Fkey_DISPN( FKeyNo6," \xE6\x9E ");
					break;
				case 1:
					Fkey_Icon( FKeyNo1, 476 );	//	Fkey_dspRB( FKeyNo1, "Y>");
					Fkey_Icon( FKeyNo2, 477 );	//	Fkey_dspRB( FKeyNo2, "Y<");
					Fkey_Icon( FKeyNo3, 478 );	//	Fkey_dspRB( FKeyNo3, "Y>=");
					Fkey_Icon( FKeyNo4, 479 );	//	Fkey_dspRB( FKeyNo4, "Y<=");
					FkeyClear( FKeyNo5 );
					Fkey_Icon(FKeyNo6, 6 ); //Fkey_DISPN( FKeyNo6," \xE6\x9E ");
					break;
				case 2:
					Fkey_Icon( FKeyNo1, 469 );	//	Fkey_dspRB( FKeyNo1, "X>");
					Fkey_Icon( FKeyNo2, 470 );	//	Fkey_dspRB( FKeyNo2, "X<");
					Fkey_Icon( FKeyNo3, 481 );	//	Fkey_dspRB( FKeyNo3, "X>=");
					Fkey_Icon( FKeyNo4, 482 );	//	Fkey_dspRB( FKeyNo4, "X<=");
					FkeyClear( FKeyNo5 );
					Fkey_Icon(FKeyNo6, 6 ); //Fkey_DISPN( FKeyNo6," \xE6\x9E ");
					break;
			}
			break;		
		case CMD_MENU_TABL:
			switch ( CommandPage ) {
				case 0:
					Fkey_Icon( FKeyNo1, 480 );	//	Fkey_dspRB( FKeyNo1, "On ");
					Fkey_Icon( FKeyNo2, 422 );	//	Fkey_dspRB( FKeyNo2, "Off");
					Fkey_Icon( FKeyNo3,  57 );	//	Fkey_dispR( FKeyNo3, "TYPE");
					Fkey_Icon( FKeyNo4, 305 );	//	Fkey_dispR( FKeyNo4, "STYLE");
					FkeyClear( FKeyNo5 );
					FkeyClear( FKeyNo6 );
					break;
			}
			break;
		case CMD_MENU_TABL_TYPE:
			switch ( CommandPage ) {
				case 0:
					Fkey_Icon( FKeyNo1, 472 );	//	Fkey_dspRB( FKeyNo1, "Y=");
					Fkey_Icon( FKeyNo2, 473 );	//	Fkey_dspRB( FKeyNo2, "r=");
					Fkey_Icon( FKeyNo3, 474 );	//	Fkey_dspRB( FKeyNo3, "Param");
					FkeyClear( FKeyNo4 );
					FkeyClear( FKeyNo5 );
					FkeyClear( FKeyNo6 );
					break;
			}
			break;
		case CMD_MENU_GRPH_STYL:
		case CMD_MENU_TABL_STYL:
			switch ( CommandPage ) {
				case 0:
					FkeyS_L_();
					break;
			}
			break;
		case CMD_MENU_EXT:
			switch ( CommandPage ) {
				case 0:
					Menu_CMD_MENU_EXT();
					break;
			}
			break;

		case CMD_SETUP:		//	-----------------------------------------------------------SETUP
			switch ( CommandPage ) {
				case 0:
					Fkey_Icon( FKeyNo1, 145 );	//	Fkey_dispR( FKeyNo1, "ANGL");
					Fkey_Icon( FKeyNo2, 754 );	//	Fkey_dispR( FKeyNo2, "COOR");
					Fkey_Icon( FKeyNo3, 755 );	//	Fkey_dispR( FKeyNo3, "GRID");
					Fkey_Icon( FKeyNo4, 756 );	//	Fkey_dispR( FKeyNo4, "AXES");
					Fkey_Icon( FKeyNo5, 757 );	//	Fkey_dispR( FKeyNo5, "LABL");
					Fkey_Icon(FKeyNo6, 6 ); //Fkey_DISPN( FKeyNo6," \xE6\x9E ");
					break;
				case 1:
					Fkey_Icon( FKeyNo1, 512 );	//	Fkey_dispR( FKeyNo1, "DISP");
					Fkey_Icon( FKeyNo2, 758 );	//	Fkey_dispR( FKeyNo2, "S/L");
					Fkey_Icon( FKeyNo3, 759 );	//	Fkey_dispR( FKeyNo3, "DRAW");
					Fkey_Icon( FKeyNo4, 760 );	//	Fkey_dispR( FKeyNo4, "DERV");
					Fkey_Icon( FKeyNo5, 689 );	//	Fkey_dispR( FKeyNo5, "BACK");
					Fkey_Icon(FKeyNo6, 6 ); //Fkey_DISPN( FKeyNo6," \xE6\x9E ");
					break;
				case 2:
					Fkey_Icon( FKeyNo1, 761 );	//	Fkey_dispR( FKeyNo1, "FUNC");
					Fkey_Icon( FKeyNo2, 180 );	//	Fkey_dispR( FKeyNo2, "CPLX");
					Fkey_Icon( FKeyNo3, 762 );	//	Fkey_dispR( FKeyNo3, "SWin");
					Fkey_Icon( FKeyNo4, 178 );	//	Fkey_dispR( FKeyNo3, "LIST");
					FkeyClear( FKeyNo5 );
					Fkey_Icon(FKeyNo6, 6 ); //Fkey_DISPN( FKeyNo6," \xE6\x9E ");
					break;
				case 3:
					Fkey_dspRB( FKeyNo1, "RfCtl");
					Fkey_dspRB( FKeyNo2, "RfTm");
					Fkey_dspRB( FKeyNo3, "DATE");
					Fkey_dspRB( FKeyNo4, "TIME");
					Fkey_dspRB( FKeyNo5, "Wait");
					Fkey_Icon(FKeyNo6, 6 ); //Fkey_DISPN( FKeyNo6," \xE6\x9E ");
					break;
				case 4:
					FkeyClear( FKeyNo1 );
					FkeyClear( FKeyNo2 );
					FkeyClear( FKeyNo3 );
					Fkey_dspRB( FKeyNo4, "System(");
					Fkey_dspRB( FKeyNo5, "Version");
					Fkey_Icon(FKeyNo6, 6 ); //Fkey_DISPN( FKeyNo6," \xE6\x9E ");
					break;
			}
			break;
		case CMD_SETUP_ANGL:
			switch ( CommandPage ) {
				case 0:
					Fkey_Icon( FKeyNo1, 489 );	//	Fkey_dspRB( FKeyNo1, "Deg ");
					Fkey_Icon( FKeyNo2, 490 );	//	Fkey_dspRB( FKeyNo2, "Rad ");
					Fkey_Icon( FKeyNo3, 491 );	//	Fkey_dspRB( FKeyNo3, "Gra ");
					FkeyClear( FKeyNo4 );
					FkeyClear( FKeyNo5 );
					FkeyClear( FKeyNo6 );
					break;
			}
			break;
		case CMD_SETUP_COOR:
		case CMD_SETUP_GRID:
		case CMD_SETUP_AXES:
		case CMD_SETUP_LABL:
		case CMD_SETUP_DERV:
		case CMD_SETUP_FUNC:
			switch ( CommandPage ) {
				case 0:
					Fkey_Icon( FKeyNo1, 480 );	//	Fkey_dspRB( FKeyNo1, "On ");
					Fkey_Icon( FKeyNo2, 422 );	//	Fkey_dspRB( FKeyNo2, "Off");
					FkeyClear( FKeyNo3 );
					FkeyClear( FKeyNo4 );
					FkeyClear( FKeyNo5 );
					FkeyClear( FKeyNo6 );
					break;
			}
			break;
		case CMD_SETUP_DISP:
			switch ( CommandPage ) {
				case 0:
					Fkey_Icon( FKeyNo1, 485 );	//	Fkey_dspRB( FKeyNo1, "Fix");
					Fkey_Icon( FKeyNo2, 486 );	//	Fkey_dspRB( FKeyNo2, "Sci");
					Fkey_Icon( FKeyNo3, 487 );	//	Fkey_dspRB( FKeyNo3, "Nrm");
					Fkey_Icon( FKeyNo4, 275 );	//	Fkey_dispR( FKeyNo4, "Eng");
					FkeyClear( FKeyNo5 );
					FkeyClear( FKeyNo6 );
					break;
			}
			break;
		case CMD_SETUP_SL:
			switch ( CommandPage ) {
				case 0:
					FkeyS_L_();
					break;
			}
			break;
		case CMD_SETUP_DISP_ENG:
			switch ( CommandPage ) {
				case 0:
					Fkey_Icon( FKeyNo1, 480 );	//	Fkey_dspRB( FKeyNo1, "On ");
					Fkey_Icon( FKeyNo2, 422 );	//	Fkey_dspRB( FKeyNo2, "Off");
					Fkey_Icon( FKeyNo3, 488 );	//	Fkey_dspRB( FKeyNo3, "Eng");
					FkeyClear( FKeyNo4 );
					FkeyClear( FKeyNo5 );
					FkeyClear( FKeyNo6 );
					break;
			}
			break;
		case CMD_SETUP_DRAW:
			switch ( CommandPage ) {
				case 0:
					Fkey_Icon( FKeyNo1, 496 );	//	Fkey_dspRB( FKeyNo1, "Con");
					Fkey_Icon( FKeyNo2, 497 );	//	Fkey_dspRB( FKeyNo2, "Plot");
					FkeyClear( FKeyNo3 );
					FkeyClear( FKeyNo4 );
					FkeyClear( FKeyNo5 );
					FkeyClear( FKeyNo6 );
					break;
			}
			break;
		case CMD_SETUP_BACK:
			switch ( CommandPage ) {
				case 0:
					Fkey_Icon( FKeyNo1, 494 );	//	Fkey_dspRB( FKeyNo1, "None");
					Fkey_Icon( FKeyNo2, 495 );	//	Fkey_dspRB( FKeyNo2, "Pict");
					FkeyClear( FKeyNo3 );
					FkeyClear( FKeyNo4 );
					FkeyClear( FKeyNo5 );
					FkeyClear( FKeyNo6 );
					break;
			}
			break;
		case CMD_SETUP_SWIN:
			switch ( CommandPage ) {
				case 0:
					Fkey_Icon( FKeyNo1, 492 );	//	Fkey_dspRB( FKeyNo1, "Auto");
					Fkey_Icon( FKeyNo2, 493 );	//	Fkey_dspRB( FKeyNo2, "Man");
					FkeyClear( FKeyNo3 );
					FkeyClear( FKeyNo4 );
					FkeyClear( FKeyNo5 );
					FkeyClear( FKeyNo6 );
					break;
			}
			break;
		case CMD_SETUP_CPLX:
			switch ( CommandPage ) {
				case 0:
					Fkey_Icon( FKeyNo1, 277 );	//	Fkey_dspRB( FKeyNo1, "Real");
					Fkey_Icon( FKeyNo2, 278 );	//	Fkey_dspRB( FKeyNo2, "a+bi");
					Fkey_Icon( FKeyNo3, 279 );	//	Fkey_dspRB( FKeyNo3, "r+theta");
					FkeyClear( FKeyNo4 );
					FkeyClear( FKeyNo5 );
					FkeyClear( FKeyNo6 );
					break;
			}
			break;
		case CMD_SETUP_LIST:
			switch ( CommandPage ) {
				case 0:
					Fkey_Icon( FKeyNo1,  43 );	//	Fkey_dspRB( FKeyNo1, "File");
					FkeyClear( FKeyNo2 );
					FkeyClear( FKeyNo3 );
					FkeyClear( FKeyNo4 );
					FkeyClear( FKeyNo5 );
					FkeyClear( FKeyNo6 );
					break;
			}
			break;

		default:
			break;
	}
}

