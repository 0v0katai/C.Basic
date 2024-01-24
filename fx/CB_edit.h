#define EditMaxfree 1024
#define EditMaxProg  128
#define NewMax 2048+0x4C

#define ClipMax 1024

extern char ClipBuffer[ClipMax];

extern int Contflag;	// Continue mode    0:disable  1:enable

int SrcSize( char *src ) ;

unsigned int EditRun(int run);
int CB_BreakStop() ;

