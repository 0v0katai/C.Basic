/*****************************************************************/
/*                                                               */
/*   Genuine Command input Library  ver 1.0                      */
/*                                                               */
/*   written by sentaro21                                        */
/*                                                               */
/*****************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <fxlib.h>
#include "fx_syscall.h"
#include "CB_inp.h"
#include "CB_io.h"
#include "CB_Kana.h"

//----------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------
int  CommandType=0;
char CommandPage=0;

void SHIFT_MENU(){
					Fkey_Clear( 0 );
					Fkey_Clear( 1 );
					Fkey_dispR( 2, "VWIN");
					Fkey_dispR( 3, "SKTCH");
					Fkey_Clear( 4 );
					Fkey_Clear( 5 );
}

void DispGenuineCmdMenu(){
	switch ( CommandType ) {
		case CMD_PRGM:		//	------------------------------------------------------------PRGM
			switch ( CommandPage ) {
				case 0:
					Fkey_dispR( 0, "COM");
					Fkey_dispR( 1, "CTL");
					Fkey_dispR( 2, "JUMP");
					Fkey_DISPR( 3, " ? ");
					Fkey_DISPR( 4, " \x0C ");
					Fkey_DISPN( 5," \xE6\x9E ");
					break;
				case 1:
					Fkey_dispR( 0, "CLR ");
					Fkey_dispR( 1, "DISP");
					Fkey_dispR( 2, "REL ");
					Fkey_dispR( 3, "I/O ");
					Fkey_DISPR( 4, " :");
					Fkey_DISPN( 5," \xE6\x9E ");
					break;
				case 2:
					Fkey_dispR( 0, "STR ");
					Fkey_DISPR( 1, " $ ");		// $
					Fkey_Clear( 2 );
					Fkey_dspRB( 3, "Alias");
					Fkey_dispR( 4, "EXEC");
					Fkey_DISPN( 5," \xE6\x9E ");
					break;
			}
			break;
		case CMD_PRGM_COM:
			switch ( CommandPage ) {
				case 0:
					Fkey_dspRB( 0, "If ");
					Fkey_dspRB( 1, "Then");
					Fkey_dspRB( 2, "Else");
					Fkey_dspRB( 3, "IEnd");
					Fkey_dspRB( 4, "E.If");
					Fkey_DISPN( 5," \xE6\x9E ");
					break;
				case 1:
					Fkey_dspRB( 0, "For ");
					Fkey_dspRB( 1, " To ");
					Fkey_dspRB( 2, "Step");
					Fkey_dspRB( 3, "Next");
					Fkey_Clear( 4 );
					Fkey_DISPN( 5," \xE6\x9E ");
					break;
				case 2:
					Fkey_dspRB( 0, "Whle");
					Fkey_dspRB( 1, "WEnd");
					Fkey_dspRB( 2, " Do");
					Fkey_dspRB( 3, "Lp.W");
					Fkey_Clear( 4 );
					Fkey_DISPN( 5," \xE6\x9E ");
					break;
				case 3:
					Fkey_dspRB( 0, "Swit");
					Fkey_dspRB( 1, "Case");
					Fkey_dspRB( 2, "Deft");
					Fkey_dspRB( 3, "Brk");
					Fkey_dspRB( 4, "S.End");
					Fkey_DISPN( 5," \xE6\x9E ");
					break;
			}
			break;
		case CMD_PRGM_CTL:
			switch ( CommandPage ) {
				case 0:
					Fkey_dspRB( 0, "Prog");
					Fkey_dspRB( 1, "Rtrn");
					Fkey_dspRB( 2, "Brk");
					Fkey_dspRB( 3, "Stop");
					Fkey_dspRB( 4, "Locl");
					Fkey_DISPN( 5," \xE6\x9E ");
					break;
				case 1:
					Fkey_dspRB( 0, "Gsub");
					Fkey_dspRB( 1, "ACB");
					Fkey_Clear( 2 );
					Fkey_Clear( 3 );
					Fkey_Clear( 4 );
					Fkey_DISPN( 5," \xE6\x9E ");
					break;
			}
			break;
		case CMD_PRGM_JUMP:
			switch ( CommandPage ) {
				case 0:
					Fkey_dspRB( 0, "Lbl");
					Fkey_dspRB( 1, "Goto");
					Fkey_DISPR( 2, " \x13 ");
					Fkey_dspRB( 3, "ISZ");
					Fkey_dspRB( 4, "DSZ");
					Fkey_dspRB( 5, "Menu");
					break;
			}
			break;
		case CMD_PRGM_CLR:
			switch ( CommandPage ) {
				case 0:
					Fkey_dspRB( 0, "Text");		// ClrText
					Fkey_dspRB( 1, "Grph");		// ClrGraph
					Fkey_dspRB( 2, "List");		// ClrList
					Fkey_dspRB( 3, "Mat ");		// ClrMat
					Fkey_Clear( 4 );
					Fkey_Clear( 5 );
					break;
			}
			break;
		case CMD_PRGM_DISP:
			switch ( CommandPage ) {
				case 0:
					Fkey_dspRB( 0, "Stat");		// DrawStat
					Fkey_dspRB( 1, "Grph");		// DrawGraph
					Fkey_Clear( 2 );
					Fkey_Clear( 3 );
					Fkey_Clear( 4 );
					Fkey_dspRB( 5, "Disp");		// Disp
					break;
			}
			break;
		case CMD_PRGM_REL:
			switch ( CommandPage ) {
				case 0:
					Fkey_DISPR( 0, " = ");
					Fkey_DISPR( 1, " \x11 ");
					Fkey_DISPR( 2, " > ");
					Fkey_DISPR( 3, " < ");
					Fkey_DISPR( 4, " \x12 ");
					Fkey_DISPR( 5, " \x10 ");
					break;
			}
			break;
		case CMD_PRGM_I_O:
			switch ( CommandPage ) {
				case 0:
					Fkey_dspRB( 0, "Lcte");
					Fkey_dspRB( 1, "Gtky");
					Fkey_Clear( 2 );
					Fkey_Clear( 3 );
					Fkey_dspRB( 4, "KeyR");
					Fkey_DISPN( 5," \xE6\x9E ");
					break;
				case 1:
					Fkey_dspRB( 0, "IsExs");
					Fkey_dspRB( 1, "Save");
					Fkey_dspRB( 2, "Load");
					Fkey_dspRB( 3, "Delet");
					Fkey_Clear( 4 );
					Fkey_DISPN( 5," \xE6\x9E ");
					break;
			}
			break;
		case CMD_PRGM_STR:
			switch ( CommandPage ) {
				case 0:
					Fkey_dspRB( 0, "Join");
					Fkey_dspRB( 1, "Len");
					Fkey_dspRB( 2, "Cmp");
					Fkey_dspRB( 3, "Src");
					Fkey_Clear( 4 );
					Fkey_DISPN( 5," \xE6\x9E ");
					break;
				case 1:
					Fkey_dspRB( 0, "Left");
					Fkey_dspRB( 1, "Right");
					Fkey_dspRB( 2, "Mid");
					Fkey_dspRB( 3, "E\xE6\x9ES");
					Fkey_dspRB( 4, "Exp");
					Fkey_DISPN( 5," \xE6\x9E ");
					break;
				case 2:
					Fkey_dspRB( 0, "Upr");
					Fkey_dspRB( 1, "Lwr");
					Fkey_dspRB( 2, "Inv");
					Fkey_dspRB( 3, "Shift");
					Fkey_dspRB( 4, "Rot");
					Fkey_DISPN( 5," \xE6\x9E ");
					break;
				case 3:
					Fkey_dspRB( 0, "Str");		// Str
					Fkey_dspRB( 1, "Str(");		// Str(
					Fkey_dspRB( 2, "Sptf");		// Sprintf
					Fkey_DISPR( 3, " % ");
					Fkey_DISPR( 4, " # ");
					Fkey_DISPN( 5," \xE6\x9E ");
					break;
			}
			break;
		case CMD_PRGM_EXEC:
			switch ( CommandPage ) {
				case 0:
					Fkey_dspRB( 0, "SysCl");
					Fkey_dspRB( 1, "Call");
					Fkey_dspRB( 2, "Poke");
					Fkey_dspRB( 3, "Peek");
					Fkey_Clear( 4 );
					Fkey_DISPN( 5," \xE6\x9E ");
					break;
				case 1:
					Fkey_dspRB( 0, "VarPt");
					Fkey_dspRB( 1, "PrgPt");
					Fkey_DISPR( 2, " *");
					Fkey_DISPR( 3, " &");
					Fkey_Clear( 4 );
					Fkey_DISPN( 5," \xE6\x9E ");
					break;
			}
			break;
			
		case CMD_OPTN:		//	------------------------------------------------------------OPTN
			switch ( CommandPage ) {
				case 0:
					Fkey_dispR( 0, "LIST");
					Fkey_dispR( 1, "MAT ");
					Fkey_dispN( 2, "Extd");
					Fkey_dispR( 3, "CALC");
					Fkey_Clear( 4 );
					Fkey_DISPN( 5," \xE6\x9E ");
					break;
				case 1:
					Fkey_Clear( 0 );
					Fkey_dispR( 1, "HYP");
					Fkey_dispR( 2, "PROB");
					Fkey_dispR( 3, "NUM");
					Fkey_dispR( 4, "ANGL");
					Fkey_DISPN( 5," \xE6\x9E ");
					break;
				case 2:
					Fkey_dispR( 0, "ESYM");
					Fkey_dispR( 1, "PICT");
					Fkey_dispR( 2, "fMEM");
					Fkey_dispR( 3, "LOGIC");
					Fkey_Clear( 4 );
					Fkey_DISPN( 5," \xE6\x9E ");
					break;
			}
			break;
		case CMD_OPTN_LIST:
			switch ( CommandPage ) {
				case 0:
					Fkey_dspRB( 0, "LIST");
					Fkey_dspRB( 1, "L\xE6\x91M");
					Fkey_dspRB( 2, "Dim ");
					Fkey_dspRB( 3, "Fill");
					Fkey_dspRB( 4, "Seq ");
					Fkey_DISPN( 5," \xE6\x9E ");
					break;
				case 1:
					Fkey_dspRB( 0, "Min ");
					Fkey_dspRB( 1, "Max ");
					Fkey_dspRB( 2, "Mean");
					Fkey_Clear( 3 );
					Fkey_dspRB( 4, "Augm");
					Fkey_DISPN( 5," \xE6\x9E ");
					break;
				case 2:
					Fkey_dspRB( 0, "Sum ");
					Fkey_dspRB( 1, "Prod");
					Fkey_Clear( 2 );
					Fkey_Clear( 3 );
					Fkey_Clear( 4 );
					Fkey_DISPN( 5," \xE6\x9E ");
					break;
			}
			break;
		case CMD_OPTN_MAT:
			switch ( CommandPage ) {
				case 0:
					Fkey_dspRB( 0, "Mat");
					Fkey_dspRB( 1, "M\xE6\x91L");
					Fkey_Clear( 2 );
					Fkey_dspRB( 3, "Trn ");
					Fkey_dspRB( 4, "Aug ");
					Fkey_DISPN( 5," \xE6\x9E ");
					break;
				case 1:
					Fkey_Clear( 0 );
					Fkey_dspRB( 1, "Dim ");
					Fkey_dspRB( 2, "Fill");
					Fkey_Clear( 3 );
					Fkey_dispR( 4, "Size");
					Fkey_DISPN( 5," \xE6\x9E ");
					break;
			}
			break;
		case CMD_OPTN_MAT_SIZE:
			switch ( CommandPage ) {
				case 0:
					Fkey_dspRB( 0, "Base");
					Fkey_dspRB( 1, "Elem");
					Fkey_dspRB( 2, "Col");
					Fkey_dspRB( 3, "Row");
					Fkey_Clear( 4 );
					Fkey_Clear( 5 );
					break;
			}
			break;
		case CMD_OPTN_CALC:
			switch ( CommandPage ) {
				case 1:
					Fkey_Clear( 0 );
					Fkey_Clear( 1 );
					Fkey_dspRB( 2, " \xE5\x51( ");
					Fkey_dspRB( 3, "logab");
					Fkey_Clear( 4 );
					Fkey_DISPN( 5," \xE6\x9E ");
					break;
				case 2:
					Fkey_dspRB( 0, "Int\xB9");
					Fkey_dspRB( 1, "Rmdr");
					Fkey_Clear( 2 );
					Fkey_Clear( 3 );
					Fkey_Clear( 4 );
					Fkey_DISPN( 5," \xE6\x9E ");
					break;
			}
			break;
		case CMD_OPTN_HYP:
			switch ( CommandPage ) {
				case 0:
					Fkey_dspRB( 0, "sinh");
					Fkey_dspRB( 1, "cosh");
					Fkey_dspRB( 2, "tanh");
					Fkey_dspRB( 3, "sih\xE5\xCA");
					Fkey_dspRB( 4, "coh\xE5\xCA");
					Fkey_dspRB( 5, "tah\xE5\xCA");
					break;
			}
			break;
		case CMD_OPTN_PROB:
			switch ( CommandPage ) {
				case 0:
					Fkey_dspRB( 0, " X !");
					Fkey_Clear( 1 );
					Fkey_Clear( 2 );
					Fkey_dispR( 3, "RAND");
					Fkey_Clear( 4 );
					Fkey_Clear( 5 );
					break;
			}
			break;
		case CMD_OPTN_PROB_RAND:
			switch ( CommandPage ) {
				case 0:
					Fkey_dspRB( 0, "Ran#");
					Fkey_dspRB( 1, "Int ");
					Fkey_Clear( 2 );
					Fkey_Clear( 3 );
					Fkey_dspRB( 4, "List");
					Fkey_Clear( 5 );
					break;
			}
			break;
		case CMD_OPTN_NUM:
			switch ( CommandPage ) {
				case 0:
					Fkey_dspRB( 0, "Abs ");
					Fkey_dspRB( 1, "int ");
					Fkey_dspRB( 2, "Frac");
					Fkey_dspRB( 3, "Rnd ");
					Fkey_dspRB( 4, "Intg");
					Fkey_DISPN( 5," \xE6\x9E ");
					break;
				case 1:
					Fkey_dspRB( 0, "RndFi");
					Fkey_Clear( 1 );
					Fkey_Clear( 2 );
					Fkey_dspRB( 3, "MOD ");
					Fkey_Clear( 4 );
					Fkey_DISPN( 5," \xE6\x9E ");
					break;
			}
			break;
		case CMD_OPTN_ANGL:
			switch ( CommandPage ) {
				case 0:
					Fkey_DISPR( 0, " \x9C ");
					Fkey_DISPR( 1, " \xAC ");
					Fkey_DISPR( 2, " \xBC ");
					Fkey_dspRB( 3, "\x9C,,,");
					Fkey_Clear( 4 );
					Fkey_DISPN( 5," \xE6\x9E ");
					break;
				case 1:
					Fkey_dspRB( 0, "Pol(");
					Fkey_dspRB( 1, "Rec(");
					Fkey_dspRB( 2, "\xE6\x9E.DMS");
					Fkey_Clear( 3 );
					Fkey_Clear( 4 );
					Fkey_DISPN( 5," \xE6\x9E ");
					break;
			}
			break;
		case CMD_OPTN_ESYM:
			switch ( CommandPage ) {
				case 0:
					Fkey_DISPR( 0, " m ");
					Fkey_DISPR( 1, " \xE6\x4B ");
					Fkey_DISPR( 2, " \x03 ");
					Fkey_DISPR( 3, " p ");
					Fkey_DISPR( 4, " \x01 ");
					Fkey_DISPN( 5," \xE6\x9E ");
					break;
				case 1:
					Fkey_DISPR( 0, " K ");
					Fkey_DISPR( 1, " \x07 ");
					Fkey_DISPR( 2, " \x08 ");
					Fkey_DISPR( 3, " \x09 ");
					Fkey_DISPR( 4, " \x0A ");
					Fkey_DISPN( 5," \xE6\x9E ");
					break;
				case 2:
					Fkey_DISPR( 0, " \x0B ");
					Fkey_Clear( 1 );
					Fkey_Clear( 2 );
					Fkey_Clear( 3 );
					Fkey_Clear( 4 );
					Fkey_DISPN( 5," \xE6\x9E ");
					break;
			}
			break;
		case CMD_OPTN_PICT:
			switch ( CommandPage ) {
				case 0:
					Fkey_dspRB( 0, "Sto ");
					Fkey_dspRB( 1, "Rcl ");
					Fkey_Clear( 2 );
					Fkey_Clear( 3 );
					Fkey_Clear( 4 );
					Fkey_Clear( 5 );
					break;
			}
			break;
		case CMD_OPTN_FMEM:
			switch ( CommandPage ) {
				case 0:
					Fkey_Clear( 0 );
					Fkey_Clear( 1 );
					Fkey_dspRB( 2, " fn ");
					Fkey_Clear( 3 );
					Fkey_Clear( 4 );
					Fkey_Clear( 5 );
					break;
			}
			break;
		case CMD_OPTN_LOGIC:
			switch ( CommandPage ) {
				case 0:
					Fkey_dspRB( 0, "And");
					Fkey_dspRB( 1, " Or");
					Fkey_dspRB( 2, "Not");
					Fkey_dspRB( 3, "Xor");
					Fkey_Clear( 4 );
					Fkey_DISPN( 5," \xE6\x9E ");
					break;
				case 1:
					Fkey_dspRB( 0, "and");
					Fkey_dspRB( 1, " or");
					Fkey_dspRB( 2, "not");
					Fkey_dspRB( 3, "xor");
					Fkey_Clear( 4 );
					Fkey_DISPN( 5," \xE6\x9E ");
					break;
			}
			break;
		case CMD_OPTN_EXT:
			switch ( CommandPage ) {
				case 0:
					Fkey_dspRB( 0, "Alias");
					Fkey_DISPR( 1, " ~ ");
					Fkey_DISPR( 2, " & ");
					Fkey_DISPR( 3, " | ");
					Fkey_DISPR( 4, " # ");
					Fkey_DISPR( 5, " % ");
					break;
			}
			break;
			
		case CMD_VARS:		//	------------------------------------------------------------VARS
			switch ( CommandPage ) {
				case 0:
					Fkey_dispR( 0, "VWin");
					Fkey_dispR( 1, "FACT");
					Fkey_dispN( 2, "Extd");
					Fkey_dispR( 3, "GRPH");
					Fkey_dispN( 4, "Setup");
					Fkey_DISPN( 5," \xE6\x9E ");
					break;
				case 1:
					Fkey_Clear( 0 );
					Fkey_Clear( 1 );
					Fkey_Clear( 2 );
					Fkey_DISPR( 3, " $ ");
					Fkey_dspRB( 4, "Str ");
					Fkey_DISPN( 5," \xE6\x9E ");
					break;
			}
			break;
		case CMD_VARS_VWIN:
			switch ( CommandPage ) {
				case 0:
					Fkey_DISPR( 0, " X ");
					Fkey_DISPR( 1, " Y ");
					Fkey_DISPR( 2, "T,\xE6\x47");
					Fkey_Clear( 3 );
					Fkey_Clear( 4 );
					Fkey_Clear( 5 );
					break;
			}
			break;
		case CMD_VARS_VWIN_X:
			switch ( CommandPage ) {
				case 0:
					Fkey_dspRB( 0, "min");
					Fkey_dspRB( 1, "max");
					Fkey_dspRB( 2, "scal");
					Fkey_dspRB( 3, "dot" );
					Fkey_Clear( 4 );
					Fkey_Clear( 5 );
					break;
			}
			break;
		case CMD_VARS_VWIN_Y:
			switch ( CommandPage ) {
				case 0:
					Fkey_dspRB( 0, "min");
					Fkey_dspRB( 1, "max");
					Fkey_dspRB( 2, "scal");
					Fkey_Clear( 3 );
					Fkey_Clear( 4 );
					Fkey_Clear( 5 );
					break;
			}
			break;
		case CMD_VARS_VWIN_T:
			switch ( CommandPage ) {
				case 0:
					Fkey_dspRB( 0, "min");
					Fkey_dspRB( 1, "max");
					Fkey_dspRB( 2, "ptch");
					Fkey_Clear( 3 );
					Fkey_Clear( 4 );
					Fkey_Clear( 5 );
					break;
			}
			break;
		case CMD_VARS_FACT:
			switch ( CommandPage ) {
				case 0:
					Fkey_dspRB( 0, "Xfct");
					Fkey_dspRB( 1, "Yfct");
					Fkey_Clear( 2 );
					Fkey_Clear( 3 );
					Fkey_Clear( 4 );
					Fkey_Clear( 5 );
					break;
			}
			break;
		case CMD_VARS_GRPH:
			switch ( CommandPage ) {
				case 0:
					Fkey_DISPR( 0, " \xE5\xB4 ");
					Fkey_Clear( 1 );
					Fkey_Clear( 2 );
					Fkey_Clear( 3 );
					Fkey_Clear( 4 );
					Fkey_Clear( 5 );
					break;
			}
			break;
		case CMD_VARS_EXT:
			switch ( CommandPage ) {
				case 0:
					Fkey_dspRB( 0, "Ticks");
					Fkey_dspRB( 1, "DATE");
					Fkey_dspRB( 2, "TIME");
					Fkey_dspRB( 3, "BackL");
					Fkey_dspRB( 4, "RfCtl");
					Fkey_dspRB( 5, "RfTm");
					break;
			}
			break;

		case CMD_SHIFT:		//	-----------------------------------------------------------SHIFT
			switch ( CommandPage ) {
				case 0:
					SHIFT_MENU();
					break;
			}
			break;
		case CMD_SHIFT_VWIN:
			switch ( CommandPage ) {
				case 0:
					Fkey_dspRB( 0, "VWin");
					Fkey_dspRB( 1, "Sto");
					Fkey_dspRB( 2, "Rcl");
					Fkey_Clear( 3 );
					Fkey_Clear( 4 );
					Fkey_Clear( 5 );
					break;
			}
			break;
		case CMD_SHIFT_SKTCH:
			switch ( CommandPage ) {
				case 0:
					Fkey_dspRB( 0, "Cls");
					Fkey_dispN( 1, "Extd");
					Fkey_dispN( 2, " ML ");
					Fkey_Clear( 3 );
					Fkey_dispR( 4, "GRPH");
					Fkey_DISPN( 5," \xE6\x9E ");
					break;
				case 1:
					Fkey_dispR( 0, "PLOT");
					Fkey_dispR( 1, "LINE");
					Fkey_dspRB( 2, "Crcl");
					Fkey_dspRB( 3, "Vert");
					Fkey_dspRB( 4, "Hztl");
					Fkey_DISPN( 5," \xE6\x9E ");
					break;
				case 2:
					Fkey_Clear( 0 );
					Fkey_dspRB( 1, "Text");
					Fkey_dispR( 2, "PIXL");
					Fkey_dspRB( 3, "Test");
					Fkey_dispR( 4, "STYL");
					Fkey_DISPN( 5," \xE6\x9E ");
					break;
			}
			break;
		case CMD_SHIFT_SKTCH_GRPH:
			switch ( CommandPage ) {
				case 0:
					Fkey_dspRB( 0, "Y= ");
					Fkey_Clear( 1 );
					Fkey_dspRB( 2, "Parm");
					Fkey_Clear( 3 );
					Fkey_Clear( 4 );
					Fkey_Clear( 5 );
					break;
			}
			break;
		case CMD_SHIFT_SKTCH_PLOT:
			switch ( CommandPage ) {
				case 0:
					Fkey_dspRB( 0, "Plot");
					Fkey_dspRB( 1, "PlOn");
					Fkey_dspRB( 2, "PlOff");
					Fkey_dspRB( 3, "PlChg");
					Fkey_Clear( 4 );
					Fkey_Clear( 5 );
					break;
			}
			break;
		case CMD_SHIFT_SKTCH_LINE:
			switch ( CommandPage ) {
				case 0:
					Fkey_dspRB( 0, "Line");
					Fkey_dspRB( 1, "FLine");
					Fkey_Clear( 2 );
					Fkey_Clear( 3 );
					Fkey_Clear( 4 );
					Fkey_Clear( 5 );
					break;
			}
			break;
		case CMD_SHIFT_SKTCH_PIXL:
			switch ( CommandPage ) {
				case 0:
					Fkey_dspRB( 0, "On ");
					Fkey_dspRB( 1, "Off");
					Fkey_dspRB( 2, "Chg");
					Fkey_Clear( 3 );
					Fkey_Clear( 4 );
					Fkey_Clear( 5 );
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
					Fkey_dspRB( 0, "Scrn");
					Fkey_dspRB( 1, "DspDD");
					Fkey_dspRB( 2, "LctYX");
					Fkey_dspRB( 3, "Fkey(");
					Fkey_dspRB( 4, "PupW");
					Fkey_DISPN( 5," \xE6\x9E ");
					break;
				case 1:
					Fkey_dspRB( 0, "Rect");
					Fkey_dspRB( 1, "FRect");
					Fkey_dspRB( 2, "RdGR");
					Fkey_dspRB( 3, "WrGR");
					Fkey_Clear( 4 );
					Fkey_DISPN( 5," \xE6\x9E ");
					break;
				case 2:
					Fkey_dspRB( 0, "DGet");
					Fkey_dspRB( 1, "DPut");
					Fkey_dspRB( 2, "Dtrim");
					Fkey_dspRB( 3, "Dlife");
					Fkey_dspRB( 4, "CeSum");
					Fkey_DISPN( 5," \xE6\x9E ");
					break;
			}
			break;
		case CMD_SHIFT_SKTCH_ML:
			switch ( CommandPage ) {
				case 0:
					Fkey_dspRB( 0, "ClVra");
					Fkey_dspRB( 1, "ClScr");
					Fkey_dspRB( 2, "DspVr");
					Fkey_dspRB( 3, "Point");
					Fkey_dspRB( 4, "Pixel");
					Fkey_DISPN( 5," \xE6\x9E ");
					break;
				case 1:
					Fkey_dspRB( 0, "PxTest");
					Fkey_dspRB( 1, "Line");
					Fkey_dspRB( 2, "Rect");
					Fkey_dspRB( 3, "Horiz");
					Fkey_dspRB( 4, "Verti");
					Fkey_DISPN( 5," \xE6\x9E ");
					break;
				case 2:
					Fkey_dspRB( 0, "Poly");
					Fkey_dspRB( 1, "FPoly");
					Fkey_dspRB( 2, "Circl");
					Fkey_dspRB( 3, "FCirc");
					Fkey_Clear( 4 );
					Fkey_DISPN( 5," \xE6\x9E ");
					break;
				case 3:
					Fkey_dspRB( 0, "Elips");
					Fkey_dspRB( 1, "FElip");
					Fkey_dspRB( 2, "EInR");
					Fkey_dspRB( 3, "FEInR");
					Fkey_Clear( 4 );
					Fkey_DISPN( 5," \xE6\x9E ");
					break;
				case 4:
					Fkey_dspRB( 0, "Hscrl");
					Fkey_dspRB( 1, "Vscrl");
					Fkey_dspRB( 2, "Bmp");
					Fkey_dspRB( 3, "Bm8");
					Fkey_dspRB( 4, "Bm16");
					Fkey_DISPN( 5," \xE6\x9E ");
					break;
			}
			break;

		case CMD_MENU:		//	------------------------------------------------------------MENU
			switch ( CommandPage ) {
				case 0:
					Fkey_dispR( 0, "STAT");
					Fkey_dispR( 1, "MAT ");
					Fkey_dispR( 2, "LIST");
					Fkey_dispN( 3, "extc");
					Fkey_DISPN( 4, " % ");
					Fkey_DISPN( 5, " # ");
					break;
			}
			break;
		case CMD_MENU_STAT:
			switch ( CommandPage ) {
				case 0:
					Fkey_dispR( 0, "DRAW");
					Fkey_dispR( 1, "GRPH ");
					Fkey_dspRB( 2, "List");
					Fkey_dispR( 3, "TYPE");
					Fkey_Clear( 4 );	// DIST
					Fkey_Clear( 5 );	// CALC
					break;
			}
			break;
		case CMD_MENU_STAT_DRAW:
			switch ( CommandPage ) {
				case 0:
					Fkey_dspRB( 0, "On ");
					Fkey_dspRB( 1, "Off");
					Fkey_Clear( 2 );
					Fkey_Clear( 3 );
					Fkey_Clear( 4 );
					Fkey_Clear( 5 );
					break;
			}
			break;
		case CMD_MENU_STAT_GRPH:
			switch ( CommandPage ) {
				case 0:
					Fkey_dspRB( 0, "GPH1");
					Fkey_dspRB( 1, "GPH2");
					Fkey_dspRB( 2, "GPH3");
					Fkey_dspRB( 3, "Scat");
					Fkey_dspRB( 4, "xy");
					Fkey_Clear( 5 );
					break;
			}
			break;
		case CMD_MENU_STAT_TYPE:
			switch ( CommandPage ) {
				case 0:
					Fkey_dspRB( 0, " \xE6\xA5 ");	// Square
					Fkey_dspRB( 1, " \xA9 ");		// Cross
					Fkey_dspRB( 2, " \xE6\xAA ");	// Dot
					Fkey_Clear( 3 );
					Fkey_Clear( 4 );
					Fkey_Clear( 5 );
					break;
			}
			break;
		case CMD_MENU_MAT:
			switch ( CommandPage ) {
				case 0:
					Fkey_dspRB( 0, "Swap");
					Fkey_dspRB( 1, "*Rw");
					Fkey_dspRB( 2, "*Rw+");
					Fkey_dspRB( 3, "Rw+");
					Fkey_Clear( 4 );
					Fkey_Clear( 5 );
					break;
			}
			break;
		case CMD_MENU_LIST:
			switch ( CommandPage ) {
				case 0:
					Fkey_dspRB( 0, "SrtA");
					Fkey_dspRB( 1, "SrtD");
					Fkey_Clear( 2 );
					Fkey_Clear( 3 );
					Fkey_Clear( 4 );
					Fkey_Clear( 5 );
					break;
			}
			break;
		case CMD_MENU_EXT:
			switch ( CommandPage ) {
				case 0:
					Fkey_DISPR( 0, " ? ");
					Fkey_DISPR( 1, " \x0C ");
					Fkey_DISPR( 2, " : ");
					Fkey_DISPR( 3, " \x13 ");
					Fkey_DISPR( 4, " ' ");
					Fkey_DISPR( 5, " / ");
					break;
			}
			break;

		case CMD_SETUP:		//	-----------------------------------------------------------SETUP
			switch ( CommandPage ) {
				case 0:
					Fkey_dispR( 0, "ANGL");
					Fkey_dispR( 1, "COOR");
					Fkey_dispR( 2, "GRID");
					Fkey_dispR( 3, "AXES");
					Fkey_dispR( 4, "LABL");
					Fkey_DISPN( 5," \xE6\x9E ");
					break;
				case 1:
					Fkey_dispR( 0, "DISP");
					Fkey_dispR( 1, "S/L");
					Fkey_dispR( 2, "DRAW");
					Fkey_dispR( 3, "DERV");
					Fkey_dispR( 4, "BACK");
					Fkey_DISPN( 5," \xE6\x9E ");
					break;
				case 2:
					Fkey_dispR( 0, "FUNC");
					Fkey_Clear( 1 );
					Fkey_dispR( 2, "SWin");
					Fkey_Clear( 3 );
					Fkey_Clear( 4 );
					Fkey_DISPN( 5," \xE6\x9E ");
					break;
				case 3:
					Fkey_dspRB( 0, "RfCtl");
					Fkey_dspRB( 1, "RfTm");
					Fkey_dspRB( 2, "DATE");
					Fkey_dspRB( 3, "TIME");
					Fkey_dspRB( 4, "Wait");
					Fkey_DISPN( 5," \xE6\x9E ");
					break;
			}
			break;
		case CMD_SETUP_ANGL:
			switch ( CommandPage ) {
				case 0:
					Fkey_dspRB( 0, "Deg ");
					Fkey_dspRB( 1, "Rad ");
					Fkey_dspRB( 2, "Gra ");
					Fkey_Clear( 3 );
					Fkey_Clear( 4 );
					Fkey_Clear( 5 );
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
					Fkey_dspRB( 0, "On ");
					Fkey_dspRB( 1, "Off");
					Fkey_Clear( 2 );
					Fkey_Clear( 3 );
					Fkey_Clear( 4 );
					Fkey_Clear( 5 );
					break;
			}
			break;
		case CMD_SETUP_DISP:
			switch ( CommandPage ) {
				case 0:
					Fkey_dspRB( 0, "Fix");
					Fkey_dspRB( 1, "Sci");
					Fkey_dspRB( 2, "Nrm");
					Fkey_dispR( 3, "Eng");
					Fkey_Clear( 4 );
					Fkey_Clear( 5 );
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
					Fkey_dspRB( 0, "On ");
					Fkey_dspRB( 1, "off");
					Fkey_dspRB( 2, "Eng");
					Fkey_Clear( 3 );
					Fkey_Clear( 4 );
					Fkey_Clear( 5 );
					break;
			}
			break;
		case CMD_SETUP_DRAW:
			switch ( CommandPage ) {
				case 0:
					Fkey_dspRB( 0, "Con");
					Fkey_dspRB( 1, "Plot");
					Fkey_Clear( 2 );
					Fkey_Clear( 3 );
					Fkey_Clear( 4 );
					Fkey_Clear( 5 );
					break;
			}
			break;
		case CMD_SETUP_BACK:
			switch ( CommandPage ) {
				case 0:
					Fkey_dspRB( 0, "None");
					Fkey_dspRB( 1, "Pict");
					Fkey_Clear( 2 );
					Fkey_Clear( 3 );
					Fkey_Clear( 4 );
					Fkey_Clear( 5 );
					break;
			}
			break;
		case CMD_SETUP_SWIN:
			switch ( CommandPage ) {
				case 0:
					Fkey_dspRB( 0, "Auto");
					Fkey_dspRB( 1, "Man");
					Fkey_Clear( 2 );
					Fkey_Clear( 3 );
					Fkey_Clear( 4 );
					Fkey_Clear( 5 );
					break;
			}
			break;

		default:
			break;
	}
}

