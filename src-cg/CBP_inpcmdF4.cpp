extern "C" {

#include "CBP_inp.h"

//----------------------------------------------------------------------------------------------
void GetGenuineCmdF4( int *code ){
	switch ( CommandType ) {
		case CMD_PRGM:		//	------------------------------------------------------------PRGM_F4
			switch ( CommandPage ) {
				case 0: (*code)=0x003F;return;	// ?
				case 1: CommandType=CMD_PRGM_I_O;CommandPage=0;break;
//				case 2: (*code)=0xF90F;return;	// Alias
			} break;
		case CMD_PRGM_COM:
			switch ( CommandPage ) {
				case 0: (*code)=0xF703;return;	// IfEnd
				case 1: (*code)=0xF707;return;	// Next
				case 2: (*code)=0xF70B;return;	// LpWhile
				case 3: (*code)=0xF70D;return;	// Break
			} break;
		case CMD_PRGM_CTL:
			switch ( CommandPage ) {
				case 0: (*code)=0xF70E;return;	// Stop
				case 1: (*code)=0xF738;return;	// Except
			} break;
		case CMD_PRGM_JUMP:
			switch ( CommandPage ) {
				case 0: (*code)=0xE9;return;	// Isz
			} break;
		case CMD_PRGM_CLR:
			switch ( CommandPage ) {
				case 0: (*code)=0xF91E;return;	// ClrMat
			} break;
		case CMD_PRGM_DISP:
			switch ( CommandPage ) {
				case 0: (*code)=0xF72E;return;	// DispF-Tbl
			} break;
		case CMD_PRGM_REL:
			switch ( CommandPage ) {
				case 0: (*code)=0x3C;return;	// <
			} break;
		case CMD_PRGM_I_O:
			switch ( CommandPage ) {
				case 0: (*code)=0xF712;return;	// Receive(
				case 1: (*code)=0xF714;return;	// CloseComport38k
				case 2: (*code)=0xF7DF;return;	// Delete
			} break;
		case CMD_PRGM_STR:
			switch ( CommandPage ) {
				case 0: (*code)=0xF933;return;	// StrSrc(
				case 1: (*code)=0xF937;return;	// Exp>Str(
				case 2: (*code)=0xF93C;return;	// StrShift(
			} break;
		case CMD_PRGM_EXSTR:
			switch ( CommandPage ) {
				case 0: (*code)=0x0025;return;	// %
				case 1: (*code)=0xF946;return;	// Hex(
			} break;
		case CMD_PRGM_EXEC:
			switch ( CommandPage ) {
				case 0: (*code)=0x7FF6;return;	// Peek(
				case 1: (*code)=0x0026;return;	// &
			} break;

		case CMD_OPTN:		//	------------------------------------------------------------OPTN_F4
			switch ( CommandPage ) {
				case 0: CommandType=CMD_OPTN_CALC; CommandPage=1;break;
				case 1: CommandType=CMD_OPTN_NUM;  CommandPage=0;break;
				case 2: CommandType=CMD_OPTN_LOGIC;CommandPage=0;break;
			} break;
			break;
		case CMD_OPTN_LIST:
			switch ( CommandPage ) {
				case 0: (*code)=0x7F47;return;	// Fill(
			} break;
		case CMD_OPTN_MAT:
			switch ( CommandPage ) {
				case 0: (*code)=0x7F41;return;	// Trn
				case 1: (*code)=0x7F55;return;	// ref
				case 2: (*code)=0xF96D;return;	// Angle(
			} break;
		case CMD_OPTN_MAT_SIZE:
			switch ( CommandPage ) {
				case 0: (*code)=0x7F5A;return;	// ColSize
			} break;
		case CMD_OPTN_CPLX:
			switch ( CommandPage ) {
				case 0: (*code)=0x7F23;return;	// Conjg
				case 1: (*code)=0xF906;return;	// a+bi
			} break;
		case CMD_OPTN_CALC:
			switch ( CommandPage ) {
				case 1: (*code)=0x7F85;return;	// logab
			} break;
		case CMD_OPTN_HYP:
			switch ( CommandPage ) {
				case 0: (*code)=0x00B1;return;	// arcsinh
			} break;
		case CMD_OPTN_PROB:
			switch ( CommandPage ) {
				case 0: CommandType=CMD_OPTN_PROB_RAND;CommandPage=0;break;
			} break;
		case CMD_OPTN_PROB_RAND:
			switch ( CommandPage ) {
				case 0: (*code)=0x7F89;return;	// RanInt#(
			} break;
		case CMD_OPTN_NUM:
			switch ( CommandPage ) {
				case 0: (*code)=0x00D3;return;	// Rnd
				case 1: (*code)=0x7F3A;return;	// MOD(
			} break;
		case CMD_OPTN_ANGL:
			switch ( CommandPage ) {
				case 0: (*code)=0x008C;return;	// dms
			} break;
		case CMD_OPTN_ESYM:
			switch ( CommandPage ) {
				case 0: (*code)=0x0002;return;	// pico
				case 1: (*code)=0x0009;return;	// Tera
			} break;
		case CMD_OPTN_FMEM:
			switch ( CommandPage ) {
				case 0: (*code)=0x4F91B;return;	// SEE
			} break;
		case CMD_OPTN_LOGIC:
			switch ( CommandPage ) {
				case 0: (*code)=0x7FB4;return;	// Xor
				case 1: (*code)=0x009A;return;	// xor
			} break;
		case CMD_OPTN_EXT:
			switch ( CommandPage ) {
				case 0: (*code)='&';return;	// '&'
			} break;

		case CMD_VARS:		//	------------------------------------------------------------VARS_F4
			switch ( CommandPage ) {
				case 0: CommandType=CMD_VARS_GRPH; CommandPage=0;break;
				case 1: (*code)='$';return;	// '$'
			} break;
		case CMD_VARS_VWIN_X:
			switch ( CommandPage ) {
				case 0: (*code)=0xF921;return;	// Xdot
			} break;
		case CMD_VARS_GRPH:
			switch ( CommandPage ) {
				case 0: (*code)=0x7FF3;return;	// GRAPHYt
			} break;
		case CMD_VARS_TABL:
			switch ( CommandPage ) {
				case 0: (*code)=0x7F90;return;	// F Result
			} break;
		case CMD_VARS_EXT:
			switch ( CommandPage ) {
				case 0: (*code)=0xF942;return;	// TIME
			} break;

		case CMD_SHIFT:		//	-----------------------------------------------------------SHIFT_F4
			switch ( CommandPage ) {
				case 0: CommandType=CMD_SHIFT_SKTCH;CommandPage=0;break;
			} break;
		case CMD_SHIFT_SKTCH:
			switch ( CommandPage ) {
				case 0: CommandType=CMD_SHIFT_SKTCH_BMP;CommandPage=0;break;
				case 1: (*code)=0xF7A3;return;	// Vertical
				case 2: (*code)=0xF7AF;return;	// PxlTest(
			} break;
		case CMD_SHIFT_SKTCH_GRPH:
			switch ( CommandPage ) {
				case 0: (*code)=0xF762;return;	// Graph X=
				case 1: (*code)=0x00F3;return;	// Graph Y<=
				case 2: (*code)=0xF92F;return;	// Graph X<=
			} break;
		case CMD_SHIFT_SKTCH_PLOT:
			switch ( CommandPage ) {
				case 0: (*code)=0xF7AA;return;	// PlotChg
			} break;
		case CMD_SHIFT_SKTCH_STYL:
			switch ( CommandPage ) {
				case 0: (*code)=0xF78F;return;	// SketchDot
			} break;
		case CMD_SHIFT_SKTCH_COLOR:
			switch ( CommandPage ) {
				case 0: (*code)=0x7F5D;return;	// GetRGB(
				case 1: (*code)=0x7F72;return;	// GetHSL(
			} break;
		case CMD_SHIFT_SKTCH_EXT:
			switch ( CommandPage ) {
				case 0: (*code)=0xF7FD;return;	// FKey(
				case 1: (*code)=0xF7E9;return;	// WriteGraph(
				case 2: (*code)=0xF7E0;return;	// DotLife(
			} break;
		case CMD_SHIFT_SKTCH_ML:
			switch ( CommandPage ) {
				case 0: (*code)=0xF9C6;return;	// _PixelTest(
				case 1: (*code)=0xF9C8;return;	// _Horizontal
				case 2: (*code)=0xF9CD;return;	// _Circle
				case 3: (*code)=0xF9D2;return;	// _FelipsInRct 
			} break;
		case CMD_SHIFT_SKTCH_BMP:
			switch ( CommandPage ) {
				case 0: (*code)=0xF9D9;return;	// _BmpZoom
				case 1: (*code)=0xF9DE;return;	// _BmpZoomRotate
				case 2: (*code)=0xF963;return;	// GetFontMini
			} break;

		case CMD_MENU:		//	------------------------------------------------------------MENU_F4
			switch ( CommandPage ) {
				case 0: CommandType=CMD_MENU_GRPH; CommandPage=0;break;
			} break;
		case CMD_MENU_STAT:
			switch ( CommandPage ) {
				case 0: CommandType=CMD_MENU_STAT_TYPE;CommandPage=0;break;
			} break;
		case CMD_MENU_STAT_DRAW:
			break;
		case CMD_MENU_STAT_GRPH:
			switch ( CommandPage ) {
				case 0: (*code)=0xF750;return;	// Scatter
			} break;
		case CMD_MENU_MAT:
			switch ( CommandPage ) {
				case 0: (*code)=0x7F44;return;	// Row+
			} break;
		case CMD_MENU_GRPH:
			switch ( CommandPage ) {
				case 0: CommandType=CMD_MENU_GRPH_STYL;CommandPage=0;break;
			} break;
		case CMD_MENU_GRPH_TYPE:
			switch ( CommandPage ) {
				case 0:  (*code)=0xF767;return;	// X=Type
				case 1:  (*code)=0xF76D;return;	// Y<=Type
				case 2:  (*code)=0xF76F;return;	// X<=Type
			} break;
		case CMD_MENU_GRPH_STYL:
		case CMD_MENU_TABL_STYL:
			switch ( CommandPage ) {
				case 0:  (*code)=0xF73F;return;	// DotG
			} break;
		case CMD_MENU_TABL:
			switch ( CommandPage ) {
				case 0: CommandType=CMD_MENU_TABL_STYL;CommandPage=0;break;
			} break;
		case CMD_MENU_EXT:
			switch ( CommandPage ) {
				case 0: (*code)=0x0013;return;	// =>
			} break;

		case CMD_SETUP:		//	-----------------------------------------------------------SETUP_F4
			switch ( CommandPage ) {
				case 0: CommandType=CMD_SETUP_AXES;CommandPage=0;break;
				case 1: CommandType=CMD_SETUP_DERV;CommandPage=0;break;
				case 2: CommandType=CMD_SETUP_LIST;CommandPage=0;break;
				case 3: (*code)=0xF942;return;	// TIME
				case 4: (*code)=0x7FCF;return;	// System(
			} break;
		case CMD_SETUP_DISP:
			switch ( CommandPage ) {
				case 0: CommandType=CMD_SETUP_DISP_ENG;CommandPage=0;break;
			} break;
		case CMD_SETUP_SL:
			switch ( CommandPage ) {
				case 0: (*code)=0xF71F;return;	// S-L-Dot
			} break;

		default:
			break;
	}
	(*code)=0;
	return;
}

}
