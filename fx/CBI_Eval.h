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
int EvalIntsubTop(char *SRC);

