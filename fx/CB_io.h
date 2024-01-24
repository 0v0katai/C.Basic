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


//-----------------------------------------------------------------------------

int CPU_check(void);
void * HiddenRAM(void);
void * HiddenRAM_mallocMat( size_t size );
void * HiddenRAM_mallocProg( size_t size );
void HiddenRAM_freeProg( void *ptr );
void HiddenRAM_freeMat( int reg );

void CB_Print( int x, int y, const unsigned char *str);
void CB_PrintRev( int x, int y, const unsigned char *str);
void CB_PrintC( int x, int y, const unsigned char *c);
void CB_PrintRevC( int x, int y, const unsigned char *c);
void CB_PrintXYC( int px, int py,const unsigned char *c , int mode );
void CB_PrintXY( int px, int py, const unsigned char *str, int mode );
void CB_PrintMini( int px, int py, const unsigned char *str, int mode);

void PrintXYR(int ,int ,char *,int );
void Fkey_dispN(int ,char *);
void Fkey_dispR(int ,char *) ;
void Fkey_DISPN(int ,char *);
void Fkey_DISPR(int ,char *);
void Fkey_Clear(int );
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

