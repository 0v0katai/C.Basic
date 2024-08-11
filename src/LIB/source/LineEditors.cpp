extern "C" {

#include "keyboard.hpp"
#include "keyboard_syscalls.h"
#include "disp_tools.hpp"

#include "LineEditors.hpp"

//
int TransformEditKey( int*key, TEditMode edMode, int*control ){
int result = 0;
int loc_key = *key;
int newkey = 0;
	if ( *control ){
		switch ( edMode ){
			case emANY : 
			case emINTEGER : 
				result = 1;
				break;
			case emHEX : 
				if ( loc_key == KEY_CTRL_XTT ){
					*key = (int)'A';
					*control = 0;
					result = 1;
				}else{
					result = 1;
				}
				break;
		}
	}else{
		switch ( edMode ){
			case emANY : 
				result = 1;
				break;
			case emINTEGER : 
				if ( loc_key >= (int)'0' )
					if ( loc_key <= (int)'9' ) result = 1;
				break;
			case emHEX : 
				if ( loc_key >= (int)'0' )
					if ( loc_key <= (int)'9' ){
						result = 1;
						break;
				}	
				switch ( loc_key ){
					case KEY_CHAR_LOG :
						newkey = (int)'B';
						break;
					case KEY_CHAR_LN :
						newkey = (int)'C';
						break;
					case KEY_CHAR_SIN :
						newkey = (int)'D';
						break;
					case KEY_CHAR_COS :
						newkey = (int)'E';
						break;
					case KEY_CHAR_TAN :
						newkey = (int)'F';
						break;
					default :	
						if ( loc_key >= (int)'A' )
							if ( loc_key <= (int)'F' ) result = 1;
						break;
				}
				if ( newkey ){
					*key = newkey;
					result = 1;
				}
				break;
		}
	};
	return result;
}

//
int InputAny( unsigned char*buffer, int x, int y, int posmax, int&xpos, TEditMode edMode ){
int	result = 0;
int key;
int xstart = 0;
int pos_to_clear = x + posmax - 1;
int control;
int lDisplay = 1;
	locate_OS( x-1, y );
	Print( (unsigned char *)">" );
	locate_OS( pos_to_clear+1, y );
	Print( (unsigned char *)"<" );
	while( !result ){
		if ( lDisplay ){
			if ( xpos >= posmax ) xpos = posmax - 1; // prevent the input field from left auto scrolling
			DisplayMBString2( 0, buffer, 0, xpos, 0, x, (y-1)*24, pos_to_clear, 0, 2 ); 
//			DisplayMBString( buffer, 0, xpos, x, y ); 
			lDisplay = 0;
		}
		control = !GetKey( &key );
		if ( TransformEditKey( &key, edMode, &control ) ){
			if ( control ){
				switch (key){
					case KEY_CTRL_EXIT : 
					case KEY_CTRL_EXE : 
						result = key;
						break;
					default:
	//					EditMBStringCtrl( buffer, posmax, &xstart, &xpos, &key, x, y );
						EditMBStringCtrl2( buffer, posmax, &xstart, &xpos, &key, x, (y-1)*24, 1, pos_to_clear );
	//					EditMBStringCtrl3( hb, posmax, &xstart, &xpos, &key, x, (y-1)*24, 0, 21, 3 );
	//					EditMBStringCtrl4( hb, posmax, &xstart, &xpos, &key, x, (y-1)*24, 0, 21, 3, 0 );
				}
			}else{
				xpos = EditMBStringChar( buffer, posmax, xpos, key );
				lDisplay = 1;
			}
		}
	}
	Cursor_SetFlashOff();
	return result;
}

}