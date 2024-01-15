#define VERSION 147

#define MINHEAP    1		// use heap default
#define MAXHEAP  127		// use heap max kbyte
#define MAXHEAP1 117		// use heap max kbyte
#define MAXHEAP2  96		// use heap max kbyte
#define MAXHEAPDEFAULT  86	// use heap default

extern char  MaxHeapRam  ;		// Max Heap Ram size Kbyte
extern char  EditGBFont  ;		// enable GB font
extern char  EnableExtFont ;	// enable external font
extern char  DirectGBconvert ;	// GB code direct conversion for Text

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
extern char  EditFontSize ;		// 0:standard  1:mini  2:minifix 4:rev    +0x10:line-number
extern char  EditListChar ;		// 1: List character change
extern char  EditExtFont;	// Edit ext Font enable:1

extern char  ExtendPict;	// 0:20  1~79:21~99
extern char  ExtendList;	// 0:52(default) 1:+52 ~ 19:+988=1040
extern char  ForceReturnMode;	// 0:none  1:F1 2:EXE  3:Both
extern char  ForceReturn;		// 0:none  1:return

extern char  CB_RecoverSetup;	// setup recover flag 0:none 1:recover

extern char  ComplexMode ;		// Complex mode  real:0   a+bi:1  a_b:2
extern const char *CBmode[];    //{"DBL#","INT%","CPLX"};

extern char  CB_disableOC;		// auto overclock enable :1
extern char  reentrant_SetRGBColor ;	//	reentrant
extern char  CB_fx5800P ;		// fx-5800P mode
extern char  CB_StatusDisp;		// StatusDisp off:0  on:1
extern int  IsEmu;		//
extern char  CB_G3M_TEXT;
extern char  CB_ClrPict;		// 0:clear	 1:non clear
extern char  CB_HelpOn;			// Help function
extern char  CB_EditIndent;		// indent function  0:Non  1~4:indent space

extern char BG_filename[64];

extern int StackPtr;

//------------------------------------------------------------------------------
void VerDispSub() ;
void VerDisp() ;
unsigned int XTTKey( unsigned int  key) ;
unsigned int MathKey( unsigned int  key) ;

int CB_Version() ;	// Version
int CB_System( char *SRC ) ;	// System( n )

void SetXdotYdot();
void SetVeiwWindowInit();
void SetVeiwWindowTrig();
void SetVeiwWindowSTD();

int SetViewWindow(void);		// ----------- Set  View Window variable	return 0: no change  -1 : change
void SetFactor();
int SetVar(int select);
int SetupG(int select, int limit );
int SetVarChar( char *buffer, int c ) ;
int SetVarCharMat( char *buffer, int c ) ;
int SetVarCharVct( char *buffer, int c ) ;
#define SetVarCharVct SetVarCharMat

int SelectNum1( char*msg, int n ,int min, int max, int *key) ;		//
void FkeyS_L_();

extern int SetRGBColorMode;	// 0:RGB  1:HSV   2:HSL
int SetRGBColor( unsigned short *color );	// 1-
//------------------------------------------------------------------------------
