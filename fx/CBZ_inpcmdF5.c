#include "CB.h"

//----------------------------------------------------------------------------------------------
void GetGenuineCmdF5( unsigned int *code ){
	switch ( CommandType ) {
		case CMD_PRGM:		//	------------------------------------------------------------PRGM_F5
			switch ( CommandPage ) {
				case 0: (*code)=0x0C;return;	// disps
				case 1: (*code)=0x3A;return;	// :
				case 2: CommandType=CMD_PRGM_EXEC; CommandPage=0;break;
			} break;
		case CMD_PRGM_COM:
			switch ( CommandPage ) {
				case 0: (*code)=0xF70F;return;	// ElseIf
				case 3: (*code)=0xF7ED;return;	// SwitchEnd
			} break;
		case CMD_PRGM_CTL:
			switch ( CommandPage ) {
				case 0: (*code)=0xF7F1;return;	// Local
				case 1: (*code)=0xF739;return;	// TryEnd
//				case 1: (*code)=0xF95F;return;	// IsError(
			} break;
		case CMD_PRGM_JUMP:
			switch ( CommandPage ) {
				case 0: (*code)=0xE8;return;	// Dsz
			} break;
		case CMD_PRGM_CLR:
			switch ( CommandPage ) {
				case 0: (*code)=0xF93E;return;	// ClrVct
			} break;
		case CMD_PRGM_DISP:
			switch ( CommandPage ) {
			} break;
		case CMD_PRGM_REL:
			switch ( CommandPage ) {
				case 0: (*code)=0x12;return;	// >=
			} break;
		case CMD_PRGM_I_O:
			switch ( CommandPage ) {
				case 0: (*code)=0x7F9F;return;	// KeyRow(
				case 1: (*code)=0xF7DD;return;	// Beep 
			} break;
		case CMD_PRGM_STR:
			switch ( CommandPage ) {
				case 0: (*code)=0xF949;return;	// StrRepl(
				case 1: (*code)=0xF938;return;	// Exp(
				case 2: (*code)=0xF93D;return;	// StrRotate(
			} break;
		case CMD_PRGM_EXSTR:
			switch ( CommandPage ) {
				case 0: (*code)=0x0023;return;	// #
				case 1: (*code)=0xF947;return;	// Bin(
			} break;

		case CMD_OPTN:		//	------------------------------------------------------------OPTN_F5
			switch ( CommandPage ) {
				case 0: CommandType=CMD_OPTN_EXT ; CommandPage=0;break;
				case 1: CommandType=CMD_OPTN_ANGL; CommandPage=0;break;
				case 2: CommandType=CMD_OPTN_CAPT; CommandPage=0;break;
			} break;
			break;
		case CMD_OPTN_LIST:
			switch ( CommandPage ) {
				case 0: (*code)=0x7F2C;return;	// Seq(
				case 1: (*code)=0x7F49;return;	// Augment(
				case 2: (*code)=0x7F5C;return;	// ListCmp(
			} break;
		case CMD_OPTN_MAT:
			switch ( CommandPage ) {
				case 0: (*code)=0x7F49;return;	// Augment(
				case 1: CommandType=CMD_OPTN_MAT_SIZE; CommandPage=0;break;
				case 2: (*code)=0xF95E;return;	// UnitV(
			} break;
		case CMD_OPTN_CALC:
			break;
		case CMD_OPTN_HYP:
			switch ( CommandPage ) {
				case 0: (*code)=0x00B2;return;	// arccosh
			} break;
		case CMD_OPTN_PROB:
			break;
		case CMD_OPTN_PROB_RAND:
			switch ( CommandPage ) {
				case 0: (*code)=0x7F88;return;	// RanList#(
			} break;
		case CMD_OPTN_NUM:
			switch ( CommandPage ) {
				case 0: (*code)=0x00DE;return;	// Intg
			} break;
		case CMD_OPTN_ANGL:
			break;
		case CMD_OPTN_ESYM:
			switch ( CommandPage ) {
				case 0: (*code)=0x0001;return;	// femto
				case 1: (*code)=0x000A;return;	// Peta
			} break;
		case CMD_OPTN_PICT:
			switch ( CommandPage ) {
			} break;
		case CMD_OPTN_FMEM:
			switch ( CommandPage ) {
			} break;
		case CMD_OPTN_LOGIC:
			switch ( CommandPage ) {
			} break;
		case CMD_OPTN_EXT:
			switch ( CommandPage ) {
				case 0: (*code)='#';return;	// '#'
			} break;

		case CMD_VARS:		//	------------------------------------------------------------VARS_F5
			switch ( CommandPage ) {
				case 0: return;	// setup mode
				case 1: (*code)=0xF93F;return;	// Str
			} break;
		case CMD_VARS_GRPH:
			switch ( CommandPage ) {
				case 0: (*code)=0x7FF4;return;	// GRAPHX
			} break;
		case CMD_VARS_EXT:
			switch ( CommandPage ) {
				case 0: (*code)=0xF7FE;return;	// BackLight
			} break;

		case CMD_SHIFT:		//	-----------------------------------------------------------SHIFT_F5
			switch ( CommandPage ) {
			} break;
			break;
		case CMD_SHIFT_SKTCH:
			switch ( CommandPage ) {
				case 0: CommandType=CMD_SHIFT_SKTCH_GRPH;CommandPage=0;break;
				case 1: (*code)=0xF7A4;return;	// Horizontal
				case 2: CommandType=CMD_SHIFT_SKTCH_STYL;CommandPage=0;break;
			} break;
		case CMD_SHIFT_SKTCH_GRPH:
			switch ( CommandPage ) {
				case 0: (*code)=0x00EF;return;	// Graph Integral
			} break;
		case CMD_SHIFT_SKTCH_EXT:
			switch ( CommandPage ) {
				case 0: (*code)=0xF7F2;return;	// PopUpWin(
				case 2: (*code)=0x7FE9;return;	// CellSum(
			} break;
		case CMD_SHIFT_SKTCH_ML:
			switch ( CommandPage ) {
				case 0: (*code)=0xF9D8;return;	// _Test
				case 1: (*code)=0xF9C9;return;	// _Vertical
				case 2: (*code)=0xF9CE;return;	// _Fcircle 
				case 4: (*code)=0xF9C3;return;	// _Contrast
			} break;
		case CMD_SHIFT_SKTCH_BMP:
			switch ( CommandPage ) {
				case 0: (*code)=0xF9DA;return;	// _BmpRatate
			} break;

		case CMD_MENU:		//	------------------------------------------------------------MENU_F5
			switch ( CommandPage ) {
				case 0: CommandType=CMD_MENU_EXT; CommandPage=0;break;
			} break;
		case CMD_MENU_STAT_GRPH:
			switch ( CommandPage ) {
				case 0: (*code)=0xF751;return;	// xyLine
			} break;
		case CMD_MENU_EXT:
			switch ( CommandPage ) {
				case 0: (*code)=0x0027;return;	// '
			} break;

		case CMD_SETUP:		//	-----------------------------------------------------------SETUP_F5
			switch ( CommandPage ) {
				case 0: CommandType=CMD_SETUP_LABL;CommandPage=0;break;
				case 1: CommandType=CMD_SETUP_BACK;CommandPage=0;break;
				case 3: (*code)=0xF94F;return;	// Wait
				case 4: (*code)=0x7FDF;return;	// Version
			} break;

		default:
			break;
	}
	(*code)=0;
	return;
}
