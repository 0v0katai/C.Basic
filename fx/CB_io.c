#include "fxlib.h"
#include "stdio.h"
#include "string.h"

#include "CB_io.h"

struct st_round CB_Round = { Norm , 1} ; // Round
int ENG=0;	// ENG flag

//---------------------------------------------------------------------------------------------
int CPU_check(void) {					// SH3:3 SH4A:4
	if ( ( *(volatile unsigned short*)0xFFFFFF80 == 0 ) &&
	     ( *(volatile unsigned short*)0xFFFFFF84 == 0 ) ) return 4;
	return 3 ;
}

//---------------------------------------------------------------------------------------------
void PrintXYR(int x,int y,char *buffer,int rev){
 	if ( rev ) PrintXY(x,y,(unsigned char *)buffer,1);
		else   PrintXY(x,y,(unsigned char *)buffer,0);
}

void Fkey_Clear(int n) {
	int y;
	for (y=0; y<8; y++){
		Bdisp_ClearLineVRAM(n*21+2,7*8+y,n*21+20,7*8+y);
	}
}
void Fkey_dispN(int n,char *buffer) {
	Fkey_Clear(n);
	PrintMini(n*21+4,7*8+2,(unsigned char *)buffer, MINI_OVER);
	Bdisp_DrawLineVRAM(n*21+2,7*8+0,n*21+20,7*8+0);
	Bdisp_DrawLineVRAM(n*21+2,7*8+0,n*21+2,7*8+7);
	Bdisp_ClearLineVRAM(n*21+3,7*8+1,n*21+3,7*8+7);
	Bdisp_ClearLineVRAM(n*21+20,7*8+1,n*21+20,7*8+7);
}
void Fkey_dispR(int n,char *buffer) {
	Fkey_Clear(n);
	Bdisp_DrawLineVRAM(n*21+2,7*8+0,n*21+20,7*8+0);
	Bdisp_DrawLineVRAM(n*21+2,7*8+1,n*21+20,7*8+1);
	Bdisp_DrawLineVRAM(n*21+2,7*8+2,n*21+20,7*8+2);
	Bdisp_DrawLineVRAM(n*21+2,7*8+3,n*21+20,7*8+3);
	Bdisp_DrawLineVRAM(n*21+2,7*8+4,n*21+20,7*8+4);
	Bdisp_DrawLineVRAM(n*21+2,7*8+5,n*21+19,7*8+5);
	Bdisp_DrawLineVRAM(n*21+2,7*8+6,n*21+18,7*8+6);
	Bdisp_DrawLineVRAM(n*21+2,7*8+7,n*21+17,7*8+7);
	Bdisp_DrawLineVRAM(n*21+2,7*8+0,n*21+ 2,7*8+7);
	PrintMini(n*21+3,7*8+1     ,(unsigned char *)buffer, MINI_REV);
	Bdisp_ClearLineVRAM(n*21+21,7*8+0,n*21+21,7*8+7);
	Bdisp_ClearLineVRAM(n*21+22,7*8+0,n*21+22,7*8+7);
	Bdisp_ClearLineVRAM(n*21+20,7*8+5,n*21+20,7*8+5);
	Bdisp_ClearLineVRAM(n*21+19,7*8+6,n*21+20,7*8+6);
	Bdisp_ClearLineVRAM(n*21+18,7*8+7,n*21+20,7*8+7);
}
void Fkey_DISPN(int n,char *buffer) {
	Fkey_Clear(n);
	PrintXY(n*21+2,7*8+1,(unsigned char *)buffer,0);
	Bdisp_DrawLineVRAM(n*21+2,7*8+0,n*21+20,7*8+0);
	Bdisp_DrawLineVRAM(n*21+2,7*8+0,n*21+2,7*8+7);
	Bdisp_ClearLineVRAM(n*21+3,7*8+1,n*21+3,7*8+7);
	Bdisp_ClearLineVRAM(n*21+20,7*8+1,n*21+20,7*8+7);
}
void Fkey_DISPR(int n,char *buffer) {
	Fkey_Clear(n);
	PrintXY(n*21+2,7*8+1,(unsigned char *)buffer,0);
	Bdisp_AreaReverseVRAM( n*21+2, 7*8+0, n*21+20, 7*8+7);	// reverse Fkey
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
	PopUpWin(5);
	locate(3,2); Print((unsigned char *)buffer);
	locate(3,4); Print((unsigned char *) "   Yes:[F1]");
	locate(3,5); Print((unsigned char *) "   No :[F6]");
//	Bdisp_PutDisp_DD();

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
//	Bdisp_PutDisp_DD();

	GetKey(&key);	
	RestoreDisp(SAVEDISP_PAGE1);
//	Bdisp_PutDisp_DD();
}

void ErrorADRS(char*buffer,int err){
	unsigned int key;
	char buf[20];

	SaveDisp(SAVEDISP_PAGE1);
	PopUpWin(5);
	locate(3,2); Print((unsigned char *)buffer);
	Hex8PrintXY(3,4,"addres:",err);
	locate(3,6); Print((unsigned char *) "   Press:[EXIT]");
//	Bdisp_PutDisp_DD();

	GetKey(&key);	
	RestoreDisp(SAVEDISP_PAGE1);
//	Bdisp_PutDisp_DD();
}

void MSG1(char*buffer1){
	SaveDisp(SAVEDISP_PAGE1);
	PopUpWin(1);
	locate(3,4); Print((unsigned char *)buffer1);
//	Bdisp_PutDisp_DD();
}
void MSG2(char*buffer1,char*buffer2){
	SaveDisp(SAVEDISP_PAGE1);
	PopUpWin(2);
	locate(3,3); Print((unsigned char *)buffer1);
	locate(3,4); Print((unsigned char *)buffer2);
//	Bdisp_PutDisp_DD();
}
void MSGpop(void){
	RestoreDisp(SAVEDISP_PAGE1);
	Bdisp_PutDisp_DD();
}
//---------------------------------------------------------------------------------------------

