//-----------------------------------------------------------------------------
struct st_round {
	char MODE;	//    mode :   0:normal  1:Fix   2: Sci
	char DIGIT;	//    digits   0~15
	char ExpTYPE; //  exponent display type
	char dummy;
};

extern struct st_round CB_Round;	// struct Round
extern char ENG;	// ENG flag

#define Norm 0
#define Fix  1
#define Sci  2

extern char UseHiddenRAM;		// 0: no use   1: use hiddenRAM
extern char IsHiddenRAM;		// 0: hiddenRAM not exist   1: exist

#define NORMALRAM_TOP    0xAC000000
#define HIDDENRAM_TOPTOP 0x8C200000
#define HIDDENRAM_TOP    0x8C300000
#define HIDDENRAM_END    0x8C800000-16
#define EmuRAMdiff	 0x4000000
extern char * HiddenRAM_Top ;			// Hidden RAM TOP
extern char * HiddenRAM_End ;			// Hidden RAM END
extern char * HiddenRAM_ProgNextPtr;	// Hidden RAM Prog next ptr
extern char * HiddenRAM_MatTopPtr;		// Hidden RAM Mat top ptr

extern char IsCG20;	//	1:CG10/20   0:CG50

#define FKeyNo1	0
#define FKeyNo2	1
#define FKeyNo3	2
#define FKeyNo4	3
#define FKeyNo5	4
#define FKeyNo6	5

//-----------------------------------------------------------------------------

//void Prints( unsigned char *buf ) ;
//void PrintRevs( unsigned char *buf ) ;

void * HiddenRAM(void);
void * HiddenRAM_mallocTemp( size_t size );
void * HiddenRAM_mallocMat( size_t size );
void * HiddenRAM_mallocProg( size_t size );
unsigned char *  HiddenRAM_mallocPict( int pictNo );
void HiddenRAM_freeTemp( void *ptr );
void HiddenRAM_freeProg( void *ptr );
void HiddenRAM_freeMat( int reg );
void HiddenRAM_MatAryInit();	// HiddenRAM Initialize
void HiddenRAM_MatAryStore();	// MatAry ptr -> HiddenRAM
void HiddenRAM_MatAryClear();	// MatAry ptr clear
int  HiddenRAM_MatAryRestore();	//  HiddenRAM -> MatAry ptr	 1:ok
void HeapRAM_malloc( int sizeK );

int CheckFontByteCount( unsigned char *str ) ;
void CB_Prints( int x, int y, unsigned char *str);
void CB_PrintRevs( int x, int y, unsigned char *str);
int CB_PrintC( int x, int y, unsigned char *str );
int CB_PrintRevC( int x, int y, unsigned char *str );
int CB_PrintC_ext( int x, int y, unsigned char *str, int extflag );
int CB_PrintRevC_ext( int x, int y, unsigned char *str, int extflag );
void CB_Prints_ext( int x, int y, unsigned char *str, int extflag );
void CB_PrintRevs_ext( int x, int y, unsigned char *str, int extflag );
void CB_PrintLocateXYC( int px, int py, unsigned char *str, int mode, int extflag);
void CB_PrintLocateXY( int px, int py, unsigned char *str, int mode, int extflag);
void CB_PrintXYC( int px, int py,unsigned char *c , int mode );
void CB_PrintXYCx3( int px, int py,unsigned char *c , int mode );
void CB_PrintXY( int px, int py, unsigned char *str, int mode );
void CB_PrintXYx3( int px, int py, unsigned char *str, int mode );
int CB_PrintLengthStr( unsigned char *str, int extflag );
int CB_PrintMinix1( int X, int Y, unsigned char*buf, int mode ) ;	// {384,192} mode str
int CB_PrintMiniC( int px, int py, unsigned char *str, int mode);	// {384,192} mode char
void CB_PrintMini( int px, int py, unsigned char *str, int mode );	// {384,192} mode string
void CB_PrintMini_wx( int px, int py, unsigned char *str, int mode, int xlim );	// {384,192} mode string
int CB_PrintMiniC_Fix( int px, int py, unsigned char *str, int mode);	// {384,192} mode char Fixed pitch 12dot
void CB_PrintMini_Fix( int px, int py, unsigned char *str, int mode );	// {384,192} mode string Fixed pitch 12dot
int CB_PrintMiniC_Fix10( int px, int py, unsigned char *str, int mode);	// {384,192} mode char Fixed pitch 10dot
void CB_PrintMini_Fix10( int px, int py, unsigned char *str, int mode );	// {384,192} mode string Fixed pitch 10dot
int CB_PrintMiniCx3( int px, int py, unsigned char *str, int mode);	// {128,64} mode fx mode str
void CB_PrintMinix3( int px, int py, unsigned char *str, int mode);	// {128,64} mode fx mode char
int CB_PrintMiniC_fx( int px, int py, unsigned char *str, int mode);	// {384,192} mode string fx font
void CB_PrintMini_fx( int px, int py, unsigned char *str, int mode );	// {384,192} mode string fx font
int CB_PrintMiniLength( unsigned char *str, int extflag  );
int CB_PrintMiniLengthStr( unsigned char *str, int extflag  );
int CB_PrintMiniLength_fxed( unsigned char *str, int extflag  );
int CB_PrintMiniLengthStr_fxed( unsigned char *str, int extflag  );
int CB_PrintMiniMiniC( int px, int py, unsigned char *str, int mode);	// {384,192} mode char   MiniMini font
void CB_PrintMiniMini( int px, int py, unsigned char *str, int mode );	// {384,192} mode string MiniMini font
void CB_PrintMiniMiniorWB( int X, int Y, unsigned char*buf ) ;
int CB_PrintMiniMiniBoldC( int px, int py, unsigned char *str, int mode);	// {384,192} mode char   MiniMini font Bold
void CB_PrintMiniMiniBold( int px, int py, unsigned char *str, int mode );	// {384,192} mode string MiniMini font Bold
int CB_PrintMiniMiniFXC( int px, int py, unsigned char *str, int mode);	// {384,192} mode char   6x8dot FX font
void CB_PrintMiniMiniFX( int px, int py, unsigned char *str, int mode );	// {384,192} mode string 6x8dot FX font
int CB_PrintMiniMiniC_Fix( int px, int py, unsigned char *str, int mode);	// {384,192} mode char Fixed pitch 8dot
void CB_PrintMiniMini_Fix( int px, int py, unsigned char *str, int mode );	// {384,192} mode string Fixed pitch 8dot
int CB_PrintMiniMiniC_Fix7( int px, int py, unsigned char *str, int mode);	// {384,192} mode char Fixed pitch 7dot
void CB_PrintMiniMini_Fix7( int px, int py, unsigned char *str, int mode );	// {384,192} mode string Fixed pitch 8dot
int CB_PrintMiniMiniBoldC_Fix( int px, int py, unsigned char *str, int mode);	// {384,192} mode char Fixed pitch 9dot
void CB_PrintMiniMiniBold_Fix( int px, int py, unsigned char *str, int mode );	// {384,192} mode string Fixed pitch 9dot
int CB_PrintMiniMiniLength( unsigned char *str );
int CB_PrintMiniMiniLengthStr( unsigned char *str );
int CB_PrintMiniMiniBoldLength( unsigned char *str );
int CB_PrintMiniMiniBoldLengthStr( unsigned char *str );

int CB_PrintMiniC_ed( int px, int py, unsigned char *str, int mode );	// {384,192} mode char
void CB_PrintMini_ed( int px, int py, unsigned char *str, int mode );	// {384,192} mode string
int CB_PrintMiniC_Fix_ed( int px, int py, unsigned char *str, int mode );	// {384,192} mode char Fixed pitch 12dot edit
void CB_PrintMini_Fix_ed( int px, int py, unsigned char *str, int mode );	// {384,192} mode string Fixed pitch 12dot edit
int CB_PrintMiniC_fx_ed( int px, int py, unsigned char *str, int mode );	// {384,192} mode string fx font
int CB_PrintMiniMiniC_ed( int px, int py, unsigned char *str, int mode );	// {384,192} mode char   MiniMini font
void CB_PrintMiniMini_ed( int px, int py, unsigned char *str, int mode );	// {384,192} mode string   MiniMini font
int CB_PrintMiniMiniBoldC_ed( int px, int py, unsigned char *str, int mode );	// {384,192} mode char   MiniMini font Bold
void CB_PrintMiniMiniBold_ed( int px, int py, unsigned char *str, int mode );	// {384,192} mode string   MiniMini font Bold
int CB_PrintMiniMiniC_Fix_ed( int px, int py, unsigned char *str, int mode );	// {384,192} mode char Fixed pitch 8dot
void CB_PrintMiniMini_Fix_ed( int px, int py, unsigned char *str, int mode );	// {384,192} mode string Fixed pitch 8dot
int CB_PrintMiniMiniBoldC_Fix_ed( int px, int py, unsigned char *str, int mode );	// {384,192} mode char Fixed pitch 9dot
void CB_PrintMiniMiniBold_Fix_ed( int px, int py, unsigned char *str, int mode );	// {384,192} mode string Fixed pitch 9dot
int CB_PrintMiniMiniFXC_ed( int px, int py, unsigned char *str, int mode ) ;	// {384,192} mode char   6x8dot FX font
void CB_PrintMiniMiniFX_ed( int px, int py, unsigned char *str, int mode ) ;	// {384,192} mode string   6x8dot FX font

int CB_PrintModeC( int px, int py, unsigned char *str, int mode);	// {384,192} mode char
void CB_PrintMode( int px, int py, unsigned char *str, int mode, int xlimit );	// {384,192} mode string

void PrintXYR(int ,int ,char *,int );
void Fkey_dispN(int ,char *);
void Fkey_dspRB(int ,char *);
void Fkey_dispRR(int n,char *buf) ;
void Fkey_dispRS(int n,char *buf) ;
void Fkey_dispR(int ,char *);
void Fkey_DISPN(int ,char *);
void Fkey_DISPR(int ,char *);
void FkeyClear(int );
void FkeyClearAll();
void Fkey_Icon(int n, int IconNo);
void Fkey_dispN_Aa(int n, char *buffer) ;
void Fkey_dispN_aA(int n, char *buffer) ;
void Fkey_AreaReverseVRAM( int n, int x1, int x2 );
void Fkey_Icon_color(int n, int IconNo, int color, int backcolor);
void Fkey_dispN_ext( int ,char *, int ofset, int extend, int color, int backcolor);
void Fkey_dispR_ext( int ,char *, int ofset, int extend, int color, int backcolor);
void Fkey_dispRR_ext(int ,char *, int ofset, int extend, int color, int backcolor);
void Fkey_dispRS_ext(int ,char *, int ofset, int extend, int color, int backcolor);
void FkeyMask(int n) ;
void FkeyMaskColor(int n, int backcolor ) ;

void Hex8PrintXY(int , int , char *, unsigned int );
void Hex4PrintXY(int , int , char *, unsigned int );
void Hex2PrintXY(int , int , char *, unsigned int );
int YesNo( char*buffer);
int YesNo2( char*buffer, char*buffer2);
int YesNo_NoSave( char*buffer);
void ErrorMSG(char*, int );
void ErrorMSGstr(char*buffer,char*buffer2);
void ErrorMSGstr1(char*buffer);
void ErrorADRS(char*, int );
void MSG1(char*);
void MSG2(char*, char*);
void MSGpop(void);
void ExitKey();
void OkMSGstr2(char*buffer,char*buffer2);
int YesNo2sub( char*buffer, char*buffer2);

void ProgressBarPopUp( char *buffer, char *buffer2 ) ;
void ProgressBar(int current, int max) ;

