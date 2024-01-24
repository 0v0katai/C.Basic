/*****************************************************************/
/*                                                               */
/*   inp Library  ver 1.00                                       */
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

void Fkey_dispN_Aa(int n, char *buffer) ;
void Fkey_dispN_aA(int n, char *buffer) ;

extern const unsigned short oplistInp[];
extern const unsigned short oplistOPTN[];
extern const unsigned short oplistPRGM[];
extern const unsigned short oplistVARS[];
extern const unsigned short oplistCMD[];

int SelectOpcode(unsigned short *oplist, int *select) ;
int SelectOpcode5800P(unsigned short *oplist, int *select) ;

unsigned int SelectChar() ;
int SelectOpcode(unsigned short *oplist, int *select) ;

int OpcodeLen( unsigned short opcode );
int OpcodeStrlen(int c);
int OpcodeStrLenBuf(unsigned char *SRC, int offset) ;
unsigned short GetOpcode( unsigned char *SRC, int ptr );
int NextOpcode( unsigned char *SRC, int *offset );
int NextLine( unsigned char *SRC, int *offset );
int PrevOpcode( unsigned char *SRC, int *offset );
int PrevLine( unsigned char *SRC, int *offset );
int strlenOp( unsigned char *buffer ) ;

int CB_MB_ElementCount( unsigned char *str);
int CB_OpcodeToStr( unsigned short opcode, unsigned char *string );
void CB_Print( int x, int y, const unsigned char *str);
void CB_PrintRev( int x, int y, const unsigned char *str);
void CB_PrintC( int x, int y, const unsigned char *c);
void CB_PrintRevC( int x, int y, const unsigned char *c);
void CB_PrintXYC( int px, int py,const unsigned char *c );
void CB_PrintXY( int px, int py, const unsigned char *str);

extern int selectCMD;
extern int selectOPTN;
extern int selectVARS;
extern int selectPRGM;

void PutAlphamode1( int CursorStyle );
int InputStrSub(int x, int y, int width, int ptrX, unsigned char* buffer, int MaxStrlen, char SPC, int rev_mode, int float_mode, int exp_mode, int alpha_mode, int hex_mode, int pallet_mode, int exit_cancel );

double Round( double num, int round_mode, int digit);
void sprintGRS(unsigned char* buffer, double num, int width, int align_mode, int round_mode, int round_digit) ; 
void sprintGR(unsigned char* buffer, double num, int width, int align_mode, int round_mode, int digit) ; // ENG mode 
void sprintG(unsigned char* buffer, double num, int width, int align_mode);

double InputNumD(int x, int y, int width, double defaultNum, char SPC, int rev_mode, int float_mode, int exp_mode);
unsigned int InputStr( int x, int y, int width, unsigned char* buffer,      char SPC, int rev_mode);


