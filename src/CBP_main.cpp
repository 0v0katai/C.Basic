extern "C" {

/*
===============================================================================

 C.Basic for CG ver 1.xx beta

 copyright(c)2015/2016/2017/2018/2019 by sentaro21
 e-mail sentaro21@pm.matrix.jp

===============================================================================
*/
#include "prizm.h"
#include "CBP.h"
#include "SH7305_CPG_BSC.h"
#include "Ptune2_direct.h"

//----------------------------------------------------------------------------------------------

#define P7305_EXTRA_TMU5_COUNT 0xA44D00D8
int BenchDD() {
	int TMU5_COUNT;
	TMU5_COUNT = *(unsigned int*)P7305_EXTRA_TMU5_COUNT;
	Bdisp_PutDisp_DD();
	TMU5_COUNT = TMU5_COUNT - *(unsigned int*)P7305_EXTRA_TMU5_COUNT ;
	return 3276800/TMU5_COUNT;
}

void CG20_overclock(){
	int dd = BenchDD();
	char buffer[32];
//	if ( dd > 20000 ) IsEmu=1;	// over 200fps is emulator
	if ( *(int*)0x8001FFD0 == -1 ) IsEmu=EmuRAMdiff;	// is emulator
	if ( CB_disableOC ) return ;
	if ( dd < 5000 ) {	//  < 50fps
		   sprintf3( buffer, "RefreshRate%3dfps",dd/100);
		if ( YesNo2( buffer, "Auto OverClock ?" ) ) LoadDataF3();	// ->118MHz
	}
}


#define BE_MAX 32

typedef struct{	//
	char sname[12];
	int execptr;
}beFiles;


//****************************************************************************
int main() {
	int key;
	char *ptr,*stat;
	int i,j,run;

	char *src;
	char filename[112];
	char sname[16];
	beFiles befiles[BE_MAX];
	char clipbuffer[CLIPMAX2+1+4];
	double traceAryR[386*8+4];
	char wbuf[0x40000+16];	// work buffer	256KB
	
	WorkBuf = (char*)wbuf;

	Set_Timer_id();
	
	SetVeiwWindowInit();
	for ( i=0; i<6; i++) VWinflag[i]=0;
//	Previous_X=1e308; Previous_Y=1e308; 	// ViewWindow Previous XY init
//	Previous_PX=-1;   Previous_PY=-1; 		// ViewWindow Previous PXY init
	LoadConfig();
	CB_INT=0;	// double mode default
	
	PictAry[0]=(unsigned char*)GetVRAMAddress();
	IsCG20=0;	// CG50
	if ( (int)PictAry[0] != 0xAC000000 ) {
		IsCG20=1;	// CG10/20
		IsHiddenRAM=0;
		UseHiddenRAM=0;
		CG20_overclock();
	}
	HiddenRAM();	// Check HiddenRAM
	
	HeapRAM = (char *)malloc( MAXHEAP*1024 );						// 127KB C.Basic area (program & Mat)
	HeapRAM_malloc( MaxHeapRam );
//	HeapRAM = (char *)realloc( HeapRAM, MAXHEAP1*1024+512 );		// ???KB C.Basic area (program & Mat)

//	HiddenRAM_MatAryStore();	// MatAry ptr -> HiddenRAM
//	HiddenRAM_MatAryInit();		// RAM Initialize

	TVRAM = (char*)PictAry[0]+0x000F0000;

	GVRAM = TVRAM+0x28800;
	memset( GVRAM, 0xFFFF, 0x28800);
	BufVRAM = (char*)GetVRAMWorkBuffer();
	memset( BufVRAM, 0, 0x28800);

	PictBuf = GVRAM;

	ClipBuffer = clipbuffer;		// stack
	ClipBuffer[0] = '\0';
	traceAry   = traceAryR;

//	StoPict( 1 );
	
	ProgNo=0;
	strncpy(ProgfileAdrs[ProgNo]+0x3C,(const char*)"",8);

	CB_AliasVarClr();
	DeletePictPtr();
	ClearExtFontflag();

	memset( befiles[0].sname, 0, sizeof(beFiles)*(BE_MAX) );

	while (1) {
		EnableColor( 1 );
		CB_BackPict=0;				// back image
		CB_ColorIndex=-1;			// current color index reset
		CB_BackColorIndex=0xFFFF;	// Back color index (default White)
		CB_FrameColor=0xFFFF;		// current frame color (default White)
		DrawFrame( CB_FrameColor );

		for (i=0; i<=ProgMax; i++) {
			ProgfileAdrs[i]=NULL;	// Prog Entry clear
			ProgfileEdit[i]=0;		// Prog Edit flag clear
			for (j=0; j<ArgcMAX; j++)	ProgLocalVar[i][j]=-1;
		}

		if ( ( UseHiddenRAM ) && ( IsHiddenRAM ) ) {		// hidden RAM init
			HiddenRAM_Top  =(char*)(HIDDENRAM_TOP+16+256 -(IsEmu));	// Hidden RAM TOP
		} else {		// use heap RAM
			HiddenRAM_Top  = HeapRAM;						// Heap RAM TOP
		}
		HiddenRAM_ProgNextPtr = HiddenRAM_Top;			// RAM Prog next ptr
//		HiddenRAM_freeProg(HiddenRAM_Top);	// Prog memory init

		CB_INT = CB_INTDefault;
		ExecPtr=0;	
		DebugMode=0;
		DebugScreen=0;
		ForceDebugMode=0;
		ForceReturn=0;
		CB_INT = CB_INTDefault;
		MatBase= MatBaseDefault;

		InitLocalVar();		// init Local variable
		CB_TryEnd();		// clear TryFlag
	
		DeleteStrBuffer();
//		DeletePictPtr();
		BG_Buf = NULL;
		
		HelpText = NULL;
		
		if ( ExitDebugModeCheck ) ExitDebugModeCheck=3;
		
		key =( SelectFile( filename ) ) ;
		memset( sname, 0,12 );
		SetShortName( sname, filename) ; 
		switch ( key ) {
			case FileCMD_DebugRUN:
				DebugMode=9; // debug mode start
				ForceDebugMode=1;
				ForceReturn=0;
				goto runjp;
				
			case FileCMD_RUN_F1:
				if ( ForceReturnMode & 1 ) ForceReturn=1;
				goto runjp;

			case FileCMD_RUN:
//			case KEY_CTRL_EXE:
				if ( ForceReturnMode & 2 ) ForceReturn=1;
		runjp:	run=1;
				i=LoadProgfile( filename, 0, EditMaxfree, 1 ) ;
				goto bejmp1;
				break;

			case FileCMD_EDIT:
				run=2;
				i=LoadProgfile( filename, 0, EditMaxfree, 1 ) ;
			  bejmp1:
				ExecPtr=0;
				if ( i==0 )	{
				  bejmp2:
					PP_ReplaceCode( ProgfileAdrs[0] + 0x56 );	//
					ExecPtr=0;
					for (j=0; j<BE_MAX; j++) {
						if ( strncmp( befiles[j].sname, sname, 12) == 0 ) { 
							ExecPtr = befiles[j].execptr;
							break;
						}
					}
					EditRun(run);			// Program listing & edit
				} else
				if ( i==NotfoundProgERR ) { ProgNo=ErrorProg; ExecPtr=ErrorPtr; if (ProgNo>=0) EditRun(2); }	// Program listing & edit
					for (j=0; j<BE_MAX; j++) {
						if ( strncmp( befiles[j].sname, sname, 12) == 0 ) { j++; break; }
					}
					j--;
					if ( j ) memcpy2( befiles[1].sname, befiles[0].sname, sizeof(beFiles)*j );
					strncpy( befiles[0].sname, sname, 12);
					befiles[0].execptr = ExecPtr;
				break;

			case FileCMD_NEW:
				if ( NewProg() ) break ;
				run=2;
				goto bejmp2;
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
			if ( ProgfileAdrs[i] != NULL ) HiddenRAM_freeProg(ProgfileAdrs[0]);		// Prog memory init	
		}	
	}
}


#pragma section _BR_Size
unsigned long BR_Size;
#pragma section

}

