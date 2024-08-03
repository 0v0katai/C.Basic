extern "C" {

/*
===============================================================================

 C.Basic for CG ver 0.10

 copyright(c)2015/2016/2017 by sentaro21
 e-mail sentaro21@pm.matrix.jp

===============================================================================
*/
#include "prizm.h"
#include "CBP.h"
#include "SH7305_CPG_BSC.h"
#include "Ptune2_direct.h"

//----------------------------------------------------------------------------------------------

void CG20_overclock(){
	if ( 
		( BSC.CS0WCR.BIT.WR == WAIT_3 ) &&
		( BSC.CS2WCR.BIT.WR == WAIT_2 ) &&
		( CPG.FLLFRQ.LONG   == FLLFRQ_default ) &&
		( CPG.FRQCRA.LONG   == FRQCRA_default ) &&
		( BSC.CS0BCR.LONG   == CS0BCR_default ) &&
		( BSC.CS2BCR.LONG   == CS2BCR_default ) &&
		( BSC.CS0WCR.LONG   == CS0WCR_default ) &&
		( BSC.CS2WCR.LONG   == CS2WCR_default ) &&
		( BSC.CS5ABCR.LONG  == CS5aBCR_default ) &&
		( BSC.CS5AWCR.LONG  == CS5aWCR_default ) ) {
		LoadDataF3();	// 118MHz
	}
}


//****************************************************************************
void G3A_main( void ){
	int key;
	char buffer[32];
	char *ptr,*stat;
	int i,j;

	char filename[50];
	char *src;
	char vbuf[VRAMSIZE];	// VRAM buffer
	char clipbuffer[CLIPMAX2+1+4];

	EnableColor( 1 );
	SetVeiwWindowInit();
	for ( i=0; i<6; i++) VWinflag[i]=0;
//	Previous_X=1e308; Previous_Y=1e308; 	// ViewWindow Previous XY init
//	Previous_PX=-1;   Previous_PY=-1; 		// ViewWindow Previous PXY init
	LoadConfig();
	CB_INT=0;	// double mode default
	
	IsCG20=0;	// CG50
	PictAry[0]=(unsigned char*)GetVRAMAddress();
	if ( (int)PictAry[0] != 0xAC000000 ) {
		IsCG20=1;	// CG10/20
		IsHiddenRAM=0;
		CG20_overclock();
//		MSG2("Not Support CG10/20","Please Restart");
//		while (1) GetKey(&key);
	}
	HiddenRAM();	// Check HiddenRAM
//	if ( IsHiddenRAM == 0 ) {
//		MSG2("Not Support CG10/20","Please Restart");
//		while (1) GetKey(&key);
//	}

//	HiddenRAM_MatAryStore();	// MatAry ptr -> HiddenRAM
	HiddenRAM_MatAryInit();	// HiddenRAM Initialize

	
	TVRAM = (char*)PictAry[0]+0x000F0000;
//	TVRAM = PictAry[0]+0x00200000;
//	TVRAM = (char *)malloc( 2048+4 );
//	if ( TVRAM == NULL )  { CB_ErrMsg(MemoryERR); return 1 ; }

	GVRAM = TVRAM+0x28800;
	BufVRAM = vbuf;
//	memset( BufVRAM, 0, 0x28800);
//	GVRAM = TVRAM+1024;;
//	if ( TVRAM == NULL )  { CB_ErrMsg(MemoryERR); return 1 ; }

//	ClipBuffer = (char *)malloc( ClipMax+1+4 );		// normal heap
	ClipBuffer = clipbuffer;		// stack
//	if ( ClipBuffer == NULL )  { CB_ErrMsg(MemoryERR); return 1 ; }

	traceAry = (double *)malloc( 386*8+4 );
//	if ( traceAry == NULL )  { CB_ErrMsg(MemoryERR); return 1 ; }

//	if ( StorageMode ) StorageMode = CheckSD() ; // SD mode
	
	StoPict( 1 );	// pre malloc ( pict 1 array )
//	calc_cordic_kvalue();

	while (1) {
		for (i=0; i<=ProgMax; i++) {
			ProgfileAdrs[i]=NULL;	// Prog Entry clear
			ProgfileEdit[i]=0;		// Prog Edit flag clear
			for (j=0; j<26; j++)	ProgLocalVar[i][j]=-1;
		}

		CB_ColorIndex=-1;				// current color index reset
		CB_BackColorIndex=0xFFFF;		// Back color index (default White)
		InitLocalVar();		// init Local variable
		CB_AliasVarClr();
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
//				RenameDirectorys(filename);
//				break;
			default:
				break;
		}
		SaveConfig();
		
		CB_ColorIndex=-1;				// current color index reset
		CB_BackColorIndex=0xFFFF;		// Back color index (default White)
		for (i=ProgMax; i>=0; i--) {			// memory free
			if ( ProgfileEdit[i] ) SaveProgfile(i);	// edited file ?
			ptr=ProgfileAdrs[i];
			if ( ptr != NULL ) 
				HiddenRAM_freeProg(ptr);
			ProgfileAdrs[i]=NULL;
		}
//		for (i=PictbaseMAX-1; i>=0; i--) {			// Pict memory free
//			ptr=(char*)Pictbase[i];
//			if ( ptr != NULL )
//				free(ptr);
//			Pictbase[i]=NULL;
//		}
	}
}

#pragma section _BR_Size
unsigned long BR_Size;
#pragma section

}

