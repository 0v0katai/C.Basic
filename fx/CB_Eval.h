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
double ListEvalsub1Top(char *SRC) ;
double ListEvalsub1Ans(char *SRC) ;	//  
double ListEvalsubTop(char *SRC) ;
double ListEvalsubTopAns(char *SRC) ;	//  
double NoListEvalsubTop(char *SRC) ;	//  

double CB_Sigma( char *SRC ) ; //	Sigma(X^2,X,1.10)


