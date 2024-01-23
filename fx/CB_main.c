/*
===============================================================================

 Casio Basic Interpreter (& Compiler) ver 0.40.00 

 copyright(c)2015 by sentaro21
 e-mail sentaro21@pm.matrix.jp

===============================================================================
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <fxlib.h>
#include "CB_io.h"
#include "CB_inp.h"
#include "CB_file.h"
#include "CB_edit.h"

#include "CB_lib.h"
#include "CB_interpreter.h"
#include "CB_error.h"
#include "CB_sample.h"

//----------------------------------------------------------------------------------------------
int LoadFileSDK( unsigned char *src ) {
	int size,i;
	unsigned char *buffer;

	if ( src[0]=='\0' ) return 0 ;
	size=(src[0x47]&0xFF)*256+(src[0x48]&0xFF)+0x4C;
	locate( 1, 1);
	buffer = (unsigned char *)malloc( size*sizeof(char)+EditMaxfree +4 );
	memset( buffer, 0x00,             size*sizeof(char)+EditMaxfree +4 );
	if ( buffer == NULL )  { CB_ErrNo(MemoryERR); return 1 ; }

	for (i=0;i<size;i++) buffer[i]=src[i];

	ProgfileAdrs[ProgEntryN]= buffer;
	ProgfileMax[ProgEntryN]= SrcSize( buffer ) +EditMaxfree ;
	ProgfileEdit[ProgEntryN]= 1;
	ProgEntryN++;
	CB_PreProcess( buffer + 0x56 );

	return 0 ;
}

//----------------------------------------------------------------------------------------------

//****************************************************************************
//  AddIn_main (Sample program main function)
//
//  param   :   isAppli   : 1 = This application is launched by MAIN MENU.
//                        : 0 = This application is launched by a strip in eACT application.
//
//              OptionNum : Strip number (0~3)
//                         (This parameter is only used when isAppli parameter is 0.)
//
//  retval  :   1 = No error / 0 = Error
//
//****************************************************************************
int AddIn_main(int isAppli, unsigned short OptionNum)
{
	unsigned int key;
	char buffer[32];
	unsigned char *ptr,*stat;
	int i,reg,run=0;

	char filename[50];
	char *src;

	SetVeiwWindowInit();
//	Previous_X=1e308; Previous_Y=1e308; 	// ViewWindow Previous XY init
//	Previous_PX=-1;   Previous_PY=-1; 		// ViewWindow Previous PXY init

	while (1) {
		for (i=0; i<=ProgMax; i++) {
			ProgfileAdrs[i]=NULL;	// Prog Entry clear
			ProgfileEdit[i]=0;		// Prog Edit flag clear
		}
		key =( SelectFilefree( filename ) ) ;
		switch ( key ) {
			case KEY_CHAR_POWROOT:				// -- test for SDK (internal sample program)
				ProgEntryN=0;						// Main program
				LoadFileSDK( bas_src );
				LoadFileSDK( bas_src1 );
				LoadFileSDK( bas_src2 );
				LoadFileSDK( bas_src3 );
				LoadFileSDK( bas_src4 );
				LoadFileSDK( bas_src5 );
				EditRun(2);		// Program listing & edit
				run=0;
				break;
			case KEY_CTRL_EXE:
			case KEY_CTRL_F1:
				if ( LoadProgfile( filename ) ) break;
				EditRun(1);			// Program run
				break;
			case KEY_CTRL_F2:
				if ( LoadProgfile( filename ) ) break;
				EditRun(2);			// Program listing & edit
				break;
			case KEY_CTRL_F3:
				if ( NewProg() ) break ;
				EditRun(2);			// Program listing & edit
				break;
			case KEY_CTRL_F4:
				RenameFile(filename);
				run=0;
				break;
			case KEY_CTRL_F5:
				DeleteFile(filename,1);
				run=0;
				break;
			default:
				run=0;
				break;
		}
		
//		for ( reg=0; reg<26; reg++){
//			ptr = MatAry[reg] ;							// Matrix array ptr*
//			if (ptr != NULL ) free(ptr);		
//			MatArySizeA[reg]=0;							// Matrix array size
//			MatArySizeB[reg]=0;							// Matrix array size
//			MatAry[reg]=NULL ;							// Matrix array ptr*		
//		}
		for (i=ProgMax; i>=0; i--) {			// memory free
			if ( ProgfileEdit[i] ) SaveProgfile(i);	// edited file ?
			ptr=ProgfileAdrs[i];
			if ( ptr != NULL ) free(ptr);
			ProgfileAdrs[i]=0;
		}
	}
}


//****************************************************************************
//**************                                              ****************
//**************                 Notice!                      ****************
//**************                                              ****************
//**************  Please do not change the following source.  ****************
//**************                                              ****************
//****************************************************************************


#pragma section _BR_Size
unsigned long BR_Size;
#pragma section


#pragma section _TOP

//****************************************************************************
//  InitializeSystem
//
//  param   :   isAppli   : 1 = Application / 0 = eActivity
//              OptionNum : Option Number (only eActivity)
//
//  retval  :   1 = No error / 0 = Error
//
//****************************************************************************
int InitializeSystem(int isAppli, unsigned short OptionNum)
{
    return INIT_ADDIN_APPLICATION(isAppli, OptionNum);
}

#pragma section

