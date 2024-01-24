#define STRLENMAX 32

typedef struct{
	char str[STRLENMAX];
	int flag;
} Ststring;

extern char  Str[STRLENMAX*20];
extern char* StrPtr[20];

int StrSrc( char *str ) ;
int StrJoin( char *str1, char *str2 ) ;
int StrLen( char *str ) ;
int StrLeft( char *str, int n ) ;
int StrMid( char *str, int n, int m ) ;
int StrRight( char *str, int n ) ;
int StrInv( char *str ) ;
int StrRotate( char *str, int n ) ;
int StrShift( char *str, int n ) ;
int StrLwr( char *str ) ;
int StrUpr( char *str ) ;
//-----------------------------------------------------------------------------
// Casio Basic
//-----------------------------------------------------------------------------
void CB_Exp( char *SRC ) ;
void CB_ExpToStr( char *SRC ) ;
void CB_StrCmp( char *SRC ) ;
void CB_StrSrc( char *SRC ) ;

void CB_StrJoin( char *SRC ) ;
void CB_StrLen( char *SRC ) ;
void CB_StrLeft( char *SRC ) ;
void CB_StrMid( char *SRC ) ;
void CB_StrRight( char *SRC ) ;

void CB_StrInv( char *SRC ) ;
void CB_StrRotate( char *SRC ) ;
void CB_StrShift( char *SRC ) ;
void CB_StrLwr( char *SRC ) ;
void CB_StrUpr( char *SRC ) ;
