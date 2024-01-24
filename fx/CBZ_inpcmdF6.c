#include "CB_inp.h"

//----------------------------------------------------------------------------------------------
void GetGenuineCmdF6( unsigned int *code ){
	switch ( CommandType ) {
		case CMD_PRGM:		//	------------------------------------------------------------PRGM_F6
			switch ( CommandPage ) {
				case 0: CommandPage=1;break;
				case 1: CommandPage=2;break;
				case 2: CommandPage=0;break;
			} break;
		case CMD_PRGM_COM:
			switch ( CommandPage ) {
				case 0: CommandPage=1;break;
				case 1: CommandPage=2;break;
				case 2: CommandPage=3;break;
				case 3: CommandPage=0;break;
			} break;
		case CMD_PRGM_CTL:
			switch ( CommandPage ) {
				case 0: CommandPage=1;break;
				case 1: CommandPage=0;break;
			} break;
		case CMD_PRGM_JUMP:
			switch ( CommandPage ) {
				case 0: (*code)=0xF79E;return;	// Menu
			} break;
		case CMD_PRGM_DISP:
			switch ( CommandPage ) {
				case 0: (*code)=0xF7E4;return;	// Disp
			} break;
		case CMD_PRGM_REL:
			switch ( CommandPage ) {
				case 0: (*code)=0x0010;return;	// <=
			} break;
		case CMD_PRGM_I_O:
			switch ( CommandPage ) {
				case 0: CommandPage=1;break;
				case 1: CommandPage=0;break;
			} break;
		case CMD_PRGM_STR:
			switch ( CommandPage ) {
				case 0: CommandPage=1;break;
				case 1: CommandPage=2;break;
				case 2: CommandPage=3;break;
				case 3: CommandPage=0;break;
			} break;
		case CMD_PRGM_EXEC:
			switch ( CommandPage ) {
				case 0: CommandPage=1;break;
				case 1: CommandPage=0;break;
			} break;
			
		case CMD_OPTN:		//	------------------------------------------------------------OPTN_F6
			switch ( CommandPage ) {
				case 0: CommandPage=1;break;
				case 1: CommandPage=2;break;
				case 2: CommandPage=0;break;
			} break;
		case CMD_OPTN_LIST:
			switch ( CommandPage ) {
				case 0: CommandPage=1;break;
				case 1: CommandPage=2;break;
				case 2: CommandPage=0;break;
			} break;
		case CMD_OPTN_MAT:
			switch ( CommandPage ) {
				case 0: CommandPage=1;break;
				case 1: CommandPage=0;break;
			} break;
		case CMD_OPTN_CALC:
			switch ( CommandPage ) {
				case 1: CommandPage=2;break;
				case 2: CommandPage=1;break;
			} break;
		case CMD_OPTN_HYP:
			switch ( CommandPage ) {
				case 0: (*code)=0x00B3;return;	// arctanh
			} break;
		case CMD_OPTN_NUM:
			switch ( CommandPage ) {
				case 0: CommandPage=1;break;
				case 1: CommandPage=0;break;
			} break;
		case CMD_OPTN_ANGL:
			switch ( CommandPage ) {
				case 0: CommandPage=1;break;
				case 1: CommandPage=0;break;
			} break;
		case CMD_OPTN_ESYM:
			switch ( CommandPage ) {
				case 0: CommandPage=1;break;
				case 1: CommandPage=2;break;
				case 2: CommandPage=0;break;
			} break;
		case CMD_OPTN_LOGIC:
			switch ( CommandPage ) {
				case 0: CommandPage=1;break;
				case 1: CommandPage=0;break;
			} break;
		case CMD_OPTN_EXT:
			switch ( CommandPage ) {
				case 0: (*code)='%';return;	// '%'
			} break;
			
		case CMD_VARS:		//	------------------------------------------------------------VARS_F6
			switch ( CommandPage ) {
				case 0: CommandPage=1;break;
				case 1: CommandPage=0;break;
			} break;
		case CMD_VARS_EXT:
			switch ( CommandPage ) {
				case 0: (*code)=0xF7FA;return;	// RefreshTime
			} break;
			
		case CMD_SHIFT:		//	-----------------------------------------------------------SHIFT_F6
			switch ( CommandPage ) {
			} break;
		case CMD_SHIFT_SKTCH:
			switch ( CommandPage ) {
				case 0: CommandPage=1;break;
				case 1: CommandPage=2;break;
				case 2: CommandPage=0;break;
			} break;
		case CMD_SHIFT_SKTCH_EXT:
			switch ( CommandPage ) {
				case 0: CommandPage=1;break;
				case 1: CommandPage=2;break;
				case 2: CommandPage=0;break;
			} break;
		case CMD_SHIFT_SKTCH_ML:
			switch ( CommandPage ) {
				case 0: CommandPage=1;break;
				case 1: CommandPage=2;break;
				case 2: CommandPage=3;break;
				case 3: CommandPage=4;break;
				case 4: CommandPage=0;break;
			} break;

		case CMD_MENU:		//	------------------------------------------------------------MENU_F6
			switch ( CommandPage ) {
				case 0: (*code)='#';return;	// #
			} break;
		case CMD_MENU_EXT:
			switch ( CommandPage ) {
				case 0: (*code)='/';return;	// /
			} break;
			
		case CMD_SETUP:		//	-----------------------------------------------------------SETUP_F6
			switch ( CommandPage ) {
				case 0: CommandPage=1;break;
				case 1: CommandPage=2;break;
				case 2: CommandPage=3;break;
				case 3: CommandPage=0;break;
			} break;
		default:
			break;
	}
	(*code)=0;
	return;
}


