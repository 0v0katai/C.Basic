
//------------------------------------------------------------------------------
extern int ErrorPtr;
extern int ErrorNo;

//------------------------------------------------------------------------------
#define SyntaxERR  1
#define MaERR      2
#define GoERR      3
#define NestingERR 4
#define StackERR   5
#define MemoryERR  6
#define ArgumentERR 7
#define DimensionERR 8
#define RangeERR 9
#define ConditionERR 10
#define NonRealERR 11
#define ComplexNumberinList 12
#define ComplexNumberinMatrix 13
#define ComplexNumberinMatrixOrVector 14
#define ComplexNumberinData 15
#define CantSimplify 16
#define CantSolver 17
#define NoVariable 18
#define ConversionERR 19
#define ComERR 20
#define TransmitERR 21
#define ReceiveERR 22
#define MemoryFull 23
#define InvalidDataSize 24
#define TimeOut 25
#define CircularERR 26
#define PleaseReconnect 27
#define TooMuchData 28
#define FragmentationERR 29
#define InvalidType 30
#define StorageMemoryFull 31

//------------------------------------------------------------------------------
void CB_ERROR(char *buffer) ;
void CB_ErrNo(int ErrorNo) ;
void CB_BreakStop() ;

