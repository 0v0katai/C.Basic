#define EDITMAXFREE 2048
#define EDITMAXFREE2 8192
#define EDITMAXPROG   256
#define EDITMAXPROG2  512
#define NEWMAX    4096+0x4C
#define NEWMAX2   9999+0x4C
#define EDITMAX   65000
#define EDITMAX2  65000

#define CLIPMAX  2048
#define CLIPMAX2 8192

//extern char *ClipBuffer;
extern char *ClipBuffer;

extern char DebugMode;			// 0:disable   1:cont   2:trace   3:step over   4:step out   9:debug mode start
extern char DebugScreen;		// 0:no   1:Screen debug mode   2: Screen debug mode with fkey 

extern int EditMaxfree;
extern int EditMaxProg;
extern int NewMax;
extern int ClipMax;

extern short EditLineNum;
extern char  UpdateLineNum;

int SrcSize( char *src ) ;

unsigned int EditRun(int run);
int CB_BreakStop() ;

