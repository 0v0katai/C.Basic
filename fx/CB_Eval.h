/*
===============================================================================

 Casio Basic RUNTIME Eval library for fx-9860G series      v0.99

===============================================================================
*/
//-----------------------------------------------------------------------------
// Casio Basic Gloval variable
//-----------------------------------------------------------------------------
#define ExpMax 128
extern char ExpBuffer[];
//-----------------------------------------------------------------------------
void CheckMathERR( double *result ) ;
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
double Evalsub5(char *SRC);
double Evalsub6(char *SRC);
double Evalsub7(char *SRC);
double Evalsub13(char *SRC);
double EvalsubTop(char *SRC);
double CB_EvalDbl( char *SRC ) ;

