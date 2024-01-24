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


#define CMD_MENU	1
#define CMD_OPTN	2
#define CMD_VARS	3
#define CMD_PRGM	4
#define CMD_SETUP	5
#define CMD_SHIFT	6

#define CMD_MENU_STAT		0x100
#define CMD_MENU_STAT_DRAW	0x10000
#define CMD_MENU_STAT_GRPH	0x10001
#define CMD_MENU_STAT_TYPE	0x10003
#define CMD_MENU_STAT_DIST	0x10004
#define CMD_MENU_STAT_CALC	0x10005
#define CMD_MENU_MAT		0x101
#define CMD_MENU_LIST		0x102
#define CMD_MENU_GRPH		0x103
#define CMD_MENU_GRPH_SEL	0x10300
#define CMD_MENU_GRPH_TYPE	0x10301
#define CMD_MENU_GRPH_STYL	0x10302
#define CMD_MENU_GRPH_GMEM	0x10303
#define CMD_MENU_DYNA		0x104
#define CMD_MENU_DYNA_TYPE	0x10400
#define CMD_MENU_TABL		0x110
#define CMD_MENU_TABL_TYPE	0x10500
#define CMD_MENU_TABL_STYL	0x10501
#define CMD_MENU_RECR		0x111
#define CMD_MENU_RECR_SELS	0x10600
#define CMD_MENU_RECR_TYPE	0x10601
#define CMD_MENU_RECR_RANG	0x10602
#define CMD_MENU_EXT		0x107


#define CMD_OPTN_LIST	0x200
#define CMD_OPTN_MAT	0x201
#define CMD_OPTN_MAT_SIZE	0x20110
//#define CMD_OPTN_CPLX	0x202
#define CMD_OPTN_CHR	0x202
#define CMD_OPTN_CALC	0x203
#define CMD_OPTN_STAT	0x204
#define CMD_OPTN_EXT	0x205
#define CMD_OPTN_CONV	0x210
#define CMD_OPTN_HYP	0x211
#define CMD_OPTN_PROB	0x212
#define CMD_OPTN_PROB_RAND	0x21200
#define CMD_OPTN_NUM	0x213
#define CMD_OPTN_ANGL	0x214
#define CMD_OPTN_ESYM	0x220
#define CMD_OPTN_PICT	0x221
#define CMD_OPTN_FMEM	0x222
#define CMD_OPTN_LOGIC	0x223
#define CMD_OPTN_CAPT	0x224
#define CMD_OPTN_TVM	0x225

#define CMD_VARS_VWIN	0x300
#define CMD_VARS_VWIN_X	0x30000
#define CMD_VARS_VWIN_Y	0x30001
#define CMD_VARS_VWIN_T	0x30002
#define CMD_VARS_FACT	0x301
//#define CMD_VARS_STAT	0x302
#define CMD_VARS_EXT	0x302
#define CMD_VARS_GRPH	0x303
#define CMD_VARS_DYNA	0x304
#define CMD_VARS_TABL	0x310
#define CMD_VARS_RECR	0x311
#define CMD_VARS_EQUA	0x312
#define CMD_VARS_TVM	0x313
#define CMD_VARS_STR	0x314

#define CMD_PRGM_COM	0x400
#define CMD_PRGM_CTL	0x401
#define CMD_PRGM_JUMP	0x402
#define CMD_PRGM_QMK	0x403
#define CMD_PRGM_DISPS	0x404
#define CMD_PRGM_CLR	0x410
#define CMD_PRGM_DISP	0x411
#define CMD_PRGM_REL	0x412
#define CMD_PRGM_I_O	0x413
#define CMD_PRGM_CRN	0x414
#define CMD_PRGM_STR	0x420
#define CMD_PRGM_EXEC	0x424

#define CMD_SETUP_ANGL		0x500
#define CMD_SETUP_COOR		0x501
#define CMD_SETUP_GRID		0x502
#define CMD_SETUP_AXES		0x503
#define CMD_SETUP_LABL		0x504
#define CMD_SETUP_DISP		0x510
#define CMD_SETUP_DISP_ENG	0x51000
#define CMD_SETUP_SL		0x511
#define CMD_SETUP_DRAW		0x512
#define CMD_SETUP_DERV		0x513
#define CMD_SETUP_BACK		0x514
#define CMD_SETUP_FUNC		0x520
#define CMD_SETUP_SIML		0x521
#define CMD_SETUP_SWIN		0x522
#define CMD_SETUP_LIST		0x523
#define CMD_SETUP_LOCS		0x524
#define CMD_SETUP_TVAR		0x530
#define CMD_SETUP_SDSP		0x531
#define CMD_SETUP_RESID		0x532
#define CMD_SETUP_CPLX		0x533
#define CMD_SETUP_FRAC		0x534
#define CMD_SETUP_YSPD		0x540
#define CMD_SETUP_DATE		0x541
#define CMD_SETUP_PMT		0x542
#define CMD_SETUP_PRD		0x543
#define CMD_SETUP_INEQ		0x544
#define CMD_SETUP_SIMP		0x550
#define CMD_SETUP_Q1Q3		0x551

#define CMD_SHIFT_ZOOM		0x7
#define CMD_SHIFT_VWIN		0x8
#define CMD_SHIFT_SKTCH		0x9
#define CMD_SHIFT_SKTCH_EXT		0x901
#define CMD_SHIFT_SKTCH_ML		0x902
#define CMD_SHIFT_SKTCH_GRPH	0x904
#define CMD_SHIFT_SKTCH_PLOT	0x910
#define CMD_SHIFT_SKTCH_LINE	0x911
#define CMD_SHIFT_SKTCH_PIXL	0x922
#define CMD_SHIFT_SKTCH_STYL	0x924




extern const short oplistInp[];
extern const short oplistOPTN[];
extern const short oplistPRGM[];
extern const short oplistVARS[];
extern const short oplistCMD[];

unsigned int SelectChar( int *ContinuousSelect ) ;
int SelectOpcode( int listselect, int flag ) ;
int SelectOpcode5800P( int flag ) ;

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

extern short selectCMD;
extern short selectOPTN;
extern short selectVARS;
extern short selectPRGM;
extern char lowercase;

void PutAlphamode1( int CursorStyle );
int InputStrSub(int x, int y, int width, int ptrX, char* buffer, int MaxStrlen, char SPC, int rev_mode, int float_mode, int exp_mode, int alpha_mode, int hex_mode, int pallet_mode, int exit_cancel );

double Round( double num, int round_mode, int digit);
void sprintGRS( char* buffer, double num, int width, int align_mode, int round_mode, int round_digit) ; 
void sprintGR( char* buffer, double num, int width, int align_mode, int round_mode, int digit) ; // ENG mode 
void sprintG( char* buffer, double num, int width, int align_mode);

double InputNumD(int x, int y, int width, double defaultNum, char SPC, int rev_mode, int float_mode, int exp_mode);
unsigned int InputStr(int x, int y, int width,  char* buffer, int MaxStrlen, char SPC, int rev_mode) ;		// ABCDEF0123456789.(-)exp

double InputNumD_fullsub(int x, int y, int width, double defaultNum ) ;
double InputNumD_fullhex(int x, int y, int width, double defaultNum, int hex) ;
double InputNumD_full(int x, int y, int width, double defaultNum) ;
double InputNumD_Char(int x, int y, int width, double defaultNum, char code) ;
double InputNumD_replay(int x, int y, int width, double defaultNum) ;

double InputNumD_CB(int x, int y, int width, int SPC, int REV, double defaultNum) ;		//  Basic Input
double InputNumD_CB1(int x, int y, int width, int SPC, int REV, double defaultNum) ;		//  Basic Input 1
double InputNumD_CB2(int x, int y, int width, int SPC, int REV, double defaultNum) ;		//  Basic Input 2
	

extern int CommandType;
extern char CommandPage;

void Menu_SHIFT_MENU();
void Menu_CMD_PRGM_REL();
void Menu_CMD_MENU_EXT();
void DispGenuineCmdMenu();
void GetGenuineCmdF1( unsigned int *code );
void GetGenuineCmdF2( unsigned int *code );
void GetGenuineCmdF3( unsigned int *code );
void GetGenuineCmdF4( unsigned int *code );
void GetGenuineCmdF5( unsigned int *code );
void GetGenuineCmdF6( unsigned int *code );

