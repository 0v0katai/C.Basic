/*
===============================================================================

 Casio Basic RUNTIME EvalInt library for fx-9860G series      v0.80

===============================================================================
*/
//-----------------------------------------------------------------------------
// Casio Basic Gloval variable
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
typedef struct Ev1i {
	int flag;
	int value;
};
//-----------------------------------------------------------------------------
int fintint( int x ) ;
int fabsint( int x ) ;
int fnotint( int x ) ;
int fsqrtint( int x ) ;
int flog10int( int x ) ;
int fpow10int( int x ) ;
int flnint( int x ) ;
int ffracint( int x ) ;
int fexpint( int x ) ;
int fcuberootint( int x ) ;
int fsquint( int x ) ;
int ffactint( int x ) ;
int frecipint( int x ) ;	// ^(-1) RECIP
int fsignint( int x ) ;	// -x
int fADDint( int x, int y ) ;	// x + y
int fSUBint( int x, int y ) ;	// x - y
int fMULint( int x, int y ) ;	// x * y
int fDIVint( int x, int y ) ;	// x / y
int fANDint( int x, int y ) ;	// x & y
int fORint( int x, int y ) ;	// x | y
int fXORint( int x, int y ) ;	// x ^ y
int fNotint( int x ) ;			// ! x
int fANDint_logic( int x, int y ) ;	// x && y
int fORint_logic( int x, int y ) ;	// x || y
int fXORint_logic( int x, int y ) ;	// (x!=0) ^ (y!=0)
int fNotint_logic( int x ) ;		//  x == 0
int fcmpEQint( int x, int y ) ;	//  x = y
int fcmpGTint( int x, int y ) ;	//  x > y
int fcmpLTint( int x, int y ) ;	//  x < y
int fcmpNEint( int x, int y ) ;	//  x != y
int fcmpGEint( int x, int y ) ;	//  x >= y
int fcmpLEint( int x, int y ) ;	//  x <= y
int fpowint( int x, int y ) ;	// pow(x,y)
int fpowrootint( int x, int y ) ;	// powroot(x,y)
int flogabint( int x, int y ) ;	// flogab(x,y)
int frandint() ;
int frandIntint( int x, int y ) ;
int fMODint( int x, int y ) ;	// fMODint(x,y)
int fGCDint( int x, int y ) ;	// GCD(x,y)
int fLCMint( int x, int y ) ;	// LCM(x,y)
//-----------------------------------------------------------------------------
int InputNumI_full(int x, int y, int width, int defaultNum) ;
int InputNumI_Char(int x, int y, int width, int defaultNum, char code) ;
int InputNumI_replay(int x, int y, int width, int defaultNum) ;
int InputNumI_CB(int x, int y, int width, int defaultNum) ;
int InputNumI_CB1(int x, int y, int width, int defaultNum) ;
int InputNumI_CB2(int x, int y, int width, int defaultNum) ;
int EvalInt(char *SRC);
int EvalIntsub1(char *SRC);
int EvalIntsub5(char *SRC);
int EvalIntsub6(char *SRC);
int EvalIntsub7(char *SRC);
int EvalIntsub14(char *SRC);
int EvalIntsubTop(char *SRC);
int CB_EvalInt( char *SRC ) ;
int Eval_atoi(char *SRC, int c ) ;

int EvalInt(char *SRC) ;		// Eval temp
int CB_RTC_GetTicks() ;
int CB_GetkeyEntry( char *SRC ) ;
int CB_Getkey3( char *SRC ) ;
int CB_KeyRow( char *SRC ) ;		// Row Keyscan


int ListEvalIntsub1(char *SRC) ;
int ListEvalIntsub2(char *SRC) ;
int ListEvalIntsub3(char *SRC) ;
int ListEvalIntsub4(char *SRC) ;
int ListEvalIntsub5(char *SRC) ;
int ListEvalIntsub6(char *SRC) ;
int ListEvalIntsub7(char *SRC) ;
int ListEvalIntsub8(char *SRC) ;
int ListEvalIntsub9(char *SRC) ;
int ListEvalIntsub10(char *SRC) ;
int ListEvalIntsub11(char *SRC) ;
int ListEvalIntsub12(char *SRC) ;
int ListEvalIntsub13(char *SRC) ;
int ListEvalIntsub14(char *SRC) ;
int NoListEvalIntsubTop(char *SRC) ;	//  
int ListEvalIntsubTop(char *SRC) ;
int ListEvalIntsub1Ans(char *SRC) ;	//  
int ListEvalIntsubTopAns(char *SRC) ;	//  

