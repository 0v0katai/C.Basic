
//------------------------------------------------------------------------------
extern double CursorX;
extern double CursorY;

extern int ExecPtr;
extern int BreakPtr;

extern char   CB_CurrentStr[128];	//
extern double CB_CurrentValue;	// Ans

#define ProgMax 10
extern int ProgEntryPtr;		// how many subroutin
extern int ProgNo;			// current Prog No
extern unsigned char *ProgfileAdrs[ProgMax+1];
extern int ProgfileMax[ProgMax+1] ;	// Max edit filesize 
extern int ProgfileEdit[ProgMax+1];	// no change : 0     edited : 1

//------------------------------------------------------------------------------
void Skip_block( unsigned char *SRC );

void CB_ProgEntry( unsigned char *SRC ) ; //	Prog "..." into memory

int CB_interpreter( unsigned char *SRC) ;

void CB_test() ;
