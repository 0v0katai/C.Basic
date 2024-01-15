
//------------------------------------------------------------------------------
extern int ErrorProg;
extern int ErrorPtr;
extern int ErrorNo;

//------------------------------------------------------------------------------
#define SyntaxERR  1
#define MathERR     2
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

#define OutOfRangeERR 32
#define ForWithoutNextERR 33
#define NextWithoutForERR 34
#define WhileWithoutWhileEndERR 35
#define WhileEndWithoutWhileERR 36
#define DoWithoutLpWhileERR 37
#define LpWhileWithoutDoERR 38
#define NotLoopERR 39
#define DivisionByZeroERR 40
#define UndefinedLabelERR 41
#define NotEnoughMemoryERR 42
#define StringTooLongERR 43
#define NoMatrixArrayERR 44
#define ArraySizeERR 45
#define NotfoundProgERR 46
#define TooManyProgERR 47
#define IfWithoutIfEndERR 48
#define ThenWithoutIfERR 49
#define CaseWithoutSwitchERR 50
#define DefaultWithoutSwitchERR 51
#define SwitchWithoutSwitchEndERR 52
#define SwitchEndWithoutSwitchERR 53
#define CantFindFileERR 54
#define ElementSizeERR 55
#define AlreadyOpenERR 56
#define ComNotOpenERR 57
#define TypeMismatchERR 58
#define OutOfDomainERR 59
#define UndefinedVarERR 60
#define UndefinedFuncERR 61
#define NotSupportERR 62
#define TooManyVarERR 63
#define DuplicateDefERR 64
#define AlignmentERR 65
#define NotAccuracyERR 66
#define TryWithoutExceptERR 70
#define ExcpetWithoutTryERR 71
#define TryEndWithoutTryERR 72
#define CanNotSupport_003F_ERR 73
#define CanNotSupport_041F_ERR 74
#define CanNotSupport_F820_ERR 75
#define CanNotSupport_FC00_ERR 76

#define FileERR 100

//------------------------------------------------------------------------------
void ERROR(char *buffer) ;
void CB_ErrMsg(int ErrNo) ;
void CB_Error(int ErrNo) ;
int CannotSupportERROR( int n ) ;

