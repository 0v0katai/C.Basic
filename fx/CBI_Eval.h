/*
===============================================================================

 Casio Basic RUNTIME EvalInt library for fx-9860G series      v0.40

===============================================================================
*/
//-----------------------------------------------------------------------------
// Casio Basic Gloval variable
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
int InputNumI_full(int x, int y, int width, int defaultNum) ;
int InputNumI_Char(int x, int y, int width, int defaultNum, char code) ;
int InputNumI_replay(int x, int y, int width, int defaultNum) ;
int InputNumI_CB(int x, int y, int width, int defaultNum) ;
int InputNumI_CB1(int x, int y, int width, int defaultNum) ;
int InputNumI_CB2(int x, int y, int width, int defaultNum) ;
int EvalInt(unsigned char *expbuf);
int EvalIntsub1(unsigned char *expbuf);
int EvalIntsub5(unsigned char *expbuf);
int EvalIntsub6(unsigned char *expbuf);
int EvalIntsub7(unsigned char *expbuf);
int EvalIntsubTop(unsigned char *expbuf);

