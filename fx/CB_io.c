#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <fxlib.h>
#include "fx_syscall.h"

#include "CB_interpreter.h"
#include "CB_edit.h"
#include "CB_io.h"
#include "CB_inp.h"
#include "CB_error.h"
#include "CB_Kana.h"
#include "CB_Matrix.h"
#include "CB_glib.h"
#include "CB_setup.h"
#include "KeyScan.h"
#include "MonochromeLib.h"

struct st_round CB_Round = { Norm , 1} ; // Round
char ENG=0;	// ENG flag  1:ENG  3:3digit separate

char UseHiddenRAM=0;	//	0x11 :HiddenRAMInit off
char IsHiddenRAM=0;

#define PICTBACKSPACE 512	//
#define HIDDENRAM_TOP 0x88040000
char * HiddenRAM_Top  =(char*)0x88040000+16+256;	// Hidden RAM TOP
char * HiddenRAM_End  =(char*)0x88080000-PICTBACKSPACE;	// Hidden RAM END

char * HiddenRAM_ProgNextPtr=(char*)0x88040000+16+256;	// Hidden RAM Prog next ptr
char * HiddenRAM_MatTopPtr =(char*)0x88080000-PICTBACKSPACE;	// Hidden RAM Mat top ptr

char IsSH3;	//	3:SH3   4:SH4

/*
----------------------------------------
HiddenRAM_Top(0x88040000)
		>>>
		History buffer(256byte)
		(prog area) HiddenRAM_ProgNextPtr
			... 
			(free area)
				...
				(Mat area)	HiddenRAM_MatTopPtr
				<<<
					Matrix & List data area
					<<<
					(*MatAry)
					MatAry index area
						<<<
							Pict buffer ( 1~20~99)
							<<<
								HiddenRAM_End(0x88080000)
----------------------------------------
*/
//---------------------------------------------------------------------------------------------
int CPU_check(void) {					// SH3:1 SH4A:0
	return ! ( ( *(volatile unsigned short*)0xFFFFFF80 == 0 ) && ( *(volatile unsigned short*)0xFFFFFF84 == 0 ) ) ;
}

//---------------------------------------------------------------------------------------------
void * HiddenRAM(void){	// Check HiddenRAM 
	volatile unsigned int *NorRAM=(volatile unsigned int*)0xA8000000;	// Nomarl RAM TOP (no cache area)
	volatile unsigned int *HidRAM=(volatile unsigned int*)0x88040000;	// Hidden RAM TOP (cashe area)
	int a,b;
	int K55=0x55555555;
	int KAA=0xAAAAAAAA;
	char * HidAddress=NULL;

	IsHiddenRAM=0;
	a= *NorRAM;
	b= *HidRAM;
	*NorRAM=K55;
	*HidRAM=KAA;
	if ( *NorRAM != *HidRAM ) {
			HidAddress=(char*)HidRAM;	// Hidden RAM Exist
			IsHiddenRAM=1;
	}
	*NorRAM=a;
	*HidRAM=b;
	return HidAddress;
}

void * HiddenRAM_mallocProg( size_t size ){
	char * ptr;
	if ( ( UseHiddenRAM ) && ( IsHiddenRAM ) ) {
		ptr = HiddenRAM_ProgNextPtr;
		HiddenRAM_ProgNextPtr += ( (size+3) & 0xFFFFFFFC );	// 4byte align
		if ( HiddenRAM_ProgNextPtr < HiddenRAM_MatTopPtr ) return ptr;
		HiddenRAM_ProgNextPtr = ptr;
		return malloc( size );
	} else {
		return malloc( size );
	}
}

void * HiddenRAM_mallocMat( size_t size ){
	char * ptr;
	if ( ( UseHiddenRAM ) && ( IsHiddenRAM ) ) {
		ptr = HiddenRAM_MatTopPtr;
		ptr -= ( (size+7) & 0xFFFFFFF8 );	// 8byte align
		if ( ptr < HiddenRAM_ProgNextPtr ) return malloc( size );
		HiddenRAM_MatTopPtr = ptr;
		return ptr;
	} else {
		return malloc( size );
	}
}

unsigned char * GetheapPict(){
	unsigned char *pict;
	if ( PictbaseCount >= PictbaseCountMAX ) {
		if ( PictbasePtr < PictbaseMAX ) Pictbase[++PictbasePtr] = (unsigned char *) malloc( 1024 * PictbaseMAX +4 );
		else { CB_Error(NotEnoughMemoryERR); return NULL; }	// Not enough memory error
		if( Pictbase[PictbasePtr] == NULL ) { CB_Error(NotEnoughMemoryERR); return NULL; }	// Not enough memory error
		PictbaseCount=0;
	}
	pict = Pictbase[PictbasePtr] + 1024 * PictbaseCount;
	PictbaseCount++;
	return pict;
}
unsigned char *  HiddenRAM_mallocPict( int pictNo ){
	char *  ptr;
	if ( ( UseHiddenRAM ) && ( IsHiddenRAM ) ) {
		ptr = HiddenRAM_End-1024*(pictNo);
		return (unsigned char *)ptr;
	} else {
		return GetheapPict();
	}
}

void HiddenRAM_freeProg( void *ptr ){
	if ( (int)ptr < (int)HiddenRAM_Top ) free( ptr );
	else 
		HiddenRAM_ProgNextPtr=HiddenRAM_Top;	// Hidden RAM Prog next ptr
}
void HiddenRAM_freeMat( int reg ){
	char *ptr = (char *)MatAry[reg].Adrs;
	int	size = MatAry[reg].Maxbyte; 
	if ( (int)ptr < (int)HiddenRAM_Top ) free( ptr );
	else {
		if ( (int)HiddenRAM_MatTopPtr == (int)ptr ) HiddenRAM_MatTopPtr += ( (size+7) & 0xFFFFFFF8 );
	}
}

const char MatAryCheckStr[]="#CBasic163#";

void HiddenRAM_MatAryStore(){	// MatAry ptr -> HiddenRAM
	int *iptr1=(int*)(HIDDENRAM_TOP+12);
	int *iptr2=(int*)(HiddenRAM_End+12);
	if ( ( UseHiddenRAM ) && ( IsHiddenRAM ) ) {
		memcpy( (char *)HIDDENRAM_TOP, MatAryCheckStr, sizeof(MatAryCheckStr) );
		memcpy( HiddenRAM_End,         MatAryCheckStr, sizeof(MatAryCheckStr) );
		memcpy( HiddenRAM_End+16,      PictAry, sizeof(PictAry) );
		iptr1[0]=(int)MatAry;
		iptr2[0]=(int)HiddenRAM_MatTopPtr;
	}
}
int HiddenRAM_MatAryRestore(){	//  HiddenRAM -> MatAry ptr
	char buffer[10];
	int *iptr1=(int*)(HIDDENRAM_TOP+12);
	int *iptr2=(int*)(HiddenRAM_End+12);
	char *tmp;
	tmp=HiddenRAM_End - 1024*(20+ExtendPict) - sizeof(matary)*MatAryMax ;
	if ( !(UseHiddenRAM&0xF0) )return 0;	// hidden init
	if ( ( UseHiddenRAM ) && ( IsHiddenRAM ) && ( (char *)iptr1[0]==tmp ) ){
		if ( ( strcmp((char *)HIDDENRAM_TOP, MatAryCheckStr) == 0 ) && ( strcmp(HiddenRAM_End, MatAryCheckStr) == 0 ) ) {
			memcpy( PictAry, HiddenRAM_End+16, sizeof(PictAry) );
			HiddenRAM_MatTopPtr=(char*)iptr2[0];
			MatAry=(matary *)tmp;
			return 1;	// ok
		}
	}
	return 0;
}
void HiddenRAM_MatAryInit(){	// HiddenRAM Initialize
	char buffer[10];
	int *iptr1=(int*)(HIDDENRAM_TOP+12);
	int *iptr2=(int*)(HiddenRAM_End+12);
	MatAryMax=MATARY_MAX +ExtendList*52;
	Mattmpreg=MatAryMax-1;
	ExtListMax=MatAryMax-33;
	EditMaxfree = EDITMAXFREE;
	EditMaxProg = EDITMAXPROG;
	NewMax      = NEWMAX;
	ClipMax     = CLIPMAX;
	if ( ( UseHiddenRAM ) && ( IsHiddenRAM ) ) {		// hidden RAM init
		EditMaxfree = EDITMAXFREE2;
		EditMaxProg = EDITMAXPROG2;
		NewMax      = NEWMAX2;
		ClipMax     = CLIPMAX2;
		OplistRecentFreq=(toplistrecentfreq *)(HIDDENRAM_TOP+16);
		OplistRecent    =(short *)(HIDDENRAM_TOP+16+128);
		if ( HiddenRAM_MatAryRestore() ) return ;			// hidden RAM ready
		HiddenRAM_MatTopPtr = HiddenRAM_End - 1024*(20+ExtendPict) - sizeof(matary)*MatAryMax ;
		MatAry = (matary *)HiddenRAM_MatTopPtr;
		HiddenRAM_MatAryStore();
		InitOpcodeRecent();
	} else {		// use heap RAM
		MatAry = ( matary *)malloc( sizeof(matary)*MatAryMax );
		OplistRecentFreq=(toplistrecentfreq *)OplistRecentFreqMem;
		OplistRecent    =(short *)OplistRecentMem;
		InitOpcodeRecent();
	}
	memset( MatAry, 0, sizeof(matary)*MatAryMax );
}

//---------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------
void CB_PrintC_ext( int x, int y,const unsigned char *c, int extflag ){
	if ( ( *c == 0xFF ) || ( *c == 0xE7 ) )	KPrintChar( (--x)*6, (--y)*8, c );
	else {
		if ( ( extflag ) && ( ExtCharAnkFX ) && ( 0x20 <= *c ) && ( *c < 0x7F ) ) KPrintChar( (--x)*6, (--y)*8, c );
		else {
			locate (x,y);
			PrintC( c );
		}
	}
}
void CB_Print_ext( int x, int y, const unsigned char *str, int extflag ){
	int c=(char)*str;
	while ( c ) {
		CB_PrintC_ext( x, y, str++, extflag );
		if ( (c==0x7F)||(c==0xFFFFFFF9)||(c==0xFFFFFFE5)||(c==0xFFFFFFE6)||(c==0xFFFFFFE7)||(c==0xFFFFFFFF) )  str++;
		x++;
		if ( x>21 ) break;
		c=(char)*str;
	}
}
void CB_PrintRevC_ext( int x, int y,const unsigned char *c, int extflag ){
	if ( ( *c == 0xFF ) || ( *c == 0xE7 ) )	KPrintRevChar( (--x)*6, (--y)*8, c );
	else {
		if ( ( extflag ) && ( ExtCharAnkFX ) && ( 0x20 <= *c ) && ( *c < 0x7F ) ) KPrintRevChar( (--x)*6, (--y)*8, c );
		else {
			locate (x,y);
			PrintRevC( c );
		}
	}
}
void CB_PrintRev_ext( int x, int y, const unsigned char *str, int extflag ){
	unsigned int c=(char)*str;
	while ( c ) {
		CB_PrintRevC_ext( x, y, str++, extflag );
		if ( (c==0x7F)||(c==0xFFFFFFF9)||(c==0xFFFFFFE5)||(c==0xFFFFFFE6)||(c==0xFFFFFFE7)||(c==0xFFFFFFFF) )  str++;
		x++;
		if ( x>21 ) break;
		c=(char)*str;
	}
}

void CB_PrintC( int x, int y,const unsigned char *c ){
	CB_PrintC_ext( x, y, c, 0 );
}
void CB_PrintRevC( int x, int y,const unsigned char *c ){
	CB_PrintRevC_ext( x, y, c, 0 );
}
void CB_Print( int x, int y, const unsigned char *str){
	CB_Print_ext( x, y, str, 0 );
}
void CB_PrintRev( int x, int y, const unsigned char *str){
	CB_PrintRev_ext( x, y, str, 0 );
}


void CB_PrintXYC( int px, int py,const unsigned char *c , int mode ){	// mode >0x100 extflag
	if ( ( *c == 0xFF ) || ( *c == 0xE7 ) ) {
		if ( mode & 0xFF )	KPrintRevChar( px, py, c );
		else				KPrintChar( px, py, c );
	} else {
		if ( ( mode & 0xFF00 ) && ( ExtCharAnkFX ) && ( 0x20 <= *c ) && ( *c < 0x7F ) ) {
			if ( mode & 0xFF )	KPrintRevChar( px, py, c );
			else				KPrintChar( px, py, c );
		} else
		PrintXY( px, py, c ,mode & 0xFF );
	}
}
void CB_PrintXY( int px, int py, const unsigned char *str, int mode){	// mode >0x100 extflag
	int c=(char)*str;
	while ( c ) {
		CB_PrintXYC( px, py, str++ , mode);
		if ( (c==0x7F)||(c==0xFFFFFFF9)||(c==0xFFFFFFE5)||(c==0xFFFFFFE6)||(c==0xFFFFFFE7)||(c==0xFFFFFFFF) )  str++;
		px+=6;
		if ( px>127 ) break;
		c=(char)*str;
	}
}


//---------------------------------------------------------------------------------------------
void PrintXYR(int x,int y,char *buffer,int rev){
 	if ( rev ) PrintXY(x,y,(unsigned char *)buffer,1);
		else   PrintXY(x,y,(unsigned char *)buffer,0);
}

void Hex8PrintXY(int x, int y, char *str, unsigned int hex){
	char buffer[22];
	sprintf(buffer,"%08X",hex);
	locate(x,y); Print((unsigned char *)str); Print((unsigned char *)buffer);
}
void Hex4PrintXY(int x, int y, char *str, unsigned int hex){
	char buffer[22];
	sprintf(buffer,"%04X",hex);
	locate(x,y); Print((unsigned char *)str); Print((unsigned char *)buffer);
}
void Hex2PrintXY(int x, int y, char *str, unsigned int hex){
	char buffer[22];
	sprintf(buffer,"%02X",hex);
	locate(x,y); Print((unsigned char *)str); Print((unsigned char *)buffer);
}

int YesNo2sub( char*buffer, char*buffer2){
	unsigned int key;
	int y;

	SaveDisp(SAVEDISP_PAGE1);
	if ( buffer2[0]=='\0' ) {
		PopUpWin(4); 
		y=4;
	} else { 
		PopUpWin(5);
		CB_Print( 3, 3, (unsigned char *)buffer2);
		y=5;
	}
	CB_Print( 3, 2,  (unsigned char *)buffer);
	CB_Print( 3, y,  (unsigned char *) "   Yes:[F1]");
	CB_Print( 3, y+1,(unsigned char *) "   No :[F6]");

	KeyRecover(); 
	while ( 1 ) {
		GetKey(&key);	
		if ( key == KEY_CTRL_F1  ) break ;
		if ( key == KEY_CTRL_F6  ) break ;
		if ( key == KEY_CTRL_EXIT) { while ( KeyCheckEXIT() ) ; break ; }
		if ( key == KEY_CTRL_AC  ) break ;
		if ( key == KEY_CTRL_EXE ) break ;
	}
	RestoreDisp(SAVEDISP_PAGE1);
	if ( key == KEY_CTRL_F1  ) return 1 ; // Yes
	if ( key == KEY_CTRL_EXE ) return 1 ; // Yes
//	Bdisp_PutDisp_DD();
	return 0 ; // No
}

int YesNo( char*buffer){
	int r;
	r=YesNo2sub( buffer, "" );
	return r;
}
int YesNo2( char*buffer, char*buffer2 ){
	int r;
	r=YesNo2sub( buffer, buffer2 );
	return r;
}

void ExitKey(){
	unsigned int key;
	KeyRecover(); 
	while ( 1 ) {
		GetKey(&key);	
		if ( key == KEY_CTRL_EXIT) { while ( KeyCheckEXIT() ) ; break ; }
		if ( key == KEY_CTRL_AC  ) break ;
	}
}
void OkMSGstr2(char*buffer,char*buffer2){
	unsigned int key;
	char buf[20];
	int y;

	SaveDisp(SAVEDISP_PAGE1);
	if ( buffer2[0]=='\0' ) {
		PopUpWin(4); 
		y=5;
	} else { 
		PopUpWin(5);
		CB_Print( 3, 4, (unsigned char *)buffer2);
		y=6;
	}
	CB_Print( 3, 8-y, (unsigned char *)buffer);
	CB_Print( 3, y, (unsigned char *) "   Press:[EXIT]");

	ExitKey();
	RestoreDisp(SAVEDISP_PAGE1);
}

void ErrorMSGstr1( char*buffer){
	OkMSGstr2( buffer, "" );
}
void ErrorMSGstr( char*buffer, char*buffer2 ){
	OkMSGstr2( buffer, buffer2 );
}

void ErrorMSG(char*buffer,int err){
	unsigned int key;
	char buf[20];

	SaveDisp(SAVEDISP_PAGE1);
	PopUpWin(5);
	sprintf(buf,"code:%d",err);
	locate(3,2); Print((unsigned char *)buffer);
	locate(3,4); Print((unsigned char *)buf);
	locate(3,6); Print((unsigned char *) "   Press:[EXIT]");

	ExitKey();
	RestoreDisp(SAVEDISP_PAGE1);
}

void ErrorADRS(char*buffer,int err){
	unsigned int key;
	char buf[20];

	SaveDisp(SAVEDISP_PAGE1);
	PopUpWin(5);
	locate(3,2); Print((unsigned char *)buffer);
	Hex8PrintXY(3,4,"addres:",err);
	locate(3,6); Print((unsigned char *) "   Press:[EXIT]");

	ExitKey();
	RestoreDisp(SAVEDISP_PAGE1);
}

void MSG1(char*buffer1){
	SaveDisp(SAVEDISP_PAGE1);
	PopUpWin(1);
	locate(3,4); Print((unsigned char *)buffer1);
	Bdisp_PutDisp_DD_DrawBusy();
}
void MSG2(char*buffer1,char*buffer2){
	SaveDisp(SAVEDISP_PAGE1);
	PopUpWin(2);
	locate(3,3); Print((unsigned char *)buffer1);
	locate(3,4); Print((unsigned char *)buffer2);
	Bdisp_PutDisp_DD_DrawBusy();
}
void MSGpop(void){
	RestoreDisp(SAVEDISP_PAGE1);
	Bdisp_PutDisp_DD();
}

//---------------------------------------------------------------------------------------------

void ProgressBarPopUp( char *buffer, char *buffer2 ) {
		SaveDisp(SAVEDISP_PAGE1);
		PopUpWin(5);
		locate(3,2); Print((unsigned char *)buffer);
		locate(3,3); Print((unsigned char *)buffer2);
		ML_rectangle( 17, 32, 106, 40, 1, 1, 0);
}

void ProgressBar(int current, int max) {
	int i;
	unsigned int t=RTC_GetTicks();
	if ( abs(t-skip_count)>4 ) { skip_count=t;		// default 128/4=32  1/32s
		GUI_ProgressBar( current, max);
		Bdisp_PutDisp_DD();
	}
}

//----------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------
//int IObjectAlign4a( unsigned int n ){ return n; }	// align +4byte
//int IObjectAlign4b( unsigned int n ){ return n; }	// align +4byte
//int IObjectAlign4c( unsigned int n ){ return n; }	// align +4byte
//int IObjectAlign4d( unsigned int n ){ return n; }	// align +4byte
//int IObjectAlign4e( unsigned int n ){ return n; }	// align +4byte
//int IObjectAlign4f( unsigned int n ){ return n; }	// align +4byte
//int IObjectAlign4g( unsigned int n ){ return n; }	// align +4byte
//int IObjectAlign4h( unsigned int n ){ return n; }	// align +4byte
//int IObjectAlign4i( unsigned int n ){ return n; }	// align +4byte
//----------------------------------------------------------------------------------------------

