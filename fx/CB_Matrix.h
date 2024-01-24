#define MatAryMax 26+6+26
//extern int	MatAryMaxbyte[MatAryMax];		// Matrix array max memory size
//extern char	MatAryBase[MatAryMax]			// Matrix array base 0 or 1
//extern char	MatAryElementSize[MatAryMax];	// Matrix array word size
//extern short	MatArySizeA[MatAryMax];			// Matrix array size m Y
//extern short	MatArySizeB[MatAryMax];			// Matrix array size n X
//extern double *MatAry[MatAryMax];				// Matrix array ptr*

typedef struct {		// 14 bytes
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

//------------------------------------------------------------------------------
int ReadMatrixInt( int reg, int dimA, int dimB);				// 0-
double ReadMatrix( int reg, int dimA, int dimB);				// 0-
void WriteMatrixint( int reg, int dimA, int dimB, int value);	// 0-
void WriteMatrix( int reg, int dimA, int dimB, double value);	// 0-

char *  MatrixPtr( int reg, int m, int n );						// 0-
int	MatrixSize( int reg, int sizeA, int sizeB ) ;	// size 1-

int DimMatrixDefaultElementSize( int reg ) ;
int DimMatrixSub( int reg, int ElementSize, int dimA, int dimA, int base ) ;	// 1-
int DimMatrix( int reg, int dimA, int dimB, int base ) ;						// 1-

void DeleteMatrix( int reg ) ;
void InitMatSub( int reg, double value );
void InitMatIntSub( int reg, int value );
void NumToHex( char *buffer, unsigned int n, int digit) ;
void EditMatrix(int reg);		// ----------- Edit Matrix
int SetMatrix(int select);		// ----------- Set Matrix

int ElementSizeSelect( char *SRC, int reg , int *base ) ;
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
void MatOprand2(    char *SRC, int reg, int *dimA, int *dimB );		// 0-
void MatOprandInt2( char *SRC, int reg, int *dimA, int *dimB );		// 0-
int  MatrixOprand( char *SRC, int *reg, int *dimA, int *dimB );		// 0-
int MatrixOprandreg( char *SRC, int *reg) ;

int Cellsum( int reg, int x, int y );								// 0-
