//-----------------------------------------------------------------------------
// Casio Basic inside
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Casio Basic Gloval variable
//-----------------------------------------------------------------------------
extern int  REGINT[59];

#define regintA REGINT[ 0]
#define regintB REGINT[ 1]
#define regintC REGINT[ 2]
#define regintD REGINT[ 3]
#define regintE REGINT[ 4]
#define regintF REGINT[ 5]
#define regintG REGINT[ 6]
#define regintH REGINT[ 7]
#define regintI REGINT[ 8]
#define regintJ REGINT[ 9]
#define regintK REGINT[10]
#define regintL REGINT[11]
#define regintM REGINT[12]
#define regintN REGINT[13]
#define regintO REGINT[14]
#define regintP REGINT[15]
#define regintQ REGINT[16]
#define regintR REGINT[17]
#define regintS REGINT[18]
#define regintT REGINT[19]
#define regintU REGINT[20]
#define regintV REGINT[21]
#define regintW REGINT[22]
#define regintX REGINT[23]
#define regintY REGINT[24]
#define regintZ REGINT[25]

extern int CBint_CurrentValue;	// Ans

//------------------------------------------------------------------------------
int MatOprandInt( unsigned char *SRC, int *reg);
void CBint_Store( unsigned char *SRC );
int  CBint_Input( unsigned char *SRC );
void CBint_Locate( unsigned char *SRC );
void CBint_Text( unsigned char *SRC, int *dspflag ) ; //	Text
void CBint_For( unsigned char *SRC ,int *StackForPtr, int *StackForAdrs, int *StackForVar, int *StackForEndint, int *StackForStepint, int *CurrentStructCNT, int *CurrentStructloop);
void CBint_Next( unsigned char *SRC ,int *StackForPtr, int *StackForAdrs, int *StackForVar, int *StackForEndint, int *StackForStepint, int *CurrentStructCNT, int *CurrentStructloop );
void CBint_Dsz( unsigned char *SRC ) ; //	Dsz
void CBint_Isz( unsigned char *SRC ) ; //	Isz

void CBint_PxlOprand( unsigned char *SRC, int *py, int *px) ;
void CBint_PxlOn( unsigned char *SRC ) ; //	PxlOn
void CBint_PxlOff( unsigned char *SRC ) ; //	PxlOff
void CBint_PxlChg( unsigned char *SRC ) ; //	PxlChg

int CBint_BinaryEval( unsigned char *SRC ) ;
int CBint_UnaryEval( unsigned char *SRC ) ;

int CBint_interpreter( unsigned char *SRC) ;

void CBint_test() ;
