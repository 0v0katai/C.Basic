/*
===============================================================================

 Casio Basic RUNTIME Eval library for fx-9860G series      v1.10

===============================================================================
*/
//-----------------------------------------------------------------------------
// Casio Basic Gloval variable
//-----------------------------------------------------------------------------
#define ExpMax 128
extern char ExpBuffer[];
//-----------------------------------------------------------------------------
void CheckMathERR( double *result ) ;
double asinh( double x ) ;
double acosh( double x ) ;
double atanh( double x ) ;
double frac( double x ) ;
double fdegree( double x ) ;	//	Rad,Grad ->Deg
double finvdegree( double x ) ;	//	Deg -> Rad,Grad
double fgrad( double x ) ;		//	Deg,Rad -> Grad
double finvgrad( double x ) ;		//	Grad -> Deg,Rad
double fradian( double x ) ;	//	Deg,Grad ->Rad
double finvradian( double x ) ;	//	Rad -> Deg,Grad
double fsin( double x ) ;
double fcos( double x ) ;
double ftan( double x ) ;
double fasin( double x ) ;
double facos( double x ) ;
double fatan( double x ) ;
double fpolr( double x, double y ) ;	// Pol(x,y) -> r
double fpolt( double x, double y ) ;	// Pol(x,y) -> Theta
double frecx( double r, double t ) ;	// Rec(r,Theta) -> x
double frecy( double r, double t ) ;	// Rec(r,Theta) -> y
double fint( double x ) ;
double fnot( double x ) ;
double fsqrt( double x ) ;
double flog10( double x ) ;
double fpow10( double x ) ;
double fln( double x ) ;
double fexp( double x ) ;
double fcuberoot( double x ) ;
double fsqu( double x ) ;
double ffact( double x ) ;
double frecip( double x ) ;	// ^(-1) RECIP
double ffemto( double x ) ;	// femto
double fpico( double x ) ;	// pico
double fnano( double x ) ;	// nano
double fmicro( double x ) ;	// micro
double fmilli( double x ) ;	// milli
double fKiro( double x ) ;	// Kiro
double fMega( double x ) ;	// Mega
double fGiga( double x ) ;	// Giga
double fTera( double x ) ;	// Tera
double fPeta( double x ) ;	// Peta
double fExa( double x ) ;	// Exa

double fsign( double x ) ;	// -x
double fADD( double x, double y ) ;	// x + y
double fSUB( double x, double y ) ;	// x - y
double fMUL( double x, double y ) ;	// x * y
double fDIV( double x, double y ) ;	// x / y
double fAND( double x, double y ) ;	// x and y
double fOR( double x, double y ) ;	// x or y
double fXOR( double x, double y ) ;	// x xor y
double fNot( double x ) ;	// Not x
double fcmpEQ( double x, double y ) ;	// x = y
double fcmpGT( double x, double y ) ;	// x > y
double fcmpLT( double x, double y ) ;	// x < y
double fcmpNE( double x, double y ) ;	// x != y
double fcmpGE( double x, double y ) ;	// x >= y
double fcmpLE( double x, double y ) ;	// x <= y
double fpow( double x, double y ) ;	// pow(x,y)
double fpowroot( double x, double y ) ;	// powroot(x,y)
double fMOD( double x, double y ) ;	// fMOD(x,y)
double fIDIV( double x, double y ) ;	// (int)x / (int)y
double flogab( double x, double y ) ;	// flogab(x,y)
double frand() ;

double Eval_atof(char *SRC, int c) ;
double InputNumD_fullsub(int x, int y, int width, double defaultNum ) ;
double InputNumD_fullhex(int x, int y, int width, double defaultNum, int hex) ;
double InputNumD_full(int x, int y, int width, double defaultNum) ;
double InputNumD_Char(int x, int y, int width, double defaultNum, char code) ;
double InputNumD_replay(int x, int y, int width, double defaultNum) ;
double InputNumD_CB(int x, int y, int width, double defaultNum) ;
double InputNumD_CB1(int x, int y, int width, double defaultNum) ;
double InputNumD_CB2(int x, int y, int width, double defaultNum) ;
double Eval(char *SRC);
double Evalsub1(char *SRC);
double Evalsub2(char *SRC);
double Evalsub3(char *SRC);
double Evalsub4(char *SRC);
double Evalsub5(char *SRC);
double Evalsub6(char *SRC);
double Evalsub7(char *SRC);
double Evalsub8(char *SRC);
double Evalsub9(char *SRC);
double Evalsub10(char *SRC);
double Evalsub11(char *SRC);
double Evalsub12(char *SRC);
double Evalsub13(char *SRC);
double Evalsub14(char *SRC);
double EvalsubTop(char *SRC);
double CB_EvalDbl( char *SRC ) ;
int RegVar( int c ) ;
int ListRegVar( char *SRC ) ;	// return reg no
//-----------------------------------------------------------------------------
void NewMatListAns( int dimA, int dimB, int base, int element );
void DeleteMatListAns() ;
void DeleteMatListAnsAll() ;
void CopyMatList2Ans( int reg ) ;	// List 1 -> ListAns
void CopyMatList2AnsTop( int reg ) ;	// List 1 -> ListAns top
void CopyAns2MatList( char* SRC, int reg ) ;	// ListAns -> List 1
int CheckAnsMatList( int reg ) ;	// ListAns <> List 1
void WriteListAns2( double x, double y ) ;

double ListEvalsub1(char *SRC) ;
double ListEvalsub2(char *SRC) ;
double ListEvalsub3(char *SRC) ;
double ListEvalsub4(char *SRC) ;
double ListEvalsub5(char *SRC) ;
double ListEvalsub6(char *SRC) ;
double ListEvalsub7(char *SRC) ;
double ListEvalsub8(char *SRC) ;
double ListEvalsub9(char *SRC) ;
double ListEvalsub10(char *SRC) ;
double ListEvalsub11(char *SRC) ;
double ListEvalsub12(char *SRC) ;
double ListEvalsub13(char *SRC) ;
double ListEvalsub14(char *SRC) ;
double NoListEvalsubTop(char *SRC) ;	//  
double ListEvalsubTop(char *SRC) ;
int ListEvalsub1Ans(char *SRC) ;	//  
int ListEvalsubTopAns(char *SRC) ;	//  

double CB_Sigma( char *SRC ) ; //	Sigma(X^2,X,1.10)
int CB_SigmaInt( char *SRC ) ; //	Sigma(X^2,X,1.10)


