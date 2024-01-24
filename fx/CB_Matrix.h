int DimMatrixDefaultElementSize( int reg ) ;
int DimMatrixSub( int reg, int ElementSize, int dimA, int dimB ) ;
int DimMatrix( int reg, int dimA, int dimB ) ;
double ReadMatrix( int reg, int dimA, int dimB);
void WriteMatrix( int reg, int dimA, int dimB, double value);
int ReadMatrixInt( int reg, int dimA, int dimB);
void WriteMatrixint( int reg, int dimA, int dimB, int value);
void DeleteMatrix( int reg ) ;
void InitMatSub( int reg, double value );
void InitMatIntSub( int reg, int value );
void EditMatrix(int reg);		// ----------- Edit Matrix
int SetMatrix(int select);		// ----------- Set Matrix
int MatOprand( char *SRC, int *reg);
int MatOprandInt( char *SRC, int *reg); 
