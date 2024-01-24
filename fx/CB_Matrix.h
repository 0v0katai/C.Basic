#define MatAryMax 27
extern int		MatAryMaxbyte[MatAryMax];		// Matrix array max memory size
extern short	MatArySizeA[MatAryMax];			// Matrix array size m Y
extern short	MatArySizeB[MatAryMax];			// Matrix array size n X
extern char		MatAryElementSize[MatAryMax];	// Matrix array word size
extern double *MatAry[MatAryMax];				// Matrix array ptr*

//------------------------------------------------------------------------------
int ReadMatrixInt( int reg, int dimA, int dimB);
double ReadMatrix( int reg, int dimA, int dimB);
void WriteMatrixint( int reg, int dimA, int dimB, int value);
void WriteMatrix( int reg, int dimA, int dimB, double value);

char *  MatrixPtr( int reg, int m, int n );
int	MatrixSize( int reg, int m, int m ) ;

int DimMatrixDefaultElementSize( int reg ) ;
int DimMatrixSub( int reg, int ElementSize, int m, int n ) ;
int DimMatrix( int reg, int m, int n ) ;

void DeleteMatrix( int reg ) ;
void InitMatSub( int reg, double value );
void InitMatIntSub( int reg, int value );
void EditMatrix(int reg);		// ----------- Edit Matrix
int SetMatrix(int select);		// ----------- Set Matrix

int ElementSizeSelect( char *SRC, int reg ) ;
void CB_MatrixInitsub( char *SRC, int *reg, int dimA, int dimB ) ; 
void CB_MatrixInit( char *SRC ) ; //	{n,m}->Dim Mat A[.B][.W][.L][.F]
void CB_MatrixInit2( char *SRC ) ; //	[[1.2,3][4,5,6]]->Mat A[.B][.W][.L][.F]
void CB_ClrMat( char *SRC ) ; //	ClrMat A
void CB_MatCalc( char *SRC ) ; //	Mat A -> Mat B  etc
void CB_MatFill( char *SRC ) ; //	Fill(value, Mat A)
void CB_MatTrn( char *SRC ) ; //	Trn Mat A


//int MatOprand(    char *SRC, int reg );
//int MatOprandInt( char *SRC, int reg );
void MatOprand2(    char *SRC, int reg, int *dimA, int *dimB );
void MatOprandInt2( char *SRC, int reg, int *dimA, int *dimB );
int  MatrixOprand( char *SRC, int *reg, int *dimA, int *dimB );

int Cellsum( int reg, int x, int y );
