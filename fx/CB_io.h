//-----------------------------------------------------------------------------
struct st_round {
	int MODE;	//    mode :   0:normal  1:Fix   2: Sci
	int DIGIT;	//    digits   0~15
};

extern struct st_round CB_Round;	// struct Round
extern int ENG;	// ENG flag
extern char UseHiddenRAM;		// 0: no use   1: use hiddenRAM
extern char IsHiddenRAM;		// 0: hiddenRAM not exist   1: exist

#define Norm 0
#define Fix  1
#define Sci  2

//-----------------------------------------------------------------------------

int CPU_check(void);
void * HiddenRAM(void);

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

