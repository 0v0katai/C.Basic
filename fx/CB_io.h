//-----------------------------------------------------------------------------
struct st_round {
	char MODE;	//    mode :   0:normal  1:Fix   2: Sci
	char DIGIT;	//    digits   0~15
};

extern struct st_round CB_Round;	// struct Round
extern char ENG;	// ENG flag  1:ENG  3:3digit separate

#define Norm 0
#define Fix  1
#define Sci  2

extern char UseHiddenRAM;		// 0: no use   1: use hiddenRAM
extern char IsHiddenRAM;		// 0: hiddenRAM not exist   1: exist

extern char * HiddenRAM_Top ;			// Hidden RAM TOP
extern char * HiddenRAM_End ;			// Hidden RAM END

extern char * HiddenRAM_ProgNextPtr;	// Hidden RAM Prog next ptr
extern char * HiddenRAM_MatTopPtr;		// Hidden RAM Mat top ptr

extern char IsSH3;	//	1:SH3   0:SH4

#define FKeyNo1	0
#define FKeyNo2	1
#define FKeyNo3	2
#define FKeyNo4	3
#define FKeyNo5	4
#define FKeyNo6	5

//-----------------------------------------------------------------------------

int CPU_check(void);
void * HiddenRAM(void);
void * HiddenRAM_mallocMat( size_t size );
void * HiddenRAM_mallocProg( size_t size );
unsigned char *  HiddenRAM_mallocPict( int pictNo );
void HiddenRAM_freeProg( void *ptr );
void HiddenRAM_freeMat( int reg );
void HiddenRAM_MatAryInit();	// HiddenRAM Initialize
void HiddenRAM_MatAryStore();	// MatAry ptr -> HiddenRAM
int  HiddenRAM_MatAryRestore();	//  HiddenRAM -> MatAry ptr	 1:ok
void HiddenRAM_ExtFontAryInit() ;

void CB_PrintC_ext( int x, int y,const unsigned char *c, int extflag );
void CB_PrintRevC_ext( int x, int y,const unsigned char *c, int extflag );
void CB_Print_ext( int x, int y,const unsigned char *c, int extflag );
void CB_PrintRev_ext( int x, int y,const unsigned char *c, int extflag );
void CB_PrintC( int x, int y, const unsigned char *c);
void CB_PrintRevC( int x, int y, const unsigned char *c);
void CB_Print( int x, int y, const unsigned char *str);
void CB_PrintRev( int x, int y, const unsigned char *str);
void CB_PrintXYC( int px, int py,const unsigned char *c , int mode );
void CB_PrintXY( int px, int py, const unsigned char *str, int mode );
void CB_PrintMini( int px, int py, const unsigned char *str, int mode);
int  CB_PrintMiniC( int px, int py, const unsigned char *str, int mode);
int  CB_PrintMiniLength( unsigned char *str, int extflag  );
int  CB_PrintMiniLengthStr( unsigned char *str, int extflag  );

void PrintXYR(int ,int ,char *,int );
void Fkey_dispN(int ,char *);
void Fkey_dspRB(int ,char *);
void Fkey_dispR(int ,char *);
void Fkey_DISPN(int ,char *);
void Fkey_DISPR(int ,char *);
void FkeyClear(int );
void Fkey_Icon(int n, int IconNo);
void Fkey_dispN_Aa(int n, char *buffer) ;
void Fkey_dispN_aA(int n, char *buffer) ;


void Hex8PrintXY(int , int , char *, unsigned int );
void Hex4PrintXY(int , int , char *, unsigned int );
void Hex2PrintXY(int , int , char *, unsigned int );
int YesNo( char*buffer);
int YesNo2( char*buffer, char*buffer2);
void ErrorMSG(char*, int );
void ErrorMSGstr(char*buffer,char*buffer2);
void ErrorMSGstr1(char*buffer);
void ErrorADRS(char*, int );
void MSG1(char*);
void MSG2(char*, char*);
void MSGpop(void);
int YesNo2sub( char*buffer, char*buffer2);
void OkMSGstr2(char*buffer,char*buffer2);

void ProgressBarPopUp( char *buffer, char *buffer2 ) ;
void ProgressBar(int current, int max) ;
