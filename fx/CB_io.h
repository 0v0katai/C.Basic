//-----------------------------------------------------------------------------
struct st_round {
	int MODE;	//    mode :   0:normal  1:Fix   2: Sci
	int DIGIT;	//    digits   0~15
};

extern struct st_round CB_Round;	// struct Round
extern int ENG;	// ENG flag

#define Norm 0
#define Fix  1
#define Sci  2

//-----------------------------------------------------------------------------

int CPU_check(void);

void PrintXYR(int ,int ,char *,int );
void Fkey_dispN(int ,char *);
void Fkey_dispR(int ,char *) ;
void Fkey_DISPN(int ,char *);
void Fkey_DISPR(int ,char *);
void Fkey_Clear(int );
void Hex8PrintXY(int , int , char *, unsigned int );
void Hex4PrintXY(int , int , char *, unsigned int );
void Hex2PrintXY(int , int , char *, unsigned int );
int YesNo( char*);
void ErrorMSG(char*, int );
void ErrorADRS(char*, int );
void MSG1(char*);
void MSG2(char*, char*);
void MSGpop(void);

