#define EDITMAXFREE   4096*2
#define EDITMAXFREE2  4096*16
#define EDITMAXPROG    512
#define EDITMAXPROG2  1024
#define NEWMAX    4096*8+0x4C
#define NEWMAX2   4096*16+0x4C
#define EDITMAX   1000000
#define EDITMAX2  1000000

#define CLIPMAX  8192*4
#define CLIPMAX2 8192*4

//extern char *ClipBuffer;
extern char *ClipBuffer;

extern char DebugMode;			// 0:disable   1:cont   2:trace   3:step over   4:step out   9:debug mode start
extern char DebugScreen;		// 0:no   1:Screen debug mode   2: Screen debug mode with fkey

extern int   EDITpxMAX;

extern int EditMaxfree;
extern int EditMaxProg;
extern int NewMax;
extern int ClipMax;

extern short EditLineNum;
extern char  UpdateLineNum;

extern char  MiniCursorClipMode;
extern char  MiniCursorflag ;	// Cursor Pixel ON:1 OFF:0
extern short MiniCursorX;
extern short MiniCursorY;
void MiniCursorFlashing() ;		// timer IRQ handler
void MiniCursorSetFlashMode(int set) ;	// 1:on  0:off

int SrcSize( char *src ) ;
int FixSrcSize( char *filebase ) ;

int EditRun(int run);
int CB_BreakStop(char *SRC) ;
void SetSrcSize( char *src, int size );


typedef struct {
	char	enable;
	int		sPtr;
	int		ePtr;
	int		Len;
} cUndo;


extern char *Undo;
extern int *UndoPtr;
extern int *UndoLen;
