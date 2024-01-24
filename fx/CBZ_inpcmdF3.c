#include "CB_inp.h"

//----------------------------------------------------------------------------------------------

void GetGenuineCmdF3( unsigned int *code ){
	switch ( CommandType ) {
		case CMD_PRGM:		//	------------------------------------------------------------PRGM_F3
			switch ( CommandPage ) {
				case 0: CommandType=CMD_PRGM_JUMP; CommandPage=0;break;
				case 1: CommandType=CMD_PRGM_REL;  CommandPage=0;break;
				case 2: CommandType=CMD_PRGM_EXSTR;CommandPage=0;break;
			} break;
		case CMD_PRGM_COM:
			switch ( CommandPage ) {
				case 0: (*code)=0xF702;return;	// Else
				case 1: (*code)=0xF706;return;	// Step
				case 2: (*code)=0xF70A;return;	// Do
				case 3: (*code)=0xF7EC;return;	// Default
			} break;
		case CMD_PRGM_CTL:
			switch ( CommandPage ) {
				case 0: (*code)=0xF70D;return;	// Break
			} break;
		case CMD_PRGM_JUMP:
			switch ( CommandPage ) {
				case 0: (*code)=0x13;return;	// =>
			} break;
		case CMD_PRGM_CLR:
			switch ( CommandPage ) {
				case 0: (*code)=0xF71A;return;	// ClrList
			} break;
		case CMD_PRGM_REL:
			switch ( CommandPage ) {
				case 0: (*code)=0x3E;return;	// >
			} break;
		case CMD_PRGM_I_O:
			switch ( CommandPage ) {
				case 1: (*code)=0xF7EF;return;	// Load(
			} break;
		case CMD_PRGM_STR:
			switch ( CommandPage ) {
				case 0: (*code)=0xF932;return;	// StrCmp(
				case 1: (*code)=0xF936;return;	// StrMid(
				case 2: (*code)=0xF93B;return;	// StrInv(
			} break;
		case CMD_PRGM_EXSTR:
			switch ( CommandPage ) {
				case 0: (*code)=0xF943;return;	// Sprintf(
				case 1: (*code)=0xF948;return;	// StrBase(
			} break;
		case CMD_PRGM_EXEC:
			switch ( CommandPage ) {
				case 0: (*code)=0xF7F6;return;	// Poke
				case 1: (*code)=0x002A;return;	// *
			} break;

		case CMD_OPTN:		//	------------------------------------------------------------OPTN_F3
			switch ( CommandPage ) {
				case 0: CommandType=CMD_OPTN_EXT; CommandPage=0;break;
				case 1: CommandType=CMD_OPTN_PROB;CommandPage=0;break;
				case 2: CommandType=CMD_OPTN_FMEM;CommandPage=0;break;
			} break;
			break;
		case CMD_OPTN_LIST:
			switch ( CommandPage ) {
				case 0: (*code)=0x7F46;return;	// Dim
				case 1: (*code)=0x7F2E;return;	// Mean(
			} break;
		case CMD_OPTN_MAT_SIZE:
			switch ( CommandPage ) {
				case 0: (*code)=0x7F59;return;	// ColSize
			} break;
		case CMD_OPTN_MAT:
			switch ( CommandPage ) {
				case 1: (*code)=0x7F47;return;	// Fill(
			} break;
		case CMD_OPTN_CALC:
			switch ( CommandPage ) {
				case 1: (*code)=0x7F29;return;	// Sigma(
			} break;
		case CMD_OPTN_HYP:
			switch ( CommandPage ) {
				case 0: (*code)=0x00A3;return;	// tanh
			} break;
		case CMD_OPTN_PROB_RAND:
			switch ( CommandPage ) {
				case 0: (*code)=0x7F8A;return;	// RanNorm#(
			} break;
		case CMD_OPTN_NUM:
			switch ( CommandPage ) {
				case 0: (*code)=0x00B6;return;	// Frac
				case 1: (*code)=0x7F3D;return;	// LCM
			} break;
		case CMD_OPTN_ANGL:
			switch ( CommandPage ) {
				case 0: (*code)=0x00BC;return;	// grad
				case 1: (*code)=0xF905;return;	// >DMS
			} break;
		case CMD_OPTN_ESYM:
			switch ( CommandPage ) {
				case 0: (*code)=0x0003;return;	// nano
				case 1: (*code)=0x0008;return;	// Giga
			} break;
		case CMD_OPTN_FMEM:
			switch ( CommandPage ) {
				case 0: (*code)=0xF91B;return;	// fn
			} break;
		case CMD_OPTN_LOGIC:
			switch ( CommandPage ) {
				case 0: (*code)=0x7FB3;return;	// Not
				case 1: (*code)=0x00A7;return;	// not
			} break;
		case CMD_OPTN_EXT:
			switch ( CommandPage ) {
				case 0: (*code)='&';return;	// '&'
			} break;

		case CMD_VARS:		//	------------------------------------------------------------VARS_F3
			switch ( CommandPage ) {
				case 0: CommandType=CMD_VARS_EXT; CommandPage=0;break;
				case 1: (*code)='~';return;	// '~'
			} break;
		case CMD_VARS_VWIN:
			switch ( CommandPage ) {
				case 0:	CommandType=CMD_VARS_VWIN_T;CommandPage=0;break;
			} break;
		case CMD_VARS_VWIN_X:
			switch ( CommandPage ) {
				case 0: (*code)=0x7F02;return;	// Xscl
			} break;
		case CMD_VARS_VWIN_Y:
			switch ( CommandPage ) {
				case 0: (*code)=0x7F06;return;	// Yscl
			} break;
		case CMD_VARS_VWIN_T:
			switch ( CommandPage ) {
				case 0: (*code)=0x7F0A;return;	// TThetaptch
			} break;
		case CMD_VARS_EXT:
			switch ( CommandPage ) {
				case 0: (*code)=0xF941;return;	// DATE
			} break;

		case CMD_SHIFT:		//	-----------------------------------------------------------SHIFT_F3
			switch ( CommandPage ) {
				case 0: CommandType=CMD_SHIFT_VWIN;CommandPage=0;break;
			} break;
		case CMD_SHIFT_VWIN:
			switch ( CommandPage ) {
				case 0: (*code)=0xF798;return;	// RclV-Win
			} break;
		case CMD_SHIFT_SKTCH:
			switch ( CommandPage ) {
				case 0: CommandType=CMD_SHIFT_SKTCH_ML;CommandPage=0;break;
				case 1: (*code)=0xF7A6;return;	// Circle
				case 2: CommandType=CMD_SHIFT_SKTCH_PIXL;CommandPage=0;break;
			} break;
		case CMD_SHIFT_SKTCH_GRPH:
			switch ( CommandPage ) {
				case 0: (*code)=0x00F5;return;	// Graph(X,Y)=(
			} break;
		case CMD_SHIFT_SKTCH_PLOT:
			switch ( CommandPage ) {
				case 0: (*code)=0xF7A9;return;	// PlotOff
			} break;
		case CMD_SHIFT_SKTCH_PIXL:
			switch ( CommandPage ) {
				case 0: (*code)=0xF7AD;return;	// PxlChg
			} break;
		case CMD_SHIFT_SKTCH_STYL:
			switch ( CommandPage ) {
				case 0: (*code)=0xF78E;return;	// SketchBroken
			} break;
		case CMD_SHIFT_SKTCH_EXT:
			switch ( CommandPage ) {
				case 0: (*code)=0xF7E3;return;	// LocateYX
				case 1: (*code)=0xF7E8;return;	// ReadGraph(
				case 2: (*code)=0xF73D;return;	// DotTrim(
			} break;
		case CMD_SHIFT_SKTCH_ML:
			switch ( CommandPage ) {
				case 0: (*code)=0xF9C2;return;	// _DispVRAM
				case 1: (*code)=0xF9CA;return;	// _Rect 
				case 2: (*code)=0xF9CD;return;	// _Circle
				case 3: (*code)=0xF9D1;return;	// _ElipsInRct 
				case 4: (*code)=0xF9D5;return;	// _Bmp 
			} break;

		case CMD_MENU:		//	------------------------------------------------------------MENU_F3
			switch ( CommandPage ) {
				case 0: CommandType=CMD_MENU_LIST;CommandPage=0;break;
			} break;
		case CMD_MENU_STAT:
			switch ( CommandPage ) {
				case 0: (*code)=0x7F51;return;	// List
			} break;
		case CMD_MENU_STAT_GRPH:
			switch ( CommandPage ) {
				case 0: (*code)=0xF74C;return;	// S-Gph3
			} break;
		case CMD_MENU_STAT_TYPE:
			switch ( CommandPage ) {
				case 0: (*code)=0xF74F;return;	// Dot
			} break;
		case CMD_MENU_MAT:
			switch ( CommandPage ) {
				case 0: (*code)=0x7F43;return;	// *Row+
			} break;
		case CMD_MENU_EXT:
			switch ( CommandPage ) {
				case 0: (*code)=':';return;	// :
			} break;

		case CMD_SETUP:		//	-----------------------------------------------------------SETUP_F3
			switch ( CommandPage ) {
				case 0: CommandType=CMD_SETUP_GRID;CommandPage=0;break;
				case 1: CommandType=CMD_SETUP_DRAW;CommandPage=0;break;
				case 2: CommandType=CMD_SETUP_SWIN;CommandPage=0;break;
				case 3: (*code)=0xF941;return;	// DATE
			} break;
		case CMD_SETUP_ANGL:
			switch ( CommandPage ) {
				case 0: (*code)=0x00DC;return;	// Grad
			} break;
		case CMD_SETUP_DISP:
			switch ( CommandPage ) {
				case 0: (*code)=0x00D9;return;	// Norm
			} break;
		case CMD_SETUP_DISP_ENG:
			switch ( CommandPage ) {
				case 0: (*code)=0x00DD;return;	// Eng
			} break;
		case CMD_SETUP_SL:
			switch ( CommandPage ) {
				case 0: (*code)=0xF71E;return;	// S-L-Broken
			} break;

		default:
			break;
	}
	(*code)=0;
	return;
}
