#define EditMaxfree 1024
#define NewMax 2048+0x4C

#define ClipMax 1024

extern unsigned char ClipBuffer[];

extern int Contflag;	// Continue mode    0:disable  1:enable

void CB_PreProcess(unsigned char *SRC);
int SrcSize( unsigned char *src ) ;

unsigned int EditRun(int run);
int CB_BreakStop() ;

