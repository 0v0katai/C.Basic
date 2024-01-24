/*
===============================================================================

 Casio Basic Interpreter (& Compiler) ver 1.40 

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
#include "CB_Str.h"

#include "CB_interpreter.h"
#include "CB_error.h"
#include "CB_setup.h"
#include "CB_Kana.h"
#include "fx_syscall.h"

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
//	char buffer[32];
	char *ptr,*stat;
	int i,j;

	char filename[50];
	char *src;

	TVRAM = (char *)malloc( 1024 + 1024 + 128*8*4  +4 );
	if ( TVRAM == NULL )  { CB_ErrMsg(MemoryERR); return 1 ; }
	GVRAM = TVRAM+1024;
	ExtAnkFontFX     =(unsigned char *)GVRAM         + 1024  ;		// Ext Ascii font
	ExtAnkFontFXmini =(unsigned char *)ExtAnkFontFX    + 128*8 ;		// Ext Ascii font
	ExtKanaFontFX    =(unsigned char *)ExtAnkFontFXmini+ 128*8 ;		// Ext Kana & Gaiji font
	ExtKanaFontFXmini=(unsigned char *)ExtKanaFontFX   + 128*8 ;		// Ext Kana & Gaiji font

	AliasVarCode   =(ALIAS_VAR *)malloc( sizeof(ALIAS_VAR)*ALIASVARMAX    );		// 
	AliasVarCodeMat=(ALIAS_VAR *)malloc( sizeof(ALIAS_VAR)*ALIASVARMAXMAT );		// 
	AliasVarCodeLbl=(ALIAS_VAR *)malloc( sizeof(ALIAS_VAR)*ALIASVARMAXLBL );		// 

	IsSH3=CPU_check();
	SetVeiwWindowInit();
	for ( i=0; i<6; i++) VWinflag[i]=0;
//	Previous_X=1e308; Previous_Y=1e308; 	// ViewWindow Previous XY init
//	Previous_PX=-1;   Previous_PY=-1; 		// ViewWindow Previous PXY init
	LoadConfig();
	CB_INT=0;	// double mode default
	
	HiddenRAM();	// Check HiddenRAM
//	HiddenRAM_MatAryStore();	// MatAry ptr -> HiddenRAM
	HiddenRAM_MatAryInit();	// HiddenRAM Initialize

	ClipBuffer = (char *)malloc( ClipMax+1 );		// normal heap
	traceAry = (double *)malloc( 130*8+4 );
//	if ( traceAry == NULL )  { CB_ErrMsg(MemoryERR); return 1 ; }
	
	if ( StorageMode ) StorageMode = CheckSD() ; // SD mode
	
	PictAry[0]=GetVRAMAddress();

	InitLocalVar();		// init Local variable
	CB_AliasVarClr();

	while (1) {
		for (i=0; i<=ProgMax; i++) {
			ProgfileAdrs[i]=NULL;	// Prog Entry clear
			ProgfileEdit[i]=0;		// Prog Edit flag clear
			for (j=0; j<26; j++)	ProgLocalVar[i][j]=-1;
		}

		ExecPtr=0;	
		DebugMode=0;
		DebugScreen=0;
		ForceDebugMode=0;
		ForceReturn=0;
		
		PictbasePtr=-1;
		PictbaseCount=PictbaseCountMAX;
		for (i=1; i<=PictMax; i++) {
			if ( !( (UseHiddenRAM&0xF0) && ( UseHiddenRAM ) && ( IsHiddenRAM ) ) )
			PictAry[i]=NULL;		// Pict ptr clear
		}
		
		key =( SelectFile( filename ) ) ;
		switch ( key ) {
			case FileCMD_DebugRUN:
				DebugMode=9; // debug mode start
				ForceDebugMode=1;
			case FileCMD_RUN_F1:
				if ( ForceReturnMode & 1 ) ForceReturn=1;
				goto runjp;
			case FileCMD_RUN:
//			case KEY_CTRL_EXE:
				if ( ForceReturnMode & 2 ) ForceReturn=1;
		runjp:
				i=LoadProgfile( filename, 0, EditMaxfree, 1 ) ;
				if ( i==0 )	{
					PP_ReplaceCode( ProgfileAdrs[0] + 0x56 );	//
					ExecPtr=0;
					EditRun(1);			// Program run
				}else
				if ( i==NotfoundProgERR ) { ProgNo=ErrorProg; ExecPtr=ErrorPtr; if (ProgNo>=0) EditRun(2); }	// Program listing & edit
				break;
			case FileCMD_EDIT:
				ExecPtr=0;
				i=LoadProgfile( filename, 0, EditMaxfree, 1 ) ;
				if ( i==0 )	{
					PP_ReplaceCode( ProgfileAdrs[0] + 0x56 );	//
					ExecPtr=0;
					EditRun(2);			// Program listing & edit
				} else
				if ( i==NotfoundProgERR ) { ProgNo=ErrorProg; ExecPtr=ErrorPtr; if (ProgNo>=0) EditRun(2); }	// Program listing & edit
				break;
			case FileCMD_NEW:
				if ( NewProg() ) break ;
				EditRun(2);			// Program listing & edit
				break;
			case FileCMD_RENAME:
				RenameCopyFile(filename, 0);
				break;
			case FileCMD_DEL:
				DeleteFileFav(filename, 1);
				break;
			case FileCMD_COPY:
				RenameCopyFile(filename, 1);
				break;
			case FileCMD_TEXT:
				ConvertToText(filename);
				break;
			case FileCMD_PASS:
				NewPassWord(filename);
				break;
//			case FileCMD_MKDIR:
//				MakeDirectory();
//				break;
//			case FileCMD_RENDIR:
//				RenameDirectory(filename);
//				break;
			default:
				break;
		}
		SaveConfig();
		
		for (i=ProgMax; i>=0; i--) {			// memory free
			if ( ProgfileEdit[i] ) SaveProgfile(i);	// edited file ?
			ptr=ProgfileAdrs[i];
			if ( ptr != NULL ) 
				HiddenRAM_freeProg(ptr);
			ProgfileAdrs[i]=NULL;
		}
		for (i=PictbaseMAX-1; i>=0; i--) {			// Pict memory free
			ptr=(char*)Pictbase[i];
			if ( ptr != NULL )
				free(ptr);
			Pictbase[i]=NULL;
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

