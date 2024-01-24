#define EditMaxfree 2048
#define EditMaxProg  256
#define NewMax 4096+0x4C
#define EditMAX	65000-EditMaxfree

#define ClipMax 2048

//extern char *ClipBuffer;
extern char *ClipBuffer;

extern char DebugMode;			// 0:disable   1:cont   2:trace   3:step over   4:step out   9:debug mode start
extern char DebugScreen;		// 0:no   1:Screen debug mode   2: Screen debug mode with fkey 

int SrcSize( char *src ) ;

unsigned int EditRun(int run);
int CB_BreakStop() ;

