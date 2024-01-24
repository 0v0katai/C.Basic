/*
===============================================================================

 Casio Basic Interpreter (& Compiler) ver 0.99 

 copyright(c)2015/2016 by sentaro21
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

#include "CB_interpreter.h"
#include "CB_error.h"
#include "CB_sample.h"
#include "fx_syscall.h"

//----------------------------------------------------------------------------------------------
int LoadFileSDK( char *src ) {	// test source to SDK
	int size,i;
	char *buffer;

	if ( src[0]=='\0' ) return 0 ;
	size=(src[0x47]&0xFF)*256+(src[0x48]&0xFF)+0x4C;
	locate( 1, 1);
	buffer = (char *)malloc( size*sizeof(char)+EditMaxfree +4 );
	if ( buffer == NULL )  { CB_ErrMsg(MemoryERR); return 1 ; }
	memset( buffer, 0x00,             size*sizeof(char)+EditMaxfree +4 );

	for (i=0;i<size;i++) buffer[i]=src[i];

	ProgfileAdrs[ProgEntryN]= buffer;
	ProgfileMax[ProgEntryN]= SrcSize( buffer ) +EditMaxfree ;
	ProgfileEdit[ProgEntryN]= 1;
	ProgEntryN++;

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
	char *ptr,*stat;
	int i,j,reg,run=0;

	char filename[50];
	char *src;

	SetVeiwWindowInit();
//	Previous_X=1e308; Previous_Y=1e308; 	// ViewWindow Previous XY init
//	Previous_PX=-1;   Previous_PY=-1; 		// ViewWindow Previous PXY init
	LoadConfig();
	CB_INT=0;	// double mode default
	
	ClipBuffer = (char *)malloc( ClipMax+1 );
	if ( ClipBuffer == NULL )  { CB_ErrMsg(MemoryERR); return 1 ; }

	while (1) {
		for (i=0; i<=ProgMax; i++) {
			ProgfileAdrs[i]=NULL;	// Prog Entry clear
			ProgfileEdit[i]=0;		// Prog Edit flag clear
			for (j=0; j<26; j++)	ProgLocalVar[i][j]=-1;
		}
		for (i=1; i<=PictMax; i++) {
			PictAry[i]=NULL;		// Pict ptr clear
		}
		PictAry[0]=GetVRAMAddress();
		
		for ( i=0; i<26; i++ ) LocalDbl[i]=&REGsmall[i];	// local var init
		for ( i=0; i<26; i++ ) LocalInt[i]=&REGINTsmall[i];	// local var init
	
		key =( SelectFile( filename ) ) ;
		switch ( key ) {
			case FileCMD_Prog:				// -- test for SDK (internal sample program)
				ProgEntryN=0;						// Main program
				LoadFileSDK( bas_src );
				LoadFileSDK( bas_src1 );
				LoadFileSDK( bas_src2 );
				LoadFileSDK( bas_src3 );
				LoadFileSDK( bas_src4 );
				LoadFileSDK( bas_src5 );
				DebugMode=0;
				EditRun(2);		// Program listing & edit
				run=0;
				SaveConfig();
				break;
			case KEY_CTRL_EXE:
			case FileCMD_RUN:
				DebugMode=0;
		runjp:	ExecPtr=0;
				i=LoadProgfile( filename, EditMaxfree ) ;
				DebugScreen=0;
				if ( i==0 )	EditRun(1);			// Program run
				else
				if ( i==NotfoundProgERR ) { ProgNo=ErrorProg; ExecPtr=ErrorPtr; if (ProgNo>=0) EditRun(2); }	// Program listing & edit
				SaveConfig();
				break;
			case FileCMD_DebugRUN:
				DebugMode=9; // debug mode start
 				goto runjp;
				break;
			case FileCMD_EDIT:
				DebugMode=0;
				ExecPtr=0;
				i=LoadProgfile( filename, EditMaxfree ) ;
				if ( i==0 )	EditRun(2);			// Program listing & edit
				else
				if ( i==NotfoundProgERR ) { ProgNo=ErrorProg; ExecPtr=ErrorPtr; if (ProgNo>=0) EditRun(2); }	// Program listing & edit
				SaveConfig();
				break;
			case FileCMD_NEW:
				if ( NewProg() ) break ;
				DebugMode=0;
				EditRun(2);			// Program listing & edit
				SaveConfig();
				break;
			case FileCMD_RENAME:
				RenameFile(filename);
				run=0;
				SaveConfig();
				break;
			case FileCMD_DEL:
				DeleteFileFav(filename);
				run=0;
				SaveConfig();
				break;
			case FileCMD_COPY:
				CopyFile(filename);
				run=0;
				SaveConfig();
				break;
			case FileCMD_TEXT:
				ConvertToText(filename);
				run=0;
				SaveConfig();
				break;
			default:
				run=0;
				break;
		}
		
//		for ( reg=0; reg<26; reg++){
//			ptr = MatAry[reg].Adrs ;					// Matrix array ptr*
//			if (ptr != NULL ) free(ptr);		
//			MatAry[reg].SizeA=0;						// Matrix array size
//			MatAry[reg].SizeB=0;						// Matrix array size
//			MatAry[reg].Adrs =NULL ;					// Matrix array ptr*		
//		}
		for (i=ProgMax; i>=0; i--) {			// memory free
			if ( ProgfileEdit[i] ) SaveProgfile(i);	// edited file ?
			ptr=ProgfileAdrs[i];
			if ( ptr != NULL ) free(ptr);
			ProgfileAdrs[i]=0;
		}
		for (i=PictMax; i>=1; i--) {			// memory free
			ptr=(char*)PictAry[i];
			if ( ptr != NULL ) free(ptr);
			PictAry[i]=0;
		}
		FileListfree();
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

