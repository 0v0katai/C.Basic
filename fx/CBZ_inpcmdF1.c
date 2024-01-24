#include "CB_inp.h"

//----------------------------------------------------------------------------------------------
void GetGenuineCmdF1( unsigned int *code ){
	switch ( CommandType ) {
		case CMD_PRGM:		//	------------------------------------------------------------PRGM_F1
			switch ( CommandPage ) {
				case 0:	CommandType=CMD_PRGM_COM;CommandPage=0;break;
				case 1: CommandType=CMD_PRGM_CLR;CommandPage=0;break;
				case 2: CommandType=CMD_PRGM_STR;CommandPage=0;break;
			} break;
		case CMD_PRGM_COM:
			switch ( CommandPage ) {
				case 0: (*code)=0xF700;return;	// If
				case 1: (*code)=0xF704;return;	// For
				case 2: (*code)=0xF708;return;	// While
				case 3: (*code)=0xF7EA;return;	// Switch
			} break;
		case CMD_PRGM_CTL:
			switch ( CommandPage ) {
				case 0: (*code)=0x00ED;return;	// Prog
				case 1: (*code)=0xFA;return;	// Gosub
			} break;
		case CMD_PRGM_JUMP:
			switch ( CommandPage ) {
				case 0: (*code)=0x00E2;return;	// Lbl
			} break;
		case CMD_PRGM_CLR:
			switch ( CommandPage ) {
				case 0: (*code)=0xF718;return;	// ClrText
			} break;
		case CMD_PRGM_DISP:
			switch ( CommandPage ) {
				case 0: (*code)=0xF723;return;	// DrawStat
			} break;
		case CMD_PRGM_REL:
			switch ( CommandPage ) {
				case 0: (*code)=0x003D;return;	// =
			} break;
		case CMD_PRGM_I_O:
			switch ( CommandPage ) {
				case 0: (*code)=0xF710;return;	// Locate
				case 1: (*code)=0xF715;return;	// Send38K
				case 2: (*code)=0x7FF5;return;	// IsExist(
			} break;
		case CMD_PRGM_STR:
			switch ( CommandPage ) {
				case 0: (*code)=0xF930;return;	// StrJoin(
				case 1: (*code)=0xF934;return;	// StrLeft(
				case 2: (*code)=0xF939;return;	// StrUpr(
			} break;
		case CMD_PRGM_EXSTR:
			switch ( CommandPage ) {
				case 0: (*code)=0xF93F;return;	// Str
				case 1: (*code)=0xF944;return;	// StrChar(
				case 2: (*code)=0xF94D;return;	// _StrSplit(
			} break;
		case CMD_PRGM_EXEC:
			switch ( CommandPage ) {
				case 0: (*code)=0xF7F4;return;	// SysCall
				case 1: (*code)=0x7FF8;return;	// VarPtr(
			} break;

		case CMD_OPTN:		//	------------------------------------------------------------OPTN_F1
			switch ( CommandPage ) {
				case 0:	CommandType=CMD_OPTN_LIST;CommandPage=0;break;
				case 2: CommandType=CMD_OPTN_ESYM;CommandPage=0;break;
			} break;
		case CMD_OPTN_LIST:
			switch ( CommandPage ) {
				case 0: (*code)=0x7F51;return;	// List
				case 1: (*code)=0x7F2D;return;	// Min(
				case 2: (*code)=0x7F4C;return;	// Sum
			} break;
		case CMD_OPTN_MAT:
			switch ( CommandPage ) {
				case 0: (*code)=0x7F40;return;	// Mat 
			} break;
		case CMD_OPTN_MAT_SIZE:
			switch ( CommandPage ) {
				case 0: (*code)=0x7F5B;return;	// MatBase
			} break;
		case CMD_OPTN_CALC:
			switch ( CommandPage ) {
				case 2: (*code)=0x7FBC;return;	// Int/ 
			} break;
		case CMD_OPTN_HYP:
			switch ( CommandPage ) {
				case 0: (*code)=0x00A1;return;	// sinh
			} break;
		case CMD_OPTN_PROB:
			switch ( CommandPage ) {
				case 0: (*code)=0x00AB;return;	// X!
			} break;
		case CMD_OPTN_PROB_RAND:
			switch ( CommandPage ) {
				case 0: (*code)=0x00C1;return;	// Ran#
			} break;
		case CMD_OPTN_NUM:
			switch ( CommandPage ) {
				case 0: (*code)=0x0097;return;	// Abs
				case 1: (*code)=0x7F86;return;	// RndFix(
			} break;
		case CMD_OPTN_ANGL:
			switch ( CommandPage ) {
				case 0: (*code)=0x009C;return;	// deg
				case 1: (*code)=0x0080;return;	// Pol(
			} break;
		case CMD_OPTN_ESYM:
			switch ( CommandPage ) {
				case 0: (*code)=0x0005;return;	// mill
				case 1: (*code)=0x0006;return;	// Kiro
				case 2: (*code)=0x000B;return;	// Exa
			} break;
		case CMD_OPTN_PICT:
			switch ( CommandPage ) {
				case 0: (*code)=0xF793;return;	// StoPict
			} break;
		case CMD_OPTN_FMEM:
			switch ( CommandPage ) {
			} break;
		case CMD_OPTN_LOGIC:
			switch ( CommandPage ) {
				case 0: (*code)=0x7FB0;return;	// And
				case 1: (*code)=0x00BA;return;	// and
			} break;
		case CMD_OPTN_CAPT:
			switch ( CommandPage ) {
				case 0: (*code)=0xF79D;return;	// StoCapt
			} break;
		case CMD_OPTN_EXT:
			switch ( CommandPage ) {
				case 0: (*code)=0xF90F;return;	// AliasVar
			} break;

		case CMD_VARS:		//	------------------------------------------------------------VARS_F1
			switch ( CommandPage ) {
				case 0:	CommandType=CMD_VARS_VWIN;CommandPage=0;break;
				case 1: (*code)='#';return;	// '#'
			} break;
		case CMD_VARS_VWIN:
			switch ( CommandPage ) {
				case 0:	CommandType=CMD_VARS_VWIN_X;CommandPage=0;break;
			} break;
		case CMD_VARS_VWIN_X:
			switch ( CommandPage ) {
				case 0: (*code)=0x7F00;return;	// Xmin
			} break;
		case CMD_VARS_VWIN_Y:
			switch ( CommandPage ) {
				case 0: (*code)=0x7F04;return;	// Ymin
			} break;
		case CMD_VARS_VWIN_T:
			switch ( CommandPage ) {
				case 0: (*code)=0x7F08;return;	// TThetamin
			} break;
		case CMD_VARS_FACT:
			switch ( CommandPage ) {
				case 0: (*code)=0x7F0B;return;	// Xfct
			} break;
		case CMD_VARS_GRPH:
			switch ( CommandPage ) {
				case 0: (*code)=0x7FF0;return;	// GRAPHY
			} break;
		case CMD_VARS_EXT:
			switch ( CommandPage ) {
				case 0: (*code)=0x7F5F;return;	// Ticks
//				case 1: (*code)=0xF7F8;return;	// RefreshCtrl
			} break;

		case CMD_SHIFT:		//	-----------------------------------------------------------SHIFT_F1
			switch ( CommandPage ) {
			} break;
		case CMD_SHIFT_VWIN:
			switch ( CommandPage ) {
				case 0: (*code)=0x00EB;return;	// ViewWindow
			} break;
		case CMD_SHIFT_SKTCH:
			switch ( CommandPage ) {
				case 0: (*code)=0x00D1;return;	// Cls
				case 1: CommandType=CMD_SHIFT_SKTCH_PLOT;CommandPage=0;break;
			} break;
		case CMD_SHIFT_SKTCH_GRPH:
			switch ( CommandPage ) {
				case 0: (*code)=0x00EE;return;	// GRAPH Y=
			} break;
		case CMD_SHIFT_SKTCH_PLOT:
			switch ( CommandPage ) {
				case 0: (*code)=0x00E0;return;	// Plot
			} break;
		case CMD_SHIFT_SKTCH_LINE:
			switch ( CommandPage ) {
				case 0: (*code)=0x00E1;return;	// Line
			} break;
		case CMD_SHIFT_SKTCH_PIXL:
			switch ( CommandPage ) {
				case 0: (*code)=0xF7AB;return;	// PxlOn
			} break;
		case CMD_SHIFT_SKTCH_STYL:
			switch ( CommandPage ) {
				case 0: (*code)=0xF78C;return;	// SketchNormal
			} break;
		case CMD_SHIFT_SKTCH_EXT:
			switch ( CommandPage ) {
				case 0: (*code)=0xF7FB;return;	// Screen
				case 1: (*code)=0xF7E1;return;	// Rect(
				case 2: (*code)=0xF73E;return;	// DotGet(
			} break;
		case CMD_SHIFT_SKTCH_ML:
			switch ( CommandPage ) {
				case 0: (*code)=0xF9C0;return;	// _ClrVRAM
				case 1: (*code)=0xF9C4;return;	// _Pixel 
				case 2: (*code)=0xF9CA;return;	// _Rect 
				case 3: (*code)=0xF9CF;return;	// _Elips 
				case 4: (*code)=0xF9D3;return;	// _Hscroll
			} break;
		case CMD_SHIFT_SKTCH_BMP:
			switch ( CommandPage ) {
				case 0: (*code)=0xF9D5;return;	// _Bmp 
				case 1: (*code)=0xF9DB;return;	// BmpSave
				case 2: (*code)=0xF960;return;	// SetFont(
			} break;

		case CMD_MENU:		//	------------------------------------------------------------MENU_F1
			switch ( CommandPage ) {
				case 0: CommandType=CMD_MENU_STAT;CommandPage=0;break;
			} break;
		case CMD_MENU_STAT:
			switch ( CommandPage ) {
				case 0: CommandType=CMD_MENU_STAT_DRAW;CommandPage=0;break;
			} break;
		case CMD_MENU_STAT_DRAW:
			switch ( CommandPage ) {
				case 0: (*code)=0xF7CC;return;	// DrawOn
			} break;
		case CMD_MENU_STAT_GRPH:
			switch ( CommandPage ) {
				case 0: (*code)=0xF74A;return;	// S-Gph1
			} break;
		case CMD_MENU_STAT_TYPE:
			switch ( CommandPage ) {
				case 0: (*code)=0xF74D;return;	// Square
			} break;
		case CMD_MENU_MAT:
			switch ( CommandPage ) {
				case 0: (*code)=0x7F45;return;	// Swap
			} break;
		case CMD_MENU_LIST:
			switch ( CommandPage ) {
				case 0: (*code)=0xF7B0;return;	// SortA(
			} break;
		case CMD_MENU_EXT:
			switch ( CommandPage ) {
				case 0: (*code)='?';return;	// ?
			} break;

		case CMD_SETUP:		//	-----------------------------------------------------------SETUP_F1
			switch ( CommandPage ) {
				case 0: CommandType=CMD_SETUP_ANGL;CommandPage=0;break;
				case 1: CommandType=CMD_SETUP_DISP;CommandPage=0;break;
				case 2: CommandType=CMD_SETUP_FUNC;CommandPage=0;break;
				case 3: (*code)=0xF7F8;return;	// RefreshCtrl
			} break;
		case CMD_SETUP_ANGL:
			switch ( CommandPage ) {
				case 0: (*code)=0x00DA;return;	// Deg
			} break;
		case CMD_SETUP_DISP:
			switch ( CommandPage ) {
				case 0: (*code)=0x00E3;return;	// Fix
			} break;
		case CMD_SETUP_DISP_ENG:
			switch ( CommandPage ) {
				case 0: (*code)=0xF90B;return;	// EngOn
			} break;
			
		case CMD_SETUP_COOR:
			switch ( CommandPage ) {
				case 0: (*code)=0xF7C3;return;	// CoordOn
			} break;
		case CMD_SETUP_GRID:
			switch ( CommandPage ) {
				case 0: (*code)=0xF77D;return;	// GridOn
			} break;
		case CMD_SETUP_AXES:
			switch ( CommandPage ) {
				case 0: (*code)=0xF7C2;return;	// AxesOn
			} break;
		case CMD_SETUP_LABL:
			switch ( CommandPage ) {
				case 0: (*code)=0xF7C4;return;	// LabelOn
			} break;
		case CMD_SETUP_DERV:
			switch ( CommandPage ) {
				case 0: (*code)=0xF7C5;return;	// DerivOn
			} break;
		case CMD_SETUP_FUNC:
			switch ( CommandPage ) {
				case 0: (*code)=0xF7C0;return;	// FuncOn
			} break;
		case CMD_SETUP_DRAW:
			switch ( CommandPage ) {
				case 0: (*code)=0xF770;return;	// G-Connect
			} break;
		case CMD_SETUP_BACK:
			switch ( CommandPage ) {
				case 0: (*code)=0xF778;return;	// BG-None
			} break;
		case CMD_SETUP_SWIN:
			switch ( CommandPage ) {
				case 0: (*code)=0xF760;return;	// S-WindAuto
			} break;
		case CMD_SETUP_SL:
			switch ( CommandPage ) {
				case 0: (*code)=0xF71C;return;	// S-L-Normal
			} break;

		default:
			break;
	}
	(*code)=0;
	return;
}

