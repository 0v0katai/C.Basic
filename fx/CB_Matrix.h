#define MatAryMax 26+6+26+1
//extern int	MatAryMaxbyte[MatAryMax];		// Matrix array max memory size
//extern char	MatAryBase[MatAryMax]			// Matrix array base 0 or 1
//extern char	MatAryElementSize[MatAryMax];	// Matrix array word size
//extern short	MatArySizeA[MatAryMax];			// Matrix array size m Y
//extern short	MatArySizeB[MatAryMax];			// Matrix array size n X
//extern double *MatAry[MatAryMax];				// Matrix array ptr*

typedef struct {		// 20 bytes
	int		Maxbyte;
	short	Base;
	short	ElementSize;
	short	SizeA;
	short	SizeB;
	double *Adrs;
} matary;

extern matary MatAry[MatAryMax];

extern char	MatBaseDefault;
extern char	MatBase;

extern char   CB_MatListAnsreg;

extern char listdsp;	// Mat:0  list:1

//------------------------------------------------------------------------------
int ReadMatrixInt( int reg, int dimA, int dimB);				// 0-
double ReadMatrix( int reg, int dimA, int dimB);				// 0-
void WriteMatrixint( int reg, int dimA, int dimB, int value);	// 0-
void WriteMatrix( int reg, int dimA, int dimB, double value);	// 0-

char *  MatrixPtr( int reg, int m, int n );						// 0-
int	MatrixSize( int reg, int sizeA, int sizeB ) ;	// size 1-

int DimMatrixDefaultElementSize() ;
int DimMatrixSub( int reg, int ElementSize, int dimA, int dimA, int base ) ;	// 1-
int DimMatrix( int reg, int dimA, int dimB, int base ) ;						// 1-
int MatElementPlus( int reg, int m, int n ) ;	// 1-
void CopyMatrix( int reg2, int reg ) ;	// reg -> reg2

void DeleteMatrix( int reg ) ;
void InitMatSub( int reg, double value );
void InitMatIntSub( int reg, int value );
void NumToHex( char *buffer, unsigned int n, int digit) ;
void EditMatrix(int reg, int ans );		// ----------- Edit Matrix
int SetMatrix(int select);		// ----------- Set Matrix

int ElementSizeSelect( char *SRC, int *base ) ;
void CB_MatrixInitsubNoMat( char *SRC, int *reg, int dimA, int dimB , int ElementSize ) ; 	// 1-
void CB_MatrixInitsub( char *SRC, int *reg, int dimA, int dimB , int ElementSize ) ; 	// 1-
void CB_MatrixInit( char *SRC ) ; //	{n,m}->Dim Mat A[.B][.W][.L][.F]
void CB_MatrixInit2( char *SRC ) ; //	[[1.2,3][4,5,6]]->Mat A[.B][.W][.L][.F]
void CB_MatrixInit2Str( char *SRC ) ; //	["ABCD","12345","XYZ"]->Mat A[.B]
void CB_ClrMat( char *SRC ) ; //	ClrMat A
int CB_MatCalc( char *SRC ) ; //	Mat A -> Mat B  etc
void CB_MatFill( char *SRC ) ; //	Fill(value, Mat A)
void CB_MatTrn( char *SRC ) ; //	Trn Mat A


//int MatOprand(    char *SRC, int reg );
//int MatOprandInt( char *SRC, int reg );
int EvalEndCheck( int c ) ;
int MatEndCheck( int c ) ;
void MatOprand1num( char *SRC, int reg, int *dimA, int *dimB );	// A0,A1,b3,c9 etc. error check
void MatOprand1(    char *SRC, int reg, int *dimA, int *dimB );		// 0-
void MatOprand2(    char *SRC, int reg, int *dimA, int *dimB );		// 0-
void MatOprandInt1( char *SRC, int reg, int *dimA, int *dimB );		// 0-
void MatOprandInt2( char *SRC, int reg, int *dimA, int *dimB );		// 0-
int  MatrixOprand( char *SRC, int *reg, int *dimA, int *dimB );		// 0-
int MatrixOprandreg( char *SRC, int *reg) ;

int Cellsum( int reg, int x, int y );								// 0-

//-----------------------------------------------------------------------------

void CB_ListInitsub( char *SRC, int *reg, int dimA , int ElementSize ) ; 	// 1-
void CB_ListInit2( char *SRC ) ; //	{1.2,3,4,5,6}->List 1[.B][.W][.L][.F]
void CB_Seq( char *SRC ) ; //	Seq(X^2,X,1.10,1)->List 1[.B][.W][.L][.F]
int CB_ListCalc( char *SRC ) ; //	List 1 -> List 2  etc
void CB_Argument( char *SRC ) ;	// Argument( List1, List2 )		Argument( Mat A, Mat B)
void CB_SortAD( char *SRC, int flagAD) ;	// SortA( List 1 ) or 	// SortD( List 1 )
double CB_Min( char *SRC ) ;	// Min( List 1 )
double CB_Max( char *SRC ) ;	// Max( List 1 )
double CB_Sum( char *SRC ) ;	// Sum( List 1 )
double CB_Prod( char *SRC ) ;	// Prod( List 1 )

//-----------------------------------------------------------------------------
double CB_Peek( char *SRC, int adrs ) ;	// Peek(123456).f
int CB_PeekInt( char *SRC, int adrs ) ;	// Peek(123456).w
void CB_PokeSub( char *SRC, double data, int adrs ) ;	// Poke(123456).f
void CB_PokeSubInt( char *SRC, int data, int adrs ) ;	// Poke(123456).w
void CB_Poke( char *SRC ) ;
int CB_Call( char *SRC ) ;
int CB_SysCall( char *SRC ) ;
int CB_VarPtr( char *SRC ) ;
int CB_ProgPtr( char *SRC ) ; //	ProgPtr(
