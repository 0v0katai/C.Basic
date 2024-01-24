#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <fxlib.h>
#include "fx_syscall.h"

#include "CB_interpreter.h"
#include "CB_io.h"
#include "CB_error.h"
#include "CB_Kana.h"
#include "CB_Matrix.h"
#include "CB_glib.h"
#include "KeyScan.h"
#include "MonochromeLib.h"

struct st_round CB_Round = { Norm , 1} ; // Round
char ENG=0;	// ENG flag  1:ENG  3:3digit separate

char UseHiddenRAM=0;	//	0x11 :HiddenRAMInit off
char IsHiddenRAM=0;

#define MATBACKSPACE 1360	//	(85*16=1360)
char * HiddenRAM_Top =(char*)0x88040000+16+20480;	// Hidden RAM TOP
char * HiddenRAM_End =(char*)0x88080000-MATBACKSPACE-128;	// Hidden RAM END

char * HiddenRAM_ProgNextPtr=(char*)0x88040000+16+20480;	// Hidden RAM Prog next ptr
char * HiddenRAM_MatTopPtr =(char*)0x88080000-MATBACKSPACE-128;	// Hidden RAM Mat top ptr

char IsSH3;	//	3:SH3   4:SH4

/*
----------------------------------------
HiddenRAM_Top(0x88040000)
		>>>
		(prog area) HiddenRAM_ProgNextPtr
			... 
			(free area)
				...
				(Mat area)	HiddenRAM_MatTopPtr
					<<<
						HiddenRAM_End(0x88080000-1920-128)
						MatAry ptr backup (1920bytes)
----------------------------------------
*/

//---------------------------------------------------------------------------------------------
void CB_PrintC( int x, int y,const unsigned char *c ){
	if ( *c == 0xFF ) {
		*c++;
		if ( ( (*c)>=0xA0 ) && ( (*c)<=0xDF ) ) KPrintChar( (--x)*6, (--y)*8, *c );
		else {
			locate (x,y);
			Print( (unsigned char*)"@" );
		}
	} else {
		locate (x,y);
		PrintC( c );
	}
}
void CB_PrintRevC( int x, int y,const unsigned char *c ){
	if ( *c == 0xFF ) {
		*c++;
		if ( ( (*c)>=0xA0 ) && ( (*c)<=0xDF ) ) KPrintRevChar( (--x)*6, (--y)*8, *c );
		else	{
			locate (x,y);
			PrintRev( (unsigned char*)"@" );
		}
	} else {
		jmp:
		locate (x,y);
		PrintRevC( c );
	}
}
void CB_Print( int x, int y, const unsigned char *str){
	int c=(char)*str;
	while ( c ) {
		CB_PrintC( x, y, str++ );
		if ( (c==0x7F)||(c==0xFFFFFFF9)||(c==0xFFFFFFE5)||(c==0xFFFFFFE6)||(c==0xFFFFFFE7)||(c==0xFFFFFFFF) )  str++;
		x++;
		if ( x>21 ) break;
		c=(char)*str;
	}
}
void CB_PrintRev( int x, int y, const unsigned char *str){
	unsigned int c=(char)*str;
	while ( c ) {
		CB_PrintRevC( x, y, str++ );
		if ( (c==0x7F)||(c==0xFFFFFFF9)||(c==0xFFFFFFE5)||(c==0xFFFFFFE6)||(c==0xFFFFFFE7)||(c==0xFFFFFFFF) )  str++;
		x++;
		if ( x>21 ) break;
		c=(char)*str;
	}
}

void CB_PrintXYC( int px, int py,const unsigned char *c , int mode ){
	if ( *c == 0xFF ) {
		*c++;
		if ( mode ) KPrintRevChar( px, py, *c );
		else		KPrintChar( px, py, *c );
	} else {
		PrintXY( px, py, c ,mode);
	}
}
void CB_PrintXY( int px, int py, const unsigned char *str, int mode){
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

int YesNo( char*buffer){
	unsigned int key;

	SaveDisp(SAVEDISP_PAGE1);
	PopUpWin(4);
	locate(3,2); Print((unsigned char *)buffer);
	locate(3,4); Print((unsigned char *) "   Yes:[F1]");
	locate(3,5); Print((unsigned char *) "   No :[F6]");
//	Bdisp_PutDisp_DD();

	KeyRecover(); 
	while ( 1 ) {
		GetKey(&key);	
		if ( key == KEY_CTRL_F1  ) break ;
		if ( key == KEY_CTRL_F6  ) break ;
		if ( key == KEY_CTRL_EXIT) break ;
		if ( key == KEY_CTRL_AC  ) break ;
		if ( key == KEY_CTRL_EXE ) break ;
	}
	RestoreDisp(SAVEDISP_PAGE1);
	if ( key == KEY_CTRL_F1  ) return 1 ; // Yes
	if ( key == KEY_CTRL_EXE ) return 1 ; // Yes
//	Bdisp_PutDisp_DD();
	return 0 ; // No
}

int YesNo2( char*buffer, char*buffer2){
	unsigned int key;

	SaveDisp(SAVEDISP_PAGE1);
	PopUpWin(5);
	locate(3,2); Print((unsigned char *)buffer);
	locate(3,3); Print((unsigned char *)buffer2);
	locate(3,5); Print((unsigned char *) "   Yes:[F1]");
	locate(3,6); Print((unsigned char *) "   No :[F6]");
//	Bdisp_PutDisp_DD();

	KeyRecover(); 
	while ( 1 ) {
		GetKey(&key);	
		if ( key == KEY_CTRL_F1  ) break ;
		if ( key == KEY_CTRL_F6  ) break ;
		if ( key == KEY_CTRL_EXIT) break ;
		if ( key == KEY_CTRL_AC  ) break ;
		if ( key == KEY_CTRL_EXE ) break ;
	}
	RestoreDisp(SAVEDISP_PAGE1);
	if ( key == KEY_CTRL_F1  ) return 1 ; // Yes
	if ( key == KEY_CTRL_EXE ) return 1 ; // Yes
//	Bdisp_PutDisp_DD();
	return 0 ; // No
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

	GetKey(&key);	
	RestoreDisp(SAVEDISP_PAGE1);
}
void ErrorMSGstr(char*buffer,char*buffer2){
	unsigned int key;
	char buf[20];

	SaveDisp(SAVEDISP_PAGE1);
	PopUpWin(5);
	locate(3,2); Print((unsigned char *)buffer);
	locate(3,4); Print((unsigned char *)buffer2);
	locate(3,6); Print((unsigned char *) "   Press:[EXIT]");

	GetKey(&key);	
	RestoreDisp(SAVEDISP_PAGE1);
}
void ErrorMSGstr1(char*buffer){
	unsigned int key;

	SaveDisp(SAVEDISP_PAGE1);
	PopUpWin(4);
	locate(3,3); Print((unsigned char *)buffer);
	locate(3,5); Print((unsigned char *) "   Press:[EXIT]");

	GetKey(&key);	
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

	GetKey(&key);	
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

//---------------------------------------------------------------------------------------------
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
		ptr = HiddenRAM_Top-20480+1024*(pictNo-1);
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

const char MatAryCheckStr[]="##CBasic17#";

void HiddenRAM_MatAryStore(){	// MatAry ptr -> HiddenRAM
	int *iptr1=(int*)(HiddenRAM_Top-4);
	int *iptr2=(int*)(HiddenRAM_End+12);
	if ( ( UseHiddenRAM ) && ( IsHiddenRAM ) ) {
		memcpy( HiddenRAM_Top-16, MatAryCheckStr, sizeof(MatAryCheckStr) );
		memcpy( HiddenRAM_End,    MatAryCheckStr, sizeof(MatAryCheckStr) );
		memcpy( HiddenRAM_End+16, MatAry, sizeof(MatAry) );
		memcpy( HiddenRAM_End+16+MATBACKSPACE, PictAry, sizeof(PictAry) );
		iptr1[0]=(int)HiddenRAM_MatTopPtr;
		iptr2[0]=(int)HiddenRAM_MatTopPtr;
	}
}
void HiddenRAM_MatAryRestore(){	//  HiddenRAM -> MatAry ptr
	char buffer[10];
	int *iptr1=(int*)(HiddenRAM_Top-4);
	int *iptr2=(int*)(HiddenRAM_End+12);
	if ( !(UseHiddenRAM&0xF0) ) return;
	if ( ( UseHiddenRAM ) && ( IsHiddenRAM ) && ( iptr1[0]==iptr2[0] ) ){
		if ( ( strcmp(HiddenRAM_Top-16, MatAryCheckStr) == 0 ) && ( strcmp(HiddenRAM_End, MatAryCheckStr) == 0 ) ) {
			memcpy( MatAry, HiddenRAM_End+16, sizeof(MatAry) );
			memcpy( PictAry, HiddenRAM_End+16+MATBACKSPACE, sizeof(PictAry) );
			HiddenRAM_MatTopPtr=(char*)iptr1[0];
		}
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

