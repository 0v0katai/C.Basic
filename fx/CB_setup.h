extern int KeyRepeatFirstCount;	// pointer to repeat time of first repeat (20:default)
extern int KeyRepeatNextCount;	// pointer to repeat time of second repeat( 5:default)
extern int selectSetup;
extern int selectVar;
extern int selectMatrix;
extern const char VerMSG[];

#define PageUpDownNumDefault 1
extern short PageUpDownNum;		// Pageup/down count
extern char AutoDebugMode;		// 0:disable debug mode
extern char ForceDebugMode;
extern char  EditTopLine ;		// 1: use top of line (edit)
extern char  EditFontSize ;		// 0:standard  1:mini
extern char  EditListChar ;		// 1: List character change
extern char  ExtendPict;	// 0:20  1~79:21~99
extern char  ExtendList;	// 0:52(default) 1:+16 ~ 63:+1008

//------------------------------------------------------------------------------
void VerDispSub() ;
void VerDisp() ;
unsigned int MathKey( unsigned int  key) ;

void SetVeiwWindowInit();
void SetVeiwWindowTrig();
void SetVeiwWindowSTD();

int SetViewWindwo(void);
void SetFactor();
int SetVar(int select);
int SetupG(int select);
int SetVarChar( char *buffer, int c ) ;

int SelectNum2( char*msg, int n ,int min, int max) ;		// 

//------------------------------------------------------------------------------
