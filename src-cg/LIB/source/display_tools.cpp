extern "C" {

#include "disp_tools.hpp"
#include "display_tools.h"
#include "keyboard_syscalls.h"
#include "keyboard.hpp"
#include "StrList.hpp"
#include "LineEditors.hpp"
#include "convert_tools.h"
#include "convert_syscalls.h"
#include "STD_syscalls.h"
#include "STD_tools.h"
#include "system_syscalls.h"

#define ssALPHASHIFT 0x14
#define ssINSOVER 0x15

//
TMsgRes EditDialog( TEditDialog edlg, void*edlgwork ){
TMsgRes result = mrNONE;
int x, y, h;
int key, i;
int top,bottom;
unsigned char buffer[ 20 ];
int xpos = 0;
unsigned char kbdstatus, oldalphashift;
TEditMode edm;
int oldflags;

	oldalphashift = GetSetupSetting( ssALPHASHIFT );
	oldflags = DefineStatusAreaFlags( 2, 0, 0, 0 );
	DefineStatusAreaFlags( 3, SAF_BATTERY|SAF_ALPHA_SHIFT|SAF_GLYPH, 0, 0 );

	h = 3;
	BoxYLimits( h, &top, &bottom );
	top += 8;
	x = BOX_XMIN + 1;
	y = top;		   
	
// initialize	
	switch ( edlg ){
		case edSTR :
			SetSetupSetting( ssALPHASHIFT, 0x84 );
		case edHEXSTR :
		case edHEX :
		default :
			kbdstatus = GetSetupSetting( ssINSOVER );
			SetSetupSetting( ssINSOVER, 1 );
			MsgBoxPush( h );
			break;
	}
	DisplayStatusArea();

// edit	
	edm = emANY;
	switch ( edlg ){
		case edHEXSTR :
			edm = emHEX;
		case edSTR :
			if ( (*(TedSTR*)edlgwork).pretext ){
				locate_OS( 4, 3 );
				PrintLine( (*(TedSTR*)edlgwork).pretext, 18 );
			}
			key = InputAny( (*(TedSTR*)edlgwork).buffer, 4, 4, (*(TedSTR*)edlgwork).maxlen, xpos, edm );
			if ( key == KEY_CTRL_EXE ){
				(*(TedSTR*)edlgwork).maxlen = strlen( (char*)buffer );
				result = mrOK;
			}	
			break;
		case edHEX :
			if ( (*(TedHEX*)edlgwork).pretext ){
				locate_OS( 4, 3 );
				PrintLine( (*(TedHEX*)edlgwork).pretext, 18 );
			}
			LongToAscHex( (*(TedHEX*)edlgwork).value, buffer , 8 );
			key = InputAny( buffer, 4, 4, 8, xpos, emHEX );
			if ( key == KEY_CTRL_EXE ){
				(*(TedHEX*)edlgwork).maxlen = ( strlen( (char*)buffer ) + 1 ) & ~1;	// 2-align
				(*(TedHEX*)edlgwork).value = HexToInt( buffer );
				result = mrOK;
			}
			break;
	}

// finalize	
	switch ( edlg ){
		case edHEXSTR :
		case edSTR :
		case edHEX :
		default :
			MsgBoxPop();
			SetSetupSetting( ssINSOVER, kbdstatus );
			break;
	}
	
	SetSetupSetting( ssALPHASHIFT, oldalphashift );
	DefineStatusAreaFlags( 3, oldflags, 0, 0 );
	DisplayStatusArea();
	
	return result;
}

//
void SetStatusMessage( char*msg, char short_color ){
	DefineStatusMessage( msg, 1, short_color, 7 );
	DisplayStatusArea();
}

//
void StatusArea( char*msg, char short_color ){
	Bdisp_AllClr_VRAM();
	SetStatusMessage( msg, short_color );
}

const short empty[18] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

//
int PrintMiniFix( int x, int y, const char*Msg, const int flags, const short color, const short bcolor ){
int i = 0, dx;
unsigned short width;
void*p;

	while ( Msg[ i ] ){
		p = GetMiniGlyphPtr( Msg[ i ], &width );
		dx = ( 12 - width ) / 2;
		if ( dx > 0 ) {
			PrintMiniGlyph( x, y, (void*)empty, flags, dx, 0, 0, 0, 0, color, bcolor, 0 );
		}else dx = 0;
		PrintMiniGlyph( x+dx, y, p, flags, width, 0, 0, 0, 0, color, bcolor, 0 );
		if ( width+dx < 12 ){
			PrintMiniGlyph( x+width+dx, y, (void*)empty, flags, 12-width-dx, 0, 0, 0, 0, color, bcolor, 0 );
		}
		x += 12;
		i++;
	}
	return x;
}

//
TMsgRes MessageDlg( void*Msg, TMsg msgtyp, int height ){
TMsgRes result = mrNONE;
int top,bottom;
int x, y, h;
int key, i;
char*p;
char hb[20];
unsigned char*heading;
unsigned short color;
	switch ( msgtyp ){
		case mtDebugInt :
		case mtDebugStr :
			h = 1;
			break;
		default :
			h = StrListCnt( (unsigned char*)Msg ) + 1;
			break;
	}		
	if ( h < height ) h = height;
	BoxYLimits( h, &top, &bottom );
	top += 8;
	x = BOX_XMIN + 1;
	y = top;		   
	heading = 0;
	color = 0;
	MsgBoxPush( h );
	switch ( msgtyp ){
		case mtConfirmation :
			heading = "Confirmation!";
			color = 0x001F;
			break;
		case mtWarning :
			heading = "Warning!";
			color = 0x07E0;
			break;
		case mtError :
			heading = "Error!";
			color = 0xF800;
			break;
		case mtInformation :
			heading = "Info!";
			color = 0x07FF;
			break;
		case mtDebugInt :
			heading = "DebugInt!";
			color = 0xF81F;
			break;
		case mtDebugStr :
			heading = "DebugStr!";
			color = 0xF81F;
			break;
	}
	if ( heading ){
		PrintMini( &x, &y, heading, 0, 
					PMF_UPPER_X_LIMIT_IGNORED, 
					(int)0, (int)0, 
					color, (int)0xFFFF, (int)1,  (int)0 );
	}
	if ( Msg ) {
		switch ( msgtyp ){
			case mtDebugInt :
				LongToAscHex( *(unsigned int*)Msg, (unsigned char*)hb, 8 );
				x = BOX_XMIN + 1;
				y += 18;		   
				PrintMini( &x, &y, (unsigned char*)hb, 0, 
							PMF_UPPER_X_LIMIT_IGNORED, 
							(int)0, (int)0, 
							(int)0x0000, (int)0xFFFF, (int)1,  (int)0 );
				break;
			case mtDebugStr :
				x = BOX_XMIN + 1;
				y += 18;		   
				PrintMini( &x, &y, (unsigned char*)Msg, 0, 
							PMF_UPPER_X_LIMIT_IGNORED, 
							(int)0, (int)0, 
							(int)0x0000, (int)0xFFFF, (int)1,  (int)0 );
				break;
			default :
				p = (char*)Msg;
				while( p[ 0 ] ){
					x = BOX_XMIN + 1;
					y += 18;		   
					PrintMini( &x, &y, (unsigned char*)p, 0, 
								PMF_UPPER_X_LIMIT_IGNORED, 
								(int)0, (int)0, 
								(int)0x0000, (int)0xFFFF, (int)1,  (int)0 );
					p = (char*)StrListNext( (unsigned char*)p );
				}				
				break;
		}		
	}				
	switch ( msgtyp ){
		case mtConfirmation :
			x = BOX_XMIN + 1;
			y += 18;		   
			PrintMini( &x, &y, "F1: Yes; F6: No; EXIT: cancel", 0, 
						PMF_UPPER_X_LIMIT_IGNORED, 
						(int)0, (int)0, 
						(int)0x0000, (int)0xFFFF, (int)1,  (int)0 );
			break;
			
		case mtDebugStr :
		case mtDebugInt :
		case mtWarning :
		case mtError :
		case mtInformation :
			x = BOX_XMIN + 1;
			y += 18;		   
			PrintMini( &x, &y, "EXIT to return", 0, 
						PMF_UPPER_X_LIMIT_IGNORED, 
						(int)0, (int)0, 
						(int)0x0000, (int)0xFFFF, (int)1,  (int)0 );
			break;
	}

	while ( result == mrNONE ){
		GetKey( &key );
		switch ( key ){
			case KEY_CTRL_F1 :
				if ( msgtyp == mtConfirmation ) result = mrYES;
				break;
			case KEY_CTRL_F6 :
				if ( msgtyp == mtConfirmation ) result = mrNO;
				break;
			case KEY_CTRL_EXIT :
			case KEY_CTRL_QUIT :
			case KEY_CTRL_AC :
				result = mrCANCEL;
				break;
		}
	}

	MsgBoxPop();
	return result;
}
}
