extern int KeyRepeatFirstCount;	// pointer to repeat time of first repeat (20:default)
extern int KeyRepeatNextCount;	// pointer to repeat time of second repeat( 5:default)
extern int selectSetup;
extern int selectVar;
extern int selectMatrix;
extern const char VerMSG[];

#define PageUpDownNumDefault 1
extern short PageUpDownNum;		// Pageup/down count
extern int8_t  DisableDebugMode;		// 0:enable debug mode
extern int8_t  ExitDebugModeCheck;  // ExitDebugModeCheck
extern int8_t  ForceDebugMode;
extern int8_t   EditTopLine ;		// 1: use top of line (edit)
extern int8_t   EditFontSize ;		// 0:standard  1:mini
extern int8_t   EditListChar ;		// 1: List character change
extern int8_t   EditExtFont;	// Edit ext Font enable:1

extern int8_t   ExtendPict;	// 0:20  1~79:21~99
extern int8_t   ExtendList;	// 0:52(default) 1:+16 ~ 63:+1008
extern int8_t   ForceReturnMode;	// 0:none  1:F1 2:EXE  3:Both
extern int8_t   ForceReturn;		// 0:none  1:return

extern int8_t   CB_RecoverSetup;	// setup recover flag 0:none 1:recover

extern int8_t   EnableExtFont ;	// enable external font
extern int8_t   MaxMemMode ;		// Maximam memory mode
extern int8_t   ComplexMode ;		// Complex mode  real:0   a+bi:1  a_b:2
extern const char *CBmode[];    //{"DBL#","INT%","CPLX"};
extern int8_t   CB_fx5800P ;		// fx-5800P mode
extern int8_t   CB_HelpOn;			// Help function
extern int8_t   CB_EditIndent;		// indent function  0:Non  1~4:indent space

extern int StackPtr;

//------------------------------------------------------------------------------
void VerDispSub( int flag ) ;
void VerDisp( int flag ) ;
unsigned int XTTKey( unsigned int  key) ;
unsigned int MathKey( unsigned int  key) ;

int CB_Version() ;	// Version
int CB_System( char *SRC ) ;	// System( n )

void SetXdotYdot();
void SetVeiwWindowInit();
void SetVeiwWindowTrig();
void SetVeiwWindowSTD();

int SetViewWindow();
void SetFactor();
int SetVar(int select);
int SetupG(int select, int limit);
int SetVarChar( char *buffer, int c ) ;
int SetVarCharMat( char *buffer, int c ) ;
int SetVarCharVct( char *buffer, int c ) ;
#define SetVarCharVct SetVarCharMat

int SelectNum1( char*msg, int n ,int min, int max, unsigned int *key) ;		// 
void FkeyS_L_();

//------------------------------------------------------------------------------
