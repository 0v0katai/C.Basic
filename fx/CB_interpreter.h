
//------------------------------------------------------------------------------
extern double CursorX;
extern double CursorY;

extern int ExecPtr;
extern int BreakPtr;

extern char   CB_CurrentStr[128];	//
extern double CB_CurrentValue;	// Ans

extern int CB_round;	// round mode    0:normal  1:Fix  2:Sci
extern int CB_fix;
extern int CB_sci;

#define ProgMax 10
extern int ProgEntryN;		// how many subroutin
extern int ProgNo;			// current Prog No
extern unsigned char *ProgfileAdrs[ProgMax+1];
extern int ProgfileMax[ProgMax+1] ;	// Max edit filesize 
extern int ProgfileEdit[ProgMax+1];	// no change : 0     edited : 1

#define StackGotoMax 10+26+6+26+1
#define StackForMax 8
#define StackWhileMax 8
#define StackDoMax 8

extern int BreakCheck;	// Break Stop on/off

//------------------------------------------------------------------------------
int GetQuotOpcode(unsigned char *SRC, unsigned char *buffer ) ;
void GetLocateStr(unsigned char *SRC, unsigned char *buffer ) ;
void Skip_block( unsigned char *SRC );

int StoPict( int pictNo );
void RclPict( int pictNo );

int CB_interpreter( unsigned char *SRC) ;

void CB_test() ;
