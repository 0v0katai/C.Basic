extern int KeyRepeatFirstCount;	// pointer to repeat time of first repeat (20:default)
extern int KeyRepeatNextCount;	// pointer to repeat time of second repeat( 5:default)
extern int selectSetup;
extern int selectVar;
extern int selectMatrix;
extern const char VerMSG[];

#define PageUpDownNumDefault 1
extern short PageUpDownNum;		// Pageup/down count
extern char DisableDebugMode;		// 0:enable debug mode
extern char ExitDebugModeCheck;  // ExitDebugModeCheck
extern char ForceDebugMode;
extern char  EditTopLine ;		// 1: use top of line (edit)
extern char  EditFontSize ;		// 0:standard  1:mini
extern char  EditListChar ;		// 1: List character change
extern char  EditExtFont;	// Edit ext Font enable:1

extern char  ExtendPict;	// 0:20  1~79:21~99
extern char  ExtendList;	// 0:52(default) 1:+16 ~ 63:+1008
extern char  ForceReturnMode;	// 0:none  1:F1 2:EXE  3:Both
extern char  ForceReturn;		// 0:none  1:return

extern char  CB_RecoverSetup;	// setup recover flag 0:none 1:recover

extern char  EnableExtFont ;	// enable external font
extern char  MaxMemMode ;		// Maximam memory mode
extern char  ComplexMode ;		// Complex mode  real:0   a+bi:1  a_b:2
extern const char *CBmode[];    //{"DBL#","INT%","CPLX"};
extern char  CB_fx5800P ;		// fx-5800P mode
extern char  CB_HelpOn;			// Help function
extern char  CB_EditIndent;		// indent function  0:Non  1~4:indent space

extern int StackPtr;

//------------------------------------------------------------------------------
void VerDispSub() ;
void VerDisp() ;
unsigned int MathKey( unsigned int  key) ;

int CB_Version() ;	// Version
int CB_System( char *SRC ) ;	// System( n )

void SetXdotYdot();
void SetVeiwWindowInit();
void SetVeiwWindowTrig();
void SetVeiwWindowSTD();

int SetViewWindwo(void);
void SetFactor();
int SetVar(int select);
int SetupG(int select, int limit);
int SetVarChar( char *buffer, int c ) ;
int SetVarCharMat( char *buffer, int c ) ;
int SetVarCharVct( char *buffer, int c ) ;
#define SetVarCharVct SetVarCharMat

int SelectNum1( char*msg, int n ,int min, int max, unsigned int *key) ;		// 

//------------------------------------------------------------------------------
