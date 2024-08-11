#include "dispbios.h"
#include "filebios.h"

#define VRAMSIZE 0x28800
#define VRAMSIZE_FX 1024

int OS_Version();	// 03.10.0010 -> 0310

//----------------------------------------------------------------------------- std
#define RAND_MAX 65535
#define KEY_CTRL_FORMAT    30101
unsigned char ColorIndexDialog0( unsigned char initial_index );

//extern char *strcpy(char *, const char *);
//extern char *strncpy(char *, const char *, size_t);
extern char *strcat(char *, const char *);
extern char *strncat(char *, const char *,size_t);
//extern int  memcmp(const void *, const void *,size_t);
extern int  strcmp(const char *, const char *);
extern int  strncmp(const char *, const char *, size_t);
extern void *memchr(const void *, int, size_t);
//extern char *strchr(const char *, int);
extern size_t strcspn(const char *, const char *);
extern char *strpbrk(const char *, const char *);
extern char *strrchr(const char *, int);
extern size_t strspn(const char *, const char *);
extern char *strstr2(const char *, const char *);
extern char *strtok(char *, const char *);
//extern size_t strlen(const char *);

int strlenOp( char *buffer ) ;
//extern void *memset(void *, int, size_t);
int sprintf(char *str,const char *fmt,...);
int sprintf3(char *str,const char *fmt,...);
void qsort(void *aa, size_t n, size_t es, int (*cmp)(const void *, const void *));
void *memcpy2(void *buf1, const void *buf2, size_t n);

int OpcodeToStr2( int opcode, unsigned char *string ) ; //
int rand() ;
int srand( int seed ) ;
//----------------------------------------------------------------------------- math
double atof( char* buf ) ;
/*

double fmod(double x,double y);
double fabs(double x);
double floor( double x ) ;
double sinh( double x );
double cosh( double x );
double tanh( double x );
double acos(double);
double asin(double);
double atan(double);
double atan2(double,double);
double cos(double);
double sin(double);
double tan(double);
double cosh(double);
double tanh(double);
double sinh(double);
double exp(double);
double log(double);
double log10(double);
double pow(double,double);
double sqrt(double);
*/
//----------------------------------------------------------------------------- Bdisp
/*
void Bdisp_AllClr_DD(void);
void Bdisp_AllClr_VRAM(void);
void Bdisp_AllClr_DDVRAM(void);
void Bdisp_AreaClr_DD(const DISPBOX *pArea);
void Bdisp_AreaClr_VRAM(const DISPBOX *pArea);
void Bdisp_AreaClr_DDVRAM(const DISPBOX *pArea);
void Bdisp_AreaReverseVRAM(int x1, int y1, int x2, int y2);
void Bdisp_GetDisp_DD(unsigned char *pData);
void Bdisp_GetDisp_VRAM(unsigned char *pData);
void Bdisp_PutDisp_DD(void);
void Bdisp_PutDispArea_DD(const DISPBOX *PutDispArea);
void Bdisp_SetPoint_DD(int x, int y, unsigned char point);
void Bdisp_SetPoint_VRAM(int x, int y, unsigned char point);
void Bdisp_SetPoint_DDVRAM(int x, int y, unsigned char point);
int  Bdisp_GetPoint_VRAM(int x, int y);
void Bdisp_WriteGraph_DD(const DISPGRAPH *WriteGraph);
void Bdisp_WriteGraph_VRAM(const DISPGRAPH *WriteGraph);
void Bdisp_WriteGraph_DDVRAM(const DISPGRAPH *WriteGraph);
void Bdisp_ReadArea_DD(const DISPBOX *ReadArea, unsigned char *ReadData);
void Bdisp_ReadArea_VRAM(const DISPBOX *ReadArea, unsigned char *ReadData);
void Bdisp_DrawLineVRAM(int x1, int y1, int x2, int y2);
void Bdisp_ClearLineVRAM(int x1, int y1, int x2, int y2);

void PrintLine(const unsigned char *str, int max);
void PrintRLine(const unsigned char *str, int max);
void PrintXY(int x, int y, const unsigned char *str, int type);
int PrintMini(int x, int y, const unsigned char *str, int type);
void SaveDisp(unsigned char num);
void RestoreDisp(unsigned char num);
void PopUpWin(int n);

int Bfile_OpenFile(const FONTCHARACTER *filename, int mode);
int Bfile_OpenMainMemory(const unsigned char *name);
int Bfile_ReadFile(int HANDLE, void *buf, int size, int readpos);
int Bfile_WriteFile(int HANDLE, const void *buf, int size);
int Bfile_SeekFile(int HANDLE, int pos);
int Bfile_CloseFile(int HANDLE);
int Bfile_GetMediaFree(enum DEVICE_TYPE devicetype, int *freebytes);
int Bfile_GetFileSize(int HANDLE);
int Bfile_CreateFile(const FONTCHARACTER *filename, int size);
int Bfile_CreateDirectory(const FONTCHARACTER *pathname);
int Bfile_CreateMainMemory(const unsigned char *name);
int Bfile_RenameMainMemory(const unsigned char *oldname, const unsigned char *newname);
int Bfile_DeleteFile(const FONTCHARACTER *filename);
int Bfile_DeleteDirectory(const FONTCHARACTER *pathname);
int Bfile_DeleteMainMemory(const unsigned char *name);
int Bfile_FindFirst(const FONTCHARACTER *pathname, int *FindHandle, FONTCHARACTER *foundfile, FILE_INFO *fileinfo);
int Bfile_FindNext(int FindHandle, FONTCHARACTER *foundfile, FILE_INFO *fileinfo);
int Bfile_FindClose(int FindHandle);

void Bkey_Set_RepeatTime_Default(void);
*/
void Bkey_Set_RepeatTime(long FirstCount, long NextCount);
void Bkey_Get_RepeatTime(long *FirstCount, long *NextCount);

#define SAVEDISP_PAGE1              1
void SaveDisp( int n );
void RestoreDisp(int n);
void Bdisp_PutDisp_DD_stripe_BG( int y1, int y2 ) ;

void *GetVRAMAddress(void); // Return a pointer to the system's video memory.
void *GetVRAMWorkBuffer(void);
void DD_SetContrast( int n ) ;
void Cursor_SetPosition( int x, int y );
void PutKey( unsigned int key, int mode );
void Keyboard_ClrBuffer();
int OpCodeToStr( const short opcode, const char*result );
int Calljmp( int r4, int r5, int r6, int r7, int adrs );
int SysCalljmp( int r4, int r5, int r6, int r7, int No);
int SysCalljmp12( int r1, int r2, int r3, int r4, int r5, int r6, int r7, int r8, int r9, int r10, int r11, int r12, int No);
void locate(int x, int y);
void Prints(const unsigned char *str);
void PrintRevs(const unsigned char *str);
void PrintC(const unsigned char *c);
void PrintRevC(const unsigned char *str);
void PrintCXY1(int x, int y, unsigned char*cptr, int mode_flags, int P5, int color, int back_color, int P8, int P9 );
int PrintMinix1( int X, int Y, unsigned char*buf, int _flags, unsigned short  color, unsigned short  backcolor ) ;
int PrintMinix3( int x, int y, unsigned char*buf, int mode ) ;
int PrintMinix1orWB( int X, int Y, unsigned char*buf ) ;
int PrintMinix3orWB( int X, int Y, unsigned char*buf ) ;
void PrintMiniXY(int X, int Y, char*buf, int COLOR,int BackCOLOR, int extflag, int xlength ) ;
void Bdisp_AllClr_DDVRAM();
void Bdisp_AllClr_VRAM1();	// 0~192 0xFFFF
void Bdisp_AllClr_VRAM2();	// -24~192
void Bdisp_AllClr_VRAM3(int y0, int y1 );	// y0~y1
void Bdisp_Fill_DD( int color, int mode );
void Bdisp_AllClr_VRAM_mesh( int y0, int y1 , unsigned short color );

void SaveDisp( int n );
void RestoreDisp(int n);
void Bdisp_AllClr_DDVRAM();
void Bdisp_AreaReverseVRAM( int x1, int y1, int x2, int y2 ) ;
void Bdisp_AreaReverseVRAMx3( int x1, int y1, int x2, int y2 ) ;
void Bdisp_AreaClr_VRAM( int x1, int y1, int x2, int y2 ) ;
void Bdisp_SetPoint_DDVRAM(int x, int y, unsigned char point);
void Bdisp_GetDisp_VRAM(unsigned char *pData);

void Bdisp_WriteGraph_VRAM_CG(const DISPGRAPH *WriteGraph, int bitmapWidth);	//	 16bit{384,192} -> CG {384,192} 16bit
void Bdisp_WriteGraph_RAM_CG_1bit(const DISPGRAPH *WriteGraph, unsigned char* ram );	// 1bit {384,192} -> RAM  1*1 dot {384,192}
void Bdisp_WriteGraph_VRAM_CG_1bit(const DISPGRAPH *WriteGraph);	// 1bit {384,192} -> VRAM  1*1 dot {384,192}
void Bdisp_WriteGraph_VRAM_CG_1bit_75(const DISPGRAPH *WriteGraph);	// 1bit {384,192} -> VRAM  1*1 dot {384,192} 75% reduce
void Bdisp_WriteGraph_RAM_fx_xy1(const DISPGRAPH *WriteGraph, unsigned char* ram, int factor );	// 1bit {128,64}or{384,192} ->  RAM 3*3 dot {384,192}
void Bdisp_WriteGraph_RAM_fx( const DISPGRAPH *WriteGraph, unsigned char* ram  );	// 1bit {128,64} ->  RAM 3*3 dot {384,192}
void Bdisp_WriteGraph_VRAM_fx( const DISPGRAPH *WriteGraph);	// 1bit {128,64} ->  VRAM 3*3 dot {384,192}
void Bdisp_WriteGraph_VRAM_fx_xy1(const DISPGRAPH *WriteGraph);	// 1bit {384,192} -> VRAM  3*3 dot {384,192}
void Bdisp_WriteGraph_VRAM(const DISPGRAPH *WriteGraph);
void Bdisp_WriteGraph_DD(const DISPGRAPH *WriteGraph);

void Bdisp_ReadArea_VRAM_CG(const DISPBOX *ReadArea, unsigned char *ReadData);	// CG (384,192) 16bit -> 16bit data
void Bdisp_ReadArea_RAM_CG_xy1(const DISPBOX *ReadArea, unsigned char *ReadData, unsigned char* ram );	// CG {384,192} mem -> 1bit data {384,192}
void Bdisp_ReadArea_VRAM_CG_1bit(const DISPBOX *ReadArea, unsigned char *ReadData);	// CG 1*1 fot {384,192} VRAM -> 1bit data{384,192}
void Bdisp_ReadArea_RAM_fx(const DISPBOX *ReadArea, unsigned char *ReadData, unsigned char* ram );	// fx 3*3 dot {384,192} mem -> 1bit data {128,64}
void Bdisp_ReadArea_VRAM_fx(const DISPBOX *ReadArea, unsigned char *ReadData);	// fx 3*3 dot {384,192} mem -> 1bit data {128,64}
void Bdisp_ReadArea_VRAM(const DISPBOX *ReadArea, unsigned char *ReadData);	// CG or fx

void Bdisp_DrawLineVRAM(int x1, int y1, int x2, int y2);
void Bdisp_ClearLineVRAM(int x1, int y1, int x2, int y2);

void PopUpWin( int n );

#define locate locate_OS

//int Cursor_SetFlashOn( unsigned char cursor_type );
void Cursor_SetFlashMode( int mode ); 			// cursor flashing
unsigned int Cursor_GetSettings(unsigned int *settingsarray);
int Cursor_GetFlashStyle();

int Bdisp_SetDDRegisterB( int mode );
int Bdisp_IsZeroDDRegisterB( void );

void EnableDisplayStatusArea();
void EnableDisplayStatusArea2();
void SetStatusMessage_( char*msg, char short_color );
void StatusArea_( char*msg, char short_color );
void StatusArea_sub();
void StatusArea_Edit( char *name, int num);
void StatusArea_Run_sub( char *str, int intmode, int g1morg3m );
void StatusArea_Run();
void StatusArea_Time();

int GetColorIndex( int color ) ;
int GetColor2Index( int colorRGB ) ;

//#define	SaveDisp(SAVEDISP_PAGE1)	memcpy(vbuf,(char*)PictAry[0],VRAMSIZE)
//#define	RestoreDisp(SAVEDISP_PAGE1)	memcpy((char*)PictAry[0],vbuf,VRAMSIZE)

//	memcpy(vbuf,(char*)PictAry[0],VRAMSIZE);	//	SaveDisp(SAVEDISP_PAGE1);
//	memcpy((char*)PictAry[0],vbuf,VRAMSIZE);	//	RestoreDisp(SAVEDISP_PAGE1);

void ProgressBar2( unsigned char*heading, int current, int max );
void ProgressBar0( int P1, int P2, int P3, int current, int max );
void ProgressBar1( int current, int max );

void CB_ScrollBar(int max, int pos, int height, int x1, int y1, int dx, int dy  ) ;
void CB_StandardScrollBar(int max, int pos ) ;

void*GetMiniGlyphPtr_MB( unsigned short mb_glyph_no, unsigned short*glyph_info );
void*GetMiniMiniGlyphPtr_MB( unsigned short mb_glyph_no, unsigned short*glyph_info );
void*GetBoldMiniMiniGlyphPtr_MB( unsigned short mb_glyph_no, unsigned short*glyph_info );
void*GetGBGlyphPtr( unsigned short mb_glyph_no, unsigned short*glyph_info );
void*GetGBMiniGlyphPtr( unsigned short mb_glyph_no, unsigned short*glyph_info );
void*GetGlyphPtr( unsigned char glyph_no );
void*GetGlyphPtr_F9( unsigned char glyph_no );
void*GetGlyphPtr_E7( unsigned char glyph_no );
void*GetGlyphPtr_E6( unsigned char glyph_no );
void*GetGlyphPtr_E5( unsigned char glyph_no );
void*GetGlyphPtr_7F( unsigned char glyph_no );
void*GetGlyphPtr_F7( unsigned char glyph_no );
void*GetMiniGlyphPtr_E7( unsigned char glyph_no , unsigned short*glyph_info );

void StatusArea_SetGlyph( int mode ) ;	// mode:0 file   1:edit   2:run

//----------------------------------------------------------------------------- Bfile

int Bfile_OpenFile(const FONTCHARACTER *filename, int mode);
int Bfile_WriteFile(int HANDLE, const void *buf, int size);
int Bfile_GetMediaFree(enum DEVICE_TYPE devicetype, int *freebytes);
int Bfile_GetFileSize(int HANDLE);
int Bfile_CreateFile(const FONTCHARACTER *filename, int size);
int Bfile_CreateDirectory(const FONTCHARACTER *pathname);
int Bfile_OpenMainMemory(const unsigned char *name);
int Bfile_CreateMainMemory(const unsigned char *name);
int Bfile_DeleteFile(const FONTCHARACTER *filename);
int Bfile_DeleteDirectory(const FONTCHARACTER *pathname);
int Bfile_DeleteMainMemory(const unsigned char *name);
int Bfile_GetMediaFree_OS(unsigned short* media_id, int* freespace) ;

//int Bfile_RenameEntry( const unsigned short *oldname , const unsigned short *newname );
int Bfile_GetFileInfo( const unsigned short *filename, FILE_INFO *fileinfo );

#define Bfile_CloseFile Bfile_CloseFile_OS
#define Bfile_SeekFile Bfile_SeekFile_OS
#define Bfile_ReadFile Bfile_ReadFile_OS
#define Bfile_WriteFile Bfile_WriteFile_OS
//-----------------------------------------------------------------------------
int OpenFileDialog(unsigned short keycode, unsigned short* filenamebuffer, int filenamebufferlength) ;

//----------------------------------------------------------------------------- Serial
int Serial_Open(unsigned char *mode);
int Serial_Close(int mode);

int Serial_IsOpen( void );
int Serial_SpyNthByte( int byteno_to_spy, unsigned char*result );
int Serial_ReadOneByte( unsigned char*result );
int Serial_ReadNBytes( unsigned char*result, int max_size, short*actually_transferred );
int Serial_BufferedTransmitOneByte( unsigned char byte_to_transmit );
int Serial_GetReceivedBytesAvailable( void );
int Serial_GetFreeTransmitSpace( void );
int Serial_ClearReceiveBuffer( void );
int Serial_ClearTransmitBuffer( void );

//----------------------------------------------------------------------------- Timer
#define ID_USER_TIMER1          1
#define ID_USER_TIMER2          2
#define ID_USER_TIMER3          3
#define ID_USER_TIMER4          4
#define ID_USER_TIMER5          5

extern int timer_id0;	// Time id default
extern int timer_id ;	// Time id

int SetTimer(int ID, int elapse, void (*hander)(void));
int KillTimer(int ID);
void Set_Timer_id();

void RTC_GetDateTime( unsigned char timestr[ 8 ] );
unsigned int SetGetkeyToMainFunctionReturnFlag( unsigned int enabled );


//-----------------------------------------------------------------------------
extern int Ticks32768;
unsigned int GetTicks32768();
void WaitTimer32768() ;

//-----------------------------------------------------------------------------
// refer to https://www.cemetech.net/forum/viewtopic.php?t=11908
char GetLightLevel(void);
void DecLightLevel(void);
void IncLightLevel(void);
void ClrLightLevel(void);
void Bdisp_SetBacklightLevel(char level);
void Bdisp_WriteDDRegister5A1( unsigned char finelevel );
void Bdisp_DDRegisterSelect( unsigned short reg );
void SetRawBacklightSubLevel(int level);

//-----------------------------------------------------------------------------
void Bkey_SetFlag( short flagpattern );
void Bkey_ClrFlag( short flagpattern );
//-----------------------------------------------------------------------------
char Setup_GetEntry(unsigned int index);
char *Setup_SetEntry(unsigned int index, char setting);
int IsCG10( void  );

//-----------------------------------------------------------------------------
//------------------------------------------------------------------------- MCS
int MCS_GetState( int*maxspace, int*currentload, int*remainingspace );

//-----------------------------------------------------------------------------
//-------------------------------------------------------------------------Fkey
//	https://egadget.blog.fc2.com/blog-entry-685.html#comment5017
//	discoveried by Colon
// p1:
//
//
int Bdisp_FkeyColor( int p1, short p2);

