/* *****************************************************************************
 * CBZ_error.c -- Error handling
 * Copyright (C) 2015-2024 Sentaro21 <sentaro21@pm.matrix.jp>
 *
 * This file is part of C.Basic.
 * C.Basic is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2.0 of the License,
 * or (at your option) any later version.
 *
 * C.Basic is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with C.Basic; if not, see <https://www.gnu.org/licenses/>.
 * ************************************************************************** */

#include "CB.h"

int g_error_ptr=0;
int g_error_prog=0;
int g_error_type;

void ERROR(char *buffer) {
	unsigned int key;
	unsigned int pad = ( strlen(buffer) < 16 );	// Pad error text with length < 16
	char msg[22];

	if ( TryFlag ) return ;
	
	CB_SelectTextDD();
	SaveDisp(SAVEDISP_PAGE1);

//	PopUpWin(5);
//	locate(3,2); Print((unsigned char *)buffer);
//	sprintf(buf," ptr:0x%X(%d)",ErrorPtr,ErrorPtr);
//	locate(3,4); Print((unsigned char *)buf);
//	locate(3,6); Print((unsigned char *) "   Press:[EXIT]");

	PopUpWin(4);
	locate(3+pad,3); Print((unsigned char *)buffer);
	sprintf(msg, "(%d)", g_error_type);
	locate(3,5); Print((unsigned char *)msg);
	locate(14,5); Print((unsigned char *)"[EXIT]");

	Bdisp_PutDisp_DD();

	WaitKeyAC();
	KeyRecover(); 
	while (1) {
		GetKey_DisableMenu(&key);
		if (key == KEY_CTRL_EXIT  ||
		    key == KEY_CTRL_AC    ||
			key == KEY_CTRL_RIGHT ||
			key == KEY_CTRL_LEFT )	break ;
	}
	
	RestoreDisp(SAVEDISP_PAGE1);
	Bdisp_PutDisp_DD();
}

void CB_ErrMsg(int ErrNo) {
	switch (ErrNo) {

		// Casio Basic
		case SyntaxERR:
			ERROR("Syntax ERROR");
			break;
		case MathERR:
			ERROR("Ma ERROR");
			break;
		case GoERR:
			ERROR("Go ERROR");
			break;
		case NestingERR:
			ERROR("Nesting ERROR");
			break;
		case StackERR:
			ERROR("Stack ERROR");
			break;
		case MemoryERR:
			ERROR("Memory ERROR");
			break;
		case ArgumentERR:
			ERROR("Argument ERROR");
			break;
		case DimensionERR:
			ERROR("Dimension ERROR");
			break;
		case RangeERR:
			ERROR("Range ERROR");
			break;
		case NonRealERR:
			ERROR("Non-Real ERROR");
			break;
		case ComERR:
			ERROR("Com ERROR");
			break;
		case TransmitERR:
			ERROR("Transmit ERROR");
			break;
		case ReceiveERR:
			ERROR("Receive ERROR");
			break;
		case TooMuchData:
			ERROR("Too Much Data");
			break;

		// C.Basic
		case MissingNext:
			ERROR("Missing Next");
			break;
		case MissingFor:
			ERROR("Missing For");
			break;
		case MissingWhileEnd:
			ERROR("Missing WhileEnd");
			break;
		case MissingWhile:
			ERROR("Missing While");
			break;
		case MissingLpWhile:
			ERROR("Missing LpWhile");
			break;
		case MissingDo:
			ERROR("Missing Do");
			break;
		case NotInLoop:
			ERROR("Not in Loop");
			break;
		case ZeroDivision:
			ERROR("Zero Division");
			break;
		case UndefinedLabel:
			ERROR("Undefined Label");
			break;
		case NotEnoughMemory:
			ERROR("Not Enough Memory");
			break;
		case StringTooLong:
			ERROR("String Too Long");
			break;
		case UndefinedMatrix:
			ERROR("Undefined Matrix");
			break;
		case InvalidSize:
			ERROR("Invalid Size");
			break;
		case ProgNotFound:
			ERROR("Prog Not Found");
			break;
		case TooManyProg:
			ERROR("Too Many Prog");
			break;
		case MissingIfEnd:
			ERROR("Missing IfEnd");
			break;
		case MissingIf:
			ERROR("Missing If");
			break;
		case MissingSwitchEnd:
			ERROR("Missing SwitchEnd");
			break;
		case MissingSwitch:
			ERROR("Missing Switch");
			break;
		case FileNotFound:
			ERROR("File Not Found");
			break;
	
		case ComClosed:
			ERROR("Com Closed");
			break;
		case InvalidType:
			ERROR("Invalid Type");
			break;
		case OutOfDomain:
			ERROR("Out of Domain");
			break;
		case UndefinedAlias:
			ERROR("Undefined Alias");
			break;
		case VarMemoryFull:
			ERROR("Var Memory Full");
			break; 
		case AssignERR:
			ERROR("Assign ERROR");
			break;
		case AddressERR:
			ERROR("Address ERROR");
			break;
		case TimeOut:
			ERROR("Time Out");
			break;
		case MissingExcept:
			ERROR("Missing Except");
			break;
		case MissingTry:
			ERROR("Missing Try");
			break;

		default:
//		case FileERR:
			ERROR("File ERROR");
			break;
	}
}

void CB_Error(int error_macro) {

	/* exit this function if another error occured earlier */
	if (g_error_type)
		return;
	
	g_error_type = error_macro;
	g_error_ptr = g_exec_ptr;
	g_error_prog = g_current_prog;

	/* mimic Casio Basic behaviour */
	if (error_macro == MathERR)
		g_error_ptr--;
}