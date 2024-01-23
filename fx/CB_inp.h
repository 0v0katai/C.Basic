/*****************************************************************/
/*                                                               */
/*   inp Library  ver 0.93                                       */
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

int OpcodeLen( unsigned short opcode );
int OpcodeStrlen(int c);
int OpcodeStrLenBuf(unsigned char *SRC, int offset) ;
unsigned short GetOpcode( unsigned char *SRC, int ptr );
int NextOpcode( unsigned char *SRC, int *offset );
int NextLine( unsigned char *SRC, int *offset );
int PrevOpcode( unsigned char *SRC, int *offset );
int PrevLine( unsigned char *SRC, int *offset );
int PrintlineOpcode(int *y, unsigned char *buffer, int ofst, int csrPtr, int *cx, int *cy) ;
void OpStrToStr(char *buffer, char *buffer2);

int InputStrSub(int x, int y, int width, int ptrX, char* buffer, int MaxStrlen, char SPC, int rev_mode, int float_mode, int exp_mode, int alpha_mode, int hex_mode, int pallet_mode, int exit_cancel );

void   sprintG(char* buffer, double num, int width, int align_mode);

double InputNumD(int x, int y, int width, double defaultNum, char SPC, int rev_mode, int float_mode, int exp_mode);

unsigned int InputStr( int x, int y, int width, char* buffer,      char SPC, int rev_mode);


