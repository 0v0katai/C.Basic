#include "CB.h"

//----------------------------------------------------------------------------------------------

void GetGenuineCmdF2( unsigned int *code ){
	switch ( CommandType ) {
		case CMD_PRGM:		//	------------------------------------------------------------PRGM_F2
			switch ( CommandPage ) {
				case 0: CommandType=CMD_PRGM_CTL; CommandPage=0;break;
				case 1: CommandType=CMD_PRGM_DISP;CommandPage=0;break;
				case 2: (*code)=0x0024;return;	// $
			} break;
		case CMD_PRGM_COM:
			switch ( CommandPage ) {
				case 0: (*code)=0xF701;return;	// Then
				case 1: (*code)=0xF705;return;	// To
				case 2: (*code)=0xF709;return;	// WhileEnd
				case 3: (*code)=0xF7EB;return;	// Case
			} break;
		case CMD_PRGM_CTL:
			switch ( CommandPage ) {
				case 0: (*code)=0xF70C;return;	// Return
				case 1: (*code)=0xF717;return;	// ACBreak
			} break;
		case CMD_PRGM_JUMP:
			switch ( CommandPage ) {
				case 0: (*code)=0xEC;return;	// Goto
			} break;
		case CMD_PRGM_CLR:
			switch ( CommandPage ) {
				case 0: (*code)=0xF719;return;	// ClrGraph
			} break;
		case CMD_PRGM_DISP:
			switch ( CommandPage ) {
				case 0: (*code)=0xF720;return;	// DrawGraph
			} break;
		case CMD_PRGM_REL:
			switch ( CommandPage ) {
				case 0: (*code)=0x11;return;	// !=
			} break;
		case CMD_PRGM_I_O:
			switch ( CommandPage ) {
				case 0: (*code)=0x7F8F;return;	// Getkey
				case 1: (*code)=0xF716;return;	// Receive38k
				case 2: (*code)=0xF7EE;return;	// Save
			} break;
		case CMD_PRGM_STR:
			switch ( CommandPage ) {
				case 0: (*code)=0xF931;return;	// StrLen(
				case 1: (*code)=0xF935;return;	// StrRight(
				case 2: (*code)=0xF93A;return;	// StrLwr(
			} break;
		case CMD_PRGM_EXSTR:
			switch ( CommandPage ) {
				case 0: (*code)=0xF940;return;	// Str(
				case 1: (*code)=0xF945;return;	// StrCenter(
				case 2: (*code)=0xF950;return;	// StrAsc(
			} break;
		case CMD_PRGM_EXEC:
			switch ( CommandPage ) {
				case 0: (*code)=0xF7F5;return;	// Call
				case 1: (*code)=0x7FFA;return;	// ProgPtr(
			} break;

		case CMD_OPTN:		//	------------------------------------------------------------OPTN_F2
			switch ( CommandPage ) {
				case 0: CommandType=CMD_OPTN_MAT; CommandPage=0;break;
				case 1: CommandType=CMD_OPTN_HYP; CommandPage=0;break;
				case 2: CommandType=CMD_OPTN_PICT;CommandPage=0;break;
			} break;
			break;
		case CMD_OPTN_LIST:
			switch ( CommandPage ) {
				case 0: (*code)=0x7F4A;return;	// List->Mat(
				case 1: (*code)=0x7F20;return;	// Max(
				case 2: (*code)=0x7F4D;return;	// ProdSum
			} break;
		case CMD_OPTN_MAT:
			switch ( CommandPage ) {
				case 0: (*code)=0x7F4B;return;	// Mat->List(
				case 1: (*code)=0x7F46;return;	// Dim
				case 2: (*code)=0xF94B;return;	// DotP(
			} break;
		case CMD_OPTN_MAT_SIZE:
			switch ( CommandPage ) {
				case 0: (*code)=0x7F58;return;	// ElemSize(
			} break;
		case CMD_OPTN_CPLX:
			switch ( CommandPage ) {
				case 0: (*code)=0x0097;return;	// Abs
				case 1: (*code)=0x7F25;return;	// ImP
			} break;
		case CMD_OPTN_CALC:
			switch ( CommandPage ) {
				case 2: (*code)=0x7FBD;return;	// Rmdr
			} break;
		case CMD_OPTN_HYP:
			switch ( CommandPage ) {
				case 0: (*code)=0x00A2;return;	// cosh
			} break;
		case CMD_OPTN_PROB:
			switch ( CommandPage ) {
				case 0: (*code)=0x0088;return;	// nPr
			} break;
		case CMD_OPTN_PROB_RAND:
			switch ( CommandPage ) {
				case 0: (*code)=0x7F87;return;	// RanInt#
			} break;
		case CMD_OPTN_NUM:
			switch ( CommandPage ) {
				case 0: (*code)=0x00A6;return;	// Int
				case 1: (*code)=0x7F3C;return;	// GCD
			} break;
		case CMD_OPTN_ANGL:
			switch ( CommandPage ) {
				case 0: (*code)=0x00AC;return;	// rad
				case 1: (*code)=0x00A0;return;	// Rec(
			} break;
		case CMD_OPTN_ESYM:
			switch ( CommandPage ) {
				case 0: (*code)=0x0004;return;	// micro
				case 1: (*code)=0x0007;return;	// Mega
			} break;
		case CMD_OPTN_PICT:
			switch ( CommandPage ) {
				case 0: (*code)=0xF794;return;	// RclPict
			} break;
		case CMD_OPTN_LOGIC:
			switch ( CommandPage ) {
				case 0: (*code)=0x7FB1;return;	// Or
				case 1: (*code)=0x00AA;return;	// or
			} break;
		case CMD_OPTN_CAPT:
			switch ( CommandPage ) {
				case 0: (*code)=0xF79F;return;	// StoCapt
			} break;
		case CMD_OPTN_EXT:
			switch ( CommandPage ) {
				case 0: (*code)='_';return;	// '_'
			} break;

		case CMD_VARS:		//	------------------------------------------------------------VARS_F2
			switch ( CommandPage ) {
				case 0:	CommandType=CMD_VARS_FACT;CommandPage=0;break;
				case 1: (*code)='%';return;	// '%'
			} break;
		case CMD_VARS_VWIN:
			switch ( CommandPage ) {
				case 0:	CommandType=CMD_VARS_VWIN_Y;CommandPage=0;break;
			} break;
		case CMD_VARS_VWIN_X:
			switch ( CommandPage ) {
				case 0: (*code)=0x7F01;return;	// Xmax
			} break;
		case CMD_VARS_VWIN_Y:
			switch ( CommandPage ) {
				case 0: (*code)=0x7F05;return;	// Ymax
			} break;
		case CMD_VARS_VWIN_T:
			switch ( CommandPage ) {
				case 0: (*code)=0x7F09;return;	// TThetamax
			} break;
		case CMD_VARS_FACT:
			switch ( CommandPage ) {
				case 0: (*code)=0x7F0C;return;	// Yfct
			} break;
		case CMD_VARS_EXT:
			switch ( CommandPage ) {
				case 0: (*code)=0xF94F;return;	// Wait
//				case 1: (*code)=0xF7FA;return;	// RefreshTime
			} break;

		case CMD_SHIFT:		//	-----------------------------------------------------------SHIFT_F2
			break;
		case CMD_SHIFT_VWIN:
			switch ( CommandPage ) {
				case 0: (*code)=0xF797;return;	// StoV-Win
			} break;
		case CMD_SHIFT_SKTCH:
			switch ( CommandPage ) {
				case 0: CommandType=CMD_SHIFT_SKTCH_EXT;CommandPage=0;break;
				case 1: CommandType=CMD_SHIFT_SKTCH_LINE;CommandPage=0;break;
				case 2: (*code)=0xF7A5;return;	// Text
			} break;
		case CMD_SHIFT_SKTCH_GRPH:
			switch ( CommandPage ) {
				case 0: (*code)=0x00F4;return;	// Graph r=
				case 1: (*code)=0x00F1;return;	// Graph Y<
				case 2: (*code)=0xF92D;return;	// Graph X<
			} break;
		case CMD_SHIFT_SKTCH_PLOT:
			switch ( CommandPage ) {
				case 0: (*code)=0xF7A8;return;	// PlotOn
			} break;
		case CMD_SHIFT_SKTCH_LINE:
			switch ( CommandPage ) {
				case 0: (*code)=0xF7A7;return;	// F-Line
			} break;
		case CMD_SHIFT_SKTCH_PIXL:
			switch ( CommandPage ) {
				case 0: (*code)=0xF7AC;return;	// PxlOff
			} break;
		case CMD_SHIFT_SKTCH_STYL:
			switch ( CommandPage ) {
				case 0: (*code)=0xF78D;return;	// SketchThick
			} break;
		case CMD_SHIFT_SKTCH_EXT:
			switch ( CommandPage ) {
				case 0: (*code)=0xF7FC;return;	// PutDispDD
				case 1: (*code)=0xF7E2;return;	// FillRect(
				case 2: (*code)=0xF73B;return;	// DotPut(
			} break;
		case CMD_SHIFT_SKTCH_ML:
			switch ( CommandPage ) {
				case 0: (*code)=0xF9C1;return;	// _ClrScreen
				case 1: (*code)=0xF9C5;return;	// _Point 
				case 2: (*code)=0xF9CB;return;	// _Polygon
				case 3: (*code)=0xF9D0;return;	// _Felips 
				case 4: (*code)=0xF9D4;return;	// _Vscroll 
			} break;
		case CMD_SHIFT_SKTCH_BMP:
			switch ( CommandPage ) {
				case 0: (*code)=0xF9D6;return;	// _Bmp8
				case 1: (*code)=0xF9DC;return;	// BmpLoad
				case 2: (*code)=0xF961;return;	// GetFont
			} break;

		case CMD_MENU:		//	------------------------------------------------------------MENU_F2
			switch ( CommandPage ) {
				case 0: CommandType=CMD_MENU_MAT;CommandPage=0;break;
			} break;
		case CMD_MENU_STAT:
			switch ( CommandPage ) {
				case 0: CommandType=CMD_MENU_STAT_GRPH;CommandPage=0;break;
			} break;
		case CMD_MENU_STAT_DRAW:
			switch ( CommandPage ) {
				case 0: (*code)=0xF7DC;return;	// DrawOff
			} break;
		case CMD_MENU_STAT_GRPH:
			switch ( CommandPage ) {
				case 0: (*code)=0xF74B;return;	// S-Gph2
			} break;
		case CMD_MENU_STAT_TYPE:
			switch ( CommandPage ) {
				case 0: (*code)=0xF74E;return;	// Cross
			} break;
		case CMD_MENU_MAT:
			switch ( CommandPage ) {
				case 0: (*code)=0x7F42;return;	// *Row
			} break;
		case CMD_MENU_LIST:
			switch ( CommandPage ) {
				case 0: (*code)=0xF7B1;return;	// SortB(
			} break;
		case CMD_MENU_EXT:
			switch ( CommandPage ) {
				case 0: (*code)=0x000C;return;	// disps
			} break;

		case CMD_SETUP:		//	-----------------------------------------------------------SETUP_F2
			switch ( CommandPage ) {
				case 0: CommandType=CMD_SETUP_COOR;CommandPage=0;break;
				case 1: CommandType=CMD_SETUP_SL;  CommandPage=0;break;
				case 2: CommandType=CMD_SETUP_CPLX;CommandPage=0;break;
				case 3: (*code)=0xF7FA;return;	// RefreshTime
			} break;
		case CMD_SETUP_ANGL:
			switch ( CommandPage ) {
				case 0: (*code)=0x00DB;return;	// Rad
			} break;
		case CMD_SETUP_DISP:
			switch ( CommandPage ) {
				case 0: (*code)=0x00E4;return;	// Sci
			} break;
		case CMD_SETUP_DISP_ENG:
			switch ( CommandPage ) {
				case 0: (*code)=0xF90C;return;	// EngOff
			} break;
		case CMD_SETUP_COOR:
			switch ( CommandPage ) {
				case 0: (*code)=0xF7D3;return;	// CoordOff
			} break;
		case CMD_SETUP_GRID:
			switch ( CommandPage ) {
				case 0: (*code)=0xF77A;return;	// GridOff
			} break;
		case CMD_SETUP_AXES:
			switch ( CommandPage ) {
				case 0: (*code)=0xF7D2;return;	// AxesOff
			} break;
		case CMD_SETUP_LABL:
			switch ( CommandPage ) {
				case 0: (*code)=0xF7D4;return;	// LabelOff
			} break;
		case CMD_SETUP_DERV:
			switch ( CommandPage ) {
				case 0: (*code)=0xF7D5;return;	// DerivOff
			} break;
		case CMD_SETUP_FUNC:
			switch ( CommandPage ) {
				case 0: (*code)=0xF7D0;return;	// FuncOff
			} break;
		case CMD_SETUP_DRAW:
			switch ( CommandPage ) {
				case 0: (*code)=0xF771;return;	// G-Plot
			} break;
		case CMD_SETUP_BACK:
			switch ( CommandPage ) {
				case 0: (*code)=0xF779;return;	// BG-Pict
			} break;
		case CMD_SETUP_SWIN:
			switch ( CommandPage ) {
				case 0: (*code)=0xF761;return;	// S-WindMan
			} break;
		case CMD_SETUP_SL:
			switch ( CommandPage ) {
				case 0: (*code)=0xF71D;return;	// S-L-Thick
			} break;
		case CMD_SETUP_CPLX:
			switch ( CommandPage ) {
				case 0: (*code)=0xF909;return;	// a+bi
			} break;

		default:
			break;
	}
	(*code)=0;
	return;
}

