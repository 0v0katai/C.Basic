#define EditMaxfree 1024
#define EditMaxProg  128
#define NewMax 2048+0x4C

#define ClipMax 1024

extern char *ClipBuffer;
//extern char ClipBuffer[ClipMax];

extern char DebugMode;			// 0:disable   1:cont   2:trace   3:step over   4:step out   9:debug mode start
extern char DebugScreen;		// 0:no   1:Screen debug mode   2: Screen debug mode with fkey 

int SrcSize( char *src ) ;

unsigned int EditRun(int run);
int CB_BreakStop() ;

