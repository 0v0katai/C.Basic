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
int MatOprandInt( char *SRC, int *reg);
void CBint_Store( char *SRC );
void CBint_For( char *SRC ,StkFor *StackFor, CurrentStk *CurrentStruct );
void CBint_Next( char *SRC ,StkFor *StackFor, CurrentStk *CurrentStruct );
void CBint_Dsz( char *SRC ) ; //	Dsz
void CBint_Isz( char *SRC ) ; //	Isz

void CBint_PxlOprand( char *SRC, int *py, int *px) ;
void CBint_PxlOn( char *SRC ) ; //	PxlOn
void CBint_PxlOff( char *SRC ) ; //	PxlOff
void CBint_PxlChg( char *SRC ) ; //	PxlChg

int CBint_BinaryEval( char *SRC ) ;
int CBint_UnaryEval( char *SRC ) ;

int CBint_interpreter( char *SRC) ;
