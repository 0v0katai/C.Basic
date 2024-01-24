extern int KeyRepeatFirstCount;	// pointer to repeat time of first repeat (20:default)
extern int KeyRepeatNextCount;	// pointer to repeat time of second repeat( 5:default)
extern int selectSetup;
extern int selectVar;
extern int selectMatrix;

#define PageUpDownNumDefault 1
extern short PageUpDownNum;		// Pageup/down count
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

//------------------------------------------------------------------------------
