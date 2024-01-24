#include "CB.h"

struct st_round CB_Round = { Norm , 1} ; // Round
char ENG=0;	// ENG flag  1:ENG  3:3digit separate

char UseHiddenRAM=1;	//	0x11 :HiddenRAMInit off
char IsHiddenRAM =0;

#define HIDDENRAM_TOP 0x88040000
#define HIDDENRAM_END 0x88080000
char * HiddenRAM_Top        =(char*)HIDDENRAM_TOP+16+256;		// Hidden RAM TOP
char * HiddenRAM_End        =(char*)HIDDENRAM_END;				// Hidden RAM END

char * HiddenRAM_ProgNextPtr=(char*)HIDDENRAM_TOP+16+256;		// Hidden RAM Prog next ptr
char * HiddenRAM_MatTopPtr  =(char*)HIDDENRAM_END;				// Hidden RAM Mat top ptr

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
						HiddenRAM_End(0x88080000)
----------------------------------------
*/
//---------------------------------------------------------------------------------------------
int CPU_check(void) {					// SH3:1 SH4A:0   2:Slim
	int slim=0;
	if ( *(unsigned int*)0x80000300 == 0x80005D7C ){
		if ( ( *(unsigned char*)0xA4000128 & 0x08 ) == 0 ) {
			slim = 1;;
		}
	}
	return ! ( ( *(unsigned short*)0xFFFFFF80 == 0 ) && ( *(unsigned short*)0xFFFFFF84 == 0 ) ) + slim;
}

int OS_Version(){
	int ver;
	unsigned char version[16];
	System_GetOSVersion( &version[0] );
	return (version[1]-'0')*100 + (version[3]-'0')*10 + (version[4]-'0');
}
//---------------------------------------------------------------------------------------------
void * HiddenRAM(void){	// Check HiddenRAM 
	volatile unsigned int *NorRAM=(volatile unsigned int*)0xA8000000;	// Nomarl RAM TOP (no cache area)
	volatile unsigned int *HidRAM=(volatile unsigned int*)0x88040000;	// Hidden RAM TOP (cache area)
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
	ptr = HiddenRAM_ProgNextPtr;
	HiddenRAM_ProgNextPtr += ( (size+3) & 0xFFFFFFFC );	// 4byte align
	if ( HiddenRAM_ProgNextPtr < HiddenRAM_MatTopPtr ) return ptr;
	HiddenRAM_ProgNextPtr = ptr;
	return 0;
}

void * HiddenRAM_mallocMat( size_t size ){
	char * ptr;
	ptr = HiddenRAM_MatTopPtr;
	ptr -= ( (size+7) & 0xFFFFFFF8 );	// 8byte align
	if ( ptr < HiddenRAM_ProgNextPtr ) return 0;
	HiddenRAM_MatTopPtr = ptr;
	return ptr;
}

unsigned char *  HiddenRAM_mallocPict( int pictNo ){
	int dimA,dimB;
	unsigned char *buffer;
	int pictsize=1024;
	dimA = pictNo;
	buffer = (unsigned char *)HiddenRAM_mallocProg( pictsize );
	return buffer;
}

void HiddenRAM_freeProg( void *ptr ){
	if ( ( (int)HiddenRAM_Top <= (int)ptr ) && ( (int)ptr < (int)HiddenRAM_ProgNextPtr ) ) HiddenRAM_ProgNextPtr= (char*)ptr;	// Hidden RAM Prog next ptr
}

void * memcpy2(void *dest, const void *src, size_t len){
	char *d = (char*)dest+len-1;
	const char *s = (char*)src+len-1;
	while (len--)
    	*d-- = *s--;
  	return dest;
}
//		HiddenRAM_MatTopPtr
//		...
//		ptr
//		ptr+size
//		...
//		
//
void HiddenRAM_freeMat( int reg ){
	int ptr = (int)MatAry[reg].Adrs;
	int	adrs, size = MatAry[reg].Maxbyte; 
	if ( size <= 0 ) return ;
	if ( (int)HiddenRAM_MatTopPtr == ptr ) goto plus;
	else
	if ( ( (int)HiddenRAM_MatTopPtr < ptr ) && ( ptr < (int)HiddenRAM_End ) ) {
		memcpy2( HiddenRAM_MatTopPtr+size, HiddenRAM_MatTopPtr, ptr-(int)HiddenRAM_MatTopPtr );
		for ( reg=0; reg<MatAryMax; reg++){
			adrs = (int)MatAry[reg].Adrs;
			if ( ( MatAry[reg].SizeA ) && ( adrs > (int)HiddenRAM_ProgNextPtr ) && ( adrs < ptr ) ) MatAry[reg].Adrs =(double*)(adrs+size);
		}
	  plus:
		HiddenRAM_MatTopPtr += size;
	} 
}

const char MatAryCheckStr[]="#CBasic163#";

void HiddenRAM_MatAryStore(){	// MatAry ptr -> HiddenRAM
	int *iptr1=(int*)(HIDDENRAM_TOP+12);
	int *iptr2=(int*)(HiddenRAM_End+12);
	if ( ( UseHiddenRAM ) && ( IsHiddenRAM ) ) {
		memcpy( (char *)HIDDENRAM_TOP, MatAryCheckStr, sizeof(MatAryCheckStr) );
		memcpy( HiddenRAM_End,         MatAryCheckStr, sizeof(MatAryCheckStr) );
		iptr1[0]=(int)MatAry;
		iptr2[0]=(int)HiddenRAM_MatTopPtr;
	}
}
int HiddenRAM_MatAryRestore(){	//  HiddenRAM -> MatAry ptr
	char buffer[10];
	int *iptr1=(int*)(HIDDENRAM_TOP+12);
	int *iptr2=(int*)(HiddenRAM_End+12);
	char *tmp;
	tmp=HiddenRAM_End - sizeof(matary)*MatAryMax ;
	if ( !(UseHiddenRAM&0xF0) )return 0;	// hidden init
	if ( ( UseHiddenRAM ) && ( IsHiddenRAM ) && ( (char *)iptr1[0]==tmp ) ){
		if ( ( strcmp((char *)HIDDENRAM_TOP, MatAryCheckStr) == 0 ) && ( strcmp(HiddenRAM_End, MatAryCheckStr) == 0 ) ) {
			HiddenRAM_MatTopPtr=(char*)iptr2[0];
			MatAry=(matary *)tmp;
			return 1;	// ok
		}
	}
	return 0;
}

void HiddenRAM_ExtFontAryInit() {
	if ( EnableExtFont ) {
		ExtAnkFontFX     =(unsigned char *)HiddenRAM_Top ;				// Ext Ascii font
		ExtAnkFontFXmini =(unsigned char *)(ExtAnkFontFX     + 96*8) ;	// Ext Ascii font
		ExtKanaFontFX    =(unsigned char *)(ExtAnkFontFXmini + 96*8) ;	// Ext Kana & Gaiji font
		ExtKanaFontFXmini=(unsigned char *)(ExtKanaFontFX    + 112*8) ;	// Ext Kana & Gaiji font
		HiddenRAM_Top    =((char *)ExtKanaFontFX + 112*8) ;			// Heap RAM top ptr
	} else {
		ExtAnkFontFX     =(unsigned char *)Font00   +32*8;	//  Ascii font
		ExtAnkFontFXmini =(unsigned char *)Fontmini +32*8;	//  Ascii font
		ExtKanaFontFX    =(unsigned char *)KanaFont ;		//  Kana & Gaiji font
		ExtKanaFontFXmini=(unsigned char *)KanaFontmini ;	// Ext Kana & Gaiji font
	}
}
void HiddenRAM_MatAryInit(){	// HiddenRAM Initialize
	char buffer[10];
	int *iptr1=(int*)(HIDDENRAM_TOP+12);
	int *iptr2=(int*)(HiddenRAM_End+12);
	MatAryMax=MATARY_MAX +ExtendList*52;
	Mattmpreg=MatAryMax-1;
	ExtListMax=MatAryMax-33-26-3;
	if ( ( UseHiddenRAM ) && ( IsHiddenRAM ) ) {		// hidden RAM init
		EditMaxfree = EDITMAXFREE2;
		EditMaxProg = EDITMAXPROG2;
		NewMax      = NEWMAX2;
		ClipMax     = CLIPMAX2;
		HiddenRAM_Top         = (char*)HIDDENRAM_TOP+16+256;		// Hidden RAM TOP
		HiddenRAM_End         = (char*)HIDDENRAM_END;				// Hidden RAM END
		HiddenRAM_ExtFontAryInit();
		HiddenRAM_ProgNextPtr = HiddenRAM_Top;						// Hidden RAM Prog next ptr
		OplistRecentFreq=(toplistrecentfreq *)(HIDDENRAM_TOP+16);
		OplistRecent    =(short *)(HIDDENRAM_TOP+16+128);
		if ( HiddenRAM_MatAryRestore() ) return ;			// hidden RAM ready
		HiddenRAM_MatTopPtr   = HiddenRAM_End - sizeof(matary)*MatAryMax ;
		MatAry = (matary *)HiddenRAM_MatTopPtr;
		HiddenRAM_MatAryStore();
		InitOpcodeRecent();
	} else {		// use heap RAM
		HiddenRAM_Top    = (char *)((char*)AliasVarCodeLbl+ sizeof(ALIAS_VAR)*ALIASVARMAXLBL );
		EditMaxfree = EDITMAXFREE;
		EditMaxProg = EDITMAXPROG;
		NewMax      = NEWMAX;
		ClipMax     = CLIPMAX;
		HiddenRAM_ExtFontAryInit();
		HiddenRAM_End    = HeapRAM+MAXHEAP;			// Heap RAM END
		HiddenRAM_ProgNextPtr = HiddenRAM_Top;					// Heap RAM Prog next ptr
		HiddenRAM_MatTopPtr   = HiddenRAM_End - sizeof(matary)*MatAryMax;
		MatAry = (matary *)HiddenRAM_MatTopPtr;
		OplistRecentFreq=(toplistrecentfreq *)OplistRecentFreqMem;
		OplistRecent    =(short *)OplistRecentMem;
		InitOpcodeRecent();
	}
	memset( MatAry, 0, sizeof(matary)*MatAryMax );
	DeleteStrBuffer();
	DeletePictPtr();
	FileListUpdate=1;
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
	int i,y;
	char buf[1024];
	SaveVRAM( buf );
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
	RestoreVRAM( buf );
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

