/*****************************************************************/
/*                                                               */
/*   inp Library  ver 1.11                                       */
/*                                                               */
/*   written by sentaro21                                        */
/*                                                               */
/*****************************************************************/
//----------------------------------------------------------------------------------------------

#define  FLOAT_OFF    0
#define  FLOAT_ON     1
#define  REV_OFF      0
#define  REV_ON       1
#define  EXP_OFF      0
#define  EXP_ON       1
#define  ALPHA_OFF    0
#define  ALPHA_ON     1
#define  HEX_OFF      0
#define  HEX_ON       1
#define  PAL_OFF      0
#define  PAL_ON       1
#define  EXIT_CANCEL_OFF 0
#define  EXIT_CANCEL_ON  1
#define  LEFT_ALIGN   0
#define  RIGHT_ALIGN  1

#define  CMDLIST_CMD  0
#define  CMDLIST_OPTN 1
#define  CMDLIST_VARS 2
#define  CMDLIST_PRGM 3

void Fkey_dispN_Aa(int n, char *buffer) ;
void Fkey_dispN_aA(int n, char *buffer) ;

extern const short oplistInp[];
extern const short oplistOPTN[];
extern const short oplistPRGM[];
extern const short oplistVARS[];
extern const short oplistCMD[];

unsigned int SelectChar( int *ContinuousSelect ) ;
int SelectOpcode( int listselect ) ;
int SelectOpcode5800P() ;

int GetOpcodeLen( char *SRC, int ptr, int *opcode );
int OpcodeLen( int opcode );
int OpcodeStrlen(int c);
int OpcodeStrLenBuf(char *SRC, int offset) ;
int GetOpcode( char *SRC, int ptr );
int NextOpcode( char *SRC, int *offset );
int NextLine( char *SRC, int *offset );
int PrevOpcode( char *SRC, int *offset );
int PrevLine( char *SRC, int *offset );
int strlenOp( char *buffer ) ;

int CB_MB_ElementCount( char *str);
int CB_MB_strlen( char *str ) ;
int CB_OpcodeToStr( int opcode, char *string );

extern int selectCMD;
extern int selectOPTN;
extern int selectVARS;
extern int selectPRGM;
extern char lowercase;

void PutAlphamode1( int CursorStyle );
int InputStrSub(int x, int y, int width, int ptrX, char* buffer, int MaxStrlen, char SPC, int rev_mode, int float_mode, int exp_mode, int alpha_mode, int hex_mode, int pallet_mode, int exit_cancel );

double Round( double num, int round_mode, int digit);
void sprintGRS( char* buffer, double num, int width, int align_mode, int round_mode, int round_digit) ; 
void sprintGR( char* buffer, double num, int width, int align_mode, int round_mode, int digit) ; // ENG mode 
void sprintG( char* buffer, double num, int width, int align_mode);

double InputNumD(int x, int y, int width, double defaultNum, char SPC, int rev_mode, int float_mode, int exp_mode);
unsigned int InputStr( int x, int y, int width,  char* buffer,      char SPC, int rev_mode);


