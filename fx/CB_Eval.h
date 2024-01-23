/*
===============================================================================

 Casio Basic RUNTIME Eval library for fx-9860G series      v0.20

===============================================================================
*/
//-----------------------------------------------------------------------------
// Casio Basic Gloval variable
//-----------------------------------------------------------------------------
extern int ExpPtr;
//-----------------------------------------------------------------------------
double InputNumD_full(int x, int y, int width, double defaultNum) ;
double InputNumD_Char(int x, int y, int width, double defaultNum, char code) ;
double InputNumD_replay(int x, int y, int width, double defaultNum) ;
double InputNumD_CB(int x, int y, int width, double defaultNum) ;
double InputNumD_CB1(int x, int y, int width, double defaultNum) ;
double InputNumD_CB2(int x, int y, int width, double defaultNum) ;
double Eval(unsigned char *expbuf);
double Evalsub0(unsigned char *expbuf);
double Evalsub1(unsigned char *expbuf);
double Evalsub5(unsigned char *expbuf);
double Evalsub6(unsigned char *expbuf);
double Evalsub7(unsigned char *expbuf);
double EvalsubTop(unsigned char *expbuf);

