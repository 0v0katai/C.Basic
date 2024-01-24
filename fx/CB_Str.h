extern char  *CB_CurrentStr;	//
extern char  *CB_CurrentStr2;	//

#define CB_StrBufferCNTMax 8
#define CB_StrBufferMax 256
extern char   CB_StrBufferCNT;
extern char  *CB_StrBuffer;	//[CB_StrBufferCNTMax][CB_StrBufferMax];	//

extern char   defaultStrAry;
extern char   defaultStrAryN;
extern short  defaultStrArySize;

extern char   defaultFnAry;
extern char   defaultFnAryN;
extern short  defaultFnArySize;

extern char   defaultGraphAry;
extern char   defaultGraphAryN;
extern short  defaultGraphArySize;
//-----------------------------------------------------------------------------

int StrSrc( char *SrcBase, char *searchstr, int *csrptr, int size);
void StrJoin( char *str1, char *str2, int maxlen ) ;
int StrLen( char *str , int *oplen ) ;
void StrMid( char *str1, char *str2, int n, int m ) ;	// mid$(str2,n,m) -> str1
void StrRight( char *str1, char *str2, int n ) ;	// Right$(str2,n) -> str1
int StrInv( char *str1, char *str2 ) ;	// mirror$(str2) -> str1
int StrRotate( char *str1, char *str2, int n ) ;	// Rotate$("1234567",  2) -> "6712345"
int StrShift( char *str1, char *str2, int n ) ;		// Shift$("1234567",  2) -> "34567"
int StrLwr( char *str1, char *str2 ) ;		// Lwr$(str2, n) -> str1
int StrUpr( char *str1, char *str2 ) ;		// Upr$(str2, n) -> str1

int OpcodeCopy(char *buffer, char *SRC, int Maxlen) ;
void OpcodeStringToAsciiString(char *buffer, char *SRC, int Maxlen ) ;	// Opcode String  ->  Ascii String

void StrDMSsub( char *buffer, double a ) ;	// 
//-----------------------------------------------------------------------------
// Casio Basic
//-----------------------------------------------------------------------------
char* NewStrBuffer();

int CB_GetQuotOpcode(char *SRC, char *buffer, int Maxlen) ;

int CB_IsStr( char *SRC, int execptr ) ;
char* CB_GetOpStr1( char *SRC ,int *maxlen ) ;		// String -> buffer	return
char* CB_GetOpStr( char *SRC, int *maxoplen ) ;	// Get opcode String 
double CB_EvalStrDBL( char *buffer, int calcflag ); //
double CB_EvalStr( char *SRC, int calcflag) ;		// Eval str -> complex
complex CB_Cplx_EvalStrDBL( char *buffer, int calcflag ); //
complex CB_Cplx_EvalStr( char *SRC, int calcflag) ;		// Eval str -> complex
int CBint_EvalStr( char *SRC, int calcflag) ;		// Eval str -> int
void CB_GetLocateStr(char *SRC, char *buffer, int Maxlen ) ;

double  CB_GraphYStr( char *SRC, int calcflag ) ;	//
int CBint_GraphYStr( char *SRC, int calcflag ) ;	// 
double CB_FnStr( char *SRC, int calcflag ) ;	//
int CBint_FnStr( char *SRC, int calcflag ) ;	// 
complex CB_Cplx_GraphYStr( char *SRC, int calcflag ) ;	//
complex CB_Cplx_FnStr( char *SRC, int calcflag ) ;	//

void CB_StorStr( char *SRC ) ;	// ->Stor
void CB_Str( char *SRC );		// "" ""

int CB_StrCmp( char *SRC ) ;
int CB_StrSrc( char *SRC ) ;
int CB_StrLen( char *SRC ) ;
int CB_StrAsc( char *SRC ) ;	// StrAsc("A")  -> 0x41

int CB_StrJoin( char *SRC ) ;
int CB_StrLeft( char *SRC ) ;
int CB_StrMid( char *SRC ) ;
int CB_StrRight( char *SRC ) ;
int CB_ExpToStr( char *SRC ) ;

int CB_StrUpr( char *SRC ) ;
int CB_StrLwr( char *SRC ) ;
int CB_StrInv( char *SRC ) ;
int CB_StrShift( char *SRC ) ;
int CB_StrRotate( char *SRC ) ;
int CB_Sprintf( char *SRC ) ;	// Ssprintf( "%4.4f %d %d", -1.2345,%123,%A)

//int CB_StrDMS( char *SRC ) ;

void StorDATE( char *buffer ) ;	// "2017/01/17" -> DATE
void StorTIME( char *buffer ) ;	// "23:59:59" -> TIME
int DateToStr( char *buffer ) ;	// -> "2017/01/17 TUE"
int TimeToStr( char *buffer ) ;	// -> "23:59:59"
int CB_DateToStr() ;	// -> "2017/01/17 TUE"
int CB_TimeToStr() ;	// -> "23:59:59"

int CB_StrChar( char *SRC ) ;	// StrChar("*"[,n])
int CB_StrCenter( char *SRC );	// StrCenter( Str1,max[,"SpaceChar"])
int CB_EvalToStr( char *SRC );	// Str( n 
int CB_Hex( char *SRC );		// Hex(
int CB_Bin( char *SRC );		// Bin(
int CB_StrBase( char *SRC );	// StrBase( Str1,base1,base2 )->str2
int CB_StrRepl( char *SRC );	// StrRepl( Str1,Str2,Str3,n )->str4

int CB_StrSplit( char *SRC ) ;	// StrStip( "123,4567,89","[n,]) -> MatAns[["1232]["4567"]["89"]]

